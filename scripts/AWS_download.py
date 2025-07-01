#$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
#$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 

'''
DOWNLOAD_RAW_ATMS_DATA

Purpose: Download NOAA ATMS Data from NOAA's AWS repository
  
Input:
    1) udata: Dynamic Python Dictionary that contains all user settings

Output:
   1) None (but raw data are downloaded to user-specified raw data directory)

    
Version:
2025-05-14: Original code started by Stephen D. Nicholls
2025-06-10: Added Option to grab  ATMS TDR and ATMS GEO and GFS files by Narges Shahroudi
2026-07-01: Added a script option to make changes externally for running MiRS by Narges Shahroudi 
IMPORTANT NOTES:
  1) This program requires python package boto3 to download data from AWS, which can be acquired from conda repositories
  2) NOAA NWS AWS Page for ATMS-SDR  
     NOAA-20: https://noaa-nesdis-n20-pds.s3.amazonaws.com/index.html
     NOAA-21: https://noaa-nesdis-n21-pds.s3.amazonaws.com/index.html
     
'''

####################
# Import Required Libraries   
import boto3                  	# Interface for accessing AWS services (e.g., S3 buckets)
from botocore import UNSIGNED   	# Allows anonymous (unsigned) access to AWS resources
from botocore.config import Config  	# Used to configure AWS client behavior
import datetime as dt        	# Provides classes for manipulating dates and times
import os                     	# Enables interaction with the operating system (e.g., file paths, commands)
import concurrent.futures     	# Supports launching parallel tasks using threads or processes
import argparse
####################
		
#cdata_name = 'ATMS-NOAA20'		# ATMS source name
#sdate = [dt.datetime(2024,10,30,00)]	# Starting day for download
#edate = [dt.datetime(2024,10,30,18)]	# Last day for download
#local_data_dir = os.getcwd()	# Local directory to which data will be written


# ---------------------------------------
# Extract datetime from ATMS filename
# ---------------------------------------
def extract_datetime_from_filename(key):
   try:
      t_index = key.find("_t")
      if t_index == -1:
         return None
      dt_str = key[t_index - 8:t_index + 7]  # Format: '20241010_t012339'
      return dt.datetime.strptime(dt_str, "%Y%m%d_t%H%M%S")
   except Exception as e:
      print(f"Error parsing datetime from {key}: {e}")
      return None

# ---------------------------------------
# Download a single file (used in parallel)
# ---------------------------------------
def download_file_if_needed(bucket_name, key, local_path):
   try:
      s3 = boto3.client('s3', config=Config(signature_version=UNSIGNED))
      if not os.path.exists(local_path):
         print(f"Downloading {os.path.basename(key)}")
         s3.download_file(bucket_name, key, local_path)
      else:
         print(f"File already exists: {os.path.basename(key)}")
   except Exception as e:
      print(f"Error downloading {key}: {e}")

# ---------------------------------------
# Main ATMS Data Download Function
# ---------------------------------------
def download_atms_data(local_data_dir, sdate, edate, cdata_name,prefix_base):
   print(f"Date range: {sdate[0]} to {edate[0]}")
   print(f"Satellite: {cdata_name}")

   if cdata_name in ['ATMS-NOAA20', 'ATMS-NOAA20-GEO']:
      bucket_name = "noaa-nesdis-n20-pds"
   elif cdata_name in ['ATMS-NOAA21', 'ATMS-NOAA21-GEO']:
      bucket_name = "noaa-nesdis-n21-pds"


   else:
      print(f"Satellite Data Source '{cdata_name}' is not supported.")
      return

   cur_date = sdate[0]

   while cur_date <= edate[0]:
      print(f"\nFetching {cdata_name} data for {cur_date.strftime('%Y/%m/%d')}")
#      prefix = f"ATMS-SDR/{cur_date.strftime('%Y/%m/%d')}/"
      prefix = f"{prefix_base}{cur_date.strftime('%Y/%m/%d')}/"  
      cur_download_dir = os.path.join(local_data_dir, cdata_name, cur_date.strftime('%Y%m%d'))
      os.makedirs(cur_download_dir, exist_ok=True)

      s3 = boto3.client('s3', config=Config(signature_version=UNSIGNED))
      continuation_token = None
      files_to_download = []

      while True:
         if continuation_token:
            response = s3.list_objects_v2(Bucket=bucket_name, Prefix=prefix, ContinuationToken=continuation_token)
         else:
            response = s3.list_objects_v2(Bucket=bucket_name, Prefix=prefix)

         if 'Contents' not in response:
            print(f"No files found for {cur_date}")
            break

         for obj in response['Contents']:
            key = obj['Key']
            file_time = extract_datetime_from_filename(key)

            if file_time and sdate[0] <= file_time <= edate[0]:
               file_name = os.path.basename(key)
               local_path = os.path.join(cur_download_dir, file_name)
               files_to_download.append((bucket_name, key, local_path))

         if response.get("IsTruncated"):
            continuation_token = response["NextContinuationToken"]
         else:
            break

      print(f"Downloading {len(files_to_download)} files in parallel...")

      with concurrent.futures.ThreadPoolExecutor(max_workers=6) as executor:
         future_to_file = {
            executor.submit(download_file_if_needed, bucket, key, path): (bucket, key, path)
            for bucket, key, path in files_to_download
         }

         for future in concurrent.futures.as_completed(future_to_file):
            bucket, key, path = future_to_file[future]
            try:
               future.result()
            except Exception as e:
               print(f"Failed to download {key}: {e}")

      cur_date += dt.timedelta(days=1)

   print("All downloads complete.")
    

def download_gfs_data(local_data_dir, sdate, edate):
    bucket_name = "noaa-gfs-bdp-pds"
    s3 = boto3.client('s3', config=Config(signature_version=UNSIGNED))

    cur_date = sdate[0]
    while cur_date <= edate[0]:
        for hour in range(0, 24, 6):  # every 6 hours by default (adjust if needed)
            hour_str = f"{hour:02d}"
            prefix = f"gfs.{cur_date.strftime('%Y%m%d')}/{hour_str}/atmos/"
            file_name = f"gfs.t{hour_str}z.pgrb2.0p25.f000"
            key = prefix + file_name

            cur_download_dir = os.path.join(local_data_dir, "GFS", cur_date.strftime('%Y%m%d'), hour_str)
            os.makedirs(cur_download_dir, exist_ok=True)
            local_path = os.path.join(cur_download_dir, file_name)

            try:
                print(f"Attempting to download GFS file: {key}")
                download_file_if_needed(bucket_name, key, local_path)
            except Exception as e:
                print(f"Error processing {key}: {e}")

        cur_date += dt.timedelta(days=1)

def download_data(local_data_dir, sdate, edate, cdata_name):
    if cdata_name in ['ATMS-NOAA20', 'ATMS-NOAA21']:
        prefix_base = "ATMS-TDR/"
        download_atms_data(local_data_dir, sdate, edate, cdata_name,prefix_base)
    elif cdata_name in ['ATMS-NOAA20-GEO', 'ATMS-NOAA21-GEO']:
        prefix_base = "ATMS-SDR-GEO/"
        download_atms_data(local_data_dir, sdate, edate, cdata_name,prefix_base)  # Might need minor edits if prefix structure changes
    elif cdata_name == 'GFS':
        download_gfs_data(local_data_dir, sdate, edate)
    else:
        print(f"Unknown data source: {cdata_name}")
    print(f"Downloading {cdata_name} from {sdate} to {edate} into {local_data_dir}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Download satellite or GFS data.")
    parser.add_argument('--local_data_dir', type=str, required=True, help='Path to save data')
    parser.add_argument('--sdate', type=str, required=True, help='Start date in format YYYY-MM-DD-HH')
    parser.add_argument('--edate', type=str, required=True, help='End date in format YYYY-MM-DD-HH')
    parser.add_argument('--cdata_name', type=str, required=True, help='Data name (e.g., GFS or ATMS-NOAA21)')

    args = parser.parse_args()


    sdate = [dt.datetime.strptime(args.sdate, "%Y-%m-%d-%H")]
    edate = [dt.datetime.strptime(args.edate, "%Y-%m-%d-%H")]
    download_data(args.local_data_dir, sdate, edate, args.cdata_name)







quit()
#download_atms_data(local_data_dir, sdate, edate, cdata_name)    

#'''
# ---------------------------------------
# Example Usage
# ---------------------------------------
if __name__ == "__main__":
    local_data_dir='/discover/nobackup/projects/hympi/nshahrou/atms_data'  # <-- update this path
    sdate = [dt.datetime(2024,10,30,00)]   # Starting day for download
    edate = [dt.datetime(2024,10,30,18)]   
    cdata_name = 'GFS' #'ATMS-NOAA21-GEO'  # or 'ATMS-NOAA21'
    download_data(local_data_dir, sdate, edate, cdata_name)

    
    
#    download_atms_data(local_data_dir, sdate, edate, cdata_name)
   
#'''
