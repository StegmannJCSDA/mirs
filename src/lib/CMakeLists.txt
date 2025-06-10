include(AddApp)
include(TestBigEndian)
TEST_BIG_ENDIAN(MIRS_TEST_ENDIANNESS_RESULT)
if(MIRS_TEST_ENDIANNESS_RESULT)
	set(MIRS_ENDIAN_PLATFORM "big")
else()
	set(MIRS_ENDIAN_PLATFORM "little")
endif()


include(files.cmake)
add_library(MiRS STATIC ${libmirs_srcs})
addlib_f(MiRS MiRS)
add_library(MiRS-f INTERFACE)
#target_link_libraries(MiRS-f INTERFACE MiRS CRTM-f hdf5::hdf5_fortran hdf5::hdf5)
target_link_libraries(MiRS-f INTERFACE MiRS crtm hdf5::hdf5_fortran hdf5::hdf5)
INSTALL(TARGETS MiRS-f EXPORT MiRSTargets)
addpack(MiRS-f)
target_link_libraries(MiRS hdf5::hdf5_fortran hdf5::hdf5 crtm)
target_include_directories(MiRS INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
	$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/MiRS>
	$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
	$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/MiRS>
	)
#set_target_properties(MiRS PROPERTIES
#	Fortran_STANDARD 2003
#	Fortran_STANDARD_REQUIRED YES
#	Fortran_FORMAT FREE
#	)

# NOTE: This is a terrible option, as it only applies to the main program being compiled.
# This causes considerable problems when mixing sources of different endianness.
if(CMAKE_Fortran_COMPILER_ID MATCHES GNU)
	if (WIN32)
		if (MIRS_ENDIAN_PLATFORM MATCHES little)
			target_compile_options(MiRS PRIVATE /fconvert=big-endian)
			target_compile_options(MiRS-f INTERFACE /fconvert=big-endian)
		endif()
		target_compile_options(MiRS PRIVATE /fimplicit=none)
		target_compile_options(MiRS-f INTERFACE /fimplicit=none)
	else()
		if (MIRS_ENDIAN_PLATFORM MATCHES little)
			target_compile_options(MiRS PRIVATE -fconvert=big-endian)
			target_compile_options(MiRS-f INTERFACE -fconvert=big-endian)
		endif()
		target_compile_options(MiRS PRIVATE -fimplicit-none -fbounds-check -ffpe-trap=overflow,zero,invalid )
		target_compile_options(MiRS-f INTERFACE -fimplicit-none -fbounds-check -ffpe-trap=overflow,zero,invalid )
	endif()
elseif(CMAKE_Fortran_COMPILER_ID MATCHES Intel)
	if(NOT WIN32)
		target_compile_options(MiRS PRIVATE -assume byterecl)
		target_compile_options(MiRS-f INTERFACE -assume byterecl)
		target_compile_options(MiRS PRIVATE -convert big_endian)
		target_compile_options(MiRS-f INTERFACE -convert big_endian)
	else()
		target_compile_options(MiRS PRIVATE /assume:byterecl)
		target_compile_options(MiRS-f INTERFACE /assume:byterecl)
		target_compile_options(MiRS PRIVATE /convert:big_endian)
		target_compile_options(MiRS-f INTERFACE /convert:big_endian)
	endif()
else()
	message(WARNING "Unsupported compiler ${CMAKE_Fortran_COMPILER_ID}. No idea what compiler options to set for MiRS.")
endif()



