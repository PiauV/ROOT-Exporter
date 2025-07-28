cmake_minimum_required(VERSION 3.17)

project(REx LANGUAGES CXX)

if(MSVC)
    # level 4 warnings but without:
    #   - C4100 (unreferenced format parameter)
    #   - C4127 (constant conditional expression)
    #   - C4512 (assignment operator) --> caused by TStringToken
    add_compile_options(/W4 /wd4100 /wd4127 /wd4512)
else()
    add_compile_options(-Wall -Wextra -Wshadow)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

#---Locate the ROOT package
# set(ROOT_CONFIG_DEBUG true)
find_package(ROOT REQUIRED)
message(STATUS "Found ROOT ${ROOT_VERSION}")
include(${ROOT_USE_FILE})
set(CMAKE_CXX_FLAGS "${ROOT_CXX_FLAGS}")
include_directories(SYSTEM ${ROOT_INCLUDE_DIRS}) # to avoid warnings coming from ROOT headers... does not really seem to work on Windows, though
link_directories(${ROOT_LIBRARY_DIR})

if(${ROOT_VERSION} VERSION_GREATER_EQUAL "6")
    set(ROOT_LIBS Core Rint Hist Graf Gpad)
else()
    set(ROOT_LIBS Core Cint Hist Graf Gpad)
endif()

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
