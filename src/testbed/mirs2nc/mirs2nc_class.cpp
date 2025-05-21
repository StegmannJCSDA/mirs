/*************************************************************************************
*  Program Name      : mirs2nc_class.cpp
*  Type              : Subroutine
*                      Implements the constructor nad deconstructor of class mirs2nc_class
*  Function          : Constructor allocates memory for global variables
*                      Deconstructor releases allocated memory
*  Input Files       : None
*  Output Files      : None
*  Subroutine Called : None
*  Called by         : mirs2nc.cpp
*
*  Modification History:
*      Date       Version         Description of Changes
*     ------     ---------       ------------------------
*
*  02/2020         v1             Ming Fang
**************************************************************************************/
#include "swath.h"
using namespace std;

mirs2nc_class::mirs2nc_class() {


/* MiRS version information */

  #define           MAXSTR   256
  #define           MAXCSTR  255
  dap_ver_str       = new char[MAXSTR];
  mirs_ver_str      = new char[MAXSTR];
  hist_pack_str     = new char[MAXSTR];
  mode_flag_value   = new int[1]; //descending ascending flag for the whole file
  foname_img        = new char[MAXSTR];
  foname_snd        = new char[MAXSTR];

/**************************************************
* NPP ATMS Special parameters
**************************************************/
  geospatial_first_scanline_first_fov_lat = new float[1];
  geospatial_first_scanline_first_fov_lon = new float[1];
  geospatial_first_scanline_last_fov_lat  = new float[1];
  geospatial_first_scanline_last_fov_lon  = new float[1];
  geospatial_last_scanline_first_fov_lat  = new float[1];
  geospatial_last_scanline_first_fov_lon  = new float[1];
  geospatial_last_scanline_last_fov_lat   = new float[1];
  geospatial_last_scanline_last_fov_lon   = new float[1];
  
  total_number_retrievals                 = new int[1]; // nprf : total numer of profiles
  start_orbit_number                      = new int[1]; // starting orbit number
  end_orbit_number                        = new int[1]; // ending orbit number
  percentage_optimal_retrievals           = new float[1]; // QC=0
  percentage_suboptimal_retrievals        = new float[1]; // QC=1
  percentage_bad_retrievals               = new float[1]; // QC=2
  
  time_coverage_start                     = new char[21];
  time_coverage_end                       = new char[21];
  //char          date_created = new char[21];
  
  EDR_name = new char[256];
  DEP_name = new char[256];
  RDR_name = new char[256];
  
  production_site_str         = new char[50];
  production_environment_str  = new char[50];
  
  date_created_snd            = new char[21];

} //end of constructor

mirs2nc_class::~mirs2nc_class() {

  if(qc_dep)   delete[] qc_dep;
  if(nattempt) delete[] nattempt;
  if(niter)    delete[] niter;
  
  if(prob_rf) delete[] prob_rf;
  
  if(yfwd)    delete[] yfwd;
   
  if(dap_ver_str)       delete[] dap_ver_str;
  if(mirs_ver_str)      delete[] mirs_ver_str;
  if(hist_pack_str)     delete[] hist_pack_str;
  if (foname_img)       delete[] foname_img;
  if (foname_snd)       delete[] foname_snd;
  
  if(geospatial_first_scanline_first_fov_lat)
    delete[] geospatial_first_scanline_first_fov_lat;
  if(geospatial_first_scanline_first_fov_lon)
    delete[] geospatial_first_scanline_first_fov_lon;
  if(geospatial_first_scanline_last_fov_lat)
    delete[] geospatial_first_scanline_last_fov_lat;
  if(geospatial_first_scanline_last_fov_lon)
    delete[] geospatial_first_scanline_last_fov_lon;
  if(geospatial_last_scanline_first_fov_lat)
    delete[] geospatial_last_scanline_first_fov_lat;
  if(geospatial_last_scanline_first_fov_lon)
    delete[] geospatial_last_scanline_first_fov_lon;
  if(geospatial_last_scanline_last_fov_lat)
    delete[] geospatial_last_scanline_last_fov_lat;
  if(geospatial_last_scanline_last_fov_lon)
    delete[] geospatial_last_scanline_last_fov_lon;

  if(total_number_retrievals) // nprf : total numer of profiles
    delete[] total_number_retrievals;
  if(start_orbit_number) // starting orbit number
    delete[] start_orbit_number;
  if(end_orbit_number) // ending orbit number
    delete[] end_orbit_number;
  if(percentage_optimal_retrievals) // QC=0
    delete[] percentage_optimal_retrievals;
  if(percentage_suboptimal_retrievals) // QC=1
    delete[] percentage_suboptimal_retrievals;
  if(percentage_bad_retrievals[1]) // QC=2
    delete[] percentage_bad_retrievals;

  if(time_coverage_start) delete[] time_coverage_start;
  if(time_coverage_end)   delete[] time_coverage_end;

  if(EDR_name) delete[] EDR_name;
  if(DEP_name) delete[] DEP_name;
  if(RDR_name) delete[] RDR_name;

  if(production_site_str)         delete[] production_site_str;
  if(production_environment_str)  delete[] production_environment_str;
  
  if(date_created_snd) delete[] date_created_snd;
 
}

