#if defined(__CINT__) || defined(__CLING__)
// CINT is ROOT5
// CLING is ROOT6

#include "LinkDefBase.h" // creation of base dictionaries for Expad

// Turn on creation of additional dictionaries for Expad/ExPlot module
#pragma link C++ class Expad::PlotSerializer;
#pragma link C++ class Expad::ExportManager;
#pragma link C++ class Expad::DataExportManager;
#pragma link C++ class Expad::GleExportManager;
#pragma link C++ class Expad::GnuplotExportManager;
#pragma link C++ class Expad::PyplotExportManager;

#endif // __CINT__ / __CLING__
