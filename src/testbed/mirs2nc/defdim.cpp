/******************************************************
 *	Implementation of subroutines that are defined in 
 *  defualt.h but not implemented in any other files.
 *****************************************************/
#include  "defaults.h"

/**
* The reason is that mirs binary output file EDR is in Big-Endian format.
* It need to be converted into native machine format.
* Also fortran write binary has an 4-byte overhead at begining and at end.
*/

/**
* return 1 if little endian machine,
* return 0 if big endian machine.
*/

int getEndian()
{
  union {
  int theInteger;
  char theByte;
  } endianUnion;
  
  endianUnion.theInteger = 1 ;
  return endianUnion.theByte;
}


int intSwap(char *value)
{
  char buffer[ 4 ];
  
  if( getEndian() == 1 ) {
    buffer[ 0 ] = value[ 3 ];
    buffer[ 1 ] = value[ 2 ];
    buffer[ 2 ] = value[ 1 ];
    buffer[ 3 ] = value[ 0 ];
  }
  else {
    buffer[ 0 ] = value[ 0 ];
    buffer[ 1 ] = value[ 1 ];
    buffer[ 2 ] = value[ 2 ];
    buffer[ 3 ] = value[ 3 ];
  }

  return *( (int *) &buffer );
}


short int shortSwap(char *value)
{
  char buffer[ 2 ];
  
  if( getEndian() == 1 ) {
    buffer[ 0 ] = value[ 1 ];
    buffer[ 1 ] = value[ 0 ];
  }
  else {
    buffer[ 0 ] = value[ 0 ];
    buffer[ 1 ] = value[ 1 ];
  }
  return *( (short int *) &buffer );
}


float floatSwap(char *value)
{
  char buffer[ 4 ];
  
  if( getEndian() == 1 ) {
    buffer[ 0 ] = value[ 3 ];
    buffer[ 1 ] = value[ 2 ];
    buffer[ 2 ] = value[ 1 ];
    buffer[ 3 ] = value[ 0 ];
  }
  else {
    buffer[ 0 ] = value[ 0 ];
    buffer[ 1 ] = value[ 1 ];
    buffer[ 2 ] = value[ 2 ];
    buffer[ 3 ] = value[ 3 ];
  }
  
  return *( (float *) &buffer );
}

/*---------------------------*
* Generate netcdf file name
*---------------------------*/
void gen_nc_fname( char *fname_edr, char *nc_name_img, char *output_img,
                   char *nc_name_snd, char *output_dir, char *output_snd )
{

  /*
  if( strcmp(satid,"NA") == 0 )
  gnrt_nc_fname_npp(fname_edr, output_dir, nc_name_snd, nc_name_img);
  else
  gnrt_nc_fname(fname_edr, output_dir, nc_name_snd, nc_name_img);
  printf("The output sounding & imaging netcdf files:\n%s\n%s\n\n", nc_name_snd, nc_name_img);
  */
  
  strcpy(nc_name_img,output_dir);
  strcat(nc_name_img,output_img);
  
  strcpy(nc_name_snd,output_dir);
  strcat(nc_name_snd,output_snd);
  
  printf("Output sounding & imaging netcdf4 files:\n%s\n%s\n\n", nc_name_snd, nc_name_img);

}

/*------------------------------------
 * Check if stat is equal to NC_ERROR
 *-----------------------------------*/
void check_err(const int stat, const int line, const char *file, int err_id)
{
  if (stat != NC_NOERR) {
    (void) fprintf(stderr, "line %d of %s: %s\n", line, file, nc_strerror(stat));
    exit(err_id);
  }
}

/*----------------------------------------------
 * Wrte out text global attribute to a NC file
 *--------------------------------------------*/
void  put_gatt_txt(int ncid, const char* name, const char* var)
{
  int stat, errid = 15;
  stat = nc_put_att_text(ncid, NC_GLOBAL, name, strlen(var), var);
  check_err(stat,__LINE__,__FILE__,errid);
}

/*--------------------------------------------------------------
 * Define a dimention appearing at the beginning of a NC file
 *------------------------------------------------------------*/
void  dfdim(int ncid, const char* name, size_t len, int* idp)
{
  int stat, errid = 15;
  stat = nc_def_dim(ncid, name, len, idp);
  check_err(stat,__LINE__,__FILE__,errid);
}

/*-----------------------------------------------
 * Wrte out text variable attribute to a NC file
 *----------------------------------------------*/
void  put_vatt_txt(int ncid, int vid, const char* name, const char* value)
{
  int stat, errid = 15;
  stat = nc_put_att_text(ncid, vid, name, strlen(value), value);
  check_err(stat,__LINE__,__FILE__,errid);
}


