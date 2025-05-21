/***********************************************************************        
 *  File    name  :  constants.h
 *  Date          :  11/15/2006             
 *  Contains the constants used in orbital processing.
 *   
 ***********************************************************************/ 
#pragma once
/*-------------------------------------------------
 * define input argument number
 *-------------------------------------------------*/
#define narg 13
/*-------------------------------------------------
 * define start algorithm number with SFR
 *-------------------------------------------------*/
#define alg_num_sfr 4150
/*-------------------------------------------------
 * define satelite ID 
 *-------------------------------------------------*/
#define NPP 6
#define N20 20
#define N21 23
#define METOPSGA1 22
#define N18 1
#define METOPA 2
#define METOPB 14
#define METOPC 21
#define N19 4
#define F16 4
#define F17 4
#define F18 4
#define SAPHIR 4
#define GPM 10
/**************************************************
 *  Orbital part:
 *-------------------------------------------------
 *  lat/lon limits for SSM/I
 *-------------------------------------------------*/
#define LAT_LOWLIMIT   0
#define LAT_HILIMIT    18000
#define LON_LOWLIMIT   0
#define LON_HILIMIT    36000

/*-------------------------------------------------
 * File name
 *-------------------------------------------------*/
/*#define SWATH_NAME        "MIRS_Swath"*/

/*-------------------------------------------------
 * maximum scanline, view and channel
 *-------------------------------------------------*/
#define MAXSCAN        8000
#define MAXFOV         208
#define MAXCH          24
#define MAX_ORB        30
#define MAXLAY         100

/*-------------------------------------------------
 * Parameters for different files
 *-------------------------------------------------*/
#define PARM           0
#define SDR            1
#define EDR            2
#define SHDF           3
#define EHDF           4

/*-------------------------------------------------
 * flags
 *-------------------------------------------------*/
#define FILLER         -1 
#define BT_SCALE       0.015
#define SCALE          100

/**************************************************
 * Mapping part constants:
 *-------------------------------------------------
 * grid box size and polar stereographic size
 *-------------------------------------------------*/

#define  GRID_LAT       360
#define  GRID_LON       720
#define  MESH           512
#define  GRID           64.0/MESH
#define  MAXPNUM        3
#define  MISSING        -999
#define  ANCI_MISSING   0 
#define  PSMISSING      -9999
#define  NAVA           -888

/**************************************************
 * New scaling constants that matches AMSU products
 **************************************************/

#define TEMP_SCAL      0.015
#define TPW_SCAL       0.1
#define CLW_SCAL       1.
#define RWP_SCAL       1.
#define LWP_SCAL       1.
#define SWP_SCAL       0.01
#define IWP_SCAL       0.01
#define GWP_SCAL       1.
#define RR_SCAL        1.

#define SWE_SCAL       0.01
#define SNOWGS_SCAL    0.01
#define SNOW_SCAL      1.
#define STEMP_SCAL     10.
#define SICE_SCAL      1.
#define SFR_PROB_SF_SCAL      1.
#define SFR_PROB_RF_SCAL      1.

#define TSKIN_SCAL     1.
#define SURFP_SCAL     0.1
#define EMIS_SCAL      0.0001

#define SFR_SCAL        0.01
#define CLDTOP_SCAL     0.1
#define CLDBASE_SCAL    0.1
#define CLDTHICK_SCAL   0.1
#define PRECIPTYPE_SCAL 1.
#define RFLAG_SCAL      1.

#define SURFM_SCAL      0.1
#define WINDSP_SCAL     0.01
#define WINDDIR_SCAL    0.01
#define WINDU_SCAL      0.01
#define WINDV_SCAL      0.01
