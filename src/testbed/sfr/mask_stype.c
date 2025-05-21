/***********************************************************************
 *  Program Name      : mask_stype.c
 *  Type              : Subroutine
 *  Function          : Read topo data to determine surface type
 *  Input Files       : tbase.bin 
 *  Output Files      : None
 *  Subroutine Called : mask_stype(), rd_mask(), rd_latbox()
 *  Called by         : process.c
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


FILE    *openfs();


extern  float       **lon, **lat;
extern  int         **stype;
extern  float       *hd_flb, **ti_flb;


int mask_stype(int nscan, int nfov, int *nfova,  
        int MAP_ROWS, int MAP_COLS, int MASK_RESOLUTION, char **mask)
{
    const float lon0 = 180., lat0 = 90.;

    int         iscan, ifov, i, j;
    int         lat_pts, latbot_mask, lattop_mask;
    int         lonleft_mask, lonright_mask;
    long int    indx;
    float       alat, alon, fov_size, loncorr, sum;

    int         latbox_up[nfov], latbox_down[nfov];
    float       fovsize[nfov];

    // check
    //printf("MAP_ROWS, MAP_COLS, MASK_RESOLUTION: %d, %d, %d\n",
    //        MAP_ROWS, MAP_COLS, MASK_RESOLUTION);
    //printf("fovsize, latbox_up, latbox_down\n");
    for (ifov=0; ifov<nfov; ifov++)  {
        fovsize    [ifov] =       ti_flb[ifov][1];
        latbox_up  [ifov] = round(ti_flb[ifov][2]);
        latbox_down[ifov] = round(ti_flb[ifov][3]);
        //printf("%f  %f   %1d  %1f   %1d  %1f\n", 
        //        fovsize[ifov],     ti_flb[ifov][1], 
        //        latbox_up[ifov],   ti_flb[ifov][2], 
        //        latbox_down[ifov], ti_flb[ifov][3]);
    }
    //getchar();


    //-------------------------------------------------*
    // * determine stype
    // *-------------------------------------------------
    for (iscan=0; iscan<nscan; iscan++)  {
        for (ifov=0; ifov<nfov; ifov++)  {

            fov_size = fovsize[ifov];
            alat = lat[iscan][ifov];
            alon = lon[iscan][ifov];

            //-------------------------------------------------*
            // * Determine the coordinates of FOV boundaries 
            // * in the mask data coordinate system 
            // *-------------------------------------------------
            loncorr = fabs(1 / cos(M_PI*alat/180));
            lonleft_mask = (alon+lon0-fov_size*loncorr) * MASK_RESOLUTION;
            lonright_mask = (alon+lon0+fov_size*loncorr) * MASK_RESOLUTION;

            if (lonleft_mask < 0)
                lonleft_mask = 0;
            if (lonright_mask > MAP_COLS - 1)
                lonright_mask = MAP_COLS - 1;

            lat_pts = MAP_ROWS - (alat + lat0) * MASK_RESOLUTION;
            latbot_mask = lat_pts - latbox_up[ifov];
            lattop_mask = lat_pts + latbox_down[ifov];

            if (lattop_mask > MAP_ROWS - 1)
                lattop_mask = MAP_ROWS - 1;
            if (latbot_mask > MAP_ROWS - 1)
                latbot_mask = MAP_ROWS - 1;
            if (lattop_mask < 0)
                lattop_mask = 0;
            if (latbot_mask < 0)
                latbot_mask = 0;

            //-------------------------------------------------*
            // * Determine surface type of an FOV 
            // *-------------------------------------------------
            sum = 0.;
            indx = 0;

            if (lonleft_mask<0 && lonright_mask>MAP_COLS-1)  {
                for (i=latbot_mask; i<lattop_mask+1; i++)  {
                    for(j=0; j<MAP_COLS; j++)  {
                        if (mask[i][j] == 1)
                            sum = sum + 1.;
                        indx = indx + 1;
                    }
                }
            }

            else if (lonleft_mask < 0)  {
                for (i=latbot_mask; i<lattop_mask+1; i++)  {
                    for (j=0; j<lonright_mask+1; j++)  {
                        if (mask[i][j] == 1)
                            sum = sum + 1.;
                        indx = indx + 1;
                    }

                    for (j=MAP_COLS+lonleft_mask; j<MAP_COLS; j++)  {
                        if (mask[i][j] == 1)
                            sum = sum + 1.;
                        indx = indx + 1;
                    }
                }
            }

            else if (lonright_mask > MAP_COLS - 1)  {
                for (i=latbot_mask; i<lattop_mask+1; i++)  {
                    for (j=lonleft_mask; j<MAP_COLS; j++)  {
                        if (mask[i][j] == 1)
                            sum = sum + 1.;
                        indx = indx + 1;
                    }

                    for (j=0; j<lonright_mask-MAP_COLS+1; j++)  {
                        if (mask[i][j] == 1)
                            sum = sum + 1.;
                        indx = indx + 1;
                    }

                }
            }

            else  {
                for (i=latbot_mask; i<lattop_mask+1; i++)  {
                    for (j=lonleft_mask; j<lonright_mask+1; j++)  {
                        if (mask[i][j] == 1)
                            sum = sum + 1.;
                        indx = indx + 1;
                    }
                }
            }

            sum = sum / indx;

            //printf("iscan ifov sum : %d %d %f\n", iscan, ifov, sum);

            if (sum >= 0.99)            // land 
                stype[iscan][ifov] = 1;
            else if(sum <= 0.01)        // ocean
                stype[iscan][ifov] = 0;
            else                        // coast
                stype[iscan][ifov] = 2;

        }  // end of ifov
        //getchar();
    }  // end of iscan loop


    //for (iscan=0; iscan<nscan; iscan++)  {
    //    for (ifov=0; ifov<nfov; ifov++)  {
    //        printf("%d", stype[iscan][ifov]);
    //    }
    //    printf("\n");
    //}

    return(0);
}



int rd_mask(char *fname, int mask_row, int mask_col, char **mask)
{
    FILE    *fp;
    int     i1;

    fp = openfs(fname);
    for (i1=0; i1<mask_row; i1++)
        fread(mask[i1], sizeof(char), mask_col, fp);
    fclose(fp);
    //fp = fopen("mask.bin", "wb");
    //for (i1=0; i1<mask_row; i1++)
    //    fwrite(mask[i1], sizeof(char), mask_col, fp);
    //fclose(fp);

    return(0);
}




