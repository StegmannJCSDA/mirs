/***************************************************************************
 *  Program Name      : fun_tis_xgb.c
 *  Type              : subroutine
 *  Function          : eXtreme Gradient Boosting functions 
 *  Subroutine Called : rd_tis_xgb, xgb_classifier,
 *  Called by         : snowfall(), cal_vars()
 *  Author            : Yongzhen Fan, yfan1236@umd.edu, 10/02/2021
 **************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

float sigmoid(float);
//*************************************************************************
//XGB array
extern int       *flg_rd_xgb, *flg_do_xgb;
extern  int      *ninput_xgb,*ntrees,**varid_xgb, *nnodes, *nfilter;
extern  float    **flt_xgb;
extern double    ***xgb_trees;
//*************************************************************************

int rd_tis_xgb(char *ffn)
{
    FILE    *xgbfid;
    int     i,j;
    
    // read in xgb trees
    xgbfid = fopen(ffn,"r");
    
    if (xgbfid == NULL){
        printf("Could not open file %s\n", ffn);
        exit(1);        
    }
    printf("Successfully open file: %s\n", ffn);
    
    //read in flags rd_xgb and do_xgb
    flg_rd_xgb = (int*) malloc(sizeof(int)*1);
    if (flg_rd_xgb == NULL) {
        perror("Error allocating memory for flg_rd_xgb.");
        exit(1);
    }
    fscanf(xgbfid,"%i",&flg_rd_xgb[0]);
    
    flg_do_xgb = (int*) malloc(sizeof(int)*1);
    if (flg_do_xgb == NULL) {
        perror("Error allocating memory for flg_do_xgb.");
        exit(1);
    }
    fscanf(xgbfid,"%i",&flg_do_xgb[0]);
    // reset do flag if rd flag is 0
    if (*flg_rd_xgb==0) flg_do_xgb[0]=0;
    //printf("rd_xgb: rd_xgb=%i, do_xgb=%i\n",*flg_rd_xgb,*flg_do_xgb);
    //read in number of inputs, trees and maxlevel
    if (*flg_rd_xgb==1){
        ninput_xgb = (int*) malloc(sizeof(int)*1);
        if (ninput_xgb == NULL) {
            perror("Error allocating memory for ninput_xgb.");
            exit(1);
        }
        fscanf(xgbfid,"%i",&ninput_xgb[0]);
        
        ntrees = (int*) malloc(sizeof(int)*1);
        if (ntrees == NULL) {
            perror("Error allocating memory for ntrees.");
            exit(1);
        }
        fscanf(xgbfid,"%i",&ntrees[0]);
        
        nfilter = (int*) malloc(sizeof(int)*1);
        if (nfilter == NULL) {
            perror("Error allocating memory for nfilter.");
            exit(1);
        }
        fscanf(xgbfid,"%i",&nfilter[0]);
        
        //printf("ninput=%i, ntrees=%i\n",ninput_xgb[0],ntrees[0]);
        
        //allocate input varid array
        varid_xgb = (int**) malloc(sizeof(int*) * ninput_xgb[0]);
        if (!varid_xgb) {
            perror("Error allocating memory for varid_xgb.");
            exit(1);
        }
        //read in varid_xgb
        for (i=0; i<ninput_xgb[0]; i++){
            varid_xgb[i] = (int*) malloc(sizeof(int) * 4);
            if (varid_xgb[i] == NULL) {
                perror("Error allocating memory for varid_xgb.");
                exit(1);
            }
            fscanf(xgbfid,"%i %i %i %i", &varid_xgb[i][0],&varid_xgb[i][1],\
                                        &varid_xgb[i][2],&varid_xgb[i][3]);
            //printf("%i %i %i %i\n", varid_xgb[i][0],varid_xgb[i][1],varid_xgb[i][2],varid_xgb[i][3]);
        }
        //printf("Read in input type\n");
        
        //allocate filter array
        flt_xgb = (float**) malloc(sizeof(float*) * nfilter[0]);
        if (!flt_xgb) {
            perror("Error allocating memory for flt_xgb.");
            exit(1);
        }
        //read in flt_xgb
        for (i=0; i<nfilter[0]; i++){
            flt_xgb[i] = (float*) malloc(sizeof(float) * 5);
            if (flt_xgb[i] == NULL) {
                perror("Error allocating memory for flt_xgb.");
                exit(1);
            }
            fscanf(xgbfid,"%f %f %f %f %f", &flt_xgb[i][0],&flt_xgb[i][1],\
                                        &flt_xgb[i][2],&flt_xgb[i][3],&flt_xgb[i][4]);
            //printf("%i %i %i %i\n", flt_xgb[i][0],flt_xgb[i][1],flt_xgb[i][2],flt_xgb[i][3]);
        }
        //printf("Read in input type\n");
        
        // allocate nnode array
        nnodes = (int*) malloc(sizeof(int)*ntrees[0]);
        if (nnodes == NULL) {
            perror("Error allocating memory for nnodes.");
            exit(1);
        }
        // read in nnode for each tree
        for (i=0; i<ntrees[0];i++){
            fscanf(xgbfid, "%i", &nnodes[i]);
        }
        //printf("Read in node data\n");
        
        // allocate tree array
        xgb_trees = malloc(sizeof(double **) * ntrees[0]);
        for (i=0; i<ntrees[0];i++){    
            xgb_trees[i] = malloc(sizeof(double *) * nnodes[i]);
            for (j=0; j<nnodes[i]; j++){
                xgb_trees[i][j] = malloc(sizeof(double) * 5);
            }
        }
        //printf("mem allocated for xgb trees\n");
        // read in all tree data
        for (i=0;i<ntrees[0];i++){
            for (j=0;j<nnodes[i];j++){
                fscanf(xgbfid, "%lf", &xgb_trees[i][j][0]);
                if (xgb_trees[i][j][0]==0){
                    fscanf(xgbfid, "%lf", &xgb_trees[i][j][1]);
                    fscanf(xgbfid, "%lf", &xgb_trees[i][j][2]);
                    fscanf(xgbfid, "%lf", &xgb_trees[i][j][3]);
                    fscanf(xgbfid, "%lf", &xgb_trees[i][j][4]);
                    //printf("%lli %lli %lli %lli %lli\n",xgb_trees[i][j][0],xgb_trees[i][j][1],xgb_trees[i][j][2],xgb_trees[i][j][3],xgb_trees[i][j][4]);
                }
                else{
                    fscanf(xgbfid, "%lf", &xgb_trees[i][j][1]);
                    xgb_trees[i][j][2] = -99.0;
                    xgb_trees[i][j][3] = -99.0;
                    xgb_trees[i][j][4] = -99.0;
                    //printf("%lli %lli %lli %lli %lli\n",xgb_trees[i][j][0],xgb_trees[i][j][1],xgb_trees[i][j][2],xgb_trees[i][j][3],xgb_trees[i][j][4]);
                }
                
            }
        }        
    }
    //printf("Read in all trees\n");
    fclose(xgbfid);
    
    return(0);    
}

int xgb_classifier(float *svar, float **avar, int *ntree, int *ninput,int **varid_xgb, 
                    int *nfilter, float **flt_xgb, double ***tree, float *prob, int *class)
{
    int        i, j, onleaf, next_node, var_id, idx;
    double     total_score, current_node[5], *input;

    // apply filters
    for (i=0;i<nfilter[0];i++){
        idx=round(flt_xgb[i][1]);
        if (round(flt_xgb[i][0])==0){
            if (round(flt_xgb[i][2])==1){                
                if (svar[idx] > flt_xgb[i][3]){
                    *prob = 0.0;
                    *class = 0;
                    return(0);
                }
            }
            else{
                if (svar[idx] < flt_xgb[i][3]){
                    *prob = 0.0;
                    *class = 0;
                    return(0);
                }
            }                    
        }
        else{
            if (round(flt_xgb[i][2])==1){
                if (round(flt_xgb[i][4])==0){
                   if (avar[idx][0] > flt_xgb[i][3]){
                        *prob = 0.0;
                        *class = 0;
                        return(0);
                    } 
                }
                else{
                    if ((avar[idx][1] > flt_xgb[i][3]) & (avar[idx][2] > flt_xgb[i][3])){
                        *prob = 0.0;
                        *class = 0;
                        return(0);
                    }
                }
                    
            }
            else{
                if (round(flt_xgb[i][4])==0){
                    if (avar[idx][0] < flt_xgb[i][3]){
                        *prob = 0.0;
                        *class = 0;
                        return(0);
                    }
                }
                else{
                    if ((avar[idx][1] < flt_xgb[i][3]) & (avar[idx][2] < flt_xgb[i][3])){
                        *prob = 0.0;
                        *class = 0;
                        return(0);
                    }
                }
                    
            } 
        }
    }
    
    total_score = 0.5;
    //printf("XGB SD classification...\n");
    //allocate memeory for input
    input = (double*) malloc(sizeof(double) * ninput[0]);
    if (input == NULL) {
        perror("Error allocating memory for xgb_input.");
        exit(1);
    }
    
    // form the input for XGB   
    for (i=0; i<ninput[0]; i++){
        if(varid_xgb[i][0]==0){
            if(varid_xgb[i][1]==0){
                input[i] = svar[varid_xgb[i][2]];
            }
            else{
                input[i] = svar[varid_xgb[i][2]] 
                    - svar[varid_xgb[i][3]];
            }
        }
        else{
            if(varid_xgb[i][1]==0){
                input[i] = avar[varid_xgb[i][2]][0];
            }
            else{
                input[i] = avar[varid_xgb[i][2]][0] 
                    - avar[varid_xgb[i][3]][0];
            }
        }
        //printf("varid=%i %i %i %i, input[%i]=%lf\n",varid_xgb[i][0],varid_xgb[i][1],varid_xgb[i][2],varid_xgb[i][3],i,input[i]);
        //printf("%lf ",input[i]);
    }

    for (i=0;i<ntree[0];i++){
        // initialize tree status
        onleaf = 0;
        for (j=0;j<5;j++){
            current_node[j] = tree[i][0][j];
        }
        //printf("Tree[%i]: 0->",i);
        while (onleaf == 0){
            //printf("tree=%i, iter=%i, onleaf=%i, curr.node= %lli, %lli, %lli, %lli, %lli\n", i, iter, onleaf, current_node[0],current_node[1],current_node[2],current_node[3],current_node[4]);
            if (current_node[0]==1){
                onleaf = 1;
                total_score += current_node[1];
                //printf("leaf=  %0.9f\n",current_node[1]);
            }
            else{                
                var_id=round(current_node[1]);
                next_node = (input[var_id]<current_node[2]) ? round(current_node[3]):round(current_node[4]);
                //printf("varid=%i, input=%f, split=%f\n", var_id,input[var_id], current_node[2]/scaling);
                //printf("%i->",next_node);
                for (j=0;j<5;j++){
                    current_node[j] = tree[i][next_node][j];
                }
            }
        }
    }
 
    *prob = sigmoid((float) total_score);
    //printf("Total score: %0.9f\n",total_score);
    //printf("Prob: %0.9f\n",dummy);
    
    if (*prob > 0.5){
        *class = 1;
    }
    else{
        *class = 0;
    }    
    //printf("Tot.S:%lf, prob: %f, class:%i\n",total_score,*prob,*class);
    
    // free memory
    free(input);
    return(0);
}


