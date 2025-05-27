/**********************************************************************
 *  Program Name      : MemAllocEDR.cpp
 *  Type              : Subroutine
 *  Function          : Allocation of memory for EDR related variables
                      : Initialization of arrays
 *  Input Files       :
 *  Output Files      : 
 *  Subroutine Called :
 *  Called by         : rmirs_edr.cpp
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *
 *  02/2020         v1             Ming Fang
 *********************************************************************/
#include <omp.h>
#include "swath.h"

void mirs2nc_class::MemAllocEDR(){
  //Memory allocation
  year      = new short int[MAXSCAN];
  doy       = new short int[MAXSCAN]; 
  month     = new short int[MAXSCAN];
  dom       = new short int[MAXSCAN];
  hour      = new short int[MAXSCAN];
  minute    = new short int[MAXSCAN];
  second    = new short int[MAXSCAN];
  orb_mode  = new short int[MAXSCAN];
  time_utc  = new double[MAXSCAN];
  
  int size2d = MAXSCAN*MAXFOV;
  nattempt  = new short int[size2d];
  niter     = new short int[size2d];
  tskin     = new float[size2d];
  surfp     = new short int[size2d];
  qc        = new int[size2d*4];
  chisq     = new float[size2d];
  lat       = new float[size2d];
  lon       = new float[size2d];
  angle     = new float[size2d];
  rel_azi   = new float[size2d];
  sza       = new float[size2d];
  
  int size3d = size2d*MAXLAY;
  ptemp     = new float[size3d];
  pvapor    = new float[size3d];
  pozone    = new float[size3d];
  pclw      = new float[size3d];
  prain     = new float[size3d];
  pgraupel  = new float[size3d];
  psnow     = new float[size3d];
  pice      = new float[size3d];
  
  player    = new float[MAXLAY];
  plevel    = new float[MAXLAY+1];
  freq      = new float[MAXCH]; 
  polo      = new short int[MAXCH]; 

  int size3d_ch= size2d*MAXCH;
  bt        = new short int[size3d_ch];
  ym        = new short int[size3d_ch];
  yfwd      = new short int[size3d_ch];
  chanSel   = new short int[size3d_ch];
  emis      = new short int[size3d_ch];
   
  // Initilization of variables 
  #pragma omp parallel for num_threads(9)
  for(int i=0; i<MAXSCAN; i++){
    year[i]     = MISSING;
    month[i]    = MISSING;
    dom[i]      = MISSING;
    hour[i]     = MISSING;
    minute[i]   = MISSING;
    second[i]   = MISSING;
    doy[i]      = MISSING;
    orb_mode[i] = MISSING;
    time_utc[i] = MISSING;
  }
  
  #pragma omp parallel for num_threads(8)
  for(int i=0; i<MAXLAY; i++){
    player[i]   = MISSING;
    plevel[i]   = MISSING;
  }
  plevel[MAXLAY]= MISSING;
  
  #pragma omp parallel for num_threads(8)
  for(int i=0; i<MAXCH; i++) {
    freq[i] = MISSING;
    polo[i] = MISSING;
  }
  
  #pragma omp parallel for num_threads(10)
  for(int i=0; i<size2d; i++){
    nattempt[i] = MISSING;
    niter[i]    = MISSING;
    chisq[i]    = MISSING;
    lat[i]      = MISSING;
    lon[i]      = MISSING;
    angle[i]    = MISSING;
    rel_azi[i]  = MISSING;
    sza[i]      = MISSING;
    tskin[i]    = MISSING;
    surfp[i]    = MISSING;
  }
  
  #pragma omp parallel for num_threads(8)
  for(int i=0; i<size3d; i++){
    ptemp[i]    = MISSING;
    pvapor[i]   = MISSING;
    pozone[i]   = MISSING;
    pclw[i]     = MISSING;
    prain[i]    = MISSING;
    pgraupel[i] = MISSING;
    psnow[i]    = MISSING;
    pice[i]     = MISSING;
  }
  
  int size_bt = size2d*MAXCH;
  #pragma omp parallel for num_threads(5)
  for(int i=0; i<size_bt; i++){
    emis[i]     = MISSING;
    bt[i]       = MISSING;
    ym[i]       = MISSING;
    yfwd[i]     = MISSING;
    chanSel[i]  = MISSING;
  }
  
//  int size_qc = size2d*4;
  for(int i=0; i<4; i++){
    qc[i]       = MISSING;
  }

}

void mirs2nc_class::MemDeleteEDR(){
//
//   if ( year      ) delete[] year;
//   if ( doy       ) delete[] doy;
//   if ( month     ) delete[] month;
//   if ( dom       ) delete[] dom;
//   if ( hour      ) delete[] hour;
//   if ( minute    ) delete[] minute;
//   if ( second    ) delete[] second;
//   if ( orb_mode  ) delete[] orb_mode;
//   if ( time_utc  ) delete[] time_utc;
//   if ( nattempt  ) delete[] nattempt;
//   if ( niter     ) delete[] niter;
//   if ( tskin     ) delete[] tskin;
//   if ( surfp     ) delete[] surfp;
//   if ( qc        ) delete[] qc;
//   if ( chisq     ) delete[] chisq;
//   if ( lat       ) delete[] lat;
//   if ( lon       ) delete[] lon;
//   if ( angle     ) delete[] angle;
//   if ( rel_azi   ) delete[] rel_azi;
//   if ( sza       ) delete[] sza;
//   if ( ptemp     ) delete[] ptemp;
//   if ( pvapor    ) delete[] pvapor;
//   if ( pozone    ) delete[] pozone;
//   if ( pclw      ) delete[] pclw;
//   if ( prain     ) delete[] prain;
//   if ( pgraupel  ) delete[] pgraupel;
//   if ( psnow     ) delete[] psnow;
//   if ( pice      ) delete[] pice;
//   if ( player    ) delete[] player;
//   if ( plevel    ) delete[] plevel;
//   if ( freq      ) delete[] freq;
//   if ( polo      ) delete[] polo;
//   if ( bt        ) delete[] bt;
//   if ( ym        ) delete[] ym;
//   if ( yfwd      ) delete[] yfwd;
//   if ( chanSel   ) delete[] chanSel;
//   if ( emis      ) delete[] emis;
//
}

