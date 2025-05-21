/***************************************************************************
 *  Program Name      : wrt_nc_gdata.c
 *  Type              : Subroutine
 *  Function          : Program creates netcdf sounding file 
 *  Input Files       : 
 *  Output Files      : Netcdf files
 *  Subroutine Called : 
 *  Called by         : set_nc_snd.c,set_nc_img.c
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *  
 *  12/2008      v0              JZhao (PSGS)
 *  05/2011      v1              Wanchun Chen  updated for netcdf4
 *  01/2020      v2              Ming Fang
 *************************************************************************/
#include <iostream> 
#include "constants.h"
#include "swath.h"
#include <netcdf.h>

using namespace std;

template<typename T>
void mirs2nc_class::put_nc_glb_dat( int ncid, const char* name, short int fr_mem_flg, 
                            const size_t *start, const size_t *count, const T *op )
{
  int  stat;     /* return status */
  int  errid = 18;
  int  varid;
  if ( (stat = nc_inq_varid(ncid, name, &varid)) )
    check_err(stat,__LINE__,__FILE__,errid);
  if ( (stat = nc_put_vara(ncid, varid, start, count, op)))
    check_err(stat,__LINE__,__FILE__,errid);
  if(op && fr_mem_flg && strcmp(name, "Sfc_type") !=0 && strcmp(name, "Atm_type")) 
    delete[] op;
}


int mirs2nc_class::wrt_nc_gdata(char *fname_nc_snd, short int nscan, short int nspot, short int nchan, short int fr_mem_flg){
   int  stat;
   int  ncid;  /* netCDF id */
   int  errid = 18;
   int  nqc = 4; 
   
   size_t start3[]={0,0,0};
   size_t count3[]={size_t(nscan),size_t(nspot),size_t(nqc)};
   size_t start1[]={0};
   size_t count1[]={size_t(nscan)};
   size_t start2[]={0,0};
   size_t count2[]={size_t(nscan),size_t(nspot)};

   size_t start0[]={0};
   size_t count0[]={size_t(nchan)};

   stat = nc_open(fname_nc_snd, NC_WRITE, &ncid);
   check_err(stat,__LINE__,__FILE__,errid);
    
   // channel infor  

   put_nc_glb_dat(ncid, "Freq", fr_mem_flg, start0, count0, freq);
   put_nc_glb_dat(ncid, "Polo", fr_mem_flg, start0, count0, polo);
   
   //Geo data

   put_nc_glb_dat(ncid, "ScanTime_year", fr_mem_flg, start1, count1, year);
   put_nc_glb_dat(ncid, "ScanTime_doy", fr_mem_flg, start1, count1, doy);
   put_nc_glb_dat(ncid, "ScanTime_month", fr_mem_flg, start1, count1, month);
   put_nc_glb_dat(ncid, "ScanTime_dom", fr_mem_flg, start1, count1, dom);
   put_nc_glb_dat(ncid, "ScanTime_hour", fr_mem_flg, start1, count1, hour);
   put_nc_glb_dat(ncid, "ScanTime_minute", fr_mem_flg, start1, count1, minute);
   put_nc_glb_dat(ncid, "ScanTime_second", fr_mem_flg, start1, count1, second);
   put_nc_glb_dat(ncid, "ScanTime_UTC", fr_mem_flg, start1, count1, time_utc);  
   put_nc_glb_dat(ncid, "Orb_mode", fr_mem_flg, start1, count1, orb_mode);      
   put_nc_glb_dat(ncid, "Latitude", fr_mem_flg, start2, count2, lat);   
   put_nc_glb_dat(ncid, "Longitude", fr_mem_flg, start2, count2, lon);   

   //Ancillary data
      
   if(fr_mem_flg){ 
      put_nc_glb_dat(ncid, "Sfc_type", fr_mem_flg, start2, count2, iTypSfc);         
      put_nc_glb_dat(ncid, "Atm_type", fr_mem_flg, start2, count2, iTypAtm);
   }     
    

   //QC data

   put_nc_glb_dat(ncid, "Qc", fr_mem_flg, start3, count3, qc);   
   put_nc_glb_dat(ncid, "ChiSqr", fr_mem_flg, start2, count2, chisq);
   put_nc_glb_dat(ncid, "LZ_angle", fr_mem_flg, start2, count2, angle);   
   put_nc_glb_dat(ncid, "RAzi_angle", fr_mem_flg, start2, count2, rel_azi);   
   put_nc_glb_dat(ncid, "SZ_angle", fr_mem_flg, start2, count2, sza);

   stat = nc_close(ncid);
   check_err(stat,__LINE__,__FILE__,errid);
   
   return 0;
}

