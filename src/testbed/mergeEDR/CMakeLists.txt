include(AddApp)

addapp_std(mergeEDR mergeEDR.f90)
target_link_libraries(mergeEDR MiRS-f)

