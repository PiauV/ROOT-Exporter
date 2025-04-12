#include "ExportManager.hh"

#include "PlotSerializer.hh"
#include "ROOTToText.hh"

#include "TClass.h"
#include "TSystem.h"
#include "TVirtualPad.h"

#include <fstream>
#include <iostream>
#include <memory>

namespace Expad {

ExportManager::ExportManager() {
    ext_ = "";
    dataDir_ = "";
    inFolder_ = false;
}

ExportManager::~ExportManager() {
}

void ExportManager::ExportPad(TVirtualPad* pad, const char* filename) const {
    auto ps = std::make_unique<PlotSerializer>(pad);

    auto path = GetFilePath(pad, filename);
    TString folder = gSystem->DirName(path);

    if (!gSystem->AccessPathName(path, EAccessMode::kFileExists)) {
        std::cerr << "Warning: overwriting file " << path << std::endl;
    }

    if (gSystem->AccessPathName(folder, EAccessMode::kWritePermission)) {
        std::cerr << "Error: cannot write to directory " << folder << std::endl;
        return;
        // throw std::runtime_error("Error: cannot write to this directory.")
    }

    if (!dataDir_.IsWhitespace())
        folder.Append("/").Append(dataDir_);

    TString folder_saved = gRTT->GetDirectory();
    gRTT->SetDirectory(folder);
    for (int i = 0; i < ps->dataObjects_.size(); i++) {
        auto obj = ps->dataObjects_[i];
        auto data = ps->pp_.datasets[i];
        SaveData(obj, data);
    }
    gRTT->SetDirectory(folder_saved);

    WriteToFile(path, ps->pp_);
}

TString ExportManager::GetFilePath(TVirtualPad* pad, const char* filename) const {
    TString str(filename);

    // if no filename is given, use the object name
    if (str.IsWhitespace()) {
        str = pad->GetName();
        str.ReplaceAll(' ', '_');
    }

    // Add file extension if filename does not end with it
    if (!str.EndsWith(ext_)) {
        if (inFolder_) {
            // "foo/bar" --> "foo/bar/bar.ext"
            auto basename = gSystem->BaseName(str);
            gSystem->mkdir(str); // create directory
            str.Append("/").Append(basename);
        }
        str.Append(ext_);
    }

    gSystem->ExpandPathName(str);

    return str;
}

void ExportManager::SaveData(const TObject* obj, PadProperties::Data& data) const {
    TString filename = "";
    if (gRTT->SaveObject(obj, data.type, filename)) {
        data.file = gSystem->BaseName(filename);
        if (!dataDir_.IsWhitespace())
            data.file.Prepend(dataDir_).Prepend("/");
    }
    else {
        std::cerr << "Error: could not save data " << obj->GetName() << " (" << obj->IsA()->GetName() << ")." << std::endl;
    }
}

void ExportManager::SetDataDirectory(TString folder_name) {
    // keep the data files in a separator folder
    dataDir_ = folder_name;
}

void ExportManager::SaveInFolder(bool flag) {
    inFolder_ = flag;
}

} // namespace Expad
