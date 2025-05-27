include(AddApp)

addapp_std(tdr2sdr tdr2sdr.f90)
target_link_libraries(tdr2sdr MiRS-f)

