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

#include "Random.h"


TA_BASEFUNS_CTORS_DEFN(Random);

int Random::Discrete(taMatrix* distribution, int thr_no) {
  double z = 0.0;
  // Renormalize the input distribution
  for (int i = 0; i < distribution->size; i++) {
    z += distribution->FastElAsDouble_Flat(i);
  }
  double r = MTRnd::GenRandRes53(thr_no);
  double psum;
  for (int i = 0; i < distribution->size; i++) {
    double pval = distribution->FastElAsDouble_Flat(i) / z;
    psum += pval;
    if (r < psum) {
      return i;                 // shouldn't this be i?
    }
  }
  return distribution->size-1;
}

double Random::Binom(int n, double p, int thr_no) {
  return taMath_double::binom_dev(n,p,thr_no);
}
double Random::Poisson(double l, int thr_no) {
  return taMath_double::poisson_dev(l,thr_no);
}
double Random::Gamma(double var, double j, int thr_no)  {
  return taMath_double::gamma_dev(j, var, thr_no);
}
double Random::Gauss(double stdev, int thr_no) {
  return stdev * taMath_double::gauss_dev(thr_no);
}
double Random::Beta(double a, double b, int thr_no)  {
  return taMath_double::beta_dev(a, b, thr_no);
}

double Random::BinomDen(int n, int j, double p) {
  return taMath_double::binom_den(n,j,p);
}

double Random::PoissonDen(int j, double l) {
  return taMath_double::poisson_den(j,l);
}

double Random::GammaDen(int j, double l, double t)  {
  return taMath_double::gamma_den(j,l,t);
}
double Random::GaussDen(double x, double stdev) {
  return taMath_double::gauss_den_sig(x, stdev);
}
double Random::BetaDen(double x, double a, double b) {
  return taMath_double::beta_den(x, a, b);
}

double Random::Gen(int thr_no) const {
  if(var == 0.0f) return mean;
  switch(type) {
  case NONE:
    return mean;
  case UNIFORM:
    return UniformMeanRange(mean, var, thr_no);
  case BINOMIAL:
    return mean + Binom((int)par, var, thr_no);
  case POISSON:
    return mean + Poisson(var, thr_no);
  case GAMMA:
    return mean + Gamma(var, (int)par, thr_no);
  case GAUSSIAN:
    return mean + Gauss(var, thr_no);
  case BETA:
    return mean + Beta(var, par, thr_no);
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
  case BETA:
    return BetaDen(x-mean, var, par);
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
