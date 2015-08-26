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
#include <taINBase>
#include <double_Matrix>

// member includes:
#include <MTRnd>

#include <math.h>

#include <taMath_double>

// declare all other types mentioned but not required to include:

taTypeDef_Of(Random);

class TA_API Random : public taINBase {
  // #STEM_BASE #NO_UPDATE_AFTER ##INLINE ##INLINE_DUMP ##CAT_Math Random Number Generation
INHERITED(taINBase)
public:
  enum Type {
    UNIFORM,                    // uniform with var = range on either side of the mean
    BINOMIAL,                   // binomial with var = p, par = n
    POISSON,                    // poisson with lambda = var
    GAMMA,                      // gamma with var scaling parameter and par = k stages
    GAUSSIAN,                   // normal with var
    BETA,                       // beta with var = a and par = b shape parameters
    NONE,                       // just the mean
  };

  Type          type;           // type of random variable to generate
  double        mean;           // mean of random distribution
  double        var;            // #CONDSHOW_OFF_type:NONE 'varibility' parameter for the random numbers (gauss = standard deviation, not variance; uniform = half-range)
  double        par;            // #CONDSHOW_ON_type:GAMMA,BINOMIAL,BETA extra parameter for distribution (depends on each one)

  double        Gen(int thr_no = 0) const;
  // generate a random variable according to current parameters -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  double        Density(double x) const;
  // get density of random variable according to current params

  ////////////////////////////////////////////////////////////////////////
  // various handy static random number generation functions:

  static int    IntZeroN(int n, int thr_no = 0)
  { if(n > 0) return (int)(MTRnd::GenRandInt32(thr_no) % (uint)n); return 0; }
  // #CAT_Int uniform random integer in the range between 0 and n, exclusive of n: [0,n) -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static int    IntMinMax(int min, int max, int thr_no = 0)
  { return min + IntZeroN(max - min, thr_no); }
  // #CAT_Int uniform random integer in range between min and max, exclusive of max: [min,max) -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static int    IntMeanRange(int mean, int range, int thr_no = 0)
  { return mean + (IntZeroN(2 * range + 1, thr_no) - range); }
  // #CAT_Int uniform random integer with given range on either side of the mean: [mean - range, mean + range] -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static int    Discrete(taMatrix* distribution, int thr_no = 0);
  // #CAT_Int Discrete distribtion. Samples from a discrete distribution with probabilities given (automatically renormalizes the values) -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double ZeroOne(int thr_no = 0)
  { return MTRnd::GenRandRes53(thr_no); }
  // #CAT_Float uniform random number between zero and one (inclusive of 1 due to rounding!) -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double UniformMinMax(double min, double max, int thr_no = 0)
  { return min + (max - min) * ZeroOne(thr_no); }
  // #CAT_Float uniform random number between min and max values inclusive (Do not use for generating integers - will not include max!) -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double UniformMeanRange(double mean, double range, int thr_no = 0)
  { return mean + range * 2.0 * (ZeroOne(thr_no) - 0.5); }
  // #CAT_Float uniform random number with given range on either size of the mean: [mean - range, mean + range] -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)

  static double Binom(int n, double p, int thr_no = 0);
  // #CAT_Float binomial with n trials (par) each of probability p (var) -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double Poisson(double l, int thr_no = 0);
  // #CAT_Float poisson with parameter l (var) -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double Gamma(double var, double j, int thr_no = 0);
  // #CAT_Float gamma with given variance, number of exponential stages (par) -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double Gauss(double stdev, int thr_no = 0);
  // #CAT_Float gaussian (normal) random number with given standard deviation -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)
  static double Beta(double a, double b, int thr_no = 0);
  // #CAT_Float beta random number with two shape parameters a > 0 and b > 0 -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)

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
  static double BetaDen(double x, double a, double b);
  // #CAT_Float beta density at value 0 < x < 1 for shape parameters a, b


  static bool   BoolProb(double p, int thr_no = 0) { return (ZeroOne(thr_no) < p); }
  // #CAT_Bool boolean true/false with given probability -- specify thread number if calling from thread for thread-safe operation (1 <= thr_no < cpus)

  TA_BASEFUNS(Random);
private:
  void  Copy_(const Random& cp);
  void  Initialize();
  void  Destroy()                { };
};

#endif // Random_h
