/***************************************************************************************************
 * 
 * This is the C++ decoder for metopSGA1 MWS data.
 *
 * Author: Yong-Keun Lee <yong-keun.lee@noaa.gov>
 *
 * Date: 12/09/2020 
 *
 ***************************************************************************************************/

#include <iostream>
#include <fstream>
#include <memory>
#include <new>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <string>
#include <cstring>
#include <stdlib.h>

#include "hdf5.h"

using namespace std;



////////////////////////////////////////////////////////////////////////////////////////////
//
// some global variables
// 
////////////////////////////////////////////////////////////////////////////////////////////

const int nchan_mws = 24;
float freq_metopSGA1[nchan_mws] = { 23.800, 31.400, 50.300, 52.800, 53.246, 53.596, 53.948, 54.400, 54.940, 55.500, 57.290,    
      57.290, 57.290, 57.290, 57.290, 57.290, 89.000, 165.500, 183.310, 183.310, 183.310,183.310, 183.310, 229.000 } ;

int polar_metopSGA1[nchan_mws] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 2, 2, 2, 2, 2, 2 };
       
const int NQC = 4;

const int FILESIZE = 256;
const int FILENUM  = 8192;


/**
  struct tm time1900;      // 1900
  time1900.tm_sec   = 0;
  time1900.tm_min   = 0;
  time1900.tm_hour  = 0;
  time1900.tm_mday  = 1;
  time1900.tm_mon   = 0;
  time1900.tm_year  = 0;
  time1900.tm_wday  = 1;
  time1900.tm_yday  = 0;
  time1900.tm_isdst = 0;
  time_t t1900 = mktime(&time1900);
 
  struct tm time1958;      // 1958
  time1958.tm_sec   = 0;
  time1958.tm_min   = 0;
  time1958.tm_hour  = 0;
  time1958.tm_mday  = 1;
  time1958.tm_mon   = 0;
  time1958.tm_year  = 58;
  time1958.tm_yday  = 0;
  time1958.tm_isdst = 0;
  time_t t1958 = mktime(&time1958);

  // seconds of 1958 since 1900
  unsigned long secsBefore1958 = (unsigned long)(difftime( t1958, t1900 ));
*/

unsigned long secsBefore1958 = 1830297600;  //seconds from 1900 to 1957 including both 1900 and 1957
unsigned long secs1958_2019  = 1956528000;  //seconds from 1958 to 2019 including both 1958 and 2019

// before 2012-06-30 23:59:60, LEAP_SECONDS=34
// after  2012-06-30 23:59:60, LEAP_SECONDS=35
// after  2015-06-30 23:59:60, LEAP_SECONDS=36
// after  2016-12-31    23:59:60, LEAP_SECONDS=37
const int LEAP_SECONDS = 36;

class MeasurementTypeHeader {
public: 
   int nscan;
   int nchan;
   int nfov;
   int nqc;
   int nPosScan;
   float* freq; 
        int*  polar;
   
   // constructor
   MeasurementTypeHeader(int nscan_arg, int nchan_arg, int nfov_arg, int nqc_arg ) {
      nscan = nscan_arg;
      nchan = nchan_arg;
      nfov  = nfov_arg;
      nqc   = nqc_arg;
      freq  = new float[nchan];
      polar = new int[nchan];
   };
   
   // copy constructor
   MeasurementTypeHeader(const MeasurementTypeHeader& mh) {
      
      nscan = mh.nscan;
      nchan = mh.nchan;
      nfov  = mh.nfov;
      nqc   = mh.nqc;
      freq  = new float[nchan];
      polar = new int[nchan];
   
   }

   // destructor
   ~MeasurementTypeHeader() {
      delete [] freq;
      delete [] polar;
      freq  = NULL;
      polar = NULL;
   };
   
};



class MeasurementType {
public: 
   int      nchan      ;
   int      nfov         ;
   int      nqc          ;
   
      int      node       ;
      int      jday         ;
      int      year       ;
      int      secs       ;

      float*      lat        ;
      float*      lon        ;
      float*      angle        ;
      float*      relAziAngle  ;
      float*      solZenAngle  ;
      float**     tb           ;
      int*     qc           ;
        
   // constructor
      MeasurementType(int nch, int nfv, int nc) {
      
      nchan = nch;
      nfov  = nfv;
      nqc   = nc;
      
      lat = new float[nfov];        
      lon = new float[nfov];        
      angle = new float[nfov];   
      relAziAngle = new float[nfov];   
      solZenAngle = new float[nfov];
         qc = new int[nqc];
      
      tb =  new float *[nfov];
      for(int i = 0; i<nfov; i++ ) tb[i] = new float[nchan];
      };
   
   // copy constructor
      MeasurementType(const MeasurementType& ms) {
      
      nchan = ms.nchan;
      nfov  = ms.nfov;
      nqc   = ms.nqc;
      
      lat = new float[nfov];        
      lon = new float[nfov];        
      angle = new float[nfov];   
      relAziAngle = new float[nfov];   
      solZenAngle = new float[nfov];
         qc = new int[nqc];
      
      tb = new float *[nfov];
      for(int i = 0; i<nfov; i++ ) tb[i] = new float[nchan];
      
      };
   
   // destructor
   ~MeasurementType() {
      
      delete [] lat;
      delete [] lon;
      delete [] angle;
      delete [] relAziAngle;
      delete [] solZenAngle;
      delete [] qc;
      
      lat         = NULL;
      lon         = NULL;
      angle       = NULL;
      relAziAngle = NULL;
      solZenAngle = NULL;
      qc          = NULL;
      
      for(int i = 0; i<nfov; i++ ) 
      { 
        delete [] tb[i]; 
        tb[i] = NULL ; 
      }
        
      delete [] tb;
      tb = NULL;
   
   };
     
};


int writeMeasurementHdr(ofstream &myFile, MeasurementTypeHeader& header) {

    myFile.width(4); myFile << header.nscan ;
    myFile.width(4); myFile << header.nfov;
    myFile.width(4); myFile << header.nqc ;
    myFile.width(4); myFile << header.nchan << '\n';
    
    for( int ichan=0; ichan<header.nchan; ichan++ )
    {
       myFile << setprecision(5) << setw(10) << right << fixed << header.freq[ichan] ;
       if ( (((ichan+1) % 10) == 0) && ((ichan+1) < header.nchan) ) myFile << endl;
    }
    myFile << endl;
    
    for( int ichan=0; ichan<header.nchan; ichan++ )
    {
       myFile << setw(3) << header.polar[ichan] ;
       if ( (((ichan+1) % 20) == 0) && ((ichan+1) < header.nchan) ) myFile << endl;
    }
    myFile << endl;
    
    return 0;

}



int writeMeasurement(ofstream &myFile, MeasurementType& measurement) {
    
    int NFOV  = measurement.nfov;
    int NCHAN = measurement.nchan;
    int NQC   = measurement.nqc;

    myFile.width(4);  myFile << measurement.node ;
    myFile.width(10); myFile << measurement.jday;
    myFile.width(10); myFile << measurement.year;
    myFile.width(10); myFile << measurement.secs << '\n';

    // lat
    for(int ifov=0;ifov<NFOV;ifov++) {
      myFile << setprecision(2) << setw(8) << fixed << measurement.lat[ifov] ;
      if ( (((ifov+1) % 10) == 0) && ((ifov+1) < NFOV) ) myFile << endl;
    }
    myFile << endl;

    // lon
    for(int ifov=0;ifov<NFOV;ifov++) {
      myFile << setprecision(2) << setw(8) << fixed << measurement.lon[ifov] ;
      if ( (((ifov+1) % 10) == 0) && ((ifov+1) < NFOV) ) myFile << endl;
    }
    myFile << endl;

    // angle - Sensor Zenith Angle
    for(int ifov=0;ifov<NFOV;ifov++) {
      myFile << setprecision(2) << setw(8) << fixed << measurement.angle[ifov] ;
      if ( (((ifov+1) % 10) == 0) && ((ifov+1) < NFOV) ) myFile << endl;
    }
    myFile << endl;
    
    // relAziAngle
    for(int ifov=0;ifov<NFOV;ifov++) {
      myFile << setprecision(2) << setw(8) << fixed << measurement.relAziAngle[ifov] ;
      if ( (((ifov+1) % 10) == 0) && ((ifov+1) < NFOV) ) myFile << endl;
    }
    myFile << endl;
   
    // solZenAngle
    for(int ifov=0;ifov<NFOV;ifov++) {
      myFile << setprecision(2) << setw(8) << fixed << measurement.solZenAngle[ifov] ;
      if ( (((ifov+1) % 10) == 0) && ((ifov+1) < NFOV) ) myFile << endl;
    }
    myFile << endl;

    
    // !!!!  C/C++ is row major, while fortran/IDL is column major !!!!
    // !!!!  We write in Fortran/IDL order : row(ifov) changes faster than column(ichan)
    
    // tb[nfov][nchan]
    for(int ichan=0; ichan<NCHAN; ichan++) {
    for(int ifov=0;  ifov<NFOV; ifov++) {
      myFile << setprecision(2) << setw(8) << fixed << measurement.tb[ifov][ichan];
   
   // C/C++ order in memory
        // int index = ichan + ifov * NCHAN ;   
   
   // Fortran/IDL order in memory
   int index = ichan * NFOV + ifov;

      if ( (((index+1) % 10) == 0) && ( (index+1) < (NCHAN*NFOV) ) ) myFile << endl;
    }
    }
    myFile << endl;

    // qc 
    if ( NQC > 0 ) {
      for(int iqc=0; iqc<NQC; iqc++ ) {
      myFile.width(4); myFile << measurement.qc[iqc];
      if ( (((iqc+1) % 10) == 0) && ((iqc+1) < NQC) ) myFile << endl;
      }
    }
    myFile << endl;

    return 0;
}



/**  struct tm 
int    tm_sec   seconds [0,61]  tm_sec is generally 0-59. Extra range to accommodate for leap seconds in certain systems.
int    tm_min   minutes [0,59]
int    tm_hour  hour [0,23]
int    tm_mday  day of month [1,31]
int    tm_mon   month of year [0,11]
int    tm_year  years since 1900
int    tm_wday  day of week [0,6] (Sunday = 0)
int    tm_yday  day of year [0,365]
int    tm_isdst daylight savings flag 
       is greater than zero if Daylight Saving Time is in effect, 
       zero if Daylight Saving Time is not in effect, 
       and less than zero if the information is not available.
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


long long longlongSwap(char *value)
{
  char buffer[ 8 ];
  
  if( getEndian() == 1 ) {
    buffer[ 0 ] = value[ 7 ];
    buffer[ 1 ] = value[ 6 ];
    buffer[ 2 ] = value[ 5 ];
    buffer[ 3 ] = value[ 4 ];
    buffer[ 4 ] = value[ 3 ];
    buffer[ 5 ] = value[ 2 ];
    buffer[ 6 ] = value[ 1 ];
    buffer[ 7 ] = value[ 0 ];
  }
  else {
    buffer[ 0 ] = value[ 0 ];
    buffer[ 1 ] = value[ 1 ];
    buffer[ 2 ] = value[ 2 ];
    buffer[ 3 ] = value[ 3 ];
    buffer[ 4 ] = value[ 4 ];
    buffer[ 5 ] = value[ 5 ];
    buffer[ 6 ] = value[ 6 ];
    buffer[ 7 ] = value[ 7 ];
  }
  return *( (long long *) &buffer );
}


double doubleSwap(char *value)
{
  char buffer[ 8 ];

  if( getEndian() == 1 ) {
    buffer[ 0 ] = value[ 7 ];
    buffer[ 1 ] = value[ 6 ];
    buffer[ 2 ] = value[ 5 ];
    buffer[ 3 ] = value[ 4 ];
    buffer[ 4 ] = value[ 3 ];
    buffer[ 5 ] = value[ 2 ];
    buffer[ 6 ] = value[ 1 ];
    buffer[ 7 ] = value[ 0 ];
  }
  else {
    buffer[ 0 ] = value[ 0 ];
    buffer[ 1 ] = value[ 1 ];
    buffer[ 2 ] = value[ 2 ];
    buffer[ 3 ] = value[ 3 ];
    buffer[ 4 ] = value[ 4 ];
    buffer[ 5 ] = value[ 5 ];
    buffer[ 6 ] = value[ 6 ];
    buffer[ 7 ] = value[ 7 ];
  }
  return *( (double *) &buffer );
}


int BSWAP32(int x)  {
  
  int y =  ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | 
            ( ((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))  ;
  return y;

}


int cal2jday(int year, int month, int day )
{
  if ( day > 31 || day < 1 || month > 12 || month < 1 )  return -1;
  
  int JulianDate1[12] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 } ;
  int JulianDate2[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 } ;

  int leap = 0; 
  if ( ( year % 4 == 0 && year % 100 != 0 ) || year % 400 == 0 ) leap = 1;
  
  int jday;  
  if ( leap == 1 ) jday = JulianDate1[month-1] + day ;
  else         jday = JulianDate2[month-1] + day ;
  
  return jday;
}


/*--------------------------------------------------------------------------*/
/* This routine converts a date given in seconds into years, months, days,  */
/* hours, minutes and seconds which are fields in tm struct.  The zero      */
/* point of the date is set to 1900 Jan 1, at 00:00. This routine will work */
/* only between the years 1900 to 2040. The algorithm is from the book:     */
/* Jean Meeus, Astronomical Algorithms, p 59-66.                            */
/*--------------------------------------------------------------------------*/
void seconds2tm(unsigned long Secs, struct tm *tp) {

  long Seconds;
  long alfa,B,C,D,E;
  long DayNbr;
  
  Seconds = ((unsigned long) Secs) % ((unsigned long) 86400);
  DayNbr  = ((unsigned long) Secs) / ((unsigned long) 86400);

  tp->tm_hour = (int)(Seconds/3600);
  tp->tm_min  = (int)((Seconds % 3600)/60);
  tp->tm_sec  = (int)(Seconds % 60);

  DayNbr += 2415021;  /* 1900, Jan 1, 00:00 is Julian day 2415020.5 */
  alfa = (long)((DayNbr-1867216.25)/36524.25);
  B = DayNbr+alfa-alfa/4+1525;
  C = (long)((B-122.1)/365.25);
  D = (long)(365.25*C);
  E = (long)((B-D)/30.6001);
  tp->tm_mday = B-D-(long)(30.6001*E);
  tp->tm_mon  = (E<14) ? E-2 : E-14;
  tp->tm_year = (tp->tm_mon > 1) ? C-6616 : C-6615;
  
  tp->tm_yday = cal2jday(tp->tm_year, tp->tm_mon+1, tp->tm_mday) - 1 ;
  
  tp->tm_isdst = 0;

}




/***************************************************************************************************
 *
 * The subroutine to read MWS data
 *
 ***************************************************************************************************/

int rdr2tdr_metopSGA1_mws_TDR(char *file_RDR,char *file_OUT,char *file_NEDT) {

    hid_t dsetr_id;
    hid_t space_id;
    hid_t type_id;
    int storage_size;
    hssize_t points;
    int size_bytes;
    int status; 
    int index;
  
    ////////////////////////////////////////////////////////////////////////////////////////////
    //
    // MWS file ( contains meta data, lat/lon/zenith angle/time, etc )
    // 
    ////////////////////////////////////////////////////////////////////////////////////////////
    hid_t file_id    = H5Fopen(file_RDR, H5F_ACC_RDONLY,  H5P_DEFAULT);
    hid_t root_id    = H5Gopen1(file_id, "/");
    hid_t data_id    = H5Gopen1(root_id, "data");
    hid_t gran_id    = H5Gopen1(data_id, "navigation_data");
    

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Latitude      32-bit integer    
    ////////////////////////////////////////////////////////////////////////////////////////////
    dsetr_id = H5Dopen1(gran_id, "mws_lat");
   
    // get space id for antenna temperature
    space_id = H5Dget_space( dsetr_id );
    // data type id
    type_id = H5Dget_type( dsetr_id );
    // number of dimensions
    int ndims = H5Sget_simple_extent_ndims(space_id);
    // each dimension size
    hsize_t *dims = new hsize_t[ndims];
    hsize_t *maxdims = new hsize_t[ndims];
    status = H5Sget_simple_extent_dims(space_id, dims, maxdims );
    int NSCAN = dims[0];
    int NFOV  = dims[1];

    points = H5Sget_simple_extent_npoints(space_id);
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    int *out_Latitude = new int[storage_size/4];
    status = H5Dread(dsetr_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_Latitude);

    // row major array,
    // offset = row*NUMCOLS + column
    float lats[NSCAN][NFOV];
    float scale = 0.0001;
    for( int iscan=0; iscan<NSCAN; iscan++ ) {
      for(int ifov=0; ifov<NFOV; ifov++ ) {
   index = iscan * NFOV + ifov;
   lats[iscan][ifov] = out_Latitude[index] * scale;
      }
    }

    delete [] out_Latitude;
    out_Latitude = NULL;
    H5Dclose(dsetr_id);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Longitude     32-bit integer
    ////////////////////////////////////////////////////////////////////////////////////////////////
    dsetr_id = H5Dopen1(gran_id, "mws_lon");
    space_id = H5Dget_space( dsetr_id );
    points = H5Sget_simple_extent_npoints(space_id);
    type_id = H5Dget_type( dsetr_id );
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    int *out_Longitude = new int[storage_size/4];
    status = H5Dread(dsetr_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_Longitude);

    float lons[NSCAN][NFOV];
    for( int iscan=0; iscan<NSCAN; iscan++ ) {
      for(int ifov=0; ifov<NFOV; ifov++ ) {
   index = iscan * NFOV + ifov;
   lons[iscan][ifov] = out_Longitude[index] * scale;
      }
    }

    delete [] out_Longitude;
    out_Longitude = NULL;
    H5Dclose(dsetr_id);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //   ScanStartTime  64-bit float
    ////////////////////////////////////////////////////////////////////////////////////////////////
    dsetr_id = H5Dopen1(gran_id, "time_startscan_utc_earthview");
    space_id = H5Dget_space( dsetr_id );
    points = H5Sget_simple_extent_npoints(space_id);
    type_id = H5Dget_type( dsetr_id );
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    double    *out_ScanStartTime = new double[storage_size/8];
    status = H5Dread(dsetr_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_ScanStartTime);

    float     scanStartTime[NSCAN];
    for( int iscan=0; iscan<NSCAN; iscan++ )
   scanStartTime[iscan] = out_ScanStartTime[iscan] ;
    
    delete [] out_ScanStartTime;
    out_ScanStartTime = NULL;
    H5Dclose(dsetr_id);
   
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SensorAzimuthAngle     16-bit integer
    ////////////////////////////////////////////////////////////////////////////////////////////////
    dsetr_id = H5Dopen1(gran_id, "mws_satellite_azimuth_angle");
    space_id = H5Dget_space( dsetr_id );
    points = H5Sget_simple_extent_npoints(space_id);
    type_id = H5Dget_type( dsetr_id );
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    short *out_SensorAzimuthAngle = new short [storage_size/2];
    status = H5Dread(dsetr_id, H5T_NATIVE_SHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_SensorAzimuthAngle);

    float sensorAzimuthAngle[NSCAN][NFOV];
    scale = 0.01;
    for( int iscan=0; iscan<NSCAN; iscan++ ) {
      for(int ifov=0; ifov<NFOV; ifov++ ) {
        index = iscan * NFOV + ifov;
        sensorAzimuthAngle[iscan][ifov] = out_SensorAzimuthAngle[index] * scale;
      }
    }

    delete [] out_SensorAzimuthAngle;
    out_SensorAzimuthAngle = NULL;
    H5Dclose(dsetr_id);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SensorZenithAngle      16-bit integer
    ////////////////////////////////////////////////////////////////////////////////////////////////
    dsetr_id = H5Dopen1(gran_id, "mws_satellite_zenith_angle");
    points = H5Sget_simple_extent_npoints(space_id);
    type_id = H5Dget_type( dsetr_id );
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    short int *out_SensorZenithAngle = new short int[storage_size/2];
    status = H5Dread(dsetr_id, H5T_NATIVE_SHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_SensorZenithAngle);

    float sensorZenithAngle[NSCAN][NFOV];

    for( int iscan=0; iscan<NSCAN; iscan++ ) {
      for(int ifov=0; ifov<NFOV; ifov++ ) {
        index = iscan * NFOV + ifov;
        sensorZenithAngle[iscan][ifov] = out_SensorZenithAngle[index] * scale;
      }
    }

    delete [] out_SensorZenithAngle;
    out_SensorZenithAngle = NULL;
    H5Dclose(dsetr_id);
    

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SolarAzimuthAngle      16-bit integer
    ////////////////////////////////////////////////////////////////////////////////////////////////
    dsetr_id = H5Dopen1(gran_id, "mws_solar_azimuth_angle");
    points = H5Sget_simple_extent_npoints(space_id);
    type_id = H5Dget_type( dsetr_id );
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    short int *out_SolarAzimuthAngle = new short int[storage_size/2];
    status = H5Dread(dsetr_id, H5T_NATIVE_SHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_SolarAzimuthAngle);
    
    float solarAzimuthAngle[NSCAN][NFOV];

    for( int iscan=0; iscan<NSCAN; iscan++ ) {
      for(int ifov=0; ifov<NFOV; ifov++ ) {
        index = iscan * NFOV + ifov;
        solarAzimuthAngle[iscan][ifov] = out_SolarAzimuthAngle[index] * scale;
      }
    }

    delete [] out_SolarAzimuthAngle;
    out_SolarAzimuthAngle = NULL;
    H5Dclose(dsetr_id);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // SolarZenithAngle       16-bit integer
    ////////////////////////////////////////////////////////////////////////////////////////////////
    dsetr_id = H5Dopen1(gran_id, "mws_solar_zenith_angle");
    points = H5Sget_simple_extent_npoints(space_id);
    type_id = H5Dget_type( dsetr_id );
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    short int  *out_SolarZenithAngle = new short int[storage_size/2];
    status = H5Dread(dsetr_id, H5T_NATIVE_SHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_SolarZenithAngle);

    float solarZenithAngle[NSCAN][NFOV];

    for( int iscan=0; iscan<NSCAN; iscan++ ) {
      for(int ifov=0; ifov<NFOV; ifov++ ) {
        index = iscan * NFOV + ifov;
        solarZenithAngle[iscan][ifov] = out_SolarZenithAngle[index] * scale;
      }
    }

    delete [] out_SolarZenithAngle;
    out_SolarZenithAngle = NULL;
    H5Dclose(dsetr_id);


    ////////////////////////////////////////////////////////////////////////////////////////////////           
    //
    // Close navigation_data group
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////           

    H5Gclose( gran_id ) ;

   
    ////////////////////////////////////////////////////////////////////////////////////////////////           
    //
    // Open calibration_data group ( contains brightness temperature )
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////           

    ////////////////////////////////////////////////////////////////////////////////////////////////////           
    //   brightness temperature  32-bit integer, 
    ////////////////////////////////////////////////////////////////////////////////////////////////////           
    // Open and get the id for the antenna temperature
    gran_id = H5Gopen1(data_id, "calibration_data");
    dsetr_id = H5Dopen1(gran_id, "mws_toa_brightness_temperature");
    // get space id for antenna temperature
    space_id = H5Dget_space( dsetr_id );
    // data type id
    type_id = H5Dget_type( dsetr_id );
    
    // number of dimensions
    ndims = H5Sget_simple_extent_ndims(space_id);
    hsize_t *dims_tb = new hsize_t[ndims];
    hsize_t *maxdims_tb = new hsize_t[ndims];
    status = H5Sget_simple_extent_dims(space_id, dims_tb, maxdims_tb );
    
    int NCHAN = dims_tb[2]; 

    // get storage size
    points = H5Sget_simple_extent_npoints(space_id);
    type_id = H5Dget_type( dsetr_id );
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;

    // read antenna temperature: 16-bit unsigned integer
    int* out_AntennaTemperature = new int[storage_size/4];
    status = H5Dread(dsetr_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_AntennaTemperature);

    scale = 0.001;
    float temp[NSCAN][NFOV][NCHAN];
    for(int iscan=0; iscan<NSCAN; iscan++ ) {
    for(int ifov=0;  ifov<NFOV;   ifov++  ) {
    for(int ichan=0; ichan<NCHAN; ichan++ ) {
        temp[iscan][ifov][ichan] = -999.0;
    }
    }
    }

    int contms = 0;
    const int minAntTemp = 50000;
    const int maxAntTemp = 350000;

    for(int iscan=0; iscan<NSCAN; iscan++ ) {
    for(int ifov=0;  ifov<NFOV;   ifov++  ) {
    for(int ichan=0; ichan<NCHAN; ichan++ ) {
        index = iscan * NFOV * NCHAN + ifov * NCHAN + ichan;
        if (out_AntennaTemperature[index] < maxAntTemp && out_AntennaTemperature[index] > minAntTemp) {
          temp[iscan][ifov][ichan] = out_AntennaTemperature[index] * scale;
        }
        else {
          contms++;
        }
    }
    }
    }

    if (contms>0) {
      cout << "contms: " << contms/NCHAN/NFOV << file_RDR <<"\n";
    }

    delete [] out_AntennaTemperature;
    out_AntennaTemperature = NULL;
    H5Dclose(dsetr_id);


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //    NEdTWarm                32-bit  integer
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    dsetr_id = H5Dopen1(gran_id, "mws_nedt_warm");
    points = H5Sget_simple_extent_npoints(space_id);
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    int* out_NEdTWarm = new int[storage_size/4];
    status = H5Dread(dsetr_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_NEdTWarm);
    float NEdTWarm[NSCAN][NCHAN];

    scale = 0.001;
    for(int iscan=0; iscan<NSCAN; iscan++ ) {
      for(int ichan=0; ichan<NCHAN; ichan++ ) {
        index = iscan * NCHAN + ichan;
        NEdTWarm[iscan][ichan] = out_NEdTWarm[index] * scale ;
      }
    }

    delete [] out_NEdTWarm;
    out_NEdTWarm = NULL;
    H5Dclose(dsetr_id);

    H5Gclose(gran_id);

    ////////////////////////////////////////////////////////////////////////////////////////////////           
    //
    // Open quality_information group 
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////           

    ////////////////////////////////////////////////////////////////////////////////////////////////////           
    //   mws_navigation_status   32-bit  integer,    
    ////////////////////////////////////////////////////////////////////////////////////////////////////           
    gran_id = H5Gopen1(data_id, "quality_information");
    dsetr_id = H5Dopen1(gran_id, "mws_navigation_status");
    space_id = H5Dget_space( dsetr_id );
    points = H5Sget_simple_extent_npoints(space_id);
    type_id = H5Dget_type( dsetr_id );
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    int* out_QF19 = new int[storage_size/4];
    status = H5Dread(dsetr_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_QF19);
    int QF19[NSCAN];

    for(int iscan=0; iscan<NSCAN; iscan++ ) {
      QF19[iscan] = out_QF19[iscan];
    }

    delete [] out_QF19;
    out_QF19 = NULL;
    H5Dclose(dsetr_id);
    H5Gclose(gran_id);

    ////////////////////////////////////////////////////////////////////////////////////////////////           
    //
    // Open processing_flag group 
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////           

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //   QF1_ATMSSDRGEO 8-bit unsigned character,   
    ////////////////////////////////////////////////////////////////////////////////////////////////

    gran_id = H5Gopen1(data_id, "processing_flags");
    dsetr_id = H5Dopen1(gran_id, "mws_scanline_quality");
    space_id = H5Dget_space( dsetr_id );
    points = H5Sget_simple_extent_npoints(space_id);
    type_id = H5Dget_type( dsetr_id );
    size_bytes = H5Tget_size(type_id);
    storage_size = points*size_bytes;
    char *out_QF1_ATMSSDRGEO = new char[storage_size];
    status = H5Dread(dsetr_id, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, out_QF1_ATMSSDRGEO);

    unsigned short QF1_ATMSSDRGEO[NSCAN];
    for( int iscan=0; iscan<NSCAN; iscan++ )
   QF1_ATMSSDRGEO[iscan] = out_QF1_ATMSSDRGEO[iscan] ;
    
    delete [] out_QF1_ATMSSDRGEO;
    out_QF1_ATMSSDRGEO = NULL;
    H5Dclose(dsetr_id);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // free up resource to avoid memory leak
    // 
    ////////////////////////////////////////////////////////////////////////////////////////////////
    
    delete [] dims;
    delete [] maxdims;
    delete [] dims_tb;
    delete [] maxdims_tb;
    
    dims = NULL;
    maxdims = NULL;
    dims_tb = NULL;
    maxdims_tb = NULL;


    ////////////////////////////////////////////////////////////////////////////////////////////////           
    //
    // Close MWS  group and file
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////           

    H5Gclose( gran_id ) ;
    H5Gclose( data_id ) ;
    H5Gclose( root_id ) ;
    H5Fclose( file_id ) ;
    

    ////////////////////////////////////////////////////////////////////////////////////////////////           
    //
    // Output Section
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////           
    
    int nscan = NSCAN;
    MeasurementTypeHeader  header(nscan,NCHAN,NFOV,NQC);
    
    if( NCHAN == nchan_mws ) { 
   for(int ichan=0; ichan<NCHAN; ichan++) {
       header.freq[ichan]  = freq_metopSGA1[ichan];
       header.polar[ichan] = polar_metopSGA1[ichan];
   }
    }

    // open file 
    ofstream myFile(file_OUT, ios::out);
    if( !myFile ) {
      cerr << "ERROR: " << file_OUT << endl;
   exit(1);
    }
    
    // header part, so that can be read out by fortran subroutine ReadRadHdrScanLMode_ascii 
    writeMeasurementHdr(myFile, header);

    // Content part, can be read by fortran subroutine ReadRadMeasScanLMode_ascii

    int node=0;
    if ( lats[1][0]  >= lats[0][0] )   node = 0;
    else             node = 1;
    
    MeasurementType ms(NCHAN,NFOV,NQC);

    for(int iscan=0; iscan<nscan; iscan++) {
      
        // Default QC flags to zero
        for(int iqc=0; iqc<NQC; iqc++) {
     ms.qc[iqc]=0;
   }

   // Define the node (ascending/descending)
   if ( iscan >= 1 ) {
       if ( lats[iscan][0] > lats[iscan-1][0] )    node = 0;
       else                node = 1;
   }
      
   ms.node = node;
   
   // date/time information
   unsigned long secsAfter1958 = (unsigned long)( scanStartTime[iscan] + secs1958_2019 ); 
   //unsigned long seconds = secsBefore1958 + secsAfter1958 - LEAP_SECONDS;
   unsigned long seconds = 0L;
   if( secsAfter1958 < 1719792035 ) { // 1719792035 ---> 2012/07/01 00:00:00 
       seconds = secsBefore1958 + secsAfter1958 - 34;  
   }
   else if ( secsAfter1958 < 1814400036 ) { // 1814400036 ---> 2015/07/01 00:00:00 
       seconds = secsBefore1958 + secsAfter1958 - 35;  
   }
   else if ( secsAfter1958 < 1861920037 ) { // 1861920037 ---> 2017/01/01 00:00:00 
       seconds = secsBefore1958 + secsAfter1958 - 36;  
   }
   else {
       seconds = secsBefore1958 + secsAfter1958 - 37;  
   }
   // more branches are needed in the future when leap second gets adjusted again.
   
   struct tm time_str;
   struct tm *tp = &time_str;
   seconds2tm( seconds, tp );
   
   ms.year = tp->tm_year + 1900;
   ms.jday = tp->tm_yday + 1;
   ms.secs = ( tp->tm_hour * 3600 + tp->tm_min * 60 + tp->tm_sec ) * 1000 ; // time in milli-seconds since the start of day
   
   // lat/lon/angle/relAziAngle/solZenAngle
   for(int ifov=0; ifov<NFOV; ifov++) {
       ms.lat[ifov]         = lats[iscan][ifov];
       ms.lon[ifov]         = lons[iscan][ifov];
       ms.angle[ifov]       = sensorZenithAngle[iscan][ifov];
            if( ifov <= 47 && ms.angle[ifov] > 0 ) ms.angle[ifov] = -1.0 * ms.angle[ifov];  // 1-48 to negative values
       ms.relAziAngle[ifov] = sensorAzimuthAngle[iscan][ifov];
       ms.solZenAngle[ifov] = solarZenithAngle[iscan][ifov];
   }
   
   // tb
   for(int ifov=0;ifov<NFOV;ifov++) {
     for(int ichan=0;ichan<NCHAN;ichan++) {
       ms.tb[ifov][ichan] = temp[iscan][ifov][ichan];
     }
   }
// yklee begin
        if( QF19[iscan] != 0 ) ms.qc[0] = 1;
//
        if( QF1_ATMSSDRGEO[iscan] != 0 ) ms.qc[0] = 1;
// yklee end

   // write measurement content of this scan
   writeMeasurement( myFile, ms );    
   
    }

    myFile.close();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Output NEDT file
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
             
    //float NEdTWarm[NSCAN][NCHAN];
    float nedts[NCHAN];

    for(int ichan=0; ichan<NCHAN; ichan++) nedts[ichan]=0.0;

    for(int ichan=0; ichan<NCHAN; ichan++) {
      for(int iscan=0; iscan<NSCAN; iscan++) {
        if( NEdTWarm[iscan][ichan] > 0 )  nedts[ichan] += NEdTWarm[iscan][ichan] ;
      }
    }

    for(int ichan=0; ichan<NCHAN; ichan++) nedts[ichan] = nedts[ichan] / NSCAN ;

    ofstream nedtFile(file_NEDT, ios::out);

    nedtFile << "nChan                   =";
    nedtFile.width(6); nedtFile << NCHAN << '\n';

    nedtFile << "CentrFreq               =\n";
    for( int ichan=0; ichan<NCHAN; ichan++ )
    {
       nedtFile << setprecision(3) << setw(10) << right << fixed << header.freq[ichan] ;
       if ( (((ichan+1) % 10) == 0) && ((ichan+1) < NCHAN) ) nedtFile << endl;
    }
    nedtFile << endl;

    nedtFile << "RMSnoise                =\n";
    for( int ichan=0; ichan<NCHAN; ichan++ )
    {
       nedtFile << setprecision(3) << setw(10) << right << fixed << nedts[ichan] ;
       if ( (((ichan+1) % 10) == 0) && ((ichan+1) < NCHAN) ) nedtFile << endl;
    }
    nedtFile << endl;

    nedtFile << "NEDTnoise               =\n";
    for( int ichan=0; ichan<NCHAN; ichan++ )
    {
       nedtFile << setprecision(3) << setw(10) << right << fixed << nedts[ichan] ;
       if ( (((ichan+1) % 10) == 0) && ((ichan+1) < NCHAN) ) nedtFile << endl;
    }
    nedtFile << endl;

    nedtFile.close();
  
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// This is the main program to call different subroutine according to different data types
//
////////////////////////////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[] )
{
  char file_RDR[FILESIZE];
  char file_OUT[FILESIZE];
  char file_NEDT[FILESIZE];

  string files_rdr[FILENUM];
  string files_out[FILENUM];
  string files_nedt[FILENUM];

  char rdrFileList[FILESIZE];
  char pathTDR[FILESIZE];
  char pathNEDT[FILESIZE];
  char instrConfigFile[FILESIZE];
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  //
  // to get file list and control parameters from command line
  //
  ////////////////////////////////////////////////////////////////////////////////////////////

  cin.getline(rdrFileList,FILESIZE);
  cin.getline(pathTDR,FILESIZE);
  
  cin.getline(pathNEDT,FILESIZE);
  cin.getline(instrConfigFile,FILESIZE);
  
  
  char token[7] = "";
  strcpy(token,"MWS_");
   
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  //
  // fill in 3 file name arrays
  //
  ////////////////////////////////////////////////////////////////////////////////////////////
  ifstream inFile(rdrFileList, ios::in);
  if ( !inFile ) {
      cerr << "RDR file could not be opened: " << rdrFileList << endl;
      exit( 1 );
  }

  int nfile=0;
  char filename[FILESIZE] = "";
  
  string line;
  while ( inFile.getline(filename,FILESIZE) ) {
      files_rdr[nfile] = filename;
      string line2(filename);
      int pos = line2.find(token);
      int pos_end = line2.find(".nc");
      int length = pos_end - pos;
      files_out[nfile]  = string(pathTDR)  + "TDR_"  + line2.substr(pos,FILESIZE);
      files_nedt[nfile] = string(pathNEDT) + "NEDT_" + line2.substr(pos,length) + "_befFM.dat";
      cout << "file_nedt  =  " << files_nedt[nfile] << endl;
      nfile++;
  }

  inFile.close();
  
  
  cout << "nfile=" << nfile << endl;
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  //
  // start loop around files
  //
  ////////////////////////////////////////////////////////////////////////////////////////////
  
    for(int ifile=0; ifile<nfile; ifile++ ) {
      strcpy( file_RDR,files_rdr[ifile].c_str() );
      strcpy( file_OUT,files_out[ifile].c_str() );
      strcpy( file_NEDT,files_nedt[ifile].c_str() );
      rdr2tdr_metopSGA1_mws_TDR(file_RDR,file_OUT,file_NEDT);
    }

  return 0;
  
}
