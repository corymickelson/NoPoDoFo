cmake_minimum_required(VERSION 3.8)

project(npdf)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -fPIC)
set(NAPI_CPP_EXCEPTIONS "yes")
if(MSVC)
        set(CMAKE_CXX_FLAGS /EH)
        set(CMAKE_CXX_FLAGS /EHsc)
endif(MSVC)

file(GLOB SOURCE_FILES
    "src/*.h"
    "src/*.cc"
    "src/base/*.cc"
    "src/base/*.h"
    "src/doc/*.h"
    "src/doc/*.cc" )

# Configure for .node
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
set(CMAKE_MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
find_package(OpenSSL)
find_path(PODOFO_INCLUDE_DIR podofo/podofo.h PATHS /usr/include /usr/local/include ${CMAKE_SOURCE_DIR}/deps/podofo/include)
# Using LitePDF in place of PoDoFo is an experiment with windows. Allowing windows user's easier access to building and running npdf
find_library(PODOFO_LIBRARY NAMES podofo litePDF PATHS /usr/lib64 /usr/lib /usr/local/lib ${CMAKE_SOURCE_DIR}/deps/podofo/lib/x64)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${OPENSSL_INCLUDE_DIR}
        ${PODOFO_INCLUDE_DIR}
        ${CMAKE_JS_INC}
        ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api/src
        ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api )

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OPENSSL_LIBRARIES}
        ${PODOFO_LIBRARY}
        ${CMAKE_JS_LIB} )
