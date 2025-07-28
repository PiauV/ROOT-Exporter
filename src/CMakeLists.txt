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


##### Export Plots And Datasets (ExPad)

set(EXPAD_HEADERS
DataType.hh
ExportManager.hh
GleExportManager.hh
GnuplotExportManager.hh
# PadProperties.hh # not needed for dictionnary generation
PlotSerializer.hh
PyplotExportManager.hh
)

set(EXPAD_SRC
DataType.cpp
ExportManager.cpp
GleExportManager.cpp
GnuplotExportManager.cpp
PadProperties.cpp
PlotSerializer.cpp
PyplotExportManager.cpp
)

set(EXPAD_DICT ExPad_dict)

#--- Create ROOT dictonary
ROOT_GENERATE_DICTIONARY(${EXPAD_DICT} ${EXPAD_HEADERS} LINKDEF LinkDefExPad.h)

#--- Create a shared library
add_library(libExPad SHARED ${EXPAD_SRC} ${EXPAD_DICT}.cxx)
add_dependencies(libExPad libROOTToText)
target_link_libraries(libExPad PUBLIC libROOTToText ${ROOT_LIBS})


set_target_properties(libROOTToText libExPad PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
