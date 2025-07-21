#ifndef GNUPLOTEXPORTMANAGER_HH
#define GNUPLOTEXPORTMANAGER_HH

#include "ExportManager.hh"

#include <fstream>

namespace Expad {

class GnuplotExportManager : public ExportManager {
public:
    GnuplotExportManager();
    ~GnuplotExportManager();

protected:
    virtual void WriteToFile(const char* filename, const PadProperties& pp) const;
    virtual void SetTitleAndAxis(std::ofstream& ofs, const PadProperties& pp) const;
    virtual void SetData(std::ofstream& ofs, const PadProperties& pp) const;
    virtual void SetLegend(std::ofstream& ofs, const PadProperties& pp) const;
    virtual void SetDecorators(std::ofstream& ofs, const PadProperties& pp) const;

    virtual TString FormatLabel(const TString& str) const;

private:
    void InitFile(std::ofstream& ofs, const TString& file) const;
};
} // namespace Expad

#endif
