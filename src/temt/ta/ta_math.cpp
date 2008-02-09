// Copyright, 1995-2007, Regents of the University of Colorado,
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

#ifdef _WINDOWS //note: have to use the raw ms switch, since our own file isn't loaded
// note: MSVC requires the following on first read of math.h to enable defines
// we have to put this absolutely first to insure it is defined for the first read
// of math.h, which could occur anywhere in any included file
# define _USE_MATH_DEFINES
#endif

#include "ta_math.h"
#include "ta_platform.h"

#ifdef HAVE_LIBGSL
# include <gsl/gsl_errno.h>
# include <gsl/gsl_eigen.h>
# include <gsl/gsl_linalg.h>
# include <gsl/gsl_fft_real.h>
# include <gsl/gsl_fft_halfcomplex.h>
#endif

#include <math.h>

#include <float.h>

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


//////////////////////////
//  	Relation     	//
//////////////////////////

void Relation::Initialize() {
  rel = LESSTHANOREQUAL;
  val = 0.0;
  use_var = false;
}

bool Relation::CacheVar(Relation& tmp_rel) {
  tmp_rel.rel = rel;
  if(use_var && (bool)var) {
    tmp_rel.val = var->GetVar().toDouble();
    return true;
  }
  tmp_rel.val = val;
  return false;
}

bool Relation::Evaluate(double cmp) const {
  double eff_val = val;
  if(use_var && (bool)var) {
    eff_val = var->GetVar().toDouble();
  }
    
  switch(rel) {
  case EQUAL:
    if(cmp == eff_val)	return true;
    break;
  case NOTEQUAL:
    if(cmp != eff_val)	return true;
    break;
  case LESSTHAN:
    if(cmp < eff_val)	return true;
    break;
  case GREATERTHAN:
    if(cmp > eff_val)	return true;
    break;
  case LESSTHANOREQUAL:
    if(cmp <= eff_val)	return true;
    break;
  case GREATERTHANOREQUAL:
    if(cmp >= eff_val)	return true;
    break;
  }
  return false;
}


//////////////////////////
//  	Aggregate    	//
//////////////////////////

void Aggregate::Initialize() {
  op = MEAN;
}

void Aggregate::Destroy() {
}

String Aggregate::GetAggName() const {
  return GetTypeDef()->GetEnumString("Operator", op);
}

bool Aggregate::RealVal() const {
  if(op == MEAN || op == VAR || op == STDEV || op == SEM) return true;
  return false;
}

//////////////////////////
//  	SimpleMathSpec 	//
//////////////////////////

void SimpleMathSpec::Initialize() {
  opr = NONE;
  arg = 0.0;
  lw = -1.0;
  hi = 1.0;
}

double SimpleMathSpec::Evaluate(double val) const {
  switch(opr) {
  case NONE:
    return val;
  case THRESH:
    return (val >= arg) ? hi : lw;
  case ABS:
    return fabsf(val);
  case SQUARE:
    return val * val;
  case SQRT:
    return sqrtf(val);
  case LOG:
    return logf(val);
  case LOG10:
    return ((val <= 0) ? taMath::flt_min_10_exp : log10(val));
  case EXP:
    return expf(val);
  case ADD:
    return val + arg;
  case SUB:
    return val - arg;
  case MUL:
    return val * arg;
  case DIV:
    return val / arg;
  case POWER:
    return powf(val, arg);
  case MAX:
    return MAX(val, arg);
  case MIN:
    return MIN(val, arg);
  case MINMAX:
    val = MIN(val, hi);
    return MAX(val, lw);
  }
  return val;
}

Variant& SimpleMathSpec::EvaluateVar(Variant& val) const {
  // we just detect the NONE case to avoid conversions, otherwise we just double it!
  if (opr == NONE) return val;
  val = Evaluate(val.toDouble());
  return val;
}

/////////////////////////////////////////////////////////////////////////////////
// Params: parameters controlling various math functions

int taMath::max_iterations = 100;
double taMath::err_tolerance = 3.0e-7;

/////////////////////////////////////////////////////////////////////////////////
// Arithmetic: basic arithmatic

float taMath::flt_max = FLT_MAX;
float taMath::flt_max_10_exp = FLT_MAX_10_EXP;
float taMath::flt_max_exp = FLT_MAX_EXP;

float taMath::flt_min = FLT_MIN;
float taMath::flt_min_10_exp = FLT_MIN_10_EXP;
float taMath::flt_min_exp = FLT_MIN_EXP;
float taMath::flt_epsilon = FLT_EPSILON;

double taMath::dbl_max = DBL_MAX;
double taMath::dbl_max_10_exp = DBL_MAX_10_EXP;
double taMath::dbl_max_exp = DBL_MAX_EXP;

double taMath::dbl_min = DBL_MIN;
double taMath::dbl_min_10_exp = DBL_MIN_10_EXP;
double taMath::dbl_min_exp = DBL_MIN_EXP;
double taMath::dbl_epsilon = DBL_EPSILON;

// double taMath::INT_MAX = INT_MAX;
// double taMath::INT_MIN = INT_MIN;
// double taMath::LONG_MAX = LONG_MAX;
// double taMath::LONG_MIN = LONG_MIN;

bool taMath::dist_larger_further(DistMetric metric) {
  switch(metric) {
  case SUM_SQUARES:
    return true;
  case EUCLIDIAN:
    return true;
  case HAMMING:
    return true;
  case COVAR:
    return false;
  case CORREL:
    return false;
  case INNER_PROD:
    return false;
  case CROSS_ENTROPY:
    return false;
  }
  return false;
}

void taMath::mat_cvt_double_to_float(float_Matrix* flt_dest, const double_Matrix* dbl_src) {
  flt_dest->geom.Reset();
  flt_dest->SetGeomN(dbl_src->geom);
  for (int i = 0; i < flt_dest->size; ++i) {
    flt_dest->FastEl_Flat(i) = (float)dbl_src->FastEl_Flat(i);
  }
}

void taMath::mat_cvt_float_to_double(double_Matrix* dbl_dest, const float_Matrix* flt_src) {
  dbl_dest->geom.Reset();
  dbl_dest->SetGeomN(flt_src->geom);
  for (int i = 0; i < dbl_dest->size; ++i) {
    dbl_dest->FastEl_Flat(i) = (double)flt_src->FastEl_Flat(i);
  }
}


////////////////////////////////////////////////////////////////////////////////
// 			double precision math
////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// ExpLog: exponential and logarithmic functions

double taMath_double::e = M_E;

/////////////////////////////////////////////////////////////////////////////////
// Trigonometry

double taMath_double::pi = M_PI;
double taMath_double::deg_per_rad = 180.0 / M_PI;

/////////////////////////////////////////////////////////////////////////////////
// Probability distributions, etc

double taMath_double::fact_ln(int n) {
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
  if(k > n) 	{ fprintf(stderr, "k > n in binom_cum()\n"); return 0; }
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
  if(x < 0.0)	{ fprintf(stderr, "x < 0 in poisson_cum()\n"); return 0; }
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


double taMath_double::gauss_dev() {
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

double taMath_double::chisq_p(double X, double v) {
  return gamma_p(0.5 * v, 0.5 * X * X);
}

double taMath_double::chisq_q(double X, double v) {
  return gamma_q(0.5 * v, 0.5 * X * X);
}

double taMath_double::students_cum(double t, double df) {
  return 1.0 - beta_i(0.5*df, 0.5, df / (df + t * t));
}

double taMath_double::students_den(double t, double df) {
  // just use discrete approximation..
  return (students_cum(t + 1.0e-6, df) - students_cum(t - 1.0e-6, df)) / 4.0e-6;
}

double taMath_double::Ftest_q(double F, double v1, double v2) {
  return beta_i(0.5*v2, 0.5*v1, v2 / (v2 + (v1 * F)));
}



/////////////////////////////////////////////////////////////////////////////////
// Vector operations (operate on Matrix objects, treating as a single linear guy)

///////////////////////////////////////
// arithmetic ops

bool taMath_double::vec_check_same_size(const double_Matrix* a, const double_Matrix* b, bool quiet) {
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

bool taMath_double::vec_add(double_Matrix* a, const double_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) += b->FastEl_Flat(i);
  return true;
}

bool taMath_double::vec_sub(double_Matrix* a, const double_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) -= b->FastEl_Flat(i);
  return true;
}

bool taMath_double::vec_mult_els(double_Matrix* a, const double_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) *= b->FastEl_Flat(i);
  return true;
}

bool taMath_double::vec_div_els(double_Matrix* a, const double_Matrix* b) {
  if(!vec_check_same_size(a, b)) return false;
  for(int i=0;i<a->size;i++) {
    double bv = b->FastEl_Flat(i);
    if(bv != 0.0)
      a->FastEl_Flat(i) /= bv;
  }
  return true;
}

bool taMath_double::vec_add_scalar(double_Matrix* a, double b) {
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) += b;
  return true;
}

bool taMath_double::vec_sub_scalar(double_Matrix* a, double b) {
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) -= b;
  return true;
}

bool taMath_double::vec_mult_scalar(double_Matrix* a, double b) {
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) *= b;
  return true;
}

bool taMath_double::vec_div_scalar(double_Matrix* a, double b) {
  if(b == 0.0) return false;
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) /= b;
  return true;
}

bool taMath_double::vec_simple_math(double_Matrix* vec, const SimpleMathSpec& math_spec) {
  for(int i=0;i<vec->size;i++)
    vec->FastEl_Flat(i) = math_spec.Evaluate(vec->FastEl_Flat(i));
  return true;
}

bool taMath_double::vec_simple_math_arg(double_Matrix* vec, const double_Matrix* arg_ary,
					const SimpleMathSpec& math_spec) {
  if(!vec_check_same_size(vec, arg_ary)) return false;
  SimpleMathSpec myms = math_spec;
  for(int i=0;i<vec->size;i++) {
    myms.arg = arg_ary->FastEl_Flat(i);
    vec->FastEl_Flat(i) = myms.Evaluate(vec->FastEl_Flat(i));
  }
  return true;
}

double taMath_double::vec_first(const double_Matrix* vec) {
  if(vec->size == 0) return 0.0;
  return vec->FastEl_Flat(0);
}

double taMath_double::vec_last(const double_Matrix* vec) {
  if(vec->size == 0) return 0.0;
  return vec->FastEl_Flat(vec->size-1);
}

int taMath_double::vec_find_first(const double_Matrix* vec, Relation& rel) {
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  for(int i=0;i<vec->size;i++) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) return i;
  }
  return -1;
}

int taMath_double::vec_find_last(const double_Matrix* vec, Relation& rel) {
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  for(int i=vec->size-1;i>=0;i--) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) return i;
  }
  return -1;
}

double taMath_double::vec_max(const double_Matrix* vec, int& idx) {
  idx = 0;
  if(vec->size == 0) return 0.0;
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
  idx = 0;
  if(vec->size == 0) return 0.0;
  double rval = fabs(vec->FastEl_Flat(0));
  for(int i=1;i<vec->size;i++) {
    if(fabs(vec->FastEl_Flat(i)) > rval) {
      idx = i;
      rval = fabs(vec->FastEl_Flat(i));
    }
  }
  return rval;
}

double taMath_double::vec_min(const double_Matrix* vec, int& idx) {
  idx = 0;
  if(vec->size == 0) return 0.0;
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
  idx = 0;
  if(vec->size == 0) return 0.0;
  double rval = fabs(vec->FastEl_Flat(0));
  for(int i=1;i<vec->size;i++) {
    if(fabs(vec->FastEl_Flat(i)) < rval) {
      idx = i;
      rval = fabs(vec->FastEl_Flat(i));
    }
  }
  return rval;
}

double taMath_double::vec_sum(const double_Matrix* vec) {
  double rval = 0.0;
  for(int i=0;i<vec->size;i++)
    rval += vec->FastEl_Flat(i);
  return rval;
}

double taMath_double::vec_prod(const double_Matrix* vec) {
  if(vec->size == 0) return 0.0;
  double rval = vec->FastEl_Flat(0);
  for(int i=1;i<vec->size;i++)
    rval *= vec->FastEl_Flat(i);
  return rval;
}

double taMath_double::vec_mean(const double_Matrix* vec) {
  if(vec->size == 0)	return 0.0;
  return vec_sum(vec) / (double)vec->size;
}

double taMath_double::vec_var(const double_Matrix* vec, double mean, bool use_mean) {
  if(vec->size == 0)	return 0.0;
  if(!use_mean)    mean = vec_mean(vec);
  double rval = 0.0;
  for(int i=0;i<vec->size;i++)
    rval += (vec->FastEl_Flat(i) - mean) * (vec->FastEl_Flat(i) - mean);
  return rval / (double)vec->size;
}

double taMath_double::vec_std_dev(const double_Matrix* vec, double mean, bool use_mean) {
  return sqrt(vec_var(vec, mean, use_mean));
}

double taMath_double::vec_sem(const double_Matrix* vec, double mean, bool use_mean) {
  if(vec->size == 0)	return 0.0;
  return vec_std_dev(vec, mean, use_mean) / sqrt((double)vec->size);
}

double taMath_double::vec_ss_len(const double_Matrix* vec) {
  double rval = 0.0;
  for(int i=0;i<vec->size;i++)
    rval += vec->FastEl_Flat(i) * vec->FastEl_Flat(i);
  return rval;
}

void taMath_double::vec_histogram(double_Matrix* vec, const double_Matrix* oth, double bin_size) {
  vec->Reset();
  if(oth->size == 0) return;
  double_Array tmp;
  tmp.SetSize(oth->size);
  for(int i=0;i<oth->size;i++)
    tmp[i] = oth->FastEl_Flat(i);
  tmp.Sort();
  double min_v = tmp.FastEl(0);
  double max_v = tmp.Peek();
  int src_idx = 0;
  int trg_idx = 0;
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
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  double rval = 0.0;
  for(int i=0;i<vec->size;i++) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) rval += 1.0;
  }
  return rval;
}

double taMath_double::vec_median(const double_Matrix* vec) {
  if(vec->size == 0) return 0.0f;
  double_Array tmp;
  tmp.SetSize(vec->size);
  for(int i=0;i<vec->size;i++)
    tmp[i] = vec->FastEl_Flat(i);
  tmp.Sort();
  int idx = tmp.size / 2;
  return tmp[idx];
}

double taMath_double::vec_quantile(const double_Matrix* vec, double quant_pos) {
  if(vec->size == 0) return 0.0f;
  double_Array tmp;
  tmp.SetSize(vec->size);
  for(int i=0;i<vec->size;i++)
    tmp[i] = vec->FastEl_Flat(i);
  tmp.Sort();
  int idx = (int)(quant_pos * (double)tmp.size);
  if(idx >= tmp.size) idx = tmp.size-1;
  if(idx < 0) idx = 0;
  return tmp[idx];
}

double taMath_double::vec_mode(const double_Matrix* vec) {
  if(vec->size == 0) return 0.0f;
  double_Array tmp;
  tmp.SetSize(vec->size);
  for(int i=0;i<vec->size;i++)
    tmp[i] = vec->FastEl_Flat(i);
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

static int double_vec_sort_cmp(double v1, double v2) {
  if(v1 > v2) return 1;
  else if(v1 == v2) return 0;
  return -1;
}

void taMath_double::vec_sort(double_Matrix* vec, bool descending) {
  if(vec->size <= 1) return;
  int lt_compval = -1;		// comparison return value for less-than
  if(descending)
    lt_compval = 1;
  // lets do a heap sort since it requires no secondary storage
  int n = vec->size;
  int l,j,ir,i;
  double tmp;

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
      if(j<ir && (double_vec_sort_cmp(vec->FastEl_Flat(j-1),vec->FastEl_Flat(j)) == lt_compval)) j++;
      if(double_vec_sort_cmp(tmp,vec->FastEl_Flat(j-1)) == lt_compval) { // tmp < ra[j]
	vec->FastEl_Flat(i-1) = vec->FastEl_Flat(j-1); // ra[i]=ra[j];
	j += (i=j);
      }
      else j = ir+1;
    }
    vec->FastEl_Flat(i-1) = tmp; // ra[i] = tmp;
  }
}

String taMath_double::vec_stats(const double_Matrix* vec) {
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

///////////////////////////////////////
// distance metrics (comparing two vectors)

double taMath_double::vec_ss_dist(const double_Matrix* vec, const double_Matrix* oth, bool norm,
				  double tolerance)
{
  if(!vec_check_same_size(vec, oth)) return -1.0;
  double rval = 0.0;
  for(int i=0;i<vec->size;i++) {
    double d = vec->FastEl_Flat(i) - oth->FastEl_Flat(i);
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
  for(int i=0;i<vec->size;i++) {
    double d = fabs(vec->FastEl_Flat(i) - oth->FastEl_Flat(i));
    if(d <= tolerance)  d = 0.0;
    rval += d;
    if(norm) {
      alen += fabs(vec->FastEl_Flat(i));
      blen += fabs(oth->FastEl_Flat(i));
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
  for(int i=0;i<vec->size;i++)
    rval += (vec->FastEl_Flat(i) - my_mean) * (oth->FastEl_Flat(i) - oth_mean);
  return rval / (double)vec->size;
}

double taMath_double::vec_correl(const double_Matrix* vec, const double_Matrix* oth) {
  if(!vec_check_same_size(vec, oth)) return -1.0;
  double my_mean = vec_mean(vec);
  double oth_mean = vec_mean(oth);
  double my_var = 0.0;
  double oth_var = 0.0;
  double rval = 0.0;
  for(int i=0;i<vec->size;i++) {
    double my_val = vec->FastEl_Flat(i) - my_mean;
    double oth_val = oth->FastEl_Flat(i) - oth_mean;
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
  for(int i=0;i<vec->size;i++)
    rval += vec->FastEl_Flat(i) * oth->FastEl_Flat(i);
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
  for(int i=0;i<vec->size;i++) {
    double p = vec->FastEl_Flat(i);
    double q = oth->FastEl_Flat(i);
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

///////////////////////////////////////
// Normalization

double taMath_double::vec_norm_len(double_Matrix* vec, double len) {
  if(vec->size == 0) 	return 0.0;
  double scale = (len * len) / vec_ss_len(vec);
  for(int i=0;i<vec->size;i++) {
    double mag = (vec->FastEl_Flat(i) * vec->FastEl_Flat(i)) * scale;
    vec->FastEl_Flat(i) = (vec->FastEl_Flat(i) >= 0.0f) ? mag : -mag;
  }
  return scale;
}

double taMath_double::vec_norm_sum(double_Matrix* vec, double sum, double min_val) {
  if(vec->size == 0)	return 0.0;
  double act_sum = 0.0;
//notused  int min_idx;
  for(int i=0;i<vec->size;i++)
    act_sum += (vec->FastEl_Flat(i) - min_val);
  double scale = (sum / act_sum);
  for(int i=0;i<vec->size;i++)
    vec->FastEl_Flat(i) = ((vec->FastEl_Flat(i) - min_val) * scale) + min_val;
  return scale;
}

double taMath_double::vec_norm_max(double_Matrix* vec, double max) {
  if(vec->size == 0)	return 0.0;
  int idx;
  double cur_max = vec_max(vec, idx);
  if(cur_max == 0.0) return 0.0;
  double scale = (max / cur_max);
  for(int i=0;i<vec->size;i++)
    vec->FastEl_Flat(i) *= scale;
  return scale;
}

double taMath_double::vec_norm_abs_max(double_Matrix* vec, double max) {
  if(vec->size == 0)	return 0.0;
  int idx;
  double cur_max = vec_abs_max(vec, idx);
  if(cur_max == 0.0) return 0.0;
  double scale = (max / cur_max);
  for(int i=0;i<vec->size;i++)
    vec->FastEl_Flat(i) *= scale;
  return scale;
}

int taMath_double::vec_threshold(double_Matrix* vec, double thresh, double low, double high) {
  if(vec->size == 0)  return 0;
  int rval = 0;
  for(int i=0;i<vec->size;i++) {
    if(vec->FastEl_Flat(i) >= thresh) {
      vec->FastEl_Flat(i) = high;
      rval++;
    }
    else
      vec->FastEl_Flat(i) = low;
  }
  return rval;
}

double taMath_double::vec_aggregate(const double_Matrix* vec, Aggregate& agg) {
  int idx;
  switch(agg.op) {
  case Aggregate::GROUP:
    return vec->SafeEl_Flat(0);	// first guy..
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
  case Aggregate::QUANTILE:
    return taMath_double::vec_quantile(vec, agg.rel.val);
  case Aggregate::NONE:
    return 0.0;
  }
  return 0.0;
}

/////////////////////////////////////////////////////////////////////////////////
// Convolution

bool taMath_double::vec_kern_uniform(double_Matrix* kernel, int half_sz,
				     bool neg_tail, bool pos_tail) {
  int sz = half_sz * 2 + 1;
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
      y = exp(-exp_mult * x);	// always count point
    }
    kernel->FastEl(i) = y;
  }
  vec_norm_sum(kernel);
  return true;
}

bool taMath_double::vec_kern_pow(double_Matrix* kernel, int half_sz, double pow_exp,
				 bool neg_tail, bool pos_tail) {
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
      y = 1.0;			// count self as 1
    }
    kernel->FastEl(i) = y;
  }
  vec_norm_sum(kernel);
  return true;
}

bool taMath_double::vec_convolve(double_Matrix* out_vec, const double_Matrix* in_vec,
				   const double_Matrix* kernel, bool keep_edges) {
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

/////////////////////////////////////////////////////////////////////////////////
// Matrix operations

bool taMath_double::mat_col(double_Matrix* col, const double_Matrix* mat, int col_no) {
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
  if(ta_mat->dims() != 2) return false;
  gsl_mat->size1 = ta_mat->dim(0); // "rows" (rows are contiguous in mem)
  gsl_mat->size2 = ta_mat->dim(1); // "columns"
  gsl_mat->tda = ta_mat->dim(0); // actual size of row in memory
  gsl_mat->data = (double*)ta_mat->data();
  gsl_mat->block = NULL;
  gsl_mat->owner = false;
  return true;
}

bool taMath_double::vec_get_gsl_fm_ta(gsl_vector* gsl_vec, const double_Matrix* ta_vec) {
  if(ta_vec->dims() != 1) return false;
  gsl_vec->size = ta_vec->dim(0); 
  gsl_vec->stride = 1;
  gsl_vec->data = (double*)ta_vec->data();
  gsl_vec->block = NULL;
  gsl_vec->owner = false;
  return true;
}

bool taMath_double::mat_add(double_Matrix* a, const double_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(&g_a, a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(&g_b, b);
  return gsl_matrix_add(&g_a, &g_b);
}

bool taMath_double::mat_sub(double_Matrix* a, const double_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(&g_a, a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(&g_b, b);
  return gsl_matrix_sub(&g_a, &g_b);
}

bool taMath_double::mat_mult_els(double_Matrix* a, const double_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(&g_a, a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(&g_b, b);
  return gsl_matrix_mul_elements(&g_a, &g_b);
}

bool taMath_double::mat_div_els(double_Matrix* a, const double_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(&g_a, a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(&g_b, b);
  return gsl_matrix_div_elements(&g_a, &g_b);
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
  double_Matrix a_copy = *a;
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
  *u = *a;			// u is copy of a
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

  double_Matrix evals;
  double_Matrix evecs;
  mat_eigen_owrite(a, &evals, &evecs);

  xy_coords->SetGeom(2, 2, dim);
  // multiply the eigenvectors by sqrt(eigen values)
  double evsq = taMath_double::sqrt(taMath_double::fabs(evals.FastEl(x_axis_c)));
  for(int i=0;i<dim;i++) {
    float val = evecs.FastEl(i, x_axis_c) * evsq;
    xy_coords->FastEl(0, i) = val;	// 0 = x coord
  }

  evsq = taMath_double::sqrt(taMath_double::fabs(evals.FastEl(y_axis_c)));
  for(int i=0;i<dim;i++) {
    float val = evecs.FastEl(i, y_axis_c) * evsq;
    xy_coords->FastEl(1, i) = val;	// 1 = y coord
  }

  return true;
}

bool taMath_double::mat_mds(const double_Matrix* a, double_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  double_Matrix a_copy = *a;
  return mat_mds_owrite(&a_copy, xy_coords, x_axis_c, y_axis_c);
}

/* bool taMath_double::fft_real_wavetable(double_Matrix* out_mat, int n) {
  if (!out_mat || (n <= 0)) return false;
  gsl_fft_real_wavetable* gwt = gsl_fft_real_wavetable_alloc((size_t)n);
  //note: unknown length, and only needed opaquely, so size=0 is set to prevent access
  MatrixGeom geom(1, 0);
  out_mat->SetFixedData((double*)gwt, geom,
    (fixed_dealloc_fun)gsl_fft_real_wavetable_free);
  return true;
}*/

bool taMath_double::fft_real_transform(double_Matrix* out_mat,
  const double_Matrix* in_mat, bool real_out, bool norm)
{
  if (!out_mat || !in_mat) return false;
  int n = in_mat->dim(0); // inner guy is frame size
  if (n == 0) return false; // huh?
  // calculate the number of flat d0 frames, often just 1
  int frames = in_mat->size / n;
  
  // set dims on output -- we provide full redundant real/complex
  // note: we will reduce later if user requested real_out
  // note: out_geom needs to remain valid for use later in reduction
  MatrixGeom out_geom(in_mat->dims() + 1);
  out_geom.Set(0, 2); // real/imag
  for (int i = 0; i < in_mat->dims(); ++i) {
    out_geom.Set(i+1, in_mat->dim(i));
  }
  out_mat->SetGeomN(out_geom);
  
  
  // make scratch
  // declare all, so we can jump to exit
  bool rval = false; 
  gsl_fft_real_wavetable* gwt = NULL;
  gsl_fft_real_workspace* work = NULL;
  double* data = NULL;
  const size_t stride = 1;
  
  gwt = gsl_fft_real_wavetable_alloc((size_t)n);
  if (!gwt) goto exit; // unlikely
  work = gsl_fft_real_workspace_alloc((size_t) n);
  if (!work) goto exit; // unlikely
  data = new double[n];
  if (!data) goto exit; // unlikely
    
  
  // do the fft for each frame, typically just 1
  for (int fr = 0; fr < frames; ++fr) {
    // copy the in data
    memcpy(data, in_mat->FastEl_Flat_(fr * n),
      sizeof(double) * n);
    // do the fft itself -- returns result in-place
    int gsl_errno = gsl_fft_real_transform (
      data,
      stride, (size_t) n,
      gwt,  work);
    if (gsl_errno != 0) {
      taMisc::Warning("taMath_double::fft_real_transform",
        gsl_strerror(gsl_errno));
      goto exit;
    }
    
    // descramble the result into the output mat -- our format is compatible
    void* complex_out = out_mat->FastEl_Flat_(fr * n * 2);
    gsl_errno = gsl_fft_halfcomplex_unpack (
      data,
      (gsl_complex_packed_array) complex_out, stride, (size_t) n);
    if (gsl_errno != 0) {
      taMisc::Warning("taMath_double::fft_real_transform",
        gsl_strerror(gsl_errno));
      goto exit;
    }
    
    // results are scaled by N -- we usually want to renormalize by 1/sqrt(N)
    if (norm) for (int i = 0, idx = fr * n * 2; i < (2 * n); ++i, ++idx) {
      out_mat->FastEl_Flat(idx) /= (sqrt(n));
    }
  }
  
  // if user wants real only, then reduce here
  // our in-place approach is a bit sleazy, but works...
  if (real_out) {
    for (int o = 0, in = 0; o < (frames * n); ++o, in += 2) {
      out_mat->FastEl_Flat(o) = sqrt(
        (pow(out_mat->FastEl_Flat(in),2)) +
        (pow(out_mat->FastEl_Flat(in+1),2)));
    }
    // now redim, which will preserve the data -- we can just reuse geom
    out_geom.SetSize(out_mat->dims() - 1); // reduce 1 dim
    for (int i = 1; i < out_mat->dims(); ++i) 
      out_geom.Set(i-1, out_mat->dim(i));
    
    out_mat->SetGeomN(out_geom);
  }
  
  rval = true;
exit:
  // cleanup
  delete[] data; // ok if NULL
  if (work) gsl_fft_real_workspace_free(work);
  if (gwt) gsl_fft_real_wavetable_free(gwt);
  return rval;
}


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

// bool taMath_double::fft_real_wavetable(double_Matrix* out_mat, int n) {
//   return false;
// }

bool taMath_double::fft_real_transform(double_Matrix* out_mat,
				       const double_Matrix* in_mat, bool real_out, bool norm)
{
  return false;
}

#endif	// HAVE_LIBGSL

bool taMath_double::mat_dist(double_Matrix* dist_mat, const double_Matrix* src_mat,
			     DistMetric metric, bool norm, double tolerance) {
  if(src_mat->dims() < 2) {
    taMisc::Error("*** mat_dist: matrix is < 2 dimensional!");
    return false;
  }
  int n = src_mat->frames();
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

bool taMath_double::mat_correl(double_Matrix* correl_mat, const double_Matrix* src_mat) {
  if(src_mat->dims() < 2) {
    taMisc::Error("*** mat_correl: src_matrix is < 2 dimensional!");
    return false;
  }

  int n = src_mat->frameSize();
  correl_mat->SetGeom(2, n, n);

  double_Matrix p1vals;
  double_Matrix p2vals;

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
  if(avg_dt < 0.0f || avg_dt > 1.0f) {
    taMisc::Error("mat_time_avg: avg_dt is not in 0..1 range:", String(avg_dt));
    return false;
  }
  double avg_dt_c = 1.0 - avg_dt;
  for(int i=1;i<a->frames();i++) {
    int fr_idx = a->FrameStartIdx(i);
    int frm1_idx = a->FrameStartIdx(i-1);
    for(int j=0;j<a->frameSize();j++) {
      a->FastEl_Flat(fr_idx + j) = avg_dt * a->FastEl_Flat(frm1_idx + j) + avg_dt_c * a->FastEl_Flat(fr_idx + j);
    }
  }
  return true;
}

bool taMath_double::mat_frame_convolve(double_Matrix* out_vec, const double_Matrix* in_vec,
				       const double_Matrix* kernel) {
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

bool taMath_double::mat_fmt_out_frame(double_Matrix* out_mat, const double_Matrix* in_mat) {
  MatrixGeom frg = in_mat->geom;
  if(frg.size == 1 || in_mat->frames() == 0) return false;
  frg.SetSize(frg.size-1);	// nuke last dim
  out_mat->SetGeomN(frg);
  return true;
}

bool taMath_double::mat_frame_set_n(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  double frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = frn;
  }
  return true;
}

bool taMath_double::mat_frame_first(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = in_mat->FastEl_Flat(i);
  }
  return true;
}

bool taMath_double::mat_frame_last(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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

bool taMath_double::mat_frame_find_last(double_Matrix* out_mat, const double_Matrix* in_mat,
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

bool taMath_double::mat_frame_max(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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
  double nrm = 1.0 / (double)in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) *= nrm;
  }
  return true;
}

bool taMath_double::mat_frame_var(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  double nrm = 1.0 / (double)frn;
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

bool taMath_double::mat_frame_std_dev(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_frame_var(out_mat, in_mat)) return false;
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = sqrt(out_mat->FastEl_Flat(i));
  }
  return true;
}

bool taMath_double::mat_frame_sem(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_frame_std_dev(out_mat, in_mat)) return false;
  int frs = in_mat->frameSize();
  double nrm = 1.0 / sqrt((double)in_mat->frames());
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) *= nrm;
  }
  return true;
}

bool taMath_double::mat_frame_ss_len(double_Matrix* out_mat, const double_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    double sum = 0.0f;
    for(int j=0;j<frn;j++) {
      sum += in_mat->FastEl_Flat(j * frs + i) * in_mat->FastEl_Flat(j * frs + i);
    }
    out_mat->FastEl_Flat(i) = sum;
  }
  return true;
}

bool taMath_double::mat_frame_count(double_Matrix* out_mat, const double_Matrix* in_mat, Relation& rel) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
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

////////////////////////////////////////////////////////////////////////////////
// 			single precision math
////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// ExpLog: exponential and logarithmic functions

float taMath_float::e = (float)M_E;

/////////////////////////////////////////////////////////////////////////////////
// Trigonometry

float taMath_float::pi = (float)M_PI;
float taMath_float::deg_per_rad = (float)(180.0 / M_PI);


/////////////////////////////////////////////////////////////////////////////////
// Probability distributions, etc

float taMath_float::fact_ln(int n) {
  static float_Array table;

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
  float x,tmp,ser;		/* make sure float-precision.. */
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
  if(k > n) 	{ fprintf(stderr, "k > n in binom_cum()\n"); return 0; }
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
  if(x < 0.0)	{ fprintf(stderr, "x < 0 in poisson_cum()\n"); return 0; }
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
  return ::erf(x);
}

float taMath_float::erfc(float x) {
  return ::erfc(x);
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
  float	minz = -6.0;
  float	maxz = 6.0;
  float	zval = 0.0;
  float	pval;

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


float taMath_float::gauss_dev() {
  static int iset=0;
  static float gset;
  float fac,r,v1,v2;

  if(iset==0) {
    do {
      v1=2.0*MTRnd::genrand_res53()-1.0;
      v2=2.0*MTRnd::genrand_res53()-1.0;
      r=v1*v1+v2*v2;
    } while ((r >= 1.0) || (r == 0));

    fac=sqrt((-2.0*log(r))/r);
    gset=v1*fac;
    iset=1;
    return (float)(v2*fac);
  }
  else {
    iset=0;
    return gset;
  }
}

/**********************************
  misc statistical
***********************************/

float taMath_float::chisq_p(float X, float v) {
  return gamma_p(0.5 * v, 0.5 * X * X);
}

float taMath_float::chisq_q(float X, float v) {
  return gamma_q(0.5 * v, 0.5 * X * X);
}

float taMath_float::students_cum(float t, float df) {
  return 1.0 - beta_i(0.5*df, 0.5, df / (df + t * t));
}

float taMath_float::students_den(float t, float df) {
  // just use discrete approximation..
  return (students_cum(t + 1.0e-6, df) - students_cum(t - 1.0e-6, df)) / 4.0e-6;
}

float taMath_float::Ftest_q(float F, float v1, float v2) {
  return beta_i(0.5*v2, 0.5*v1, v2 / (v2 + (v1 * F)));
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

bool taMath_float::vec_check_same_size(const float_Matrix* a, const float_Matrix* b, bool quiet) {
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
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) += b;
  return true;
}

bool taMath_float::vec_sub_scalar(float_Matrix* a, float b) {
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) -= b;
  return true;
}

bool taMath_float::vec_mult_scalar(float_Matrix* a, float b) {
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) *= b;
  return true;
}

bool taMath_float::vec_div_scalar(float_Matrix* a, float b) {
  if(b == 0.0) return false;
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) /= b;
  return true;
}

bool taMath_float::vec_simple_math(float_Matrix* vec, const SimpleMathSpec& math_spec) {
  for(int i=0;i<vec->size;i++)
    vec->FastEl_Flat(i) = math_spec.Evaluate(vec->FastEl_Flat(i));
  return true;
}

bool taMath_float::vec_simple_math_arg(float_Matrix* vec, const float_Matrix* arg_ary,
					const SimpleMathSpec& math_spec) {
  if(!vec_check_same_size(vec, arg_ary)) return false;
  SimpleMathSpec myms = math_spec;
  for(int i=0;i<vec->size;i++) {
    myms.arg = arg_ary->FastEl_Flat(i);
    vec->FastEl_Flat(i) = myms.Evaluate(vec->FastEl_Flat(i));
  }
  return true;
}

float taMath_float::vec_first(const float_Matrix* vec) {
  if(vec->size == 0) return 0.0;
  return vec->FastEl_Flat(0);
}

float taMath_float::vec_last(const float_Matrix* vec) {
  if(vec->size == 0) return 0.0;
  return vec->FastEl_Flat(vec->size-1);
}

int taMath_float::vec_find_first(const float_Matrix* vec, Relation& rel) {
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  for(int i=0;i<vec->size;i++) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) return i;
  }
  return -1;
}

int taMath_float::vec_find_last(const float_Matrix* vec, Relation& rel) {
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  for(int i=vec->size-1;i>=0;i--) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) return i;
  }
  return -1;
}

float taMath_float::vec_max(const float_Matrix* vec, int& idx) {
  idx = 0;
  if(vec->size == 0) return 0.0;
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
  idx = 0;
  if(vec->size == 0) return 0.0;
  float rval = fabs(vec->FastEl_Flat(0));
  for(int i=1;i<vec->size;i++) {
    if(fabs(vec->FastEl_Flat(i)) > rval) {
      idx = i;
      rval = fabs(vec->FastEl_Flat(i));
    }
  }
  return rval;
}

float taMath_float::vec_min(const float_Matrix* vec, int& idx) {
  idx = 0;
  if(vec->size == 0) return 0.0;
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
  idx = 0;
  if(vec->size == 0) return 0.0;
  float rval = fabs(vec->FastEl_Flat(0));
  for(int i=1;i<vec->size;i++) {
    if(fabs(vec->FastEl_Flat(i)) < rval) {
      idx = i;
      rval = fabs(vec->FastEl_Flat(i));
    }
  }
  return rval;
}

float taMath_float::vec_sum(const float_Matrix* vec) {
  float rval = 0.0;
  for(int i=0;i<vec->size;i++)
    rval += vec->FastEl_Flat(i);
  return rval;
}

float taMath_float::vec_prod(const float_Matrix* vec) {
  if(vec->size == 0) return 0.0;
  float rval = vec->FastEl_Flat(0);
  for(int i=1;i<vec->size;i++)
    rval *= vec->FastEl_Flat(i);
  return rval;
}

float taMath_float::vec_mean(const float_Matrix* vec) {
  if(vec->size == 0)	return 0.0;
  return vec_sum(vec) / (float)vec->size;
}

float taMath_float::vec_var(const float_Matrix* vec, float mean, bool use_mean) {
  if(vec->size == 0)	return 0.0;
  if(!use_mean)    mean = vec_mean(vec);
  float rval = 0.0;
  for(int i=0;i<vec->size;i++)
    rval += (vec->FastEl_Flat(i) - mean) * (vec->FastEl_Flat(i) - mean);
  return rval / (float)vec->size;
}

float taMath_float::vec_std_dev(const float_Matrix* vec, float mean, bool use_mean) {
  return sqrt(vec_var(vec, mean, use_mean));
}

float taMath_float::vec_sem(const float_Matrix* vec, float mean, bool use_mean) {
  if(vec->size == 0)	return 0.0;
  return vec_std_dev(vec, mean, use_mean) / sqrt((float)vec->size);
}

float taMath_float::vec_ss_len(const float_Matrix* vec) {
  float rval = 0.0;
  for(int i=0;i<vec->size;i++)
    rval += vec->FastEl_Flat(i) * vec->FastEl_Flat(i);
  return rval;
}

void taMath_float::vec_histogram(float_Matrix* vec, const float_Matrix* oth, float bin_size) {
  vec->Reset();
  if(oth->size == 0) return;
  float_Array tmp;
  tmp.SetSize(oth->size);
  for(int i=0;i<oth->size;i++)
    tmp[i] = oth->FastEl_Flat(i);
  tmp.Sort();
  float min_v = tmp.FastEl(0);
  float max_v = tmp.Peek();
  int src_idx = 0;
  int trg_idx = 0;
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
  Relation tmp_rel;
  rel.CacheVar(tmp_rel);
  float rval = 0.0;
  for(int i=0;i<vec->size;i++) {
    if(tmp_rel.Evaluate(vec->FastEl_Flat(i))) rval += 1.0;
  }
  return rval;
}

float taMath_float::vec_median(const float_Matrix* vec) {
  if(vec->size == 0) return 0.0f;
  float_Array tmp;
  tmp.SetSize(vec->size);
  for(int i=0;i<vec->size;i++)
    tmp[i] = vec->FastEl_Flat(i);
  tmp.Sort();
  int idx = tmp.size / 2;
  return tmp[idx];
}

float taMath_float::vec_quantile(const float_Matrix* vec, float quant_pos) {
  if(vec->size == 0) return 0.0f;
  float_Array tmp;
  tmp.SetSize(vec->size);
  for(int i=0;i<vec->size;i++)
    tmp[i] = vec->FastEl_Flat(i);
  tmp.Sort();
  int idx = (int)(quant_pos * (float)tmp.size);
  if(idx >= tmp.size) idx = tmp.size-1;
  if(idx < 0) idx = 0;
  return tmp[idx];
}

float taMath_float::vec_mode(const float_Matrix* vec) {
  if(vec->size == 0) return 0.0f;
  float_Array tmp;
  tmp.SetSize(vec->size);
  for(int i=0;i<vec->size;i++)
    tmp[i] = vec->FastEl_Flat(i);
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
  if(vec->size <= 1) return;
  int lt_compval = -1;		// comparison return value for less-than
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

///////////////////////////////////////
// Normalization

float taMath_float::vec_norm_len(float_Matrix* vec, float len) {
  if(vec->size == 0) 	return 0.0;
  float scale = (len * len) / vec_ss_len(vec);
  for(int i=0;i<vec->size;i++) {
    float mag = (vec->FastEl_Flat(i) * vec->FastEl_Flat(i)) * scale;
    vec->FastEl_Flat(i) = (vec->FastEl_Flat(i) >= 0.0f) ? mag : -mag;
  }
  return scale;
}

float taMath_float::vec_norm_sum(float_Matrix* vec, float sum, float min_val) {
  if(vec->size == 0)	return 0.0;
  float act_sum = 0.0;
//notused  int min_idx;
  for(int i=0;i<vec->size;i++)
    act_sum += (vec->FastEl_Flat(i) - min_val);
  float scale = (sum / act_sum);
  for(int i=0;i<vec->size;i++)
    vec->FastEl_Flat(i) = ((vec->FastEl_Flat(i) - min_val) * scale) + min_val;
  return scale;
}

float taMath_float::vec_norm_max(float_Matrix* vec, float max) {
  if(vec->size == 0)	return 0.0;
  int idx;
  float cur_max = vec_max(vec, idx);
  if(cur_max == 0.0) return 0.0;
  float scale = (max / cur_max);
  for(int i=0;i<vec->size;i++)
    vec->FastEl_Flat(i) *= scale;
  return scale;
}

float taMath_float::vec_norm_abs_max(float_Matrix* vec, float max) {
  if(vec->size == 0)	return 0.0;
  int idx;
  float cur_max = vec_abs_max(vec, idx);
  if(cur_max == 0.0) return 0.0;
  float scale = (max / cur_max);
  for(int i=0;i<vec->size;i++)
    vec->FastEl_Flat(i) *= scale;
  return scale;
}

int taMath_float::vec_threshold(float_Matrix* vec, float thresh, float low, float high) {
  if(vec->size == 0)  return 0;
  int rval = 0;
  for(int i=0;i<vec->size;i++) {
    if(vec->FastEl_Flat(i) >= thresh) {
      vec->FastEl_Flat(i) = high;
      rval++;
    }
    else
      vec->FastEl_Flat(i) = low;
  }
  return rval;
}

float taMath_float::vec_aggregate(const float_Matrix* vec, Aggregate& agg) {
  int idx;
  switch(agg.op) {
  case Aggregate::GROUP:
    return vec->SafeEl_Flat(0);	// first guy..
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
      y = exp(-exp_mult * x);	// always count point
    }
    kernel->FastEl(i) = y;
  }
  vec_norm_sum(kernel);
  return true;
}

bool taMath_float::vec_kern_pow(float_Matrix* kernel, int half_sz, float pow_exp,
				 bool neg_tail, bool pos_tail) {
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
      y = 1.0;			// count self as 1
    }
    kernel->FastEl(i) = y;
  }
  vec_norm_sum(kernel);
  return true;
}

bool taMath_float::vec_convolve(float_Matrix* out_vec, const float_Matrix* in_vec,
				   const float_Matrix* kernel, bool keep_edges) {
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

/////////////////////////////////////////////////////////////////////////////////
// Matrix operations

bool taMath_float::mat_col(float_Matrix* col, const float_Matrix* mat, int col_no) {
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
  if(ta_mat->dims() != 2) return false;
  gsl_mat->size1 = ta_mat->dim(0); // "rows" (rows are contiguous in mem)
  gsl_mat->size2 = ta_mat->dim(1); // "columns"
  gsl_mat->tda = ta_mat->dim(0); // actual size of row in memory
  gsl_mat->data = (float*)ta_mat->data();
  gsl_mat->block = NULL;
  gsl_mat->owner = false;
  return true;
}

bool taMath_float::vec_get_gsl_fm_ta(gsl_vector_float* gsl_vec, const float_Matrix* ta_vec)
{
  if(ta_vec->dims() != 1) return false;
  gsl_vec->size = ta_vec->dim(0); 
  gsl_vec->stride = 1;
  gsl_vec->data = (float*)ta_vec->data();
  gsl_vec->block = NULL;
  gsl_vec->owner = false;
  return true;
}

bool taMath_float::mat_add(float_Matrix* a, const float_Matrix* b) {
  gsl_matrix_float g_a;  mat_get_gsl_fm_ta(&g_a, a);
  gsl_matrix_float g_b;  mat_get_gsl_fm_ta(&g_b, b);
  return gsl_matrix_float_add(&g_a, &g_b);
}

bool taMath_float::mat_sub(float_Matrix* a, const float_Matrix* b) {
  gsl_matrix_float g_a;  mat_get_gsl_fm_ta(&g_a, a);
  gsl_matrix_float g_b;  mat_get_gsl_fm_ta(&g_b, b);
  return gsl_matrix_float_sub(&g_a, &g_b);
}

bool taMath_float::mat_mult_els(float_Matrix* a, const float_Matrix* b) {
  gsl_matrix_float g_a;  mat_get_gsl_fm_ta(&g_a, a);
  gsl_matrix_float g_b;  mat_get_gsl_fm_ta(&g_b, b);
  return gsl_matrix_float_mul_elements(&g_a, &g_b);
}

bool taMath_float::mat_div_els(float_Matrix* a, const float_Matrix* b) {
  gsl_matrix_float g_a;  mat_get_gsl_fm_ta(&g_a, a);
  gsl_matrix_float g_b;  mat_get_gsl_fm_ta(&g_b, b);
  return gsl_matrix_float_div_elements(&g_a, &g_b);
}

bool taMath_float::mat_eigen_owrite(float_Matrix* a, float_Matrix* eigen_vals,
				    float_Matrix* eigen_vecs) {
  double_Matrix da;
  mat_cvt_float_to_double(&da, a);
  double_Matrix deval;
  double_Matrix devec;
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
  double_Matrix da;
  mat_cvt_float_to_double(&da, a);
  double_Matrix ds;
  double_Matrix dv;
  bool rval = taMath_double::mat_svd_owrite(&da, &ds, &dv);
  mat_cvt_double_to_float(s, &ds);
  mat_cvt_double_to_float(v, &dv);
  return rval;
}

bool taMath_float::mat_svd(const float_Matrix* a, float_Matrix* u, float_Matrix* s, float_Matrix* v) {
  double_Matrix da;
  mat_cvt_float_to_double(&da, a);
  double_Matrix du;
  double_Matrix ds;
  double_Matrix dv;
  bool rval = taMath_double::mat_svd(&da, &du, &ds, &dv);
  mat_cvt_double_to_float(u, &du);
  mat_cvt_double_to_float(s, &ds);
  mat_cvt_double_to_float(v, &dv);
  return rval;
}

bool taMath_float::mat_mds_owrite(float_Matrix* a, float_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  double_Matrix da;
  mat_cvt_float_to_double(&da, a);
  double_Matrix dxy;
  bool rval = taMath_double::mat_mds_owrite(&da, &dxy, x_axis_c, y_axis_c);
  mat_cvt_double_to_float(xy_coords, &dxy);
  return rval;
}

bool taMath_float::mat_mds(const float_Matrix* a, float_Matrix* xy_coords, int x_axis_c, int y_axis_c) {
  return mat_mds_owrite((float_Matrix*)a, xy_coords, x_axis_c, y_axis_c);
}

bool taMath_float::fft_real_transform(float_Matrix* out_mat, const float_Matrix* in_mat,
    bool real_out, bool norm)
{
  if (!out_mat || !in_mat) return false;
  double_Matrix dout_mat;
  double_Matrix din_mat;
  din_mat.Copy(in_mat);
  bool rval = taMath_double::fft_real_transform(&dout_mat, &din_mat,
    real_out, norm);
  out_mat->Copy(&dout_mat);
  return rval;
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

#endif	// HAVE_LIBGSL

bool taMath_float::mat_dist(float_Matrix* dist_mat, const float_Matrix* src_mat,
			     DistMetric metric, bool norm, float tolerance) {
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
  if(src_mat->dims() < 2) {
    taMisc::Error("*** mat_correl: src_matrix is < 2 dimensional!");
    return false;
  }

  int n = src_mat->frameSize();
  correl_mat->SetGeom(2, n, n);

  float_Matrix p1vals;
  float_Matrix p2vals;

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
  MatrixGeom frg = in_mat->geom;
  if(frg.size == 1 || in_mat->frames() == 0) return false;
  frg.SetSize(frg.size-1);	// nuke last dim
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

bool taMath_float::mat_frame_var(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_fmt_out_frame(out_mat, in_mat)) return false;
  int frn = in_mat->frames();
  int frs = in_mat->frameSize();
  float nrm = 1.0 / (float)frn;
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

bool taMath_float::mat_frame_std_dev(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_frame_var(out_mat, in_mat)) return false;
  int frs = in_mat->frameSize();
  for(int i=0;i<frs;i++) {
    out_mat->FastEl_Flat(i) = sqrt(out_mat->FastEl_Flat(i));
  }
  return true;
}

bool taMath_float::mat_frame_sem(float_Matrix* out_mat, const float_Matrix* in_mat) {
  if(!mat_frame_std_dev(out_mat, in_mat)) return false;
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
  float_Array tmp;
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
  float_Array tmp;
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
  float_Array tmp;
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

//////////////////////////////////////////////////////////////////////

//////////////////////////
//  	RndSeed     	//
//////////////////////////

void RndSeed::Initialize() {
  GetCurrent();
}

void RndSeed::GetCurrent() {
  seed.SetSize(MTRnd::N);
  int i;
  for(i=0;i<seed.size;i++) {
    seed.FastEl(i) = (long)MTRnd::mt[i];
  }
  mti = MTRnd::mti;
}

void RndSeed::NewSeed() {
  MTRnd::seed_time_pid();
  GetCurrent();
}

void RndSeed::OldSeed() {
  seed.SetSize(MTRnd::N);
  bool all_zero = true;
  int i;
  for (i=0;i<seed.size;i++) {
    if (seed.FastEl(i) != 0) {
      all_zero = false;
      break;
    }
  }
  if (all_zero) {
    taMisc::Warning("*** RndSeed::OldSeed: random seed is all zero and this doesn't work; getting current random seed!");
    GetCurrent();
    return;
  }
  for (i=0;i<seed.size;i++) {
    MTRnd::mt[i] = (ulong)seed.FastEl(i);
  }
  MTRnd::mti = mti;
}

void RndSeed::Init(ulong i) {
  MTRnd::seed(i);
  GetCurrent();
}

#ifdef DMEM_COMPILE
void RndSeed::DMem_Sync(MPI_Comm comm) {
  if(taMisc::dmem_nprocs <= 1)
    return;

  // just blast the first guy to all members of the same communicator
  DMEM_MPICALL(MPI_Bcast(seed.el, MTRnd::N, MPI_LONG, 0, comm),
	       "Process::SyncAllSeeds", "Bcast");
  OldSeed();		// then get my seed!
}

#else

void RndSeed::DMem_Sync(MPI_Comm) {
}

#endif // DMEM_COMPILE

//////////////////////////
//  	RndSeed_List   	//
//////////////////////////

void RndSeed_List::MakeSeeds(int n_seeds) {
  seeds.SetSize(n_seeds);
}

void RndSeed_List::NewSeeds() {
  if(seeds.size == 0) return;
  MTRnd::seed_time_pid();
  int rnd = MTRnd::genrand_int31();
  for(int i=0;i<seeds.size;i++) {
    int seed = rnd + i;
    seeds.FastEl(i)->Init(seed);
  }
}

void RndSeed_List::UseSeed(int idx) {
  if(seeds.size == 0) {
    taMisc::Error("RndSeed_List: no seeds present in list!");
    return;
  }
  int use_idx = idx % seeds.size;
  TestWarning(idx != use_idx, "UseSeed", "requested seed beyond end of list, wrapping around!");
  seeds.FastEl(use_idx)->OldSeed();
}

//////////////////////////
//  	Random     	//
//////////////////////////

double Random::Gen() const {
  if(var == 0.0f) return mean;
  switch(type) {
  case NONE:
    return mean;
  case UNIFORM:
    return UniformMeanRange(mean, var);
  case BINOMIAL:
    return mean + Binom((int)par, var);
  case POISSON:
    return mean + Poisson(var);
  case GAMMA:
    return mean + Gamma(var, (int)par);
  case GAUSSIAN:
    return mean + Gauss(var);
  }
  return 0.0f;
}

double Random::Density(double x) const {
  if(var == 0.0f) return 0.0f;
  switch(type) {
  case NONE:
    return 0.0f;
  case UNIFORM:
    return UniformDen(x - mean, var);
  case BINOMIAL:
    return BinomDen((int)par, (int)(x-mean), var);
  case POISSON:
    return PoissonDen((int)(x-mean), var);
  case GAMMA:
    return GammaDen((int)par, var, x - mean);
  case GAUSSIAN:
    return GaussDen(x-mean, var);
  }
  return 0.0f;
}

void Random::Initialize() {
  type = UNIFORM;
  mean = 0.0f;
  var = 1.0f;
  par = 1.0f;
}

void Random::Copy_(const Random& cp){
  type = cp.type;
  mean = cp.mean;
  var = cp.var;
  par = cp.par;
}

