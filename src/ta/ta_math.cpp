// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "ta_math.h"

/////////////////////////////////////////////////////////////////////////////////
// Params: parameters controlling various math functions

int taMath::max_iterations = 100;
double taMath::err_tolerance = 3.0e-7;

/////////////////////////////////////////////////////////////////////////////////
// Arithmetic: basic arithmatic

double taMath::FLT_MAX = FLT_MAX;
double taMath::FLT_MAX_10_EXP = FLT_MAX_10_EXP;
double taMath::FLT_MAX_EXP = FLT_MAX_EXP;

double taMath::FLT_MIN = FLT_MIN;
double taMath::FLT_MIN_10_EXP = FLT_MIN_10_EXP;
double taMath::FLT_MIN_EXP = FLT_MIN_EXP;
double taMath::FLT_EPSILON = FLT_EPSILON;

double taMath::DBL_MAX = DBL_MAX;
double taMath::DBL_MAX_10_EXP = DBL_MAX_10_EXP;
double taMath::DBL_MAX_EXP = DBL_MAX_EXP;

double taMath::DBL_MIN = DBL_MIN;
double taMath::DBL_MIN_10_EXP = DBL_MIN_10_EXP;
double taMath::DBL_MIN_EXP = DBL_MIN_EXP;
double taMath::DBL_EPSILON = DBL_EPSILON;

// double taMath::INT_MAX = INT_MAX;
// double taMath::INT_MIN = INT_MIN;
// double taMath::LONG_MAX = LONG_MAX;
// double taMath::LONG_MIN = LONG_MIN;

/////////////////////////////////////////////////////////////////////////////////
// ExpLog: exponential and logarithmic functions

double taMath::e = M_E;

/////////////////////////////////////////////////////////////////////////////////
// Trigonometry

double taMath::pi = M_PI;
double taMath::deg_per_rad = 180.0 / M_PI;

double taMath::fact_ln(int n) {
  static double_Array table;

  if(n < 0) { fprintf(stderr, "Negative factorial fact_ln()\n"); return 0; }
  if(n <= 1) return 0.0;
  if(n < table.size) return table[n] ? table[n] : (table[n] = gamma_ln(n + 1.0));

  table.Alloc(n+1);		// allocate new size
  int i=table.size;
  for(; i<=n; i++)
    table.FastEl(i) = 0.0;
  table.size = n+1;

  return (table[n] = gamma_ln(n + 1.0));
}

double taMath::bico_ln(int n, int j) {
  return fact_ln(n)-fact_ln(j)-fact_ln(n-j);
}

double taMath::hyperg(int j, int s, int t, int n) {
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

  gln=taMath::gamma_ln(a);
  if (x <= 0.0) {
    if (x < 0.0) { fprintf(stderr, "x < 0 in gser()\n"); return 0; }
    return 0;
  }
  else {
    ap=a;
    del=sum=1.0/a;
    for (n=1;n<=taMath::max_iterations;n++) {
      ap += 1.0;
      del *= x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*taMath::err_tolerance)
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

  gln=taMath::gamma_ln(a);
  a1=x;
  for (n=1;n<=taMath::max_iterations;n++) {
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
      if (fabs((g-gold)/g) < taMath::err_tolerance)
	return exp(-x+a*log(x)-(gln))*g;
      gold=g;
    }
  }
  fprintf(stderr, "a too large, max_iterations too small in gcf()\n");
  return 0;
}

double taMath::gamma_ln(double z) {
  double x,tmp,ser;		/* make sure double-precision.. */
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

double taMath::gamma_p(double a, double x) {
  if (x < 0.0 || a <= 0.0) { fprintf(stderr, "Invalid args in gamma_p()\n"); return 0; }

  if (x < (a+1.0))
    return gser(a,x);
  else
    return 1.0 - gcf(a,x);
}

double taMath::gamma_q(double a, double x) {
  if (x < 0.0 || a <= 0.0) { fprintf(stderr, "Invalid args in gamma_q()\n"); return 0; }
  if (x < (a+1.0))
    return 1.0 - gser(a,x);
  else
    return gcf(a,x);
}

/**********************************
  beta function
***********************************/

static double betacf(double a, double b, double x) {
  double qap,qam,qab,em,tem,d;
  double bz,bm=1.0,bp,bpp;
  double az=1.0,am=1.0,ap,app,aold;
  int m;

  qab=a+b;
  qap=a+1.0;
  qam=a-1.0;
  bz=1.0-qab*x/qap;
  for (m=1;m<=taMath::max_iterations;m++) {
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
    if (fabs(az-aold) < (taMath::err_tolerance*fabs(az))) return az;
  }
  fprintf(stderr, "a or b too big, or max_iterations too small in betacf()\n");
  return 0;
}

double taMath::beta(double z, double w) {
  return exp(gamma_ln(z) + gamma_ln(w) - gamma_ln(z + w));
}

double taMath::beta_i(double a, double b, double x) {
  double bt;

  if (x < 0.0 || x > 1.0) { fprintf(stderr, "Bad x in beta_i()\n"); return 0; }
  if (x == 0.0 || x == 1.0) bt=0.0;
  else
    bt=exp(gamma_ln(a+b)-gamma_ln(a)-gamma_ln(b)+a*log(x)+b*log(1.0-x));
  if (x < (a+1.0)/(a+b+2.0))
    return bt*betacf(a,b,x)/a;
  else
    return 1.0-bt*betacf(b,a,1.0-x)/b;
}


/**********************************
  the binomial distribution
***********************************/

double taMath::binom_den(int n, int j, double p) {
  if(j > n) { fprintf(stderr, "j > n in binom()\n"); return 0; }
  return exp(bico_ln(n,j) + (double)j * log(p) + (double)(n-j) * log(1.0 - p));
}

double taMath::binom_cum(int n, int k, double p) {
  if(k > n) 	{ fprintf(stderr, "k > n in binom_cum()\n"); return 0; }
  return beta_i(k, n-k + 1, p);
}

double taMath::binom_dev(int n, double pp) {
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

double taMath::poisson_den(int j, double l) {
  return exp((double)j * log(l) - fact_ln(j) - l);
}

double taMath::poisson_cum(int j, double x) {
  if(x < 0.0)	{ fprintf(stderr, "x < 0 in poisson_cum()\n"); return 0; }
  if(j > 0)
    return gamma_q(j, x);
  else
    return 0;
}

double taMath::poisson_dev(double xm) {
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

double taMath::gamma_den(int j, double l, double t) {
  if(t < 0) return 0;
  return exp((double)j * log(l) + (double)(j-1) * log(t) - gamma_ln(j) - (l * t));
}

double taMath::gamma_cum(int j, double l, double t) {
  return gamma_p(j, l * t);
}

double taMath::gamma_dev(int ia) {
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

double taMath::gauss_den(double z) {
  return 0.398942280 * exp(-0.5 * z * z);
}

double taMath::gauss_cum(double z) {
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

double taMath::gauss_inv(double p) {
  double	minz = -6.0;
  double	maxz = 6.0;
  double	zval = 0.0;
  double	pval;

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


double taMath::gauss_dev() {
  static int iset=0;
  static double gset;
  double fac,r,v1,v2;

  if(iset==0) {
    do {
      v1=2.0*MTRnd::genrand_res53()-1.0;
      v2=2.0*MTRnd::genrand_res53()-1.0;
      r=v1*v1+v2*v2;
    } while ((r >= 1.0) || (r == 0));

    fac=sqrt((-2.0*log(r))/r);
    gset=v1*fac;
    iset=1;
    return (double)(v2*fac);
  }
  else {
    iset=0;
    return gset;
  }
}

/**********************************
  misc statistical
***********************************/

double taMath::chisq_p(double X, double v) {
  return gamma_p(0.5 * v, 0.5 * X * X);
}

double taMath::chisq_q(double X, double v) {
  return gamma_q(0.5 * v, 0.5 * X * X);
}

double taMath::students_cum(double t, double df) {
  return 1.0 - beta_i(0.5*df, 0.5, df / (df + t * t));
}

double taMath::students_den(double t, double df) {
  // just use discrete approximation..
  return (students_cum(t + 1.0e-6, df) - students_cum(t - 1.0e-6, df)) / 4.0e-6;
}

double taMath::Ftest_q(double F, double v1, double v2) {
  return beta_i(0.5*v2, 0.5*v1, v2 / (v2 + (v1 * F)));
}



/////////////////////////////////////////////////////////////////////////////////
// Matrix operations (note: only float, not double, is supported

#ifdef HAVE_GSL

bool taMath::get_gsl_matrix_fm_ta(const double_Matrix* ta_mat, gsl_matrix* gsl_mat)
{
  if(ta_mat->dims() != 2) return false;
  gsl_mat->size1 = ta_mat->dim(0); // "rows" (rows are contiguous in mem)
  gsl_mat->size2 = ta_mat->dim(1); // "columns"
  gsl_mat->tda = ta_mat->dim(0); // actual size of row in memory
  gsl_mat->data = (double*)ta_mat->data();
  gsl_mat->block = NULL;
  gsl_mat->owner = false;
  return true;
}

bool taMath::get_gsl_matrix_fm_ta_f(float_Matrix* ta_mat, gsl_matrix_float* gsl_mat) 
{
  if(ta_mat->dims() != 2) return false;
  gsl_mat->size1 = ta_mat->dim(0); // "rows" (rows are contiguous in mem)
  gsl_mat->size2 = ta_mat->dim(1); // "columns"
  gsl_mat->tda = ta_mat->dim(0); // actual size of row in memory
  gsl_mat->data = (float*)ta_mat->data();
  gsl_mat->block = NULL;
  gsl_mat->owner = false;
  return true;
}

bool taMath::matrix_add(double_Matrix* a, double_Matrix* b) {
  gsl_matrix g_a;  get_gsl_matrix_fm_ta_f(a, &g_a);
  gsl_matrix g_b;  get_gsl_matrix_fm_ta_f(b, &g_b);
  return gsl_matrix_add(&g_a, &g_b);
}

bool taMath::matrix_sub(double_Matrix* a, double_Matrix* b) {
  gsl_matrix g_a;  get_gsl_matrix_fm_ta_f(a, &g_a);
  gsl_matrix g_b;  get_gsl_matrix_fm_ta_f(b, &g_b);
  return gsl_matrix_sub(&g_a, &g_b);
}

bool taMath::matrix_mult_els(double_Matrix* a, double_Matrix* b) {
  gsl_matrix g_a;  get_gsl_matrix_fm_ta_f(a, &g_a);
  gsl_matrix g_b;  get_gsl_matrix_fm_ta_f(b, &g_b);
  return gsl_matrix_mul_elements(&g_a, &g_b);
}

bool taMath::matrix_div_els(double_Matrix* a, double_Matrix* b) {
  gsl_matrix g_a;  get_gsl_matrix_fm_ta_f(a, &g_a);
  gsl_matrix g_b;  get_gsl_matrix_fm_ta_f(b, &g_b);
  return gsl_matrix_div_elements(&g_a, &g_b);
}

//

bool taMath::matrix_add_f(float_Matrix* a, float_Matrix* b) {
  gsl_matrix_float g_a;  get_gsl_matrix_fm_ta_f(a, &g_a);
  gsl_matrix_float g_b;  get_gsl_matrix_fm_ta_f(b, &g_b);
  return gsl_matrix_float_add(&g_a, &g_b);
}

bool taMath::matrix_sub_f(float_Matrix* a, float_Matrix* b) {
  gsl_matrix_float g_a;  get_gsl_matrix_fm_ta_f(a, &g_a);
  gsl_matrix_float g_b;  get_gsl_matrix_fm_ta_f(b, &g_b);
  return gsl_matrix_float_sub(&g_a, &g_b);
}

bool taMath::matrix_mult_els_f(float_Matrix* a, float_Matrix* b) {
  gsl_matrix_float g_a;  get_gsl_matrix_fm_ta_f(a, &g_a);
  gsl_matrix_float g_b;  get_gsl_matrix_fm_ta_f(b, &g_b);
  return gsl_matrix_float_mul_elements(&g_a, &g_b);
}

bool taMath::matrix_div_els_f(float_Matrix* a, float_Matrix* b) {
  gsl_matrix_float g_a;  get_gsl_matrix_fm_ta_f(a, &g_a);
  gsl_matrix_float g_b;  get_gsl_matrix_fm_ta_f(b, &g_b);
  return gsl_matrix_float_div_elements(&g_a, &g_b);
}

#endif
