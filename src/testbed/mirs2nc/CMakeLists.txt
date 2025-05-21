include(AddApp)
option(MIRS_BUILD_MIRS2NC "Build mirs2nc" ON)
if (MIRS_BUILD_MIRS2NC)

	addapp_std(mirs2nc mirs2nc.cpp mirs2nc_class.cpp defdim.cpp gnrt_nc_img.cpp gnrt_nc_snd.cpp 
		jd2md.cpp leapyear.cpp memallocdep.cpp memallocedr.cpp openfs_sw.cpp 
		rmirs_dep.cpp rmirs_edr.cpp rmirs_wnetcdf.cpp set_nc_img.cpp set_nc_snd.cpp
		utc2hms.cpp wrt_nc_gdata.cpp wrt_nc_snd.cpp wrt_nc_img.cpp
		constants.h defaults.h defvar.h common_header.h swath.h
		)
	target_link_libraries(mirs2nc hdf5::hdf5-shared NetCDF::NetCDF)
	# Same for gfortran in Intel
	target_compile_options(mirs2nc PRIVATE -mcmodel=large -std=c++11)

#[[
	addapp_std(mirs2nc mirs2nc_class.c rmirs_wnetcdf.c init_sw.c rmirs_dep.c rmirs_edr.c
		set_nc_snd.c gnrt_nc_snd.c wrt_nc_snd.c openfs_sw.c
		utc2hms.c jd2md.c leapyear.c swath.c
		set_nc_img.c gnrt_nc_img.c wrt_nc_img.c wrt_nc_gdata.c
		constants.h  defaults.h  n20_header.h  npp_header.h  swath.h
		)
	target_link_libraries(mirs2nc hdf5::hdf5-shared NetCDF::NetCDF)
	# Same for gfortran in Intel
	target_compile_options(mirs2nc PRIVATE -mcmodel=large -std=c++11)
]]
endif()

