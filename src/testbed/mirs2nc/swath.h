/***************************************************************
 *  File name          :  SWATH.h
 *  Function           :  Definition of all the relevant data 
 *  Called by          : 
 **************************************************************/
#pragma once

#include <stdio.h>
#include <iostream> 
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include "constants.h"
#include "defaults.h"
#include "defvar.h"

/* Time */

class mirs2nc_class {

public:

  short int*   year; //MAXSCAN
  short int*   doy; //MAXSCAN
  short int*   month; //MAXSCAN
  short int*   dom; //MAXSCAN
  short int*   hour; //MAXSCAN
  short int*   minute; //MAXSCAN
  short int*   second; //MAXSCAN
  
  double*      time_utc; //MAXSCAN
  
  /* Ancillary Data */
  short int*   orb_mode; //MAXSCAN
  short int*   iTypAtm; //MAXSCAN*MAXFOV 
  short int*   iTypSfc; //MAXSCAN*MAXFOV    
  int*   qc;//MAXSCAN*MAXFOV*4
  int*   qc_dep; //MAXSCAN*MAXFOV*4
  short int*   nattempt;//MAXSCAN*MAXFOV
  short int*   niter;//MAXSCAN*MAXFOV
  
  float*       chisq;//MAXSCAN*MAXFOV
  float*       lat;//MAXSCAN*MAXFOV
  float*       lon;//MAXSCAN*MAXFOV
  float*       angle;//MAXSCAN*MAXFOV
  float*       rel_azi;//MAXSCAN*MAXFOV
  float*       sza; //MAXSCAN*MAXFOV
  
  
  /* Products */
  
  float*       ptemp;//MAXSCAN*MAXFOV*MAXLAY
  float*       pvapor;//MAXSCAN*MAXFOV*MAXLAY
  float*       pozone;//MAXSCAN*MAXFOV*MAXLAY
  float*       pclw;//MAXSCAN*MAXFOV*MAXLAY
  float*       prain;//MAXSCAN*MAXFOV*MAXLAY
  float*       pgraupel;//MAXSCAN*MAXFOV*MAXLAY
  float*       psnow;//MAXSCAN*MAXFOV*MAXLAY
  float*       pice;//MAXSCAN*MAXFOV*MAXLAY
  
  /* Surface products */
  
  float*   tskin;//MAXSCAN*MAXFOV
  short int*   surfp;//MAXSCAN*MAXFOV
  short int*   emis;//MAXSCAN*MAXFOV*MAXCH
  short int*   prob_sf;//MAXSCAN*MAXFOV 
  short int*   prob_rf;//MAXSCAN*MAXFOV
  
  /* Misc. Data */
  short int     nqc;
  float*        player;//MAXLAY
  float*        plevel;//MAXLAY+1
  float*        freq; //MAXCH
  short int*    polo; //MAXCH
  
  int           alg_sn;
  
  /* MiRS version information */
  #define       MAXSTR   256
  #define       MAXCSTR  255
  char*        dap_ver_str; //MAXSTR
  char*        mirs_ver_str; //MAXSTR   
  char*        hist_pack_str; //MAXSTR
  int*         mode_flag_value;
  char*        foname_img; //MAXSTR
  char*        foname_snd; //MAXSTR
  short int*   bt;//MAXSCAN*MAXFOV*MAXCH
  short int*   ym;//MAXSCAN*MAXFOV*MAXCH
  short int*   yfwd;//MAXSCAN*MAXFOV*MAXCH
  short int*   chanSel;//MAXSCAN*MAXFOV*MAXCH
  
  /*DEP products *******/
  short int*   xxx; //MAXSCAN*MAXFOV
  short int*   tpw; //MAXSCAN*MAXFOV
  float*   clw; //MAXSCAN*MAXFOV
  float*   rwp; //MAXSCAN*MAXFOV
  float*   lwp; //MAXSCAN*MAXFOV
  short int*   swp; //MAXSCAN*MAXFOV
  short int*   iwp; //MAXSCAN*MAXFOV
  float*   gwp; //MAXSCAN*MAXFOV
  float*   rr; //MAXSCAN*MAXFOV
  short int*   swe; //MAXSCAN*MAXFOV
  short int*   snowgs; //MAXSCAN*MAXFOV
  short int*   snow; //MAXSCAN*MAXFOV
  short int*   sice; //MAXSCAN*MAXFOV
  short int*   sice_my; //MAXSCAN*MAXFOV
  short int*   sice_fy; //MAXSCAN*MAXFOV 
  short int*   sfr; //MAXSCAN*MAXFOV
  short int*   cldtop; //MAXSCAN*MAXFOV
  short int*   cldbase; //MAXSCAN*MAXFOV
  short int*   cldthick; //MAXSCAN*MAXFOV
  short int*   preciptype; //MAXSCAN*MAXFOV
  short int*   rflag; //MAXSCAN*MAXFOV
  short int*   surfm; //MAXSCAN*MAXFOV
  short int*   windsp; //MAXSCAN*MAXFOV
  short int*   winddir; //MAXSCAN*MAXFOV
  short int*   windu; //MAXSCAN*MAXFOV
  short int*   windv; //MAXSCAN*MAXFOV
  char* ncFname_snd;
  char* date_created_snd;
  
  /******************************
   * NPP ATMS Special parameters
   ******************************/
  float*    geospatial_first_scanline_first_fov_lat;
  float*    geospatial_first_scanline_first_fov_lon;
  float*    geospatial_first_scanline_last_fov_lat;
  float*    geospatial_first_scanline_last_fov_lon;
  float*    geospatial_last_scanline_first_fov_lat;
  float*    geospatial_last_scanline_first_fov_lon;
  float*    geospatial_last_scanline_last_fov_lat;
  float*    geospatial_last_scanline_last_fov_lon;
  
  int*      total_number_retrievals;// nprf : total numer of profiles
  int*      start_orbit_number;
  int*      end_orbit_number;
  float*    percentage_optimal_retrievals;// QC=0
  float*    percentage_suboptimal_retrievals;// QC=1
  float*    percentage_bad_retrievals;// QC=2
 
  char*     time_coverage_start;
  char*     time_coverage_end; 
  char*     EDR_name;
  char*     DEP_name;
  char*     RDR_name; 
  char*     production_site_str;
  int       production_site_len;
  char*     production_environment_str;
  int       production_environment_len;
  
  
  /* Define subroutines */
  
  int   gnrt_nc_img( char * fname_nc_img, short int nscan, short int nspot, short int  nchan, char *satid );
  int   gnrt_nc_snd( char *fname_nc_snd, short int nscan, short int nspot, 
                     short int nlay, short int nchan, char *satid  );
  void  rmirs_dep( FILE *mirsd, short int *nscan_old, short int *nspot, char *satid );
  void  rmirs_edr( FILE *mirsd, short int *nscan_old, short int *nspot, 
                   short int *nlayer, short int *nchan_old, char * satid );
  void  rmirs_wnetcdf( char *input_edr, char *input_dep, char *output_dir, char *output_img, 
                       char *output_snd,  char *start_orbit_number_str, char *end_orbit_number_str, 
                       char *prod_site_str, char *prod_environment_str, char* mirs_major, char* mirs_minorr, char* hist_pack );
  void  set_nc_img( char * fname_nc_img, short int nscan, short int nspot, 
                    short int  nchan, char *satid );
  void  set_nc_snd( char *fname_nc_snd, short int nscan, short int nspot, 
                    short int nlay, short int nchan, char *satid );
  int   wrt_nc_gdata( char *fname_nc_snd, short int nscan, short int nspot, 
                      short int nchan, short int fr_mem_flg );
  int   wrt_nc_img( char * fname_nc_img, short int nscan, short int nspot, short int  nchan );
  int   wrt_nc_snd( char * fname_nc_snd, short int nscan, short int nspot, short int  nlay );
  void  wrt_Sfc_Atm( short int nscan, short int nspot, char* satid, char* nc_name_snd );
  void  check_scan_numb( char *start_orbit_number_str,
                         char *end_orbit_number_str,
                         char *prod_site_str,
                         char *prod_environment_str,
                         short int numscan,short int numchan,
                         short int numspot,short int numlayer );                      
  void  get_base_name( int intsatid, char *fname_edr, char *fname_dep, char *input_edr, char *input_dep );                        
  void  gnrt_attr_snd( int ncid, char* satid, char* date_created_snd, char* fname_nc_snd); 
  void  gnrt_attr_img( int ncid, char* satid, char* date_created_img, char* fname_nc_img ); 
  
  /* Output Geo and ancillary data */
  template <typename T>
  void put_nc_glb_dat( int ncid, const char* name, short int fr_mem_flg,
                       const size_t *start, const size_t *count, const T *op );
  
  /* Output variables of SND and ING to NC file */                   
//  template <typename T>
//  void put_nc_snd_img( int ncid, const char* name, const size_t *start, const size_t *count, const T *op );                     
  
  void  MemAllocEDR();
  void  MemAllocDEP();
  
  void  MemDeleteEDR();
  void  MemDeleteDEP();

  mirs2nc_class();
  ~mirs2nc_class();

};
