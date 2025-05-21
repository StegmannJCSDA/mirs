#.rst:
# FindJPEG
# ---------
#
# Try to find JPEG
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines :prop_tgt:`IMPORTED` target ``JPEG::JPEG``, if
# JPEG has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ``JPEG_FOUND``
#   system has JPEG
# ``JPEG_INCLUDE_DIRS``
#   the JPEG include directories
# ``JPEG_LIBRARIES``
#   Link these to use JPEG
#
# Cache variables
# ^^^^^^^^^^^^^^^
#
# The following cache variables may also be set:
#
# ``JPEG_INCLUDE_DIR``
#   the JPEG include directory

set(_JPEG_PATHS PATHS
   "[HKEY_LOCAL_MACHINE\\SOFTWARE\\JPEG;InstallPath]"
  )
if (DEFINED ENV{CONDA_PREFIX})
   list(APPEND _JPEG_PATHS "$ENV{CONDA_PREFIX}")
endif()
if (DEFINED ENV{JPEG_ROOT})
   list(APPEND _JPEG_PATHS "$ENV{JPEG_ROOT}")
endif()

find_path(JPEG_INCLUDE_DIR jpeglib.h ${_JPEG_PATHS} PATH_SUFFIXES include)

if (NOT JPEG_LIBRARIES)
   find_library(JPEG_LIBRARY_RELEASE NAMES jpeg ${_JPEG_PATHS} PATH_SUFFIXES lib)

    include(${CMAKE_CURRENT_LIST_DIR}/SelectLibraryConfigurations.cmake)
    SELECT_LIBRARY_CONFIGURATIONS(JPEG)

else ()
   file(TO_CMAKE_PATH "${JPEG_LIBRARIES}" JPEG_LIBRARIES)
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(JPEG
                                  REQUIRED_VARS JPEG_LIBRARIES JPEG_INCLUDE_DIR
                                  )

if (JPEG_FOUND)
   set(JPEG_INCLUDE_DIRS ${JPEG_INCLUDE_DIR})
   include(${CMAKE_CURRENT_LIST_DIR}/CMakePushCheckState.cmake)
   cmake_push_check_state()
   set(CMAKE_REQUIRED_QUIET ${JPEG_FIND_QUIETLY})
   set(CMAKE_REQUIRED_INCLUDES ${JPEG_INCLUDE_DIR})
   set(CMAKE_REQUIRED_LIBRARIES ${JPEG_LIBRARIES})
   cmake_pop_check_state()

   if(NOT TARGET JPEG::JPEG)
      add_library(JPEG::JPEG UNKNOWN IMPORTED)
      set_target_properties(JPEG::JPEG PROPERTIES
         INTERFACE_INCLUDE_DIRECTORIES "${JPEG_INCLUDE_DIRS}")

      if(JPEG_LIBRARY_RELEASE)
         set_property(TARGET JPEG::JPEG APPEND PROPERTY
          IMPORTED_CONFIGURATIONS RELEASE)
       set_target_properties(JPEG::JPEG PROPERTIES
          IMPORTED_LOCATION_RELEASE "${JPEG_LIBRARY_RELEASE}")
      endif()

      if(NOT JPEG_LIBRARY_RELEASE AND NOT JPEG_LIBRARY_DEBUG)
         set_property(TARGET JPEG::JPEG APPEND PROPERTY
            IMPORTED_LOCATION "${JPEG_LIBRARY}")
      endif()
    endif()
endif ()

mark_as_advanced(JPEG_INCLUDE_DIR)

