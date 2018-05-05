cmake_minimum_required(VERSION 3.8)

project(npdf)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN)
#set(CMAKE_CXX_FLAGS "-fsanitize=address")
#set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-rpath=/var/task/node_modules/nopodofolambda/lib")

# set(CMAKE_CXX_FLAGS -fPIC)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3")
set(NAPI_CPP_EXCEPTIONS "yes")
if (MSVC)
    set(PODOFO_LIBRARY "C:\\Users\\micke\\CMakeBuilds\\4c25184c-e5d2-673f-92ce-a630b81f074c\\build\\x86-Release\\src\\Release\\podofo.lib")
    set(PODOFO_INCLUDE_DIR "S:\\projects\\cmakeProjects\\podofo")
    set(CMAKE_CXX_FLAGS /EH)
    set(CMAKE_CXX_FLAGS /EHsc)
    set(CMAKE_TOOLCHAIN_FILE "C:\\Srcs\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")
endif (MSVC)

file(GLOB SOURCE_FILES
        "src/*.h"
        "src/*.cc"
        "src/base/*.cc"
        "src/base/*.h"
        "src/doc/*.h"
        "src/doc/*.cc"
        "src/crypto/*.h"
        "src/crypto/*.cc" )

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

find_package(OpenSSL REQUIRED)
if (NOT PROJECT_DIR)
    message(WARNING "Project dir not explicity set using cmake source directory: ${CMAKE_SOURCE_DIR}")
    set(NAPI_MODULE "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api")
    set(NAPI_SRC "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api/src")
else()
    set(NAPI_MODULE "${PROJECT_DIR}/node_modules/node-addon-api")
    set(NAPI_SRC  "${PROJECT_DIR}/node_modules/node-addon-api/src")
endif()
message(WARNING "Using napi: ${NAPI_MODULE}")
target_include_directories(${PROJECT_NAME} PRIVATE
        ${NAPI_MODULE}
        ${NAPI_SRC}
        ${OPENSSL_INCLUDE_DIR}
        ${CMAKE_JS_INC})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_JS_LIB} ${OPENSSL_LIBRARIES})

message(WARNING "Openssl version: ${OPENSSL_VERSION}")

if (MSVC)
    add_definitions(-DUSING_SHARED_PODOFO=1)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PODOFO_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PODOFO_LIBRARY})
else ()
    find_path(PODOFO_INCLUDE_DIR podofo/podofo.h PATHS /usr/include /usr/local/include)
    find_library(PODOFO_LIBRARY NAMES podofo PATHS /usr/lib64 /usr/lib /usr/local/lib)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PODOFO_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PODOFO_LIBRARY})
endif (MSVC)
