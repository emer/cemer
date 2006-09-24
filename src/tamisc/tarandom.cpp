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



#include "tarandom.h"

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

//////////////////////////
//  	Random     	//
//////////////////////////

float Random::Gen() const {
  if(var == 0.0f) return mean;
  switch(type) {
  case NONE:
    return mean;
  case UNIFORM:
    return mean + Uniform(var);
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

float Random::Density(float x) const {
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

