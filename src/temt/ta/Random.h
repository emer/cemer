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

#ifndef Random_h
#define Random_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <MTRnd>

#include <math.h>

// declare all other types mentioned but not required to include:

TypeDef_Of(Random);

class TA_API Random : public taNBase {
  // #STEM_BASE #NO_UPDATE_AFTER ##INLINE ##INLINE_DUMP ##CAT_Math Random Number Generation
INHERITED(taNBase)
public:
  enum Type {
    UNIFORM,                    // uniform with var = range on either side of the mean
    BINOMIAL,                   // binomial with var = p, par = n
    POISSON,                    // poisson with lambda = var
    GAMMA,                      // gamma with var and par = stages
    GAUSSIAN,                   // normal with var
    NONE                        // just the mean
  };

  Type          type;           // type of random variable to generate
  double        mean;           // mean of random distribution
  double        var;            // #CONDEDIT_OFF_type:NONE 'varibility' parameter for the random numbers (gauss = standard deviation, not variance; uniform = half-range)
  double        par;            // #CONDEDIT_ON_type:GAMMA,BINOMIAL extra parameter for distribution (depends on each one)

  double        Gen() const;
  // generate a random variable according to current parameters
  double        Density(double x) const;
  // get density of random variable according to current params

  ////////////////////////////////////////////////////////////////////////
  // various handy static random number generation functions:

  static int    IntZeroN(int n)
  { if(n > 0) return (int)(MTRnd::genrand_int32() % (uint)n); return 0; }
  // #CAT_Int uniform random integer in the range between 0 and n, exclusive of n: [0,n)
  static int    IntMinMax(int min, int max)
  { return min + IntZeroN(max - min); }
  // #CAT_Int uniform random integer in range between min and max, exclusive of max: [min,max)
  static int    IntMeanRange(int mean, int range)
  { return mean + (IntZeroN(2 * range + 1) - range); }
  // #CAT_Int uniform random integer with given range on either side of the mean: [mean - range, mean + range]

  static double ZeroOne()               { return MTRnd::genrand_res53(); }
  // #CAT_Float uniform random number between zero and one (inclusive of 1 due to rounding!)
  static double UniformMinMax(double min, double max)
  { return min + (max - min) * ZeroOne(); }
  // #CAT_Float uniform random number between min and max values (inclusive)
  static double UniformMeanRange(double mean, double range)
  { return mean + range * 2.0 * (ZeroOne() - 0.5); }
  // #CAT_Float uniform random number with given range on either size of the mean: [mean - range, mean + range]

  static double Binom(int n, double p);
  // #CAT_Float binomial with n trials (par) each of probability p (var)
  static double Poisson(double l);
  // #CAT_Float poisson with parameter l (var)
  static double Gamma(double var, int j);
  // #CAT_Float gamma with given variance, number of exponential stages (par)
  static double Gauss(double stdev);
  // #CAT_Float gaussian (normal) random number with given standard deviation

  static double UniformDen(double x, double range)
  { double rval = 0.0; if(fabs(x) <= range) rval = 1.0 / (2.0 * range); return rval; }
  // #CAT_Float uniform density at x with given range on either size of 0 (subtr mean from x before)
  static double BinomDen(int n, int j, double p);
  // #CAT_Float binomial density at j with n trials (par) each of probability p (var)
  static double PoissonDen(int j, double l);
  // #CAT_Float poisson density with parameter l (var)
  static double GammaDen(int j, double l, double t);
  // #CAT_Float gamma density at time t with given number of stages (par), lambda (var)
  static double GaussDen(double x, double stdev);
  // #CAT_Float gaussian (normal) density for given standard deviation (0 mean)


  static bool   BoolProb(double p) { return (ZeroOne() < p); }
  // #CAT_Bool boolean true/false with given probability

  void  Initialize();
  void  Destroy()                { };
  void  Copy_(const Random& cp);
  TA_BASEFUNS(Random); //
};

#endif // Random_h
