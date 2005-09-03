/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#ifndef random_h
#define random_h

#include "ta_base.h"
#include "css_special_math.h"
#include "tamisc_TA_type.h"

class RndSeed : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER random seed support
public:
  long_Array		seed;	// #READ_ONLY the seed, 624 elements long
  int			mti;	// #READ_ONLY the index into the seed, also needs to be saved

  void	 NewSeed();
  // set the seed to a new random value (based on time and process id)
  void	 OldSeed();
  // restore current seed to random num generator
  void	 GetCurrent();
  // get the current seed in use by the generator
  void   Init(ulong i);
  // initialize the seed based on given initializer

  void	Initialize();
  void	Destroy()		 { };
  void	Copy_(const RndSeed& cp);
  COPY_FUNS(RndSeed, taBase);
  TA_BASEFUNS(RndSeed);
};

class Random : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE Random Number Generation
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

  static int	IntZeroN(int n) 	{ if(n > 0) return (int)(MTRnd::genrand_int32() % (ulong)n); return 0; }
  // uniform random integer in the range between 0 and n, exclusive of n: [0,n)
  static float 	ZeroOne() 		{ return MTRnd::genrand_res53(); }
  // uniform random number between zero and one (inclusive of 1 due to rounding!)
  static float 	Range(float rng) 	{ return rng * (ZeroOne() - 0.5f); }
  // uniform random number with given range centered at 0
  static float 	Uniform(float half_rng)	{ return half_rng * 2.0f * (ZeroOne() - 0.5f); }
  // uniform random number with given half_range (var) centered at 0
  static float 	Binom(int n, float p) 	{ return binom_dev(n,p); }
  // binomial with n trials (par) each of probability p (var)
  static float 	Poisson(float l)   	{ return poisson_dev(l); }
  // poisson with parameter l (var)
  static float 	Gamma(float var, int j)  { return var * gamma_dev(j); }
  // gamma with given variance, number of exponential stages (par)
  static float 	Gauss(float var)  	{ return var * gauss_dev(); }
  // gaussian (normal) random number with given variance

  static float 	UniformDen(float x, float half_rng)
  { float rval = 0.0f; if(fabsf(x) <= half_rng) rval = 1.0f / (2.0f * half_rng); return rval; }
  // uniform density at x with given half_range (var) centered at 0 (subtr mean)
  static float 	BinomDen(int n, int j, float p) { return binom_den(n,j,p); }
  // binomial density at j with n trials (par) each of probability p (var)
  static float 	PoissonDen(int j, float l) { return poisson_den(j,l); }
  // poisson density with parameter l (var)
  static float 	GammaDen(int j, float l, float t)  { return gamma_den(j,l,t); }
  // gamma density at time t with given number of stages (par), lambda (var)
  static float 	GaussDen(float x, float var)  	{ return gauss_den(x / var); }
  // gaussian (normal) density for given variance (0 mean)

  float 	Gen() const;
  // generate a random variable according to current parameters

  float		Density(float x) const;
  // get density of random variable according to current params

  void	Initialize();
  void	Destroy()		 { };
  void	Copy_(const Random& cp);
  COPY_FUNS(Random, taBase);
  TA_BASEFUNS(Random);
};

class TimeUsed : public taBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER stores and computes time used for processing information
public:
  bool		rec;		// flag that determines whether to record timing information: OFF by default
  long 		usr;		// user clock ticks used
  long		sys;		// system clock ticks used
  long		tot;		// total time ticks used (all clock ticks on the CPU)
  long		n;		// number of times time used collected using GetUsed

  void 		operator += (const TimeUsed& td)	{ usr += td.usr; sys += td.sys; tot += td.tot; }
  void 		operator -= (const TimeUsed& td)	{ usr -= td.usr; sys -= td.sys; tot -= td.tot; }
  void 		operator *= (const TimeUsed& td)	{ usr *= td.usr; sys *= td.sys; tot *= td.tot; }
  void 		operator /= (const TimeUsed& td)	{ usr /= td.usr; sys /= td.sys; tot /= td.tot; }
  TimeUsed 	operator + (const TimeUsed& td) const;
  TimeUsed 	operator - (const TimeUsed& td) const;
  TimeUsed 	operator * (const TimeUsed& td) const;
  TimeUsed 	operator / (const TimeUsed& td) const;

  void		InitTimes();	// initialize the times
  void		GetTimes();	// get the clock ticks used to this point
  void		GetUsed(const TimeUsed& start);
  // get amount of time used by subtracting start from current time and adding to me, and incrementing n
  String	GetString();	// get string output as seconds and fractions of seconds

  void 	Initialize();
  void	Destroy()		{ }
  SIMPLE_COPY(TimeUsed);
  COPY_FUNS(TimeUsed, taBase);
  TA_BASEFUNS(TimeUsed);
};

#endif // random_h
