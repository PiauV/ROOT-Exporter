#ifndef EXPORTMANAGER_HH
#define EXPORTMANAGER_HH

#include "PadProperties.hh"

class TVirtualPad;

namespace Expad {

class BaseExportManager {
public:
    BaseExportManager();
    ~BaseExportManager();

    void ExportPad(TVirtualPad* pad, const char* filename) const;
    void SetDataDirectory(TString folder_name);
    void SaveInFolder(bool flag);

protected:
    TString GetFilePath(TVirtualPad* pad, const char* filename) const;
    virtual void SaveData(const TObject* obj, PadProperties::Data& data) const;
    virtual void WriteToFile(const char* filename, const PadProperties& pp) const = 0;

protected:
    TString ext_;
    char com_; // comment char
    TString dataDir_;
    bool inFolder_;
};

class DataExportManager : public BaseExportManager {
public:
    DataExportManager() {};
    ~DataExportManager() {};

protected:
    void WriteToFile(const char*, const PadProperties&) const override {};
};

class ExportManager : public BaseExportManager {
public:
    ExportManager();
    ~ExportManager();

    inline void EnableLatex(bool flag = true) { latex_ = flag; };

protected:
    virtual TString FormatLabel(const TString& str) const;

    virtual void WriteToFile(const char*, const PadProperties&) const = 0;

    virtual void SetTitleAndAxis(std::ofstream& ofs, const PadProperties& pp) const = 0;
    virtual void SetData(std::ofstream& ofs, const PadProperties& pp) const = 0;
    virtual void SetLegend(std::ofstream& ofs, const PadProperties& pp) const = 0;

protected:
    bool latex_;
};

} // namespace Expad

#endif
