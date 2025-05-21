function(GetMiRSversion)

file(STRINGS ${CMAKE_SOURCE_DIR}/setup/dap_version.txt dapver_strings)
file(STRINGS ${CMAKE_SOURCE_DIR}/version.txt ver_strings)
list(GET dapver_strings 0 iMIRS_MAJOR)
list(GET dapver_strings 1 iMIRS_MINOR)
list(GET ver_strings 0 iMIRS_REVISION)
set(MIRS_MAJOR ${iMIRS_MAJOR} PARENT_SCOPE)
set(MIRS_MINOR ${iMIRS_MINOR} PARENT_SCOPE)
set(MIRS_REVISION ${iMIRS_REVISION} PARENT_SCOPE)

set(MIRS_VERSION ${iMIRS_MAJOR}.${iMIRS_MINOR}.${iMIRS_REVISION} PARENT_SCOPE)
endfunction()

GetMiRSversion()
message(STATUS "This is MiRS version ${MIRS_VERSION}.")

if(NOT CMAKE_INSTALL_PREFIX)
	#	set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories." FORCE)
	set(CMAKE_INSTALL_PREFIX "/usr/local/MiRS/MiRS-${MIRS_VERSION}" CACHE STRING "Install path prefix, prepended onto install directories." FORCE)

endif()
if(NOT CPACK_PACKAGING_INSTALL_PREFIX)
	#set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local/MiRS/MiRS-${MIRS_VERSION}" CACHE STRING "Packaging path prefix." FORCE)
	set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()

