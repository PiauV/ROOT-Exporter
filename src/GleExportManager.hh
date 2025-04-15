#ifndef GLEEXPORTMANAGER_HH
#define GLEEXPORTMANAGER_HH

#include "ExportManager.hh"

#include <fstream>

namespace Expad {

class GleExportManager : public ExportManager {
public:
    GleExportManager();
    ~GleExportManager();

    void EnableLatex(bool flag);

protected:
    void WriteToFile(const char* filename, const PadProperties& pp) const;

private:
    void InitFile(std::ofstream& ofs) const;
    TString ProcessLatex(const TString& str) const;

private:
    bool latex_;
};
} // namespace Expad

#endif
