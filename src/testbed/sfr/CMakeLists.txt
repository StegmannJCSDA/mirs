list(APPEND SFR_FORT_FILES
	GET_EMISS_1DALG.f90  onelyr.f90)
list(APPEND SFR_C_FILES
   sfr.c process.c sat_data.c avn_data.c prod_data.c 
	fun_sat.c fun_tis.c mask_stype.c snowfall.c avn_add_wth.c 
	sfr_intg.c rd_mhs.c rd_gmi.c cal_vars.c rd_ssmis.c fun_tis_nn.c
   dsr_run.c fun_tis_xgb.c
	d_fun/d_darrc.c d_fun/d_jsec.c d_fun/d_sys.c 
	)

include(AddApp)
add_library(sfr_fort STATIC ${SFR_FORT_FILES})
#addapp_std(sfr ${SFR_C_FILES})
#target_link_libraries(sfr sfr_fort HDF-EOS2::HDF-EOS2 HDF4::HDF4 HDF4::HDF4DF JPEG::JPEG ZLIB::ZLIB hdf5::hdf5-shared)
#target_include_directories(sfr PRIVATE
#	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/d_fun>
#	)
if(CMAKE_Fortran_COMPILER_ID MATCHES GNU)
	if (WIN32)
		target_compile_options(sfr_fort PRIVATE /fimplicit=none)
	else()
		target_compile_options(sfr_fort PRIVATE -fimplicit-none -fbounds-check -ffpe-trap=overflow,zero,invalid )
	endif()
elseif(CMAKE_Fortran_COMPILER_ID MATCHES Intel)
	if(NOT WIN32)
		target_compile_options(sfr_fort PRIVATE -assume byterecl)
	else()
		target_compile_options(sfr_fort PRIVATE /assume:byterecl)
	endif()
else()
	message(WARNING "Unsupported compiler ${CMAKE_Fortran_COMPILER_ID}. No idea what compiler options to set for MiRS.")
endif()



#target_link_libraries(sfr sfr_fort HDF-EOS2::HDF-EOS2 HDF-EOS5::HDF-EOS5 HDF4::HDF4 HDF4::HDF4DF hdf5::hdf5_hl-shared hdf5::hdf5-shared)


