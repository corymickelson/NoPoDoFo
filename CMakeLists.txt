cmake_minimum_required(VERSION 3.8)
project(npdf)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -fPIC)

file(GLOB SOURCE_FILES
    "src/*.hpp"
    "src/*.cc")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

add_subdirectory(cmakeModules/boost-cmake)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_JS_INC}
        #PRIVATE $ENV{HOME}/.nvm/versions/node/v$ENV{NODE_VERSION}/include/node
        PRIVATE $ENV{HOME}/.nvm/versions/node/v8.4.0/include/node
        PRIVATE ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api)

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_JS_LIB}
        PUBLIC Boost::filesystem
        podofo)

#add_executable(npdf ${SOURCE_FILES})
