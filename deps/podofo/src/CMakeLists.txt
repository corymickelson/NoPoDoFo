# litePDF build script

cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)

project(litePDF
	VERSION 2.0.0.0
	LANGUAGES CXX)

# Load modules from the source tree
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT WIN32)
	message(FATAL "This is a Windows library only")
endif(NOT WIN32)

# ******************************
# Dependencies
# ******************************

find_package(LIBJPEG)
find_package(OpenSSL)
find_package(PoDoFo)
include(UninstallTarget)

# Generate the ${PROJECT_NAME}-version.h file
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/${PROJECT_NAME}-version.h.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-version.h)

set(SOURCES
	encodings.cpp
	encodings.h
	litePDF.cpp
	litePDF.h
	litePDF.def
	litePDF.rc
	litePDF-version.h
	meta2pdf.cpp
	meta2pdf.h
)

set(COMPILE_DEFINITIONS
	-DLITEPDF_DLL_BUILD=1
	-DPODOFO_HAVE_LIBIDN=1
)

set(INCLUDE_DIRECTORIES
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}
	${LIBJPEG_INCLUDE_DIR}
	${OPENSSL_INCLUDE_DIR}
	${PODOFO_INCLUDE_DIR}
)

set(LINK_LIBRARIES
	${LIBJPEG_LIBRARIES}
	${OPENSSL_LIBRARIES}
	${PODOFO_LIBRARIES}
)

if(NOT DEFINED LITEPDF_BUILD_SHARED AND NOT DEFINED LITEPDF_BUILD_STATIC)
	set(LITEPDF_BUILD_SHARED ON)
endif(NOT DEFINED LITEPDF_BUILD_SHARED AND NOT DEFINED LITEPDF_BUILD_STATIC)

if(LITEPDF_BUILD_STATIC)
	add_library(${PROJECT_NAME}-static STATIC
		${SOURCES}
	)

	target_compile_definitions(${PROJECT_NAME}-static PUBLIC
		${COMPILE_DEFINITIONS}
		-DLITEPDF_STATIC_BUILD=1
	)

	target_include_directories(${PROJECT_NAME}-static PUBLIC
		${INCLUDE_DIRECTORIES}
	)

	target_link_libraries(${PROJECT_NAME}-static
		${LINK_LIBRARIES}
	)

	install(TARGETS ${PROJECT_NAME}-static
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	)
endif(LITEPDF_BUILD_STATIC)

if(LITEPDF_BUILD_SHARED)
	add_library(${PROJECT_NAME}-shared SHARED
		${SOURCES}
	)

	set_target_properties(${PROJECT_NAME}-shared PROPERTIES
		PREFIX ""
		OUTPUT_NAME ${PROJECT_NAME}
		LIBRARY_OUTPUT_NAME ${PROJECT_NAME}
	)

	target_compile_definitions(${PROJECT_NAME}-shared PUBLIC
		${COMPILE_DEFINITIONS}
		-DLITEPDF_SHARED_BUILD=1
	)

	target_include_directories(${PROJECT_NAME}-shared PUBLIC
		${INCLUDE_DIRECTORIES}
	)

	target_link_libraries(${PROJECT_NAME}-shared
		${LINK_LIBRARIES}
	)

	install(TARGETS ${PROJECT_NAME}-shared
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	)

	set(LITEPDF_LIB_DEP ${PROJECT_NAME}-shared)

	# examples work only with shared library
	macro(add_example _name)
		add_executable(${_name}
			${CMAKE_SOURCE_DIR}/../examples/${_name}/${_name}.cpp
			${CMAKE_SOURCE_DIR}/../share/litePDF.h
			${CMAKE_SOURCE_DIR}/../share/litePDF.cpp
		)

		add_dependencies(${_name}
			${LITEPDF_LIB_DEP}
		)

		target_compile_definitions(${_name} PRIVATE
			${COMPILE_DEFINITIONS}
		)

		target_include_directories(${_name} PUBLIC
			${CMAKE_BINARY_DIR}
			${CMAKE_SOURCE_DIR}
			${CMAKE_SOURCE_DIR}/..
			${INCLUDE_DIRECTORIES}
		)

		target_link_libraries(${_name}
			${LITEPDF_LIB_DEP}
			-lversion
		)

		install(TARGETS ${_name}
			DESTINATION ${CMAKE_INSTALL_PREFIX}/examples
		)
	endmacro(add_example)

	add_example(attachments)
	add_example(bookmarks)
	add_example(dataops)
	add_example(docinfo)
	add_example(drawtoresource)
	add_example(encrypt)
	add_example(fromdoc)
	add_example(helloworld)
	add_example(incrementalupdate)
	add_example(pagesperpage)
	add_example(podofodocument)
	add_example(sign)
	add_example(signmanual)
	add_example(unitvalues)

	target_link_libraries(signmanual
		-lcrypt32
	)
endif(LITEPDF_BUILD_SHARED)
