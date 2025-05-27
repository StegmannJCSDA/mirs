/**************************************************************************
 *  Program Name      : avn_data.c
 *  Type              : Subroutine
 *  Function          : Update avn data, read in 2 avn data, 
 *                      only update when needed, also include function 
 *                      to collocate data with satellite
 *  Input Files       : None
 *  Output Files      : None
 *  Subroutine Called : avn_update(), rd_avn(), avn_llt2ijr(), rpt_dttm()
 *  Called by         : snowfall.c
 *  Author            : Jun Dong, jundong@umd.edu, 03/19/2019
 **************************************************************************/

#include "stdio.h"
#include "math.h"

#include "d_jsec.h"
#include "d_sys.h"
#include "d_darrc.h"

#define     SL9     512
#define     SL5     32


// avn data, global
extern float    ****hgt_a, ****tmp_a, ****rh_a, ****vvel_a, ****clwmr_a, 
                ***rh2m_a, ***pres0m_a, ***hgt0m_a, ***cwat_a, ***ugrd_a, 
                ***vgrd_a, ***pwat_a, ***tmp0m_a, ***tmp2m_a;
extern float    *hgt_a1, *tmp_a1, *rh_a1, *vvel_a1, *clwmr_a1,
                *rh2m_a1, *pres0m_a1, *hgt0m_a1, *cwat_a1, *ugrd_a1,
                *vgrd_a1, *pwat_a1, *tmp0m_a1, *tmp2m_a1;

int     rpl_dttm();
int     rd_avn();
int     rd_gfs();


/************************************************************/
int avn_update(char *a_name, long long jsec, char *dn_pat, char *fn_pat, 
        int navn, int *avn_t0dt, int *avn_nl, int avn_nlev, 
        long long *avn_jsec, int *avn_ind, int *avn_flag)
{
    //printf("****** avn_update ******\n");
    //printf("a_name               : %s\n", a_name);
    //printf("jsec                 : %lld\n", jsec);
    //printf("dn_pat               : %s\n", dn_pat);
    //printf("fn_pat               : %s\n", fn_pat);
    //printf("avn_t0dt[0], t0dt[1] : %d %d\n", avn_t0dt[0], avn_t0dt[1]);
    //printf("avn_jsec[0], jsec[1] : %lld %lld\n", avn_jsec[0], avn_jsec[1]);
    //printf("avn_ind[0],  ind[1]  : %d %d\n", avn_ind[0], avn_ind[1]);
    //printf("avn_flag[0], flag[1] : %d %d\n", avn_flag[0], avn_flag[1]);

    int         dp[1][6];  //year1, mon1, day1, hour1, min1, sec1;
    char        ffn0[SL9], ffn1[SL9];;

    long long   avn_jsec1[navn];

    int         ind, ret=0, ret1;
    

    avn_jsec1[0] = (jsec + avn_t0dt[0]) / avn_t0dt[1] * avn_t0dt[1];
    avn_jsec1[1] = (jsec + avn_t0dt[0] + avn_t0dt[1]) 
            / avn_t0dt[1] * avn_t0dt[1];
    

    // ffn0
    //printf("------- ffn0 ------\n");
    ind = avn_jsec1[0] / avn_t0dt[1] % 2;
    //printf("a_jsec1[0], ind, a_ind[0], a_jsec[ind]: %lld, %d, %d, %lld\n", 
    //        avn_jsec1[0], ind, avn_ind[0], avn_jsec[ind]);
    avn_ind[0] = ind;
    if (avn_jsec[ind] != avn_jsec1[0])  {
        avn_jsec[ind] = avn_jsec1[0];

        jsec2cal(avn_jsec1[0], &dp[0][0], &dp[0][1], &dp[0][2], 
                &dp[0][3], &dp[0][4], &dp[0][5]);
        strncpy(ffn0, dn_pat, SL9);
        strncat(ffn0, "/", 1);  strncat(ffn0, fn_pat, strlen(fn_pat));
        rpl_dttm(ffn0, dp, 1);
        printf("ffn0: |%s|\n", ffn0);

        // read file
        if (strcmp(a_name, "avn") == 0)  {
            ret1 = rd_avn(ffn0, ind, avn_nl, avn_nlev);
        }
        else if (strcmp(a_name, "gfs") == 0)  {
            ret1 = rd_gfs(ffn0, ind, avn_nl, avn_nlev);
        }
        else  {
            printf("Wrong model data name\n");
            exit(1);
        }
        avn_flag[ind] = ret1;
        printf("avn_ind[0] %d  avn_flag[%d] %d\n", ind, ind, avn_flag[ind]);
    }
    else  {
        //jsec2cal(avn_jsec1[0], &dp[0][0], &dp[0][1], &dp[0][2], 
        //        &dp[0][3], &dp[0][4], &dp[0][5]);
        //printf("%04d %02d %02d %02d\n", dp[0][0], dp[0][1], 
        //        dp[0][2], dp[0][3]);
        //printf("same %d %d\n", avn_ind[0], avn_flag[ind]);
    }


    // ffn1
    //printf("------- ffn1 ------\n");
    ind = avn_jsec1[1] / avn_t0dt[1] % 2;
    //printf("a_jsec1[1], ind, a_ind[1], a_jsec[ind]: %lld, %d, %d, %lld\n", 
    //        avn_jsec1[1], ind, avn_ind[1], avn_jsec[ind]);
    avn_ind[1] = ind;
    if (avn_jsec[ind] != avn_jsec1[1])  {
        avn_jsec[ind] = avn_jsec1[1];

        jsec2cal(avn_jsec1[1], &dp[0][0], &dp[0][1], &dp[0][2], 
                &dp[0][3], &dp[0][4], &dp[0][5]);
        strncpy(ffn1, dn_pat, SL9);
        strncat(ffn1, "/", 1);    strncat(ffn1, fn_pat, strlen(fn_pat));
        rpl_dttm(ffn1, dp, 1);
        printf("ffn1: |%s|\n", ffn1);

        // read file
        ret1 = rd_avn(ffn1, ind, avn_nl, avn_nlev); 
        avn_flag[ind] = ret1;
        printf("avn_ind[1] %d  avn_flag[%d] %d\n", ind, ind, avn_flag[ind]);
    }
    else  {
        //jsec2cal(avn_jsec1[1], &dp[0][0], &dp[0][1], &dp[0][2], 
        //        &dp[0][3], &dp[0][4], &dp[0][5]);
        //printf("%04d %02d %02d %02d\n", dp[0][0], dp[0][1], 
        //        dp[0][2], dp[0][3]);
        //printf("same %d %d\n", avn_ind[1], avn_flag[ind]);
    }

    return(ret);
}


// read gfs data
int rd_gfs(char *ffn, int ind, int *nl, int nv)
{
    FILE*   fp;
    printf("ffn: %s\n", ffn);

    fp = fopen(ffn, "r");
    fclose(fp);

    return(0);
}



// read avn data
int rd_avn(char *ffn, int ind, int *nl, int nv)
{
    FILE    *fp;
    int     lenf;
    size_t  ne, ne0;

    fp = fopen(ffn, "r");
    if (fp == NULL)  {
        printf("Cannot open %s\n", ffn);
        return(1);
    }

    fseek(fp, 0L, SEEK_END);
    lenf = ftell(fp);
    //printf("file len: %d\n", lenf);
    fseek(fp, 0L, SEEK_SET);
    if (lenf == 0)  {
        printf("%s is empty\n", ffn);
        fclose(fp);
        return(2);
    }

    ne0 = nl[0]*nl[1]*nv;

    // set var pointers
    ne = fread(hgt_a[ind][0][0], sizeof(float), ne0, fp);
    //printf("ne, ne0: %zu, %zu\n", ne, ne0);
    if (ne != ne0)  { printf("Load hgt wrong\n");  fclose(fp);  return(3); }
    //bin_output_3d("temp/bin.hgt", hgt_a[ind], nv, nl[1], nl[0]);

    ne = fread(tmp_a[ind][0][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load tmp wrong\n");  fclose(fp);  return(3); }
    //bin_output_3d("temp/bin.tmp", tmp_a[ind], nv, nl[1], nl[0]);

    ne = fread(rh_a[ind][0][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load rh wrong\n");  fclose(fp);  return(3); }
    //bin_output_3d("temp/bin.rh", rh_a[ind], nv, nl[1], nl[0]);

    ne = fread(vvel_a[ind][0][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load vvel wrong\n");  fclose(fp);  return(3); }
    //bin_output_3d("temp/bin.vvel", vvel_a[ind], nv, nl[1], nl[0]);

    ne = fread(clwmr_a[ind][0][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load clwmr wrong\n");  fclose(fp);  return(3); }
    //bin_output_3d("temp/bin.clwmr", clwmr_a[ind], nv, nl[1], nl[0]);

    ne0 = nl[0]*nl[1];

    ne = fread(rh2m_a[ind][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load rh2m wrong\n");  fclose(fp);  return(3); }
    //bin_output_2d("temp/bin.rh2m", rh2m_a[ind], nl[1], nl[0]);

    ne = fread(pres0m_a[ind][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load pres0m wrong\n");  fclose(fp);  return(3); }
    //bin_output_2d("temp/bin.pres0m", pres0m_a[ind], nl[1], nl[0]);

    ne = fread(hgt0m_a[ind][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load hgt0m wrong\n");  fclose(fp);  return(3); }
    //bin_output_2d("temp/bin.hgt0m", hgt0m_a[ind], nl[1], nl[0]);

    ne = fread(cwat_a[ind][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load cwat wrong\n");  fclose(fp);  return(3); }
    //bin_output_2d("temp/bin.cwat", cwat_a[ind], nl[1], nl[0]);

    ne = fread(ugrd_a[ind][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load ugrd wrong\n");  fclose(fp);  return(3); }
    //bin_output_2d("temp/bin.ugrd", ugrd_a[ind], nl[1], nl[0]);

    ne = fread(vgrd_a[ind][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load vgrd wrong\n");  fclose(fp);  return(3); }
    //bin_output_2d("temp/bin.vgrd", vgrd_a[ind], nl[1], nl[0]);

    ne = fread(pwat_a[ind][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load pwat wrong\n");  fclose(fp);  return(3); }
    //bin_output_2d("temp/bin.pwat", pwat_a[ind], nl[1], nl[0]);

    ne = fread(tmp0m_a[ind][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load tmp0m wrong\n");  fclose(fp);  return(3); }
    //bin_output_2d("temp/bin.tmp0m", tmp0m_a[ind], nl[1], nl[0]);

    ne = fread(tmp2m_a[ind][0], sizeof(float), ne0, fp);
    if (ne != ne0)  { printf("Load tmp2m wrong\n");  fclose(fp);  return(3); }
    //bin_output_2d("temp/bin.tmp2m", tmp2m_a[ind], nl[1], nl[0]);

    fclose(fp);

    return(0);
}



// calculate index
int avn_llt2ijr(float p_lon, float p_lat, long long p_jsec, 
        int *ilon, int *ilat, float *rot, 
        float *a_l0, float *a_dl, int *a_nl, int *a_t0dt, 
        long long *ajsec1)
{
    (*ilon) = round((p_lon-a_l0[0]) / a_dl[0]);
    if ((*ilon) < 0)           (*ilon) = (*ilon) + a_nl[0];
    if ((*ilon) == a_nl[0])    (*ilon) = (*ilon) - a_nl[0];
    (*ilat) = round((p_lat-a_l0[1]) / a_dl[1]);

    if ((*ilon)<0 || (*ilon)>a_nl[0] || (*ilat)<0 || (*ilat)>a_nl[0])  {
        printf("ilon/ilat out of range: %d, %d\n", *ilon, *ilat);
        exit(1);
    }

    //printf("%d %d\n", p_jsec-ajsec1[0], ajsec1[1]-ajsec1[0]);
    (*rot) = (p_jsec-ajsec1[0])*1.0 / ((ajsec1[1]-ajsec1[0]));
    if (*rot<0 || *rot>1)  {
        printf("time out of range: %f\n", *rot);
        exit(1);
    }
    
    return(0);
}






// init avn vars
int avn_init(int navn, int *avn_nl, int avn_nlev)
{
    hgt_a1   = init_4dc_float(navn, avn_nlev, avn_nl[1], avn_nl[0], &hgt_a);
    tmp_a1   = init_4dc_float(navn, avn_nlev, avn_nl[1], avn_nl[0], &tmp_a);
    rh_a1    = init_4dc_float(navn, avn_nlev, avn_nl[1], avn_nl[0], &rh_a);
    vvel_a1  = init_4dc_float(navn, avn_nlev, avn_nl[1], avn_nl[0], &vvel_a);
    clwmr_a1 = init_4dc_float(navn, avn_nlev, avn_nl[1], avn_nl[0], &clwmr_a);

    rh2m_a1  = init_3dc_float(navn, avn_nl[1], avn_nl[0], &rh2m_a);
    pres0m_a1= init_3dc_float(navn, avn_nl[1], avn_nl[0], &pres0m_a);
    hgt0m_a1 = init_3dc_float(navn, avn_nl[1], avn_nl[0], &hgt0m_a);
    cwat_a1  = init_3dc_float(navn, avn_nl[1], avn_nl[0], &cwat_a);
    ugrd_a1  = init_3dc_float(navn, avn_nl[1], avn_nl[0], &ugrd_a);
    vgrd_a1  = init_3dc_float(navn, avn_nl[1], avn_nl[0], &vgrd_a);
    pwat_a1  = init_3dc_float(navn, avn_nl[1], avn_nl[0], &pwat_a);
    tmp0m_a1 = init_3dc_float(navn, avn_nl[1], avn_nl[0], &tmp0m_a);
    tmp2m_a1 = init_3dc_float(navn, avn_nl[1], avn_nl[0], &tmp2m_a);

    return(0);
} 



// free avn vars
int avn_free(int navn, int *avn_nl, int avn_nlev)
{
    int     irt, sirt=0;

    irt = free_4dc_float(navn, avn_nlev, avn_nl[1], hgt_a, hgt_a1);
    sirt+=irt;
    irt = free_4dc_float(navn, avn_nlev, avn_nl[1], tmp_a, tmp_a1);
    sirt+=irt;
    irt = free_4dc_float(navn, avn_nlev, avn_nl[1], rh_a, rh_a1);
    sirt+=irt;
    irt = free_4dc_float(navn, avn_nlev, avn_nl[1], vvel_a, vvel_a1);
    sirt+=irt;
    irt = free_4dc_float(navn, avn_nlev, avn_nl[1], clwmr_a, clwmr_a1);
    sirt+=irt;
    irt = free_3dc_float(navn, avn_nl[1], rh2m_a, rh2m_a1);
    sirt+=irt;
    irt = free_3dc_float(navn, avn_nl[1], pres0m_a, pres0m_a1);
    sirt+=irt;
    irt = free_3dc_float(navn, avn_nl[1], hgt0m_a, hgt0m_a1);
    sirt+=irt;
    irt = free_3dc_float(navn, avn_nl[1], cwat_a, cwat_a1);
    sirt+=irt;
    irt = free_3dc_float(navn, avn_nl[1], ugrd_a, ugrd_a1);
    sirt+=irt;
    irt = free_3dc_float(navn, avn_nl[1], vgrd_a, vgrd_a1);
    sirt+=irt;
    irt = free_3dc_float(navn, avn_nl[1], pwat_a, pwat_a1);
    sirt+=irt;
    irt = free_3dc_float(navn, avn_nl[1], tmp0m_a, tmp0m_a1);
    sirt+=irt;
    irt = free_3dc_float(navn, avn_nl[1], tmp2m_a, tmp2m_a1);
    sirt+=irt;
    //printf("free avn sirt: %d\n", sirt);

    return(0);
};


/*
int bin_output_3d(char *ffn, float ***p, int n1, int n2, int n3)
{
    FILE    *fp;
    int     i1, i2, i3;

    fp = fopen(ffn, "w");
    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)
            for (i3=0; i3<n3; i3++)
                fwrite(&p[i1][i2][i3], sizeof(float), 1, fp);
    }
    fclose(fp);

    return(0);
}



int bin_output_2d(char *ffn, float **p, int n1, int n2)
{
    FILE    *fp;
    int     i1, i2;

    fp = fopen(ffn, "w");
    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)
            fwrite(&p[i1][i2], sizeof(float), 1, fp);
    }
    fclose(fp);

    return(0);
}
*/



