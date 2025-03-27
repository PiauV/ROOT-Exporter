cmake_minimum_required(VERSION 3.5)

project(RTT LANGUAGES CXX)

# set(ROOT_CONFIG_DEBUG true)

#---Locate the ROOT package
find_package(ROOT REQUIRED)
message(STATUS "Found ROOT ${ROOT_VERSION}")
include(${ROOT_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR}/src ${ROOT_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${ROOT_CXX_FLAGS}")
link_directories(${ROOT_LIBRARY_DIR})

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
