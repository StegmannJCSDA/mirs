option(MIRS_BUILD_FORTRAN "Build the Fortran components" ON)
list(APPEND MIRS_LANGS C CXX)
if(MIRS_BUILD_FORTRAN)
	list(APPEND MIRS_LANGS Fortran)
endif()
message(STATUS "Enabling languages ${MIRS_LANGS}.")


option(USE_OLD_CPP03_ABI "C++11 led to changes in the GLIB C++ ABI. Turn this on if you are linking against old system libraries" OFF)
mark_as_advanced(USE_OLD_CPP03_ABI)


option(SAT_GPM "Build GPM programs" ON)

option(SAT_TRMM "Build TRMM programs" ON)

option(SAT_FY3RI "Build fy3ri programs" OFF)


option(GETDEPS "Get dependencies and build MiRS. Otherwise, only configure scripts." ON)
mark_as_advanced(GETDEPS)


option(INSTALL_DATA "Install / package the data directory. " ON)
mark_as_advanced(INSTALL_DATA)

if (DEFINED MIRS_TARGETS_TO_PACK)
	unset(MIRS_TARGETS_TO_PACK CACHE)
endif()

