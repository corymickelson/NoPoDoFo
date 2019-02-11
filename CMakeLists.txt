cmake_minimum_required(VERSION 3.8)

project(nopodofo)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(NOPODOFO_DEBUG 1)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(NOPODOFO_DEBUG 0)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(NAPI_CPP_EXCEPTIONS "yes")

if(NOPODOFO_AWS)
  set(CMAKE_SHARED_LINKER_FLAGS
      "-Wl,-rpath=/var/task/node_modules/nopodofolambda/lib")
endif(NOPODOFO_AWS)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  set(NOPODOFO_CXXFLAGS "" CACHE STRING "NoPoDoFo Compiler Flags")

  option(NOPODOFO_BUILD_TESTS "Build native bindings tests" OFF)

  option(NOPODOFO_BUILD_SDK "Build the NoPoDoFo SDK" OFF)

  option(NOPODOFO_BUILD_LAMBDA "Build NoPoDoFo for use in AWS Lambda" OFF)
endif()

add_subdirectory(src)

if(NOPODOFO_BUILD_SDK)
    set(NOPODOFO_SDK 1)
    add_subdirectory(sdk)
else()
    set(NOPODOFO_SDK 0)
endif(NOPODOFO_BUILD_SDK)

if(NOPODOFO_BUILD_TESTS)
  add_subdirectory(spec/unit)
endif(NOPODOFO_BUILD_TESTS)

