macro(amend_basic_libs libname resname)
	get_target_property(lib_debug ${libname} IMPORTED_LOCATION_DEBUG)
	get_target_property(lib_release ${libname} IMPORTED_LOCATION_RELEASE)
	if (WIN32 AND NOT CYGWIN)
		string(REPLACE "\.lib" ".dll" dll_debug_a ${lib_debug} )
		string(REPLACE "\.lib" ".dll" dll_release_a ${lib_release} )
		string(REPLACE "/lib/" "/bin/" dll_debug ${dll_debug_a} )
		string(REPLACE "/lib/" "/bin/" dll_release ${dll_release_a} )
	else()
		set(dll_debug ${lib_debug})
		set(dll_release ${lib_release})
	endif()

	add_library(${resname} UNKNOWN IMPORTED)
	set_property(TARGET ${resname} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	set_target_properties(${resname} PROPERTIES IMPORTED_LOCATION_RELEASE "${dll_release}")
	set_property(TARGET ${resname} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
	set_target_properties(${resname} PROPERTIES IMPORTED_LOCATION_DEBUG "${dll_debug}")
	set_property(TARGET ${resname} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
	set_target_properties(${resname} PROPERTIES IMPORTED_LOCATION_RELWITHDEBINFO "${dll_debug}")
	set_property(TARGET ${resname} APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
	set_target_properties(${resname} PROPERTIES IMPORTED_LOCATION_MINSIZEREL "${dll_debug}")
	#message("${dll_debug} ${dll_release}")
endmacro(amend_basic_libs libname resname)


if (DEFINED ENV{CONDA_PREFIX})
	set(ZLIB_ROOT "$ENV{CONDA_PREFIX}") # CACHE STRING "Include dir for zlib")
endif()

if (WIN32 AND NOT CYGWIN)
	set(ZLIB_ROOT "C:/Program Files/zlib")
endif()
find_package(ZLIB REQUIRED)
amend_basic_libs(ZLIB::ZLIB impZLIB::ZLIB)

if(MIRS_BUILD_FORTRAN)
	find_package(HDF5 REQUIRED COMPONENTS C HL Fortran )
else()
	find_package(HDF5 REQUIRED COMPONENTS C HL )
endif()

# These get set: HDF5_C_INCLUDE_DIRS HDF5_C_LIBRARIES HDF5_C_HL_LIBRARIES HDF5_C_DEFINITIONS
# Unfortunately, the hdf5:: targets are not always available. Ugh.
if (NOT TARGET hdf5::hdf5-shared)
	add_library(hdf5::hdf5-shared UNKNOWN IMPORTED)
	set_property(TARGET hdf5::hdf5-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	#message("${HDF5_C_LIBRARIES}")
	list(GET HDF5_C_LIBRARIES 0 HDF5_MAIN_C_LIB)
	list(REMOVE_AT HDF5_C_LIBRARIES 0)
	#message("${HDF5_MAIN_C_LIB}")
	#message("${HDF5_C_LIBRARIES}")
	set_target_properties(hdf5::hdf5-shared PROPERTIES IMPORTED_LOCATION ${HDF5_MAIN_C_LIB})
	set_target_properties(hdf5::hdf5-shared PROPERTIES LINK_LIBRARIES "${HDF5_C_LIBRARIES}")
	if(DEFINED HDF5_C_DEFINITIONS)
		set_target_properties(hdf5::hdf5-shared PROPERTIES COMPILE_DEFINITIONS "${HDF5_C_DEFINITIONS}")
	endif()
	if(NOT "${HDF5_C_INCLUDE_DIRS}" STREQUAL "")
		set_target_properties(hdf5::hdf5-shared PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${HDF5_C_INCLUDE_DIRS}")
		set_target_properties(hdf5::hdf5-shared PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${HDF5_C_INCLUDE_DIRS}")

	endif()
	#target_include_directories(hdf5::hdf5-shared SYSTEM INTERFACE ${HDF5_C_INCLUDE_DIRS})
endif()

if (NOT TARGET hdf5::hdf5_hl-shared)
	add_library(hdf5::hdf5_hl-shared UNKNOWN IMPORTED)
	set_property(TARGET hdf5::hdf5_hl-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	list(GET HDF5_HL_LIBRARIES 0 HDF5_MAIN_HL_LIB)
	list(REMOVE_AT HDF5_HL_LIBRARIES 0)

	#message("${HDF5_C_HL_LIBRARIES} ... ${HDF5_HL_LIBRARIES}")
	# HDF5_hdf5_hl_LIBRARY_RELEASE
	if(NOT "${HDF5_MAIN_HL_LIB}" STREQUAL "")
		set_target_properties(hdf5::hdf5_hl-shared PROPERTIES IMPORTED_LOCATION ${HDF5_MAIN_HL_LIB})
		set_target_properties(hdf5::hdf5_hl-shared PROPERTIES LINK_LIBRARIES "${HDF5_HL_LIBRARIES}")

	endif()
endif()

if(MIRS_BUILD_FORTRAN)
if (NOT TARGET hdf5::hdf5_fortran)
	add_library(hdf5::hdf5_fortran UNKNOWN IMPORTED)
	set_property(TARGET hdf5::hdf5_fortran APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	list(GET HDF5_Fortran_LIBRARIES 0 HDF5_MAIN_FORTRAN_LIB)
	list(REMOVE_AT HDF5_Fortran_LIBRARIES 0)

	#message("${HDF5_C_HL_LIBRARIES} ... ${HDF5_HL_LIBRARIES}")
	# HDF5_hdf5_hl_LIBRARY_RELEASE
	if(NOT "${HDF5_MAIN_FORTRAN_LIB}" STREQUAL "")
		set_target_properties(hdf5::hdf5_fortran PROPERTIES IMPORTED_LOCATION ${HDF5_MAIN_FORTRAN_LIB})
		set_target_properties(hdf5::hdf5_fortran PROPERTIES LINK_LIBRARIES "${HDF5_FORTRAN_LIBRARIES}")

	endif()
endif()
mark_as_advanced(
	HDF5_Fortran_LIBRARY_dl HDF5_Fortran_LIBRARY_hdf5 HDF5_Fortran_LIBRARY_hdf5_fortran
	HDF5_Fortran_LIBRARY_hdf5_hl
	HDF5_Fortran_LIBRARY_hdf5hl_fortran
	HDF5_Fortran_LIBRARY_m
	HDF5_Fortran_LIBRARY_pthread
	HDF5_Fortran_LIBRARY_rt
	HDF5_Fortran_LIBRARY_z
	HDF5_Fortran_LIBRARY_sz
	)
endif()

# These are rather polluting
mark_as_advanced(
	HDF5_C_LIBRARY_dl HDF5_C_LIBRARY_hdf5 HDF5_C_LIBRARY_hdf5_hl 
	HDF5_C_LIBRARY_rt
	HDF5_C_LIBRARY_m HDF5_C_LIBRARY_pthread HDF5_C_LIBRARY_sz HDF5_C_LIBRARY_z HDF5_DIR jconfig_dir
	)

# This is set: HDF5_DIFF_EXECUTABLE
# Use it to extract the bin directory component.
get_filename_component(PATHS_H5BINDIR ${HDF5_DIFF_EXECUTABLE} DIRECTORY )

# CMake find_package script creator stuff

find_package(HDF4 REQUIRED)
find_package(HDF-EOS2 REQUIRED)
find_package(JPEG REQUIRED)

#find_package(HDF-EOS5)

find_package(NetCDF)
find_package(SZIP)


#get_filename_component(
