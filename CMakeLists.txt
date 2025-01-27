# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.5)

project(RTT LANGUAGES CXX)

# set(ROOT_CONFIG_DEBUG true)

#---Locate the ROOT package
find_package(ROOT REQUIRED)
message(STATUS "Found ROOT ${ROOT_VERSION}")
include(${ROOT_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${ROOT_CXX_FLAGS}")
link_directories(${ROOT_LIBRARY_DIR})

# create dictonary
ROOT_GENERATE_DICTIONARY(ROOTToText_dict RootToText.h LINKDEF LinkDef.h)

#---Create a shared library with geneated dictionary
add_library(ROOTToText SHARED ROOTToText.cpp ROOTToText_dict.cxx)
target_link_libraries(ROOTToText PUBLIC libCore libHist libCint)

set_target_properties(ROOTToText PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
