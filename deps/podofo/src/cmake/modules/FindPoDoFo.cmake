# FindPoDoFo.cmake
#
# Searches for PoDoFo. Users can define PODOFO_EXTRA_LIBRARIES
# to specify extra libraries to add for PODOFO.
#
# Output is:
#   PODOFO_INCLUDE_DIR - where header files can be found
#   PODOFO_LIBRARIES - library file name and flags for it
#   PODOFO_FOUND - whether PoDoFo had been found

if(PODOFO_INCLUDE_DIR)
	# Already in cache, be silent
	set(PODOFO_FIND_QUIETLY TRUE)
endif(PODOFO_INCLUDE_DIR)

find_path(PODOFO_INCLUDE_DIR podofo/podofo.h)

set(PODOFO_LIBRARY_NAMES_RELEASE ${PODOFO_LIBRARY_NAMES_RELEASE} ${PODOFO_LIBRARY_NAMES} libpodofo)
find_library(PODOFO_LIBRARY_RELEASE NAMES ${PODOFO_LIBRARY_NAMES_RELEASE} )

# Find a debug library if one exists and use that for debug builds.
# This really only does anything for win32, but does no harm on other
# platforms.
set(PODOFO_LIBRARY_NAMES_DEBUG ${PODOFO_LIBRARY_NAMES_DEBUG} libpodofod)
find_library(PODOFO_LIBRARY_DEBUG NAMES ${PODOFO_LIBRARY_NAMES_DEBUG})

include(LibraryDebugAndRelease)
set_library_from_debug_and_release(PODOFO)

# handle the QUIETLY and REQUIRED arguments and set PODOFO_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PODOFO DEFAULT_MSG PODOFO_LIBRARY PODOFO_INCLUDE_DIR)

if(PODOFO_FOUND)
	set(PODOFO_LIBRARIES ${PODOFO_LIBRARY} ${PODOFO_EXTRA_LIBRARIES})
else(PODOFO_FOUND)
	set(PODOFO_LIBRARIES ${PODOFO_EXTRA_LIBRARIES})
endif(PODOFO_FOUND)

mark_as_advanced(PODOFO_LIBRARIES PODOFO_INCLUDE_DIR)
