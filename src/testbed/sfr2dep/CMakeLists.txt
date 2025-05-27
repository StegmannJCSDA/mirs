include(AddApp)

addapp_std(sfr2dep sfr2dep.f90)
target_link_libraries(sfr2dep MiRS-f)

