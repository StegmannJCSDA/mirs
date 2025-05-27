/********************************************************************************
 *  Program Name      : rmirs_dep.cpp
 *  Type              : Subroutine
 *  Function          : Program reads the MIRS DEP output 
 *  Input Files       : 
 *  Output Files      : None
 *  Subroutine Called : None 
 *  Called by         : rmirs_wnetcdf.cpp
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *   06/14/2007    v1.0 
 *   06/06/2008    V2.0               Jiang Zhao
 *   08/24/2009    V3.0               Wanchun Chen     modified for DEP
 *   03/06/2019    V4.0               Ryan Honeyager   added SFR DEP variables
 *   02/24/2020    V5.0               Ming Fang        
 *                                    1. Changed global arrays to 1D poiters
 *                                    2. Made changes associated with 1
 ********************************************************************************/
#include "constants.h"
#include "swath.h"

//#include <iostream>
//using namespace std;

void mirs2nc_class::rmirs_dep(FILE *mirsd, short int *nscan_old, short int *nspot, char *satid)
{
   short int i,k;

   // header identifiers
   char dummy[4];
   char char_itype[4], char_alg[4], char_nprf[4], char_npos[4], char_nscan[4];
   int itype, alg, nprf, npos, nscan;;


   // content identifiers
   char char_profIndx[4];
   int profIndx;
   
   char char_iatm[4], char_tpw[4], char_clw[4], char_rwp[4], char_swp[4], char_iwp[4], char_gwp[4], char_rr[4],char_sfr[4],
        char_cldtop[4], char_cldbase[4], char_cldthick[4], char_preciptype[4], char_rainflag[4], char_lwp[4]; 
   int value_iatm; 
   float value_tpw, value_clw, value_rwp, value_swp, value_iwp, value_gwp, value_rr, value_sfr, 
         value_cldtop, value_cldbase, value_cldthick, value_preciptype, value_rainflag, value_lwp; 
  
   char char_isfc[4], char_swe[4], char_snow[4], char_sm[4], char_sice[4], char_windsp[4], char_winddir[4], char_u[4], 
        char_v[4], char_snowgs[4], char_sicefy[4], char_sicemy[4];
   int value_isfc;
   float value_swe, value_snow, value_sm, value_sice, value_windsp, value_winddir, 
         value_u, value_v, value_snowgs, value_sicefy, value_sicemy;
   float value_prob_sf = 0, value_prob_rf = 0;
 
   char char_qc[4];
   int value_qc[4];

   char char_lat[4], char_lon[4], char_node[4], char_utc[4], char_year[4], char_day[4], char_ipos[4], char_iscan[4], 
        char_angle[4], char_realAziAngle[4], char_solZenAngle[4];
   //float lat, lon, utc, angle, realAziAngle, solZenAngle; 
   //int value_node,value_year, value_day, ipos, iscan; 
   int ipos, iscan; 

   char char_nIter[4], char_chisq[4];
   //int nIter, chisq;
   char char_prob_sf[4], char_prob_rf[4]; 
   int iprf;
   int qc0,qc1,qc2;
   
   qc0 = 0;
   qc1 = 0;
   qc2 = 0;
   

   // reader header part
   fread(&dummy, 4, 1, mirsd); 
   fread(&char_itype, 4, 1, mirsd); 
   fread(&char_alg, 4, 1, mirsd);  
   fread(&dummy, 4, 1, mirsd);
   
   fread(&dummy, 4, 1, mirsd); fread(&char_nprf,     4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_npos,     4, 1, mirsd); fread(&dummy, 4, 1, mirsd);
   fread(&dummy, 4, 1, mirsd); fread(&char_nscan,    4, 1, mirsd); fread(&dummy, 4, 1, mirsd);

   itype    = intSwap(char_itype);
   alg      = intSwap(char_alg);
   nprf     = intSwap(char_nprf);
   npos     = intSwap(char_npos);
   nscan    = intSwap(char_nscan);
   
   alg_sn = alg;
   
  /* read content*/
  //Memory allocation and initilization of DEP-associated variables
  MemAllocDEP();
  
  for(iprf=0; iprf<nprf; iprf++) {
    
    fread(&dummy, 4, 1, mirsd); 
    fread(&char_profIndx,  4, 1, mirsd);
    fread(&dummy, 4, 1, mirsd);
    profIndx = intSwap(char_profIndx); 
    
    fread(&dummy, 4, 1, mirsd);
    fread(&char_iatm,  4, 1, mirsd);
    fread(&char_tpw,  4, 1, mirsd);
    fread(&char_clw,  4, 1, mirsd);
    fread(&char_rwp,  4, 1, mirsd);
    fread(&char_swp,  4, 1, mirsd);
    fread(&char_iwp,  4, 1, mirsd);
    fread(&char_gwp,  4, 1, mirsd);
    fread(&char_rr,  4, 1, mirsd);
    fread(&char_sfr,  4, 1, mirsd);
    fread(&char_cldtop,  4, 1, mirsd);
    fread(&char_cldbase,  4, 1, mirsd);
    fread(&char_cldthick,  4, 1, mirsd);
    fread(&char_preciptype,  4, 1, mirsd);
    fread(&char_rainflag,  4, 1, mirsd);
    fread(&char_lwp,  4, 1, mirsd);
    fread(&dummy, 4, 1, mirsd);
    
    value_iatm       = intSwap(char_iatm);
    value_tpw        = floatSwap(char_tpw);
    value_clw        = floatSwap(char_clw);
    value_rwp        = floatSwap(char_rwp);
    value_swp        = floatSwap(char_swp);
    value_iwp        = floatSwap(char_iwp);
    value_gwp        = floatSwap(char_gwp);
    value_rr         = floatSwap(char_rr);
    value_sfr        = floatSwap(char_sfr);
    value_cldtop     = floatSwap(char_cldtop);
    value_cldbase    = floatSwap(char_cldbase);
    value_cldthick   = floatSwap(char_cldthick);
    value_preciptype = floatSwap(char_preciptype);
    value_rainflag   = floatSwap(char_rainflag);
    value_lwp        = floatSwap(char_lwp);
    
    fread(&dummy, 4, 1, mirsd);
    fread(&char_isfc,  4, 1, mirsd);
    fread(&char_swe,  4, 1, mirsd);
    fread(&char_snow,  4, 1, mirsd);
    fread(&char_sm,  4, 1, mirsd);
    fread(&char_sice,  4, 1, mirsd);
    fread(&char_windsp,  4, 1, mirsd);
    fread(&char_winddir,  4, 1, mirsd);
    fread(&char_u,  4, 1, mirsd);
    fread(&char_v,  4, 1, mirsd);
    fread(&char_snowgs,  4, 1, mirsd);
    fread(&char_sicefy,  4, 1, mirsd);
    fread(&char_sicemy,  4, 1, mirsd);
    fread(&dummy, 4, 1, mirsd);
 
    fread(&dummy, 4, 1, mirsd);
    for(i=0; i<4; i++) {
      fread(&char_qc,  4, 1, mirsd);
      value_qc[i] = intSwap(char_qc);
    }    
    if( value_qc[0] == 0 ) qc0++;
    if( value_qc[0] == 1 ) qc1++;
    if( value_qc[0] == 2 ) qc2++;
    fread(&dummy, 4, 1, mirsd);
 
    value_isfc      = intSwap(char_isfc);
    value_swe       = floatSwap(char_swe);
    value_snow      = floatSwap(char_snow);
    value_sm        = floatSwap(char_sm);
    value_sice      = floatSwap(char_sice);
    value_windsp    = floatSwap(char_windsp);
    value_winddir   = floatSwap(char_winddir);
    value_u         = floatSwap(char_u);
    value_v         = floatSwap(char_v);
    value_snowgs    = floatSwap(char_snowgs);
    value_sicefy    = floatSwap(char_sicefy);
    value_sicemy    = floatSwap(char_sicemy);
    
    fread(&dummy, 4, 1, mirsd);
    fread(&char_lat, 4, 1, mirsd);
    fread(&char_lon, 4, 1, mirsd);
    fread(&char_node, 4, 1, mirsd);
    fread(&char_utc, 4, 1, mirsd);
    fread(&char_year, 4, 1, mirsd);
    fread(&char_day, 4, 1, mirsd);
    fread(&char_ipos, 4, 1, mirsd);
    fread(&char_iscan, 4, 1, mirsd);
    fread(&char_angle, 4, 1, mirsd);
    fread(&char_realAziAngle, 4, 1, mirsd);
    fread(&char_solZenAngle, 4, 1, mirsd);
    fread(&dummy, 4, 1, mirsd);
    
    // !!! note Fortran starts from 1, while C starts from 0
    ipos  = intSwap(char_ipos) - 1;
    iscan = intSwap(char_iscan) - 1 ;
    
    // !!! dep qc are the same as edr qc, so no need to update qc array
    for(k=0;k<4;k++){
      int idx3d = iscan*npos*4 + ipos*4 + k;
      qc_dep[idx3d] = value_qc[k];
    }
    
    
    int idx2d = iscan*npos + ipos;
    
    iTypAtm[idx2d] = value_iatm;
    iTypSfc[idx2d] = value_isfc;
    
    if( value_tpw > 0 )
      tpw[idx2d] = value_tpw / TPW_SCAL;
    else
      tpw[idx2d] = value_tpw;
    
    if( value_clw > 0 )
      clw[idx2d] = value_clw / CLW_SCAL;
    else
      clw[idx2d] = value_clw;
      
    if( value_rwp > 0 )   
      rwp[idx2d] = value_rwp / RWP_SCAL;
    else
      rwp[idx2d] = value_rwp;
      
    if( value_iwp > 0 )   
      iwp[idx2d] = value_iwp / IWP_SCAL;
    else
      iwp[idx2d] = value_iwp;
      
    if( value_lwp > 0 )   
      lwp[idx2d] = value_lwp / LWP_SCAL;
    else
      lwp[idx2d] = value_lwp;
      
    if( value_swp > 0 )  
      swp[idx2d] = value_swp / SWP_SCAL;
    else
      swp[idx2d] = value_swp;
    
   //std::cout<<"gwp = "<<std::endl;
   //std::cout<<value_gwp<<std::endl;

    if( value_gwp > 0 )   
      gwp[idx2d] = value_gwp / GWP_SCAL;
    else
      gwp[idx2d] = value_gwp;

    if( value_rr > 0 )   
      rr[idx2d] = value_rr / RR_SCAL;
    else
      rr[idx2d] = value_rr;

    if( value_sfr > 0 )
       sfr[idx2d] = value_sfr / SFR_SCAL;
     else
       sfr[idx2d] = value_sfr;
      
    if( value_swe > 0 )   
      swe[idx2d]  = value_swe / SWE_SCAL;
    else
      swe[idx2d]  = value_swe;
      
    if( value_snow > 0 )   
      snow[idx2d] = value_snow / SNOW_SCAL;
    else
      snow[idx2d] = value_snow;
     
    if( value_sice > 0 )   
      sice[idx2d] = value_sice / SICE_SCAL;
    else
      sice[idx2d] = value_sice / SICE_SCAL;
    
    if( value_sicemy > 0 )   
      sice_my[idx2d] = value_sicemy / SICE_SCAL;
    else
      sice_my[idx2d] = value_sicemy;
    
    if( value_sicefy > 0 )   
      sice_fy[idx2d] = value_sicefy / SICE_SCAL; 
    else
      sice_fy[idx2d] = value_sicefy; 
    
    if( value_snowgs > 0 )
      snowgs[idx2d]  = value_snowgs * SNOWGS_SCAL;
    else
      snowgs[idx2d]  = value_snowgs;
      
    /*
    if( value_cldtop> 0 )   
      cldtop[idx2d] = value_cldtop / CLDTOP_SCAL; 
    else
      cldtop[idx2d] = value_cldtop; 
    
    if( value_cldbase > 0 )   
      cldbase[idx2d] = value_cldbase / CLDBASE_SCAL;
    else
      cldbase[idx2d] = value_cldbase;
    
    if( value_cldthick > 0 )   
      cldthick[idx2d] = value_cldthick / CLDTHICK_SCAL;
    else
      cldthick[idx2d] = value_cldthick;
      
    if( value_sm > 0 )   
      surfm[idx2d]   = value_sm / SURFM_SCAL; 
    else
      surfm[idx2d]   = value_sm; 
    
    if( value_windsp > 0 )   
      windsp[idx2d]  = value_windsp / WINDSP_SCAL;
    else
      windsp[idx2d]  = value_windsp;
    
    winddir[idx2d] = value_winddir / WINDDIR_SCAL; 
    windu[idx2d]   = value_u / WINDU_SCAL; 
    windv[idx2d]   = value_v / WINDV_SCAL;  
    */
    
    
    /* Those variables are not retrieved in DEP, */
      
    //sfr[idx2d]      = NAVA; 
    cldtop[idx2d]     = NAVA; 
    cldbase[idx2d]    = NAVA;
    cldthick[idx2d]   = NAVA;
    preciptype[idx2d] = NAVA;
    rflag[idx2d]      = NAVA;

    surfm[idx2d]      = NAVA;
    windsp[idx2d]     = NAVA;
    winddir[idx2d]    = NAVA;
    windu[idx2d]      = NAVA;
    windv[idx2d]      = NAVA;
    
   
    if( itype == 1 ) {
      fread(&dummy, 4, 1, mirsd);
      fread(&char_nIter, 4, 1, mirsd);
      fread(&char_chisq, 4, 1, mirsd);
      fread(&dummy, 4, 1, mirsd);
      //printf("iprf=%i, nIter=%i, chisq=%g\n", iprf, intSwap(char_nIter), floatSwap(char_chisq) ); 
    } 

      if (alg > alg_num_sfr) {
       // char char_prob_sf[4], char_prob_rr[4];
       fread(&dummy, 4, 1, mirsd);
       fread(&char_prob_sf, 4, 1, mirsd);
       fread(&char_prob_rf, 4, 1, mirsd);
       fread(&dummy, 4, 1, mirsd);

       value_prob_sf = floatSwap(char_prob_sf);
       value_prob_rf = floatSwap(char_prob_rf);
       //extern float      prob_sf[MAXSCAN][MAXFOV];
       //extern float      prob_rr[MAXSCAN][MAXFOV];
       prob_sf[idx2d] = (short) (value_prob_sf / SFR_PROB_SF_SCAL);
       prob_rf[idx2d] = (short) (value_prob_rf / SFR_PROB_RF_SCAL);
     } else {
       prob_sf[idx2d] = MISSING;
       prob_rf[idx2d] = MISSING;
     }

 
  } //end loop of iprf


 
  // compute qc percentage rate
  if( nprf > 0 ) {
    percentage_optimal_retrievals[0] = (float)qc0 / (float)nprf;
    percentage_suboptimal_retrievals[0] = (float)qc1 / (float)nprf;
    percentage_bad_retrievals[0] = (float)qc2 / (float)nprf;
  }
  
  return;

} /* end of rmirs_dep.c */
