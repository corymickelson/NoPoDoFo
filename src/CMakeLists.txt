cmake_minimum_required(VERSION 3.8)

project(nopodofo)

file(GLOB NOPODOFO_SOURCES
        "*.cc"
        "base/*.cc"
        "doc/*.cc")

file(GLOB NOPODOFO_HEADERS
        "*.h"
        "base/*.h"
        "doc/*.h")

add_library(${PROJECT_NAME} SHARED ${NOPODOFO_SOURCES} ${NOPODOFO_HEADERS})
set_target_properties(${PROJECT_NAME}
        PROPERTIES PREFIX
        ""
        SUFFIX
        ".node")

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

if (NOT PROJECT_DIR)
    message(
            WARNING
            "Project dir not explicity set using cmake source directory: ${CMAKE_SOURCE_DIR}"
    )
    set(NAPI_MODULE "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api")
else ()
    set(NAPI_MODULE "${PROJECT_DIR}/node_modules/node-addon-api")
endif ()

message("Using napi: ${NAPI_MODULE}")
message("CMAKE-JS includes: ${CMAKE_JS_INC}")
message("Openssl version: ${OPENSSL_VERSION}")
message("CMAKE-JS version: ${CMAKE_JS_VERSION}")

if (MSVC)
    set(LITEPDF_PATH "$ENV{litepdf_path}")
#    set(PODOFO_LIBRARY "${LITEPDF_PATH}\\lib\\x64\\litePDF.lib")
    set(PODOFO_LIBRARY "C:\\Users\\micke\\CMakeBuilds\\podofo\\build\\x64-Debug\\src\\Debug\\podofo.lib")
    message("PODOFO_LIBRARY: ${PODOFO_LIBRARY}")
#    set(PODOFO_INCLUDE_DIR "${LITEPDF_PATH}\\include")
        set(PODOFO_INCLUDE_DIR "C:\\Users\\micke\\CMakeBuilds\\podofo\\95126ad7-30b9-6631-b83a-ea33f4537cf1\\install\\x64-Debug\\include")
    message("PODOFO_INCLUDE_DIR: ${PODOFO_INCLUDE_DIR}")
    set(CMAKE_CXX_FLAGS "/std:c++latest /EHsc /EH /MT ${CMAKE_CXX_FLAGS}")
    add_definitions(-DUSING_SHARED_PODOFO=1)
    target_include_directories(${PROJECT_NAME}
            PRIVATE
            ${CMAKE_SOURCE_DIR}/deps/include
            ${NAPI_MODULE}
            ${OPENSSL_INCLUDE_DIR}
            ${CMAKE_JS_INC}
            ${PODOFO_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            ${CMAKE_JS_LIB}
            ${OPENSSL_LIBRARIES}
            Threads::Threads
            ${PODOFO_LIBRARY}
			OpenSSL::SSL
			OpenSSL::Crypto)
#    set(DLL "${LITEPDF_PATH}\\bin\\x64\\litePDF.dll")
        set(DLL "C:\\Users\\micke\\CMakeBuilds\\podofo\\build\\x64-Debug\\src\\Debug\\podofo.dll")
    if (EXISTS "${DLL}")
        add_custom_command(TARGET nopodofo POST_BUILD
                COMMAND "${CMAKE_COMMAND}"
                ARGS -E
                copy
                "${DLL}"
                "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
    else ()
        message(WARNING "DLL not found: ${DLL}")
    endif (EXISTS "${DLL}")
else ()
    message("Creating build output directory: build/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
    find_path(PODOFO_INCLUDE_DIR podofo/podofo.h
            PATHS /opt/local/include /usr/include /usr/local/include)
    find_library(PODOFO_LIBRARY
            NAMES podofo
            PATHS /opt/local/lib
            /usr/lib64
            /usr/lib
            /usr/local/lib)
    target_include_directories(${PROJECT_NAME}
            PRIVATE
            ${CMAKE_SOURCE_DIR}/deps/include
            ${NAPI_MODULE}
            ${OPENSSL_INCLUDE_DIR}
            ${CMAKE_JS_INC}
            ${PODOFO_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            ${CMAKE_JS_LIB}
            ${OPENSSL_LIBRARIES}
            Threads::Threads
            ${PODOFO_LIBRARY}
			stdc++fs)
    add_custom_command(TARGET nopodofo POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:nopodofo>
            ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
endif (MSVC)
