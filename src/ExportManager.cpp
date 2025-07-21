#include "ExportManager.hh"

#include "PlotSerializer.hh"
#include "ROOTToText.hh"

#include "TClass.h"
#include "TGraph.h"
#include "TH1.h"
#include "TSystem.h"
#include "TVirtualPad.h"

#include <fstream>
#include <iostream>
#include <memory>

namespace Expad {

BaseExportManager::BaseExportManager() {
    ext_ = "";
    com_ = '#';
    dataDir_ = "";
    inFolder_ = false;
}

BaseExportManager::~BaseExportManager() {
}

void BaseExportManager::ExportPad(TVirtualPad* pad, const char* filename) const {
    auto ps = std::make_unique<PlotSerializer>(pad);

    auto path = GetFilePath(pad, filename);
    TString folder = gSystem->DirName(path);

    if (ext_.Length()) {
        if (!gSystem->AccessPathName(path, EAccessMode::kFileExists)) {
            std::cerr << "Warning: overwriting file " << path << std::endl;
        }
    }
    else {
        folder = path;
    }

    if (gSystem->AccessPathName(folder, EAccessMode::kWritePermission)) {
        std::cerr << "Error: cannot write to directory " << folder << std::endl;
        return;
        // throw std::runtime_error("Error: cannot write to this directory.")
    }

    if (!dataDir_.IsWhitespace())
        folder.Append("/").Append(dataDir_);

    auto rtt_folder = gRTT->GetDirectory();
    auto rtt_cc = gRTT->GetCommentChar();
    gRTT->SetDirectory(folder);
    gRTT->SetCommentChar(com_);
    // std::cout << "RTT directory: " << gRTT->GetDirectory() << " (" << pad->GetName() << ")" << std::endl;
    for (int i = 0; i < ps->dataObjects_.size(); i++) {
        SaveData(ps->dataObjects_[i], ps->pp_.datasets[i]);
    }
    gRTT->SetDirectory(rtt_folder);
    gRTT->SetCommentChar(rtt_cc);

    WriteToFile(path, ps->pp_);
}

TString BaseExportManager::GetFilePath(TVirtualPad* pad, const char* filename) const {
    TString str(filename);

    // if no filename is given, use the object name
    if (str.IsWhitespace()) {
        str = pad->GetName();
        str.ReplaceAll(' ', '_');
    }

    if (ext_.Length()) {
        // Add file extension if filename does not end with it
        if (!str.EndsWith(ext_)) {
            if (inFolder_) {
                // "foo/bar" --> "foo/bar/bar.ext"
                TString basename = gSystem->BaseName(str);
                gSystem->mkdir(str); // create directory
                str.Append("/" + basename);
            }
            str.Append(ext_);
        }
        else if (inFolder_) {
            // "foo/bar.ext" --> "foo/bar/bar.ext"
            auto s1 = str.Last('/');
            auto s2 = str.Last('.');
            if (s1 >= 0) {
                if (s2 < s1) throw std::runtime_error("cannot handle this path : " + str);
                str.Replace(s1, 0, str(s1, s2 - s1), s2 - s1);
            }
            else {
                str.Prepend(str(0, s2) + "/");
            }
            gSystem->mkdir(gSystem->DirName(str)); // create directory
        }
        // std::cout << str << std::endl;
    }
    else {
        // no extension --> filename is actually a folder name
        if (gSystem->AccessPathName(str)) gSystem->mkdir(str); // create folder if it does not exist
    }

    gSystem->ExpandPathName(str);

    return str;
}

void BaseExportManager::SaveData(const TObject* obj, PadProperties::Data& data) const {
    TString option = "";
    int ncol = 0;
    switch (data.type) {
        case Graph1D: {
            auto gr = dynamic_cast<const TGraph*>(obj);
            if (gr) {
                ncol = 2;
                if (gr->GetEY()) {
                    // TGraphErrors
                    ncol++;
                    // do not save EX if it is an array of 0
                    auto EX = gr->GetEX();
                    auto np = gr->GetN();
                    bool is_empty_EX = true;
                    for (int i = 0; i < np; i++) {
                        if (EX[i] > 0) {
                            is_empty_EX = false;
                            break;
                        };
                    }
                    if (!is_empty_EX) {
                        ncol++;
                        option = "H";
                    }
                }
            }
        } break;
        case Histo1D: {
            auto h = dynamic_cast<const TH1*>(obj);
            if (h) {
                ncol = 2;
                TString opth = h->GetDrawOption();
                // opth.Append(h->GetOption());
                opth.ToUpper();
                bool with_err = false;
                if (opth.Contains("E")) with_err = true;
                if (!with_err && h->GetSumw2()->GetSize()) with_err = true;
                if (with_err && opth.Contains("HIST")) with_err = false;
                if (with_err) {
                    option = "E";
                    ncol++;
                }
            }
        } break;
        default:
            ncol = 2;
            break;
    }

    TString filename = "";
    if (gRTT->SaveObject(obj, data.type, filename, option)) {
        data.file.first = gSystem->BaseName(filename);
        data.file.second = ncol;
        if (!dataDir_.IsWhitespace())
            data.file.first.Prepend(dataDir_ + "/");
    }
    else {
        std::cerr << "Error: could not save data " << obj->GetName() << " (" << obj->IsA()->GetName() << ")." << std::endl;
    }
}

void BaseExportManager::SetDataDirectory(TString folder_name) {
    // keep the data files in a separator folder
    dataDir_ = folder_name;
}

void BaseExportManager::SaveInFolder(bool flag) {
    inFolder_ = flag;
}

ExportManager::ExportManager() : BaseExportManager() {
    EnableLatex();
}

ExportManager::~ExportManager() {
}

TString ExportManager::FormatLabel(const TString& str) const {
    TString label(str);
    if (latex_) {
        // process LaTeX symbols : replace '#sym' with '$\sym$'
        int s = label.Index('#');
        bool eol = false; // set to true if we need to end the line with '$'
        while (s >= 0) {
            label.Replace(s, 1, "\\");
            if (!eol) {
                label.Insert(s, '$');
                s = label.Index(' ', s);
                if (s >= 0)
                    label.Insert(s, '$');
                else
                    eol = true;
                s = label.Index(s, '$');
            }
            s = label.Index('#', s);
        }
        if (eol)
            label.Append('$');
    }
    label.Prepend('\"').Append('\"');
    return label;
}

} // namespace Expad
