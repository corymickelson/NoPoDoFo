cmake_minimum_required(VERSION 3.8)

project(npdf)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN)

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

#set(CMAKE_TOOLCHAIN_FILE "C:\\Srcs\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

if(MSVC)
	target_include_directories(${PROJECT_NAME} PRIVATE C:/Srcs/vcpkg/installed/x86-windows/include)
	target_link_libraries(${PROJECT_NAME} PRIVATE C:/Srcs/vcpkg/installed/x86-windows/debug/lib/podofo.lib)
else()
        find_path(PODOFO_INCLUDE_DIR podofo/podofo.h PATHS /usr/include /usr/local/include)
        find_library(PODOFO_LIBRARY NAMES podofo PATHS /usr/lib64 /usr/lib /usr/local/lib)
        target_include_directories(${PROJECT_NAME} PRIVATE ${PODOFO_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} PRIVATE ${PODOFO_LIBRARY})
endif(MSVC)

set_target_properties(${PROJECT_NAME} PROPERTIES 
	BUILD_WITH_INSTALL_RPATH TRUE
	INSTALL_RPATH_USE_LINK_PATH FALSE)

#if(CMAKE_JS_VERSION)
	target_include_directories(${PROJECT_NAME} PRIVATE
       ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api/src
       ${CMAKE_SOURCE_DIR}/node_modules/node-addon-api
       ${CMAKE_JS_INC})
	target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_JS_LIB} )
#else()
#	message("CMake-js required")
#endif()



