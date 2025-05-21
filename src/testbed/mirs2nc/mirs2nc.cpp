/**************************************************************************
 *  Program Name      : mirs2nc.cpp 
 *  Type              : Main program     
 *  Function          : Program reads MIRS  generated products and 
 *       creates netcdf  files 
 *  Input Files       : Binary sounding/dep files
 *  Output Files      : netcdf sounding/dep files 
 *  Subroutine Called : rama_wnetcdf.cpp
 *  Called by          : None
 * 
 *  Modification History:
 *      Date      Version     Description of Changes
 *     ------     ---------       ------------------------
 *           
 *   12/05/2008     v   Based on MIRS2HDF.c        jzhao(PSGS)
 *   04/21/2008     Modified for NPP ATMS cases    wchen(DELL)
 *   05/17/2011     Let script level to create output filenames and output files
 *                  names are passed in as arguments.  wchen(DELL)
 *   12/20/2016     Added JPSS1 (N20)              jchen(ESSIC/UMD) 
 *   04/23/2018     Fixes for build rhoneyager(IMSG)
 *   02/24/2020     1. Changed C to C++            mfang(SMCD)
 *                  2. Changed workflow
 *                  3. Changed all global arrays to 1D poiters
 *                  2. Changed static space allocation to dynamic allocation
 *                  5. Introduced parallel computation
 *                  @  Reduce required maximum memory and speed up computation
 *
 *************************************************************************/
#include <iostream>
#include "swath.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != narg ){
     printf("Usage:mirs2nc inputfile_edr inputfile_dep outputdir outputfile_img outputfile_snd start_orbit_number_str end_orbit_number_str prod_site_str prod_environment_str mirs_dap_major_num mirs_dap_minor_num hist_pack\n");
     exit(1);
   }

   char  *inputfile_edr          = argv[1];
   char  *inputfile_dep          = argv[2];
   char  *output_dir             = argv[3];
   char  *outputfile_img         = argv[4];
   char  *outputfile_snd         = argv[5];
   char  *start_orbit_number_str = argv[6];
   char  *end_orbit_number_str   = argv[7];
   char  *prod_site_str          = argv[8];
   char  *prod_environment_str   = argv[9];
   char  *mirs_dap_major         = argv[10];
   char  *mirs_dap_minor         = argv[11];
   char   *hist_pack_version     = argv[12];

/*---------------------------------------------------*
 * get arguments from command line
 *---------------------------------------------------*/
   //   printf("prod_site_str & prod_environment_str:\n%s\n%s\n", prod_site_str, prod_environment_str);   
   
/*-------------------------------------------------------*
 * Read MIRS EDR & DEP files and save into netcdf4 files
 *-------------------------------------------------------*/
 
   mirs2nc_class *p = new mirs2nc_class;
   
   p->rmirs_wnetcdf( inputfile_edr,inputfile_dep,output_dir,outputfile_img,
           outputfile_snd,start_orbit_number_str,end_orbit_number_str,
           prod_site_str,prod_environment_str,mirs_dap_major,mirs_dap_minor,hist_pack_version );


   delete p; 
  
   return 0;
   
}  /* end of mirs2nc.c */
