/***************************************************************************
 *  Program Name      : set_nc_img.cpp
 *  Type              : Subroutine
 *  Function          : Program creates netcdf sounding file 
 *  Input Files       : 
 *  Output Files      : Netcdf files
 *  Subroutine Called : gnrt_nc_img, wrt_nc_img
 *  Called by         : rmirs_wnetcdf.cpp
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *  
 *  12/05/2008      v              JZhao (PSGS)
 *  02/24/2020                     Changed .c to .cpp
 *************************************************************************/
#include "constants.h"
#include "swath.h"

void mirs2nc_class::set_nc_img(char * fname_nc_img, short int nscan, short int nspot, short int  nchan, char *satid){

  int status = 0;
  
  status=gnrt_nc_img(fname_nc_img, nscan, nspot, nchan, satid);
  if(status != 0) {
    printf("Error in generate the image netcdf file %s\n",fname_nc_img);
    exit(17);
  }
  
  int fr_mem_flg = 1;
  status=wrt_nc_gdata(fname_nc_img, nscan, nspot, nchan, fr_mem_flg);
  if(status != 0) {
    printf("Error in writing geo_data to the image netcdf file %s\n",fname_nc_img);
    exit(18);
  }
  
  status=wrt_nc_img(fname_nc_img, nscan, nspot, nchan);
  if(status != 0) {
    printf("Error in writing data to the image netcdf file %s\n",fname_nc_img);
    exit(19);
  }

}
