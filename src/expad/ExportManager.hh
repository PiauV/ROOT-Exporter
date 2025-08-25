#ifndef EXPORTMANAGER_HH
#define EXPORTMANAGER_HH

#include "PadProperties.hh"

class TVirtualPad;

namespace REx {

/// @brief Base class for exporting plots
class BaseExportManager {
public:
    BaseExportManager();
    virtual ~BaseExportManager();

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

/// @brief Export all data objects drawn in a plot to text files
class DataExportManager : public BaseExportManager {
public:
    DataExportManager() {};
    virtual ~DataExportManager() {};

protected:
    void WriteToFile(const char*, const PadProperties&) const override {};
};

/// @brief Base virtual class for exporting plots to external tools as text files (script + data)
class VirtualExportManager : public BaseExportManager {
public:
    VirtualExportManager();
    virtual ~VirtualExportManager();

    inline void EnableLatex(bool flag = true) { latex_ = flag; };

protected:
    virtual TString FormatLabel(const TString& str) const;

    virtual void WriteToFile(const char*, const PadProperties&) const = 0;

    virtual void SetTitleAndAxis(std::ofstream& ofs, const PadProperties& pp) const = 0;
    virtual void SetData(std::ofstream& ofs, const PadProperties& pp) const = 0;
    virtual void SetLegend(std::ofstream& ofs, const PadProperties& pp) const = 0;
    virtual void SetDecorators(std::ofstream& ofs, const PadProperties& pp) const = 0;

protected:
    bool latex_; // (partial) LaTeX support
};

} // namespace REx

#endif
