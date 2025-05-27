/***************************************************************************
 *  Program Name      : wrt_nc_snd.cpp
 *  Type              : Subroutine
 *  Function          : Program creates netcdf sounding file 
 *  Input Files       : 
 *  Output Files      : Netcdf files
 *  Subroutine Called : 
 *  Called by         : set_nc_snd.cpp
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *  
 *  12/2008      v0              JZhao (PSGS)
 *  05/2011      v1              Wanchun Chen  updated for netcdf4
 *  11/2011      v2              Kevin Garrett commented out writing O3 profile
 *  02/2020      v3              Ming Fang added put_nc_snd_img and wrt_Sfc_Atm
 *                               and modified wrt_nc_snd
 **************************************************************************/
 
#include "constants.h"
#include "swath.h"
#include <netcdf.h>

using namespace std;

int mirs2nc_class::wrt_nc_snd(char * fname_nc_snd, short int nscan, short int nspot, short int  nlay){
   int  stat;  /* return status */
   int  ncid;  /* netCDF id */
   int  errid = 16;
   int  varid;
   size_t start3[]={0,0,0};
   size_t count3[]={size_t(nscan),size_t(nspot),size_t(nlay)};

   stat = nc_open(fname_nc_snd, NC_WRITE, &ncid);
   check_err(stat,__LINE__,__FILE__,errid);

   //Layer of the products
   if ((stat = nc_inq_varid(ncid, "Player", &varid)))
   check_err(stat,__LINE__,__FILE__,errid); 
   if ((stat = nc_put_var_float(ncid, varid, player)))
   check_err(stat,__LINE__,__FILE__,errid);
   if(player)   delete[] player;
   
   if ((stat = nc_inq_varid(ncid, "Plevel", &varid)))
   check_err(stat,__LINE__,__FILE__,errid); 
   if ((stat = nc_put_var_float(ncid, varid, plevel)))
   check_err(stat,__LINE__,__FILE__,errid);
   if(plevel)   delete[] plevel;

   //PROFILE products

   put_nc_snd_img( ncid, "PTemp", start3, count3, ptemp );
   put_nc_snd_img( ncid, "PVapor", start3, count3, pvapor );
   put_nc_snd_img( ncid, "PClw", start3, count3, pclw );
   put_nc_snd_img( ncid, "PRain", start3, count3, prain );
   put_nc_snd_img( ncid, "PGraupel", start3, count3, pgraupel );
   put_nc_snd_img( ncid, "PSnow", start3, count3, psnow );
   put_nc_snd_img( ncid, "PIce", start3, count3, pice );
   put_nc_snd_img( ncid, "SurfP", start3, count3, surfp );


   stat = nc_close(ncid);
   check_err(stat,__LINE__,__FILE__,errid);

   return 0;
}

