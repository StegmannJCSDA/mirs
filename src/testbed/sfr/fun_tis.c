/***********************************************************************
 *  Program Name      : fun_tis.c
 *  Type              : Subroutine
 *  Function          : Reads table of indexing, read in coefs for 
 *                      each step and control each step
 *  Input Files       : None
 *  Output Files      : None
 *  Subroutine Called : sat_var_add(), sat_filter(), sat_lrm(), sfr_cal(), 
 *                      avn_filer() 
 *  Called by         : process.c, snowfall.c
 *  Author            : Jun Dong, jundong@umd.edu, 03/19/2019
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define     SL5     32
#define     SL9     512
#define     SL11    2048


int     get_one_line(FILE *, int, char *, int *);
int     rd_hd_ti(FILE *, float **, float ***);

extern  float   *hd_flb, **ti_flb, *hd_sqc, **ti_sqc;

extern  float   *nti1, **ti1, *hd_suv, **ti_suv, *nti3, **ti3, *nti4, **ti4;
extern  float   *nti5, **ti5, *nti6, **ti6, *nti7, **ti7;

extern  float   *hd_one, **ti_one, *hd_tbo, **ti_tbo, *hd_iwp, **ti_iwp; 
extern  float   *hd_de,  **ti_de,  *hd_tc,  **ti_tc;
extern  float   *hd_em,  **ti_em;
extern  float   *hd_frc, **ti_frc, *hd_freq,**ti_freq, *hd_ecm, **ti_ecm;
extern  float   *hd_kv,  **ti_kv,  *hd_ko2, **ti_ko2;

extern  float   *hd_cal, **ti_cal, *hd_layer, **ti_layer;
//*************************************************************************************************
//nn arrays
extern  int         *flg_rd_sdnn, *flg_rd_tbnn, *flg_rd_iwpnn, *flg_rd_tcnn, *flg_rd_sfrnn;
extern  int         *flg_do_sdnn, *flg_do_tbnn, *flg_do_iwpnn, *flg_do_tcnn, *flg_do_sfrnn;
extern  int         *flg_gb_sdnn, *flg_gb_tbnn, *flg_gb_iwpnn, *flg_gb_tcnn, *flg_gb_sfrnn;
extern  int         *flg_us_sdnn, *flg_us_tbnn, *flg_us_iwpnn, *flg_us_tcnn, *flg_us_sfrnn;
extern  int         *flg_ak_sdnn, *flg_ak_tbnn, *flg_ak_iwpnn, *flg_ak_tcnn, *flg_ak_sfrnn;

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
// xgb array
extern int          *flg_rd_xgb, *flg_do_xgb;
extern  int         *ninput_xgb,*ntrees,**varid_xgb, *nnodes, *nfilter;
extern  float       **flt_xgb;
extern double       ***xgb_trees;

// sigmoid function
float sigmoid(float x) {
     float result;
     result = 1 / (1 + exp(-x));
     return result;
}

void prtbin(int n, int len)
{
    int     i;

    for (i=len-1; i>=0; i--)   printf("%1d", i%10);    printf("\n");
    for (i=len-1; i>=0; i--)   {
        if (n & 1U<<i)      printf("1");
        else                printf("0");
    }
    printf("\n");
}

int sat_var_add(float *svar, float *nti1, float **ti1)
{
    int     i1, i2, i3, ip, nadd, nterm, nv, ii, pp, ip1;
    float   stt, tt;

    nadd = round(nti1[0]);
    for (i1=0; i1<nadd; i1++)  {
        // each chan
        nterm = round(ti1[i1][1]);
        ip = 2;
        stt = 0.0;
        for (i2=0; i2<nterm; i2++)  {
            // each term
            nv = round(ti1[i1][ip]);
            //printf("%d ", nv);
            //printf("%f ", ti1[i1][ip+1]);
            tt = 1;
            for (i3=0; i3<nv; i3++)  {
                // each variable
                ip1 = ip + 2 + i3*2;
                ii = round(ti1[i1][ip1]);
                pp = round(ti1[i1][ip1+1]);
                tt = tt * pow(svar[ii], pp);
                //printf("%d %d ", ii, pp);
            }
            stt = stt + ti1[i1][ip+1]*tt;
            ip = ip + 2 + nv*2;
            //printf("\n");
        }
        //printf("\n");

        ii = round(ti1[i1][0]);    svar[ii] = stt;
        //printf("svar[%d]: %f\n", ii, svar[ii]);
    }

    return(0);
}



int sat_add_user(float *svar, float **avar, float *hd, float **ti, 
        float *p_val)
{
    int     nseg, nout, iseg;
    int     iout, fdif, fneg;
    int     ic, ncond, fyes;

    int     iv, id, ilow, ihigh, ip, ip1;
    int     nterm, nv, fab, isexp;
    float   tt, stt, v1, var, x0, x1, pp, fmin, y;

    int     i1, i2, i3;

    nseg  = round(hd[0]);
    nout  = round(hd[1]);
    //printf("nseg, nout: %d, %d\n", nseg, nout);

    // if nout==0, skip this
    if (nout == 0)      return(-1);

    // main
    iseg = 0;
    for (i3=0; i3<nseg; i3++)  {

        // diff, negtive and min flag
        iout = round(ti[iseg+i3][0]);
        fdif = round(ti[iseg+i3][1]);
        fneg = round(ti[iseg+i3][2]);
        fmin = ti[iseg+i3][3];
        //printf("iout, fdif, fneg, fmin: %d, %d, %d, %f\n", 
        //        iout, fdif, fneg, fmin);

        // loop condition
        fyes = 0;
        ip = 4;
        ncond = round(ti[iseg+i3][ip]);
        ip = ip + 1;
        for (ic=0; ic<ncond; ic++)  {

            iv = round(ti[iseg+i3][ip+0]);
            id = round(ti[iseg+i3][ip+1]);
            if (iv == 0)
                var = svar[id];
            else if (iv ==1)
                var = avar[id][0];
            else  {
                printf("Wrong iv\n");
                exit(1);
            }
            //printf("var: %f\n", var);

            fab = round(ti[iseg+i3][ip+6]);
            if (fab == 0)
                ;
            else if (fab == 1)
                var = fabsf(var);
            else  {
                printf("Wrong fabs\n");
                exit(1);
            }
            //printf("fab, var: %d, %f\n", fab, var);

            x0 = ti[iseg+i3][ip+2];
            x1 = ti[iseg+i3][ip+3];
            ilow  = round(ti[iseg+i3][ip+4]);
            ihigh = round(ti[iseg+i3][ip+5]);

            //printf("iv, id: %d, %d\n", iv, id);
            //printf("x0, x1: %f, %f\n", x0, x1);
            //printf("ilow, ihigh: %d, %d\n", ilow, ihigh);
            //printf("var: %f\n", var);

            if (ilow==0 && ihigh==0)  {
                if (var>x0 && var<x1)       fyes++;
            }
            else if (ilow==1 && ihigh==0)  {
                if (var>=x0 && var<x1)      fyes++;
            }
            else if (ilow==0 && ihigh==1)  {
                if (var>x0 && var<=x1)      fyes++;
            }
            else if (ilow==1 && ihigh==1)  {
                if (var>=x0 && var<=x1)     fyes++;
            }
            else  {
                printf("Wrong bc\n");
                exit(1);
            }
            ip = ip + 7;
        }
        //printf("ncond, fyes: %d, %d\n", ncond, fyes);

        // check if satisfy
        if (fyes != ncond)  {
            // not satisfy all conds
            //printf("not satisfy, fyes/ncond: %d/%d\n", fyes, ncond);
            //getchar();
            continue;
        }
        
        nterm = round(ti[iseg+i3][ip]);
        //printf("nterm: %d\n", nterm);
        ip = ip + 1;

        // all terms loop
        stt = 0.0;
        for (i2=0; i2<nterm; i2++)  {
            nv = round(ti[iseg+i3][ip]);
            //printf("nv: %d\n", nv);

            tt = 1.0;
            // exp function
            isexp = 0;
            if (nv == 101)  {
                nv = 1;
                isexp = 1;
            }

            for (i1=0; i1<nv; i1++)  {
                // each variable
                ip1 = ip + 2 + i1*3;
                iv = round(ti[iseg+i3][ip1]);
                id = round(ti[iseg+i3][ip1+1]);
                //pp = round(ti[iseg+i3][ip1+2]);
                pp = ti[iseg+i3][ip1+2];
                //printf("iv, id, pp: %2d %2d %f\n", iv, id, pp);
                if (iv == 0)
                    v1 = svar[id];
                else if (iv == 1)
                    v1 = avar[id][0];
                else  {
                    printf("Wrong term iv\n");
                    exit(1);
                }
                //printf("v1: %16.8f\n", v1);

                if (isexp != 1)
                    tt = tt * pow(v1, pp);
                else  {
                    //printf("exp, coe: %f, %f\n", 
                    //        ti[iseg+i3][ip+1], 
                    //        ti[iseg+i3][ip+4]);
                    tt = exp(ti[iseg+i3][ip+4]*v1);
                }
                //printf("tt: %f\n", tt);
            }
            //printf("coe: %e\n", ti[iseg+i3][ip+1]);
            stt = stt + ti[iseg+i3][ip+1]*tt;
            //printf("stt: %f\n", stt);
            ip = ip + 2 + nv*3;
        }  // end nterms

        // regress variable or delta
        if (fdif == 0)
            y = stt;
        else if (fdif == 1)
            y = *p_val - stt;
        else  {
            printf("Wrong suv dif flag\n");
            exit(1);
        }

        // deal with <0
        if (fneg == -1)  {
            if (y < 0)      y = -y;
        }
        else if (fneg == 0)  {
            ;
        }
        else if (fneg == 1)  {
            if (y < 0)      y = fmin;
        }
        else  {
            printf("Wrong suv flag2\n");
            exit(1);
        }

        // set value
        svar[iout] = y;
        *p_val = y;

        //break;

    } // end iseg

    return(0);
}



int sat_filter(float *svar, float *nti3, float **ti3, 
        int *p_flag, int *asd, int *asf)
{
    int     nr, ilow, ihigh, ii, ibit;
    float   var;
    int     i;

    nr = round(nti3[0]);

    *asd = 0;
    for (i=0; i<nr; i++)  {
        ii    = round(ti3[i][0]);
        ilow  = round(ti3[i][4]);
        ihigh = round(ti3[i][5]);
        ibit  = round(ti3[i][6]);
        *asf  = round(ti3[i][7]);
        var   = svar[ii];
        //printf("i, ii, var, ibit, asf: %d, %d, %f, %d, %d\n", 
        //        i, ii, var, ibit, *asf);

        if (ilow==0 && ihigh==0)  {
            if (var>ti3[i][2] && var<ti3[i][3])  {
                *p_flag |= 1U << ibit;
                (*asd)++;
                break;
            }
        }
        else if (ilow==1 && ihigh==0)  {
            if (var>=ti3[i][2] && var<ti3[i][3])  {
                *p_flag |= 1U << ibit;
                (*asd)++;
                break;
            }
        }
        else if (ilow==0 && ihigh==1)  {
            if (var>ti3[i][2] && var<=ti3[i][3])  {
                *p_flag |= 1U << ibit;
                (*asd)++;
                break;
            }
        }
        else if (ilow==1 && ihigh==1)  {
            if (var>=ti3[i][2] && var<=ti3[i][3])  {
                *p_flag |= 1U << ibit;
                (*asd)++;
                break;
            }
        }
        else  {
            printf("Wrong boundary setting\n");
            exit(1);
        }
    }

    return(0);
}



int avn_filter(float **avar, float **ti5, float *nti5, 
        int *p_flag, int *aad, int *aaf)
{
    int     nr, ilow, ihigh, ii, ibit, jj;
    float   var;
    int     i, tt;

    nr = round(nti5[0]);

    *aad = 0;
    for (i=0; i<nr; i++)  {
        ii =    round(ti5[i][0]);
        tt =    round(ti5[i][1]);
        jj =    tt % 10;
        ilow =  round(ti5[i][4]);
        ihigh = round(ti5[i][5]);
        ibit =  round(ti5[i][6]);
        if (tt/10 > 0)  *aaf = round(ti5[i][7]);
        else            *aaf = 0;
        var = avar[ii][jj];
        //printf("i, ii, jj, var, ilow, ihigh, ibit, aaf: "
        //        "%d, %d, %d, %f, %d, %d, %d, %d\n", 
        //        i, ii, jj, var, ilow, ihigh, ibit, *aaf);

        if (ilow==0 && ihigh==0)  {
            if (var<=ti5[i][2] || var>=ti5[i][3])  {
                *p_flag |= 1U << ibit;
                (*aad)++;
                break;
            }
        }
        else if (ilow==1 && ihigh==0)  {
            if (var<ti5[i][2] || var>=ti5[i][3])  {
                *p_flag |= 1U << ibit;
                (*aad)++;
                break;
            }
        }
        else if (ilow==0 && ihigh==1)  {
            if (var<=ti5[i][2] || var>ti5[i][3])  {
                *p_flag |= 1U << ibit;
                (*aad)++;
                break;
            }
        }
        else if (ilow==1 && ihigh==1)  {
            if (var<ti5[i][2] || var>ti5[i][3])  {
                *p_flag |= 1U << ibit;
                (*aad)++;
                break;
            }
        }
        else  {
            printf("Wrong boundary setting\n");
            exit(1);
        }
    }
    //printf("aad, aaf: %d, %d\n", *aad, *aaf);
    //getchar();

    return(0);
}



int sat_lrm(float *svar, float **avar, float **ti6, float *nti6, 
        float *prob_s, float *pthhd, int *p_flag)
{
    int     i1, i2, i3, ii, ip, ip1, ii1, ii2;
    int     nseg, ilow, ihigh, nterm, nv, pp;
    float   var, tt, stt;

    // main var
    ii = round(nti6[1]);
    if (ii == 0)  {
        ii = round(nti6[2]);
        var = svar[ii];
    }
    else if (ii == 1)  {
        ii = round(nti6[2]);
        var = avar[ii][0];
    }
    else  {
        printf("Wrong index\n");
        exit(1);
    }
    nseg = round(nti6[0]);


    // find range
    ip = 3;
    for (i3=0; i3<nseg; i3++)  {
        //printf('iseg, hd(ip+2), hd(ip+3): %d, %f, %f\n', ...
        //    i3,hd(ip+2),hd(ip+3));
        //printf('iseg, hd(ip+0), hd(ip+1): %d, %f, %f\n', ...
        //    i3,hd(ip+0),hd(ip+1));
        ilow = round(nti6[ip+2]);    ihigh = round(nti6[ip+3]);
        if (ilow==0 && ihigh==0)  {
            if (var>nti6[ip+0] && var<nti6[ip+1])  {
                i1 = i3;
                break;
            }
        }
        else if (ilow==1 && ihigh==0)  {
            if (var>=nti6[ip+0] && var<nti6[ip+1])  {
                i1 = i3;
                break;
            }
        }
        else if (ilow==0 && ihigh==1)  {
            if (var>nti6[ip+0] && var<=nti6[ip+1])  {
                i1 = i3;
                break;
            }
        }
        else if (ilow==1 && ihigh==1)  {
            if (var>=nti6[ip+0] && var<=nti6[ip+1])  {
                i1 = i3;
                break;
            }
        }
        else  {
            printf("wrong bc\n");
            exit(1);
        }
        ip = ip + 6;
    }


    // flag
    ii = round(nti6[ip+5]);
    (*p_flag) |= (1U<<ii);
    *pthhd = nti6[ip+4];


    // calculate
    nterm = round(ti6[i1][0]);
    ip = 1;
    stt = 0;
    for (i2=0; i2<nterm; i2++)  {
        // each term
        nv = round(ti6[i1][ip]);
        tt = 1;
        //printf('%f',ti6{i1}(ip+1));
        for (i3=0; i3<nv; i3++)  {
            // each variable
            ip1 = ip + 2 + i3*3;
            ii1 = round(ti6[i1][ip1]);
            ii2 = round(ti6[i1][ip1+1]);
            pp  = round(ti6[i1][ip1+2]);
            if (ii1 == 0)  {
                tt = tt * pow(svar[ii2],pp);
                //printf(' svar(%d)^%d',ii2,pp);
            }
            else if (ii1 == 1)  {
                tt = tt * pow(avar[ii2][ii1],pp);
                //printf(' gvar(%d)^%d',ii2,pp);
            }
            else  {
                printf("Wrong svar/avar\n");
                exit(1);
            }
        }
        stt = stt + ti6[i1][ip+1]*tt;
        //printf('  %f  %f\n',tt,ti6{i1}(ip+1)*tt);
        ip = ip + 2 + nv*3;
    }

    *prob_s = exp(stt) / (1+exp(stt));

    return(0);
}



int sd_comb(float prob_s, float *prob_a, float *svar, float **avar, 
        float **ti7, float *nti7, float pthhd, float *p_prob, 
        int *p_sd, int *p_flag)
{
    int     i1, i3, ii, ip, ilow, ihigh, ibit, nseg;
    float   var, weight, prob1, prob2;

    // main var
    ii = round(ti7[0][1]);
    if (ii == 0)  {
        ii = round(ti7[0][2]);
        var = svar[ii];
    }
    else if (ii == 1)  {
        ii = round(ti7[0][2]);
        var = avar[ii][0];
    }
    else  {
        printf("Wrong svar/avar\n");
        exit(1);
    }
    nseg = round(ti7[0][0]);


    // find range
    ip = 3;
    for (i3=0; i3<nseg; i3++)  {
        ilow  = round(ti7[0][ip+2]);
        ihigh = round(ti7[0][ip+3]);
        ibit  = round(ti7[0][ip+5]);
        //printf("iseg, var, ibit: %d, %f, %d\n", i3, var, ibit);

        if (ilow==0 && ihigh==0)  {
            if (var>ti7[0][ip+0] && var<ti7[0][ip+1])  {
                i1 = i3;
                break;
            }
        }
        else if (ilow==1 && ihigh==0)  {
            if (var>=ti7[0][ip+0] && var<ti7[0][ip+1])  {
                i1 = i3;
                break;
            }
        }
        else if (ilow==0 && ihigh==1)  {
            if (var>ti7[0][ip+0] && var<=ti7[0][ip+1])  {
                i1 = i3;
                break;
            }
        }
        else if (ilow==1 && ihigh==1)  {
            if (var>=ti7[0][ip+0] && var<=ti7[0][ip+1])  {
                i1 = i3;
                break;
            }
        }
        else  {
            printf("wrong bc\n");
            exit(1);
        }
        ip = ip + 6;
    }


    // flag
    ii = round(ti7[0][ip+5]);
    (*p_flag) |= (1U<<ii);
    weight = ti7[0][ip+4];


    // prob and sd
    prob1 = weight*prob_a[1] + (1-weight)*prob_s;
    prob2 = weight*prob_a[2] + (1-weight)*prob_s;


    // combine
    //if (prob1 > pthhd)  {
    //    *p_prob = prob1;
    //}
    //else if (prob2 > pthhd)  {
    //    *p_prob = prob2;
    //}

    if (prob1 > prob2)      *p_prob = prob1;
    else                    *p_prob = prob2;

    // sd
    if (*p_prob > pthhd)    *p_sd = 1;
    else                    *p_sd = 0;

    return(0);
}



int init_tbo(float *tbo, float *svar, float **avar, float *hd_tbo, 
        float **ti_tbo, float *tbo2)
{
    int     nreg, nterm, nvar; 
    float   v1, tt, stt;

    int     i1, i2, i3, ip0, ip1, iv, id, pp, ff1, ff2;


    nreg = 6;
    nreg = round(hd_tbo[0]);
    //printf("nreg: %d\n", nreg);
    for (i1=0; i1<nreg; i1++)  {
        nterm = round(ti_tbo[i1][0]);
        ff1   = round(ti_tbo[i1][1]);
        ff2   = round(ti_tbo[i1][2]);
        //printf("%d  %d %d\n", nterm, ff1, ff2);

        stt = 0.0;
        ip0 = 3;
        for (i2=0; i2<nterm; i2++)  {
            nvar = round(ti_tbo[i1][ip0]);
            //printf("%d  %14.6e", nvar, ti_tbo[i1][ip0+1]);

            tt = 1.0;
            for (i3=0; i3<nvar; i3++)  {
                ip1 = ip0 + 2 + 3*i3;
                iv = round(ti_tbo[i1][ip1]);
                id = round(ti_tbo[i1][ip1+1]);
                pp = round(ti_tbo[i1][ip1+2]);
                //printf("  %2d %2d  %2d", iv, id, pp);
                if (iv == 0)    
                    v1 = svar[id];
                else if (iv == 1)  
                    v1 = avar[id][0];
                else  {
                    printf("Wrong term iv\n");
                    exit(1);
                }
                tt = tt * pow(v1, pp);
            }
            //printf("\n");
            stt = stt + ti_tbo[i1][ip0+1]*tt;
            ip0 = ip0 + 2 + 3*nvar;
        }

        if (ff1 == 0)
            tbo2[i1] = stt;
        else if (ff1 == 1)
            tbo2[i1] = tbo[i1] - stt;
        else  {
            printf("Wrong tbo flag1\n");
            exit(1);
        }

        // deal with <0
        if (ff2 == 0)  {
            ;
        }
        else if (ff2 == 1)  {
            if (tbo2[i1] < 0)     tbo2[i1] = -tbo2[i1];
        }
        else  {
            printf("Wrong tbo flag2\n");
            exit(1);
        }
        

    }
    
    return(0);
}



int init_iwp(float *svar, float **avar, float *hd, float **ti, 
        float *p_val)
{
    int     nseg, nout, iseg;
    int     iout, fdif, fneg;
    int     ic, ncond, fyes;

    int     iv, id, ilow, ihigh, ip, ip1;
    int     nterm, nv, fab, isexp;
    float   tt, stt, v1, var, x0, x1, pp, fmin;

    int     i1, i2, i3;


    //printf("p_val: %f\n", *p_val);

    nseg  = round(hd[0]);
    nout  = round(hd[1]);
    //printf("nseg, nout: %d, %d\n", nseg, nout);

    float   y;

    // main
    iseg = 0;
    for (i3=0; i3<nseg; i3++)  {

        // diff, negtive and min flag
        iout = round(ti[iseg+i3][0]);
        fdif = round(ti[iseg+i3][1]);
        fneg = round(ti[iseg+i3][2]);
        fmin = ti[iseg+i3][3];
        //printf("iout, fdif, fneg, fmin: %d, %d, %d, %f\n", 
        //        iout, fdif, fneg, fmin);

        // loop condition
        fyes = 0;
        ip = 4;
        ncond = round(ti[iseg+i3][ip]);
        ip = ip + 1;
        for (ic=0; ic<ncond; ic++)  {

            iv = round(ti[iseg+i3][ip+0]);
            id = round(ti[iseg+i3][ip+1]);
            if (iv == 0)
                var = svar[id];
            else if (iv ==1)
                var = avar[id][0];
            else  {
                printf("Wrong iv\n");
                exit(1);
            }
            //printf("var: %f\n", var);

            fab = round(ti[iseg+i3][ip+6]);
            if (fab == 0)
                ;
            else if (fab == 1)
                var = fabsf(var);
            else  {
                printf("Wrong fabs\n");
                exit(1);
            }
            //printf("fab, var: %d, %f\n", fab, var);

            x0 = ti[iseg+i3][ip+2];
            x1 = ti[iseg+i3][ip+3];
            ilow  = round(ti[iseg+i3][ip+4]);
            ihigh = round(ti[iseg+i3][ip+5]);

            //printf("iv, id: %d, %d\n", iv, id);
            //printf("x0, x1: %f, %f\n", x0, x1);
            //printf("ilow, ihigh: %d, %d\n", ilow, ihigh);
            //printf("var: %f\n", var);

            if (ilow==0 && ihigh==0)  {
                if (var>x0 && var<x1)       fyes++;
            }
            else if (ilow==1 && ihigh==0)  {
                if (var>=x0 && var<x1)      fyes++;
            }
            else if (ilow==0 && ihigh==1)  {
                if (var>x0 && var<=x1)      fyes++;
            }
            else if (ilow==1 && ihigh==1)  {
                if (var>=x0 && var<=x1)     fyes++;
            }
            else  {
                printf("Wrong bc\n");
                exit(1);
            }
            ip = ip + 7;
        }
        //printf("ncond, fyes: %d, %d\n", ncond, fyes);

        // check if satisfy
        if (fyes != ncond)  {
            // not satisfy all conds
            //printf("not satisfy, fyes/ncond: %d/%d\n", fyes, ncond);
            //getchar();
            continue;
        }
        
        nterm = round(ti[iseg+i3][ip]);
        //printf("nterm: %d\n", nterm);
        ip = ip + 1;

        // all terms loop
        stt = 0.0;
        for (i2=0; i2<nterm; i2++)  {
            nv = round(ti[iseg+i3][ip]);
            //printf("nv: %d\n", nv);

            tt = 1.0;
            // exp function
            isexp = 0;
            if (nv == 101)  {
                nv = 1;
                isexp = 1;
            }

            for (i1=0; i1<nv; i1++)  {
                // each variable
                ip1 = ip + 2 + i1*3;
                iv = round(ti[iseg+i3][ip1]);
                id = round(ti[iseg+i3][ip1+1]);
                //pp = round(ti[iseg+i3][ip1+2]);
                pp = ti[iseg+i3][ip1+2];
                //printf("iv, id, pp: %2d %2d %f\n", iv, id, pp);
                if (iv == 0)
                    v1 = svar[id];
                else if (iv == 1)
                    v1 = avar[id][0];
                else  {
                    printf("Wrong term iv\n");
                    exit(1);
                }
                //printf("v1: %16.8f\n", v1);

                if (isexp != 1)
                    tt = tt * pow(v1, pp);
                else  {
                    //printf("exp, coe: %f, %f\n", 
                    //        ti[iseg+i3][ip+1], 
                    //        ti[iseg+i3][ip+4]);
                    tt = exp(ti[iseg+i3][ip+4]*v1);
                }
                //printf("tt: %f\n", tt);
            }
            //printf("coe: %e\n", ti[iseg+i3][ip+1]);
            stt = stt + ti[iseg+i3][ip+1]*tt;
            //printf("stt: %f\n", stt);
            ip = ip + 2 + nv*3;
        }  // end nterms

        // regress variable or delta
        if (fdif == 0)
            y = stt;
        else if (fdif == 1)
            y = *p_val - stt;
        else if (fdif == 2)
            y = pow(10, stt);
        else  {
            printf("Wrong iwp dif flag\n");
            exit(1);
        }

        // deal with <0
        if (fneg == -1)  {
            if (y < 0)      y = -y;
        }
        else if (fneg == 0)  {
            ;
        }
        else if (fneg == 1)  {
            if (y < 0)      y = fmin;
        }
        else if (fneg == 2)  {
            if (y < fmin)      y = fmin;
        }
        else  {
            printf("Wrong iwp flag2\n");
            exit(1);
        }

        // set value
        if (iout > 0)   svar[iout] = y;
        *p_val = y;

        break;

    } // end iseg

    return(0);
}



int init_de(float *svar, float **avar, float *hd, float **ti, 
        float *p_val)
{
    int     nseg, nout, iseg;
    int     iout, fdif, fneg;
    int     ic, ncond, fyes;

    int     iv, id, ilow, ihigh, ip, ip1;
    int     nterm, nv, fab, isexp;
    float   tt, stt, v1, var, x0, x1, pp, fmin;

    int     i1, i2, i3;


    //printf("p_val: %f\n", *p_val);

    nseg  = round(hd[0]);
    nout  = round(hd[1]);
    //printf("nseg, nout: %d, %d\n", nseg, nout);

    float   y;

    // main
    iseg = 0;
    for (i3=0; i3<nseg; i3++)  {

        // diff, negtive and min flag
        iout = round(ti[iseg+i3][0]);
        fdif = round(ti[iseg+i3][1]);
        fneg = round(ti[iseg+i3][2]);
        fmin = ti[iseg+i3][3];
        //printf("iout, fdif, fneg, fmin: %d, %d, %d, %f\n", 
        //        iout, fdif, fneg, fmin);

        // loop condition
        fyes = 0;
        ip = 4;
        ncond = round(ti[iseg+i3][ip]);
        ip = ip + 1;
        for (ic=0; ic<ncond; ic++)  {

            iv = round(ti[iseg+i3][ip+0]);
            id = round(ti[iseg+i3][ip+1]);
            if (iv == 0)
                var = svar[id];
            else if (iv ==1)
                var = avar[id][0];
            else  {
                printf("Wrong iv\n");
                exit(1);
            }
            //printf("var: %f\n", var);

            fab = round(ti[iseg+i3][ip+6]);
            if (fab == 0)
                ;
            else if (fab == 1)
                var = fabsf(var);
            else  {
                printf("Wrong fabs\n");
                exit(1);
            }
            //printf("fab, var: %d, %f\n", fab, var);

            x0 = ti[iseg+i3][ip+2];
            x1 = ti[iseg+i3][ip+3];
            ilow  = round(ti[iseg+i3][ip+4]);
            ihigh = round(ti[iseg+i3][ip+5]);

            //printf("iv, id: %d, %d\n", iv, id);
            //printf("x0, x1: %f, %f\n", x0, x1);
            //printf("ilow, ihigh: %d, %d\n", ilow, ihigh);
            //printf("var: %f\n", var);

            if (ilow==0 && ihigh==0)  {
                if (var>x0 && var<x1)       fyes++;
            }
            else if (ilow==1 && ihigh==0)  {
                if (var>=x0 && var<x1)      fyes++;
            }
            else if (ilow==0 && ihigh==1)  {
                if (var>x0 && var<=x1)      fyes++;
            }
            else if (ilow==1 && ihigh==1)  {
                if (var>=x0 && var<=x1)     fyes++;
            }
            else  {
                printf("Wrong bc\n");
                exit(1);
            }
            ip = ip + 7;
        }
        //printf("ncond, fyes: %d, %d\n", ncond, fyes);

        // check if satisfy
        if (fyes != ncond)  {
            // not satisfy all conds
            //printf("not satisfy, fyes/ncond: %d/%d\n", fyes, ncond);
            //getchar();
            continue;
        }
        
        nterm = round(ti[iseg+i3][ip]);
        //printf("nterm: %d\n", nterm);
        ip = ip + 1;

        // all terms loop
        stt = 0.0;
        for (i2=0; i2<nterm; i2++)  {
            nv = round(ti[iseg+i3][ip]);
            //printf("nv: %d\n", nv);

            tt = 1.0;
            // exp function
            isexp = 0;
            if (nv == 101)  {
                nv = 1;
                isexp = 1;
            }

            for (i1=0; i1<nv; i1++)  {
                // each variable
                ip1 = ip + 2 + i1*3;
                iv = round(ti[iseg+i3][ip1]);
                id = round(ti[iseg+i3][ip1+1]);
                //pp = round(ti[iseg+i3][ip1+2]);
                pp = ti[iseg+i3][ip1+2];
                //printf("iv, id, pp: %2d %2d %f\n", iv, id, pp);
                if (iv == 0)
                    v1 = svar[id];
                else if (iv == 1)
                    v1 = avar[id][0];
                else  {
                    printf("Wrong term iv\n");
                    exit(1);
                }
                //printf("v1: %16.8f\n", v1);

                if (isexp != 1)
                    tt = tt * pow(v1, pp);
                else  {
                    //printf("exp, coe: %f, %f\n", 
                    //        ti[iseg+i3][ip+1], 
                    //        ti[iseg+i3][ip+4]);
                    tt = exp(ti[iseg+i3][ip+4]*v1);
                }
                //printf("tt: %f\n", tt);
            }
            //printf("coe: %e\n", ti[iseg+i3][ip+1]);
            stt = stt + ti[iseg+i3][ip+1]*tt;
            //printf("stt: %f\n", stt);
            ip = ip + 2 + nv*3;
        }  // end nterms

        // regress variable or delta
        if (fdif == 0)
            y = stt;
        else if (fdif == 1)
            y = *p_val - stt;
        else  {
            printf("Wrong de dif flag\n");
            exit(1);
        }

        // deal with <0
        if (fneg == -1)  {
            if (y < 0)      y = -y;
        }
        else if (fneg == 0)  {
            ;
        }
        else if (fneg == 1)  {
            if (y < 0)      y = fmin;
        }
        else  {
            printf("Wrong de flag2\n");
            exit(1);
        }

        // set value
        if (iout >= 0)      svar[iout] = y;
        *p_val = y;

        break;

    } // end iseg

    return(0);
}



int init_tc(float *svar, float **avar, float *hd, float **ti, float *p_val)
{
    int     nseg, nout, iseg;
    int     iout, fdif, fneg;
    int     ic, ncond, fyes;

    int     iv, id, ilow, ihigh, ip, ip1;
    int     nterm, nv, fab, isexp;
    float   tt, stt, v1, var, x0, x1, pp, fmin;

    int     i1, i2, i3;


    //printf("p_val: %f\n", *p_val);

    nseg  = round(hd[0]);
    nout  = round(hd[1]);
    //printf("nseg, nout: %d, %d\n", nseg, nout);

    float   y;

    // main
    iseg = 0;
    for (i3=0; i3<nseg; i3++)  {

        // diff, negtive and min flag
        iout = round(ti[iseg+i3][0]);
        fdif = round(ti[iseg+i3][1]);
        fneg = round(ti[iseg+i3][2]);
        fmin = ti[iseg+i3][3];
        //printf("iout, fdif, fneg, fmin: %d, %d, %d, %f\n", 
        //        iout, fdif, fneg, fmin);

        // loop condition
        fyes = 0;
        ip = 4;
        ncond = round(ti[iseg+i3][ip]);
        ip = ip + 1;
        for (ic=0; ic<ncond; ic++)  {

            iv = round(ti[iseg+i3][ip+0]);
            id = round(ti[iseg+i3][ip+1]);
            if (iv == 0)
                var = svar[id];
            else if (iv ==1)
                var = avar[id][0];
            else  {
                printf("Wrong iv\n");
                exit(1);
            }
            //printf("var: %f\n", var);

            fab = round(ti[iseg+i3][ip+6]);
            if (fab == 0)
                ;
            else if (fab == 1)
                var = fabsf(var);
            else  {
                printf("Wrong fabs\n");
                exit(1);
            }
            //printf("fab, var: %d, %f\n", fab, var);

            x0 = ti[iseg+i3][ip+2];
            x1 = ti[iseg+i3][ip+3];
            ilow  = round(ti[iseg+i3][ip+4]);
            ihigh = round(ti[iseg+i3][ip+5]);

            //printf("iv, id: %d, %d\n", iv, id);
            //printf("x0, x1: %f, %f\n", x0, x1);
            //printf("ilow, ihigh: %d, %d\n", ilow, ihigh);
            //printf("var: %f\n", var);

            if (ilow==0 && ihigh==0)  {
                if (var>x0 && var<x1)       fyes++;
            }
            else if (ilow==1 && ihigh==0)  {
                if (var>=x0 && var<x1)      fyes++;
            }
            else if (ilow==0 && ihigh==1)  {
                if (var>x0 && var<=x1)      fyes++;
            }
            else if (ilow==1 && ihigh==1)  {
                if (var>=x0 && var<=x1)     fyes++;
            }
            else  {
                printf("Wrong bc\n");
                exit(1);
            }
            ip = ip + 7;
        }
        //printf("ncond, fyes: %d, %d\n", ncond, fyes);

        // check if satisfy
        if (fyes != ncond)  {
            // not satisfy all conds
            //printf("not satisfy, fyes/ncond: %d/%d\n", fyes, ncond);
            //getchar();
            continue;
        }
        
        nterm = round(ti[iseg+i3][ip]);
        //printf("nterm: %d\n", nterm);
        ip = ip + 1;

        // all terms loop
        stt = 0.0;
        for (i2=0; i2<nterm; i2++)  {
            nv = round(ti[iseg+i3][ip]);
            //printf("nv: %d\n", nv);

            tt = 1.0;
            // exp function
            isexp = 0;
            if (nv == 101)  {
                nv = 1;
                isexp = 1;
            }

            for (i1=0; i1<nv; i1++)  {
                // each variable
                ip1 = ip + 2 + i1*3;
                iv = round(ti[iseg+i3][ip1]);
                id = round(ti[iseg+i3][ip1+1]);
                //pp = round(ti[iseg+i3][ip1+2]);
                pp = ti[iseg+i3][ip1+2];
                //printf("iv, id, pp: %2d %2d %f\n", iv, id, pp);
                if (iv == 0)
                    v1 = svar[id];
                else if (iv == 1)
                    v1 = avar[id][0];
                else  {
                    printf("Wrong term iv\n");
                    exit(1);
                }
                //printf("v1: %16.8f\n", v1);

                if (isexp != 1)
                    tt = tt * pow(v1, pp);
                else  {
                    //printf("exp, coe: %f, %f\n", 
                    //        ti[iseg+i3][ip+1], 
                    //        ti[iseg+i3][ip+4]);
                    tt = exp(ti[iseg+i3][ip+4]*v1);
                }
                //printf("tt: %f\n", tt);
            }
            //printf("coe: %e\n", ti[iseg+i3][ip+1]);
            stt = stt + ti[iseg+i3][ip+1]*tt;
            //printf("stt: %f\n", stt);
            ip = ip + 2 + nv*3;
        }  // end nterms

        // regress variable or delta
        if (fdif == 0)
            y = stt;
        else if (fdif == 1)
            y = *p_val - stt;
        else  {
            printf("Wrong tc dif flag\n");
            exit(1);
        }

        // deal with <0
        if (fneg == -1)  {
            if (y < 0)      y = -y;
        }
        else if (fneg == 0)  {
            ;
        }
        else if (fneg == 1)  {
            if (y < 0)      y = fmin;
        }
        else  {
            printf("Wrong tc flag2\n");
            exit(1);
        }

        // set value
        if (iout >= 0)      svar[iout] = y;
        *p_val = y;

        break;

    } // end iseg

    return(0);
}



int init_em(float *svar, float **avar, float *hd, float **ti, float *p_val)
{
    int     nseg, nout, iseg;
    int     iout, fdif, fneg;
    int     ic, ncond, fyes;

    int     iv, id, ilow, ihigh, ip, ip1;
    int     nterm, nv, fab, isexp;
    float   tt, stt, v1, var, x0, x1, pp, fmin, y;

    int     i1, i2, i3;


    //printf("p_val: %f\n", *p_val);

    nseg  = round(hd[0]);
    nout  = round(hd[1]);
    //printf("nseg, nout: %d, %d\n", nseg, nout);


    // main
    iseg = 0;
    for (i3=0; i3<nseg; i3++)  {

        // diff, negtive and min flag
        iout = round(ti[iseg+i3][0]);
        fdif = round(ti[iseg+i3][1]);
        fneg = round(ti[iseg+i3][2]);
        fmin = ti[iseg+i3][3];
        //printf("iout, fdif, fneg, fmin: %d, %d, %d, %f\n", 
        //        iout, fdif, fneg, fmin);

        // loop condition
        fyes = 0;
        ip = 4;
        ncond = round(ti[iseg+i3][ip]);
        ip = ip + 1;
        for (ic=0; ic<ncond; ic++)  {

            iv = round(ti[iseg+i3][ip+0]);
            id = round(ti[iseg+i3][ip+1]);
            if (iv == 0)
                var = svar[id];
            else if (iv ==1)
                var = avar[id][0];
            else  {
                printf("Wrong iv\n");
                exit(1);
            }
            //printf("var: %f\n", var);

            fab = round(ti[iseg+i3][ip+6]);
            if (fab == 0)
                ;
            else if (fab == 1)
                var = fabsf(var);
            else  {
                printf("Wrong fabs\n");
                exit(1);
            }
            //printf("fab, var: %d, %f\n", fab, var);

            x0 = ti[iseg+i3][ip+2];
            x1 = ti[iseg+i3][ip+3];
            ilow  = round(ti[iseg+i3][ip+4]);
            ihigh = round(ti[iseg+i3][ip+5]);

            //printf("iv, id: %d, %d\n", iv, id);
            //printf("x0, x1: %f, %f\n", x0, x1);
            //printf("ilow, ihigh: %d, %d\n", ilow, ihigh);
            //printf("var: %f\n", var);

            if (ilow==0 && ihigh==0)  {
                if (var>x0 && var<x1)       fyes++;
            }
            else if (ilow==1 && ihigh==0)  {
                if (var>=x0 && var<x1)      fyes++;
            }
            else if (ilow==0 && ihigh==1)  {
                if (var>x0 && var<=x1)      fyes++;
            }
            else if (ilow==1 && ihigh==1)  {
                if (var>=x0 && var<=x1)     fyes++;
            }
            else  {
                printf("Wrong bc\n");
                exit(1);
            }
            ip = ip + 7;
        }
        //printf("ncond, fyes: %d, %d\n", ncond, fyes);

        // check if satisfy
        if (fyes != ncond)  {
            // not satisfy all conds, not right
            //printf("not satisfy, fyes/ncond: %d/%d\n", fyes, ncond);
            //exit(1);
            //getchar();
            continue;
        }
        
        nterm = round(ti[iseg+i3][ip]);
        //printf("nterm: %d\n", nterm);
        ip = ip + 1;

        // all terms loop
        stt = 0.0;
        for (i2=0; i2<nterm; i2++)  {
            nv = round(ti[iseg+i3][ip]);
            //printf("nv: %d\n", nv);

            tt = 1.0;
            // exp function
            isexp = 0;
            if (nv == 101)  {
                nv = 1;
                isexp = 1;
            }

            for (i1=0; i1<nv; i1++)  {
                // each variable
                ip1 = ip + 2 + i1*3;
                iv = round(ti[iseg+i3][ip1]);
                id = round(ti[iseg+i3][ip1+1]);
                //pp = round(ti[iseg+i3][ip1+2]);
                pp = ti[iseg+i3][ip1+2];
                //printf("iv, id, pp: %2d %2d %f\n", iv, id, pp);
                if (iv == 0)
                    v1 = svar[id];
                else if (iv == 1)
                    v1 = avar[id][0];
                else  {
                    printf("Wrong term iv\n");
                    exit(1);
                }
                //printf("v1: %16.8f\n", v1);

                if (isexp != 1)
                    tt = tt * pow(v1, pp);
                else  {
                    //printf("exp, coe: %f, %f\n", 
                    //        ti[iseg+i3][ip+1], 
                    //        ti[iseg+i3][ip+4]);
                    tt = exp(ti[iseg+i3][ip+4]*v1);
                }
                //printf("tt: %f\n", tt);
            }
            //printf("coe: %e\n", ti[iseg+i3][ip+1]);
            stt = stt + ti[iseg+i3][ip+1]*tt;
            //printf("stt: %f\n", stt);
            ip = ip + 2 + nv*3;
        }  // end nterms

        // regress variable or delta
        if (fdif == 0)
            y = stt;
        else if (fdif == 1)
            y = *p_val - stt;
        else  {
            printf("Wrong em dif flag\n");
            exit(1);
        }

        // deal with <0
        if (fneg == -1)  {
            if (y < 0)      y = -y;
        }
        else if (fneg == 0)  {
            ;
        }
        else if (fneg == 1)  {
            if (y < 0)      y = fmin;
        }
        else  {
            printf("Wrong em flag2\n");
            exit(1);
        }

        // set value
        p_val[i3] = y;
        if (iout >= 0)       svar[iout] = y;

        //break;

    } // end iseg

    return(0);
}



int sfr_cal_max(float *svar, float **avar, float *hd_cal, float **ti_cal, 
        float *p_sfr)
{
    int     i1, i2, i3, iv, id, ff1, ff2, ff3, nseg, ilow, ihigh, ip, ip1;
    int     nterm, pp, nv;
    float   x0, x1, tt, stt, v1, var;

    int     ncal, nmax, isexp, fyes, ncond, ic;

    int     iseg, iout0, ind=-1;

    //printf("p_sfr0: %f\n", *p_sfr);

    iout0 = round(hd_cal[0]);
    ncal  = round(hd_cal[1]);
    nseg  = round(hd_cal[2]);
    nmax  = round(hd_cal[3]);
    //printf("iout0, ncal, nseg, nmax: %d, %d, %d, %d\n", 
    //        iout0, ncal, nseg, nmax);

    // if 0 seg, not run max; just copy p_sfr to svar
    if (nmax < 1)  {
        svar[iout0] = *p_sfr;
        return(0);
    }

    float   y[nseg];

    // condition var
    iv = round(hd_cal[4]);
    id = round(hd_cal[5]);
    if (iv == 0)
        var = svar[id];
    else if (iv ==1)
        var = avar[id][0];
    else  {
        printf("Wrong iv\n");
        exit(1);
    }
    //printf("iv, id, var: %d, %d, %f\n", iv, id, var);


    // main
    iseg = 0;
    for (i3=0; i3<nseg; i3++)  {
        //printf("reg %d ------------------------\n", i3);

        // diff flag and negtive flag
        ff1 = round(ti_cal[iseg+i3][1]);
        ff2 = round(ti_cal[iseg+i3][2]);
        ff3 = ti_cal[iseg+i3][3];


        // loop condition
        fyes = 0;
        ip = 4;
        ncond = round(ti_cal[iseg+i3][ip]);
        ip = ip + 1;
        for (ic=0; ic<ncond; ic++)  {

            iv = round(ti_cal[iseg+i3][ip+0]);
            id = round(ti_cal[iseg+i3][ip+1]);
            if (iv == 0)
                var = svar[id];
            else if (iv ==1)
                var = avar[id][0];
            else  {
                printf("Wrong iv\n");
                exit(1);
            }
            //printf("var: %f\n", var);

            //fab = round(ti_cal[iseg+i3][ip+6]);
            //if (fab == 0)
            //    ;
            //else if (fab == 1)
            //    var = fabsf(var);
            //else  {
            //    printf("Wrong fabs\n");
            //    exit(1);
            //}
            //printf("fab, var: %d, %f\n", fab, var);

            x0 = ti_cal[iseg+i3][ip+2];
            x1 = ti_cal[iseg+i3][ip+3];
            ilow  = round(ti_cal[iseg+i3][ip+4]);
            ihigh = round(ti_cal[iseg+i3][ip+5]);

            //printf("iv, id: %d, %d\n", iv, id);
            //printf("x0, x1: %f, %f\n", x0, x1);
            //printf("ilow, ihigh: %d, %d\n", ilow, ihigh);
            //printf("var: %f\n", var);

            if (ilow==0 && ihigh==0)  {
                if (var>x0 && var<x1)       fyes++;
            }
            else if (ilow==1 && ihigh==0)  {
                if (var>=x0 && var<x1)      fyes++;
            }
            else if (ilow==0 && ihigh==1)  {
                if (var>x0 && var<=x1)      fyes++;
            }
            else if (ilow==1 && ihigh==1)  {
                if (var>=x0 && var<=x1)     fyes++;
            }
            else  {
                printf("Wrong bc\n");
                exit(1);
            }
            ip = ip + 6;
        }
        //printf("ncond, fyes: %d, %d\n", ncond, fyes);
        //getchar();

        // check if satisfy
        if (fyes == ncond)  {
            if (ind == -1)
                ind = i3;
            else  {
                // not satisfy all conds, not right
                printf("not satisfy, fyes/ncond: %d/%d\n", fyes, ncond);
                //getchar();
                //continue;
            }
        }
        //printf("icond: %d\n", ind);
        
        
        //ip = 10;
        nterm = round(ti_cal[iseg+i3][ip]);
        //printf("nterm: %d\n", nterm);
        ip = ip + 1;

        // all terms loop
        stt = 0.0;
        for (i2=0; i2<nterm; i2++)  {
            nv = round(ti_cal[iseg+i3][ip]);
            //printf("nv: %d\n", nv);

            tt = 1.0;
            // exp function
            isexp = 0;
            if (nv == 101)  {
                nv = 1;
                isexp = 1;
            }

            for (i1=0; i1<nv; i1++)  {
                // each variable
                ip1 = ip + 2 + i1*3;
                iv = round(ti_cal[iseg+i3][ip1]);
                id = round(ti_cal[iseg+i3][ip1+1]);
                pp = round(ti_cal[iseg+i3][ip1+2]);
                //printf(" %2d %2d %2d\n", iv, id, pp);
                if (iv == 0)
                    v1 = svar[id];
                else if (iv == 1)
                    v1 = avar[id][0];
                else  {
                    printf("Wrong term iv\n");
                    exit(1);
                }
                //printf("v1: %16.8f\n", v1);

                if (isexp != 1)
                    tt = tt * pow(v1, pp);
                else  {
                    //printf("exp, coe: %f, %f\n", 
                    //        ti_cal[iseg+i3][ip+1], 
                    //        ti_cal[iseg+i3][ip+4]);
                    tt = exp(ti_cal[iseg+i3][ip+4]*v1);
                }
                //printf("tt: %f\n", tt);
            }
            //printf("coe: %e\n", ti_cal[i3][ip+1]);
            stt = stt + ti_cal[iseg+i3][ip+1]*tt;
            //printf("stt: %f\n", stt);
            ip = ip + 2 + nv*3;
        }  // end nterms

        // regress the variable or the delta
        //printf("ff1, ff2: %d, %d\n", ff1, ff2);
        if (ff1 == 0)
            y[i3] = stt;
        else if (ff1 == 1)
            y[i3] = *p_sfr - stt;
        else  {
            printf("Wrong max flag1\n");
            exit(1);
        }
        //printf("ireg, stt, ind: %d, %f, %d\n", i3, y[i3], ind);

        // deal with <0
        if (ff2 == 0)  {
            if (y[i3] < 0)     y[i3] = y[i3];
        }
        else if (ff2 == 1)  {
            if (y[i3]<0 && ff3>-0.001 && ff3 < 0.001)  {
                y[i3] = -y[i3];
            }
            else if (y[i3]<0 && ff3 >= 0.001)  {
                y[i3] = ff3;
            }
            else if (y[i3]<0 && ff3<-0.001)  {
                id = -round(ff3);
                y[i3] = svar[id];
            }
            else  {
                printf("Wrong ff3, never happen\n");
                exit(1);
            }
        }
        else  {
            printf("Wrong max flag2\n");
            exit(1);
        }

    } // end seg

    //for (i1=0; i1<nseg; i1++)
    //    printf("y[%d]=%f  ", i1, y[i1]);
    //printf("\n");


    // pick max set value
    ff1 = 0;
    ip1 = 6 + ind*(4+nseg);
    for (i1=0; i1<nseg; i1++)  {
        ff2 = round(hd_cal[ip1+i1+4]);
        if (ff2 == 1)  {
            if (ff1 == 0)  {
                *p_sfr = y[i1];
                ff1 = 1;
            }
            else  {
                if ( *p_sfr < y[i1])  {
                    *p_sfr = y[i1];
                }
            }
        }
        //printf("pick, ff2, p_sfr: %d, %f\n", ff2, *p_sfr);
    }
    if (ff1 != 1)  {
        printf("Wrong max set value\n");
        exit(1);
    }


    // negtive, set a value from svar/avar
    ff1 = round(hd_cal[ip1+2]);
    if (*p_sfr<0 && ff1==1)  {
        iv = round(hd_cal[ip1]);
        id = round(hd_cal[ip1+1]);
        if (iv == 0)
            var = svar[id];
        else if (iv ==1)
            var = avar[id][0];
        else  {
            printf("Wrong max set non-neg value\n");
            exit(1);
        }
        *p_sfr = var;
        //getchar();
    }
   
    //printf("iout0, p_sfr: %d, %f\n", iout0, *p_sfr);
    svar[iout0] = *p_sfr;

    //printf("=================\n");
    //getchar();

    return(0);
}



int sfr_cal_layer(float *svar, float **avar, float *hd_layer, float **ti_layer, 
        float *p_sfr)
{
    int     i1, i2, i3, iv, id, ff1, ff2, nseg, ilow, ihigh, ip, ip1;
    int     nterm, pp, nv;
    float   x0, x1, tt, stt, v1, var;

    int     nlayer, i4, i, fyes, ic, isexp;

    nlayer = round(hd_layer[0]);
    //printf("nlayer: %d\n", nlayer);

    int     nsegs[nlayer], iseg, iout, iout0, ncond;

    nseg = 0;
    for (i=0; i<nlayer; i++)  {
        nsegs[i] = round(hd_layer[i+1]);
        nseg = nseg + nsegs[i];
        //printf("nsegs[%d]: %d / %d\n", i, nsegs[i], nseg);
    }


    iseg = 0;
    for (i4=0; i4<nlayer; i4++)  {

        // init, if not adj in one layer, use default
        iout = round(ti_layer[iseg][0]);
        if (i4 == 0)  {
            svar[iout] = *p_sfr;
        }
        else  {
            iout0 = round(ti_layer[iseg-1][0]);
            svar[iout] = svar[iout0];
        }

        // find range and calculate
        stt = 0;
        //printf("nsegs[%d]: %d\n", i4, nsegs[i4]);
        for (i3=0; i3<nsegs[i4]; i3++)  {
            //printf("seg: %d/%d\n", i3, nsegs[i4]);

            // seg and main var
            iout  = round(ti_layer[iseg+i3][0]);
            ff1   = round(ti_layer[iseg+i3][1]);
            ff2   = round(ti_layer[iseg+i3][2]);
            ncond = round(ti_layer[iseg+i3][3]);
            //printf("iseg, iout, ff1, ff2, ncond: %d, %d, %d, %d, %d\n", 
            //        iseg, iout, ff1, ff2, ncond);

            // loop condition
            fyes = 0;
            ip = 4;
            for (ic=0; ic<ncond; ic++)  {
                iv = round(ti_layer[iseg+i3][ip+0]);
                id = round(ti_layer[iseg+i3][ip+1]);
                if (iv == 0)
                    var = svar[id];
                else if (iv ==1)
                    var = avar[id][0];
                else  {
                    printf("Wrong iv\n");
                    exit(1);
                }
                x0 = ti_layer[iseg+i3][ip+2];
                x1 = ti_layer[iseg+i3][ip+3];
                ilow  = round(ti_layer[iseg+i3][ip+4]);
                ihigh = round(ti_layer[iseg+i3][ip+5]);

                //printf("iv, id: %d, %d\n", iv, id);
                //printf("x0, x1: %f, %f\n", x0, x1);
                //printf("ilow, ihigh: %d, %d\n", ilow, ihigh);
                //printf("var: %f\n", var);

                if (ilow==0 && ihigh==0)  {
                    if (var>x0 && var<x1)       fyes++;
                }
                else if (ilow==1 && ihigh==0)  {
                    if (var>=x0 && var<x1)      fyes++;
                }
                else if (ilow==0 && ihigh==1)  {
                    if (var>x0 && var<=x1)      fyes++;
                }
                else if (ilow==1 && ihigh==1)  {
                    if (var>=x0 && var<=x1)     fyes++;
                }
                else  {
                    printf("Wrong bc\n");
                    exit(1);
                }
                ip = ip + 6;
            }

            // check if satisfy
            if (fyes != ncond)  {
                // not satisfy all conds
                //printf("not satisfy, fyes/ncond: %d/%d\n", fyes, ncond);
                //getchar();
                continue;
            }
            else  {
                // all satisfied
                nterm = round(ti_layer[iseg+i3][ip]);
                //printf("nterm: %d\n", nterm);
                ip = ip + 1;

                // all terms loop
                stt = 0.0;
                for (i2=0; i2<nterm; i2++)  {
                    nv = round(ti_layer[iseg+i3][ip]);
                    //printf("nv: %d\n", nv);

                    tt = 1.0;
                    // exp function
                    isexp = 0;
                    if (nv == 101)  {
                        nv = 1;
                        isexp = 1;
                    }

                    for (i1=0; i1<nv; i1++)  {
                        // each variable
                        ip1 = ip + 2 + i1*3;
                        iv = round(ti_layer[iseg+i3][ip1]);
                        id = round(ti_layer[iseg+i3][ip1+1]);
                        pp = round(ti_layer[iseg+i3][ip1+2]);
                        //printf(" %2d %2d %2d\n", iv, id, pp);
                        if (iv == 0)
                            v1 = svar[id];
                        else if (iv == 1)
                            v1 = avar[id][0];
                        else  {
                            printf("Wrong term iv\n");
                            exit(1);
                        }
                        //printf("v1: %16.8f\n", v1);

                        if (isexp != 1)
                            tt = tt * pow(v1, pp);
                        else  {
                            //printf("exp, coe: %f, %f\n", 
                            //        ti_layer[iseg+i3][ip+1], 
                            //        ti_layer[iseg+i3][ip+4]);
                            tt = exp(ti_layer[iseg+i3][ip+4]*v1);
                        }
                        //printf("tt: %f\n", tt);
                    }
                    //printf("coe: %e\n", ti_layer[i3][ip+1]);
                    stt = stt + ti_layer[iseg+i3][ip+1]*tt;
                    //printf("stt: %f\n", stt);
                    ip = ip + 2 + nv*3;
                }  // end nterms

            }  // end if satisfied

            // regress the variable or the delta
            if (ff1 == 0)       *p_sfr = stt;
            else if (ff1 == 1)  *p_sfr = *p_sfr - stt;
            else  {
                printf("Wrong layer flag1\n");
                exit(1);
            }

            // deal with <0 and min
            if (ff2 == 0)  {
                if (*p_sfr < 0)     *p_sfr = *p_sfr;
            }
            else if (ff2 == 1)  {
                if (*p_sfr < 0)     *p_sfr = -*p_sfr;
            }
            // min
            else if (ff2 > 1)  {
                if (*p_sfr < svar[ff2])     *p_sfr = svar[ff2];
            }
            else  {
                printf("Wrong layer flag2\n");
                exit(1);
            }

            // write out to svar
            svar[iout] = *p_sfr;

            //printf("satisfied, fyes/ncond: %d/%d\n", fyes, ncond);
            //printf("p_sfr: %f\n", *p_sfr);
            //printf("end layer %d, seg: %d\n", i4, i3);
            //getchar();

            break;

        }  // end seg

        // if not meet any condition
        if (fyes != ncond)  {
            svar[iout] = *p_sfr;
        }

        // next iseg
        iseg = iseg + nsegs[i4];

        //printf("=================\n");
        //getchar();

    }  // end layer


    return(0);
}



// read table of indexing
int rd_tis(char *ffn)
{
    FILE    *fp;
    int     i, j, nnum, irt, ss, nn;
    char    ps[SL11];


    // open file
    fp = fopen(ffn, "r");
    if (fp == NULL)  {
        printf("ERROR: Fail to open file: %s\n", ffn);
        exit(1);
    }
    printf("Successfully open file: %s\n", ffn);


    // ti_flb
    irt = rd_hd_ti(fp, &hd_flb, &ti_flb);
    //printf("ti_flb:\n");
    //for (i=0; i<hd_flb[0]; i++)
    //    printf("%2.0f  %8.6f  %2.0f  %2.0f  %8.6f  %8.6f  %8.6f  %8.6f\n", 
    //            ti_flb[i][0], ti_flb[i][1], ti_flb[i][2], ti_flb[i][3], 
    //            ti_flb[i][4], ti_flb[i][9], ti_flb[i][10], ti_flb[i][14]);
    //printf("\n");


    // sat qc ti
    irt = rd_hd_ti(fp, &hd_sqc, &ti_sqc);


    // ti1
    // head
    irt = get_one_line(fp, SL9, ps, &nnum);
    //printf("%s|%d\n", ps, nnum);
    if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
    nti1 = (float*) malloc((nnum) * sizeof(float));
    if (nti1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    sscanf(ps, "%f", &nti1[0]);
    nn = nti1[0];
    // table
    ti1 = (float**) malloc(nn * sizeof(float*));
    if (ti1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i=0; i<nn; i++)  {
        // read one useful line
        irt = get_one_line(fp, SL9, ps, &nnum);
        //printf("%s|%d\n", ps, nnum);
        if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
        ti1[i] = (float*) malloc(nnum * sizeof(float));
        if (ti1[i] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        ss = -1;
        for (j=0; j<nnum; j++)   {
            ss++;
            irt = sscanf(&ps[ss], "%f", &ti1[i][j]);
            while (ps[ss] != ' ' && j != nnum-1)    ss++;
        }
    }
    //printf("ti1, %d, %d\n", nn, nnum);
    //for (i=0; i<nn; i++)  {
    //    for (j=0; j<nnum; j++)
    //        printf("%f ", ti1[i][j]);
    //    printf("\n");
    //}
    //printf("--------------------------\n");


    // ti_suv
    irt = rd_hd_ti(fp, &hd_suv, &ti_suv);


    // ti3
    // head
    irt = get_one_line(fp, SL9, ps, &nnum);
    //printf("%s|%d\n", ps, nnum);
    if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
    nti3 = (float*) malloc((nnum) * sizeof(float));
    if (nti3 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    sscanf(ps, "%f", &nti3[0]);
    nn = nti3[0];
    // table
    ti3 = (float**) malloc(nn * sizeof(float*));
    if (ti3 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i=0; i<nn; i++)  {
        // read one useful line
        irt = get_one_line(fp, SL9, ps, &nnum);
        //printf("%s|%d %d %d\n", ps, nnum, strlen(ps), ps[0]);
        if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
        ti3[i] = (float*) malloc(nnum * sizeof(float));
        if (ti3[i] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        ss = -1;
        for (j=0; j<nnum; j++)   {
            ss++;
            irt = sscanf(&ps[ss], "%f", &ti3[i][j]);
            while ((ps[ss] != 0x20) && j != nnum-1)    ss++;
        }
    }
    //printf("ti3, %d, %d\n", nn, nnum);
    //for (i=0; i<nn; i++)  {
    //    for (j=0; j<nnum; j++)
    //        printf("%f ", ti3[i][j]);
    //    printf("\n");
    //}
    //printf("--------------------------\n");
    //getchar();


    // ti4
    // head
    irt = get_one_line(fp, SL9, ps, &nnum);
    //printf("%s|%d\n", ps, nnum);
    if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
    nti4 = (float*) malloc((nnum) * sizeof(float));
    if (nti4 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    sscanf(ps, "%f", &nti4[0]);
    nn = nti4[0];
    // table
    ti4 = (float**) malloc(nn * sizeof(float*));
    if (ti4 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i=0; i<nn; i++)  {
        // read one useful line
        irt = get_one_line(fp, SL9, ps, &nnum);
        //printf("%s|%d %d %d\n", ps, nnum, strlen(ps), ps[0]);
        if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
        ti4[i] = (float*) malloc(nnum * sizeof(float));
        if (ti4[i] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        ss = -1;
        for (j=0; j<nnum; j++)   {
            ss++;
            irt = sscanf(&ps[ss], "%f", &ti4[i][j]);
            while ((ps[ss] != 0x20) && j != nnum-1)    ss++;
        }
    }
    //printf("ti4, %d, %d\n", nn, nnum);
    //for (i=0; i<nn; i++)  {
    //    for (j=0; j<nnum; j++)
    //        printf("%f ", ti4[i][j]);
    //    printf("\n");
    //}
    //printf("--------------------------\n");


    // ti5
    // head
    irt = get_one_line(fp, SL9, ps, &nnum);
    //printf("%s|%d\n", ps, nnum);
    if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
    nti5 = (float*) malloc((nnum) * sizeof(float));
    if (nti5 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    sscanf(ps, "%f", &nti5[0]);
    nn = nti5[0];
    // table
    ti5 = (float**) malloc(nn * sizeof(float*));
    if (ti5 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i=0; i<nn; i++)  {
        // read one useful line
        irt = get_one_line(fp, SL9, ps, &nnum);
        //printf("%s|%d %d %d\n", ps, nnum, strlen(ps), ps[0]);
        if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
        ti5[i] = (float*) malloc(nnum * sizeof(float));
        if (ti5[i] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        ss = -1;
        for (j=0; j<nnum; j++)   {
            ss++;
            irt = sscanf(&ps[ss], "%f", &ti5[i][j]);
            while ((ps[ss] != ' ') && j != nnum-1)    ss++;
        }
    }
    //printf("ti5, %d, %d\n", nn, nnum);
    //for (i=0; i<nn; i++)  {
    //    for (j=0; j<nnum; j++)
    //        printf("%f ", ti5[i][j]);
    //    printf("\n");
    //}
    //printf("--------------------------\n");


    // ti6
    // head
    irt = get_one_line(fp, SL9, ps, &nnum);
    //printf("%s|%d\n", ps, nnum);
    if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
    nti6 = (float*) malloc((nnum) * sizeof(float));
    if (nti6 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    //sscanf(ps, "%f", &nti6[0]);
    //nn = nti6[0];
    ss = -1;
    for (j=0; j<nnum; j++)  {
        ss++;
        sscanf(&ps[ss], "%f", &nti6[j]);
        while ((ps[ss] != 0x20) && j != nnum-1)    ss++;
        //printf("%f ", nti6[j]);
    }
    //printf("\n");
    nn = nti6[0];
    // table
    ti6 = (float**) malloc(nn * sizeof(float*));
    if (ti6 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i=0; i<nn; i++)  {
        // read one useful line
        irt = get_one_line(fp, SL9, ps, &nnum);
        //printf("%s|%d %d %d\n", ps, nnum, strlen(ps), ps[0]);
        if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
        ti6[i] = (float*) malloc(nnum * sizeof(float));
        if (ti6[i] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        ss = -1;
        for (j=0; j<nnum; j++)   {
            ss++;
            irt = sscanf(&ps[ss], "%f", &ti6[i][j]);
            while ((ps[ss] != ' ') && j != nnum-1)    ss++;
        }
    }
    //printf("ti6, %d, %d\n", nn, nnum);
    //for (i=0; i<nn; i++)  {
    //    for (j=0; j<nnum; j++)
    //        printf("%f ", ti6[i][j]);
    //    printf("\n");
    //}
    //printf("--------------------------\n");


    // ti7
    // head
    irt = get_one_line(fp, SL9, ps, &nnum);
    //printf("%s|%d\n", ps, nnum);
    if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
    nti7 = (float*) malloc((nnum) * sizeof(float));
    if (nti7 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    sscanf(ps, "%f", &nti7[0]);
    nn = nti7[0];
    // table
    ti7 = (float**) malloc(nn * sizeof(float*));
    if (ti7 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i=0; i<nn; i++)  {
        // read one useful line
        irt = get_one_line(fp, SL9, ps, &nnum);
        //printf("%s|%d %d %d\n", ps, nnum, strlen(ps), ps[0]);
        if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
        ti7[i] = (float*) malloc(nnum * sizeof(float));
        if (ti7[i] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        ss = -1;
        for (j=0; j<nnum; j++)   {
            ss++;
            irt = sscanf(&ps[ss], "%f", &ti7[i][j]);
            while ((ps[ss] != ' ') && j != nnum-1)    ss++;
        }
    }
    //printf("ti7, %d, %d\n", nn, nnum);
    //for (i=0; i<nn; i++)  {
    //    for (j=0; j<nnum; j++)
    //        printf("%f ", ti7[i][j]);
    //    printf("\n");
    //}
    //printf("--------------------------\n");


    // ti_one
    // head
    irt = get_one_line(fp, SL9, ps, &nnum);
    //printf("%s|%d\n", ps, nnum);
    if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
    hd_one = (float*) malloc((nnum) * sizeof(float));
    if (hd_one == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    ss = -1;
    for (j=0; j<nnum; j++)  {
        ss++;
        sscanf(&ps[ss], "%f", &hd_one[j]);
        while (ps[ss] != ' ' && j != nnum-1)    ss++;
        //printf("%f ", hd_one[j]);
    }
    //printf("\n");
    // table
    nn = hd_one[0];
    ti_one = (float**) malloc(nn * sizeof(float*));
    if (ti_one == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i=0; i<nn; i++)  {
        // read one useful line
        irt = get_one_line(fp, SL9, ps, &nnum);
        //printf("%s|%d %d\n", ps, nnum, ps[0]);
        if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
        ti_one[i] = (float*) malloc(nnum * sizeof(float));
        if (ti_one[i] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        ss = -1;
        for (j=0; j<nnum; j++)   {
            ss++;
            irt = sscanf(&ps[ss], "%f", &ti_one[i][j]);
            while (ps[ss] != ' ' && j != nnum-1)    ss++;
        }
    }
    //printf("ti, %d, %d\n", nn, nnum);
    //for (i=0; i<nn; i++)  {
    //    for (j=0; j<nnum; j++)
    //        printf("%f ", ti_one[i][j]);
    //    printf("\n");
    //}
    //printf("--------------------------\n");
    //getchar();


    // ti_tbo
    irt = rd_hd_ti(fp, &hd_tbo, &ti_tbo);

    // ti_iwp
    irt = rd_hd_ti(fp, &hd_iwp, &ti_iwp);

    // ti_de
    irt = rd_hd_ti(fp, &hd_de, &ti_de);

    // ti_tc
    irt = rd_hd_ti(fp, &hd_tc, &ti_tc);

    // ti_em
    //irt = rd_hd_ti_2(fp, &hd_em, &ti_em);
    irt = rd_hd_ti(fp, &hd_em, &ti_em);

    // flag of rtm channel, to fortran
    irt = rd_hd_ti(fp, &hd_frc, &ti_frc);

    // freq
    irt = rd_hd_ti(fp, &hd_freq, &ti_freq);

    // error covariance matrix
    irt = rd_hd_ti(fp, &hd_ecm, &ti_ecm);

    // kv
    irt = rd_hd_ti(fp, &hd_kv, &ti_kv);

    // ko2
    irt = rd_hd_ti(fp, &hd_ko2, &ti_ko2);
    //printf("hd_ko2: %f  %f\n", hd_ko2[0], hd_ko2[1]);
    //printf("ti_ko2:\n");
    //for (i=0; i<round(hd_ko2[0]); i++)  {
    //    for (j=0; j<round(hd_ko2[1]); j++)
    //        printf("%14.6e", ti_ko2[i][j]);
    //    printf("\n");
    //}
    //printf("--------------------------\n");
    //getchar();


    // ti_cal
    // head
    irt = get_one_line(fp, SL11, ps, &nnum);
    //printf("%s|%d\n", ps, nnum);
    if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
    hd_cal = (float*) malloc((nnum) * sizeof(float));
    if (hd_cal == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    ss = -1;
    for (j=0; j<nnum; j++)  {
        ss++;
        sscanf(&ps[ss], "%f", &hd_cal[j]);
        while (ps[ss] != ' ' && j != nnum-1)    ss++;
        //printf("%f ", hd_cal[j]);
    }
    //printf("\n");
    // table
    nn = hd_cal[2];
    //printf("nn: %d\n", nn);
    ti_cal = (float**) malloc(nn * sizeof(float*));
    if (ti_cal == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i=0; i<nn; i++)  {
        // read one useful line
        irt = get_one_line(fp, SL9, ps, &nnum);
        //printf("%s|%d %d\n", ps, nnum, ps[0]);
        if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
        ti_cal[i] = (float*) malloc(nnum * sizeof(float));
        if (ti_cal[i] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        ss = -1;
        for (j=0; j<nnum; j++)   {
            ss++;
            irt = sscanf(&ps[ss], "%f", &ti_cal[i][j]);
            while (ps[ss] != ' ' && j != nnum-1)    ss++;
        }
    }
    //printf("ti, %d, %d\n", nn, nnum);
    //for (i=0; i<nn; i++)  {
    //    for (j=0; j<nnum; j++)
    //        printf("%f ", ti_cal[i][j]);
    //    printf("\n");
    //}
    //printf("--------------------------\n");
    //getchar();


    // ti_layer
    // head
    irt = get_one_line(fp, SL11, ps, &nnum);
    //printf("%s|%d\n", ps, nnum);
    if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
    hd_layer = (float*) malloc((nnum) * sizeof(float));
    if (hd_layer == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    ss = -1;
    for (j=0; j<nnum; j++)  {
        ss++;
        sscanf(&ps[ss], "%f", &hd_layer[j]);
        while (ps[ss] != ' ' && j != nnum-1)    ss++;
        //printf("%f ", hd_layer[j]);
    }
    //printf("\n");
    // table
    nn = 0;
    for (i=1; i<nnum; i++)    nn = nn + hd_layer[i];
    //printf("nn: %d\n", nn);
    ti_layer = (float**) malloc(nn * sizeof(float*));
    if (ti_layer == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i=0; i<nn; i++)  {
        // read one useful line
        irt = get_one_line(fp, SL9, ps, &nnum);
        //printf("%s|%d %d\n", ps, nnum, ps[0]);
        if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
        ti_layer[i] = (float*) malloc(nnum * sizeof(float));
        if (ti_layer[i] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        ss = -1;
        for (j=0; j<nnum; j++)   {
            ss++;
            irt = sscanf(&ps[ss], "%f", &ti_layer[i][j]);
            while (ps[ss] != ' ' && j != nnum-1)    ss++;
        }
    }
    //printf("ti, %d, %d\n", nn, nnum);
    //for (i=0; i<nn; i++)  {
    //    for (j=0; j<nnum; j++)
    //        printf("%f ", ti_layer[i][j]);
    //    printf("\n");
    //}
    //printf("--------------------------\n");
    //getchar();


    fclose(fp);

    return(0);
}



int rd_hd_ti(FILE *fp, float **p_head, float ***p_table)
{
    int     nnum, nn, irt, ss, i, j;
    char    ps[SL9];
    float   *head, **table;

    // head
    irt = get_one_line(fp, SL9, ps, &nnum);
    //printf("%s|%d\n", ps, nnum);
    if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
    head = (float*) malloc((nnum) * sizeof(float));
    if (head == NULL)    { printf("Memory not allocated.\n");  exit(1);  }
    ss = -1;
    for (j=0; j<nnum; j++)  {
        ss++;
        sscanf(&ps[ss], "%f", &head[j]);
        while (ps[ss] != ' ' && j != nnum-1)    ss++;
        //printf("%f ", head[j]);
    }
    //printf("\n");

    // table
    nn = round(head[0]);
    table = (float**) malloc(nn * sizeof(float*));
    if (table == NULL)    {printf("Memory not allocated.\n");  exit(1);  }
    for (i=0; i<nn; i++)  {
        // read one useful line
        irt = get_one_line(fp, SL9, ps, &nnum);
        //printf("%s|%d %d\n", ps, nnum, ps[0]);
        if (nnum<0)  { printf("Wrong tis format\n");  exit(1); }
        table[i] = (float*) malloc(nnum * sizeof(float));
        if (table[i] == NULL)  { printf("Memory not allocated.\n");  exit(1);  }
        ss = -1;
        for (j=0; j<nnum; j++)   {
            ss++;
            irt = sscanf(&ps[ss], "%f", &table[i][j]);
            while (ps[ss] != ' ' && j != nnum-1)    ss++;
            //printf("%f ", table[i][j]);
        }
    }
    //printf("\n");
    //getchar();

    *p_head = head;  *p_table = table;

    return(0);
}



// free table of indexing
int free_tis()
{
    int     i, j, n, nn;

    n = round(hd_flb[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_flb[i]);
    free(ti_flb);
    free(hd_flb);

    n = round(hd_sqc[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_sqc[i]);
    free(ti_sqc);
    free(hd_sqc);

    n = round(nti1[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti1[i]);
    free(ti1);
    free(nti1);

    n = round(hd_suv[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_suv[i]);
    free(ti_suv);
    free(hd_suv);

    n = round(nti3[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti3[i]);
    free(ti3);
    free(nti3);

    n = round(nti4[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti4[i]);
    free(ti4);
    free(nti4);

    n = round(nti5[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti5[i]);
    free(ti5);
    free(nti5);

    n = round(nti6[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti6[i]);
    free(ti6);
    free(nti6);

    n = round(nti7[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti7[i]);
    free(ti7);
    free(nti7);

    n = round(hd_one[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_one[i]);
    free(ti_one);
    free(hd_one);

    n = round(hd_tbo[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_tbo[i]);
    free(ti_tbo);
    free(hd_tbo);

    n = round(hd_iwp[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_iwp[i]);
    free(ti_iwp);
    free(hd_iwp);

    n = round(hd_de[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_de[i]);
    free(ti_de);
    free(hd_de);

    n = round(hd_tc[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_tc[i]);
    free(ti_tc);
    free(hd_tc);

    n = round(hd_em[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_em[i]);
    free(ti_em);
    free(hd_em);

    n = round(hd_frc[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_frc[i]);
    free(ti_frc);
    free(hd_frc);

    n = round(hd_freq[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_freq[i]);
    free(ti_freq);
    free(hd_freq);

    n = round(hd_ecm[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_ecm[i]);
    free(ti_ecm);
    free(hd_ecm);

    n = round(hd_kv[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_kv[i]);
    free(ti_kv);
    free(hd_kv);

    n = round(hd_ko2[0]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_ko2[i]);
    free(ti_ko2);
    free(hd_ko2);

    n = round(hd_cal[1]);
    //printf("n: %d\n", n);
    for (i=0; i<n; i++)     free(ti_cal[i]);
    free(ti_cal);
    free(hd_cal);

    n = round(hd_layer[0]);
    nn = 0;
    for (i=0; i<n; i++)
        nn = nn + round(hd_layer[1+i]);
    //printf("n: %d\n", n);
    for (i=0; i<nn; i++)     free(ti_layer[i]);
    free(ti_layer);
    free(hd_layer);
    
//***********************************************************************************    
// free tis NN (yf)    
    if (*flg_rd_sdnn==1){
        // free sdnn global
        for (i=0; i<layers_sdnn_gb[0]; i++)   
            free(norm_in_sdnn_gb[i]);
        free(norm_in_sdnn_gb);
        for (i=0; i<layers_sdnn_gb[nlayers_sdnn_gb[0]-1]; i++)
            free(norm_out_sdnn_gb[i]);
        free(norm_out_sdnn_gb);
        for (i=0; i<nlayers_sdnn_gb[0]-1; i++){
            for (j=0;j<layers_sdnn_gb[i+1]; j++){
                free(weights_sdnn_gb[i][j]);
            }
            free(weights_sdnn_gb[i]);
            free(bias_sdnn_gb[i]);
        }
        free(weights_sdnn_gb);
        free(bias_sdnn_gb);
        for (i=0; i<layers_sdnn_gb[0]; i++)
            free(varid_sdnn_gb[i]);
        free(varid_sdnn_gb);
        free(nlayers_sdnn_gb);
        free(layers_sdnn_gb);
        free(nntype_sdnn_gb);
        free(varid_threshold_adj_sdnn_gb);
        free(threshold_adj_nsec_sdnn_gb);
        free(threshold_adj_sec_sdnn_gb);
        free(threshold_sdnn_gb);
        free(geolimit_sdnn_gb);
        
        // free sdnn conus
        for (i=0; i<layers_sdnn_us[0]; i++)   
            free(norm_in_sdnn_us[i]);
        free(norm_in_sdnn_us);
        for (i=0; i<layers_sdnn_us[nlayers_sdnn_us[0]-1]; i++)
            free(norm_out_sdnn_us[i]);
        free(norm_out_sdnn_us);
        for (i=0; i<nlayers_sdnn_us[0]-1; i++){
            for (j=0;j<layers_sdnn_us[i+1]; j++){
                free(weights_sdnn_us[i][j]);
            }
            free(weights_sdnn_us[i]);
            free(bias_sdnn_us[i]);
        }
        free(weights_sdnn_us);
        free(bias_sdnn_us);
        for (i=0; i<layers_sdnn_us[0]; i++)
            free(varid_sdnn_us[i]);
        free(varid_sdnn_us);
        free(nlayers_sdnn_us);
        free(layers_sdnn_us);
        free(nntype_sdnn_us);
        free(varid_threshold_adj_sdnn_us);
        free(threshold_adj_nsec_sdnn_us);
        free(threshold_adj_sec_sdnn_us);
        free(threshold_sdnn_us);
        free(geolimit_sdnn_us);
        
        // free sdnn alaska
        for (i=0; i<layers_sdnn_ak[0]; i++)   
            free(norm_in_sdnn_ak[i]);
        free(norm_in_sdnn_ak);
        for (i=0; i<layers_sdnn_ak[nlayers_sdnn_ak[0]-1]; i++)
            free(norm_out_sdnn_ak[i]);
        free(norm_out_sdnn_ak);
        for (i=0; i<nlayers_sdnn_ak[0]-1; i++){
            for (j=0;j<layers_sdnn_ak[i+1]; j++){
                free(weights_sdnn_ak[i][j]);
            }
            free(weights_sdnn_ak[i]);
            free(bias_sdnn_ak[i]);
        }
        free(weights_sdnn_ak);
        free(bias_sdnn_ak);
        for (i=0; i<layers_sdnn_ak[0]; i++)
            free(varid_sdnn_ak[i]);
        free(varid_sdnn_ak);
        free(nlayers_sdnn_ak);
        free(layers_sdnn_ak);
        free(nntype_sdnn_ak);
        free(varid_threshold_adj_sdnn_ak);
        free(threshold_adj_nsec_sdnn_ak);
        free(threshold_adj_sec_sdnn_ak);
        free(threshold_sdnn_ak);
        free(geolimit_sdnn_ak);
    }
    
    if (*flg_rd_tbnn==1){
        // free tbnn global
        for (i=0; i<layers_tbnn_gb[0]; i++)   
            free(norm_in_tbnn_gb[i]);
        free(norm_in_tbnn_gb);
        for (i=0; i<layers_tbnn_gb[nlayers_tbnn_gb[0]-1]; i++)
            free(norm_out_tbnn_gb[i]);
        free(norm_out_tbnn_gb);
        for (i=0; i<nlayers_tbnn_gb[0]-1; i++){
            for (j=0;j<layers_tbnn_gb[i+1]; j++){
                free(weights_tbnn_gb[i][j]);
            }
            free(weights_tbnn_gb[i]);
            free(bias_tbnn_gb[i]);
        }
        free(weights_tbnn_gb);
        free(bias_tbnn_gb);
        for (i=0; i<layers_tbnn_gb[0]; i++)
            free(varid_tbnn_gb[i]);
        free(varid_tbnn_gb);
        free(nlayers_tbnn_gb);
        free(layers_tbnn_gb);
        free(nntype_tbnn_gb);
        free(geolimit_tbnn_gb);
        
        // free tbnn conus
        for (i=0; i<layers_tbnn_us[0]; i++)   
            free(norm_in_tbnn_us[i]);
        free(norm_in_tbnn_us);
        for (i=0; i<layers_tbnn_us[nlayers_tbnn_us[0]-1]; i++)
            free(norm_out_tbnn_us[i]);
        free(norm_out_tbnn_us);
        for (i=0; i<nlayers_tbnn_us[0]-1; i++){
            for (j=0;j<layers_tbnn_us[i+1]; j++){
                free(weights_tbnn_us[i][j]);
            }
            free(weights_tbnn_us[i]);
            free(bias_tbnn_us[i]);
        }
        free(weights_tbnn_us);
        free(bias_tbnn_us);
        for (i=0; i<layers_tbnn_us[0]; i++)
            free(varid_tbnn_us[i]);
        free(varid_tbnn_us);
        free(nlayers_tbnn_us);
        free(layers_tbnn_us);
        free(nntype_tbnn_us);
        free(geolimit_tbnn_us);
        
        // free tbnn alaska
        for (i=0; i<layers_tbnn_ak[0]; i++)   
            free(norm_in_tbnn_ak[i]);
        free(norm_in_tbnn_ak);
        for (i=0; i<layers_tbnn_ak[nlayers_tbnn_ak[0]-1]; i++)
            free(norm_out_tbnn_ak[i]);
        free(norm_out_tbnn_ak);
        for (i=0; i<nlayers_tbnn_ak[0]-1; i++){
            for (j=0;j<layers_tbnn_ak[i+1]; j++){
                free(weights_tbnn_ak[i][j]);
            }
            free(weights_tbnn_ak[i]);
            free(bias_tbnn_ak[i]);
        }
        free(weights_tbnn_ak);
        free(bias_tbnn_ak);
        for (i=0; i<layers_tbnn_ak[0]; i++)
            free(varid_tbnn_ak[i]);
        free(varid_tbnn_ak);
        free(nlayers_tbnn_ak);
        free(layers_tbnn_ak);
        free(nntype_tbnn_ak);
        free(geolimit_tbnn_ak);
    }
    
    if (*flg_rd_iwpnn==1){
        // free iwpnn global
        for (i=0; i<layers_iwpnn_gb[0]; i++)   
            free(norm_in_iwpnn_gb[i]);
        free(norm_in_iwpnn_gb);
        for (i=0; i<layers_iwpnn_gb[nlayers_iwpnn_gb[0]-1]; i++)
            free(norm_out_iwpnn_gb[i]);
        free(norm_out_iwpnn_gb);
        for (i=0; i<nlayers_iwpnn_gb[0]-1; i++){
            for (j=0;j<layers_iwpnn_gb[i+1]; j++){
                free(weights_iwpnn_gb[i][j]);
            }
            free(weights_iwpnn_gb[i]);
            free(bias_iwpnn_gb[i]);
        }
        free(weights_iwpnn_gb);
        free(bias_iwpnn_gb);
        for (i=0; i<layers_iwpnn_gb[0]; i++)
            free(varid_iwpnn_gb[i]);
        free(varid_iwpnn_gb);
        free(nlayers_iwpnn_gb);
        free(layers_iwpnn_gb);
        free(nntype_iwpnn_gb);
        free(geolimit_iwpnn_gb);
        
        // free iwpnn conus
        for (i=0; i<layers_iwpnn_us[0]; i++)   
            free(norm_in_iwpnn_us[i]);
        free(norm_in_iwpnn_us);
        for (i=0; i<layers_iwpnn_us[nlayers_iwpnn_us[0]-1]; i++)
            free(norm_out_iwpnn_us[i]);
        free(norm_out_iwpnn_us);
        for (i=0; i<nlayers_iwpnn_us[0]-1; i++){
            for (j=0;j<layers_iwpnn_us[i+1]; j++){
                free(weights_iwpnn_us[i][j]);
            }
            free(weights_iwpnn_us[i]);
            free(bias_iwpnn_us[i]);
        }
        free(weights_iwpnn_us);
        free(bias_iwpnn_us);
        for (i=0; i<layers_iwpnn_us[0]; i++)
            free(varid_iwpnn_us[i]);
        free(varid_iwpnn_us);
        free(nlayers_iwpnn_us);
        free(layers_iwpnn_us);
        free(nntype_iwpnn_us);
        free(geolimit_iwpnn_us);
        
        // free iwpnn alaska
        for (i=0; i<layers_iwpnn_ak[0]; i++)   
            free(norm_in_iwpnn_ak[i]);
        free(norm_in_iwpnn_ak);
        for (i=0; i<layers_iwpnn_ak[nlayers_iwpnn_ak[0]-1]; i++)
            free(norm_out_iwpnn_ak[i]);
        free(norm_out_iwpnn_ak);
        for (i=0; i<nlayers_iwpnn_ak[0]-1; i++){
            for (j=0;j<layers_iwpnn_ak[i+1]; j++){
                free(weights_iwpnn_ak[i][j]);
            }
            free(weights_iwpnn_ak[i]);
            free(bias_iwpnn_ak[i]);
        }
        free(weights_iwpnn_ak);
        free(bias_iwpnn_ak);
        for (i=0; i<layers_iwpnn_ak[0]; i++)
            free(varid_iwpnn_ak[i]);
        free(varid_iwpnn_ak);
        free(nlayers_iwpnn_ak);
        free(layers_iwpnn_ak);
        free(nntype_iwpnn_ak);
        free(geolimit_iwpnn_ak);
    }
    
    if (*flg_rd_tcnn==1){    
        // free tcnn global
        for (i=0; i<layers_tcnn_gb[0]; i++)   
            free(norm_in_tcnn_gb[i]);
        free(norm_in_tcnn_gb);
        for (i=0; i<layers_tcnn_gb[nlayers_tcnn_gb[0]-1]; i++)
            free(norm_out_tcnn_gb[i]);
        free(norm_out_tcnn_gb);
        for (i=0; i<nlayers_tcnn_gb[0]-1; i++){
            for (j=0;j<layers_tcnn_gb[i+1]; j++){
                free(weights_tcnn_gb[i][j]);
            }
            free(weights_tcnn_gb[i]);
            free(bias_tcnn_gb[i]);
        }
        free(weights_tcnn_gb);
        free(bias_tcnn_gb);
        for (i=0; i<layers_tcnn_gb[0]; i++)
            free(varid_tcnn_gb[i]);
        free(varid_tcnn_gb);
        free(nlayers_tcnn_gb);
        free(layers_tcnn_gb);
        free(nntype_tcnn_gb);
        free(geolimit_tcnn_gb);    
    
    
        // free tcnn conus
        for (i=0; i<layers_tcnn_us[0]; i++)   
            free(norm_in_tcnn_us[i]);
        free(norm_in_tcnn_us);
        for (i=0; i<layers_tcnn_us[nlayers_tcnn_us[0]-1]; i++)
            free(norm_out_tcnn_us[i]);
        free(norm_out_tcnn_us);
        for (i=0; i<nlayers_tcnn_us[0]-1; i++){
            for (j=0;j<layers_tcnn_us[i+1]; j++){
                free(weights_tcnn_us[i][j]);
            }
            free(weights_tcnn_us[i]);
            free(bias_tcnn_us[i]);
        }
        free(weights_tcnn_us);
        free(bias_tcnn_us);
        for (i=0; i<layers_tcnn_us[0]; i++)
            free(varid_tcnn_us[i]);
        free(varid_tcnn_us);
        free(nlayers_tcnn_us);
        free(layers_tcnn_us);
        free(nntype_tcnn_us);
        free(geolimit_tcnn_us);
        
        // free tcnn alaska
        for (i=0; i<layers_tcnn_ak[0]; i++)   
            free(norm_in_tcnn_ak[i]);
        free(norm_in_tcnn_ak);
        for (i=0; i<layers_tcnn_ak[nlayers_tcnn_ak[0]-1]; i++)
            free(norm_out_tcnn_ak[i]);
        free(norm_out_tcnn_ak);
        for (i=0; i<nlayers_tcnn_ak[0]-1; i++){
            for (j=0;j<layers_tcnn_ak[i+1]; j++){
                free(weights_tcnn_ak[i][j]);
            }
            free(weights_tcnn_ak[i]);
            free(bias_tcnn_ak[i]);
        }
        free(weights_tcnn_ak);
        free(bias_tcnn_ak);
        for (i=0; i<layers_tcnn_ak[0]; i++)
            free(varid_tcnn_ak[i]);
        free(varid_tcnn_ak);
        free(nlayers_tcnn_ak);
        free(layers_tcnn_ak);
        free(nntype_tcnn_ak);
        free(geolimit_tcnn_ak);
    }
    
    if (*flg_rd_sfrnn==1){
        // free sfrnn global
        for (i=0; i<layers_sfrnn_gb[0]; i++)   
            free(norm_in_sfrnn_gb[i]);
        free(norm_in_sfrnn_gb);
        for (i=0; i<layers_sfrnn_gb[nlayers_sfrnn_gb[0]-1]; i++)
            free(norm_out_sfrnn_gb[i]);
        free(norm_out_sfrnn_gb);
        for (i=0; i<nlayers_sfrnn_gb[0]-1; i++){
            for (j=0;j<layers_sfrnn_gb[i+1]; j++){
                free(weights_sfrnn_gb[i][j]);
            }
            free(weights_sfrnn_gb[i]);
            free(bias_sfrnn_gb[i]);
        }
        free(weights_sfrnn_gb);
        free(bias_sfrnn_gb);
        for (i=0; i<layers_sfrnn_gb[0]; i++)
            free(varid_sfrnn_gb[i]);
        free(varid_sfrnn_gb);
        free(nlayers_sfrnn_gb);
        free(layers_sfrnn_gb);
        free(nntype_sfrnn_gb);
        free(flg_post_adj_sfrnn_gb);
        free(post_adj_varid_sfrnn_gb);
        free(post_adj_sfrnn_gb);       
        free(geolimit_sfrnn_gb);
        
        // free sfrnn conus
        for (i=0; i<layers_sfrnn_us[0]; i++)   
            free(norm_in_sfrnn_us[i]);
        free(norm_in_sfrnn_us);
        for (i=0; i<layers_sfrnn_us[nlayers_sfrnn_us[0]-1]; i++)
            free(norm_out_sfrnn_us[i]);
        free(norm_out_sfrnn_us);
        for (i=0; i<nlayers_sfrnn_us[0]-1; i++){
            for (j=0;j<layers_sfrnn_us[i+1]; j++){
                free(weights_sfrnn_us[i][j]);
            }
            free(weights_sfrnn_us[i]);
            free(bias_sfrnn_us[i]);
        }
        free(weights_sfrnn_us);
        free(bias_sfrnn_us);
        for (i=0; i<layers_sfrnn_us[0]; i++)
            free(varid_sfrnn_us[i]);
        free(varid_sfrnn_us);
        free(nlayers_sfrnn_us);
        free(layers_sfrnn_us);
        free(nntype_sfrnn_us);
        free(flg_post_adj_sfrnn_us);
        free(post_adj_varid_sfrnn_us);
        free(post_adj_sfrnn_us);       
        free(geolimit_sfrnn_us);
        
        // free sfrnn alaska
        for (i=0; i<layers_sfrnn_ak[0]; i++)   
            free(norm_in_sfrnn_ak[i]);
        free(norm_in_sfrnn_ak);
        for (i=0; i<layers_sfrnn_ak[nlayers_sfrnn_ak[0]-1]; i++)
            free(norm_out_sfrnn_ak[i]);
        free(norm_out_sfrnn_ak);
        for (i=0; i<nlayers_sfrnn_ak[0]-1; i++){
            for (j=0;j<layers_sfrnn_ak[i+1]; j++){
                free(weights_sfrnn_ak[i][j]);
            }
            free(weights_sfrnn_ak[i]);
            free(bias_sfrnn_ak[i]);
        }
        free(weights_sfrnn_ak);
        free(bias_sfrnn_ak);
        for (i=0; i<layers_sfrnn_ak[0]; i++)
            free(varid_sfrnn_ak[i]);
        free(varid_sfrnn_ak);
        free(nlayers_sfrnn_ak);
        free(layers_sfrnn_ak);
        free(nntype_sfrnn_ak);
        free(flg_post_adj_sfrnn_ak);
        free(post_adj_varid_sfrnn_ak);
        free(post_adj_sfrnn_ak);       
        free(geolimit_sfrnn_ak);
    }
     
    // free NN flags
    free(flg_rd_sdnn);
    free(flg_do_sdnn);
    free(flg_gb_sdnn);
    free(flg_us_sdnn);
    free(flg_ak_sdnn);
    
    free(flg_rd_tbnn);
    free(flg_do_tbnn);
    free(flg_gb_tbnn);
    free(flg_us_tbnn);
    free(flg_ak_tbnn);
    
    free(flg_rd_iwpnn);
    free(flg_do_iwpnn);
    free(flg_gb_iwpnn);
    free(flg_us_iwpnn);
    free(flg_ak_iwpnn);
    
    free(flg_rd_tcnn);
    free(flg_do_tcnn);
    free(flg_gb_tcnn);
    free(flg_us_tcnn);
    free(flg_ak_tcnn);
    
    free(flg_rd_sfrnn);
    free(flg_do_sfrnn);
    free(flg_gb_sfrnn);
    free(flg_us_sfrnn);
    free(flg_ak_sfrnn);    
//***************************************************************************    
// free tis_XGB (yf)    
    if (*flg_rd_xgb==1){
        // free sdnn global
        for (i=0; i<ntrees[0]; i++){
            for (j=0;j<nnodes[i];j++){
                free(xgb_trees[i][j]);
            }
            free(xgb_trees[i]);
        }
        free(xgb_trees);
        for (i=0; i<ninput_xgb[0]; i++)
            free(varid_xgb[i]);
        for (i=0; i<nfilter[0]; i++)
            free(flt_xgb[i]);
        free(varid_xgb);
        free(flt_xgb);
        free(ntrees);
        free(nnodes);
        free(nfilter);
        free(ninput_xgb);
    }
        free(flg_do_xgb);
        free(flg_rd_xgb);               
//***************************************************************************
    return(0);
}




