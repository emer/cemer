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


#include "special_math.h"
#include <stdlib.h>
#include <stdio.h>

/* pi */
#define PI 3.14159265358979323846
/* size of the fact_ln buffer */
#define FACTLNSIZE 100
/* max iterations */
#define ITMAX 100		
/* error tolerance */
#define EPS 3.0e-7
    

Real fact_ln(int n) {
  static Real table[FACTLNSIZE + 1]; /* initialized to 0\'s */

  if(n < 0) { fprintf(stderr, "Negative factorial fact_ln()\n"); return 0; }
  if(n <= 1) return 0.0;
  if(n <= FACTLNSIZE) return table[n] ? table[n] : (table[n] = gamma_ln(n + 1.0));
  else return gamma_ln(n+1.0);
}

Real bico_ln(int n, int j) {
  return fact_ln(n)-fact_ln(j)-fact_ln(n-j);
}

Real hyperg(int j, int s, int t, int n) {
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

Real gser(Real a, Real x) {
  int n;
  Real gln,sum,del,ap;
  
  gln=gamma_ln(a);
  if (x <= 0.0) {
    if (x < 0.0) { fprintf(stderr, "x < 0 in gser()\n"); return 0; }
    return 0;
  }
  else {
    ap=a;
    del=sum=1.0/a;
    for (n=1;n<=ITMAX;n++) {
      ap += 1.0;
      del *= x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*EPS)
	return sum*exp(-x+a*log(x)-(gln));
    }
    fprintf(stderr, "a too large, ITMAX too small in gser()\n");
    return 0;
  }
}
    
Real gcf(Real a, Real x) {
  int n;
  Real gln;
  Real gold=0.0,g,fac=1.0,b1=1.0;
  Real b0=0.0,anf,ana,an,a1,a0=1.0;
  
  gln=gamma_ln(a);
  a1=x;
  for (n=1;n<=ITMAX;n++) {
    an=(Real) n;
    ana=an-a;
    a0=(a1+a0*ana)*fac;
    b0=(b1+b0*ana)*fac;
    anf=an*fac;
    a1=x*a0+anf*a1;
    b1=x*b0+anf*b1;
    if (a1) {
      fac=1.0/a1;
      g=b1*fac;
      if (fabs((g-gold)/g) < EPS)
	return exp(-x+a*log(x)-(gln))*g;
      gold=g;
    }
  }
  fprintf(stderr, "a too large, ITMAX too small in gcf()\n");
}

Real gamma_ln(Real z) {
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

Real gamma_p(Real a, Real x) {
  Real gamser,gammcf,gln;
  
  if (x < 0.0 || a <= 0.0) { fprintf(stderr, "Invalid args in gamma_p()\n"); return 0; }

  if (x < (a+1.0))
    return gser(a,x);
  else
    return 1.0 - gcf(a,x);
}

Real gamma_q(Real a, Real x) {
  Real gamser,gammcf,gln;
  
  if (x < 0.0 || a <= 0.0) { fprintf(stderr, "Invalid args in gamma_q()\n"); return 0; }
  if (x < (a+1.0))
    return 1.0 - gser(a,x);
  else
    return gcf(a,x);
}

/**********************************
  beta function
***********************************/

Real betacf(Real a, Real b, Real x) {
  Real qap,qam,qab,em,tem,d;
  Real bz,bm=1.0,bp,bpp;
  Real az=1.0,am=1.0,ap,app,aold;
  int m;
  
  qab=a+b;
  qap=a+1.0;
  qam=a-1.0;
  bz=1.0-qab*x/qap;
  for (m=1;m<=ITMAX;m++) {
    em=(Real) m;
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
    if (fabs(az-aold) < (EPS*fabs(az))) return az;
  }
  fprintf(stderr, "a or b too big, or ITMAX too small in betacf()\n");
  return 0;
}

Real beta(Real z, Real w) {
  return exp(gamma_ln(z) + gamma_ln(w) - gamma_ln(z + w));
}

Real beta_i(Real a, Real b, Real x) {
  Real bt;
  
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

Real binom_den(int n, int j, Real p) {
  if(j > n) { fprintf(stderr, "j > n in binom()\n"); return 0; }
  return exp(bico_ln(n,j) + (Real)j * log(p) + (Real)(n-j) * log(1.0 - p));
} 

Real binom_cum(int n, int k, Real p) {
  if(k > n) 	{ fprintf(stderr, "k > n in binom_cum()\n"); return 0; }
  return beta_i(k, n-k + 1, p);
}

Real binom_dev(int n, Real pp) {
  int j;
  static int nold=(-1);
  Real am,em,g,angle,p,bnl,sq,t,y;
  static Real pold=(-1.0),pc,plog,pclog,en,oldg;
  
  p=(pp <= 0.5 ? pp : 1.0-pp);
  am=n*p;
  if (n < 25) {
    bnl=0.0;
    for (j=1;j<=n;j++)
      if (drand48() < p) bnl += 1.0;
  }
  else if (am < 1.0) {
    g=exp(-am);
    t=1.0;
    for (j=0;j<=n;j++) {
      t *= drand48();
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
	angle=PI*drand48();
	y=tan(angle);
	em=sq*y+am;
      } while (em < 0.0 || em >= (en+1.0));
      em=floor(em);
      t=1.2*sq*(1.0+y*y)*exp(oldg-gamma_ln(em+1.0)
			     -gamma_ln(en-em+1.0)+em*plog+(en-em)*pclog);
    } while (drand48() > t);
    bnl=em;
  }
  if (p != pp) bnl=n-bnl;
  return bnl;
}


/**********************************
  the poisson distribution
***********************************/

Real poisson_den(int j, Real l) {
  return exp((Real)j * log(l) - fact_ln(j) - l);
}

Real poisson_cum(int j, Real x) {
  return gamma_q(j, x);
}

Real poisson_dev(Real xm) {
  static Real sq,alxm,g,oldm=(-1.0);
  Real em,t,y;
  
  if (xm < 12.0) {
    if (xm != oldm) {
      oldm=xm;
      g=exp(-xm);
    }
    em = -1;
    t=1.0;
    do {
      em += 1.0;
      t *= drand48();
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
	y=tan(PI*drand48());
	em=sq*y+xm;
      } while (em < 0.0);
      em=floor(em);
      t=0.9*(1.0+y*y)*exp(em*alxm-gamma_ln(em+1.0)-g);
    } while (drand48() > t);
  }
  return em;
}


/**********************************
  the gamma distribution
***********************************/

Real gamma_den(int j, Real l, Real t) {
  if(t < 0) return 0;
  return exp((Real)j * log(l) + (Real)(j-1) * log(t) - gamma_ln(j) - (l * t));
}

/* uses P(S_j <= t) = P(N_t >= j) */

Real gamma_cum(int j, Real l) {
  return 1.0 - poisson_cum(j, l);
}

Real gamma_dev(int ia) {
  int j;
  Real am,e,s,v1,v2,x,y;
  
  if (ia < 1) { fprintf(stderr, "ia < 1 in gamma_dev()\n"); return 0; }
  if (ia < 6) {
    x=1.0;
    for (j=1;j<=ia;j++) x *= drand48();
    x = -log(x);
  }
  else {
    do {
      do {
	do {
	  v1=2.0*drand48()-1.0;
	  v2=2.0*drand48()-1.0;
	} while (v1*v1+v2*v2 > 1.0);
	y=v2/v1;
	am=ia-1;
	s=sqrt(2.0*am+1.0);
	x=s*y+am;
      } while (x <= 0.0);
      e=(1.0+y*y)*exp(am*log(x/am)-s*y);
    } while (drand48() > e);
  }
  return x;
}



/**********************************
  the normal distribution (& error fun)
***********************************/

Real gauss_den(Real z) {
  return 0.398942280 * exp(-0.5 * z * z);
}

Real gauss_cum(Real z) {
  Real y, x, w;
  
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


Real gauss_dev() {
  static int iset=0;
  static Real gset;
  Real fac,r,v1,v2;
  
  if(iset==0) {
    do {
      v1=2.0*drand48()-1.0;
      v2=2.0*drand48()-1.0;
      r=v1*v1+v2*v2;
    } while ((r >= 1.0) || (r == 0));
    
    fac=sqrt((-2.0*log(r))/r);
    gset=v1*fac;
    iset=1;
    return (Real)(v2*fac);
  }
  else {
    iset=0;
    return gset;
  }
}

Real erf_c(Real x) {
  Real t,z,ans;
  
  z=fabs(x);
  t=1.0/(1.0+0.5*z);
  ans=t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
	t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
	t*(-0.82215223+t*0.17087277)))))))));
  return  x >= 0.0 ? ans : 2.0-ans;
}

Real erf(Real x) {
  return 1.0 - erfc(x);
}

/* less-efficient versions..
Real erf(Real x) {
  return x < 0.0 ? -gamma_p(0.5, x * x) : gamma_p(0.5, x * x);
}

Real erfc(Real x) {
  return x < 0.0 ? 1.0+gamma_p(0.5, x * x) : gamma_q(0.5, x * x);
}
*/



/**********************************
  misc statistical 
***********************************/
    
Real chisq_p(Real X, Real v) {
  return gamma_p(0.5 * v, 0.5 * X * X);
}

Real chisq_q(Real X, Real v) {
  return gamma_q(0.5 * v, 0.5 * X * X);
}

Real students(Real t, Real v) {
  return 1.0 - beta_i(0.5*v, 0.5, v / (v * t * t));
}

Real Ftest_q(Real F, Real v1, Real v2) {
  return beta_i(0.5*v2, 0.5*v1, v2 / (v2 + (v1 * F)));
}


    
