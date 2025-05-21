#.rst:
# FindHDF4
# ---------
#
# Try to find HDF4
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines :prop_tgt:`IMPORTED` target ``HDF4::HDF4``, if
# HDF4 has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ``HDF4_FOUND``
#   system has HDF4
# ``HDF4_INCLUDE_DIRS``
#   the HDF4 include directories
# ``HDF4_LIBRARIES``
#   Link these to use HDF4
#
# Cache variables
# ^^^^^^^^^^^^^^^
#
# The following cache variables may also be set:
#
# ``HDF4_INCLUDE_DIR``
#   the HDF4 include directory

set(_HDF4_PATHS PATHS
	"[HKEY_LOCAL_MACHINE\\SOFTWARE\\HDF4;InstallPath]"
  )
if (DEFINED ENV{CONDA_PREFIX})
	list(APPEND _HDF4_PATHS "$ENV{CONDA_PREFIX}")
endif()
if (DEFINED ENV{HDF4_ROOT})
	list(APPEND _HDF4_PATHS "$ENV{HDF4_ROOT}")
endif()



find_path(HDF4_INCLUDE_DIR hdf.h ${_HDF4_PATHS} PATH_SUFFIXES include)

if (NOT HDF4_LIBRARIES)
    find_library(HDF4_LIBRARY_RELEASE NAMES mfhdf ${_HDF4_PATHS} PATH_SUFFIXES lib)
    find_library(HDF4-DF_LIBRARY_RELEASE NAMES df ${_HDF4_PATHS} PATH_SUFFIXES lib)

    include(${CMAKE_CURRENT_LIST_DIR}/SelectLibraryConfigurations.cmake)
    SELECT_LIBRARY_CONFIGURATIONS(HDF4)
    SELECT_LIBRARY_CONFIGURATIONS(HDF4-DF)

else ()
    file(TO_CMAKE_PATH "${HDF4_LIBRARIES}" HDF4_LIBRARIES)
    file(TO_CMAKE_PATH "${HDF4-DF_LIBRARIES}" HDF4-DF_LIBRARIES)
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HDF4
                                  REQUIRED_VARS HDF4_LIBRARIES HDF4-DF_LIBRARIES HDF4_INCLUDE_DIR
                                  )

if (HDF4_FOUND)
   set(HDF4_INCLUDE_DIRS ${HDF4_INCLUDE_DIR})
   include(${CMAKE_CURRENT_LIST_DIR}/CMakePushCheckState.cmake)
   cmake_push_check_state()
   set(CMAKE_REQUIRED_QUIET ${HDF4_FIND_QUIETLY})
   set(CMAKE_REQUIRED_INCLUDES ${HDF4_INCLUDE_DIR})
   set(CMAKE_REQUIRED_LIBRARIES ${HDF4_LIBRARIES} ${HDF4-DF_LIBRARIES})
   cmake_pop_check_state()

    if(NOT TARGET HDF4::HDF4DF)
      add_library(HDF4::HDF4DF UNKNOWN IMPORTED)
      set_target_properties(HDF4::HDF4DF PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${HDF4_INCLUDE_DIRS}")

      if(HDF4-DF_LIBRARY_RELEASE)
        set_property(TARGET HDF4::HDF4DF APPEND PROPERTY
	  IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(HDF4::HDF4DF PROPERTIES
          IMPORTED_LOCATION_RELEASE "${HDF4-DF_LIBRARY_RELEASE}")
      endif()

      if(NOT HDF4-DF_LIBRARY_RELEASE AND NOT HDF4-DF_LIBRARY_DEBUG)
        set_property(TARGET HDF4::HDF4DF APPEND PROPERTY
          IMPORTED_LOCATION "${HDF4-DF_LIBRARY}")
      endif()
    endif()

    if(NOT TARGET HDF4::HDF4)
      add_library(HDF4::HDF4 UNKNOWN IMPORTED)
      set_target_properties(HDF4::HDF4 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${HDF4_INCLUDE_DIRS}")

      if(HDF4_LIBRARY_RELEASE)
        set_property(TARGET HDF4::HDF4 APPEND PROPERTY
          IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(HDF4::HDF4 PROPERTIES
          IMPORTED_LOCATION_RELEASE "${HDF4_LIBRARY_RELEASE}")
      endif()

      if(NOT HDF4_LIBRARY_RELEASE AND NOT HDF4_LIBRARY_DEBUG)
        set_property(TARGET HDF4::HDF4 APPEND PROPERTY
          IMPORTED_LOCATION "${HDF4_LIBRARY}")
      endif()
    endif()
endif ()

mark_as_advanced(HDF4_INCLUDE_DIR)

