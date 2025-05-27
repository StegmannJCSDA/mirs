/***********************************************************************
 *  Program Name      : rpl_ssr.c
 *  Type              : Subroutine
 *  Function          : functions related with satellite, such as
 *                      form static data file name, set config file, 
 *                      read in static file
 *  Input Files       : None
 *  Output Files      : None
 *  Subroutine Called : rpl_ssr(), rpl_dttm(), rd_pin(), rd_conf(), ...
 *  Called by         : process.c
 *  Author            : Jun Dong, jundong@umd.edu, 03/19/2019
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <libgen.h>

#include "d_sys.h"


int     str2farr(char *, int, float *);
int     str2iarr(char *, int, int *);
int     get_one_line();
int     prt_conf0();


#define     SL9    512
#define     SL5     32



// find and replace 3 basic {vid} ({sat}, {sen}, {reg}) in string
int rpl_ssr(char *str, char *sat, char *sen, char *reg)
{
    int     i, irt=0;
    char    tsat[SL5], tsen[SL5], treg[SL5];

    strncpy(tsat, sat, SL5);
    strncpy(tsen, sen, SL5);
    strncpy(treg, reg, SL5);

    strrepa(str, "{reg}", treg);
    strrepa(str, "{sen}", tsen);
    strrepa(str, "{sat}", tsat);
    i=0;  while (treg[i])  { treg[i]=toupper(treg[i]);  i++; }
    strrepa(str, "{REG}", treg);
    i=0;  while (tsen[i])  { tsen[i]=toupper(tsen[i]);  i++; }
    strrepa(str, "{SEN}", tsen);
    i=0;  while (tsat[i])  { tsat[i]=toupper(tsat[i]);  i++; }
    strrepa(str, "{SAT}", tsat);

    return(irt);
}



// find and replace 3 sub basic {vid} ({sat1}, {sen1}, {reg1}) in string
int rpl_ssr1(char *str, char *sat1, char *sen1, char *reg1)
{
    int     i, irt=0;
    char    tsat[SL5], tsen[SL5], treg[SL5];

    strncpy(tsat, sat1, SL5);
    strncpy(tsen, sen1, SL5);
    strncpy(treg, reg1, SL5);

    strrepa(str, "{reg1}", treg);
    strrepa(str, "{sen1}", tsen);
    strrepa(str, "{sat1}", tsat);
    i=0;  while (treg[i])  { treg[i]=toupper(treg[i]);  i++; }
    strrepa(str, "{REG1}", treg);
    i=0;  while (tsen[i])  { tsen[i]=toupper(tsen[i]);  i++; }
    strrepa(str, "{SEN1}", tsen);
    i=0;  while (tsat[i])  { tsat[i]=toupper(tsat[i]);  i++; }
    strrepa(str, "{SAT1}", tsat);

    return(irt);
}



// find and replace all date/time {vid} in string
int rpl_dttm(char *ffn, int *dp, int ni)
{
    int     i, irt=0;
    char    str[16], sfg[16];

    // replace {yyyy} using dp[0][*] ...
    sprintf(str, "%04d", *(dp+0));    strrepa(ffn, "{yyyy}", str);
    sprintf(str, "%02d", *(dp+1));    strrepa(ffn, "{mm}", str);
    sprintf(str, "%02d", *(dp+2));    strrepa(ffn, "{dd}", str);
    sprintf(str, "%02d", *(dp+3));    strrepa(ffn, "{HH}", str);
    sprintf(str, "%02d", *(dp+4));    strrepa(ffn, "{MM}", str);
    sprintf(str, "%02d", *(dp+5));    strrepa(ffn, "{SS}", str);
    //printf("ffn: %s\n", ffn);

    // replace {yyyy#}
    for (i=0; i<ni; i++)  {
        sprintf(str, "%04d", *(dp+i*6+0));
        sprintf(sfg, "{yyyy%d}", i);
        strrepa(ffn, sfg, str);

        sprintf(str, "%02d", *(dp+i*6+1));
        sprintf(sfg, "{mm%d}", i);
        strrepa(ffn, sfg, str);

        sprintf(str, "%02d", *(dp+i*6+2));
        sprintf(sfg, "{dd%d}", i);
        strrepa(ffn, sfg, str);

        sprintf(str, "%02d", *(dp+i*6+3));
        sprintf(sfg, "{HH%d}", i);
        strrepa(ffn, sfg, str);

        sprintf(str, "%02d", *(dp+i*6+4));
        sprintf(sfg, "{MM%d}", i);
        strrepa(ffn, sfg, str);

        sprintf(str, "%02d", *(dp+i*6+5));
        sprintf(sfg, "{SS%d}", i);
        strrepa(ffn, sfg, str);
    }

    return(irt);
}



// find and replace folder {vid} ({cdn}, {sdn}) in path
int rpl_csdn(char *ffn_conf, char *ffn_sfr, char *ffn)
{
    char    tdn[SL9], *ptdn;

    //printf("ffn_conf: %s\n", ffn_conf);
    strncpy(tdn, ffn_conf, SL9);
    ptdn = dirname(tdn);
    strrepa(ffn, "{cdn}", ptdn);
    //printf("ptdn: %s\n", ptdn);

    strncpy(tdn, ffn_sfr, SL9);
    dirname(tdn);
    strrepa(ffn, "{sdn}", tdn);

    return(0);
}



// default conf vars
// yf: add ffn_tisnn
int set_conf0(
        char *sat,          char *sen,          char *reg,
        char *sat1,         char *sen1,         char *reg1,
        char *s_iftype,     char *s_flist0,     char *s_flist1,
        char *a_name,       char *a_dn_pat,     char *a_fn_pat,
        char *o_fn_mode,    char *o_dn_pat,     char *o_fn_pat,
        char *ffn_mask,     char *ffn_lese,     char *ffn_pin,
        char *ffn_tis,      char *ffn_lut,      char *ffn_tisnn, 
        char *ffn_latbox,   char *ffn_ccoef,    char *ffn_tisxgb
        )
{
    strncpy(sat,         "sat", SL5);
    strncpy(sen,         "sen", SL5);
    strncpy(reg,         "swath", SL5);
    strncpy(sat1,        "SAT", SL5);
    strncpy(sen1,        "SEN", SL5);
    strncpy(reg1,        "SWATH", SL5);

    strncpy(s_iftype,    "fmsdr", SL5);
    strncpy(s_flist0,    "fname0.list", SL9);
    strncpy(s_flist1,    "fname1.list", SL9);

    strncpy(a_name,      "avn", SL5);
    strncpy(a_dn_pat,    "../avn/{yyyy}{mm}", SL9);
    strncpy(a_fn_pat,    "avn_comb{HH}.bin.{yyyy}{mm}{dd}", SL9);

    strncpy(o_fn_mode,   "std", SL5);
    strncpy(o_dn_pat,    "../output/{yyyy}{mm}", SL9);
    strncpy(o_fn_pat,    "SFR_{sat1}_{sen1}_{sat1}_S{yyyy0}{mm0}{dd0}"
                        "_{HH0}{MM0}{SS0}", SL9);

    strncpy(ffn_lut,     "../static/sphere_shape_{sen}.dat", SL9);
    strncpy(ffn_mask,    "../static/mask.bin", SL9);
    strncpy(ffn_lese,    "../static/lese_tab.dat", SL9);

    strncpy(ffn_pin,     "../static/input_{sen}_{sat}.dat", SL9);
    strncpy(ffn_tis,     "../static/tis_{sen}_{sat}.dat", SL9);
    strncpy(ffn_tisnn,   "../static/tis_{sen}_{sat}_NN.dat",SL9);
    strncpy(ffn_tisxgb,  "../static/tis_{sen}_{sat}_XGB.dat",SL9);
    return(0);
}



// read config file
// yf: add ffn_tisnn
int rd_conf(char *ffn_conf, char *ffn_sfr, 
        char *sat,          char *sen,          char *reg,
        char *sat1,         char *sen1,         char *reg1,
        char *s_iftype,     char *s_flist0,     char *s_flist1,
        char *a_name,       char *a_dn_pat,     char *a_fn_pat,
        char *o_fn_mode,    char *o_dn_pat,     char *o_fn_pat,
        char *ffn_mask,     char *ffn_lese,     char *ffn_pin,
        char *ffn_tis,      char *ffn_lut,      char *ffn_tisnn,
        char *ffn_tisxgb)
{
    int     nw, ss, irt;
    char    ps[SL9], ps1[SL9], *ps2;

    FILE    *fp;

    // if conf file exists, rewrite vars
    printf("ffn_conf: %s\n", ffn_conf);
    fp = openfc(ffn_conf);

    if (fp == NULL)  {
        printf("Error to load config file: %s\n", ffn_conf);
        printf("Use ./sfr to generate example of config file.\n\n");
        exit(1);
    }

    irt = get_one_line(fp, SL9, ps, &nw);
    while (irt == 0)  {
        // parse
        ss = 0;    while (ps[ss] != ' ')    ss++;
        strncpy(ps1, ps, ss);    ps1[ss] = '\0';
        ps2 = ps + ss + 1;
        //printf("ps1,2|%s|%s|%d\n", ps1, ps2, nw);

        if      (!strcmp(ps1, "sat"))           strncpy(sat, ps2, SL5);
        else if (!strcmp(ps1, "sen"))           strncpy(sen, ps2, SL5);
        else if (!strcmp(ps1, "reg"))           strncpy(reg, ps2, SL5);
        else if (!strcmp(ps1, "sat1"))          strncpy(sat1, ps2, SL5);
        else if (!strcmp(ps1, "sen1"))          strncpy(sen1, ps2, SL5);
        else if (!strcmp(ps1, "reg1"))          strncpy(reg1, ps2, SL5);

        else if (!strcmp(ps1, "s_iftype"))      strncpy(s_iftype, ps2, SL5);
        else if (!strcmp(ps1, "s_flist0"))      strncpy(s_flist0, ps2, SL9);
        else if (!strcmp(ps1, "s_flist1"))      strncpy(s_flist1, ps2, SL9);

        else if (!strcmp(ps1, "a_name"))        strncpy(a_name, ps2, SL5);
        else if (!strcmp(ps1, "a_dn_pat"))      strncpy(a_dn_pat, ps2, SL9);
        else if (!strcmp(ps1, "a_fn_pat"))      strncpy(a_fn_pat, ps2, SL9);

        else if (!strcmp(ps1, "o_fn_mode"))     strncpy(o_fn_mode, ps2, SL5);
        else if (!strcmp(ps1, "o_dn_pat"))      strncpy(o_dn_pat, ps2, SL9);
        else if (!strcmp(ps1, "o_fn_pat"))      strncpy(o_fn_pat, ps2, SL9);

        else if (!strcmp(ps1, "ffn_lut"))       strncpy(ffn_lut, ps2, SL9);
        else if (!strcmp(ps1, "ffn_mask"))      strncpy(ffn_mask, ps2, SL9);
        else if (!strcmp(ps1, "ffn_lese"))      strncpy(ffn_lese, ps2, SL9);

        else if (!strcmp(ps1, "ffn_pin"))       strncpy(ffn_pin, ps2, SL9);
        else if (!strcmp(ps1, "ffn_tis"))       strncpy(ffn_tis, ps2, SL9);
        else if (!strcmp(ps1, "ffn_tisnn"))     strncpy(ffn_tisnn, ps2, SL9);
        else if (!strcmp(ps1, "ffn_tisxgb"))    strncpy(ffn_tisxgb, ps2, SL9);
        
        else  {
            printf("Wrong parameters (%s) in %s\n", ps1, ffn_conf);
            exit(1);
        }

        irt = get_one_line(fp, SL9, ps, &nw);
    }
    fclose(fp);

    // put same conf dir or cmd dir
    rpl_csdn(ffn_conf, ffn_sfr, s_flist0);
    rpl_csdn(ffn_conf, ffn_sfr, s_flist1);
    rpl_csdn(ffn_conf, ffn_sfr, o_dn_pat);
    rpl_csdn(ffn_conf, ffn_sfr, a_dn_pat);
    //printf("s_flist0: %s\n", s_flist0);
    //printf("s_flist1: %s\n", s_flist1);
    //printf("o_dn_pat: %s\n", o_dn_pat);
    //printf("a_dn_pat: %s\n", a_dn_pat);

    rpl_csdn(ffn_conf, ffn_sfr, ffn_lut);
    rpl_csdn(ffn_conf, ffn_sfr, ffn_mask);
    rpl_csdn(ffn_conf, ffn_sfr, ffn_lese);
    rpl_csdn(ffn_conf, ffn_sfr, ffn_pin);
    rpl_csdn(ffn_conf, ffn_sfr, ffn_tis);
    rpl_csdn(ffn_conf, ffn_sfr, ffn_tisnn);
    rpl_csdn(ffn_conf, ffn_sfr, ffn_tisxgb);
    //printf("ffn_lut : %s\n", ffn_lut);
    //printf("ffn_mask: %s\n", ffn_mask);
    //printf("ffn_lese: %s\n", ffn_lese);
    //printf("ffn_pin : %s\n", ffn_pin);
    //printf("ffn_tis : %s\n", ffn_tis);
    //getchar(); 

    return(0);
}



// print example of config file
// yf: add ffn_tisnn
int prt_conf0()
{
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% input parameters");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% satellite");
    puts("sat             npp");
    puts("sen             atms");
    puts("reg             conus");
    puts("");
    puts("");
    puts("% names, by seperate with {sat}, sat name can change to anything");
    puts("% such as n20 to j01/jpss1");
    puts("sat1            npp");
    puts("sen1            atms");
    puts("reg1            conus");
    puts("");
    puts("");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% folder info");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% default: relative to current/run path");
    puts("% {cdn}  : relative to config file path");
    puts("% {sdn}  : relative to sfr command path");
    puts("");
    puts("");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% sat input");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% choice: satms, tatms, fmsdr, srs, l1b, gmi, ssmis");
    puts("s_iftype        l1b");
    puts("");
    puts("s_flist0        ffname00.list");
    puts("s_flist1        ffname01.list");
    puts("");
    puts("");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% product output");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% fn_mode: fmbin, std, pat, (fmbin, std, pat)"
            "_(fmbin, he4, sf0, sf1, sf2)");
    puts("o_fn_mode       pat_he4");
    puts("");
    puts("o_fn_pat        SFR_{REG}_{SEN}_{SAT}_S{yyyy0}{mm0}{dd0}"
            "_{HH0}{MM0}{SS0}&");
    puts("                _E{yyyy1}{mm1}{dd1}_{HH1}{MM1}{SS1}.hdf");
    puts("");
    puts("o_dn_pat        output/{sat}");
    puts("");
    puts("");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% model data");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("a_name          avn");
    puts("");
    puts("a_dn_pat        {sdn}/../avn/{yyyy}{mm}");
    puts("");
    puts("a_fn_pat        avn_comb{HH}.bin.{yyyy}{mm}{dd}");
    puts("");
    puts("");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% others");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("ffn_lut         {sdn}/../static/snowShape_sphere_optics_{sen}.dat");
    puts("ffn_mask        {sdn}/../static/mask.bin");
    puts("ffn_lese        {sdn}/../static/lese_tab.dat");
    puts("");
    puts("ffn_pin         {sdn}/../static/input_prod_{sat}_{sen}.dat");
    puts("ffn_tis         {sdn}/../static/tis_{sat}_{sen}.dat");
    puts("ffn_tisnn       {sdn}/../static/tis_{sat}_{sen}_NN.dat");
    puts("ffn_tisxgb      {sdn}/../static/tis_{sat}_{sen}_XGB.dat");
    puts("");
    puts("");

    return(0);
}



// read input file parameter
int rd_pin(char *ffn_pin, 
        int *avn_t0dt, float *avn_l0, float *avn_dl, 
        int *avn_nl, int *avn_nlev, int *avn_nvar2, 
        float *sfr_lim, int *mask_row, int *mask_col, int *mask_res, 
        int *nfov0, int *nchan0)
//        float *scan_ang0, float *rsat0, float *REARTH0)
{
    int     nw, irt, ss;
    char    ps[SL9], ps1[SL9], *ps2;

    FILE    *fp;

    printf("ffn_pin: %s\n", ffn_pin);
    fp = openfs(ffn_pin);
    if (fp == NULL)  {
        printf("Cannot read %s\n", ffn_pin);
        exit(1);
    }

    irt = get_one_line(fp, SL9, ps, &nw);
    while (irt == 0)  {
        // parse
        ss = 0;    while (ps[ss] != ' ')    ss++;
        strncpy(ps1, ps, ss);    ps1[ss] = '\0';
        ps2 = ps + ss + 1;
        //printf("ps1,2|%s|%s|%d\n", ps1, ps2, nw);

        if      (!strcmp(ps1, "avn_t0dt"))
            irt = str2iarr(ps2, nw-1, avn_t0dt);
        else if (!strcmp(ps1, "avn_l0"))
            irt = str2farr(ps2, nw-1, avn_l0);
        else if (!strcmp(ps1, "avn_dl"))
            irt = str2farr(ps2, nw-1, avn_dl);
        else if (!strcmp(ps1, "avn_nl"))
            irt = str2iarr(ps2, nw-1, avn_nl);
        else if (!strcmp(ps1, "avn_nlev"))
            irt = str2iarr(ps2, nw-1, avn_nlev);
        else if (!strcmp(ps1, "avn_nvar2"))
            irt = str2iarr(ps2, nw-1, avn_nvar2);

        else if (!strcmp(ps1, "sfr_lim"))
            irt = str2farr(ps2, nw-1, sfr_lim);

        else if (!strcmp(ps1, "mask_row"))
            irt = str2iarr(ps2, nw-1, mask_row);
        else if (!strcmp(ps1, "mask_col"))
            irt = str2iarr(ps2, nw-1, mask_col);
        else if (!strcmp(ps1, "mask_res"))
            irt = str2iarr(ps2, nw-1, mask_res);

        else if (!strcmp(ps1, "nfov0"))
            irt = str2iarr(ps2, nw-1, nfov0);
        else if (!strcmp(ps1, "nchan0"))
            irt = str2iarr(ps2, nw-1, nchan0);
//        else if (!strcmp(ps1, "scan_ang0"))
//              irt = str2farr(ps2, nw-1, scan_ang0);
//        else if (!strcmp(ps1, "rsat0"))
//            irt = str2farr(ps2, nw-1, rsat0);
//        else if (!strcmp(ps1, "REARTH0"))
//            irt = str2farr(ps2, nw-1, REARTH0);

        else  {
            printf("Wrong parameters (%s) in %s\n", ps1, ffn_pin);
            exit(1);
        }

        irt = get_one_line(fp, SL9, ps, &nw);
    }

    fclose(fp);

    return(0);
}




