#include "ExportManager.hh"

#include "Log.hh"
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

namespace REx {

BaseExportManager::BaseExportManager() {
    ext_ = "";
    com_ = '#';
    dataDir_ = "";
    inFolder_ = false;
    verb_ = false;
    saveListFunc_ = true;
}

BaseExportManager::~BaseExportManager() {
}

/// @brief Save plot to text files (plotting script with data files)
/// @param pad plot to export
/// @param filename ouput script name
void BaseExportManager::ExportPad(TVirtualPad* pad, const char* filename) const {
    auto ps = std::make_unique<PlotSerializer>(pad);

    auto path = GetFilePath(pad, filename);
    TString folder = gSystem->DirName(path);

    if (ext_.Length()) {
        if (!gSystem->AccessPathName(path, EAccessMode::kFileExists)) {
            LOG_WARN("Overwriting file " << path);
        }
    }
    else {
        folder = path;
    }

    if (gSystem->AccessPathName(folder, EAccessMode::kWritePermission)) {
        LOG_ERROR("Cannot write to directory " << folder);
        return;
        // throw std::runtime_error("Error: cannot write to this directory.")
    }

    if (!dataDir_.IsWhitespace())
        folder.Append("/").Append(dataDir_);

    // save gRTT parameters
    auto rtt_folder = gRTT->GetDirectory();
    auto rtt_cc = gRTT->GetCommentChar();
    auto rtt_verb = gRTT->GetVerbose();
    gRTT->SetDirectory(folder);
    gRTT->SetCommentChar(com_);
    gRTT->SetVerbose(verb_);
    // std::cout << "RTT directory: " << gRTT->GetDirectory() << " (" << pad->GetName() << ")" << std::endl;
    for (int i = 0; i < (int)ps->dataObjects_.size(); i++) {
        SaveData(ps->dataObjects_[i], ps->pp_.datasets[i]);
    }
    // restore gRTT parameters
    gRTT->SetDirectory(rtt_folder);
    gRTT->SetCommentChar(rtt_cc);
    gRTT->SetVerbose(rtt_verb);

    WriteToFile(path, ps->pp_);

    if (verb_) LOG_INFO("Saved plot from " << pad->GetName() << " in " << path);
}

/// @brief Get the ouput file path for exporting a plot
/// @param pad plot to export
/// @param filename output file name
/// @return output file path
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

/// @brief Save a drawn data object to an external file using ROOTToText
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
            else
                LOG_ERROR("bad dynamic_cast in " << __FUNCTION__);
        } break;
        case Histo1D: {
            auto h = dynamic_cast<const TH1*>(obj);
            if (h) {
                ncol = 2;
                TString opth = h->GetDrawOption();
                // opth.Append(h->GetOption());
                opth.ToUpper();
                bool with_err = false;
                if (h->GetSumw2()->GetSize()) with_err = true;           // sum of weight -> error bars
                if (with_err && opth.Contains("HIST")) with_err = false; // remove errors if drawn with HIST
                if (!with_err && opth.Contains("E")) with_err = true;    // ...unless option 'E' is given
                if (with_err) {
                    option = "E";
                    ncol++;
                }
            }
            else
                LOG_ERROR("bad dynamic_cast in " << __FUNCTION__);
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
        LOG_ERROR("Could not save data " << obj->GetName() << " (" << obj->IsA()->GetName() << ").");
    }
}

/// @brief If set, the data files will be saved in a separated folder
void BaseExportManager::SetDataDirectory(TString folder_name) {
    // keep the data files in a separator folder
    dataDir_ = folder_name;
}

/// @brief Set to true to save the plot and the data files in a dedicated folder
void BaseExportManager::SaveInFolder(bool flag) {
    inFolder_ = flag;
}

/// @brief Set to true to add verbosity
void BaseExportManager::SetVerbose(bool v) {
    verb_ = v;
}

/// @brief Set to true to ignore functions associated with histograms (e.g. from fits)
void BaseExportManager::IgnoreListOfFunctions(bool flag) {
    saveListFunc_ = !flag;
}

VirtualExportManager::VirtualExportManager() : BaseExportManager() {
    EnableLatex();
}

VirtualExportManager::~VirtualExportManager() {
}

/// @brief Export LaTeX symbols and formulas
TString VirtualExportManager::FormatLabel(const TString& str, bool escape) const {
    TString label(str);
    if (latex_) {
        int s = 0;
        bool in_formula = false;
        while (s < label.Length()) {
            char c = label[s];
            switch (c) {
                case '#':
                    // process LaTeX symbols : replace '#sym' with '$\sym$'
                    label.Replace(s, 1, "\\");
                    if (!in_formula) {
                        label.Insert(s, '$'); // start formula
                        in_formula = true;
                    }
                    s++;
                    break;
                case ' ':
                    if (in_formula) {
                        label.Insert(s, '$'); // end formula
                        in_formula = false;
                    }
                    s++;
                    break;
                case '_':
                    if (!in_formula && escape) {
                        label.Insert(s, '\\'); // escape
                        s++;
                    }
                    s++;
                    break;
                case '%':
                    if (escape) {
                        label.Insert(s, '\\'); // escape
                        s++;
                    }
                    s++;
                    break;
                default:
                    s++; // next character
                    break;
            }
        }
        if (in_formula) label.Append('$');
    }
    label.Prepend('\"').Append('\"');
    return label;
}

} // namespace REx
