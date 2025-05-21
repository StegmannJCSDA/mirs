/***************************************************************************
 *  Program Name      : rmirs_edr.cpp
 *  Type              : Subroutine
 *  Function          : Program reads the edr record of MIRS file 
 *  Input Files       : 
 *  Output Files      : None
 *  Subroutine Called : None 
 *  Called by         : rmirs_wnetcdf.cpp
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *   11/30/2006    v1.0
 *   06//302008    Jiang Zhao      Modified for new Release
 *   08/20/2009    Wanchun Chen    Modified for MIRS EDR
 *   02/24/2020    v2.0            Ming Fang
 *                                 1. Changed global arrays to 1D poiters
 *                                 2. Made changes associated with 1
 *
 *************************************************************************/
#include <iostream>
#include "constants.h"
#include "swath.h"

using namespace std;

void mirs2nc_class::rmirs_edr(FILE *mirsd, short int *nscan_old, short int *nspot, short int *nlayer, short int *nchan_old, char * satid)
{
   short int i, j, day, mon, hr, min, sec;
   short int ich;
   
   char dummy[4] ;

   char char_itype[4], char_alg[4], char_nprf[4], char_nlay[4], char_nlev[4], char_nchan[4];
   int itype, alg, nprf, nlay, nlev, nchan;

   char char_npos[4],char_nscan[4],char_nabsorb[4],char_nclw[4],char_nrain[4],char_nsnow[4],char_nice[4],char_ngraupel[4],char_nqc[4];
   int npos,nscan,nabsorb,nclw,nrain,nsnow,nice,ngraupel,nqc;
   
   char *absorbID;
   char fr[4], po[4];
   
   
   char char_profIndx[4]; 
   int  profIndx;
   
   char char_press_lay[4]; 
   float  value_press_lay;
   
   char char_press_lev[4]; 
   float  value_press_lev;
   
   char char_temp[4]; 
   float  layer_temp[MAXLAY];
   
   char char_absorb[4]; 
   float  layer_vapor[MAXLAY], layer_ozone[MAXLAY];
   
   char char_clw[4]; 
   float  layer_clw[MAXLAY];
   
   char char_rain[4]; 
   float  layer_rain[MAXLAY];
   
   char char_graupel[4]; 
   float  layer_graupel[MAXLAY];
   
   char char_emis[4]; 
   float  chan_emis[MAXCH];
   
   char char_angle[4], char_windsp[4], char_tskin[4], char_psfc[4], char_sfc[4], char_u[4], char_v[4],
        char_realAziAngle[4], char_solZenAngle[4],char_snowDepth[4];
   // float value_windsp;
   float value_angle, value_tskin, value_psfc ;
   // int value_sfc ; 
   // float value_u, value_v, value_realAziAngle, value_solZenAngle, value_snowDepth;
   float value_realAziAngle, value_solZenAngle;
   
   char char_qc[2];
   int value_qc[4];
   
   char char_lat[4], char_lon[4], char_node[4], char_utc[4], char_year[4], char_day[4], char_ipos[4], char_iscan[4];
   float value_lat, value_lon ; 
   int value_node; 
   float value_utc; 
   int value_year, value_day, ipos, iscan;
   
   
   char char_nAttempt[4], char_nIter[4];
   int value_nAttempt, value_nIter;
   char char_chisq[4];
   float value_chisq;
   
   char char_yfwd[4], char_chanSel[4], char_ym[4], char_ymCorr[4];
   float value_yfwd, value_chanSel, value_ym, value_ymCorr;
   
   
   int  iprf ;
   int  time_coverage_start_done = 0; // to record if an valid starting time found or not

   int find_lastValidScanIdx = 0; //liusy+
   int lastValidScanIdx,time_utc_lastValid,year_lastValid,month_lastValid,day_lastValid; //liusy+
   
   int isAscend   = 0;
   int isDescend  = 0;
/*---------------------------------------------------*
 * Read the header
 *---------------------------------------------------*/
   fread(&dummy, 4, 1, mirsd); 
   fread(&char_itype, 4, 1, mirsd); 
   fread(&char_alg, 4, 1, mirsd);  
   fread(&dummy, 4, 1, mirsd);
   
   fread(&dummy, 4, 1, mirsd); fread(&char_nprf,     4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_nlay,     4, 1, mirsd); fread(&dummy, 4, 1, mirsd);   
   fread(&dummy, 4, 1, mirsd); fread(&char_nlev,     4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_nchan,    4, 1, mirsd); fread(&dummy, 4, 1, mirsd);  
   fread(&dummy, 4, 1, mirsd); fread(&char_npos,     4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_nscan,    4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_nabsorb,  4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_nclw,     4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_nrain,    4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_nsnow,    4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_nice,     4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_ngraupel, 4, 1, mirsd); fread(&dummy, 4, 1, mirsd);

   itype    = intSwap(char_itype);
   alg      = intSwap(char_alg);
   nprf     = intSwap(char_nprf);
   nlay     = intSwap(char_nlay);
   nlev     = intSwap(char_nlev);
   nchan    = intSwap(char_nchan);
   npos     = intSwap(char_npos);
   nscan    = intSwap(char_nscan);
   nabsorb  = intSwap(char_nabsorb);
   nclw     = intSwap(char_nclw);
   nrain    = intSwap(char_nrain);
   nsnow    = intSwap(char_nsnow);
   nice     = intSwap(char_nice);
   ngraupel = intSwap(char_ngraupel);

   alg_sn = alg;
   total_number_retrievals[0] = nprf;
   
   //absorbID = (char *)malloc( (nabsorb * 4 + 1 ) * sizeof(char) );
   absorbID = new char[nabsorb * 4];   
   fread(&dummy, 4, 1, mirsd); fread(absorbID, 4, nabsorb, mirsd);  fread(&dummy, 4, 1, mirsd);
   

   //Memory allocation and initilization of EDR-associated variables
   MemAllocEDR();
     
   fread(&dummy, 4, 1, mirsd); 
   for(i=0; i<nchan; i++) {
     fread(&fr, 4, 1, mirsd);
     freq[i] = floatSwap(fr);
     //printf("fr=%g\n", floatSwap(fr)); 
   }
   fread(&dummy, 4, 1, mirsd);
   
   
   fread(&dummy, 4, 1, mirsd); 
   for(i=0; i<nchan; i++) {
     fread(&po, 4, 1, mirsd);
     polo[i] = intSwap(po);
     //printf("po=%i\n", intSwap(po)); 
   }
   fread(&dummy, 4, 1, mirsd);
   
   fread(&dummy, 4, 1, mirsd); fread(&char_nqc,  4, 1, mirsd);    fread(&dummy, 4, 1, mirsd);
   nqc = intSwap(char_nqc);
   
   //printf("nqc=%i\n", nqc);


/*---------------------------------------------------*
   read content
-----------------------------------------------------*/
  
  for(iprf=0; iprf<nprf; iprf++) {   
  
    fread(&dummy, 4, 1, mirsd); fread(&char_profIndx,  4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
    profIndx = intSwap(char_profIndx); 
      
    fread(&dummy, 4, 1, mirsd);
    for(i=0; i<nlay; i++) {
      fread(&char_press_lay,  4, 1, mirsd);
      value_press_lay = floatSwap(char_press_lay);
      player[i] = value_press_lay;
    }
    fread(&dummy, 4, 1, mirsd);
    
    fread(&dummy, 4, 1, mirsd);
    for(i=0; i<nlev; i++) {
      fread(&char_press_lev,  4, 1, mirsd);
      value_press_lev = floatSwap(char_press_lev);
      plevel[i] = value_press_lev;
    }
    fread(&dummy, 4, 1, mirsd);
    
    fread(&dummy, 4, 1, mirsd);
    for(i=0; i<nlay; i++) {
      fread(&char_temp,  4, 1, mirsd);
      layer_temp[i] = floatSwap(char_temp);
    }
    fread(&dummy, 4, 1, mirsd);
    
  
    /** note: Native FORTRAN layout (collumn-major order): 
   
    INTEGER A(2,3)
    a(1,1) a(2,1) a(1,2) a(2,2)  a(1,3) a(2,3)
   
    Or INTEGER A(2,2,2) 
    a(1,1,1) a(2,1,1) a(1,2,1) a(2,2,1) a(1,1,2) a(2,1,2) a(1,2,2) a(2,2,2)
   
    Native C layout (row-major order) is NOT equivalent to the FORTRAN layout: int a[2][3];
    a[0][0] a[0][1] a[0][2] a[1][0] a[1][1] a[1][2]
    
    
    so a fortran write statement:
    WRITE(iu) arr(1:nLay,1:nAbsorb)
    
    will be read by C this way:
      FILE *fp;
      char chars[4], dummy[4]; 
      fread(&dummy, 4, 1, fp); // skip 4-byte leading dummy
      for(iAbsorb=0; iAbsorb<nAbsorb; iAbsorb++) {
      for(iLay=0; iLay<Nlay; iLay++) {
    result = fread(&chars,  4, 1, fp);
    //values = chars2float(chars);
      }
      }
      fread(&dummy, 4, 1, fp); // skip 4-byte trailing dummy
      
    */
    
    fread(&dummy, 4, 1, mirsd);
    for(i=0; i<nabsorb; i++) {
      for(j=0; j<nlay; j++) {
        fread(&char_absorb,  4, 1, mirsd);
   if( i == 0 ) {
     layer_vapor[j] = floatSwap(char_absorb);
     //printf("layer_vapor[j]=%f\n",layer_vapor[j]);
   }
   else if ( i == 1 ) {
     layer_ozone[j] = floatSwap(char_absorb);
     //printf("layer_ozone[j]=%f\n",layer_ozone[j]);
   }
      }
    }
    fread(&dummy, 4, 1, mirsd);
    
    
    fread(&dummy, 4, 1, mirsd);
    for(i=0; i<nclw; i++) {
      fread(&char_clw,  4, 1, mirsd);
      layer_clw[i] = floatSwap(char_clw);
    }
    fread(&dummy, 4, 1, mirsd);
    
    fread(&dummy, 4, 1, mirsd);
    for(i=0; i<nrain; i++) {
      fread(&char_rain,  4, 1, mirsd);
      layer_rain[i] = floatSwap(char_rain);
    }
    fread(&dummy, 4, 1, mirsd);
    
    fread(&dummy, 4, 1, mirsd);
    for(i=0; i<ngraupel; i++) {
      fread(&char_graupel,  4, 1, mirsd);
      layer_graupel[i] = floatSwap(char_graupel);
    }
    fread(&dummy, 4, 1, mirsd);
    
    fread(&dummy, 4, 1, mirsd);
    
    for(i=0; i<nchan; i++) {
      fread(&char_emis,  4, 1, mirsd);
      chan_emis[i] = floatSwap(char_emis);
      //printf("chan_emis=%f\n",chan_emis[i]);
    }
    fread(&dummy, 4, 1, mirsd);
    
    
    fread(&dummy, 4, 1, mirsd);
    fread(&char_angle, 4, 1, mirsd);
    fread(&char_windsp, 4, 1, mirsd);
    fread(&char_tskin, 4, 1, mirsd);
    fread(&char_psfc, 4, 1, mirsd);
    fread(&char_sfc, 4, 1, mirsd);
    fread(&char_u, 4, 1, mirsd);
    fread(&char_v, 4, 1, mirsd);
    fread(&char_realAziAngle, 4, 1, mirsd);
    fread(&char_solZenAngle, 4, 1, mirsd);
    fread(&char_snowDepth, 4, 1, mirsd);
    fread(&dummy, 4, 1, mirsd);
    
    value_angle = floatSwap(char_angle);
    value_realAziAngle = floatSwap(char_realAziAngle);
    value_solZenAngle = floatSwap(char_solZenAngle);
    value_tskin = floatSwap(char_tskin);
    value_psfc = floatSwap(char_psfc);
    
    fread(&dummy, 4, 1, mirsd);
    for(i=0; i<nqc; i++) {
      fread(&char_qc, 2, 1, mirsd);
      value_qc[i] = shortSwap(char_qc);
      //printf("i=%i, qc=%i\n", i, value_qc );
    }
    fread(&dummy, 4, 1, mirsd);

    fread(&dummy, 4, 1, mirsd);
    fread(&char_lat, 4, 1, mirsd);
    fread(&char_lon, 4, 1, mirsd);
    fread(&char_node, 4, 1, mirsd);
    fread(&char_utc, 4, 1, mirsd);
    fread(&char_year, 4, 1, mirsd);
    fread(&char_day, 4, 1, mirsd);
    fread(&char_ipos, 4, 1, mirsd);
    fread(&char_iscan, 4, 1, mirsd);
    fread(&dummy, 4, 1, mirsd);
    
    value_lat  = floatSwap(char_lat);
    value_lon  = floatSwap(char_lon);
    value_node = intSwap(char_node);
    value_utc  = floatSwap(char_utc);
    value_year = intSwap(char_year);
    value_day  = intSwap(char_day);
    
    
    // !!! note fortran starts from index 1, while C starts from index 0
    ipos       = intSwap(char_ipos) - 1;
    iscan      = intSwap(char_iscan) -1 ; 
    
    // decide descending, ascending, or both for the whole orbit
    if (value_node == 0 ) { // ascending
       isAscend = 1;
    }
    else if (value_node == 1) { // descending
       isDescend  = 1;
    }

    if ( iscan == (nscan-1) ) {
       if ( isAscend == 1 && isDescend == 0 ) {
          mode_flag_value[0] = 0; // ascending
       }
       else if ( isAscend == 0 && isDescend == 1 ) {
          mode_flag_value[0] = 1; // descending
       }
       else  {
          mode_flag_value[0] = 2; // both
       }
    }

    // get 4 boundary lat/lon pair values
    if( iscan == 0 && ipos == 0 ) {
      geospatial_first_scanline_first_fov_lat[0] = value_lat;
      geospatial_first_scanline_first_fov_lon[0] = value_lon;
    }
    
    if( iscan == 0 && ipos >0 && value_lat > -100.) { 
      geospatial_first_scanline_last_fov_lat[0] = value_lat;
      geospatial_first_scanline_last_fov_lon[0] = value_lon;
    }
    if( iscan == (nscan-1) && ipos == 0 && value_lat > -100. ) {
      geospatial_last_scanline_first_fov_lat[0] = value_lat;
      geospatial_last_scanline_first_fov_lon[0] = value_lon;
    }
    
    if( iscan == (nscan-1) && ipos == (npos-1) && value_lat > -100. ) {
      geospatial_last_scanline_last_fov_lat[0] = value_lat;
      geospatial_last_scanline_last_fov_lon[0] = value_lon;
    }
    
    for(i=0;i<nqc;i++){
      int idx3d = iscan*npos*4 + ipos*4 + i;
      qc[idx3d] = value_qc[i];
    }
    
    int idx2d = iscan*npos + ipos;
    lat[idx2d]     = value_lat;
    lon[idx2d]     = value_lon;
    angle[idx2d]   = value_angle;
    rel_azi[idx2d] = value_realAziAngle;
    sza[idx2d]     = value_solZenAngle;
 
    #pragma omp parallel for num_threads(8)
    for(i=0;i<nlay;i++) {
      int idx3d = iscan*npos*nlay + ipos*nlay + i;
      pvapor[idx3d]   = layer_vapor[i];
      pozone[idx3d]   = layer_ozone[i];
      ptemp[idx3d]    = layer_temp[i];
      pclw[idx3d]     = layer_clw[i];
      prain[idx3d]    = layer_rain[i];
      pgraupel[idx3d] = layer_graupel[i];
      
      psnow[idx3d]    = NAVA;
      pice[idx3d]     = NAVA;
    }
    
    if( value_tskin > 0 ) 
      tskin[idx2d] = value_tskin / TSKIN_SCAL;
    else
      tskin[idx2d] = value_tskin;
    
    if( value_psfc > 0 )
      surfp[idx2d] = value_psfc / SURFP_SCAL;
    else
      surfp[idx2d] = value_psfc;
       
    for(ich=0;ich<nchan;ich++) {
      int idx3d = iscan*npos*nchan + ipos*nchan + ich;
      if( chan_emis[ich] > 0 ) emis[idx3d] = chan_emis[ich] / EMIS_SCAL;
      else                     emis[idx3d] = chan_emis[ich];
    }

    year[iscan] = value_year;
    doy[iscan]  = value_day;
    time_utc[iscan] = value_utc;  
    //printf("iscan=%i, ipos=%i, iprf=%i, year=%i, time_utc=%f\n", iscan, ipos, iprf, value_year, value_utc);
    orb_mode[iscan] = value_node;

    // Now do some time conversion here 
    jd2md(value_day, value_year, &day, &mon);  
    month[iscan]  = mon;
    dom[iscan]    = day;
   
    utc2hms(value_utc, &hr, &min, &sec);
    
    hour[iscan]   = hr;
    minute[iscan] = min;
    second[iscan] = sec;

    if (value_year < 1970) {
       year[iscan] = MISSING;
    }
    if (value_day < 0) {
       doy[iscan]   = MISSING;
       month[iscan] = MISSING;
       dom[iscan]   = MISSING;
    }
    if (value_utc < 0) {
       time_utc[iscan]  = MISSING;
       hour[iscan]      = MISSING;
       minute[iscan]    = MISSING;
       second[iscan]    = MISSING;
    }

//liusy+ find the lat and lon for the last valid fov
    if (find_lastValidScanIdx == 0) {
       find_lastValidScanIdx  = 1;
       lastValidScanIdx = -99;
    }
    if ( lastValidScanIdx < 0. && ipos == (npos-1) && value_lat < -100.) {
      lastValidScanIdx  = iscan - 1 ;
    }
   if ( lastValidScanIdx < 0. && iscan == (nscan-1) && ipos == (npos-1) && value_year > 0) {
      lastValidScanIdx = nscan - 1;
   }
//liusy+: If the last scan line is valid, though there are missing scanlines in the middle, use last scan line as valid
   if (lastValidScanIdx < nscan -1) {
      //if (lat[nscan-1][0] > -100. && lat[nscan-1][npos-1] > -100.)
      if (lat[(nscan-1)*npos] > -100. && lat[nscan*npos-1] > -100.){
         lastValidScanIdx = nscan-1;
      }
   }

   if (lastValidScanIdx >= 0) {
      geospatial_last_scanline_first_fov_lat[0] = lat[lastValidScanIdx*npos];
      geospatial_last_scanline_first_fov_lon[0] = lon[lastValidScanIdx*npos];
      geospatial_last_scanline_last_fov_lat[0] = lat[lastValidScanIdx*npos+npos-1];
      geospatial_last_scanline_last_fov_lon[0] = lon[lastValidScanIdx*npos+npos-1];
   }
//liusy.
    
    // this will have problem if the first one is invalid
    //if( iprf == 0 ) {
    
    // implement a fix for NDE ATMS processing: use the earliest time, regardless of qc values
    if( time_coverage_start_done == 0 ) {
      time_coverage_start_done = 1;
      //printf("iprf == 0, value_utc=%f\n",value_utc);
      sprintf(time_coverage_start,"%4d-%02d-%02dT%02d:%02d:%02dZ",value_year,mon,day,hr,min,sec);
    }
    
    //if( iprf == (nprf-1) && value_qc[0] != 2 ) {
    // keep updating ending time to handle missing values exception,
    // so it is the last valid time
    
    // implement a fix NDE ATMS processing: the latest time, regardless of qc values
    //    if( value_qc[0] == 0 || value_qc[0] == 1 ) {
      //printf("iprf == nprf-1, value_utc=%f\n",value_utc);
      //printf("value_year=%d\n",value_year);
      //printf("value_day=%d\n",value_day);
//liusy-      utc2hms(value_utc+2.6667, &hr, &min, &sec);  // end time need add 2.667 seconds
//liusy-      sprintf(time_coverage_end,"%4d-%02d-%02dT%02d:%02d:%02dZ",value_year,mon,day,hr,min,sec);
//liusy+ find the time fof the last valide fov
      if (lastValidScanIdx >= 0) {
         if ( (strcmp(satid,"NPP")==0) or (strcmp(satid,"N20")==0) or (strcmp(satid,"N21")==0) ) {
            time_utc_lastValid   = time_utc[lastValidScanIdx] + 2.6667;  // end time need add 2.667 seconds
         }
         else {
            time_utc_lastValid   = time_utc[lastValidScanIdx] ;
         }

         year_lastValid       = year[lastValidScanIdx];
         month_lastValid      = month[lastValidScanIdx];
         day_lastValid        = dom[lastValidScanIdx];

         utc2hms(time_utc_lastValid, &hr, &min, &sec);  // end time need add 2.667 seconds
         sprintf(time_coverage_end,"%4d-%02d-%02dT%02d:%02d:%02dZ",year_lastValid,month_lastValid,day_lastValid,hr,min,sec);
      }
//liusy.
    
    if( itype == 1 ) {
    
      fread(&dummy, 4, 1, mirsd);
      fread(&char_nAttempt, 4, 1, mirsd);
      fread(&char_nIter, 4, 1, mirsd);
      fread(&char_chisq, 4, 1, mirsd);
      fread(&dummy, 4, 1, mirsd);
      
      value_nAttempt = intSwap(char_nAttempt);
      value_nIter    = intSwap(char_nIter);
      value_chisq    = floatSwap(char_chisq);

      nattempt[idx2d] = value_nAttempt;
      niter[idx2d]    = value_nIter;
      chisq[idx2d]    = value_chisq;

      fread(&dummy, 4, 1, mirsd);
      for(i=0; i<nchan; i++) {
        int idx3d = iscan*npos*nchan + ipos*nchan + i;
        fread(&char_yfwd, 4, 1, mirsd);
   value_yfwd = floatSwap(char_yfwd) ;
   if( value_yfwd > 0 )
     yfwd[idx3d] = value_yfwd / BT_SCALE;
   else
     yfwd[idx3d] = value_yfwd; 
      }
      fread(&dummy, 4, 1, mirsd);
    
      fread(&dummy, 4, 1, mirsd);
      for(i=0; i<nchan; i++) {
        int idx3d = iscan*npos*nchan + ipos*nchan + i;
        fread(&char_chanSel, 4, 1, mirsd);
   value_chanSel = intSwap(char_chanSel) ;
   if( value_chanSel > 0 )
     chanSel[idx3d] = value_chanSel;
   else
     chanSel[idx3d] = value_chanSel; 
      }
      fread(&dummy, 4, 1, mirsd);
    
      fread(&dummy, 4, 1, mirsd);
      for(i=0; i<nchan; i++) {
        int idx3d = iscan*npos*nchan + ipos*nchan + i;
        fread(&char_ym, 4, 1, mirsd);
   value_ym = floatSwap(char_ym) ;
   if( value_ym > 0 )
     ym[idx3d] = value_ym / BT_SCALE;
   else
     ym[idx3d] = value_ym; 
      }
      fread(&dummy, 4, 1, mirsd);
    
      fread(&dummy, 4, 1, mirsd);
      for(i=0; i<nchan; i++) {
        int idx3d = iscan*npos*nchan + ipos*nchan + i;
        fread(&char_ymCorr, 4, 1, mirsd);
   value_ymCorr = floatSwap(char_ymCorr) ;
   if( value_ymCorr > 0 )
     bt[idx3d] = value_ymCorr / BT_SCALE;
   else
     bt[idx3d] = value_ymCorr; 
      }
      fread(&dummy, 4, 1, mirsd);
    
    } //end of --if( itype == 1 )

  } //end of loop of iprf
 
  if(absorbID)  delete[] absorbID;

  *nchan_old = nchan;
  *nscan_old = nscan;
  *nspot = npos;
  *nlayer = nlay;

    
  return; 
   
}
