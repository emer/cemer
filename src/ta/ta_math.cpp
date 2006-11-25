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

#include <math.h>

//////////////////////////
//  	CountParam     	//
//////////////////////////

void CountParam::Initialize() {
  rel = LESSTHANOREQUAL;
  val = 0.0;
}

bool CountParam::Evaluate(double cmp) const {
  switch(rel) {
  case EQUAL:
    if(cmp == val)	return true;
    break;
  case NOTEQUAL:
    if(cmp != val)	return true;
    break;
  case LESSTHAN:
    if(cmp < val)	return true;
    break;
  case GREATERTHAN:
    if(cmp > val)	return true;
    break;
  case LESSTHANOREQUAL:
    if(cmp <= val)	return true;
    break;
  case GREATERTHANOREQUAL:
    if(cmp >= val)	return true;
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
    return ((val <= 0) ? taMath::FLT_MIN_10_EXP : log10(val));
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
  case GTEQ:
    return MAX(val, arg);
  case LTEQ:
    return MIN(val, arg);
  case GTLTEQ:
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

static double betacf(double a, double b, double x) {
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
    return bt*betacf(a,b,x)/a;
  else
    return 1.0-bt*betacf(b,a,1.0-x)/b;
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

double taMath_double::gauss_den(double z) {
  return 0.398942280 * exp(-0.5 * z * z);
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
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) /= b->FastEl_Flat(i);
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
  for(int i=0;i<oth->size;i++)
    tmp.Add(oth->FastEl_Flat(i));
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

double taMath_double::vec_count(const double_Matrix* vec, CountParam& cnt) {
  double rval = 0.0;
  for(int i=0;i<vec->size;i++) {
    if(cnt.Evaluate(vec->FastEl_Flat(i))) rval += 1.0;
  }
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
  int min_idx;
  double cur_min = vec_min(vec, min_idx);
  for(int i=0;i<vec->size;i++)
    act_sum += (vec->FastEl_Flat(i) - cur_min);
  double scale = (sum / act_sum);
  for(int i=0;i<vec->size;i++)
    vec->FastEl_Flat(i) = ((vec->FastEl_Flat(i) - cur_min) * scale) + min_val;
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
  case Aggregate::COUNT: 
    return taMath_double::vec_count(vec, agg.count);
  }
  return 0.0;
}

/////////////////////////////////////////////////////////////////////////////////
// Matrix operations

#ifdef HAVE_GSL

bool taMath_double::mat_get_gsl_fm_ta(const double_Matrix* ta_mat, gsl_matrix* gsl_mat)
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

bool taMath_double::mat_add(double_Matrix* a, double_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(a, &g_a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(b, &g_b);
  return gsl_matrix_add(&g_a, &g_b);
}

bool taMath_double::mat_sub(double_Matrix* a, double_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(a, &g_a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(b, &g_b);
  return gsl_matrix_sub(&g_a, &g_b);
}

bool taMath_double::mat_mult_els(double_Matrix* a, double_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(a, &g_a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(b, &g_b);
  return gsl_matrix_mul_elements(&g_a, &g_b);
}

bool taMath_double::mat_div_els(double_Matrix* a, double_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(a, &g_a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(b, &g_b);
  return gsl_matrix_div_elements(&g_a, &g_b);
}

#endif


////////////////////////////////////////////////////////////////////////////////
// 			single precision math
////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// ExpLog: exponential and logarithmic functions

float taMath_float::e = M_E;

/////////////////////////////////////////////////////////////////////////////////
// Trigonometry

float taMath_float::pi = M_PI;
float taMath_float::deg_per_rad = 180.0 / M_PI;


/////////////////////////////////////////////////////////////////////////////////
// Vector operations (operate on Matrix objects, treating as a single linear guy)

///////////////////////////////////////
// arithmetic ops

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
  for(int i=0;i<a->size;i++)
    a->FastEl_Flat(i) /= b->FastEl_Flat(i);
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
  for(int i=0;i<oth->size;i++)
    tmp.Add(oth->FastEl_Flat(i));
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

float taMath_float::vec_count(const float_Matrix* vec, CountParam& cnt) {
  float rval = 0.0;
  for(int i=0;i<vec->size;i++) {
    if(cnt.Evaluate(vec->FastEl_Flat(i))) rval += 1.0;
  }
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
  if(!vec_check_same_size(vec, oth)) return -1.0;
  float rval = 0.0;
  for(int i=0;i<vec->size;i++) {
    float p = vec->FastEl_Flat(i);
    float q = oth->FastEl_Flat(i);
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
  int min_idx;
  float cur_min = vec_min(vec, min_idx);
  for(int i=0;i<vec->size;i++)
    act_sum += (vec->FastEl_Flat(i) - cur_min);
  float scale = (sum / act_sum);
  for(int i=0;i<vec->size;i++)
    vec->FastEl_Flat(i) = ((vec->FastEl_Flat(i) - cur_min) * scale) + min_val;
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
  case Aggregate::COUNT: 
    return taMath_float::vec_count(vec, agg.count);
  }
  return 0.0;
}

/////////////////////////////////////////////////////////////////////////////////
// Matrix operations

#ifdef HAVE_GSL

bool taMath_float::mat_get_gsl_fm_ta(const float_Matrix* ta_mat, gsl_matrix_float* gsl_mat)
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

bool taMath_float::mat_add(float_Matrix* a, float_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(a, &g_a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(b, &g_b);
  return gsl_matrix_add(&g_a, &g_b);
}

bool taMath_float::mat_sub(float_Matrix* a, float_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(a, &g_a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(b, &g_b);
  return gsl_matrix_sub(&g_a, &g_b);
}

bool taMath_float::mat_mult_els(float_Matrix* a, float_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(a, &g_a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(b, &g_b);
  return gsl_matrix_mul_elements(&g_a, &g_b);
}

bool taMath_float::mat_div_els(float_Matrix* a, float_Matrix* b) {
  gsl_matrix g_a;  mat_get_gsl_fm_ta(a, &g_a);
  gsl_matrix g_b;  mat_get_gsl_fm_ta(b, &g_b);
  return gsl_matrix_div_elements(&g_a, &g_b);
}

#endif

//////////////////////////////////////////////////////////////////////

//////////////////////////
//  	RndSeed     	//
//////////////////////////

void RndSeed::Initialize() {
  GetCurrent();
}

void RndSeed::GetCurrent() {
  seed.EnforceSize(MTRnd::N);
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
  seed.EnforceSize(MTRnd::N);
  bool all_zero = true;
  int i;
  for (i=0;i<seed.size;i++) {
    if (seed.FastEl(i) != 0) {
      all_zero = false;
      break;
    }
  }
  if (all_zero) {
    taMisc::Error("*** RndSeed::OldSeed: random seed is all zero and this doesn't work; getting current random seed!");
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


void RndSeed_List::NewSeeds() {
  for(int i=0;i<size;i++) {
    FastEl(i)->NewSeed();
  }
}

void RndSeed_List::UseSeed(int idx) {
  if(!size) {
    taMisc::Error("RndSeed_List: no seeds present in list!");
    return;
  }
  int use_idx = idx % size;
  if(idx != use_idx) {
    taMisc::Warning("RndSeed_List: Warning -- requested seed beyond end of list:",
		    String(idx),"list size:", String(size),"wrapping around!");
  }
  FastEl(use_idx)->OldSeed();
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

