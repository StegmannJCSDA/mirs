#!/bin/bash
#
#---Ungrib GFS data for SFR 
#        e.g. Input: gfs.t00z.pgrb2.0p50.f000.20180415
#                    gfs.t00z.pgrb2.0p50.f003.20180415
#                    gfs.t00z.pgrb2.0p50.f006.20180415
#                    gfs.t00z.pgrb2.0p50.f009.20180415
#                    gfs.t00z.pgrb2.0p50.f012.20180415
#
#            Output: avn_comb00.bin.20180415
#                    avn_comb03.bin.20180415
#                    avn_comb06.bin.20180415
#                    avn_comb09.bin.20180415
#                    avn_comb12.bin.20180415
#
#   Contact: Shu-Yan.Liu@noaa.gov
#   2018-07-19
#======================================================================================
#
#---Retrieve Working and Source Directories
#
   if [ -e $1 ]; then
      WORKDIR=$1
   else  
      WORKDIR=$PWD
   fi
#
#---Source Process Control File (PCF)
#
   gfs_sfr_pcf="${WORKDIR}/gfs_sfr_pcf_nde_template.bash"
   if [ -e ${gfs_sfr_pcf} ]; then
      . ${gfs_sfr_pcf}
   else
      echo "PCF FILE: ${gfs_sfr_pcf} not found, exiting"
      exit 66
   fi
#
#---extract forecast_time and date from ${gfs_input} file
#
forecast_time=${gfs_input:5:2} 
date=`echo ${gfs_input} | cut -d'.' -f 6`
#
#---
#

fhhs=(00 03 06 09 12)
if [ ${forecast_time} -eq "00" ];  then
    zhhs=(00 03 06 09 12)
elif [ ${forecast_time} -eq "06" ];  then
    zhhs=(06 09 12 15 18)
elif [ ${forecast_time} -eq "12" ];  then
    zhhs=(12 15 18 21 24)
elif [ ${forecast_time} -eq "18" ];  then
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
    fn=gfs.t${forecast_time}z.pgrb2.0p50.f0$fhh.$date

	date1=$date
	if [ $zhh -ge 24 ];  then
		zhh=0$((zhh - 24))
		date1=`date -d "$date + 1 day" +%Y%m%d`
	fi
	fn1=${WORKDIR}/avn_comb$zhh.bin.$date1
    echo $fn $fn1 
	#continue

      ${WGRIB} ${WORKDIR}/${fn}    | grep ":HGT:"   | grep "${plevels[0]}"         | ${WGRIB} -i ${WORKDIR}/${fn} -no_header -order we:ns -bin $fn1 >> ${log_file} 2>&1

    for ((iplevel=1; iplevel<${nplevel}; iplevel++ )); do
      ${WGRIB} ${WORKDIR}/${fn}    | grep ":HGT:"   | grep "${plevels[$iplevel]}"  | ${WGRIB} -i ${WORKDIR}/${fn} -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    done

    for ((iplevel=0; iplevel<${nplevel}; iplevel++ )); do
      ${WGRIB} ${WORKDIR}/${fn}    | grep ":TMP:"   | grep "${plevels[$iplevel]}"  | ${WGRIB} -i ${WORKDIR}/${fn} -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    done

    for ((iplevel=0; iplevel<${nplevel}; iplevel++ )); do
      ${WGRIB} ${WORKDIR}/${fn}    | grep ":RH:"    | grep "${plevels[$iplevel]}"  | ${WGRIB} -i ${WORKDIR}/${fn} -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    done

    for ((iplevel=0;iplevel<${nplevel}; iplevel++ )); do
      ${WGRIB} ${WORKDIR}/${fn}    | grep ":VVEL:"  | grep "${plevels[$iplevel]}"  | ${WGRIB} -i ${WORKDIR}/${fn} -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    done

    for ((iplevel=0;iplevel<${nplevel}; iplevel++ )); do
      ${WGRIB} ${WORKDIR}/${fn}    | grep ":CLWMR:" | grep "${plevels[$iplevel]}"  | ${WGRIB} -i ${WORKDIR}/${fn} -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    done


    ${WGRIB} ${WORKDIR}/${fn}    | grep ":RH:"    |                        \
        grep ":2 m above ground:" | ${WGRIB} -i ${WORKDIR}/${fn}           \
        -no_header -order we:ns -append -bin $fn1  >> ${log_file} 2>&1
    ${WGRIB} ${WORKDIR}/${fn}    | grep ":PRES:"  |                        \
        grep ":surface:"          | ${WGRIB} -i ${WORKDIR}/${fn}           \
        -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    ${WGRIB} ${WORKDIR}/${fn}    | grep ":HGT:"    |                       \
        grep ":surface:" | ${WGRIB} -i ${WORKDIR}/${fn}                    \
        -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    ${WGRIB} ${WORKDIR}/${fn}    | grep ":CWAT:"    |                      \
        grep ":entire atmosphere" | ${WGRIB} -i ${WORKDIR}/${fn}           \
        -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    ${WGRIB} ${WORKDIR}/${fn}    | grep ":UGRD:"    |                      \
        grep ":10 m above ground:" | ${WGRIB} -i ${WORKDIR}/${fn}          \
        -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    ${WGRIB} ${WORKDIR}/${fn}    | grep ":VGRD:"    |                      \
        grep ":10 m above ground:" | ${WGRIB} -i ${WORKDIR}/${fn}          \
        -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    ${WGRIB} ${WORKDIR}/${fn}    | grep ":PWAT:"    |                      \
        grep ":entire atmosphere" | ${WGRIB} -i ${WORKDIR}/${fn}           \
        -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    ${WGRIB} ${WORKDIR}/${fn}    | grep ":TMP:"    |                       \
        grep ":surface:" | ${WGRIB} -i ${WORKDIR}/${fn}                    \
        -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1
    ${WGRIB} ${WORKDIR}/${fn}    | grep ":TMP:"    |                       \
        grep ":2 m above ground:" | ${WGRIB} -i ${WORKDIR}/${fn}           \
        -no_header -order we:ns -append -bin $fn1 >> ${log_file} 2>&1

    cp $fn1  $OutputBinDataDir
done

   find ${OutputBinDataDir}/avn_comb* -mtime +5 -exec rm {} \;  >> ${log_file} 2>&1

exit 0
