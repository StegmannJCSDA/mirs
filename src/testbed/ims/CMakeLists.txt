include(AddApp)

addapp_std(imsColloc imsColloc.f90)
target_link_libraries(imsColloc MiRS-f)

