#!/bin/bash
#
#---Ungrib GFS data for SFR 
#   Usage: ./gfs_sfr.bash ${ff} ${yyyymmdd}
#                              ${ff}: GFS forecast Hour, ${yyyymmdd}: year month day.
#          e.g. gfs_sfr.bash 06 20180625
#               Input: gfs.t06z.pgrb2.0p50.f000.20180225 
#               Output: avn_comb06.bin.20180225,avn_comb09.bin.20180225,avn_comb12.bin.20180225,avn_comb15.bin.20180225,avn_comb18.bin.20180225
#
#   IMPORTANT: Make sure that the path set in the variable ${OutputBinDataDir} is
#   the same as the path set for variable ${nwpSfrGridPath} in the MiRS pcf file e.g. npp_pcf_nde_template.bash
#   More information on running the gfs extraction script can be found in the readme under
#   data/ExternalData/gridNWP_sfr
#   
#   Contact: Shu-Yan.Liu@noaa.gov
#   2018-06-18
#======================================================================================

. ../setup/gfs_sfr_pcf.bash

# arguments
if [ $# -eq 2 ];  then
    ff=$1
    ymd=$2
else
    echo format: command ff yyyymmdd
    exit 1
fi

   data_dir_output=${OutputBinDataDir}

if [[ ! -d ${data_dir_output}/ ]]
then
   mkdir -p  ${data_dir_output}
fi

# prepare
yyyy=${ymd:0:4}
mm=${ymd:4:2}
dd=${ymd:6:2}

fhhs=(00 03 06 09 12)
if [ $ff -eq "00" ];  then
    zhhs=(00 03 06 09 12)
elif [ $ff -eq "06" ];  then
    zhhs=(06 09 12 15 18)
elif [ $ff -eq "12" ];  then
    zhhs=(12 15 18 21 24)
elif [ $ff -eq "18" ];  then
    zhhs=(18 21 24 27 30)
else
    echo Wrong forcast time
    exit 1
fi
nh=${#fhhs[@]}

declare -a plevels=("200 mb:" "250 mb:" "300 mb:" "350 mb:" "400 mb:" "450 mb:" "500 mb:" "550 mb:" "600 mb:" "650 mb:" "700 mb:" "750 mb:" 
                    "800 mb:" "850 mb:" "900 mb:" "925 mb:" "950 mb:" "975 mb:" "1000 mb:")
nplevel=${#plevels[@]}

# extract to avn
for ((ih=0; ih<${nh}; ih++));  do
    fhh=${fhhs[$ih]}
    zhh=${zhhs[$ih]}
    fn=gfs.t${ff}z.pgrb2.0p50.f0$fhh.$ymd

	ymd1=$ymd
	if [ $zhh -ge 24 ];  then
		zhh=0$((zhh - 24))
		ymd1=`date -d "$ymd + 1 day" +%Y%m%d`
	fi
	fn1=${data_dir_output}/avn_comb$zhh.bin.$ymd1
    echo $fn $fn1
	#continue

      ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":HGT:"   | grep "${plevels[0]}"         | ${WGRIB} -i ${InputGribDataDir}/${fn} -no_header -order we:ns -bin $fn1

    for ((iplevel=1; iplevel<${nplevel}; iplevel++ )); do
      ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":HGT:"   | grep "${plevels[$iplevel]}"  | ${WGRIB} -i ${InputGribDataDir}/${fn} -no_header -order we:ns -append -bin $fn1
    done

    for ((iplevel=0; iplevel<${nplevel}; iplevel++ )); do
      ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":TMP:"   | grep "${plevels[$iplevel]}"  | ${WGRIB} -i ${InputGribDataDir}/${fn} -no_header -order we:ns -append -bin $fn1
    done

    for ((iplevel=0; iplevel<${nplevel}; iplevel++ )); do
      ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":RH:"    | grep "${plevels[$iplevel]}"  | ${WGRIB} -i ${InputGribDataDir}/${fn} -no_header -order we:ns -append -bin $fn1
    done

    for ((iplevel=0;iplevel<${nplevel}; iplevel++ )); do
      ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":VVEL:"  | grep "${plevels[$iplevel]}"  | ${WGRIB} -i ${InputGribDataDir}/${fn} -no_header -order we:ns -append -bin $fn1
    done

    for ((iplevel=0;iplevel<${nplevel}; iplevel++ )); do
      ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":CLWMR:" | grep "${plevels[$iplevel]}"  | ${WGRIB} -i ${InputGribDataDir}/${fn} -no_header -order we:ns -append -bin $fn1
    done


    ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":RH:"    |                        \
        grep ":2 m above ground:" | ${WGRIB} -i ${InputGribDataDir}/${fn}           \
        -no_header -order we:ns -append -bin $fn1 
    ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":PRES:"  |                        \
        grep ":surface:"          | ${WGRIB} -i ${InputGribDataDir}/${fn}           \
        -no_header -order we:ns -append -bin $fn1 
    ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":HGT:"    |                       \
        grep ":surface:" | ${WGRIB} -i ${InputGribDataDir}/${fn}                    \
        -no_header -order we:ns -append -bin $fn1 
    ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":CWAT:"    |                      \
        grep ":entire atmosphere" | ${WGRIB} -i ${InputGribDataDir}/${fn}           \
        -no_header -order we:ns -append -bin $fn1 
    ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":UGRD:"    |                      \
        grep ":10 m above ground:" | ${WGRIB} -i ${InputGribDataDir}/${fn}          \
        -no_header -order we:ns -append -bin $fn1 
    ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":VGRD:"    |                      \
        grep ":10 m above ground:" | ${WGRIB} -i ${InputGribDataDir}/${fn}          \
        -no_header -order we:ns -append -bin $fn1 
    ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":PWAT:"    |                      \
        grep ":entire atmosphere" | ${WGRIB} -i ${InputGribDataDir}/${fn}           \
        -no_header -order we:ns -append -bin $fn1 
    ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":TMP:"    |                       \
        grep ":surface:" | ${WGRIB} -i ${InputGribDataDir}/${fn}                    \
        -no_header -order we:ns -append -bin $fn1 
    ${WGRIB} ${InputGribDataDir}/${fn}    | grep ":TMP:"    |                       \
        grep ":2 m above ground:" | ${WGRIB} -i ${InputGribDataDir}/${fn}           \
        -no_header -order we:ns -append -bin $fn1
done

exit 0
