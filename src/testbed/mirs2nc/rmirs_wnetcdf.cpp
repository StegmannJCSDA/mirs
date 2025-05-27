/****************************************************************************************
 *  Program Name      : rmirs_wnetcdf.cpp
 *  Type              : Subroutine
 *  Function          : Program reads MIRS  generated products
 *                      and creates netcdf file 
 *  Input Files       : EDR binary data and the output directory
 *  Output Files      : Netcdf files
 *  Subroutine Called : gnrt_nc_fname.cpp,  openfs_sw.cpp, rmirs_edr.cpp, rmirs_dep.cpp,
 *                      set_nc_snd.cpp, set_nc_img.cpp
 *  Called by         : mirs2nc.cpp
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *  
 *  12/05/2008      v              JZhao (PSGS)
 *  12/20/2016                     JChen (ESSIC/UMD) Added JPSS1 (N20)
 *  02/24/2020      v2             Ming Fang
 *                                 1. Changed workflow 
 *                                 2. Changed staic space allocaion to dynamical
 *                                    allocation, which reduces required maximum memory
 *                                 3. Made necessary associatd changes
 ****************************************************************************************/
#include <iostream>
#include "constants.h"
#include "swath.h"
#include <string>

using namespace std;
using std::string;

/*-----------------------------------------------------*
* If an orbit has scan number larger than an allowed
* limit (MAXSCAN = 1200 ), it'll be cut to the
* limit to prevent the program from crashing.
*-----------------------------------------------------*/
void mirs2nc_class::check_scan_numb(char *start_orbit_number_str,
                      char *end_orbit_number_str,
                      char *prod_site_str,
                      char *prod_environment_str,
                      short int numscan,short int numchan,
                      short int numspot,short int numlayer)
{

    printf("Data dimension of orbit: nscan=%d, npos=%d, nchan=%d, nlay=%d\n", numscan,numspot,numchan,numlayer);
    sscanf(start_orbit_number_str,"%d",&start_orbit_number[0]);
    sscanf(end_orbit_number_str,"%d",&end_orbit_number[0]);
    
    
    production_site_len=strlen(prod_site_str);
    strncpy(production_site_str,prod_site_str,production_site_len);
    production_environment_len=strlen(prod_environment_str);
    strncpy(production_environment_str,prod_environment_str,production_environment_len);
    
    if( numscan > MAXSCAN )
    {
    //numscan = MAXSCAN;
    printf("numscan > MAXSCAN, numscan=%d, MAXSCAN=%d\n", numscan, MAXSCAN );
    exit(13);
    }
    
    if( numchan > MAXCH ) {
    printf("numchan > MAXCH, numchan=%d, MAXCH=%d\n", numchan, MAXCH);
    exit(13);
    }
    
    if( numspot > MAXFOV  ) {
    printf("numspot > MAXFOV, numspot=%d, MAXFOV=%d\n", numspot, MAXFOV);
    exit(13);
    }
    
    if( numlayer > MAXLAY ) {
    printf("numlayer > MAXLAY, numlayer=%d, MAXLAY=%d\n", numlayer, MAXLAY);
    exit(13);
    }
}

/*******************************************************
*Figure out the EDR/DEP file nude name from input files
********************************************************/
void mirs2nc_class::get_base_name(int intsatid, char *fname_edr, char *fname_dep, char *input_edr, char *input_dep)
{
    char *pch;
    
    fname_edr = strstr(input_edr,"EDR_");
    fname_dep = strstr(input_dep,"DEP_");
    
    strcpy(EDR_name,fname_edr);
    strcpy(DEP_name,fname_dep);
    
    if( intsatid == NPP ) {
    pch = strstr(fname_edr,"_npp_d");
    strcpy(RDR_name,"SATMS");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".LR.ORB" or ".HR.ORB"
    }
    else if( intsatid == N20 ) {
    pch = strstr(fname_edr,"_n20_d");
    strcpy(RDR_name,"SATMS");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".LR.ORB" or ".HR.ORB"
    }
    else if( intsatid == N21 ) {
    pch = strstr(fname_edr,"_n21_d");
    strcpy(RDR_name,"SATMS");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".LR.ORB" or ".HR.ORB"
    }
    else if( intsatid == METOPSGA1 ) {
    pch = strstr(fname_edr,"_metopSGA1_d");
    strcpy(RDR_name,"EPS");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".LR.ORB" or ".HR.ORB"
    }
    else if ( intsatid == N18 ) {
    pch = strstr(fname_edr,".NN.D");
    strcpy(RDR_name,"NSS.AMAX");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".LR.ORB" or ".HR.ORB"
    }
    else if ( intsatid == METOPA ) {
    pch = strstr(fname_edr,".M2.D");
    strcpy(RDR_name,"NSS.AMAX");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".LR.ORB" or ".HR.ORB"
    }
    else if ( intsatid == METOPB ) {
    pch = strstr(fname_edr,".M1.D");
    strcpy(RDR_name,"NSS.AMAX");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".LR.ORB" or ".HR.ORB"
    }
    else if ( intsatid == METOPC ) {
    pch = strstr(fname_edr,".M3.D");
    strcpy(RDR_name,"NSS.AMAX");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".LR.ORB" or ".HR.ORB"
    }
    else if ( intsatid == N19 ) {
    pch = strstr(fname_edr,".NP.D");
    strcpy(RDR_name,"NSS.AMAX");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".LR.ORB" or ".HR.ORB"
    }
    else if ( intsatid == F16 ) {
    pch = strstr(fname_edr,".SA.D");
    strcpy(RDR_name,"NPR.TDRN");
    strncat(RDR_name,pch,strlen(pch)-14); // strip trailing ".CALIB.UAS.ORB"
    }
    else if ( intsatid == F17 ) {
    pch = strstr(fname_edr,".SB.D");
    strcpy(RDR_name,"NPR.TDRN");
    strncat(RDR_name,pch,strlen(pch)-8); // strip trailing ".UAS.ORB" or ".IMG.ORB"
    }
    else if ( intsatid == F18 ) {
    pch = strstr(fname_edr,".SC.D");
    strcpy(RDR_name,"NPR.TDRN");
    strncat(RDR_name,pch,strlen(pch)-8); // strip trailing ".UAS.ORB"
    }
    else if ( intsatid == SAPHIR ) {
    pch = strstr(fname_edr,"_1.08_");
    strcpy(RDR_name,"MT1SAPSL1A2");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".CR.ORB"
    }
    else if ( intsatid == GPM ) {
    pch = strstr(fname_edr,".XCAL");
    strcpy(RDR_name,"1C-R.GPM.GMI");
    strncat(RDR_name,pch,strlen(pch)-7); // strip trailing ".HR.ORB"
    }
}    

/*****************************************************************/
void mirs2nc_class::rmirs_wnetcdf(char *input_edr, char *input_dep, char *output_dir, char *output_img, char *output_snd,  
                       char *start_orbit_number_str, char *end_orbit_number_str, char *prod_site_str, 
                       char *prod_environment_str, char *mirs_major, char *mirs_minor, char *hist_pack )
{
   FILE        *fp_edr, * fp_dep;

   short int   numscan, numspot,numlayer,numchan;
   char        nc_name_snd[300], nc_name_img[300];
   char*       fname_edr = NULL;
   char*       fname_dep = NULL;
   char         satid[8]="";
   int         intsatid=-99;

/* Generate the MiRS version strings for the IMG and SND attributes */
   strncpy(dap_ver_str, "v", MAXCSTR);
   strncat(dap_ver_str, mirs_major, strlen(mirs_major));
   strncat(dap_ver_str, "r", 1);
   strncat(dap_ver_str, mirs_minor, strlen(mirs_minor));

   strncpy(mirs_ver_str, "Created by MIRS Version ", MAXCSTR);
   strncat(mirs_ver_str, mirs_major, strlen(mirs_major));
   strncat(mirs_ver_str, ".", 1);
   strncat(mirs_ver_str, mirs_minor, strlen(mirs_minor));

   strncpy(hist_pack_str, hist_pack, MAXCSTR);

/*-----------------------------------------------------*
 * Initialize the AT and product arrays
 *-----------------------------------------------------*/

/*  We comment this out, not necessary

// check the consistency of EDR and DEP files : OSDPD version
     fname_dep = strstr(input_dep,".D");
     temp = strstr(input_edr,".D");

     if(strcmp(fname_dep,temp)!= 0){
       printf("The input edr & dep name are not consistent!\n %s\n %s\n",fname_dep,temp);
       exit(3);
     }

// check the consistency of EDR and DEP files : NDE NPP ATMS version

     fname_dep = strstr(input_dep,"_d");
     temp   = strstr(input_edr,"_d");

     if( strcmp(fname_dep,temp) != 0 ){
       printf("The input edr & dep name are not consistent!\n %s\n %s\n",fname_dep,temp);
       exit(3);
     }

*/


/*********************added here to get the info of satid  ************/

   if ( strstr(input_dep,".NN") > 0) {
     strcpy(satid,"NN");
     intsatid=1;
    }else if( strstr(input_dep,".NP") > 0) {
     strcpy(satid,"NP");    
     intsatid=4;
   }else if( strstr(input_dep,".M2") > 0) {
     strcpy(satid,"M2");
     intsatid=2; 
   }else if( strstr(input_dep,".M1") > 0) {
     strcpy(satid,"M1");
     intsatid=14; 
   }else if( strstr(input_dep,".M3") > 0) {
     strcpy(satid,"M3");
     intsatid=21; 
   }else if( strstr(input_dep,".SA") > 0) {
     strcpy(satid,"SA");
     intsatid=3;
   }else if( strstr(input_dep,".SB") > 0) {
     strcpy(satid,"SB");
     intsatid=18;
   }else if( strstr(input_dep,".SC") > 0) {
     strcpy(satid,"SC");
     intsatid=5;
   }else if( strstr(input_dep,"npp_d") > 0) {
     strcpy(satid,"NPP");
     intsatid=6;
   }else if( strstr(input_dep,"n20_d") > 0) {
     strcpy(satid,"N20");
     intsatid=20;
   }else if( strstr(input_dep,"j01_d") > 0) {
     strcpy(satid,"N20");
     intsatid=20;
   }else if( strstr(input_dep,"n21_d") > 0) {
     strcpy(satid,"N21");
     intsatid=23;
   }else if( strstr(input_dep,"j02_d") > 0) {
     strcpy(satid,"N21");
     intsatid=23;
   }else if( strstr(input_dep,"metopSGA1_d") > 0) {
     strcpy(satid,"metopSGA1");
     intsatid=22;
   }else if( strstr(input_dep,"SR_E") > 0) {
     strcpy(satid,"AE");
     intsatid=7;
   }else if( strstr(input_dep,"_MT1SA") > 0) {
     strcpy(satid,"MTSA");
     intsatid=13;
   }else if( strstr(input_dep,"GPM") > 0) {
     strcpy(satid,"GPM");
     intsatid=10;
   }else{
     strcpy(satid,"XX");
   }
   
/************************************************     
 *        Read the input Mirs EDR data  
 ************************************************/
     printf("Input edr & dep files:\n%s\n%s\n", input_edr, input_dep);

     fp_edr = openfs_sw(input_edr);
     rmirs_edr(fp_edr, &numscan, &numspot, &numlayer, &numchan, satid);
     fclose(fp_edr);
//liusy+ for filename timestamp
     string output_snd_new;
     string output_img_new;

     char timestamp_start_seconds[3];
     char timestamp_end_seconds[3];
     char timestamp_start_minutes[3];
     char timestamp_end_minutes[3];

     memset(timestamp_start_seconds,'\0',3);
     memset(timestamp_end_seconds,'\0',3);
     memset(timestamp_start_minutes,'\0',3);
     memset(timestamp_end_minutes,'\0',3);

     strncpy(timestamp_start_seconds,time_coverage_start+17,2); //e.g. time_coverage_start=2022-09-14T13:41:14Z
     strncpy(timestamp_end_seconds,time_coverage_end+17,2); //e.g. time_coverage_end=2022-09-14T15:21:03Z
     strncpy(timestamp_start_minutes,time_coverage_start+14,2); //e.g. time_coverage_start=2022-09-14T13:41:14Z
     strncpy(timestamp_end_minutes,time_coverage_end+14,2); //e.g. time_coverage_end=2022-09-14T15:21:03Z

	timestamp_start_seconds[2]='\0';
	timestamp_end_seconds[2]='\0';
	timestamp_start_minutes[2]='\0';
	timestamp_end_minutes[2]='\0';

     output_snd_new = output_snd;
     output_img_new = output_img;

     size_t found_pos;
     char str_snd[MAXCSTR];
     char str_img[MAXCSTR];

	if (intsatid == 2 || intsatid == 14 || intsatid == 21) {
		found_pos = output_snd_new.find("000_e");
		if (found_pos != string::npos){
			output_snd_new.replace(found_pos,2,timestamp_start_seconds);
			output_snd_new.replace(found_pos-2,2,timestamp_start_minutes);
		}

		found_pos = output_snd_new.find("000_c");
		if (found_pos != string::npos){
			output_snd_new.replace(found_pos,2,timestamp_end_seconds);
			output_snd_new.replace(found_pos-2,2,timestamp_end_minutes);
		}



		//==============================================================
		found_pos = output_img_new.find("000_e");
		if (found_pos != string::npos){
			output_img_new.replace(found_pos,2,timestamp_start_seconds);
			output_img_new.replace(found_pos-2,2,timestamp_start_minutes);
		}
		found_pos = output_img_new.find("000_c");
		if (found_pos != string::npos){
			output_img_new.replace(found_pos,2,timestamp_end_seconds);
			output_img_new.replace(found_pos-2,2,timestamp_end_minutes);
		}

	}
	// copy the string to the char* after modifications if the sat needs changes
	// if not then just copy the string back to char
	strcpy(str_snd,output_snd_new.c_str());
	strcpy(str_img,output_img_new.c_str());

     //// ===========================================================
     //// this is very important, this the only place foname_snd/foname_img are set  --20230125-ismail

     strncpy(foname_snd, output_snd, MAXCSTR);
     strncpy(foname_img, output_img, MAXCSTR);
//liusy.
//
     /*-- Check Scan Number --*/
     check_scan_numb(start_orbit_number_str,end_orbit_number_str,prod_site_str,
                prod_environment_str,numscan,numchan,numspot,numlayer);
     
     /*-- Get Base Name of EDR/DEP File --*/
     get_base_name(intsatid, fname_edr, fname_dep, input_edr, input_dep);
     
     /*-- Generate NETCDF File Name --*/     
     //gen_nc_fname(fname_edr, nc_name_img, output_img, nc_name_snd, output_dir, output_snd);
     gen_nc_fname(fname_edr, nc_name_img, str_img, nc_name_snd, output_dir, str_snd);

     /*-- Write EDR data to NC file --*/
     set_nc_snd(nc_name_snd, numscan,numspot,numlayer,numchan,satid);

/************************************************
 *        Read the input Mirs DEP data
 ************************************************/
 
     fp_dep = openfs_sw(input_dep);
     rmirs_dep(fp_dep, &numscan, &numspot, satid); //allocate memory and read in dep
     fclose(fp_dep);
     
     /*-- Write DEP data to NC file --*/    
     set_nc_img(nc_name_img, numscan,numspot,numchan,satid);
     
/****************************************************************
 *  Write Sfc_type, Atm_type and global attributes to MIRS-SND 
 *  file, because former two are read in from DEP, not EDR
 ***************************************************************/     
     
     wrt_Sfc_Atm(numscan,numspot,satid,nc_name_snd);

/*--------------------------------------------------------
 * Write out data in binary format
 *--------------------------------------------------------*/

}  /* end of rmirs_wnetcdf.c */


/************************************************************************************
*  Following parameters are output at the latest minute because Sfc_type, Atm_type
*  and some global attributes need to calculate using variables in DEP file
************************************************************************************/
void mirs2nc_class::wrt_Sfc_Atm(short int nscan, short int nspot, char* satid, char* nc_name_snd)
{
  int ncid_snd, stat_snd;
  int errid = 16;
  size_t start2[]={0,0};
  size_t count2[]={size_t(nscan),size_t(nspot)};

  stat_snd = nc_open(ncFname_snd, NC_WRITE, &ncid_snd);
  check_err(stat_snd,__LINE__,__FILE__,errid);

  //Output Sfc_type and Atm_type, they are generated in DEP not EDR

  put_nc_snd_img( ncid_snd, "Sfc_type", start2, count2, iTypSfc );
  put_nc_snd_img( ncid_snd, "Atm_type", start2, count2, iTypAtm );

  //Output global attributes
  gnrt_attr_snd(ncid_snd, satid, date_created_snd, nc_name_snd);

  stat_snd = nc_close(ncid_snd);
  check_err(stat_snd,__LINE__,__FILE__,errid);

}

