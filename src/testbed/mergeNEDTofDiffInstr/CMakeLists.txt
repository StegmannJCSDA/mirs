include(AddApp)

addapp_std(mergeNEDT mergeNEDT.f90)
target_link_libraries(mergeNEDT MiRS-f)
addapp_std(mergeNedt_npp_atms mergeNedt_npp_atms.f90)
target_link_libraries(mergeNedt_npp_atms MiRS-f)
addapp_std(mergeNedt_n20_atms mergeNedt_n20_atms.f90)
target_link_libraries(mergeNedt_n20_atms MiRS-f)
addapp_std(mergeNedt_n21_atms mergeNedt_n21_atms.f90)
target_link_libraries(mergeNedt_n21_atms MiRS-f)
addapp_std(mergeNedt_metopSGA1_mws mergeNedt_metopSGA1_mws.f90)
target_link_libraries(mergeNedt_metopSGA1_mws MiRS-f)

