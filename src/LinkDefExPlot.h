#if defined(__CINT__) || defined(__CLING__)
// CINT is ROOT5
// CLING is ROOT6

// turn off creation of dictionaries
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Turn on creation of dictionaries for Expad module
#pragma link C++ namespace Expad;
#pragma link C++ enum Expad::DataType;
#pragma link C++ function Expad::GetDataType;
#pragma link C++ class Expad::PlotSerializer;

#endif // __CINT__ / __CLING__
