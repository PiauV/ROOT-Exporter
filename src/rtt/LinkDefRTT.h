#if defined(__CINT__) || defined(__CLING__)
// CINT is ROOT5
// CLING is ROOT6

#include "LinkDefBase.h" // creation of base dictionaries for REx

// Turn on creation of additional dictionaries REx/RTT module
#pragma link C++ class REx::ROOTToText;
#pragma link C++ global gRTT;
#pragma link C++ typedef REx::rtt_writer;

#endif // __CINT__ / __CLING__
