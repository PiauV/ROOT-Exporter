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
    void WriteToFile(const char* filename, const PadProperties& pp) const;
    TString FormatLabel(const TString& str) const;

private:
    void WriteHeader(std::ofstream& ofs, const char* basename) const;
};
} // namespace Expad

#endif
