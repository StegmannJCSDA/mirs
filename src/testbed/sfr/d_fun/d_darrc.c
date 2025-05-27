/***********************************************************************
 *  Program Name      : d_darrc.c
 *  Type              : Subroutine
 *  Function          : Dynamic allocate memory, for 1-4 dim
 *                      It can allocate array in one memory block
 *  Input Files       : None
 *  Output Files      : None
 *  Subroutine Called : init_{n}dc_{type}(), free_{n}dc_{type}()
 *  Called by         : This is a function lib
 *  Author            : Jun Dong, jim.dong@gmail.com, 03/19/2019
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>


/* ################################
 *   1d
 * ################################ */

/* ============================
 *   1.1 char
 * ============================ */

/* ------------------------
    1.1.1 char
   ------------------------ */
char *init_1dc_char(int n1, char **pp)
{
    char    *p=NULL, *p1;
    int     i1;

    p1 = (char*) malloc(n1 * sizeof(char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_char(char *p, char *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}


/* ------------------------
    1.1.2 signed char
   ------------------------ */
signed char *init_1dc_schar(int n1, signed char **pp)
{
    signed char     *p=NULL, *p1;
    int             i1;

    p1 = (signed char*) malloc(n1 * sizeof(signed char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_schar(signed char *p, signed char *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}


/* ------------------------
    1.1.3 unsigned char
   ------------------------ */
unsigned char *init_1dc_uchar(int n1, unsigned char **pp)
{
    unsigned char   *p=NULL, *p1;
    int             i1;

    p1 = (unsigned char*) malloc(n1 * sizeof(unsigned char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_uchar(unsigned char *p, unsigned char *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}


/* ============================
 *   1.2 int
 * ============================ */

/* ------------------------
    1.2.1 int
   ------------------------ */
int *init_1dc_int(int n1, int **pp)
{
    int     *p=NULL, *p1;
    int     i1;

    p1 = (int*) malloc(n1 * sizeof(int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_int(int *p, int *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}


/* ------------------------
    1.2.2 short int
   ------------------------ */
short int *init_1dc_sint(int n1, short int **pp)
{
    short int   *p=NULL, *p1;
    int         i1;

    p1 = (short int*) malloc(n1 * sizeof(short int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_sint(short int *p, short int *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}


/* ------------------------
    1.2.3 long int
   ------------------------ */
long int *init_1dc_lint(int n1, long int **pp)
{
    long int    *p=NULL, *p1;
    int            i1;

    p1 = (long int*) malloc(n1 * sizeof(long int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_lint(long int *p, long int *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}


/* ------------------------
    1.2.4 long long int
   ------------------------ */
long long int *init_1dc_llint(int n1, long long int **pp)
{
    long long int   *p=NULL, *p1;
    int             i1;

    p1 = (long long int*) malloc(n1 * sizeof(long long int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_llint(long long int *p, long long int *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}


/* ------------------------
    1.2.5 unsigned int
   ------------------------ */
unsigned int *init_1dc_uint(int n1, unsigned int **pp)
{
    unsigned int    *p=NULL, *p1;
    int             i1;

    p1 = (unsigned int*) malloc(n1 * sizeof(unsigned int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_uint(unsigned int *p, unsigned int *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}


/* ============================
 *   1.3 float
 * ============================ */

/* ------------------------
    1.3.1 float
   ------------------------ */
float *init_1dc_float(int n1, float **pp)
{
    float   *p=NULL, *p1;
    int     i1;

    p1 = (float*) malloc(n1 * sizeof(float));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_float(float *p, float *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}


/* ------------------------
    1.3.2 dboule
   ------------------------ */
double *init_1dc_double(int n1, double **pp)
{
    double  *p=NULL, *p1;
    int     i1;

    p1 = (double*) malloc(n1 * sizeof(double));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_double(double *p, double *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}


/* ------------------------
    1.3.3 long double
   ------------------------ */
long double *init_1dc_ldouble(int n1, long double **pp)
{
    long double     *p=NULL, *p1;
    int             i1;

    p1 = (long double*) malloc(n1 * sizeof(long double));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = p1;

    for (i1=0; i1<n1; i1++)    p[i1] = -99;

    *pp = p;
    return(p1);
}


int free_1dc_ldouble(long double *p, long double *p1)
{
    int     ret=0;

    free(p1);

    return(ret);
}



/* ################################
 *   2d
 * ################################ */

/* ============================
 *   2.1 char
 * ============================ */

/* ------------------------
    2.1.1 char
   ------------------------ */
char *init_2dc_char(int n1, int n2, char ***pp)
{
    char    **p=NULL, *p1;
    int     i1, i2;

    p1 = (char*) malloc(n1 * n2 * sizeof(char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (char**) malloc(n1 * sizeof(char*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_char(int n1, char **p, char *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    2.1.2 signed char
   ------------------------ */
signed char *init_2dc_schar(int n1, int n2, signed char ***pp)
{
    signed char     **p=NULL, *p1;
    int             i1, i2;

    p1 = (signed char*) malloc(n1 * n2 * sizeof(signed char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (signed char**) malloc(n1 * sizeof(signed char*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_schar(int n1, signed char **p, signed char *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    2.1.3 unsigned char
   ------------------------ */
unsigned char *init_2dc_uchar(int n1, int n2, unsigned char ***pp)
{
    unsigned char   **p=NULL, *p1;
    int             i1, i2;

    p1 = (unsigned char*) malloc(n1 * n2 * sizeof(unsigned char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (unsigned char**) malloc(n1 * sizeof(unsigned char*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_uchar(int n1, unsigned char **p, unsigned char *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}


/* ============================
 *   2.2 int
 * ============================ */

/* ------------------------
    2.2.1 int
   ------------------------ */
int *init_2dc_int(int n1, int n2, int ***pp)
{
    int     **p=NULL, *p1;
    int     i1, i2;

    p1 = (int*) malloc(n1 * n2 * sizeof(int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    
    p = (int**) malloc(n1 * sizeof(int*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_int(int n1, int **p, int *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    2.2.2 short int
   ------------------------ */
short int *init_2dc_sint(int n1, int n2, short int ***pp)
{
    short int   **p=NULL, *p1;
    int         i1, i2;

    p1 = (short int*) malloc(n1 * n2 * sizeof(short int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    
    p = (short int**) malloc(n1 * sizeof(short int*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_sint(int n1, short int **p, short int *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    2.2.3 long int
   ------------------------ */
long int *init_2dc_lint(int n1, int n2, long int ***pp)
{
    long int    **p=NULL, *p1;
    int         i1, i2;

    p1 = (long int*) malloc(n1 * n2 * sizeof(long int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    
    p = (long int**) malloc(n1 * sizeof(long int*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_lint(int n1, long int **p, long int *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    2.2.4 long long int
   ------------------------ */
long long int *init_2dc_llint(int n1, int n2, long long int ***pp)
{
    long long int   **p=NULL, *p1;
    int             i1, i2;

    p1 = (long long int*) malloc(n1 * n2 * sizeof(long long int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    
    p = (long long int**) malloc(n1 * sizeof(long long int*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_llint(int n1, long long int **p, long long int *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    2.2.5 unsigned int
   ------------------------ */
unsigned int *init_2dc_uint(int n1, int n2, unsigned int ***pp)
{
    unsigned int    **p=NULL, *p1;
    int             i1, i2;

    p1 = (unsigned int*) malloc(n1 * n2 * sizeof(unsigned int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    
    p = (unsigned int**) malloc(n1 * sizeof(unsigned int*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_uint(int n1, unsigned int **p, unsigned int *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}


/* ============================
 *   2.3 float
 * ============================ */

/* ------------------------
    2.3.1 float
   ------------------------ */
float *init_2dc_float(int n1, int n2, float ***pp)
{
    float   **p=NULL, *p1;
    int     i1, i2;

    p1 = (float*) malloc(n1 * n2 * sizeof(float));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    
    p = (float**) malloc(n1 * sizeof(float*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_float(int n1, float **p, float *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    2.3.2 double
   ------------------------ */
double *init_2dc_double(int n1, int n2, double ***pp)
{
    double  **p=NULL, *p1;
    int     i1, i2;

    p1 = (double*) malloc(n1 * n2 * sizeof(double));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    
    p = (double**) malloc(n1 * sizeof(double*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_double(int n1, double **p, double *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    2.3.3 long double
   ------------------------ */
long double *init_2dc_ldouble(int n1, int n2, long double ***pp)
{
    long double     **p=NULL, *p1;
    int             i1, i2;

    p1 = (long double*) malloc(n1 * n2 * sizeof(long double));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (long double**) malloc(n1 * sizeof(long double*));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = p1 + n2*i1;
        for (i2=0; i2<n2; i2++)    p[i1][i2] = -99;
    }

    *pp = p;
    return(p1);
}


int free_2dc_ldouble(int n1, long double **p, long double *p1)
{
    int     ret=0;

    free(p);
    free(p1);

    return(ret);
}



/* ################################
 *   3d
 * ################################ */

/* ============================
 *   3.1 char
 * ============================ */

/* ------------------------
    3.1.1 char
   ------------------------ */
char *init_3dc_char(int n1, int n2, int n3, char ****pp)
{
    char    ***p=NULL, *p1;
    int     i1, i2, i3;

    p1 = (char*) malloc(n1 * n2 * n3 * sizeof(char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (char***) malloc(n1 * sizeof(char**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (char**) malloc(n2 * sizeof(char*));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_char(int n1, int n2, char ***p, char *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    3.1.2 signed char
   ------------------------ */
signed char *init_3dc_schar(int n1, int n2, int n3, signed char ****pp)
{
    signed char     ***p=NULL, *p1;
    int             i1, i2, i3;

    p1 = (signed char*) malloc(n1 * n2 * n3 * sizeof(signed char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (signed char***) malloc(n1 * sizeof(signed char**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (signed char**) malloc(n2 * sizeof(signed char*));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_schar(int n1, int n2, signed char ***p, char *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    3.1.3 unsigned char
   ------------------------ */
unsigned char *init_3dc_uchar(int n1, int n2, int n3, unsigned char ****pp)
{
    unsigned char   ***p=NULL, *p1;
    int             i1, i2, i3;

    p1 = (unsigned char*) malloc(n1 * n2 * n3 * sizeof(unsigned char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (unsigned char***) malloc(n1 * sizeof(unsigned char**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (unsigned char**) malloc(n2 * sizeof(unsigned char*));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_uchar(int n1, int n2, unsigned char ***p, unsigned char *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ============================
 *   3.2 int
 * ============================ */

/* ------------------------
    3.2.1 int
   ------------------------ */
int *init_3dc_int(int n1, int n2, int n3, int ****pp)
{
    int     ***p=NULL, *p1;
    int     i1, i2, i3;

    p1 = (int*) malloc(n1 * n2 * n3 * sizeof(int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (int***) malloc(n1 * sizeof(int**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (int**) malloc(n2 * sizeof(int*));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_int(int n1, int n2, int ***p, int *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    3.2.2 short int
   ------------------------ */
short int *init_3dc_sint(int n1, int n2, int n3, short int ****pp)
{
    short int   ***p=NULL, *p1;
    int         i1, i2, i3;

    p1 = (short int*) malloc(n1 * n2 * n3 * sizeof(short int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (short int***) malloc(n1 * sizeof(short int**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (short int**) malloc(n2 * sizeof(short int*));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_sint(int n1, int n2, short int ***p, short int *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    3.2.3 long int
   ------------------------ */
long int *init_3dc_lint(int n1, int n2, int n3, long int ****pp)
{
    long int    ***p=NULL, *p1;
    int         i1, i2, i3;

    p1 = (long int*) malloc(n1 * n2 * n3 * sizeof(long int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (long int***) malloc(n1 * sizeof(long int**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (long int**) malloc(n2 * sizeof(long int*));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_lint(int n1, int n2, long int ***p, long int *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    3.2.4 long long int
   ------------------------ */
long long int *init_3dc_llint(int n1, int n2, int n3, long long int ****pp)
{
    long long int   ***p=NULL, *p1;
    int             i1, i2, i3;

    p1 = (long long int*) malloc(n1 * n2 * n3 * sizeof(long long int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (long long int***) malloc(n1 * sizeof(long long int**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (long long int**) malloc(n2 * sizeof(long long int*));
        for (i2=0; i2<n2; i2++)  {
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_llint(int n1, int n2, long long int ***p, long long int *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    3.2.5 unsigned int
   ------------------------ */
unsigned int *init_3dc_uint(int n1, int n2, int n3, unsigned int ****pp)
{
    unsigned int    ***p=NULL, *p1;
    int             i1, i2, i3;

    p1 = (unsigned int*) malloc(n1 * n2 * n3 * sizeof(unsigned int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (unsigned int***) malloc(n1 * sizeof(unsigned int**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (unsigned int**) malloc(n2 * sizeof(unsigned int*));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_uint(int n1, int n2, unsigned int ***p, unsigned int *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ============================
 *   3.3 float
 * ============================ */

/* ------------------------
    3.3.1 float
   ------------------------ */
float *init_3dc_float(int n1, int n2, int n3, float ****pp)
{
    float   ***p=NULL, *p1;
    int     i1, i2, i3;

    p1 = (float*) malloc(n1 * n2 * n3 * sizeof(float));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (float***) malloc(n1 * sizeof(float**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (float**) malloc(n2 * sizeof(float*));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_float(int n1, int n2, float ***p, float *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    3.3.2 double
   ------------------------ */
double *init_3dc_double(int n1, int n2, int n3, double ****pp)
{
    double  ***p=NULL, *p1;
    int     i1, i2, i3;

    p1 = (double*) malloc(n1 * n2 * n3 * sizeof(double));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (double***) malloc(n1 * sizeof(double**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (double**) malloc(n2 * sizeof(double*));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_double(int n1, int n2, double ***p, double *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    3.3.3 long double
   ------------------------ */
long double *init_3dc_ldouble(int n1, int n2, int n3, long double ****pp)
{
    long double     ***p=NULL, *p1;
    int             i1, i2, i3;

    p1 = (long double*) malloc(n1 * n2 * n3 * sizeof(long double));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (long double***) malloc(n1 * sizeof(long double**));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (long double**) malloc(n2 * sizeof(long double*));
        for (i2=0; i2<n2; i2++)  {
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
            p[i1][i2] = p1 + n3*i2 + n3*n2*i1;
            for (i3=0; i3<n3; i3++)    p[i1][i2][i3] = -99;
        }
    }

    *pp = p;
    return(p1);
}


int free_3dc_ldouble(int n1, int n2, long double ***p, long double *p1)
{
    int     i1, ret=0;

    for (i1=0; i1<n1; i1++)  {
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}



/* ################################
 *   4d
 * ################################ */

/* ============================
 *   4.1 char
 * ============================ */

/* ------------------------
    4.1.1 char
   ------------------------ */
char *init_4dc_char(int n1, int n2, int n3, int n4, char *****pp)
{
    char    ****p=NULL, *p1;
    int     i1, i2, i3, i4;

    p1 = (char*) malloc(n1 * n2 * n3 * n4 * sizeof(char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (char****) malloc(n1 * sizeof(char***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (char***) malloc(n2 * sizeof(char**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (char**) malloc(n3 * sizeof(char*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_char(int n1, int n2, int n3, char ****p, char *p1)
{
    int     i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    4.1.2 signed char
   ------------------------ */
signed char *init_4dc_schar(int n1, int n2, int n3, int n4, 
        signed char *****pp)
{
    signed char     ****p=NULL, *p1;
    int             i1, i2, i3, i4;

    p1 = (signed char*) malloc(n1 * sizeof(signed));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (signed char****) malloc(n1 * sizeof(signed***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (signed char***) malloc(n2 * sizeof(signed char**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (signed char**) malloc(n3 * sizeof(signed char*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_schar(int n1, int n2, int n3, signed char ****p, 
        signed char *p1)
{
    int     i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    4.1.3 unsigned char
   ------------------------ */
unsigned char *init_4dc_uchar(int n1, int n2, int n3, int n4, 
        unsigned char *****pp)
{
    unsigned char   ****p=NULL, *p1;
    int             i1, i2, i3, i4;

    p1 = (unsigned char*) malloc(n1 * n2 * n3 * n4 * sizeof(unsigned char));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (unsigned char****) malloc(n1 * sizeof(unsigned char***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (unsigned char***) malloc(n2 * sizeof(unsigned char**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (unsigned char**) malloc(n3 * sizeof(unsigned char*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_uchar(int n1, int n2, int n3, unsigned char ****p, 
        unsigned char *p1)
{
    int     i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ============================
 *   4.2 int
 * ============================ */

/* ------------------------
    4.2.1 int
   ------------------------ */
int *init_4dc_int(int n1, int n2, int n3, int n4, int *****pp)
{
    int     ****p=NULL, *p1;
    int     i1, i2, i3, i4;

    p1 = (int*) malloc(n1 * n2 * n3 * n4 * sizeof(int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (int****) malloc(n1 * sizeof(int***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (int***) malloc(n2 * sizeof(int**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (int**) malloc(n3 * sizeof(int*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_int(int n1, int n2, int n3, int ****p, int *p1)
{
    int     i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    4.2.2 short int
   ------------------------ */
short int *init_4dc_sint(int n1, int n2, int n3, int n4, short int *****pp)
{
    short int   ****p=NULL, *p1;
    int         i1, i2, i3, i4;

    p1 = (short int*) malloc(n1 * n2 * n3 * n4 * sizeof(short int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (short int****) malloc(n1 * sizeof(short int***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (short int***) malloc(n2 * sizeof(short int**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (short int**) malloc(n3 * sizeof(short int*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_sint(int n1, int n2, int n3, short int ****p, short int *p1)
{
    int     i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    4.2.3 long int
   ------------------------ */
long int *init_4dc_lint(int n1, int n2, int n3, int n4, long int *****pp)
{
    long int    ****p=NULL, *p1;
    int         i1, i2, i3, i4;

    p1 = (long int*) malloc(n1 * n2 * n3 * n4 * sizeof(long int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (long int****) malloc(n1 * sizeof(long int***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (long int***) malloc(n2 * sizeof(long int**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (long int**) malloc(n3 * sizeof(long int*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_lint(int n1, int n2, int n3, long int ****p, long int *p1)
{
    int     i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    4.2.4 long long int
   ------------------------ */
long long int *init_4dc_llint(int n1, int n2, int n3, int n4, 
        long long int *****pp)
{
    long long int   ****p=NULL, *p1;
    int             i1, i2, i3, i4;

    p1 = (long long int*) malloc(n1 * n2 * n3 * n4 * sizeof(long long int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (long long int****) malloc(n1 * sizeof(long long int***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (long long int***) malloc(n2 * sizeof(long long int**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (long long int**) malloc(n3 * sizeof(long long int*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_llint(int n1, int n2, int n3, long long int ****p, 
        long long int *p1)
{
    int     i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    4.2.5 unsigned int
   ------------------------ */
unsigned int *init_4dc_uint(int n1, int n2, int n3, int n4, 
        unsigned int *****pp)
{
    unsigned int    ****p=NULL, *p1;
    int             i1, i2, i3, i4;

    p1 = (unsigned int*) malloc(n1 * n2 * n3 * n4 * sizeof(unsigned int));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (unsigned int****) malloc(n1 * sizeof(unsigned int***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (unsigned int***) malloc(n2 * sizeof(unsigned int**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (unsigned int**) malloc(n3 * sizeof(unsigned int*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_uint(int n1, int n2, int n3, unsigned int ****p, 
        unsigned int *p1)
{
    int        i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ============================
 *   4.3 float
 * ============================ */

/* ------------------------
    4.3.1 float
   ------------------------ */
float *init_4dc_float(int n1, int n2, int n3, int n4, float *****pp)
{
    float   ****p=NULL, *p1;
    int     i1, i2, i3, i4;

    p1 = (float*) malloc(n1 * n2 * n3 * n4 * sizeof(float));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (float****) malloc(n1 * sizeof(float***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (float***) malloc(n2 * sizeof(float**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (float**) malloc(n3 * sizeof(float*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_float(int n1, int n2, int n3, float ****p, float *p1)
{
    int     i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ----- -------------------
    4.3.2 double
   ------------------------ */
double *init_4dc_double(int n1, int n2, int n3, int n4, double *****pp)
{
    double  ****p=NULL, *p1;
    int     i1, i2, i3, i4;

    p1 = (double*) malloc(n1 * n2 * n3 * n4 * sizeof(double));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (double****) malloc(n1 * sizeof(double***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (double***) malloc(n2 * sizeof(double**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (double**) malloc(n3 * sizeof(double*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_double(int n1, int n2, int n3, double ****p, double *p1)
{
    int     i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ------------------------
    4.3.3 long double
   ------------------------ */
long double *init_4dc_ldouble(int n1, int n2, int n3, int n4, 
        long double *****pp)
{
    long double     ****p=NULL, *p1;
    int             i1, i2, i3, i4;

    p1 = (long double*) malloc(n1 * n2 * n3 * n4 * sizeof(long double));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (long double****) malloc(n1 * sizeof(long double***));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (long double***) malloc(n2 * sizeof(long double**));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (long double**) malloc(n3 * sizeof(long double*));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = p1 + n4*i3 + n4*n3*i2 + n4*n3*n2*i1;
                for (i4=0; i4<n4; i4++)    p[i1][i2][i3][i4] = -99;
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_4dc_ldouble(int n1, int n2, int n3, long double ****p, 
        long double *p1)
{
    int     i1, i2, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}


/* ============================
 *   5.3 float
 * ============================ */

/* ------------------------
    5.3.1 float
   ------------------------ */
float *init_5dc_float(int n1, int n2, int n3, int n4, int n5, float ******pp)
{
    float   *****p=NULL, *p1;
    int     i1, i2, i3, i4, i5;

    p1 = (float*) malloc(n1 * n2 * n3 * n4 * n5 * sizeof(float));
    if (p1 == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }

    p = (float*****) malloc(n1 * sizeof(float****));
    if (p == NULL)  {
        printf("Memory not allocated.\n");
        exit(1);
    }
    for (i1=0; i1<n1; i1++)  {
        p[i1] = (float****) malloc(n2 * sizeof(float***));
        if (p[i1] == NULL)  {
            printf("Memory not allocated.\n");
            exit(1);
        }
        for (i2=0; i2<n2; i2++)  {
            p[i1][i2] = (float***) malloc(n3 * sizeof(float**));
            if (p[i1][i2] == NULL)  {
                printf("Memory not allocated.\n");
                exit(1);
            }
            for (i3=0; i3<n3; i3++)  {
                p[i1][i2][i3] = (float**) malloc(n4 * sizeof(float*));
                if (p[i1][i2][i3] == NULL)  {
                    printf("Memory not allocated.\n");
                    exit(1);
                }
                for (i4=0; i4<n4; i4++)  {
                    p[i1][i2][i3][i4] = p1 + n5*i4 + n5*n4*i3 
                        + n5*n4*n3*i2 + n5*n4*n3*n2*i1;
                    for (i5=0; i5<n5; i5++)    p[i1][i2][i3][i4][i5] = -99;
                }
            }
        }
    }

    *pp = p;
    return(p1);
}


int free_5dc_float(int n1, int n2, int n3, int n4, int n5, 
        float *****p, float *p1)
{
    int     i1, i2, i3, ret=0;

    for (i1=0; i1<n1; i1++)  {
        for (i2=0; i2<n2; i2++)  {
            for (i3=0; i3<n3; i3++)  {
                free(p[i1][i2][i3]);
            }
            free(p[i1][i2]);
        }
        free(p[i1]);
    }
    free(p);
    free(p1);

    return(ret);
}




