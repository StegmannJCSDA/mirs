#!/bin/bash

# === Safety check: ensure date is provided ===
if [ $# -lt 1 ]; then
  echo "Usage: $0 yyyymmdd"
  exit 1
fi

# === Parse input date ===
ymd="$1"
yyyy=${ymd:0:4}
mm2=${ymd:4:2}
dd2=${ymd:6:2}

# === Directory paths ===
dir_in="/discover/nobackup/projects/hympi/nshahrou/data/GFS/"      
dir_out="/discover/nobackup/projects/hympi/nshahrou/mirs/data/ExternalData/gridNWP_analys/"   
IDL=/discover/vis/exelis/idl/idl85/bin/idl
# === Hour list ===
hhs=(00 06 12 18)

# === Surface variable list ===
sfc_vars=(
  ":LAND:surface:"
  ":PRES:surface:"
  ":TMP:surface:"
  ":TMP:surface:"
  ":PWAT:entire atmosphere"
  ":CWAT:entire atmosphere"
  ":WEASD:surface:"
  ":ICEC:surface:"
  ":UGRD:10 m above ground:"
  ":VGRD:10 m above ground:"
  ":RH:2 m above ground:"
  ":PRATE:surface:"
  ":PRATE:surface:"
)

# === Pressure levels (in mb) ===


levels=(100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 925 950 975 1000)
# === Loop over hours ===
for (( ih=0; ih<${#hhs[@]}; ih++ )); do
  hh=${hhs[$ih]}
  echo "Processing ymd=$ymd hour=$hh"

  # Define input GRIB file and output paths
  dir_in1="${dir_in}/${yyyy}${mm2}${dd2}/${hh}"
  grib_file="${dir_in1}/gfs.t${hh}z.pgrb2.0p25.f000" 
  echo $grib_file
  sfc_bin="${dir_out}/gfs_sfc${yyyy}-${mm2}-${dd2}.t${hh}"
  atm_bin="${dir_out}/gfs_atm${yyyy}-${mm2}-${dd2}.t${hh}"
  sfc_file="${dir_out}/gfs_sfc${ymd}.t${hh}"
  atm_file="${dir_out}/gfs_atm${ymd}.t${hh}"


  # Remove previous output files
  rm -f "$sfc_bin" "$atm_bin"


  # === Extract surface variables ===
  echo "Extracting surface fields to $sfc_bin"
  for i in "${!sfc_vars[@]}"; do
    var=${sfc_vars[$i]}
    if [[ $i -eq 0 ]]; then

 
      wgrib2 "$grib_file" -match "$var" -no_header -order we:ns -bin "$sfc_bin"
    else
      wgrib2 "$grib_file" -match "$var" -no_header -order we:ns -append -bin "$sfc_bin"
    fi
  done

# === Write TMP @ pressure levels first ===
  echo " Writing TMP @ pressure levels..."
  for i in "${!levels[@]}"; do
    level=${levels[$i]}
    match_expr=":TMP:${level} mb:"
    if [[ $i -eq 0 ]]; then

      echo "Writing TMP @ ${level} mb"
      wgrib2 "$grib_file" -match "$match_expr" -no_header -order we:ns -bin "$atm_bin"

    else
      echo "Writing TMP @ ${level} mb"
      wgrib2 "$grib_file" -match "$match_expr" -no_header -order we:ns -append -bin "$atm_bin"

    fi
  done

# === Then write RH @ same pressure levels ===
 echo "Writing RH @ pressure levels..."
 for level in "${levels[@]}"; do
  match_expr=":RH:${level} mb:"
  wgrib2 "$grib_file" -match "$match_expr" -no_header -order we:ns -append -bin "$atm_bin"
 done


#looks like don't need this machine (DISCOVER) is little endian
#${IDL} <<EOL
#.r convert_endian.pro
#convert_endian, "${atm_bin}", "${atm_file}", 42
#convert_endian, "${sfc_bin}", "${sfc_file}", 13
#exit
#EOL

done



