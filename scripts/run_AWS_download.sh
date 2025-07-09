#!/bin/bash

##cdata_name = 'GFS' or  'ATMS-NOAA21-GEO'   or 'ATMS-NOAA21'

python AWS_download.py \
  --local_data_dir="/discover/nobackup/projects/hympi/nshahrou/data/" \
  --sdate="2024-10-30-00" \
  --edate="2024-10-31-18" \
  --cdata_name="ATMS-NOAA21"
