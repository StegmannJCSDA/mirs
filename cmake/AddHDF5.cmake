if(BUILD_FORTRAN)
	find_package(HDF5 REQUIRED COMPONENTS C Fortran HL)
else()
	find_package(HDF5 REQUIRED COMPONENTS C HL)
endif()
# These get set: HDF5_C_INCLUDE_DIRS HDF5_C_LIBRARIES HDF5_C_HL_LIBRARIES HDF5_C_DEFINITIONS
# Unfortunately, the hdf5:: targets are not always available. Ugh.
if (NOT TARGET hdf5::hdf5-shared)
	add_library(hdf5::hdf5-shared UNKNOWN IMPORTED)
	set_property(TARGET hdf5::hdf5-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	list(GET HDF5_C_LIBRARIES 0 HDF5_MAIN_C_LIB)
	list(REMOVE_AT HDF5_C_LIBRARIES 0)
	set_target_properties(hdf5::hdf5-shared PROPERTIES IMPORTED_LOCATION ${HDF5_MAIN_C_LIB})
	set_target_properties(hdf5::hdf5-shared PROPERTIES LINK_LIBRARIES "${HDF5_C_LIBRARIES}")
	set_target_properties(hdf5::hdf5-shared PROPERTIES COMPILE_DEFINITIONS "${HDF5_C_DEFINITIONS}")
	set_target_properties(hdf5::hdf5-shared PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${HDF5_C_INCLUDE_DIRS})
	set_target_properties(hdf5::hdf5-shared PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${HDF5_C_INCLUDE_DIRS})
	#target_include_directories(hdf5::hdf5-shared SYSTEM INTERFACE ${HDF5_C_INCLUDE_DIRS})
endif()
if (NOT TARGET hdf5::hdf5_hl-shared)
	add_library(hdf5::hdf5_hl-shared UNKNOWN IMPORTED)
	set_property(TARGET hdf5::hdf5_hl-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
	set_target_properties(hdf5::hdf5_hl-shared PROPERTIES IMPORTED_LOCATION ${HDF5_C_HL_LIBRARIES})
endif()

if(BUILD_FORTRAN)
	if (NOT TARGET hdf5::hdf5_fortran)
		add_library(hdf5::hdf5_fortran UNKNOWN IMPORTED)
		set_property(TARGET hdf5::hdf5_fortran APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_target_properties(hdf5::hdf5_fortran PROPERTIES IMPORTED_LOCATION ${HDF5_Fortran_LIBRARIES})
	endif()
endif()

# These are rather polluting in the UI view, so we are hiding 
# TODO: Add Fortran libraries
mark_as_advanced(HDF5_CXX_LIBRARY_dl HDF5_CXX_LIBRARY_hdf5 HDF5_CXX_LIBRARY_hdf5_cpp
	HDF5_CXX_LIBRARY_hdf5_hl HDF5_CXX_LIBRARY_hdf5_hl_cpp HDF5_CXX_LIBRARY_m
	HDF5_CXX_LIBRARY_pthread HDF5_CXX_LIBRARY_sz HDF5_CXX_LIBRARY_z 
	HDF5_C_LIBRARY_dl HDF5_C_LIBRARY_hdf5 HDF5_C_LIBRARY_hdf5_hl 
	HDF5_C_LIBRARY_m HDF5_C_LIBRARY_pthread HDF5_C_LIBRARY_sz HDF5_C_LIBRARY_z)

