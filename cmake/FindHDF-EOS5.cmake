#.rst:
# FindHDF-EOS5
# ---------
#
# Try to find HDF-EOS5
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines :prop_tgt:`IMPORTED` target ``HDF-EOS5::HDF-EOS5``, if
# HDF-EOS5 has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ``HDF-EOS5_FOUND``
#   system has HDF-EOS5
# ``HDF-EOS5_INCLUDE_DIRS``
#   the HDF-EOS5 include directories
# ``HDF-EOS5_LIBRARIES``
#   Link these to use HDF-EOS5
#
# Cache variables
# ^^^^^^^^^^^^^^^
#
# The following cache variables may also be set:
#
# ``HDF-EOS5_INCLUDE_DIR``
#   the HDF-EOS5 include directory

set(_HDF-EOS5_PATHS PATHS
	"[HKEY_LOCAL_MACHINE\\SOFTWARE\\HDF-EOS5;InstallPath]"
  )
if (DEFINED ENV{CONDA_PREFIX})
	list(APPEND _HDF-EOS5_PATHS "$ENV{CONDA_PREFIX}")
endif()
if (DEFINED ENV{HDFEOS_ROOT})
	list(APPEND _HDF-EOS5_PATHS "$ENV{HDFEOS_ROOT}")
endif()
if (DEFINED ENV{HDFEOS5_ROOT})
	list(APPEND _HDF-EOS5_PATHS "$ENV{HDFEOS5_ROOT}")
endif()



find_path(HDF-EOS5_INCLUDE_DIR HE5_HdfEosDef.h ${_HDF-EOS5_PATHS} PATH_SUFFIXES include)

if (NOT HDF-EOS5_LIBRARIES)
    find_library(HDF-EOS5_LIBRARY_RELEASE NAMES he5_hdfeos ${_HDF-EOS5_PATHS} PATH_SUFFIXES lib lib/linux64)
    find_library(HDF-EOS5-GCTP_LIBRARY_RELEASE NAMES Gctp ${_HDF-EOS5_PATHS} PATH_SUFFIXES lib lib/linux64)

    include(${CMAKE_CURRENT_LIST_DIR}/SelectLibraryConfigurations.cmake)
    SELECT_LIBRARY_CONFIGURATIONS(HDF-EOS5)
    SELECT_LIBRARY_CONFIGURATIONS(HDF-EOS5-GCTP)

else ()
    file(TO_CMAKE_PATH "${HDF-EOS5_LIBRARIES}" HDF-EOS5_LIBRARIES)
    file(TO_CMAKE_PATH "${HDF-EOS5-GCTP_LIBRARIES}" HDF-EOS5-GCTP_LIBRARIES)
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HDF-EOS5
                                  REQUIRED_VARS HDF-EOS5_LIBRARIES HDF-EOS5-GCTP_LIBRARIES HDF-EOS5_INCLUDE_DIR
                                  )

if (HDF-EOS5_FOUND)
   set(HDF-EOS5_INCLUDE_DIRS ${HDF-EOS5_INCLUDE_DIR})
   include(${CMAKE_CURRENT_LIST_DIR}/CMakePushCheckState.cmake)
   cmake_push_check_state()
   set(CMAKE_REQUIRED_QUIET ${HDF-EOS5_FIND_QUIETLY})
   set(CMAKE_REQUIRED_INCLUDES ${HDF-EOS5_INCLUDE_DIR})
   set(CMAKE_REQUIRED_LIBRARIES ${HDF-EOS5_LIBRARIES} ${HDF-EOS5-GCTP_LIBRARIES})
   cmake_pop_check_state()

    if(NOT TARGET HDF-EOS5::HDF-EOS5)
      add_library(HDF-EOS5::HDF-EOS5 UNKNOWN IMPORTED)
      set_target_properties(HDF-EOS5::HDF-EOS5 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${HDF-EOS5_INCLUDE_DIRS}")

      if(HDF-EOS5_LIBRARY_RELEASE)
        set_property(TARGET HDF-EOS5::HDF-EOS5 APPEND PROPERTY
          IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(HDF-EOS5::HDF-EOS5 PROPERTIES
          IMPORTED_LOCATION_RELEASE "${HDF-EOS5_LIBRARY_RELEASE}")
	set_target_properties(HDF-EOS5::HDF-EOS5 PROPERTIES LINK_LIBRARIES "${HDF-EOS5-GCTP_LIBRARY_RELEASE}")

      endif()

      if(NOT HDF-EOS5_LIBRARY_RELEASE AND NOT HDF-EOS5_LIBRARY_DEBUG)
        set_property(TARGET HDF-EOS5::HDF-EOS5 APPEND PROPERTY
          IMPORTED_LOCATION "${HDF-EOS5_LIBRARY}")
        set_property(TARGET HDF-EOS5::HDF-EOS5 APPEND PROPERTY
		LINK_LIBRARIES "${HDF-EOS5-GCTP_LIBRARY}")

      endif()
    endif()
endif ()

mark_as_advanced(HDF-EOS5_INCLUDE_DIR)

