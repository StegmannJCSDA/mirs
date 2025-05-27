include(AddApp)

addapp_std(colocNWPwRad colocNWPwRad.f90)
target_link_libraries(colocNWPwRad MiRS-f)
addapp_std(prepNWP prepNWP.f90)
target_link_libraries(prepNWP MiRS-f)

