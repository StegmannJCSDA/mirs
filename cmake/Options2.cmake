# These options go after the project statement

# Global definitions
set(MIRS_STANDALONE_PROJECT OFF)
if ( CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME )
	set( MIRS_STANDALONE_PROJECT ON )
else()
	set( MIRS_STANDALONE_PROJECT OFF )
endif()

message(STATUS "Is the MiRS build standalone? ${MIRS_STANDALONE_PROJECT}")


if(MIRS_STANDALONE_PROJECT)
	#set(CMAKE_CXX_STANDARD 17)
	#set(CMAKE_CXX_STANDARD_REQUIRED YES)
	set(CMAKE_CXX_EXTENSIONS OFF)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
	endif()
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Release Debug RelWithDebInfo MinSizeRel)
	if(NOT CMAKE_INSTALL_PREFIX)
		set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/install" CACHE STRING "Set the installation destination directory." FORCE)
	endif()

endif()

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

