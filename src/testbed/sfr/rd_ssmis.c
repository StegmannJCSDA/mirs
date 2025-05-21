#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "stdint.h"

#include "d_sys.h"
#include "d_darrc.h"
#include "d_jsec.h"


#define HEADER_SIZE 40
#define MAX_SCANS   13102   //max_scan for one orbit of SSMIS data
#define NPIX        180     //Number of pixels per scan line (91GHz)
#define SCAN_LENGTH 9592    //number of bytes in each scan line (SSMIS)

#define     LON_MIN     -400
#define     LON_MAX     400
#define     LAT_MIN     -100
#define     LAT_MAX     100
#define     AT_MIN      50
#define     AT_MAX      500


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
extern  float       *hd_sqc, **ti_sqc, *hd_flb, **ti_flb;

uint16_t    swap_uint16(uint16_t);
int16_t     swap_int16(int16_t);
uint32_t    swap_uint32(uint32_t);
int32_t     swap_int32(int32_t);


/*---------------------------------*
 *         header record           *
 *---------------------------------*/
typedef struct
{
    char                dummy0[8];
    unsigned int        year;
    unsigned short      jday;
    unsigned char       hrs;
    unsigned char       mns;
    char                dummy1[2];
    unsigned short      scanlines;
    char                dummy2[20];
}HBLOCK;
#define L_HBLOCK  sizeof(HBLOCK)

/*---------------------------------*
 *         img record (91 GHz)     *
 *---------------------------------*/
typedef struct
{
    char                dummy0[6];
    unsigned char       sfc_img;
    char                dummy1;
    short               a150;
    short               a176;
    short               a180;
    short               a182;
    short               alat1;
    short               alon1;
    short               a91v;
    short               a91h;
}IBLOCK;
#define L_IBLOCK  sizeof(IBLOCK)

/*---------------------------------*
 *         env record (19, 22, 37) *
 *---------------------------------*/
typedef struct
{
    short               alat3;
    short               alon3;
    char                dummy1;
    unsigned char       sfc_env;
    short               a19h;
    short               a19v;
    short               a22v;
    short               alat2;
    short               alon2;
    short               a37h;
    short               a37v;
}EBLOCK;
#define L_EBLOCK  sizeof(EBLOCK)

/*---------------------------------*
 *         las record (53 GHz)     *
 *---------------------------------*/
typedef struct
{
    short               alat4;
    short               alon4;
    short               dummy1;
    unsigned short      sfc_las;
    short               a503;
    short               a528;
    short               a536;
    short               a544;
    short               a555;
    short               a573;
    short               a594;
    short               ch24;
}LBLOCK;
#define L_LBLOCK  sizeof(LBLOCK)

/*---------------------------------*
 *         scanline data           *
 *---------------------------------*/
typedef struct
{
    unsigned int        year;
    unsigned short      jday;
    char                dummy2[6];
    unsigned int        timetemp;
    char                dummy3[80];
    IBLOCK              iblock[NPIX];
    EBLOCK              eblock[NPIX/2];
    LBLOCK              lblock[NPIX/3];
    char                dummy4[1936];
}SCANLINE;
#define L_SCANLINE  sizeof(SCANLINE)



float           **ini_2d_arr_float();
unsigned char   **ini_2d_arr_uchar();
unsigned short  **ini_2d_arr_ushort();



int rd_ssmis(char *ffn0, char *ftype,
        int *nscanp, int *nfovp, int *nchanp, int **nfovap, int **nfova_p1p,
        int ***flag0p, int **flag0_p1p, int nyref, int **lese_tab)
{
    //printf("===========================\n");
    //printf("in new: ffn0: %s\n", ffn0);

    HBLOCK      hblock;
    SCANLINE    sline;

    int         nscan0, nfov0, nchan0, nscan1, nfov1, nchan1;

    int         iscan, i;
    //float       tempbt;
    //short       templat;

    short       year, doy, hrs, mns;
    float       btime;

    FILE        *fid;
    int         **flag0, *flag0_p1, *nfova, *nfova_p1;

    long long int   jsec0, btjs1;


    fid = fopen(ffn0, "r");

    //==== Read header data ====================
    fread(&hblock, L_HBLOCK, 1, fid);

    nscan0 = swap_uint16(hblock.scanlines);
    nfov0 = 180;
    nchan0 = 24;

    year = swap_uint32(hblock.year);
    doy = swap_uint16(hblock.jday);
    hrs  = hblock.hrs;
    mns  = hblock.mns;
    //printf("date = %d, %d, %d, %d\n", year, doy, hrs, mns);
    //printf("scanlines = %d, scanline_ncrr = %d\n",
    //        nscan0, hblock.scanlines);

    nscan1 = nscan0;
    nfov1 = nfov0;
    nchan1 = nchan0;

    // init sat var
    lon_p1  = init_2dc_float(nscan1, nfov1, &lon);
    lat_p1  = init_2dc_float(nscan1, nfov1, &lat);
    btjs_p1 = init_2dc_llint(nscan1, nfov1, &btjs);
    dsec_p1 = init_2dc_float(nscan1, nfov1, &dsec);
    obmd_p1 = init_2dc_char (nscan1, nfov1, &obmd);
    lza_p1  = init_2dc_float(nscan1, nfov1, &lza);
    sza_p1  = init_2dc_float(nscan1, nfov1, &sza);
    at_p1   = init_3dc_float(nscan1, nfov1, nchan1, &at);

    flag0_p1 = init_2dc_int (nscan1, nfov1, &flag0);

    nfova_p1 = init_1dc_int(nchan1, &nfova);
    for (i=0; i<nchan1; i++)    nfova[i] = nfov1;

    
    //==== Read scanline data and fill the arrays ====================
    for (iscan=0; iscan<nscan1; iscan++) {
        fread(&sline, L_SCANLINE, 1, fid);

        // btjs
        year = swap_uint32(sline.year);
        doy = swap_uint16(sline.jday);
        cal2jsec(year, 1, 1, 0, 0, 0, &jsec0);
        btime = swap_uint32(sline.timetemp)/1000.;
        btjs1 = jsec0 + (doy-1)*86400 + round(btime);
        for (i=0; i<NPIX; i++)  {
            btjs[iscan][i] = btjs1;
        }
        //printf("iscan=%d, btime=%f, btjs1=%lld\n", iscan, btime, btjs1);

        // start/end dttm
        if (iscan == 0) {
            jsec2cal(btjs1, &dtse[0][0], &dtse[0][1], &dtse[0][2],
                    &dtse[0][3], &dtse[0][4], &dtse[0][5]);
            printf("dtse[0]: %04d/%02d/%02d %02d:%02d:%02d\n", dtse[0][0], 
                    dtse[0][1], dtse[0][2], dtse[0][3], dtse[0][4], dtse[0][5]);
            //getchar();
        }
        else if (iscan == nscan1-1) {
            jsec2cal(btjs1, &dtse[1][0], &dtse[1][1], &dtse[1][2],
                    &dtse[1][3], &dtse[1][4], &dtse[1][5]);
            printf("dtse[1]: %04d/%02d/%02d %02d:%02d:%02d\n", dtse[1][0], 
                    dtse[1][1], dtse[1][2], dtse[1][3], dtse[1][4], dtse[1][5]);
            //getchar();
        }

        for (i = 0; i < NPIX; i++) {
            //sfc_img[iscan][i] = sline.iblock[i].sfc_img;
            lon[iscan][i] = (short)swap_int16(sline.iblock[i].alon1)*0.01;
            lat[iscan][i] = (short)swap_int16(sline.iblock[i].alat1)*0.01;
            at[iscan][i][16] = (short)swap_int16(sline.iblock[i].a91v)*0.01+273.15;
            at[iscan][i][17] = (short)swap_int16(sline.iblock[i].a91h)*0.01+273.15;
            at[iscan][i][ 7] = (short)swap_int16(sline.iblock[i].a150)*0.01+273.15;
            at[iscan][i][ 8] = (short)swap_int16(sline.iblock[i].a176)*0.01+273.15;
            at[iscan][i][ 9] = (short)swap_int16(sline.iblock[i].a180)*0.01+273.15;
            at[iscan][i][10] = (short)swap_int16(sline.iblock[i].a182)*0.01+273.15;
            at[iscan][i][16] *= ti_flb[i][8];
            at[iscan][i][17] *= ti_flb[i][9];
            at[iscan][i][ 7] *= ti_flb[i][4];
            at[iscan][i][ 8] *= ti_flb[i][5];
            at[iscan][i][ 9] *= ti_flb[i][6];
            at[iscan][i][10] *= ti_flb[i][7];;
        }

        for (i = 0; i < NPIX/2; i++) {
            at[iscan][2*i][13] = (short)swap_int16(sline.eblock[i].a22v)*0.01+273.15;
            at[iscan][2*i][14] = (short)swap_int16(sline.eblock[i].a37h)*0.01+273.15;
            at[iscan][2*i][15] = (short)swap_int16(sline.eblock[i].a37v)*0.01+273.15;
            at[iscan][2*i][13] *= ti_flb[i][12];
            at[iscan][2*i][14] *= ti_flb[i][13];
            at[iscan][2*i][15] *= ti_flb[i][14];
            at[iscan][2*i+1][13] = at[iscan][2*i][13];
            at[iscan][2*i+1][14] = at[iscan][2*i][14];
            at[iscan][2*i+1][15] = at[iscan][2*i][15];
        }

        for (i = 0; i < NPIX/3; i++) {
            at[iscan][3*i][ 0] = (short)swap_int16(sline.lblock[i].a503)*0.01+273.15;
            at[iscan][3*i][ 1] = (short)swap_int16(sline.lblock[i].a528)*0.01+273.15;
            at[iscan][3*i][ 2] = (short)swap_int16(sline.lblock[i].a536)*0.01+273.15;
            at[iscan][3*i+1][ 0] = at[iscan][3*i][ 0];
            at[iscan][3*i+1][ 1] = at[iscan][3*i][ 1];
            at[iscan][3*i+1][ 2] = at[iscan][3*i][ 2];
            at[iscan][3*i+2][ 0] = at[iscan][3*i][ 0];
            at[iscan][3*i+2][ 1] = at[iscan][3*i][ 1];
            at[iscan][3*i+2][ 2] = at[iscan][3*i][ 2];
        }

        for (i = 0; i < NPIX; i++) {
            lza[iscan][i] = 53.1;
            sza[iscan][i] = 53.1;
        }
    }

    //tempbt = (short)swap_int16(sline.iblock[NPIX/2-1].a150)*0.01+273.15; 
    //templat = (short)swap_int16(sline.eblock[NPIX/2-10].alat2);
    //printf("L_LBLOCK = %d, L_SCANLINE = %d\n", L_LBLOCK, L_SCANLINE);
    //printf("a150_ncor = %f, alat2 = %d\n", tempbt, templat);
    //printf("a150_corr = %f, alat2 = %f\n", a150[iscan-1][NPIX/2-1], 
    //        alat2[iscan-1][NPIX/2-10]);
    //printf("a180_corr = %f, alat3 = %f\n", a180[iscan-1][NPIX-1], 
    //        alat3[iscan-1][NPIX/2-10]);
    //printf("a536 = %f, alat4 = %f\n", a536[iscan-1][NPIX/3-1], 
    //        alat4[iscan-1][NPIX/3-10]);
    //printf("jdate = %d, btime = %f\n", jdate[iscan-1], btime[iscan-1]);
    //printf("jdate1 = %d, bitme1 = %f\n", jdate[0], btime[0]);

    fclose(fid);


    ///////////////////////////////////////////
    // qc
    int     tt, ifov, ichan;
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)  {

            flag0[iscan][ifov] = 0;
            // time
            if (btjs[iscan][ifov] < 0)  {
                flag0[iscan][ifov] |= (1U<<8);
            }
            // location
            if (lon[iscan][ifov]<LON_MIN || lon[iscan][ifov]>LON_MAX 
                    || lat[iscan][ifov]<LAT_MIN || lat[iscan][ifov]>LAT_MAX)  {
                flag0[iscan][ifov] |= (1U<<9);
                printf("lon, lat: %f, %f\n", lon[iscan][ifov],
                        lat[iscan][ifov]);
            }
            // at
            for (ichan=0; ichan<nchan1; ichan++)  {
                tt = round(ti_sqc[0][ichan]);
                if (tt == 1)  {
                    if (at[iscan][ifov][ichan]<AT_MIN 
                            || at[iscan][ifov][ichan]>AT_MAX)  {
                        flag0[iscan][ifov] |= (1U<<10);
                        printf(" N");
                    }
                }
            }  // ichan

        }  // ifov
    }  // iscan


    // set return variables
    *nfovp = nfov1;     *nscanp = nscan1;    *nchanp = nchan1;
    *nfovap = nfova;    *nfova_p1p = nfova_p1;
    *flag0p = flag0;    *flag0_p1p = flag0_p1;

    //printf("ctr-c to quit\n");
    //getchar();

    return(0);
}



void read_ssmis(char *ffn)
{
    HBLOCK      hblock;
    SCANLINE    sline;

    short       scanlines;

    int         iline, i;
    float        tempbt;
    short       templat;

    short       tyear, jday, hrs, mns, syear, sjday, eyear, ejday;
    short       *year, *doy;
    char        *month, *dom, *hour, *minute, *second, *orb_mode;
    int         *jdate;
    float       *btime;
    double      *time_tai93;

    FILE        *fid;

    fid = fopen(ffn, "r");

    //==== Read header data ====================
    fread(&hblock, L_HBLOCK, 1, fid);

    scanlines = swap_uint16(hblock.scanlines);
    tyear = swap_uint32(hblock.year);
    jday = swap_uint16(hblock.jday);
    hrs  = hblock.hrs;
    mns  = hblock.mns;

    printf("date = %d, %d, %d, %d\n", tyear, jday, hrs, mns);
    printf("scanlines = %d, scanline_ncrr = %d\n",
            scanlines, hblock.scanlines);

    //==== Allocate the arrays according to header info ====================
    jdate = (int *)malloc(scanlines*sizeof(int));        
    btime = (float *)malloc(scanlines*sizeof(float));

    year = (short *)malloc(scanlines*sizeof(short));
    doy = (short *)malloc(scanlines*sizeof(short));
    month = (char *)malloc(scanlines*sizeof(char));
    dom = (char *)malloc(scanlines*sizeof(char));
    hour = (char *)malloc(scanlines*sizeof(char));
    minute = (char *)malloc(scanlines*sizeof(char));
    second = (char *)malloc(scanlines*sizeof(char));
    time_tai93 = (double *)malloc(scanlines*sizeof(double));
    orb_mode = (char *)malloc(scanlines*sizeof(char));


    float           **a19v, **a19h, **a22v, **a37v, **a37h;
    float           **alat2, **alon2, **alat3, **alon3;
    unsigned char   **sfc_env;

    float           **a91v, **a91h, **a150, **a176, **a180, **a182;
    float           **alat1, **alon1;
    unsigned char   **sfc_img;

    float           **a503, **a528, **a536, **a544, **a555, **a573, **a594;
    float           **alat4, **alon4;
    unsigned short  **sfc_las;

    float           **lz_angle, **sz_angle;


    a19v = ini_2d_arr_float(scanlines,NPIX/2);    
    a19h = ini_2d_arr_float(scanlines,NPIX/2);    
    a22v = ini_2d_arr_float(scanlines,NPIX/2);    
    a37v = ini_2d_arr_float(scanlines,NPIX/2);    
    a37h = ini_2d_arr_float(scanlines,NPIX/2);    
    alat2 = ini_2d_arr_float(scanlines,NPIX/2);    
    alon2 = ini_2d_arr_float(scanlines,NPIX/2);    
    alat3 = ini_2d_arr_float(scanlines,NPIX/2);    
    alon3 = ini_2d_arr_float(scanlines,NPIX/2);   

    sfc_env = ini_2d_arr_uchar(scanlines,NPIX/2);    
 
    a91v = ini_2d_arr_float(scanlines,NPIX);    
    a91h = ini_2d_arr_float(scanlines,NPIX);    
    a150 = ini_2d_arr_float(scanlines,NPIX);    
    a176 = ini_2d_arr_float(scanlines,NPIX);    
    a180 = ini_2d_arr_float(scanlines,NPIX);    
    a182 = ini_2d_arr_float(scanlines,NPIX);    
    alat1 = ini_2d_arr_float(scanlines,NPIX);    
    alon1 = ini_2d_arr_float(scanlines,NPIX); 
   
    sfc_img = ini_2d_arr_uchar(scanlines,NPIX);    

    a503 = ini_2d_arr_float(scanlines,NPIX/3);
    a528 = ini_2d_arr_float(scanlines,NPIX/3);
    a536 = ini_2d_arr_float(scanlines,NPIX/3);
    a544 = ini_2d_arr_float(scanlines,NPIX/3);
    a555 = ini_2d_arr_float(scanlines,NPIX/3);
    a573 = ini_2d_arr_float(scanlines,NPIX/3);
    a594 = ini_2d_arr_float(scanlines,NPIX/3);
    alat4 = ini_2d_arr_float(scanlines,NPIX/3);    
    alon4 = ini_2d_arr_float(scanlines,NPIX/3); 

    sfc_las = ini_2d_arr_ushort(scanlines,NPIX/3);

    lz_angle = ini_2d_arr_float(scanlines,NPIX);
    sz_angle = ini_2d_arr_float(scanlines,NPIX);
    
    //==== Read scanline data and fill the arrays ====================
    for (iline = 0; iline < scanlines; iline ++) {
        fread(&sline, L_SCANLINE, 1, fid);

        tyear = swap_uint32(sline.year);
        jday = swap_uint16(sline.jday);
        year[iline] = tyear;
        doy[iline] = jday;
        jdate[iline] = tyear*1000 + jday;
        btime[iline] = swap_uint32(sline.timetemp)/1000.;
        //printf("btime[%d]=%f\n", iline, btime[iline]);
        //getchar();
        if (iline == 0) {
            syear = tyear;
            sjday = jday;
        }
        else if (iline == scanlines - 1) {
            eyear = tyear;
            ejday = jday;
        }

        for (i = 0; i < NPIX; i++) {
            sfc_img[iline][i] = sline.iblock[i].sfc_img;
            alat1[iline][i] = (short)swap_int16(sline.iblock[i].alat1)*0.01;
            alon1[iline][i] = (short)swap_int16(sline.iblock[i].alon1)*0.01;
            a91v[iline][i] = (short)swap_int16(sline.iblock[i].a91v)*0.01+273.15;
            a91h[iline][i] = (short)swap_int16(sline.iblock[i].a91h)*0.01+273.15;
            a150[iline][i] = (short)swap_int16(sline.iblock[i].a150)*0.01+273.15;
            a176[iline][i] = (short)swap_int16(sline.iblock[i].a176)*0.01+273.15;
            a180[iline][i] = (short)swap_int16(sline.iblock[i].a180)*0.01+273.15;
            a182[iline][i] = (short)swap_int16(sline.iblock[i].a182)*0.01+273.15;
            //a91v[iline][i] *= coeff_91v[i];
            //a91h[iline][i] *= coeff_91h[i];
            //a150[iline][i] *= coeff_150[i];
            //a176[iline][i] *= coeff_176[i];
            //a180[iline][i] *= coeff_180[i];
            //a182[iline][i] *= coeff_182[i];
            a91v[iline][i] *= ti_flb[i][8];
            a91h[iline][i] *= ti_flb[i][9];
            a150[iline][i] *= ti_flb[i][4];
            a176[iline][i] *= ti_flb[i][5];
            a180[iline][i] *= ti_flb[i][6];
            a182[iline][i] *= ti_flb[i][7];;
        }

        for (i = 0; i < NPIX/2; i++) {
            sfc_env[iline][i] = sline.eblock[i].sfc_env;
            alat2[iline][i] = (short)swap_int16(sline.eblock[i].alat2)*0.01;
            alon2[iline][i] = (short)swap_int16(sline.eblock[i].alon2)*0.01;
            a22v[iline][i] = (short)swap_int16(sline.eblock[i].a22v)*0.01+273.15;
            a37h[iline][i] = (short)swap_int16(sline.eblock[i].a37h)*0.01+273.15;
            a37v[iline][i] = (short)swap_int16(sline.eblock[i].a37v)*0.01+273.15;
            //a22v[iline][i] *= coeff_22v[i];
            //a37h[iline][i] *= coeff_37h[i];
            //a37v[iline][i] *= coeff_37v[i];
            a22v[iline][i] *= ti_flb[i][12];
            a37h[iline][i] *= ti_flb[i][13];
            a37v[iline][i] *= ti_flb[i][14];
        }

        for (i = 0; i < NPIX/3; i++) {
            sfc_las[iline][i] = (short)swap_uint16(sline.lblock[i].sfc_las);
            alat4[iline][i] = (short)swap_int16(sline.lblock[i].alat4)*0.01;
            alon4[iline][i] = (short)swap_int16(sline.lblock[i].alon4)*0.01;
            a503[iline][i] = (short)swap_int16(sline.lblock[i].a503)*0.01+273.15;
            a528[iline][i] = (short)swap_int16(sline.lblock[i].a528)*0.01+273.15;
            a536[iline][i] = (short)swap_int16(sline.lblock[i].a536)*0.01+273.15;
        }

    }

    tempbt = (short)swap_int16(sline.iblock[NPIX/2-1].a150)*0.01+273.15; 
    templat = (short)swap_int16(sline.eblock[NPIX/2-10].alat2);
    //printf("L_LBLOCK = %d, L_SCANLINE = %d\n", L_LBLOCK, L_SCANLINE);
    printf("a150_ncor = %f, alat2 = %d\n", tempbt, templat);
    printf("a150_corr = %f, alat2 = %f\n", a150[iline-1][NPIX/2-1], 
            alat2[iline-1][NPIX/2-10]);
    //printf("a180_corr = %f, alat3 = %f\n", a180[iline-1][NPIX-1], 
    //        alat3[iline-1][NPIX/2-10]);
    printf("a536 = %f, alat4 = %f\n", a536[iline-1][NPIX/3-1], 
            alat4[iline-1][NPIX/3-10]);
    printf("jdate = %d, btime = %f\n", jdate[iline-1], btime[iline-1]);
    printf("jdate1 = %d, bitme1 = %f\n", jdate[0], btime[0]);

    fclose(fid);

} /* end of read_ssmis.c */



float **ini_2d_arr_float(int pix, int scan)
{
   float **var = NULL;
   int   i, j;
   var = (float **)malloc(pix*sizeof(float *));
   for (i = 0; i < pix; i++) {
       var[i] = (float *)malloc(scan*sizeof(float));
       for ( j = 0; j < scan; j++)
           var[i][j] = -999.9;
   }
   return (var);
}



unsigned char **ini_2d_arr_uchar(int pix, int scan)
{
   unsigned char **var = NULL;
   int   i, j;
   var = (unsigned char **)malloc(pix*sizeof(unsigned char *));
   for (i = 0; i < pix; i++) {
       var[i] = (unsigned char *)malloc(scan*sizeof(unsigned char));
       for ( j = 0; j < scan; j++)
           var[i][j] = -99;
   }
   return (var);
}



unsigned short **ini_2d_arr_ushort(int pix, int scan)
{
   unsigned short **var = NULL;
   int   i, j;
   var = (unsigned short **)malloc(pix*sizeof(unsigned short *));
   for (i = 0; i < pix; i++) {
       var[i] = (unsigned short *)malloc(scan*sizeof(unsigned short));
       for ( j = 0; j < scan; j++)
           var[i][j] = -99;
   }
   return (var);
}


uint16_t swap_uint16(uint16_t val)
{
    return (val << 8) | (val >> 8);
}



int16_t swap_int16(int16_t val)
{
    return (val << 8) | ((val >> 8) & 0xFF);
}



uint32_t swap_uint32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}



int32_t swap_int32(int32_t val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | ((val >> 16) & 0xFFFF);
}




