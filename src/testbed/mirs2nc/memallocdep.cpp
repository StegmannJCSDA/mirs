/**********************************************************************
 *  Program Name      : MemAllocDEP.cpp
 *  Type              : Subroutine
 *  Function          : Allocation of memory for DEP related variables
                      : Initialization of arrays
 *  Input Files       :
 *  Output Files      : 
 *  Subroutine Called :
 *  Called by         : rmirs_dep.cpp
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *
 *  02/2020         v1             Ming Fang
 *********************************************************************/
#include <omp.h>
#include "swath.h"
using namespace std;

void mirs2nc_class::MemAllocDEP(){
  /*DEP products *******/
  
  //Memory allocation
  
  int size2d = MAXSCAN*MAXFOV;
  int size3d = size2d*4;
  qc_dep = new int[size3d];
  
  iTypAtm = new short int[size2d];
  iTypSfc = new short int[size2d];
  prob_sf = new short int[size2d];
  prob_rf = new short int[size2d];
  
  tpw = new short int[size2d];
  clw = new float[size2d];
  rwp = new float[size2d];
  lwp = new float[size2d];
  swp = new short int[size2d];
  iwp = new short int[size2d];
  gwp = new float[size2d];
  rr  = new float[size2d];
  swe = new short int[size2d]; 
  snowgs  = new short int[size2d];
  snow    = new short int[size2d];
  sice    = new short int[size2d];
  sice_my = new short int[size2d];
  sice_fy = new short int[size2d];

  sfr         = new short int[size2d];
  cldtop      = new short int[size2d];
  cldbase     = new short int[size2d];
  cldthick    = new short int[size2d];
  preciptype  = new short int[size2d];
  rflag       = new short int[size2d];

  surfm   = new short int[size2d];
  windsp  = new short int[size2d];
  winddir = new short int[size2d];
  windu   = new short int[size2d];
  windv   = new short int[size2d];
  
  //Initilization of variables 
  
  #pragma omp parallel for num_threads(8)
  for(int i=0; i<size3d; i++)
  {  qc_dep[i] = MISSING;}


  #pragma omp parallel for num_threads(8)
  for(int i=0; i<size2d; i++) {
    iTypAtm[i] = MISSING;
    iTypSfc[i] = MISSING;
    prob_sf[i] = MISSING;
    prob_rf[i] = MISSING;
  
    tpw[i]     = MISSING;
    clw[i]     = MISSING;
    rwp[i]     = MISSING;
    lwp[i]     = MISSING;
    swp[i]     = MISSING;
    iwp[i]     = MISSING;
    gwp[i]     = MISSING;
    rr[i]      = MISSING;
    swe[i]     = MISSING;  
    snowgs[i]  = MISSING;
    snow[i]    = MISSING;
    sice[i]    = MISSING;
    sice_my[i] = MISSING;
    sice_fy[i] = MISSING;
  
    sfr[i]         = MISSING;
    cldtop[i]      = MISSING;
    cldbase[i]     = MISSING;
    cldthick[i]    = MISSING;
    preciptype[i]  = MISSING;
    rflag[i]       = MISSING;
  
    surfm[i]       = MISSING;
    windsp[i]      = MISSING;
    winddir[i]     = MISSING;
    windu[i]       = MISSING;
    windv[i]       = MISSING;
  }
}

void mirs2nc_class::MemDeleteDEP(){

//   if ( qc_dep    ) delete[] qc_dep;
//   if ( iTypAtm   ) delete[] iTypAtm;
//   if ( iTypSfc   ) delete[] iTypSfc;
//   if ( prob_sf   ) delete[] prob_sf;
//   if ( prob_rf   ) delete[] prob_rf;
//   if ( tpw       ) delete[] tpw;
//   if ( clw       ) delete[] clw;
//   if ( rwp       ) delete[] rwp;
//   if ( lwp       ) delete[] lwp;
//   if ( swp       ) delete[] swp;
//   if ( iwp       ) delete[] iwp;
//   if ( gwp       ) delete[] gwp;
//   if ( rr        ) delete[] rr;
//   if ( swe       ) delete[] swe;
//   if ( snowgs    ) delete[] snowgs;
//   if ( snow      ) delete[] snow;
//   if ( sice      ) delete[] sice;
//   if ( sice_my   ) delete[] sice_my;
//   if ( sice_fy   ) delete[] sice_fy;
//   if ( sfr       ) delete[] sfr;
//   if ( cldtop    ) delete[] cldtop;
//   if ( cldbase   ) delete[] cldbase;
//   if ( cldthick  ) delete[] cldthick;
//   if ( preciptype) delete[] preciptype;
//   if ( rflag     ) delete[] rflag;
//   if ( surfm     ) delete[] surfm;
//   if ( windsp    ) delete[] windsp;
//   if ( winddir   ) delete[] winddir;
//   if ( windu     ) delete[] windu;
//   if ( windv     ) delete[] windv;

}


