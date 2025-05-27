include(AddApp)

addapp_std(ApplyRegress ApplyRegress.f90)
target_link_libraries(ApplyRegress MiRS-f)

