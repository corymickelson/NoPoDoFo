cmake_minimum_required(VERSION 3.8)

project(npdf)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -fPIC)

file(GLOB SOURCE_FILES
    "src/*.h"
    "src/*.cc" )

# Configure for .node 
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Add Boost and CMakeJS Deps
add_subdirectory(boost-cmake)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_JS_INC}
        PRIVATE ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api/src
        PRIVATE ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api)

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_JS_LIB}
        PUBLIC Boost::filesystem
        podofo)
