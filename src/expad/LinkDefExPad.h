#if defined(__CINT__) || defined(__CLING__)
// CINT is ROOT5
// CLING is ROOT6

#include "LinkDefBase.h" // creation of base dictionaries for REx

// Turn on creation of additional dictionaries for REx/ExPad module
#pragma link C++ class REx::BaseExportManager;
#pragma link C++ class REx::VirtualExportManager;
#pragma link C++ class REx::DataExportManager;
#pragma link C++ class REx::GleExportManager;
#pragma link C++ class REx::GnuplotExportManager;
#pragma link C++ class REx::PyplotExportManager;

#endif // __CINT__ / __CLING__
