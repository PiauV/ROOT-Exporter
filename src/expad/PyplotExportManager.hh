#ifndef PYPLOTEXPORTMANAGER_HH
#define PYPLOTEXPORTMANAGER_HH

#include "ExportManager.hh"

#include <fstream>

namespace REx {

class PyplotExportManager : public ExportManager {
public:
    PyplotExportManager();
    virtual ~PyplotExportManager();

protected:
    virtual void WriteToFile(const char* filename, const PadProperties& pp) const;
    virtual void SetTitleAndAxis(std::ofstream& ofs, const PadProperties& pp) const;
    virtual void SetData(std::ofstream& ofs, const PadProperties& pp) const;
    virtual void SetLegend(std::ofstream& ofs, const PadProperties& pp) const;
    virtual void SetDecorators(std::ofstream& ofs, const PadProperties& pp) const;

    virtual TString FormatLabel(const TString& str) const;

private:
    void InitFile(std::ofstream& ofs) const;
    char* getColor(PadProperties::Color c) const;
};
} // namespace REx

#endif
