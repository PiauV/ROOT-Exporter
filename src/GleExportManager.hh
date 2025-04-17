#ifndef GLEEXPORTMANAGER_HH
#define GLEEXPORTMANAGER_HH

#include "ExportManager.hh"

#include <fstream>

namespace Expad {

class GleExportManager : public ExportManager {
public:
    GleExportManager();
    ~GleExportManager();

protected:
    void WriteToFile(const char* filename, const PadProperties& pp) const;

private:
    void InitFile(std::ofstream& ofs) const;
};
} // namespace Expad

#endif
