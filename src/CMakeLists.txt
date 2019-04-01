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
	if (NOT DEFINED PODOFO_BUILD_PATH)
		message(FATAL_ERROR "PODOFO build path is required")
	endif (NOT DEFINED PODOFO_BUILD_PATH)

	if (NOT DEFINED PODOFO_INSTALL_PATH)
		message(FATAL_ERROR "PODOFO installation path is required")
	endif (NOT DEFINED PODOFO_INSTALL_PATH)

	set(PODOFO_LIBRARY "${PODOFO_INSTALL_PATH}\\lib\\podofo.lib")
	set(PODOFO_INCLUDE_DIR "${PODOFO_INSTALL_PATH}\\include")

	set(CMAKE_CXX_FLAGS "/std:c++latest /EHsc /MT ${CMAKE_CXX_FLAGS}")

	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CMAKE_CXX_FLAGS "/Zi ${CMAKE_CXX_FLAGS}")
	endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_custom_command(TARGET nopodofo POST_BUILD
	                   COMMAND "${CMAKE_COMMAND}" -E copy_directory
	                   "${PODOFO_BUILD_PATH}/src" "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
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
