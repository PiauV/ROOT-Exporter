set(CMAKE_SHARED_LIBRARY_PREFIX "") # do not prepend 'lib' in front of libnames in Unix build

##### Data export (ROOTToText)

set(RTT_HEADERS ROOTToText.hh DataType.hh)

set(RTT_SRC ROOTToText.cpp DataType.cpp)

set(RTT_DICT ROOTToText_dict)

#--- Create ROOT dictonary
ROOT_GENERATE_DICTIONARY(${RTT_DICT} ${RTT_HEADERS} LINKDEF LinkDefRTT.h)

#--- Create a shared library
add_library(libROOTToText SHARED ${RTT_SRC} ${RTT_DICT}.cxx)
target_link_libraries(libROOTToText PUBLIC ${ROOT_LIBS})


##### Plot export (ExPlot)

set(EXPLOT_HEADERS
GleExportManager.hh
DataExportManager.hh
ExportManager.hh
PlotSerializer.hh
DataType.hh
)

set(EXPLOT_SRC
GleExportManager.cpp
ExportManager.cpp
PlotSerializer.cpp
DataType.cpp
)

set(EXPLOT_DICT ExPlot_dict)

#--- Create ROOT dictonary
ROOT_GENERATE_DICTIONARY(${EXPLOT_DICT} ${EXPLOT_HEADERS} LINKDEF LinkDefExPlot.h)

#--- Create a shared library
add_library(libExPlot SHARED ${EXPLOT_SRC} ${EXPLOT_DICT}.cxx)
add_dependencies(libExPlot libROOTToText)
target_link_libraries(libExPlot PUBLIC libROOTToText ${ROOT_LIBS})


set_target_properties(libROOTToText libExPlot PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
