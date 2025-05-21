#!/bin/bash
#
#---path set up for gfs_sfr.bash
#   Modify the paths according to user's machine environment
#
#   IMPORTANT: Make sure that the path set in the variable ${OutputBinDataDir} is 
#   the same as the path set for variable ${nwpSfrGridPath} in the MiRS pcf file e.g. npp_pcf_nde_template.bash
#   More information on running the gfs extraction script can be found in the readme under 
#   data/ExternalData/gridNWP_sfr
#---Contact: Shu-Yan.Liu@noaa.gov 2018-06-18
#======================================================================================
#
WGRIB=/data/data412/pub/sliu/DAP_20180405/mirs_dap/data/ExternalData/gridNWP_sfr/wgrib2
InputGribDataDir=/data/data411/pub/sliu/ExternalData/gfs_0p50
OutputBinDataDir=/data/data412/pub/sliu/mirs/data/ExternalData/gridNWP_sfr

