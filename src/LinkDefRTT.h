#if defined(__CINT__) || defined(__CLING__)
// CINT is ROOT5
// CLING is ROOT6

#include "LinkDefBase.h" // creation of base dictionaries for Expad

// Turn on creation of additional dictionaries Expad/RTT module
#pragma link C++ class Expad::ROOTToText;
#pragma link C++ global gRTT;

#endif // __CINT__ / __CLING__
