#.rst:
# FindHDF-EOS2
# ---------
#
# Try to find HDF-EOS2
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines :prop_tgt:`IMPORTED` target ``HDF-EOS2::HDF-EOS2``, if
# HDF-EOS2 has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ``HDF-EOS2_FOUND``
#   system has HDF-EOS2
# ``HDF-EOS2_INCLUDE_DIRS``
#   the HDF-EOS2 include directories
# ``HDF-EOS2_LIBRARIES``
#   Link these to use HDF-EOS2
#
# Cache variables
# ^^^^^^^^^^^^^^^
#
# The following cache variables may also be set:
#
# ``HDF-EOS2_INCLUDE_DIR``
#   the HDF-EOS2 include directory

set(_HDF-EOS2_PATHS PATHS
	"[HKEY_LOCAL_MACHINE\\SOFTWARE\\HDF-EOS2;InstallPath]"
  )
if (DEFINED ENV{CONDA_PREFIX})
	list(APPEND _HDF-EOS2_PATHS "$ENV{CONDA_PREFIX}")
endif()
if (DEFINED ENV{HDFEOS_ROOT})
	list(APPEND _HDF-EOS2_PATHS "$ENV{HDFEOS_ROOT}")
endif()
if (DEFINED ENV{HDFEOS2_ROOT})
	list(APPEND _HDF-EOS2_PATHS "$ENV{HDFEOS2_ROOT}")
endif()




find_path(HDF-EOS2_INCLUDE_DIR HdfEosDef.h ${_HDF-EOS2_PATHS} PATH_SUFFIXES include)

if (NOT HDF-EOS2_LIBRARIES)
    find_library(HDF-EOS2_LIBRARY_RELEASE NAMES hdfeos ${_HDF-EOS2_PATHS} PATH_SUFFIXES lib lib/linux64)
    find_library(HDF-EOS2-GCTP_LIBRARY_RELEASE NAMES Gctp ${_HDF-EOS2_PATHS} PATH_SUFFIXES lib lib/linux64)

    include(${CMAKE_CURRENT_LIST_DIR}/SelectLibraryConfigurations.cmake)
    SELECT_LIBRARY_CONFIGURATIONS(HDF-EOS2)
    SELECT_LIBRARY_CONFIGURATIONS(HDF-EOS2-GCTP)

else ()
    file(TO_CMAKE_PATH "${HDF-EOS2_LIBRARIES}" HDF-EOS2_LIBRARIES)
    file(TO_CMAKE_PATH "${HDF-EOS2-GCTP_LIBRARIES}" HDF-EOS2-GCTP_LIBRARIES)
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HDF-EOS2
                                  REQUIRED_VARS HDF-EOS2_LIBRARIES HDF-EOS2-GCTP_LIBRARIES HDF-EOS2_INCLUDE_DIR
                                  )

if (HDF-EOS2_FOUND)
   set(HDF-EOS2_INCLUDE_DIRS ${HDF-EOS2_INCLUDE_DIR})
   include(${CMAKE_CURRENT_LIST_DIR}/CMakePushCheckState.cmake)
   cmake_push_check_state()
   set(CMAKE_REQUIRED_QUIET ${HDF-EOS2_FIND_QUIETLY})
   set(CMAKE_REQUIRED_INCLUDES ${HDF-EOS2_INCLUDE_DIR})
   set(CMAKE_REQUIRED_LIBRARIES ${HDF-EOS2_LIBRARIES} ${HDF-EOS2-GCTP_LIBRARIES})
   cmake_pop_check_state()

    if(NOT TARGET HDF-EOS2::HDF-EOS2)
      add_library(HDF-EOS2::HDF-EOS2 UNKNOWN IMPORTED)
      set_target_properties(HDF-EOS2::HDF-EOS2 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${HDF-EOS2_INCLUDE_DIRS}")

      if(HDF-EOS2_LIBRARY_RELEASE)
        set_property(TARGET HDF-EOS2::HDF-EOS2 APPEND PROPERTY
          IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(HDF-EOS2::HDF-EOS2 PROPERTIES
          IMPORTED_LOCATION_RELEASE "${HDF-EOS2_LIBRARY_RELEASE}")
	set_target_properties(HDF-EOS2::HDF-EOS2 PROPERTIES LINK_LIBRARIES "${HDF-EOS2-GCTP_LIBRARY_RELEASE}")

      endif()

      if(NOT HDF-EOS2_LIBRARY_RELEASE AND NOT HDF-EOS2_LIBRARY_DEBUG)
        set_property(TARGET HDF-EOS2::HDF-EOS2 APPEND PROPERTY
          IMPORTED_LOCATION "${HDF-EOS2_LIBRARY}")
        set_property(TARGET HDF-EOS2::HDF-EOS2 APPEND PROPERTY
		LINK_LIBRARIES "${HDF-EOS2-GCTP_LIBRARY}")

      endif()
    endif()
endif ()

mark_as_advanced(HDF-EOS2_INCLUDE_DIR)

