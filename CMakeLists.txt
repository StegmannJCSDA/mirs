# This is the new CMake-based build system for MiRS!

# This project needs at least CMake 3.1. If your system
# does not provide this version, you can get it from
# https://cmake.org/download/

cmake_minimum_required (VERSION 3.12)
cmake_policy(SET CMP0074 OLD)
cmake_policy(SET CMP0076 OLD)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(ExternalProject)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GetMiRSversion) # Sets the MiRS version information variables.
include(Options)
project(MiRS VERSION ${MIRS_VERSION} LANGUAGES ${MIRS_LANGS})
include(Options2)

if(GETDEPS) # See cmake/Options.cmake
	include(GetDependencies)
else()
	# This has to be set for the scripts to run!!! Used in configure_file setup/paths below.
	set(PATHS_H5BINDIR "/data/starfs1/libs/hdf5-1.8.19-gfortran/bin")
include(AddDocs)
endif()
enable_testing()
include(Testing)

# Configure the paths file.
message(STATUS "Creating a custom setup/paths file.")

set(PATHS_ROOTDIR "${PROJECT_BINARY_DIR}")
set(PATHS_ROOTDIRAT "\@${PROJECT_BINARY_DIR}/src/lib_idl")

configure_file(${PROJECT_SOURCE_DIR}/setup/paths.in
	${PROJECT_BINARY_DIR}/setup/paths
	@ONLY)
configure_file(${PROJECT_SOURCE_DIR}/setup/paths_idl.pro.in
	${PROJECT_BINARY_DIR}/setup/paths_idl.pro
	@ONLY)
set(PATHS_ROOTDIR "${CMAKE_INSTALL_PREFIX}")
configure_file(${PROJECT_SOURCE_DIR}/setup/paths.in
	${PROJECT_BINARY_DIR}/setup-install/paths
	@ONLY)
configure_file(${PROJECT_SOURCE_DIR}/setup/paths_idl.pro.in
	${PROJECT_BINARY_DIR}/setup-install/paths_idl.pro
	@ONLY)
# When testing MiRS, we need to make _copies_ of these in the build directory.
# Symlinks are undesirable because we would taint the source directory.
message(STATUS "Copying the data directories from the source tree to the build location.")
file(COPY ${PROJECT_SOURCE_DIR}/data DESTINATION ${PROJECT_BINARY_DIR} )
message(STATUS "Copying the scripts and setup directories from the source tree to the build location.")
file(COPY ${PROJECT_SOURCE_DIR}/scripts DESTINATION ${PROJECT_BINARY_DIR}
	USE_SOURCE_PERMISSIONS
	)
file(COPY ${PROJECT_SOURCE_DIR}/setup DESTINATION ${PROJECT_BINARY_DIR}
	USE_SOURCE_PERMISSIONS
	PATTERN "paths" EXCLUDE
	PATTERN "paths_idl.pro" EXCLUDE
	)
file(COPY ${PROJECT_SOURCE_DIR}/logs DESTINATION ${PROJECT_BINARY_DIR} )
file(COPY ${PROJECT_SOURCE_DIR}/version.txt DESTINATION ${PROJECT_BINARY_DIR} )
message(STATUS "Copying the source directories from the source tree to the build location.")
file(COPY ${PROJECT_SOURCE_DIR}/src DESTINATION ${PROJECT_BINARY_DIR} )
file(COPY ${PROJECT_SOURCE_DIR}/gui DESTINATION ${PROJECT_BINARY_DIR} )

message(STATUS "Adding the libraries and executables.")
if(GETDEPS) # See cmake/Options.cmake
	add_subdirectory(src)
	include(Installing)
    include(Packaging)
endif()


