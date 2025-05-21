/***************************************************************************
 *  Program Name      : process.c
 *  Type              : subroutine
 *  Function          : control input, retrieval, and output
 *  Subroutine Called : beamtime_cal.c, mask_stype.c, prod_init.c, 
 *                      gfs_update.c, snowfall_sd.c, snowfall_rate.c
 *                      prod_output.c
 *  Called by         : sfr.c
 *  Author            : Jun Dong, jundong@umd.edu, 03/19/2019
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "d_darrc.h"
#include "d_jsec.h"
#include "d_sys.h"


#define     SL9     512
#define     NAVN    2
#define     NRCH    6


int     rpl_ssr();
int     rpl_ssr1();
int     rd_pin();
int     rd_mask();
int     rd_tis();
int     avn_init();
int     sat_data();
int     prod_init();
int     mask_stype();
int     snowfall();
int     set_flag();
int     sat_free();
int     prod_free();
int     avn_free();
int     free_tis();

int     prod_output(char*, char*, char*, char*, char*, char*, char*, char*, 
                char*, char*, char*, char*, 
                int, int, int, int*, int**, float*, int, int**);

int     cal_vars();
int     dsr_run();
int     dsr_1dvar0();

// yf: tisnn read, and nn flags
int             rd_tis_nn();
extern  int     *flg_do_sdnn, *flg_do_tbnn, *flg_do_iwpnn, *flg_do_tcnn, 
                *flg_do_sfrnn;

// yf: tis_xgb read and flags
int             rd_tis_xgb();
extern int      *flg_do_xgb;


// entrance of process sfr
int process(char *sat, char *sen, char *reg, 
        char *sat1, char *sen1, char *reg1, 
        char *s_iftype, char *s_flist0, char *s_flist1, 
        char *a_name, char *a_dn_pat, char *a_fn_pat, 
        char *o_fn_mode, char *o_dn_pat, char *o_fn_pat, 
        char *ffn_mask, char *ffn_lese, char *ffn_pin, 
        char *ffn_tis, char *ffn_lut, char *ffn_tisnn, char *ffn_tisxgb)
{
    char        ffn_file0[SL9], ffn_file1[SL9], ffn_out[SL9];

    int         nfov0, nchan0, nfov, nscan, nchan;
    int         *nfova, *nfova_p1, **flag0, *flag0_p1;

    float       sfr_lim[2];

    int         aind[NAVN]={-1,-1}, aflag[NAVN]={-1,-1};
    long long   ajsec[NAVN]={-1,-1};
    float       a_l0[2], a_dl[2];
    int         a_t0dt[2], a_nl[2], a_nlev, a_nvar2;

    int         mask_row, mask_col, mask_res;
    char        **mask, *mask_p1;

    int         nyref, **lese_tab;

    int         irt, sl, i, ifile;
    char        *cp;
    FILE        *fp_fl0, *fp_fl1;


    printf("sat, sen, reg: %s, %s, %s\n", sat, sen, reg);

    // open input flist0, flist1
    printf("s_iftype: %s\n", s_iftype);
    irt = rpl_ssr(s_flist0, sat, sen, reg);
    irt = rpl_ssr1(s_flist0, sat1, sen1, reg1);
    fp_fl0 = openfs(s_flist0);

    irt = rpl_ssr(s_flist1, sat, sen, reg);
    irt = rpl_ssr1(s_flist1, sat1, sen1, reg1);
    fp_fl1 = fopen(s_flist1, "r");
    if (fp_fl1 == NULL)  {
        printf("Cannot open %s\n", s_flist1);
        printf("This file may not needed, decide later ...\n");
    }
    else  {
        printf("Successfully open file: %s\n", s_flist1);
    }
    printf("---------------------------------\n");

    // read input parameter file
    irt = rpl_ssr(ffn_pin, sat, sen, reg);
    irt = rpl_ssr1(ffn_pin, sat1, sen1, reg1);
    irt = rd_pin(ffn_pin, a_t0dt, a_l0, a_dl, a_nl, &a_nlev, &a_nvar2,  
            sfr_lim, &mask_row, &mask_col, &mask_res, &nfov0, &nchan0);
    if (irt == 0)    printf("Successfully read in %s\n", ffn_pin);
    printf("---------------------------------\n");

    // read mask file
    mask_p1 = init_2dc_char(mask_row, mask_col, &mask);
    irt = rd_mask(ffn_mask, mask_row, mask_col, mask);
    if (irt == 0)    printf("Successfully read in %s\n", ffn_mask);
    printf("---------------------------------\n");

    // read lese_tab.dat
    irt = rd_lese_tab(ffn_lese, &nyref, &lese_tab);
    if (irt == 0)    printf("Successfully read in %s\n", ffn_lese);
    printf("---------------------------------\n");

    // read table of indexing
    irt = rpl_ssr(ffn_tis, sat, sen, reg);
    irt = rpl_ssr1(ffn_tis, sat1, sen1, reg1);
    irt = rd_tis(ffn_tis);
    if (irt == 0)    printf("Successfully read in %s\n", ffn_tis);
    printf("---------------------------------\n");
    
    //**************************************************************
    // yf: read tis_nn 
    irt = rpl_ssr(ffn_tisnn, sat, sen, reg);
    irt = rd_tis_nn(ffn_tisnn);
    if (irt == 0)    printf("Successfully read in %s\n", ffn_tisnn);
    printf("---------------------------------\n");
    //**************************************************************
    // yf: read tis_xgb 
    irt = rpl_ssr(ffn_tisxgb, sat, sen, reg);
    irt = rd_tis_xgb(ffn_tisxgb);
    if (irt == 0)    printf("Successfully read in %s\n", ffn_tisxgb);
    printf("---------------------------------\n");
    //**************************************************************
    
    // lut file
    // open lut file
    //irt = rd_snow_lut(ffn_lut);
    FILE    *fp_lut;
    int     mtc_l=46, mrwp_l=51, mde_l=45, npara_l=3, nch_l=6, nel_l;
    float   *****ss_lut, *ss_lut_p1;

    ss_lut_p1 = init_5dc_float(nch_l, npara_l, mde_l, mrwp_l, mtc_l, &ss_lut);

    irt = rpl_ssr(ffn_lut, sat, sen, reg);
    printf("ffn_lut: %s\n", ffn_lut);

    nel_l = mtc_l * mrwp_l * mde_l * npara_l * nch_l;

    fp_lut = openfs(ffn_lut);
    fread(ss_lut_p1, 4, nel_l, fp_lut);
    fclose(fp_lut);

    printf("nel_l=%d\n", nel_l);
    printf("%16.8e%16.8e%16.8e\n", ss_lut[4][0][0][2][0], 
            ss_lut[4][0][43][2][0], ss_lut[4][0][0][49][0]);
    printf("%16.8e%16.8e%16.8e\n", 
            5.21916672E-06, 0.89876878, 5.21916627E-06);
    printf("---------------------------------\n");
    //getchar();

    // init avn data
    avn_init(NAVN, a_nl, a_nlev);
    

    ///////////////////////////////////////////////////
    // dataset run
    ////////////////////////////////////////////////////
    if (strncmp(s_iftype, "dsr", 3) == 0)  {
        ifile = 0;
        if (strcmp(s_iftype, "dsr") == 0)
            dsr_run(ifile, sat, nscan, nfov0, nchan0, nfova, flag0, 
                    ajsec, aflag, aind, 
                    a_dn_pat, a_fn_pat, a_t0dt, a_l0, a_dl, a_nl, 
                    a_nlev, NAVN, a_nvar2, sfr_lim, ss_lut_p1,
                    flg_do_sdnn, flg_do_tbnn, flg_do_iwpnn, flg_do_tcnn,
                    flg_do_sfrnn, flg_do_xgb, 
                    s_flist0, s_flist1, o_dn_pat, o_fn_pat);
        else if (strcmp(s_iftype, "dsr1dvar0") == 0)
            dsr_1dvar0(ifile, sat, nscan, nfov0, nchan0, nfova, flag0, 
                    ajsec, aflag, aind, 
                    a_dn_pat, a_fn_pat, a_t0dt, a_l0, a_dl, a_nl, 
                    a_nlev, NAVN, a_nvar2, sfr_lim, ss_lut_p1,
                    flg_do_sdnn, flg_do_tbnn, flg_do_iwpnn, flg_do_tcnn,
                    flg_do_sfrnn, flg_do_xgb, 
                    s_flist0, s_flist1, o_dn_pat, o_fn_pat);
        else {
            printf("Dataset run %s not supported\n", s_iftype);
            printf("support s_iftype = dsr or dsr1dvar0\n");
        }
        exit(1);
    }
    /////////////////////////////////////


    // main loop
    ifile = 0;
    while (1)  {

        // read each file name
        cp = fgets(ffn_file0, SL9, fp_fl0);
        if (cp == NULL)    break;

        printf("#####################\n");
        printf("file index : %d\n", ifile);

        // read ffn_file0/1
        sl = strlen(ffn_file0);    ffn_file0[sl-1] = '\0';
        printf("ffn_file0: %s\n", ffn_file0);

        if (fp_fl1 == NULL)  {
            ffn_file1[0] = '\0';
        }
        else  {
            cp = fgets(ffn_file1, SL9, fp_fl1);
            if (cp != NULL)  {
                sl = strlen(ffn_file1);    ffn_file1[sl-1] = '\0';
                printf("ffn_file1: %s\n", ffn_file1);
            }
            else  {
                ffn_file1[0] = '\0';
            }
        }

        
        // read satellite data
        //printf("nfov0, nchan0=%d %d\n", nfov0, nchan0);
        sat_data(sat, sen, reg, ffn_file0, ffn_file1, s_iftype, 
                &nscan, &nfov, &nchan, &nfova, &nfova_p1, &flag0, &flag0_p1,  
                nyref, lese_tab);
        printf("nscan, nfov, nchan: %d %d %d\n", nscan, nfov, nchan);

        // init product vars
        prod_init(nscan, nfov, nchan, nfova);

        // Generate surface type
        mask_stype(nscan, nfov, nfova, mask_row, mask_col, mask_res, mask);
        

        ///////////////////////////////////////////////////
        // Generate SFR product
        // yf: add nn flags
        ////////////////////////////////////////////////////
        snowfall(ifile, sat, nscan, nfov, nchan, nfova, flag0, 
                a_name, ajsec, aflag, aind, 
                a_dn_pat, a_fn_pat, a_t0dt, a_l0, a_dl, a_nl, 
                a_nlev, NAVN, a_nvar2, sfr_lim, ss_lut_p1,
                flg_do_sdnn, flg_do_tbnn, flg_do_iwpnn, flg_do_tcnn,
                flg_do_sfrnn,flg_do_xgb);
        /////////////////////////////////////
        

        // set flag
        set_flag(nscan, nfov, flag0);

        // prod data output
        prod_output(sat, sen, reg, sat1, sen1, reg1, ffn_file0, ffn_file1, 
                o_fn_mode, o_dn_pat, o_fn_pat, ffn_out, 
                nscan, nfov, nchan, nfova, flag0, sfr_lim, nyref, lese_tab);

        // calibration output
        // yf: add nn flags
        if (strcmp(o_fn_mode, "sf01") == 0)  {
            cal_vars(ffn_out, sat, nscan, nfov, nchan, nfova, flag0, 
                    a_name, ajsec, aflag, aind, 
                    a_dn_pat, a_fn_pat, a_t0dt, a_l0, a_dl, a_nl, 
                    a_nlev, NAVN, a_nvar2, sfr_lim, ss_lut_p1,
                    flg_do_sdnn, flg_do_tbnn, flg_do_iwpnn, flg_do_tcnn, 
                    flg_do_sfrnn,flg_do_xgb);
        }
        
        // free sat and prod
        //printf("free sat and prod\n");
        sat_free(nscan, nfov, nchan, nfova);
        prod_free(nscan, nfov, nchan, nfova);

        // free other
        //printf("free nfova\n");
        irt = free_1dc_int (nfova, nfova_p1);
        //printf("free flag0\n");
        irt = free_2dc_int(nscan, flag0, flag0_p1);

        ifile++;
    }  // End of while loop

    irt = free_5dc_float(nch_l, npara_l, mde_l, mrwp_l, mtc_l, 
            ss_lut, ss_lut_p1);

    fclose(fp_fl0);
    if (fp_fl1 != NULL)     fclose(fp_fl1);

    printf("#####################\n");


    // free avn
    //printf("free avn\n");
    avn_free(NAVN, a_nl, a_nlev);

    // free para
    //printf("free parameters\n");
    for (i=0; i<nyref; i++)    free(lese_tab[i]);
    free(lese_tab);
    free_tis();
    irt = free_2dc_char(mask_row, mask, mask_p1);


    printf("\n\n");
    printf("##########################################\n");
    printf("# report\n");
    printf("##########################################\n");
    printf("Total             : %d files\n", ifile);
    printf("Input file format : %s\n", s_iftype);
    printf("Output file mode  : %s\n", o_fn_mode);
    printf("##########################################\n");
    printf("\n\n");


    return(0);
}




