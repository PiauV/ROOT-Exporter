#if defined(__CINT__) || defined(__CLING__)
// CINT is ROOT5
// CLING is ROOT6

// turn off creation of dictionaries
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Turn on creation of base dictionaries for REx (DataType + namespace)
#pragma link C++ namespace REx;
#pragma link C++ enum REx::DataType;
#pragma link C++ function REx::GetDataType;
#pragma link C++ function REx::GetDataDimension;

#endif // __CINT__ / __CLING__
