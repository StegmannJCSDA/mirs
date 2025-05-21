/***********************************************************************        
 *  Header file name  :  DEFAULTS.h     
 *  Programer         :  Bing-Zhang (SMSRC)
 *  Date              :  2/8/95              
 *  Function          :  This is the header file for all standard headers.
 ***********************************************************************/ 

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <netcdf.h>


void gnrt_nc_fname(char* in_edrn, char* outdir, char* nc_edrn, char* nc_depn);
void gnrt_nc_fname_npp(char* in_edrn, char* outdir, char* nc_edrn, char* nc_depn);
void check_err(const int stat, const int line, const char *file, int err_id);
void init_sw();
void jd2md(short int jday, short int year, short int *dom, short int *mon);
int leapyear(short int);
void utc2hms(float today_sec, short int *hr, short int *min, short int *sec);
FILE *openfs_sw(char *fname);
int intSwap(char *value) ;
short int shortSwap(char *value);
float floatSwap(char *value);
int getEndian();
void  gen_nc_fname( char *fname_edr, char *nc_name_img, char *output_img,
                    char *nc_name_snd, char *output_dir, char *output_snd );
void  dfdim(int ncid, const char *name, size_t len, int *idp);
void  put_vatt_txt(int ncid, int vid, const char* name, const char* value);  
void  put_gatt_txt(int ncid, const char* name, const char* var);                 




