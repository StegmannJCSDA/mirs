/**************************************************************************
 *  Program Name      : cal_vars.c
 *  Type              : subroutine
 *  Function          : Save all the intermittent data for research
 *  Input data        : Satllite and model data
 *  Output data       : SFR product variables
 *  Subroutine Called : sat_var(), avn_var(), sd_comb(), onelyr_(), ...
 *  Called by         : process()
 *  Author            : Jun Dong, jundong@umd.edu, 03/19/2020
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <libgen.h>

#include "d_darrc.h"
#include "d_sys.h"
#include "d_jsec.h"

#define     NACOL   3
#define     NRCH    6
#define     SL9     512
#define     ITER_MAX    5


int     sat_var(int, int, int, float *, float *, float *, long long *);
int     sat_filter(float *, float *, float **, int *, int *, int *);
int     avn_update(char *, long long, char *, char *, int , int *, int *, int ,
            long long *, int *, int *);
int     avn_llt2ijr(float, float, long long, int *, int *, float *, 
            float *, float *, int *, int *, long long *);
int     avn_var(int, int, float, float **, int, int *, int, int, int *);
int     avn_add_wth(float **, float **, float *, int, float);
int     avn_filter(float **, float **, float *, int *, int *, int *);
int     sat_lrm(float *, float **, float **, float *, 
            float *, float *, int *);
int     sd_comb(float, float *, float *, float **, 
            float **, float *, float, float *, int *, int *);

int     init_tbo(float *, float *, float **, float *, float **, float *);
int     init_iwp(float *, float **, float *, float **, float *);
int     init_de(float *, float **, float *, float **, float *);
int     init_tc(float *, float **, float *, float **, float *);
int     init_em(float *, float **, float *, float **, float *);

extern void onelyr_(float[], float[], float *, float *, float *, float *, 
                    float *, float *, float[], 
                    float[], float[], float[], float[], float[], int *);

float   sfr_intg(float, float, float, float, float);

int     sfr_cal_max(float *, float **, float *, float **, float *);
int     sfr_cal_layer(float *, float **, float *, float **, float *);

void    prtbin(int, int);

int     sat_var_add();
int     sat_add_user();

//****************************************************************************
// yf: function to compute Twet2m and Lapse rate @ 500m 
int avn_add_tw_lpsrt(float **, int, int, int);

// yf: MLNN functions
int sdnn_run(float, float, float *, float **, 
             int *, int **, float **, float **,float *, int *, int *, float ***, float **,
             int *, int *, float *,float *, 
             int *, int **, float **, float **,float *, int *, int *, float ***, float **,
             int *, int *, float *,float *, 
             int *, int **, float **, float **,float *, int *, int *, float ***, float **,
             int *, int *, float *,float *, 
             float *, int *);
int tbnn_run(float, float, float *, float **, 
             int *, int **, float **, float **,float *, int *, int *, float ***, float **, 
             int *, int **, float **, float **,float *, int *, int *, float ***, float **, 
             int *, int **, float **, float **,float *, int *, int *, float ***, float **, 
             float *); 
int iwpnn_run(float, float, float *, float **, 
              int *, int **, float **, float **,float *, int *, int *, float ***, float **,
              int *, int **, float **, float **,float *, int *, int *, float ***, float **,
              int *, int **, float **, float **,float *, int *, int *, float ***, float **,
              float **, float *);
int tcnn_run(float, float, float *, float **, 
             int *, int **, float **, float **,float *, int *, int *, float ***, float **,
             int *, int **, float **, float **,float *, int *, int *, float ***, float **,
             int *, int **, float **, float **,float *, int *, int *, float ***, float **,
             float *);
int sfrnn_run(float, float, float *, float **, 
              int *, int **, float **, float **,float *, int *, int *, float ***, float **, 
              int *, float *, int *, 
              int *, int **, float **, float **,float *, int *, int *, float ***, float **, 
              int *, float *, int *,
              int *, int **, float **, float **,float *, int *, int *, float ***, float **, 
              int *, float *, int *,
              float *);               
//****************************************************************************
// yf: xgb function
int xgb_classifier(float *, float **, int *, int *, int **,
                   int *, float **, double ***, float *, int *);
                   
// avn data
extern  float       ****hgt_a, ****tmp_a, ****rh_a, ****vvel_a, ****clwmr_a,
                    ***rh2m_a, ***pres0m_a, ***hgt0m_a, ***cwat_a, ***ugrd_a,
                    ***vgrd_a, ***pwat_a, ***tmp0m_a, ***tmp2m_a;
// sat data
extern  float       **lon, **lat, ***at, **lza, **sza;
extern  char        **obmd;
extern  long long   **btjs;
// mirs need
extern  float       **dsec;

// product
extern  int         **stype, **sd, **flag;
extern  float       **sfr, **prob, **iwp, **de;

// index table
extern  float       *hd_sqc, **ti_sqc;
extern  float       *nti1, **ti1, *hd_suv, **ti_suv, *nti3, **ti3,*nti4,**ti4;
extern  float       *nti5, **ti5, *nti6, **ti6, *nti7, **ti7;
extern  float       *hd_one, **ti_one, *hd_tbo, **ti_tbo, *hd_iwp, **ti_iwp; 
extern  float       *hd_de,  **ti_de,  *hd_tc,  **ti_tc;
extern  float       *hd_em,  **ti_em;
extern  float       *hd_frc, **ti_frc, *hd_freq,**ti_freq;
extern  float       *hd_ecm, **ti_ecm, *hd_kv,  **ti_kv,  *hd_ko2, **ti_ko2;
extern  float       *hd_cal, **ti_cal, *hd_layer, **ti_layer;

//*************************************************************************************************
//yf: nn arrays
extern  int         *flg_do_sdnn, *flg_do_tbnn, *flg_do_iwpnn, *flg_do_tcnn, *flg_do_sfrnn;

extern  int         *nlayers_sdnn_gb, *layers_sdnn_gb, **varid_sdnn_gb, *nntype_sdnn_gb;
extern  float       **norm_in_sdnn_gb,**norm_out_sdnn_gb,***weights_sdnn_gb,**bias_sdnn_gb;
extern  int         *varid_threshold_adj_sdnn_gb, *threshold_adj_nsec_sdnn_gb;
extern  float       *threshold_adj_sec_sdnn_gb, *threshold_sdnn_gb, *geolimit_sdnn_gb;

extern  int         *nlayers_sdnn_us, *layers_sdnn_us, **varid_sdnn_us, *nntype_sdnn_us;
extern  float       **norm_in_sdnn_us,**norm_out_sdnn_us,***weights_sdnn_us,**bias_sdnn_us;
extern  int         *varid_threshold_adj_sdnn_us, *threshold_adj_nsec_sdnn_us;
extern  float       *threshold_adj_sec_sdnn_us, *threshold_sdnn_us, *geolimit_sdnn_us;

extern  int         *nlayers_sdnn_ak, *layers_sdnn_ak, **varid_sdnn_ak, *nntype_sdnn_ak;
extern  float       **norm_in_sdnn_ak,**norm_out_sdnn_ak,***weights_sdnn_ak,**bias_sdnn_ak;
extern  int         *varid_threshold_adj_sdnn_ak, *threshold_adj_nsec_sdnn_ak;
extern  float       *threshold_adj_sec_sdnn_ak, *threshold_sdnn_ak, *geolimit_sdnn_ak;

extern  int         *nlayers_tbnn_gb, *layers_tbnn_gb, **varid_tbnn_gb, *nntype_tbnn_gb;
extern  float       **norm_in_tbnn_gb,**norm_out_tbnn_gb,***weights_tbnn_gb,**bias_tbnn_gb, *geolimit_tbnn_gb;

extern  int         *nlayers_tbnn_us, *layers_tbnn_us, **varid_tbnn_us, *nntype_tbnn_us;
extern  float       **norm_in_tbnn_us,**norm_out_tbnn_us,***weights_tbnn_us,**bias_tbnn_us, *geolimit_tbnn_us;

extern  int         *nlayers_tbnn_ak, *layers_tbnn_ak, **varid_tbnn_ak, *nntype_tbnn_ak;
extern  float       **norm_in_tbnn_ak,**norm_out_tbnn_ak,***weights_tbnn_ak,**bias_tbnn_ak, *geolimit_tbnn_ak;

extern  int         *nlayers_iwpnn_gb, *layers_iwpnn_gb, **varid_iwpnn_gb, *nntype_iwpnn_gb;
extern  float       **norm_in_iwpnn_gb,**norm_out_iwpnn_gb,***weights_iwpnn_gb,**bias_iwpnn_gb, *geolimit_iwpnn_gb;

extern  int         *nlayers_iwpnn_us, *layers_iwpnn_us, **varid_iwpnn_us, *nntype_iwpnn_us;
extern  float       **norm_in_iwpnn_us,**norm_out_iwpnn_us,***weights_iwpnn_us,**bias_iwpnn_us, *geolimit_iwpnn_us;

extern  int         *nlayers_iwpnn_ak, *layers_iwpnn_ak, **varid_iwpnn_ak, *nntype_iwpnn_ak;
extern  float       **norm_in_iwpnn_ak,**norm_out_iwpnn_ak,***weights_iwpnn_ak,**bias_iwpnn_ak, *geolimit_iwpnn_ak;

extern  int         *nlayers_tcnn_gb, *layers_tcnn_gb, **varid_tcnn_gb, *nntype_tcnn_gb;
extern  float       **norm_in_tcnn_gb,**norm_out_tcnn_gb,***weights_tcnn_gb,**bias_tcnn_gb, *geolimit_tcnn_gb;

extern  int         *nlayers_tcnn_us, *layers_tcnn_us, **varid_tcnn_us, *nntype_tcnn_us;
extern  float       **norm_in_tcnn_us,**norm_out_tcnn_us,***weights_tcnn_us,**bias_tcnn_us, *geolimit_tcnn_us;

extern  int         *nlayers_tcnn_ak, *layers_tcnn_ak, **varid_tcnn_ak, *nntype_tcnn_ak;
extern  float       **norm_in_tcnn_ak,**norm_out_tcnn_ak,***weights_tcnn_ak,**bias_tcnn_ak, *geolimit_tcnn_ak;

extern  int         *nlayers_sfrnn_gb, *layers_sfrnn_gb, **varid_sfrnn_gb, *nntype_sfrnn_gb, *flg_post_adj_sfrnn_gb, *post_adj_varid_sfrnn_gb;
extern  float       **norm_in_sfrnn_gb,**norm_out_sfrnn_gb,***weights_sfrnn_gb,**bias_sfrnn_gb, *post_adj_sfrnn_gb, *geolimit_sfrnn_gb;

extern  int         *nlayers_sfrnn_us, *layers_sfrnn_us, **varid_sfrnn_us, *nntype_sfrnn_us, *flg_post_adj_sfrnn_us, *post_adj_varid_sfrnn_us;
extern  float       **norm_in_sfrnn_us,**norm_out_sfrnn_us,***weights_sfrnn_us,**bias_sfrnn_us, *post_adj_sfrnn_us, *geolimit_sfrnn_us;

extern  int         *nlayers_sfrnn_ak, *layers_sfrnn_ak, **varid_sfrnn_ak, *nntype_sfrnn_ak, *flg_post_adj_sfrnn_ak, *post_adj_varid_sfrnn_ak;
extern  float       **norm_in_sfrnn_ak,**norm_out_sfrnn_ak,***weights_sfrnn_ak,**bias_sfrnn_ak, *post_adj_sfrnn_ak, *geolimit_sfrnn_ak;
//*********************************************************************************************************
// yf: xgb array
extern  int         *flg_do_xgb;
extern  int         *ninput_xgb,*ntrees,**varid_xgb, *nnodes, *nfilter;
extern  float       **flt_xgb;
extern  double      ***xgb_trees;

int cal_vars(char *ffn, char *sen, int nscan, int nfov, int nchan, int *nfova, 
        int **flag0, char *a_name, long long *ajsec, int *aflag, int *aind, 
        char *a_dn_pat, char *a_fn_pat, int *a_t0dt, 
        float *a_l0, float *a_dl, int *a_nl, 
        int a_nlev, int a_navn, int a_nvar2, float *sfr_lim, float *ss_lut_p1,
        int *flg_do_sdnn, int *flg_do_tbnn, int *flg_do_iwpnn, int *flg_do_tcnn, int *flg_do_sfrnn,
        int *flg_do_xgb)
{
    long long   p_jsec;
    float       p_lon, p_lat;
    int         p_flag, p_flags, p_flaga; //yf: separate sat and avn filter flags

    float       prob_a[NACOL], prob_s, pthhd, p_prob;
    float       p_sfr, bsfr;

    int         iscan, ifov, navadd, nsvar, navar, p_sd;

    float       tbo[NRCH], tbo2[NRCH], p_em0[NRCH];
    float       p_iwp0, p_de0, p_tc0, p_lza;
    float       p_tmp0m, p_tpw, p_rh2m, p_pres0m, p_val; 

    int         asd, asf, aad, aaf, ilon, ilat;
    float       rot;
    long long   ajsec1[a_navn];

    int         i, ii, rst, nnst;


    // nsvar, navar
    nsvar = nchan + 4 + round(nti1[0]) + round(hd_suv[1]) 
        + round(ti_one[3][0]) + round(hd_cal[1]) + round(hd_layer[0]);
    navadd = round(nti4[0]);
    navar = a_nvar2 + navadd + 2; //yf: added 2 more for Twet2m and LapseRate 500m
    //printf("nsvar, navar: %d, %d\n", nsvar, navar);
    //getchar();

    float       svar[nsvar], **avar, *avar_p1;
    avar_p1 = init_2dc_float(navar, NACOL, &avar);

    //cal
    FILE    *fp1, *fp2;
    char    ffn1[SL9], ffn2[SL9];

    int     nrvar;
    nrvar = NRCH + NRCH + 5 + 1;

    float   rvar[nrvar][2];
    int     icol;
    //int     iter_max;

    strncpy(ffn1, ffn, SL9);
    strrepa(ffn1, "SFR_", "SF1_");
    strrepa(ffn1, ".hdf", ".bin");
    printf("ffn_out: %s\n", ffn);
    printf("sf1_out: %s\n", ffn1);
    fp1 = fopen(ffn1, "wb");
    if (fp1 == NULL)  {
        printf("Fail to open output file %s\n", ffn1);
        exit(1);
    }

    strncpy(ffn2, ffn, SL9);
    strrepa(ffn2, "SFR_", "SF2_");
    strrepa(ffn2, ".hdf", ".bin");
    printf("ffn_out: %s\n", ffn);
    printf("sf2_out: %s\n", ffn2);
    fp2 = fopen(ffn2, "wb");
    if (fp2 == NULL)  {
        printf("Fail to open output file %s\n", ffn2);
        exit(1);
    }

    int     noretrv;
    
    
    
    // Generate SFR product
    for (iscan=0; iscan<nscan; iscan++)  {
        //printf("iscan: %d/%d\n", iscan, nscan);
        for (ifov=0; ifov<nfov; ifov++)  {
            //printf("ifile, iscan, ifov, btjs: %d, %d/%d, %d/%d, %lld\n", 
            //        ifile, iscan, nscan, ifov, nfov, btjs[iscan][ifov]); 

            noretrv = 0;            
            /////////////////////////
            // init output data, again
            ////////////////////////
            sfr[iscan][ifov]  = -99;
            sd[iscan][ifov]   = -99;
            prob[iscan][ifov] = -99;
            iwp[iscan][ifov]  = -99;
            de[iscan][ifov]   = -99;

            //cal
            for (i=0; i<nsvar; i++)    svar[i]    = -99;
            for (i=0; i<navar; i++)    avar[i][0] = -99;
            for (i=0; i<nrvar; i++)    rvar[i][0] = -99;
            for (i=0; i<nrvar; i++)    rvar[i][1] = -99;
            
            //20211208 yf: reset flag0
            flag0[iscan][ifov] = 0;

            ////////////////////
            // stype
            ////////////////////
            if (stype[iscan][ifov] != 1)  {
                flag0[iscan][ifov] |= (1U<<16);
                sfr[iscan][ifov]  = -10;
                sd[iscan][ifov]   = -10;
                prob[iscan][ifov] = -10;
                iwp[iscan][ifov]  = -10;
                de[iscan][ifov]   = -10;

                noretrv = 1;
            }            

            //////////////////////
            // sat var
            ////////////////////
            sat_var(iscan, ifov, nchan, svar, &p_lon, &p_lat, &p_jsec);
            
            //20211208 yf: flag0 has been changed, use lat/lon to deal with bad data
            for (i=0; i<nchan; i++){
                ii = round(ti_sqc[0][i]);
                if (ii==1) {
                    if (svar[i]<50 || svar[i]>330.01)
                        flag0[iscan][ifov] |= (1U<<10);
                }                    
            }
            if (p_jsec == -999)
                flag0[iscan][ifov] |= (1U<<8);
            
            if (p_lon<-180 || p_lon>180 || p_lat<-90 || p_lat>90)
                flag0[iscan][ifov] |= (1U<<9);
            
            ////////////////////
            // init local vars
            ////////////////////
            p_flag = flag0[iscan][ifov];
            
            if (flag0[iscan][ifov] != 0)  {
                for (i=0; i<nsvar; i++)    svar[i]    = -99;
                //cal
                fwrite(avar_p1, 4, navar*NACOL, fp1);
                fwrite(svar, 4, nsvar, fp1);
                fwrite(rvar, 4, nrvar*2, fp2);
                continue;
            }
            
            sat_var_add(svar, nti1, ti1);
            sat_add_user(svar, avar, hd_suv, ti_suv, &p_val);
            //printf("svar: %f, %f\n", svar[0], svar[1]);
            //getchar();    

            //////////////////////
            // sat filter
            ////////////////////
            if (noretrv == 0)  {
                sat_filter(svar, nti3, ti3, &p_flags, &asd, &asf);
                //*********************************************************
                //yf: only set flag if using LRSD
                if (*flg_do_xgb==0)  { //yf: modify to use xgb, not sdnn
                    if (asd > 0)  {
                        flag0[iscan][ifov] = p_flags;
                        sfr[iscan][ifov]  = asf;
                        sd[iscan][ifov]   = asf;
                        prob[iscan][ifov] = asf;
                        iwp[iscan][ifov]  = asf;
                        de[iscan][ifov]   = asf;

                        noretrv = 1;
                    }
                }
                //*********************************************************
            }

            ///////////////////////
            // avn update, based on date/time
            ///////////////////////
            avn_update(a_name, p_jsec, a_dn_pat, a_fn_pat, 
                    a_navn, a_t0dt, a_nl, a_nlev, ajsec, aind, aflag);
            if (aflag[aind[0]] != 0)    p_flag |= (1U<<13);
            if (aflag[aind[1]] != 0)    p_flag |= (1U<<14);
            if (aflag[0] != 0 || aflag[1] != 0)  {
                flag0[iscan][ifov] = p_flag;
                sfr[iscan][ifov]  = -98;
                sd[iscan][ifov]   = -98;
                prob[iscan][ifov] = -98;
                iwp[iscan][ifov]  = -98;
                de[iscan][ifov]   = -98;
                //printf("Cannot update avn\n");
                //printf("%s\n%s\n", a_dn_pat, a_fn_pat);

                //cal
                fwrite(avar_p1, 4, navar*NACOL, fp1);
                fwrite(svar, 4, nsvar, fp1);
                fwrite(rvar, 4, nrvar*2, fp2);
                continue;
            }
            ///////////////////////
            // avn var
            ///////////////////////
            for (i=0; i<a_navn; i++)    ajsec1[i] = ajsec[aind[i]];
            avn_llt2ijr(p_lon, p_lat, p_jsec, &ilon, &ilat, &rot, 
                    a_l0, a_dl, a_nl, a_t0dt, ajsec1);
            avn_var(ilon, ilat, rot, avar, a_navn, a_nl, a_nlev, 
                    a_nvar2, aind);
            avn_add_wth(avar, ti4, nti4, a_nlev, rot);
            
            //**********************************************************
            //yf: new function to compute Twet2m and Lapse rate at 500m
            avn_add_tw_lpsrt(avar, a_nlev, a_nvar2, navadd);
            //**********************************************************
            //printf("cal_vars: done avn filter\n");
            // save for sd_comb
            for (i=0; i<NACOL; i++)  {
                prob_a[i] = avar[a_nvar2+navadd-1][i];
            }
            //printf("cal_vars: done avn add\n");
            //////////////////////////
            // avn filter
            //////////////////////////
            avn_filter(avar, ti5, nti5, &p_flaga, &aad, &aaf);
            //*******************************************************
            //yf: only set flag if using LRSD
            if (*flg_do_xgb==0)  { // yf: modify to use xgb, not sdnn, and disable avn filter for xgb
                if (aad > 0)  {
                    flag0[iscan][ifov] = p_flaga;
                    sfr[iscan][ifov]  = aaf;
                    sd[iscan][ifov]   = aaf;
                    prob[iscan][ifov] = aaf;
                    iwp[iscan][ifov]  = aaf;
                    de[iscan][ifov]   = aaf;

                    //cal
                    fwrite(avar_p1, 4, navar*NACOL, fp1);
                    fwrite(svar, 4, nsvar, fp1);
                    fwrite(rvar, 4, nrvar*2, fp2);
                    continue;
                }
                else  {
                    sd[iscan][ifov] = 1;
                }
            }
            else{
                sd[iscan][ifov] = 1;                
            }
            //*******************************************************************
            //printf("cal_vars: done avn filter\n");
            //*******************************************************************
            // yf: add nn function for SD 
            nnst=10;
            // yf: disable sdnn, use xgb
            //printf("cal_vars: before xgbsd\n");
            if (*flg_do_xgb==1){
                nnst = xgb_classifier(svar, avar, ntrees, ninput_xgb, varid_xgb,
                        nfilter, flt_xgb, xgb_trees,&p_prob,&p_sd);
            }
            //printf("cal_vars: done xgbsd\n");
            /*
            if (*flg_do_sdnn==1){
                nnst = sdnn_run(p_lat,p_lon, svar, avar, 
                                nntype_sdnn_gb, varid_sdnn_gb, norm_in_sdnn_gb, norm_out_sdnn_gb,
                                geolimit_sdnn_gb, nlayers_sdnn_gb, layers_sdnn_gb, weights_sdnn_gb, bias_sdnn_gb,
                                varid_threshold_adj_sdnn_gb, threshold_adj_nsec_sdnn_gb, threshold_adj_sec_sdnn_gb, threshold_sdnn_gb, 
                                nntype_sdnn_us, varid_sdnn_us, norm_in_sdnn_us, norm_out_sdnn_us,
                                geolimit_sdnn_us, nlayers_sdnn_us, layers_sdnn_us, weights_sdnn_us, bias_sdnn_us,
                                varid_threshold_adj_sdnn_us, threshold_adj_nsec_sdnn_us, threshold_adj_sec_sdnn_us, threshold_sdnn_us,
                                nntype_sdnn_ak, varid_sdnn_ak, norm_in_sdnn_ak, norm_out_sdnn_ak,
                                geolimit_sdnn_ak, nlayers_sdnn_ak, layers_sdnn_ak, weights_sdnn_ak, bias_sdnn_ak,
                                varid_threshold_adj_sdnn_ak, threshold_adj_nsec_sdnn_ak, threshold_adj_sec_sdnn_ak, threshold_sdnn_ak,
                                &p_prob, &p_sd);
            }
            */

            if (nnst>0){
                //reapply sat and avn filter if NNSD fails
                if(asd > 0){
                    flag0[iscan][ifov] = p_flags;
                    sfr[iscan][ifov]  = asf;
                    sd[iscan][ifov]   = asf;
                    prob[iscan][ifov] = asf;
                    iwp[iscan][ifov]  = asf;
                    de[iscan][ifov]   = asf;
                    //cal
                    fwrite(avar_p1, 4, navar*NACOL, fp1);
                    fwrite(svar, 4, nsvar, fp1);
                    fwrite(rvar, 4, nrvar*2, fp2);
                    continue; 
                }
                // if (aad > 0 )  {
                    // flag0[iscan][ifov] = p_flaga;
                    // sfr[iscan][ifov]  = aaf;
                    // sd[iscan][ifov]   = aaf;
                    // prob[iscan][ifov] = aaf;
                    // iwp[iscan][ifov]  = aaf;
                    // de[iscan][ifov]   = aaf;
                    // //cal
                    // fwrite(avar_p1, 4, navar*NACOL, fp1);
                    // fwrite(svar, 4, nsvar, fp1);
                    // fwrite(rvar, 4, nrvar*2, fp2);
                    // continue;
                // }
                
                //////////////////////////
                // sat lr model
                //////////////////////////
                sat_lrm(svar, avar, ti6, nti6, &prob_s, &pthhd, &p_flag);

                ///////////////////////////
                // sd combine
                ///////////////////////////
                sd_comb(prob_s, prob_a, svar, avar, 
                        ti7, nti7, pthhd, &p_prob, &p_sd, &p_flag);
                //printf("p_prob, p_sd: %f, %d\n", p_prob, p_sd);
                //printf("prob_a[*]: %f, %f, %f\n", 
                //        prob_a[0], prob_a[1], prob_a[2]);
                flag0[iscan][ifov] = p_flag;
            }
            //*******************************************************************

            // final sd, if not detect, next fov
            if (sd[iscan][ifov]==1 && p_sd==1)  {
                sd[iscan][ifov] = 1;
                prob[iscan][ifov] = p_prob;
            }
            else if (sd[iscan][ifov]==1 && p_sd==0)  {
                sd[iscan][ifov] = 0;
                prob[iscan][ifov] = p_prob;
                sfr[iscan][ifov] = 0;
                flag0[iscan][ifov] |= (1U<<25);

                //cal
                fwrite(avar_p1, 4, navar*NACOL, fp1);
                fwrite(svar, 4, nsvar, fp1);
                fwrite(rvar, 4, nrvar*2, fp2);
                continue;
            }
            else if (sd[iscan][ifov]==0 && p_sd==1)  {
                sd[iscan][ifov] = 0;
                prob[iscan][ifov] = p_prob;
                sfr[iscan][ifov]  = 0;

                //cal
                fwrite(avar_p1, 4, navar*NACOL, fp1);
                fwrite(svar, 4, nsvar, fp1);
                fwrite(rvar, 4, nrvar*2, fp2);
                continue;
            }
            else if (sd[iscan][ifov]==0 && p_sd==0)  {
                sd[iscan][ifov] = 0;
                prob[iscan][ifov] = p_prob;
                sfr[iscan][ifov]  = 0;

                //cal
                fwrite(avar_p1, 4, navar*NACOL, fp1);
                fwrite(svar, 4, nsvar, fp1);
                fwrite(rvar, 4, nrvar*2, fp2);
                continue;
            }
            else  {
                printf("Wrong sd: %d, %d\n", sd[iscan][ifov], p_sd);
                exit(1);
            }

            //////////////////////////
            // one layer RTM
            //////////////////////////

            //prep_svar(svar, hd9, ti9, tbo, tbo0);
            //printf("---------------------------\n");
            //printf("tbo: in c:\n");
            for (i=0; i<NRCH; i++)  {
                ii = round(ti_one[0][i+1]);
                tbo[i] = svar[ii];
                //printf("%12.6f", tbo[i]);
            }
            //printf("\n");
            ii = round(ti_one[1][1]);        p_lza    = svar[ii];
            ii = round(ti_one[2][1]);        p_tmp0m  = avar[ii][0];
            ii = round(ti_one[2][2]);        p_tpw    = avar[ii][0];
            ii = round(ti_one[2][3]);        p_rh2m   = avar[ii][0];
            ii = round(ti_one[2][4]);        p_pres0m = avar[ii][0];

            //**************************************************************************
            //yf: add NN funtion for init_tbo, init_iwp, init_tc
            // tbo init, with bias-removed            
            nnst=10;            
            if (*flg_do_tbnn==1){
                nnst=tbnn_run(p_lat, p_lon, svar, avar, 
                              nntype_tbnn_gb, varid_tbnn_gb, norm_in_tbnn_gb, norm_out_tbnn_gb,
                              geolimit_tbnn_gb, nlayers_tbnn_gb, layers_tbnn_gb, weights_tbnn_gb, bias_tbnn_gb,
                              nntype_tbnn_us, varid_tbnn_us, norm_in_tbnn_us, norm_out_tbnn_us,
                              geolimit_tbnn_us, nlayers_tbnn_us, layers_tbnn_us, weights_tbnn_us, bias_tbnn_us,
                              nntype_tbnn_ak, varid_tbnn_ak, norm_in_tbnn_ak, norm_out_tbnn_ak,
                              geolimit_tbnn_ak, nlayers_tbnn_ak, layers_tbnn_ak, weights_tbnn_ak, bias_tbnn_ak,
                              tbo2);
            }
            if (nnst>0){                         
                init_tbo(tbo, svar, avar, hd_tbo, ti_tbo, tbo2);
            }
            
            // init iwp
            nnst=10;
            if (*flg_do_iwpnn==1){
                nnst=iwpnn_run(p_lat, p_lon, svar, avar, 
                               nntype_iwpnn_gb, varid_iwpnn_gb, norm_in_iwpnn_gb, norm_out_iwpnn_gb,
                               geolimit_iwpnn_gb, nlayers_iwpnn_gb, layers_iwpnn_gb, weights_iwpnn_gb, bias_iwpnn_gb,
                               nntype_iwpnn_us, varid_iwpnn_us, norm_in_iwpnn_us, norm_out_iwpnn_us,
                               geolimit_iwpnn_us, nlayers_iwpnn_us, layers_iwpnn_us, weights_iwpnn_us, bias_iwpnn_us,
                               nntype_iwpnn_ak, varid_iwpnn_ak, norm_in_iwpnn_ak, norm_out_iwpnn_ak,
                               geolimit_iwpnn_ak, nlayers_iwpnn_ak, layers_iwpnn_ak, weights_iwpnn_ak, bias_iwpnn_ak,
                               ti_iwp, &p_iwp0);
            }
            if(nnst>0){                
                init_iwp(svar, avar, hd_iwp, ti_iwp, &p_iwp0);
                //printf("p_iwp0: %f\n", p_iwp0);
            }
            //********************************************************************************

            // init de
            init_de(svar, avar, hd_de, ti_de, &p_de0);
            //printf("p_de0: %f\n", p_de0);

            //**************************************************************************
            //yf: add NN funtion for init_tc
            // init tc
            nnst=10;
            if (*flg_do_tcnn==1){
                nnst=tcnn_run(p_lat, p_lon, svar, avar, 
                              nntype_tcnn_gb, varid_tcnn_gb, norm_in_tcnn_gb, norm_out_tcnn_gb,
                              geolimit_tcnn_gb, nlayers_tcnn_gb, layers_tcnn_gb, weights_tcnn_gb, bias_tcnn_gb,
                              nntype_tcnn_us, varid_tcnn_us, norm_in_tcnn_us, norm_out_tcnn_us,
                              geolimit_tcnn_us, nlayers_tcnn_us, layers_tcnn_us, weights_tcnn_us, bias_tcnn_us,
                              nntype_tcnn_ak, varid_tcnn_ak, norm_in_tcnn_ak, norm_out_tcnn_ak,
                              geolimit_tcnn_ak, nlayers_tcnn_ak, layers_tcnn_ak, weights_tcnn_ak, bias_tcnn_ak,
                              &p_tc0);
            }
            if(nnst>0){
                init_tc(svar, avar, hd_tc, ti_tc, &p_tc0);
                //printf("p_tc0: %f\n", p_tc0);
            }    
            //***************************************************************************

            // init em
            init_em(svar, avar, hd_em, ti_em, p_em0);

            ///////////////////////////////////
            // save rvar init
            ///////////////////////////////////
            icol = 0;
            for (i=0; i<NRCH; i++)  {
                rvar[i][icol] = tbo2[i];
                rvar[NRCH+i][icol] = p_em0[i];
            }
            rvar[NRCH*2+0][icol] = p_tpw;
            rvar[NRCH*2+1][icol] = p_iwp0;
            rvar[NRCH*2+2][icol] = p_de0;
            rvar[NRCH*2+3][icol] = p_tc0;
            rvar[NRCH*2+4][icol] = p_tmp0m;
            rvar[NRCH*2+5][icol] = ITER_MAX;


            ///////////////////////////////////
            // call external one layer rtm
            ///////////////////////////////////
            rst = ITER_MAX; 
            onelyr_(tbo2, p_em0, &p_tpw, &p_iwp0, &p_de0, &p_tc0, &p_tmp0m, 
                    &p_lza, ti_ecm[0], ti_kv[0], ti_ko2[0], ti_frc[0], 
                    ti_freq[0], ss_lut_p1, &rst);
            ////////////////////////////////////
            //printf("iwp, de, rst: %12.6f, %12.6f, %d\n", p_iwp0, p_de0, rst);

            // rtm return
            //if (rst == -1)  {
            if (rst == ITER_MAX)  {
                p_flag |= (1U<<0);
                flag0[iscan][ifov] = p_flag;
            }


            ///////////////////////////////////
            // save rvar init
            ///////////////////////////////////
            icol = 1;
            for (i=0; i<NRCH; i++)  {
                rvar[i][icol] = tbo2[i];
                rvar[NRCH+i][icol] = p_em0[i];
            }
            rvar[NRCH*2+0][icol] = p_tpw;
            rvar[NRCH*2+1][icol] = p_iwp0;
            rvar[NRCH*2+2][icol] = p_de0;
            rvar[NRCH*2+3][icol] = p_tc0;
            rvar[NRCH*2+4][icol] = p_tmp0m;
            rvar[NRCH*2+5][icol] = rst;


            //////////////////////////
            // integrate terminal velocity, bsfr
            //////////////////////////
            if (p_iwp0 <= 0.)  {
                sd[iscan][ifov] = 0;        sfr[iscan][ifov] = 0;
                iwp[iscan][ifov] = 0;       de[iscan][ifov] = 0;
                p_flag |= (1U<<27);
                flag0[iscan][ifov] = p_flag;

                //cal
                fwrite(avar_p1, 4, navar*NACOL, fp1);
                fwrite(svar, 4, nsvar, fp1);
                fwrite(rvar, 4, nrvar*2, fp2);
                continue;
            }
            bsfr = sfr_intg(p_iwp0, p_de0/1000, p_tmp0m, p_rh2m/100, p_pres0m);
            bsfr = bsfr * 2.0;
            //printf("bsfr: %f\n", bsfr);

            // add to svar
            //svar_add_idb(svar, hd_idb, ti_idb, p_iwp0, de, bsnowr);
            if (noretrv == 0)  {
                ii = round(ti_one[3][1]);
                if (rst == ITER_MAX)            svar[ii] = 1;
                else                            svar[ii] = 0;
                ii = round(ti_one[3][2]);       svar[ii] = p_iwp0;
                ii = round(ti_one[3][3]);       svar[ii] = p_de0;
                ii = round(ti_one[3][4]);       svar[ii] = bsfr;
            }
            else{
                ii = round(ti_one[3][1]);       svar[ii] = -99;
                ii = round(ti_one[3][2]);       svar[ii] = -99;
                ii = round(ti_one[3][3]);       svar[ii] = -99;
                ii = round(ti_one[3][4]);       svar[ii] = -99;
            }


            //////////////////////////
            // sfr cal max model
            //////////////////////////
            p_sfr = bsfr;
            sfr_cal_max(svar, avar, hd_cal, ti_cal, &p_sfr);
            //printf("max p_sfr: %f\n", p_sfr);


            //*******************************************************************
            // yf: add NN function for sfr calibration
            //////////////////////////
            // sfr cal layer model
            //////////////////////////
            p_sfr = p_sfr;
            nnst=10;
            if (*flg_do_sfrnn==1){
                nnst=sfrnn_run(p_lat, p_lon, svar, avar, 
                               nntype_sfrnn_gb, varid_sfrnn_gb, norm_in_sfrnn_gb, norm_out_sfrnn_gb,
                               geolimit_sfrnn_gb, nlayers_sfrnn_gb, layers_sfrnn_gb, weights_sfrnn_gb, bias_sfrnn_gb, 
                               flg_post_adj_sfrnn_gb, post_adj_sfrnn_gb, post_adj_varid_sfrnn_gb,
                               nntype_sfrnn_us, varid_sfrnn_us, norm_in_sfrnn_us, norm_out_sfrnn_us,
                               geolimit_sfrnn_us, nlayers_sfrnn_us, layers_sfrnn_us, weights_sfrnn_us, bias_sfrnn_us, 
                               flg_post_adj_sfrnn_us, post_adj_sfrnn_us, post_adj_varid_sfrnn_us,
                               nntype_sfrnn_ak, varid_sfrnn_ak, norm_in_sfrnn_ak, norm_out_sfrnn_ak,
                               geolimit_sfrnn_ak, nlayers_sfrnn_ak, layers_sfrnn_ak, weights_sfrnn_ak, bias_sfrnn_ak, 
                               flg_post_adj_sfrnn_ak, post_adj_sfrnn_ak, post_adj_varid_sfrnn_ak,
                               &p_sfr);
            }
            if(nnst>0){
                sfr_cal_layer(svar, avar, hd_layer, ti_layer, &p_sfr);
                //printf("layer p_sfr: %f\n", p_sfr);
            }
            //********************************************************************

            //cal
            fwrite(avar_p1, 4, navar*NACOL, fp1);
            fwrite(svar, 4, nsvar, fp1);
            fwrite(rvar, 4, nrvar*2, fp2);


            //////////////////////////
            // sfr limit
            //////////////////////////
            if (p_sfr < sfr_lim[0])  {
                p_sfr = 0;
                sd[iscan][ifov] = -17;
                sfr[iscan][ifov] = 0;
                iwp[iscan][ifov] = p_iwp0;
                de[iscan][ifov] = p_de0;
                p_flag |= (1U<<26);
                p_flag |= (1U<<1);
                flag0[iscan][ifov] = p_flag;
            }
            else if (p_sfr > sfr_lim[1])  {
                p_sfr = sfr_lim[1];
                sd[iscan][ifov] = 1;
                sfr[iscan][ifov] = p_sfr;
                iwp[iscan][ifov] = p_iwp0;
                de[iscan][ifov] = p_de0;
            }
            else  {
                sd[iscan][ifov] = 1;
                sfr[iscan][ifov] = p_sfr;
                iwp[iscan][ifov] = p_iwp0;
                de[iscan][ifov] = p_de0;
            }
            
        }
        //printf("\n");
    }

    fclose(fp1);
    fclose(fp2);

    free_2dc_float(navar, avar, avar_p1);

    return(0);
}






