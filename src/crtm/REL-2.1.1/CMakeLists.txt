include(AddApp)
include(TestBigEndian)

set(CRTM_MAJOR 2)
set(CRTM_MINOR 1)
set(CRTM_SUBMINOR 1)

set(CRTM_SVNREVISION 99117)
TEST_BIG_ENDIAN(CRTM_TEST_ENDIANNESS_RESULT)
if(CRTM_TEST_ENDIANNESS_RESULT)
	set(CRTM_ENDIAN_PLATFORM "big")
else()
	set(CRTM_ENDIAN_PLATFORM "little")
endif()

option(CRTM_IO_ENDIANNESS_IS_BIG "Is the I/O big endian?" YES)
if(CRTM_IO_ENDIANNESS_IS_BIG)
	set(CRTM_ENDIAN_IO "big")
else()
	set(CRTM_ENDIAN_IO "little")
	if(CRTM_ENDIAN_PLATFORM MATCHES big)
		error("Little-endian I/O on big-endian platforms not supported via compiler switches")
	endif()
endif()
mark_as_advanced(CRTM_IO_ENDIANNESS_IS_BIG)

# The CRTM library-specific setup

include(files.cmake)

add_library(CRTM STATIC ${crtm_libsrcs})
addlib_f(CRTM CRTM)
add_library(CRTM-f INTERFACE)
target_link_libraries(CRTM-f INTERFACE CRTM)
INSTALL(TARGETS CRTM-f EXPORT MiRSTargets)
addpack(CRTM-f)

#target_include_directories(CRTM INTERFACE "${INSTALL_CMAKE_DIR}/${REL_INCLUDE_DIR}/CRTM")
target_include_directories(CRTM INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
	$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/CRTM>
	$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INSTALL_INCLUDE_DIR}>
	$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INSTALL_INCLUDE_DIR}/CRTM>
	)
set_target_properties(CRTM PROPERTIES
	Fortran_STANDARD 2003
	Fortran_STANDARD_REQUIRED YES
	Fortran_FORMAT FREE
	)
# Little endian vs big
if (CRTM_ENDIAN_IO MATCHES little)
	target_compile_definitions(CRTM PUBLIC "LITTLE_ENDIAN")
endif()

# ...Big-endian I/O on little-endian platforms
# NOTE: This is a terrible option, as it only applies to the main program being compiled.
# This causes considerable problems when mixing sources of different endianness.
if (CRTM_ENDIAN_IO MATCHES big)
	if (CRTM_ENDIAN_PLATFORM MATCHES little)
		if(CMAKE_Fortran_COMPILER_ID MATCHES GNU)
			if (WIN32)
				target_compile_options(CRTM PRIVATE /fconvert=big-endian)
				target_compile_options(CRTM-f INTERFACE /fconvert=big-endian)
			else()
				target_compile_options(CRTM PRIVATE -fconvert=big-endian)
				target_compile_options(CRTM-f INTERFACE -fconvert=big-endian)
			endif()
		elseif(CMAKE_Fortran_COMPILER_ID MATCHES Intel)
			if (NOT WIN32)
				target_compile_options(CRTM PRIVATE -convert big_endian)
				target_compile_options(CRTM-f INTERFACE -convert big_endian)
			else()
				target_compile_options(CRTM PRIVATE /convert:big_endian)
				target_compile_options(CRTM-f INTERFACE /convert:big_endian)
			endif()
		elseif(CMAKE_Fortran_COMPILER_ID MATCHES PGI)
			target_compile_options(CRTM PRIVATE -Mbyteswapio)
			target_compile_options(CRTM-f INTERFACE -Mbyteswapio)
		else()
			message(WARNING "unhandled compiler: cannot set endianness flags for compiler ${CMAKE_Fortran_COMPILER_ID}")
		endif()
	endif()
endif()

# ...Special case the ifort compiler to force byte-unit record lengths
if(CMAKE_Fortran_COMPILER_ID MATCHES Intel)
	if(NOT WIN32)
		target_compile_options(CRTM PRIVATE -assume byterecl)
		target_compile_options(CRTM-f INTERFACE -assume byterecl)
	else()
		target_compile_options(CRTM PRIVATE /assume:byterecl)
		target_compile_options(CRTM-f INTERFACE /assume:byterecl)
	endif()
elseif(CMAKE_Fortran_COMPILER_ID MATCHES PGI)
	# No need to set anything, as PGI's compiler assumes this automatically
elseif(CMAKE_Fortran_COMPILER_ID MATCHES GNU)
	#message("Unknown option for gfortran to match Intel's -assume byterecl")
else()
	message(WARNING "unhandled compiler: cannot set byterecl flags for compiler ${CMAKE_Fortran_COMPILER_ID}")
endif()

# Other options
if(CMAKE_Fortran_COMPILER_ID MATCHES Intel)
elseif(CMAKE_Fortran_COMPILER_ID MATCHES GNU)
	if (WIN32)
		target_compile_options(CRTM PRIVATE /fimplicit=none)
		target_compile_options(CRTM-f INTERFACE /fimplicit=none)
	else()
		target_compile_options(CRTM PRIVATE -fimplicit-none -fno-second-underscore -frecord-marker=4 -funroll-loops)
		target_compile_options(CRTM-f INTERFACE -fimplicit-none -fno-second-underscore -frecord-marker=4 -funroll-loops)
	endif()
else()
	message(WARNING "Unsupported compiler ${CMAKE_Fortran_COMPILER_ID}. No idea what compiler options to set for crtm.")
endif()


