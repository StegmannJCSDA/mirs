
####################################################################################################
#
#
#   S E C T I O N   O F   D A T A   A N D   P A T H S
#
#
####################################################################################################

#-------------------------------------------------------------------------------
#        Satellite, sensor and default date used in GUI
#-------------------------------------------------------------------------------
satId=n18
sensor1=amsua
sensor2=mhs
date=2006-02-01

#-------------------------------------------------------------------------------
#        Major root paths
#-------------------------------------------------------------------------------
pcf_script_dir=`dirname "$BASH_SOURCE"`
pcf_script_dir_real=`readlink -f "$pcf_script_dir"`
rootPath=`dirname "$pcf_script_dir_real"`
dataPath=${rootPath}/data
binPath=${rootPath}/bin
logPath=${rootPath}/logs
IDL=/usr/local/bin/idl
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib	#OS dependent(HP is SHLIB_PATH; AIX is LIBPATH; Linux is ID_LIBRARAY_PATH)

#-------------------------------------------------------------------------------
#        Research data & Paths
#-------------------------------------------------------------------------------
researchDataPath=/net/orbit006L/home/sidb/ResearchData
fwdPath=${researchDataPath}/FwdSimulOutputs
out1dvarPath=${researchDataPath}/1dvarOutputs
monitorFile=${researchDataPath}/IterProcessMonitor/Monitoring.dat
modelNonErrPath=${researchDataPath}/ModelErrStats/amsua_mhs

#-------------------------------------------------------------------------------
#        External data & Paths
#-------------------------------------------------------------------------------
externalDataPath=${dataPath}/ExternalData
rdrSensor1Path=${externalDataPath}/rdr/${satId}_${sensor1}_${sensor2}
rdrSensor2Path=${externalDataPath}/rdr/${satId}_${sensor1}_${sensor2}
rdrOrbitPath=${externalDataPath}/rdr/OrbitalMode
nwpGdasGridPath=${externalDataPath}/gridNWP_analys
nwpEcmwfGridPath=${externalDataPath}/gridNWP_analys
imsAnalysisPath=${externalDataPath}/ims_analys
nwpSfrGridPath=${externalDataPath}/gridNWP_sfr
nwpGfsGridPath=${externalDataPath}/gridNWP_analys

#-------------------------------------------------------------------------------
#        Static data & Paths
#-------------------------------------------------------------------------------
staticDataPath=${dataPath}/StaticData
instrumentPath=${staticDataPath}/InstrConfigInfo
instrumentSensor1File=${instrumentPath}/InstrConfig_${satId}_${sensor1}.dat
instrumentSensor2File=${instrumentPath}/InstrConfig_${satId}_${sensor2}.dat
instrumentSensor1Sensor2File=${instrumentPath}/InstrConfig_${satId}_${sensor1}_${sensor2}.dat
topographyFile=${staticDataPath}/Topography/topography.bin_sgi
antennaPath=${staticDataPath}/AntennaPatterns
antennaSensor1File=${antennaPath}/${satId}_${sensor1}_antennaPattern.dat
antennaSensor2File=${antennaPath}/${satId}_${sensor2}_antennaPattern.dat
tune1File=${staticDataPath}/TuningData/TunParams_${satId}_${sensor1}_${sensor2}.in
tune2File=${staticDataPath}/TuningData/TunParams_${satId}_${sensor1}_${sensor2}_2.in
nedtNominalFile=${staticDataPath}/NominalNedts/${satId}_NoiseFile.dat
covBkgAtm1File=${staticDataPath}/CovBkgStats/CovBkgMatrxTotAtm_all.dat
covBkgAtm2File=${staticDataPath}/CovBkgStats/CovBkgMatrxTotAtm_all.dat
covBkgSfc1File=${staticDataPath}/CovBkgStats/CovBkgMatrxTotSfc_all_${satId}_${sensor1}_${sensor2}.dat
covBkgSfc2File=${staticDataPath}/CovBkgStats/CovBkgMatrxTotSfc_all_${satId}_${sensor1}_${sensor2}.dat
extBkgAtmFile=${staticDataPath}/CovBkgStats/atmBkg_ECMWF_bin.dat
siceEmissCatalogFile=${staticDataPath}/EmissCatalog/SeaIceEmissCatalog_${satId}_${sensor1}_${sensor2}.dat
snowEmissCatalogFile=${staticDataPath}/EmissCatalog/SnowEmissCatalog_${satId}_${sensor1}_${sensor2}.dat
SweClimoMeanFile=${staticDataPath}/SWE_Climatology/swe_ssmis_climo_ease_NHSH.dat_ascii
SweClimoStdevFile=${staticDataPath}/SWE_Climatology/swe_ssmis_stdev_ease_NHSH.dat_ascii
ffFile=${staticDataPath}/ForestFraction/VIIRS_2014_GST_latlon_avg25km.dat
ffLatLonFile=${staticDataPath}/ForestFraction/latlon_avg25km.dat
sfrDataPath=${staticDataPath}/SFR/
CRTMcoeffPath=${staticDataPath}/CRTMFiles/

#-------------------------------------------------------------------------------
#        Semi-Static data & Paths
#-------------------------------------------------------------------------------
semiStaticDataPath=${dataPath}/SemiStaticData
biasPath=${semiStaticDataPath}/biasCorrec
regressPath=${semiStaticDataPath}/regressAlgors
regressCoeffOceanClwFile=${regressPath}/Oc_regressCoeffs_${satId}_clw.dat
regressCoeffSeaIceClwFile=${regressPath}/SeaIce_regressCoeffs_${satId}_clw.dat
regressCoeffLandClwFile=${regressPath}/Land_regressCoeffs_${satId}_clw.dat
regressCoeffSnowClwFile=${regressPath}/Snow_regressCoeffs_${satId}_clw.dat
regressCoeffOceanTskinFile=${regressPath}/Oc_regressCoeffs_${satId}_tskin.dat
regressCoeffSeaIceTskinFile=${regressPath}/SeaIce_regressCoeffs_${satId}_tskin.dat
regressCoeffLandTskinFile=${regressPath}/Land_regressCoeffs_${satId}_tskin.dat
regressCoeffSnowTskinFile=${regressPath}/Snow_regressCoeffs_${satId}_tskin.dat
regressCoeffOceanTpwFile=${regressPath}/Oc_regressCoeffs_${satId}_tpw.dat
regressCoeffSeaIceTpwFile=${regressPath}/SeaIce_regressCoeffs_${satId}_tpw.dat
regressCoeffLandTpwFile=${regressPath}/Land_regressCoeffs_${satId}_tpw.dat
regressCoeffSnowTpwFile=${regressPath}/Snow_regressCoeffs_${satId}_tpw.dat
regressCoeffOceanEmFile=${regressPath}/Oc_regressCoeffs_${satId}_em.dat
regressCoeffSeaIceEmFile=${regressPath}/SeaIce_regressCoeffs_${satId}_em.dat
regressCoeffLandEmFile=${regressPath}/Land_regressCoeffs_${satId}_em.dat
regressCoeffSnowEmFile=${regressPath}/Snow_regressCoeffs_${satId}_em.dat
regressCoeffOceanWvFile=${regressPath}/Oc_regressCoeffs_${satId}_wv.dat
regressCoeffSeaIceWvFile=${regressPath}/SeaIce_regressCoeffs_${satId}_wv.dat
regressCoeffLandWvFile=${regressPath}/Land_regressCoeffs_${satId}_wv.dat
regressCoeffSnowWvFile=${regressPath}/Snow_regressCoeffs_${satId}_wv.dat
regressCoeffOceanTempFile=${regressPath}/Oc_regressCoeffs_${satId}_temp.dat
regressCoeffSeaIceTempFile=${regressPath}/SeaIce_regressCoeffs_${satId}_temp.dat
regressCoeffLandTempFile=${regressPath}/Land_regressCoeffs_${satId}_temp.dat
regressCoeffSnowTempFile=${regressPath}/Snow_regressCoeffs_${satId}_temp.dat
regressCoeffOceanGwpFile=${regressPath}/Oc_regressCoeffs_${satId}_gwp.dat
regressCoeffSeaIceGwpFile=${regressPath}/SeaIce_regressCoeffs_${satId}_gwp.dat
regressCoeffLandGwpFile=${regressPath}/Land_regressCoeffs_${satId}_gwp.dat
regressCoeffSnowGwpFile=${regressPath}/Snow_regressCoeffs_${satId}_gwp.dat
regressCoeffDesertFile=${regressPath}/Desert_regressCoeffs_${satId}.dat
biasFileToUse=${biasPath}/biasCorrec_${satId}.dat
calibBiasFitFile=${biasPath}/calibBiasFit_${satId}.dat
calibDTRlutFile=${biasPath}/calibDTRlut_${satId}.dat
TBbias_NNModelFile=${semiStaticDataPath}/NeuralNetwork_ModelFiles/TBbias_NNmodel_${satId}_${sensor1}.h5
TPW_NNModelFile=${semiStaticDataPath}/NeuralNetwork_ModelFiles/TPW_NNmodel_${satId}_${sensor1}.h5
Tskin_NNModelFile=${semiStaticDataPath}/NeuralNetwork_ModelFiles/Tskin_NNmodel_${satId}_${sensor1}.h5

#-------------------------------------------------------------------------------
#        Testbed data & Paths
#-------------------------------------------------------------------------------
testbedDataPath=${dataPath}/TestbedData
nedtPath=${testbedDataPath}/nedt
nedtSensor1Path=${nedtPath}/${satId}_${sensor1}
nedtSensor2Path=${nedtPath}/${satId}_${sensor2}
nedtSensor1Sensor2Path=${nedtPath}/${satId}_${sensor1}_${sensor2}
edrPath=${testbedDataPath}/Outputs/edr/${satId}_${sensor1}_${sensor2}
depPath=${testbedDataPath}/Outputs/dep/${satId}_${sensor1}_${sensor2}
sfrPath=${testbedDataPath}/Outputs/sfr/${satId}_${sensor1}_${sensor2}
gridPath=${testbedDataPath}/Outputs/grid/${satId}_${sensor1}_${sensor2}
ncPath=${testbedDataPath}/Outputs/nc/${satId}_${sensor1}_${sensor2}
figsPath=${testbedDataPath}/Outputs/figs/${satId}_${sensor1}_${sensor2}
perfsMonitorPath=${testbedDataPath}/PerfsMonitoring/${satId}_${sensor1}_${sensor2}
logFile=${logPath}/${satId}_logFile

#-------------------------------------------------------------------------------
#        Dynamic data & Paths
#-------------------------------------------------------------------------------
dynamicDataPath=${testbedDataPath}/DynamicData
tdrPath=${dynamicDataPath}/tdr
tdrSensor1Path=${tdrPath}/${satId}_${sensor1}
tdrSensor2Path=${tdrPath}/${satId}_${sensor2}
sdrPath=${dynamicDataPath}/sdr
sdrSensor1Path=${sdrPath}/${satId}_${sensor1}
sdrSensor2Path=${sdrPath}/${satId}_${sensor2}
fmsdrPath=${dynamicDataPath}/fmsdr/${satId}_${sensor1}_${sensor2}
choppPath=${dynamicDataPath}/fmsdrchopp/${satId}_${sensor1}_${sensor2}
nwpAnalysPath=${dynamicDataPath}/nwp_analys/${satId}_${sensor1}_${sensor2}
fwdAnalysPath=${dynamicDataPath}/fwd_analys/${satId}_${sensor1}_${sensor2}
prepSweClimoPath=${dynamicDataPath}/swe_climo/${satId}_${sensor1}_${sensor2}
ffCollocPath=${dynamicDataPath}/ff_colloc/${satId}_${sensor1}_${sensor2}
imsCollocPath=${dynamicDataPath}/ims_colloc/${satId}_${sensor1}_${sensor2}
regressRetrPath=${dynamicDataPath}/regress_retr/${satId}_${sensor1}_${sensor2}

#-------------------------------------------------------------------------------
#        Control Files
#-------------------------------------------------------------------------------
controlDataPath=${dataPath}/ControlData
rdr2tdrSensor1ControlFile=${controlDataPath}/${satId}_${sensor1}_rdr2tdr
rdr2tdrSensor2ControlFile=${controlDataPath}/${satId}_${sensor2}_rdr2tdr
mergeNedtControlFile=${controlDataPath}/${satId}_mergeNEDT
tdr2sdrSensor1ControlFile=${controlDataPath}/${satId}_${sensor1}_tdr2sdr
tdr2sdrSensor2ControlFile=${controlDataPath}/${satId}_${sensor2}_tdr2sdr
fmControlFile=${controlDataPath}/${satId}_${sensor1}_${sensor2}_fm
fmsdr2edrControlFile=${controlDataPath}/${satId}_CntrlConfig_1dvar
grid2nwpControlFile=${controlDataPath}/${satId}_${sensor1}_${sensor2}_colocNWPwRAD
fwdControlFile=${controlDataPath}/${satId}_cntrl_fwd
regressControlFile=${controlDataPath}/${satId}_ApplyRegress
prepSweClimoControlFile=${controlDataPath}/${satId}_prepSweClimo
ffCollocControlFile=${controlDataPath}/${satId}_ffColloc
imsCollocControlFile=${controlDataPath}/${satId}_imsColloc
choppControlFile=${controlDataPath}/${satId}_Chopp
mergeEdrControlFile=${controlDataPath}/${satId}_MergeEDR
vippControlFile=${controlDataPath}/${satId}_Vipp
sfrControlFile=${controlDataPath}/${satId}_Sfr
sfr2depControlFile=${controlDataPath}/${satId}_Sfr2dep
gridControlFile=${controlDataPath}/${satId}_Grid
nwpGridControlFile=${controlDataPath}/${satId}_NWPGrid
fwdGridControlFile=${controlDataPath}/${satId}_FWDGrid
biasGridControlFile=${controlDataPath}/${satId}_BiasGrid
biasCompuControlFile=${controlDataPath}/${satId}_Inputs4BiasComputation
biasVerifControlFile=${controlDataPath}/${satId}_Inputs4BiasVerification
regressGenControlFile=${controlDataPath}/${satId}_Inputs4RegressGen
modifyNedtControlFile=Dummy
figsGenControlFile=${controlDataPath}/${satId}_Inputs4FigsGener

#-------------------------------------------------------------------------------
#        File List
#-------------------------------------------------------------------------------
inputDataPath=${dataPath}/InputsData
rdrSensor1List=${inputDataPath}/${satId}_${sensor1}_rdrFiles
rdrSensor2List=${inputDataPath}/${satId}_${sensor2}_rdrFiles
tdrSensor1List=${inputDataPath}/${satId}_${sensor1}_tdrFiles
tdrSensor2List=${inputDataPath}/${satId}_${sensor2}_tdrFiles
sdrSensor1List=${inputDataPath}/${satId}_${sensor1}_sdrFiles
sdrSensor2List=${inputDataPath}/${satId}_${sensor2}_sdrFiles
fmsdrList=${inputDataPath}/${satId}_fmsdrFiles
fmsdr4ChoppList=${inputDataPath}/${satId}_fmsdrFiles_4Chopping
fmsdr4NwpList=${inputDataPath}/${satId}_fmsdrFiles_4nwp
fmsdr4BiasList=${inputDataPath}/${satId}_fmsdrFiles_4Bias
fmsdr4RegressList=${inputDataPath}/${satId}_fmsdrFiles_4regress
fmsdr4ApplyRegressList=${inputDataPath}/${satId}_fmsdrFiles_4ApplyRegress
fmsdr4PrepSweList=${inputDataPath}/${satId}_fmsdrFiles_4PrepSweClimo
fmsdr4ffCollocList=${inputDataPath}/${satId}_fmsdrFiles_4ffColloc
fmsdr4imsCollocList=${inputDataPath}/${satId}_fmsdrFiles_4imsColloc
imsColloc4nwpList=${inputDataPath}/${satId}_imsCollocFiles_4nwp
imsColloc4retrList=${inputDataPath}/${satId}_imsCollocFiles_4retr
fmsdr4SfrList=${inputDataPath}/${satId}_fmsdrFiles_4Sfr
prepSweClimo4VippList=${inputDataPath}/${satId}_sweClimoFiles_4Vipp
ffColloc4VippList=${inputDataPath}/${satId}_ffCollocFiles_4Vipp
edrList=${inputDataPath}/${satId}_edrFiles
edr4BiasList=${inputDataPath}/${satId}_edrFiles_4Bias
dep4BiasList=${inputDataPath}/${satId}_depFiles_4Bias
edr4MergeList=${inputDataPath}/${satId}_FullOrbitEDR_4Merging
depList=${inputDataPath}/${satId}_depFiles
sfrList=${inputDataPath}/${satId}_sfrFiles
nedtList=${inputDataPath}/${satId}_nedtDirs_${sensor1}_${sensor2}
nedtSensor1List=${inputDataPath}/${satId}_nedtDirs_${sensor1}
nedtSensor2List=${inputDataPath}/${satId}_nedtDirs_${sensor2}
gridSfcNwpAnalysList=${inputDataPath}/${satId}_sfcNWPanalys
gridAtmNwpAnalysList=${inputDataPath}/${satId}_atmNWPanalys
gridNwpSfrList=${inputDataPath}/${satId}_NWPsfr
nwpAnalysList=${inputDataPath}/${satId}_NWPanalysFiles
nwpAnalysRetrList=${inputDataPath}/${satId}_NWPanalysFiles_4retr
nwpAnalys4BiasList=${inputDataPath}/${satId}_NWPanalysFiles_4Bias
nwpAnalys4RegressList=${inputDataPath}/${satId}_NWPanalysFiles_4Regress
fwdAnalys4BiasList=${inputDataPath}/${satId}_FWDanalysSimulFiles_4Bias


####################################################################################################
#
#
#   S E C T I O N   O F   A P P L I C A T I O N S   A N D   P R O C E S S E S
#
#
####################################################################################################

#-------------------------------------------------------------------------------
#        Source Directories
#-------------------------------------------------------------------------------
rdr2tdrSensor1Src=${rootPath}/src/testbed/rdr2tdr
rdr2tdrSensor2Src=${rootPath}/src/testbed/rdr2tdr
mergeNedtSrc=${rootPath}/src/testbed/mergeNEDTofDiffInstr
tdr2sdrSrc=${rootPath}/src/testbed/tdr2sdr
fmSrc=${rootPath}/src/testbed/fm
choppSrc=${rootPath}/src/testbed/chopp
fmsdr2edrSrc=${rootPath}/src/1dvar
mergeEdrSrc=${rootPath}/src/testbed/mergeEDR
vippSrc=${rootPath}/src/testbed/vipp
sfrSrc=${rootPath}/src/testbed/sfr
sfr2depSrc=${rootPath}/src/testbed/sfr2dep
gridSrc=${rootPath}/src/testbed/grid
ncSrc=${rootPath}/src/testbed/mirs2nc
nedtMonitorSrc=${rootPath}/src/testbed/nedtMonitoring
nwpGenAnalysSrc=${rootPath}/src/testbed/nwp
fwdSrc=${rootPath}/src/fwd
determineBiasSrc=${rootPath}/src/testbed/biasGenerAndMonit
prepSweSrc=${rootPath}/src/testbed/sweClimo
ffCollocSrc=${rootPath}/src/testbed/ffColloc
imsCollocSrc=${rootPath}/src/testbed/ims
applyRegressAlgSrc=${rootPath}/src/testbed/retrRegress


####################################################################################################
#
#
#   S E C T I O N   O F   S W I T C H E S (W H I C H   A P P L I C A T I O N   T O   R U N)
#
#
####################################################################################################

step_rdr2tdrSensor1=1	         #RDR->TDR (Sensor1)
step_rdr2tdrSensor2=1	         #RDR->TDR (Sensor2)
step_mergeNedt=1	               #MERGE NEDTs (Sensor1 and Sensor2)
step_tdr2sdrSensor1=1	         #TDR->SDR (Sensor1)
step_tdr2sdrSensor2=1	         #TDR->SDR (Sensor2)
step_fm=1	                     #FOOTPRINT MATCHING
step_ims4nwp=0	                  #IMS SFC TYPE COLLOCATION FOR NWP STEP
step_nwp=0	                     #CREATE NWP SCENE (GDAS)
step_fwd=0	                     #USE FWD OPERATOR ON NWP SCENE
step_biasGen=0	                  #GENERATE A NEW TB EC
step_choppRadFiles=1	            #CHOPPING RADIANCE FILES FOR MULTIPLE PROCESS SUBMISSION
step_ims4retr=0	               #IMS SFC TYPE COLLOCATION FOR 1DVAR STEP
step_externalDataFromRegress=1	#USE OF REGRESSION ALGORIHMS TO GENERATE EXTERN DATA
step_fmsdr2edr=1	               #FMSDR->EDR
step_mergeEdr=1	               #MERGE THE MINI EDR FILES INTO A FULL ORBITAL EDR FILE 
step_prepSweClimo=1	            #PREPARE AND GENERATE THE COLLOCATED SWE CLIMATOLOGY FILES FOR VIPP
step_ffColloc=0	               #FOREST FRACTION COLLOCATION FOR VIPP SWE
step_vipp=1	                     #VERTICAL INTEGRATION AND POST PROCESSING
step_sfr=0	                     #Produce Snow Fall Rate
step_grid=0	                     #Gridded LEVEL III DATA GENERATION
step_nc=1	                     #Convert EDR & DEP into NETCDF4 format
step_figsGen=0	                  #FIGS GENERATION
step_biasFigsGen=0	            #BIAS FIGS GENERATION
step_dataMonitor=0	            #MONITORING OF DATA QUALITY
step_clean=0	                  #DISK CLEANING/PURGING


####################################################################################################
#
#
#   S E C T I O N  OF  C O N T R O L L I N G  F L A G S
#
#
####################################################################################################

processMode=0	#0:Orbit processing  1:Daily processing
sensorId=1	#1:N18,2:MetopA,3:F16,4:N19,5:F18,6:NPP/ATMS,7:AMSRE,8:FY3/MWRI,9:TMI,10:GMI,12:MADRAS,13:SAPHIR,14:MetopB,15:AMSR2,18:F17,19:F19,20:JPSS(N20)/ATMS,21:MetopC
outFMAccuracy=0	#Flag to output of the FM accuracy metric (DeltaTB @89)
prefixFMAccuracy=QCcheck	#Prefix of file(s) w FM-acuracy metric (only if outFMaccur=1)
nProfs2Retr=1000000	#Maximum number of profiles to process in retrieval
nProfs2Fwd=1000000	#Maximum number of profiles to simulate using the fwd operator (over analyses)
nAttempts=2	#Number of retrieval attempts in case of non-convergence 
fmType=1	#POES/NPP/N20:0(low),1(high); DMSP:0(UAS-low),1(LAS),2(ENV),3(IMG-high); TMI/AMSR2/GMI:-1(coarse),0(low),1(high)
addDeviceNoise=0	#=1 Flag to add noise to the fwd simulations (over analyses), =0->no Noise added 
monitorIterative=0	#=1->Yes, monitor iterative process, =0-> Do not
monitorRetrieval=0	#=1->Yes, on-screen-monitoring of retrieval,  =0-> Do not.
monitorFwd=0	#=1->Yes, on-screen-monitoring of fwd-simul,  =0-> Do not.
externalDataAvailable=1	#=1->Ext data available, use ExtDataUse =0-> No Ext data available,
externalDataSrc=2	#Source of external Data (only if externDataAvailable=1). =1-> ANALYS, =2->REGRESS
nwpGdasUse=0	#=1->To Use GDAS in NWP Collocation,  =0-> Not Use GDAS
nwpEcmwfUse=0	#=1->To Use ECMWF in NWP Collocation, =0-> Not Use ECMWF
nwpGfsUse=0	#=1->To Use GFS in NWP Collocation,  =0-> Not Use GFS
extBkgAtmUse=1	#=1->To use spat/temp variable Atm Background, =0-> Do not use spat/temp variable Atm Background (use glbl climatology)
sweClimoUse=1	#=1->To use SWE climatology in SWE retrieval (vipp step), 0->Do not use SWE climatology
ffUse=0	#=1->To use forest fraction correction in SWE retrieval (vipp step), 0->Do not use forest fraction
imsSfcUse=0	#=1->To use IMS surface type in nwp and/or retrieval steps, 0->Do not use IMS surface type
ndayLagIms=-1	#Number of days lag specified between processing date and valid date of IMS analysis (=0->same day, =-1->previous day)
geoLimit=0	#=0-> Process all data. =1-> only in the lat/lon box. 2->Only ocean, 3->Only land
minLat=-90.	#Min latitude of data to be processed (only if GeogrLimits=1)
maxLat=90.	#Max latitude of data to be processed (only if GeogrLimits=1)
minLon=-180.	#Min longitude of data to be processed (only if GeogrLimits=1)
maxLon=180.	#Max longitude of data to be processed (only if GeogrLimits=1)
cend=2	#Orbit(s) 2 process: 0:ascending, 1:descending, 2:both
nDaysBack=2	#TB-Bias assessed w. data from NdayBack, due to reference data delay. NdayBack between 0 & mxDaysAllowed-1
maxDaysArchived=0	#Max number of days allowed for archiving (purged regularily)
dayUsed4Bias=2006_02_01	#Extension used to determine which bias to use in the ec process.
dayUsed4Alg=2006_02_01	#Extension used to determine which algorithms to use in the externDataFromRegress step
nOrbits2Process=1000000	#Number of orbits/suborbits to process: overwrites the existing number of orbits
tdrFormat=1	#Format of TDR (depends on the RDR decoder at hand):0->ascii, 1->binary
rdrType=0
gifDensity=100	#density used when converting PS files into Gif images
gridFactor=4	#grid factor used when gridding level III data
nScanLineSensor1Skip=0	#Number of sensor 1 scan lines to skip upfront (to accomodate geolocation issues) 
nScanLineSensor2Skip=1	#Number of sensor 2 scan lines to skip upfront (to accomodate geolocation issues) 
scanLineIndexSensor2TimeColloc=2	#Sensor2 ScanLine index (1,2 or 3) that corresponds in time to sensor1
fwdCloudOffOrOn=0	#Set Hydrometeors to Zero or retain values (FWD step only).  0->Set to Zero, 1->Retain
biasComputeMethod=1	#Method for computing the bias. 0->Simple bias, 1->Histogram adjustment
regressionBiasApplyDomain=-2	#Domain of application of bias(regression).-2 nowhere, -1->everywhere
nChoppedFilesPerOrbit=10	#Number of chopped sub-orbits per orbit. If 1 no chopping is done
retrOnOrbitOrSubOrbit=1	#Switches between performing retr on full-orbits (0) or chopped ones (1)
retrOnWhichSDR=1	#Switches between retr on uncorrected TBs (1) or NWP-based simuls(2)
fwdMatrix2Use=0	#Switches fwd model error matrix (0:dynamically generated by compar with simul, 1:Non-error)
makeOrNot=0	#Switches between making (1) the executables on the fly or not (0)
useCPU=1	#0->Use one CPU; 1->Use all CPUs; 2->Use QSUB
makeClean=0	#During cleaning step, do we want make-clean as well (=1) or not (=0)
email=Shu-Yan.Liu@noaa.gov	#comma separated email addresses to notify
website=http://www.star.nesdis.noaa.gov/mirs/dataqualityv.php	#website address to view result
production_site=NSOF	#For use at NDE
production_environment=OE	#For use at NDE
