/***************************************************************************
 *  Program Name      : sfr_intg.c
 *  Type              : subroutine
 *  Function          : integrate sfr using iwp, de and termial vel
 *  Subroutine Called : qromb()
 *  Called by         : snowfall.c
 **************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#define   DELTA0    8.0     // dimensionless coeff associated with 
                            //   the depth of boundary layer for computation 
                            //   of drag
#define   C0        0.35    // inviscid drag coefficient
#define   RhoW      1000.0  // Water density (kg/m**3) 
#define   RhoI      600.0   // Ice particle density (kg/m**3) 
#define   Rd        287.058 // Specific gas constant for dry air (J/kg.K)
#define   Rv        461.495 // Specific gas constant for water vapor (J/kg.K)
#define   T0ETA     524.07  // Reference temperature for standard air 
                            //   for computing dynamic viscosity (R) 
#define   Ceta1      120.0  // Coefficient for standard air for 
                            //   computing dynamic viscosity 
#define   ETA0      1.827E-5// Reference dynamic viscosity for 
                            //   standard air (Pa.S) 
#define   g         9.81    // Gravity m/s**2
#define   T0        273.15  // K 
#define   INT_LOW   5.0E-3  // Lower integration limit for D (mm)
#define   INT_UP    5.0     // Upper integration limit for D (mm) 


float qromb(float, float, float, float);


float sfr_intg(float iwp, float de, float ts, float rh2m, float pres0)
{
    float  coea, coeb, eta1;
    float  psat, pv, pd, rhoa;
    float  coe1, coe2, sfr;

    // Compute dynamic viscosity of the air using Sutherland's formula.
    // temperature in Rankine (R=K*9/5)
    coea = 0.555 * T0ETA + Ceta1;
    coeb = 0.555 * ts * 9./5. + Ceta1;
    eta1 = ETA0 * (coea/coeb) * pow(ts*9./(5*T0ETA),1.5);   // in Pa.s 

    // Compute air density (rho_a=P_dryair/(R_dryair*T) 
    // + P_watervapor/(R_watervapor*T)), P in Pa, R in J/(kg.K), T in K
    // (Pa). From Bolton (1980) (0.3% within -35C to 35C) 
    psat = 611.2 * exp(17.67*(ts-T0)/((ts-T0)+243.5));
    pv = rh2m * psat;
    pd = pres0 - pv;
    rhoa = pd/(Rd*ts) + pv/(Rv*ts);  // rhoa in kg/m**3

    // Compute integration coefficients
    coe1 = pow(DELTA0,2)*eta1*iwp / (24.*RhoW*rhoa*pow(de,4));
    coe2 = 8.0 * sqrt(RhoI*rhoa*g/(3.0*C0)) / (eta1*pow(DELTA0,2));  

    // Integration 
    sfr = coe1 * pow(10., -6) 
        * qromb(INT_LOW, INT_UP, de, coe2);

    return(sfr);
}



#define NR_END 0
#define FREE_ARG char*

#define EPS 1.0e-6
#define JMAX 20
#define JMAXP (JMAX+1)
#define K 5
#define NRANSI


// Numerical Recipes standard error handler
void nrerror(error_text)
char error_text[];
{
    void exit();

    fprintf(stderr,"Numerical Recipes run-time error...\n");
    fprintf(stderr,"%s\n",error_text);
    fprintf(stderr,"...now exiting to system...\n");
    exit(1);
}


// allocate a real vector with subscript range v[nl..nh]
// can replace with "new" in C++
//float *vector(long nl, long nh)
//{
//    float *v;
//    v = (float*) malloc((size_t) ((nh-nl+1+NR_END) * sizeof(float)));
//    if (!v)    nrerror("allocation failure in vector()");
//    return(v-nl+NR_END);
//}


// free a real vector allocated with vector()
// can replace with "delete" in C++
//void free_vector(float *v, long nl, long nh)
//{
//    free((FREE_ARG) (v+nl-NR_END));
//}


// *********************************************************************
float qromb(float a, float b, float de1, float coe)
{
    void polint(float xa[], float ya[], int n, float x, float *y, float *dy);
    float trapzd(float a, float b, float de1, float coe, int n);

    int     j;
    float   ss,dss;
    float   s[JMAXP],h[JMAXP+1];

    h[1] = 1.0;
    for (j=1; j<=JMAX; j++)  {
        s[j] = trapzd(a, b, de1, coe,j);
        if (j >= K)  {
            polint(&h[j-K], &s[j-K], K, 0.0, &ss, &dss);
            if (fabs(dss) <= EPS*fabs(ss)) 
                return ss;
        }
        h[j+1] = 0.25 * h[j];
    }

    // if run nrerror, it will exit code
    //nrerror("Too many steps in routine qromb");
    printf("Too many steps in routine qromb!\n");
    return(0.0);

}


// *********************************************************************
void polint(float xa[], float ya[], int n, float x, float *y, float *dy)
{
    int i, m, ns=1;
    float den, dif, dift, ho, hp, w;
    float *c, *d;
    long    nh, nl;

    dif = fabs(x-xa[1]);
    // remove function to avoid mem leakage warning
    //c = vector(1,n);
    //d = vector(1,n);
    nl = 1;  nh = n;
    c = (float*) malloc((size_t) ((nh-nl+1+NR_END) * sizeof(float)));
    if (!c)    nrerror("allocation failure");
    c = c - nl + NR_END;
    d = (float*) malloc((size_t) ((nh-nl+1+NR_END) * sizeof(float)));
    if (!d)    nrerror("allocation failure");
    d = d - nl + NR_END;

    for (i=1; i<=n; i++)  {
        if ((dift = fabs(x-xa[i])) < dif)  {
            ns = i;
            dif = dift;
        }
        c[i] = ya[i];
        d[i] = ya[i];
    }

    *y = ya[ns--];

    for (m=1; m<n; m++)  {
        for (i=1; i<=n-m; i++)  {
            ho = xa[i] - x;
            hp = xa[i+m] - x;
            w = c[i+1] - d[i];
            if ((den = ho - hp) == 0.0)
                nrerror("Error in routine polint");
            den = w/den;
            d[i] = hp*den;
            c[i] = ho*den;
        }
        *y += (*dy=(2*ns < (n-m) ? c[ns+1] : d[ns--]));
    }

    // remove free_vector function
    //free_vector(d,1,n);
    //free_vector(c,1,n);
    free(d+nl-NR_END);
    free(c+nl-NR_END);
}


// ***************************************************************
#undef NRANSI
#undef EPS
#undef JMAX
#undef JMAXP
#undef K


float trapzd(float a, float b, float de1, float coe, int n)
{
    float           x,tnm,sum,del;
    static float    s;
    int             it, j;

    if (n == 1)  {
        s = 0.5*(b-a) * (pow(a,2)*exp(-a/(de1*1000)) 
                * pow((pow((1+pow(10.,-4.5)*coe*pow(a,1.5)),0.5)-1),2)
                + pow(b,2)*exp(-b/(de1*1000)) 
                * pow((pow((1+pow(10.,-4.5)*coe*pow(b,1.5)),0.5)-1),2));
        return(s);
    }
    else  {
        for (it=1,j=1; j<n-1; j++)    it <<= 1;

        tnm = it;
        del = (b - a) / tnm;
        x = a + 0.5*del;

        for (sum=0.0, j=1; j<=it; j++, x+=del)
            sum += pow(x,2)*exp(-x/(de1*1000))
                * pow((pow((1+pow(10.,-4.5)*coe*pow(x,1.5)),0.5)-1),2);
        s=0.5 * (s+(b-a) * sum/tnm);
        return(s);
    }

}





