#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#include "d_sys.h"
#include "d_darrc.h"
#include "d_jsec.h"

#include "hdf5.h"


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
extern  float       *hd_sqc, **ti_sqc;



int rd_gmi(char *ffn0, char *ftype, 
        int *nscanp, int *nfovp, int *nchanp, int **nfovap, int **nfova_p1p, 
        int ***flag0p, int **flag0_p1p, int nyref, int **lese_tab)
{
    //printf("in new: ffn0: %s\n", ffn0);

    int            i, ndims, status;
    unsigned long int    index;

    int     ifov, iscan, ichan;
    int     nfov1, nscan1, nchan1, *nfova, *nfova_p1;
    int     nfov0, nscan0, nchan0;

    int     **flag0, *flag0_p1;

    hid_t       file_id, root_id, ss_id;
    hid_t       dsetr_id, space_id, type_id;
    hsize_t        dims[2],    maxdims[2];


    ////////////////////////////////////////////////////////////
    // Open GEO file (contains meta data, lat/lon/zenith angle, etc)
    ////////////////////////////////////////////////////////////
    file_id = H5Fopen(ffn0, H5F_ACC_RDWR, H5P_DEFAULT);

    root_id = H5Gopen2(file_id, "/", H5P_DEFAULT);
    //printf("root_id=%lu\n", (long int)root_id);
    ss_id   = H5Gopen2(root_id, "S1", H5P_DEFAULT);
    //printf("s1_id=%lu\n", (long int)ss_id);

    dsetr_id = H5Dopen2(ss_id, "Latitude", H5P_DEFAULT);
    //printf("dsetr_id=%lu\n", (long int)dsetr_id);
    space_id = H5Dget_space(dsetr_id);
    //printf("space_id=%lu\n", (long int)space_id);
    type_id = H5Dget_type(dsetr_id);
    //printf("type_id=%lu\n", (long int)type_id);
    ndims = H5Sget_simple_extent_ndims(space_id);
    //printf("ndims=%d\n", ndims);
    status = H5Sget_simple_extent_dims(space_id, dims, maxdims);
    //printf("dims={%lu %lu}; maxdims={%lu %lu}\n", (long int)dims[0], 
    //        (long int)dims[1], (long int)maxdims[0], (long int)maxdims[1]);

    nscan0 = dims[0];
    nfov0 = dims[1];
    nchan0 = 9;

    nscan1 = dims[0];
    nfov1 = 161;
    nchan1 = 13;


    // init sat var
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

    float   *out_float_2d_p1, *out_float_2d;
    out_float_2d_p1 = init_1dc_float(nscan0*nfov0*nchan0, &out_float_2d);


    // lat
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                    H5P_DEFAULT, out_float_2d);
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)  {
            index = iscan * nfov0 + ifov + 30;
            lat[iscan][ifov] = out_float_2d[index];
        }
    }
    //printf("lat\n");
    //for (iscan=0; iscan<5; iscan++)  {
    //    for (ifov=0; ifov<5; ifov++)  {
    //        printf(" %f", lat[iscan][ifov]);
    //    }
    //    printf("\n");
    //}


    // lon
    dsetr_id = H5Dopen2(ss_id, "Longitude", H5P_DEFAULT);
    space_id = H5Dget_space(dsetr_id);
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                    H5P_DEFAULT, out_float_2d);
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)  {
            index = iscan * nfov0 + ifov + 30;
            lon[iscan][ifov] = out_float_2d[index];
        }
    }
    //printf("lon\n");
    //for (iscan=0; iscan<5; iscan++)  {
    //    for (ifov=0; ifov<5; ifov++)  {
    //        printf(" %f", lon[iscan][ifov]);
    //    }
    //    printf("\n");
    //}


    // at
    dsetr_id = H5Dopen2(ss_id, "Tc", H5P_DEFAULT);
    space_id = H5Dget_space(dsetr_id);
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                    H5P_DEFAULT, out_float_2d);

    for (ichan=0; ichan<nchan0; ichan++)  {
        for (iscan=0; iscan<nscan1; iscan++)  {
            for (ifov=0; ifov<nfov1; ifov++)  {
                index = nchan0 * (iscan * nfov0 + ifov + 30) + ichan;
                at[iscan][ifov][ichan] = out_float_2d[index];
            }
        }
        //if (ichan==0 || ichan==nchan0-1)  {
        //    printf("at ch %d\n", ichan);
        //    for (iscan=0; iscan<5; iscan++)  {
        //        for (ifov=0; ifov<5; ifov++)
        //            printf(" %f", at[iscan][ifov][ichan]);
        //        printf("\n");
        //    }
        //    getchar();
        //}
    }

    short int   year[nscan0], doy[nscan0];
    int         btime[nscan0];

    // year
    //year = ini_1d_arr_short(scanlines, NPIX);
    ss_id   = H5Gopen2(root_id, "S1/ScanTime", H5P_DEFAULT);
    //printf("dttm_id=%lu\n", (long int)ss_id);
    dsetr_id = H5Dopen2(ss_id, "Year", H5P_DEFAULT);
    space_id = H5Dget_space(dsetr_id);
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                    H5P_DEFAULT, out_float_2d);
    for (iscan=0; iscan<nscan1; iscan++)
        year[iscan] = round(out_float_2d[iscan]);
    //printf("year\n");
    //for (iscan=0; iscan<5; iscan++)
    //    printf(" %hd\n", year[iscan]);

    // doy
    //doy = ini_1d_arr_short(scanlines, NPIX);
    dsetr_id = H5Dopen2(ss_id, "DayOfYear", H5P_DEFAULT);
    space_id = H5Dget_space(dsetr_id);
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                    H5P_DEFAULT, out_float_2d);
    for (iscan=0; iscan<nscan1; iscan++)
        doy[iscan] = round(out_float_2d[iscan]);
    //printf("doy\n");
    //for (iscan=0; iscan<5; iscan++)
    //    printf(" %hd\n", doy[iscan]);

    //btime
    //btime = ini_1d_arr_float(scanlines, NPIX);
    dsetr_id = H5Dopen2(ss_id, "SecondOfDay", H5P_DEFAULT);
    space_id = H5Dget_space(dsetr_id);
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                    H5P_DEFAULT, out_float_2d);
    for (iscan=0; iscan<nscan1; iscan++)
        btime[iscan] = out_float_2d[iscan];
    //printf("btime\n");
    //for (iscan=0; iscan<5; iscan++)
    //    printf(" %d\n", btime[iscan]);

    // btjs
    long long int   jsec0;

    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)  {
            cal2jsec(year[iscan], 1, 1, 0, 0, 0, &jsec0);
            btjs[iscan][ifov] = jsec0 + (doy[iscan]-1)*86400 + btime[iscan];
        }
    }
    // qc
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)
            if (btjs[iscan][ifov] < 0)  {
                btjs[iscan][ifov] = -999;
                flag0[iscan][ifov] |= (1U<<8);
            }
    }
    //printf("btjs\n");
    //for (iscan=0; iscan<5; iscan++)
    //    printf(" %lld", btjs[iscan][0]);
    //printf("\n");
    //int     year1, mon1, day1, hour1, min1, sec1;
    //for (iscan=0; iscan<5; iscan++)  {
    //    for (ifov=0; ifov<1; ifov++)  {
    //        jsec2cal(btjs[iscan][ifov], &year1, &mon1, &day1, 
    //                &hour1, &min1, &sec1);
    //        printf("%d %d  %lld ", iscan, ifov, btjs[iscan][ifov]);
    //        printf("%04d %02d %02d %02d %02d %02d\n", 
    //                year1, mon1, day1, hour1, min1, sec1);
    //    }
    //}


    // start and end time
    jsec2cal(btjs[0][0], &dtse[0][0], &dtse[0][1], &dtse[0][2], 
            &dtse[0][3], &dtse[0][4], &dtse[0][5]);
    //printf("Start: %04d %02d %02d %02d %02d %02d\n", 
    //        dtse[0][0], dtse[0][1], dtse[0][2], dtse[0][3], 
    //        dtse[0][4], dtse[0][5]);
    jsec2cal(btjs[nscan1-1][nfov1-1], &dtse[1][0], &dtse[1][1], &dtse[1][2], 
            &dtse[1][3], &dtse[1][4], &dtse[1][5]);
    //printf("End  : %04d %02d %02d %02d %02d %02d\n", 
    //        dtse[1][0], dtse[1][1], dtse[1][2], dtse[1][3], 
    //        dtse[1][4], dtse[1][5]);

    // lza, sza
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)  {
            lza[iscan][ifov] = 53.1;
            sza[iscan][ifov] = 53.1;
        }
    }


    // s2
    ss_id   = H5Gopen2(root_id, "S2", H5P_DEFAULT);
    //printf("s2_id=%lu\n", (long int)ss_id);
    dsetr_id = H5Dopen2(ss_id, "Tc", H5P_DEFAULT);
    space_id = H5Dget_space(dsetr_id);
    status = H5Dread(dsetr_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                    H5P_DEFAULT, out_float_2d);

    int     nchant;
    nchant = nchan0;
    nchan0 = 4;

    for (ichan=0; ichan<nchan0; ichan++)  {
        for (iscan=0; iscan<nscan1; iscan++)  {
            for (ifov=0; ifov<nfov1; ifov++)  {
                index = nchan0 * (iscan * nfov0 + ifov + 30) + ichan;
                at[iscan][ifov][nchant+ichan] = out_float_2d[index];
            }
        }
        //if (ichan==0 || ichan==nchan0-1)  {
        //    printf("at ch %d\n", ichan);
        //    for (iscan=0; iscan<5; iscan++)  {
        //        for (ifov=0; ifov<5; ifov++)
        //            printf(" %f", at[iscan][ifov][nchant+ichan]);
        //        printf("\n");
        //    }
        //    getchar();
        //}
    }


    ///////////////////////////////////////////
    // qc
    int     tt;
    for (iscan=0; iscan<nscan1; iscan++)  {
        for (ifov=0; ifov<nfov1; ifov++)  {

            flag0[iscan][ifov] = 0;
            // time
            if (btjs[iscan][ifov] < 0)  {
                flag0[iscan][ifov] |= (1U<<8);
            }
            // location
            if (lon[iscan][ifov]<LON_MIN || lon[iscan][ifov]>LON_MAX
                    || lat[iscan][ifov]<LAT_MIN || lat[iscan][ifov]>LAT_MAX) {
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


    /////////////////////////////////////////////////////////           
    // set return variables
    /////////////////////////////////////////////////////////
    *nfovp = nfov1;     *nscanp = nscan1;    *nchanp = nchan1;
    *nfovap = nfova;    *nfova_p1p = nfova_p1;
    *flag0p = flag0;    *flag0_p1p = flag0_p1;

    return(0);
}




