/***********************************************************************
 *  Program Name      : prod_output.c
 *  Type              : Subroutines
 *  Function          : creates product output files
 *  Input Files       : None
 *  Output Files      : sfr output files
 *  Subroutine Called : None
 *  Called by         : proceed.c
 **************************************************************/

#include "stdio.h"
#include "math.h"
#include "ctype.h"
#include "libgen.h"

#include "d_jsec.h"
#include "d_darrc.h"
#include "d_sys.h"

#include "hdf.h"
#include "HdfEosDef.h"

#define SL5 32
#define SL9 512


int         rpl_dttm();
int         rpl_ssr();
int         rpl_ssr1();
long int    swath_def();
long int    swath_wrt();
int         wrt_fmbin();
int         wrt_he4();
int         fname_pat();

int         wrt_sf0();
int         wrt_sf1();
int         wrt_sf01();


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


// product
extern  int         **stype, **sd, **flag;
extern  float       **sfr, **prob, **iwp, **de;
extern  int         *stype_p1, *sd_p1, *flag_p1;
extern  float       *sfr_p1, *prob_p1, *iwp_p1, *de_p1;



int prod_output(char *sat, char *sen, char *reg, char *sat1, char *sen1, 
        char *reg1, char *ffn_file0, char *ffn_file1, 
        char *o_fn_mode, char *o_dn_pat, char *o_fn_pat, char *ffn_out, 
        int nscan, int nfov, int nchan, int *nfova, int **flag0, 
        float *sfr_lim, int nyref, int **lese_tab)
{
    char    tdn[SL9], tfn[SL9], *pfn, *ipl;
    char    fnmod[SL5], fcfmt[SL5];

    int     len;


    // Create folder
    strncpy(tdn, o_dn_pat, SL9);
    rpl_ssr(tdn, sat, sen, reg);
    rpl_ssr1(tdn, sat1, sen1, reg1);
    rpl_dttm(tdn, dtse, 1);
    // create if not exist; working for sub/sub dir;
    mkdir_p(tdn);


    // decide fn mode and file context format
    ipl = strchr(o_fn_mode, '_');
    if (ipl == NULL)  {
        // three preset fn/fmt mode
        if (strcmp(o_fn_mode, "fmbin") == 0)  {
            strncpy(fnmod, "fmbin", SL5);
            strncpy(fcfmt, "fmbin", SL5);
        }
        else if (strcmp(o_fn_mode, "std") == 0)  {
            strncpy(fnmod, "std", SL5);
            strncpy(fcfmt, "he4", SL5);
        }
        else if (strcmp(o_fn_mode, "pat") == 0)  {
            strncpy(fnmod, "pat", SL5);
            strncpy(fcfmt, "he4", SL5);
        }
        else if (strcmp(o_fn_mode, "sf0") == 0)  {
            strncpy(fnmod, "pat", SL5);
            strncpy(fcfmt, "sf0", SL5);
        }
        else if (strcmp(o_fn_mode, "sf01") == 0)  {
            strncpy(fnmod, "pat", SL5);
            strncpy(fcfmt, "sf0", SL5);
        }
        else  {
            printf("%s: o_fn_mode not supported.\n", o_fn_mode);
            printf("Check configure file\n");
            exit(1);
        }
    }
    else  {
        // combination setting "xxx_xxx", such as "pat_he4"
        len = strlen(o_fn_mode);
        //printf("len, ipl: %d, %ld\n", len, ipl-o_fn_mode);
        strncpy(fnmod, o_fn_mode, SL5);
        fnmod[ipl-o_fn_mode] = '\0';
        strncpy(fcfmt, ipl+1, SL5);
    }
    printf("fnmod, fcfmt: %s, %s\n", fnmod, fcfmt);


    // generate file name
    pfn = basename(ffn_file0);
    strncpy(tfn, pfn, SL9);
    if (strcmp(fnmod, "fmbin") == 0)  {
        // fmbin format
        strrepa(tfn, "FMSDR", "SFR");
    }
    else if (strcmp(fnmod, "std") == 0)  {
        // add prefix "SFR_"
        strncpy(tfn, "SFR_", 4);
        strncat(tfn, pfn, strlen(pfn));
    }
    else if (strcmp(fnmod, "pat") == 0)  {
        // patten mode based on reg/sat/sen and time
        fname_pat(o_fn_pat, tfn, sat, sen, reg, sat1, sen1, reg1, dtse, 
                nyref, lese_tab); 
    }
    else {
        printf("Error: fnmod: %s: not supported.\n", fnmod);
        printf("Check configure file field: o_fn_mode\n");
        exit(1);
    }
    strncpy(ffn_out, tdn, SL9);
    strncat(ffn_out, "/", 1);
    strncat(ffn_out, tfn, strlen(tfn));
    printf("ffn_out: %s\n", ffn_out);
    //getchar();


    // write to file
    if (strcmp(fcfmt, "fmbin") == 0)  {
        wrt_fmbin(ffn_out, nscan, nfov, flag0);
    }
    else if (strcmp(fcfmt, "sf0") == 0)  {
        wrt_sf0(ffn_out, nscan, nfov, flag0);
    }
    else if (strcmp(fcfmt, "he4") == 0)  {
        wrt_he4(ffn_out, sat, sen, reg, sat1, sen1, reg1, 
                o_dn_pat, o_fn_pat, ffn_file0, ffn_file1, 
                nscan, nfov, nchan, nfova, flag0, sfr_lim, nyref, lese_tab);
    }
    else {
        printf("Error: fcfmt: %s: not supported.\n", fcfmt);
        printf("Check configure file field: o_fn_mode\n");
        exit(1);
    }

    return(0);
}



int fname_pat(char *o_fn_pat, char *ofn, char *sat, char *sen, char *reg, 
        char *sat1, char *sen1, char *reg1, int **dtse, 
        int nyref, int **lese_tab)
{
    // set sat, sat, reg
    strncpy(ofn, o_fn_pat, SL9);
    rpl_ssr(ofn, sat, sen, reg);
    rpl_ssr1(ofn, sat1, sen1, reg1);


    // set date time
    rpl_dttm(ofn, dtse, 2);
    //printf("ofn: |%s|\n", ofn);

    return(0);
}



int wrt_sf0(char *ffn, int nscan, int nfov, int **flag0)
{
    FILE    *fp;
    char    ffn1[SL9];
    int     ne;

    strncpy(ffn1, ffn, SL9);
    strrepa(ffn1, "SFR_", "SF0_");
    strrepa(ffn1, ".hdf", ".bin");
    printf("sf0_out: %s\n", ffn1);
    
    fp = fopen(ffn1, "wb");
    if (fp == NULL)  {
        printf("Fail to open output file %s\n", ffn1);
        exit(1);
    }
    //printf("Sucessfully open output file: %s\n", ffn);

    ne = nscan * nfov;
    fwrite(btjs_p1,  sizeof(btjs_p1[0]),  ne, fp);
    fwrite(lon_p1,   sizeof(lon_p1[0]),   ne, fp);
    fwrite(lat_p1,   sizeof(lat_p1[0]),   ne, fp);
    fwrite(obmd_p1,  sizeof(obmd_p1[0]),  ne, fp);
    fwrite(stype_p1, sizeof(stype_p1[0]), ne, fp);
    fwrite(lza_p1,   sizeof(lza_p1[0]),   ne, fp);
    fwrite(sza_p1,   sizeof(sza_p1[0]),   ne, fp);
    fwrite(sfr_p1,   sizeof(sfr_p1[0]),   ne, fp);
    fwrite(flag_p1,  sizeof(flag_p1[0]),  ne, fp);
    fwrite(sd_p1,    sizeof(sd_p1[0]),    ne, fp);
    fwrite(iwp_p1,   sizeof(iwp_p1[0]),   ne, fp);
    fwrite(de_p1,    sizeof(de_p1[0]),    ne, fp);
    fwrite(prob_p1,  sizeof(prob_p1[0]),  ne, fp);
    //printf("done %s\n", fn);
    //getchar();

    fclose(fp);

    return(0);
}



int wrt_fmbin(char *ffn, int nscan, int nfov, int **flag0)
{
    FILE        *fp;
    int         iscan, ifov;
    int         year, mon, day, hour, min, sec, doy;

    float       sod, blon, blat, bsfr, bprob;
    int         bflag;

    fp = fopen(ffn, "wb");
    if (fp == NULL)  {
        printf("Fail to open output file %s\n", ffn);
        exit(1);
    }

    for (iscan = 0; iscan < nscan; iscan++)  {
        for(ifov = 0; ifov < nfov; ifov++)  {
            //printf("iscan, ifov, btjs: %d/%d, %d/%d, %lld\n", 
            //        iscan, nscan, ifov, nfov, btjs[iscan][ifov]);
            
            jsec2cal(btjs[iscan][ifov], &year, &mon, &day, &hour, &min, &sec);
            cal2doy(year, mon, day, &doy);
            sod = hour*3600 + min*60 + sec + dsec[iscan][ifov];
            //printf("dttm: %d, %d, %d, %d, %d, %d, %f\n", 
            //        year, mon, day, hour, min, sec, sod);

            blon  = lon[iscan][ifov];
            blat  = lat[iscan][ifov];
            bsfr  = sfr[iscan][ifov];
            bprob = prob[iscan][ifov];
            bflag = flag0[iscan][ifov];
            if (bprob > 0)      bprob = bprob * 100;

            if (bsfr < 0)  {
                bsfr = -999;
                bprob = -999;
            }
            
            fwrite(&doy,   sizeof(doy),   1, fp);
            fwrite(&sod,   sizeof(sod),   1, fp);
            fwrite(&blat,  sizeof(blat),  1, fp);
            fwrite(&blon,  sizeof(blon),  1, fp);
            fwrite(&bsfr,  sizeof(bsfr),  1, fp);
            fwrite(&bprob, sizeof(bprob), 1, fp);
            fwrite(&bflag, sizeof(bflag), 1, fp);
        }
    }

    fclose(fp);

    return(0);
}



int wrt_he4(char *ffn_out, char *sat, char *sen, char *reg, 
        char *sat1, char *sen1, char *reg1, char *fdn_pat, char *fn_pat, 
        char *ffn0, char *ffn1, int nscan, int nfov, int nchan, int *nfova, 
        int **flag0, float *sfr_lim, int nyref, int **lese_tab)
{
    char    swname[SL5];
    int     i;

    strncpy(swname, sen, SL5);
    for (i=0; i<strlen(swname); i++)
        swname[i] = toupper(swname[i]);
    strcat(swname, "_Swath");
    //printf("%s\n", swname);

    swath_def(ffn_out, swname, nscan, nfov, nchan, nfova);
    swath_wrt(ffn_out, swname, nscan, nfov, nchan, nfova, 
            sfr_lim, nyref, lese_tab);

    return(0);
}



long int swath_def(char *ffn, char *swname, int nscan, int nfov, int nchan, 
        int *nfova)
{ 
    char        chan_name[SL9];
    long int    swhdf_id, sw_id;
    long int    status, results = 0;
    int         compparm = 5, i;

    /*--------------------------------------------------------*
      Create HDF-EOS swath file
     *--------------------------------------------------------*/
    swhdf_id = SWopen(ffn, DFACC_CREATE);
    //printf ("sw_def/swath hdfeos id %ld\n", swhdf_id);

    /*--------------------------------------------------------*
      Create the first swath
     *--------------------------------------------------------*/
    sw_id = SWcreate(swhdf_id, swname);
    //printf ("sw_def/swath_id %ld\n", sw_id);

    /*---------------------------------------------------------*
     * Define swath dimensions for the data fields
     *---------------------------------------------------------*/
    status = SWdefdim(sw_id, "Scanline", nscan);
    results += status;
    //printf ("def_dim/define dim Scanline  %ld\n", status);
    status = SWdefdim(sw_id, "Field_of_view", nfov);
    results += status;
    //printf ("def_dim/define dim FielsOfView  %ld\n", status);

    /*---------------------------------------------------------*
     * Define swath dimensions for the geolocation fields
     *---------------------------------------------------------*/
    status = SWdefdim(sw_id, "Position1", nscan);
    results += status;
    //printf ("def_dim/define dim Position1  %ld\n", status);
    status = SWdefdim(sw_id, "Position2", nfov);
    results += status;
    //printf ("def_dim/define dim Position2  %ld\n", status);

    /*---------------------------------------------------------*
     * Establish mapping between data and geolocation fields
     *---------------------------------------------------------*/
    status = SWdefdimmap(sw_id, "Position1", "Scanline", 0,1);
    results += status;
    //printf ("def_dim/define map1 %ld\n", status);
    status = SWdefdimmap(sw_id, "Position2", "Field_of_view", 0,1);
    results += status;
    //printf ("def_dim/define map2 %ld\n", status);

    /*---------------------------------------------------*
     * Define the geolocation fields: scan time fields
     *---------------------------------------------------*/
    status = SWdefgeofield(sw_id, "ScanTime_year", "Position1",
            DFNT_INT16, HDFE_NOMERGE);
    //printf ("def_geo/define geofield ScanTime_year %ld\n", status);
    results = results + status;

    status = SWdefgeofield(sw_id, "ScanTime_month", "Position1",
            DFNT_INT8, HDFE_NOMERGE);
    //printf ("def_geo/define geofield ScanTime_month %ld\n", status);
    results = results + status;

    status = SWdefgeofield(sw_id, "ScanTime_dom", "Position1",
            DFNT_INT8, HDFE_NOMERGE);
    //printf ("def_geo/define geofield ScanTime_dom %ld\n", status);
    results = results + status;

    status = SWdefgeofield(sw_id, "ScanTime_hour", "Position1",
            DFNT_INT8, HDFE_NOMERGE);
    //printf ("def_geo/define geofield ScanTime_hour %ld\n", status);
    results = results + status;

    status = SWdefgeofield(sw_id, "ScanTime_minute", "Position1",
            DFNT_INT8, HDFE_NOMERGE);
    //printf ("def_geo/define geofield ScanTime_minute %ld\n", status);
    results = results + status;

    status = SWdefgeofield(sw_id, "ScanTime_second", "Position1",
            DFNT_INT8, HDFE_NOMERGE);
    //printf ("def_geo/define geofield ScanTime_second %ld\n", status);
    results = results + status;

    /*---------------------------------------------------*
     * Define the geolocation fields: latitude, longitude
     * and Time(TAI93) fields
     *---------------------------------------------------*/
    status = SWdefgeofield(sw_id, "BeamTime", "Position1",
            DFNT_FLOAT64, HDFE_NOMERGE);
    //printf ("def_geo/define geofield Time %ld\n", status);
    results = results + status;

    status = SWdefgeofield(sw_id, "Latitude", "Position1,Position2",
            DFNT_FLOAT32, HDFE_NOMERGE);
    //printf ("def_geo/define geofield Latitude %ld\n", status);
    results = results + status;

    status = SWdefgeofield(sw_id, "Longitude", "Position1,Position2",
            DFNT_FLOAT32, HDFE_NOMERGE);
    //printf ("def_geo/define geofield Longitude %ld\n", status);
    results = results + status;

    /*--------------------------------------------------------*
     * Set field compression for all subsequent field definitions
     *--------------------------------------------------------*/
    status = SWdefcomp(sw_id, HDFE_COMP_DEFLATE, &compparm);
    //printf ("def_data/swdefcomp %ld\n", status);
    results = results + status;

    /*--------------------------------------------------------*
     * Define ancillary fields
     *--------------------------------------------------------*/
    status = SWdefdatafield(sw_id, "Orbit_mode", "Scanline",
            DFNT_INT8, HDFE_NOMERGE);
    //printf ("def_data/define datafield Orbit_mode %ld\n", status);
    results = results + status;

    status = SWdefdatafield(sw_id, "LZ_angle", "Scanline,Field_of_view",
            DFNT_FLOAT32, HDFE_NOMERGE);
    //printf ("def_data/define datafield LZ_angle  %ld\n", status);
    results = results + status;

    status = SWdefdatafield(sw_id, "SZ_angle", "Scanline,Field_of_view",
            DFNT_FLOAT32, HDFE_NOMERGE);
    //printf ("def_data/define datafield SZ_angle  %ld\n", status);
    results = results + status;

    status = SWdefdatafield(sw_id, "Sfc_type", "Scanline,Field_of_view",
            DFNT_INT8, HDFE_NOMERGE);
    //printf ("def_data/define datafield Surface_type %ld\n", status);
    results = results + status;

    /*--------------------------------------------------------*
     * Define product fields
     *--------------------------------------------------------*/
    status = SWdefdatafield(sw_id, "SFR", "Scanline,Field_of_view",
            DFNT_INT16, HDFE_NOMERGE);
    //printf ("def_prod/define product field SFR %ld\n", status);
    results = results + status;

    status = SWdefdatafield(sw_id, "FLAGS", "Scanline,Field_of_view",
            DFNT_UINT32, HDFE_NOMERGE);
    //printf ("def_prod/define product field SFR %ld\n", status);
    results = results + status;

    status = SWdefdatafield(sw_id, "SD", "Scanline,Field_of_view",
            DFNT_INT16, HDFE_NOMERGE);
    //printf ("def_prod/define product field SD %ld\n", status);
    results = results + status;

    status = SWdefdatafield(sw_id, "IWP", "Scanline,Field_of_view",
            DFNT_INT16, HDFE_NOMERGE);
    //printf ("def_prod/define product field PROB %ld\n", status);
    results = results + status;

    status = SWdefdatafield(sw_id, "De", "Scanline,Field_of_view",
            DFNT_INT16, HDFE_NOMERGE);
    //printf ("def_prod/define product field PROB %ld\n", status);
    results = results + status;

    status = SWdefdatafield(sw_id, "PROB", "Scanline,Field_of_view",
            DFNT_INT16, HDFE_NOMERGE);
    //printf ("def_prod/define product field PROB %ld\n", status);
    results = results + status;

    /*--------------------------------------------------------*
     * Define AT data fields
     *--------------------------------------------------------*/
    for(i = 0; i < nchan; i++)  {
        sprintf(chan_name, "Chan%d_AT", i+1);
        status = SWdefdatafield(sw_id, chan_name, "Scanline,Field_of_view",
                DFNT_INT16, HDFE_NOMERGE);
        //printf ("def_data/define datafield Chan%d_AT %ld\n", i+1, status);
        results = results + status;
    }

    status = SWdefcomp(sw_id, HDFE_COMP_NONE, NULL);
    //printf ("def_data/swdefcomp %ld\n", status);
    results = results + status;

    /*-------------------------------------------*
     * Detach and close swath file
     *-------------------------------------------*/
    SWdetach(sw_id);
    SWclose(swhdf_id);

    return(results);
}



long int swath_wrt(char *ffn, char *swname, int nscan, int nfov, int nchan, 
        int *nfova, float *sfr_lim, int nyref, int **lese_tab) 
{ 
    char        atname[SL5];
    long int    swhdf_id, sw_id;
    long int    status, results = 0;

    float       sfr_scal, iwp_scal, de_scal;
    float       IWP_SCAL=100.0, DE_SCAL=100.0, SFR_SCAL=100.0, AT_SCAL=100.0;
    int         MISSING=-99; 

    char        ctmp2[nscan][nfov];
    short int   stmp2[nscan][nfov];

    int         yeart, mont, dayt, hourt, mint, sect;
    double      fjsec1[nscan];
    short int   year1[nscan];
    char        mon1[nscan], day1[nscan], hour1[nscan], 
                min1[nscan], sec1[nscan];

    int         i, j, k;
    int32       start[2]  = {0, 0}, stride[2] = {1, 1}, edges[2];

    // Generate edges
    edges[0] = nscan;
    edges[1] = nfov;

    // Open HDF-EOS swath file
    swhdf_id = SWopen(ffn, DFACC_RDWR);
    //printf ("sw_wrt/ swhdf_id %ld\n", swhdf_id);

    HEprint(stdout,0);
    sw_id = SWattach(swhdf_id, swname);
    HEprint(stdout,0);
    //printf ("sw_wrt/ sw_id %ld\n", sw_id);

    // Write product limits
    status = SWwriteattr(sw_id, "SFR_Limits", DFNT_FLOAT32, 2, sfr_lim);
    //printf("wrt_limits/writeattr SFR_Limits %ld\n", status);
    results = results + status;

    // Write product scaling factors
    iwp_scal = IWP_SCAL;
    de_scal = DE_SCAL;
    sfr_scal = SFR_SCAL;

    status = SWwriteattr(sw_id, "IWP_SCAL", DFNT_FLOAT32, 1, &iwp_scal);
    //printf("wrt_scal/writeattr IWP_SCAL %ld\n",status);
    results = results + status;

    status = SWwriteattr(sw_id, "De_SCAL", DFNT_FLOAT32, 1, &de_scal);
    //printf("wrt_scal/writeattr De_SCAL %ld\n",status);
    results = results + status;

    status = SWwriteattr(sw_id, "SFR_SCAL", DFNT_FLOAT32, 1, &sfr_scal);
    //printf("wrt_scal/writeattr SFR_SCAL %ld\n",status);
    results = results + status;

    // Write geolocation fields
    for (i=0; i<nscan; i++)  {
        fjsec1[i] = btjs[i][0] + (double)dsec[i][0];
        //printf("fjsec1[%d]=%lf %lld %lf\n", i, fjsec1[i], btjs[i][0], 
        //        dsec[i][0]);
        jsec2cal(btjs[i][0], &yeart, &mont, &dayt, 
                &hourt, &mint, &sect, nyref, lese_tab);
        year1[i] = yeart;    mon1[i] = mont;    day1[i] = dayt;
        hour1[i] = hourt;    min1[i] = mint;    sec1[i] = sect;
    }

    status = SWwritefield(sw_id, "ScanTime_year", start,stride,edges, year1);
    //printf("wrt_geo/writefield ScanTime_year %ld\n",status);
    results = results + status;

    status = SWwritefield(sw_id, "ScanTime_month", start,stride,edges, mon1);
    //printf("wrt_geo/writefield ScanTime_month %ld\n",status);
    results = results + status;

    status = SWwritefield(sw_id, "ScanTime_dom", start, stride, edges, day1);
    //printf("wrt_geo/writefield ScanTime_dom %ld\n",status);
    results = results + status;

    status = SWwritefield(sw_id, "ScanTime_hour", start,stride,edges, hour1);
    //printf("wrt_geo/writefield ScanTime_hour %ld\n",status);
    results = results + status;

    status = SWwritefield(sw_id, "ScanTime_minute", start,stride,edges, min1);
    //printf("wrt_geo/writefield ScanTime_minute %ld\n",status);
    results = results + status;

    status = SWwritefield(sw_id, "ScanTime_second", start,stride,edges, sec1);
    //printf("wrt_geo/writefield ScanTime_second %ld\n",status);
    results = results + status;

    status = SWwritefield(sw_id, "BeamTime", start, stride, edges, fjsec1);
    //printf("wrt_geo/writefield BeamTime %ld\n",status);
    results = results + status;

    status = SWwritefield(sw_id, "Latitude", start, stride, edges, lat_p1);
    //printf("wrt_geo/writefield latitude %ld\n",status);
    results = results + status;

    status = SWwritefield(sw_id, "Longitude", start, stride, edges, lon_p1);
    //printf("wrt_geo/writefield longitude %ld\n",status);
    results = results + status;
    // Write data fields
    status = SWwritefield(sw_id, "Orbit_mode", start, stride, edges, obmd_p1);
    //printf("wrt_data/writefield orbit_mode %ld\n", status);
    results = results + status;

    status = SWwritefield(sw_id, "LZ_angle", start, stride, edges, lza_p1);
    //printf("wrt_data/writefield lza %ld\n", status);
    results = results + status;

    status = SWwritefield(sw_id, "SZ_angle", start, stride, edges, sza_p1);
    //printf("wrt_data/writefield sza %ld\n", status);
    results = results + status;

    for (i=0; i<nscan; i++)  {
        for (j=0; j<nfov; j++)
            ctmp2[i][j] = stype[i][j];
    }
    status = SWwritefield(sw_id, "Sfc_type", start, stride, edges, ctmp2);
    //printf("wrt_data/writefield Surface_type %ld\n", status);
    results = results + status;

    /*-----------------------------------------------*
     * Write product fields 
     *-----------------------------------------------*/
    for (i=0; i<nscan; i++)  {
        for (j=0; j<nfov; j++)
            if (sfr[i][j] > 0)
                stmp2[i][j] = round(sfr[i][j]*100);
            else
                stmp2[i][j] = round(sfr[i][j]);
    }
    status = SWwritefield(sw_id, "SFR", start, stride, edges, stmp2);
    //printf("wrt_prod/write field SFR %ld\n", status);
    results = results + status;

    status = SWwritefield(sw_id, "FLAGS", start, stride, edges, flag_p1);
    //printf("wrt_prod/write field SFR %ld\n", status);
    results = results + status;

    for (i=0; i<nscan; i++)  {
        for (j=0; j<nfov; j++)
            stmp2[i][j] = sd[i][j];
    }
    status = SWwritefield(sw_id, "SD", start, stride, edges, stmp2);
    //printf("wrt_prod/write field SD %ld\n", status);
    results = results + status;

    for (i=0; i<nscan; i++)  {
        for (j=0; j<nfov; j++)
            if (iwp[i][j] > 0)
                stmp2[i][j] = round(iwp[i][j]*100);
            else
                stmp2[i][j] = round(iwp[i][j]);
    }
    status = SWwritefield(sw_id, "IWP", start, stride, edges, stmp2);
    //printf("wrt_prod/write field PROB %ld\n", status);
    results = results + status;

    for (i=0; i<nscan; i++)  {
        for (j=0; j<nfov; j++)
            if (de[i][j] > 0)
                stmp2[i][j] = round(de[i][j]*100);
            else
                stmp2[i][j] = round(de[i][j]);
    }
    status = SWwritefield(sw_id, "De", start, stride, edges, stmp2);
    //printf("wrt_prod/write field PROB %ld\n", status);
    results = results + status;

    for (i=0; i<nscan; i++)  {
        for (j=0; j<nfov; j++)
            if (prob[i][j] > 0)
                stmp2[i][j] = round(prob[i][j]*100);
            else
                stmp2[i][j] = round(prob[i][j]);
    }
    status = SWwritefield(sw_id, "PROB", start, stride, edges, stmp2);
    //printf("wrt_prod/write field PROB %ld\n", status);
    results = results + status;

    for(k = 0; k < nchan; k++)  {
        for(i = 0; i < nscan; i++)  {
            for(j = 0; j < nfov; j++)
                if(at[i][j][k] != MISSING)
                    stmp2[i][j] = round(AT_SCAL*at[i][j][k]);
                else
                    stmp2[i][j] = MISSING;
        }
        sprintf(atname,"Chan%d_AT",k+1);
        status = SWwritefield(sw_id, atname, start, stride, edges, stmp2);
        //printf("wrt_data/writefield at%d  %ld\n", k+1, status);
        results = results + status;
    }

    printf("wrt_prod: %ld\n", results);

    /*-----------------------------------------------*
     * Detach and close HDF-EOS swath file
     *-----------------------------------------------*/
    status = SWdetach(sw_id);
    //printf ("sw_wrt/value of detach swath file %ld\n", status);
    status = SWclose (swhdf_id);
    //printf ("sw_wrt/value of close swath file %ld\n", status);

    return(sw_id);
}



int prod_init(int nscan, int nfov, int nchan, int *nfova)
{
    stype_p1   = init_2dc_int (nscan, nfov, &stype);
    sfr_p1     = init_2dc_float(nscan, nfov, &sfr);
    sd_p1      = init_2dc_int (nscan, nfov, &sd);
    prob_p1    = init_2dc_float(nscan, nfov, &prob);
    iwp_p1     = init_2dc_float(nscan, nfov, &iwp);
    de_p1      = init_2dc_float(nscan, nfov, &de);
    flag_p1    = init_2dc_int (nscan, nfov, &flag);

    return(0);
}



int set_flag(int nscan, int nfov, int **flag0)
{
    int     iscan, ifov;

    for (iscan=0; iscan<nscan; iscan++)  {
        //printf("iscan: %d/%d\n", iscan, nscan);
        for (ifov=0; ifov<nfov; ifov++)  {
            //printf("iscan, ifov: %d/%d, %d/%d\n", iscan, nscan, ifov, nscan);
            flag[iscan][ifov] = flag0[iscan][ifov];
        }
    }

    return(0);
}



int prod_free(int nscan, int nfov, int nchan, int *nfova)
{
    int irt, sirt;

    sirt = 0;
    irt = free_2dc_int (nscan, stype, stype_p1);
    sirt+=irt;
    irt = free_2dc_float(nscan, sfr, sfr_p1);
    sirt+=irt;
    irt = free_2dc_int (nscan, sd, sd_p1);
    sirt+=irt;
    irt = free_2dc_float(nscan, prob, prob_p1);
    sirt+=irt;
    irt = free_2dc_float(nscan, iwp, iwp_p1);
    sirt+=irt;
    irt = free_2dc_float(nscan, de, de_p1);
    sirt+=irt;
    irt = free_2dc_int (nscan, flag, flag_p1);
    sirt+=irt;
    //printf("free prod: %d\n", sirt);

    return(0);
}




