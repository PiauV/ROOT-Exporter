#ifndef DATAEXPORTMANAGER_HH
#define DATAEXPORTMANAGER_HH

#include "ExportManager.hh"

namespace Expad {

/// Export all 1D data from a Pad to text files
class DataExportManager : public ExportManager {
public:
    DataExportManager() {};
    ~DataExportManager() {};

protected:
    void WriteToFile(const char* filename, const PadProperties& pp) const override {};
};
} // namespace Expad

#endif
