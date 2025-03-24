cmake_minimum_required(VERSION 3.5)

#--- Create dictonary
ROOT_GENERATE_DICTIONARY(ROOTToText_dict RootToText.hh DataType.hh LINKDEF LinkDef.h)

#--- Create a shared library
add_library(libROOTToText SHARED ROOTToText.cpp DataType.cpp ROOTToText_dict.cxx)
target_link_libraries(libROOTToText PUBLIC libCore libHist libCint)

set_target_properties(libROOTToText PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)