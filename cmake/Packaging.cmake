# Handles generation of binary packages (.tar.gz, .tar.bz2, .deb, .rpm).
export(TARGETS ${MIRS_TARGETS_TO_PACK}
	FILE "${PROJECT_BINARY_DIR}/MiRSTargets.cmake")
export(PACKAGE MiRS)

# CMake find_package script creator stuff # TODO: Put this in a separate script
set(pkgname "MiRS")
set(pkglibs MiRS)
set(pkgversion ${MIRS_VERSION})
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/lib")
configure_file(cmake/Pkg_Config.cmake.in
        "${PROJECT_BINARY_DIR}/MiRSConfig.cmake" @ONLY)
# ... and for the install tree
set(CONF_INCLUDE_DIRS "\${MiRS_CMAKE_DIR}/${REL_INCLUDE_DIR}") # Before WiX
#INSTALL(DIRECTORY "${PROJECT_BINARY_DIR}/environment-modules/install/icedb"
#       DESTINATION ${INSTALL_CMAKE_DIR}/${REL_SHARE_DIR}/environment-modules
#       COMPONENT Errata
#       )
configure_file(cmake/Pkg_Config.cmake.in
        "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MiRSConfig.cmake" @ONLY)
# ... for both
configure_file(cmake/Pkg_ConfigVersion.cmake.in
        "${PROJECT_BINARY_DIR}/MiRSConfigVersion.cmake" @ONLY)

install(EXPORT MiRSTargets
	DESTINATION lib COMPONENT Libraries
	)

# CPack stuff
set(CPACK_PACKAGE_NAME "MiRS")
set(CPACK_PACKAGE_VENDOR "Christopher Grassotti")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NOAA's Microwave Integrated Retrieval System")
set(CPACK_PACKAGE_VERSION "${MIRS_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MIRS_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MIRS_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MIRS_REVISION}")
set(CPACK_DEBIAN_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_RELEASE "1")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Christopher Grassotti")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/readme")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "MiRS")
set(CPACK_PACKAGE_CONTACT "Christopher Grassotti (Christopher.Grassotti@noaa.gov)")

set (CPACK_COMPONENTS_ALL
        Applications
	Base
	Data
        Documentation
        Libraries
        Headers
        Scripts
	Source
        GUI
)

set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION
	"Applications")
set(CPACK_COMPONENT_BASE_DESCRIPTION
	"Base objects required by MiRS")
set(CPACK_COMPONENT_DATA_DESCRIPTION
	"Data files required by MiRS")
set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION
	"Documentation")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION
        "The compiled libraries")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION
        "Headers for code development")
set(CPACK_COMPONENT_SOURCE_DESCRIPTION
        "The source code")
set(CPACK_COMPONENT_SCRIPTS_DESCRIPTION
        "Processing scripts")
set(CPACK_COMPONENT_GUI_DESCRIPTION
        "The GUI")

set(CPACK_COMPONENT_HEADERS_DEPENDS Libraries)
#set(CPACK_COMPONENT_EXAMPLES_DEPENDS Libraries Applications Headers)
set(CPACK_COMPONENT_APPLICATIONS_DEPENDS Libraries Headers)
set(CPACK_COMPONENT_BASE_REQUIRED 1)
set(CPACK_COMPONENT_LIBRARIES_REQUIRED 1)
set(CPACK_RPM_PACKAGE_REQUIRES
        "cmake >= 3.1, netcdf-devel, hdf5-devel, hdf5, git, zlib-devel"
        )
set(CPACK_RPM_PACKAGE_LICENSE "The MiRS license")
set(CPACK_RPM_PACKAGE_SUGGESTS
	"gcc-c++ >= 4.8, gcc-gfortran >= 4.8, gcc >= 4.8"
	)
set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "cmake (>= 3.1), libnetcdf-dev, libhdf5-dev, zlib1g-dev"
        )
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "gfortran (>= 4.8), gcc (>= 4.8), g++ (>= 4.8), hdf5-tools, git, git-lfs")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

SET(CPACK_STRIP_FILES FALSE)
SET(CPACK_SOURCE_STRIP_FILES FALSE)
SET(CPACK_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION TRUE)

# This must always be last!
include(CPack)

