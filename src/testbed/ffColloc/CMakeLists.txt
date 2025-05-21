include(AddApp)

addapp_std(viirsForFracColloc viirsForFracColloc.f90)
target_link_libraries(viirsForFracColloc MiRS-f)

