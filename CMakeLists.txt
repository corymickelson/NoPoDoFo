cmake_minimum_required(VERSION 3.8)

project(nopodofo)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-fPIC")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(NOPODOFO_DEBUG 1)
else(CMAKE_BUILDL_TYPE)
    set(NOPODOFO_DEBUG 0)
endif(CMAKE_BUILDL_TYPE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3")
set(NAPI_CPP_EXCEPTIONS "yes")

if(NOPODOFO_AWS)
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-rpath=/var/task/node_modules/nopodofolambda/lib")
endif(NOPODOFO_AWS)

if (MSVC)
    set(VCPKG_PATH "$ENV{vcpkg_path}\\installed\\x64-windows")
    message("Windows VCPKG PATH: ${VCPKG_PATH}")

    if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
        # set(PODOFO_LIBRARY "${VCPKG_PATH}\\debug\\lib\\podofo.lib")
        set(PODOFO_LIBRARY "D:\\local\\litepdf\\lib\\x64\\litePDF.lib")
    else()
        # set(PODOFO_LIBRARY "${VCPKG_PATH}\\lib\\podofo.lib")
        set(PODOFO_LIBRARY "D:\\local\\litepdf\\lib\\x64\\litePDF.lib")
    endif(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    message("PODOFO_LIBRARY: ${PODOFO_LIBRARY}")

    # set(PODOFO_INCLUDE_DIR "${VCPKG_PATH}\\include")
    set(PODOFO_INCLUDE_DIR "D:\\local\\litepdf\\include")
    message("PODOFO_INCLUDE_DIR: ${PODOFO_INCLUDE_DIR}")

    set(CMAKE_CXX_FLAGS /EH)
    set(CMAKE_CXX_FLAGS /EHsc)
    message("CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
endif (MSVC)

file(GLOB SOURCE_FILES
        "src/*.h"
        "src/*.cc"
        "src/base/*.cc"
        "src/base/*.h"
        "src/doc/*.h"
        "src/doc/*.cc")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

if (NOT PROJECT_DIR)
    message(WARNING "Project dir not explicity set using cmake source directory: ${CMAKE_SOURCE_DIR}")
    set(NAPI_MODULE "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api")
else()
    set(NAPI_MODULE "${PROJECT_DIR}/node_modules/node-addon-api")
endif()
message("Using napi: ${NAPI_MODULE}")
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/deps/include
        ${CMAKE_SOURCE_DIR}/node_modules/napi-thread-safe-callback
        ${NAPI_MODULE}
        ${OPENSSL_INCLUDE_DIR}
        ${CMAKE_JS_INC})
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_JS_LIB}
        ${OPENSSL_LIBRARIES}
        Threads::Threads )

message("CMAKE-JS includes: ${CMAKE_JS_INC}")
message("Openssl version: ${OPENSSL_VERSION}")
message("CMAKE-JS version: ${CMAKE_JS_VERSION}")

if (MSVC)
    add_definitions(-DUSING_SHARED_PODOFO=1)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PODOFO_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${PODOFO_LIBRARY}
            OpenSSL::SSL
            OpenSSL::Crypto )
    message("what is the output directory: ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
    set(WIN_REQUIRED_DLLS
      "freetype.dll"
      "jpeg62.dll" 
      "libbz2.dll" 
      "libeay32.dll"
      "libpng16.dll"
      "lzma.dll" 
      "podofo.dll" 
      "ssleay32.dll" 
      "tiff.dll" 
      "tiffxx.dll" 
      "turbojpeg.dll" 
      "zlib1.dll" 
    )
    foreach(file_i ${WIN_REQUIRED_DLLS})
        set (DLL "$ENV{vcpkg_path}\\installed\\x64-windows\\bin\\${file_i}")
        if(EXISTS "${DLL}")
            add_custom_command(TARGET nopodofo
            POST_BUILD
                COMMAND "${CMAKE_COMMAND}"
                ARGS -E copy
                "${DLL}" "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
                )
        else()
            message(WARNING "DLL not found: ${DLL}")
        endif(EXISTS "${DLL}")
    endforeach(file_i)

else ()
    message("Creating build output directory: build/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
    find_path(PODOFO_INCLUDE_DIR podofo/podofo.h PATHS /opt/local/include /usr/include /usr/local/include)
    find_library(PODOFO_LIBRARY NAMES podofo PATHS /opt/local/lib /usr/lib64 /usr/lib /usr/local/lib)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PODOFO_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${PODOFO_LIBRARY}
            stdc++fs)
    add_custom_command(TARGET nopodofo
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}"
            -E copy $<TARGET_FILE:nopodofo> ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
endif (MSVC)
