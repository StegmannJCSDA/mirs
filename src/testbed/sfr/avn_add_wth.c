/**************************************************************************
 *  Program Name      : avn_add_wth.c
 *  Type              : Subroutine
 *  Function          : Weather model snow detection model 
 *  Input Files       : None
 *  Output Files      : None
 *  Subroutine Called : wth_cloud()
 *  Called by         : snowfall.c
 **************************************************************************/

#include "stdio.h"
#include "math.h"

#define     T0                  273.15
#define     CTOP_HI_LIMIT       5000
#define     CLOUD_LOW_LIMIT     0.0
#define     MINRH               89.0

// Weather model coefficients for thick cloud
#define     B0      -8.129    // Constant
#define     B1      0.044     // RH at surface 
#define     B2      0.001     // Cthick
#define     B3      0.028     // RH at 1 km 
#define     B4      0.007     // RH at 2 km 
#define     B5      -0.008    // RH at 3 km 
#define     B6      0.000     // Vvel at 2 km 
#define     B7      -0.001    // Vvel at 3 km 

// Weather model coefficients for shallow cloud
#define     C0      -7.726    // Constant
#define     C1      0.039     // RH at surface 
#define     C2      0.001     // Cthick
#define     C3      0.028     // RH at 1 km 
#define     C4      0.006     // RH at 2 km 
#define     C5      -0.009    // RH at 3 km 
#define     C6      0.001     // Vvel at 2 km 
#define     C7      -0.001    // Vvel at 3 km 

#define     CLOUDTHICK    3500 
#define     NUMLV         17
#define     LARGE         1.E4
#define     NIVAR       6



int wth_cloud(float elev1, float rh2m, float *hgt0, float *temp0, 
        float *rh0, float *vvel0, int nlev,  
        float *maxtemp1p, float *cthick1p, float *sdprob_e1p);



int avn_add_wth(float **avar, float **ti, float *nti, int nlev, float rot)
{
    float   elev1, rh2m;
    float   hgt0[nlev], temp0[nlev], rh0[nlev], vvel0[nlev];

    float   maxtemp, cthick, prob_e;

    int     i, jj, i0[NIVAR], ni1, ii;

    ni1 = round(nti[0]);
    int     i1[ni1];

    for (i=0; i<ni1; i++)       i1[i] = round(ti[i][0]);
    i0[0] = round(ti[0][1]);
    i0[1] = round(ti[0][2]);
    i0[2] = round(ti[0][3]);
    i0[3] = round(ti[0][5]);
    i0[4] = round(ti[0][7]);
    i0[5] = round(ti[0][9]);

    //for (i=0; i<ni1; i++)       printf("%d ", i1[i]);
    //printf("\n");
    //for (i=0; i<NIVAR; i++)     printf("%d ", i0[i]);
    //printf("\n");


    // first
    jj = 1;

    elev1 = avar[i0[0]][jj];
    rh2m = avar[i0[1]][jj];

    for (i=0; i<nlev; i++)  {
        hgt0 [i] = avar[i0[2]+i][jj];
        temp0[i] = avar[i0[3]+i][jj];
        rh0  [i] = avar[i0[4]+i][jj];
        vvel0[i] = avar[i0[5]+i][jj];
    }

    //printf("next wth_cloud\n");
    wth_cloud(elev1, rh2m, hgt0, temp0, rh0, vvel0, nlev, 
            &maxtemp, &cthick, &prob_e);

    avar[i1[0]][jj] = 1.0;
    avar[i1[1]][jj] = maxtemp;
    avar[i1[2]][jj] = cthick;
    avar[i1[3]][jj] = prob_e;


    // second
    jj = 2;

    rh2m = avar[i0[1]][jj];

    for (i=0; i<nlev; i++)  {
        hgt0 [i] = avar[i0[2]+i][jj];
        temp0[i] = avar[i0[3]+i][jj];
        rh0  [i] = avar[i0[4]+i][jj];
        vvel0[i] = avar[i0[5]+i][jj];
    }

    wth_cloud(elev1, rh2m, hgt0, temp0, rh0, vvel0, nlev, 
            &maxtemp, &cthick, &prob_e);

    avar[i1[0]][jj] = 1.0;
    avar[i1[1]][jj] = maxtemp;
    avar[i1[2]][jj] = cthick;
    avar[i1[3]][jj] = prob_e;


    // interpted at fov
    jj = 0;
    for (i=0; i<ni1; i++)  {
        ii = i1[i];
        avar[ii][jj] = (1-rot)*avar[ii][1] + rot*avar[ii][2];
    }

    return(0);
}



int wth_cloud(float elev1, float rh2m, float *hgt0, float *temp0, 
        float *rh0, float *vvel0, int nlev, 
        float *maxtemp1p, float *cthick1p, float *sdprob_e1p)
{
    float   rh[NUMLV], hgt[NUMLV], temp[NUMLV], vvel[NUMLV];
    float   maxtemp1, cthick1, prob_e1;
    //int    sdprob_e1;

    int     ilv, ilv0, ilv03, ilv1, ilv2, ilv3;
    float   minh03, minh1, minh2, minh3;
    float   cflag[NUMLV];

    
    // Environment model at the first GFS time
    for (ilv=nlev-1; ilv>nlev-NUMLV-1; ilv--)  {
        hgt [nlev-1-ilv] = hgt0[ilv];
        temp[nlev-1-ilv] = temp0[ilv];
        rh  [nlev-1-ilv] = rh0[ilv];
        vvel[nlev-1-ilv] = vvel0[ilv];
    }

    minh03 = minh1 = minh2 = minh3 = LARGE;
    ilv0 = ilv03 = ilv1 = ilv2 = ilv3 = 0;

    // Find the level closest to the surface
    for (ilv=0; ilv<NUMLV; ilv++)  {
        if (hgt[ilv] >= elev1)  {
            ilv0 = ilv;
            break;
        }
    }

    // Find the levels closest to 1, 2, and 3 km
    for (ilv=ilv0; ilv<NUMLV; ilv++)  {
        if (fabs(hgt[ilv]-elev1-300.) < minh03)  {
            ilv03 = ilv;
            minh03 = fabs(hgt[ilv]-elev1-300.);
        }
        if (fabs(hgt[ilv]-elev1-1000.) < minh1)  {
            ilv1 = ilv;
            minh1 = fabs(hgt[ilv]-elev1-1000.);
        }
        if (fabs(hgt[ilv]-elev1-2000.) < minh2)  {
            ilv2 = ilv;
            minh2 = fabs(hgt[ilv]-elev1-2000.);
        }
        if(fabs(hgt[ilv]-elev1-3000.) < minh3)  {
            ilv3 = ilv;
            minh3 = fabs(hgt[ilv]-elev1-3000.);
        }
    }

    // Find the maximum temperature from hundreds of meters to 3 km
    maxtemp1 = -50.;
    for (ilv=ilv03+1; ilv<ilv3+1; ilv++)  {
        if (temp[ilv] > maxtemp1)
            maxtemp1 = temp[ilv];
    }
    maxtemp1 = maxtemp1 - T0;

    // Check conditions for the existence of cloud
    for (ilv = ilv0; ilv < NUMLV; ilv++)  {
        cflag[ilv] = 0;
        if ((rh[ilv]>=MINRH) 
                && ((hgt[ilv]-elev1)>=CLOUD_LOW_LIMIT) 
                && ((hgt[ilv]-elev1) <= CTOP_HI_LIMIT))
            cflag[ilv] = 1;
    }

    cthick1 = 0;

    for (ilv = ilv0+1; ilv < NUMLV; ilv++)  {

        // First layer of cloud above surface
        if (cthick1 < 1. && cflag[ilv] == 1 
                && (hgt[ilv]-elev1) <= CTOP_HI_LIMIT)  {
            if (cflag[ilv-1] == 1)
                cthick1 = hgt[ilv] - hgt[ilv-1];
            else //if(cflag[ilv-1] == 0)
                cthick1 = (hgt[ilv]-hgt[ilv-1]) * (rh[ilv]-MINRH)
                    / (rh[ilv] - rh[ilv-1]);
        }

        // More layers of cloud
        else if (cthick1>1. && (hgt[ilv]-elev1)<=CTOP_HI_LIMIT)  {
            if (cflag[ilv] == 1 && cflag[ilv-1] == 1)
                cthick1 += hgt[ilv] - hgt[ilv-1];
            else if(cflag[ilv] == 1 && cflag[ilv-1] == 0)
                cthick1 += (hgt[ilv] - hgt[ilv-1])*(rh[ilv]-MINRH)
                    /(rh[ilv] - rh[ilv-1]);
            else if(cflag[ilv] == 0 && cflag[ilv-1] == 1)
                cthick1 += (hgt[ilv]-hgt[ilv-1]) * (MINRH-rh[ilv-1])
                    / (rh[ilv] - rh[ilv-1]);
        }

    } // end of cthickness calculation

    // Snowfall probability from environmental conditions
    // at the first GFS time
    if (cthick1 >= CLOUDTHICK)  {
        prob_e1 = B0 + B1 * rh2m  
            + B2 * cthick1 + B3 * rh[ilv1] + B4 * rh[ilv2] 
            + B5 * rh[ilv3] + B6 * 1000*vvel[ilv2] 
            + B7 * 1000 * vvel[ilv3];
        prob_e1 = exp(prob_e1)/(1. + exp(prob_e1));
    }
    else  {
        prob_e1 = C0 + C1 * rh2m 
            + C2 * cthick1 + C3 * rh[ilv1] + C4 * rh[ilv2] 
            + C5 * rh[ilv3] + C6 * 1000*vvel[ilv2] + 
            C7 * 1000 * vvel[ilv3];
        prob_e1 = exp(prob_e1)/(1. + exp(prob_e1));
    }

    //sdprob_e1 = prob_e1 * 100 + fix;

    *maxtemp1p = maxtemp1;
    *cthick1p = cthick1;
    *sdprob_e1p = prob_e1;

    return(0);
}







