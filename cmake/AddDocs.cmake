set(MIRS_BUILD_DOCUMENTATION_DEFAULT "No")
if(MIRS_STANDALONE_PROJECT AND BUILD_DOCUMENTATION)
	set(MIRS_BUILD_DOCUMENTATION_DEFAULT "Separate")
endif()

if(NOT MIRS_BUILD_DOCUMENTATION)
	set(MIRS_BUILD_DOCUMENTATION ${MIRS_BUILD_DOCUMENTATION_DEFAULT} CACHE STRING 
	"Choose if MiRS documentation is generated. Options: No, Separate, Integrated." FORCE)
	set_property(CACHE MIRS_BUILD_DOCUMENTATION PROPERTY STRINGS No Separate Integrated)
endif()

message(STATUS "MiRS documentation will be built: ${MIRS_BUILD_DOCUMENTATION}.")
if(MIRS_BUILD_DOCUMENTATION STREQUAL "No")
else()
	#if (NOT BUILD_DOCUMENTATION)
		#message(STATUS "No MiRS documentation will be built.")
		#else()
		find_package(Doxygen)

		if (NOT DOXYGEN_FOUND)
			message(SEND_ERROR "MiRS documentation build requested but Doxygen is not found.")
		endif()

		if (NOT DOXYGEN_DOT_EXECUTABLE)
			set(HAVE_DOT NO)
		else()
			set(HAVE_DOT YES)
		endif()

		configure_file(${PROJECT_SOURCE_DIR}/cmake/Doxyfile.in
			"${PROJECT_BINARY_DIR}/Doxyfile" @ONLY)

		 # This builds the html docs
		 add_custom_target(MIRS-docs-html ${ALL_FLAG}
			${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
			COMMENT "Generating API html documentation with Doxygen" VERBATIM
		)
		# This builds the latex docs
		#    add_custom_target(doc-latex ${ALL_FLAG}
		#        latex refman.tex
		#        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/docs/latex
		#        COMMENT "Generating API pdf documentation with Doxygen" VERBATIM
		#    )

		if (MIRS_BUILD_DOCUMENTATION STREQUAL "INTEGRATED")
			add_custom_target(MIRS-docs ALL DEPENDS MIRS-docs-html)
			# Provides html and pdf
			install(CODE "execute_process(COMMAND ${CMAKE_BUILD_TOOL} docs)")
			# html
			install(DIRECTORY ${CMAKE_BINARY_DIR}/docs/html/ DESTINATION ${CMAKE_INSTALL_DOCDIR}/html)
			# pdf
			#    install(DIRECTORY ${CMAKE_BINARY_DIR}/docs/latex/ DESTINATION ${CMAKE_INSTALL_DOCDIR}/latex)
		else()
			add_custom_target(MIRS-docs DEPENDS MIRS-docs-html)
		endif()

		if(MIRS_STANDALONE_PROJECT)
			add_custom_target(docs DEPENDS MIRS-docs)
			add_custom_target(doc DEPENDS MIRS-docs)
		endif()


		#endif()
endif()

