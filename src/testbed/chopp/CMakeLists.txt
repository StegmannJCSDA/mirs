include(AddApp)

addapp_std(chopp chopp.f90)
target_link_libraries(chopp MiRS-f)

