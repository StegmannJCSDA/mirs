/***********************************************************************
 *  Program Name      : d_jsec.c
 *  Type              : Subroutine
 *  Function          : Convert calendar date, julian second and tai second
 *                      the reference date is 1958/01/01 00:00:00
 *                      Once decide the leap sec, the data file need be updated
 *  Input Files       : None
 *  Output Files      : None
 *  Subroutine Called : cal2jday(), jday2cal(), doy2cal(), cal2doy(), doy2jday()
 *                      cal2tsec(), tsec2cal(), tsec2jsec()
 *  Called by         : This is a function lib
 *  Author            : Jun Dong, jim.dong@gmail.com, 03/19/2019
 ***********************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"


// Fliegel and van Flandern (1968)
int cal2jday(int year, int mon, int day, int *pjday)
{
    int     I, J, K;
    I=year;    J=mon;    K=day;

    // -2451545 changes ref to 2000/01/01 00:00:00
    //   +10957 changes ref to 1970/01/01 00:00:00
    //    +4383 changes ref to 1958/01/01 00:00:00
    *pjday=K-32075+1461*(I+4800+(J-14)/12)/4+367*(J-2-(J-14)/12*12) 
            /12-3*((I+4900+(J-14)/12)/100)/4-2451545+10957+4383;

    return(0);
}


int jday2cal(int jday, int *pyear, int *pmon, int *pday)
{
    int     L, N, I, J, K;

    // shift ref back
    L=jday+68569+2451545-10957-4383;
    N=4*L/146097;
    L=L-(146097*N+3)/4;
    I=4000*(L+1)/1461001;
    L=L-1461*I/4+31;
    J=80*L/2447;
    K=L-2447*J/80;
    L=J/11;
    J=J+2-12*L;
    I=100*(N-49)+I+L;
    *pyear=I;  *pmon=J;  *pday=K;

    return(0);
}


int cal2jsec(int year, int mon, int day, 
        int hour, int min, int sec, long long int *pjsec)
{
    int     jday;

    cal2jday(year, mon, day, &jday);
    *pjsec=jday*86400+hour*3600+min*60+sec;

    return(0);
}


int jsec2cal(long long int jsec, int *pyear, int *pmon, int *pday, 
        int *phour, int *pmin, int *psec)
{
    int     jday, rsec;

    jday=jsec/86400;  rsec=jsec%86400;
    if (jsec<0)  {
        jday=jday-1;  rsec=rsec+86400;
    }
    
    jday2cal(jday, pyear, pmon, pday);
    *phour=rsec/3600;  rsec=rsec%3600;
    *pmin=rsec/60;     *psec=rsec%60;

    return(0);
}


int doy2cal(int doy, int year, int *pmon, int *pday)
{
    int     jday;

    cal2jday(year, 1, 1, &jday);
    jday=jday+doy-1;
    jday2cal(jday, &year, pmon, pday);

    return(0);
}


int cal2doy(int year, int mon, int day, int *pdoy)
{
    int     jday;

    cal2jday(year, mon, day, pdoy);
    cal2jday(year, 1, 1, &jday);
    *pdoy = *pdoy - jday + 1;

    return(0);
}


int doy2jday(int doy, int year, int *pjday)
{
    int     mon, day;

    doy2cal(doy, year, &mon, &day);
    cal2jday(year, mon, day, pjday);

    return(0);
}


int cal2tsec(int year, int mon, int day, int hour, int min, int sec, 
        long long int *ptsec, int NYREF, int **LESE_TAB)
{
    int     iyr;
    int     YRREF;

    YRREF = LESE_TAB[0][0];

    cal2jsec(year, mon, day, hour, min, sec, ptsec);

    iyr = year - YRREF;
    if (iyr < 0)        iyr = 0;
    if (iyr > NYREF-1)    iyr = NYREF - 1;
    *ptsec = *ptsec + LESE_TAB[iyr][mon/7+1];

    return(0);
}


int tsec2cal(long long int tsec, int *pyear, int *pmon, int *pday, 
        int *phour, int *pmin, int *psec, int NYREF, int **LESE_TAB)
{
    int     iyr1, iyr;
    int     year1, mon1, day1, hour1, min1, sec1;
    int     YRREF;

    YRREF = LESE_TAB[0][0];

    jsec2cal(tsec, &year1, &mon1, &day1, &hour1, &min1, &sec1);

    iyr1 = year1 - YRREF;
    if (iyr1 < 0)            iyr1 = 0;
    if (iyr1 > NYREF-1)      iyr1 = NYREF - 1;
    tsec = tsec - LESE_TAB[iyr1][mon1/7+1];
    
    jsec2cal(tsec, pyear, pmon, pday, phour, pmin, psec);
    iyr = *pyear - YRREF;
    if (iyr < 0)            iyr = 0;
    if (iyr > NYREF-1)      iyr = NYREF - 1;
    if (*pmon != mon1)  {
        tsec = tsec + LESE_TAB[iyr1][mon1/7+1] - LESE_TAB[iyr][*pmon/7+1];
        jsec2cal(tsec, pyear, pmon, pday, phour, pmin, psec);
    }

    return(0);
}


int tsec2jsec(long long int tsec, long long *jsec, 
        int NYREF, int **LESE_TAB)
{
    int     year1, mon1, day1, hour1, min1, sec1;
    int     YRREF;

    YRREF = LESE_TAB[0][0];

    tsec2cal(tsec, &year1, &mon1, &day1, &hour1, &min1, &sec1, 
            NYREF, LESE_TAB);

    cal2jsec(year1, mon1, day1, hour1, min1, sec1, jsec);

    return(0);
}


#define SZ_BUF  64

int rd_lese_tab(char *ffn_lese, int *nyrefp, int ***lese_tabp)
{
    int     maxnum=0;
    size_t  newnum;
    int     **lese_tab, nyref;

    int     irt, i, j, len;
    char    ps[SZ_BUF], *cp;
    FILE    *fp;


    // open file
    fp = fopen(ffn_lese, "r");
    if (fp == NULL)  {
        printf("ERROR: Fail to open file: %s.\n", ffn_lese);
        exit(1);
    }
    //printf("open file: %s.\n", ffn_lese);

    // read file
    i = 0;
    lese_tab = (int**) malloc(1 * sizeof(*lese_tab));
    while (1)  {
        // read valid line
        cp = fgets(ps, SZ_BUF, fp);
        if (cp == NULL)  break;
        len = strlen(ps);  len--;  ps[len] = '\0';
        //printf("ps:|%s|%d\n", ps, len);
        if (len < 4)  continue;

        if (i == maxnum)  {
            newnum = maxnum + 1;
            lese_tab = (int**) realloc(lese_tab, newnum*sizeof(*lese_tab));
            if (lese_tab == NULL)  {
                //for (j=0; j<newnum; j++)    free(lese_tab[j]);
                //free(lese_tab);
                exit(1);
            }
            maxnum = newnum;
        }
        lese_tab[i] = (int*) malloc(3 * sizeof(int));
        if (lese_tab[i] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }

        // format data
        irt = sscanf(ps, "%d %d %d", 
                &lese_tab[i][0], &lese_tab[i][1], &lese_tab[i][2]);
        //printf("%4d, %02d, %02d\n", 
        //        lese_tab[i][0], lese_tab[i][1], lese_tab[i][2]);

        i++;
    }
    nyref = i;
    //printf("%d\n", nyref);

    *nyrefp = nyref;
    *lese_tabp = lese_tab;

    fclose(fp);
    
    return(0);
}




