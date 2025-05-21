/***************************************************************************
 *  Program Name      : fun_tis_nn.c
 *  Type              : subroutine
 *  Function          : neural network functions 
 *  Subroutine Called : sigmoid, rd_tis_nn, mlnn, sdnn_run,tbnn_run, 
 *                      iwpnn_run, tcnn_run, sfrnn_run 
 *  Called by         : snowfall(), cal_vars()
 *  Author            : Yongzhen Fan, yfan1236@umd.edu, 05/06/2021
 **************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

float sigmoid(float);
// *******************************************************************
// nn arrays
extern  int     *flg_rd_sdnn, *flg_rd_tbnn, *flg_rd_iwpnn, *flg_rd_tcnn, 
                *flg_rd_sfrnn;
extern  int     *flg_do_sdnn, *flg_do_tbnn, *flg_do_iwpnn, *flg_do_tcnn, 
                *flg_do_sfrnn;
extern  int     *flg_gb_sdnn, *flg_gb_tbnn, *flg_gb_iwpnn, *flg_gb_tcnn, 
                *flg_gb_sfrnn;
extern  int     *flg_us_sdnn, *flg_us_tbnn, *flg_us_iwpnn, *flg_us_tcnn, 
                *flg_us_sfrnn;
extern  int     *flg_ak_sdnn, *flg_ak_tbnn, *flg_ak_iwpnn, *flg_ak_tcnn, 
                *flg_ak_sfrnn;

extern  int     *nlayers_sdnn_gb, *layers_sdnn_gb, **varid_sdnn_gb, 
                *nntype_sdnn_gb;
extern  float   **norm_in_sdnn_gb, **norm_out_sdnn_gb, ***weights_sdnn_gb,
                **bias_sdnn_gb;
extern  int     *varid_threshold_adj_sdnn_gb, *threshold_adj_nsec_sdnn_gb;
extern  float   *threshold_adj_sec_sdnn_gb, *threshold_sdnn_gb, 
                *geolimit_sdnn_gb;

extern  int     *nlayers_sdnn_us, *layers_sdnn_us, **varid_sdnn_us, 
                *nntype_sdnn_us;
extern  float   **norm_in_sdnn_us, **norm_out_sdnn_us, 
                ***weights_sdnn_us,**bias_sdnn_us;
extern  int     *varid_threshold_adj_sdnn_us, *threshold_adj_nsec_sdnn_us;
extern  float   *threshold_adj_sec_sdnn_us, *threshold_sdnn_us, 
                *geolimit_sdnn_us;

extern  int     *nlayers_sdnn_ak, *layers_sdnn_ak, **varid_sdnn_ak, 
                *nntype_sdnn_ak;
extern  float   **norm_in_sdnn_ak, **norm_out_sdnn_ak, ***weights_sdnn_ak,
                **bias_sdnn_ak;
extern  int     *varid_threshold_adj_sdnn_ak, *threshold_adj_nsec_sdnn_ak;
extern  float   *threshold_adj_sec_sdnn_ak, *threshold_sdnn_ak, 
                *geolimit_sdnn_ak;

extern  int     *nlayers_tbnn_gb, *layers_tbnn_gb, **varid_tbnn_gb, 
                *nntype_tbnn_gb;
extern  float   **norm_in_tbnn_gb, **norm_out_tbnn_gb, ***weights_tbnn_gb, 
                **bias_tbnn_gb, *geolimit_tbnn_gb;

extern  int     *nlayers_tbnn_us, *layers_tbnn_us, **varid_tbnn_us, 
                *nntype_tbnn_us;
extern  float   **norm_in_tbnn_us, **norm_out_tbnn_us, ***weights_tbnn_us, 
                **bias_tbnn_us, *geolimit_tbnn_us;

extern  int     *nlayers_tbnn_ak, *layers_tbnn_ak, **varid_tbnn_ak, 
                *nntype_tbnn_ak;
extern  float   **norm_in_tbnn_ak, **norm_out_tbnn_ak, ***weights_tbnn_ak, 
                **bias_tbnn_ak, *geolimit_tbnn_ak;

extern  int     *nlayers_iwpnn_gb, *layers_iwpnn_gb, **varid_iwpnn_gb, 
                *nntype_iwpnn_gb;
extern  float   **norm_in_iwpnn_gb, **norm_out_iwpnn_gb, ***weights_iwpnn_gb, 
                **bias_iwpnn_gb, *geolimit_iwpnn_gb;

extern  int     *nlayers_iwpnn_us, *layers_iwpnn_us, **varid_iwpnn_us, 
                *nntype_iwpnn_us;
extern  float   **norm_in_iwpnn_us, **norm_out_iwpnn_us, ***weights_iwpnn_us, 
                **bias_iwpnn_us, *geolimit_iwpnn_us;

extern  int     *nlayers_iwpnn_ak, *layers_iwpnn_ak, **varid_iwpnn_ak, 
                *nntype_iwpnn_ak;
extern  float   **norm_in_iwpnn_ak, **norm_out_iwpnn_ak, ***weights_iwpnn_ak, 
                **bias_iwpnn_ak, *geolimit_iwpnn_ak;

extern  int     *nlayers_tcnn_gb, *layers_tcnn_gb, **varid_tcnn_gb, 
                *nntype_tcnn_gb;
extern  float   **norm_in_tcnn_gb, **norm_out_tcnn_gb, ***weights_tcnn_gb, 
                **bias_tcnn_gb, *geolimit_tcnn_gb;

extern  int     *nlayers_tcnn_us, *layers_tcnn_us, **varid_tcnn_us, 
                *nntype_tcnn_us;
extern  float   **norm_in_tcnn_us, **norm_out_tcnn_us, ***weights_tcnn_us, 
                **bias_tcnn_us, *geolimit_tcnn_us;

extern  int     *nlayers_tcnn_ak, *layers_tcnn_ak, **varid_tcnn_ak, 
                *nntype_tcnn_ak;
extern  float   **norm_in_tcnn_ak, **norm_out_tcnn_ak, ***weights_tcnn_ak, 
                **bias_tcnn_ak, *geolimit_tcnn_ak;

extern  int     *nlayers_sfrnn_gb, *layers_sfrnn_gb, **varid_sfrnn_gb, 
                *nntype_sfrnn_gb, *flg_post_adj_sfrnn_gb, 
                *post_adj_varid_sfrnn_gb;
extern  float   **norm_in_sfrnn_gb,**norm_out_sfrnn_gb, ***weights_sfrnn_gb, 
                **bias_sfrnn_gb, *post_adj_sfrnn_gb, *geolimit_sfrnn_gb;

extern  int     *nlayers_sfrnn_us, *layers_sfrnn_us, **varid_sfrnn_us, 
                *nntype_sfrnn_us, *flg_post_adj_sfrnn_us, 
                *post_adj_varid_sfrnn_us;
extern  float   **norm_in_sfrnn_us, **norm_out_sfrnn_us, ***weights_sfrnn_us, 
                **bias_sfrnn_us, *post_adj_sfrnn_us, *geolimit_sfrnn_us;

extern  int     *nlayers_sfrnn_ak, *layers_sfrnn_ak, **varid_sfrnn_ak, 
                *nntype_sfrnn_ak, *flg_post_adj_sfrnn_ak, 
                *post_adj_varid_sfrnn_ak;
extern  float   **norm_in_sfrnn_ak, **norm_out_sfrnn_ak, ***weights_sfrnn_ak, 
                **bias_sfrnn_ak, *post_adj_sfrnn_ak, *geolimit_sfrnn_ak;
// *******************************************************************

// read nn tis file
int rd_tis_nn(char *ffn)    
{  
    FILE    *nnfid;
    int     i,j,k;
    
    
    // read in tis_NN
    nnfid = fopen(ffn,"r");
    if (nnfid == NULL){
        printf("Could not open file %s\n", ffn);
        exit(1);        
    }
    printf("Successfully open file: %s\n", ffn);
    
    //read in 5 rd flags. if flag set, then read the nn data, otherwise skip.
    flg_rd_sdnn = (int*) malloc(sizeof(int)*1);
    if (flg_rd_sdnn == NULL) {
        perror("Error allocating memory for flg_rd_sdnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_rd_sdnn[0]);
    
    flg_rd_tbnn = (int*) malloc(sizeof(int)*1);
    if (flg_rd_tbnn == NULL) {
        perror("Error allocating memory for flg_rd_tbnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_rd_tbnn[0]);
    
    flg_rd_iwpnn = (int*) malloc(sizeof(int)*1);
    if (flg_rd_iwpnn == NULL) {
        perror("Error allocating memory for flg_rd_iwpnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_rd_iwpnn[0]);
    
    flg_rd_tcnn = (int*) malloc(sizeof(int)*1);
    if (flg_rd_tcnn == NULL) {
        perror("Error allocating memory for flg_rd_tcnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_rd_tcnn[0]);
    
    flg_rd_sfrnn = (int*) malloc(sizeof(int)*1);
    if (flg_rd_sfrnn == NULL) {
        perror("Error allocating memory for flg_rd_sfrnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_rd_sfrnn[0]);
    
    //read in 5 do flags. if flag set, then use nn in processing.
    flg_do_sdnn = (int*) malloc(sizeof(int)*1);
    if (flg_do_sdnn == NULL) {
        perror("Error allocating memory for flg_do_sdnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_do_sdnn[0]);
    // reset do flag if rd flag is not set
    if (*flg_rd_sdnn == 0) flg_do_sdnn[0] = 0;
    
    flg_do_tbnn = (int*) malloc(sizeof(int)*1);
    if (flg_do_tbnn == NULL) {
        perror("Error allocating memory for flg_do_tbnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_do_tbnn[0]);
    // reset do flag if rd flag is not set
    if (*flg_rd_tbnn == 0) flg_do_tbnn[0] = 0;
    
    flg_do_iwpnn = (int*) malloc(sizeof(int)*1);
    if (flg_do_iwpnn == NULL) {
        perror("Error allocating memory for flg_do_iwpnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_do_iwpnn[0]);
    // reset do flag if rd flag is not set
    if (*flg_rd_iwpnn == 0) flg_do_iwpnn[0] = 0;
    
    flg_do_tcnn = (int*) malloc(sizeof(int)*1);
    if (flg_do_tcnn == NULL) {
        perror("Error allocating memory for flg_do_tcnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_do_tcnn[0]);
    // reset do flag if rd flag is not set
    if (*flg_rd_tcnn == 0) flg_do_tcnn[0] = 0;
    
    flg_do_sfrnn = (int*) malloc(sizeof(int)*1);
    if (flg_do_sfrnn == NULL) {
        perror("Error allocating memory for flg_do_sfrnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_do_sfrnn[0]);
    // reset do flag if rd flag is not set
    if (*flg_rd_sfrnn == 0) flg_do_sfrnn[0] = 0;
    
    //read in 5 global NN flags. if flag set, then use global NN.
    flg_gb_sdnn = (int*) malloc(sizeof(int)*1);
    if (flg_gb_sdnn == NULL) {
        perror("Error allocating memory for flg_gb_sdnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_gb_sdnn[0]);
    
    flg_gb_tbnn = (int*) malloc(sizeof(int)*1);
    if (flg_gb_tbnn == NULL) {
        perror("Error allocating memory for flg_gb_tbnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_gb_tbnn[0]);
    
    flg_gb_iwpnn = (int*) malloc(sizeof(int)*1);
    if (flg_gb_iwpnn == NULL) {
        perror("Error allocating memory for flg_gb_iwpnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_gb_iwpnn[0]);
    
    flg_gb_tcnn = (int*) malloc(sizeof(int)*1);
    if (flg_gb_tcnn == NULL) {
        perror("Error allocating memory for flg_gb_tcnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_gb_tcnn[0]);
    
    flg_gb_sfrnn = (int*) malloc(sizeof(int)*1);
    if (flg_gb_sfrnn == NULL) {
        perror("Error allocating memory for flg_gb_sfrnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_gb_sfrnn[0]);
    
    //read in 5 CONUS NN flags. if flag set, then use CONUS NN.
    flg_us_sdnn = (int*) malloc(sizeof(int)*1);
    if (flg_us_sdnn == NULL) {
        perror("Error allocating memory for flg_us_sdnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_us_sdnn[0]);
    
    flg_us_tbnn = (int*) malloc(sizeof(int)*1);
    if (flg_us_tbnn == NULL) {
        perror("Error allocating memory for flg_us_tbnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_us_tbnn[0]);
    
    flg_us_iwpnn = (int*) malloc(sizeof(int)*1);
    if (flg_us_iwpnn == NULL) {
        perror("Error allocating memory for flg_us_iwpnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_us_iwpnn[0]);
    
    flg_us_tcnn = (int*) malloc(sizeof(int)*1);
    if (flg_us_tcnn == NULL) {
        perror("Error allocating memory for flg_us_tcnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_us_tcnn[0]);
    
    flg_us_sfrnn = (int*) malloc(sizeof(int)*1);
    if (flg_us_sfrnn == NULL) {
        perror("Error allocating memory for flg_us_sfrnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_us_sfrnn[0]);
    
    //read in 5 ALASKA NN flags. if flag set, then use ALASKA NN.
    flg_ak_sdnn = (int*) malloc(sizeof(int)*1);
    if (flg_ak_sdnn == NULL) {
        perror("Error allocating memory for flg_ak_sdnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_ak_sdnn[0]);
    
    flg_ak_tbnn = (int*) malloc(sizeof(int)*1);
    if (flg_ak_tbnn == NULL) {
        perror("Error allocating memory for flg_ak_tbnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_ak_tbnn[0]);
    
    flg_ak_iwpnn = (int*) malloc(sizeof(int)*1);
    if (flg_ak_iwpnn == NULL) {
        perror("Error allocating memory for flg_ak_iwpnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_ak_iwpnn[0]);
    
    flg_ak_tcnn = (int*) malloc(sizeof(int)*1);
    if (flg_ak_tcnn == NULL) {
        perror("Error allocating memory for flg_ak_tcnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_ak_tcnn[0]);
    
    flg_ak_sfrnn = (int*) malloc(sizeof(int)*1);
    if (flg_ak_sfrnn == NULL) {
        perror("Error allocating memory for flg_ak_sfrnn.");
        exit(1);
    }
    fscanf(nnfid,"%i",&flg_ak_sfrnn[0]);
    

    //read SDNN if rd flag is set
    if (*flg_rd_sdnn==1){        
        // read in global NN
        
        //read in nn type
        nntype_sdnn_gb = (int*) malloc(sizeof(int)*1);
        if (nntype_sdnn_gb == NULL) {
            perror("Error allocating memory for nntype_sdnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_sdnn_gb[0]);
        //read in number of layers
        nlayers_sdnn_gb = (int*) malloc(sizeof(int)*1);
        if (nlayers_sdnn_gb == NULL) {
            perror("Error allocating memory for nlayers_sdnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_sdnn_gb[0]);
        
        //allocate memory for layer array
        layers_sdnn_gb = malloc(sizeof(int) * nlayers_sdnn_gb[0]);
        if (layers_sdnn_gb == NULL) {
            perror("Error allocating memory for layer_sdnn_gb.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_sdnn_gb[0]; i++){        
            fscanf(nnfid,"%i",&layers_sdnn_gb[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_sdnn_gb = (float*) malloc(sizeof(float) * 4);
        if (geolimit_sdnn_gb == NULL) {
            perror("Error allocating memory for geolimit_sdnn_gb.");
            exit(1);
        } 
        fscanf(nnfid,"%f %f %f %f", &geolimit_sdnn_gb[0], 
                &geolimit_sdnn_gb[1], &geolimit_sdnn_gb[2], 
                &geolimit_sdnn_gb[3]);
        
        //allocate normalization array for input
        norm_in_sdnn_gb = (float**) malloc(sizeof(float*) * layers_sdnn_gb[0]);
        if (norm_in_sdnn_gb == NULL) {
            perror("Error allocating memory for norm_in_sdnn_gb.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_sdnn_gb[0]; i++){
            norm_in_sdnn_gb[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_sdnn_gb[i] == NULL) {
                perror("Error allocating memory for norm_in_sdnn_gb.");
                exit(1);
            } 
            fscanf(nnfid,"%f %f", &norm_in_sdnn_gb[i][0], 
                    &norm_in_sdnn_gb[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_sdnn_gb = (float**) malloc(sizeof(float*) 
                * layers_sdnn_gb[nlayers_sdnn_gb[0]-1]);
        if (norm_out_sdnn_gb == NULL) {
            perror("Error allocating memory for norm_out_sdnn_gb.");
            exit(1);
        }    
        //init. norm_out data
        for (i=0; i<layers_sdnn_gb[nlayers_sdnn_gb[0]-1]; i++){
            norm_out_sdnn_gb[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_sdnn_gb[i] == NULL) {
                perror("Error allocating memory for norm_out_sdnn_gb.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_out_sdnn_gb[i][0], 
                    &norm_out_sdnn_gb[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_sdnn_gb = malloc(sizeof(float **) * nlayers_sdnn_gb[0]-1);
        bias_sdnn_gb    = malloc(sizeof(float *) * nlayers_sdnn_gb[0]-1);
        for (i=0; i<nlayers_sdnn_gb[0]-1; i++) {
            weights_sdnn_gb[i] = malloc(sizeof(float*) * layers_sdnn_gb[i+1]);
            bias_sdnn_gb[i]    = malloc(sizeof(float) * layers_sdnn_gb[i+1]);
            for (j=0;j<layers_sdnn_gb[i+1]; j++) {
                weights_sdnn_gb[i][j] = malloc(sizeof(float) 
                        * layers_sdnn_gb[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_sdnn_gb[0]-1; i++){
            for (j=0;j<layers_sdnn_gb[i+1]; j++){
                for (k=0; k<layers_sdnn_gb[i]; k++){
                    fscanf(nnfid,"%f",&weights_sdnn_gb[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_sdnn_gb[i][j]);
            }       
        }
            
        //allocate varid_sdnn array
        varid_sdnn_gb = (int**) malloc(sizeof(int*) * layers_sdnn_gb[0]);
        if (!varid_sdnn_gb) {
            perror("Error allocating memory for varid_sdnn_gb.");
            exit(1);
        }
        //read in varid_sdnn
        for (i=0; i<layers_sdnn_gb[0]; i++){
            varid_sdnn_gb[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_sdnn_gb[i] == NULL) {
                perror("Error allocating memory for varid_sdnn_gb.");
                exit(1);
            }
            fscanf(nnfid,"%i %i %i %i", &varid_sdnn_gb[i][0], 
                    &varid_sdnn_gb[i][1], 
                    &varid_sdnn_gb[i][2],&varid_sdnn_gb[i][3]);
            //printf("%i %i %i %i\n", varid_sdnn[i][0], varid_sdnn[i][1], 
            //        varid_sdnn[i][2],varid_sdnn[i][3]);
        }
        //read in therohold adjustment 
        varid_threshold_adj_sdnn_gb = (int*) malloc(sizeof(int) * 4);
        if (varid_threshold_adj_sdnn_gb == NULL) {
            perror("Error allocating memory for varid_threshold_adj_sdnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i %i %i %i", &varid_threshold_adj_sdnn_gb[0], 
                &varid_threshold_adj_sdnn_gb[1], 
                &varid_threshold_adj_sdnn_gb[2], 
                &varid_threshold_adj_sdnn_gb[3]);
                                    
        if(varid_threshold_adj_sdnn_gb[0]>=0){
            // only read in the threshold adjustment if varid is valid
            threshold_adj_nsec_sdnn_gb = (int*) malloc(sizeof(int)*1);
            if (threshold_adj_nsec_sdnn_gb == NULL) {
                perror("Error alloc memory for threshold_adj_nsec_sdnn_gb.");
                exit(1);
            }
            fscanf(nnfid,"%i",&threshold_adj_nsec_sdnn_gb[0]);
            
            threshold_adj_sec_sdnn_gb = (float*) 
                malloc(sizeof(float)*threshold_adj_nsec_sdnn_gb[0]-1);
            if (threshold_adj_nsec_sdnn_gb == NULL) {
                perror("Error alloc memory for threshold_adj_sec_sdnn_gb.");
                exit(1);
            }
            for (i=0; i<threshold_adj_nsec_sdnn_gb[0]-1; i++){
                fscanf(nnfid,"%f",&threshold_adj_sec_sdnn_gb[i]);
            }
            
            threshold_sdnn_gb = (float*) 
                malloc(sizeof(float)*threshold_adj_nsec_sdnn_gb[0]);
            if (threshold_sdnn_gb == NULL) {
                perror("Error allocating memory for threshold_sdnn_gb.");
                exit(1);
            }
            for (i=0; i<threshold_adj_nsec_sdnn_gb[0]; i++){
                fscanf(nnfid,"%f",&threshold_sdnn_gb[i]);
            }            
        }

        // read in CONUS NN
        
        //read in nn type
        nntype_sdnn_us = (int*) malloc(sizeof(int)*1);
        if (nntype_sdnn_us == NULL) {
            perror("Error allocating memory for nntype_sdnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_sdnn_us[0]);
        //read in number of layers
        nlayers_sdnn_us = (int*) malloc(sizeof(int)*1);
        if (nlayers_sdnn_us == NULL) {
            perror("Error allocating memory for nlayers_sdnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_sdnn_us[0]);
        
        //allocate memory for layer array
        layers_sdnn_us = malloc(sizeof(int) * nlayers_sdnn_us[0]);
        if (layers_sdnn_us == NULL) {
            perror("Error allocating memory for layer_sdnn_us.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_sdnn_us[0]; i++){        
            fscanf(nnfid,"%i",&layers_sdnn_us[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_sdnn_us = (float*) malloc(sizeof(float) * 4);
        if (geolimit_sdnn_us == NULL) {
            perror("Error allocating memory for geolimit_sdnn_us.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_sdnn_us[0], 
                &geolimit_sdnn_us[1], &geolimit_sdnn_us[2], 
                &geolimit_sdnn_us[3]);
        
        
        //allocate normalization array for input
        norm_in_sdnn_us = (float**) malloc(sizeof(float*) * layers_sdnn_us[0]);
        if (norm_in_sdnn_us == NULL) {
            perror("Error allocating memory for norm_in_sdnn_us.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_sdnn_us[0]; i++){
            norm_in_sdnn_us[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_sdnn_us[i] == NULL) {
                perror("Error allocating memory for norm_in_sdnn_us.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_sdnn_us[i][0], 
                    &norm_in_sdnn_us[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_sdnn_us = (float**) malloc(sizeof(float*) 
                * layers_sdnn_us[nlayers_sdnn_us[0]-1]);
        if (norm_out_sdnn_us == NULL) {
            perror("Error allocating memory for norm_out_sdnn_us.");
            exit(1);
        }    
        //init. norm_out data
        for (i=0; i<layers_sdnn_us[nlayers_sdnn_us[0]-1]; i++){
            norm_out_sdnn_us[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_sdnn_us[i] == NULL) {
                perror("Error allocating memory for norm_out_sdnn_us.");
                exit(1);
            } 
            fscanf(nnfid,"%f %f", &norm_out_sdnn_us[i][0], 
                    &norm_out_sdnn_us[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_sdnn_us = malloc(sizeof(float **) * nlayers_sdnn_us[0]-1);
        bias_sdnn_us    = malloc(sizeof(float *) * nlayers_sdnn_us[0]-1);
        for (i=0; i<nlayers_sdnn_us[0]-1; i++){
            weights_sdnn_us[i] = malloc(sizeof(float*) * layers_sdnn_us[i+1]);
            bias_sdnn_us[i]    = malloc(sizeof(float) * layers_sdnn_us[i+1]);
            for (j=0;j<layers_sdnn_us[i+1]; j++){
                weights_sdnn_us[i][j] = malloc(sizeof(float) 
                        * layers_sdnn_us[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_sdnn_us[0]-1; i++){
            for (j=0;j<layers_sdnn_us[i+1]; j++){
                for (k=0; k<layers_sdnn_us[i]; k++){
                    fscanf(nnfid,"%f",&weights_sdnn_us[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_sdnn_us[i][j]);
            }       
        }
            
        //allocate varid_sdnn array
        varid_sdnn_us = (int**) malloc(sizeof(int*) * layers_sdnn_us[0]);
        if (!varid_sdnn_us) {
            perror("Error allocating memory for varid_sdnn_us.");
            exit(1);
        }
        //read in varid_sdnn
        for (i=0; i<layers_sdnn_us[0]; i++){
            varid_sdnn_us[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_sdnn_us[i] == NULL) {
                perror("Error allocating memory for varid_sdnn_us.");
                exit(1);
            }
            fscanf(nnfid,"%i %i %i %i", &varid_sdnn_us[i][0], 
                    &varid_sdnn_us[i][1], &varid_sdnn_us[i][2], 
                    &varid_sdnn_us[i][3]);
            //printf("%i %i %i %i\n", varid_sdnn[i][0], varid_sdnn[i][1], 
            //        varid_sdnn[i][2],varid_sdnn[i][3]);
        }
        //read in therohold adjustment 
        varid_threshold_adj_sdnn_us = (int*) malloc(sizeof(int) * 4);
        if (varid_threshold_adj_sdnn_us == NULL) {
            perror("Error allocating memory for varid_threshold_adj_sdnn_us.");
            exit(1);
        }
        fscanf(nnfid, "%i %i %i %i", &varid_threshold_adj_sdnn_us[0], 
                &varid_threshold_adj_sdnn_us[1], 
                &varid_threshold_adj_sdnn_us[2], 
                &varid_threshold_adj_sdnn_us[3]);
                                    
        if(varid_threshold_adj_sdnn_us[0]>=0){
            // only read in the threshold adjustment if varid is valid
            threshold_adj_nsec_sdnn_us = (int*) malloc(sizeof(int)*1);
            if (threshold_adj_nsec_sdnn_us == NULL) {
                perror("Error alloc memory for threshold_adj_nsec_sdnn_us.");
                exit(1);
            }
            fscanf(nnfid,"%i",&threshold_adj_nsec_sdnn_us[0]);
            
            threshold_adj_sec_sdnn_us = (float*) 
                malloc(sizeof(float)*threshold_adj_nsec_sdnn_us[0]-1);
            if (threshold_adj_nsec_sdnn_us == NULL) {
                perror("Error alloc memory for threshold_adj_sec_sdnn_us.");
                exit(1);
            }
            for (i=0; i<threshold_adj_nsec_sdnn_us[0]-1; i++){
                fscanf(nnfid,"%f",&threshold_adj_sec_sdnn_us[i]);
            }
            
            threshold_sdnn_us = (float*) 
                malloc(sizeof(float)*threshold_adj_nsec_sdnn_us[0]);
            if (threshold_sdnn_us == NULL) {
                perror("Error allocating memory for threshold_sdnn_us.");
                exit(1);
            }
            for (i=0; i<threshold_adj_nsec_sdnn_us[0]; i++){
                fscanf(nnfid,"%f",&threshold_sdnn_us[i]);
            }            
        }
        
        // read in Alaska NN
        
        //read in nn type
        nntype_sdnn_ak = (int*) malloc(sizeof(int)*1);
        if (nntype_sdnn_ak == NULL) {
            perror("Error allocating memory for nntype_sdnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_sdnn_ak[0]);
        //read in number of layers
        nlayers_sdnn_ak = (int*) malloc(sizeof(int)*1);
        if (nlayers_sdnn_ak == NULL) {
            perror("Error allocating memory for nlayers_sdnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_sdnn_ak[0]);
        
        //allocate memory for layer array
        layers_sdnn_ak = malloc(sizeof(int) * nlayers_sdnn_ak[0]);
        if (layers_sdnn_ak == NULL) {
            perror("Error allocating memory for layer_sdnn_ak.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_sdnn_ak[0]; i++){        
            fscanf(nnfid,"%i",&layers_sdnn_ak[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_sdnn_ak = (float*) malloc(sizeof(float) * 4);
        if (geolimit_sdnn_ak == NULL) {
            perror("Error allocating memory for geolimit_sdnn_ak.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_sdnn_ak[0], 
                &geolimit_sdnn_ak[1], &geolimit_sdnn_ak[2],
                &geolimit_sdnn_ak[3]);
        
        
        //allocate normalization array for input
        norm_in_sdnn_ak = (float**) malloc(sizeof(float*) * layers_sdnn_ak[0]);
        if (norm_in_sdnn_ak == NULL) {
            perror("Error allocating memory for norm_in_sdnn_ak.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_sdnn_ak[0]; i++){
            norm_in_sdnn_ak[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_sdnn_ak[i] == NULL) {
                perror("Error allocating memory for norm_in_sdnn_ak.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_sdnn_ak[i][0], 
                    &norm_in_sdnn_ak[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_sdnn_ak = (float**) malloc(sizeof(float*) 
                * layers_sdnn_ak[nlayers_sdnn_ak[0]-1]);
        if (norm_out_sdnn_ak == NULL) {
            perror("Error allocating memory for norm_out_sdnn_ak.");
            exit(1);
        }    
        //init. norm_out data
        for (i=0; i<layers_sdnn_ak[nlayers_sdnn_ak[0]-1]; i++){
            norm_out_sdnn_ak[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_sdnn_ak[i] == NULL) {
                perror("Error allocating memory for norm_out_sdnn_ak.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_out_sdnn_ak[i][0], 
                    &norm_out_sdnn_ak[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_sdnn_ak = malloc(sizeof(float **) * nlayers_sdnn_ak[0]-1);
        bias_sdnn_ak    = malloc(sizeof(float *) * nlayers_sdnn_ak[0]-1);
        for (i=0; i<nlayers_sdnn_ak[0]-1; i++){
            weights_sdnn_ak[i] = malloc(sizeof(float*) * layers_sdnn_ak[i+1]);
            bias_sdnn_ak[i]    = malloc(sizeof(float) * layers_sdnn_ak[i+1]);
            for (j=0;j<layers_sdnn_ak[i+1]; j++){
                weights_sdnn_ak[i][j] = malloc(sizeof(float) 
                        * layers_sdnn_ak[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_sdnn_ak[0]-1; i++){
            for (j=0;j<layers_sdnn_ak[i+1]; j++){
                for (k=0; k<layers_sdnn_ak[i]; k++){
                    fscanf(nnfid,"%f",&weights_sdnn_ak[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_sdnn_ak[i][j]);
            }       
        }
            
        //allocate varid_sdnn array
        varid_sdnn_ak = (int**) malloc(sizeof(int*) * layers_sdnn_ak[0]);
        if (!varid_sdnn_ak) {
            perror("Error allocating memory for varid_sdnn_ak.");
            exit(1);
        }
        //read in varid_sdnn
        for (i=0; i<layers_sdnn_ak[0]; i++){
            varid_sdnn_ak[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_sdnn_ak[i] == NULL) {
                perror("Error allocating memory for varid_sdnn_ak.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_sdnn_ak[i][0], 
                    &varid_sdnn_ak[i][1], &varid_sdnn_ak[i][2], 
                    &varid_sdnn_ak[i][3]);
            //printf("%i %i %i %i\n", varid_sdnn[i][0], varid_sdnn[i][1], 
            //        varid_sdnn[i][2],varid_sdnn[i][3]);
        }
        //read in therohold adjustment 
        varid_threshold_adj_sdnn_ak = (int*) malloc(sizeof(int) * 4);
        if (varid_threshold_adj_sdnn_ak == NULL) {
            perror("Error allocating memory for varid_threshold_adj_sdnn_ak.");
            exit(1);
        }
        fscanf(nnfid, "%i %i %i %i", &varid_threshold_adj_sdnn_ak[0], 
                &varid_threshold_adj_sdnn_ak[1], 
                &varid_threshold_adj_sdnn_ak[2], 
                &varid_threshold_adj_sdnn_ak[3]);
                                    
        if(varid_threshold_adj_sdnn_ak[0]>=0){
            // only read in the threshold adjustment if varid is valid
            threshold_adj_nsec_sdnn_ak = (int*) malloc(sizeof(int)*1);
            if (threshold_adj_nsec_sdnn_ak == NULL) {
                perror("Error alloc memory for threshold_adj_nsec_sdnn_ak.");
                exit(1);
            }
            fscanf(nnfid,"%i",&threshold_adj_nsec_sdnn_ak[0]);
            
            threshold_adj_sec_sdnn_ak = (float*) 
                malloc(sizeof(float)*threshold_adj_nsec_sdnn_ak[0]-1);
            if (threshold_adj_nsec_sdnn_ak == NULL) {
                perror("Error alloc memory for threshold_adj_sec_sdnn_ak.");
                exit(1);
            }
            for (i=0; i<threshold_adj_nsec_sdnn_ak[0]-1; i++){
                fscanf(nnfid,"%f",&threshold_adj_sec_sdnn_ak[i]);
            }
            
            threshold_sdnn_ak = (float*) 
                malloc(sizeof(float)*threshold_adj_nsec_sdnn_ak[0]);
            if (threshold_sdnn_ak == NULL) {
                perror("Error allocating memory for threshold_sdnn_ak.");
                exit(1);
            }
            for (i=0; i<threshold_adj_nsec_sdnn_ak[0]; i++){
                fscanf(nnfid,"%f",&threshold_sdnn_ak[i]);
            }            
        }
    }
    
    //read TbNN if rd flag is set
    if (*flg_rd_tbnn==1){
        // read in global NN
        
        //read in nn type
        nntype_tbnn_gb = (int*) malloc(sizeof(int)*1);
        if (nntype_tbnn_gb == NULL) {
            perror("Error allocating memory for nntype_tbnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_tbnn_gb[0]);
        //read in number of layers
        nlayers_tbnn_gb = (int*) malloc(sizeof(int)*1);
        if (nlayers_tbnn_gb == NULL) {
            perror("Error allocating memory for nlayers_tbnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_tbnn_gb[0]);
        
        //allocate memory for layer array
        layers_tbnn_gb = malloc(sizeof(int) * nlayers_tbnn_gb[0]);
        if (layers_tbnn_gb == NULL) {
            perror("Error allocating memory for layer_tbnn_gb.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_tbnn_gb[0]; i++){        
            fscanf(nnfid,"%i",&layers_tbnn_gb[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_tbnn_gb = (float*) malloc(sizeof(float) * 4);
        if (geolimit_tbnn_gb == NULL) {
            perror("Error allocating memory for geolimit_tbnn_gb.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_tbnn_gb[0], 
                &geolimit_tbnn_gb[1], &geolimit_tbnn_gb[2], 
                &geolimit_tbnn_gb[3]);
        
        //allocate normalization array for input
        norm_in_tbnn_gb = (float**) malloc(sizeof(float*) * layers_tbnn_gb[0]);
        if (norm_in_tbnn_gb == NULL) {
            perror("Error allocating memory for norm_in_tbnn_gb.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_tbnn_gb[0]; i++){
            norm_in_tbnn_gb[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_tbnn_gb[i] == NULL) {
                perror("Error allocating memory for norm_in_tbnn_gb.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_tbnn_gb[i][0], 
                    &norm_in_tbnn_gb[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_tbnn_gb = (float**) malloc(sizeof(float*) 
                * layers_tbnn_gb[nlayers_tbnn_gb[0]-1]);
        if (norm_out_tbnn_gb == NULL) {
            perror("Error allocating memory for norm_out_tbnn_gb.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_tbnn_gb[nlayers_tbnn_gb[0]-1]; i++){
            norm_out_tbnn_gb[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_tbnn_gb[i] == NULL) {
                perror("Error allocating memory for norm_out_tbnn_gb.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_out_tbnn_gb[i][0], 
                    &norm_out_tbnn_gb[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_tbnn_gb = malloc(sizeof(float **) * nlayers_tbnn_gb[0]-1);
        bias_tbnn_gb    = malloc(sizeof(float *) * nlayers_tbnn_gb[0]-1);
        for (i=0; i<nlayers_tbnn_gb[0]-1; i++){
            weights_tbnn_gb[i] = malloc(sizeof(float*) * layers_tbnn_gb[i+1]);
            bias_tbnn_gb[i]    = malloc(sizeof(float) * layers_tbnn_gb[i+1]);
            for (j=0;j<layers_tbnn_gb[i+1]; j++){
                weights_tbnn_gb[i][j] = malloc(sizeof(float) 
                        * layers_tbnn_gb[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_tbnn_gb[0]-1; i++){
            for (j=0;j<layers_tbnn_gb[i+1]; j++){
                for (k=0; k<layers_tbnn_gb[i]; k++){
                    fscanf(nnfid,"%f",&weights_tbnn_gb[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_tbnn_gb[i][j]);
            }       
        }
            
        //allocate varid array
        varid_tbnn_gb = (int**) malloc(sizeof(int*) * layers_tbnn_gb[0]);
        if (!varid_tbnn_gb) {
            perror("Error allocating memory for varid_tbnn_gb.");
            exit(1);
        }
        //read in varid_tbnn_gb
        for (i=0; i<layers_tbnn_gb[0]; i++){
            varid_tbnn_gb[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_tbnn_gb[i] == NULL) {
                perror("Error allocating memory for varid_tbnn_gb.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_tbnn_gb[i][0], 
                    &varid_tbnn_gb[i][1], &varid_tbnn_gb[i][2], 
                    &varid_tbnn_gb[i][3]);
            //printf("%i %i %i %i\n", varid_tbnn_gb[i][0], varid_tbnn_gb[i][1], 
            //        varid_tbnn_gb[i][2],varid_tbnn_gb[i][3]);
        }
        
        // read in CONUS NN
        
        //read in nn type
        nntype_tbnn_us = (int*) malloc(sizeof(int)*1);
        if (nntype_tbnn_us == NULL) {
            perror("Error allocating memory for nntype_tbnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_tbnn_us[0]);
        //read in number of layers
        nlayers_tbnn_us = (int*) malloc(sizeof(int)*1);
        if (nlayers_tbnn_us == NULL) {
            perror("Error allocating memory for nlayers_tbnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_tbnn_us[0]);
        
        //allocate memory for layer array
        layers_tbnn_us = malloc(sizeof(int) * nlayers_tbnn_us[0]);
        if (layers_tbnn_us == NULL) {
            perror("Error allocating memory for layer_tbnn_us.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_tbnn_us[0]; i++){        
            fscanf(nnfid,"%i",&layers_tbnn_us[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_tbnn_us = (float*) malloc(sizeof(float) * 4);
        if (geolimit_tbnn_us == NULL) {
            perror("Error allocating memory for geolimit_tbnn_us.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_tbnn_us[0], 
                &geolimit_tbnn_us[1], &geolimit_tbnn_us[2], 
                &geolimit_tbnn_us[3]);
        
        //allocate normalization array for input
        norm_in_tbnn_us = (float**) malloc(sizeof(float*) * layers_tbnn_us[0]);
        if (norm_in_tbnn_us == NULL) {
            perror("Error allocating memory for norm_in_tbnn_us.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_tbnn_us[0]; i++){
            norm_in_tbnn_us[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_tbnn_us[i] == NULL) {
                perror("Error allocating memory for norm_in_tbnn_us.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_tbnn_us[i][0], 
                    &norm_in_tbnn_us[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_tbnn_us = (float**) malloc(sizeof(float*) 
                * layers_tbnn_us[nlayers_tbnn_us[0]-1]);
        if (norm_out_tbnn_us == NULL) {
            perror("Error allocating memory for norm_out_tbnn_us.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_tbnn_us[nlayers_tbnn_us[0]-1]; i++){
            norm_out_tbnn_us[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_tbnn_us[i] == NULL) {
                perror("Error allocating memory for norm_out_tbnn_us.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_out_tbnn_us[i][0], 
                    &norm_out_tbnn_us[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_tbnn_us = malloc(sizeof(float **) * nlayers_tbnn_us[0]-1);
        bias_tbnn_us    = malloc(sizeof(float *) * nlayers_tbnn_us[0]-1);
        for (i=0; i<nlayers_tbnn_us[0]-1; i++){
            weights_tbnn_us[i] = malloc(sizeof(float*) * layers_tbnn_us[i+1]);
            bias_tbnn_us[i]    = malloc(sizeof(float) * layers_tbnn_us[i+1]);
            for (j=0;j<layers_tbnn_us[i+1]; j++){
                weights_tbnn_us[i][j] = malloc(sizeof(float) 
                        * layers_tbnn_us[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_tbnn_us[0]-1; i++){
            for (j=0;j<layers_tbnn_us[i+1]; j++){
                for (k=0; k<layers_tbnn_us[i]; k++){
                    fscanf(nnfid,"%f",&weights_tbnn_us[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_tbnn_us[i][j]);
            }       
        }
            
        //allocate varid array
        varid_tbnn_us = (int**) malloc(sizeof(int*) * layers_tbnn_us[0]);
        if (!varid_tbnn_us) {
            perror("Error allocating memory for varid_tbnn_us.");
            exit(1);
        }
        //read in varid_tbnn_us
        for (i=0; i<layers_tbnn_us[0]; i++){
            varid_tbnn_us[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_tbnn_us[i] == NULL) {
                perror("Error allocating memory for varid_tbnn_us.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_tbnn_us[i][0], 
                    &varid_tbnn_us[i][1], &varid_tbnn_us[i][2], 
                    &varid_tbnn_us[i][3]);
            //printf("%i %i %i %i\n", varid_tbnn_us[i][0], varid_tbnn_us[i][1], 
            //        varid_tbnn_us[i][2],varid_tbnn_us[i][3]);
        }
        
        // read in ALASKA NN
        
        //read in nn type
        nntype_tbnn_ak = (int*) malloc(sizeof(int)*1);
        if (nntype_tbnn_ak == NULL) {
            perror("Error allocating memory for nntype_tbnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_tbnn_ak[0]);
        //read in number of layers
        nlayers_tbnn_ak = (int*) malloc(sizeof(int)*1);
        if (nlayers_tbnn_ak == NULL) {
            perror("Error allocating memory for nlayers_tbnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_tbnn_ak[0]);
        
        //allocate memory for layer array
        layers_tbnn_ak = malloc(sizeof(int) * nlayers_tbnn_ak[0]);
        if (layers_tbnn_ak == NULL) {
            perror("Error allocating memory for layer_tbnn_ak.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_tbnn_ak[0]; i++){        
            fscanf(nnfid,"%i",&layers_tbnn_ak[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_tbnn_ak = (float*) malloc(sizeof(float) * 4);
        if (geolimit_tbnn_ak == NULL) {
            perror("Error allocating memory for geolimit_tbnn_ak.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_tbnn_ak[0], 
                &geolimit_tbnn_ak[1], &geolimit_tbnn_ak[2], 
                &geolimit_tbnn_ak[3]);
        
        //allocate normalization array for input
        norm_in_tbnn_ak = (float**) malloc(sizeof(float*) * layers_tbnn_ak[0]);
        if (norm_in_tbnn_ak == NULL) {
            perror("Error allocating memory for norm_in_tbnn_ak.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_tbnn_ak[0]; i++){
            norm_in_tbnn_ak[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_tbnn_ak[i] == NULL) {
                perror("Error allocating memory for norm_in_tbnn_ak.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_tbnn_ak[i][0], 
                    &norm_in_tbnn_ak[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_tbnn_ak = (float**) malloc(sizeof(float*) 
                * layers_tbnn_ak[nlayers_tbnn_ak[0]-1]);
        if (norm_out_tbnn_ak == NULL) {
            perror("Error allocating memory for norm_out_tbnn_ak.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_tbnn_ak[nlayers_tbnn_ak[0]-1]; i++){
            norm_out_tbnn_ak[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_tbnn_ak[i] == NULL) {
                perror("Error allocating memory for norm_out_tbnn_ak.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_out_tbnn_ak[i][0], 
                    &norm_out_tbnn_ak[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_tbnn_ak = malloc(sizeof(float **) * nlayers_tbnn_ak[0]-1);
        bias_tbnn_ak    = malloc(sizeof(float *) * nlayers_tbnn_ak[0]-1);
        for (i=0; i<nlayers_tbnn_ak[0]-1; i++){
            weights_tbnn_ak[i] = malloc(sizeof(float*) * layers_tbnn_ak[i+1]);
            bias_tbnn_ak[i]    = malloc(sizeof(float) * layers_tbnn_ak[i+1]);
            for (j=0;j<layers_tbnn_ak[i+1]; j++){
                weights_tbnn_ak[i][j] = malloc(sizeof(float) 
                        * layers_tbnn_ak[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_tbnn_ak[0]-1; i++){
            for (j=0;j<layers_tbnn_ak[i+1]; j++){
                for (k=0; k<layers_tbnn_ak[i]; k++){
                    fscanf(nnfid,"%f",&weights_tbnn_ak[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_tbnn_ak[i][j]);
            }       
        }
            
        //allocate varid array
        varid_tbnn_ak = (int**) malloc(sizeof(int*) * layers_tbnn_ak[0]);
        if (!varid_tbnn_ak) {
            perror("Error allocating memory for varid_tbnn_ak.");
            exit(1);
        }
        //read in varid_tbnn_ak
        for (i=0; i<layers_tbnn_ak[0]; i++){
            varid_tbnn_ak[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_tbnn_ak[i] == NULL) {
                perror("Error allocating memory for varid_tbnn_ak.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_tbnn_ak[i][0], 
                    &varid_tbnn_ak[i][1], 
                    &varid_tbnn_ak[i][2],&varid_tbnn_ak[i][3]);
            //printf("%i %i %i %i\n", varid_tbnn_ak[i][0], varid_tbnn_ak[i][1], 
            //        varid_tbnn_ak[i][2],varid_tbnn_ak[i][3]);
        }
    }
    
    //read IWPNN if rd flag is set
    if (*flg_rd_iwpnn==1){
        // read in global NN
        
        //read in nn type
        nntype_iwpnn_gb = (int*) malloc(sizeof(int)*1);
        if (nntype_iwpnn_gb == NULL) {
            perror("Error allocating memory for nntype_iwpnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_iwpnn_gb[0]);
        //read in number of layers
        nlayers_iwpnn_gb = (int*) malloc(sizeof(int)*1);
        if (nlayers_iwpnn_gb == NULL) {
            perror("Error allocating memory for nlayers_iwpnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_iwpnn_gb[0]);
        
        //allocate memory for layer array
        layers_iwpnn_gb = malloc(sizeof(int) * nlayers_iwpnn_gb[0]);
        if (layers_iwpnn_gb == NULL) {
            perror("Error allocating memory for layer_iwpnn_gb.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_iwpnn_gb[0]; i++){        
            fscanf(nnfid,"%i",&layers_iwpnn_gb[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_iwpnn_gb = (float*) malloc(sizeof(float) * 4);
        if (geolimit_iwpnn_gb == NULL) {
            perror("Error allocating memory for geolimit_iwpnn_gb.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_iwpnn_gb[0], 
                &geolimit_iwpnn_gb[1], &geolimit_iwpnn_gb[2], 
                &geolimit_iwpnn_gb[3]);
        
        //allocate normalization array for input
        norm_in_iwpnn_gb = (float**) malloc(sizeof(float*) 
                * layers_iwpnn_gb[0]);
        if (norm_in_iwpnn_gb == NULL) {
            perror("Error allocating memory for norm_in_iwpnn_gb.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_iwpnn_gb[0]; i++){
            norm_in_iwpnn_gb[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_iwpnn_gb[i] == NULL) {
                perror("Error allocating memory for norm_in_iwpnn_gb.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_iwpnn_gb[i][0], 
                    &norm_in_iwpnn_gb[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_iwpnn_gb = (float**) malloc(sizeof(float*) 
                * layers_iwpnn_gb[nlayers_iwpnn_gb[0]-1]);
        if (norm_out_iwpnn_gb == NULL) {
            perror("Error allocating memory for norm_out_iwpnn_gb.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_iwpnn_gb[nlayers_iwpnn_gb[0]-1]; i++){
            norm_out_iwpnn_gb[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_iwpnn_gb[i] == NULL) {
                perror("Error allocating memory for norm_out_iwpnn_gb.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_out_iwpnn_gb[i][0], 
                    &norm_out_iwpnn_gb[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_iwpnn_gb = malloc(sizeof(float **) * nlayers_iwpnn_gb[0]-1);
        bias_iwpnn_gb    = malloc(sizeof(float *) * nlayers_iwpnn_gb[0]-1);
        for (i=0; i<nlayers_iwpnn_gb[0]-1; i++){
            weights_iwpnn_gb[i] = malloc(sizeof(float*) * layers_iwpnn_gb[i+1]);
            bias_iwpnn_gb[i]    = malloc(sizeof(float) * layers_iwpnn_gb[i+1]);
            for (j=0;j<layers_iwpnn_gb[i+1]; j++){
                weights_iwpnn_gb[i][j] = malloc(sizeof(float) 
                        * layers_iwpnn_gb[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_iwpnn_gb[0]-1; i++){
            for (j=0;j<layers_iwpnn_gb[i+1]; j++){
                for (k=0; k<layers_iwpnn_gb[i]; k++){
                    fscanf(nnfid,"%f",&weights_iwpnn_gb[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_iwpnn_gb[i][j]);
            }       
        }
            
        //allocate varid array
        varid_iwpnn_gb = (int**) malloc(sizeof(int*) * layers_iwpnn_gb[0]);
        if (!varid_iwpnn_gb) {
            perror("Error allocating memory for varid_iwpnn_gb.");
            exit(1);
        }
        //read in varid_iwpnn_gb
        for (i=0; i<layers_iwpnn_gb[0]; i++){
            varid_iwpnn_gb[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_iwpnn_gb[i] == NULL) {
                perror("Error allocating memory for varid_iwpnn_gb.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_iwpnn_gb[i][0], 
                    &varid_iwpnn_gb[i][1], &varid_iwpnn_gb[i][2], 
                    &varid_iwpnn_gb[i][3]);
            //printf("%i %i %i %i\n", varid_iwpnn_gb[i][0], 
            //        varid_iwpnn_gb[i][1], varid_iwpnn_gb[i][2],
            //        varid_iwpnn_gb[i][3]);
        }
        
        // read in CONUS NN
        
        //read in nn type
        nntype_iwpnn_us = (int*) malloc(sizeof(int)*1);
        if (nntype_iwpnn_us == NULL) {
            perror("Error allocating memory for nntype_iwpnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_iwpnn_us[0]);
        //read in number of layers
        nlayers_iwpnn_us = (int*) malloc(sizeof(int)*1);
        if (nlayers_iwpnn_us == NULL) {
            perror("Error allocating memory for nlayers_iwpnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_iwpnn_us[0]);
        
        //allocate memory for layer array
        layers_iwpnn_us = malloc(sizeof(int) * nlayers_iwpnn_us[0]);
        if (layers_iwpnn_us == NULL) {
            perror("Error allocating memory for layer_iwpnn_us.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_iwpnn_us[0]; i++){        
            fscanf(nnfid,"%i",&layers_iwpnn_us[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_iwpnn_us = (float*) malloc(sizeof(float) * 4);
        if (geolimit_iwpnn_us == NULL) {
            perror("Error allocating memory for geolimit_iwpnn_us.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_iwpnn_us[0], 
                &geolimit_iwpnn_us[1], &geolimit_iwpnn_us[2], 
                &geolimit_iwpnn_us[3]);
        
        //allocate normalization array for input
        norm_in_iwpnn_us = (float**) malloc(sizeof(float*) 
                * layers_iwpnn_us[0]);
        if (norm_in_iwpnn_us == NULL) {
            perror("Error allocating memory for norm_in_iwpnn_us.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_iwpnn_us[0]; i++){
            norm_in_iwpnn_us[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_iwpnn_us[i] == NULL) {
                perror("Error allocating memory for norm_in_iwpnn_us.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_iwpnn_us[i][0], 
                    &norm_in_iwpnn_us[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_iwpnn_us = (float**) malloc(sizeof(float*) 
                * layers_iwpnn_us[nlayers_iwpnn_us[0]-1]);
        if (norm_out_iwpnn_us == NULL) {
            perror("Error allocating memory for norm_out_iwpnn_us.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_iwpnn_us[nlayers_iwpnn_us[0]-1]; i++){
            norm_out_iwpnn_us[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_iwpnn_us[i] == NULL) {
                perror("Error allocating memory for norm_out_iwpnn_us.");
                exit(1);
            } 
            fscanf(nnfid,"%f %f", &norm_out_iwpnn_us[i][0], 
                    &norm_out_iwpnn_us[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_iwpnn_us = malloc(sizeof(float **) * nlayers_iwpnn_us[0]-1);
        bias_iwpnn_us    = malloc(sizeof(float *) * nlayers_iwpnn_us[0]-1);
        for (i=0; i<nlayers_iwpnn_us[0]-1; i++){
            weights_iwpnn_us[i] = malloc(sizeof(float*) * layers_iwpnn_us[i+1]);
            bias_iwpnn_us[i]    = malloc(sizeof(float) * layers_iwpnn_us[i+1]);
            for (j=0;j<layers_iwpnn_us[i+1]; j++){
                weights_iwpnn_us[i][j] = malloc(sizeof(float) 
                        * layers_iwpnn_us[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_iwpnn_us[0]-1; i++){
            for (j=0;j<layers_iwpnn_us[i+1]; j++){
                for (k=0; k<layers_iwpnn_us[i]; k++){
                    fscanf(nnfid,"%f",&weights_iwpnn_us[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_iwpnn_us[i][j]);
            }       
        }
            
        //allocate varid array
        varid_iwpnn_us = (int**) malloc(sizeof(int*) * layers_iwpnn_us[0]);
        if (!varid_iwpnn_us) {
            perror("Error allocating memory for varid_iwpnn_us.");
            exit(1);
        }
        //read in varid_iwpnn_us
        for (i=0; i<layers_iwpnn_us[0]; i++){
            varid_iwpnn_us[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_iwpnn_us[i] == NULL) {
                perror("Error allocating memory for varid_iwpnn_us.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_iwpnn_us[i][0], 
                    &varid_iwpnn_us[i][1], &varid_iwpnn_us[i][2], 
                    &varid_iwpnn_us[i][3]);
            //printf("%i %i %i %i\n", varid_iwpnn_us[i][0], 
            //        varid_iwpnn_us[i][1], varid_iwpnn_us[i][2], 
            //        varid_iwpnn_us[i][3]);
        }
        
        // read in ALASKA NN
        
        //read in nn type
        nntype_iwpnn_ak = (int*) malloc(sizeof(int)*1);
        if (nntype_iwpnn_ak == NULL) {
            perror("Error allocating memory for nntype_iwpnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_iwpnn_ak[0]);
        //read in number of layers
        nlayers_iwpnn_ak = (int*) malloc(sizeof(int)*1);
        if (nlayers_iwpnn_ak == NULL) {
            perror("Error allocating memory for nlayers_iwpnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_iwpnn_ak[0]);
        
        //allocate memory for layer array
        layers_iwpnn_ak = malloc(sizeof(int) * nlayers_iwpnn_ak[0]);
        if (layers_iwpnn_ak == NULL) {
            perror("Error allocating memory for layer_iwpnn_ak.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_iwpnn_ak[0]; i++){        
            fscanf(nnfid,"%i",&layers_iwpnn_ak[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_iwpnn_ak = (float*) malloc(sizeof(float) * 4);
        if (geolimit_iwpnn_ak == NULL) {
            perror("Error allocating memory for geolimit_iwpnn_ak.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_iwpnn_ak[0], 
                &geolimit_iwpnn_ak[1], &geolimit_iwpnn_ak[2], 
                &geolimit_iwpnn_ak[3]);
        
        //allocate normalization array for input
        norm_in_iwpnn_ak = (float**) malloc(sizeof(float*) 
                * layers_iwpnn_ak[0]);
        if (norm_in_iwpnn_ak == NULL) {
            perror("Error allocating memory for norm_in_iwpnn_ak.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_iwpnn_ak[0]; i++){
            norm_in_iwpnn_ak[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_iwpnn_ak[i] == NULL) {
                perror("Error allocating memory for norm_in_iwpnn_ak.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_iwpnn_ak[i][0], 
                    &norm_in_iwpnn_ak[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_iwpnn_ak = (float**) malloc(sizeof(float*) 
                * layers_iwpnn_ak[nlayers_iwpnn_ak[0]-1]);
        if (norm_out_iwpnn_ak == NULL) {
            perror("Error allocating memory for norm_out_iwpnn_ak.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_iwpnn_ak[nlayers_iwpnn_ak[0]-1]; i++){
            norm_out_iwpnn_ak[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_iwpnn_ak[i] == NULL) {
                perror("Error allocating memory for norm_out_iwpnn_ak.");
                exit(1);
            } 
            fscanf(nnfid,"%f %f", &norm_out_iwpnn_ak[i][0], 
                    &norm_out_iwpnn_ak[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_iwpnn_ak = malloc(sizeof(float **) * nlayers_iwpnn_ak[0]-1);
        bias_iwpnn_ak    = malloc(sizeof(float *) * nlayers_iwpnn_ak[0]-1);
        for (i=0; i<nlayers_iwpnn_ak[0]-1; i++){
            weights_iwpnn_ak[i] = malloc(sizeof(float*) * layers_iwpnn_ak[i+1]);
            bias_iwpnn_ak[i]    = malloc(sizeof(float) * layers_iwpnn_ak[i+1]);
            for (j=0;j<layers_iwpnn_ak[i+1]; j++){
                weights_iwpnn_ak[i][j] = malloc(sizeof(float) 
                        * layers_iwpnn_ak[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_iwpnn_ak[0]-1; i++){
            for (j=0;j<layers_iwpnn_ak[i+1]; j++){
                for (k=0; k<layers_iwpnn_ak[i]; k++){
                    fscanf(nnfid,"%f",&weights_iwpnn_ak[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_iwpnn_ak[i][j]);
            }       
        }
            
        //allocate varid array
        varid_iwpnn_ak = (int**) malloc(sizeof(int*) * layers_iwpnn_ak[0]);
        if (!varid_iwpnn_ak) {
            perror("Error allocating memory for varid_iwpnn_ak.");
            exit(1);
        }
        //read in varid_iwpnn_ak
        for (i=0; i<layers_iwpnn_ak[0]; i++){
            varid_iwpnn_ak[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_iwpnn_ak[i] == NULL) {
                perror("Error allocating memory for varid_iwpnn_ak.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_iwpnn_ak[i][0], 
                    &varid_iwpnn_ak[i][1], &varid_iwpnn_ak[i][2], 
                    &varid_iwpnn_ak[i][3]);
            //printf("%i %i %i %i\n", varid_iwpnn_ak[i][0], 
            //        varid_iwpnn_ak[i][1], varid_iwpnn_ak[i][2], 
            //        varid_iwpnn_ak[i][3]);
        }
    }
    
    //read TcNN if rd flag is set
    if (*flg_rd_tcnn==1){
        // read in global NN
        
        //read in nn type
        nntype_tcnn_gb = (int*) malloc(sizeof(int)*1);
        if (nntype_tcnn_gb == NULL) {
            perror("Error allocating memory for nntype_tcnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_tcnn_gb[0]);
        //read in number of layers
        nlayers_tcnn_gb = (int*) malloc(sizeof(int)*1);
        if (nlayers_tcnn_gb == NULL) {
            perror("Error allocating memory for nlayers_tcnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_tcnn_gb[0]);
        
        //allocate memory for layer array
        layers_tcnn_gb = malloc(sizeof(int) * nlayers_tcnn_gb[0]);
        if (layers_tcnn_gb == NULL) {
            perror("Error allocating memory for layer_tcnn_gb.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_tcnn_gb[0]; i++){        
            fscanf(nnfid,"%i",&layers_tcnn_gb[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_tcnn_gb = (float*) malloc(sizeof(float) * 4);
        if (geolimit_tcnn_gb == NULL) {
            perror("Error allocating memory for geolimit_tcnn_gb.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_tcnn_gb[0], 
                &geolimit_tcnn_gb[1], &geolimit_tcnn_gb[2],
                &geolimit_tcnn_gb[3]);
        
        //allocate normalization array for input
        norm_in_tcnn_gb = (float**) malloc(sizeof(float*) * layers_tcnn_gb[0]);
        if (norm_in_tcnn_gb == NULL) {
            perror("Error allocating memory for norm_in_tcnn_gb.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_tcnn_gb[0]; i++){
            norm_in_tcnn_gb[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_tcnn_gb[i] == NULL) {
                perror("Error allocating memory for norm_in_tcnn_gb.");
                exit(1);
            } 
            fscanf(nnfid,"%f %f", &norm_in_tcnn_gb[i][0], 
                    &norm_in_tcnn_gb[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_tcnn_gb = (float**) malloc(sizeof(float*) 
                * layers_tcnn_gb[nlayers_tcnn_gb[0]-1]);
        if (norm_out_tcnn_gb == NULL) {
            perror("Error allocating memory for norm_out_tcnn_gb.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_tcnn_gb[nlayers_tcnn_gb[0]-1]; i++){
            norm_out_tcnn_gb[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_tcnn_gb[i] == NULL) {
                perror("Error allocating memory for norm_out_tcnn_gb.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_out_tcnn_gb[i][0], 
                    &norm_out_tcnn_gb[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_tcnn_gb = malloc(sizeof(float **) * nlayers_tcnn_gb[0]-1);
        bias_tcnn_gb    = malloc(sizeof(float *) * nlayers_tcnn_gb[0]-1);
        for (i=0; i<nlayers_tcnn_gb[0]-1; i++){
            weights_tcnn_gb[i] = malloc(sizeof(float*) * layers_tcnn_gb[i+1]);
            bias_tcnn_gb[i]    = malloc(sizeof(float) * layers_tcnn_gb[i+1]);
            for (j=0;j<layers_tcnn_gb[i+1]; j++){
                weights_tcnn_gb[i][j] = malloc(sizeof(float) 
                        * layers_tcnn_gb[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_tcnn_gb[0]-1; i++){
            for (j=0;j<layers_tcnn_gb[i+1]; j++){
                for (k=0; k<layers_tcnn_gb[i]; k++){
                    fscanf(nnfid,"%f",&weights_tcnn_gb[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_tcnn_gb[i][j]);
            }       
        }
            
        //allocate varid array
        varid_tcnn_gb = (int**) malloc(sizeof(int*) * layers_tcnn_gb[0]);
        if (!varid_tcnn_gb) {
            perror("Error allocating memory for varid_tcnn_gb.");
            exit(1);
        }
        //read in varid_tcnn_gb
        for (i=0; i<layers_tcnn_gb[0]; i++){
            varid_tcnn_gb[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_tcnn_gb[i] == NULL) {
                perror("Error allocating memory for varid_tcnn_gb.");
                exit(1);
            }
            fscanf(nnfid,"%i %i %i %i", &varid_tcnn_gb[i][0], 
                    &varid_tcnn_gb[i][1], &varid_tcnn_gb[i][2], 
                    &varid_tcnn_gb[i][3]);
            //printf("%i %i %i %i\n", varid_tcnn_gb[i][0], varid_tcnn_gb[i][1], 
            //        varid_tcnn_gb[i][2],varid_tcnn_gb[i][3]);
        }
        
        // read in CONUS NN
        
        //read in nn type
        nntype_tcnn_us = (int*) malloc(sizeof(int)*1);
        if (nntype_tcnn_us == NULL) {
            perror("Error allocating memory for nntype_tcnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_tcnn_us[0]);
        //read in number of layers
        nlayers_tcnn_us = (int*) malloc(sizeof(int)*1);
        if (nlayers_tcnn_us == NULL) {
            perror("Error allocating memory for nlayers_tcnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_tcnn_us[0]);
        
        //allocate memory for layer array
        layers_tcnn_us = malloc(sizeof(int) * nlayers_tcnn_us[0]);
        if (layers_tcnn_us == NULL) {
            perror("Error allocating memory for layer_tcnn_us.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_tcnn_us[0]; i++){        
            fscanf(nnfid,"%i",&layers_tcnn_us[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_tcnn_us = (float*) malloc(sizeof(float) * 4);
        if (geolimit_tcnn_us == NULL) {
            perror("Error allocating memory for geolimit_tcnn_us.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_tcnn_us[0], 
                &geolimit_tcnn_us[1], &geolimit_tcnn_us[2], 
                &geolimit_tcnn_us[3]);
        
        //allocate normalization array for input
        norm_in_tcnn_us = (float**) malloc(sizeof(float*) * layers_tcnn_us[0]);
        if (norm_in_tcnn_us == NULL) {
            perror("Error allocating memory for norm_in_tcnn_us.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_tcnn_us[0]; i++){
            norm_in_tcnn_us[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_tcnn_us[i] == NULL) {
                perror("Error allocating memory for norm_in_tcnn_us.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_tcnn_us[i][0], 
                    &norm_in_tcnn_us[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_tcnn_us = (float**) malloc(sizeof(float*) 
                * layers_tcnn_us[nlayers_tcnn_us[0]-1]);
        if (norm_out_tcnn_us == NULL) {
            perror("Error allocating memory for norm_out_tcnn_us.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_tcnn_us[nlayers_tcnn_us[0]-1]; i++){
            norm_out_tcnn_us[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_tcnn_us[i] == NULL) {
                perror("Error allocating memory for norm_out_tcnn_us.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_out_tcnn_us[i][0], 
                    &norm_out_tcnn_us[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_tcnn_us = malloc(sizeof(float **) * nlayers_tcnn_us[0]-1);
        bias_tcnn_us    = malloc(sizeof(float *) * nlayers_tcnn_us[0]-1);
        for (i=0; i<nlayers_tcnn_us[0]-1; i++){
            weights_tcnn_us[i] = malloc(sizeof(float*) * layers_tcnn_us[i+1]);
            bias_tcnn_us[i]    = malloc(sizeof(float) * layers_tcnn_us[i+1]);
            for (j=0;j<layers_tcnn_us[i+1]; j++){
                weights_tcnn_us[i][j] = malloc(sizeof(float) 
                        * layers_tcnn_us[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_tcnn_us[0]-1; i++){
            for (j=0;j<layers_tcnn_us[i+1]; j++){
                for (k=0; k<layers_tcnn_us[i]; k++){
                    fscanf(nnfid,"%f",&weights_tcnn_us[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_tcnn_us[i][j]);
            }       
        }
            
        //allocate varid array
        varid_tcnn_us = (int**) malloc(sizeof(int*) * layers_tcnn_us[0]);
        if (!varid_tcnn_us) {
            perror("Error allocating memory for varid_tcnn_us.");
            exit(1);
        }
        //read in varid_tcnn_us
        for (i=0; i<layers_tcnn_us[0]; i++){
            varid_tcnn_us[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_tcnn_us[i] == NULL) {
                perror("Error allocating memory for varid_tcnn_us.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_tcnn_us[i][0], 
                    &varid_tcnn_us[i][1], &varid_tcnn_us[i][2], 
                    &varid_tcnn_us[i][3]);
            //printf("%i %i %i %i\n", varid_tcnn_us[i][0], varid_tcnn_us[i][1], 
            //        varid_tcnn_us[i][2],varid_tcnn_us[i][3]);
        }
        
        // read in ALASKA NN
        
        //read in nn type
        nntype_tcnn_ak = (int*) malloc(sizeof(int)*1);
        if (nntype_tcnn_ak == NULL) {
            perror("Error allocating memory for nntype_tcnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_tcnn_ak[0]);
        //read in number of layers
        nlayers_tcnn_ak = (int*) malloc(sizeof(int)*1);
        if (nlayers_tcnn_ak == NULL) {
            perror("Error allocating memory for nlayers_tcnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_tcnn_ak[0]);
        
        //allocate memory for layer array
        layers_tcnn_ak = malloc(sizeof(int) * nlayers_tcnn_ak[0]);
        if (layers_tcnn_ak == NULL) {
            perror("Error allocating memory for layer_tcnn_ak.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_tcnn_ak[0]; i++){        
            fscanf(nnfid,"%i",&layers_tcnn_ak[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_tcnn_ak = (float*) malloc(sizeof(float) * 4);
        if (geolimit_tcnn_ak == NULL) {
            perror("Error allocating memory for geolimit_tcnn_ak.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_tcnn_ak[0], 
                &geolimit_tcnn_ak[1], &geolimit_tcnn_ak[2],
                &geolimit_tcnn_ak[3]);
        
        //allocate normalization array for input
        norm_in_tcnn_ak = (float**) malloc(sizeof(float*) * layers_tcnn_ak[0]);
        if (norm_in_tcnn_ak == NULL) {
            perror("Error allocating memory for norm_in_tcnn_ak.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_tcnn_ak[0]; i++){
            norm_in_tcnn_ak[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_tcnn_ak[i] == NULL) {
                perror("Error allocating memory for norm_in_tcnn_ak.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_tcnn_ak[i][0], 
                    &norm_in_tcnn_ak[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_tcnn_ak = (float**) malloc(sizeof(float*) 
                * layers_tcnn_ak[nlayers_tcnn_ak[0]-1]);
        if (norm_out_tcnn_ak == NULL) {
            perror("Error allocating memory for norm_out_tcnn_ak.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_tcnn_ak[nlayers_tcnn_ak[0]-1]; i++){
            norm_out_tcnn_ak[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_tcnn_ak[i] == NULL) {
                perror("Error allocating memory for norm_out_tcnn_ak.");
                exit(1);
            } 
            fscanf(nnfid,"%f %f", &norm_out_tcnn_ak[i][0], 
                    &norm_out_tcnn_ak[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_tcnn_ak = malloc(sizeof(float **) * nlayers_tcnn_ak[0]-1);
        bias_tcnn_ak    = malloc(sizeof(float *) * nlayers_tcnn_ak[0]-1);
        for (i=0; i<nlayers_tcnn_ak[0]-1; i++){
            weights_tcnn_ak[i] = malloc(sizeof(float*) * layers_tcnn_ak[i+1]);
            bias_tcnn_ak[i]    = malloc(sizeof(float) * layers_tcnn_ak[i+1]);
            for (j=0;j<layers_tcnn_ak[i+1]; j++){
                weights_tcnn_ak[i][j] = malloc(sizeof(float) 
                        * layers_tcnn_ak[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_tcnn_ak[0]-1; i++){
            for (j=0;j<layers_tcnn_ak[i+1]; j++){
                for (k=0; k<layers_tcnn_ak[i]; k++){
                    fscanf(nnfid,"%f",&weights_tcnn_ak[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_tcnn_ak[i][j]);
            }       
        }
            
        //allocate varid array
        varid_tcnn_ak = (int**) malloc(sizeof(int*) * layers_tcnn_ak[0]);
        if (!varid_tcnn_ak) {
            perror("Error allocating memory for varid_tcnn_ak.");
            exit(1);
        }
        //read in varid_tcnn_ak
        for (i=0; i<layers_tcnn_ak[0]; i++){
            varid_tcnn_ak[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_tcnn_ak[i] == NULL) {
                perror("Error allocating memory for varid_tcnn_ak.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_tcnn_ak[i][0], 
                    &varid_tcnn_ak[i][1], &varid_tcnn_ak[i][2], 
                    &varid_tcnn_ak[i][3]);
            //printf("%i %i %i %i\n", varid_tcnn_ak[i][0], varid_tcnn_ak[i][1], 
            //        varid_tcnn_ak[i][2], varid_tcnn_ak[i][3]);
        }
    }
    
    //read SFRNN if rd flag is set
    if (*flg_rd_sfrnn==1){
        // read in global NN
        
        //read in nn type
        nntype_sfrnn_gb = (int*) malloc(sizeof(int)*1);
        if (nntype_sfrnn_gb == NULL) {
            perror("Error allocating memory for nntype_sfrnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_sfrnn_gb[0]);
        //read in number of layers
        nlayers_sfrnn_gb = (int*) malloc(sizeof(int)*1);
        if (nlayers_sfrnn_gb == NULL) {
            perror("Error allocating memory for nlayers_sfrnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_sfrnn_gb[0]);
        
        //allocate memory for layer array
        layers_sfrnn_gb = malloc(sizeof(int) * nlayers_sfrnn_gb[0]);
        if (layers_sfrnn_gb == NULL) {
            perror("Error allocating memory for layer_sfrnn_gb.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_sfrnn_gb[0]; i++){        
            fscanf(nnfid,"%i",&layers_sfrnn_gb[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_sfrnn_gb = (float*) malloc(sizeof(float) * 4);
        if (geolimit_sfrnn_gb == NULL) {
            perror("Error allocating memory for geolimit_sfrnn_gb.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_sfrnn_gb[0], 
                &geolimit_sfrnn_gb[1], &geolimit_sfrnn_gb[2], 
                &geolimit_sfrnn_gb[3]);
        
        //allocate normalization array for input
        norm_in_sfrnn_gb = (float**) malloc(sizeof(float*) 
                * layers_sfrnn_gb[0]);
        if (norm_in_sfrnn_gb == NULL) {
            perror("Error allocating memory for norm_in_sfrnn_gb.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_sfrnn_gb[0]; i++){
            norm_in_sfrnn_gb[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_sfrnn_gb[i] == NULL) {
                perror("Error allocating memory for norm_in_sfrnn_gb.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_sfrnn_gb[i][0], 
                    &norm_in_sfrnn_gb[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_sfrnn_gb = (float**) malloc(sizeof(float*) 
                * layers_sfrnn_gb[nlayers_sfrnn_gb[0]-1]);
        if (norm_out_sfrnn_gb == NULL) {
            perror("Error allocating memory for norm_out_sfrnn_gb.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_sfrnn_gb[nlayers_sfrnn_gb[0]-1]; i++){
            norm_out_sfrnn_gb[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_sfrnn_gb[i] == NULL) {
                perror("Error allocating memory for norm_out_sfrnn_gb.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_out_sfrnn_gb[i][0], 
                    &norm_out_sfrnn_gb[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_sfrnn_gb = malloc(sizeof(float **) * nlayers_sfrnn_gb[0]-1);
        bias_sfrnn_gb    = malloc(sizeof(float *) * nlayers_sfrnn_gb[0]-1);
        for (i=0; i<nlayers_sfrnn_gb[0]-1; i++){
            weights_sfrnn_gb[i] = malloc(sizeof(float*) * layers_sfrnn_gb[i+1]);
            bias_sfrnn_gb[i]    = malloc(sizeof(float) * layers_sfrnn_gb[i+1]);
            for (j=0;j<layers_sfrnn_gb[i+1]; j++){
                weights_sfrnn_gb[i][j] = malloc(sizeof(float) 
                        * layers_sfrnn_gb[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_sfrnn_gb[0]-1; i++){
            for (j=0;j<layers_sfrnn_gb[i+1]; j++){
                for (k=0; k<layers_sfrnn_gb[i]; k++){
                    fscanf(nnfid,"%f",&weights_sfrnn_gb[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_sfrnn_gb[i][j]);
            }       
        }
            
        //allocate varid array
        varid_sfrnn_gb = (int**) malloc(sizeof(int*) * layers_sfrnn_gb[0]);
        if (!varid_sfrnn_gb) {
            perror("Error allocating memory for varid_sfrnn_gb.");
            exit(1);
        }
        //read in varid_sfrnn_gb
        for (i=0; i<layers_sfrnn_gb[0]; i++){
            varid_sfrnn_gb[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_sfrnn_gb[i] == NULL) {
                perror("Error allocating memory for varid_sfrnn_gb.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_sfrnn_gb[i][0], 
                    &varid_sfrnn_gb[i][1], &varid_sfrnn_gb[i][2], 
                    &varid_sfrnn_gb[i][3]);
            //printf("%i %i %i %i\n", varid_sfrnn_gb[i][0], 
            //        varid_sfrnn_gb[i][1], varid_sfrnn_gb[i][2], 
            //        varid_sfrnn_gb[i][3]);
        }
        
        //read in post adjustment flag  
        flg_post_adj_sfrnn_gb = (int*) malloc(sizeof(int)*1);
        if (flg_post_adj_sfrnn_gb == NULL) {
            perror("Error allocating memory for flg_post_adj_sfrnn_gb.");
            exit(1);
        }
        fscanf(nnfid,"%i",&flg_post_adj_sfrnn_gb[0]);
        
        post_adj_varid_sfrnn_gb = (int*) malloc(sizeof(int)*2);
        if (post_adj_varid_sfrnn_gb == NULL) {
            perror("Error allocating memory for post_adj_varid_sfrnn_gb.");
            exit(1);
        }
        for (i=0; i<2; i++) {
            fscanf(nnfid,"%i",&post_adj_varid_sfrnn_gb[i]);
        }  

        //read in post adjustment info if flag set
        post_adj_sfrnn_gb = (float*) malloc(sizeof(float)*6);
        if (post_adj_sfrnn_gb == NULL) {
            perror("Error allocating memory for post_adj_sfrnn_gb.");
            exit(1);
        }
        if (flg_post_adj_sfrnn_gb[0]==1){
            for (i=0; i<6; i++) {
                fscanf(nnfid,"%f",&post_adj_sfrnn_gb[i]);
            }
        }
        else{
            for (i=0; i<6; i++) {
                post_adj_sfrnn_gb[i]=-99;
            }
        }

        // read in CONUS NN
        
        //read in nn type
        nntype_sfrnn_us = (int*) malloc(sizeof(int)*1);
        if (nntype_sfrnn_us == NULL) {
            perror("Error allocating memory for nntype_sfrnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_sfrnn_us[0]);
        //read in number of layers
        nlayers_sfrnn_us = (int*) malloc(sizeof(int)*1);
        if (nlayers_sfrnn_us == NULL) {
            perror("Error allocating memory for nlayers_sfrnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_sfrnn_us[0]);
        
        //allocate memory for layer array
        layers_sfrnn_us = malloc(sizeof(int) * nlayers_sfrnn_us[0]);
        if (layers_sfrnn_us == NULL) {
            perror("Error allocating memory for layer_sfrnn_us.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_sfrnn_us[0]; i++){        
            fscanf(nnfid,"%i",&layers_sfrnn_us[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_sfrnn_us = (float*) malloc(sizeof(float) * 4);
        if (geolimit_sfrnn_us == NULL) {
            perror("Error allocating memory for geolimit_sfrnn_us.");
            exit(1);
        } 
        fscanf(nnfid, "%f %f %f %f", &geolimit_sfrnn_us[0], 
                &geolimit_sfrnn_us[1], &geolimit_sfrnn_us[2],
                &geolimit_sfrnn_us[3]);
        
        //allocate normalization array for input
        norm_in_sfrnn_us = (float**) malloc(sizeof(float*) 
                * layers_sfrnn_us[0]);
        if (norm_in_sfrnn_us == NULL) {
            perror("Error allocating memory for norm_in_sfrnn_us.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_sfrnn_us[0]; i++){
            norm_in_sfrnn_us[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_sfrnn_us[i] == NULL) {
                perror("Error allocating memory for norm_in_sfrnn_us.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_sfrnn_us[i][0], 
                    &norm_in_sfrnn_us[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_sfrnn_us = (float**) malloc(sizeof(float*) 
                * layers_sfrnn_us[nlayers_sfrnn_us[0]-1]);
        if (norm_out_sfrnn_us == NULL) {
            perror("Error allocating memory for norm_out_sfrnn_us.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_sfrnn_us[nlayers_sfrnn_us[0]-1]; i++){
            norm_out_sfrnn_us[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_sfrnn_us[i] == NULL) {
                perror("Error allocating memory for norm_out_sfrnn_us.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_out_sfrnn_us[i][0], 
                    &norm_out_sfrnn_us[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_sfrnn_us = malloc(sizeof(float **) * nlayers_sfrnn_us[0]-1);
        bias_sfrnn_us    = malloc(sizeof(float *) * nlayers_sfrnn_us[0]-1);
        for (i=0; i<nlayers_sfrnn_us[0]-1; i++){
            weights_sfrnn_us[i] = malloc(sizeof(float*) * layers_sfrnn_us[i+1]);
            bias_sfrnn_us[i]    = malloc(sizeof(float) * layers_sfrnn_us[i+1]);
            for (j=0;j<layers_sfrnn_us[i+1]; j++){
                weights_sfrnn_us[i][j] = malloc(sizeof(float) 
                        * layers_sfrnn_us[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_sfrnn_us[0]-1; i++){
            for (j=0;j<layers_sfrnn_us[i+1]; j++){
                for (k=0; k<layers_sfrnn_us[i]; k++){
                    fscanf(nnfid,"%f",&weights_sfrnn_us[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_sfrnn_us[i][j]);
            }       
        }
            
        //allocate varid array
        varid_sfrnn_us = (int**) malloc(sizeof(int*) * layers_sfrnn_us[0]);
        if (!varid_sfrnn_us) {
            perror("Error allocating memory for varid_sfrnn_us.");
            exit(1);
        }
        //read in varid_sfrnn_us
        for (i=0; i<layers_sfrnn_us[0]; i++){
            varid_sfrnn_us[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_sfrnn_us[i] == NULL) {
                perror("Error allocating memory for varid_sfrnn_us.");
                exit(1);
            }
            fscanf(nnfid, "%i %i %i %i", &varid_sfrnn_us[i][0], 
                    &varid_sfrnn_us[i][1], &varid_sfrnn_us[i][2], 
                    &varid_sfrnn_us[i][3]);
            //printf("%i %i %i %i\n", varid_sfrnn_us[i][0], 
            //        varid_sfrnn_us[i][1],varid_sfrnn_us[i][2],
            //        varid_sfrnn_us[i][3]);
        }
        
        //read in post adjustment flag  
        flg_post_adj_sfrnn_us = (int*) malloc(sizeof(int)*1);
        if (flg_post_adj_sfrnn_us == NULL) {
            perror("Error allocating memory for flg_post_adj_sfrnn_us.");
            exit(1);
        }
        fscanf(nnfid,"%i",&flg_post_adj_sfrnn_us[0]);
        
        post_adj_varid_sfrnn_us = (int*) malloc(sizeof(int)*2);
        if (post_adj_varid_sfrnn_us == NULL) {
            perror("Error allocating memory for post_adj_varid_sfrnn_us.");
            exit(1);
        }
        for (i=0; i<2; i++) {
            fscanf(nnfid,"%i",&post_adj_varid_sfrnn_us[i]);
        }  

        //read in post adjustment info if flag set
        post_adj_sfrnn_us = (float*) malloc(sizeof(float)*6);
        if (post_adj_sfrnn_us == NULL) {
            perror("Error allocating memory for post_adj_sfrnn_us.");
            exit(1);
        }
        if (flg_post_adj_sfrnn_us[0]==1){
            for (i=0; i<6; i++) {
                fscanf(nnfid,"%f",&post_adj_sfrnn_us[i]);
            }
        }
        else{
            for (i=0; i<6; i++) {
                post_adj_sfrnn_us[i]=-99;
            }
        }
        
        // read in ALASKA NN
        
        //read in nn type
        nntype_sfrnn_ak = (int*) malloc(sizeof(int)*1);
        if (nntype_sfrnn_ak == NULL) {
            perror("Error allocating memory for nntype_sfrnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nntype_sfrnn_ak[0]);
        //read in number of layers
        nlayers_sfrnn_ak = (int*) malloc(sizeof(int)*1);
        if (nlayers_sfrnn_ak == NULL) {
            perror("Error allocating memory for nlayers_sfrnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&nlayers_sfrnn_ak[0]);
        
        //allocate memory for layer array
        layers_sfrnn_ak = malloc(sizeof(int) * nlayers_sfrnn_ak[0]);
        if (layers_sfrnn_ak == NULL) {
            perror("Error allocating memory for layer_sfrnn_ak.");
            exit(1);
        }    
        //read in NN layer structure
        for (i=0; i<nlayers_sfrnn_ak[0]; i++){        
            fscanf(nnfid,"%i",&layers_sfrnn_ak[i]);
        }
        
        //allocate memory for geolimit array
        geolimit_sfrnn_ak = (float*) malloc(sizeof(float) * 4);
        if (geolimit_sfrnn_ak == NULL) {
            perror("Error allocating memory for geolimit_sfrnn_ak.");
            exit(1);
        } 
        fscanf(nnfid,"%f %f %f %f", &geolimit_sfrnn_ak[0], 
                &geolimit_sfrnn_ak[1], &geolimit_sfrnn_ak[2], 
                &geolimit_sfrnn_ak[3]);
        
        //allocate normalization array for input
        norm_in_sfrnn_ak = (float**) malloc(sizeof(float*) 
                * layers_sfrnn_ak[0]);
        if (norm_in_sfrnn_ak == NULL) {
            perror("Error allocating memory for norm_in_sfrnn_ak.");
            exit(1);
        }    
        //read in norm_in data
        for (i=0; i<layers_sfrnn_ak[0]; i++){
            norm_in_sfrnn_ak[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_in_sfrnn_ak[i] == NULL) {
                perror("Error allocating memory for norm_in_sfrnn_ak.");
                exit(1);
            } 
            fscanf(nnfid, "%f %f", &norm_in_sfrnn_ak[i][0], 
                    &norm_in_sfrnn_ak[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate normalization array for output
        norm_out_sfrnn_ak = (float**) malloc(sizeof(float*) 
                * layers_sfrnn_ak[nlayers_sfrnn_ak[0]-1]);
        if (norm_out_sfrnn_ak == NULL) {
            perror("Error allocating memory for norm_out_sfrnn_ak.");
            exit(1);
        }    
        //read in norm_out data
        for (i=0; i<layers_sfrnn_ak[nlayers_sfrnn_ak[0]-1]; i++){
            norm_out_sfrnn_ak[i] = (float*) malloc(sizeof(float) * 2);
            if (norm_out_sfrnn_ak[i] == NULL) {
                perror("Error allocating memory for norm_out_sfrnn_ak.");
                exit(1);
            } 
            fscanf(nnfid,"%f %f", &norm_out_sfrnn_ak[i][0], 
                    &norm_out_sfrnn_ak[i][1]);
            //printf("%20.16e %20.16e\n", norm_in[i][0],norm_in[i][1]);
        }
        
        //allocate the layer weights and bias
        weights_sfrnn_ak = malloc(sizeof(float **) * nlayers_sfrnn_ak[0]-1);
        bias_sfrnn_ak    = malloc(sizeof(float *) * nlayers_sfrnn_ak[0]-1);
        for (i=0; i<nlayers_sfrnn_ak[0]-1; i++){
            weights_sfrnn_ak[i] = malloc(sizeof(float*) * layers_sfrnn_ak[i+1]);
            bias_sfrnn_ak[i]    = malloc(sizeof(float) * layers_sfrnn_ak[i+1]);
            for (j=0;j<layers_sfrnn_ak[i+1]; j++){
                weights_sfrnn_ak[i][j] = malloc(sizeof(float) 
                        * layers_sfrnn_ak[i]);
            }
        }
        //read in weithts and bias for each layer
        for (i=0;i<nlayers_sfrnn_ak[0]-1; i++){
            for (j=0;j<layers_sfrnn_ak[i+1]; j++){
                for (k=0; k<layers_sfrnn_ak[i]; k++){
                    fscanf(nnfid,"%f",&weights_sfrnn_ak[i][j][k]);
                }
                fscanf(nnfid,"%f",&bias_sfrnn_ak[i][j]);
            }       
        }
            
        //allocate varid array
        varid_sfrnn_ak = (int**) malloc(sizeof(int*) * layers_sfrnn_ak[0]);
        if (!varid_sfrnn_ak) {
            perror("Error allocating memory for varid_sfrnn_ak.");
            exit(1);
        }
        //read in varid_sfrnn_ak
        for (i=0; i<layers_sfrnn_ak[0]; i++){
            varid_sfrnn_ak[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_sfrnn_ak[i] == NULL) {
                perror("Error allocating memory for varid_sfrnn_ak.");
                exit(1);
            }
            fscanf(nnfid,"%i %i %i %i", &varid_sfrnn_ak[i][0], 
                    &varid_sfrnn_ak[i][1], &varid_sfrnn_ak[i][2], 
                    &varid_sfrnn_ak[i][3]);
            //printf("%i %i %i %i\n", varid_sfrnn_ak[i][0], 
            //        varid_sfrnn_ak[i][1], varid_sfrnn_ak[i][2], 
            //        varid_sfrnn_ak[i][3]);
        }
        
        //read in post adjustment flag  
        flg_post_adj_sfrnn_ak = (int*) malloc(sizeof(int)*1);
        if (flg_post_adj_sfrnn_ak == NULL) {
            perror("Error allocating memory for flg_post_adj_sfrnn_ak.");
            exit(1);
        }
        fscanf(nnfid,"%i",&flg_post_adj_sfrnn_ak[0]);
        
        post_adj_varid_sfrnn_ak = (int*) malloc(sizeof(int)*2);
        if (post_adj_varid_sfrnn_ak == NULL) {
            perror("Error allocating memory for post_adj_varid_sfrnn_ak.");
            exit(1);
        }
        for (i=0; i<2; i++) {
            fscanf(nnfid,"%i",&post_adj_varid_sfrnn_ak[i]);
        }  

        //read in post adjustment info if flag set
        post_adj_sfrnn_ak = (float*) malloc(sizeof(float)*6);
        if (post_adj_sfrnn_ak == NULL) {
            perror("Error allocating memory for post_adj_sfrnn_ak.");
            exit(1);
        }
        if (flg_post_adj_sfrnn_ak[0]==1){
            for (i=0; i<6; i++) {
                fscanf(nnfid,"%f",&post_adj_sfrnn_ak[i]);
            }
        }
        else{
            for (i=0; i<6; i++) {
                post_adj_sfrnn_ak[i]=-99;
            }
        }
        
    }  
    fclose(nnfid);
    
    return(0);
}



// mlnn
int mlnn(int *nn_type, float *nn_input, int *nlayers, int *layers, 
        float **norm_in, float**norm_out, 
         float ***weights, float **bias,  float *nn_output)
{
    int     i, j, k, max_neu;
    float   *dummy, *temp;
    
    // find maximum number of neurons
    max_neu=0;
    for (i=0; i<nlayers[0]; i++){
        if (layers[i]>max_neu)
            max_neu = layers[i];
    }
    
    //allocate memory for dummy and temp array
    dummy = (float*) malloc(sizeof(float) * max_neu);
    temp  = (float*) malloc(sizeof(float) * max_neu);
    
    // normalize the input
    for (i=0; i<layers[0]; i++){
        nn_input[i] = 2.0*(nn_input[i]-norm_in[i][0])/(norm_in[i][1]
                -norm_in[i][0])-1.0;
        if(fabs(nn_input[i]) > 1.0) return(1);
    }

    // MLNN
    for (i=0; i<nlayers[0]-1; i++){
        if(i==0){// first layer
            for (j=0; j<layers[i+1]; j++){
                dummy[j] = 0.0;
                for (k=0; k< layers[i]; k++){
                    dummy[j] = dummy[j] + nn_input[k] * weights[i][j][k];
                }
                dummy[j] = dummy[j] + bias[i][j];
                dummy[j] = tanh(dummy[j]);
            }
        }
        else if (i<nlayers[0]-2){//second to second to last layer
            for (k=0; k< layers[i]; k++){
                temp[k] = dummy[k]; // save layer output to temp array
            }
            for (j=0; j<layers[i+1]; j++){
                dummy[j] = 0.0;
                for (k=0; k< layers[i]; k++){
                    dummy[j] = dummy[j] + temp[k] * weights[i][j][k];
                }
                dummy[j] = dummy[j] + bias[i][j];
                dummy[j] = tanh(dummy[j]);
            }
        }
        else{// last layer, normally use a different activation function
            for (k=0; k< layers[i]; k++){
                temp[k] = dummy[k]; // save layer output to temp array
            }
            for (j=0; j<layers[i+1]; j++){
                dummy[j] = 0.0;
                for (k=0; k< layers[i]; k++){
                    dummy[j] = dummy[j] + temp[k] * weights[i][j][k];
                }
                dummy[j] = dummy[j] + bias[i][j];
                if(*nn_type==1){
                    dummy[j] = sigmoid(dummy[j]); 
                    // classifier use sigmoid function in last layer
                }                
            }
        }            
    }
    
    // output, regressor need to denormalize the output
    if(*nn_type==0){
        for (i=0; i<layers[nlayers[0]-1]; i++){
            nn_output[i] = (dummy[i]+1.0) / 2.0 * (norm_out[i][1]
                    -norm_out[i][0]) + norm_out[i][0];
        }            
    }
    else{
        for (i=0; i<layers[nlayers[0]-1]; i++){
            nn_output[i] = dummy[i];
        }
    }
    
    //free memory
    free(dummy);
    free(temp);
    
   return(0);
   
}



// sd
int sdnn_run(float lat, float lon, float *svar, float **avar, 
             int *nntype_sdnn_gb, int **varid_sdnn_gb, float **norm_in_sdnn_gb,
             float **norm_out_sdnn_gb, float *geolimit_sdnn_gb,
             int *nlayers_sdnn_gb, int *layers_sdnn_gb, 
             float ***weights_sdnn_gb, float **bias_sdnn_gb,
             int *varid_threshold_adj_sdnn_gb, int *threshold_adj_nsec_sdnn_gb,
             float *threshold_adj_sec_sdnn_gb, float *threshold_sdnn_gb,
             int *nntype_sdnn_us, int **varid_sdnn_us, float **norm_in_sdnn_us,
             float **norm_out_sdnn_us, float *geolimit_sdnn_us,
             int *nlayers_sdnn_us, int *layers_sdnn_us, 
             float ***weights_sdnn_us, float **bias_sdnn_us,
             int *varid_threshold_adj_sdnn_us, int *threshold_adj_nsec_sdnn_us,
             float *threshold_adj_sec_sdnn_us, float *threshold_sdnn_us,
             int *nntype_sdnn_ak, int **varid_sdnn_ak, float **norm_in_sdnn_ak,
             float **norm_out_sdnn_ak, float *geolimit_sdnn_ak,
             int *nlayers_sdnn_ak, int *layers_sdnn_ak, 
             float ***weights_sdnn_ak, float **bias_sdnn_ak,
             int *varid_threshold_adj_sdnn_ak, int *threshold_adj_nsec_sdnn_ak,
             float *threshold_adj_sec_sdnn_ak, float *threshold_sdnn_ak, 
             float *p_prob, int *p_sd)
{            
    int     i, nn_stat;
    float   *sdnn_input, *sdnn_output, p_threshold, p_delta, var;   
    
    // run CONUS NN        
    if (lat>=geolimit_sdnn_us[0] && lat<=geolimit_sdnn_us[1] 
            && lon>=geolimit_sdnn_us[2] && lon<=geolimit_sdnn_us[3] 
            && *flg_us_sdnn==1){
        //allocate memeory for sdnn_input 
        sdnn_input = (float*) malloc(sizeof(float) * layers_sdnn_us[0]);    
        if (sdnn_input == NULL) {
            perror("Error allocating memory for sdnn_input.");
            exit(1);
        }
        //allocate memeory for sdnn_output 
        sdnn_output = (float*) malloc(sizeof(float) 
                * layers_sdnn_us[nlayers_sdnn_us[0]-1]);    
        if (sdnn_output == NULL) {
            perror("Error allocating memory for sdnn_output.");
            exit(1);
        }    
        
        // form the input for SDNN    
        for (i=0; i<layers_sdnn_us[0]; i++){
            if(varid_sdnn_us[i][0]==0){
                if(varid_sdnn_us[i][1]==0){
                    sdnn_input[i] = svar[varid_sdnn_us[i][2]];
                }
                else{
                    sdnn_input[i] = svar[varid_sdnn_us[i][2]] 
                        - svar[varid_sdnn_us[i][3]];
                }
            }
            else{
                if(varid_sdnn_us[i][1]==0){
                    sdnn_input[i] = avar[varid_sdnn_us[i][2]][0];
                }
                else{
                    sdnn_input[i] = avar[varid_sdnn_us[i][2]][0] 
                        - avar[varid_sdnn_us[i][3]][0];
                }
            }
            //printf("%f ",sdnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat= mlnn(nntype_sdnn_us, sdnn_input, nlayers_sdnn_us, 
                layers_sdnn_us, norm_in_sdnn_us, norm_out_sdnn_us, 
                weights_sdnn_us, bias_sdnn_us, sdnn_output);
        //printf("nn_st=%i, sdnn_output=%f ",nn_stat,*sdnn_output);
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(sdnn_input);
            free(sdnn_output);
            return(2);
        }              
             
        //threshold adjustment for sd
        if (varid_threshold_adj_sdnn_us[0]>=0){
            if (varid_threshold_adj_sdnn_us[0]==0){
                var=svar[varid_threshold_adj_sdnn_us[2]];
            } 
            else if (varid_threshold_adj_sdnn_us[0]==1){
                var=avar[varid_threshold_adj_sdnn_us[2]][0];
            }
            for (i=0;i<threshold_adj_nsec_sdnn_us[0]; i++){
                if(i==0){
                    if (var<=threshold_adj_sec_sdnn_us[0]){
                        p_threshold = threshold_sdnn_us[i];
                        p_delta = p_threshold-0.5;
                    }
                }
                else if (i>0 && i<threshold_adj_nsec_sdnn_us[0]-1){
                    if (var>=threshold_adj_sec_sdnn_us[i-1] 
                            && var<threshold_adj_sec_sdnn_us[i]){
                        p_threshold = threshold_sdnn_us[i];
                        p_delta = p_threshold-0.5;
                    }
                }
                else{
                    if(var>=threshold_adj_sec_sdnn_us[i]){
                        p_threshold = threshold_sdnn_us[i];
                        p_delta = p_threshold-0.5;
                    }                    
                }
                
            }
        }
        else{
            p_threshold = 0.5;
            p_delta = p_threshold-0.5;
        }    
        
        if(sdnn_output[0]>p_threshold){
            *p_sd = 1;        
        }
        else{
            *p_sd = 0;
        }
        *p_prob = sdnn_output[0] - p_delta;
            
        //*p_sd = round(dummy[0]);
        //*p_prob = dummy[0];
        //printf("%lf, ",dummy[0]);
        
        // free memory
        free(sdnn_input);
        free(sdnn_output);
        
        return(0);
    }
    
    //run ALASKA NN        
    else if (lat>=geolimit_sdnn_ak[0] && lat<=geolimit_sdnn_ak[1] 
            && lon>=geolimit_sdnn_ak[2] && lon<=geolimit_sdnn_ak[3] 
            && *flg_ak_sdnn==1){
        //allocate memeory for sdnn_input 
        sdnn_input = (float*) malloc(sizeof(float) * layers_sdnn_ak[0]);    
        if (sdnn_input == NULL) {
            perror("Error allocating memory for sdnn_input.");
            exit(1);
        }
        //allocate memeory for sdnn_output 
        sdnn_output = (float*) malloc(sizeof(float) 
                * layers_sdnn_ak[nlayers_sdnn_ak[0]-1]);    
        if (sdnn_output == NULL) {
            perror("Error allocating memory for sdnn_output.");
            exit(1);
        }    
        
        // form the input for SDNN    
        for (i=0; i<layers_sdnn_ak[0]; i++){
            if(varid_sdnn_ak[i][0]==0){
                if(varid_sdnn_ak[i][1]==0){
                    sdnn_input[i] = svar[varid_sdnn_ak[i][2]];
                }
                else{
                    sdnn_input[i] = svar[varid_sdnn_ak[i][2]] 
                        - svar[varid_sdnn_ak[i][3]];
                }
            }
            else{
                if(varid_sdnn_ak[i][1]==0){
                    sdnn_input[i] = avar[varid_sdnn_ak[i][2]][0];
                }
                else{
                    sdnn_input[i] = avar[varid_sdnn_ak[i][2]][0] 
                        - avar[varid_sdnn_ak[i][3]][0];
                }
            }
            //printf("%f ",sdnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat= mlnn(nntype_sdnn_ak, sdnn_input, nlayers_sdnn_ak, 
                layers_sdnn_ak, norm_in_sdnn_ak, norm_out_sdnn_ak,
                weights_sdnn_ak, bias_sdnn_ak, sdnn_output);
        //printf("nn_st=%i, sdnn_output=%f ",nn_stat,*sdnn_output);
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(sdnn_input);
            free(sdnn_output);
            return(2);
        }              
             
        //threshold adjustment for sd
        if (varid_threshold_adj_sdnn_ak[0]>=0){
            if (varid_threshold_adj_sdnn_ak[0]==0){
                var=svar[varid_threshold_adj_sdnn_ak[2]];
            } 
            else if (varid_threshold_adj_sdnn_ak[0]==1){
                var=avar[varid_threshold_adj_sdnn_ak[2]][0];
            }
            for (i=0;i<threshold_adj_nsec_sdnn_ak[0]; i++){
                if(i==0){
                    if (var<=threshold_adj_sec_sdnn_ak[0]){
                        p_threshold = threshold_sdnn_ak[i];
                        p_delta = p_threshold-0.5;
                    }
                }
                else if (i>0 && i<threshold_adj_nsec_sdnn_ak[0]-1){
                    if (var>=threshold_adj_sec_sdnn_ak[i-1] 
                            && var<threshold_adj_sec_sdnn_ak[i]){
                        p_threshold = threshold_sdnn_ak[i];
                        p_delta = p_threshold-0.5;
                    }
                }
                else{
                    if(var>=threshold_adj_sec_sdnn_ak[i]){
                        p_threshold = threshold_sdnn_ak[i];
                        p_delta = p_threshold-0.5;
                    }                    
                }
                
            }
        }
        else{
            p_threshold = 0.5;
            p_delta = p_threshold-0.5;
        }    
        
        if(sdnn_output[0]>p_threshold){
            *p_sd = 1;        
        }
        else{
            *p_sd = 0;
        }
        *p_prob = sdnn_output[0] - p_delta;
            
        //*p_sd = round(dummy[0]);
        //*p_prob = dummy[0];
        //printf("%lf, ",dummy[0]);
        
        // free memory
        free(sdnn_input);
        free(sdnn_output);
        
        return(0);
    }  

    //run global NN        
    else if (lat>=geolimit_sdnn_gb[0] && lat<=geolimit_sdnn_gb[1] 
            && lon>=geolimit_sdnn_gb[2] && lon<=geolimit_sdnn_gb[3] 
            && *flg_gb_sdnn==1){        
        //allocate memeory for sdnn_input 
        sdnn_input = (float*) malloc(sizeof(float) * layers_sdnn_gb[0]);    
        if (sdnn_input == NULL) {
            perror("Error allocating memory for sdnn_input.");
            exit(1);
        }
        //allocate memeory for sdnn_output 
        sdnn_output = (float*) malloc(sizeof(float) 
                * layers_sdnn_gb[nlayers_sdnn_gb[0]-1]);    
        if (sdnn_output == NULL) {
            perror("Error allocating memory for sdnn_output.");
            exit(1);
        }    

        // form the input for SDNN    
        for (i=0; i<layers_sdnn_gb[0]; i++){
            //printf("%d %d %d %d:  ", varid_sdnn_gb[i][0], 
            //        varid_sdnn_gb[i][1], varid_sdnn_gb[i][2], 
            //        varid_sdnn_gb[i][3]);
            if(varid_sdnn_gb[i][0]==0){
                if(varid_sdnn_gb[i][1]==0){
                    sdnn_input[i] = svar[varid_sdnn_gb[i][2]];
                }
                else{
                    sdnn_input[i] = svar[varid_sdnn_gb[i][2]] 
                        - svar[varid_sdnn_gb[i][3]];
                }
            }
            else{
                if(varid_sdnn_gb[i][1]==0){
                    sdnn_input[i] = avar[varid_sdnn_gb[i][2]][0];
                }
                else{
                    sdnn_input[i] = avar[varid_sdnn_gb[i][2]][0] 
                        - avar[varid_sdnn_gb[i][3]][0];
                }
            }
            //printf("%f \n",sdnn_input[i]);
        }
        nn_stat=0;
        nn_stat= mlnn(nntype_sdnn_gb, sdnn_input, nlayers_sdnn_gb, 
                layers_sdnn_gb, norm_in_sdnn_gb, norm_out_sdnn_gb,
                weights_sdnn_gb, bias_sdnn_gb, sdnn_output);
        //printf("nn_st=%i, sdnn_output=%f ",nn_stat,*sdnn_output);
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(sdnn_input);
            free(sdnn_output);
            return(2);
        }              
             
        //threshold adjustment for sd
        if (varid_threshold_adj_sdnn_gb[0]>=0){
            if (varid_threshold_adj_sdnn_gb[0]==0){
                var=svar[varid_threshold_adj_sdnn_gb[2]];
            } 
            else if (varid_threshold_adj_sdnn_gb[0]==1){
                var=avar[varid_threshold_adj_sdnn_gb[2]][0];
            }
            for (i=0;i<threshold_adj_nsec_sdnn_gb[0]; i++){
                if(i==0){
                    if (var<=threshold_adj_sec_sdnn_gb[0]){
                        p_threshold = threshold_sdnn_gb[i];
                        p_delta = p_threshold-0.5;
                    }
                }
                else if (i>0 && i<threshold_adj_nsec_sdnn_gb[0]-1){
                    if (var>=threshold_adj_sec_sdnn_gb[i-1] 
                            && var<threshold_adj_sec_sdnn_gb[i]){
                        p_threshold = threshold_sdnn_gb[i];
                        p_delta = p_threshold-0.5;
                    }
                }
                else{
                    if(var>=threshold_adj_sec_sdnn_gb[i]){
                        p_threshold = threshold_sdnn_gb[i];
                        p_delta = p_threshold-0.5;
                    }                    
                }
                
            }
        }
        else{
            p_threshold = 0.5;
            p_delta = p_threshold-0.5;
        }    
        
        if(sdnn_output[0]>p_threshold){
            *p_sd = 1;        
        }
        else{
            *p_sd = 0;
        }
        *p_prob = sdnn_output[0] - p_delta;
            
        //*p_sd = round(dummy[0]);
        //*p_prob = dummy[0];
        //printf("%lf, ",dummy[0]);
        
        // free memory
        free(sdnn_input);
        free(sdnn_output);
        
        return(0);
    }
    
    else{
        return(1);
    }
 
}



// tb run
int tbnn_run(float lat, float lon, float *svar, float **avar, 
             int *nntype_tbnn_gb, int **varid_tbnn_gb, float **norm_in_tbnn_gb,
             float **norm_out_tbnn_gb, float *geolimit_tbnn_gb,
             int *nlayers_tbnn_gb, int *layers_tbnn_gb, 
             float ***weights_tbnn_gb, float **bias_tbnn_gb, 
             int *nntype_tbnn_us, int **varid_tbnn_us, float **norm_in_tbnn_us,
             float **norm_out_tbnn_us, float *geolimit_tbnn_us,
             int *nlayers_tbnn_us, int *layers_tbnn_us, 
             float ***weights_tbnn_us, float **bias_tbnn_us,
             int *nntype_tbnn_ak, int **varid_tbnn_ak, float **norm_in_tbnn_ak,
             float **norm_out_tbnn_ak, float *geolimit_tbnn_ak,
             int *nlayers_tbnn_ak, int *layers_tbnn_ak, 
             float ***weights_tbnn_ak, float **bias_tbnn_ak,
             float *tbo2)
{            
    int     i, nn_stat;
    float   *tbnn_input, *tbnn_output;
    
    //run CONUS NN        
    if (lat>=geolimit_tbnn_us[0] && lat<=geolimit_tbnn_us[1] 
            && lon>=geolimit_tbnn_us[2] && lon<=geolimit_tbnn_us[3] 
            && *flg_us_tbnn==1){
    
        //allocate memeory for tbnn_input 
        tbnn_input = (float*) malloc(sizeof(float) * layers_tbnn_us[0]);    
        if (tbnn_input == NULL) {
            perror("Error allocating memory for tbnn_input.");
            exit(1);
        }
        //allocate memeory for tbnn_output 
        tbnn_output = (float*) malloc(sizeof(float) 
                * layers_tbnn_us[nlayers_tbnn_us[0]-1]);    
        if (tbnn_output == NULL) {
            perror("Error allocating memory for tbnn_output.");
            exit(1);
        }    
        
        // form the input for TbNN    
        for (i=0; i<layers_tbnn_us[0]; i++){
            if(varid_tbnn_us[i][0]==0){
                if(varid_tbnn_us[i][1]==0){
                    tbnn_input[i] = svar[varid_tbnn_us[i][2]];
                }
                else{
                    tbnn_input[i] = svar[varid_tbnn_us[i][2]] 
                        - svar[varid_tbnn_us[i][3]];
                }
            }
            else{
                if(varid_tbnn_us[i][1]==0){
                    tbnn_input[i] = avar[varid_tbnn_us[i][2]][0];
                }
                else{
                    tbnn_input[i] = avar[varid_tbnn_us[i][2]][0] 
                        - avar[varid_tbnn_us[i][3]][0];
                }
            }
            //printf("%f ",tbnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_tbnn_us, tbnn_input, nlayers_tbnn_us, 
                layers_tbnn_us, norm_in_tbnn_us, norm_out_tbnn_us,
                weights_tbnn_us, bias_tbnn_us, tbnn_output);
        //printf("nn_st=%i, tbnn_output=%f %f %f %f %f %f ", 
        //        nn_stat,tbnn_output[0], tbnn_output[1], tbnn_output[2],
        //        tbnn_output[3],tbnn_output[4],tbnn_output[5]);
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(tbnn_input);
            free(tbnn_output);
            return(2);
        }             
             
        for (i=0; i<layers_tbnn_us[nlayers_tbnn_us[0]-1]; i++){
            tbo2[i] = tbnn_output[i];
        }
        
        // free memory
        free(tbnn_input);
        free(tbnn_output);
        
        return(0);
    }
    
    //run ALASKA NN        
    else if (lat>=geolimit_tbnn_ak[0] && lat<=geolimit_tbnn_ak[1] 
            && lon>=geolimit_tbnn_ak[2] && lon<=geolimit_tbnn_ak[3] 
            && *flg_ak_tbnn==1){
    
        //allocate memeory for tbnn_input 
        tbnn_input = (float*) malloc(sizeof(float) * layers_tbnn_ak[0]);    
        if (tbnn_input == NULL) {
            perror("Error allocating memory for tbnn_input.");
            exit(1);
        }
        //allocate memeory for tbnn_output 
        tbnn_output = (float*) malloc(sizeof(float) 
                * layers_tbnn_ak[nlayers_tbnn_ak[0]-1]);    
        if (tbnn_output == NULL) {
            perror("Error allocating memory for tbnn_output.");
            exit(1);
        }    
        
        // form the input for TbNN    
        for (i=0; i<layers_tbnn_ak[0]; i++){
            if(varid_tbnn_ak[i][0]==0){
                if(varid_tbnn_ak[i][1]==0){
                    tbnn_input[i] = svar[varid_tbnn_ak[i][2]];
                }
                else{
                    tbnn_input[i] = svar[varid_tbnn_ak[i][2]] 
                        - svar[varid_tbnn_ak[i][3]];
                }
            }
            else{
                if(varid_tbnn_ak[i][1]==0){
                    tbnn_input[i] = avar[varid_tbnn_ak[i][2]][0];
                }
                else{
                    tbnn_input[i] = avar[varid_tbnn_ak[i][2]][0] 
                        - avar[varid_tbnn_ak[i][3]][0];
                }
            }
            //printf("%f ",tbnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_tbnn_ak, tbnn_input, nlayers_tbnn_ak, 
                layers_tbnn_ak, norm_in_tbnn_ak, norm_out_tbnn_ak,
                weights_tbnn_ak, bias_tbnn_ak, tbnn_output);
        //printf("nn_st=%i, tbnn_output=%f %f %f %f %f %f ", nn_stat, 
        //        tbnn_output[0], tbnn_output[1], tbnn_output[2], 
        //        tbnn_output[3],tbnn_output[4],tbnn_output[5]);
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(tbnn_input);
            free(tbnn_output);
            return(2);
        }             
             
        for (i=0; i<layers_tbnn_ak[nlayers_tbnn_ak[0]-1]; i++){
            tbo2[i] = tbnn_output[i];
        }
        
        // free memory
        free(tbnn_input);
        free(tbnn_output);
        
        return(0);
    }
    
    //run global NN        
    else if (lat>=geolimit_tbnn_gb[0] && lat<=geolimit_tbnn_gb[1] 
            && lon>=geolimit_tbnn_gb[2] && lon<=geolimit_tbnn_gb[3] 
            && *flg_gb_tbnn==1){
    
        //allocate memeory for tbnn_input 
        tbnn_input = (float*) malloc(sizeof(float) * layers_tbnn_gb[0]);    
        if (tbnn_input == NULL) {
            perror("Error allocating memory for tbnn_input.");
            exit(1);
        }
        //allocate memeory for tbnn_output 
        tbnn_output = (float*) malloc(sizeof(float) 
                * layers_tbnn_gb[nlayers_tbnn_gb[0]-1]);    
        if (tbnn_output == NULL) {
            perror("Error allocating memory for tbnn_output.");
            exit(1);
        }    
        
        // form the input for TbNN    
        for (i=0; i<layers_tbnn_gb[0]; i++){
            if(varid_tbnn_gb[i][0]==0){
                if(varid_tbnn_gb[i][1]==0){
                    tbnn_input[i] = svar[varid_tbnn_gb[i][2]];
                }
                else{
                    tbnn_input[i] = svar[varid_tbnn_gb[i][2]] 
                        - svar[varid_tbnn_gb[i][3]];
                }
            }
            else{
                if(varid_tbnn_gb[i][1]==0){
                    tbnn_input[i] = avar[varid_tbnn_gb[i][2]][0];
                }
                else{
                    tbnn_input[i] = avar[varid_tbnn_gb[i][2]][0] 
                        - avar[varid_tbnn_gb[i][3]][0];
                }
            }
            //printf("%f ",tbnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_tbnn_gb, tbnn_input, nlayers_tbnn_gb, 
                layers_tbnn_gb, norm_in_tbnn_gb, norm_out_tbnn_gb,
                weights_tbnn_gb, bias_tbnn_gb, tbnn_output);
        printf("nn_st=%i, tbnn_output=%f %f %f %f %f %f ", nn_stat, 
                tbnn_output[0], tbnn_output[1], tbnn_output[2], 
                tbnn_output[3],tbnn_output[4],tbnn_output[5]);
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(tbnn_input);
            free(tbnn_output);
            return(2);
        }             
             
        for (i=0; i<layers_tbnn_gb[nlayers_tbnn_gb[0]-1]; i++){
            tbo2[i] = tbnn_output[i];
        }
        
        // free memory
        free(tbnn_input);
        free(tbnn_output);
        
        return(0);
    }
    
    else{
        return(1);
    }       
}



// iwp
int iwpnn_run(float lat, float lon, float *svar, float **avar, 
              int *nntype_iwpnn_gb, int **varid_iwpnn_gb, 
              float **norm_in_iwpnn_gb, float **norm_out_iwpnn_gb, 
              float *geolimit_iwpnn_gb,
              int *nlayers_iwpnn_gb, int *layers_iwpnn_gb, 
              float ***weights_iwpnn_gb, float **bias_iwpnn_gb,
              int *nntype_iwpnn_us, int **varid_iwpnn_us, 
              float **norm_in_iwpnn_us, float **norm_out_iwpnn_us, 
              float *geolimit_iwpnn_us,
              int *nlayers_iwpnn_us, int *layers_iwpnn_us, 
              float ***weights_iwpnn_us, float **bias_iwpnn_us,
              int *nntype_iwpnn_ak, int **varid_iwpnn_ak, 
              float **norm_in_iwpnn_ak, float **norm_out_iwpnn_ak, 
              float *geolimit_iwpnn_ak,
              int *nlayers_iwpnn_ak, int *layers_iwpnn_ak, 
              float ***weights_iwpnn_ak, float **bias_iwpnn_ak,
              float **ti, float *p_val)
{            
    int     i, nn_stat, iout;
    float   *iwpnn_input, *iwpnn_output;
    
    //run CONUS NN
    if (lat>=geolimit_iwpnn_us[0] && lat<=geolimit_iwpnn_us[1] 
            && lon>=geolimit_iwpnn_us[2] && lon<=geolimit_iwpnn_us[3] 
            && *flg_us_iwpnn==1){
        //allocate memeory for iwpnn_input 
        iwpnn_input = (float*) malloc(sizeof(float) * layers_iwpnn_us[0]);    
        if (iwpnn_input == NULL) {
            perror("Error allocating memory for iwpnn_input.");
            exit(1);
        }
        //allocate memeory for iwpnn_output 
        iwpnn_output = (float*) malloc(sizeof(float) 
                * layers_iwpnn_us[nlayers_iwpnn_us[0]-1]);    
        if (iwpnn_output == NULL) {
            perror("Error allocating memory for iwpnn_output.");
            exit(1);
        }    
        
        // form the input for iwpnn    
        for (i=0; i<layers_iwpnn_us[0]; i++){
            if(varid_iwpnn_us[i][0]==0){
                if(varid_iwpnn_us[i][1]==0){
                    iwpnn_input[i] = svar[varid_iwpnn_us[i][2]];
                }
                else{
                    iwpnn_input[i] = svar[varid_iwpnn_us[i][2]] 
                        - svar[varid_iwpnn_us[i][3]];
                }
            }
            else{
                if(varid_iwpnn_us[i][1]==0){
                    iwpnn_input[i] = avar[varid_iwpnn_us[i][2]][0];
                }
                else{
                    iwpnn_input[i] = avar[varid_iwpnn_us[i][2]][0] 
                        - avar[varid_iwpnn_us[i][3]][0];
                }
            }
            //printf("%f ",iwpnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_iwpnn_us, iwpnn_input, nlayers_iwpnn_us, 
                layers_iwpnn_us, norm_in_iwpnn_us, norm_out_iwpnn_us,
                weights_iwpnn_us, bias_iwpnn_us, iwpnn_output);
        //printf("nn_st=%i, iwpnn_output=%f ",nn_stat,*iwpnn_output);             
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(iwpnn_input);
            free(iwpnn_output);
            return(2);
        }    
           
        for (i=0; i<layers_iwpnn_us[nlayers_iwpnn_us[0]-1]; i++){
            p_val[i] = pow(10, iwpnn_output[i]); // iwpnn use power 10 
            if (p_val[i]<0.001) p_val[i]=0.001; // need to update to tis file
        }
        iout = round(ti[0][0]);
        svar[iout] = p_val[0]; //save to svar        
        
        // free memory
        free(iwpnn_input);
        free(iwpnn_output);
        
        return(0);    
    }
    
    //run ALASKA NN
    else if (lat>=geolimit_iwpnn_ak[0] && lat<=geolimit_iwpnn_ak[1] 
            && lon>=geolimit_iwpnn_ak[2] && lon<=geolimit_iwpnn_ak[3] 
            && *flg_ak_iwpnn==1){
        //allocate memeory for iwpnn_input 
        iwpnn_input = (float*) malloc(sizeof(float) * layers_iwpnn_ak[0]);    
        if (iwpnn_input == NULL) {
            perror("Error allocating memory for iwpnn_input.");
            exit(1);
        }
        //allocate memeory for iwpnn_output 
        iwpnn_output = (float*) malloc(sizeof(float) 
                * layers_iwpnn_ak[nlayers_iwpnn_ak[0]-1]);    
        if (iwpnn_output == NULL) {
            perror("Error allocating memory for iwpnn_output.");
            exit(1);
        }    
        
        // form the input for iwpnn    
        for (i=0; i<layers_iwpnn_ak[0]; i++){
            if(varid_iwpnn_ak[i][0]==0){
                if(varid_iwpnn_ak[i][1]==0){
                    iwpnn_input[i] = svar[varid_iwpnn_ak[i][2]];
                }
                else{
                    iwpnn_input[i] = svar[varid_iwpnn_ak[i][2]] 
                        - svar[varid_iwpnn_ak[i][3]];
                }
            }
            else{
                if(varid_iwpnn_ak[i][1]==0){
                    iwpnn_input[i] = avar[varid_iwpnn_ak[i][2]][0];
                }
                else{
                    iwpnn_input[i] = avar[varid_iwpnn_ak[i][2]][0] 
                        - avar[varid_iwpnn_ak[i][3]][0];
                }
            }
            //printf("%f ",iwpnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_iwpnn_ak, iwpnn_input, nlayers_iwpnn_ak, 
                layers_iwpnn_ak, norm_in_iwpnn_ak, norm_out_iwpnn_ak,
                weights_iwpnn_ak, bias_iwpnn_ak, iwpnn_output);
        //printf("nn_st=%i, iwpnn_output=%f ",nn_stat,*iwpnn_output);             
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(iwpnn_input);
            free(iwpnn_output);
            return(2);
        }    
           
        for (i=0; i<layers_iwpnn_ak[nlayers_iwpnn_ak[0]-1]; i++){
            p_val[i] = pow(10, iwpnn_output[i]); // iwpnn use power 10 
            if (p_val[i]<0.001) p_val[i]=0.001; // need to update to tis file
        }
        iout = round(ti[0][0]);
        svar[iout] = p_val[0]; //save to svar        
        
        // free memory
        free(iwpnn_input);
        free(iwpnn_output);
        
        return(0);    
    }
    
    //run global NN
    else if (lat>=geolimit_iwpnn_gb[0] && lat<=geolimit_iwpnn_gb[1] 
            && lon>=geolimit_iwpnn_gb[2] && lon<=geolimit_iwpnn_gb[3] 
            && *flg_gb_iwpnn==1){
        //allocate memeory for iwpnn_input 
        iwpnn_input = (float*) malloc(sizeof(float) * layers_iwpnn_gb[0]);    
        if (iwpnn_input == NULL) {
            perror("Error allocating memory for iwpnn_input.");
            exit(1);
        }
        //allocate memeory for iwpnn_output 
        iwpnn_output = (float*) malloc(sizeof(float) 
                * layers_iwpnn_gb[nlayers_iwpnn_gb[0]-1]);    
        if (iwpnn_output == NULL) {
            perror("Error allocating memory for iwpnn_output.");
            exit(1);
        }    
        
        // form the input for iwpnn    
        for (i=0; i<layers_iwpnn_gb[0]; i++){
            if(varid_iwpnn_gb[i][0]==0){
                if(varid_iwpnn_gb[i][1]==0){
                    iwpnn_input[i] = svar[varid_iwpnn_gb[i][2]];
                }
                else{
                    iwpnn_input[i] = svar[varid_iwpnn_gb[i][2]] 
                        - svar[varid_iwpnn_gb[i][3]];
                }
            }
            else{
                if(varid_iwpnn_gb[i][1]==0){
                    iwpnn_input[i] = avar[varid_iwpnn_gb[i][2]][0];
                }
                else{
                    iwpnn_input[i] = avar[varid_iwpnn_gb[i][2]][0] 
                        - avar[varid_iwpnn_gb[i][3]][0];
                }
            }
            //printf("%f ",iwpnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_iwpnn_gb, iwpnn_input, nlayers_iwpnn_gb, 
                layers_iwpnn_gb, norm_in_iwpnn_gb, norm_out_iwpnn_gb,
                weights_iwpnn_gb, bias_iwpnn_gb, iwpnn_output);
        //printf("nn_st=%i, iwpnn_output=%f ",nn_stat,*iwpnn_output);             
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(iwpnn_input);
            free(iwpnn_output);
            return(2);
        }    
           
        for (i=0; i<layers_iwpnn_gb[nlayers_iwpnn_gb[0]-1]; i++){
            p_val[i] = pow(10, iwpnn_output[i]); // iwpnn use power 10 
            if (p_val[i]<0.001) p_val[i]=0.001; // need to update to tis file
        }
        iout = round(ti[0][0]);
        svar[iout] = p_val[0]; //save to svar        
        
        // free memory
        free(iwpnn_input);
        free(iwpnn_output);
        
        return(0);    
    }
    
    else{
        return(1);
    }  
}



// tc
int tcnn_run(float lat, float lon, float *svar, float **avar, 
             int *nntype_tcnn_gb, int **varid_tcnn_gb, float **norm_in_tcnn_gb,
             float **norm_out_tcnn_gb, float *geolimit_tcnn_gb,
             int *nlayers_tcnn_gb, int *layers_tcnn_gb, 
             float ***weights_tcnn_gb, float **bias_tcnn_gb, 
             int *nntype_tcnn_us, int **varid_tcnn_us, float **norm_in_tcnn_us,
             float **norm_out_tcnn_us, float *geolimit_tcnn_us,
             int *nlayers_tcnn_us, int *layers_tcnn_us, 
             float ***weights_tcnn_us, float **bias_tcnn_us,
             int *nntype_tcnn_ak, int **varid_tcnn_ak, float **norm_in_tcnn_ak,
             float **norm_out_tcnn_ak, float *geolimit_tcnn_ak,
             int *nlayers_tcnn_ak, int *layers_tcnn_ak, 
             float ***weights_tcnn_ak, float **bias_tcnn_ak,
             float *p_val)
{            
    int     i, nn_stat;
    float   *tcnn_input, *tcnn_output;
    
    //run CONUS NN
    if (lat>=geolimit_tcnn_us[0] && lat<=geolimit_tcnn_us[1] 
            && lon>=geolimit_tcnn_us[2] && lon<=geolimit_tcnn_us[3] 
            && *flg_us_tcnn==1){        
        //allocate memeory for tcnn_input 
        tcnn_input = (float*) malloc(sizeof(float) * layers_tcnn_us[0]);    
        if (tcnn_input == NULL) {
            perror("Error allocating memory for tcnn_input.");
            exit(1);
        }
        //allocate memeory for tcnn_output 
        tcnn_output = (float*) malloc(sizeof(float) 
                * layers_tcnn_us[nlayers_tcnn_us[0]-1]);    
        if (tcnn_output == NULL) {
            perror("Error allocating memory for tcnn_output.");
            exit(1);
        }    
        
        // form the input for tcnn    
        for (i=0; i<layers_tcnn_us[0]; i++){
            if(varid_tcnn_us[i][0]==0){
                if(varid_tcnn_us[i][1]==0){
                    tcnn_input[i] = svar[varid_tcnn_us[i][2]];
                }
                else{
                    tcnn_input[i] = svar[varid_tcnn_us[i][2]] 
                        - svar[varid_tcnn_us[i][3]];
                }
            }
            else{
                if(varid_tcnn_us[i][1]==0){
                    tcnn_input[i] = avar[varid_tcnn_us[i][2]][0];
                }
                else{
                    tcnn_input[i] = avar[varid_tcnn_us[i][2]][0] 
                        - avar[varid_tcnn_us[i][3]][0];
                }
            }
            //printf("%f ",tcnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_tcnn_us, tcnn_input, nlayers_tcnn_us, 
                layers_tcnn_us, norm_in_tcnn_us, norm_out_tcnn_us,
                weights_tcnn_us, bias_tcnn_us, tcnn_output);
        //printf("nn_st=%i, tcnn_output=%f ",nn_stat,*tcnn_output);              
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(tcnn_input);
            free(tcnn_output);
            return(2);
        }
             
        for (i=0; i<layers_tcnn_us[nlayers_tcnn_us[0]-1]; i++){
            p_val[i] = tcnn_output[i];
            if (p_val[i]<215.0) p_val[i]=215.0; // need to update to tis file
            if (p_val[i]>275.0) p_val[i]=275.0; // need to update to tis file
        }
        
        // free memory
        free(tcnn_input);
        free(tcnn_output);
        
        return(0);    
    }
    
    //run ALASKA NN
    else if (lat>=geolimit_tcnn_ak[0] && lat<=geolimit_tcnn_ak[1] 
            && lon>=geolimit_tcnn_ak[2] && lon<=geolimit_tcnn_ak[3] 
            && *flg_ak_tcnn==1){        
        //allocate memeory for tcnn_input 
        tcnn_input = (float*) malloc(sizeof(float) * layers_tcnn_ak[0]);    
        if (tcnn_input == NULL) {
            perror("Error allocating memory for tcnn_input.");
            exit(1);
        }
        //allocate memeory for tcnn_output 
        tcnn_output = (float*) malloc(sizeof(float) 
                * layers_tcnn_ak[nlayers_tcnn_ak[0]-1]);    
        if (tcnn_output == NULL) {
            perror("Error allocating memory for tcnn_output.");
            exit(1);
        }    
        
        // form the input for tcnn    
        for (i=0; i<layers_tcnn_ak[0]; i++){
            if(varid_tcnn_ak[i][0]==0){
                if(varid_tcnn_ak[i][1]==0){
                    tcnn_input[i] = svar[varid_tcnn_ak[i][2]];
                }
                else{
                    tcnn_input[i] = svar[varid_tcnn_ak[i][2]] 
                        - svar[varid_tcnn_ak[i][3]];
                }
            }
            else{
                if(varid_tcnn_ak[i][1]==0){
                    tcnn_input[i] = avar[varid_tcnn_ak[i][2]][0];
                }
                else{
                    tcnn_input[i] = avar[varid_tcnn_ak[i][2]][0] 
                        - avar[varid_tcnn_ak[i][3]][0];
                }
            }
            //printf("%f ",tcnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_tcnn_ak, tcnn_input, nlayers_tcnn_ak, 
                layers_tcnn_ak, norm_in_tcnn_ak, norm_out_tcnn_ak,
                weights_tcnn_ak, bias_tcnn_ak, tcnn_output);
        //printf("nn_st=%i, tcnn_output=%f ",nn_stat,*tcnn_output);              
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(tcnn_input);
            free(tcnn_output);
            return(2);
        }
             
        for (i=0; i<layers_tcnn_ak[nlayers_tcnn_ak[0]-1]; i++){
            p_val[i] = tcnn_output[i];
            if (p_val[i]<215.0) p_val[i]=215.0; // need to update to tis file
            if (p_val[i]>275.0) p_val[i]=275.0; // need to update to tis file
        }
        
        // free memory
        free(tcnn_input);
        free(tcnn_output);
        
        return(0);    
    }
    
    //run global NN 
    else if (lat>=geolimit_tcnn_gb[0] && lat<=geolimit_tcnn_gb[1] 
            && lon>=geolimit_tcnn_gb[2] && lon<=geolimit_tcnn_gb[3] 
            && *flg_gb_tcnn==1){        
        //allocate memeory for tcnn_input 
        tcnn_input = (float*) malloc(sizeof(float) * layers_tcnn_gb[0]);    
        if (tcnn_input == NULL) {
            perror("Error allocating memory for tcnn_input.");
            exit(1);
        }
        //allocate memeory for tcnn_output 
        tcnn_output = (float*) malloc(sizeof(float) 
                * layers_tcnn_gb[nlayers_tcnn_gb[0]-1]);    
        if (tcnn_output == NULL) {
            perror("Error allocating memory for tcnn_output.");
            exit(1);
        }    
        
        // form the input for tcnn    
        for (i=0; i<layers_tcnn_gb[0]; i++){
            if(varid_tcnn_gb[i][0]==0){
                if(varid_tcnn_gb[i][1]==0){
                    tcnn_input[i] = svar[varid_tcnn_gb[i][2]];
                }
                else{
                    tcnn_input[i] = svar[varid_tcnn_gb[i][2]] 
                        - svar[varid_tcnn_gb[i][3]];
                }
            }
            else{
                if(varid_tcnn_gb[i][1]==0){
                    tcnn_input[i] = avar[varid_tcnn_gb[i][2]][0];
                }
                else{
                    tcnn_input[i] = avar[varid_tcnn_gb[i][2]][0] 
                        - avar[varid_tcnn_gb[i][3]][0];
                }
            }
            //printf("%f ",tcnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_tcnn_gb, tcnn_input, nlayers_tcnn_gb, 
                layers_tcnn_gb, norm_in_tcnn_gb, norm_out_tcnn_gb,
                weights_tcnn_gb, bias_tcnn_gb, tcnn_output);
        //printf("nn_st=%i, tcnn_output=%f ",nn_stat,*tcnn_output);              
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(tcnn_input);
            free(tcnn_output);
            return(2);
        }
             
        for (i=0; i<layers_tcnn_gb[nlayers_tcnn_gb[0]-1]; i++){
            p_val[i] = tcnn_output[i];
            if (p_val[i]<215.0) p_val[i]=215.0; // need to update to tis file
            if (p_val[i]>275.0) p_val[i]=275.0; // need to update to tis file
        }
        
        // free memory
        free(tcnn_input);
        free(tcnn_output);
        
        return(0);    
    }
    
    else{
        return(1);
    }    
}



// sfr
int sfrnn_run(float lat, float lon, float *svar, float **avar, 
              int *nntype_sfrnn_gb, int **varid_sfrnn_gb, 
              float **norm_in_sfrnn_gb, float **norm_out_sfrnn_gb, 
              float *geolimit_sfrnn_gb,
              int *nlayers_sfrnn_gb, int *layers_sfrnn_gb, 
              float ***weights_sfrnn_gb, float **bias_sfrnn_gb, 
              int *flg_post_adj_sfrnn_gb, float *post_adj_sfrnn_gb, 
              int *post_adj_varid_sfrnn_gb,
              int *nntype_sfrnn_us, int **varid_sfrnn_us, 
              float **norm_in_sfrnn_us, float **norm_out_sfrnn_us, 
              float *geolimit_sfrnn_us,
              int *nlayers_sfrnn_us, int *layers_sfrnn_us, 
              float ***weights_sfrnn_us, float **bias_sfrnn_us, 
              int *flg_post_adj_sfrnn_us, float *post_adj_sfrnn_us, 
              int *post_adj_varid_sfrnn_us,
              int *nntype_sfrnn_ak, int **varid_sfrnn_ak, 
              float **norm_in_sfrnn_ak, float **norm_out_sfrnn_ak, 
              float *geolimit_sfrnn_ak,
              int *nlayers_sfrnn_ak, int *layers_sfrnn_ak, 
              float ***weights_sfrnn_ak, float **bias_sfrnn_ak, 
              int *flg_post_adj_sfrnn_ak, float *post_adj_sfrnn_ak, 
              int *post_adj_varid_sfrnn_ak,
              float *p_val)
{            
    int     i, bsfr_id, nconv_id, nn_stat;
    float   *sfrnn_input, *sfrnn_output, bsfr;
    
    //run CONUS NN
    if (lat>=geolimit_sfrnn_us[0] && lat<=geolimit_sfrnn_us[1] 
            && lon>=geolimit_sfrnn_us[2] && lon<=geolimit_sfrnn_us[3] 
            && *flg_us_sfrnn==1){
        bsfr = *p_val;
        bsfr_id = post_adj_varid_sfrnn_us[0];
        nconv_id = post_adj_varid_sfrnn_us[1];
        
        //allocate memeory for sfrnn_input 
        sfrnn_input = (float*) malloc(sizeof(float) * layers_sfrnn_us[0]);    
        if (sfrnn_input == NULL) {
            perror("Error allocating memory for sfrnn_input.");
            exit(1);
        }
        //allocate memeory for sfrnn_output 
        sfrnn_output = (float*) malloc(sizeof(float) 
                * layers_sfrnn_us[nlayers_sfrnn_us[0]-1]);    
        if (sfrnn_output == NULL) {
            perror("Error allocating memory for sfrnn_output.");
            exit(1);
        }    
        
        // form the input for sfrnn    
        for (i=0; i<layers_sfrnn_us[0]; i++){
            if(varid_sfrnn_us[i][0]==0){
                if(varid_sfrnn_us[i][1]==0){
                    sfrnn_input[i] = svar[varid_sfrnn_us[i][2]];
                }
                else{
                    sfrnn_input[i] = svar[varid_sfrnn_us[i][2]] 
                        - svar[varid_sfrnn_us[i][3]];
                }
            }
            else{
                if(varid_sfrnn_us[i][1]==0){
                    sfrnn_input[i] = avar[varid_sfrnn_us[i][2]][0];
                }
                else{
                    sfrnn_input[i] = avar[varid_sfrnn_us[i][2]][0] 
                        - avar[varid_sfrnn_us[i][3]][0];
                }
            }
            //printf("%f ",sfrnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_sfrnn_us, sfrnn_input, nlayers_sfrnn_us, 
                layers_sfrnn_us, norm_in_sfrnn_us, norm_out_sfrnn_us,
                weights_sfrnn_us, bias_sfrnn_us, sfrnn_output);
        //printf("nn_st=%i, sfrnn_output=%f ",nn_stat,*sfrnn_output);              
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(sfrnn_input);
            free(sfrnn_output);
            return(2);
        }
        
        *p_val = *p_val - *sfrnn_output;
        svar[bsfr_id] = *p_val;
        
        // post adjustment
        if (flg_post_adj_sfrnn_us[0]==1){
            if(*p_val <= post_adj_sfrnn_us[0]){
                *p_val = *p_val - post_adj_sfrnn_us[2];
            }
            else if (*p_val <= post_adj_sfrnn_us[1] 
                    && *p_val > post_adj_sfrnn_us[0]){
                *p_val = *p_val * post_adj_sfrnn_us[3] + post_adj_sfrnn_us[4];
            }
            
            if (bsfr > post_adj_sfrnn_us[5] && svar[nconv_id]==0 
                    && *p_val<bsfr){
                *p_val = bsfr;
            }
            svar[bsfr_id+1] = *p_val;
            
            if(*p_val<0.0) *p_val = bsfr;
            svar[bsfr_id+2] = *p_val;        
        }
        
        
        
        // free memory
        free(sfrnn_input);
        free(sfrnn_output);
        
        return(0);  
    }
    
    //run ALASKA NN
    else if (lat>=geolimit_sfrnn_us[0] && lat<=geolimit_sfrnn_us[1] 
            && lon>=geolimit_sfrnn_us[2] && lon<=geolimit_sfrnn_us[3] 
            && *flg_ak_sfrnn==1){
        bsfr = *p_val;
        bsfr_id = post_adj_varid_sfrnn_us[0];
        nconv_id = post_adj_varid_sfrnn_us[1];
        
        //allocate memeory for sfrnn_input 
        sfrnn_input = (float*) malloc(sizeof(float) * layers_sfrnn_us[0]);    
        if (sfrnn_input == NULL) {
            perror("Error allocating memory for sfrnn_input.");
            exit(1);
        }
        //allocate memeory for sfrnn_output 
        sfrnn_output = (float*) malloc(sizeof(float) 
                * layers_sfrnn_us[nlayers_sfrnn_us[0]-1]);    
        if (sfrnn_output == NULL) {
            perror("Error allocating memory for sfrnn_output.");
            exit(1);
        }    
        
        // form the input for sfrnn    
        for (i=0; i<layers_sfrnn_us[0]; i++){
            if(varid_sfrnn_us[i][0]==0){
                if(varid_sfrnn_us[i][1]==0){
                    sfrnn_input[i] = svar[varid_sfrnn_us[i][2]];
                }
                else{
                    sfrnn_input[i] = svar[varid_sfrnn_us[i][2]] 
                        - svar[varid_sfrnn_us[i][3]];
                }
            }
            else{
                if(varid_sfrnn_us[i][1]==0){
                    sfrnn_input[i] = avar[varid_sfrnn_us[i][2]][0];
                }
                else{
                    sfrnn_input[i] = avar[varid_sfrnn_us[i][2]][0] 
                        - avar[varid_sfrnn_us[i][3]][0];
                }
            }
            //printf("%f ",sfrnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_sfrnn_us, sfrnn_input, nlayers_sfrnn_us, 
                layers_sfrnn_us, norm_in_sfrnn_us, norm_out_sfrnn_us,
                weights_sfrnn_us, bias_sfrnn_us, sfrnn_output);
        //printf("nn_st=%i, sfrnn_output=%f ",nn_stat,*sfrnn_output);              
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(sfrnn_input);
            free(sfrnn_output);
            return(2);
        }
        
        *p_val = *p_val - *sfrnn_output;
        svar[bsfr_id] = *p_val;
        
        // post adjustment
        if (flg_post_adj_sfrnn_us[0]==1){
            if(*p_val <= post_adj_sfrnn_us[0]){
                *p_val = *p_val - post_adj_sfrnn_us[2];
            }
            else if (*p_val <= post_adj_sfrnn_us[1] 
                    && *p_val > post_adj_sfrnn_us[0]){
                *p_val = *p_val * post_adj_sfrnn_us[3] + post_adj_sfrnn_us[4];
            }
            
            if (bsfr > post_adj_sfrnn_us[5] && svar[nconv_id]==0 
                    && *p_val<bsfr){
                *p_val = bsfr;
            }
            svar[bsfr_id+1] = *p_val;
            
            if(*p_val<0.0) *p_val = bsfr;
            svar[bsfr_id+2] = *p_val;        
        }
        
        
        // free memory
        free(sfrnn_input);
        free(sfrnn_output);
        
        return(0);  
    }
    
    //run global NN 
    else if (lat>=geolimit_sfrnn_gb[0] && lat<=geolimit_sfrnn_gb[1] 
            && lon>=geolimit_sfrnn_gb[2] && lon<=geolimit_sfrnn_gb[3] 
            && *flg_gb_sfrnn==1){
        bsfr = *p_val;
        bsfr_id = post_adj_varid_sfrnn_gb[0];
        nconv_id = post_adj_varid_sfrnn_gb[1];
        
        //allocate memeory for sfrnn_input 
        sfrnn_input = (float*) malloc(sizeof(float) * layers_sfrnn_gb[0]);    
        if (sfrnn_input == NULL) {
            perror("Error allocating memory for sfrnn_input.");
            exit(1);
        }
        //allocate memeory for sfrnn_output 
        sfrnn_output = (float*) malloc(sizeof(float) 
                * layers_sfrnn_gb[nlayers_sfrnn_gb[0]-1]);    
        if (sfrnn_output == NULL) {
            perror("Error allocating memory for sfrnn_output.");
            exit(1);
        }    
        
        // form the input for sfrnn    
        for (i=0; i<layers_sfrnn_gb[0]; i++){
            if(varid_sfrnn_gb[i][0]==0){
                if(varid_sfrnn_gb[i][1]==0){
                    sfrnn_input[i] = svar[varid_sfrnn_gb[i][2]];
                }
                else{
                    sfrnn_input[i] = svar[varid_sfrnn_gb[i][2]] 
                        - svar[varid_sfrnn_gb[i][3]];
                }
            }
            else{
                if(varid_sfrnn_gb[i][1]==0){
                    sfrnn_input[i] = avar[varid_sfrnn_gb[i][2]][0];
                }
                else{
                    sfrnn_input[i] = avar[varid_sfrnn_gb[i][2]][0] 
                        - avar[varid_sfrnn_gb[i][3]][0];
                }
            }
            //printf("%f ",sfrnn_input[i]);
        }
        //printf("\n");
        nn_stat=0;
        nn_stat=mlnn(nntype_sfrnn_gb, sfrnn_input, nlayers_sfrnn_gb, 
                layers_sfrnn_gb, norm_in_sfrnn_gb, norm_out_sfrnn_gb,
                weights_sfrnn_gb, bias_sfrnn_gb, sfrnn_output);
        //printf("nn_st=%i, sfrnn_output=%f ",nn_stat,*sfrnn_output);              
        if (nn_stat>0){
            // NN out of range, free memory and return
            free(sfrnn_input);
            free(sfrnn_output);
            return(2);
        }
        
        *p_val = *p_val - *sfrnn_output;
        svar[bsfr_id] = *p_val;
        
        // post adjustment
        if (flg_post_adj_sfrnn_gb[0]==1){
            if(*p_val <= post_adj_sfrnn_gb[0]){
                *p_val = *p_val - post_adj_sfrnn_gb[2];
            }
            else if (*p_val <= post_adj_sfrnn_gb[1] 
                    && *p_val > post_adj_sfrnn_gb[0]){
                *p_val = *p_val * post_adj_sfrnn_gb[3] + post_adj_sfrnn_gb[4];
            }
            
            if (bsfr > post_adj_sfrnn_gb[5] && svar[nconv_id]==0 
                    && *p_val<bsfr){
                *p_val = bsfr;
            }
            svar[bsfr_id+1] = *p_val;
            
            if(*p_val<0.0) *p_val = bsfr;
            svar[bsfr_id+2] = *p_val;        
        }
        
        
        // free memory
        free(sfrnn_input);
        free(sfrnn_output);
        
        return(0);  
    }
    
    else{
        return(1);
    }  
}




