#ifndef EXPORTMANAGER_HH
#define EXPORTMANAGER_HH

#include "PadProperties.hh"

class TVirtualPad;

namespace Expad {

/// Virtual class
class ExportManager {
public:
    ExportManager();
    ~ExportManager();

    void ExportPad(TVirtualPad* pad, const char* filename) const;
    void SetDataDirectory(TString folder_name);
    void SaveInFolder(bool flag);
    void EnableLatex(bool flag);

protected:
    TString GetFilePath(TVirtualPad* pad, const char* filename) const;
    virtual void SaveData(const TObject* obj, PadProperties::Data& data) const;
    virtual void WriteToFile(const char* filename, const PadProperties& pp) const = 0;
    virtual TString FormatLabel(const TString& str) const;

protected:
    TString ext_;
    char com_; // comment char
    TString dataDir_;
    bool inFolder_;
    bool latex_;
};

} // namespace Expad

#endif
