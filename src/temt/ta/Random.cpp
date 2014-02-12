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
#include <taMath_double>

TA_BASEFUNS_CTORS_DEFN(Random);

double Random::Binom(int n, double p) {
  return taMath_double::binom_dev(n,p);
}
double Random::Poisson(double l) {
  return taMath_double::poisson_dev(l);
}
double Random::Gamma(double var, int j)  {
  return var * taMath_double::gamma_dev(j);
}
double Random::Gauss(double stdev) {
  return stdev * taMath_double::gauss_dev();
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
