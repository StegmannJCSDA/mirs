/**************************************************************************
 *  Program Name      : rd_mhs.c
 *  Type              : subroutine
 *  Function          : Read mhs data
 *  Input data        : mhs l1b sensor data
 *  Output data       : satellite variables to use in retrieval
 *  Subroutine Called : rmhs_hd(), rmhs_scn(), rama_hd(), rama_scn(), ...
 *  Called by         : sat_data()
 *  Author            : Jun Dong, jundong@umd.edu, 03/19/2020
 **************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#include "d_sys.h"
#include "d_darrc.h"
#include "d_jsec.h"

#define     LON_MIN     -400
#define     LON_MAX     400
#define     LAT_MIN     -100
#define     LAT_MAX     100
#define     AT_MIN      50
#define     AT_MAX      500
#define     YEAR_MIN    1801
#define     YEAR_MAX    2055


// mhs header
typedef struct  {    
    char                dummy1[84];
    unsigned short      start_year;
    unsigned short      start_day_of_year;
    unsigned int        start_milliseconds_of_day;
    char                dummy2[4];
    unsigned short      end_year;
    unsigned short      end_day_of_year;
    unsigned int        end_milliseconds_of_day;
    char                dummy3[28];
    unsigned short      last_scan_record;
    char                dummy4[282];
    unsigned int        t_r_central_wave_number_1;
    char                dummy5[8];
    unsigned int        t_r_central_wave_number_2;
    char                dummy6[8];
    unsigned int        t_r_central_wave_number_3;
    char                dummy7[8];
    unsigned int        t_r_central_wave_number_4;
    char                dummy8[8];
    unsigned int        t_r_central_wave_number_5;
    char                dummy9[44];
    unsigned short      orbit_vector_epoch_year;
    unsigned short      orbit_vector_day_of_year;
    unsigned int        orbit_vector_utc_time_of_day;
    int                 semimajor_axis;
    int                 eccentricity;
    int                 inclination;
    int                 argument_of_perigee;
    int                 right_ascension;
    int                 mean_anomaly;
    char                dummy10[2528];
} HBLOCK_M;    

// mhs scanline
typedef struct  {    
    char                dummy1[8];
    unsigned int        time_of_day_of_scan;
    unsigned short      orbit_node;
    char                dummy2[10];
    unsigned int        quality_indicator_bit_field;
    char                dummy3[32];
    int                 pri_cal_coeffs[5][3];
    char                dummy4[92];
    short               angular_relationships[270];
    int                 earth_location[180];
    char                dummy5[8];
    unsigned short      earth_view_data[540];
    char                dummy6[512];
} SCANLINE_M;    

// amsua header
typedef struct  {
    char                dummy1[84];
    unsigned short      start_year;
    unsigned short      start_day_of_year;
    unsigned int        start_milliseconds_of_day;
    char                dummy2[4];
    unsigned short      end_year;
    unsigned short      end_day_of_year;
    unsigned int        end_milliseconds_of_day;
    char                dummy3[40];
    unsigned short      last_scan_record;
    char                dummy4[542];
    int                 t_r_central_wave_number_1;
    char                dummy5[8];
    int                 t_r_central_wave_number_2;
    char                dummy6[8];
    int                 t_r_central_wave_number_3;
    char                dummy7[8];
    int                 t_r_central_wave_number_4;
    char                dummy8[8];
    int                 t_r_central_wave_number_5;
    char                dummy9[8];
    int                 t_r_central_wave_number_6;
    char                dummy10[8];
    int                 t_r_central_wave_number_7;
    char                dummy11[8];
    int                 t_r_central_wave_number_8;
    char                dummy12[8];
    int                 t_r_central_wave_number_9;
    char                dummy13[8];
    int                 t_r_central_wave_number_10;
    char                dummy14[8];
    int                 t_r_central_wave_number_11;
    char                dummy15[8];
    int                 t_r_central_wave_number_12;
    char                dummy16[8];
    int                 t_r_central_wave_number_13;
    char                dummy17[8];
    int                 t_r_central_wave_number_14;
    char                dummy18[8];
    int                 t_r_central_wave_number_15;
    char                dummy19[40];
    unsigned short      orbit_vector_epoch_year;
    unsigned short      orbit_vector_day_of_year;
    unsigned int        orbit_vector_utc_time_of_day;
    int                 semimajor_axis;
    int                 eccentricity;
    int                 inclination;
    int                 argument_of_perigee;
    int                 right_ascension;
    int                 mean_anomaly;
    char                dummy20[1628];
} HBLOCK_A;

// amsua scanline
typedef struct  {
  char                  dummy1[8];
  unsigned int          time_of_day_of_scan;
  unsigned short        orbit_node;
  char                  dummy2[10];
  unsigned int          quality_indicator_bit_field;
  char                  dummy3[52];
  int                   pri_cal_coeffs[15][3];
  char                  dummy4[212];
  short                 angular_relationships[90];
  int                   earth_location[60];
  char                  dummy5[12];
  unsigned short        scene_telemetry_a1[510];
  char                  dummy6[268];
  unsigned short        scene_telemetry_a2[120];
  char                  dummy7[128];
} SCANLINE_A;


void rmhs_hd();
void rama_hd();

void rmhs_scn();
void rama_scn();
void rmhs_scn0();
void rama_scn0();

void rad2at_m(double, float, double*);
void rad2at_a(float, float, float*);

void swap_hblock_m();
void swap_hblock_a();
void swap_scanline_m();
void swap_scanline_a();

void reverse_bytes();

int ma_indse(long long*, long long*, int*, int*, int*, int*);
//int wrt_fmbin();

// sat data
extern  float       **lon, **lat, ***at, **lza, **sza;
extern  char        **obmd;
extern  long long   **btjs;
extern  float       *lon_p1, *lat_p1, *at_p1, *lza_p1, *sza_p1;
extern  char        *obmd_p1;
extern  long long   *btjs_p1;
// extra
extern  int         dtse[2][6];
extern  float       **dsec, *dsec_p1;
extern  float       *hd_sqc, **ti_sqc;

//extern  float       **sfr, **prob;
//extern  float       *sfr_p1, *prob_p1;



// sfr shin method
int rd_mhs_l1b(char *ffn0, char *ffn1, char *ftype, 
        int *nscanp, int *nfovp, int *nchanp, int **nfovap, int **nfova_p1p, 
        int ***flag0p, int **flag0_p1p, int nyref, int **lese_tab)
{
    FILE    *fp_m, *fp_a;

    float   *cwn_m, *cwn_a;
    int     nscan_m, nfov_m, nchan_m, bged_m;
    int     nscan_a, nfov_a, nchan_a, bged_a;
    int     nscan, nfov, nchan;

    int     *nfova, *nfova_p1;
    int     **flag0, *flag0_p1;

    int         i, iscan, ifov, ichan, tt;
    long long   js01m[2], js01a[2];
    double      djs01m[2], djs01a[2];
    double      js0mf, js1mf, js0af, js1af;
    int         njsm1, njsa1;
    int         im0, nim, ia0, nia;
    long long   js0mf1, js0af1;

    //printf("----------------------------------------\n");
    //printf("ffn0: %s\nffn1: %s\n", ffn0, ffn1);

    // open data files
    fp_m = fopen(ffn0, "r");
    if (fp_m == NULL)  {
        printf("ERROR: Fail to open file (stop): %s\n", ffn0);
        exit(1);
    }

    fp_a = fopen(ffn1, "r");
    if (fp_a == NULL)  {
        printf("ERROR: Fail to open file (stop): %s\n", ffn1);
        exit(1);
    }

    // read both file header for information
    rmhs_hd(fp_m, &bged_m, &cwn_m, &nscan_m, &nfov_m, &nchan_m, 
            nyref, lese_tab, js01m, djs01m);
    printf("m: nscan, nfov, nchan: %d, %d, %d\n", nscan_m, nfov_m, nchan_m);
    //for (i=0; i<nchan_m; i++)  printf("%f  ", cwn_m[i]*30);  printf("\n");
    //printf("js01m: %lld, %lld\n", js01m[0], js01m[1]);
    //printf("djs01m: %lf, %lf\n", djs01m[0], djs01m[1]);

    rama_hd(fp_a, &bged_a, &cwn_a, &nscan_a, &nfov_a, &nchan_a,
            nyref, lese_tab, js01a, djs01a);
    printf("a: nscan, nfov, nchan: %d, %d, %d\n", nscan_a, nfov_a, nchan_a);
    //for (i=0; i<nchan_a; i++)  printf("%f  ", cwn_a[i]*30);  printf("\n");
    //printf("js01a: %lld, %lld\n", js01a[0], js01a[1]);
    //printf("djs01a: %lf, %lf\n", djs01a[0], djs01a[1]);

    // start/end time (float)
    js0mf = js01m[0];    js0mf = js0mf + djs01m[0];
    js1mf = js01m[1];    js1mf = js1mf + djs01m[1];
    njsm1 = round((js1mf-js0mf)/(8.0/3)) + 1;

    js0af = js01a[0];    js0af = js0af + djs01a[0];
    js1af = js01a[1];    js1af = js1af + djs01a[1];
    njsa1 = round((js1af-js0af)/(8.0)) + 1;

    // start/end ind
    ma_indse(js01m, js01a, &im0, &nim, &ia0, &nia);
    printf("im0, nim, ia0, nia: %d, %d, %d, %d\n", im0, nim, ia0, nia);


    // Initialize
    nscan = nim;
    im0 = im0;
    nfov = nfov_m;
    nchan = nchan_m + nchan_a;
    //printf("nscan, nfov, nchan: %d, %d, %d\n", nscan, nfov, nchan);

    btjs_p1 = init_2dc_llint(nscan, nfov, &btjs);
    dsec_p1 = init_2dc_float(nscan, nfov, &dsec);
    lon_p1  = init_2dc_float(nscan, nfov, &lon);
    lat_p1  = init_2dc_float(nscan, nfov, &lat);
    obmd_p1 = init_2dc_char (nscan, nfov, &obmd);
    lza_p1  = init_2dc_float(nscan, nfov, &lza);
    sza_p1  = init_2dc_float(nscan, nfov, &sza);
    at_p1   = init_3dc_float(nscan, nfov, nchan, &at);

    for (iscan=0; iscan<nscan; iscan++)  {
        for (ifov=0; ifov<nfov; ifov++)  {
            btjs[iscan][ifov] = -999;
            dsec[iscan][ifov] = -999.0;
            lon[iscan][ifov]  = -999.0;
            lat[iscan][ifov]  = -999.0;
            obmd[iscan][ifov] = -99;
            lza[iscan][ifov]  = -999.0;
            sza[iscan][ifov]  = -999.0;
            for (ichan=0; ichan<nchan_m+nchan_a; ichan++)
                at[iscan][ifov][ichan] = -999.0;
        }
    }

    flag0_p1= init_2dc_int (nscan, nfov, &flag0);

    nfova_p1 = init_1dc_int(nchan, &nfova);
    for (i=0; i<nchan; i++)    nfova[i] = nfov;

    *nscanp = nscan;    *nfovp  = nfov;    *nchanp = nchan;
    *nfovap = nfova;    *nfova_p1p = nfova_p1;
    *flag0p = flag0;    *flag0_p1p = flag0_p1;


    // read both data
    js0mf1 = js0mf + im0*8.0/3.0;
    js0af1 = js0af + ia0*8.0;

    rmhs_scn(fp_m, &bged_m, cwn_m, nscan_m, nfov_m, nchan_m, 
            nscan_a, nfov_a, nchan_a, nscan, js0mf1, nim);

    rama_scn(fp_a, &bged_a, cwn_a, nscan_a, nfov_a, nchan_a, 
            nscan_m, nfov_m, nchan_m, nscan, js0af1, nia);


    // remove first/last 2 scanline
    for (iscan=0; iscan<nscan-2; iscan++)  {
        for (ifov=0; ifov<nfov; ifov++)  {
            btjs[iscan][ifov] = btjs[iscan+2][ifov];
            dsec[iscan][ifov] = dsec[iscan+2][ifov];
            lon[iscan][ifov]  = lon[iscan+2][ifov];
            lat[iscan][ifov]  = lat[iscan+2][ifov];
            obmd[iscan][ifov] = obmd[iscan+2][ifov];
            lza[iscan][ifov]  = lza[iscan+2][ifov];
            sza[iscan][ifov]  = sza[iscan+2][ifov];
            for (ichan=0; ichan<nchan_a+nchan_m; ichan++)
                at[iscan][ifov][ichan] = at[iscan+2][ifov][ichan];
        }
    }
    nscan = nscan - 4;
    *nscanp = nscan;


    // qc
    for (iscan=0; iscan<nscan; iscan++)  {
        for (ifov=0; ifov<nfov; ifov++)  {
            flag0[iscan][ifov] = 0;
            // time
            if (btjs[iscan][ifov] < 0)  {
                flag0[iscan][ifov] |= (1U<<8);
            }
            // location
            if (lon[iscan][ifov]<LON_MIN || lon[iscan][ifov]>LON_MAX 
                    || lat[iscan][ifov]<LAT_MIN || lat[iscan][ifov]>LAT_MAX 
                    || (lon[iscan][ifov]==0 && lat[iscan][ifov]==0))  {
                flag0[iscan][ifov] |= (1U<<9);
                //printf("lon,lat:%f,%f\n",lon[iscan][ipos],lat[iscan][ipos]);
            }
            // at
            for (ichan=0; ichan<nchan; ichan++)  {
                tt = round(ti_sqc[0][ichan]);
                if (tt == 1)  {
                    if (at[iscan][ifov][ichan]<AT_MIN 
                            || at[iscan][ifov][ichan]>AT_MAX)  {
                        flag0[iscan][ifov] |= (1U<<10);
                        //printf(" N");
                    }
                }
            }  // ichan
        }  // ifov
    }  // iscan

    // save for check: fmbin and at
    //FILE    *fp1;
    //sfr_p1 = init_2dc_float(nscan, nfov_m, &sfr);
    //prob_p1 = init_2dc_float(nscan, nfov_m, &prob);
    //wrt_fmbin("fmbin.bin", nscan, nfov_m, flag0);
    //fp1 = fopen("bt.bin", "wb");
    //fwrite(at_p1, 4, nscan*nfov_m*(nchan_m+nchan_a), fp1);
    //fclose(fp1);
   
    free(cwn_m);
    free(cwn_a);
    fclose(fp_m);
    fclose(fp_a);
    //printf("Done mhs/amsua l1b data.\n");

    return(0);
}



void rmhs_scn(FILE *fp, int *bged, float *cwn_m, int nscan_m, int nfov_m, 
        int nchan_m, int nscan_a, int nfov_a, int nchan_a, int nscan, 
        long long js0mf, int nim)
{
    //printf("----------------------------------------\n");
    //printf("rmhs_scn: nscan_m, nfov_m: %d, %d\n", nscan_m, nfov_m);

    SCANLINE_M  scn;

    int         count;
    double      clcoef0, clcoef1, clcoef2, calrad, rad, rat;
    int         iscan, ifov, ichan;
    long long   js0_day;
    double      jsmf;
    int         ind;

    cal2jsec(dtse[0][0], dtse[0][1], dtse[0][2], 0, 0, 0, &js0_day);
    //printf("js0_day: %lld\n", js0_day);

    // Read MHS 1B* scanline data records
    for(iscan=0; iscan<nscan_m; iscan++)  {
        // read
        fread(&scn, sizeof(SCANLINE_M), 1, fp);
        if (*bged == 1)  {
            swap_scanline_m(&scn);
            //printf("big endian, swapped\n");
        }

        // jsmf and ind
        jsmf = js0_day + scn.time_of_day_of_scan / 1000.0;
        if (jsmf < js0mf-43200)    jsmf = jsmf + 86400;
        ind = round((jsmf-js0mf) / (8.0/3.0));
        //printf("jsmf, ind: %f, %d\n", jsmf, ind);
        if (ind<0 || ind>nim-1)  {
            //printf("ind: %d, skip\n", ind);
            continue;
        }

        // convert to standard vars
        for (ifov=0; ifov<nfov_m; ifov++)  {
            // btjs, dsec
            btjs[ind][ifov] = js0_day + scn.time_of_day_of_scan / 1000;
            dsec[ind][ifov] = jsmf - btjs[ind][ifov];

            // lon, lat
            lon[ind][ifov] = scn.earth_location[ifov*2+1] * 1.0E-4;
            lat[ind][ifov] = scn.earth_location[ifov*2+0] * 1.0E-4;

            // lza, sza, obmd
            if (ifov < nfov_m/2)
                lza[ind][ifov]=-scn.angular_relationships[ifov*3+1] * 1.0E-2;
            else
                lza[ind][ifov] = scn.angular_relationships[ifov*3+1] * 1.0E-2;
            sza[ind][ifov] = scn.angular_relationships[ifov*3] * 1.0E-2;
            obmd[ind][ifov] = scn.orbit_node>>15;

            // at
            for (ichan = 0; ichan<nchan_m; ichan++)  {
                // counts
                count = scn.earth_view_data[ifov*6+ichan+1];
                if (count < 0)      count = count + 65536;
                // coeff
                clcoef2 = scn.pri_cal_coeffs[ichan][0]*1.0E-16;
                clcoef1 = scn.pri_cal_coeffs[ichan][1]*1.0E-10;
                clcoef0 = scn.pri_cal_coeffs[ichan][2]*1.0E-06;
                // cnt2rd(numchan, numspot);
                calrad = clcoef0 + clcoef1*count + clcoef2*count*count;
                rad = calrad;
                //rad2at
                rad2at_m(rad, cwn_m[ichan], &rat);
                // set to at var
                at[ind][ifov][ichan+nchan_a] = rat;
            }

        }  // ifov
    }  // iscan
}



void rama_scn(FILE *fp, int *bged, float *cwn_a, int nscan_a, int nfov_a, 
        int nchan_a, int nscan_m, int nfov_m, int nchan_m, int nscan, 
        long long js0mf, int nia)
{
    //printf("----------------------------------------\n");
    //printf("rama_scn: nscan_a, nfov_a: %d, %d\n", nscan_a, nfov_a);

    SCANLINE_A  scn;

    unsigned short  count;
    float           clcoef0, clcoef1, clcoef2, calrad, rad, rat;
    int             iscan, ifov, ichan, ind;
    long long       js0_day;
    double          jsaf;
    float           aa[nscan][nfov_a][nchan_a];
    int             i20, i21, j2;
    float           di20, tt, tt0, tt1;

    cal2jsec(dtse[0][0], dtse[0][1], dtse[0][2], 0, 0, 0, &js0_day);
    //printf("js0_day: %lld\n", js0_day);
    
    // Read AMSU-A 1B* scanline data records
    for(iscan=0; iscan<nscan_a; iscan++)  {
        // read
        fread(&scn, sizeof(scn), 1, fp);
        if (*bged == 1)  {
            swap_scanline_a(&scn);
            //printf("big endian, swapped\n");
        }

        // jsaf and ind
        jsaf = js0_day + scn.time_of_day_of_scan / 1000.0;
        if (jsaf < js0mf-43200)    jsaf = jsaf + 86400;
        ind = round((jsaf-js0mf) / 8.0);
        if (ind<0 || ind>nia-1)  {
            //printf("ind: %d, skip\n", ind);
            continue;
        }
        
        // convert to standard vars
        for (ifov=0; ifov<nfov_a; ifov++)  {
            for(ichan = 0; ichan<nchan_a; ichan++)  {
                // counts
                if (ichan < 2)
                    count = scn.scene_telemetry_a2[ifov*4+ichan+2];
                else
                    count = scn.scene_telemetry_a1[ifov*17+ichan+2];
                // coeff
                clcoef2 = scn.pri_cal_coeffs[ichan][0]*1.0E-19;
                clcoef1 = scn.pri_cal_coeffs[ichan][1]*1.0E-13;
                clcoef0 = scn.pri_cal_coeffs[ichan][2]*1.0E-09;
                // cnt2rd(numchan, numspot);
                calrad = clcoef0 + clcoef1*count + clcoef2*count*count;
                rad = calrad;
                //rad2at
                rad2at_a(rad, cwn_a[ichan], &rat);
                // set to at var
                aa[ind][ifov][ichan] = rat;
            }
        }  // ifov
    }  // iscan


    // put to mhs fov
    // ifov: 1, 4, ..., 88
    for (iscan=0; iscan<nscan; iscan++)  {
        for (ifov=1; ifov<=nfov_m-2; ifov+=3)  {
            tt = (iscan-2.0/87.0*(ifov-1)) / 3;
            i20 = floor(tt);
            di20 = tt - i20;
            i21 = i20 + 1;
            // the last/last point
            if (iscan==nscan-1 && ifov==nfov_m-2)    i21=i20;
            j2 = (ifov-1) / 3;
            //printf("mhs: iscan, ifov: %d, %d\n", iscan, ifov);
            //printf("ama grid: i20, i21, di20: %d, %d, %f\n", i20, i21, di20);
            //printf("ama grid: j2: %d\n", j2);
            if (i20>=0 && i21<=nscan-1)  {
                for (ichan=0; ichan<nchan_a; ichan++)  {
                    tt0 = aa[i20][j2][ichan];    tt1 = aa[i21][j2][ichan];
                    if (tt0 > AT_MIN && tt1 > AT_MIN)  {
                        at[iscan][ifov][ichan] = tt0*(1-di20) + tt1*di20;
                    }
                    else if (tt0 > AT_MIN && tt1 < AT_MIN)  {
                        if (di20 <= 0.5)    at[iscan][ifov][ichan] = tt0;
                    }
                    else if (tt0 < AT_MIN && tt1 > AT_MIN)  {
                        if (di20 >= 0.5)    at[iscan][ifov][ichan] = tt1; 
                    }
                    else  {
                        ;
                    }
                }
            }
        }
    }
    // ifov: 2,3,  5,6, ...
    for (iscan=0; iscan<nscan; iscan++)  {
        for (ifov=2; ifov<nfov_m-3; ifov+=3)  {
            for (ichan=0; ichan<nchan_a; ichan++)  {
                tt0=at[iscan][ifov-1][ichan];  tt1=at[iscan][ifov+2][ichan];
                if (tt0 > AT_MIN && tt1 > AT_MIN)  {
                    at[iscan][ifov][ichan] = tt0*(2.0/3.0) + tt1*(1.0/3.0);
                    at[iscan][ifov+1][ichan] = tt0*(1.0/3.0) + tt1*(2.0/3.0);
                }
                else if (tt0 > AT_MIN && tt1 < AT_MIN)  {
                    at[iscan][ifov][ichan] = tt0;
                }
                else if (tt0 < AT_MIN && tt1 > AT_MIN)  {
                    at[iscan][ifov+1][ichan] = tt1;
                }
                else  {
                    ;
                }
            }
        }
    }
    // ifov: 0 and 89
    for (iscan=0; iscan<nscan; iscan++)  {
        for (ichan=0; ichan<nchan_a; ichan++)  {
            at[iscan][0][ichan] = at[iscan][1][ichan];
            at[iscan][nfov_m-1][ichan] = at[iscan][nfov_m-2][ichan];
        }
    }
}



int ma_indse(long long *js01m, long long *js01a, 
        int *im0p, int *nimp, int *ia0p, int *niap)
{
    int         im0, nim, ia0, nia, nastep, nmstep, niam;
    long long   dsec, js00;
    
    // assume all amsua values in mhs, ref to first amsua value
    nastep = (js01m[0]-js01a[0]) / 8;
    dsec = (js01m[0]-js01a[0]) % 8;
    if (dsec < 0)  {
        nastep--;
        dsec += 8;
    }
    //printf("nastep, dsec: %d, %lld\n", nastep, dsec);
    if (dsec == 0)
        nmstep = 0;
    else if (dsec==2 || dsec==3)
        nmstep = 1;
    else if (dsec==5 || dsec==6)
        nmstep = 2;
    else
        exit(1);
    //printf("nmstep, nastep: %d, %d\n", nmstep, nastep);

    // cal first postion
    if (nmstep == 0)  {
        ia0 = nastep;
        im0 = 0;
    }
    else  {
        ia0 = nastep + 1;
        im0 = 3 - nmstep;
    }

    // if ia0<0, shift
    if (ia0 < 0)  {
        im0 = im0 - ia0*3;
        ia0 = 0;
    }

    // max len
    js00 = js01a[0] + ia0*8;
    nia = (js01a[1]-js00)/8 + 1;
    niam = (js01m[1]-js00)/8;
    if (nia > niam)
        nia = niam;
    nim = nia * 3;
    //printf("im0, nim, ia0, nia: %d, %d, %d, %d\n", im0, nim, ia0, nia);

    *im0p = im0;  *nimp = nim;  *ia0p = ia0;  *niap = nia;

    return(0);
}



void rmhs_hd(FILE *fp, int *bged, float **cwn_p, int *nscan, int *nfov, 
        int *nchan, int nyref, int **lese_tab, long long *js01, float *djs01)
{
    HBLOCK_M    hblk;
    float       *cwn;

    fread(&hblk, sizeof(HBLOCK_M), 1, fp);

    if (hblk.start_year < YEAR_MIN || hblk.start_year > YEAR_MAX)  {
        swap_hblock_m(&hblk);
        *bged = 1;
        printf("big endian, swapped\n");
    }

    // set ouptput vars
    *nscan = hblk.last_scan_record;
    *nfov = 90;
    *nchan = 5;

    // central wave numbers to return
    cwn = (float*) malloc((*nchan) * sizeof(float));
    if (cwn == NULL)  {
        printf("cwn can not allocated.\n");
        exit(1);
    }
    cwn[0] = hblk.t_r_central_wave_number_1*1.0E-6;
    cwn[1] = hblk.t_r_central_wave_number_2*1.0E-6;
    cwn[2] = hblk.t_r_central_wave_number_3*1.0E-6;
    cwn[3] = hblk.t_r_central_wave_number_4*1.0E-6;
    cwn[4] = hblk.t_r_central_wave_number_5*1.0E-6;
    *cwn_p = cwn;

    // set start/end time
    dtse[0][0] = hblk.start_year;
    doy2cal(hblk.start_day_of_year, dtse[0][0], &dtse[0][1], &dtse[0][2]);
    dtse[0][3] = hblk.start_milliseconds_of_day / 1000 / 3600;
    dtse[0][4] = hblk.start_milliseconds_of_day / 1000 / 60 % 60;
    dtse[0][5] = hblk.start_milliseconds_of_day / 1000 % 60;
    cal2jsec(hblk.start_year, 1, 1, 0, 0, 0, &js01[0]);
    js01[0] = js01[0] + (hblk.start_day_of_year-1)*86400 
        + hblk.start_milliseconds_of_day / 1000;
    djs01[0] = (hblk.start_milliseconds_of_day % 1000) / 1000.0;

    dtse[1][0] = hblk.end_year;
    doy2cal(hblk.end_day_of_year, dtse[1][0], &dtse[1][1], &dtse[1][2]);
    dtse[1][3] = hblk.end_milliseconds_of_day / 1000 / 3600;
    dtse[1][4] = hblk.end_milliseconds_of_day / 1000 / 60 % 60;
    dtse[1][5] = hblk.end_milliseconds_of_day / 1000 % 60;
    cal2jsec(hblk.end_year, 1, 1, 0, 0, 0, &js01[1]);
    js01[1] = js01[1] + (hblk.end_day_of_year-1)*86400 
        + hblk.end_milliseconds_of_day / 1000;
    djs01[1] = (hblk.end_milliseconds_of_day % 1000) / 1000.0;

    // Print out header block1 information
    //printf("-----------------------------------------------------\n");
    //printf(" dtse0: %04d/%02d/%02d %02d:%02d:%02d\n", dtse[0][0], 
    //        dtse[0][1], dtse[0][2], dtse[0][3], dtse[0][4], dtse[0][5]);
    //printf(" dtse1: %04d/%02d/%02d %02d:%02d:%02d\n", dtse[0][0], 
    //        dtse[1][1], dtse[1][2], dtse[1][3], dtse[1][4], dtse[1][5]);
    //printf(" k, f 1: %8d, %6.2f\n", hblk.t_r_central_wave_number_1, 
    //        hblk.t_r_central_wave_number_1*3/100000.0);
    //printf(" k, f 2: %8d, %6.2f\n", hblk.t_r_central_wave_number_2, 
    //        hblk.t_r_central_wave_number_2*3/100000.0);
    //printf(" k, f 3: %8d, %6.2f\n", hblk.t_r_central_wave_number_3, 
    //        hblk.t_r_central_wave_number_3*3/100000.0);
    //printf(" k, f 4: %8d, %6.2f\n", hblk.t_r_central_wave_number_4, 
    //        hblk.t_r_central_wave_number_4*3/100000.0);
    //printf(" k, f 5: %8d, %6.2f\n", hblk.t_r_central_wave_number_5, 
    //        hblk.t_r_central_wave_number_5*3/100000.0);
    //printf("-----------------------------------------------------\n");
}



void rama_hd(FILE *fp, int *bged, float **cwn_p, int *nscan, int *nfov, 
        int *nchan, int nyref, int **lese_tab, long long *js01, float *djs01)
{
    HBLOCK_A    hblk;
    float       *cwn;

    fread(&hblk, sizeof(HBLOCK_A), 1, fp);

    if (hblk.start_year < YEAR_MIN || hblk.start_year > YEAR_MAX)  {
        swap_hblock_a(&hblk);
        *bged = 1;
        printf("big endian, swapped\n");
    }

    // set ouptput vars
    *nscan = hblk.last_scan_record;
    *nfov = 30;
    *nchan = 15;

    // central wave numbers to return
    cwn = (float*) malloc((*nchan) * sizeof(float));
    if (cwn == NULL)  {
        printf("cwn can not allocated.\n");
        exit(1);
    }
    cwn[0] = hblk.t_r_central_wave_number_1*1.0E-6;
    cwn[1] = hblk.t_r_central_wave_number_2*1.0E-6;
    cwn[2] = hblk.t_r_central_wave_number_3*1.0E-6;
    cwn[3] = hblk.t_r_central_wave_number_4*1.0E-6;
    cwn[4] = hblk.t_r_central_wave_number_5*1.0E-6;
    cwn[5] = hblk.t_r_central_wave_number_6*1.0E-6;
    cwn[6] = hblk.t_r_central_wave_number_7*1.0E-6;
    cwn[7] = hblk.t_r_central_wave_number_8*1.0E-6;
    cwn[8] = hblk.t_r_central_wave_number_9*1.0E-6;
    cwn[9] = hblk.t_r_central_wave_number_10*1.0E-6;
    cwn[10] = hblk.t_r_central_wave_number_11*1.0E-6;
    cwn[11] = hblk.t_r_central_wave_number_12*1.0E-6;
    cwn[12] = hblk.t_r_central_wave_number_13*1.0E-6;
    cwn[13] = hblk.t_r_central_wave_number_14*1.0E-6;
    cwn[14] = hblk.t_r_central_wave_number_15*1.0E-6;
    *cwn_p = cwn;

    // cal start/end jsec
    cal2jsec(hblk.start_year, 1, 1, 0, 0, 0, &js01[0]);
    js01[0] = js01[0] + (hblk.start_day_of_year-1)*86400 
        + hblk.start_milliseconds_of_day / 1000;
    djs01[0] = (hblk.start_milliseconds_of_day % 1000) / 1000.0;

    cal2jsec(hblk.end_year, 1, 1, 0, 0, 0, &js01[1]);
    js01[1] = js01[1] + (hblk.end_day_of_year-1)*86400 
        + hblk.end_milliseconds_of_day / 1000;
    djs01[1] = (hblk.end_milliseconds_of_day % 1000) / 1000.0;

    // cal start/end time
    int dtse1[2][6];

    dtse1[0][0] = hblk.start_year;
    doy2cal(hblk.start_day_of_year, dtse1[0][0], &dtse1[0][1], &dtse1[0][2]);
    dtse1[0][3] = hblk.start_milliseconds_of_day / 1000 / 3600;
    dtse1[0][4] = hblk.start_milliseconds_of_day / 1000 / 60 % 60;
    dtse1[0][5] = hblk.start_milliseconds_of_day / 1000 % 60;

    dtse1[1][0] = hblk.end_year;
    doy2cal(hblk.end_day_of_year, dtse1[1][0], &dtse1[1][1], &dtse1[1][2]);
    dtse1[1][3] = hblk.end_milliseconds_of_day / 1000 / 3600;
    dtse1[1][4] = hblk.end_milliseconds_of_day / 1000 / 60 % 60;
    dtse1[1][5] = hblk.end_milliseconds_of_day / 1000 % 60;

    // Print out header block1 information
    //printf("-----------------------------------------------------\n");
    //printf(" dtse0: %04d/%02d/%02d %02d:%02d:%02d\n", dtse1[0][0], 
    //        dtse1[0][1], dtse1[0][2], dtse1[0][3], dtse1[0][4], dtse1[0][5]);
    //printf(" dtse1: %04d/%02d/%02d %02d:%02d:%02d\n", dtse1[0][0], 
    //        dtse1[1][1], dtse1[1][2], dtse1[1][3], dtse1[1][4], dtse1[1][5]);
    //printf(" k, f  1: %8d, %6.2f\n", hblk.t_r_central_wave_number_1, 
    //        hblk.t_r_central_wave_number_1*3/100000.0);
    //printf(" k, f  2: %8d, %6.2f\n", hblk.t_r_central_wave_number_2, 
    //        hblk.t_r_central_wave_number_2*3/100000.0);
    //printf(" k, f  3: %8d, %6.2f\n", hblk.t_r_central_wave_number_3, 
    //        hblk.t_r_central_wave_number_3*3/100000.0);
    //printf(" k, f  4: %8d, %6.2f\n", hblk.t_r_central_wave_number_4, 
    //        hblk.t_r_central_wave_number_4*3/100000.0);
    //printf(" k, f  5: %8d, %6.2f\n", hblk.t_r_central_wave_number_5, 
    //        hblk.t_r_central_wave_number_5*3/100000.0);
    //printf(" k, f  6: %8d, %6.2f\n", hblk.t_r_central_wave_number_6, 
    //        hblk.t_r_central_wave_number_6*3/100000.0);
    //printf(" k, f  7: %8d, %6.2f\n", hblk.t_r_central_wave_number_7, 
    //        hblk.t_r_central_wave_number_7*3/100000.0);
    //printf(" k, f  8: %8d, %6.2f\n", hblk.t_r_central_wave_number_8, 
    //        hblk.t_r_central_wave_number_8*3/100000.0);
    //printf(" k, f  9: %8d, %6.2f\n", hblk.t_r_central_wave_number_9, 
    //        hblk.t_r_central_wave_number_9*3/100000.0);
    //printf(" k, f 10: %8d, %6.2f\n", hblk.t_r_central_wave_number_10, 
    //        hblk.t_r_central_wave_number_10*3/100000.0);
    //printf(" k, f 11: %8d, %6.2f\n", hblk.t_r_central_wave_number_11, 
    //        hblk.t_r_central_wave_number_11*3/100000.0);
    //printf(" k, f 12: %8d, %6.2f\n", hblk.t_r_central_wave_number_12, 
    //        hblk.t_r_central_wave_number_12*3/100000.0);
    //printf(" k, f 13: %8d, %6.2f\n", hblk.t_r_central_wave_number_13, 
    //        hblk.t_r_central_wave_number_13*3/100000.0);
    //printf(" k, f 14: %8d, %6.2f\n", hblk.t_r_central_wave_number_14, 
    //        hblk.t_r_central_wave_number_14*3/100000.0);
    //printf(" k, f 15: %8d, %6.2f\n", hblk.t_r_central_wave_number_15, 
    //        hblk.t_r_central_wave_number_15*3/100000.0);
    //printf("-----------------------------------------------------\n");
}



void rmhs_scn0(FILE *fp, int *bged, float *cwn_m, int nscan_m, int nfov_m, 
        int nchan_m, int nscan_a, int nfov_a, int nchan_a, int nscan)
{
    SCANLINE_M  scn;

    int         count;
    double      clcoef0, clcoef1, clcoef2, calrad, rad, rat;
    int         iscan, ifov, ichan;
    long long   jsec0;

    //printf("rmhs_scn: nscan_m, nfov_m: %d, %d\n", nscan_m, nfov_m);

    cal2jsec(dtse[0][0], dtse[0][1], dtse[0][2], 0, 0, 0, &jsec0);


    // Read MHS 1B* scanline data records
    for(iscan=0; iscan<nscan_m; iscan++)  {

        fread(&scn, sizeof(SCANLINE_M), 1, fp);

        if (*bged == 1)  {
            swap_scanline_m(&scn);
            //printf("big endian, swapped\n");
        }

        // convert to standard vars
        for (ifov=0; ifov<nfov_m; ifov++)  {
                 
            btjs[iscan][ifov] = jsec0 + scn.time_of_day_of_scan / 1000;
            dsec[iscan][ifov] = scn.time_of_day_of_scan % 1000 /1000.0;

            if (iscan>0 && btjs[iscan][ifov]<btjs[iscan-1][ifov])
                btjs[iscan][ifov] = btjs[iscan][ifov] + 86400;

            lon[iscan][ifov] = scn.earth_location[ifov*2+1] * 1.0E-4;
            lat[iscan][ifov] = scn.earth_location[ifov*2+0] * 1.0E-4;
            if (ifov < nfov_m/2)
                lza[iscan][ifov] = -scn.angular_relationships[ifov*3+1] 
                    * 1.0E-2;
            else
                lza[iscan][ifov] = scn.angular_relationships[ifov*3+1] 
                    * 1.0E-2;
            sza[iscan][ifov] = scn.angular_relationships[ifov*3] * 1.0E-2;

            obmd[iscan][ifov] = scn.orbit_node>>15;

            //getcal();
            for (ichan = 0; ichan<nchan_m; ichan++)  {

                // counts
                count = scn.earth_view_data[ifov*6+ichan+1];
                if (count < 0)      count = count + 65536;

                // coeff
                clcoef2 = scn.pri_cal_coeffs[ichan][0]*1.0E-16;
                clcoef1 = scn.pri_cal_coeffs[ichan][1]*1.0E-10;
                clcoef0 = scn.pri_cal_coeffs[ichan][2]*1.0E-06;
                
                // cnt2rd(numchan, numspot);
                calrad = clcoef0 + clcoef1*count + clcoef2*count*count;
                rad = calrad;

                //rad2at
                rad2at_m(rad, cwn_m[ichan], &rat);

                // set to at var
                at[iscan][ifov][ichan+nchan_a] = rat;
            }

        }  // ifov
    }  // iscan

    // Print out some scanline information
    //printf("rmhs_scn print for check\n");
    //for (iscan=0; iscan<6; iscan++)  {
    //    for (ifov=0; ifov<1; ifov++)  {
    //        printf("time_of_day: %u\n", scn.time_of_day_of_scan);
    //        printf("btjs, dsec: %lld, %6.3f\n", btjs[iscan][0], 
    //                dsec[iscan][0]);
    //        printf("lon, lat: %10.4f, %10.4f\n", lon[iscan][0], 
    //                lat[iscan][0]);
    //        printf("lza, sza: %8.2f, %8.2f\n", lza[iscan][0], 
    //                sza[iscan][0]);
    //        printf("obmd: %d\n", obmd[iscan][0]);
    //        printf("at: %f, %f, %f, %f, %f\n", at[iscan][0][0+nchan_a], 
    //                at[iscan][0][1+nchan_a], at[iscan][0][2+nchan_a], 
    //                at[iscan][0][3+nchan_a], at[iscan][0][4+nchan_a]);
    //    }
    //    //getchar();
    //}
    //printf("mhs scn\n");
}



void rama_scn0(FILE *fp, int *bged, float *cwn_a, int nscan_a, int nfov_a, 
        int nchan_a, int nscan_m, int nfov_m, int nchan_m, int nscan)
{
    SCANLINE_A  scn;

    unsigned short  count;
    float           clcoef0, clcoef1, clcoef2, calrad, rad, rat;
    int             iscan, ifov, ichan, ib1, ib2, ind;

    //printf("rama_scn: nscan_a, nfov_a: %d, %d\n", nscan_a, nfov_a);

    // Read AMSU-A 1B* scanline data records
    for(iscan = 0; iscan < nscan_a; iscan++)  {
        //printf("iscan, iscan*3, nscan_a, nscan: %d, %d, %d, %d\n", 
        //        iscan, iscan*3, nscan_a, nscan);

        if (3*iscan >= nscan)      break;

        fread(&scn, sizeof(scn), 1, fp);

        if (*bged == 1)  {
            swap_scanline_a(&scn);
            //printf("big endian, swapped\n");
        }

        // convert to standard vars
        for (ifov=0; ifov<nfov_a; ifov++)  {

            //getcal();
            for(ichan = 0; ichan<nchan_a; ichan++)  {

                // counts
                if (ichan < 2)
                    count = scn.scene_telemetry_a2[ifov*4+ichan+2];
                else
                    count = scn.scene_telemetry_a1[ifov*17+ichan+2];
                //printf("count: %hu\n", count);

                // coeff
                clcoef2 = scn.pri_cal_coeffs[ichan][0]*1.0E-19;
                clcoef1 = scn.pri_cal_coeffs[ichan][1]*1.0E-13;
                clcoef0 = scn.pri_cal_coeffs[ichan][2]*1.0E-09;
                //printf("coe: %lf, %lf, %lf\n", clcoef2, clcoef1, clcoef0);
                
                // cnt2rd(numchan, numspot);
                calrad = clcoef0 + clcoef1*count + clcoef2*count*count;
                rad = calrad;
                //printf("rad: %lf\n", rad);

                //rad2at
                rad2at_a(rad, cwn_a[ichan], &rat);
                //printf("rat: %f\n", rat);
                //getchar();

                // set to at var
                for (ib1=0; ib1<3; ib1++)  {
                    ind = 3 * iscan + ib1;
                    //printf("ind: %d\n", ind);
                    if (ind >= nscan)   break;
                    for (ib2=0; ib2<3; ib2++)
                        at[ind][3*ifov+ib2][ichan] = rat;
                }

            }  // ichan
        }  // ifov

    }  // iscan

    // print out
    //printf("rama_scn print out for check\n");
    //for (iscan=0; iscan<6; iscan++)  {
    //    for (ifov=0; ifov<1; ifov++)  {
    //        printf("at: %f, %f, %f, %f, %f\n", at[iscan][ifov][0], 
    //                at[iscan][ifov][1], at[iscan][ifov][2], 
    //                at[iscan][ifov][3], at[iscan][ifov][4]);
    //        printf("at: %f, %f, %f, %f, %f\n", at[iscan][89][0], 
    //                at[iscan][89][1], at[iscan][89][2], 
    //                at[iscan][89][3], at[iscan][89][4]);
    //    }
    //    //getchar();
    //}
    //printf("ama scn\n");
}



void rad2at_m(double rad, float cwn, double *rat)
{
   float c1v3, c2v, pl;
   float plcoef1 = 1.1910659E-05;
   float plcoef2 = 1.438833;

   double temp;

   pl = cwn;

   c1v3 = plcoef1*pl*pl*pl;
   c2v  = plcoef2*pl;

   *rat = -99;
   if (rad != 0)  {
       temp =(double)c1v3/rad;
       if (temp > 0)  {
           temp = log(temp + 1.0);
           if (temp != 0)     *rat = c2v/(float)temp;
       }
   }
}



void rad2at_a(float rad, float cwn, float *rat)
{
   float c1v3, c2v, pl;
   float plcoef1 = 1.1910659E-05;
   float plcoef2 = 1.438833;

   double temp;

   pl = cwn;

   c1v3 = plcoef1*pl*pl*pl;
   c2v  = plcoef2*pl;

   *rat = -99;
   if (rad != 0)  {
       temp =(double)c1v3/rad;
       if (temp > 0)  {
           temp = log(temp + 1.0);
           if (temp != 0)     *rat = c2v/(float)temp;
       }
   }
}



void reverse_bytes(void *data, size_t size)
{
    char *i, *j;
    char tmp;

    for (i = (char*) data, j = i + size - 1; i < j; i++, j--) {
        tmp = *i;
        *i = *j;
        *j = tmp;
    }
}



void swap_hblock_m(HBLOCK_M *hblock)
{
    unsigned short   *p_us;
    unsigned int     *p_ul;
    int              *p_l;

    p_us = &hblock->start_year;
    reverse_bytes(p_us, sizeof(*p_us));

    p_us = &hblock->start_day_of_year;
    reverse_bytes(p_us, sizeof(*p_us));

    p_ul = &hblock->start_milliseconds_of_day;
    reverse_bytes(p_ul, sizeof(*p_ul));

    p_us = &hblock->end_year;
    reverse_bytes(p_us, sizeof(*p_us));

    p_us = &hblock->end_day_of_year;
    reverse_bytes(p_us, sizeof(*p_us));

    p_ul = &hblock->end_milliseconds_of_day;
    reverse_bytes(p_ul, sizeof(*p_ul));

    p_us = &hblock->last_scan_record;
    reverse_bytes(p_us, sizeof(*p_us));

    p_ul = &hblock->t_r_central_wave_number_1;
    reverse_bytes(p_ul, sizeof(*p_ul));
    p_ul = &hblock->t_r_central_wave_number_2;
    reverse_bytes(p_ul, sizeof(*p_ul));
    p_ul = &hblock->t_r_central_wave_number_3;
    reverse_bytes(p_ul, sizeof(*p_ul));
    p_ul = &hblock->t_r_central_wave_number_4;
    reverse_bytes(p_ul, sizeof(*p_ul));
    p_ul = &hblock->t_r_central_wave_number_5;
    reverse_bytes(p_ul, sizeof(*p_ul));

    p_us = &hblock->orbit_vector_epoch_year;
    reverse_bytes(p_us, sizeof(*p_us));
    p_us = &hblock->orbit_vector_day_of_year;
    reverse_bytes(p_us, sizeof(*p_us));
    p_ul = &hblock->orbit_vector_utc_time_of_day;
    reverse_bytes(p_ul, sizeof(*p_ul));

    p_l = &hblock->semimajor_axis;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->eccentricity;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->inclination;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->argument_of_perigee;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->right_ascension;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->mean_anomaly;
    reverse_bytes(p_l, sizeof(*p_l));
}



void swap_hblock_a(HBLOCK_A *hblock)
{
    unsigned short   *p_us;
    unsigned int     *p_ul;
    int              *p_l;

    p_us = &hblock->start_year;
    reverse_bytes(p_us, sizeof(*p_us));

    p_us = &hblock->start_day_of_year;
    reverse_bytes(p_us, sizeof(*p_us));

    p_ul = &hblock->start_milliseconds_of_day;
    reverse_bytes(p_ul, sizeof(*p_ul));

    p_us = &hblock->end_year;
    reverse_bytes(p_us, sizeof(*p_us));

    p_us = &hblock->end_day_of_year;
    reverse_bytes(p_us, sizeof(*p_us));

    p_ul = &hblock->end_milliseconds_of_day;
    reverse_bytes(p_ul, sizeof(*p_ul));

    p_us = &hblock->last_scan_record;
    reverse_bytes(p_us, sizeof(*p_us));

    p_l = &hblock->t_r_central_wave_number_1;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_2;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_3;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_4;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_5;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_6;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_7;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_8;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_9;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_10;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_11;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_12;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_13;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_14;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->t_r_central_wave_number_15;
    reverse_bytes(p_l, sizeof(*p_l));

    p_us = &hblock->orbit_vector_epoch_year;
    reverse_bytes(p_us, sizeof(*p_us));
    p_us = &hblock->orbit_vector_day_of_year;
    reverse_bytes(p_us, sizeof(*p_us));
    p_ul = &hblock->orbit_vector_utc_time_of_day;
    reverse_bytes(p_ul, sizeof(*p_ul));

    p_l = &hblock->semimajor_axis;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->eccentricity;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->inclination;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->argument_of_perigee;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->right_ascension;
    reverse_bytes(p_l, sizeof(*p_l));
    p_l = &hblock->mean_anomaly;
    reverse_bytes(p_l, sizeof(*p_l));
}



void swap_scanline_m(SCANLINE_M *scanline)
{
    unsigned short   *p_us;
    unsigned int     *p_ul;
    int              *p_l;
    short            *p_s;
    int              i, j;

    p_ul = &scanline->time_of_day_of_scan;
    reverse_bytes(p_ul, sizeof(*p_ul));

    p_us = &scanline->orbit_node;
    reverse_bytes(p_us, sizeof(*p_us));

    p_ul = &scanline->quality_indicator_bit_field;
    reverse_bytes(p_ul, sizeof(*p_ul));

    for (i=0; i<5; i++)  {
        for (j=0; j<3; j++)  {
            p_l = &scanline->pri_cal_coeffs[i][j];
            reverse_bytes(p_l, sizeof(*p_l));
        }
    }

    for (i=0; i<270; i++)  {
        p_s = &scanline->angular_relationships[i];
        reverse_bytes(p_s, sizeof(*p_s));
    }

    for (i=0; i<180; i++)  {
        p_l = &scanline->earth_location[i];
        reverse_bytes(p_l, sizeof(*p_l));
    }

    for (i=0; i<540; i++)  {
        p_us = &scanline->earth_view_data[i];
        reverse_bytes(p_us, sizeof(*p_us));
    }
}



void swap_scanline_a(SCANLINE_A *scanline)
{
    unsigned short   *p_us;
    unsigned int     *p_ul;
    int              *p_l;
    short            *p_s;
    int              i, j;

    p_ul = &scanline->time_of_day_of_scan;
    reverse_bytes(p_ul, sizeof(*p_ul));

    p_us = &scanline->orbit_node;
    reverse_bytes(p_us, sizeof(*p_us));

    p_ul = &scanline->quality_indicator_bit_field;
    reverse_bytes(p_ul, sizeof(*p_ul));

    for (i=0; i<15; i++)  {
        for (j=0; j<3; j++)  {
            p_l = &scanline->pri_cal_coeffs[i][j];
            reverse_bytes(p_l, sizeof(*p_l));
        }
    }

    for (i=0; i<90; i++)  {
        p_s = &scanline->angular_relationships[i];
        reverse_bytes(p_s, sizeof(*p_s));
    }

    for (i=0; i<60; i++)  {
        p_l = &scanline->earth_location[i];
        reverse_bytes(p_l, sizeof(*p_l));
    }

    for (i=0; i<510; i++)  {
        p_us = &scanline->scene_telemetry_a1[i];
        reverse_bytes(p_us, sizeof(*p_us));
    }

    for (i=0; i<120; i++)  {
        p_us = &scanline->scene_telemetry_a2[i];
        reverse_bytes(p_us, sizeof(*p_us));
    }
}


//////////////////////////////
// old version
//////////////////////////////
//void rama_whdfeos(FILE *fp_FNAME, char *ama_direct, char *direct, char *sid)
int rd_mhs_l1b0(char *ffn0, char *ffn1, char *ftype,
        int *nscanp, int *nfovp, int *nchanp, int **nfovap, int **nfova_p1p,
        int ***flag0p, int **flag0_p1p, int nyref, int **lese_tab)
{
    FILE    *fp_m, *fp_a;

    float   *cwn_m, *cwn_a;
    int     nscan_m, nfov_m, nchan_m, bged_m;
    int     nscan_a, nfov_a, nchan_a, bged_a;
    int     nscan, nfov, nchan;

    int     *nfova, *nfova_p1;
    int     **flag0, *flag0_p1;

    int     i, iscan, ifov, ichan, tt;

    //printf("----------------------------------------\n");
    //printf("ffn0: %s\n", ffn0);
    //printf("ffn1: %s\n", ffn1);

    // open data files
    fp_m = fopen(ffn0, "r");
    if (fp_m == NULL)  {
        printf("ERROR: Fail to open file (stop): %s\n", ffn0);
        exit(1);
    }

    fp_a = fopen(ffn1, "r");
    if (fp_a == NULL)  {
        printf("ERROR: Fail to open file (stop): %s\n", ffn1);
        exit(1);
    }

    long long   js01m[2], js01a[2];
    float       djs01m[2], djs01a[2];

    // read both file header for information
    //rmhs_hd(fp_m, &bged_m, &cwn_m, &nscan_m, &nfov_m, &nchan_m,
    //        nyref, lese_tab);
    rmhs_hd(fp_m, &bged_m, &cwn_m, &nscan_m, &nfov_m, &nchan_m, 
            nyref, lese_tab, js01m, djs01m);
    printf("m: nscan, nfov, nchan: %d, %d, %d\n", nscan_m, nfov_m, nchan_m);
    //for (i=0; i<nchan_m; i++)
    //    printf("%f  ", cwn_m[i]);
    //printf("\n");

    //rama_hd(fp_a, &bged_a, &cwn_a, &nscan_a, &nfov_a, &nchan_a,
    //        nyref, lese_tab);
    rama_hd(fp_a, &bged_a, &cwn_a, &nscan_a, &nfov_a, &nchan_a,
            nyref, lese_tab, js01a, djs01a);
    printf("a: nscan, nfov, nchan: %d, %d, %d\n", nscan_a, nfov_a, nchan_a);
    //for (i=0; i<nchan_a; i++)
    //    printf("%f  ", cwn_a[i]);
    //printf("\n");


    // Initialize the AT 
    nscan = nscan_m;
    nfov = nfov_m;
    nchan = nchan_m + nchan_a;
    //printf("nscan, nfov, nchan: %d, %d, %d\n", nscan, nfov, nchan);

    // init sat var
    lon_p1  = init_2dc_float(nscan, nfov, &lon);
    lat_p1  = init_2dc_float(nscan, nfov, &lat);
    btjs_p1 = init_2dc_llint(nscan, nfov, &btjs);
    dsec_p1 = init_2dc_float(nscan, nfov, &dsec);
    obmd_p1 = init_2dc_char (nscan, nfov, &obmd);
    lza_p1  = init_2dc_float(nscan, nfov, &lza);
    sza_p1  = init_2dc_float(nscan, nfov, &sza);
    at_p1   = init_3dc_float(nscan, nfov, nchan, &at);

    flag0_p1= init_2dc_int (nscan, nfov, &flag0);

    nfova_p1 = init_1dc_int(nchan, &nfova);
    for (i=0; i<nchan; i++)    nfova[i] = nfov;

    *nscanp = nscan;    *nfovp  = nfov;    *nchanp = nchan;
    *nfovap = nfova;    *nfova_p1p = nfova_p1;
    *flag0p = flag0;    *flag0_p1p = flag0_p1;


    // read both data
    rmhs_scn0(fp_m, &bged_m, cwn_m, nscan_m, nfov_m, nchan_m,
            nscan_a, nfov_a, nchan_a, nscan);
    rama_scn0(fp_a, &bged_a, cwn_a, nscan_a, nfov_a, nchan_a,
            nscan_m, nfov_m, nchan_m, nscan);


    // qc
    for (iscan=0; iscan<nscan; iscan++)  {
        for (ifov=0; ifov<nfov; ifov++)  {

            flag0[iscan][ifov] = 0;
            // time
            if (btjs[iscan][ifov] < 0)  {
                flag0[iscan][ifov] |= (1U<<8);
            }
            // location
            if (lon[iscan][ifov]<-400 || lon[iscan][ifov]>400
                    || lat[iscan][ifov]<-100 || lat[iscan][ifov]>100)  {
                flag0[iscan][ifov] |= (1U<<9);
                //printf("lon, lat: %f, %f\n", lon[iscan][ipos], 
                //        lat[iscan][ipos]);
            }
            // at
            for (ichan=0; ichan<nchan; ichan++)  {
                tt = round(ti_sqc[0][ichan]);
                if (tt == 1)  {
                    if (at[iscan][ifov][ichan]<50
                            || at[iscan][ifov][ichan]>500)  {
                        flag0[iscan][ifov] |= (1U<<10);
                        //printf(" N");
                    }
                }
            }  // ichan

        }  // ifov
    }  // iscan

    free(cwn_m);
    free(cwn_a);
    fclose(fp_m);
    fclose(fp_a);
    //printf("Done mhs/amsua l1b data.\n");

    //exit(1);
    return(0);
}




