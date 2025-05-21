/***************************************************************************
 *  Program Name      : gnrt_nc_snd.cpp
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
 *  12/2008         v0              Jiang Zhao
 *  07/2011         v1              Wanchun Chen
 *  12/2016         v1.1            Junye Chen 
 *  02/2020         v2              Ming Fang
 *                                  Added gnrt_attr_snd, modified gnrt_nc_snd
 *************************************************************************/
#include "constants.h"
#include "swath.h"
#include <netcdf.h>
#include "common_header.h"
#include <time.h>
#include <unistd.h>

int mirs2nc_class::gnrt_nc_snd(char *fname_nc_snd, short int nscan, short int nspot, short int nlay, short int nchan, char *satid) {  
   int  stat;  /* return status */
   int  ncid;  /* netCDF id */
   int  errid = 15;

   /* dimension ids */
   int Scanline_dim;
   int Field_of_view_dim;
   int P_Layer_dim;
   int P_Level_dim;
   int Channel_dim;
   int Qc_dim_dim;

   /* dimension lengths */
   size_t Scanline_len = nscan;
   size_t Field_of_view_len = nspot;
   size_t P_Layer_len = nlay;
   size_t P_Level_len = nlay+1;
   size_t Channel_len = nchan;
   size_t Qc_dim_len = 4;

   /* variable ids */
   int ScanTime_year_id;
   int ScanTime_doy_id;
   int ScanTime_month_id;
   int ScanTime_dom_id;
   int ScanTime_hour_id;
   int ScanTime_minute_id;
   int ScanTime_second_id;
   int ScanTime_UTC_id;
   int Orb_mode_id;
   int Latitude_id;
   int Longitude_id;
   int Sfc_type_id;
   int Atm_type_id;
   int Qc_id;
   int ChiSqr_id;
   int LZ_angle_id;
   int RAzi_angle_id;
   int SZ_angle_id;
   int Player_id;
   int Plevel_id;
   int PTemp_id;
   int PVapor_id;
   int PClw_id;
   int PRain_id;
   int PGraupel_id;
   int PSnow_id;
   int PIce_id;
   int SurfP_id;
   int Freq_id;
   int Polo_id;
   int quality_info_id;

/* rank (number of dimensions) for each variable */
#  define RANK_ScanTime_year 1
#  define RANK_ScanTime_doy 1
#  define RANK_ScanTime_month 1
#  define RANK_ScanTime_dom 1
#  define RANK_ScanTime_hour 1
#  define RANK_ScanTime_minute 1
#  define RANK_ScanTime_second 1
#  define RANK_ScanTime_UTC 1
#  define RANK_Orb_mode 1
#  define RANK_Latitude 2
#  define RANK_Longitude 2
#  define RANK_Sfc_type 2
#  define RANK_Atm_type 2
#  define RANK_Qc 3
#  define RANK_NAttempt 2
#  define RANK_NIter 2
#  define RANK_ChiSqr 2
#  define RANK_LZ_angle 2
#  define RANK_RAzi_angle 2
#  define RANK_SZ_angle 2
#  define RANK_Player 1
#  define RANK_Plevel 1
#  define RANK_PTemp 3
#  define RANK_PVapor 3
#  define RANK_POzone 3
#  define RANK_PClw 3
#  define RANK_PRain 3
#  define RANK_PGraupel 3
#  define RANK_PSnow 3
#  define RANK_PIce 3
#  define RANK_SurfP 2
#  define RANK_Freq 1
#  define RANK_Polo 1

   /* variable shapes */
   int Player_dims[RANK_Player];
   int Plevel_dims[RANK_Plevel];


   /* attribute vectors */
   double SurfP_scale[1];
   float fillValF[1];
   short fillValS[1];
   double fillValD[1];

   float LZ_angle_range[2];
   float Latitude_range[2];
   float Longitude_range[2];
   float ChiSqr_range[2];
   float PTemp_range[2];
   float PVapor_range[2];
   float PClw_range[2];
   float PRain_range[2];
   float PGraupel_range[2];
   float PSnow_range[2];
   float PIce_range[2];
   int SurfP_range[2];
   int Orb_mode_range[2];
   int Sfc_type_range[2];
   float Player_range[2];
   float Plevel_range[2];

   double ScanTime_UTC_range[2];
   int ScanTime_dom_range[2];
   int ScanTime_doy_range[2];
   int ScanTime_hour_range[2];
   int ScanTime_minute_range[2];
   int ScanTime_month_range[2];
   int ScanTime_second_range[2];
   int ScanTime_year_range[2];

   time_t rawtime;
   struct tm * timeinfo;

   fillValF[0] = -999.;
   fillValS[0] = -999;
   fillValD[0] = -999;

   /* enter define mode */

   stat = nc_create(fname_nc_snd, NC_CLOBBER|NC_NETCDF4, &ncid);
   check_err(stat,__LINE__,__FILE__,errid);

   /* creation time */
   time ( &rawtime );
   timeinfo = gmtime ( &rawtime );
   sprintf(date_created_snd,"%4d-%02d-%02dT%02d:%02d:%02dZ",
      timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

   /* define dimensions */
   dfdim(ncid, "Scanline", Scanline_len, &Scanline_dim);
   dfdim(ncid, "Field_of_view", Field_of_view_len, &Field_of_view_dim);
   dfdim(ncid, "P_Layer", P_Layer_len, &P_Layer_dim);
   dfdim(ncid, "P_Level", P_Level_len, &P_Level_dim);
   dfdim(ncid, "Channel", Channel_len, &Channel_dim);
   dfdim(ncid, "Qc_dim", Qc_dim_len, &Qc_dim_dim);

   /* define variables */
   
   int dim1D[1];
   dim1D[0] = Channel_dim;
   
   dfvar(ncid, "Freq", NC_FLOAT, RANK_Freq, dim1D, &Freq_id);
   dfvar(ncid, "Polo", NC_SHORT, RANK_Polo, dim1D, &Polo_id);

   dim1D[0] = Scanline_dim;

   dfvar(ncid, "ScanTime_year", NC_SHORT, RANK_ScanTime_year, dim1D, &ScanTime_year_id);
   dfvar(ncid, "ScanTime_doy", NC_SHORT, RANK_ScanTime_doy, dim1D, &ScanTime_doy_id);
   dfvar(ncid, "ScanTime_month", NC_SHORT, RANK_ScanTime_month, dim1D, &ScanTime_month_id);
   dfvar(ncid, "ScanTime_dom", NC_SHORT, RANK_ScanTime_dom, dim1D, &ScanTime_dom_id);
   dfvar(ncid, "ScanTime_hour", NC_SHORT, RANK_ScanTime_hour, dim1D, &ScanTime_hour_id);
   dfvar(ncid, "ScanTime_minute", NC_SHORT, RANK_ScanTime_minute, dim1D, &ScanTime_minute_id);
   dfvar(ncid, "ScanTime_second", NC_SHORT, RANK_ScanTime_second, dim1D, &ScanTime_second_id);
   dfvar(ncid, "ScanTime_UTC", NC_DOUBLE, RANK_ScanTime_UTC, dim1D, &ScanTime_UTC_id);
   dfvar(ncid, "Orb_mode", NC_SHORT, RANK_Orb_mode, dim1D, &Orb_mode_id);

   int dim2D[2];
   dim2D[0] = Scanline_dim;
   dim2D[1] = Field_of_view_dim;

   dfvar(ncid, "Latitude", NC_FLOAT, RANK_Latitude, dim2D, &Latitude_id);
   dfvar(ncid, "Longitude", NC_FLOAT, RANK_Longitude, dim2D, &Longitude_id);
   dfvar(ncid, "Sfc_type", NC_SHORT, RANK_Sfc_type, dim2D, &Sfc_type_id);
   dfvar(ncid, "Atm_type", NC_SHORT, RANK_Atm_type, dim2D, &Atm_type_id);

   int dim3D[3];
   dim3D[0] = Scanline_dim;
   dim3D[1] = Field_of_view_dim;
   dim3D[2] = Qc_dim_dim;
   dfvar(ncid, "Qc", NC_INT, RANK_Qc, dim3D, &Qc_id);
   
   dfvar(ncid, "ChiSqr", NC_FLOAT, RANK_ChiSqr, dim2D, &ChiSqr_id);
   
   dfvar(ncid, "LZ_angle", NC_FLOAT, RANK_LZ_angle, dim2D, &LZ_angle_id); 
   put_vatt_txt(ncid, LZ_angle_id, "units", "degrees");
   put_vatt(ncid, LZ_angle_id, "_FillValue", NC_FLOAT, 1, fillValF);
   LZ_angle_range[0] = -70.;
   LZ_angle_range[1] = 70.;
   put_vatt(ncid, LZ_angle_id, "valid_range", NC_FLOAT, 2, LZ_angle_range);

   dfvar(ncid, "RAzi_angle", NC_FLOAT, RANK_RAzi_angle, dim2D, &RAzi_angle_id); 
   dfvar(ncid, "SZ_angle", NC_FLOAT, RANK_SZ_angle, dim2D, &SZ_angle_id);
   
   Player_dims[0] = P_Layer_dim;
   dfvar(ncid, "Player", NC_FLOAT, RANK_Player, Player_dims, &Player_id);

   Plevel_dims[0] = P_Level_dim;
   dfvar(ncid, "Plevel", NC_FLOAT, RANK_Plevel, Plevel_dims, &Plevel_id);

   dim3D[2] = P_Layer_dim;
   dfvar(ncid, "PTemp", NC_FLOAT, RANK_PTemp, dim3D, &PTemp_id);
   dfvar(ncid, "PVapor", NC_FLOAT, RANK_PVapor, dim3D, &PVapor_id);
   dfvar(ncid, "PClw", NC_FLOAT, RANK_PClw, dim3D, &PClw_id);
   dfvar(ncid, "PRain", NC_FLOAT, RANK_PRain, dim3D, &PRain_id);
   dfvar(ncid, "PGraupel", NC_FLOAT, RANK_PGraupel, dim3D, &PGraupel_id);
   dfvar(ncid, "PSnow", NC_FLOAT, RANK_PSnow, dim3D, &PSnow_id);
   dfvar(ncid, "PIce", NC_FLOAT, RANK_PIce, dim3D, &PIce_id);

   dfvar(ncid, "SurfP", NC_SHORT, RANK_SurfP, dim2D, &SurfP_id);

   dfvar(ncid, "quality_information",NC_CHAR,0,0, &quality_info_id);

   /* assign attributes of variables */
   
   put_vatt_txt(ncid, Freq_id, "long_name", "Central Frequency");
   put_vatt_txt(ncid, Freq_id, "units", "GHz");
   
   put_vatt_txt(ncid, Polo_id, "long_name", "Polarizations(Horizonal or Vertical)");
   put_vatt_txt(ncid, Polo_id, "units", "1");
   
   put_vatt_txt(ncid, ScanTime_year_id, "long_name", "Calendar Year 20XX");
   put_vatt_txt(ncid, ScanTime_year_id, "units", "years");
   put_vatt(ncid, ScanTime_year_id, "_FillValue", NC_SHORT, 1, fillValS);  
   ScanTime_year_range[0] = 2011;
   ScanTime_year_range[1] = 2050;
   put_vatt(ncid, ScanTime_year_id, "valid_range", NC_INT, 2, ScanTime_year_range);
   
   put_vatt_txt(ncid, ScanTime_doy_id, "long_name", "julian day 1-366");
   put_vatt_txt(ncid, ScanTime_doy_id, "units", "days");
   put_vatt(ncid, ScanTime_doy_id, "_FillValue", NC_SHORT, 1, fillValS); 
   ScanTime_doy_range[0] = 1;
   ScanTime_doy_range[1] = 366;
   put_vatt(ncid, ScanTime_doy_id, "valid_range", NC_INT, 2, ScanTime_doy_range);

   put_vatt_txt(ncid, ScanTime_month_id, "long_name", "Calendar month 1-12");
   put_vatt_txt(ncid, ScanTime_month_id, "units", "months");
   put_vatt(ncid, ScanTime_month_id, "_FillValue", NC_SHORT, 1, fillValS);
   ScanTime_month_range[0] = 1;
   ScanTime_month_range[1] = 12;
   put_vatt(ncid, ScanTime_month_id, "valid_range", NC_INT, 2, ScanTime_month_range);

   put_vatt_txt(ncid, ScanTime_dom_id, "long_name", "Calendar day of the month 1-31");
   put_vatt_txt(ncid, ScanTime_dom_id, "units", "days");
   put_vatt(ncid, ScanTime_dom_id, "_FillValue", NC_SHORT, 1, fillValS);
   ScanTime_dom_range[0] = 1;
   ScanTime_dom_range[1] = 31;
   put_vatt(ncid, ScanTime_dom_id, "valid_range", NC_INT, 2, ScanTime_dom_range);

   put_vatt_txt(ncid, ScanTime_hour_id, "long_name", "hour of the day 0-23");
   put_vatt_txt(ncid, ScanTime_hour_id, "units", "hours");
   put_vatt(ncid, ScanTime_hour_id, "_FillValue", NC_SHORT, 1, fillValS);
   ScanTime_hour_range[0] = 0;
   ScanTime_hour_range[1] = 23;
   put_vatt(ncid, ScanTime_hour_id, "valid_range", NC_INT, 2, ScanTime_hour_range);

   put_vatt_txt(ncid, ScanTime_minute_id, "long_name", "minute of the hour 0-59");
   put_vatt_txt(ncid, ScanTime_minute_id, "units", "minutes");
   put_vatt(ncid, ScanTime_minute_id, "_FillValue", NC_SHORT, 1, fillValS);
   ScanTime_minute_range[0] = 0;
   ScanTime_minute_range[1] = 59;
   put_vatt(ncid, ScanTime_minute_id, "valid_range", NC_INT, 2, ScanTime_minute_range);

   put_vatt_txt(ncid, ScanTime_second_id, "long_name", "second of the minute 0-59");
   put_vatt_txt(ncid, ScanTime_second_id, "units", "seconds");
   put_vatt(ncid, ScanTime_second_id, "_FillValue", NC_SHORT, 1, fillValS);
   ScanTime_second_range[0] = 0;
   ScanTime_second_range[1] = 59;
   put_vatt(ncid, ScanTime_second_id, "valid_range", NC_INT, 2, ScanTime_second_range);

   put_vatt_txt(ncid, ScanTime_UTC_id, "long_name", "Number of seconds since 00:00:00 UTC");
   put_vatt_txt(ncid, ScanTime_UTC_id, "units", "seconds");
   put_vatt(ncid, ScanTime_UTC_id, "_FillValue", NC_DOUBLE, 1, fillValD);
   ScanTime_UTC_range[0] = 0;
   ScanTime_UTC_range[1] = 86400;
   put_vatt(ncid, ScanTime_UTC_id, "valid_range", NC_DOUBLE, 2, ScanTime_UTC_range);

   put_vatt_txt(ncid, Orb_mode_id, "long_name", "Orbital Mode: 0-ascending,1-descending,2-transitioning");
   put_vatt_txt(ncid, Orb_mode_id, "units", "1");
   put_vatt(ncid, Orb_mode_id, "_FillValue", NC_SHORT, 1, fillValS);
   Orb_mode_range[0] = 0;
   Orb_mode_range[1] = 2;
   put_vatt(ncid, Orb_mode_id, "valid_range", NC_INT, 2, Orb_mode_range);

   put_vatt_txt(ncid, Latitude_id, "long_name", "Latitude of the view (-90,90)");
   put_vatt_txt(ncid, Latitude_id, "units", "degrees");
   put_vatt(ncid, Latitude_id, "_FillValue", NC_FLOAT, 1, fillValF);
   Latitude_range[0] = -90.;
   Latitude_range[1] = 90.;
   put_vatt(ncid, Latitude_id, "valid_range", NC_FLOAT, 2, Latitude_range);

   put_vatt_txt(ncid, Longitude_id, "long_name", "Longitude of the view (-180,180)");
   put_vatt_txt(ncid, Longitude_id, "units", "degrees");
   put_vatt(ncid, Longitude_id, "_FillValue", NC_FLOAT, 1, fillValF);
   Longitude_range[0] = -180.;
   Longitude_range[1] = 180.;
   put_vatt(ncid, Longitude_id, "valid_range", NC_FLOAT, 2, Longitude_range);

   put_vatt_txt(ncid, Sfc_type_id, "long_name", "Type of Surface:0-ocean,1-sea-ice,2-land,3-snow");
   put_vatt_txt(ncid, Sfc_type_id, "units", "1");
   put_vatt_txt(ncid, Sfc_type_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, Sfc_type_id, "_FillValue", NC_SHORT, 1, fillValS);
   Sfc_type_range[0] = 0;
   Sfc_type_range[1] = 3;
   put_vatt(ncid, Sfc_type_id, "valid_range", NC_INT, 2, Sfc_type_range);

   put_vatt_txt(ncid, Atm_type_id, "long_name", "Type of Atmosphere:0-simple scene, 1-retrieved scene");
   put_vatt_txt(ncid, Atm_type_id, "units", "1");
   put_vatt_txt(ncid, Atm_type_id, "coordinates", "Longitude Latitude"); 
   
   put_vatt_txt(ncid, Qc_id, "long_name", "Quality Flag: 0-good,1-usable with problem ,2-bad");
   put_vatt_txt(ncid, Qc_id, "units", "1");

   put_vatt_txt(ncid, ChiSqr_id, "long_name", "Convergence rate: <3-good,>10-bad");
   put_vatt_txt(ncid, ChiSqr_id, "units", "1");
   put_vatt_txt(ncid, ChiSqr_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, ChiSqr_id, "_FillValue", NC_FLOAT, 1, fillValF);
   ChiSqr_range[0] = 0.;
   ChiSqr_range[1] = 1000.;
   put_vatt(ncid, ChiSqr_id, "valid_range", NC_FLOAT, 2, ChiSqr_range);

   put_vatt_txt(ncid, LZ_angle_id, "long_name", "Local zenith angle: (-59,59) degree");
   put_vatt_txt(ncid, LZ_angle_id, "units", "degrees");
   put_vatt_txt(ncid, LZ_angle_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, LZ_angle_id, "_FillValue", NC_FLOAT, 1, fillValF);
   LZ_angle_range[0] = -70.;
   LZ_angle_range[1] = 70.;
   put_vatt(ncid, LZ_angle_id, "valid_range", NC_FLOAT, 2, LZ_angle_range);

   put_vatt_txt(ncid, RAzi_angle_id, "long_name", "Relative azimuth angle 0-360 degree");
   put_vatt_txt(ncid, RAzi_angle_id, "units", "degrees");
   put_vatt_txt(ncid, RAzi_angle_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, RAzi_angle_id, "_FillValue", NC_FLOAT, 1, fillValF);
   
   put_vatt_txt(ncid, SZ_angle_id, "long_name", "Solar zenith angle (-90,90) degree");
   put_vatt_txt(ncid, SZ_angle_id, "units", "degrees");
   put_vatt_txt(ncid, SZ_angle_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, SZ_angle_id, "_FillValue", NC_FLOAT, 1, fillValF);
   
   put_vatt_txt(ncid, Player_id, "long_name", "Pressure for each layer in mb");
   put_vatt_txt(ncid, Player_id, "units", "millibars");
//   put_vatt(ncid, Player_id, "_FillValue", NC_FLOAT, 1, fillValF);
   Player_range[0] = 0.011;
   Player_range[1] = 1085.458;
   put_vatt(ncid, Player_id, "valid_range", NC_FLOAT, 2, Player_range);
   
   put_vatt_txt(ncid, Plevel_id, "long_name", "Pressure for each level in mb");
   put_vatt_txt(ncid, Plevel_id, "units", "millibars");
 // put_vatt(ncid, Plevel_id, "_FillValue", NC_FLOAT, 1, fillValF);
   Plevel_range[0] = 0.005;
   Plevel_range[1] = 1100.;
   put_vatt(ncid, Plevel_id, "valid_range", NC_FLOAT, 2, Plevel_range);
  
   put_vatt_txt(ncid, PTemp_id, "long_name", "Temperature profile in K");
   put_vatt_txt(ncid, PTemp_id, "units", "Kelvin");
   put_vatt_txt(ncid, PTemp_id, "coordinates", "Longitude Latitude Player");  
   put_vatt(ncid, PTemp_id, "_FillValue", NC_FLOAT, 1, fillValF);
   PTemp_range[0] = 0.;
   PTemp_range[1] = 1000.;
   put_vatt(ncid, PTemp_id, "valid_range", NC_FLOAT, 2, PTemp_range);

   put_vatt_txt(ncid, PVapor_id, "long_name", "Water vapor profile in g/kg");
   put_vatt_txt(ncid, PVapor_id, "units", "g/kg");
   put_vatt_txt(ncid, PVapor_id, "coordinates", "Longitude Latitude Player");
   put_vatt(ncid, PVapor_id, "_FillValue", NC_FLOAT, 1, fillValF);
   PVapor_range[0] = 0.;
   PVapor_range[1] = 100.;
   put_vatt(ncid, PVapor_id, "valid_range", NC_FLOAT, 2, PVapor_range);

   put_vatt_txt(ncid, PClw_id, "long_name", "Cloud liquid water profile in mm");
   put_vatt_txt(ncid, PClw_id, "units", "mm");
   put_vatt_txt(ncid, PClw_id, "coordinates", "Longitude Latitude Player");
   put_vatt(ncid, PClw_id, "_FillValue", NC_FLOAT, 1, fillValF);
   PClw_range[0] = 0.;
   PClw_range[1] = 1.;
   put_vatt(ncid, PClw_id, "valid_range", NC_FLOAT, 2, PClw_range);

   put_vatt_txt(ncid, PRain_id, "long_name", "Rain mass profile in mm");
   put_vatt_txt(ncid, PRain_id, "units", "mm");
   put_vatt_txt(ncid, PRain_id, "coordinates", "Longitude Latitude Player");
   put_vatt(ncid, PRain_id, "_FillValue", NC_FLOAT, 1, fillValF);
   PRain_range[0] = 0.;
   PRain_range[1] = 1.;
   put_vatt(ncid, PRain_id, "valid_range", NC_FLOAT, 2, PRain_range);

   put_vatt_txt(ncid, PGraupel_id, "long_name", "Graupel mass profile in mm");
   put_vatt_txt(ncid, PGraupel_id, "units", "mm");
   put_vatt_txt(ncid, PGraupel_id, "coordinates", "Longitude Latitude Player");
   put_vatt(ncid, PGraupel_id, "_FillValue", NC_FLOAT, 1, fillValF);
   PGraupel_range[0] = 0.;
   PGraupel_range[1] = 1.;
   put_vatt(ncid, PGraupel_id, "valid_range", NC_FLOAT, 2, PGraupel_range);

   put_vatt_txt(ncid, PSnow_id, "long_name", "Snow mass profile in mm");
   put_vatt_txt(ncid, PSnow_id, "units", "mm");
   put_vatt_txt(ncid, PSnow_id, "coordinates", "Longitude Latitude Player");
   put_vatt(ncid, PSnow_id, "_FillValue", NC_FLOAT, 1, fillValF);
   PSnow_range[0] = 0.;
   PSnow_range[1] = 1.;
   put_vatt(ncid, PSnow_id, "valid_range", NC_FLOAT, 2, PSnow_range);

   put_vatt_txt(ncid, PIce_id, "long_name", "Ice mass profile in mm");
   put_vatt_txt(ncid, PIce_id, "units", "mm");
   put_vatt_txt(ncid, PIce_id, "coordinates", "Longitude Latitude Player");
   put_vatt(ncid, PIce_id, "_FillValue", NC_FLOAT, 1, fillValF);
   PIce_range[0] = 0.;
   PIce_range[1] = 1.;
   put_vatt(ncid, PIce_id, "valid_range", NC_FLOAT, 2, PIce_range);

   put_vatt_txt(ncid, SurfP_id, "long_name", "Surface pressure in mb");
   put_vatt_txt(ncid, SurfP_id, "units", "millibars");
   put_vatt_txt(ncid, SurfP_id, "coordinates", "Longitude Latitude");
   SurfP_scale[0] = 0.1;
   put_vatt(ncid, SurfP_id, "scale_factor", NC_DOUBLE, 1, SurfP_scale);
   put_vatt(ncid, SurfP_id, "_FillValue", NC_SHORT, 1, fillValS);
   SurfP_range[0] = 0.;
   SurfP_range[1] = 12000.;
   put_vatt(ncid, SurfP_id, "valid_range", NC_INT, 2, SurfP_range);

   put_vatt_txt(ncid, quality_info_id, "long_name", "total number retrievals, percentage optimal retrievals, percentage sub optimal retrievals, percentage bad retrievals");
   put_vatt_txt(ncid, quality_info_id, "units", "1");
   put_vatt(ncid, quality_info_id, "total_number_retrievals",NC_INT, 1, total_number_retrievals);
   put_vatt(ncid, quality_info_id, "percentage_optimal_retrievals", NC_FLOAT, 1,percentage_optimal_retrievals);
   put_vatt(ncid, quality_info_id, "percentage_sub_optimal_retrievals",NC_FLOAT, 1, percentage_suboptimal_retrievals);
   put_vatt(ncid, quality_info_id, "percentage_bad_retrievals", NC_FLOAT, 1,percentage_bad_retrievals);
   
   //Output global attributes

   //gnrt_attr_snd(ncid, satid, date_created_snd);

   // leave define mode 
   stat = nc_enddef (ncid);
   check_err(stat,__LINE__,__FILE__,errid);
   
   stat = nc_close(ncid);
   check_err(stat,__LINE__,__FILE__,errid);
   return 0;
}

void mirs2nc_class::gnrt_attr_snd( int ncid, char* satid, char* date_created_snd, char* nc_name_snd )
{
  char uuid[1024];
  char hostname[256];
  char char_random_number[17];
  unsigned int iseed, random_number;
  int cdf_missing_value[1];
  int cdf_notretrievedproduct_value[1];
  int cdf_noretrieval_value[1];
  double cdf_version[1];
  int alg_version[1];
  char geospatial_bounds_str[86];

  const char* project_str;
  const char* naming_authority_str;
  const char* publisher_name_str;
  const char* keyword_str;

  memset(geospatial_bounds_str,'\0',86);

/*
// NCCF or NDE strings 
  if ( (strcmp(satid,"NPP")==0) or (strcmp(satid,"N20")==0) or (strcmp(satid,"N21")==0) ) { //NDE projects: NPP,N20,N21
        project_str           = project_nde_str;
        naming_authority_str  = naming_authority_nde_str;
        publisher_name_str    = publisher_name_nde_str;
  }
  else if ( (strcmp(satid,"M1")==0) or (strcmp(satid,"M2")==0) or (strcmp(satid,"M3")==0) or (strcmp(satid,"GPM")==0) ) { //NCCF projects: MetopB/C,GPM
     // M1: metopB; M2: metopA; M3: metopC
        project_str           = project_nccf_str;
        naming_authority_str  = naming_authority_nccf_str;
        publisher_name_str    = publisher_name_nccf_str;
  }
  else if ( (strcmp(satid,"NN")==0) or (strcmp(satid,"NP")==0) or (strcmp(satid,"SB")==0) or (strcmp(satid,"SC")==0) ) { 
     //NN: N18; NP: N19; SB: F17; SC: F18
        project_str           = project_nccf_str;
        naming_authority_str  = naming_authority_nccf_str;
        publisher_name_str    = publisher_name_nccf_str;
  }
  else {
        project_str           = project_common_str;
        naming_authority_str  = naming_authority_common_str;
        publisher_name_str    = publisher_name_common_str;
  }
*/
// use nccf only 2022-09-12
   project_str           = project_nccf_str;
   naming_authority_str  = naming_authority_nccf_str;
   publisher_name_str    = publisher_name_nccf_str;

//keyword strings
  if ( (strcmp(satid,"NPP")==0) or (strcmp(satid,"N20")==0) or (strcmp(satid,"N21")==0) ) { 
        keyword_str           = keyword_atms_str;
  }
  else if ( (strcmp(satid,"M1")==0 ) or (strcmp(satid,"M3")==0 ) or (strcmp(satid,"M2")==0)  \
         or (strcmp(satid,"NN")==0) or (strcmp(satid,"NP")==0) ) {
        keyword_str           = keyword_amsua_str;
  }
  else if ( strcmp(satid,"GPM")==0)  {
        keyword_str           = keyword_gmi_str;
  }
  else if ( strcmp(satid,"SB")==0 or (strcmp(satid,"SC")==0) )  {
        keyword_str           = keyword_ssmis_str;
  }
  else {
        keyword_str           = keyword_common_str;
  }
  
  //Generate global attribute
  cdf_missing_value[0] = -999;
  put_gatt(ncid, "missing_value", NC_INT, 1, cdf_missing_value);

  cdf_notretrievedproduct_value[0] = NAVA;
  put_gatt(ncid, "notretrievedproduct_value", NC_INT, 1, cdf_notretrievedproduct_value);

  cdf_noretrieval_value[0] = -99;
  put_gatt(ncid, "noretrieval_value", NC_INT, 1, cdf_noretrieval_value);

  cdf_version[0] = 4;
  put_gatt(ncid, "cdf_version", NC_DOUBLE, 1, cdf_version);

  alg_version[0] = alg_sn;
  put_gatt(ncid, "alg_version", NC_INT, 1, alg_version);

  put_gatt_txt(ncid, "dap_version", dap_ver_str);

  put_gatt_txt(ncid, "history_package", hist_pack_str);
  put_gatt_txt(ncid, "keywords", keyword_str);
  put_gatt(ncid, "ascend_descend_data_flag", NC_INT, 1, mode_flag_value);
  
    put_gatt_txt(ncid, "Conventions", Conventions_str);
    put_gatt_txt(ncid, "Metadata_Conventions", Metadata_Conventions_str);
    put_gatt_txt(ncid, "standard_name_vocabulary", standard_name_vocabulary_str);
    put_gatt_txt(ncid, "project", project_str);
    put_gatt_txt(ncid, "title", title_snd_str);
    put_gatt_txt(ncid, "summary", summary_snd_str);
    put_gatt_txt(ncid, "date_created", date_created_snd);
    put_gatt_txt(ncid, "institution", institution_str);
    put_gatt_txt(ncid, "naming_authority", naming_authority_str);
    put_gatt_txt(ncid, "production_site", production_site_str);
    put_gatt_txt(ncid, "production_environment", production_environment_str);

  if( strcmp( satid, "NPP" ) == 0 ) {
    put_gatt_txt(ncid, "platform", NPP_platform_str);
    put_gatt_txt(ncid, "instrument", NPP_instrument_str);
  }
  else if( strcmp( satid, "N20" ) == 0 ) {
    put_gatt_txt(ncid, "platform", N20_platform_str);
    put_gatt_txt(ncid, "instrument", N20_instrument_str);
  }
  else if( strcmp( satid, "N21" ) == 0 ) {
    put_gatt_txt(ncid, "platform", N21_platform_str);
    put_gatt_txt(ncid, "instrument", N21_instrument_str);
  }
  else if( strcmp( satid, "metopSGA1" ) == 0 ) {
    put_gatt_txt(ncid, "platform", metopSGA1_platform_str);
    put_gatt_txt(ncid, "instrument", metopSGA1_instrument_str);
  }
  else if( strcmp( satid, "M2" ) == 0 ) {
    put_gatt_txt(ncid, "platform", METOPA_platform_str);
    put_gatt_txt(ncid, "instrument", METOPA_instrument_str);
  }
  else if( strcmp( satid, "M1" ) == 0 ) {
    put_gatt_txt(ncid, "platform", METOPB_platform_str);
    put_gatt_txt(ncid, "instrument", METOPB_instrument_str);
  }
  else if( strcmp( satid, "M3" ) == 0 ) {
    put_gatt_txt(ncid, "platform", METOPC_platform_str);
    put_gatt_txt(ncid, "instrument", METOPC_instrument_str);
  }
  else if( strcmp( satid, "GPM" ) == 0 ) {
    put_gatt_txt(ncid, "platform", GPM_platform_str);
    put_gatt_txt(ncid, "instrument", GPM_instrument_str);
  }

    put_gatt_txt(ncid, "creator_name", creator_name_str);
    put_gatt_txt(ncid, "creator_email", creator_email_str);
    put_gatt_txt(ncid, "creator_url",creator_url_str);
    put_gatt_txt(ncid, "publisher_name", publisher_name_str);
    put_gatt_txt(ncid, "publisher_email", publisher_email_str);
    put_gatt_txt(ncid, "publisher_url", publisher_url_str);

    put_gatt_txt(ncid, "metadata_link", foname_snd);

    put_gatt_txt(ncid, "references", references_str);
    put_gatt_txt(ncid, "history", mirs_ver_str);
    put_gatt_txt(ncid, "processing_level", processing_level_str);
    put_gatt_txt(ncid, "source", RDR_name);
    put_gatt_txt(ncid, "time_coverage_start", time_coverage_start);
    put_gatt_txt(ncid, "time_coverage_end", time_coverage_end);
    put_gatt_txt(ncid, "cdm_data_type", cdm_data_type_str);
    put_gatt_txt(ncid, "geospatial_lat_units", geospatial_lat_units_str);
    put_gatt_txt(ncid, "geospatial_lon_units", geospatial_lon_units_str);

    //geospatial_bounds =  “POLYGON((lon1 lat1, lon2 lat2,…,lonN latN, lon1 lat1)), The first point is repeated at the end to close the loop.  
    sprintf(geospatial_bounds_str,"POLYGON((%7.2f %6.2f,%7.2f %6.2f,%7.2f %6.2f,%7.2f %6.2f,%7.2f %6.2f))"
            , geospatial_first_scanline_first_fov_lon[0]
            , geospatial_first_scanline_first_fov_lat[0]
            , geospatial_first_scanline_last_fov_lon[0]
            , geospatial_first_scanline_last_fov_lat[0]
            , geospatial_last_scanline_last_fov_lon[0]
            , geospatial_last_scanline_last_fov_lat[0]
            , geospatial_last_scanline_first_fov_lon[0]
            , geospatial_last_scanline_first_fov_lat[0]
            , geospatial_first_scanline_first_fov_lon[0]
            , geospatial_first_scanline_first_fov_lat[0]
          );

    put_gatt_txt(ncid, "geospatial_bounds", geospatial_bounds_str);
    put_gatt(ncid, "geospatial_first_scanline_first_fov_lat", NC_FLOAT, 1, geospatial_first_scanline_first_fov_lat);
    put_gatt(ncid, "geospatial_first_scanline_first_fov_lon", NC_FLOAT, 1, geospatial_first_scanline_first_fov_lon);
    put_gatt(ncid, "geospatial_first_scanline_last_fov_lat", NC_FLOAT, 1, geospatial_first_scanline_last_fov_lat);
    put_gatt(ncid, "geospatial_first_scanline_last_fov_lon", NC_FLOAT, 1, geospatial_first_scanline_last_fov_lon);
    put_gatt(ncid, "geospatial_last_scanline_first_fov_lat", NC_FLOAT, 1, geospatial_last_scanline_first_fov_lat);
    put_gatt(ncid, "geospatial_last_scanline_first_fov_lon", NC_FLOAT, 1, geospatial_last_scanline_first_fov_lon);
    put_gatt(ncid, "geospatial_last_scanline_last_fov_lat", NC_FLOAT, 1, geospatial_last_scanline_last_fov_lat);
    put_gatt(ncid, "geospatial_last_scanline_last_fov_lon", NC_FLOAT, 1, geospatial_last_scanline_last_fov_lon);

    put_gatt(ncid, "quality_information_total_number_retrievals", NC_INT, 1, total_number_retrievals);
    put_gatt(ncid, "quality_information_percentage_optimal_retrievals", NC_FLOAT, 1, percentage_optimal_retrievals);
    put_gatt(ncid, "quality_information_percentage_suboptimal_retrievals", NC_FLOAT, 1, percentage_suboptimal_retrievals);
    put_gatt(ncid, "quality_information_percentage_bad_retrievals", NC_FLOAT, 1, percentage_bad_retrievals);

    put_gatt(ncid, "start_orbit_number", NC_INT, 1, start_orbit_number);
    put_gatt(ncid, "end_orbit_number", NC_INT, 1, end_orbit_number);
    
    // generate a unique ID for this granule: hostname + date_creation_time + random number + RDR filename
    
    hostname[255] = '\0';
    gethostname(hostname, 254);
    iseed = (unsigned int)time(NULL);
    srand(iseed);
    random_number = rand();
    sprintf(char_random_number,"%016u",random_number);
    
    strcpy(uuid,hostname);
    strcat(uuid,"_");
    strcat(uuid,date_created_snd);
    strcat(uuid,"_");
    strcat(uuid,char_random_number);
    strcat(uuid,"_");
    strcat(uuid,RDR_name);
    
    put_gatt_txt(ncid, "id", uuid);
  
}


