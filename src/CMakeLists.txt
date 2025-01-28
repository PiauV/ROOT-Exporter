cmake_minimum_required(VERSION 3.5)

# create dictonary
ROOT_GENERATE_DICTIONARY(ROOTToText_dict RootToText.h LINKDEF LinkDef.h)

#---Create a shared library with geneated dictionary
add_library(libROOTToText SHARED ROOTToText.cpp ROOTToText_dict.cxx)
target_link_libraries(libROOTToText PUBLIC libCore libHist libCint)

set_target_properties(libROOTToText PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)