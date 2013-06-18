// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "taMath_double.h"
#include <double_Array>
#include <int_Array>
#include <double_Matrix>
#include <int_Matrix>
#include <complex_Matrix>
#include <MTRnd>
#include <Random>
#include <Aggregate>
#include <Relation>
#include <SimpleMathSpec>
#include <taBaseItr>

#include <taMisc>

#ifdef _WINDOWS //note: have to use the raw ms switch, since our own file isn't loaded
// note: MSVC requires the following on first read of math.h to enable defines
// we have to put this absolutely first to insure it is defined for the first read
// of math.h, which could occur anywhere in any included file
# define _USE_MATH_DEFINES
#endif

#ifdef HAVE_LIBGSL
# include <gsl/gsl_blas.h>
# include <gsl/gsl_errno.h>
# include <gsl/gsl_eigen.h>
# include <gsl/gsl_linalg.h>
# include <gsl/gsl_fit.h>
# include <gsl/gsl_fft_real.h>
# include <gsl/gsl_fft_complex.h>
# include <gsl/gsl_fft_halfcomplex.h>
# include <gsl/gsl_multifit.h>
# include <gsl/gsl_sf.h>
# include <gsl/gsl_cdf.h>
#endif

#include <math.h>
#include <float.h>

using namespace std;

#ifdef _WINDOWS
//# include <stdlib.h>
//# include "../../../3rdparty/misc/s_erf.c"
 // extern double erf(double x); // in s_erf.c
 // extern double erfc(double x); // in s_erf.c

double erfc(double x) {
  double t,z,ans;

  z=fabs(x);
  t=1.0/(1.0+0.5*z);
  ans=t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
        t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
        t*(-0.82215223+t*0.17087277)))))))));
  return  x >= 0.0 ? ans : 2.0-ans;
}

double erf(double x) {
  return 1.0 - erfc(x);
}


#endif


/////////////////////////////////////////////////////////////////////////////////
// ExpLog: exponential and logarithmic functions

double taMath_double::e = M_E;

typedef union {
  double d;
  struct {
#ifdef TA_LITTLE_ENDIAN
    int j, i;
#else
    int i, j;
#endif
  } n;
} _eco;

#define EXP_A (1048576/M_LN2)
#define EXP_C 60801

double taMath_double::exp_fast(double x) {
  _eco tmp;
  tmp.n.j = 0;
  tmp.n.i = (int)(EXP_A*x + (1072693248 - EXP_C));
  return tmp.d;
}

/////////////////////////////////////////////////////////////////////////////////
// Trigonometry

double taMath_double::pi = M_PI;
double taMath_double::two_pi = 2.0 * M_PI;
double taMath_double::oneo_two_pi = 1.0 / (2.0 * M_PI);
double taMath_double::deg_per_rad = 180.0 / M_PI;
double taMath_double::rad_per_deg = M_PI / 180.0;

/////////////////////////////////////////////////////////////////////////////////
// Probability distributions, etc

double taMath_double::fact_ln(int n) {
  static double_Array table;

  if(n < 0) { fprintf(stderr, "Negative factorial fact_ln()\n"); return 0; }
  if(n <= 1) return 0.0;
  if(n < table.size) return table[n] ? table[n] : (table[n] = gamma_ln(n + 1.0));

  table.Alloc(n+1);             // allocate new size
  int i=table.size;
  for(; i<=n; i++)
    table.FastEl(i) = 0.0;
  table.size = n+1;

  return (table[n] = gamma_ln(n + 1.0));
}

double taMath_double::fact(int n) {
  return gsl_sf_fact(n);
}

double taMath_double::bico_ln(int n, int j) {
  return fact_ln(n)-fact_ln(j)-fact_ln(n-j);
}

double taMath_double::hyperg(int j, int s, int t, int n) {
  if(t > n) { fprintf(stderr, "t > n in hyperg()\n"); return 0; }
  if(s > n) { fprintf(stderr, "s > n in hyperg()\n"); return 0; }
  if(j > t) return 0.0;
  if(j > s) return 0.0;
  if(s - j > n - t) return 0.0;

  return exp(bico_ln(t, j) + bico_ln(n-t, s-j) - bico_ln(n, s));
}

/**********************************
  gamma function
***********************************/

static double gser(double a, double x) {
  int n;
  double gln,sum,del,ap;

  gln=taMath_double::gamma_ln(a);
  if (x <= 0.0) {
    if (x < 0.0) { fprintf(stderr, "x < 0 in gser()\n"); return 0; }
    return 0;
  }
  else {
    ap=a;
    del=sum=1.0/a;
    for (n=1;n<=taMath_double::max_iterations;n++) {
      ap += 1.0;
      del *= x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*taMath_double::err_tolerance)
        return sum*exp(-x+a*log(x)-(gln));
    }
    fprintf(stderr, "a too large, max_iterations too small in gser()\n");
    return 0;
  }
}

static double gcf(double a, double x) {
  int n;
  double gln;
  double gold=0.0,g,fac=1.0,b1=1.0;
  double b0=0.0,anf,ana,an,a1,a0=1.0;

  gln=taMath_double::gamma_ln(a);
  a1=x;
  for (n=1;n<=taMath_double::max_iterations;n++) {
    an=(double) n;
    ana=an-a;
    a0=(a1+a0*ana)*fac;
    b0=(b1+b0*ana)*fac;
    anf=an*fac;
    a1=x*a0+anf*a1;
    b1=x*b0+anf*b1;
    if (a1) {
      fac=1.0/a1;
      g=b1*fac;
      if (fabs((g-gold)/g) < taMath_double::err_tolerance)
        return exp(-x+a*log(x)-(gln))*g;
      gold=g;
    }
  }
  fprintf(stderr, "a too large, max_iterations too small in gcf()\n");
  return 0;
}

double taMath_double::gamma_ln(double z) {
  double x,tmp,ser;             /* make sure double-precision.. */
  static double cof[6]={ 76.18009173, -86.50532033, 24.01409822,
                         -1.231739516, 0.120858003e-2, -0.536382e-5 };
  int j;

  x=z-1.0;
  tmp=x+5.5;
  tmp -= (x+0.5)*log(tmp);
  ser=1.0;
  for (j=0;j<=5;j++) {
    x += 1.0;
    ser += cof[j]/x;
  }
  return -tmp+log(2.50662827465*ser);
}

double taMath_double::gamma_p(double a, double x) {
  if (x < 0.0 || a <= 0.0) { fprintf(stderr, "Invalid args in gamma_p()\n"); return 0; }

  if (x < (a+1.0))
    return gser(a,x);
  else
    return 1.0 - gcf(a,x);
}

double taMath_double::gamma_q(double a, double x) {
  if (x < 0.0 || a <= 0.0) { fprintf(stderr, "Invalid args in gamma_q()\n"); return 0; }
  if (x < (a+1.0))
    return 1.0 - gser(a,x);
  else
    return gcf(a,x);
}

/**********************************
  beta function
***********************************/

static double betacf_double(double a, double b, double x) {
  double qap,qam,qab,em,tem,d;
  double bz,bm=1.0,bp,bpp;
  double az=1.0,am=1.0,ap,app,aold;
  int m;

  qab=a+b;
  qap=a+1.0;
  qam=a-1.0;
  bz=1.0-qab*x/qap;
  for (m=1;m<=taMath_double::max_iterations;m++) {
    em=(double) m;
    tem=em+em;
    d=em*(b-em)*x/((qam+tem)*(a+tem));
    ap=az+d*am;
    bp=bz+d*bm;
    d = -(a+em)*(qab+em)*x/((qap+tem)*(a+tem));
    app=ap+d*az;
    bpp=bp+d*bz;
    aold=az;
    am=ap/bpp;
    bm=bp/bpp;
    az=app/bpp;
    bz=1.0;
    if (fabs(az-aold) < (taMath_double::err_tolerance*fabs(az))) return az;
  }
  fprintf(stderr, "a or b too big, or max_iterations too small in betacf()\n");
  return 0;
}

double taMath_double::beta(double z, double w) {
  return exp(gamma_ln(z) + gamma_ln(w) - gamma_ln(z + w));
}

double taMath_double::beta_i(double a, double b, double x) {
  double bt;

  if (x < 0.0 || x > 1.0) { fprintf(stderr, "Bad x in beta_i()\n"); return 0; }
  if (x == 0.0 || x == 1.0) bt=0.0;
  else
    bt=exp(gamma_ln(a+b)-gamma_ln(a)-gamma_ln(b)+a*log(x)+b*log(1.0-x));
  if (x < (a+1.0)/(a+b+2.0))
    return bt*betacf_double(a,b,x)/a;
  else
    return 1.0-bt*betacf_double(b,a,1.0-x)/b;
}


/**********************************
  the binomial distribution
***********************************/

double taMath_double::binom_den(int n, int j, double p) {
  if(j > n) { fprintf(stderr, "j > n in binom()\n"); return 0; }
  return exp(bico_ln(n,j) + (double)j * log(p) + (double)(n-j) * log(1.0 - p));
}

double taMath_double::binom_cum(int n, int k, double p) {
  if(k > n)     { fprintf(stderr, "k > n in binom_cum()\n"); return 0; }
  return beta_i(k, n-k + 1, p);
}

double taMath_double::binom_dev(int n, double pp) {
  int j;
  static int nold=(-1);
  double am,em,g,angle,p,bnl,sq,t,y;
  static double pold=(-1.0),pc,plog,pclog,en,oldg;

  p=(pp <= 0.5 ? pp : 1.0-pp);
  am=n*p;
  if (n < 25) {
    bnl=0.0;
    for (j=1;j<=n;j++)
      if (MTRnd::genrand_res53() < p) bnl += 1.0;
  }
  else if (am < 1.0) {
    g=exp(-am);
    t=1.0;
    for (j=0;j<=n;j++) {
      t *= MTRnd::genrand_res53();
      if (t < g) break;
    }
    bnl=(j <= n ? j : n);
  }
  else {
    if (n != nold) {
      en=n;
      oldg=gamma_ln(en+1.0);
      nold=n;
    }
    if (p != pold) {
      pc=1.0-p;
      plog=log(p);
      pclog=log(pc);
      pold=p;
    }
    sq=sqrt(2.0*am*pc);
    do {
      do {
        angle=pi*MTRnd::genrand_res53();
        y=tan(angle);
        em=sq*y+am;
      } while (em < 0.0 || em >= (en+1.0));
      em=floor(em);
      t=1.2*sq*(1.0+y*y)*exp(oldg-gamma_ln(em+1.0)
                             -gamma_ln(en-em+1.0)+em*plog+(en-em)*pclog);
    } while (MTRnd::genrand_res53() > t);
    bnl=em;
  }
  if (p != pp) bnl=n-bnl;
  return bnl;
}


/**********************************
  the poisson distribution
***********************************/

double taMath_double::poisson_den(int j, double l) {
  return exp((double)j * log(l) - fact_ln(j) - l);
}

double taMath_double::poisson_cum(int j, double x) {
  if(x < 0.0)   { fprintf(stderr, "x < 0 in poisson_cum()\n"); return 0; }
  if(j > 0)
    return gamma_q(j, x);
  else
    return 0;
}

double taMath_double::poisson_dev(double xm) {
  static double sq,alxm,g,oldm=(-1.0);
  double em,t,y;

  if (xm < 12.0) {
    if (xm != oldm) {
      oldm=xm;
      g=exp(-xm);
    }
    em = -1;
    t=1.0;
    do {
      em += 1.0;
      t *= MTRnd::genrand_res53();
    } while (t > g);
  }
  else {
    if (xm != oldm) {
      oldm=xm;
      sq=sqrt(2.0*xm);
      alxm=log(xm);
      g=xm*alxm-gamma_ln(xm+1.0);
    }
    do {
      do {
        y=tan(pi*MTRnd::genrand_res53());
        em=sq*y+xm;
      } while (em < 0.0);
      em=floor(em);
      t=0.9*(1.0+y*y)*exp(em*alxm-gamma_ln(em+1.0)-g);
    } while (MTRnd::genrand_res53() > t);
  }
  return em;
}


/**********************************
  the gamma distribution
***********************************/

double taMath_double::gamma_den(int j, double l, double t) {
  if(t < 0) return 0;
  return exp((double)j * log(l) + (double)(j-1) * log(t) - gamma_ln(j) - (l * t));
}

double taMath_double::gamma_cum(int j, double l, double t) {
  return gamma_p(j, l * t);
}

double taMath_double::gamma_dev(int ia) {
  int j;
  double am,e,s,v1,v2,x,y;

  if (ia < 1) { fprintf(stderr, "ia < 1 in gamma_dev()\n"); return 0; }
  if (ia < 6) {
    x=1.0;
    for (j=1;j<=ia;j++) x *= MTRnd::genrand_res53();
    x = -log(x);
  }
  else {
    do {
      do {
        do {
          v1=2.0*MTRnd::genrand_res53()-1.0;
          v2=2.0*MTRnd::genrand_res53()-1.0;
        } while (v1*v1+v2*v2 > 1.0);
        y=v2/v1;
        am=ia-1;
        s=sqrt(2.0*am+1.0);
        x=s*y+am;
      } while (x <= 0.0);
      e=(1.0+y*y)*exp(am*log(x/am)-s*y);
    } while (MTRnd::genrand_res53() > e);
  }
  return x;
}



/**********************************
  the normal distribution (& error fun)
***********************************/

//note: put into .cpp to deal with win
double taMath_double::erf(double x) {
  return ::erf(x);
}

double taMath_double::erfc(double x) {
  return ::erfc(x);
}

double taMath_double::gauss_den(double z) {
  return 0.398942280 * exp(-0.5 * z * z);
}

double taMath_double::gauss_den_sig(double x, double sig) {
  x /= sig;
  return 0.398942280 * exp(-0.5 * x * x) / sig;
}

double taMath_double::gauss_den_sq_sig(double x_sq, double sig) {
  return 0.398942280 * exp(-0.5 * x_sq / (sig * sig)) / sig;
}

double taMath_double::gauss_den_nonorm(double x, double sig) {
  x /= sig;
  return exp(-0.5 * x * x);
}

double taMath_double::gauss_cum(double z) {
  double y, x, w;

  if (z == 0.0)
    x = 0.0;
  else {
    y = 0.5 * fabs (z);
    if (y >= 3.0)
      x = 1.0;
    else if (y < 1.0) {
      w = y*y;
      x = ((((((((0.000124818987 * w
                  -0.001075204047) * w +0.005198775019) * w
                -0.019198292004) * w +0.059054035642) * w
              -0.151968751364) * w +0.319152932694) * w
            -0.531923007300) * w +0.797884560593) * y * 2.0;
    }
    else {
      y -= 2.0;
      x = (((((((((((((-0.000045255659 * y
                       +0.000152529290) * y -0.000019538132) * y
                     -0.000676904986) * y +0.001390604284) * y
                   -0.000794620820) * y -0.002034254874) * y
                 +0.006549791214) * y -0.010557625006) * y
               +0.011630447319) * y -0.009279453341) * y
             +0.005353579108) * y -0.002141268741) * y
           +0.000535310849) * y +0.999936657524;
    }
  }
  return (z > 0.0 ? ((x + 1.0) / 2.0) : ((1.0 - x) / 2.0));
}

double taMath_double::gauss_inv(double p) {
  double        minz = -6.0;
  double        maxz = 6.0;
  double        zval = 0.0;
  double        pval;

  if (p <= 0.0 || p >= 1.0 || p == .5)
    return (0.0);

  while (maxz - minz > 0.000001) {
    pval = gauss_cum(zval);
    if (pval > p)
      maxz = zval;
    else
      minz = zval;
    zval = (maxz + minz) * 0.5;
  }
  return (zval);
}

double taMath_double::gauss_inv_lim(double p) {
  double        minz = -6.0;
  double        maxz = 6.0;
  double        zval = 0.0;
  double        pval;

  if (p < 0.0 || p > 1.0 || p == .5)
    return (0.0);
  if(p==0.0)
    return minz;
  if(p==1.0)
    return maxz;

  while (maxz - minz > 0.000001) {
    pval = gauss_cum(zval);
    if (pval > p)
      maxz = zval;
    else
      minz = zval;
    zval = (maxz + minz) * 0.5;
  }
  return (zval);
}


double taMath_double::gauss_dev() {
  return MTRnd::genrand_gauss_dev_double();
}

/**********************************
  misc statistical
***********************************/

double taMath_double::dprime(double mean_signal, double stdev_signal,
			     double mean_noise, double stdev_noise) {
  double avg_stdev = sqrt(0.5 * stdev_signal * stdev_signal + stdev_noise * stdev_noise);
  if(avg_stdev == 0.0) return 0.0;
  return (mean_signal - mean_noise) / avg_stdev;
}

double taMath_double::hits_fa(double dprime, double& hits, double& false_alarms, double crit_z) {
  dprime = fabs(dprime);	// keep it positive
  if(crit_z < 0.0) crit_z = 0.0;
  if(crit_z > 1.0) crit_z = 1.0;
  hits = gauss_cum(crit_z * dprime);
  false_alarms = gauss_cum(-(1.0 - crit_z)*dprime);
  double pct_correct = (hits + (1.0 - false_alarms)) / 2.0;
  return pct_correct;
}

double taMath_double::chisq_p(double X, double v) {
  return gamma_p(0.5 * v, 0.5 * X * X);
}

double taMath_double::chisq_q(double X, double v) {
  return gamma_q(0.5 * v, 0.5 * X * X);
}

double taMath_double::students_cum(double t, double df) {
  return 1.0 - beta_i(0.5*df, 0.5, df / (df + t * t));
}

double taMath_double::students_cum_cum(double t, double df) {
  float pgtabst = 1.0 - students_cum(t,df);
  if(t<0)
    return 0.5*(pgtabst);
  else
    return 1.0 - 0.5*pgtabst;
}

double taMath_double::students_den(double t, double df) {
  // just use discrete approximation..
  return (students_cum(t + 1.0e-6, df) - students_cum(t - 1.0e-6, df)) / 4.0e-6;
}

double taMath_double::Ftest_q(double F, double v1, double v2) {
  return beta_i(0.5*v2, 0.5*v1, v2 / (v2 + (v1 * F)));
}

double taMath_double::d_sub_a(double_Matrix* vec_signal, double_Matrix* vec_noise) {
  return ( vec_mean(vec_signal) - vec_mean(vec_noise) ) / ( sqrt( ( vec_std_dev(vec_signal) + vec_std_dev(vec_noise) ) / 2 ) );
}

double taMath_double::integrate_polynomial(double_Matrix* coef, double min, double max) {
  double area = 0;

  for (int n = 0; n < coef->size; n++)
    area += coef->FastEl(n) * ( ( pow(max, n + 1) / (n + 1) )  -  ( pow(min, n + 1 ) / ( n + 1 ) )  );

  return area;

}

double taMath_double::cdf_inv(double x) {return gsl_cdf_ugaussian_Pinv(x);}

/////////////////////////////////////////////////////////////////////////////////
// Vector operations (operate on Matrix objects, treating as a single linear guy)

///////////////////////////////////////
// arithmetic ops

void taMath_double::vec_fm_ints(double_Matrix* double_mat, const int_Matrix* int_mat) {
  double_mat->SetGeomN(int_mat->geom);
  for(int i=0;i<int_mat->size;i++)
    double_mat->FastEl_Flat(i) = (double)int_mat->FastEl_Flat(i);
  double_mat->CopyElView(*int_mat);
}

void taMath_double::vec_to_ints(int_Matrix* int_mat, const double_Matrix* double_mat) {
  int_mat->SetGeomN(double_mat->geom);
  for(int i=0;i<double_mat->size;i++)
    int_mat->FastEl_Flat(i) = (int)double_mat->FastEl_Flat(i);
  int_mat->CopyElView(*double_mat);
}

bool taMath_double::vec_check_type(const double_Matrix* a) {
  if(!a) {
    taMisc::Error("taMath: vector is NULL");
    return false;
  }
  if(a->GetDataValType() != taBase::VT_DOUBLE) {
    taMisc::Error("taMath: vector is not of type double -- must be for this operation");
    return false;
  }
  return true;
}

bool taMath_double::vec_check_same_size(const double_Matrix* a, const double_Matrix* b,
                                        bool quiet, bool flex) {
  if(!vec_check_type(a) || !vec_check_type(b)) return false;
  return a->ElemWiseOpTest(*b, flex, "math");
}

// todo: add some update signal after mod ops?  ItemsChanged?

bool taMath_double::vec_add(double_Matrix* a, const double_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  *a += *b;                      // use matrix code
  return true;
}

bool taMath_double::vec_sub(double_Matrix* a, const double_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  *a -= *b;                      // use matrix code
  return true;
}

bool taMath_double::vec_mult_els(double_Matrix* a, const double_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  *a *= *b;                      // use matrix code
  return true;
}

bool taMath_double::vec_div_els(double_Matrix* a, const double_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  *a /= *b;                      // use matrix code
  return true;
}

bool taMath_double::vec_add_scalar(double_Matrix* a, double b) {
  if(!vec_check_type(a)) return false;
  TA_FOREACH_INDEX(i, *a) {
    a->FastEl_Flat(i) += b;
  }
  return true;
}

bool taMath_double::vec_sub_scalar(double_Matrix* a, double b) {
  if(!vec_check_type(a)) return false;
  TA_FOREACH_INDEX(i, *a) {
    a->FastEl_Flat(i) -= b;
  }
  return true;
}

bool taMath_double::vec_mult_scalar(double_Matrix* a, double b) {
  if(!vec_check_type(a)) return false;
  TA_FOREACH_INDEX(i, *a) {
    a->FastEl_Flat(i) *= b;
  }
  return true;
}

bool taMath_double::vec_div_scalar(double_Matrix* a, double b) {
  if(!vec_check_type(a)) return false;
  if(b == 0.0) return false;
  TA_FOREACH_INDEX(i, *a) {
    a->FastEl_Flat(i) /= b;
  }
  return true;
}

bool taMath_double::vec_quantize(double_Matrix* vec, double grid) {
  if(!vec_check_type(vec)) return false;
  if(grid == 0.0) return false;
  TA_FOREACH_INDEX(i, *vec) {
    vec->FastEl_Flat(i) = quantize(vec->FastEl_Flat(i), grid);
  }
  return true;
}

bool taMath_double::vec_simple_math(double_Matrix* vec, const SimpleMathSpec& math_spec) {
  if(!vec_check_type(vec)) return false;
  TA_FOREACH_INDEX(i, *vec) {
    vec->FastEl_Flat(i) = math_spec.Evaluate(vec->FastEl_Flat(i));
  }
  return true;
}

bool taMath_double::vec_simple_math_arg(double_Matrix* vec, const double_Matrix* arg_ary,
                                        const SimpleMathSpec& math_spec) {
  if(!vec_check_same_size(vec, arg_ary)) return false;
  SimpleMathSpec myms = math_spec;
  TA_FOREACH_INDEX_TWO(ai, *vec, bi, *arg_ary) {
    myms.arg = arg_ary->FastEl_Flat(bi);
    vec->FastEl_Flat(ai) = myms.Evaluate(vec->FastEl_Flat(ai));
  }
  return true;
}

bool taMath_double::vec_students_cum(double_Matrix* t, const double_Matrix* df) {
  if(!vec_check_same_size(t, df)) return false;
  TA_FOREACH_INDEX_TWO(ai, *t, bi, *df) {
    t->FastEl_Flat(ai) = students_cum(t->FastEl_Flat(ai), df->FastEl_Flat(bi));
  }
  return true;
}

bool taMath_double::vec_students_cum_cum(double_Matrix* t, const double_Matrix* df) {
  if(!vec_check_same_size(t, df)) return false;
  TA_FOREACH_INDEX_TWO(ai, *t, bi, *df) {
    t->FastEl_Flat(ai) = students_cum_cum(t->FastEl_Flat(ai), df->FastEl_Flat(bi));
  }
  return true;
}

bool taMath_double::vec_gauss_inv(double_Matrix* p) {
  if(!vec_check_type(p)) return false;
  TA_FOREACH_INDEX(i, *p) {
    p->FastEl_Flat(i) = gauss_inv(p->FastEl_Flat(i));
  }
  return true;
}

bool taMath_double::vec_gauss_inv_lim(double_Matrix* p) {
  if(!vec_check_type(p)) return false;
  TA_FOREACH_INDEX(i, *p) {
    p->FastEl_Flat(i) = gauss_inv_lim(p->FastEl_Flat(i));
  }
  return true;
}

double taMath_double::vec_first(const double_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    TA_FOREACH_INDEX(i, *vec) {
      return vec->FastEl_Flat(i);
    }
  }
  return vec->FastEl_Flat(0);
}

double taMath_double::vec_last(const double_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    TA_FOREACH_INDEX_REV(i, *vec) {
      return vec->FastEl_Flat(i);
    }
  }
  return vec->FastEl_Flat(vec->size-1);
}

int taMath_double::vec_find_first(const double_Matrix* vec, Relation& rel) {
  if(!vec_check_type(vec)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  TA_FOREACH_INDEX(i, *vec) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) return i;
  }
  return -1;
}

int taMath_double::vec_find_last(const double_Matrix* vec, Relation& rel) {
  if(!vec_check_type(vec)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  TA_FOREACH_INDEX_REV(i, *vec) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) return i;
  }
  return -1;
}

double taMath_double::vec_max(const double_Matrix* vec, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    return vec->Max().toDouble(); // just use logic in obj
  }
  double rval = vec->FastEl_Flat(0);
  for(int i=1;i<vec->size;i++) {
    if(vec->FastEl_Flat(i) > rval) {
      idx = i;
      rval = vec->FastEl_Flat(i);
    }
  }
  return rval;
}

double taMath_double::vec_abs_max(const double_Matrix* vec, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    bool first = true;
    double rval = 0.0;
    TA_FOREACH_INDEX(i, *vec) {
      if(first) {
	rval = fabs(vec->FastEl_Flat(i));
	first = false;
      }
      else {
	if(fabs(vec->FastEl_Flat(i)) > rval) {
	  idx = i;
	  rval = fabs(vec->FastEl_Flat(i));
	}
      }
    }
    return rval;
  }
  double rval = fabs(vec->FastEl_Flat(0));
  for(int i=1;i<vec->size;i++) {
    if(fabs(vec->FastEl_Flat(i)) > rval) {
      idx = i;
      rval = fabs(vec->FastEl_Flat(i));
    }
  }
  return rval;
}

double taMath_double::vec_next_max(const double_Matrix* vec, int max_idx, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size < 2) return 0.0;
  double rval;
  if(max_idx == 0)
    rval = vec->FastEl_Flat(1);
  else
    rval = vec->FastEl_Flat(0);
  for(int i=1;i<vec->size;i++) {
    if(i == max_idx) continue;
    if(vec->FastEl_Flat(i) > rval) {
      idx = i;
      rval = vec->FastEl_Flat(i);
    }
  }
  return rval;
}

double taMath_double::vec_min(const double_Matrix* vec, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    return vec->Min().toDouble(); // just use logic in obj
  }
  double rval = vec->FastEl_Flat(0);
  for(int i=1;i<vec->size;i++) {
    if(vec->FastEl_Flat(i) < rval) {
      idx = i;
      rval = vec->FastEl_Flat(i);
    }
  }
  return rval;
}

double taMath_double::vec_abs_min(const double_Matrix* vec, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    bool first = true;
    double rval = 0.0;
    TA_FOREACH_INDEX(i, *vec) {
      if(first) {
	rval = fabs(vec->FastEl_Flat(i));
	first = false;
      }
      else {
	if(fabs(vec->FastEl_Flat(i)) < rval) {
	  idx = i;
	  rval = fabs(vec->FastEl_Flat(i));
	}
      }
    }
    return rval;
  }
  double rval = fabs(vec->FastEl_Flat(0));
  for(int i=1;i<vec->size;i++) {
    if(fabs(vec->FastEl_Flat(i)) < rval) {
      idx = i;
      rval = fabs(vec->FastEl_Flat(i));
    }
  }
  return rval;
}

double taMath_double::vec_next_min(const double_Matrix* vec, int min_idx, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size < 2) return 0.0;
  double rval;
  if(min_idx == 0)
    rval = vec->FastEl_Flat(1);
  else
    rval = vec->FastEl_Flat(0);
  for(int i=1;i<vec->size;i++) {
    if(i == min_idx) continue;
    if(vec->FastEl_Flat(i) < rval) {
      idx = i;
      rval = vec->FastEl_Flat(i);
    }
  }
  return rval;
}

double taMath_double::vec_sum(const double_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  double rval = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    rval += vec->FastEl_Flat(i);
  }
  return rval;
}

double taMath_double::vec_sum_range(const double_Matrix* vec, int start, int end) {
  if(!vec_check_type(vec)) return false;
  double rval = 0.0;
  if(start<0)
    start = vec->size - 1;
  if(end<0)
    end = vec->size;
  if(start>end)
    return rval;
  for(int i=start;i<end;i++)
    rval += vec->FastEl_Flat(i);
  return rval;
}

double taMath_double::vec_prod(const double_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0) return 0.0;
  double rval = 0.0;
  if(vec->ElView()) {		// significantly less efficient
    TA_FOREACH_INDEX(i, *vec) {
      if(FOREACH_itr->count == 0)
	rval = vec->FastEl_Flat(i);
      else
	rval *= vec->FastEl_Flat(i);
    }
  }
  else {
    rval = vec->FastEl_Flat(0);
    for(int i=1;i<vec->size;i++) {
      rval *= vec->FastEl_Flat(i);
    }
  }
  return rval;
}

double taMath_double::vec_mean(const double_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0)    return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    int ic = vec->IterCount();
    if(ic == 0) return 0.0;
    return vec_sum(vec) / (double)ic;
  }
  return vec_sum(vec) / (double)vec->size;
}

double taMath_double::vec_var(const double_Matrix* vec, double mean, bool use_mean,
			      bool use_est) {
  if(!vec_check_type(vec)) return false;
  if(vec->size <= 1)    return 0.0;
  if(!use_mean)
    mean = vec_mean(vec);
  double rval = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    double val = (vec->FastEl_Flat(i) - mean);
    rval += val * val;
  }
  return rval / ((double)vec->size - (double)use_est);
}

double taMath_double::vec_std_dev(const double_Matrix* vec, double mean, bool use_mean, bool use_est) {
  if(vec->size <= 1)    return 0.0;
  return sqrt(vec_var(vec, mean, use_mean, use_est));
}

double taMath_double::vec_sem(const double_Matrix* vec, double mean, bool use_mean) {
  if(!vec_check_type(vec)) return false;
  if(vec->size <= 1)    return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    int ic = vec->IterCount();
    if(ic <= 1) return 0.0;
    return vec_std_dev(vec, mean, use_mean, true) / sqrt((double)ic);
  }
  return vec_std_dev(vec, mean, use_mean, true) / sqrt((double)vec->size);
}

double taMath_double::vec_ss_len(const double_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  double rval = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    double val = vec->FastEl_Flat(i);
    rval += val * val;
  }
  return rval;
}

double taMath_double::vec_norm(const double_Matrix* vec) {
  return sqrt(vec_ss_len(vec));
}

double taMath_double::vec_ss_mean(const double_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size <= 1)    return 0.0;
  double rval = 0.0;
  double mean = vec_mean(vec);
  TA_FOREACH_INDEX(i, *vec) {
    double val = vec->FastEl_Flat(i) - mean;
    rval += val * val;
  }
  return rval;
}

void taMath_double::vec_histogram(double_Matrix* vec, const double_Matrix* oth, double bin_size,
                                  double min_val, double max_val) {
  if(!vec_check_type(vec) || !vec_check_type(oth)) return;
  if(oth->size == 0) return;
  vec->SetGeom(1,0);
  vec->Reset();
  double_Array tmp;
  tmp.SetSize(oth->IterCount());
  TA_FOREACH_INDEX(i, *vec) {
    tmp[FOREACH_itr->count] = oth->FastEl_Flat(i);
  }
  tmp.Sort();
  double min_v = tmp.FastEl(0);
  double max_v = tmp.Peek();
  if(min_val != max_val) {
    min_v = min_val;
    max_v = max_val;
  }
  int src_idx = 0;
  int trg_idx = 0;
  while((src_idx < tmp.size) && (tmp.FastEl(src_idx) < min_v)) { // skip up to the min
    src_idx++;
  }
  for(double cur_val = min_v; cur_val <= max_v; cur_val += bin_size, trg_idx++) {
    double cur_max = cur_val + bin_size;
    vec->Add(0);
    double& cur_hist = vec->FastEl_Flat(trg_idx);
    while((src_idx < tmp.size) && (tmp.FastEl(src_idx) < cur_max)) {
      cur_hist += 1.0;
      src_idx++;
    }
  }
}

double taMath_double::vec_count(const double_Matrix* vec, Relation& rel) {
  if(!vec_check_type(vec)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  double rval = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) rval += 1.0;
  }
  return rval;
}

double taMath_double::vec_median(const double_Matrix* vec) {
  if(!vec_check_type(vec)) return 0.0;
  if(vec->size == 0) return 0.0;
  double_Array tmp;
  tmp.SetSize(vec->IterCount());
  TA_FOREACH_INDEX(i, *vec) {
    tmp[FOREACH_itr->count] = vec->FastEl_Flat(i);
  }
  tmp.Sort();
  int idx = tmp.size / 2;
  return tmp[idx];
}

double taMath_double::vec_quantile(const double_Matrix* vec, double quant_pos) {
  if(!vec_check_type(vec)) return 0.0;
  if(vec->size == 0) return 0.0;
  double_Array tmp;
  tmp.SetSize(vec->IterCount());
  TA_FOREACH_INDEX(i, *vec) {
    tmp[FOREACH_itr->count] = vec->FastEl_Flat(i);
  }
  tmp.Sort();
  int idx = (int)(quant_pos * (double)tmp.size);
  if(idx >= tmp.size) idx = tmp.size-1;
  if(idx < 0) idx = 0;
  return tmp[idx];
}

double taMath_double::vec_kwta(double_Matrix* vec, int k, bool descending) {
  if(!vec_check_type(vec)) return 0.0;
  if(vec->size == 0) return 0.0;
  int_Array act_idx;
  // first fill act buff and get k for first items in list
  act_idx.SetSize(k);
  act_idx.FillSeq();
  int k_idx = 0;                // location of k guy within act_
  double k_val = vec->FastEl_Flat(0);
  if(descending) {
    for(int i=1;i<k; i++) {
      double val = vec->FastEl_Flat(i);
      if(val < k_val) {
        k_val = val;
        k_idx = i;
      }
    }
    // now, use the "replace-the-lowest" sorting technique
    for(int i=k;i<vec->size; i++) {
      double val = vec->FastEl_Flat(i);
      if(val <= k_val) {        // not bigger than smallest one in sort buffer
        continue;
      }
      act_idx.FastEl(k_idx) = i; // replace the smallest with it
      k_val = val;              // assume its the smallest
      for(int j=0; j < k; j++) {        // and recompute the actual smallest
        double val = vec->FastEl_Flat(act_idx[j]);
        if(val < k_val) {
          k_val = val;
          k_idx = j;            // idx in top k
        }
      }
    }
  }
  else {
    for(int i=1;i<k; i++) {
      double val = vec->FastEl_Flat(i);
      if(val > k_val) {
        k_val = val;
        k_idx = i;
      }
    }
    // now, use the "replace-the-lowest" sorting technique
    for(int i=k;i<vec->size; i++) {
      double val = vec->FastEl_Flat(i);
      if(val >= k_val) {        // not bigger than smallest one in sort buffer
        continue;
      }
      act_idx.FastEl(k_idx) = i; // replace the smallest with it
      k_val = val;              // assume its the smallest
      for(int j=0; j < k; j++) {        // and recompute the actual smallest
        double val = vec->FastEl_Flat(act_idx[j]);
        if(val > k_val) {
          k_val = val;
          k_idx = j;            // idx in top k
        }
      }
    }
  }
  return k_val;
}

void taMath_double::vec_kwta_avg(double& top_k_avg, double& bot_k_avg,
                                 double_Matrix* vec, int k, bool descending) {
  if(!vec_check_type(vec)) return;
  if(vec->size == 0) return;

  // first fill act buff and get k for first items in list
  int_Array act_idx;
  act_idx.SetSize(k);
  act_idx.FillSeq();

  int_Array inact_idx;
  inact_idx.SetSize(vec->size - k);
  for(int i=k; i<vec->size; i++)
    inact_idx.FastEl(i-k) = i;

  int k_idx = 0;                // location of k guy within act_
  double k_val = vec->FastEl_Flat(0);
  if(descending) {
    for(int i=1;i<k; i++) {
      double val = vec->FastEl_Flat(i);
      if(val < k_val) {
        k_val = val;
        k_idx = i;
      }
    }
    // now, use the "replace-the-lowest" sorting technique
    for(int i=k;i<vec->size; i++) {
      double val = vec->FastEl_Flat(i);
      if(val <= k_val)  // not bigger than smallest one in sort buffer
        continue;
      inact_idx.FastEl(i-k) = act_idx.FastEl(k_idx);
      act_idx.FastEl(k_idx) = i; // replace the smallest with it
      k_val = val;              // assume its the smallest
      for(int j=0; j < k; j++) {        // and recompute the actual smallest
        double val = vec->FastEl_Flat(act_idx[j]);
        if(val < k_val) {
          k_val = val;
          k_idx = j;            // idx in top k
        }
      }
    }
  }
  else {
    for(int i=1;i<k; i++) {
      double val = vec->FastEl_Flat(i);
      if(val > k_val) {
        k_val = val;
        k_idx = i;
      }
    }
    // now, use the "replace-the-lowest" sorting technique
    for(int i=k;i<vec->size; i++) {
      double val = vec->FastEl_Flat(i);
      if(val >= k_val) {        // not bigger than smallest one in sort buffer
        continue;
      }
      inact_idx.FastEl(i-k) = act_idx.FastEl(k_idx);
      act_idx.FastEl(k_idx) = i; // replace the smallest with it
      k_val = val;              // assume its the smallest
      for(int j=0; j < k; j++) {        // and recompute the actual smallest
        double val = vec->FastEl_Flat(act_idx[j]);
        if(val > k_val) {
          k_val = val;
          k_idx = j;            // idx in top k
        }
      }
    }
  }

  float topsum = 0.0f;
  for(int j=0; j < k; j++) {
    double val = vec->FastEl_Flat(act_idx[j]);
    topsum += val;
  }
  float botsum = 0.0f;
  for(int j=0; j < inact_idx.size; j++) {
    double val = vec->FastEl_Flat(inact_idx[j]);
    botsum += val;
  }

  top_k_avg = topsum / (double)k;
  bot_k_avg = botsum / (double)(vec->size - k);
}

double taMath_double::vec_mode(const double_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0) return 0.0f;
  double_Array tmp;
  tmp.SetSize(vec->IterCount());
  TA_FOREACH_INDEX(i, *vec) {
    tmp[FOREACH_itr->count] = vec->FastEl_Flat(i);
  }
  tmp.Sort();
  int mx_frq = 0;
  double mode = 0.0f;
  int idx = 0;
  while(idx < tmp.size) {
    double val = tmp[idx];
    int st_idx = idx;
    while((idx < tmp.size-1) && (val == tmp[++idx]));
    int frq = idx - st_idx;
    if(idx == tmp.size-1) {
      if(tmp[tmp.size-1] == val) frq++;
      idx++;
    }
    if(frq > mx_frq) {
      mx_frq = frq;
      mode = val;
    }
  }
  return mode;
}

bool taMath_double::vec_uniq(const taMatrix* src_vec, taMatrix* dest_vec, const bool& sort_first) {
  if (!dest_vec || !src_vec) { taMisc::Error("vec_uniq - both src_vec and dest_vec must be defined. try dest_vec = new double_Matrix.");return false;}
  dest_vec->SetGeom(1,1);
  Variant cur_val, prv_val;

  if (sort_first) { // `uniq -s' output a list of truly unique elements
    for (int i=0; i < src_vec->size; i++) {
      cur_val = src_vec->FastElAsVar_Flat(i);
      if (i==0) {
        dest_vec->SetFmVar(cur_val, i);
        continue;
      }
      if (dest_vec->FindVal_Flat(src_vec->FastElAsVar_Flat(i), 0) == -1) {
        dest_vec->AddFrame();
        dest_vec->SetFmVar(cur_val, dest_vec->size-1);
      }
    }
  }
  else { // `uniq' remove all consecutive elements
    for (int i=0; i < src_vec->size; i++) {
      cur_val = src_vec->FastElAsVar_Flat(i);
      if (i==0) {
        prv_val = cur_val;
        dest_vec->SetFmVar(cur_val, i);
        continue;
      }
      if (cur_val != prv_val) {
        prv_val = cur_val;
        dest_vec->AddFrame();
        dest_vec->SetFmVar(cur_val, dest_vec->size-1);
      }
    }
  }
  return true;
}

bool taMath_double::mat_concat(const taMatrix* src_a_mat, const taMatrix* src_b_mat,
                               taMatrix* dest_mat, const int& dim) {

  if (!src_a_mat||!src_b_mat||!dest_mat) { taMisc::Error("mat_concat - a, b and dest matrices must all be defined"); return false; }

  int a_dims = src_a_mat->dims();
  if (a_dims != 2) {taMisc::Error("mat_concat - a and b must have 2 dimensions"); return false;};
  int a_d0 = src_a_mat->dim(0);
  int a_d1 = src_a_mat->dim(1);

  int b_dims = src_b_mat->dims();
  if (b_dims != 2) {taMisc::Error("mat_concat - a and b must have 2 dimensions"); return false;};
  int b_d0 = src_b_mat->dim(0);
  int b_d1 = src_b_mat->dim(1);

  Variant val;

  cout << a_d0 << " " << b_d0 << " " << a_d1 << " " << b_d1 << "\n";    cout << a_d0 << " " << b_d0 << " " << a_d1 << " " << b_d1 << "\n";

  if(dim == 0) { // col concat
    if (a_d1 != b_d1) {taMisc::Error("mat_concat - col concat specified (d0) but a and b do not have the same number of rows (d1)"); return false;}

    dest_mat->SetGeom(2, a_d0 + b_d0, a_d1);

    // fill a into dest
    for (int i=0; i < a_d0; i++) {
      for (int j=0; j < a_d1; j++) {
        val = src_a_mat->SafeElAsVar(i, j);
//      cout << val;
//      dest_mat->SetFmVar(val, i, j);
      }
    }
  }
  else { // row concat
    if (a_d0 != b_d0) {
      taMisc::Error("mat_concat - row concat specified (d1) but a and b do not have the same number of cols (d0)");
      return false;
    }
  }
  return true;
}


void taMath_double::vec_sort(double_Matrix* vec, bool descending) {
  if(!vec_check_type(vec)) return;
  if(vec->size <= 1) return;
  vec->Sort(descending);
}

String taMath_double::vec_stats(const double_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  String rval;
  int idx;
  rval += "n=" + String(vec->size) + "; ";
  rval += "min=" + String(vec_min(vec, idx)) + "; ";
  rval += "max=" + String(vec_max(vec, idx)) + "; ";
  double mean = vec_mean(vec);
  rval += "mean=" + String(mean) + "; ";
  rval += "median=" + String(vec_median(vec)) + "; ";
  rval += "mode=" + String(vec_mode(vec)) + "; ";
  rval += "stdev=" + String(vec_std_dev(vec, mean, true)) + "; ";
  rval += "sem=" + String(vec_sem(vec, mean, true)) + ";";
  return rval;
}

int taMath_double::vec_prob_choose(double_Matrix* vec) {
  if(!vec_check_type(vec)) return -1;
  double rndval = Random::ZeroOne();
  double psum = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    psum += vec->FastEl_Flat(i);
    if(rndval < psum) {
      return FOREACH_itr->count;
    }
  }
  return vec->IterCount()-1;		// just to be safe..
}

double taMath_double::vec_dprime(const double_Matrix* signal_vec,
				 const double_Matrix* noise_vec) {
  if(!vec_check_type(signal_vec) || !vec_check_type(noise_vec)) return 0.0f;
  double mean_signal = vec_mean(signal_vec);
  double stdev_signal = vec_std_dev(signal_vec, mean_signal, true);
  double mean_noise = vec_mean(noise_vec);
  double stdev_noise = vec_std_dev(noise_vec, mean_noise, true);
  return dprime(mean_signal, stdev_signal, mean_noise, stdev_noise);
}

bool taMath_double::vec_regress_lin(const double_Matrix* x_vec, const double_Matrix* y_vec,
                                      double& b, double& m, double& cov00, double& cov01,
                                      double& cov11, double& sum_sq) {
  if(!vec_check_type(x_vec) || !vec_check_type(y_vec)) return false;
#ifdef HAVE_LIBGSL
  gsl_fit_linear((double*)x_vec->data(), 1, (double*)y_vec->data(), 1,
                 MIN(x_vec->size, y_vec->size), &b, &m, &cov00, &cov01, &cov11, &sum_sq);
  return true;
#else
  return false;
#endif
}

bool taMath_double::vec_regress_multi_lin(double_Matrix* X, double_Matrix* Y,
                                          double_Matrix* C, double_Matrix* cov, double& chisq) {
#ifdef HAVE_LIBGSL
  gsl_matrix gsl_X, gsl_cov;
  gsl_vector gsl_Y, gsl_C;
  mat_get_gsl_fm_ta(&gsl_X, X);
  mat_get_gsl_fm_ta(&gsl_cov, cov);
  vec_get_gsl_fm_ta(&gsl_Y, Y);
  vec_get_gsl_fm_ta(&gsl_C, C);

  gsl_multifit_linear_workspace* work = gsl_multifit_linear_alloc(X->dim(1), X->dim(0));
  gsl_multifit_linear(&gsl_X, &gsl_Y, &gsl_C, &gsl_cov, &chisq, work);
  gsl_multifit_linear_free(work);

  return true;
#else
  return false;
#endif

}

bool taMath_double::vec_regress_multi_lin_polynomial(double_Matrix* dx, double_Matrix* dy,
                                                     double_Matrix* coef, double_Matrix* cov,
                                                     int degree, double& chisq) {
#ifdef HAVE_LIBGSL
  if (!(dx->size == dy->size))
    taMisc::Error("dx and dy must have the same dimensionality");

  int obs = dx->size;

  coef->SetGeom(1, degree);
  cov->SetGeom(2, degree, degree);

  gsl_matrix *X;
  //gsl_matrix gsl_dx;
  gsl_matrix gsl_cov;
  gsl_vector gsl_dy;
  gsl_vector gsl_coef;

  X = gsl_matrix_alloc(obs, degree);

  mat_get_gsl_fm_ta(&gsl_cov, cov);
  vec_get_gsl_fm_ta(&gsl_dy, dy);
  vec_get_gsl_fm_ta(&gsl_coef, coef);

    for (int i = 0; i < obs; i++) {
      gsl_matrix_set(X, i, 0, 1.0);
      for (int j = 0; j < degree; j++) {
        gsl_matrix_set(X, i, j, pow(dx->FastEl(i), j));
      }
    }

  gsl_multifit_linear_workspace* work = gsl_multifit_linear_alloc(obs, degree);

  gsl_multifit_linear(X, &gsl_dy, &gsl_coef, &gsl_cov, &chisq, work);
  gsl_multifit_linear_free(work);

  return true;
#else
  return false;
#endif

}


bool taMath_double::vec_jitter_gauss(double_Matrix* vec, double stdev) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0) return false;

  double_Array tmp;
  int jitter, vec_size;
  int zeroes = 0;
  int new_index = -1;
  double el;

  vec_size = vec->size;
  tmp.SetSize(vec_size);

  for(int i=0;i<vec_size;i++) {
    el = vec->FastEl_Flat(i);
    if(el == 0.0f)
      zeroes += 1;
    tmp[i] = el;
  }

  if(zeroes == 0) {
    taMisc::Error("There must be at least one non-zero element in vec");
    return false;
  }

  vec->InitVals(0);

  for (int i=0;i<vec_size;i++) {
    el = tmp[i];
    if(el != 0) {
      while (new_index < 0 || new_index > vec_size) {
        jitter = int(Random::Gauss(stdev)+0.5f);
        new_index = jitter+i;
        if (vec->FastEl_Flat(new_index) != 0)
          new_index = -1;
      }
      vec->Set_Flat(el, new_index);
      new_index = -1;
    }
  }

  return true;
}

///////////////////////////////////////
// distance metrics (comparing two vectors)

double taMath_double::vec_ss_dist(const double_Matrix* vec, const double_Matrix* oth, bool norm,
                                  double tolerance)
{
  if(!vec_check_same_size(vec, oth)) return -1.0;
  double rval = 0.0;
  TA_FOREACH_INDEX_TWO(ai, *vec, bi, *oth) {
    double d = vec->FastEl_Flat(ai) - oth->FastEl_Flat(bi);
    if(fabs(d) > tolerance)
      rval += d * d;
  }
  if(norm) {
    double dist = vec_ss_len(vec) + vec_ss_len(oth);
    if(dist != 0.0f)
      rval /= dist;
  }
  return rval;
}

double taMath_double::vec_euclid_dist(const double_Matrix* vec, const double_Matrix* oth, bool norm,
                                      double tolerance)
{
  double ssd = vec_ss_dist(vec, oth, norm, tolerance);
  if(ssd < 0.0) return ssd;
  return sqrt(ssd);
}

double taMath_double::vec_hamming_dist(const double_Matrix* vec, const double_Matrix* oth, bool norm,
                                      double tolerance)
{
  if(!vec_check_same_size(vec, oth)) return -1.0;
  double rval = 0.0;
  double alen = 0.0;
  double blen = 0.0;
  TA_FOREACH_INDEX_TWO(ai, *vec, bi, *oth) {
    double d = fabs(vec->FastEl_Flat(ai) - oth->FastEl_Flat(bi));
    if(d <= tolerance)  d = 0.0;
    rval += d;
    if(norm) {
      alen += fabs(vec->FastEl_Flat(ai));
      blen += fabs(oth->FastEl_Flat(bi));
    }
  }
  if(norm) {
    double dist = alen + blen;
    if(dist != 0.0f)
      rval /= dist;
  }
  return rval;
}

double taMath_double::vec_covar(const double_Matrix* vec, const double_Matrix* oth) {
  if(!vec_check_same_size(vec, oth)) return -1.0;
  double my_mean = vec_mean(vec);
  double oth_mean = vec_mean(oth);
  double rval = 0.0;
  TA_FOREACH_INDEX_TWO(ai, *vec, bi, *oth) {
    rval += (vec->FastEl_Flat(ai) - my_mean) * (oth->FastEl_Flat(bi) - oth_mean);
  }
  return rval / (double)vec->IterCount();
}

double taMath_double::vec_correl(const double_Matrix* vec, const double_Matrix* oth) {
  if(!vec_check_same_size(vec, oth)) return -1.0;
  double my_mean = vec_mean(vec);
  double oth_mean = vec_mean(oth);
  double my_var = 0.0;
  double oth_var = 0.0;
  double rval = 0.0;
  TA_FOREACH_INDEX_TWO(ai, *vec, bi, *oth) {
    double my_val = vec->FastEl_Flat(ai) - my_mean;
    double oth_val = oth->FastEl_Flat(bi) - oth_mean;
    rval += my_val * oth_val;
    my_var += my_val * my_val;
    oth_var += oth_val * oth_val;
  }
  double var_prod = sqrt(my_var * oth_var);
  if(var_prod != 0.0f)
    return rval / var_prod;
  else
    return 0.0;
}

double taMath_double::vec_inner_prod(const double_Matrix* vec, const double_Matrix* oth, bool norm) {
  if(!vec_check_same_size(vec, oth)) return -1.0;
  double rval = 0.0;
  TA_FOREACH_INDEX_TWO(ai, *vec, bi, *oth) {
    rval += vec->FastEl_Flat(ai) * oth->FastEl_Flat(bi);
  }
  if(norm) {
    double dist = sqrt(vec_ss_len(vec) * vec_ss_len(oth));
    if(dist != 0.0f)
      rval /= dist;
  }
  return rval;
}

double taMath_double::vec_cross_entropy(const double_Matrix* vec, const double_Matrix* oth) {
  if(!vec_check_same_size(vec, oth)) return -1.0;
  double rval = 0.0;
  TA_FOREACH_INDEX_TWO(ai, *vec, bi, *oth) {
    double p = vec->FastEl_Flat(ai);
    double q = oth->FastEl_Flat(bi);
    q = max(q,0.000001); q = max(q,0.999999);
    if(p >= 1.0)
      rval += -log(q);
    else if(p <= 0.0)
      rval += -log(1.0 - q);
    else
      rval += p * log(p/q) + (1.0 - p) * log((1.0 - p) / (1.0 - q));
  }
  return rval;
}

double taMath_double::vec_dist(const double_Matrix* vec, const double_Matrix* oth,
                               DistMetric metric, bool norm, double tolerance)
{
  switch(metric) {
  case SUM_SQUARES:
    return vec_ss_dist(vec, oth, norm, tolerance);
  case EUCLIDIAN:
    return vec_euclid_dist(vec, oth, norm, tolerance);
  case HAMMING:
    return vec_hamming_dist(vec, oth, norm, tolerance);
  case COVAR:
    return vec_covar(vec, oth);
  case CORREL:
    return vec_correl(vec, oth);
  case INNER_PROD:
    return vec_inner_prod(vec, oth, norm);
  case CROSS_ENTROPY:
    return vec_cross_entropy(vec, oth);
  }
  return -1.0;
}

double taMath_double::scalar_dist(double v1, double v2,
                                  DistMetric metric, double tolerance)
{
  double rval = -1.0;
  switch(metric) {
  case SUM_SQUARES:
  case COVAR:
    rval = (v1 - v2);
    if(fabs(rval) < tolerance) rval = 0.0f;
    else rval *= rval;
    break;
  case EUCLIDIAN:
  case CORREL:
    rval = (v1 - v2);
    if(fabs(rval) < tolerance) rval = 0.0f;
    else rval = sqrt(rval * rval);
    break;
  case HAMMING:
    rval = fabs(v1 - v2);
    if(rval < tolerance) rval = 0.0f;
    break;
  case INNER_PROD:
    rval = v1 * v2;
    break;
  case CROSS_ENTROPY: {
    double q = max(v2,0.000001); q = max(q,0.999999);
    if(v1 >= 1.0)
      rval = -log(q);
    else if(v1 <= 0.0)
      rval = -log(1.0 - q);
    else
      rval = v1 * log(v1/q) + (1.0 - v1) * log((1.0 - v1) / (1.0 - q));
    break;
  }
  }
  return rval;
}

///////////////////////////////////////
// Normalization

double taMath_double::vec_norm_len(double_Matrix* vec, double len) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0)    return 0.0;
  double scale = (len * len) / vec_ss_len(vec);
  TA_FOREACH_INDEX(i, *vec) {
    double val = vec->FastEl_Flat(i);
    double mag = (val * val) * scale;
    vec->FastEl_Flat(i) = (val >= 0.0f) ? mag : -mag;
  }
  return scale;
}

double taMath_double::vec_norm_sum(double_Matrix* vec, double sum, double min_val) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0)    return 0.0;
  double act_sum = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    act_sum += (vec->FastEl_Flat(i) - min_val);
  }
  if(act_sum == 0.0) return 0.0;
  double scale = (sum / act_sum);
  TA_FOREACH_INDEX(i, *vec) {
    vec->FastEl_Flat(i) = ((vec->FastEl_Flat(i) - min_val) * scale) + min_val;
  }
  return scale;
}

double taMath_double::vec_norm_max(double_Matrix* vec, double max) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0)    return 0.0;
  int idx;
  double cur_max = vec_max(vec, idx);
  if(cur_max == 0.0) return 0.0;
  double scale = (max / cur_max);
  TA_FOREACH_INDEX(i, *vec) {
    vec->FastEl_Flat(i) *= scale;
  }
  return scale;
}

double taMath_double::vec_norm_abs_max(double_Matrix* vec, double max) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0)    return 0.0;
  int idx;
  double cur_max = vec_abs_max(vec, idx);
  if(cur_max == 0.0) return 0.0;
  double scale = (max / cur_max);
  TA_FOREACH_INDEX(i, *vec) {
    vec->FastEl_Flat(i) *= scale;
  }
  return scale;
}

int taMath_double::vec_threshold(double_Matrix* vec, double thresh, double low, double high) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0)  return 0;
  int rval = 0;
  TA_FOREACH_INDEX(i, *vec) {
    if(vec->FastEl_Flat(i) >= thresh) {
      vec->FastEl_Flat(i) = high;
      rval++;
    }
    else
      vec->FastEl_Flat(i) = low;
  }
  return rval;
}

int taMath_double::vec_threshold_low(double_Matrix* vec, double thresh, double low) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0)  return 0;
  int rval = 0;
  TA_FOREACH_INDEX(i, *vec) {
    if(vec->FastEl_Flat(i) <= thresh) {
      vec->FastEl_Flat(i) = low;
      rval++;
    }
  }
  return rval;
}

int taMath_double::vec_threshold_high(double_Matrix* vec, double thresh, double high) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0)  return 0;
  int rval = 0;
  TA_FOREACH_INDEX(i, *vec) {
    if(vec->FastEl_Flat(i) >= thresh) {
      vec->FastEl_Flat(i) = high;
      rval++;
    }
  }
  return rval;
}


int taMath_double::vec_replace(double_Matrix* vec, double find1, double repl1,
                               bool do2, double find2, double repl2,
                               bool do3, double find3, double repl3,
                               bool do4, double find4, double repl4,
                               bool do5, double find5, double repl5,
                               bool do6, double find6, double repl6) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0)  return 0;
  int rval = 0;
  TA_FOREACH_INDEX(i, *vec) {
    double vl = vec->FastEl_Flat(i);
    if(vl == find1) { vec->FastEl_Flat(i) = repl1; rval++; }
    if(do2 && vl == find2) { vec->FastEl_Flat(i) = repl2; rval++; }
    if(do3 && vl == find3) { vec->FastEl_Flat(i) = repl3; rval++; }
    if(do4 && vl == find4) { vec->FastEl_Flat(i) = repl4; rval++; }
    if(do5 && vl == find5) { vec->FastEl_Flat(i) = repl5; rval++; }
    if(do6 && vl == find6) { vec->FastEl_Flat(i) = repl6; rval++; }
  }
  return rval;
}

double taMath_double::vec_aggregate(const double_Matrix* vec, Aggregate& agg) {
  if(!vec_check_type(vec)) return 0.0;
  int idx;
  switch(agg.op) {
  case Aggregate::GROUP:
    return vec->SafeEl_Flat(0); // first guy..
  case Aggregate::FIRST:
    return taMath_double::vec_first(vec);
  case Aggregate::LAST:
    return taMath_double::vec_last(vec);
  case Aggregate::FIND_FIRST:
    return (double)taMath_double::vec_find_first(vec, agg.rel);
  case Aggregate::FIND_LAST:
    return (double)taMath_double::vec_find_last(vec, agg.rel);
  case Aggregate::MIN:
    return taMath_double::vec_min(vec, idx);
  case Aggregate::MAX:
    return taMath_double::vec_max(vec, idx);
  case Aggregate::ABS_MIN:
    return taMath_double::vec_abs_min(vec, idx);
  case Aggregate::ABS_MAX:
    return taMath_double::vec_abs_max(vec, idx);
  case Aggregate::SUM:
    return taMath_double::vec_sum(vec);
  case Aggregate::PROD:
    return taMath_double::vec_prod(vec);
  case Aggregate::MEAN:
    return taMath_double::vec_mean(vec);
  case Aggregate::VAR:
    return taMath_double::vec_var(vec);
  case Aggregate::SS:
    return taMath_double::vec_ss_mean(vec);
  case Aggregate::STDEV:
    return taMath_double::vec_std_dev(vec);
  case Aggregate::SEM:
    return taMath_double::vec_sem(vec);
  case Aggregate::N:
    return (double)vec->size;
  case Aggregate::COUNT:
    return taMath_double::vec_count(vec, agg.rel);
  case Aggregate::MEDIAN:
    return taMath_double::vec_median(vec);
  case Aggregate::MODE:
    return taMath_double::vec_mode(vec);
  case Aggregate::QUANTILE: {
    double relval = agg.rel.val;
    if(agg.rel.use_var && (bool)agg.rel.var) {
      relval = agg.rel.var->GetVar().toDouble();
    }
    return taMath_double::vec_quantile(vec, relval);
  }
  case Aggregate::NONE:
    return 0.0;
  }
  return 0.0;
}

/////////////////////////////////////////////////////////////////////////////////
// Convolution

bool taMath_double::vec_kern_uniform(double_Matrix* kernel, int half_sz,
                                     bool neg_tail, bool pos_tail) {
  if(!vec_check_type(kernel)) return false;
  int sz = half_sz * 2 + 1;
  kernel->ClearElView();
  kernel->SetGeom(1, sz);
  double ctr = (double)half_sz;
  double val = 1.0 / (double)sz;
  for(int i=0;i<kernel->size;i++) {
    double x = (double)i - ctr;
    double y = 0.0;
    if(x < 0.0) {
      if(neg_tail)
        y = val;
    }
    else if(x > 0.0) {
      if(pos_tail)
        y = val;
    }
    else {
      y = val;
    }
    kernel->FastEl(i) = y;
  }
  return true;
}

bool taMath_double::vec_kern_gauss(double_Matrix* kernel, int half_sz, double sigma,
                                   bool neg_tail, bool pos_tail) {
  if(!vec_check_type(kernel)) return false;
  kernel->ClearElView();
  kernel->SetGeom(1, half_sz * 2 + 1);
  double off = (double)half_sz;
  double ssq = -1.0 / (2.0 * sigma * sigma);
  for(int i=0;i<kernel->size;i++) {
    double x = (double)i - off;
    double y = exp(ssq * x * x);
    if(x < 0.0) {
      if(!neg_tail)
        y = 0.0;
    }
    else if(x > 0.0) {
      if(!pos_tail)
        y = 0.0;
    }
    kernel->FastEl(i) = y;
  }
  vec_norm_sum(kernel);
  return true;
}

bool taMath_double::vec_kern_exp(double_Matrix* kernel, int half_sz, double exp_mult,
                                 bool neg_tail, bool pos_tail) {
  if(!vec_check_type(kernel)) return false;
  kernel->ClearElView();
  kernel->SetGeom(1, half_sz * 2 + 1);
  double ctr = (double)half_sz;
  for(int i=0;i<kernel->size;i++) {
    double x = (double)i - ctr;
    double y = 0.0;
    if(x < 0.0) {
      if(neg_tail)
        y = exp(exp_mult * x);
    }
    else if(x > 0.0) {
      if(pos_tail)
        y = exp(-exp_mult * x);
    }
    else {
      y = exp(-exp_mult * x);   // always count point
    }
    kernel->FastEl(i) = y;
  }
  vec_norm_sum(kernel);
  return true;
}

bool taMath_double::vec_kern_pow(double_Matrix* kernel, int half_sz, double pow_exp,
                                 bool neg_tail, bool pos_tail) {
  if(!vec_check_type(kernel)) return false;
  kernel->ClearElView();
  kernel->SetGeom(1, half_sz * 2 + 1);
  double ctr = (double)half_sz;
  for(int i=0;i<kernel->size;i++) {
    double x = (double)i - ctr;
    double y = 0.0;
    if(x < 0.0) {
      if(neg_tail)
        y = pow(-x, pow_exp);
    }
    else if(x > 0.0) {
      if(pos_tail)
        y = pow(x, pow_exp);
    }
    else {
      y = 1.0;                  // count self as 1
    }
    kernel->FastEl(i) = y;
  }
  vec_norm_sum(kernel);
  return true;
}

bool taMath_double::vec_convolve(double_Matrix* out_vec, const double_Matrix* in_vec,
                                   const double_Matrix* kernel, bool keep_edges) {
  if(!vec_check_type(kernel) || !vec_check_type(in_vec) || !vec_check_type(out_vec))
    return false;
  if(kernel->size == 0) {
    taMisc::Error("vec_convolve: kernel size == 0");
    return false;
  }
  int off = (kernel->size-1) / 2;
  if(keep_edges) {
    if(out_vec->size != in_vec->size)
      out_vec->SetGeom(1, in_vec->size);
    for(int i=0;i<out_vec->size;i++) {
      double sum = 0.0;
      double dnorm = 0.0;
      for(int j=0;j<kernel->size;j++) {
        int idx = i + j - off;
        if(idx < 0 || idx >= in_vec->size) {
          dnorm += kernel->FastEl(j);
        }
        else {
          sum += in_vec->FastEl(idx) * kernel->FastEl(j);
        }
      }
      if(dnorm > 0.0 && dnorm < 1.0) { // renorm
        sum /= (1.0 - dnorm);
      }
      out_vec->FastEl(i) = sum;
    }
  }
  else {
    if(in_vec->size < kernel->size) {
      taMisc::Error("vec_convolve: input vector size < kernel size -- cannot convolve");
      return false;
    }
    if(out_vec->size != in_vec->size - kernel->size)
      out_vec->SetGeom(1, in_vec->size - kernel->size);
    for(int i=0;i<out_vec->size;i++) {
      double sum = 0.0;
      for(int j=0;j<kernel->size;j++) {
        sum += in_vec->FastEl(i+j) * kernel->FastEl(j);
      }
      out_vec->FastEl(i) = sum;
    }
  }
  return true;
}

bool taMath_double::vec_kern2d_gauss(double_Matrix* kernel, int sz_x, int sz_y,
                                     double sigma_x, double sigma_y) {
  if(!vec_check_type(kernel)) return false;
  kernel->SetGeom(2, sz_x, sz_y);
  double ctr_x = (double)(sz_x-1) * 0.5;
  double ctr_y = (double)(sz_y-1) * 0.5;
  double eff_sig_x = sigma_x * ctr_x;
  double eff_sig_y = sigma_y * ctr_y;
  for(int yi=0; yi < sz_y; yi++) {
    double y = ((double)yi - ctr_y) / eff_sig_y;
    for(int xi=0; xi < sz_x; xi++) {
      double x = ((double)xi - ctr_y) / eff_sig_x;
      double gv = exp(-(x*x + y*y)/2.0f);
      kernel->FastEl(xi, yi) = gv;
    }
  }
  vec_norm_sum(kernel);
  return true;
}


/////////////////////////////////////////////////////////////////////////////////
// Matrix operations

bool taMath_double::mat_col(double_Matrix* col, const double_Matrix* mat, int col_no) {
  if(!vec_check_type(col) || !vec_check_type(mat)) return false;
  if(mat->dims() != 2) return false;
  if(col_no < 0 || col_no >= mat->dim(0))
    return false;
  int rows = mat->dim(1);
  col->SetGeom(1, rows);
  for(int i=0;i<rows;i++) {
    col->FastEl(i) = mat->FastEl(col_no, i);
  }
  return true;
}

bool taMath_double::mat_row(double_Matrix* row, const double_Matrix* mat, int row_no) {
  if(!vec_check_type(row) || !vec_check_type(mat)) return false;
  if(mat->dims() != 2) return false;
  if(row_no < 0 || row_no >= mat->dim(1))
    return false;
  int cols = mat->dim(0);
  row->SetGeom(1, cols);
  for(int i=0;i<cols;i++) {
    row->FastEl(i) = mat->FastEl(i, row_no);
  }
  return true;
}

#ifdef HAVE_LIBGSL

bool taMath_double::mat_get_gsl_fm_ta(gsl_matrix* gsl_mat, const double_Matrix* ta_mat) {
  if(!vec_check_type(ta_mat)) return false;
  if(ta_mat->dims() != 2) {
    taMisc::Error("taMath::mat_get_gsl_fm_ta: dimension of matrix must be 2 for gsl calls!");
    return false;
  }
  gsl_mat->size2 = ta_mat->dim(0); // "rows" (rows are contiguous in mem)
  gsl_mat->size1 = ta_mat->dim(1); // "columns"
  gsl_mat->tda = ta_mat->dim(0); // actual size of row in memory
  gsl_mat->data = (double*)ta_mat->data();
  gsl_mat->block = NULL;
  gsl_mat->owner = false;
  return true;
}

bool taMath_double::vec_get_gsl_fm_ta(gsl_vector* gsl_vec, const double_Matrix* ta_vec) {
  if(!vec_check_type(ta_vec)) return false;
  if(ta_vec->dims() != 1) return false;
  gsl_vec->size = ta_vec->dim(0);
  gsl_vec->stride = 1;
  gsl_vec->data = (double*)ta_vec->data();
  gsl_vec->block = NULL;
  gsl_vec->owner = false;
  return true;
}

bool taMath_double::mat_add(double_Matrix* a, const double_Matrix* b) {
  gsl_matrix g_a;  if(!mat_get_gsl_fm_ta(&g_a, a)) return false;
  gsl_matrix g_b;  if(!mat_get_gsl_fm_ta(&g_b, b)) return false;
  return gsl_matrix_add(&g_a, &g_b);
}

bool taMath_double::mat_sub(double_Matrix* a, const double_Matrix* b) {
  gsl_matrix g_a;  if(!mat_get_gsl_fm_ta(&g_a, a)) return false;
  gsl_matrix g_b;  if(!mat_get_gsl_fm_ta(&g_b, b)) return false;
  return gsl_matrix_sub(&g_a, &g_b);
}

bool taMath_double::mat_mult_els(double_Matrix* a, const double_Matrix* b) {
  gsl_matrix g_a;  if(!mat_get_gsl_fm_ta(&g_a, a)) return false;
  gsl_matrix g_b;  if(!mat_get_gsl_fm_ta(&g_b, b)) return false;
  return gsl_matrix_mul_elements(&g_a, &g_b);
}

bool taMath_double::mat_div_els(double_Matrix* a, const double_Matrix* b) {
  gsl_matrix g_a;  if(!mat_get_gsl_fm_ta(&g_a, a)) return false;
  gsl_matrix g_b;  if(!mat_get_gsl_fm_ta(&g_b, b)) return false;
  return gsl_matrix_div_elements(&g_a, &g_b);
}

bool taMath_double::mat_transpose(double_Matrix* dest, const double_Matrix* src) {
  if(!dest || !src){taMisc::Error("dest or src cannot be null. try dest=new double_Matrix");return false;}
  if(src->InheritsFrom(&TA_complex_Matrix) && dest->InheritsFrom(&TA_complex_Matrix)) {
    if(src->dims() != 3){taMisc::Error("Can only transpose a 2d matrix");return false;}
    int d0 = src->dim(1);
    int d1 = src->dim(2);
    dest->SetGeom(3,2,d1,d0);
    for(int i=0;i<d0;i++) {
      for(int j=0;j<d1;j++) {
	dest->FastEl(0,j,i) = src->FastEl(0,i,j);
	dest->FastEl(1,j,i) = src->FastEl(1,i,j);
      }
    }
  }
  else {
    if(src->dims() != 2){taMisc::Error("Can only transpose a 2d matrix");return false;}
    int d0 = src->dim(0);
    int d1 = src->dim(1);
    dest->SetGeom(2,d1,d0);
    for(int i=0;i<d0;i++) {
      for(int j=0;j<d1;j++) {
	dest->Set(src->FastElAsDouble(i,j),j,i);
      }
    }
  }
  return true;
}

bool taMath_double::mat_mult(double_Matrix* c, const double_Matrix* a, const double_Matrix* b) {
  gsl_matrix g_a;  if(!mat_get_gsl_fm_ta(&g_a, a)) return false;
  gsl_matrix g_b;  if(!mat_get_gsl_fm_ta(&g_b, b)) return false;
  if(!vec_check_type(c)) return false;
  // ensure return matrix is correct size
  c->SetGeom(2, b->dim(0), a->dim(1));
  gsl_matrix g_c;  if(!mat_get_gsl_fm_ta(&g_c, c)) return false;
  int rval = gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, &g_a, &g_b, 0.0, &g_c);
  return true;			// todo: decode rvals
}

double taMath_double::mat_det(const double_Matrix* a) {
  if(a->dims() != 2) {
    taMisc::Error("mat_det: matrix is not 2 dimensional!");
    return false;
  }
  if(a->dim(0) != a->dim(1)) {
    taMisc::Error("mat_det: matrix is not square!");
    return false;
  }
  double_Matrix tmp(*a);	// copy constructor
  gsl_matrix g_a;  if(!mat_get_gsl_fm_ta(&g_a, &tmp)) return false;
  int s;
  gsl_permutation* p = gsl_permutation_alloc(a->dim(0));
  gsl_linalg_LU_decomp(&g_a, p, &s);
  double det = gsl_linalg_LU_det(&g_a, s);
  gsl_permutation_free(p);
  return det;
}

bool taMath_double::mat_vec_product(const double_Matrix* A, const double_Matrix* x,
                                    double_Matrix* y) {
  if (!A||!x||!y) {
    taMisc::Error("mat_vec_product - you must specify matrix A and vectors x,y.");
    return false;
  }
  if (x->dims()!=1||y->dims()!=1) {
    taMisc::Error("mat_vec_product - x and y must be one dimensional vectors");
    return false;
  }
  if (A->dims()!=2) {
    taMisc::Error("mat_vec_product - A must be a 2 dimensional matrix");
    return false;
  }

  gsl_matrix A_gsl;
  mat_get_gsl_fm_ta(&A_gsl, A);
  const gsl_matrix* A_gsl_ptr = &A_gsl;

  gsl_vector x_gsl;
  vec_get_gsl_fm_ta(&x_gsl, x);
  const gsl_vector* x_gsl_ptr = &x_gsl;

  gsl_vector y_gsl;
  vec_get_gsl_fm_ta(&y_gsl, y);
  gsl_vector* y_gsl_ptr = &y_gsl;

  gsl_blas_dgemv(CblasNoTrans, 1.0, A_gsl_ptr, x_gsl_ptr, 0.0, y_gsl_ptr);

  return true;
}

bool taMath_double::mat_eigen_owrite(double_Matrix* a, double_Matrix* eigen_vals,
                                     double_Matrix* eigen_vecs) {
  if(a->dims() != 2) {
    taMisc::Error("mat_eigen: matrix is not 2 dimensional!");
    return false;
  }
  if(a->dim(0) != a->dim(1)) {
    taMisc::Error("mat_eigen: matrix is not square!");
    return false;
  }
  gsl_matrix g_a;  mat_get_gsl_fm_ta(&g_a, a);
  int n = a->dim(0);
  eigen_vals->SetGeom(1, n);
  eigen_vecs->SetGeom(2, n, n);
  gsl_matrix g_evec;  mat_get_gsl_fm_ta(&g_evec, eigen_vecs);
  gsl_vector g_eval;  vec_get_gsl_fm_ta(&g_eval, eigen_vals);
  gsl_eigen_symmv_workspace* w = gsl_eigen_symmv_alloc(n);
  gsl_eigen_symmv(&g_a, &g_eval, &g_evec, w);
  gsl_eigen_symmv_free(w);
  gsl_eigen_symmv_sort(&g_eval, &g_evec, GSL_EIGEN_SORT_ABS_DESC);
  return true;
}

bool taMath_double::mat_eigen(const double_Matrix* a, double_Matrix* eigen_vals,
                                    double_Matrix* eigen_vecs) {
  double_Matrix a_copy(false);
  a_copy = *a;
  return mat_eigen_owrite(&a_copy, eigen_vals, eigen_vecs);
}

bool taMath_double::mat_svd_owrite(double_Matrix* a, double_Matrix* s, double_Matrix* v) {
  if(a->dims() != 2) {
    taMisc::Error("mat_svd: matrix is not 2 dimensional!");
    return false;
  }
  gsl_matrix g_a;  mat_get_gsl_fm_ta(&g_a, a);
  int n = a->dim(1);
  s->SetGeom(1, n);
  v->SetGeom(2, n, n);
  gsl_matrix g_v;  mat_get_gsl_fm_ta(&g_v, v);
  gsl_vector g_s;  vec_get_gsl_fm_ta(&g_s, s);
  gsl_vector* w = gsl_vector_alloc(n);
  gsl_linalg_SV_decomp(&g_a, &g_v, &g_s, w);
  gsl_vector_free(w);
  return true;
}

bool taMath_double::mat_svd(const double_Matrix* a, double_Matrix* u, double_Matrix* s, double_Matrix* v) {
  *u = *a;                      // u is copy of a
  return mat_svd_owrite(u, s, v);
}


bool taMath_double::mat_mds_owrite(double_Matrix* a, double_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  if(a->dims() != 2) {
    taMisc::Error("*** mat_mds: matrix is not 2 dimensional!");
    return false;
  }
  if(a->dim(0) != a->dim(1)) {
    taMisc::Error("*** mat_mds: matrix is not square!");
    return false;
  }
  int dim = a->dim(0);

  if((x_axis_c < 0) || (x_axis_c >= dim)) {
    taMisc::Error("*** mat_mds: x_axis component must be between 0 and",String(dim-1));
    return false;
  }
  if((y_axis_c < 0) || (y_axis_c >= dim)) {
    taMisc::Error("*** mat_mds: y_axis component must be between 0 and",String(dim-1));
    return false;
  }

  // first square the individual elements
  mat_mult_els(a, a);

  // then double-center the matrix
  for(int i=0; i<dim; i++) {
    double sum = 0.0;
    for(int j=0; j<dim; j++)
      sum += a->FastEl(i, j);
    sum /= (double)dim;
    for(int j=0; j<dim; j++)
      a->FastEl(i, j) -= sum;
  }
  for(int j=0; j<dim; j++) {
    double sum = 0.0;
    for(int i=0; i<dim; i++)
      sum += a->FastEl(i, j);
    sum /= (double)dim;
    for(int i=0; i<dim; i++)
      a->FastEl(i, j) -= sum;
  }

  for(int i=0;i<a->size;i++) a->FastEl_Flat(i) *= -.5;

  double_Matrix evals(false);
  double_Matrix evecs(false);
  mat_eigen_owrite(a, &evals, &evecs);

  xy_coords->SetGeom(2, 2, dim);
  // multiply the eigenvectors by sqrt(eigen values)
  double evsq = taMath_double::sqrt(taMath_double::fabs(evals.FastEl(x_axis_c)));
  for(int i=0;i<dim;i++) {
    float val = evecs.FastEl(i, x_axis_c) * evsq;
    xy_coords->FastEl(0, i) = val;      // 0 = x coord
  }

  evsq = taMath_double::sqrt(taMath_double::fabs(evals.FastEl(y_axis_c)));
  for(int i=0;i<dim;i++) {
    float val = evecs.FastEl(i, y_axis_c) * evsq;
    xy_coords->FastEl(1, i) = val;      // 1 = y coord
  }

  return true;
}

bool taMath_double::mat_mds(const double_Matrix* a, double_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  double_Matrix a_copy(false);
  a_copy = *a;
  return mat_mds_owrite(&a_copy, xy_coords, x_axis_c, y_axis_c);
}

bool taMath_double::mat_slice(double_Matrix* dest, double_Matrix* src, int d0_start, int d0_end, int d1_start, int d1_end) {
  if(!dest || !src){taMisc::Error("dest or src cannot be null. try dest=new double_Matrix");return false;}
  if(src->dims() < 2) {taMisc::Error("Can only slice from a 2d matrix or the first 2 dims of a higher matrix"); return false;}
  int d0_src, d1_src, d0_dest, d1_dest, i, j, k, l;
  d0_src = src->dim(0);
  d1_src = src->dim(1);
  if(d0_end == -1) d0_end = d0_src-1;
  if(d1_end == -1) d1_end = d1_src-1;
  if(d0_start > d0_src || d0_end > d0_src || d1_start > d1_src || d1_end > d1_src){
    taMisc::Error("One of d0_start/d0_end/d1_start/d1_end is greater than the src dims"); return false;}
  d0_dest = d0_end-d0_start+1 > 0 ? d0_end-d0_start+1 : 1;
  d1_dest = d1_end-d1_start+1 > 0 ? d1_end-d1_start+1 : 1;
  dest->SetGeom(2,d0_dest,d1_dest);
  for (i = d0_start,k = 0; i <= d0_end; i++, k++)
    for (j = d1_start,l = 0; j <= d1_end; j++, l++)
      dest->Set(src->FastElAsDouble(i,j),k,l);
  return true;
}

bool taMath_double::mat_trim(double_Matrix* dest, double_Matrix* src, Relation& thresh, int intol_within, int intol_between,
                            bool left, bool right, bool top, bool bottom) {

  if(!dest || !src){taMisc::Error("dest or src cannot be null. try dest=new double_Matrix");return false;}
  if(src->dims() != 2) {taMisc::Error("Can only trim a 2d matrix"); return false;}

  int trim_left = 0, trim_right = 0, trim_top = 0, trim_bottom = 0, tol_cnt, n;
  double_Matrix* mat = new double_Matrix;

  int d0 = src->dim(0);
  int d1 = src->dim(1);

  if(left) {
    tol_cnt = 0;
    for(int i = 0;i < d0;i++) {
      mat_slice(mat,src,i,i);
      n = (int)vec_count(mat, thresh);
      if(n >= d1 - intol_within) {
        trim_left++;
        trim_left += tol_cnt;
        tol_cnt = 0;
      }
      else if ((bool)intol_between && tol_cnt < intol_between) tol_cnt++;
      else break;
    }
    if(trim_left == d0) return false;
  }
  if(right) {
    tol_cnt = 0;
    for(int i = d0-1;i >= 0;i--) {
      mat_slice(mat,src,i,i);
      n = (int)vec_count(mat, thresh);
      if(n >= d1 - intol_within) {
        trim_right++;
        trim_right += tol_cnt;
        tol_cnt = 0;
      }
      else if ((bool)intol_between && tol_cnt < intol_between) tol_cnt++;
      else break;
    }
    if(trim_right == d0) return false;
  }
  if(top) {
    tol_cnt = 0;
    for(int i = 0;i < d1;i++) {
      mat_slice(mat,src,0,-1,i,i);
      n = (int)vec_count(mat, thresh);
      if(n >= d0 - intol_within) {
        trim_top++;
        trim_top += tol_cnt;
        tol_cnt = 0;
      }
      else if ((bool)intol_between && tol_cnt < intol_between) tol_cnt++;
      else break;
    }
    if(trim_top == d1) return false;
  }
  if(bottom) {
    tol_cnt = 0;
    for(int i = d1-1;i >= 0;i--) {
      mat_slice(mat,src,0,-1,i,i);
      n = (int)vec_count(mat, thresh);
      if(n >= d0 - intol_within) {
        trim_bottom++;
        trim_bottom += tol_cnt;
        tol_cnt = 0;
      }
      else if ((bool)intol_between && tol_cnt < intol_between) tol_cnt++;
      else break;
    }
    if(trim_bottom == d1) return false;
  }
  mat_slice(dest,src,trim_left,d0-1-trim_right,trim_top,d1-1-trim_bottom);
  delete mat;
  return true;
}

bool taMath_double::fft_real(complex_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!vec_check_type(out_mat) || !vec_check_type(in_mat)) return false;
  // just as efficient given all the memcopy etc in the real routine to copy to complex
  out_mat->SetReal(*in_mat, true); // sets geom
  out_mat->SetImagAll(0.0);	   // clear out imaginary numbers just to be safe
  return fft_complex(out_mat);
}

bool taMath_double::fft_complex(complex_Matrix* mat) {
  if(!vec_check_type(mat)) return false;
  if(!complex_Matrix::CheckComplexGeom(mat->geom)) {
    return false;
  }

  int n = mat->dim(1); // inner guy is frame size
  if (n == 0) return false; // huh?
  // calculate the number of flat d0 frames, often just 1
  int frames = mat->size / (2 * n);

  // make scratch -- declare all, so we can jump to exit
  bool rval = false;
  gsl_fft_complex_wavetable* gwt = NULL;
  gsl_fft_complex_workspace* work = NULL;
  const size_t stride = 1;

  gwt = gsl_fft_complex_wavetable_alloc((size_t)n);
  if (!gwt) goto exit; // unlikely
  work = gsl_fft_complex_workspace_alloc((size_t) n);
  if (!work) goto exit; // unlikely

  for(int fr = 0; fr < frames; ++fr) {
    int gsl_errno = gsl_fft_complex_forward((double*)mat->FastEl_Flat_(fr * n * 2),
					    stride, (size_t) n, gwt, work);
    if (gsl_errno != 0) {
      taMisc::Warning("taMath_double::fft_complex", gsl_strerror(gsl_errno));
      goto exit;
    }
  }

  rval = true;
exit:
  // cleanup
  if (work) gsl_fft_complex_workspace_free(work);
  if (gwt) gsl_fft_complex_wavetable_free(gwt);
  return rval;
}

bool taMath_double::ffti_complex(complex_Matrix* mat) {
  if(!vec_check_type(mat)) return false;
  if(!complex_Matrix::CheckComplexGeom(mat->geom)) {
    return false;
  }

  int n = mat->dim(1); // inner guy is frame size
  if (n == 0) return false; // huh?
  // calculate the number of flat d0 frames, often just 1
  int frames = mat->size / (2 * n);

  // make scratch -- declare all, so we can jump to exit
  bool rval = false;
  gsl_fft_complex_wavetable* gwt = NULL;
  gsl_fft_complex_workspace* work = NULL;
  const size_t stride = 1;

  gwt = gsl_fft_complex_wavetable_alloc((size_t)n);
  if (!gwt) goto exit; // unlikely
  work = gsl_fft_complex_workspace_alloc((size_t) n);
  if (!work) goto exit; // unlikely

  for (int fr = 0; fr < frames; ++fr) {
    int gsl_errno = gsl_fft_complex_inverse((double*)mat->FastEl_Flat_(fr * n * 2),
					    stride, (size_t) n, gwt, work);
    if (gsl_errno != 0) {
      taMisc::Warning("taMath_double::ffti_complex", gsl_strerror(gsl_errno));
      goto exit;
    }
  }

  rval = true;
exit:
  // cleanup
  if (work) gsl_fft_complex_workspace_free(work);
  if (gwt) gsl_fft_complex_wavetable_free(gwt);
  return rval;
}

// fft 2d
bool taMath_double::fft2_real(complex_Matrix* out_mat,  const double_Matrix* in_mat) {
  if(!vec_check_type(out_mat) || !vec_check_type(in_mat)) return false;
  if(in_mat->dims() < 2) {
    taMisc::Error("fft2_real: input matrix is not 2d -- must have at least 2 dims");
    return false;
  }

  // just as efficient given all the memcopy etc in the real routine to copy to complex
  out_mat->SetReal(*in_mat, true); // sets geom
  out_mat->SetImagAll(0.0);	   // clear out imaginary numbers just to be safe
  return fft2_complex(out_mat);
}

bool taMath_double::fft2_complex(complex_Matrix* mat) {
  if(!vec_check_type(mat)) return false;
  if(mat->dims() < 3 || mat->dim(0) != 2) {
    taMisc::Error("fft2_complex: input matrix is not complex 2d -- must have inner dim of size 2 which is real, imag numbers, and at least 2 other dims");
    return false;
  }

  int cols = mat->dim(1);
  int rows = mat->dim(2);
  int frames = mat->size / (rows * cols * 2);

  int gsl_errno = 0;
  int rc2 = rows * cols * 2;
  int c2 = cols * 2;

  // make scratch -- declare all, so we can jump to exit
  bool rval = false;
  gsl_fft_complex_wavetable* gwt_rows = NULL;
  gsl_fft_complex_workspace* work_rows = NULL;
  gsl_fft_complex_wavetable* gwt_cols = NULL;
  gsl_fft_complex_workspace* work_cols = NULL;
  const size_t stride = 1;

  gwt_cols = gsl_fft_complex_wavetable_alloc((size_t)cols);
  if (!gwt_cols) goto exit; // unlikely
  work_cols = gsl_fft_complex_workspace_alloc((size_t)cols);
  if (!work_cols) goto exit; // unlikely

  if(rows != cols) {
    gwt_rows = gsl_fft_complex_wavetable_alloc((size_t)rows);
    if (!gwt_rows) goto exit; // unlikely
    work_rows = gsl_fft_complex_workspace_alloc((size_t)rows);
    if (!work_rows) goto exit; // unlikely
  }
  else {
    gwt_rows = gwt_cols;	// can reuse
    work_rows = work_cols;
  }

  for (int fr = 0; fr < frames; ++fr) {
    // first do rows
    for (int row = 0; row < rows; ++row) {
      gsl_errno = gsl_fft_complex_forward((double*)mat->FastEl_Flat_(fr * rc2 + row * c2),
					  stride, (size_t)cols, gwt_cols, work_cols);
      if (gsl_errno != 0) {
	taMisc::Warning("taMath_double::fft2_complex", gsl_strerror(gsl_errno));
	goto exit;
      }
    }

    // then cols -- stride is cols -- might need to be cols*2??
    for (int col = 0; col < cols; ++col) {
      gsl_errno = gsl_fft_complex_forward((double*)mat->FastEl_Flat_(fr * rc2 + col * 2),
				  (size_t)cols, (size_t)rows, gwt_rows, work_rows);
      if (gsl_errno != 0) {
	taMisc::Warning("taMath_double::fft2_complex", gsl_strerror(gsl_errno));
	goto exit;
      }
    }
  }
  rval = true;
exit:
  // cleanup
  if (work_cols) gsl_fft_complex_workspace_free(work_cols);
  if (gwt_cols) gsl_fft_complex_wavetable_free(gwt_cols);
  if(rows != cols) {
    if (work_rows) gsl_fft_complex_workspace_free(work_rows);
    if (gwt_rows) gsl_fft_complex_wavetable_free(gwt_rows);
  }
  return rval;
}

bool taMath_double::ffti2_complex(complex_Matrix* mat) {
  if(!vec_check_type(mat)) return false;
  if(mat->dims() < 3 || mat->dim(0) != 2) {
    taMisc::Error("ffti2_complex: input matrix is not complex 2d -- must have inner dim of size 2 which is real, imag numbers, and at least 2 other dims");
    return false;
  }

  int cols = mat->dim(1);
  int rows = mat->dim(2);
  int frames = mat->size / (rows * cols * 2);

  int gsl_errno = 0;
  int rc2 = rows * cols * 2;
  int c2 = cols * 2;

  // make scratch -- declare all, so we can jump to exit
  bool rval = false;
  gsl_fft_complex_wavetable* gwt_rows = NULL;
  gsl_fft_complex_workspace* work_rows = NULL;
  gsl_fft_complex_wavetable* gwt_cols = NULL;
  gsl_fft_complex_workspace* work_cols = NULL;
  const size_t stride = 1;

  gwt_cols = gsl_fft_complex_wavetable_alloc((size_t)cols);
  if (!gwt_cols) goto exit; // unlikely
  work_cols = gsl_fft_complex_workspace_alloc((size_t)cols);
  if (!work_cols) goto exit; // unlikely

  if(rows != cols) {
    gwt_rows = gsl_fft_complex_wavetable_alloc((size_t)rows);
    if (!gwt_rows) goto exit; // unlikely
    work_rows = gsl_fft_complex_workspace_alloc((size_t)rows);
    if (!work_rows) goto exit; // unlikely
  }
  else {
    gwt_rows = gwt_cols;	// can reuse
    work_rows = work_cols;
  }

  for (int fr = 0; fr < frames; ++fr) {
    // cols first for inverse -- stride is cols -- might need to be cols*2??
    for (int col = 0; col < cols; ++col) {
      gsl_errno = gsl_fft_complex_inverse((double*)mat->FastEl_Flat_(fr * rc2 + col * 2),
				  (size_t)cols, (size_t)rows, gwt_rows, work_rows);
      if (gsl_errno != 0) {
	taMisc::Warning("taMath_double::ffti2_complex", gsl_strerror(gsl_errno));
	goto exit;
      }
    }
    // then do rows
    for (int row = 0; row < rows; ++row) {
      gsl_errno = gsl_fft_complex_inverse((double*)mat->FastEl_Flat_(fr * rc2 + row * c2),
					  stride, (size_t)cols, gwt_cols, work_cols);
      if (gsl_errno != 0) {
	taMisc::Warning("taMath_double::ffti2_complex", gsl_strerror(gsl_errno));
	goto exit;
      }
    }

  }
  rval = true;
exit:
  // cleanup
  if (work_cols) gsl_fft_complex_workspace_free(work_cols);
  if (gwt_cols) gsl_fft_complex_wavetable_free(gwt_cols);
  if(rows != cols) {
    if (work_rows) gsl_fft_complex_workspace_free(work_rows);
    if (gwt_rows) gsl_fft_complex_wavetable_free(gwt_rows);
  }
  return rval;
}


//////////////////////////////////////////////////////////////////////////
#else // !HAVE_LIBGSL

bool taMath_double::mat_add(double_Matrix* a, const double_Matrix* b) {
  return false;
}

bool taMath_double::mat_sub(double_Matrix* a, const double_Matrix* b) {
  return false;
}

bool taMath_double::mat_mult_els(double_Matrix* a, const double_Matrix* b) {
  return false;
}

bool taMath_double::mat_div_els(double_Matrix* a, const double_Matrix* b) {
  return false;
}

bool taMath_double::mat_eigen_owrite(double_Matrix* a, double_Matrix* eigen_vals,
                                     double_Matrix* eigen_vecs) {
  return false;
}

bool taMath_double::mat_eigen(const double_Matrix* a, double_Matrix* eigen_vals,
                                    double_Matrix* eigen_vecs) {
  return false;
}

bool taMath_double::mat_svd_owrite(double_Matrix* a, double_Matrix* s, double_Matrix* v) {
  return false;
}

bool taMath_double::mat_svd(const double_Matrix* a, double_Matrix* u, double_Matrix* s, double_Matrix* v) {
  return false;
}


bool taMath_double::mat_mds_owrite(double_Matrix* a, double_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  return false;
}

bool taMath_double::mat_mds(const double_Matrix* a, double_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  return false;
}

bool taMath_double::fft_real(complex_Matrix* out_mat, const double_Matrix* in_mat) {
  return false;
}

bool taMath_double::fft_complex(complex_Matrix* mat) {
  return false;
}

bool taMath_double::ffti_complex(complex_Matrix* mat) {
  return false;
}

bool taMath_double::fft2_real(complex_Matrix* out_mat, const double_Matrix* in_mat) {
  return false;
}

bool taMath_double::fft2_complex(complex_Matrix* mat) {
  return false;
}

bool taMath_double::ffti2_complex(complex_Matrix* mat) {
  return false;
}

#endif  // HAVE_LIBGSL

bool taMath_double::mat_dist(double_Matrix* dist_mat, const double_Matrix* src_mat,
                             DistMetric metric, bool norm, double tolerance) {
  if(!vec_check_type(dist_mat) || !vec_check_type(src_mat)) return false;
  if(src_mat->dims() < 2) {
    taMisc::Error("*** mat_dist: matrix is < 2 dimensional!");
    return false;
  }
  int n = src_mat->Frames();
  dist_mat->SetGeom(2, n, n);
  for(int i=0;i<n;i++) {
    double_Matrix* t1 = ((double_Matrix*)src_mat)->GetFrameSlice(i);
    taBase::Ref(t1);
    for(int j=0;j<n;j++) {
      double_Matrix* t2 = ((double_Matrix*)src_mat)->GetFrameSlice(j);
      taBase::Ref(t2);
      double dist = vec_dist(t1, t2, metric, norm, tolerance);
      dist_mat->FastEl(j,i) = dist;
      taBase::unRefDone(t2);
    }
    taBase::unRefDone(t1);
  }
  return true;
}

bool taMath_double::mat_cross_dist(double_Matrix* dist_mat, const double_Matrix* src_mat_a,
                                   const double_Matrix* src_mat_b,
                                   DistMetric metric, bool norm, double tolerance) {
  if(!vec_check_type(dist_mat) || !vec_check_type(src_mat_a) || !vec_check_type(src_mat_b))
    return false;
  if(src_mat_a->dims() < 2) {
    taMisc::Error("*** mat_cross_dist: matrix a is < 2 dimensional!");
    return false;
  }
  if(src_mat_b->dims() < 2) {
    taMisc::Error("*** mat_cross_dist: matrix b is < 2 dimensional!");
    return false;
  }
  if(src_mat_a->FrameSize() != src_mat_b->FrameSize()) {
    taMisc::Error("*** mat_cross_dist: matrix a and b do not have the same framesize!");
    return false;
  }
  int n_rows = src_mat_a->Frames();
  int n_cols = src_mat_b->Frames();
  dist_mat->SetGeom(2, n_cols, n_rows);
  for(int i=0;i<n_rows;i++) {
    double_Matrix* t1 = ((double_Matrix*)src_mat_a)->GetFrameSlice(i);
    taBase::Ref(t1);
    for(int j=0;j<n_cols;j++) {
      double_Matrix* t2 = ((double_Matrix*)src_mat_b)->GetFrameSlice(j);
      taBase::Ref(t2);
      double dist = vec_dist(t1, t2, metric, norm, tolerance);
      dist_mat->FastEl(j,i) = dist;
      taBase::unRefDone(t2);
    }
    taBase::unRefDone(t1);
  }
  return true;
}

bool taMath_double::mat_cell_to_vec(double_Matrix* vec, const double_Matrix* mat, int cell_no) {
  if(!vec_check_type(vec) || !vec_check_type(mat)) return false;
  if(cell_no >= mat->FrameSize()) {
    taMisc::Error("mat_cell_to_vec: cell no:", String(cell_no), "is larger than framesize:",
                  String(mat->FrameSize()));
    return false;
  }
  int n = mat->Frames();
  vec->SetGeom(1, n);
  for(int i=0;i<n; i++) {
    vec->FastEl(i) = mat->FastEl_Flat(mat->FrameStartIdx(i) + cell_no);
  }
  return true;
}

bool taMath_double::mat_correl(double_Matrix* correl_mat, const double_Matrix* src_mat) {
  if(!vec_check_type(correl_mat) || !vec_check_type(src_mat)) return false;
  if(src_mat->dims() < 2) {
    taMisc::Error("*** mat_correl: src_matrix is < 2 dimensional!");
    return false;
  }

  int n = src_mat->FrameSize();
  correl_mat->SetGeom(2, n, n);

  double_Matrix p1vals(false);
  double_Matrix p2vals(false);

  for(int i=0;i<n;i++) {
    mat_cell_to_vec(&p1vals, src_mat, i);
    for(int j=0;j<n;j++) {
      mat_cell_to_vec(&p2vals, src_mat, j);
      correl_mat->FastEl(j,i) = vec_correl(&p1vals, &p2vals);
    }
  }
  return true;
}

bool taMath_double::mat_prjn(double_Matrix* prjn_vec, const double_Matrix* src_mat,
                             const double_Matrix* prjn_mat, DistMetric metric,
                             bool norm, double tol) {
  if(!vec_check_type(prjn_vec) || !vec_check_type(src_mat) || !vec_check_type(prjn_mat))
    return false;
  if(src_mat->dims() < 2) {
    taMisc::Error("*** mat_prjn: source matrix is < 2 dimensional!");
    return false;
  }
  if(src_mat->FrameSize() != prjn_mat->count()) {
    taMisc::Error("*** mat_prjn: source matrix frame and projection matrix don't have the same size");
    return false;
  }
  int n_rows = src_mat->Frames();
  prjn_vec->SetGeom(1, n_rows);
  for(int i=0;i<n_rows;i++) {
    double_Matrix* t1 = ((double_Matrix*)src_mat)->GetFrameSlice(i);
    taBase::Ref(t1);
    double val = vec_dist(t1, prjn_mat, metric, norm, tol);
    prjn_vec->FastEl(i) = val;
    taBase::unRefDone(t1);
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// frame-compatible versions of various functions

bool taMath_double::mat_time_avg(double_Matrix* a, double avg_dt) {
  if(!vec_check_type(a)) return false;
  if(avg_dt < 0.0 || avg_dt > 1.0) {
    taMisc::Error("mat_time_avg: avg_dt is not in 0..1 range:", String(avg_dt));
    return false;
  }
  double avg_dt_c = 1.0 - avg_dt;
  for(int i=1;i<a->Frames();i++) {
    int fr_idx = a->FrameStartIdx(i);
    int frm1_idx = a->FrameStartIdx(i-1);
    for(int j=0;j<a->FrameSize();j++) {
      a->FastEl_Flat(fr_idx + j) = avg_dt * a->FastEl_Flat(frm1_idx + j) + avg_dt_c * a->FastEl_Flat(fr_idx + j);
    }
  }
  return true;
}

bool taMath_double::mat_frame_convolve(double_Matrix* out_vec, const double_Matrix* in_vec,
                                       const double_Matrix* kernel) {
  if(!vec_check_type(kernel) || !vec_check_type(in_vec) || !vec_check_type(out_vec))
    return false;
  if(kernel->size == 0) {
    taMisc::Error("mat_frame_convolve: kernel size == 0");
    return false;
  }
  int off = (kernel->size-1) / 2;
  if(out_vec->size != in_vec->size) {
    out_vec->SetGeomN(in_vec->geom); // must be same
  }
  for(int i=0;i<in_vec->Frames();i++) {
    for(int k=0;k<in_vec->FrameSize();k++) {
      float sum = 0.0;
      float dnorm = 0.0;
      for(int j=0;j<kernel->size;j++) {
        int idx = i + j - off;
        if(idx < 0 || idx >= in_vec->Frames()) {
          dnorm += kernel->FastEl(j);
        }
        else {
          sum += in_vec->FastEl_Flat(in_vec->FrameStartIdx(idx) + k) * kernel->FastEl(j);
        }
      }
      if(dnorm > 0.0 && dnorm < 1.0) { // renorm
        sum /= (1.0 - dnorm);
      }
      out_vec->FastEl_Flat(out_vec->FrameStartIdx(i) + k) = sum;
    }
  }
  return true;
}

bool taMath_double::mat_fmt_out_frame(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!vec_check_type(in_mat) || !vec_check_type(out_mat)) return false;
  MatrixGeom frg = in_mat->geom;
  if(frg.dims() == 1 || in_mat->Frames() == 0) return false;
  frg.SetDims(frg.dims()-1);    // nuke last dim
  out_mat->SetGeomN(frg);
  return true;
}

bool taMath_double::mat_frame_set_n(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  double frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = frn;
  }
  return true;
}

bool taMath_double::mat_frame_first(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = in_mat->FastEl_Flat(i);
  }
  return true;
}

bool taMath_double::mat_frame_last(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  int off = (frn-1) * frs;
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = in_mat->FastEl_Flat(off + i);
  }
  return true;
}

bool taMath_double::mat_frame_find_first(double_Matrix* out_mat, const double_Matrix* in_mat,
                                         Relation& rel) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    int j;
    for(j=0;j<frn;j++) {
      if(tmp_rel.Evaluate(in_mat->FastEl_Flat(j * frs + i))) break;
    }
    if(j == frn) j = -1;
    out_mat->FastEl_Flat(i) = j;
  }
  return true;
}

bool taMath_double::mat_frame_find_last(double_Matrix* out_mat, const double_Matrix* in_mat,
                                         Relation& rel) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    int j;
    for(j=frn-1;j>=0;j--) {
      if(tmp_rel.Evaluate(in_mat->FastEl_Flat(j * frs + i))) break;
    }
    if(j < 0) j = -1;
    out_mat->FastEl_Flat(i) = j;
  }
  return true;
}

bool taMath_double::mat_frame_max(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    double mx = in_mat->FastEl_Flat(i);
    for(int j=1;j<frn;j++) {
      mx = MAX(in_mat->FastEl_Flat(j * frs + i), mx);
    }
    out_mat->FastEl_Flat(i) = mx;
  }
  return true;
}

bool taMath_double::mat_frame_abs_max(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    double mx = fabs(in_mat->FastEl_Flat(i));
    for(int j=1;j<frn;j++) {
      mx = MAX(fabs(in_mat->FastEl_Flat(j * frs + i)), mx);
    }
    out_mat->FastEl_Flat(i) = mx;
  }
  return true;
}

bool taMath_double::mat_frame_min(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    double mx = in_mat->FastEl_Flat(i);
    for(int j=1;j<frn;j++) {
      mx = MIN(in_mat->FastEl_Flat(j * frs + i), mx);
    }
    out_mat->FastEl_Flat(i) = mx;
  }
  return true;
}

bool taMath_double::mat_frame_abs_min(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    double mx = fabs(in_mat->FastEl_Flat(i));
    for(int j=1;j<frn;j++) {
      mx = MIN(fabs(in_mat->FastEl_Flat(j * frs + i)), mx);
    }
    out_mat->FastEl_Flat(i) = mx;
  }
  return true;
}

bool taMath_double::mat_frame_sum(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    double sum = 0.0f;
    for(int j=0;j<frn;j++) {
      sum += in_mat->FastEl_Flat(j * frs + i);
    }
    out_mat->FastEl_Flat(i) = sum;
  }
  return true;
}

bool taMath_double::mat_frame_prod(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    double prod = in_mat->FastEl_Flat(i);
    for(int j=1;j<frn;j++) {
      prod *= in_mat->FastEl_Flat(j * frs + i);
    }
    out_mat->FastEl_Flat(i) = prod;
  }
  return true;
}

bool taMath_double::mat_frame_mean(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_frame_sum(out_mat, in_mat)) return false;
  double nrm = 1.0 / (double)in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) *= nrm;
  }
  return true;
}

bool taMath_double::mat_frame_var(double_Matrix* out_mat, const double_Matrix* in_mat, bool use_est) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  double nrm = 1.0 / ((double)frn - (double)use_est);
  for(int i=0;i<frs;i++) {
    double mean = 0.0f;
    for(int j=0;j<frn;j++) {
      mean += in_mat->FastEl_Flat(j * frs + i);
    }
    mean /= (double)frn;
    double var = 0.0f;
    for(int j=0;j<frn;j++) {
      double dm = (in_mat->FastEl_Flat(j * frs + i) - mean);
      var += dm * dm;
    }
    out_mat->FastEl_Flat(i) = var * nrm;
  }
  return true;
}

bool taMath_double::mat_frame_std_dev(double_Matrix* out_mat, const double_Matrix* in_mat, bool use_est) {
  if(!mat_frame_var(out_mat, in_mat, use_est)) return false;
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = sqrt(out_mat->FastEl_Flat(i));
  }
  return true;
}

bool taMath_double::mat_frame_sem(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_frame_std_dev(out_mat, in_mat, true)) return false;
  int frs = in_mat->FrameSize();
  double nrm = 1.0 / sqrt((double)in_mat->Frames());
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) *= nrm;
  }
  return true;
}

bool taMath_double::mat_frame_ss_len(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    double sum = 0.0f;
    for(int j=0;j<frn;j++) {
      sum += in_mat->FastEl_Flat(j * frs + i) * in_mat->FastEl_Flat(j * frs + i);
    }
    out_mat->FastEl_Flat(i) = sum;
  }
  return true;
}


bool taMath_double::mat_frame_ss_mean(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    double sumsqr = 0.0f;
    double sum = 0.0f;
    for(int j=0;j<frn;j++) {
      sumsqr += in_mat->FastEl_Flat(j * frs + i) * in_mat->FastEl_Flat(j * frs + i);
      sum += in_mat->FastEl_Flat(j * frs + i);
    }
    out_mat->FastEl_Flat(i) = sumsqr - (sum * sum) / frn;
  }
  return true;
}

bool taMath_double::mat_frame_count(double_Matrix* out_mat, const double_Matrix* in_mat, Relation& rel) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  for(int i=0;i<frs;i++) {
    double sum = 0.0f;
    for(int j=0;j<frn;j++) {
      if(tmp_rel.Evaluate(in_mat->FastEl_Flat(j * frs + i))) sum += 1.0;
    }
    out_mat->FastEl_Flat(i) = sum;
  }
  return true;
}

bool taMath_double::mat_frame_median(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  int idx = frn / 2;
  double_Array tmp;
  tmp.SetSize(frn);
  for(int i=0;i<frs;i++) {
    for(int j=0;j<frn;j++) {
      tmp[j] = in_mat->FastEl_Flat(j * frs + i);
    }
    tmp.Sort();
    out_mat->FastEl_Flat(i) = tmp[idx];
  }
  return true;
}

bool taMath_double::mat_frame_quantile(double_Matrix* out_mat, const double_Matrix* in_mat, double quant_pos) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  double_Array tmp;
  tmp.SetSize(frn);
  int idx = (int)(quant_pos * (double)tmp.size);
  if(idx >= tmp.size) idx = tmp.size-1;
  if(idx < 0) idx = 0;
  for(int i=0;i<frs;i++) {
    for(int j=0;j<frn;j++) {
      tmp[j] = in_mat->FastEl_Flat(j * frs + i);
    }
    tmp.Sort();
    out_mat->FastEl_Flat(i) = tmp[idx];
  }
  return true;
}

bool taMath_double::mat_frame_mode(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->Frames();
  int frs = in_mat->FrameSize();
  double_Array tmp;
  tmp.SetSize(frn);
  for(int i=0;i<frs;i++) {
    for(int j=0;j<frn;j++) {
      tmp[j] = in_mat->FastEl_Flat(j * frs + i);
    }
    tmp.Sort();
    int mx_frq = 0;
    double mode = 0.0f;
    int idx = 0;
    while(idx < tmp.size) {
      double val = tmp[idx];
      int st_idx = idx;
      while((idx < tmp.size-1) && (val == tmp[++idx]));
      int frq = idx - st_idx;
      if(idx == tmp.size-1) {
        if(tmp[tmp.size-1] == val) frq++;
        idx++;
      }
      if(frq > mx_frq) {
        mx_frq = frq;
        mode = val;
      }
    }
    out_mat->FastEl_Flat(i) = mode;
  }
  return true;
}

bool taMath_double::mat_frame_aggregate(double_Matrix* out_mat, const double_Matrix* in_mat,
                                        Aggregate& agg) {
  switch(agg.op) {
  case Aggregate::GROUP:
  case Aggregate::FIRST:
    return taMath_double::mat_frame_first(out_mat, in_mat);
  case Aggregate::LAST:
    return taMath_double::mat_frame_last(out_mat, in_mat);
  case Aggregate::FIND_FIRST:
    return taMath_double::mat_frame_find_first(out_mat, in_mat, agg.rel);
  case Aggregate::FIND_LAST:
    return taMath_double::mat_frame_find_last(out_mat, in_mat, agg.rel);
  case Aggregate::MIN:
    return taMath_double::mat_frame_min(out_mat, in_mat);
  case Aggregate::MAX:
    return taMath_double::mat_frame_max(out_mat, in_mat);
  case Aggregate::ABS_MIN:
    return taMath_double::mat_frame_abs_min(out_mat, in_mat);
  case Aggregate::ABS_MAX:
    return taMath_double::mat_frame_abs_max(out_mat, in_mat);
  case Aggregate::SUM:
    return taMath_double::mat_frame_sum(out_mat, in_mat);
  case Aggregate::PROD:
    return taMath_double::mat_frame_prod(out_mat, in_mat);
  case Aggregate::MEAN:
    return taMath_double::mat_frame_mean(out_mat, in_mat);
  case Aggregate::VAR:
    return taMath_double::mat_frame_var(out_mat, in_mat);
  case Aggregate::SS:
    return taMath_double::mat_frame_ss_mean(out_mat, in_mat);
  case Aggregate::STDEV:
    return taMath_double::mat_frame_std_dev(out_mat, in_mat);
  case Aggregate::SEM:
    return taMath_double::mat_frame_sem(out_mat, in_mat);
  case Aggregate::N:
    return taMath_double::mat_frame_set_n(out_mat, in_mat);
  case Aggregate::COUNT:
    return taMath_double::mat_frame_count(out_mat, in_mat, agg.rel);
  case Aggregate::MEDIAN:
    return taMath_double::mat_frame_median(out_mat, in_mat);
  case Aggregate::MODE:
    return taMath_double::mat_frame_mode(out_mat, in_mat);
  case Aggregate::QUANTILE:
    return taMath_double::mat_frame_quantile(out_mat, in_mat, agg.rel.val);
  case Aggregate::NONE:
    return true;
  }
  return false;
}

