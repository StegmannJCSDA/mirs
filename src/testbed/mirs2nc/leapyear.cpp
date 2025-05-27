/**************************************************************************
 *  Program Name      : leapyear.cpp
 *  Type              : Subroutine
 *  Function          : Program determines if a given year is leap year 
 *  Input Files       : None
 *  Output Files      : None 
 *  Subroutine Called : None
 *  Called by         : jd2md.cpp 
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *   8/30/2000      v2.0
 *************************************************************************/


/*************************************************************/
#include "defaults.h"

int leapyear(short int iyear)
{
    short int    div4, div100, div400;

/*-----------------------------------------------------*
 *  Check if the given year is leap year.
 * ----------------------------------------------------*/
    div4   = iyear%4;
    div100 = iyear%100;
    div400 = iyear%400;

    if( (div4==0 && !div100==0) || div400==0)
      return(1);
    else
      return(0);

}  /* end of leapyear.c */ 
