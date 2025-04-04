set(CMAKE_SHARED_LIBRARY_PREFIX "") # do not prepend 'lib' in front of libnames in Unix build

##### Data export (ROOTToText)

#--- Create ROOT dictonary
ROOT_GENERATE_DICTIONARY(ROOTToText_dict ROOTToText.hh DataType.hh LINKDEF LinkDefRTT.h)

#--- Create a shared library
add_library(libROOTToText SHARED ROOTToText.cpp DataType.cpp ROOTToText_dict.cxx)
target_link_libraries(libROOTToText PUBLIC ${ROOT_LIBS})


##### Plot export (ExPlot)

#--- Create ROOT dictonary
ROOT_GENERATE_DICTIONARY(ExPlot_dict PlotSerializer.hh DataType.hh LINKDEF LinkDefExPlot.h)

#--- Create a shared library
add_library(libExPlot SHARED PlotSerializer.cpp DataType.cpp ExPlot_dict.cxx)
target_link_libraries(libExPlot PUBLIC ${ROOT_LIBS})


set_target_properties(libROOTToText libExPlot PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
