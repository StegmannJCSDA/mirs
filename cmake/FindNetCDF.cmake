#.rst:
# FindNetCDF
# ---------
#
# Try to find NetCDF
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines :prop_tgt:`IMPORTED` target ``NetCDF::NetCDF``, if
# NetCDF has been found. Also can define NetCDF::NetCDF-Fortran
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ``NETCDF_FOUND``
#   system has NETCDF
# ``NETCDF_INCLUDE_DIRS``
#   the NETCDF include directories
# ``NETCDF_LIBRARIES``
#   Link these to use NETCDF
#
# Cache variables
# ^^^^^^^^^^^^^^^
#
# The following cache variables may also be set:
#
# ``NETCDF_INCLUDE_DIR``
#   the NETCDF include directory

set(_NETCDF_PATHS PATHS
	"[HKEY_LOCAL_MACHINE\\SOFTWARE\\NetCDF;InstallPath]"
  )
if (DEFINED ENV{CONDA_PREFIX})
	list(APPEND _NETCDF_PATHS "$ENV{CONDA_PREFIX}")
endif()

if(DEFINED ENV{NETCDF4_ROOT})
	list(APPEND _NETCDF_PATHS "$ENV{NETCDF4_ROOT}")
endif()

find_path(NetCDF_INCLUDE_DIR netcdf.h ${_NETCDF_PATHS} PATH_SUFFIXES include)

if (NOT NetCDF_LIBRARIES)
    find_library(NetCDF_LIBRARY_RELEASE NAMES netcdf ${_NETCDF_PATHS} PATH_SUFFIXES lib)
    find_library(NetCDF-Fortran_LIBRARY_RELEASE NAMES netcdff ${_NETCDF_PATHS} PATH_SUFFIXES lib)

    include(${CMAKE_CURRENT_LIST_DIR}/SelectLibraryConfigurations.cmake)
    SELECT_LIBRARY_CONFIGURATIONS(NetCDF)
    SELECT_LIBRARY_CONFIGURATIONS(NetCDF-Fortran)

else ()
    file(TO_CMAKE_PATH "${NetCDF_LIBRARIES}" NetCDF_LIBRARIES)
    file(TO_CMAKE_PATH "${NetCDF-Fortran_LIBRARIES}" NetCDF-Fortran_LIBRARIES)
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NetCDF
                                  REQUIRED_VARS NetCDF_LIBRARIES NetCDF_INCLUDE_DIR
                                  )

if (NetCDF_FOUND)
   set(NetCDF_INCLUDE_DIRS ${NetCDF_INCLUDE_DIR})
   include(${CMAKE_CURRENT_LIST_DIR}/CMakePushCheckState.cmake)
   cmake_push_check_state()
   set(CMAKE_REQUIRED_QUIET ${NetCDF_FIND_QUIETLY})
   set(CMAKE_REQUIRED_INCLUDES ${NetCDF_INCLUDE_DIR})
   set(CMAKE_REQUIRED_LIBRARIES ${NetCDF_LIBRARIES})
   cmake_pop_check_state()

   if(NOT TARGET NetCDF::Fortran)
      add_library(NetCDF::Fortran UNKNOWN IMPORTED)
      set_target_properties(NetCDF::Fortran PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${NetCDF_INCLUDE_DIRS}")

      if(NetCDF-Fortran_LIBRARY_RELEASE)
        set_property(TARGET NetCDF::Fortran APPEND PROPERTY
	  IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(NetCDF::Fortran PROPERTIES
          IMPORTED_LOCATION_RELEASE "${NetCDF-Fortran_LIBRARY_RELEASE}")
      endif()

      if(NOT NetCDF-Fortran_LIBRARY_RELEASE AND NOT NetCDF-Fortran_LIBRARY_DEBUG)
        set_property(TARGET NetCDF::Fortran APPEND PROPERTY
          IMPORTED_LOCATION "${NetCDF-Fortran_LIBRARY}")
      endif()
    endif()

    if(NOT TARGET NetCDF::NetCDF)
      add_library(NetCDF::NetCDF UNKNOWN IMPORTED)
      set_target_properties(NetCDF::NetCDF PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${NetCDF_INCLUDE_DIRS}")

      if(NetCDF_LIBRARY_RELEASE)
        set_property(TARGET NetCDF::NetCDF APPEND PROPERTY
          IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(NetCDF::NetCDF PROPERTIES
          IMPORTED_LOCATION_RELEASE "${NetCDF_LIBRARY_RELEASE}")
      endif()

      if(NOT NetCDF_LIBRARY_RELEASE AND NOT NetCDF_LIBRARY_DEBUG)
        set_property(TARGET NetCDF::NetCDF APPEND PROPERTY
          IMPORTED_LOCATION "${NetCDF_LIBRARY}")
      endif()
    endif()
endif ()

mark_as_advanced(NetCDF_INCLUDE_DIR)

