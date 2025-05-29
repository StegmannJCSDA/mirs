include(AddApp)

add_library(calib_ssmis_sub STATIC calib_ssmis_sub.f90)
addlib(calib_ssmis_sub STATIC)
target_link_libraries(calib_ssmis_sub CRTM MiRS-f)

addapp_std(rdr2tdr_n18_amsua rdr2tdr_n18_amsua.f90)
target_link_libraries(rdr2tdr_n18_amsua MiRS-f)
addapp_std(rdr2tdr_n18_mhs rdr2tdr_n18_mhs.f90)
target_link_libraries(rdr2tdr_n18_mhs MiRS-f)
addapp_std(rdr2tdr_metopA_amsua rdr2tdr_metopA_amsua.f90)
target_link_libraries(rdr2tdr_metopA_amsua MiRS-f)
addapp_std(rdr2tdr_metopA_mhs rdr2tdr_metopA_mhs.f90)
target_link_libraries(rdr2tdr_metopA_mhs MiRS-f)
addapp_std(calib_f16_ssmis calib_f16_ssmis.f90)
target_link_libraries(calib_f16_ssmis MiRS-f)
target_link_libraries(calib_f16_ssmis calib_ssmis_sub)
addapp_std(rdr2tdr_f16_ssmis rdr2tdr_f16_ssmis.f90)
target_link_libraries(rdr2tdr_f16_ssmis MiRS-f)
target_link_libraries(rdr2tdr_f16_ssmis calib_ssmis_sub)
addapp_std(rdr2tdr_n19_amsua rdr2tdr_n19_amsua.f90)
target_link_libraries(rdr2tdr_n19_amsua MiRS-f)
addapp_std(rdr2tdr_n19_mhs rdr2tdr_n19_mhs.f90)
target_link_libraries(rdr2tdr_n19_mhs MiRS-f)
addapp_std(rdr2tdr_f18_ssmis rdr2tdr_f18_ssmis.f90)
target_link_libraries(rdr2tdr_f18_ssmis MiRS-f)

addapp_std(rdr2tdr_npp_atms rdr2tdr_npp_atms.cc)
target_link_libraries(rdr2tdr_npp_atms MiRS hdf5::hdf5-shared)

addapp_std(rdr2tdr_npp_atms_proxy rdr2tdr_npp_atms_proxy.cc)
target_link_libraries(rdr2tdr_npp_atms_proxy MiRS mt hdf5::hdf5-shared)

if (SAT_FY3RI)
	addapp_std(rdr2tdr_fy3ri_mwri rdr2tdr_fy3ri_mwri.f90)
	target_link_libraries(rdr2tdr_fy3ri_mwri MiRS-f)
	target_link_libraries(rdr2tdr_fy3ri_mwri calib_ssmis_sub hdf5::hdf5_fortran JPEG::JPEG HDF4::HDF4 HDF4::HDF4DF ZLIB::ZLIB)
endif()


addapp_std(rdr2tdr_mtma_madras rdr2tdr_mtma_madras.cc)
target_link_libraries(rdr2tdr_mtma_madras MiRS mt)

addapp_std(rdr2tdr_mtsa_saphir rdr2tdr_mtsa_saphir.cc)
target_link_libraries(rdr2tdr_mtsa_saphir MiRS mt)


addapp_std(rdr2tdr_metopB_amsua rdr2tdr_metopB_amsua.f90)
target_link_libraries(rdr2tdr_metopB_amsua MiRS-f)
addapp_std(rdr2tdr_metopB_mhs rdr2tdr_metopB_mhs.f90)
target_link_libraries(rdr2tdr_metopB_mhs MiRS-f)
addapp_std(rdr2tdr_gcomw1_amsr2 rdr2tdr_gcomw1_amsr2.cc)
target_link_libraries(rdr2tdr_gcomw1_amsr2 MiRS hdf5::hdf5-shared)


addapp_std(rdr2tdr_f17_ssmis rdr2tdr_f17_ssmis.f90)
target_link_libraries(rdr2tdr_f17_ssmis MiRS-f)

if(SAT_GPM)
	addapp_std(rdr2tdr_gpm_gmi rdr2tdr_gpm_gmi.f90)
	target_link_libraries(rdr2tdr_gpm_gmi MiRS-f hdf5::hdf5_fortran hdf5::hdf5-shared)
endif()

#if(SAT_TRMM)
#	addapp_std(rdr2tdr_trmm_tmi rdr2tdr_trmm_tmi.f90)
#	target_link_libraries(rdr2tdr_trmm_tmi MiRS-f HDF-EOS2::HDF-EOS2 HDF4::HDF4 HDF4::HDF4DF JPEG::JPEG ZLIB::ZLIB calib_ssmis_sub)
#endif()

addapp_std(rdr2tdr_f19_ssmis rdr2tdr_f19_ssmis.f90)
target_link_libraries(rdr2tdr_f19_ssmis MiRS-f)
addapp_std(rdr2tdr_n20_atms rdr2tdr_n20_atms.cc)
target_link_libraries(rdr2tdr_n20_atms MiRS hdf5::hdf5-shared)
addapp_std(rdr2tdr_n21_atms rdr2tdr_n21_atms.cc)
target_link_libraries(rdr2tdr_n21_atms MiRS hdf5::hdf5-shared)
addapp_std(rdr2tdr_metopSGA1_mws rdr2tdr_metopSGA1_mws.cc)
target_link_libraries(rdr2tdr_metopSGA1_mws MiRS hdf5::hdf5-shared)

addapp_std(rdr2tdr_metopC_amsua rdr2tdr_metopC_amsua.f90)
target_link_libraries(rdr2tdr_metopC_amsua MiRS-f)
addapp_std(rdr2tdr_metopC_mhs rdr2tdr_metopC_mhs.f90)
target_link_libraries(rdr2tdr_metopC_mhs MiRS-f)

