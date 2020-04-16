cmake_minimum_required(VERSION 3.0)
project(wisund)
set(wisund_MAJOR_VERSION 1)
set(wisund_MINOR_VERSION 0)
set(wisund_MICRO_VERSION 0)
set(wisund_VERSION ${wisund_MAJOR_VERSION}.${wisund_MINOR_VERSION}.${wisund_MICRO_VERSION})

if(${CMAKE_VERSION} VERSION_LESS "3.1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set(CMAKE_CXX_STANDARD 11)
endif()

# options off-by-default that you can enable
option(WITH_TEST "Build the test suite" OFF)

# load additional detection for asio
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# generate posistion independent code by default
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# additionally set up stack canaries
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector")

# CMake doesn't add -PIE by default for executables (Cmake issue #14983)
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z now -z relro")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/wisundConfig.h.in"
  "${PROJECT_BINARY_DIR}/wisundConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find wisundConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# wisund sources
add_subdirectory(src)
add_subdirectory(doc)

if (WITH_TEST)
    enable_testing()
    add_subdirectory(test)
endif() 

INCLUDE(InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE  
     "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_VERSION "${wisund_VERSION}")
set (CPACK_PACKAGE_VENDOR "EPRI")

# this presumes that the build is done in dir named "build"
set(CPACK_SOURCE_IGNORE_FILES "build/*;.swp")
include (CPack)
