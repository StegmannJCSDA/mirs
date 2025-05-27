macro(addpack tgtname)
	if (DEFINED MIRS_TARGETS_TO_PACK)
		set(MIRS_TARGETS_TO_PACK ${MIRS_TARGETS_TO_PACK} ${tgtname} CACHE INTERNAL "Target list" FORCE)
	else()
		set(MIRS_TARGETS_TO_PACK ${tgtname} CACHE INTERNAL "Target list" FORCE)
	endif()
endmacro(addpack tgtname)

macro(addapp_base appname)
	set_target_properties( ${appname}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		)
endmacro(addapp_base appname)

macro(addapp appname)
	addapp_base(${appname})
	INSTALL(TARGETS ${appname}
		EXPORT MiRSTargets
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		COMPONENT Applications
		)
endmacro(addapp appname)

macro(addapp_std appname)
	# Pass appname and the source files
	add_executable(${appname} ${ARGN})
	addapp(${appname})
	set_target_properties( ${appname} PROPERTIES FOLDER "Apps")
endmacro(addapp_std appname)

macro(addapp_test appname foldername)
	#target_include_directories(${appname} 
	#	PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../../lib/private )
	addapp_base(${appname})
	set_target_properties( ${appname} PROPERTIES FOLDER "${foldername}")
endmacro(addapp_test appname foldername)


macro(addlib) #libname libshared)
	set(libname "${ARGV0}")
	if ("" STREQUAL "${ARGV1}")
		set(isshared "STATIC")
	else()
		set(isshared "${ARGV1}")
	endif()
	if ("" STREQUAL "${ARGV2}")
		set(libshared ${libname})
	else()
		set(libshared ${ARGV2})
	endif()
	#message("addlib name: ${libname}, shared name: ${libshared}, is shared: ${isshared}")

	set_target_properties( ${libname}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		)
	# These are for symbol export
	target_compile_definitions(${libname} PRIVATE ${libshared}_EXPORTING)
	target_compile_definitions(${libname} PUBLIC ${libshared}_SHARED=$<STREQUAL:${isshared},SHARED>)

	INSTALL(TARGETS ${libname}
		EXPORT MiRSTargets
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		COMPONENT Libraries
		)
	addpack(${libname})
endmacro(addlib )

macro(addlib_f libname libincdir)
	addlib(${libname} STATIC)
	set_target_properties( ${libname}
		PROPERTIES
		Fortran_MODULE_DIRECTORY "${CMAKE_BINARY_DIR}/include/${libincdir}"
		)
	install(DIRECTORY "${CMAKE_BINARY_DIR}/include/${libincdir}"
		DESTINATION include
		COMPONENT Headers
		)

endmacro(addlib_f libname)





