#if defined(__CINT__) || defined(__CLING__)
// CINT is ROOT5
// CLING is ROOT6

// turn off creation of dictionaries
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Turn on creation of dictionaries for class ROOTToText and global variable gRTT
#pragma link C++ class ROOTToText;
#pragma link C++ global gRTT;

#endif // __CINT__ / __CLING__
