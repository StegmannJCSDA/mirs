/************************************************************************************
 *  Program Name      : wrt_nc_img.c
 *  Type              : Subroutine
 *  Function          : Program creates netcdf image file 
 *  Input Files       : 
 *  Output Files      : Netcdf files
 *  Subroutine Called : 
 *  Called by         : set_nc_img.c
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *  
 *  12/2008      v0              JZhao (PSGS)
 *  05/2011      v1              Wanchun Chen  updated for netcdf4
 *  01/2020      v2              Ming Fang modified wrt_nc_img using put_nc_snd_img
 ***********************************************************************************/

#include "constants.h"
#include "swath.h"
#include <netcdf.h>
//#include <iostream>
//using namespace std;

int mirs2nc_class::wrt_nc_img(char * fname_nc_img, short int nscan, short int nspot, short int  nchan){
   int  stat;  /* return status */
   int  ncid;  /* netCDF id */
   int  errid = 18;

   size_t start2[]={0,0};
   size_t count2[]={size_t(nscan),size_t(nspot)};
   size_t start3[]={0,0,0};
   size_t count3[]={size_t(nscan),size_t(nspot),size_t(nchan)};


   if ((stat = nc_open(fname_nc_img, NC_WRITE, &ncid)))
   check_err(stat,__LINE__,__FILE__,errid);


   // PROFILE products

   put_nc_snd_img( ncid, "BT", start3, count3, bt );
   put_nc_snd_img( ncid, "YM", start3, count3, ym );
   put_nc_snd_img( ncid, "ChanSel", start3, count3, chanSel );
   put_nc_snd_img( ncid, "TPW", start2, count2, tpw );
   put_nc_snd_img( ncid, "CLW", start2, count2, clw);
   put_nc_snd_img( ncid, "RWP", start2, count2, rwp);
   put_nc_snd_img( ncid, "LWP", start2, count2, lwp);
   put_nc_snd_img( ncid, "SWP", start2, count2, swp);
   put_nc_snd_img( ncid, "IWP", start2, count2, iwp);
   put_nc_snd_img( ncid, "GWP", start2, count2, gwp);
   put_nc_snd_img( ncid, "RR",  start2, count2, rr);
   put_nc_snd_img( ncid, "Snow", start2, count2, snow);
   put_nc_snd_img( ncid, "SWE", start2, count2, swe);
   put_nc_snd_img( ncid, "SnowGS", start2, count2, snowgs);
   put_nc_snd_img( ncid, "SIce", start2, count2, sice);
   put_nc_snd_img( ncid, "SIce_MY", start2, count2, sice_my);
   put_nc_snd_img( ncid, "SIce_FY", start2, count2, sice_fy);
   put_nc_snd_img( ncid, "TSkin", start2, count2, tskin);
   put_nc_snd_img( ncid, "SurfP", start2, count2, surfp);


   // Emissivity need trim as BT

   put_nc_snd_img( ncid, "Emis", start3, count3, emis);
   put_nc_snd_img( ncid, "SFR", start2, count2, sfr);
   put_nc_snd_img( ncid, "CldTop", start2, count2, cldtop);
   put_nc_snd_img( ncid, "CldBase", start2, count2, cldbase);
   put_nc_snd_img( ncid, "CldThick", start2, count2, cldthick);
   put_nc_snd_img( ncid, "PrecipType", start2, count2, preciptype);
   put_nc_snd_img( ncid, "RFlag", start2, count2, rflag);
   put_nc_snd_img( ncid, "SurfM", start2, count2, surfm);
   put_nc_snd_img( ncid, "WindSp", start2, count2, windsp);
   put_nc_snd_img( ncid, "WindDir", start2, count2, winddir);
   put_nc_snd_img( ncid, "WindU", start2, count2, windu);
   put_nc_snd_img( ncid, "WindV", start2, count2, windv);
   put_nc_snd_img( ncid, "Prob_SF", start2, count2, prob_sf);

/* Waiting for us to use this.
   put_nc_snd_img( ncid, "Prob_RF", start2, count2, prob_rf);
*/ 
   
   stat = nc_close(ncid);
   check_err(stat,__LINE__,__FILE__,errid);

   return 0;
}

