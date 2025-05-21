/* ################################
 *   1d
 * ################################ */

/* ============================
 *   1.1 char
 * ============================ */

char            *init_1dc_char(int, char**);
int              free_1dc_char(char*, char*);

signed char     *init_1dc_schar(int, signed char**);
int              free_1dc_schar(signed char*, signed char*);

unsigned char   *init_1dc_uchar(int, unsigned char**);
int              free_1dc_uchar(unsigned char*, unsigned char*);

/* ============================
 *   1.2 int
 * ============================ */

int             *init_1dc_int(int, int**);
int              free_1dc_int(int*, int*);

short int       *init_1dc_sint(int, short int**);
int              free_1dc_sint(short int*, short int*);

long int        *init_1dc_lint(int, long int**);
int              free_1dc_lint(long int*, long int*);

long long int   *init_1dc_llint(int, long long int**);
int              free_1dc_llint(long long int*, long long int*);

unsigned int    *init_1dc_uint(int, unsigned int**);
int              free_1dc_uint(unsigned int*, unsigned int*);

/* ============================
 *   1.3 float
 * ============================ */

float           *init_1dc_float(int, float**);
int              free_1dc_float(float*, float*);

double          *init_1dc_double(int, double**);
int              free_1dc_double(double*, double*);

long double     *init_1dc_ldouble(int, long double**);
int              free_1dc_ldouble(long double*, long double*);


/* ################################
 *   2d
 * ################################ */

/* ============================
 *   2.1 char
 * ============================ */

char            *init_2dc_char(int, int, char***);
int              free_2dc_char(int, char**, char*);

signed char     *init_2dc_schar(int, int, signed char***);
int              free_2dc_schar(int, signed char**, signed char*);

unsigned char   *init_2dc_uchar(int, int, unsigned char***);
int              free_2dc_uchar(int, unsigned char**, unsigned char*);

/* ============================
 *   2.2 int
 * ============================ */

int             *init_2dc_int(int, int, int***);
int              free_2dc_int(int, int**, int*);

short int       *init_2dc_sint(int, int, short int***);
int              free_2dc_sint(int, short int**, short int*);

long int        *init_2dc_lint(int, int, long int***);
int              free_2dc_lint(int, long int**, long int*);

long long int   *init_2dc_llint(int, int, long long int***);
int              free_2dc_llint(int, long long int**, long long int*);

unsigned int    *init_2dc_uint(int, int, unsigned int***);
int              free_2dc_uint(int, unsigned int**, unsigned int*);

/* ============================
 *   2.3 float
 * ============================ */

float           *init_2dc_float(int, int, float***);
int              free_2dc_float(int, float**, float*);

double          *init_2dc_double(int, int, double***);
int              free_2dc_double(int, double**, double*);

long double     *init_2dc_ldouble(int, int, long double***);
int              free_2dc_ldouble(int, long double**, long double*);


/* ################################
 *   3d
 * ################################ */

/* ============================
 *   3.1 char
 * ============================ */

char            *init_3dc_char(int, int, int, char****);
int              free_3dc_char(int, int, char***, char*);

signed char     *init_3dc_schar(int, int, int, signed char****);
int              free_3dc_schar(int, int, signed char***, char*);

unsigned char   *init_3dc_uchar(int, int, int, unsigned char****);
int              free_3dc_uchar(int, int, unsigned char***, unsigned char*);

/* ============================
 *   3.2 int
 * ============================ */

int             *init_3dc_int(int, int, int, int****);
int              free_3dc_int(int, int, int***, int*);

short int       *init_3dc_sint(int, int, int, short int****);
int              free_3dc_sint(int, int, short int***, short int*);

long int        *init_3dc_lint(int, int, int, long int****);
int              free_3dc_lint(int, int, long int***, long int*);

long long int   *init_3dc_llint(int, int, int, long long int****);
int              free_3dc_llint(int, int, long long int***, long long int*);

unsigned int    *init_3dc_uint(int, int, int, unsigned int****);
int              free_3dc_uint(int, int, unsigned int***, unsigned int*);

/* ============================
 *   3.3 float
 * ============================ */

float           *init_3dc_float(int, int, int, float****);
int              free_3dc_float(int, int, float***, float*);

double          *init_3dc_double(int, int, int, double****);
int              free_3dc_double(int, int, double***, double*);

long double     *init_3dc_ldouble(int, int, int, long double****);
int              free_3dc_ldouble(int, int, long double***, long double*);


/* ################################
 *   4d
 * ################################ */

/* ============================
 *   4.1 char
 * ============================ */

char            *init_4dc_char(int, int, int, int, char*****);
int              free_4dc_char(int, int, int, char****, char*);

signed char     *init_4dc_schar(int, int, int, int, signed char*****);
int              free_4dc_schar(int, int, int, signed char****, 
                        signed char*);

unsigned char   *init_4dc_uchar(int, int, int, int, unsigned char*****);
int              free_4dc_uchar(int, int, int, unsigned char****, 
                        unsigned char*);

/* ============================
 *   4.2 int
 * ============================ */

int             *init_4dc_int(int, int, int, int, int*****);
int              free_4dc_int(int, int, int, int****, int*);

short int       *init_4dc_sint(int, int, int, int, short int*****);
int              free_4dc_sint(int, int, int, short int****, short int*);

long int        *init_4dc_lint(int, int, int, int, long int*****);
int              free_4dc_lint(int, int, int, long int****, long int*);

long long int   *init_4dc_llint(int, int, int, int, long long int*****);
int              free_4dc_llint(int, int, int, long long int****, 
                        long long int*);

unsigned int    *init_4dc_uint(int, int, int, int, unsigned int*****);
int              free_4dc_uint(int, int, int, unsigned int****, 
                        unsigned int*);


/* ============================
 *   4.3 float
 * ============================ */

float           *init_4dc_float(int, int, int, int, float*****);
int              free_4dc_float(int, int, int, float****, float*);

double          *init_4dc_double(int, int, int, int, double*****);
int              free_4dc_double(int, int, int, double****, double*);

long double     *init_4dc_ldouble(int, int, int, int, long double*****);
int              free_4dc_ldouble(int, int, int, long double****, 
                        long double*);


/* ============================
 *   5.3 float
 * ============================ */

float           *init_5dc_float(int, int, int, int, int, float ******);
int             free_5dc_float(int, int, int, int, int, float *****p, float *);




