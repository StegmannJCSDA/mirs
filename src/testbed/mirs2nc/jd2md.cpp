/**************************************************************************
 *  Program Name      : jd2md.cpp
 *  Type              : Subroutine
 *  Function          : Program converts julian day to day of month 
 *  Input Files       : None
 *  Output Files      : None 
 *  Subroutine Called : leapyear.cpp
 *  Called by         : rmirs_edr.cpp
 *
 *  Modification History:
 *      Date       Version         Description of Changes
 *     ------     ---------       ------------------------
 *   8/30/2000      v2.0
 *************************************************************************/
#include "defaults.h"

void jd2md(short int jday, short int year, short int *dom, short int *mon)
{
    short int   i, md;
    short int   listmon[]={31,28,31,30,31,30,31,31,30,31,30,31};
    short int   is_leap;

/*------------------------------------------------------*
 *  Check if the year is leap year
 *------------------------------------------------------*/

    is_leap = leapyear(year);

    if( is_leap )
        listmon[1] = 29;

/* -----------------------------------------------------*
 *  Convert the julian day to day of month
 * -----------------------------------------------------*/
    i = 0;
    do
    {
       md = jday;
       jday = md - listmon[i];
       i++;
    }while(jday > 0);

    *mon = i;
    *dom = md;

}  /* end of jd2md.c */ 
