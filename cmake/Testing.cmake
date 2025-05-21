function(prep_test)
	if (MIRS_TEST)
		if (MIRS_STANDALONE_PROJECT)

			cmake_host_system_information(RESULT HN QUERY HOSTNAME)
			list(GET HN 0 iHostname)

			set(BUILDNAME "Unspecified_Build_Name_for_CTest" CACHE STRING "Build name variable for CTest" )
			set(SITE "${iHostname}" CACHE STRING "Site name" )
			mark_as_advanced(BUILDNAME SITE)
		endif()
		#add_subdirectory(test)
		message(STATUS "Generating MiRS tests for build \"${BUILDNAME}\" at site \"${SITE}\".")
		add_test(NAME EXAMPLE_getPWD COMMAND pwd)
	else()
		message(STATUS "Not generating MiRS tests.")
	endif()

endfunction()

option(MIRS_TEST "Generate MiRS tests." ${MIRS_STANDALONE_PROJECT})
prep_test()

add_custom_command(
    OUTPUT always_rebuild
    COMMAND cmake -E echo
    )

macro(GenerateBenchmark satname qcsatname satnum infilename)
	add_custom_target(benchmark_${satname}_run
		COMMAND ${PROJECT_BINARY_DIR}/scripts/${satname}_scs_benchmark.bash ${infilename}
#		DEPENDS mirs2nc sfr_amsua sfr_atms vipp viirsForFracColloc prepSweClimo 1dvar ApplyRegress fm_${satname} tdr2sdr mergeNEDT mergeNedt_n20_atms rdr2tdr_n20_atms sfr2dep sfr2dep_atms
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/scripts
		COMMENT "Benchmark tests for ${qcsatname}"
		)

	add_custom_target(benchmark_${satname}_valgrind
		COMMAND ${CMAKE_COMMAND} -E env SCRIPT_TESTING_PREAMBLE="./unit_tests/runValgrind.bash" ${PROJECT_BINARY_DIR}/scripts/${satname}_scs_benchmark.bash ${infilename}
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/scripts
		COMMENT "Valgrind tests for ${qcsatname}"
		)
		
	add_custom_command(
		OUTPUT ${PROJECT_BINARY_DIR}/src/qcDelivery/benchmark_${qcsatname}.tar.bz2
		COMMAND ${PROJECT_BINARY_DIR}/src/qcDelivery/benchmark.bash ${satnum} ${qcsatname}
		VERBATIM
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/src/qcDelivery
		DEPENDS always_rebuild
		DEPENDS benchmark_${satname}_run
		)

	add_custom_target(benchmark_${satname}
		DEPENDS ${PROJECT_BINARY_DIR}/src/qcDelivery/benchmark_${qcsatname}.tar.bz2
		)


endmacro(GenerateBenchmark satname qcsatname satnum infilename)

GenerateBenchmark(n18 N18 1 NSS.MHSX.NN.D20055.S1659.E1833.B7609596.GC)
GenerateBenchmark(metopA Metop-A 2 NSS.MHSX.M2.D20055.S1457.E1639.B6927273.SV)
GenerateBenchmark(f17 F17 3 NPR.TDRN.SB.D14255.S0127.E0312.B4052122.NS)
GenerateBenchmark(n19 N19 4 NSS.MHSX.NP.D20055.S0248.E0437.B5692627.GC)
GenerateBenchmark(f18 F18 5 NPR.TDRN.SC.D14255.S0001.E0159.B2526566.NS)
GenerateBenchmark(npp NPP 6 TATMS_npp_d20200224_t2021093_e2021409_b43151_c20200224204530781132_nobc_ops.h5)
GenerateBenchmark(metopB Metop-B 7 NSS.MHSX.M1.D20055.S1612.E1704.B3858686.MM)
GenerateBenchmark(gpm GPM 8 1C-R.GPM.GMI.XCAL2016-C.20200701-S192640-E193138.V05A.RT-H5)
GenerateBenchmark(n20 N20 9 TATMS_j01_d20200224_t1933093_e1933409_b11754_c20200224194808315530_nobc_ops.h5)
GenerateBenchmark(metopC Metop-C 10 NSS.MHSX.M3.D20055.S1705.E1847.B0674445.SV)
GenerateBenchmark(n21 N21 11 TATMS_j02_d20240301_t1435551_e1436267_b06772_c20240301145655432000_oebc_ops.h5)
GenerateBenchmark(metopSGA1 metopSGA1 12 MWS_metopSGA1_d20190215_t193232_e211421_b20200605064400.nc)

