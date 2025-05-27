include(AddApp)

addapp_std(prepSweClimo prepSweClimo.f90)
target_link_libraries(prepSweClimo MiRS-f)

