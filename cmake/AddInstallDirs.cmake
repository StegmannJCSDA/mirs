macro(addInstallDirs basename ) 
set (INSTALL_DIR_SUBFOLDERS ON)
set (INSTALL_LIB_DIR_DEFAULT lib)
set (INSTALL_SHARE_DIR_DEFAULT share)
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_DATA_DIR data CACHE PATH "Installation directory for data")
set(INSTALL_GUI_DIR gui CACHE PATH "Installation directory for gui")
set(INSTALL_LOGS_DIR logs CACHE PATH "Installation directory for logs")
set(INSTALL_MSPPS_DIR mspps CACHE PATH "Installation directory for mspps")
set(INSTALL_SCRIPTS_DIR scripts CACHE PATH "Installation directory for scripts")
set(INSTALL_SETUP_DIR setup CACHE PATH "Installation directory for setup")
set(INSTALL_SRC_DIR src CACHE PATH "Installation directory for sources")
set(INSTALL_WEB_DIR web CACHE PATH "Installation directory for web files")
set(INSTALL_LIB_DIR ${INSTALL_LIB_DIR_DEFAULT} CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for headers")
set(INSTALL_SHARE_DIR ${INSTALL_SHARE_DIR_DEFAULT} CACHE PATH "Installation directory for errata")
set(INSTALL_DOC_DIR ${INSTALL_SHARE_DIR}/doc CACHE PATH "Installation directory for documentation")
set(INSTALL_DATA_DIST_DIR ${INSTALL_SHARE_DIR}/data CACHE PATH "Installation directory for package-provided data")
if (USES_PLUGINS)
	set(INSTALL_PLUGIN_DIR ${INSTALL_BIN_DIR}/plugins CACHE PATH "Installation directory for plugins")
	mark_as_advanced(INSTALL_PLUGIN_DIR)
endif()

mark_as_advanced(INSTALL_BIN_DIR INSTALL_DATA_DIR INSTALL_GUI_DIR INSTALL_LOGS_DIR INSTALL_MSPPS_DIR
	INSTALL_SCRIPTS_DIR INSTALL_SETUP_DIR INSTALL_SRC_DIR INSTALL_WEB_DIR INSTALL_LIB_DIR 
	INSTALL_INCLUDE_DIR INSTALL_SHARE_DIR INSTALL_DOC_DIR INSTALL_DATA_DIST_DIR)

if(WIN32 AND NOT CYGWIN)
	set(DEF_INSTALL_CMAKE_DIR_BASE CMake)
else()
	set(DEF_INSTALL_CMAKE_DIR_BASE ${INSTALL_LIB_DIR}/CMake/${basename})
endif()
set(INSTALL_CMAKE_DIR_BASE ${DEF_INSTALL_CMAKE_DIR_BASE} CACHE PATH
	"Base installation directory for CMake files")
set(INSTALL_CMAKE_DIR ${INSTALL_CMAKE_DIR_BASE}/conf${configappend})
mark_as_advanced(INSTALL_CMAKE_DIR_BASE)


# Constructing relative and absolute paths, needed for the cmake export file header locator
# Absolute paths: ABS_INSTALL_${p}_DIR
# Relative paths: REL_${p}_DIR
set (folders LIB BIN INCLUDE CMAKE DOC DATA_DIST SHARE DATA GUI LOGS MSPPS SCRIPTS SETUP SRC WEB)
if(uses_plugins)
	SET(folders ${folders} PLUGINS)
endif()
foreach(p ${folders})
	set(var ABS_INSTALL_${p}_DIR)
	set(ABS_INSTALL_${p}_DIR ${INSTALL_${p}_DIR})
	if(NOT IS_ABSOLUTE "${${var}}")
		set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
	endif()
	#message("ABS_INSTALL_${p}_DIR - ${${var}}")
endforeach()
foreach(p ${folders})
	set(var ABS_INSTALL_${p}_DIR)
	#message("RELATIVE_PATH REL_${p}_DIR ${ABS_INSTALL_CMAKE_DIR} ${${var}}")
	file(RELATIVE_PATH REL_${p}_DIR "${ABS_INSTALL_CMAKE_DIR}" "${${var}}")
	#message("      - ${REL_${p}_DIR}")
endforeach()

set(ABS_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
file(RELATIVE_PATH REL_INSTALL_DIR "${ABS_INSTALL_CMAKE_DIR}" "${ABS_INSTALL_DIR}")

set (RBIN_DIR ${INSTALL_CMAKE_DIR}/${REL_BIN_DIR})

endmacro(addInstallDirs basename )

