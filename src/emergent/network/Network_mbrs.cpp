// contains mbrs non-inline (INIMPL) functions from _mbrs.h
// if used, must be included directly in Network.cpp, NetworkState_cpp.cpp, NetworkState_cuda.cpp

// NOTE: the main Network.cpp file must have TA_BASEFUNS_CTORS_DEFN(MySpecMember);
// for all classes defined in _mbrs.h

#ifndef STATE_MAIN

#ifdef HAVE_LIBGSL
# include <gsl/gsl_sf.h>
#endif

double STATE_CLASS(taMath_double)::pi = M_PI;


double STATE_CLASS(taMath_double)::fact_ln(int n) {
  // gsl just uses lookup tables -- use theirs!
  // todo: this is not going to work for _cuda -- will need to copy over their code?
  return gsl_sf_lnfact(n);

//   static const int table_max = 170;
//   static double table[table_max];
//   static int table_size = 0;

//   if(n < 0) {
// #ifdef STATE_CPP
//     fprintf(stderr, "Negative factorial fact_ln()\n");
// #endif
//     return 0;
//   }
//   if(n <= 1) return 0.0;
//   if(n < table_size) return table[n];
//   if(n > table_max)

//   for(int i=table_size; i < 

//   ? table[n] : (table[n] = gamma_ln(n + 1.0));

//   table.Alloc(n+1);             // allocate new size
//   int i=table.size;
//   for(; i<=n; i++)
//     table.FastEl(i) = 0.0;
//   table.size = n+1;

//   return (table[n] = gamma_ln(n + 1.0));
}

double STATE_CLASS(taMath_double)::gamma_ln(double z) {
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

static double betacf_double(double a, double b, double x) {
  double qap,qam,qab,em,tem,d;
  double bz,bm=1.0,bp,bpp;
  double az=1.0,am=1.0,ap,app,aold;
  int m;

  qab=a+b;
  qap=a+1.0;
  qam=a-1.0;
  bz=1.0-qab*x/qap;
  for (m=1;m<=100;m++) {
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
    if (fabs(az-aold) < (3.0e-7*fabs(az))) return az;
  }
  fprintf(stderr, "a or b too big, or max_iterations too small in betacf()\n");
  return 0;
}

double STATE_CLASS(taMath_double)::beta_i(double a, double b, double x) {
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

double STATE_CLASS(taMath_double)::beta_den(double x, double a, double b) {
  if(x < 0.0 || x > 1.0) {
    return 0 ;
  }
  else {
    double p;

    double gab = gamma_ln(a + b);
    double ga = gamma_ln(a);
    double gb = gamma_ln(b);
      
    if (x == 0.0 || x == 1.0) {
      p = exp (gab - ga - gb) * pow (x, a - 1) * pow (1 - x, b - 1);
    }
    else {
      p = exp (gab - ga - gb + log(x) * (a - 1)  + log1p(-x) * (b - 1));
    }

    return p;
  }
}

double STATE_CLASS(taMath_double)::binom_dev(int n, double pp, int thr_no) {
  int j;
  static int nold=(-1);
  double am,em,g,angle,p,bnl,sq,t,y;
  static double pold=(-1.0),pc,plog,pclog,en,oldg;

  p=(pp <= 0.5 ? pp : 1.0-pp);
  am=n*p;
  if (n < 25) {
    bnl=0.0;
    for (j=1;j<=n;j++)
      if (MTRnd::GenRandRes53(thr_no) < p) bnl += 1.0;
  }
  else if (am < 1.0) {
    g=exp(-am);
    t=1.0;
    for (j=0;j<=n;j++) {
      t *= MTRnd::GenRandRes53(thr_no);
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
        angle=pi*MTRnd::GenRandRes53(thr_no);
        y=tan(angle);
        em=sq*y+am;
      } while (em < 0.0 || em >= (en+1.0));
      em=floor(em);
      t=1.2*sq*(1.0+y*y)*exp(oldg-gamma_ln(em+1.0)
                             -gamma_ln(en-em+1.0)+em*plog+(en-em)*pclog);
    } while (MTRnd::GenRandRes53(thr_no) > t);
    bnl=em;
  }
  if (p != pp) bnl=n-bnl;
  return bnl;
}

double STATE_CLASS(taMath_double)::poisson_dev(double xm, int thr_no) {
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
      t *= MTRnd::GenRandRes53(thr_no);
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
        y=tan(pi*MTRnd::GenRandRes53(thr_no));
        em=sq*y+xm;
      } while (em < 0.0);
      em=floor(em);
      t=0.9*(1.0+y*y)*exp(em*alxm-gamma_ln(em+1.0)-g);
    } while (MTRnd::GenRandRes53(thr_no) > t);
  }
  return em;
}

double STATE_CLASS(taMath_double)::gamma_dev(const double a, const double b, int thr_no) {
  if (a < 1) {
    double u = MTRnd::GenRandRes53(thr_no);
    return gamma_dev(1.0 + a, b, thr_no) * pow (u, 1.0 / a);
  }

  {
    double x, v, u;
    double d = a - 1.0 / 3.0;
    double c = (1.0 / 3.0) / sqrt (d);

    while (true) {
      do {
        x = gauss_dev(thr_no);
        v = 1.0 + c * x;
      }
      while (v <= 0);

      v = v * v * v;
      u = MTRnd::GenRandRes53(thr_no);

      if (u < 1 - 0.0331 * x * x * x * x) 
        break;

      if (log (u) < 0.5 * x * x + d * (1 - v + log (v)))
        break;
    }
    
    return b * d * v;
  }
}

#endif  // ndef STATE_MAIN

