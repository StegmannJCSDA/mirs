include(AddApp)

# mt is buggy. Go figure.
if (MIRS_BUILD_FORTRAN)
	add_library(mt STATIC src/mt/mt.cc include/mt/mt.h include/mt/MeasurementType.h)
	addlib(mt STATIC)
	target_link_libraries(mt PUBLIC hdf5::hdf5-shared)
	set_target_properties( mt PROPERTIES FOLDER "Libs")
	if(USE_OLD_CPP03_ABI)
		target_compile_definitions(mt PUBLIC _GLIBCXX_USE_CXX11_ABI=0)
	endif()
	target_include_directories(mt PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/lib_cpp/include/mt>
		$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/mt>
		)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/lib_cpp/include/mt
		DESTINATION include
		)


endif()

