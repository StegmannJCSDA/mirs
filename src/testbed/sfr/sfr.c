/**************************************************************************
 *  Program Name      : sfr.c
 *  Type              : Main program
 *  Function          : Reads satellite data, generates SFR product and 
 *                      output product data
 *  Input Files       : None
 *  Output Files      : None
 *  Subroutine Called : rd_conf(), process(), prt_conf0()
 *  Called by         : None
 *  Author            : Jun Dong, jundong@umd.edu, 03/19/2019
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// ****************************************************** /
#define     SL5     32
#define     SL9     512


int     rd_conf();
int     prt_conf0();
int     process();
int     rpl_ssr();
int     rpl_ssr1();
int     prt_vnl();


// ****************************************************** /
// avn data
float           ****hgt_a, ****tmp_a, ****rh_a, ****vvel_a, ****clwmr_a,
                ***rh2m_a, ***pres0m_a, ***hgt0m_a, ***cwat_a, ***ugrd_a,
                ***vgrd_a, ***pwat_a, ***tmp0m_a, ***tmp2m_a;
float           *hgt_a1, *tmp_a1, *rh_a1, *vvel_a1, *clwmr_a1,
                *rh2m_a1, *pres0m_a1, *hgt0m_a1, *cwat_a1, *ugrd_a1,
                *vgrd_a1, *pwat_a1, *tmp0m_a1, *tmp2m_a1;
// sat data
float           **lon, **lat, ***at, **lza, **sza;
char            **obmd;
long long       **btjs;
int             dtse[2][6];
float           *lon_p1, *lat_p1, *at_p1, *lza_p1, *sza_p1;
char            *obmd_p1;
long long       *btjs_p1;
// mirs need
float           **dsec, *dsec_p1;

// product
int             **stype, **sd, **flag;
float           **sfr, **prob, **iwp, **de;
int             *stype_p1, *sd_p1, *flag_p1;
float           *sfr_p1, *prob_p1, *iwp_p1, *de_p1;

// control table of indexing
float           *hd_flb, **ti_flb, *hd_sqc, **ti_sqc;

float           *nti1, **ti1, *hd_suv, **ti_suv, *nti3, **ti3, *nti4, **ti4;
float           *nti5, **ti5, *nti6, **ti6, *nti7, **ti7;

float           *hd_one, **ti_one, *hd_tbo, **ti_tbo, *hd_iwp, **ti_iwp;
float           *hd_de,  **ti_de,  *hd_tc,  **ti_tc;
float           *hd_em,  **ti_em;
float           *hd_frc, **ti_frc, *hd_freq,**ti_freq, *hd_ecm, **ti_ecm;
float           *hd_kv,  **ti_kv,  *hd_ko2, **ti_ko2;

float           *hd_cal, **ti_cal, *hd_layer, **ti_layer;

// *******************************************************************
// yf: nn arrays
int     *flg_rd_sdnn, *flg_rd_tbnn, *flg_rd_iwpnn, *flg_rd_tcnn, *flg_rd_sfrnn;
int     *flg_do_sdnn, *flg_do_tbnn, *flg_do_iwpnn, *flg_do_tcnn, *flg_do_sfrnn;
int     *flg_gb_sdnn, *flg_gb_tbnn, *flg_gb_iwpnn, *flg_gb_tcnn, *flg_gb_sfrnn;
int     *flg_us_sdnn, *flg_us_tbnn, *flg_us_iwpnn, *flg_us_tcnn, *flg_us_sfrnn;
int     *flg_ak_sdnn, *flg_ak_tbnn, *flg_ak_iwpnn, *flg_ak_tcnn, *flg_ak_sfrnn;

int     *nlayers_sdnn_gb, *layers_sdnn_gb, **varid_sdnn_gb, *nntype_sdnn_gb;
float   **norm_in_sdnn_gb,**norm_out_sdnn_gb,***weights_sdnn_gb,**bias_sdnn_gb;
int     *varid_threshold_adj_sdnn_gb, *threshold_adj_nsec_sdnn_gb;
float   *threshold_adj_sec_sdnn_gb, *threshold_sdnn_gb, *geolimit_sdnn_gb;

int     *nlayers_sdnn_us, *layers_sdnn_us, **varid_sdnn_us, *nntype_sdnn_us;
float   **norm_in_sdnn_us,**norm_out_sdnn_us,***weights_sdnn_us,**bias_sdnn_us;
int     *varid_threshold_adj_sdnn_us, *threshold_adj_nsec_sdnn_us;
float   *threshold_adj_sec_sdnn_us, *threshold_sdnn_us, *geolimit_sdnn_us;

int     *nlayers_sdnn_ak, *layers_sdnn_ak, **varid_sdnn_ak, *nntype_sdnn_ak;
float   **norm_in_sdnn_ak,**norm_out_sdnn_ak,***weights_sdnn_ak,**bias_sdnn_ak;
int     *varid_threshold_adj_sdnn_ak, *threshold_adj_nsec_sdnn_ak;
float   *threshold_adj_sec_sdnn_ak, *threshold_sdnn_ak, *geolimit_sdnn_ak;

int     *nlayers_tbnn_gb, *layers_tbnn_gb, **varid_tbnn_gb, *nntype_tbnn_gb;
float   **norm_in_tbnn_gb, **norm_out_tbnn_gb, ***weights_tbnn_gb, 
        **bias_tbnn_gb, *geolimit_tbnn_gb;

int     *nlayers_tbnn_us, *layers_tbnn_us, **varid_tbnn_us, *nntype_tbnn_us;
float   **norm_in_tbnn_us, **norm_out_tbnn_us, ***weights_tbnn_us, 
        **bias_tbnn_us, *geolimit_tbnn_us;

int     *nlayers_tbnn_ak, *layers_tbnn_ak, **varid_tbnn_ak, *nntype_tbnn_ak;
float   **norm_in_tbnn_ak, **norm_out_tbnn_ak, ***weights_tbnn_ak, 
        **bias_tbnn_ak, *geolimit_tbnn_ak;

int     *nlayers_iwpnn_gb, *layers_iwpnn_gb, **varid_iwpnn_gb, *nntype_iwpnn_gb;
float   **norm_in_iwpnn_gb, **norm_out_iwpnn_gb, ***weights_iwpnn_gb, 
        **bias_iwpnn_gb, *geolimit_iwpnn_gb;

int     *nlayers_iwpnn_us, *layers_iwpnn_us, **varid_iwpnn_us, *nntype_iwpnn_us;
float   **norm_in_iwpnn_us, **norm_out_iwpnn_us, ***weights_iwpnn_us, 
        **bias_iwpnn_us, *geolimit_iwpnn_us;

int     *nlayers_iwpnn_ak, *layers_iwpnn_ak, **varid_iwpnn_ak, *nntype_iwpnn_ak;
float   **norm_in_iwpnn_ak, **norm_out_iwpnn_ak, ***weights_iwpnn_ak, 
        **bias_iwpnn_ak, *geolimit_iwpnn_ak;

int     *nlayers_tcnn_gb, *layers_tcnn_gb, **varid_tcnn_gb, *nntype_tcnn_gb;
float   **norm_in_tcnn_gb, **norm_out_tcnn_gb, ***weights_tcnn_gb, 
        **bias_tcnn_gb, *geolimit_tcnn_gb;

int     *nlayers_tcnn_us, *layers_tcnn_us, **varid_tcnn_us, *nntype_tcnn_us;
float   **norm_in_tcnn_us, **norm_out_tcnn_us, ***weights_tcnn_us, 
        **bias_tcnn_us, *geolimit_tcnn_us;

int     *nlayers_tcnn_ak, *layers_tcnn_ak, **varid_tcnn_ak, *nntype_tcnn_ak;
float   **norm_in_tcnn_ak, **norm_out_tcnn_ak, ***weights_tcnn_ak, 
        **bias_tcnn_ak, *geolimit_tcnn_ak;

int     *nlayers_sfrnn_gb, *layers_sfrnn_gb, **varid_sfrnn_gb, 
        *nntype_sfrnn_gb, *flg_post_adj_sfrnn_gb, *post_adj_varid_sfrnn_gb;
float   **norm_in_sfrnn_gb, **norm_out_sfrnn_gb, ***weights_sfrnn_gb, 
        **bias_sfrnn_gb, *post_adj_sfrnn_gb, *geolimit_sfrnn_gb;

int     *nlayers_sfrnn_us, *layers_sfrnn_us, **varid_sfrnn_us, 
        *nntype_sfrnn_us, *flg_post_adj_sfrnn_us, *post_adj_varid_sfrnn_us;
float   **norm_in_sfrnn_us, **norm_out_sfrnn_us, ***weights_sfrnn_us, 
        **bias_sfrnn_us, *post_adj_sfrnn_us, *geolimit_sfrnn_us;

int     *nlayers_sfrnn_ak, *layers_sfrnn_ak, **varid_sfrnn_ak, 
        *nntype_sfrnn_ak, *flg_post_adj_sfrnn_ak, *post_adj_varid_sfrnn_ak;
float   **norm_in_sfrnn_ak, **norm_out_sfrnn_ak, ***weights_sfrnn_ak, 
        **bias_sfrnn_ak, *post_adj_sfrnn_ak, *geolimit_sfrnn_ak;
// *******************************************************************
// yf: xgb arrays
int     *flg_rd_xgb, *flg_do_xgb;
int     *ninput_xgb,*ntrees,**varid_xgb, *nnodes, *nfilter;
float   **flt_xgb;
double  ***xgb_trees; 

//  ******************************************************************
int main(int argc, char *argv[])
{
    char    ffn_conf[SL9], ffn_sfr[SL9];
    char    argp[12][2][SL5];
    int     nargp;

    char    sat[SL5], sen[SL5], reg[SL5], sat1[SL5], sen1[SL5], reg1[SL5];
    char    s_iftype[SL5],  s_flist0[SL9], s_flist1[SL9];
    char    a_name[SL5],    a_dn_pat[SL9], a_fn_pat[SL9];
    char    o_fn_mode[SL5], o_dn_pat[SL9], o_fn_pat[SL9];

    char    ffn_mask[SL9],  ffn_lese[SL9];
    char    ffn_pin[SL9],   ffn_tis[SL9];
    char    ffn_lut[SL9];
    
    int     irt, i1;

    // yf: tis NN file
    char    ffn_tisnn[SL9];
    // yf: tis XGB file
    char    ffn_tisxgb[SL9];

    // detemind config file and other inline argument
    printf("argc: %d\n", argc);
    for (i1=0; i1<argc; i1++)    printf("argv[%d]: %s\n", i1, argv[i1]);

    // save command save to get its path
    strncpy(ffn_sfr, argv[0], SL9);

    nargp = 0;
    if (argc == 1)  {
        // print example config file and exit
        printf("print example\n");
        prt_conf0();
        exit(1);
    }
    else if (argc == 2)  {
        // regular run, run with config file specified
        strncpy(ffn_conf, argv[1], SL9);
    }
    else if (argc%2 == 0 && argv[1][0] != '-')  {
        // cmd conf.txt -p0 x0 -p1 x1
        strncpy(ffn_conf, argv[1], SL9);
        nargp = (argc-2) / 2;
        for (i1=0; i1<nargp; i1++)  {
            strncpy(argp[i1][0], argv[2*i1+2], SL5);
            strncpy(argp[i1][1], argv[2*i1+3], SL5);
        }
    }
    else if (argc%2 == 1 && argv[1][0] == '-')  {
        // cmd -p0 x0 -p1 x1
        nargp = (argc-1) / 2;
        for (i1=0; i1<nargp; i1++)  {
            strncpy(argp[i1][0], argv[2*i1+1], SL5);
            strncpy(argp[i1][1], argv[2*i1+2], SL5);
        }
    }
    else  {
        printf("\nWrong number of input arguments.\n\n");
        exit(1);
    }
    printf("-----------------------\n");


    // if conf file exist
    // yf: add ffn_tisnn
    //strncpy(ffn_conf, "conf.txt", SL5);
    rd_conf(ffn_conf, ffn_sfr, sat, sen, reg, sat1, sen1, reg1, 
            s_iftype, s_flist0, s_flist1, a_name, a_dn_pat, a_fn_pat, 
            o_fn_mode, o_dn_pat, o_fn_pat, ffn_mask, ffn_lese, 
            ffn_pin, ffn_tis, ffn_lut, ffn_tisnn, ffn_tisxgb);

    // deal argument, related to var name list
    for (i1=0; i1<nargp; i1++)  {
        printf("%d/%d  %s  %s\n", i1, nargp, argp[i1][0], argp[i1][1]);
        if (strcmp(argp[i1][0], "-vnl") == 0)  {
            //printf("print var name list\n");
            prt_vnl(argp[i1][0], argp[i1][1], sat, sen, reg);
        }
        else  {
            //printf("no match, ignore\n");
        }
    }

    //exit(1);

    // main process
    // yf: add ffn_sdnn
    irt = process(sat, sen, reg, sat1, sen1, reg1, 
            s_iftype, s_flist0, s_flist1, a_name, a_dn_pat, a_fn_pat,  
            o_fn_mode, o_dn_pat, o_fn_pat, 
            ffn_mask, ffn_lese, ffn_pin, ffn_tis, ffn_lut, ffn_tisnn, ffn_tisxgb);

    return(irt);
}



// print variable name list
int prt_vnl(char *opt, char *val, char *sat, char *sen, char *reg)
{
    // input
    printf("opt, val, sat, sen, reg: %s, %s, %s, %s, %s\n", 
            opt, val, sat, sen, reg);
    //printf("%zu, %zu, %zu\n", strlen(val), strlen(sat), strlen(sen));

    //char    ffn_vnl[] = "../static/vnl_{sat}_{sen}.txt";
    char    ffn_vnl[] = "../static/var_name_list.txt";
    FILE    *fp;

    int     nvn, i1;
    char    str1[SL5], str2[SL5], svn[SL5];

    //irt = rpl_ssr(ffn_vnl, sat, sen, reg);
    fp = fopen(ffn_vnl, "r");
    if (fp == NULL)  {
        printf("ERROR: Fail to open file (stop): %s\n", ffn_vnl);
        exit(1);
    }
   
    while (1)  {
        fscanf(fp, "%31s %31s %d", str1, str2, &nvn);

        // if end, exit loop
        if (feof(fp))  break;

        //printf(".....................\n");
        //printf("fmt, str1, str2: %s, %s, %d\n", str1, str2, nvn);

        i1 = 0;
        if (strcmp(str1, val)==0 && strcmp(str2, sen)==0)  {
            //printf("opt match: %s, %s, %d\n", str1, str2, nvn);
            for (i1=0; i1<nvn; i1++)  {
                fscanf(fp, "%31s", svn);
                printf("%4d: %s\n", i1, svn);
            }
            //break;
        }
        if (strcmp(str1, val)==0 && strcmp(str2, "*")==0)  {
            //printf("* match: %s, %s, %d\n", str1, str2, nvn);
            for (i1=0; i1<nvn; i1++)  {
                fscanf(fp, "%31s", svn);
                printf("%4d: %s\n", i1, svn);
            }
            //break;
        }

        // if not file, skip to next
        if (i1 == 0)  {
            for (i1=0; i1<nvn; i1++)  {
                fscanf(fp, "%31s", svn);
                //printf("%4d: %s\n", i1, svn);
            }
        }
    }

    return(0);
}




