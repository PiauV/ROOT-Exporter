set(CMAKE_SHARED_LIBRARY_PREFIX "") # do not prepend 'lib' in front of libnames in Unix build

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

##### Data export (ROOTToText)

set(RTT_HEADERS
  base/DataType.hh
  rtt/ROOTToText.hh
)

set(RTT_SRC
  base/DataType.cpp
  rtt/ROOTToText.cpp
)

set(RTT_DICT ROOTToText_dict)

#--- Create ROOT dictonary
ROOT_GENERATE_DICTIONARY(${RTT_DICT} ${RTT_HEADERS} LINKDEF rtt/LinkDefRTT.h)

#--- Create a shared library
add_library(libROOTToText SHARED ${RTT_SRC} ${RTT_DICT}.cxx)
target_link_libraries(libROOTToText PUBLIC ${ROOT_LIBS})


##### Export Plots And Datasets (ExPad)

set(EXPAD_HEADERS
  base/DataType.hh
  expad/ExportManager.hh
  expad/GleExportManager.hh
  expad/GnuplotExportManager.hh
  # expad/PadProperties.hh # not needed for dictionnary generation
  expad/PlotSerializer.hh
  expad/PyplotExportManager.hh
)

set(EXPAD_SRC
  base/DataType.cpp
  expad/ExportManager.cpp
  expad/GleExportManager.cpp
  expad/GnuplotExportManager.cpp
  expad/PadProperties.cpp
  expad/PlotSerializer.cpp
  expad/PyplotExportManager.cpp
)

set(EXPAD_DICT ExPad_dict)

#--- Create ROOT dictonary
ROOT_GENERATE_DICTIONARY(${EXPAD_DICT} ${EXPAD_HEADERS} LINKDEF expad/LinkDefExPad.h)

#--- Create a shared library
add_library(libExPad SHARED ${EXPAD_SRC} ${EXPAD_DICT}.cxx)
add_dependencies(libExPad libROOTToText)
target_link_libraries(libExPad PUBLIC libROOTToText ${ROOT_LIBS})


set_target_properties(libROOTToText libExPad PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
