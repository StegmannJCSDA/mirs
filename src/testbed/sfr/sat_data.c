/***************************************************************************
 *  Program Name      : sat_data.c
 *  Type              : Function subprogram
 *  Function          : Reads the MIRS radiometric measurement format
 *                      (e.g. FMSDR) data 
 *  Input Files       : Pointer to MIRS footprint-matched data file (FMSDR_*)
 *  Output Files      : None
 *  Output Data       : User defined structure containing entire contents 
 *                      of the FMSDR file
 *  Subroutine Called : sat_data(),  
 *  Called by         : process.c
 *************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "hdf5.h"

#include "d_sys.h"
#include "d_darrc.h"
#include "d_jsec.h"


int     sat_data_fmsdr();
int     sat_data_h5();
int     rd_mhs_l1b();
int     rd_mhs_l1b0();
int     rd_gmi();
int     rd_ssmis();
int     rd_mws2atms();


/************************************************************/
// sat data
extern  float       **lon, **lat, ***at, **lza, **sza;
extern  char        **obmd;
extern  long long   **btjs;
extern  int         dtse[2][6];
extern  float       *lon_p1, *lat_p1, *at_p1, *lza_p1, *sza_p1;
extern  char        *obmd_p1;
extern  long long   *btjs_p1;
// mirs need
extern  float       **dsec, *dsec_p1;

extern float        *hd_sqc, **ti_sqc;


/************************************************************/
int sat_data(char *sat, char *sen, char *reg, 
        char *ffn0, char *ffn1, char *s_iftype, 
        int *nscanp, int *nfovp, int *nchanp, int **nfovap, int **nfova_p1p,  
        int ***flag0p, int **flag0_p1p, int nyref, int **lese_tab)
{
    printf("s_iftype: %s\n", s_iftype);

    // based on format, read data
    if (strcmp(s_iftype, "fmsdr") == 0)  {
        //printf("%s format.\n", s_iftype);
        sat_data_fmsdr(ffn0, nscanp, nfovp, nchanp, nfovap, nfova_p1p, 
                flag0p, flag0_p1p, nyref, lese_tab);
    }
    else if (strcmp(s_iftype, "srs") == 0)  {
        //printf("%s format.\n", s_iftype);
        sat_data_fmsdr(ffn0, nscanp, nfovp, nchanp, nfovap, nfova_p1p, 
                flag0p, flag0_p1p, nyref, lese_tab);
    }
    else if (strcmp(s_iftype, "tatms") == 0)  {
        //printf("%s format.\n", s_iftype);
        sat_data_h5(ffn0, ffn1, s_iftype, nscanp, nfovp, nchanp, 
                nfovap, nfova_p1p, flag0p, flag0_p1p, nyref, lese_tab);
    }
    else if (strcmp(s_iftype, "satms") == 0)  {
        //printf("satms format\n");
        sat_data_h5(ffn0, ffn1, s_iftype, nscanp, nfovp, nchanp, 
                nfovap, nfova_p1p, flag0p, flag0_p1p, nyref, lese_tab);
    }
    else if (strcmp(s_iftype, "l1b0") == 0)  {
        //printf("l1b format\n");
        rd_mhs_l1b0(ffn0, ffn1, s_iftype, nscanp, nfovp, nchanp, 
                nfovap, nfova_p1p, flag0p, flag0_p1p, nyref, lese_tab);
    }
    else if (strcmp(s_iftype, "l1b") == 0)  {
        //printf("l1b format\n");
        rd_mhs_l1b(ffn0, ffn1, s_iftype, nscanp, nfovp, nchanp, 
                nfovap, nfova_p1p, flag0p, flag0_p1p, nyref, lese_tab);
    }
    else if (strcmp(s_iftype, "gmi") == 0)  {
        //printf("gmi format\n");
        rd_gmi(ffn0, s_iftype, nscanp, nfovp, nchanp, 
                nfovap, nfova_p1p, flag0p, flag0_p1p, nyref, lese_tab);
    }
    else if (strcmp(s_iftype, "ssmis") == 0)  {
        //printf("ssmis format\n");
        rd_ssmis(ffn0, s_iftype, nscanp, nfovp, nchanp, 
                nfovap, nfova_p1p, flag0p, flag0_p1p, nyref, lese_tab);
    }
    else if (strcmp(s_iftype, "mws2atms") == 0)  {
        //printf("mws format\n");
        rd_mws2atms(ffn0, s_iftype, nscanp, nfovp, nchanp, 
                nfovap, nfova_p1p, flag0p, flag0_p1p, nyref, lese_tab);
    }
    else  {
        printf("Cannot handle this format (%s), will add later\n.", s_iftype);
        exit(1);
    }

    return(0);
}



// read mirs data
int sat_data_fmsdr(char *ffn, int *nscanp, int *nfovp, int *nchanp, 
        int **nfovap, int **nfova_p1p, int ***flag0p, int **flag0_p1p, 
        int nyref, int **lese_tab) 
{
    FILE    *fp;

    char    dummy[4];
    char    char_nMeasurData[4], char_nchan[4], char_nPosScan[4],
            char_nScanLines[4], char_nqc[4];
    int     nMeasurData, nqc;
    int     nscan, nfov, nchan;

    int     *nfova, *nfova_p1;
    int     **flag0, *flag0_p1;

    int     iprf, ipos, iscan, ichan, i, j, tt;


    // ---------------------------------------------------
    // Read the header
    // ---------------------------------------------------/
    fp = fopen(ffn, "r");

    fread(&dummy, 4, 1, fp);
    fread(&char_nMeasurData, 4, 1, fp);
    fread(&dummy, 4, 1, fp);

    fread(&dummy, 4, 1, fp);
    fread(&char_nchan,     4, 1, fp);
    fread(&dummy, 4, 1, fp);

    fread(&dummy, 4, 1, fp);
    fread(&char_nPosScan, 4, 1, fp);
    fread(&char_nScanLines, 4, 1, fp);
    fread(&dummy, 4, 1, fp);

    fread(&dummy, 4, 1, fp);
    fread(&char_nqc,    4, 1, fp);
    fread(&dummy, 4, 1, fp);

    nMeasurData  = intSwap(char_nMeasurData);
    nchan        = intSwap(char_nchan);
    nfov         = intSwap(char_nPosScan);
    nscan        = intSwap(char_nScanLines);
    nqc          = intSwap(char_nqc);
    //printf("nMeasurData, nchan, nfov, nscan, nqc: %d, %d, %d, %d, %d\n", 
    //        nMeasurData, nchan, nfov, nscan, nqc);

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


    // content scalar identifiers
    char    char_lat[4], char_lon[4], char_RelAziAngle[4], char_SolZenAngle[4];
    char    char_Node[4], char_iscanPos[4], char_iscanLine[4],
            char_Year[4], char_julDay[4], char_secs[4];
    // vector identifies
    char    char_var[nchan*4], char_qc[nqc*4];
    //float   angle, secant_view;
    //int     qc[nscan][nfov][nchan];

    int             year1, doy1, sod1i;
    float           sod1f;
    long long int   pjsec;


    // read freq
    //printf("freq:\n");
    fread(&dummy, 4, 1, fp);
    fread(char_var, 4, nchan, fp);
    fread(&dummy, 4, 1, fp);
    for (ichan=0; ichan<nchan; ichan++)  {
        i = ichan*4;
        for (j=0; j<4; j++)    dummy[j] = char_var[i+j];
        //printf("%f ", floatSwap(dummy));
    }
    //printf("\n");

    // read polar
    //printf("polar:\n");
    fread(&dummy, 4, 1, fp);
    fread(char_var, 4, nchan, fp);
    fread(&dummy, 4, 1, fp);
    //for (ichan=0; ichan<nchan; ichan++)  {
    //    i = ichan*4;
    //    for (j=0; j<4; j++)    dummy[j] = char_var[i+j];
    //    printf("%d", intSwap(dummy));
    //}
    //printf("\n");


    // read data content by profiles
    //printf("read data by profiles\n");
    for (iprf=0; iprf<nMeasurData; iprf++) {
        // read lat, lon, RelAziAngle, SolZenAngle, Node,
        // iscanPos, icanLine, Year, julDay, secs
        fread(&dummy, 4, 1, fp);
        fread(&char_lat, 4, 1, fp);
        fread(&char_lon, 4, 1, fp);
        fread(&char_RelAziAngle, 4, 1, fp);
        fread(&char_SolZenAngle, 4, 1, fp);
        fread(&dummy, 4, 1, fp);

        fread(&dummy, 4, 1, fp);
        fread(&char_Node, 4, 1, fp);
        fread(&char_iscanPos, 4, 1, fp);
        fread(&char_iscanLine, 4, 1, fp);
        fread(&char_Year, 4, 1, fp);
        fread(&char_julDay, 4, 1, fp);
        fread(&char_secs, 4, 1, fp);
        fread(&dummy, 4, 1, fp);

        // !!! note Fortran starts from 1, while C starts from 0
        iscan = intSwap(char_iscanLine) - 1;
        ipos =  intSwap(char_iscanPos) - 1;
        //printf("iscan, ipos: %d, %d\n", iscan, ipos);

        lon[iscan][ipos] = floatSwap(char_lon);
        lat[iscan][ipos] = floatSwap(char_lat);
        sza[iscan][ipos]  = floatSwap(char_SolZenAngle);
        obmd[iscan][ipos] = intSwap(char_Node);
        //printf("lon, lat: %f, %f\n:", lon[iscan][ipos], lat[iscan][ipos]);

        flag0[iscan][ipos] = 0;
        if (lon[iscan][ipos]<-400 || lon[iscan][ipos]>400 
                || lat[iscan][ipos]<-100 || lat[iscan][ipos]>100)  {
            flag0[iscan][ipos] |= (1U<<9);
            //printf("lon, lat: %f, %f\n", lon[iscan][ipos], lat[iscan][ipos]);
        }


        // calculate BeamTime
        year1 = intSwap(char_Year);
        doy1  = intSwap(char_julDay);
        sod1f = floatSwap(char_secs);
        sod1i = sod1f + 0.5;
        
        cal2jsec(year1, 1, 1, 0, 0, 0, &pjsec);
        btjs[iscan][ipos] = pjsec + (doy1-1)*86400 + sod1i;
        dsec[iscan][ipos] = sod1f - sod1i;
        //printf("year, doy, sodf, sodi, dsec: %d, %d, %f, %d, %f\n", 
        //        year1, doy1, sod1f, sod1i, dsec[iscan][ipos]);

        if (btjs[iscan][ipos] < 0)  {
            //btjs[iscan][ipos] = -999;
            flag0[iscan][ipos] |= (1U<<8);
        }


        // read angle and calculate sect_view
        //printf("lza:\n");
        fread(&dummy, 4, 1, fp);
        fread(char_var, 4, nchan, fp);
        fread(&dummy, 4, 1, fp);
        //for (ichan=0; ichan<nchan; ichan++) {
        for (ichan=0; ichan<1; ichan++) {
            i = ichan * 4;
            for (j=0; j<4; j++)    dummy[j] = char_var[i+j];
            lza[iscan][ipos] = floatSwap(dummy);
            //printf("%f ", lza[iscan][ipos]);
            //secant_view = 1. / cos(deg2rad(angle));
        }
        //printf("\n");

        // read tb
        //printf("tb:\n");
        fread(&dummy, 4, 1, fp);
        fread(char_var, 4, nchan, fp);
        fread(&dummy, 4, 1, fp);
        for (ichan=0; ichan<nchan; ichan++) {
            i = ichan*4;
            for (j=0; j<4; j++)    dummy[j] = char_var[i+j];
            at[iscan][ipos][ichan] = floatSwap(dummy);
            tt = round(ti_sqc[0][ichan]);
            //printf("%f %d", at[iscan][ipos][ichan], tt);
            if (tt == 1)  {
                if (at[iscan][ipos][ichan]<50 || at[iscan][ipos][ichan]>500)  {
                    flag0[iscan][ipos] |= (1U<<10);
                    //printf(" N");
                }
                else  {
                    //printf(" Y");
                }
            }
            //printf("\n");
        }
        //getchar();

        // read qc
        //printf("qc:\n");
        if (nqc>0) {
            fread(&dummy, 4, 1, fp);
            fread(char_qc, 4, nqc, fp);
            fread(&dummy, 4, 1, fp);
            //for (iqc=0; iqc<nqc; iqc++) {
            //    i = iqc * 4;
            //    for (j=0; j<4; j++)    dummy[j] = char_qc[i+j];
            //    printf("%d ", intSwap(dummy));
            //}
            //printf("\n");
        }
        //getchar();

    }  // nMeasurement

    fclose(fp);

    // set begin/edn data/time
    jsec2cal(btjs[0][0], &dtse[0][0], &dtse[0][1], &dtse[0][2],
            &dtse[0][3], &dtse[0][4], &dtse[0][5]);
    jsec2cal(btjs[nscan-1][nfov-1], &dtse[1][0], &dtse[1][1], &dtse[1][2],
            &dtse[1][3], &dtse[1][4], &dtse[1][5]);

    return(0);
}



int sat_free(int nscan, int nfov, int nchan, int *nfova)
{
    int     irt, sirt;

    sirt = 0;
    irt = free_2dc_float(nscan, lon, lon_p1);
    sirt+=irt;
    irt = free_2dc_float(nscan, lat, lat_p1);
    sirt+=irt;
    irt = free_2dc_llint(nscan, btjs, btjs_p1);
    sirt+=irt;
    irt = free_3dc_float(nscan, nfov, at, at_p1);
    sirt+=irt;
    irt = free_2dc_float(nscan, lza, lza_p1);
    sirt+=irt;
    irt = free_2dc_float(nscan, sza, sza_p1);
    sirt+=irt;
    irt = free_2dc_char (nscan, obmd, obmd_p1);
    sirt+=irt;
    irt = free_2dc_float(nscan, dsec, dsec_p1);
    sirt+=irt;
    //printf("free sat: %d\n", sirt);

    return(0);
}



// read h5 data
int sat_data_h5(char *ffn0, char *ffn1, char *ftype, 
        int *nscanp, int *nfovp, int *nchanp, int **nfovap, int **nfova_p1p, 
        int ***flag0p, int **flag0_p1p, int nyref, int **lese_tab)
{
    int         status;
    int         ifov, iscan, ichan;
    int         nfov1, nscan1, nchan1, *nfova, *nfova_p1;

    int         **flag0, *flag0_p1;

    
    hid_t       file_id, root_id, all_data_id, gran_id;
    hid_t       dsetr_id, space_id, attr_id;

    //int         ndims;
    //hsize_t     dims[2],    maxdims[2];
    hsize_t     dim3[3], maxdim3[3];

    float           factors[2];
    unsigned char   tuc1d[1];


    //printf("ffn0=%s\n", ffn0);
    //printf("ffn1=%s\n", ffn1);

    /////////////////////////////////////////////////////////           
    // Open SDR file (contains antenna temperature)
    /////////////////////////////////////////////////////////           
    file_id = H5Fopen(ffn0, H5F_ACC_RDONLY, H5P_DEFAULT);

    //printf("file_id=%d\n", file_id);
    root_id = H5Gopen1(file_id, "/");
    all_data_id = H5Gopen1(root_id, "All_Data");
    //printf("all_data_id=%d\n", all_data_id);


    // ScaleFactors         32-bit floating-point,     2 x 1
    if (strcmp(ftype, "satms") == 0)  {
        //printf("satms, BrightnessTemperatureFactors\n");
        gran_id = H5Gopen1(all_data_id, "ATMS-SDR_All");
        dsetr_id = H5Dopen1(gran_id, "BrightnessTemperatureFactors");
    }
    else if (strcmp(ftype, "tatms") == 0)  {
        //printf("tatms, AntennaTemperatureFactors\n");
        gran_id   = H5Gopen1(all_data_id, "ATMS-TDR_All");
        dsetr_id = H5Dopen1(gran_id, "AntennaTemperatureFactors");
    }
    else  {
        printf("Wrong h5 format (satms|tatms): %s.\n", ftype);
        exit(1);
    }
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, factors);
    H5Dclose(dsetr_id);
    //printf("ftype, factors[*]: %s, %f, %f\n", ftype, factors[0], factors[1]);


    // AntennaTemperature   16-bit unsigned integer,    12 x 96 x 22
    if (strcmp(ftype, "satms") == 0)  {
        //gran_id = H5Gopen1(all_data_id, "ATMS-SDR_All");
        dsetr_id = H5Dopen1(gran_id, "BrightnessTemperature");
    }
    else if (strcmp(ftype, "tatms") == 0)  {
        //gran_id   = H5Gopen1(all_data_id, "ATMS-TDR_All");
        dsetr_id = H5Dopen1(gran_id, "AntennaTemperature");
    }
    else  {
        printf("Wrong h5 format (satms|tatms): %s.\n", ftype);
        exit(1);
    }
    //printf("next dim\n");

    // get space id, and dim
    space_id = H5Dget_space(dsetr_id);
    status = H5Sget_simple_extent_dims(space_id, dim3, maxdim3);
    nscan1 = dim3[0];    nfov1 = dim3[1];    nchan1 = dim3[2];
    //printf("nscan1, nfov1, nchan1, maxdim3[*]: %d, %d, %d, %d, %d, %d\n", 
    //        nscan1, nfov1, nchan1, 
    //        maxdim3[0], maxdim3[1], maxdim3[2]);

    // nfov for each chan
    nfova_p1 = init_1dc_int(nchan1, &nfova);
    for (ichan=0; ichan<nchan1; ichan++)    nfova[ichan] = nfov1;


    flag0_p1= init_2dc_int (nscan1, nfov1, &flag0);
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)
            flag0[iscan][ifov] = 0;
    }

    
    // temp vars, need dims from above
    unsigned short int  tusi3d[nscan1*nfov1*nchan1];
    long long int       tlli2d[nscan1*nfov1];
    float               tf2d[nscan1*nfov1];
    
    // read antenna temperature: 16-bit unsigned integer
    at_p1 = init_3dc_float(nscan1, nfov1, nchan1, &at);
    status = H5Dread(dsetr_id, H5T_NATIVE_UINT16, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, tusi3d);
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)  {
            for (ichan=0; ichan<nchan1; ichan++)  {
                at[iscan][ifov][ichan] = tusi3d[iscan*nfov1*nchan1 
                    + ifov*nchan1 + ichan] * factors[0] + factors[1];
                if (at[iscan][ifov][ichan]<50 || at[iscan][ifov][ichan]>330.01)
                    flag0[iscan][ifov] |= (1U<<10);
            }
        }
    }
    H5Dclose(dsetr_id);
    //printf("at\n");
    //for (iscan=0; iscan<1; iscan++)  {
    //    for (ifov=0; ifov<4; ifov++)
    //        printf("%f ", at[iscan][ifov][0]);
    //    printf("\n");
    //}

    //  BeamTime        64-bit integer,    12 x 96
    btjs_p1 = init_2dc_llint(nscan1, nfov1, &btjs);
    dsec_p1 = init_2dc_float(nscan1, nfov1, &dsec);
    dsetr_id = H5Dopen1(gran_id, "BeamTime");
    status = H5Dread(dsetr_id, H5T_NATIVE_LLONG, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, tlli2d);
    double  dd=1000000.0, tt;
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)  {
            btjs[iscan][ifov] = tlli2d[iscan*nfov1+ifov] / 1000000;
            tt = tlli2d[iscan*nfov1+ifov] / dd;
            dsec[iscan][ifov] = tt - btjs[iscan][ifov];
        }
    }
    //printf("BeamTime\n");
    //for (iscan=0; iscan<3; iscan++)  {
    //    for (ifov=0; ifov<4; ifov++)
    //        printf("%lld ", btjs[iscan][ifov]);
    //    printf("\n");
    //}
    //for (iscan=0; iscan<3; iscan++)  {
    //    for (ifov=0; ifov<4; ifov++)
    //        printf("%lf ", dsec[iscan][ifov]);
    //    printf("\n");
    //}
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)
            if (btjs[iscan][ifov] < 0)  {
                btjs[iscan][ifov] = -999;
                flag0[iscan][ifov] |= (1U<<8);
            }
    }


    //long long int   jsec;
    //int     year1, mon1, day1, hour1, min1, sec1;
    //for (iscan=0; iscan<3; iscan++)  {
    //    for (ifov=0; ifov<1; ifov++)  {
    //        tsec2cal(btjs[iscan][ifov], &year1, &mon1, &day1, 
    //                &hour1, &min1, &sec1, nyref, lese_tab);
    //        tsec2jsec(btjs[iscan][ifov], &jsec, nyref, lese_tab);
    //        printf("%d %d  %lld %lld %lld ", iscan, ifov, 
    //                btjs[iscan][ifov], jsec, btjs[iscan][ifov]-jsec);
    //        printf("%04d %02d %02d %02d %02d %02d\n", 
    //                year1, mon1, day1, hour1, min1, sec1);
    //    }
    //}
    // tai to jul sec
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++) 
            tsec2jsec(btjs[iscan][ifov], &btjs[iscan][ifov], nyref, lese_tab);
    }
    //for (iscan=0; iscan<3; iscan++)  {
    //    for (ifov=0; ifov<1; ifov++)  {
    //        jsec2cal(btjs[iscan][ifov], &year1, &mon1, &day1, 
    //                &hour1, &min1, &sec1);
    //        printf("%d %d  %lld ", iscan, ifov, btjs[iscan][ifov]);
    //        printf("%04d %02d %02d %02d %02d %02d\n", 
    //                year1, mon1, day1, hour1, min1, sec1);
    //    }
    //}
    H5Dclose(dsetr_id);
    
    // Close group and file
    H5Gclose(gran_id);
    H5Gclose(all_data_id);
    H5Gclose(root_id);
    H5Fclose(file_id);
    //printf("file_id=%d\n", file_id);


    ///////////////////////////////////////////////////////////////
    // Open GEO file (contains meta data, lat/lon/zenith angle, etc)
    ///////////////////////////////////////////////////////////////
    //printf("GEO file\n");
    file_id     = H5Fopen(ffn1, H5F_ACC_RDONLY, H5P_DEFAULT);
    //printf("file_id=%d\n", file_id);

    root_id     = H5Gopen1(file_id, "/");
    all_data_id = H5Gopen1(root_id, "All_Data");
    gran_id     = H5Gopen1(all_data_id, "ATMS-SDR-GEO_All");


    // Longitude         32-bit floating-point
    lon_p1 = init_2dc_float(nscan1, nfov1, &lon);
    dsetr_id = H5Dopen1(gran_id, "Longitude");
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, tf2d);
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)
            lon[iscan][ifov] = tf2d[iscan*nfov1+ifov];
    }
    H5Dclose(dsetr_id);
    //printf("Longitude %p\n", lon);
    //for (iscan=0; iscan<1; iscan++)  {
    //    for (ifov=0; ifov<4; ifov++)
    //        printf("%f ", lon[iscan][ifov]);
    //    printf("\n");
    //}


    // Latitude        32-bit floating-point
    lat_p1 = init_2dc_float(nscan1, nfov1, &lat);
    dsetr_id = H5Dopen1(gran_id, "Latitude");
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, tf2d);
    for(iscan=0; iscan<nscan1; iscan++ ) {
        for(ifov=0; ifov<nfov1; ifov++ )
            lat[iscan][ifov] = tf2d[iscan*nfov1+ifov];
    }
    H5Dclose(dsetr_id);
    //printf("Latitude %p\n", lon);
    //for (iscan=0; iscan<3; iscan++)  {
    //    for (ifov=0; ifov<4; ifov++)
    //        printf("%f ", lat[iscan][ifov]);
    //    printf("\n");
    //}


    // check
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)  {
            if (lon[iscan][ifov]<-400 || lon[iscan][ifov]>400 
                    || lat[iscan][ifov]<-100 || lat[iscan][ifov]>100)  {
                flag0[iscan][ifov] |= (1U<<9);
                //printf("lon, lat: %f, %f\n:", lon[iscan][ipos], 
                //      lat[iscan][ipos]);
            }
        }
    }
    

    // SensorZenithAngle        32-bit floating-point
    lza_p1 = init_2dc_float(nscan1, nfov1, &lza);
    dsetr_id = H5Dopen1(gran_id, "SatelliteZenithAngle");
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, tf2d);
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)
            lza[iscan][ifov] = tf2d[iscan*nfov1+ifov];
    }
    H5Dclose(dsetr_id);


    // SolarZenithAngle         32-bit floating-point
    sza_p1 = init_2dc_float(nscan1, nfov1, &sza);
    dsetr_id = H5Dopen1(gran_id, "SolarZenithAngle");
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, tf2d);
    for (iscan=0; iscan<nscan1; iscan++) {
        for (ifov=0; ifov<nfov1; ifov++)
            sza[iscan][ifov] = tf2d[iscan*nfov1+ifov];
    }
    H5Dclose(dsetr_id);

    // close group
    H5Gclose(gran_id) ;
    H5Gclose(all_data_id) ;


    // ATMS-SDR-GEO_Gran_0      Unsigned Char,    1 x 1
    all_data_id = H5Gopen1(root_id, "Data_Products");
    gran_id     = H5Gopen1(all_data_id, "ATMS-SDR-GEO");
    dsetr_id = H5Dopen1(gran_id, "ATMS-SDR-GEO_Gran_0");

    // orbit mode
    attr_id = H5Aopen(dsetr_id,"Ascending/Descending_Indicator",H5P_DEFAULT);
    status = H5Aread(attr_id, H5T_NATIVE_UCHAR, tuc1d);
    obmd_p1 = init_2dc_char(nscan1, nfov1, &obmd);
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)
            obmd[iscan][ifov] = tuc1d[0];
    }
    H5Aclose(attr_id);


    // beginning/ending time
    hid_t   atype, atype_mem;
    char    buf[32];
    int     tn;

    attr_id = H5Aopen(dsetr_id, "Beginning_Date", H5P_DEFAULT);
    atype = H5Aget_type(attr_id);
    atype_mem = H5Tget_native_type(atype, H5T_DIR_ASCEND);
    status = H5Aread(attr_id, atype_mem, buf);
    H5Aclose(attr_id);
    tn = atoi(buf);
    //printf("%s, %d\n", buf, tn);
    dtse[0][0] = tn / 10000;
    dtse[0][1] = tn % 10000 / 100;
    dtse[0][2] = tn % 100;

    attr_id = H5Aopen(dsetr_id, "Beginning_Time", H5P_DEFAULT);
    status = H5Aread(attr_id, atype_mem, buf);
    H5Aclose(attr_id);
    tn = atoi(buf);
    //printf("%s, %d\n", buf, tn);
    dtse[0][3] = tn / 10000;
    dtse[0][4] = tn % 10000 / 100;
    dtse[0][5] = tn % 100;

    attr_id = H5Aopen(dsetr_id, "Ending_Date", H5P_DEFAULT);
    atype = H5Aget_type(attr_id);
    atype_mem = H5Tget_native_type(atype, H5T_DIR_ASCEND);
    status = H5Aread(attr_id, atype_mem, buf);
    H5Aclose(attr_id);
    tn = atoi(buf);
    //printf("%s, %d\n", buf, tn);
    dtse[1][0] = tn / 10000;
    dtse[1][1] = tn % 10000 / 100;
    dtse[1][2] = tn % 100;

    attr_id = H5Aopen(dsetr_id, "Ending_Time", H5P_DEFAULT);
    status = H5Aread(attr_id, atype_mem, buf);
    H5Aclose(attr_id);
    tn = atoi(buf);
    //printf("%s, %d\n", buf, tn);
    dtse[1][3] = tn / 10000;
    dtse[1][4] = tn % 10000 / 100;
    dtse[1][5] = tn % 100;

    printf("dtse: %04d %02d %02d %02d %02d %02d", dtse[0][0], dtse[0][1], 
            dtse[0][2], dtse[0][3], dtse[0][4], dtse[0][5]);
    printf(" -- %04d %02d %02d %02d %02d %02d\n", dtse[1][0], dtse[1][1], 
            dtse[1][2], dtse[1][3], dtse[1][4], dtse[1][5]);

    // close group
    H5Dclose(dsetr_id);
    H5Gclose(gran_id);
    H5Gclose(all_data_id);


    //close file
    H5Gclose(root_id);
    H5Fclose(file_id);
    //printf("file_id=%d\n", file_id);


    /////////////////////////////////////////////////////////           
    // set return variables
    /////////////////////////////////////////////////////////
    *nfovp = nfov1;     *nscanp = nscan1;    *nchanp = nchan1;
    *nfovap = nfova;    *nfova_p1p = nfova_p1;
    *flag0p = flag0;    *flag0_p1p = flag0_p1;
    
    return(0);
}



///////////////////////////////////////////////////////////////
// read mws data
///////////////////////////////////////////////////////////////
int rd_mws2atms(char *ffn0, char *ftype,
        int *nscanp, int *nfovp, int *nchanp, int **nfovap, int **nfova_p1p,
        int ***flag0p, int **flag0_p1p, int nyref, int **lese_tab)
{
    printf("in rd_mws\n");

    int     i, ndims, status;
    int     storage_size, size_bytes;

    int     ifov, iscan, ichan;
    int     nfov1, nscan1, nchan1, *nfova, *nfova_p1;
    int     nfov0, nscan0, nchan0;

    int     **flag0, *flag0_p1;

    unsigned long int    index;

    hid_t       dsetr_id, space_id, type_id;
    hssize_t    points;
    hsize_t     dims2[2], maxdims2[2];
    hsize_t     dims3[3], maxdims3[3];


    ////////////////////////////////////
    // mws file
    ////////////////////////////////////
    hid_t file_id    = H5Fopen(ffn0, H5F_ACC_RDONLY,  H5P_DEFAULT);
    hid_t root_id    = H5Gopen1(file_id, "/");
    hid_t data_id    = H5Gopen1(root_id, "data");
    hid_t gran_id    = H5Gopen1(data_id, "navigation_data");

    ///////////////////////////////////////////////////           
    //   brightness temperature  32-bit integer, 
    ///////////////////////////////////////////////////           
    // Open and get the id for the antenna temperature
    gran_id = H5Gopen1(data_id, "calibration_data");
    dsetr_id = H5Dopen1(gran_id, "mws_toa_brightness_temperature");
    // get space id for antenna temperature
    space_id = H5Dget_space( dsetr_id );
    // data type id
    type_id = H5Dget_type( dsetr_id );

    // number of dimensions
    ndims = H5Sget_simple_extent_ndims(space_id);
    status = H5Sget_simple_extent_dims(space_id, dims3, maxdims3);
    printf("ndims3: %d\n", ndims);
    printf("dims3={%lu %lu %lu}; maxdims3={%lu %lu %lu}\n", (long int)dims3[0], 
            (long int)dims3[1], (long int)dims3[2], (long int)maxdims3[0], 
            (long int)maxdims3[1], (long int)maxdims3[2]);
    
    // get storage size
    points = H5Sget_simple_extent_npoints(space_id);
    type_id = H5Dget_type(dsetr_id);
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    //printf("points, size_types, storage: %d, %d, %d\n", points, size_bytes, 
    //        storage_size);


    // init sat var
    nscan0 = dims3[0];
    nfov0  = dims3[1];  //95
    nchan0 = dims3[2];  //24

    nscan1 = dims3[0];
    nfov1  = 96;
    nchan1 = 22;

    lon_p1  = init_2dc_float(nscan1, nfov1, &lon);
    lat_p1  = init_2dc_float(nscan1, nfov1, &lat);
    btjs_p1 = init_2dc_llint(nscan1, nfov1, &btjs);
    dsec_p1 = init_2dc_float(nscan1, nfov1, &dsec);
    obmd_p1 = init_2dc_char (nscan1, nfov1, &obmd);
    lza_p1  = init_2dc_float(nscan1, nfov1, &lza);
    sza_p1  = init_2dc_float(nscan1, nfov1, &sza);
    at_p1   = init_3dc_float(nscan1, nfov1, nchan1, &at);

    flag0_p1= init_2dc_int (nscan1, nfov1, &flag0);

    nfova_p1 = init_1dc_int(nchan1, &nfova);
    for (i=0; i<nchan1; i++)    nfova[i] = nfov1;

    float   *out_float_p1, *out_float;
    out_float_p1 = init_1dc_float(nscan0*nfov0*nchan0, &out_float);


    // read antenna temperature: 16-bit unsigned integer
    status = H5Dread(dsetr_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, out_float);

    for(iscan=0; iscan<nscan1; iscan++)  {
        for(ifov=0; ifov<nfov1; ifov++)  {
            for(ichan=0; ichan<nchan1; ichan++)  {
                index = iscan*nfov1*nchan1 + ifov*nchan1 + ichan;
                at[iscan][ifov][ichan] = out_float[index];
            }
        }
    }
    for (iscan=0; iscan<=nscan1-1; iscan=iscan+nscan1-1)  {
        printf("at ch %d\n", ichan);
        for (iscan=0; iscan<5; iscan++)  {
            for (ifov=0; ifov<5; ifov++)
                printf(" %f", at[iscan][ifov][ichan]);
            printf("\n");
        }
        getchar();
    }
    exit(1);

    // scale
    int     contms = 0;
    float   scale = 0.001;
    for(iscan=0; iscan<nscan1; iscan++ ) {
        for(ifov=0;  ifov<nfov1;   ifov++  ) {
            for(ichan=0; ichan<nchan1; ichan++ ) {
                //index = iscan * NFOV * NCHAN + ifov * NCHAN + ichan;
                if (at[iscan][ifov][ichan]>50000 
                        &&  at[iscan][ifov][ichan]<350000)  {
                    at[iscan][ifov][ichan] = at[iscan][ifov][ichan] * scale;
                }
                else  {
                    contms++;
                }
            }
        }
    }
    printf("contms: %d\n", contms);

    // will add qc later

    //H5Dclose(dsetr_id);
    //H5Gclose(gran_id);


    ////////////////////////////////////
    // lat
    ////////////////////////////////////
    dsetr_id = H5Dopen1(gran_id, "mws_lat");
    // get space id for antenna temperature
    space_id = H5Dget_space( dsetr_id );
    // data type id
    type_id = H5Dget_type( dsetr_id );
    // number of dimensions
    ndims = H5Sget_simple_extent_ndims(space_id);
    status = H5Sget_simple_extent_dims(space_id, dims2, maxdims2);
    printf("ndims: %d\n", ndims);
    printf("dims2={%lu %lu}; maxdims2={%lu %lu}\n", (long int)dims2[0], 
            (long int)dims2[1], (long int)maxdims2[0], (long int)maxdims2[1]);

    H5Dclose(dsetr_id);


    // close file
    H5Gclose(data_id) ;
    H5Gclose(root_id) ;
    H5Fclose(file_id) ;
    
    exit(1);

    return(0);
}
 







