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

#ifndef tarandom_h
#define tarandom_h

#include "ta_base.h"
#include "ta_dmem.h"
#include "ta_math.h"
#include "ta_mtrnd.h"
#include "tamisc_def.h"
#include "tamisc_TA_type.h"

class TAMISC_API RndSeed : public taNBase {
  // random seeds: can control the random number generator to restart with the same pseudo-random sequence or get a new one
INHERITED(taNBase)
public:
  int_Array		seed;	// #READ_ONLY the seed, 624 elements long
  int			mti;	// #READ_ONLY the index into the seed, also needs to be saved

  void	 NewSeed();
  // set the seed to a new random value (based on time and process id)
  void	 OldSeed();
  // restore current seed to random num generator
  void	 GetCurrent();
  // get the current seed in use by the generator
  void   Init(ulong i);
  // initialize the seed based on given initializer

  void	DMem_Sync(MPI_Comm comm);
   // synchronize seeds across all procs -- uses the first proc's seed

  void	Initialize();
  void	Destroy()		 { CutLinks(); }
  TA_SIMPLE_BASEFUNS(RndSeed);
};

class TAMISC_API Random : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP Random Number Generation
INHERITED(taBase)
public:
  enum Type {
    UNIFORM,			// uniform with var = half-range
    BINOMIAL,			// binomial with var = p, par = n
    POISSON,			// poisson with lambda = var
    GAMMA,			// gamma with var and par = stages
    GAUSSIAN,			// normal with var
    NONE 			// just the mean
  };

  Type		type;		// type of random variable to generate
  float		mean;		// mean of random distribution
  float		var;		// #CONDEDIT_OFF_type:NONE 'varibility' parameter for the random numbers (gauss = SD, not variance; uniform = half-range)
  float		par;		// #CONDEDIT_ON_type:GAMMA,BINOMIAL extra parameter for distribution (depends on each one)

  static int	IntZeroN(int n) 	{ if(n > 0) return (int)(MTRnd::genrand_int32() % (uint)n); return 0; }
  // uniform random integer in the range between 0 and n, exclusive of n: [0,n)
  static float 	ZeroOne() 		{ return MTRnd::genrand_res53(); }
  // uniform random number between zero and one (inclusive of 1 due to rounding!)
  static float 	Range(float rng) 	{ return rng * (ZeroOne() - 0.5f); }
  // uniform random number with given range centered at 0
  static float 	Uniform(float half_rng)	{ return half_rng * 2.0f * (ZeroOne() - 0.5f); }
  // uniform random number with given half_range (var) centered at 0
  static float 	Binom(int n, float p) 	{ return taMath_double::binom_dev(n,p); }
  // binomial with n trials (par) each of probability p (var)
  static float 	Poisson(float l)   	{ return taMath_double::poisson_dev(l); }
  // poisson with parameter l (var)
  static float 	Gamma(float var, int j)  { return var * taMath_double::gamma_dev(j); }
  // gamma with given variance, number of exponential stages (par)
  static float 	Gauss(float var)  	{ return var * taMath_double::gauss_dev(); }
  // gaussian (normal) random number with given variance

  static float 	UniformDen(float x, float half_rng)
  { float rval = 0.0f; if(fabsf(x) <= half_rng) rval = 1.0f / (2.0f * half_rng); return rval; }
  // uniform density at x with given half_range (var) centered at 0 (subtr mean)
  static float 	BinomDen(int n, int j, float p) { return taMath_double::binom_den(n,j,p); }
  // binomial density at j with n trials (par) each of probability p (var)
  static float 	PoissonDen(int j, float l) { return taMath_double::poisson_den(j,l); }
  // poisson density with parameter l (var)
  static float 	GammaDen(int j, float l, float t)  { return taMath_double::gamma_den(j,l,t); }
  // gamma density at time t with given number of stages (par), lambda (var)
  static float 	GaussDen(float x, float var)  	{ return taMath_double::gauss_den(x / var); }
  // gaussian (normal) density for given variance (0 mean)

  float 	Gen() const;
  // generate a random variable according to current parameters

  float		Density(float x) const;
  // get density of random variable according to current params

  void	Initialize();
  void	Destroy()		 { };
  void	Copy_(const Random& cp);
  COPY_FUNS(Random, taBase);
  TA_BASEFUNS(Random); //
};

#endif // tarandom_h
