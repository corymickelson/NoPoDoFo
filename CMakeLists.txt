cmake_minimum_required(VERSION 3.8)

project(npdf)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -fPIC)

file(GLOB SOURCE_FILES
    "src/*.h"
    "src/*.cc"
    "src/base/*.cc"
    "src/base/*.h"
    "src/doc/*.h"
    "src/doc/*.cc"
    "src/crypt/*.h"
    "src/crypt/*.cc" )

# Configure for .node
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Add Boost and CMakeJS Deps
# add_subdirectory(boost-cmake)

# Find podofo
find_path(PODOFO_INCLUDE_DIR podofo/podofo.h PATHS /usr/include /usr/local/include)
find_library(PODOFO_LIBRARY NAMES podofo PATHS /usr/lib64 /usr/lib /usr/local/lib)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_JS_INC}
        PRIVATE ${PODOFO_INCLUDE_DIR}
        PRIVATE ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api/src
        PRIVATE ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api)

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_JS_LIB}
        # PUBLIC Boost::filesystem
        PUBLIC ${PODOFO_LIBRARY})
