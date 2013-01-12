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

#include "taMath_float.h"
#include <taMath_double>
#include <float_Array>
#include <double_Array>
#include <int_Array>
#include <double_Matrix>
#include <float_Matrix>
#include <int_Matrix>
#include <complex_Matrix>
#include <MTRnd>
#include <Random>
#include <Aggregate>
#include <Relation>
#include <RelationFloat>
#include <SimpleMathSpec>
#include <taBaseItr>

#include <taMisc>

#ifdef _WINDOWS //note: have to use the raw ms switch, since our own file isn't loaded
// note: MSVC requires the following on first read of math.h to enable defines
// we have to put this absolutely first to insure it is defined for the first read
// of math.h, which could occur anywhere in any included file
# define _USE_MATH_DEFINES
#endif

#include "ta_math.h"

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

/////////////////////////////////////////////////////////////////////////////////
// ExpLog: exponential and logarithmic functions

float taMath_float::e = (float)M_E;

float taMath_float::exp_fast(float x) {
  return (float)taMath_double::exp_fast(x);
}

/////////////////////////////////////////////////////////////////////////////////
// Trigonometry

float taMath_float::pi = (float)M_PI;
float taMath_float::two_pi = (float)(2.0f * M_PI);
float taMath_float::oneo_two_pi = (float)(1.0f / (2.0f * M_PI));
float taMath_float::deg_per_rad = (float)(180.0f / M_PI);
float taMath_float::rad_per_deg = (float)(M_PI / 180.0f);

/////////////////////////////////////////////////////////////////////////////////
// Probability distributions, etc

float taMath_float::fact_ln(int n) {
  static float_Array table(false);

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

float taMath_float::fact(int n) {
  return (float)gsl_sf_fact(n);
}

float taMath_float::bico_ln(int n, int j) {
  return fact_ln(n)-fact_ln(j)-fact_ln(n-j);
}

float taMath_float::hyperg(int j, int s, int t, int n) {
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

static float gser(float a, float x) {
  int n;
  float gln,sum,del,ap;

  gln=taMath_float::gamma_ln(a);
  if (x <= 0.0) {
    if (x < 0.0) { fprintf(stderr, "x < 0 in gser()\n"); return 0; }
    return 0;
  }
  else {
    ap=a;
    del=sum=1.0/a;
    for (n=1;n<=taMath_float::max_iterations;n++) {
      ap += 1.0;
      del *= x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*taMath_float::err_tolerance)
        return sum*exp(-x+a*log(x)-(gln));
    }
    fprintf(stderr, "a too large, max_iterations too small in gser()\n");
    return 0;
  }
}

static float gcf(float a, float x) {
  int n;
  float gln;
  float gold=0.0,g,fac=1.0,b1=1.0;
  float b0=0.0,anf,ana,an,a1,a0=1.0;

  gln=taMath_float::gamma_ln(a);
  a1=x;
  for (n=1;n<=taMath_float::max_iterations;n++) {
    an=(float) n;
    ana=an-a;
    a0=(a1+a0*ana)*fac;
    b0=(b1+b0*ana)*fac;
    anf=an*fac;
    a1=x*a0+anf*a1;
    b1=x*b0+anf*b1;
    if (a1) {
      fac=1.0/a1;
      g=b1*fac;
      if (fabs((g-gold)/g) < taMath_float::err_tolerance)
        return exp(-x+a*log(x)-(gln))*g;
      gold=g;
    }
  }
  fprintf(stderr, "a too large, max_iterations too small in gcf()\n");
  return 0;
}

float taMath_float::gamma_ln(float z) {
  float x,tmp,ser;              /* make sure float-precision.. */
  static float cof[6]={ 76.18009173f, -86.50532033f, 24.01409822f,
                         -1.231739516f, 0.120858003e-2f, -0.536382e-5f };
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

float taMath_float::gamma_p(float a, float x) {
  if (x < 0.0 || a <= 0.0) { fprintf(stderr, "Invalid args in gamma_p()\n"); return 0; }

  if (x < (a+1.0))
    return gser(a,x);
  else
    return 1.0 - gcf(a,x);
}

float taMath_float::gamma_q(float a, float x) {
  if (x < 0.0 || a <= 0.0) { fprintf(stderr, "Invalid args in gamma_q()\n"); return 0; }
  if (x < (a+1.0))
    return 1.0 - gser(a,x);
  else
    return gcf(a,x);
}

/**********************************
  beta function
***********************************/

static float betacf_float(float a, float b, float x) {
  float qap,qam,qab,em,tem,d;
  float bz,bm=1.0,bp,bpp;
  float az=1.0,am=1.0,ap,app,aold;
  int m;

  qab=a+b;
  qap=a+1.0;
  qam=a-1.0;
  bz=1.0-qab*x/qap;
  for (m=1;m<=taMath_float::max_iterations;m++) {
    em=(float) m;
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
    if (fabs(az-aold) < (taMath_float::err_tolerance*fabs(az))) return az;
  }
  fprintf(stderr, "a or b too big, or max_iterations too small in betacf()\n");
  return 0;
}

float taMath_float::beta(float z, float w) {
  return exp(gamma_ln(z) + gamma_ln(w) - gamma_ln(z + w));
}

float taMath_float::beta_i(float a, float b, float x) {
  float bt;

  if (x < 0.0 || x > 1.0) { fprintf(stderr, "Bad x in beta_i()\n"); return 0; }
  if (x == 0.0 || x == 1.0) bt=0.0;
  else
    bt=exp(gamma_ln(a+b)-gamma_ln(a)-gamma_ln(b)+a*log(x)+b*log(1.0-x));
  if (x < (a+1.0)/(a+b+2.0))
    return bt*betacf_float(a,b,x)/a;
  else
    return 1.0-bt*betacf_float(b,a,1.0-x)/b;
}


/**********************************
  the binomial distribution
***********************************/

float taMath_float::binom_den(int n, int j, float p) {
  if(j > n) { fprintf(stderr, "j > n in binom()\n"); return 0; }
  return exp(bico_ln(n,j) + (float)j * log(p) + (float)(n-j) * log(1.0 - p));
}

float taMath_float::binom_cum(int n, int k, float p) {
  if(k > n)     { fprintf(stderr, "k > n in binom_cum()\n"); return 0; }
  return beta_i(k, n-k + 1, p);
}

float taMath_float::binom_dev(int n, float pp) {
  int j;
  static int nold=(-1);
  float am,em,g,angle,p,bnl,sq,t,y;
  static float pold=(-1.0),pc,plog,pclog,en,oldg;

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

float taMath_float::poisson_den(int j, float l) {
  return exp((float)j * log(l) - fact_ln(j) - l);
}

float taMath_float::poisson_cum(int j, float x) {
  if(x < 0.0)   { fprintf(stderr, "x < 0 in poisson_cum()\n"); return 0; }
  if(j > 0)
    return gamma_q(j, x);
  else
    return 0;
}

float taMath_float::poisson_dev(float xm) {
  static float sq,alxm,g,oldm=(-1.0);
  float em,t,y;

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

float taMath_float::gamma_den(int j, float l, float t) {
  if(t < 0) return 0;
  return exp((float)j * log(l) + (float)(j-1) * log(t) - gamma_ln(j) - (l * t));
}

float taMath_float::gamma_cum(int j, float l, float t) {
  return gamma_p(j, l * t);
}

float taMath_float::gamma_dev(int ia) {
  int j;
  float am,e,s,v1,v2,x,y;

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
float taMath_float::erf(float x) {
  return (float)taMath_double::erf(x);
}

float taMath_float::erfc(float x) {
  return (float)taMath_double::erfc(x);
}

float taMath_float::gauss_den(float z) {
  return 0.398942280 * exp(-0.5 * z * z);
}

float taMath_float::gauss_den_sig(float x, float sig) {
  x /= sig;
  return 0.398942280 * exp(-0.5 * x * x) / sig;
}

float taMath_float::gauss_den_sq_sig(float x_sq, float sig) {
  return 0.398942280 * exp(-0.5 * x_sq / (sig * sig)) / sig;
}

float taMath_float::gauss_den_nonorm(float x, float sig) {
  x /= sig;
  return exp(-0.5 * x * x);
}

float taMath_float::gauss_cum(float z) {
  float y, x, w;

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

float taMath_float::gauss_inv(float p) {
  float minz = -6.0;
  float maxz = 6.0;
  float zval = 0.0;
  float pval;

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

float taMath_float::gauss_inv_lim(float p) {
  float minz = -6.0;
  float maxz = 6.0;
  float zval = 0.0;
  float pval;

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


float taMath_float::gauss_dev() {
  return static_cast<float>(MTRnd::genrand_gauss_dev_double());
}

/**********************************
  misc statistical
***********************************/

float taMath_float::dprime(float mean_signal, float stdev_signal,
			     float mean_noise, float stdev_noise) {
  float avg_stdev = sqrt(0.5 * stdev_signal * stdev_signal + stdev_noise * stdev_noise);
  if(avg_stdev == 0.0) return 0.0;
  return (mean_signal - mean_noise) / avg_stdev;
}

float taMath_float::hits_fa(float dprime, float& hits, float& false_alarms, float crit_z) {
  dprime = fabs(dprime);	// keep it positive
  if(crit_z < 0.0) crit_z = 0.0;
  if(crit_z > 1.0) crit_z = 1.0;
  hits = gauss_cum(crit_z * dprime);
  false_alarms = gauss_cum(-(1.0 - crit_z)*dprime);
  float pct_correct = (hits + (1.0 - false_alarms)) / 2.0;
  return pct_correct;
}

float taMath_float::chisq_p(float X, float v) {
  return gamma_p(0.5 * v, 0.5 * X * X);
}

float taMath_float::chisq_q(float X, float v) {
  return gamma_q(0.5 * v, 0.5 * X * X);
}

float taMath_float::students_cum(float t, float df) {
  return 1.0 - beta_i(0.5*df, 0.5, df / (df + t * t));
}


float taMath_float::students_cum_cum(float t, float df) {
  float pgtabst = 1.0 - students_cum(t,df);
  if(t<0)
    return 0.5*(pgtabst);
  else
    return 1.0 - 0.5*pgtabst ;
}

float taMath_float::students_den(float t, float df) {
  // just use discrete approximation..
  return (students_cum(t + 1.0e-6, df) - students_cum(t - 1.0e-6, df)) / 4.0e-6;
}

float taMath_float::Ftest_q(float F, float v1, float v2) {
  return beta_i(0.5*v2, 0.5*v1, v2 / (v2 + (v1 * F)));
}

float taMath_float::d_sub_a(float_Matrix* vec_signal, float_Matrix* vec_noise) {
  return ( vec_mean(vec_signal) - vec_mean(vec_noise) ) / ( sqrt( ( vec_std_dev(vec_signal) + vec_std_dev(vec_noise) ) / 2 ) );
}


/////////////////////////////////////////////////////////////////////////////////
// Vector operations (operate on Matrix objects, treating as a single linear guy)

///////////////////////////////////////
// arithmetic ops

void taMath_float::vec_fm_ints(float_Matrix* float_mat, const int_Matrix* int_mat) {
  float_mat->SetGeomN(int_mat->geom);
  for(int i=0;i<int_mat->size;i++) float_mat->FastEl_Flat(i) = (float)int_mat->FastEl_Flat(i);
}

void taMath_float::vec_to_ints(int_Matrix* int_mat, const float_Matrix* float_mat) {
  int_mat->SetGeomN(float_mat->geom);
  for(int i=0;i<float_mat->size;i++) int_mat->FastEl_Flat(i) = (int)float_mat->FastEl_Flat(i);
}

bool taMath_float::vec_check_type(const float_Matrix* a) {
  if(!a) {
    taMisc::Error("taMath: vector is NULL");
    return false;
  }
  if(a->GetDataValType() != taBase::VT_FLOAT) {
    taMisc::Error("taMath: vector is not of type float -- must be for this operation");
    return false;
  }
  return true;
}

bool taMath_float::vec_check_same_size(const float_Matrix* a, const float_Matrix* b, bool quiet) {
  if(!vec_check_type(a) || !vec_check_type(b)) return false;
  if(a->size != b->size) {
    if(!quiet)
      taMisc::Error("taMath: vectors are not the same size");
    return false;
  }
  if(a->size == 0) {
    if(!quiet)
      taMisc::Error("taMath: vectors have no elements");
    return false;
  }
  return true;
}

// todo: add some update signal after mod ops?  ItemsChanged?

bool taMath_float::vec_add(float_Matrix* a, const float_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) += b->FastEl_Flat(i);
  return true;
}

bool taMath_float::vec_sub(float_Matrix* a, const float_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) -= b->FastEl_Flat(i);
  return true;
}

bool taMath_float::vec_mult_els(float_Matrix* a, const float_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) *= b->FastEl_Flat(i);
  return true;
}

bool taMath_float::vec_div_els(float_Matrix* a, const float_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  for(int i=0;i<a->size;i++) {
    float bv = b->FastEl_Flat(i);
    if(bv != 0.0f)
      a->FastEl_Flat(i) /= bv;
  }
  return true;
}

bool taMath_float::vec_add_scalar(float_Matrix* a, float b) {
  if(!vec_check_type(a)) return false;
  TA_FOREACH_INDEX(i, *a) {
    a->FastEl_Flat(i) += b;
  }
  return true;
}

bool taMath_float::vec_sub_scalar(float_Matrix* a, float b) {
  if(!vec_check_type(a)) return false;
  TA_FOREACH_INDEX(i, *a) {
    a->FastEl_Flat(i) -= b;
  }
  return true;
}

bool taMath_float::vec_mult_scalar(float_Matrix* a, float b) {
  if(!vec_check_type(a)) return false;
  TA_FOREACH_INDEX(i, *a) {
    a->FastEl_Flat(i) *= b;
  }
  return true;
}

bool taMath_float::vec_div_scalar(float_Matrix* a, float b) {
  if(!vec_check_type(a)) return false;
  if(b == 0.0) return false;
  TA_FOREACH_INDEX(i, *a) {
    a->FastEl_Flat(i) /= b;
  }
  return true;
}

bool taMath_float::vec_quantize(float_Matrix* vec, float grid) {
  if(!vec_check_type(vec)) return false;
  if(grid == 0.0) return false;
  TA_FOREACH_INDEX(i, *vec) {
    vec->FastEl_Flat(i) = quantize(vec->FastEl_Flat(i), grid);
  }
  return true;
}

bool taMath_float::vec_simple_math(float_Matrix* vec, const SimpleMathSpec& math_spec) {
  if(!vec_check_type(vec)) return false;
  TA_FOREACH_INDEX(i, *vec) {
    vec->FastEl_Flat(i) = math_spec.Evaluate(vec->FastEl_Flat(i));
  }
  return true;
}

bool taMath_float::vec_simple_math_arg(float_Matrix* vec, const float_Matrix* arg_ary,
                                        const SimpleMathSpec& math_spec) {
  if(!vec_check_same_size(vec, arg_ary)) return false;
  SimpleMathSpec myms = math_spec;
  TA_FOREACH_INDEX(i, *vec) {
    myms.arg = arg_ary->FastEl_Flat(i);
    vec->FastEl_Flat(i) = myms.Evaluate(vec->FastEl_Flat(i));
  }
  return true;
}

bool taMath_float::vec_students_cum(float_Matrix* t, const float_Matrix* df) {
  if(!vec_check_same_size(t, df)) return false;
  for(int i=0;i<t->size;i++)
    t->FastEl_Flat(i) = students_cum(t->FastEl_Flat(i),df->FastEl_Flat(i));
  return true;
}

bool taMath_float::vec_students_cum_cum(float_Matrix* t, const float_Matrix* df) {
  if(!vec_check_same_size(t, df)) return false;
  for(int i=0;i<t->size;i++)
    t->FastEl_Flat(i) = students_cum_cum(t->FastEl_Flat(i),df->FastEl_Flat(i));
  return true;
}

bool taMath_float::vec_gauss_inv(float_Matrix* p) {
  if(!vec_check_type(p)) return false;
  for(int i=0;i<p->size;i++)
    p->FastEl_Flat(i) = gauss_inv(p->FastEl_Flat(i));
  return true;
}

bool taMath_float::vec_gauss_inv_lim(float_Matrix* p) {
  if(!vec_check_type(p)) return false;
  for(int i=0;i<p->size;i++)
    p->FastEl_Flat(i) = gauss_inv_lim(p->FastEl_Flat(i));
  return true;
}

float taMath_float::vec_first(const float_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    TA_FOREACH_INDEX(i, *vec) {
      return vec->FastEl_Flat(i);
    }
  }
  return vec->FastEl_Flat(0);
}

float taMath_float::vec_last(const float_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0) return 0.0;
  return vec->FastEl_Flat(vec->size-1);
}

int taMath_float::vec_find_first(const float_Matrix* vec, Relation& rel) {
  if(!vec_check_type(vec)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  TA_FOREACH_INDEX(i, *vec) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) return i;
  }
  return -1;
}

int taMath_float::vec_find_last(const float_Matrix* vec, Relation& rel) {
  if(!vec_check_type(vec)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  for(int i=vec->size-1;i>=0;i--) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) return i;
  }
  return -1;
}

float taMath_float::vec_max(const float_Matrix* vec, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    return vec->Max().toDouble(); // just use logic in obj
  }
  float rval = vec->FastEl_Flat(0);
  for(int i=1;i<vec->size;i++) {
    if(vec->FastEl_Flat(i) > rval) {
      idx = i;
      rval = vec->FastEl_Flat(i);
    }
  }
  return rval;
}

float taMath_float::vec_abs_max(const float_Matrix* vec, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    bool first = true;
    float rval = 0.0;
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
  float rval = fabs(vec->FastEl_Flat(0));
  for(int i=1;i<vec->size;i++) {
    if(fabs(vec->FastEl_Flat(i)) > rval) {
      idx = i;
      rval = fabs(vec->FastEl_Flat(i));
    }
  }
  return rval;
}

float taMath_float::vec_next_max(const float_Matrix* vec, int max_idx, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size < 2) return 0.0;
  float rval;
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

float taMath_float::vec_min(const float_Matrix* vec, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    return vec->Min().toDouble(); // just use logic in obj
  }
  float rval = vec->FastEl_Flat(0);
  for(int i=1;i<vec->size;i++) {
    if(vec->FastEl_Flat(i) < rval) {
      idx = i;
      rval = vec->FastEl_Flat(i);
    }
  }
  return rval;
}

float taMath_float::vec_abs_min(const float_Matrix* vec, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size == 0) return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    bool first = true;
    float rval = 0.0;
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
  float rval = fabs(vec->FastEl_Flat(0));
  for(int i=1;i<vec->size;i++) {
    if(fabs(vec->FastEl_Flat(i)) < rval) {
      idx = i;
      rval = fabs(vec->FastEl_Flat(i));
    }
  }
  return rval;
}

float taMath_float::vec_next_min(const float_Matrix* vec, int min_idx, int& idx) {
  if(!vec_check_type(vec)) return false;
  idx = 0;
  if(vec->size < 2) return 0.0;
  float rval;
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

float taMath_float::vec_sum(const float_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  float rval = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    rval += vec->FastEl_Flat(i);
  }
  return rval;
}


float taMath_float::vec_sum_range(const float_Matrix* vec, int start, int end) {
  if(!vec_check_type(vec)) return false;
  float rval = 0.0;
  if(start<0)
    start = vec->size - 1;
  if(end<0)
    end = vec->size ;
  if(start>end)
    return rval;
  for(int i=start;i<end;i++)
    rval += vec->FastEl_Flat(i);
  return rval;
}

float taMath_float::vec_prod(const float_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0) return 0.0;
  float rval = 0.0;
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

float taMath_float::vec_mean(const float_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0)    return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    int ic = vec->IterCount();
    if(ic == 0) return 0.0;
    return vec_sum(vec) / (float)ic;
  }
  return vec_sum(vec) / (float)vec->size;
}

float taMath_float::vec_var(const float_Matrix* vec, float mean, bool use_mean,
			      bool use_est) {
  if(!vec_check_type(vec)) return false;
  if(vec->size <= 1)    return 0.0;
  if(!use_mean)
    mean = vec_mean(vec);
  float rval = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    float val = (vec->FastEl_Flat(i) - mean);
    rval += val * val;
  }
  return rval / ((float)vec->size - (float)use_est);
}

float taMath_float::vec_std_dev(const float_Matrix* vec, float mean, bool use_mean, bool use_est) {
  if(vec->size <= 1)    return 0.0;
  return sqrt(vec_var(vec, mean, use_mean, use_est));
}

float taMath_float::vec_sem(const float_Matrix* vec, float mean, bool use_mean) {
  if(!vec_check_type(vec)) return false;
  if(vec->size <= 1)    return 0.0;
  if(vec->ElView()) {		// significantly less efficient
    int ic = vec->IterCount();
    if(ic <= 1) return 0.0;
    return vec_std_dev(vec, mean, use_mean, true) / sqrt((float)ic);
  }
  return vec_std_dev(vec, mean, use_mean, true) / sqrt((float)vec->size);
}

float taMath_float::vec_ss_len(const float_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  float rval = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    float val = vec->FastEl_Flat(i);
    rval += val * val;
  }
  return rval;
}

float taMath_float::vec_norm(const float_Matrix* vec) {
  return sqrt(vec_ss_len(vec));
}

float taMath_float::vec_ss_mean(const float_Matrix* vec) {
  if(!vec_check_type(vec)) return false;
  if(vec->size <= 1)    return 0.0;
  float rval = 0.0;
  float mean = vec_mean(vec);
  TA_FOREACH_INDEX(i, *vec) {
    float val = vec->FastEl_Flat(i) - mean;
    rval += val * val;
  }
  return rval;
}

void taMath_float::vec_histogram(float_Matrix* vec, const float_Matrix* oth, float bin_size,
                                  float min_val, float max_val) {
  if(!vec_check_type(vec) || !vec_check_type(oth)) return;
  if(oth->size == 0) return;
  vec->SetGeom(1,0);
  vec->Reset();
  float_Array tmp;
  tmp.SetSize(oth->IterCount());
  TA_FOREACH_INDEX(i, *vec) {
    tmp[FOREACH_itr->count] = oth->FastEl_Flat(i);
  }
  tmp.Sort();
  float min_v = tmp.FastEl(0);
  float max_v = tmp.Peek();
  if(min_val != max_val) {
    min_v = min_val;
    max_v = max_val;
  }
  int src_idx = 0;
  int trg_idx = 0;
  while((src_idx < tmp.size) && (tmp.FastEl(src_idx) < min_v)) { // skip up to the min
    src_idx++;
  }
  for(float cur_val = min_v; cur_val <= max_v; cur_val += bin_size, trg_idx++) {
    float cur_max = cur_val + bin_size;
    vec->Add(0);
    float& cur_hist = vec->FastEl_Flat(trg_idx);
    while((src_idx < tmp.size) && (tmp.FastEl(src_idx) < cur_max)) {
      cur_hist += 1.0;
      src_idx++;
    }
  }
}

float taMath_float::vec_count(const float_Matrix* vec, Relation& rel) {
  if(!vec_check_type(vec)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  float rval = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) rval += 1.0;
  }
  return rval;
}

float taMath_float::vec_count_float(const float_Matrix* vec, RelationFloat& rel) {
  if(!vec_check_type(vec)) return false;
  float rval = 0.0;
  for(int i=0;i<vec->size;i++) {
    if(rel.Evaluate(vec->FastEl_Flat(i))) rval += 1.0;
  }
  return rval;
}

float taMath_float::vec_median(const float_Matrix* vec) {
  if(!vec_check_type(vec)) return 0.0;
  if(vec->size == 0) return 0.0;
  float_Array tmp;
  tmp.SetSize(vec->IterCount());
  TA_FOREACH_INDEX(i, *vec) {
    tmp[FOREACH_itr->count] = vec->FastEl_Flat(i);
  }
  tmp.Sort();
  int idx = tmp.size / 2;
  return tmp[idx];
}

float taMath_float::vec_quantile(const float_Matrix* vec, float quant_pos) {
  if(!vec_check_type(vec)) return 0.0;
  if(vec->size == 0) return 0.0;
  float_Array tmp;
  tmp.SetSize(vec->IterCount());
  TA_FOREACH_INDEX(i, *vec) {
    tmp[FOREACH_itr->count] = vec->FastEl_Flat(i);
  }
  tmp.Sort();
  int idx = (int)(quant_pos * (float)tmp.size);
  if(idx >= tmp.size) idx = tmp.size-1;
  if(idx < 0) idx = 0;
  return tmp[idx];
}

float taMath_float::vec_kwta(float_Matrix* vec, int k, bool descending) {
  if(!vec_check_type(vec)) return 0.0f;
  if(vec->size == 0) return 0.0f;
  int_Array act_idx;
  // first fill act buff and get k for first items in list
  act_idx.SetSize(k);
  act_idx.FillSeq();
  int k_idx = 0;                // location of k guy within act_
  float k_val = vec->FastEl_Flat(0);
  if(descending) {
    for(int i=1;i<k; i++) {
      float val = vec->FastEl_Flat(i);
      if(val < k_val) {
        k_val = val;
        k_idx = i;
      }
    }
    // now, use the "replace-the-lowest" sorting technique
    for(int i=k;i<vec->size; i++) {
      float val = vec->FastEl_Flat(i);
      if(val <= k_val) {        // not bigger than smallest one in sort buffer
        continue;
      }
      act_idx.FastEl(k_idx) = i; // replace the smallest with it
      k_val = val;              // assume its the smallest
      for(int j=0; j < k; j++) {        // and recompute the actual smallest
        float val = vec->FastEl_Flat(act_idx[j]);
        if(val < k_val) {
          k_val = val;
          k_idx = j;            // idx in top k
        }
      }
    }
  }
  else {
    for(int i=1;i<k; i++) {
      float val = vec->FastEl_Flat(i);
      if(val > k_val) {
        k_val = val;
        k_idx = i;
      }
    }
    // now, use the "replace-the-lowest" sorting technique
    for(int i=k;i<vec->size; i++) {
      float val = vec->FastEl_Flat(i);
      if(val >= k_val) {        // not bigger than smallest one in sort buffer
        continue;
      }
      act_idx.FastEl(k_idx) = i; // replace the smallest with it
      k_val = val;              // assume its the smallest
      for(int j=0; j < k; j++) {        // and recompute the actual smallest
        float val = vec->FastEl_Flat(act_idx[j]);
        if(val > k_val) {
          k_val = val;
          k_idx = j;            // idx in top k
        }
      }
    }
  }
  return k_val;
}

void taMath_float::vec_kwta_avg(float& top_k_avg, float& bot_k_avg,
                                 float_Matrix* vec, int k, bool descending) {
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
  float k_val = vec->FastEl_Flat(0);
  if(descending) {
   for(int i=1;i<k; i++) {
      float val = vec->FastEl_Flat(i);
      if(val < k_val) {
        k_val = val;
        k_idx = i;
      }
    }
    // now, use the "replace-the-lowest" sorting technique
    for(int i=k;i<vec->size; i++) {
      float val = vec->FastEl_Flat(i);
      if(val <= k_val)  // not bigger than smallest one in sort buffer
        continue;
      inact_idx.FastEl(i-k) = act_idx.FastEl(k_idx);
      act_idx.FastEl(k_idx) = i; // replace the smallest with it
      k_val = val;              // assume its the smallest
      for(int j=0; j < k; j++) {        // and recompute the actual smallest
        float val = vec->FastEl_Flat(act_idx[j]);
        if(val < k_val) {
          k_val = val;
          k_idx = j;            // idx in top k
        }
      }
    }
  }
  else {
    for(int i=1;i<k; i++) {
      float val = vec->FastEl_Flat(i);
      if(val > k_val) {
        k_val = val;
        k_idx = i;
      }
    }
    // now, use the "replace-the-lowest" sorting technique
    for(int i=k;i<vec->size; i++) {
      float val = vec->FastEl_Flat(i);
      if(val >= k_val) {        // not bigger than smallest one in sort buffer
        continue;
      }
      inact_idx.FastEl(i-k) = act_idx.FastEl(k_idx);
      act_idx.FastEl(k_idx) = i; // replace the smallest with it
      k_val = val;              // assume its the smallest
      for(int j=0; j < k; j++) {        // and recompute the actual smallest
        float val = vec->FastEl_Flat(act_idx[j]);
        if(val > k_val) {
          k_val = val;
          k_idx = j;            // idx in top k
        }
      }
    }
  }

  float topsum = 0.0f;
  for(int j=0; j < k; j++) {
    float val = vec->FastEl_Flat(act_idx[j]);
    topsum += val;
  }
  float botsum = 0.0f;
  for(int j=0; j < inact_idx.size; j++) {
    float val = vec->FastEl_Flat(inact_idx[j]);
    botsum += val;
  }

  top_k_avg = topsum / (float)k;
  bot_k_avg = botsum / (float)(vec->size - k);
}

float taMath_float::vec_mode(const float_Matrix* vec) {
  if(!vec_check_type(vec)) return 0.0f;
  if(vec->size == 0) return 0.0f;
  float_Array tmp(false);
  tmp.SetSize(vec->IterCount());
  TA_FOREACH_INDEX(i, *vec) {
    tmp[FOREACH_itr->count] = vec->FastEl_Flat(i);
  }
  tmp.Sort();
  int mx_frq = 0;
  float mode = 0.0f;
  int idx = 0;
  while(idx < tmp.size) {
    float val = tmp[idx];
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

static int float_vec_sort_cmp(float v1, float v2) {
  if(v1 > v2) return 1;
  else if(v1 == v2) return 0;
  return -1;
}

void taMath_float::vec_sort(float_Matrix* vec, bool descending) {
  if(!vec_check_type(vec)) return;
  if(vec->size <= 1) return;
  int lt_compval = -1;          // comparison return value for less-than
  if(descending)
    lt_compval = 1;
  // lets do a heap sort since it requires no secondary storage
  int n = vec->size;
  int l,j,ir,i;
  float tmp;

  l = (n >> 1) + 1;
  ir = n;
  for(;;){
    if(l>1)
      tmp = vec->FastEl_Flat(--l -1); // tmp = ra[--l]
    else {
      tmp = vec->FastEl_Flat(ir-1); // tmp = ra[ir]
      vec->FastEl_Flat(ir-1) = vec->FastEl_Flat(0); // ra[ir] = ra[1]
      if(--ir == 1) {
        vec->FastEl_Flat(0) = tmp; // ra[1]=tmp
        return;
      }
    }
    i=l;
    j=l << 1;
    while(j<= ir) {
      if(j<ir && (float_vec_sort_cmp(vec->FastEl_Flat(j-1),vec->FastEl_Flat(j)) == lt_compval)) j++;
      if(float_vec_sort_cmp(tmp,vec->FastEl_Flat(j-1)) == lt_compval) { // tmp < ra[j]
        vec->FastEl_Flat(i-1) = vec->FastEl_Flat(j-1); // ra[i]=ra[j];
        j += (i=j);
      }
      else j = ir+1;
    }
    vec->FastEl_Flat(i-1) = tmp; // ra[i] = tmp;
  }
}

String taMath_float::vec_stats(const float_Matrix* vec) {
  if(!vec_check_type(vec)) return "";
  String rval;
  int idx;
  rval += "n=" + String(vec->size) + "; ";
  rval += "min=" + String(vec_min(vec, idx)) + "; ";
  rval += "max=" + String(vec_max(vec, idx)) + "; ";
  float mean = vec_mean(vec);
  rval += "mean=" + String(mean) + "; ";
  rval += "median=" + String(vec_median(vec)) + "; ";
  rval += "mode=" + String(vec_mode(vec)) + "; ";
  rval += "stdev=" + String(vec_std_dev(vec, mean, true)) + "; ";
  rval += "sem=" + String(vec_sem(vec, mean, true)) + ";";
  return rval;
}

int taMath_float::vec_prob_choose(float_Matrix* vec) {
  if(!vec_check_type(vec)) return -1;
  float rndval = Random::ZeroOne();
  float psum = 0.0;
  int rval;
  for(rval=0;rval<vec->size;rval++) {
    psum += vec->FastEl_Flat(rval);
    if(rndval < psum) {
      return rval;
    }
  }
  return vec->size-1;		// just to be safe..
}

float taMath_float::vec_dprime(const float_Matrix* signal_vec,
				 const float_Matrix* noise_vec) {
  if(!vec_check_type(signal_vec) || !vec_check_type(noise_vec)) return 0.0f;
  float mean_signal = vec_mean(signal_vec);
  float stdev_signal = vec_std_dev(signal_vec, mean_signal, true);
  float mean_noise = vec_mean(noise_vec);
  float stdev_noise = vec_std_dev(noise_vec, mean_noise, true);
  return dprime(mean_signal, stdev_signal, mean_noise, stdev_noise);
}

bool taMath_float::vec_regress_lin(const float_Matrix* x_vec, const float_Matrix* y_vec,
                                      float& b, float& m, float& cov00, float& cov01,
                                      float& cov11, float& sum_sq) {
  if(!vec_check_type(x_vec) || !vec_check_type(y_vec)) return false;
  double_Matrix dx(false);
  mat_cvt_float_to_double(&dx, x_vec);
  double_Matrix dy(false);
  mat_cvt_float_to_double(&dy, y_vec);
  double db, dm, dcov00, dcov01, dcov11, dsum_sq;
  bool rval = taMath_double::vec_regress_lin(&dx, &dy, db, dm, dcov00, dcov01, dcov11, dsum_sq);
  b = db; m = dm; cov00 = dcov00; cov01 = dcov01; cov11 = dcov11; sum_sq = dsum_sq;
  return rval;
}

bool taMath_float::vec_jitter_gauss(float_Matrix* vec, float stdev) {
  if(!vec_check_type(vec)) return false;
  if(vec->size == 0) return false;

  float_Array tmp;
  int jitter, vec_size;
  int zeroes = 0;
  int new_index = -1;
  float el;

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

float taMath_float::vec_ss_dist(const float_Matrix* vec, const float_Matrix* oth, bool norm,
                                  float tolerance)
{
  if(!vec_check_same_size(vec, oth)) return -1.0;
  float rval = 0.0;
  for(int i=0;i<vec->size;i++) {
    float d = vec->FastEl_Flat(i) - oth->FastEl_Flat(i);
    if(fabs(d) > tolerance)
      rval += d * d;
  }
  if(norm) {
    float dist = vec_ss_len(vec) + vec_ss_len(oth);
    if(dist != 0.0f)
      rval /= dist;
  }
  return rval;
}

float taMath_float::vec_euclid_dist(const float_Matrix* vec, const float_Matrix* oth, bool norm,
                                      float tolerance)
{
  float ssd = vec_ss_dist(vec, oth, norm, tolerance);
  if(ssd < 0.0) return ssd;
  return sqrt(ssd);
}

float taMath_float::vec_hamming_dist(const float_Matrix* vec, const float_Matrix* oth, bool norm,
                                      float tolerance)
{
  if(!vec_check_same_size(vec, oth)) return -1.0;
  float rval = 0.0;
  float alen = 0.0;
  float blen = 0.0;
  for(int i=0;i<vec->size;i++) {
    float d = fabs(vec->FastEl_Flat(i) - oth->FastEl_Flat(i));
    if(d <= tolerance)  d = 0.0;
    rval += d;
    if(norm) {
      alen += fabs(vec->FastEl_Flat(i));
      blen += fabs(oth->FastEl_Flat(i));
    }
  }
  if(norm) {
    float dist = alen + blen;
    if(dist != 0.0f)
      rval /= dist;
  }
  return rval;
}

float taMath_float::vec_covar(const float_Matrix* vec, const float_Matrix* oth) {
  if(!vec_check_same_size(vec, oth)) return -1.0;
  float my_mean = vec_mean(vec);
  float oth_mean = vec_mean(oth);
  float rval = 0.0;
  for(int i=0;i<vec->size;i++)
    rval += (vec->FastEl_Flat(i) - my_mean) * (oth->FastEl_Flat(i) - oth_mean);
  return rval / (float)vec->size;
}

float taMath_float::vec_correl(const float_Matrix* vec, const float_Matrix* oth) {
  if(!vec_check_same_size(vec, oth)) return -1.0;
  float my_mean = vec_mean(vec);
  float oth_mean = vec_mean(oth);
  float my_var = 0.0;
  float oth_var = 0.0;
  float rval = 0.0;
  for(int i=0;i<vec->size;i++) {
    float my_val = vec->FastEl_Flat(i) - my_mean;
    float oth_val = oth->FastEl_Flat(i) - oth_mean;
    rval += my_val * oth_val;
    my_var += my_val * my_val;
    oth_var += oth_val * oth_val;
  }
  float var_prod = sqrt(my_var * oth_var);
  if(var_prod != 0.0f)
    return rval / var_prod;
  else
    return 0.0;
}

float taMath_float::vec_inner_prod(const float_Matrix* vec, const float_Matrix* oth, bool norm) {
  if(!vec_check_same_size(vec, oth)) return -1.0;
  float rval = 0.0;
  for(int i=0;i<vec->size;i++)
    rval += vec->FastEl_Flat(i) * oth->FastEl_Flat(i);
  if(norm) {
    float dist = sqrt(vec_ss_len(vec) * vec_ss_len(oth));
    if(dist != 0.0f)
      rval /= dist;
  }
  return rval;
}

float taMath_float::vec_cross_entropy(const float_Matrix* vec, const float_Matrix* oth) {
  if(!vec_check_same_size(vec, oth)) return -1.0f;
  float rval = 0.0f;
  for(int i=0;i<vec->size;i++) {
    float p = vec->FastEl_Flat(i);
    float q = oth->FastEl_Flat(i);
    q = max(q,0.000001f); q = max(q,0.999999f);
    if(p >= 1.0)
      rval += -log(q);
    else if(p <= 0.0)
      rval += -log(1.0 - q);
    else
      rval += p * log(p/q) + (1.0 - p) * log((1.0 - p) / (1.0 - q));
  }
  return rval;
}

float taMath_float::vec_dist(const float_Matrix* vec, const float_Matrix* oth,
                               DistMetric metric, bool norm, float tolerance)
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

float taMath_float::scalar_dist(float v1, float v2,
                                  DistMetric metric, float tolerance)
{
  float rval = -1.0;
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
  case CROSS_ENTROPY: {
    float q = max(v2,0.000001f); q = max(v2,0.999999f);
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

float taMath_float::vec_norm_len(float_Matrix* vec, float len) {
  if(!vec_check_type(vec)) return 0.0f;
  if(vec->size == 0)    return 0.0;
  float scale = (len * len) / vec_ss_len(vec);
  TA_FOREACH_INDEX(i, *vec) {
    float val = vec->FastEl_Flat(i);
    float mag = (val * val) * scale;
    vec->FastEl_Flat(i) = (val >= 0.0f) ? mag : -mag;
  }
  return scale;
}

float taMath_float::vec_norm_sum(float_Matrix* vec, float sum, float min_val) {
  if(!vec_check_type(vec)) return 0.0f;
  if(vec->size == 0)    return 0.0;
  float act_sum = 0.0;
  TA_FOREACH_INDEX(i, *vec) {
    act_sum += (vec->FastEl_Flat(i) - min_val);
  }
  if(act_sum == 0.0f) return 0.0;
  float scale = (sum / act_sum);
  TA_FOREACH_INDEX(i, *vec) {
    vec->FastEl_Flat(i) = ((vec->FastEl_Flat(i) - min_val) * scale) + min_val;
  }
  return scale;
}

float taMath_float::vec_norm_max(float_Matrix* vec, float max) {
  if(!vec_check_type(vec)) return 0.0f;
  if(vec->size == 0)    return 0.0;
  int idx;
  float cur_max = vec_max(vec, idx);
  if(cur_max == 0.0) return 0.0;
  float scale = (max / cur_max);
  TA_FOREACH_INDEX(i, *vec) {
    vec->FastEl_Flat(i) *= scale;
  }
  return scale;
}

float taMath_float::vec_norm_abs_max(float_Matrix* vec, float max) {
  if(!vec_check_type(vec)) return 0.0f;
  if(vec->size == 0)    return 0.0;
  int idx;
  float cur_max = vec_abs_max(vec, idx);
  if(cur_max == 0.0) return 0.0;
  float scale = (max / cur_max);
  TA_FOREACH_INDEX(i, *vec) {
    vec->FastEl_Flat(i) *= scale;
  }
  return scale;
}

int taMath_float::vec_threshold(float_Matrix* vec, float thresh, float low, float high) {
  if(!vec_check_type(vec)) return 0;
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

int taMath_float::vec_threshold_low(float_Matrix* vec, float thresh, float low) {
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

int taMath_float::vec_threshold_high(float_Matrix* vec, float thresh, float high) {
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

int taMath_float::vec_replace(float_Matrix* vec, float find1, float repl1,
                               bool do2, float find2, float repl2,
                               bool do3, float find3, float repl3,
                               bool do4, float find4, float repl4,
                               bool do5, float find5, float repl5,
                               bool do6, float find6, float repl6) {
  if(!vec_check_type(vec)) return 0;
  if(vec->size == 0)  return 0;
  int rval = 0;
  TA_FOREACH_INDEX(i, *vec) {
    float vl = vec->FastEl_Flat(i);
    if(vl == find1) { vec->FastEl_Flat(i) = repl1; rval++; }
    if(do2 && vl == find2) { vec->FastEl_Flat(i) = repl2; rval++; }
    if(do3 && vl == find3) { vec->FastEl_Flat(i) = repl3; rval++; }
    if(do4 && vl == find4) { vec->FastEl_Flat(i) = repl4; rval++; }
    if(do5 && vl == find5) { vec->FastEl_Flat(i) = repl5; rval++; }
    if(do6 && vl == find6) { vec->FastEl_Flat(i) = repl6; rval++; }
  }
  return rval;
}

float taMath_float::vec_aggregate(const float_Matrix* vec, Aggregate& agg) {
  if(!vec_check_type(vec)) return 0.0f;
  int idx;
  switch(agg.op) {
  case Aggregate::GROUP:
    return vec->SafeEl_Flat(0); // first guy..
  case Aggregate::FIRST:
    return taMath_float::vec_first(vec);
  case Aggregate::LAST:
    return taMath_float::vec_last(vec);
  case Aggregate::FIND_FIRST:
    return (float)taMath_float::vec_find_first(vec, agg.rel);
  case Aggregate::FIND_LAST:
    return (float)taMath_float::vec_find_last(vec, agg.rel);
  case Aggregate::MIN:
    return taMath_float::vec_min(vec, idx);
  case Aggregate::MAX:
    return taMath_float::vec_max(vec, idx);
  case Aggregate::ABS_MIN:
    return taMath_float::vec_abs_min(vec, idx);
  case Aggregate::ABS_MAX:
    return taMath_float::vec_abs_max(vec, idx);
  case Aggregate::SUM:
    return taMath_float::vec_sum(vec);
  case Aggregate::PROD:
    return taMath_float::vec_prod(vec);
  case Aggregate::MEAN:
    return taMath_float::vec_mean(vec);
  case Aggregate::VAR:
    return taMath_float::vec_var(vec);
  case Aggregate::SS:
    return taMath_float::vec_ss_mean(vec);
  case Aggregate::STDEV:
    return taMath_float::vec_std_dev(vec);
  case Aggregate::SEM:
    return taMath_float::vec_sem(vec);
  case Aggregate::N:
    return (float)vec->size;
  case Aggregate::COUNT:
    return taMath_float::vec_count(vec, agg.rel);
  case Aggregate::MEDIAN:
    return taMath_float::vec_median(vec);
  case Aggregate::MODE:
    return taMath_float::vec_mode(vec);
  case Aggregate::QUANTILE:
    return taMath_float::vec_quantile(vec, agg.rel.val);
  case Aggregate::NONE:
    return 0.0;
  }
  return 0.0;
}


/////////////////////////////////////////////////////////////////////////////////
// Convolution

bool taMath_float::vec_kern_uniform(float_Matrix* kernel, int half_sz,
                                     bool neg_tail, bool pos_tail) {
  if(!vec_check_type(kernel)) return false;
  int sz = half_sz * 2 + 1;
  kernel->SetGeom(1, sz);
  float ctr = (float)half_sz;
  float val = 1.0 / (float)sz;
  for(int i=0;i<kernel->size;i++) {
    float x = (float)i - ctr;
    float y = 0.0;
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

bool taMath_float::vec_kern_gauss(float_Matrix* kernel, int half_sz, float sigma,
                                   bool neg_tail, bool pos_tail) {
  if(!vec_check_type(kernel)) return false;
  kernel->SetGeom(1, half_sz * 2 + 1);
  float off = (float)half_sz;
  float ssq = -1.0 / (2.0 * sigma * sigma);
  for(int i=0;i<kernel->size;i++) {
    float x = (float)i - off;
    float y = exp(ssq * x * x);
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

bool taMath_float::vec_kern_exp(float_Matrix* kernel, int half_sz, float exp_mult,
                                 bool neg_tail, bool pos_tail) {
  if(!vec_check_type(kernel)) return false;
  kernel->SetGeom(1, half_sz * 2 + 1);
  float ctr = (float)half_sz;
  for(int i=0;i<kernel->size;i++) {
    float x = (float)i - ctr;
    float y = 0.0;
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

bool taMath_float::vec_kern_pow(float_Matrix* kernel, int half_sz, float pow_exp,
                                 bool neg_tail, bool pos_tail) {
  if(!vec_check_type(kernel)) return false;
  kernel->SetGeom(1, half_sz * 2 + 1);
  float ctr = (float)half_sz;
  for(int i=0;i<kernel->size;i++) {
    float x = (float)i - ctr;
    float y = 0.0;
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

bool taMath_float::vec_convolve(float_Matrix* out_vec, const float_Matrix* in_vec,
                                   const float_Matrix* kernel, bool keep_edges) {
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
      float sum = 0.0;
      float dnorm = 0.0;
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
      float sum = 0.0;
      for(int j=0;j<kernel->size;j++) {
        sum += in_vec->FastEl(i+j) * kernel->FastEl(j);
      }
      out_vec->FastEl(i) = sum;
    }
  }
  return true;
}

bool taMath_float::vec_kern2d_gauss(float_Matrix* kernel, int sz_x, int sz_y,
                                     float sigma_x, float sigma_y) {
  if(!vec_check_type(kernel)) return false;
  kernel->SetGeom(2, sz_x, sz_y);
  float ctr_x = (float)(sz_x-1) * 0.5;
  float ctr_y = (float)(sz_y-1) * 0.5;
  float eff_sig_x = sigma_x * ctr_x;
  float eff_sig_y = sigma_y * ctr_y;
  for(int yi=0; yi < sz_y; yi++) {
    float y = ((float)yi - ctr_y) / eff_sig_y;
    for(int xi=0; xi < sz_x; xi++) {
      float x = ((float)xi - ctr_y) / eff_sig_x;
      float gv = exp(-(x*x + y*y)/2.0f);
      kernel->FastEl(xi, yi) = gv;
    }
  }
  vec_norm_sum(kernel);
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Matrix operations

bool taMath_float::mat_col(float_Matrix* col, const float_Matrix* mat, int col_no) {
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

bool taMath_float::mat_row(float_Matrix* row, const float_Matrix* mat, int row_no) {
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

bool taMath_float::mat_get_gsl_fm_ta(gsl_matrix_float* gsl_mat, const float_Matrix* ta_mat)
{
  if(!vec_check_type(ta_mat)) return false;
  if(ta_mat->dims() != 2) {
    taMisc::Error("taMath::mat_get_gsl_fm_ta: dimension of matrix must be 2 for gsl calls!");
    return false;
  }
  gsl_mat->size2 = ta_mat->dim(0); // "rows" (rows are contiguous in mem)
  gsl_mat->size1 = ta_mat->dim(1); // "columns"
  gsl_mat->tda = ta_mat->dim(0); // actual size of row in memory
  gsl_mat->data = (float*)ta_mat->data();
  gsl_mat->block = NULL;
  gsl_mat->owner = false;
  return true;
}

bool taMath_float::vec_get_gsl_fm_ta(gsl_vector_float* gsl_vec, const float_Matrix* ta_vec)
{
  if(!vec_check_type(ta_vec)) return false;
  if(ta_vec->dims() != 1) return false;
  gsl_vec->size = ta_vec->dim(0);
  gsl_vec->stride = 1;
  gsl_vec->data = (float*)ta_vec->data();
  gsl_vec->block = NULL;
  gsl_vec->owner = false;
  return true;
}

bool taMath_float::mat_add(float_Matrix* a, const float_Matrix* b) {
  gsl_matrix_float g_a;  if(!mat_get_gsl_fm_ta(&g_a, a)) return false;
  gsl_matrix_float g_b;  if(!mat_get_gsl_fm_ta(&g_b, b)) return false;
  return gsl_matrix_float_add(&g_a, &g_b);
}

bool taMath_float::mat_sub(float_Matrix* a, const float_Matrix* b) {
  gsl_matrix_float g_a;  if(!mat_get_gsl_fm_ta(&g_a, a)) return false;
  gsl_matrix_float g_b;  if(!mat_get_gsl_fm_ta(&g_b, b)) return false;
  return gsl_matrix_float_sub(&g_a, &g_b);
}

bool taMath_float::mat_mult_els(float_Matrix* a, const float_Matrix* b) {
  gsl_matrix_float g_a;  if(!mat_get_gsl_fm_ta(&g_a, a)) return false;
  gsl_matrix_float g_b;  if(!mat_get_gsl_fm_ta(&g_b, b)) return false;
  return gsl_matrix_float_mul_elements(&g_a, &g_b);
}

bool taMath_float::mat_div_els(float_Matrix* a, const float_Matrix* b) {
  gsl_matrix_float g_a;  if(!mat_get_gsl_fm_ta(&g_a, a)) return false;
  gsl_matrix_float g_b;  if(!mat_get_gsl_fm_ta(&g_b, b)) return false;
  return gsl_matrix_float_div_elements(&g_a, &g_b);
}

bool taMath_float::mat_transpose(float_Matrix* dest, const float_Matrix* src) {
  if(!dest || !src){taMisc::Error("dest or src cannot be null. try dest=new float_Matrix");return false;}
  if(src->dims() != 2){taMisc::Error("Can only transpose a 2d matrix");return false;}
  int d0,d1;
  d0 = src->dim(0);
  d1 = src->dim(1);
  dest->SetGeom(2,d1,d0);
  for(int i=0;i<d0;i++) {
    for(int j=0;j<d1;j++) {
      dest->Set(src->FastElAsFloat(i,j),j,i);
    }
  }
  return true;
}

bool taMath_float::mat_mult(float_Matrix* c, const float_Matrix* a, const float_Matrix* b) {
  gsl_matrix_float g_a;  if(!mat_get_gsl_fm_ta(&g_a, a)) return false;
  gsl_matrix_float g_b;  if(!mat_get_gsl_fm_ta(&g_b, b)) return false;
  if(!vec_check_type(c)) return false;
  // ensure return matrix is correct size
  c->SetGeom(2, b->dim(0), a->dim(1));
  gsl_matrix_float g_c;  if(!mat_get_gsl_fm_ta(&g_c, c)) return false;
  int rval = gsl_blas_sgemm(CblasNoTrans, CblasNoTrans, 1.0, &g_a, &g_b, 0.0, &g_c);
  return true;			// todo: decode rvals
}

float taMath_float::mat_det(const float_Matrix* a) {
  if(a->dims() != 2) {
    taMisc::Error("mat_det: matrix is not 2 dimensional!");
    return false;
  }
  if(a->dim(0) != a->dim(1)) {
    taMisc::Error("mat_det: matrix is not square!");
    return false;
  }
  double_Matrix da(false);
  mat_cvt_float_to_double(&da, a);
  float rval = taMath_double::mat_det(&da);
  return rval;
}

bool taMath_float::mat_vec_product(const float_Matrix* A, const float_Matrix* x,
                                    float_Matrix* y) {
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

  gsl_matrix_float A_gsl;
  mat_get_gsl_fm_ta(&A_gsl, A);
  const gsl_matrix_float* A_gsl_ptr = &A_gsl;

  gsl_vector_float x_gsl;
  vec_get_gsl_fm_ta(&x_gsl, x);
  const gsl_vector_float* x_gsl_ptr = &x_gsl;

  gsl_vector_float y_gsl;
  vec_get_gsl_fm_ta(&y_gsl, y);
  gsl_vector_float* y_gsl_ptr = &y_gsl;

  gsl_blas_sgemv(CblasNoTrans, 1.0, A_gsl_ptr, x_gsl_ptr, 0.0, y_gsl_ptr);

  return true;
}

bool taMath_float::mat_eigen_owrite(float_Matrix* a, float_Matrix* eigen_vals,
                                    float_Matrix* eigen_vecs) {
  double_Matrix da(false);
  mat_cvt_float_to_double(&da, a);
  double_Matrix deval(false);
  double_Matrix devec(false);
  bool rval = taMath_double::mat_eigen_owrite(&da, &deval, &devec);
  mat_cvt_double_to_float(eigen_vals, &deval);
  mat_cvt_double_to_float(eigen_vecs, &devec);
  return rval;
}

bool taMath_float::mat_eigen(const float_Matrix* a, float_Matrix* eigen_vals,
                                    float_Matrix* eigen_vecs) {
  return mat_eigen_owrite((float_Matrix*)a, eigen_vals, eigen_vecs);
}

bool taMath_float::mat_svd_owrite(float_Matrix* a, float_Matrix* s, float_Matrix* v) {
  double_Matrix da(false);
  mat_cvt_float_to_double(&da, a);
  double_Matrix ds(false);
  double_Matrix dv(false);
  bool rval = taMath_double::mat_svd_owrite(&da, &ds, &dv);
  mat_cvt_double_to_float(s, &ds);
  mat_cvt_double_to_float(v, &dv);
  return rval;
}

bool taMath_float::mat_svd(const float_Matrix* a, float_Matrix* u, float_Matrix* s, float_Matrix* v) {
  double_Matrix da(false);
  mat_cvt_float_to_double(&da, a);
  double_Matrix du(false);
  double_Matrix ds(false);
  double_Matrix dv(false);
  bool rval = taMath_double::mat_svd(&da, &du, &ds, &dv);
  mat_cvt_double_to_float(u, &du);
  mat_cvt_double_to_float(s, &ds);
  mat_cvt_double_to_float(v, &dv);
  return rval;
}

bool taMath_float::mat_mds_owrite(float_Matrix* a, float_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  double_Matrix da(false);
  mat_cvt_float_to_double(&da, a);
  double_Matrix dxy(false);
  bool rval = taMath_double::mat_mds_owrite(&da, &dxy, x_axis_c, y_axis_c);
  mat_cvt_double_to_float(xy_coords, &dxy);
  return rval;
}

bool taMath_float::mat_mds(const float_Matrix* a, float_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  return mat_mds_owrite((float_Matrix*)a, xy_coords, x_axis_c, y_axis_c);
}

bool taMath_float::mat_slice(float_Matrix* dest, float_Matrix* src, int d0_start, int d0_end, int d1_start, int d1_end) {
  if(!dest || !src){taMisc::Error("dest or src cannot be null. try dest=new float_Matrix");return false;}
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
      dest->Set(src->FastEl(i,j),k,l);
  return true;
}

bool taMath_float::mat_trim(float_Matrix* dest, float_Matrix* src, RelationFloat& thresh, int intol_within, int intol_between,
                            bool left, bool right, bool top, bool bottom) {

  if(!dest || !src){taMisc::Error("dest or src cannot be null. try dest=new float_Matrix");return false;}
  if(src->dims() != 2) {taMisc::Error("Can only trim a 2d matrix"); return false;}

  int trim_left = 0, trim_right = 0, trim_top = 0, trim_bottom = 0, tol_cnt, n;
  float_Matrix* mat = new float_Matrix;

  int d0 = src->dim(0);
  int d1 = src->dim(1);

  if(left) {
    tol_cnt = 0;
    for(int i = 0;i < d0;i++) {
      mat_slice(mat,src,i,i);
      n = (int)vec_count_float(mat, thresh);
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
      n = (int)vec_count_float(mat, thresh);
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
      n = (int)vec_count_float(mat, thresh);
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
      n = (int)vec_count_float(mat, thresh);
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


#else // !HAVE_LIBGSL

bool taMath_float::mat_add(float_Matrix* a, const float_Matrix* b) {
  return false;
}

bool taMath_float::mat_sub(float_Matrix* a, const float_Matrix* b) {
  return false;
}

bool taMath_float::mat_mult_els(float_Matrix* a, const float_Matrix* b) {
  return false;
}

bool taMath_float::mat_div_els(float_Matrix* a, const float_Matrix* b) {
  return false;
}

bool taMath_float::mat_eigen_owrite(float_Matrix* a, float_Matrix* eigen_vals,
                                     float_Matrix* eigen_vecs) {
  return false;
}

bool taMath_float::mat_eigen(const float_Matrix* a, float_Matrix* eigen_vals,
                                    float_Matrix* eigen_vecs) {
  return false;
}

bool taMath_float::mat_svd_owrite(float_Matrix* a, float_Matrix* s, float_Matrix* v) {
  return false;
}

bool taMath_float::mat_svd(const float_Matrix* a, float_Matrix* u, float_Matrix* s, float_Matrix* v) {
  return false;
}


bool taMath_float::mat_mds_owrite(float_Matrix* a, float_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  return false;
}

bool taMath_float::mat_mds(const float_Matrix* a, float_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  return false;
}

bool taMath_float::fft_real_transform(float_Matrix* out_mat, const float_Matrix* in_mat,
    bool real_out, bool norm)
{
  return false;
}

#endif  // HAVE_LIBGSL

bool taMath_float::mat_dist(float_Matrix* dist_mat, const float_Matrix* src_mat,
                             DistMetric metric, bool norm, float tolerance) {
  if(!vec_check_type(dist_mat) || !vec_check_type(src_mat)) return false;
  if(src_mat->dims() < 2) {
    taMisc::Error("*** mat_dist: matrix is < 2 dimensional!");
    return false;
  }
  int n = src_mat->frames();
  dist_mat->SetGeom(2, n, n);
  for(int i=0;i<n;i++) {
    float_Matrix* t1 = ((float_Matrix*)src_mat)->GetFrameSlice(i);
    taBase::Ref(t1);
    for(int j=0;j<n;j++) {
      float_Matrix* t2 = ((float_Matrix*)src_mat)->GetFrameSlice(j);
      taBase::Ref(t2);
      float dist = vec_dist(t1, t2, metric, norm, tolerance);
      dist_mat->FastEl(j,i) = dist;
      taBase::unRefDone(t2);
    }
    taBase::unRefDone(t1);
  }
  return true;
}

bool taMath_float::mat_cross_dist(float_Matrix* dist_mat, const float_Matrix* src_mat_a,
                                   const float_Matrix* src_mat_b,
                                   DistMetric metric, bool norm, float tolerance) {
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
  if(src_mat_a->frameSize() != src_mat_b->frameSize()) {
    taMisc::Error("*** mat_cross_dist: matrix a and b do not have the same framesize!");
    return false;
  }
  int n_rows = src_mat_a->frames();
  int n_cols = src_mat_b->frames();
  dist_mat->SetGeom(2, n_cols, n_rows);
  for(int i=0;i<n_rows;i++) {
    float_Matrix* t1 = ((float_Matrix*)src_mat_a)->GetFrameSlice(i);
    taBase::Ref(t1);
    for(int j=0;j<n_cols;j++) {
      float_Matrix* t2 = ((float_Matrix*)src_mat_b)->GetFrameSlice(j);
      taBase::Ref(t2);
      float dist = vec_dist(t1, t2, metric, norm, tolerance);
      dist_mat->FastEl(j,i) = dist;
      taBase::unRefDone(t2);
    }
    taBase::unRefDone(t1);
  }
  return true;
}

bool taMath_float::mat_cell_to_vec(float_Matrix* vec, const float_Matrix* mat, int cell_no) {
  if(!vec_check_type(vec) || !vec_check_type(mat)) return false;
  if(cell_no >= mat->frameSize()) {
    taMisc::Error("mat_cell_to_vec: cell no:", String(cell_no), "is larger than framesize:",
                  String(mat->frameSize()));
    return false;
  }
  int n = mat->frames();
  vec->SetGeom(1, n);
  for(int i=0;i<n; i++) {
    vec->FastEl(i) = mat->FastEl_Flat(mat->FrameStartIdx(i) + cell_no);
  }
  return true;
}

bool taMath_float::mat_correl(float_Matrix* correl_mat, const float_Matrix* src_mat) {
  if(!vec_check_type(correl_mat) || !vec_check_type(src_mat)) return false;
  if(src_mat->dims() < 2) {
    taMisc::Error("*** mat_correl: src_matrix is < 2 dimensional!");
    return false;
  }

  int n = src_mat->frameSize();
  correl_mat->SetGeom(2, n, n);

  float_Matrix p1vals(false);
  float_Matrix p2vals(false);

  for(int i=0;i<n;i++) {
    mat_cell_to_vec(&p1vals, src_mat, i);
    for(int j=0;j<n;j++) {
      mat_cell_to_vec(&p2vals, src_mat, j);
      correl_mat->FastEl(j,i) = vec_correl(&p1vals, &p2vals);
    }
  }
  return true;
}

bool taMath_float::mat_prjn(float_Matrix* prjn_vec, const float_Matrix* src_mat,
                             const float_Matrix* prjn_mat, DistMetric metric,
                             bool norm, float tol) {
  if(!vec_check_type(prjn_vec) || !vec_check_type(src_mat) || !vec_check_type(prjn_mat))
    return false;
  if(src_mat->dims() < 2) {
    taMisc::Error("*** mat_prjn: source matrix is < 2 dimensional!");
    return false;
  }
  if(src_mat->frameSize() != prjn_mat->count()) {
    taMisc::Error("*** mat_prjn: source matrix frame and projection matrix don't have the same size");
    return false;
  }
  int n_rows = src_mat->frames();
  prjn_vec->SetGeom(1, n_rows);
  for(int i=0;i<n_rows;i++) {
    float_Matrix* t1 = ((float_Matrix*)src_mat)->GetFrameSlice(i);
    taBase::Ref(t1);
    float val = vec_dist(t1, prjn_mat, metric, norm, tol);
    prjn_vec->FastEl(i) = val;
    taBase::unRefDone(t1);
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
// frame-compatible versions of various functions

bool taMath_float::mat_time_avg(float_Matrix* a, float avg_dt) {
  if(!vec_check_type(a)) return false;
  if(avg_dt < 0.0f || avg_dt > 1.0f) {
    taMisc::Error("*** mat_time_avg: avg_dt is not in 0..1 range:", String(avg_dt));
    return false;
  }
  float avg_dt_c = 1.0 - avg_dt;
  for(int i=1;i<a->frames();i++) {
    int fr_idx = a->FrameStartIdx(i);
    int frm1_idx = a->FrameStartIdx(i-1);
    for(int j=0;j<a->frameSize();j++) {
      a->FastEl_Flat(fr_idx + j) = avg_dt * a->FastEl_Flat(frm1_idx + j) + avg_dt_c * a->FastEl_Flat(fr_idx + j);
    }
  }
  return true;
}

bool taMath_float::mat_frame_convolve(float_Matrix* out_vec, const float_Matrix* in_vec,
                                       const float_Matrix* kernel) {
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
  for(int i=0;i<in_vec->frames();i++) {
    for(int k=0;k<in_vec->frameSize();k++) {
      float sum = 0.0;
      float dnorm = 0.0;
      for(int j=0;j<kernel->size;j++) {
        int idx = i + j - off;
        if(idx < 0 || idx >= in_vec->frames()) {
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

bool taMath_float::mat_fmt_out_frame(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!vec_check_type(in_mat) || !vec_check_type(out_mat)) return false;
  MatrixGeom frg = in_mat->geom;
  if(frg.dims() == 1 || in_mat->frames() == 0) return false;
  frg.SetDims(frg.dims()-1);    // nuke last dim
  out_mat->SetGeomN(frg);
  return true;
}

bool taMath_float::mat_frame_set_n(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  float frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = frn;
  }
  return true;
}

bool taMath_float::mat_frame_first(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = in_mat->FastEl_Flat(i);
  }
  return true;
}

bool taMath_float::mat_frame_last(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  int off = (frn-1) * frs;
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = in_mat->FastEl_Flat(off + i);
  }
  return true;
}

bool taMath_float::mat_frame_find_first(float_Matrix* out_mat, const float_Matrix* in_mat,
                                         Relation& rel) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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

bool taMath_float::mat_frame_find_last(float_Matrix* out_mat, const float_Matrix* in_mat,
                                         Relation& rel) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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

bool taMath_float::mat_frame_max(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    float mx = in_mat->FastEl_Flat(i);
    for(int j=1;j<frn;j++) {
      mx = MAX(in_mat->FastEl_Flat(j * frs + i), mx);
    }
    out_mat->FastEl_Flat(i) = mx;
  }
  return true;
}

bool taMath_float::mat_frame_abs_max(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    float mx = fabs(in_mat->FastEl_Flat(i));
    for(int j=1;j<frn;j++) {
      mx = MAX(fabs(in_mat->FastEl_Flat(j * frs + i)), mx);
    }
    out_mat->FastEl_Flat(i) = mx;
  }
  return true;
}

bool taMath_float::mat_frame_min(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    float mx = in_mat->FastEl_Flat(i);
    for(int j=1;j<frn;j++) {
      mx = MIN(in_mat->FastEl_Flat(j * frs + i), mx);
    }
    out_mat->FastEl_Flat(i) = mx;
  }
  return true;
}

bool taMath_float::mat_frame_abs_min(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    float mx = fabs(in_mat->FastEl_Flat(i));
    for(int j=1;j<frn;j++) {
      mx = MIN(fabs(in_mat->FastEl_Flat(j * frs + i)), mx);
    }
    out_mat->FastEl_Flat(i) = mx;
  }
  return true;
}

bool taMath_float::mat_frame_sum(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    float sum = 0.0f;
    for(int j=0;j<frn;j++) {
      sum += in_mat->FastEl_Flat(j * frs + i);
    }
    out_mat->FastEl_Flat(i) = sum;
  }
  return true;
}

bool taMath_float::mat_frame_prod(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    float prod = in_mat->FastEl_Flat(i);
    for(int j=1;j<frn;j++) {
      prod *= in_mat->FastEl_Flat(j * frs + i);
    }
    out_mat->FastEl_Flat(i) = prod;
  }
  return true;
}

bool taMath_float::mat_frame_mean(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_frame_sum(out_mat, in_mat)) return false;
  float nrm = 1.0 / (float)in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) *= nrm;
  }
  return true;
}

bool taMath_float::mat_frame_var(float_Matrix* out_mat, const float_Matrix* in_mat, bool use_est) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  float nrm = 1.0 / ((float)frn - (float)use_est);
  for(int i=0;i<frs;i++) {
    float mean = 0.0f;
    for(int j=0;j<frn;j++) {
      mean += in_mat->FastEl_Flat(j * frs + i);
    }
    mean /= (float)frn;
    float var = 0.0f;
    for(int j=0;j<frn;j++) {
      float dm = (in_mat->FastEl_Flat(j * frs + i) - mean);
      var += dm * dm;
    }
    out_mat->FastEl_Flat(i) = var * nrm;
  }
  return true;
}

bool taMath_float::mat_frame_std_dev(float_Matrix* out_mat, const float_Matrix* in_mat, bool use_est) {
  if(!mat_frame_var(out_mat, in_mat, use_est)) return false;
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = sqrt(out_mat->FastEl_Flat(i));
  }
  return true;
}

bool taMath_float::mat_frame_sem(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_frame_std_dev(out_mat, in_mat, true)) return false;
  int frs = in_mat->frameSize();
  float nrm = 1.0 / sqrt((float)in_mat->frames());
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) *= nrm;
  }
  return true;
}

bool taMath_float::mat_frame_ss_len(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    float sum = 0.0f;
    for(int j=0;j<frn;j++) {
      sum += in_mat->FastEl_Flat(j * frs + i) * in_mat->FastEl_Flat(j * frs + i);
    }
    out_mat->FastEl_Flat(i) = sum;
  }
  return true;
}

bool taMath_float::mat_frame_ss_mean(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    float sumsqr = 0.0f;
    float sum = 0.0f;
    for(int j=0;j<frn;j++) {
      sumsqr += in_mat->FastEl_Flat(j * frs + i) * in_mat->FastEl_Flat(j * frs + i);
      sum += in_mat->FastEl_Flat(j * frs + i);
    }
    out_mat->FastEl_Flat(i) = sumsqr - (sum * sum) / frn;
  }
  return true;
}

bool taMath_float::mat_frame_count(float_Matrix* out_mat, const float_Matrix* in_mat, Relation& rel) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    float sum = 0.0f;
    for(int j=0;j<frn;j++) {
      if(tmp_rel.Evaluate(in_mat->FastEl_Flat(j * frs + i))) sum += 1.0;
    }
    out_mat->FastEl_Flat(i) = sum;
  }
  return true;
}

bool taMath_float::mat_frame_median(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  int idx = frn / 2;
  float_Array tmp(false);
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

bool taMath_float::mat_frame_quantile(float_Matrix* out_mat, const float_Matrix* in_mat, float quant_pos) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  float_Array tmp(false);
  tmp.SetSize(frn);
  int idx = (int)(quant_pos * (float)tmp.size);
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

bool taMath_float::mat_frame_mode(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  float_Array tmp(false);
  tmp.SetSize(frn);
  for(int i=0;i<frs;i++) {
    for(int j=0;j<frn;j++) {
      tmp[j] = in_mat->FastEl_Flat(j * frs + i);
    }
    tmp.Sort();
    int mx_frq = 0;
    float mode = 0.0f;
    int idx = 0;
    while(idx < tmp.size) {
      float val = tmp[idx];
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

bool taMath_float::mat_frame_aggregate(float_Matrix* out_mat, const float_Matrix* in_mat,
                                        Aggregate& agg) {
  switch(agg.op) {
  case Aggregate::GROUP:
  case Aggregate::FIRST:
    return taMath_float::mat_frame_first(out_mat, in_mat);
  case Aggregate::LAST:
    return taMath_float::mat_frame_last(out_mat, in_mat);
  case Aggregate::FIND_FIRST:
    return taMath_float::mat_frame_find_first(out_mat, in_mat, agg.rel);
  case Aggregate::FIND_LAST:
    return taMath_float::mat_frame_find_last(out_mat, in_mat, agg.rel);
  case Aggregate::MIN:
    return taMath_float::mat_frame_min(out_mat, in_mat);
  case Aggregate::MAX:
    return taMath_float::mat_frame_max(out_mat, in_mat);
  case Aggregate::ABS_MIN:
    return taMath_float::mat_frame_abs_min(out_mat, in_mat);
  case Aggregate::ABS_MAX:
    return taMath_float::mat_frame_abs_max(out_mat, in_mat);
  case Aggregate::SUM:
    return taMath_float::mat_frame_sum(out_mat, in_mat);
  case Aggregate::PROD:
    return taMath_float::mat_frame_prod(out_mat, in_mat);
  case Aggregate::MEAN:
    return taMath_float::mat_frame_mean(out_mat, in_mat);
  case Aggregate::VAR:
    return taMath_float::mat_frame_var(out_mat, in_mat);
  case Aggregate::SS:
    return taMath_float::mat_frame_ss_mean(out_mat, in_mat);
  case Aggregate::STDEV:
    return taMath_float::mat_frame_std_dev(out_mat, in_mat);
  case Aggregate::SEM:
    return taMath_float::mat_frame_sem(out_mat, in_mat);
  case Aggregate::N:
    return taMath_float::mat_frame_set_n(out_mat, in_mat);
  case Aggregate::COUNT:
    return taMath_float::mat_frame_count(out_mat, in_mat, agg.rel);
  case Aggregate::MEDIAN:
    return taMath_float::mat_frame_median(out_mat, in_mat);
  case Aggregate::MODE:
    return taMath_float::mat_frame_mode(out_mat, in_mat);
  case Aggregate::QUANTILE:
    return taMath_float::mat_frame_quantile(out_mat, in_mat, agg.rel.val);
  case Aggregate::NONE:
    return true;
  }
  return false;
}
