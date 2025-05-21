include(AddApp)

addapp_std(Calib_generic_rad Calib_generic_rad.f90 lsq.f90)
target_link_libraries(Calib_generic_rad MiRS-f)
