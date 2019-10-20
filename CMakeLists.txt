cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-fPIC")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(NOPODOFO_DEBUG 1)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(NOPODOFO_DEBUG 0)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(NAPI_CPP_EXCEPTIONS "yes")

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(NOPODOFO_CXXFLAGS "" CACHE STRING "NoPoDoFo Compiler Flags")

    option(NOPODOFO_BUILD_TESTS "Build native bindings tests" OFF)

    option(NOPODOFO_BUILD_AWS_LAMBDA "Build NoPoDoFo for use in AWS Lambda" OFF)
endif ()

if (MSVC AND NOT CMAKE_TOOLCHAIN_FILE)
    message(warn "vcpkg cmake_toolchain_path must be set in your environment as VCPKG_CMAKE")
endif (MSVC AND NOT CMAKE_TOOLCHAIN_FILE)

if (NOPODOFO_BUILD_AWS_LAMBDA)
    set(CMAKE_SHARED_LINKER_FLAGS
            "-Wl,-rpath=/var/task/node_modules/nopodofolambda/lib")
endif (NOPODOFO_BUILD_AWS_LAMBDA)

add_subdirectory(src)

if (NOPODOFO_BUILD_TESTS)
    add_subdirectory(spec/unit)
endif (NOPODOFO_BUILD_TESTS)

