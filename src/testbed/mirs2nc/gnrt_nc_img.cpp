/***************************************************************************
 *  Program Name      : gnrt_nc_img.cpp
 *  Type              : Subroutine
 *  Function          : Program creates netcdf image file 
 *  Input Files       : 
 *  Output Files      : Netcdf files
 *  Subroutine Called : 
 *  Called by         : set_nc_img.cpp
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *  
 *  12/2008         v0              Jiang Zhao
 *  07/2011         v1              Wanchun Chen
 *  12/2016         v1.1            Junye Chen
 *  02/2020         v2              Ming Fang
 *                                  Added gnrt_attr_img, modified gnrt_nc_img
 *************************************************************************/
#include "constants.h"
#include "swath.h"
#include <netcdf.h>
#include "common_header.h"
#include <time.h>
#include <unistd.h>
#include <string>

int mirs2nc_class::gnrt_nc_img(char * fname_nc_img, short int nscan, short int nspot, short int  nchan, char *satid)
{

   int  stat;  /* return status */
   int  ncid;  /* netCDF id */
   int  errid = 17;

   /* dimension ids */
   int Scanline_dim;
   int Field_of_view_dim;
   int P_Layer_dim;
   int Channel_dim;
   int Qc_dim_dim;

   /* dimension lengths */
   size_t Scanline_len = nscan;
   size_t Field_of_view_len = nspot;
   size_t P_Layer_len = 100;
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
   int BT_id;
   int YM_id;
   int ChanSel_id;
   int TPW_id;
   int CLW_id;
   int RWP_id;
   int LWP_id;
   int SWP_id;
   int IWP_id;
   int GWP_id;
   int RR_id;
   int Snow_id;
   int SWE_id;
   int SnowGS_id;
   int SIce_id;
   int SIce_MY_id;
   int SIce_FY_id;
   int TSkin_id;
   int SurfP_id;
   int Emis_id;
   int Freq_id;
   int Polo_id;

   int SFR_id;
   int CldTop_id;
   int CldBase_id;
   int CldThick_id;
   int PrecipType_id;
   int RFlag_id;

   int SurfM_id;
   int WindSp_id;
   int WindDir_id;
   int WindU_id;
   int WindV_id;
   int Prob_SF_id;
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
#  define RANK_ChiSqr 2
#  define RANK_LZ_angle 2
#  define RANK_RAzi_angle 2
#  define RANK_SZ_angle 2
#  define RANK_BT 3
#  define RANK_YM 3
#  define RANK_YFWD 3
#  define RANK_ChanSel 3
#  define RANK_TPW 2
#  define RANK_CLW 2
#  define RANK_RWP 2
#  define RANK_LWP 2
#  define RANK_SWP 2
#  define RANK_IWP 2
#  define RANK_GWP 2
#  define RANK_RR 2
#  define RANK_Snow 2
#  define RANK_SWE 2
#  define RANK_SnowGS 2
#  define RANK_SIce 2
#  define RANK_SIce_MY 2
#  define RANK_SIce_FY 2
#  define RANK_TSkin 2
#  define RANK_SurfP 2
#  define RANK_Emis 3
#  define RANK_Freq 1
#  define RANK_Polo 1
#  define RANK_SFR 2
#  define RANK_CldTop 2
#  define RANK_CldBase 2
#  define RANK_CldThick 2
#  define RANK_PrecipType 2
#  define RANK_RFlag 2
#  define RANK_SurfM 2
#  define RANK_WindSp 2
#  define RANK_WindDir 2
#  define RANK_WindU 2
#  define RANK_WindV 2
#  define RANK_Prob_SF 2
#  define RANK_Prob_RF 2

   /* attribute vectors */
   double BT_scale[1];
   double YM_scale[1];
   double TPW_scale[1];
   double CLW_scale[1];
   double RWP_scale[1];
   double LWP_scale[1];
   double SWP_scale[1];
   double IWP_scale[1];
   double GWP_scale[1];
   double RR_scale[1];
   double SWE_scale[1];
   double SnowGS_scale[1];
   double TSkin_scale[1];
   double SurfP_scale[1];
   double Emis_scale[1];
   
   double SFR_scale[1];
   double CldTop_scale[1];
   double CldBase_scale[1];
   double CldThick_scale[1];
   double SurfM_scale[1];
   double WindSp_scale[1];
   double WindDir_scale[1];
   double WindU_scale[1];
   double WindV_scale[1];
   short fillVal[1];
   float fillValF[1];
   double fillValD[1];

   float LZ_angle_range[2];
   float Latitude_range[2];
   float Longitude_range[2];
   float ChiSqr_range[2];
   int YM_range[2];
   int ChanSel_range[2];
   int BT_range[2];
   float CLW_range[2];
   float LWP_range[2];
   float RWP_range[2];
   float GWP_range[2];
   int IWP_range[2];
   int SWP_range[2];
   float RR_range[2];
   int SFR_range[2];
   int TPW_range[2];
   int Emis_range[2];
   float TSkin_range[2];
   int SWE_range[2];
   int SnowGS_range[2];
   int SIce_range[2];
   int SIce_FY_range[2];
   int SIce_MY_range[2];
   int Snow_range[2];
   int Orb_mode_range[2];
   int Sfc_type_range[2];
   int SurfP_range[2];
   int Prob_SF_range[2];
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
   char date_created_img[21];
  

   fillVal[0] = -999;
   fillValF[0] = -999;
   fillValD[0] = -999;

   /* enter define mode */
   stat = nc_create(fname_nc_img, NC_CLOBBER|NC_NETCDF4, &ncid);
   check_err(stat,__LINE__,__FILE__,errid);
   
   /* creation time */
   time ( &rawtime );
   timeinfo = gmtime ( &rawtime );
   sprintf(date_created_img,"%4d-%02d-%02dT%02d:%02d:%02dZ",
     timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);


   /* define dimensions */

   dfdim(ncid, "Scanline", Scanline_len, &Scanline_dim);
   dfdim(ncid, "Field_of_view", Field_of_view_len, &Field_of_view_dim);
   dfdim(ncid, "P_Layer", P_Layer_len, &P_Layer_dim);
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
   dfvar(ncid, "RAzi_angle", NC_FLOAT, RANK_RAzi_angle, dim2D, &RAzi_angle_id); 
   dfvar(ncid, "SZ_angle", NC_FLOAT, RANK_SZ_angle, dim2D, &SZ_angle_id);
   
   dim3D[2] = Channel_dim;
   dfvar(ncid, "BT", NC_SHORT, RANK_BT, dim3D, &BT_id);
   dfvar(ncid, "YM", NC_SHORT, RANK_YM, dim3D, &YM_id);
   dfvar(ncid, "ChanSel", NC_SHORT, RANK_ChanSel, dim3D, &ChanSel_id);
   
   dfvar(ncid, "TPW", NC_SHORT, RANK_TPW, dim2D, &TPW_id);
   dfvar(ncid, "CLW", NC_FLOAT, RANK_CLW, dim2D, &CLW_id);
   dfvar(ncid, "RWP", NC_FLOAT, RANK_RWP, dim2D, &RWP_id);
   dfvar(ncid, "LWP", NC_FLOAT, RANK_LWP, dim2D, &LWP_id);
   dfvar(ncid, "SWP", NC_SHORT, RANK_SWP, dim2D, &SWP_id);
   dfvar(ncid, "IWP", NC_SHORT, RANK_IWP, dim2D, &IWP_id);
   dfvar(ncid, "GWP", NC_FLOAT, RANK_GWP, dim2D, &GWP_id);
   dfvar(ncid, "RR",  NC_FLOAT, RANK_RR,  dim2D, &RR_id);
   dfvar(ncid, "Snow", NC_SHORT, RANK_Snow, dim2D, &Snow_id);
   dfvar(ncid, "SWE", NC_SHORT, RANK_SWE, dim2D, &SWE_id);
   dfvar(ncid, "SnowGS", NC_SHORT, RANK_SnowGS, dim2D, &SnowGS_id);
   dfvar(ncid, "SIce", NC_SHORT, RANK_SIce, dim2D, &SIce_id);
   dfvar(ncid, "SIce_MY", NC_SHORT, RANK_SIce_MY, dim2D, &SIce_MY_id);
   dfvar(ncid, "SIce_FY", NC_SHORT, RANK_SIce_FY, dim2D, &SIce_FY_id);
   dfvar(ncid, "TSkin", NC_FLOAT, RANK_TSkin, dim2D, &TSkin_id);
   dfvar(ncid, "SurfP", NC_SHORT, RANK_SurfP, dim2D, &SurfP_id);

   dfvar(ncid, "Emis", NC_SHORT, RANK_Emis, dim3D, &Emis_id);

   dfvar(ncid, "SFR", NC_SHORT, RANK_SFR, dim2D, &SFR_id);
   dfvar(ncid, "CldTop", NC_SHORT, RANK_CldTop, dim2D, &CldTop_id);
   dfvar(ncid, "CldBase", NC_SHORT, RANK_CldBase, dim2D, &CldBase_id);
   dfvar(ncid, "CldThick", NC_SHORT, RANK_CldThick, dim2D, &CldThick_id);
   dfvar(ncid, "PrecipType", NC_SHORT, RANK_PrecipType, dim2D, &PrecipType_id);
   dfvar(ncid, "RFlag", NC_SHORT, RANK_RFlag, dim2D, &RFlag_id);
   dfvar(ncid, "SurfM", NC_SHORT, RANK_SurfM, dim2D, &SurfM_id);
   dfvar(ncid, "WindSp", NC_SHORT, RANK_WindSp, dim2D, &WindSp_id);
   dfvar(ncid, "WindDir", NC_SHORT, RANK_WindDir, dim2D, &WindDir_id);
   dfvar(ncid, "WindU", NC_SHORT, RANK_WindU, dim2D, &WindU_id);
   dfvar(ncid, "WindV", NC_SHORT, RANK_WindV, dim2D, &WindV_id);
   dfvar(ncid, "Prob_SF", NC_SHORT, RANK_Prob_SF, dim2D, &Prob_SF_id);

   dfvar(ncid, "quality_information",NC_CHAR,0,0, &quality_info_id);

   /* assign attributes */

   put_vatt_txt(ncid, ScanTime_year_id, "long_name", "Calendar Year 20XX");
   put_vatt_txt(ncid, ScanTime_year_id, "units", "years");
   put_vatt(ncid, ScanTime_year_id, "_FillValue", NC_SHORT, 1, fillVal);
   ScanTime_year_range[0] = 2011;
   ScanTime_year_range[1] = 2050;
   put_vatt(ncid, ScanTime_year_id, "valid_range", NC_INT, 2, ScanTime_year_range);
   
   put_vatt_txt(ncid, ScanTime_doy_id, "long_name", "julian day 1-366");
   put_vatt_txt(ncid, ScanTime_doy_id, "units", "days");
   put_vatt(ncid, ScanTime_doy_id, "_FillValue", NC_SHORT, 1, fillVal);
   ScanTime_doy_range[0] = 1;
   ScanTime_doy_range[1] = 366;
   put_vatt(ncid, ScanTime_doy_id, "valid_range", NC_INT, 2, ScanTime_doy_range);
   
   put_vatt_txt(ncid, ScanTime_month_id, "long_name", "Calendar month 1-12");
   put_vatt_txt(ncid, ScanTime_month_id, "units", "months");
   put_vatt(ncid, ScanTime_month_id, "_FillValue", NC_SHORT, 1, fillVal);
   ScanTime_month_range[0] = 1;
   ScanTime_month_range[1] = 12;
   put_vatt(ncid, ScanTime_month_id, "valid_range", NC_INT, 2, ScanTime_month_range);
   
   put_vatt_txt(ncid, ScanTime_dom_id, "long_name", "Calendar day of the month 1-31");
   put_vatt_txt(ncid, ScanTime_dom_id, "units", "days");
   put_vatt(ncid, ScanTime_dom_id, "_FillValue", NC_SHORT, 1, fillVal);
   ScanTime_dom_range[0] = 1;
   ScanTime_dom_range[1] = 31;
   put_vatt(ncid, ScanTime_dom_id, "valid_range", NC_INT, 2, ScanTime_dom_range);
   
   put_vatt_txt(ncid, ScanTime_hour_id, "long_name", "hour of the day 0-23");
   put_vatt_txt(ncid, ScanTime_hour_id, "units", "hours");
   put_vatt(ncid, ScanTime_hour_id, "_FillValue", NC_SHORT, 1, fillVal);
   ScanTime_hour_range[0] = 0;
   ScanTime_hour_range[1] = 23;
   put_vatt(ncid, ScanTime_hour_id, "valid_range", NC_INT, 2, ScanTime_hour_range);
   
   put_vatt_txt(ncid, ScanTime_minute_id, "long_name", "minute of the hour 0-59");
   put_vatt_txt(ncid, ScanTime_minute_id, "units", "minutes");
   put_vatt(ncid, ScanTime_minute_id, "_FillValue", NC_SHORT, 1, fillVal);
   ScanTime_minute_range[0] = 0;
   ScanTime_minute_range[1] = 59;
   put_vatt(ncid, ScanTime_minute_id, "valid_range", NC_INT, 2, ScanTime_minute_range);
   
   put_vatt_txt(ncid, ScanTime_second_id, "long_name", "second of the minute 0-59");
   put_vatt_txt(ncid, ScanTime_second_id, "units", "seconds");
   put_vatt(ncid, ScanTime_second_id, "_FillValue", NC_SHORT, 1, fillVal);
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
   put_vatt(ncid, Orb_mode_id, "_FillValue", NC_SHORT, 1, fillVal);
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
   
   put_vatt_txt(ncid, Sfc_type_id, "long_name", "Type of Surface:0-ocean,1-sea ice,2-land,3-snow");
   put_vatt_txt(ncid, Sfc_type_id, "units", "1");
   put_vatt_txt(ncid, Sfc_type_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, Sfc_type_id, "_FillValue", NC_SHORT, 1, fillVal);
   Sfc_type_range[0] = 0;
   Sfc_type_range[1] = 3;
   put_vatt(ncid, Sfc_type_id, "valid_range", NC_INT, 2, Sfc_type_range);
   
   put_vatt_txt(ncid, Atm_type_id, "long_name", "Type of Atmosphere:currently missing");
   put_vatt_txt(ncid, Atm_type_id, "units", "1");
   put_vatt_txt(ncid, Atm_type_id, "coordinates", "Longitude Latitude"); 
   
   put_vatt_txt(ncid, Qc_id, "long_name", "Quality Flag: 0-good, 1-usable with problem, 2-bad");
   put_vatt_txt(ncid, Qc_id, "units", "1");

   put_vatt_txt(ncid, ChiSqr_id, "long_name", "Convergence rate: <3-good,>10-bad");
   put_vatt_txt(ncid, ChiSqr_id, "units", "1");
   put_vatt_txt(ncid, ChiSqr_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, ChiSqr_id, "_FillValue", NC_FLOAT, 1, fillValF);
   ChiSqr_range[0] = 0.;
   ChiSqr_range[1] = 1000.;
   put_vatt(ncid, ChiSqr_id, "valid_range", NC_FLOAT, 2, ChiSqr_range);

   put_vatt_txt(ncid, Freq_id, "long_name", "Central Frequency");
   put_vatt_txt(ncid, Freq_id, "units", "GHz");
   
   put_vatt_txt(ncid, Polo_id, "long_name", "Polarizations(Horizonal or Vertical)");
   put_vatt_txt(ncid, Polo_id, "units", "1");
  
   put_vatt_txt(ncid, LZ_angle_id, "long_name", "Local Zenith Angle degree");
   put_vatt_txt(ncid, LZ_angle_id, "units", "degrees");
   put_vatt_txt(ncid, LZ_angle_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, LZ_angle_id, "_FillValue", NC_FLOAT, 1, fillValF);
   LZ_angle_range[0] = -70.;
   LZ_angle_range[1] = 70.;
   put_vatt(ncid, LZ_angle_id, "valid_range", NC_FLOAT, 2, LZ_angle_range);

   put_vatt_txt(ncid, RAzi_angle_id, "long_name", "Relative Azimuth Angle 0-360 degree");
   put_vatt_txt(ncid, RAzi_angle_id, "units", "degrees");
   put_vatt_txt(ncid, RAzi_angle_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, RAzi_angle_id, "_FillValue", NC_FLOAT, 1, fillValF);
   
   put_vatt_txt(ncid, SZ_angle_id, "long_name", "Solar Zenith Angle (-90,90) degree");
   put_vatt_txt(ncid, SZ_angle_id, "units", "degrees");
   put_vatt_txt(ncid, SZ_angle_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, SZ_angle_id, "_FillValue", NC_FLOAT, 1, fillValF);
   
   put_vatt_txt(ncid, BT_id, "long_name", "Channel Temperature (K)");
   put_vatt_txt(ncid, BT_id, "units", "Kelvin");
   put_vatt_txt(ncid, BT_id, "coordinates", "Longitude Latitude Freq");
   BT_scale[0] = {BT_SCALE};
   put_vatt(ncid, BT_id, "scale_factor", NC_DOUBLE, 1, BT_scale);
   put_vatt(ncid, BT_id, "_FillValue", NC_SHORT, 1, fillVal);
   BT_range[0] = 0;
   BT_range[1] = 50000;
   put_vatt(ncid, BT_id, "valid_range", NC_INT, 2, BT_range);
   
   put_vatt_txt(ncid, YM_id, "long_name", "Un-Corrected Channel Temperature (K)");
   put_vatt_txt(ncid, YM_id, "units", "Kelvin");
   put_vatt_txt(ncid, YM_id, "coordinates", "Longitude Latitude Freq");
   YM_scale[0] = {BT_SCALE};
   put_vatt(ncid, YM_id, "scale_factor", NC_DOUBLE, 1, YM_scale);
   put_vatt(ncid, YM_id, "_FillValue", NC_SHORT, 1, fillVal);
   YM_range[0] = 0;
   YM_range[1] = 32767;
   put_vatt(ncid, YM_id, "valid_range", NC_INT, 2, YM_range);

   put_vatt_txt(ncid, ChanSel_id, "long_name", "Channels Selection Used in Retrieval");
   put_vatt_txt(ncid, ChanSel_id, "units", "1");
   put_vatt_txt(ncid, ChanSel_id, "coordinates", "Longitude Latitude Freq");
   put_vatt(ncid, ChanSel_id, "_FillValue", NC_SHORT, 1, fillVal);
   ChanSel_range[0] = 0;
   ChanSel_range[1] = 1;
   put_vatt(ncid, ChanSel_id, "valid_range", NC_INT, 2, ChanSel_range);
   
   put_vatt_txt(ncid, TPW_id, "long_name", "Total Precipitable Water (mm)");
   put_vatt_txt(ncid, TPW_id, "units", "mm");
   put_vatt_txt(ncid, TPW_id, "coordinates", "Longitude Latitude");
   TPW_scale[0] = {TPW_SCAL};
   put_vatt(ncid, TPW_id, "scale_factor", NC_DOUBLE, 1, TPW_scale);
   put_vatt(ncid, TPW_id, "_FillValue", NC_SHORT, 1, fillVal);
   TPW_range[0] = 0;
   TPW_range[1] = 2000;
   put_vatt(ncid, TPW_id, "valid_range", NC_INT, 2, TPW_range);
   
   put_vatt_txt(ncid, CLW_id, "long_name", "Cloud liquid Water (mm)");
   put_vatt_txt(ncid, CLW_id, "units", "mm");
   put_vatt_txt(ncid, CLW_id, "coordinates", "Longitude Latitude");
   CLW_scale[0] = {CLW_SCAL};
   //put_vatt(ncid, CLW_id, "scale_factor", NC_DOUBLE, 1, CLW_scale);
   put_vatt(ncid, CLW_id, "_FillValue", NC_FLOAT, 1, fillValF);
   CLW_range[0] = 0.;
   CLW_range[1] = 10.;
   put_vatt(ncid, CLW_id, "valid_range", NC_FLOAT, 2, CLW_range);
   
   put_vatt_txt(ncid, RWP_id, "long_name", "Rain Water Path (mm)");
   put_vatt_txt(ncid, RWP_id, "units", "mm");
   put_vatt_txt(ncid, RWP_id, "coordinates", "Longitude Latitude");
   RWP_scale[0] = {RWP_SCAL};
   //put_vatt(ncid, RWP_id, "scale_factor", NC_DOUBLE, 1, RWP_scale);
   put_vatt(ncid, RWP_id, "_FillValue", NC_FLOAT, 1, fillValF);
   RWP_range[0] = 0.;
   RWP_range[1] = 10.;
   put_vatt(ncid, RWP_id, "valid_range", NC_FLOAT, 2, RWP_range);
   
   put_vatt_txt(ncid, LWP_id, "long_name", "Liquid Water Path (mm)");
   put_vatt_txt(ncid, LWP_id, "units", "mm");
   put_vatt_txt(ncid, LWP_id, "coordinates", "Longitude Latitude");
   LWP_scale[0] = {LWP_SCAL};
   //put_vatt(ncid, LWP_id, "scale_factor", NC_DOUBLE, 1, LWP_scale);
   put_vatt(ncid, LWP_id, "_FillValue", NC_FLOAT, 1, fillValF);
   LWP_range[0] = 0;
   LWP_range[1] = 10.;
   put_vatt(ncid, LWP_id, "valid_range", NC_FLOAT, 2, LWP_range);
   
   put_vatt_txt(ncid, SWP_id, "long_name", "Snow Water Path");
   put_vatt_txt(ncid, SWP_id, "units", "mm");
   put_vatt_txt(ncid, SWP_id, "coordinates", "Longitude Latitude");
   SWP_scale[0] = {SWP_SCAL};
   put_vatt(ncid, SWP_id, "scale_factor", NC_DOUBLE, 1, SWP_scale);
   put_vatt(ncid, SWP_id, "_FillValue", NC_SHORT, 1, fillVal);
   SWP_range[0] = 0;
   SWP_range[1] = 10000;
   put_vatt(ncid, SWP_id, "valid_range", NC_INT, 2, SWP_range);
   
   put_vatt_txt(ncid, IWP_id, "long_name", "Ice Water Path (mm)");
   put_vatt_txt(ncid, IWP_id, "units", "mm");
   put_vatt_txt(ncid, IWP_id, "coordinates", "Longitude Latitude");
   IWP_scale[0] = {IWP_SCAL};
   put_vatt(ncid, IWP_id, "scale_factor", NC_DOUBLE, 1, IWP_scale);
   put_vatt(ncid, IWP_id, "_FillValue", NC_SHORT, 1, fillVal);
   IWP_range[0] = 0;
   IWP_range[1] = 10000;
   put_vatt(ncid, IWP_id, "valid_range", NC_INT, 2, IWP_range);
   
   put_vatt_txt(ncid, GWP_id, "long_name", "Graupel Water Path (mm)");
   put_vatt_txt(ncid, GWP_id, "units", "mm");
   put_vatt_txt(ncid, GWP_id, "coordinates", "Longitude Latitude");
   GWP_scale[0] = {GWP_SCAL};
   //put_vatt(ncid, GWP_id, "scale_factor", NC_DOUBLE, 1, GWP_scale);
   put_vatt(ncid, GWP_id, "_FillValue", NC_FLOAT, 1, fillValF);
   GWP_range[0] = 0;
   GWP_range[1] = 10.;
   put_vatt(ncid, GWP_id, "valid_range", NC_FLOAT, 2, GWP_range);

   put_vatt_txt(ncid, RR_id, "long_name", "Rain Rate (mm/hr)");
   put_vatt_txt(ncid, RR_id, "units", "mm/hr");
   put_vatt_txt(ncid, RR_id, "coordinates", "Longitude Latitude");
   RR_scale[0] = {RR_SCAL};
   //put_vatt(ncid, RR_id, "scale_factor", NC_DOUBLE, 1, RR_scale);
   put_vatt(ncid, RR_id, "_FillValue", NC_FLOAT, 1, fillValF);
   RR_range[0] = 0.;
   RR_range[1] = 100.;
   put_vatt(ncid, RR_id, "valid_range", NC_FLOAT, 2, RR_range);

   put_vatt_txt(ncid, Snow_id, "long_name", "Snow Cover");
   put_vatt_txt(ncid, Snow_id, "units", "1");
   put_vatt_txt(ncid, Snow_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, Snow_id, "_FillValue", NC_SHORT, 1, fillVal);
   Snow_range[0] = 0;
   Snow_range[1] = 1;
   put_vatt(ncid, Snow_id, "valid_range", NC_INT, 2, Snow_range);

   put_vatt_txt(ncid, SWE_id, "long_name", "Snow Water Equivalent (cm)");
   put_vatt_txt(ncid, SWE_id, "units", "cm");
   put_vatt_txt(ncid, SWE_id, "coordinates", "Longitude Latitude");
   SWE_scale[0] = {SWE_SCAL};
   put_vatt(ncid, SWE_id, "scale_factor", NC_DOUBLE, 1, SWE_scale);
   put_vatt(ncid, SWE_id, "_FillValue", NC_SHORT, 1, fillVal);
   SWE_range[0] = 0;
   SWE_range[1] = 10000;
   put_vatt(ncid, SWE_id, "valid_range", NC_INT, 2, SWE_range);

   put_vatt_txt(ncid, SnowGS_id, "long_name", "Snow Grain Size (mm)");
   put_vatt_txt(ncid, SnowGS_id, "units", "mm");
   put_vatt_txt(ncid, SnowGS_id, "coordinates", "Longitude Latitude");
   SnowGS_scale[0] = {SNOWGS_SCAL};
   put_vatt(ncid, SnowGS_id, "scale_factor", NC_DOUBLE, 1, SnowGS_scale);
   put_vatt(ncid, SnowGS_id, "_FillValue", NC_SHORT, 1, fillVal);
   SnowGS_range[0] = 0;
   SnowGS_range[1] = 2000;
   put_vatt(ncid, SnowGS_id, "valid_range", NC_INT, 2, SnowGS_range);
   
   put_vatt_txt(ncid, SIce_id, "long_name", "Sea Ice Concentration (%)");
   put_vatt_txt(ncid, SIce_id, "units", "percent");
   put_vatt_txt(ncid, SIce_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, SIce_id, "_FillValue", NC_SHORT, 1, fillVal);
   SIce_range[0] = 0;
   SIce_range[1] = 100;
   put_vatt(ncid, SIce_id, "valid_range", NC_INT, 2, SIce_range);
   
   put_vatt_txt(ncid, SIce_MY_id, "long_name", "Multi-Year Sea Ice Concentration (%)");
   put_vatt_txt(ncid, SIce_MY_id, "units", "percent");
   put_vatt_txt(ncid, SIce_MY_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, SIce_MY_id, "_FillValue", NC_SHORT, 1, fillVal);
   SIce_MY_range[0] = 0;
   SIce_MY_range[1] = 100;
   put_vatt(ncid, SIce_MY_id, "valid_range", NC_INT, 2, SIce_MY_range);

   put_vatt_txt(ncid, SIce_FY_id, "long_name", "First-Year Sea Ice Concentration (%)");
   put_vatt_txt(ncid, SIce_FY_id, "units", "percent");
   put_vatt_txt(ncid, SIce_FY_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, SIce_FY_id, "_FillValue", NC_SHORT, 1, fillVal);
   SIce_FY_range[0] = 0;
   SIce_FY_range[1] = 100;
   put_vatt(ncid, SIce_FY_id, "valid_range", NC_INT, 2, SIce_FY_range);
   
   put_vatt_txt(ncid, TSkin_id, "long_name", "Skin Temperature (K)");
   put_vatt_txt(ncid, TSkin_id, "units", "Kelvin");
   put_vatt_txt(ncid, TSkin_id, "coordinates", "Longitude Latitude");
   TSkin_scale[0] = {TSKIN_SCAL};
   //put_vatt(ncid, TSkin_id, "scale_factor", NC_DOUBLE, 1, TSkin_scale);
   put_vatt(ncid, TSkin_id, "_FillValue", NC_FLOAT, 1, fillValF);
   TSkin_range[0] = 0.;
   TSkin_range[1] = 400.;
   put_vatt(ncid, TSkin_id, "valid_range", NC_FLOAT, 2, TSkin_range);

   put_vatt_txt(ncid, SurfP_id, "long_name", "Surface Pressure (mb)");
   put_vatt_txt(ncid, SurfP_id, "units", "millibars");
   put_vatt_txt(ncid, SurfP_id, "coordinates", "Longitude Latitude");
   SurfP_scale[0] = {SURFP_SCAL};
   put_vatt(ncid, SurfP_id, "scale_factor", NC_DOUBLE, 1, SurfP_scale);
   put_vatt(ncid, SurfP_id, "_FillValue", NC_SHORT, 1, fillVal);
   SurfP_range[0] = 0;
   SurfP_range[1] = 12000;
   put_vatt(ncid, SurfP_id, "valid_range", NC_INT, 2, SurfP_range);

   put_vatt_txt(ncid, Emis_id, "long_name", "Channel Emissivity");
   put_vatt_txt(ncid, Emis_id, "units", "1");
   put_vatt_txt(ncid, Emis_id, "coordinates", "Longitude Latitude Freq");
   Emis_scale[0] = {EMIS_SCAL} ;
   put_vatt(ncid, Emis_id, "scale_factor", NC_DOUBLE, 1, Emis_scale);
   put_vatt(ncid, Emis_id, "_FillValue", NC_SHORT, 1, fillVal);
   Emis_range[0] = 0;
   Emis_range[1] = 10000;
   put_vatt(ncid, Emis_id, "valid_range", NC_INT, 2, Emis_range);

   put_vatt_txt(ncid, SFR_id, "long_name", "Snow Fall Rate in mm/hr");
   put_vatt_txt(ncid, SFR_id, "units", "mm/hr");
   put_vatt_txt(ncid, SFR_id, "coordinates", "Longitude Latitude");
   SFR_scale[0] = {SFR_SCAL};
   put_vatt(ncid, SFR_id, "scale_factor", NC_DOUBLE, 1, SFR_scale);
   put_vatt(ncid, SFR_id, "_FillValue", NC_SHORT, 1, fillVal);
   SFR_range[0] = 0;
   SFR_range[1] = 10000;
   put_vatt(ncid, SFR_id, "valid_range", NC_INT, 2, SFR_range);

   put_vatt_txt(ncid, CldTop_id, "long_name", "Cloud Top Pressure");
   put_vatt_txt(ncid, CldTop_id, "units", "millibars");
   put_vatt_txt(ncid, CldTop_id, "coordinates", "Longitude Latitude");
   CldTop_scale[0] = {CLDTOP_SCAL};
   put_vatt(ncid, CldTop_id, "scale_factor", NC_DOUBLE, 1, CldTop_scale);
   put_vatt(ncid, CldTop_id, "_FillValue", NC_SHORT, 1, fillVal);

   put_vatt_txt(ncid, CldBase_id, "long_name", "Cloud Base Pressure");
   put_vatt_txt(ncid, CldBase_id, "units", "millibars");
   put_vatt_txt(ncid, CldBase_id, "coordinates", "Longitude Latitude");
   CldBase_scale[0] = {CLDBASE_SCAL};
   put_vatt(ncid, CldBase_id, "scale_factor", NC_DOUBLE, 1, CldBase_scale);
   put_vatt(ncid, CldBase_id, "_FillValue", NC_SHORT, 1, fillVal);

   put_vatt_txt(ncid, CldThick_id, "long_name", "Cloud Thickness");
   put_vatt_txt(ncid, CldThick_id, "units", "millibars");
   put_vatt_txt(ncid, CldThick_id, "coordinates", "Longitude Latitude");
   CldThick_scale[0] = {CLDTHICK_SCAL};
   put_vatt(ncid, CldThick_id, "scale_factor", NC_DOUBLE, 1, CldThick_scale);
   put_vatt(ncid, CldThick_id, "_FillValue", NC_SHORT, 1, fillVal);

   put_vatt_txt(ncid, RFlag_id, "long_name", "Rain Flag");
   put_vatt_txt(ncid, RFlag_id, "units", "1");
   put_vatt_txt(ncid, RFlag_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, RFlag_id, "_FillValue", NC_SHORT, 1, fillVal);

   put_vatt_txt(ncid, PrecipType_id, "long_name", "Precipitation Type (Frozen/Liquid)");
   put_vatt_txt(ncid, PrecipType_id, "units", "1");
   put_vatt_txt(ncid, PrecipType_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, PrecipType_id, "_FillValue", NC_SHORT, 1, fillVal);

   put_vatt_txt(ncid, SurfM_id, "long_name", "Surface Moisture");
   put_vatt_txt(ncid, SurfM_id, "units", "1");
   put_vatt_txt(ncid, SurfM_id, "coordinates", "Longitude Latitude");
   SurfM_scale[0] = {SURFM_SCAL};
   put_vatt(ncid, SurfM_id, "scale_factor", NC_DOUBLE, 1, SurfM_scale);
   put_vatt(ncid, SurfM_id, "_FillValue", NC_SHORT, 1, fillVal);
   
   put_vatt_txt(ncid, WindSp_id, "long_name", "Wind Speed");
   put_vatt_txt(ncid, WindSp_id, "units", "m/s");
   put_vatt_txt(ncid, WindSp_id, "coordinates", "Longitude Latitude");
   WindSp_scale[0] = {WINDSP_SCAL};
   put_vatt(ncid, WindSp_id, "scale_factor", NC_DOUBLE, 1, WindSp_scale);
   put_vatt(ncid, WindSp_id, "_FillValue", NC_SHORT, 1, fillVal);

   put_vatt_txt(ncid, WindDir_id, "long_name", "Wind Direction");
   put_vatt_txt(ncid, WindDir_id, "units", "1");
   put_vatt_txt(ncid, WindDir_id, "coordinates", "Longitude Latitude");
   WindDir_scale[0] = {WINDDIR_SCAL};
   put_vatt(ncid, WindDir_id, "scale_factor", NC_DOUBLE, 1, WindDir_scale);
   put_vatt(ncid, WindDir_id, "_FillValue", NC_SHORT, 1, fillVal);

   put_vatt_txt(ncid, WindU_id, "long_name", "U-direction Wind Speed");
   put_vatt_txt(ncid, WindU_id, "units", "m/s");
   put_vatt_txt(ncid, WindU_id, "coordinates", "Longitude Latitude");
   WindU_scale[0] = {WINDU_SCAL};
   put_vatt(ncid, WindU_id, "scale_factor", NC_DOUBLE, 1, WindU_scale);
   put_vatt(ncid, WindU_id, "_FillValue", NC_SHORT, 1, fillVal);

   put_vatt_txt(ncid, WindV_id, "long_name", "V-direction Wind Speed");
   put_vatt_txt(ncid, WindV_id, "units", "m/s");
   put_vatt_txt(ncid, WindV_id, "coordinates", "Longitude Latitude");
   WindV_scale[0] = {WINDV_SCAL};
   put_vatt(ncid, WindV_id, "scale_factor", NC_DOUBLE, 1, WindV_scale);
   put_vatt(ncid, WindV_id, "_FillValue", NC_SHORT, 1, fillVal);
   
   put_vatt_txt(ncid, Prob_SF_id, "long_name", "Probability of falling snow (%)");
   put_vatt_txt(ncid, Prob_SF_id, "units", "percent");
   put_vatt_txt(ncid, Prob_SF_id, "coordinates", "Longitude Latitude");
   put_vatt(ncid, Prob_SF_id, "_FillValue", NC_SHORT, 1, fillVal);
   Prob_SF_range[0] = 0;
   Prob_SF_range[1] = 100;
   put_vatt(ncid, Prob_SF_id, "valid_range", NC_INT, 2, Prob_SF_range);

   put_vatt_txt(ncid, quality_info_id, "long_name", "total number retrievals, percentage optimal retrievals, percentage sub optimal retrievals, percentage bad retrievals");
   put_vatt_txt(ncid, quality_info_id, "units", "1");
   put_vatt(ncid, quality_info_id, "total_number_retrievals",NC_INT, 1, total_number_retrievals);
   put_vatt(ncid, quality_info_id, "percentage_optimal_retrievals", NC_FLOAT, 1,percentage_optimal_retrievals);
   put_vatt(ncid, quality_info_id, "percentage_sub_optimal_retrievals",NC_FLOAT, 1, percentage_suboptimal_retrievals);
   put_vatt(ncid, quality_info_id, "percentage_bad_retrievals", NC_FLOAT, 1,percentage_bad_retrievals);

   //Output global attributes
   gnrt_attr_img(ncid, satid, date_created_img,fname_nc_img);
   
   // leave define mode 
   stat = nc_enddef (ncid);
   check_err(stat,__LINE__,__FILE__,errid);
   stat = nc_close(ncid);
   check_err(stat,__LINE__,__FILE__,errid);
   
   return 0;
}

void mirs2nc_class::gnrt_attr_img(int ncid, char* satid, char* date_created_img, char* fname_nc_img)
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

// use nccf only, 2022-09-12
  project_str           = project_nccf_str;
  naming_authority_str  = naming_authority_nccf_str;
  publisher_name_str    = publisher_name_nccf_str;

//keyword strings
  if ( (strcmp(satid,"NPP")==0) or (strcmp(satid,"N20")==0) or (strcmp(satid,"N21")==0) ) { 
        keyword_str           = keyword_atms_str;
  }
  else if ( (strcmp(satid,"M1")==0 ) or (strcmp(satid,"M3" )==0) or (strcmp(satid,"M2")==0) \
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

  cdf_missing_value[0] = MISSING;
  put_gatt(ncid, "missing_value", NC_INT, 1, cdf_missing_value);
    
  cdf_notretrievedproduct_value[0] = NAVA;
  put_gatt(ncid, "notretrievedproduct_value", NC_INT, 1, cdf_notretrievedproduct_value);
    
  cdf_noretrieval_value[0] = -99;
  put_gatt(ncid, "noretrieval_value", NC_INT, 1, cdf_noretrieval_value);
    
  cdf_version[0] = 4;
  put_gatt(ncid, "cdf_version", NC_DOUBLE, 1, cdf_version);
    
  alg_version[0] = alg_sn;
  put_gatt(ncid, "alg_version", NC_INT, 1, alg_version);
    
  put_gatt_txt(ncid, "dap_version", dap_ver_str );

  put_gatt_txt(ncid, "history_package", hist_pack_str);
  put_gatt_txt(ncid, "keywords", keyword_str);
  put_gatt(ncid, "ascend_descend_data_flag", NC_INT, 1, mode_flag_value);
    
    put_gatt_txt(ncid, "Conventions", Conventions_str );
    put_gatt_txt(ncid, "Metadata_Conventions", Metadata_Conventions_str );
    put_gatt_txt(ncid, "standard_name_vocabulary", standard_name_vocabulary_str );
    put_gatt_txt(ncid, "project", project_str );
    put_gatt_txt(ncid, "title", title_img_str );
    put_gatt_txt(ncid, "summary", summary_img_str );
    put_gatt_txt(ncid, "date_created", date_created_img );
    put_gatt_txt(ncid, "institution", institution_str );
    put_gatt_txt(ncid, "naming_authority", naming_authority_str );
    put_gatt_txt(ncid, "production_site", production_site_str );
    put_gatt_txt(ncid, "production_environment", production_environment_str );

  if( strcmp( satid, "NPP" ) == 0 ) {
    put_gatt_txt(ncid, "platform", NPP_platform_str );
    put_gatt_txt(ncid, "instrument", NPP_instrument_str );
  }
  else if( strcmp( satid, "N20" ) == 0 ) {
    put_gatt_txt(ncid, "platform", N20_platform_str );
    put_gatt_txt(ncid, "instrument", N20_instrument_str );
  }
  else if( strcmp( satid, "N21" ) == 0 ) {
    put_gatt_txt(ncid, "platform", N21_platform_str );
    put_gatt_txt(ncid, "instrument", N21_instrument_str );
  }
  else if( strcmp( satid, "metopSGA1" ) == 0 ) {
    put_gatt_txt(ncid, "platform", metopSGA1_platform_str );
    put_gatt_txt(ncid, "instrument", metopSGA1_instrument_str );
  }
  else if( strcmp( satid, "M2" ) == 0 ) {
    put_gatt_txt(ncid, "platform", METOPA_platform_str );
    put_gatt_txt(ncid, "instrument", METOPA_instrument_str );
  }
  else if( strcmp( satid, "M1" ) == 0 ) {
    put_gatt_txt(ncid, "platform", METOPB_platform_str );
    put_gatt_txt(ncid, "instrument", METOPB_instrument_str );
  }
  else if( strcmp( satid, "M3" ) == 0 ) {
    put_gatt_txt(ncid, "platform", METOPC_platform_str );
    put_gatt_txt(ncid, "instrument", METOPC_instrument_str );
  }
  else if( strcmp( satid, "GPM" ) == 0 ) {
    put_gatt_txt(ncid, "platform", GPM_platform_str );
    put_gatt_txt(ncid, "instrument", GPM_instrument_str );
  }

    put_gatt_txt(ncid, "creator_name", creator_name_str );
    put_gatt_txt(ncid, "creator_email", creator_email_str );
    put_gatt_txt(ncid, "creator_url", creator_url_str );
    put_gatt_txt(ncid, "publisher_name", publisher_name_str );
    put_gatt_txt(ncid, "publisher_email", publisher_email_str );
    put_gatt_txt(ncid, "publisher_url", publisher_url_str );

    put_gatt_txt(ncid, "metadata_link", foname_img);

    put_gatt_txt(ncid, "references", references_str );
    put_gatt_txt(ncid, "history", mirs_ver_str );
    put_gatt_txt(ncid, "processing_level", processing_level_str );
    put_gatt_txt(ncid, "source", RDR_name );
    put_gatt_txt(ncid, "time_coverage_start", time_coverage_start );
    put_gatt_txt(ncid, "time_coverage_end", time_coverage_end );
    put_gatt_txt(ncid, "cdm_data_type", cdm_data_type_str );
    put_gatt_txt(ncid, "geospatial_lat_units", geospatial_lat_units_str );
    put_gatt_txt(ncid, "geospatial_lon_units", geospatial_lon_units_str );

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

    put_gatt(ncid, "total_number_retrievals", NC_INT, 1, total_number_retrievals);
    put_gatt(ncid, "percentage_optimal_retrievals", NC_FLOAT, 1, percentage_optimal_retrievals);
    put_gatt(ncid, "percentage_suboptimal_retrievals", NC_FLOAT, 1, percentage_suboptimal_retrievals);
    put_gatt(ncid, "percentage_bad_retrievals", NC_FLOAT, 1, percentage_bad_retrievals);

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
    strcat(uuid,date_created_img);
    strcat(uuid,"_");
    strcat(uuid,char_random_number);
    strcat(uuid,"_");
    strcat(uuid,RDR_name);
    
    put_gatt_txt(ncid, "id", uuid );
    
}
