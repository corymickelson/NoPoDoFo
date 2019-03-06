cmake_minimum_required(VERSION 3.8)
project(nopodofo_sdk)
file(GLOB SDK_SOURCES 
        "*.cc")
file(GLOB SDK_HEADERS
        "*.h")

add_library(${PROJECT_NAME} SHARED ${SDK_SOURCES} ${SDK_HEADERS})
set_target_properties(${PROJECT_NAME}
                      PROPERTIES PREFIX
                                 ""
                                 SUFFIX
                                 ".node")
