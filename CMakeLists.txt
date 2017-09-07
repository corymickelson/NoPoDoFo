cmake_minimum_required(VERSION 3.8)
project(npdf)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        src/npdf.cc
        src/npdf.h
        src/pdfDocument.cc
        src/pdfDocument.h
        src/addon.cc
        test/catch.hpp
        src/library.hpp)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_JS_INC}
        PRIVATE /home/skyslope/.nvm/versions/node/v8.4.0/include/node
        PRIVATE ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api)

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_JS_LIB}
        podofo)

#add_executable(npdf ${SOURCE_FILES})
