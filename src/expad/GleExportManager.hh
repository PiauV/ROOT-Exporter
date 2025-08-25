#ifndef GLEEXPORTMANAGER_HH
#define GLEEXPORTMANAGER_HH

#include "ExportManager.hh"

#include <fstream>

namespace REx {

/// @brief Export plot to the GLE scripting language (https://glx.sourceforge.io/)
class GleExportManager : public VirtualExportManager {
public:
    GleExportManager();
    virtual ~GleExportManager();

protected:
    virtual void WriteToFile(const char* filename, const PadProperties& pp) const;
    virtual void SetTitleAndAxis(std::ofstream& ofs, const PadProperties& pp) const;
    virtual void SetData(std::ofstream& ofs, const PadProperties& pp) const;
    virtual void SetLegend(std::ofstream& ofs, const PadProperties& pp) const;
    virtual void SetDecorators(std::ofstream& ofs, const PadProperties& pp) const;

private:
    void InitFile(std::ofstream& ofs) const;
};
} // namespace REx

#endif
