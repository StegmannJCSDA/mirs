/***********************************************************************
 *  Program Name      : d_sys.c
 *  Type              : Subroutine
 *  Function          : Functions to open file, string convert, 
 *                      parse config file
 *  Input Files       : None
 *  Output Files      : None
 *  Subroutine Called : openfs(), get_one_line(), getEndian()
 *  Called by         : This is a function lib
 *  Author            : Jun Dong, jim.dong@gmail.com, 03/19/2019
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define     SL9    512


FILE *openfs(char *fname)
{
    FILE     *fp;

    fp = fopen(fname, "r");

    if (fp == NULL)  {
        printf("ERROR: Fail to open file (stop): %s\n", fname);
        exit(1);
    }
    printf("Successfully open file: %s\n", fname); 

    return(fp);
}



FILE *openfc(char *fname)
{
    FILE     *fp;

    fp = fopen(fname, "r");

    if (fp == NULL)  {
        printf("WARNNING: Fail to open file (cont): %s\n", fname);
    }
    else  {
        printf("Successfully open file: %s\n", fname);
    }

    return(fp);
}



int strrepa(char *str, char *org, char *rep)
{
    char    buf[SL9], *p, buf1[SL9];

    while (1)  {
        if (!(p = strstr(str, org)))
            return(0);
        strncpy(buf, p+strlen(org), SL9);
        *p = '\0';
        snprintf(buf1, SL9, "%s%s%s", str, rep, buf);
        strncpy(str, buf1, SL9);
    }
}




#include "sys/stat.h"

int mkdir_p(char *dir)
{
    int     i, ret, len;
    char    str[SL9], dir1[SL9];

    struct stat st = {0};

    // if exist, just return
    ret = stat(dir, &st);
    if (ret == 0)    return(0);

    // format dir
    len = strlen(dir);
    strncpy(dir1, dir, len+1);
    //printf("dir: %s %d\n", dir1, len);
    if (dir1[len-1] != '/')  {
        strcat(dir1, "/");
    }
    len = len + 1;
    //printf("dir1: %s %d\n", dir1, len);

    // find inner one, then create subdir
    for (i=0; i<len; i++)  {
        //printf("%d\n", i);
        if (dir1[i] == '/')  {
            strncpy(str, dir1, i);
            str[i] = '\0';
            ret = stat(str, &st);
            //printf("%s %d %d\n", str, strlen(str), ret);

            if (ret == 0)  {
                continue;
            }
            else  {
                ret = mkdir(str, 0755);
                //printf("create %s %d\n", str, ret);
                if (ret != 0)  {
                    printf("Cannot create dir: %s\n", str);
                    exit(1);
                }
            }
            
        }  // not find dir, next char
    }

    return(0);
}



// for mirs read
//return 1 if little endian machine,
//return 0 if big endian machine.
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


float deg2rad(float angle){
    float val;
    val = angle*3.14159/180.;
    return val;
}



int file_data_line(FILE *fp, int len, char *ps, int *nnum)
{
    int     i, sl, nsp, fsp;
    char    *cp;

    *nnum = -1;     *ps = '\0';

    cp = fgets(ps, len, fp);
    //printf("fgets: %s\n", ps);

    while (cp != NULL)  {

        // empty line
        if (ps[0] == 10)  {
            cp = fgets(ps, len, fp);
            //printf("fgets: %s\n", ps);
            continue;
        }

        // remove last "enter"
        sl = strlen(ps);
        ps[sl-1] = '\0';
        sl--;

        // remove leading space
        nsp = 0;
        for (i=0; i<sl; i++)  {
            if (ps[i] == ' ')    nsp++;
            else                  break;
        }
        //printf("sl, nsp: %d %d\n", sl, nsp);
        // all space
        if (nsp == sl)  {
            cp = fgets(ps, len, fp);
            //printf("fgets: %s\n", ps);
            continue;
        }
        // move char forward
        for (i=0; i<sl-nsp; i++)    ps[i] = ps[nsp+i];
        ps[sl-nsp] = '\0';
        sl = sl - nsp;
        //printf("%s\n", ps);

        // comment
        if (ps[0] == '%')  {
            cp = fgets(ps, len, fp);
            //printf("fgets: %s\n", ps);
            continue;
        }

        // exist data
        // remove tailing space
        nsp = 0;
        for (i=0; i<sl; i++)  {
            //printf("%c", ps[sl-1-i]);
            if (ps[sl-1-i] == ' ')  {
                //printf("s");
                nsp++;
            }
            else  {
                //printf("n");
                break;
            }
        }
        sl = sl - nsp;
        ps[sl] = '\0';

        // group space
        cp = ps;
        for (i=1; i<sl; i++)  {
            if (ps[i] != ' ')  {
                cp++;
                *cp = ps[i];
            }
            else  {
                if (*cp != ' ')  {
                    cp++;
                    *cp = ps[i];
                }
            }
        }
        *(cp+1) = '\0';
        sl = strlen(ps);

        // count space -> nnum
        nsp = 0;
        for (i=0; i<sl; i++)
            if (ps[i] == ' ')    nsp++;
        *nnum = nsp + 1;
        //printf("nnum=%d\n", *nnum);

        break;
    }

    if (cp == NULL)  {
        //printf("data file format wrong!\n");
        return(1);
    }

    return(0);
}


int get_one_line(FILE *fp, int len, char *ps, int *nnum)
{
    int     nw, nw1, ss, irt;
    char    ps1[len];

    ps[0] = '\0';
    ps1[0] = '\0';

    irt = file_data_line(fp, len, ps, &nw);
    if (nw < 0)  {
        *nnum = -1;
        return(1);
    }

    // if continue line, connect
    ss = strlen(ps);
    while (ps[ss-1] == '&')  {
        irt = file_data_line(fp, len, ps1, &nw1);
        if (nw1 > 0)  {
            ps[ss-1] = '\0';
            nw = nw - 1 + nw1;
            strncat(ps, ps1, strlen(ps1));
            ss = strlen(ps);
        }
        else  {
            printf("missing continue line");
            printf("%s\n", ps);
            exit(1);
        }
    }
    *nnum = nw;

    return(0);
}


int str2farr(char *ps, int n, float *pf)
{
    int     i, sl, p0, irt;

    p0 = -1;
    for (i=0; i<n; i++)  {
        p0++;
        irt = sscanf(&ps[p0], "%f", &pf[i]);
        while ((ps[p0] != ' '))    p0++;
        //printf("%.3f ", pf[i]);
    }
    //printf("\n");

    return(0);
}



int str2iarr(char *ps, int n, int *pf)
{
    int     i, sl, p0, irt;

    p0 = -1;
    for (i=0; i<n; i++)  {
        p0++;
        irt = sscanf(&ps[p0], "%d", &pf[i]);
        while ((ps[p0] != ' '))    p0++;
        //printf("%d ", pf[i]);
    }
    //printf("\n");

    return(0);
}




