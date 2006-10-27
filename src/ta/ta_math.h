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


#ifndef TA_MATH_H
#define TA_MATH_H

#include <cmath>
#include "ta_matrix.h"

#ifdef HAVE_GSL
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_matrix_float.h>
#endif

class TA_API CountParam : public taBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE #INLINE_DUMP counting criteria params
  INHERITED(taBase)
public:
  enum Relation {
    EQUAL,		// #LABEL_=
    NOTEQUAL,		// #LABEL_!=
    LESSTHAN,		// #LABEL_<
    GREATERTHAN,	// #LABEL_>
    LESSTHANOREQUAL,	// #LABEL_<=
    GREATERTHANOREQUAL 	// #LABEL_>=
  };

  Relation	rel;		// #LABEL_ relation of statistic to target value
  double	val;		// #LABEL_ target or comparison value

  bool 		Evaluate(double cmp) const;

  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(CountParam);
};

class TA_API Aggregate : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP Basic aggregation operations
  INHERITED(taOBase)
public:
  enum Operator {		// Aggregate Operators
    GROUP,			// group by this field
    MIN,			// Minimum
    MAX,			// Maximum
    ABS_MIN,			// Minimum of absolute values
    ABS_MAX,			// Maximum of absolute values
    SUM,			// Summation
    PROD,			// Product
    MEAN,			// Mean of values
    VAR,			// Variance
    STDEV,			// Standard deviation
    SEM,			// Standard error of the mean 
    COUNT, 			// Count of the number times count relation was true
  };

  Operator      op;		// how to aggregate over the network
  CountParam	count;		// #CONDEDIT_ON_op:COUNT parameters for the COUNT aggregation

  virtual String GetAggName() const;  // get string representation of aggregation opr

  void 	Initialize();
  void 	Destroy();
  TA_SIMPLE_BASEFUNS(Aggregate);
};

class TA_API SimpleMathSpec : public taBase {
  // #INLINE #INLINE_DUMP #NO_UPDATE_AFTER ##NO_TOKENS params for std kinds of simple math operators
  INHERITED(taBase)
public:
  enum MathOpr {
    NONE,			// no function
    THRESH,			// threshold: if val >= arg then hi, else lo
    ABS,			// take absolute-value
    SQUARE,			// square (raise to 2nd power)
    SQRT,			// square root
    LOG,			// natural log
    LOG10,			// log base 10
    EXP,			// exponential (e^val)
    ADD,			// add arg value
    SUB,			// subtract arg value
    MUL,			// multiply by arg value
    POWER,			// raise to the power of arg
    DIV, 			// divide by arg value
    GTEQ,			// make all values greater than or equal to arg
    LTEQ,			// make all values less than or equal to arg
    GTLTEQ			// make all values greater than lw and less than hi
  };

  MathOpr 	opr;		// what math operator to use
  double		arg;		// #CONDEDIT_ON_opr:THRESH,ADD,SUB,MUL,POWER,DIV,GTEQ,LTEQ argument for ops (threshold add/sub/mul/div arg)
  double		lw;		// #CONDEDIT_ON_opr:THRESH,GTLTEQ the value to assign values below threshold
  double		hi;		// #CONDEDIT_ON_opr:THRESH,GTLTEQ the value to assign values above threshold

  double	Evaluate(double val) const; // evaluate math operator on given value
  Variant&	EvaluateVar(Variant& val) const; // #IGNORE evaluate math operator on given value

  void 	Initialize();
  void	Destroy()	{ };
  TA_SIMPLE_BASEFUNS(SimpleMathSpec);
};

class TA_API taMath : public taBase {
  // ##CAT_Math collection of commonly-used math functions
public:

  /////////////////////////////////////////////////////////////////////////////////
  // Params: parameters controlling various math functions
  
  static int max_iterations;
  // #DEF_100 #CAT_Params maximum number of iterations for iterative functions
  static double err_tolerance;
  // #DEF_3.0e-7 #CAT_Params error tolerance for iterative functions to stop

  /////////////////////////////////////////////////////////////////////////////////
  // Arithmetic: basic arithmetic

  static double FLT_MAX;
  // #CAT_Arithmetic
  static double FLT_MAX_10_EXP;
  // #CAT_Arithmetic
  static double FLT_MAX_EXP;
  // #CAT_Arithmetic
  
  static double FLT_MIN;
  // #CAT_Arithmetic
  static double FLT_MIN_10_EXP;
  // #CAT_Arithmetic
  static double FLT_MIN_EXP;
  // #CAT_Arithmetic
  static double FLT_EPSILON;
  // #CAT_Arithmetic

  static double DBL_MAX;
  // #CAT_Arithmetic
  static double DBL_MAX_10_EXP;
  // #CAT_Arithmetic
  static double DBL_MAX_EXP;
  // #CAT_Arithmetic

  static double DBL_MIN;
  // #CAT_Arithmetic
  static double DBL_MIN_10_EXP;
  // #CAT_Arithmetic
  static double DBL_MIN_EXP;
  // #CAT_Arithmetic
  static double DBL_EPSILON;
  // #CAT_Arithmetic

  enum DistMetric {		// generalized distance metrics
    SUM_SQUARES,		// sum of squares:  sum[(x-y)^2]
    EUCLIDIAN,			// Euclidian distance (sqrt of sum of squares)
    HAMMING, 			// Hamming distance: sum[abs(x-y)]
    COVAR,			// covariance: sum[(x-<x>)(y-<y>)]
    CORREL,			// correlation: sum[(x-<x>)(y-<y>)] / sqrt(sum[x^2 y^2])
    INNER_PROD,			// inner product: sum[x y]
    CROSS_ENTROPY		// cross entropy: sum[x ln(x/y) + (1-x)ln((1-x)/(1-y))]
  };

  static bool	dist_larger_further(DistMetric metric);
  // returns true if a larger value of given distance metric means further apart

  void Initialize() { };
  void Destroy() { };
  TA_BASEFUNS(taMath);
};

////////////////////////////////////////////////////////////////////////////////
// 			double precision math
////////////////////////////////////////////////////////////////////////////////

class TA_API taMath_double : public taMath {
  // double precision math computations
public:

  /////////////////////////////////////////////////////////////////////////////////
  // Arithmetic: basic arithmetic

  static double  add(double x, double y) { return x + y; }
  // #CAT_Arithmetic add
  static double  sub(double x, double y) { return x - y; }
  // #CAT_Arithmetic subtract
  static double  mult(double x, double y) { return x * y; }
  // #CAT_Arithmetic multiply
  static double  div(double x, double y) { return x / y; }
  // #CAT_Arithmetic divide

  static double  fabs(double x) { return std::fabs(x); }
  // #CAT_Arithmetic absolute value
//   static double copysign(double x, double y) { return std::copysign(x, y); }
//   // #CAT_Arithmetic copy sign from x to y

//   static double nextafter(double x, double y) { return std::nextafter(x, y); }
//   // #CAT_Arithmetic return next largest distinct floating point number after x in direction of y

  static double  min(double x, double y) { return (x < y) ? x : y; }
  // #CAT_Arithmetic minimum of x and y
  static double  max(double x, double y) { return (x > y) ? x : y; }
  // #CAT_Arithmetic maximum of x and y
//   static double fmax(double x, double y) { return std::fmax(x,y); }
//   // #CAT_Arithmetic maximum of x and y
//   static double fmin(double x, double y) { return std::fmin(x,y); }
//   // #CAT_Arithmetic minimum of x and y

  static double ceil(double x) { return std::ceil(x); }
  // #CAT_Arithmetic ceiling of x: next largest integer value from x
  static double floor(double x) { return std::floor(x); }
  // #CAT_Arithmetic floor of x: next smallest integer value from x
//   static double rint(double x) { return std::rint(x); }
//   // #CAT_Arithmetic round value to an integer using current rounding direction

//   static double round(double x) { return std::round(x); }
//   // #CAT_Arithmetic round value to an integer irrespective of current rounding direction
//   static double trunc(double x) { return std::trunc(x); }
//   // #CAT_Arithmetic round to truncated integer value (nearest lower magnitude integer) -- like floor but irrespective of sign

  static double fmod(double x, double y) { return std::fmod(x, y); }
  // #CAT_Arithmetic floating-point modulus function: remainder of x / y

//   static double remainder(double x, double y) { return std::remainder(x, y); }
//   // #CAT_Arithmetic remainder r = x - ny where n is integer value nearest to x/y

  /////////////////////////////////////////////////////////////////////////////////
  // ExpLog: exponential and logarithmic functions

  static double e;
  // #CAT_Trigonometry #READ_ONLY e: the natural exponential number

  static double exp(double x) { return std::exp(x); }
  // #CAT_ExpLog The natural exponential (e to the power x: e^x)
//   static double exp2(double x) { return std::exp2(x); }
//   // #CAT_ExpLog The base-2 expoenetial (2 to the power x: 2^x)
  static double log(double x) { return std::log(x); }
  // #CAT_ExpLog The natural logarithm of x: ln(x)
  static double log10(double x) { return std::log10(x); }
  // #CAT_ExpLog The logarithm of x, base 10
//   static double log2(double x) { return std::log2(x); }
//   // #CAT_ExpLog The logarithm of x, base 2
  static double pow(double x, double p) { return std::pow(x, p); }
  // #CAT_ExpLog x to the power p (x^p)
  static double sqrt(double x) { return std::sqrt(x); }
  // #CAT_ExpLog square root of x (i.e., x^1/2)

  /////////////////////////////////////////////////////////////////////////////////
  // Trigonometry

  static double pi;
  // #CAT_Trigonometry #READ_ONLY pi: the ratio of circumference to diameter
  static double deg_per_rad;
  // #CAT_Trigonometry #READ_ONLY degrees per radian (180 / pi)

//   static double hypot(double x, double y) { return std::hypot(x,y); }
//   // #CAT_Trigonometry the length of the hypotenuse (i.e., Euclidean distance): sqrt(x^2 + y^2)
  static double  acos(double X) { return std::acos(X); }
  // #CAT_Trigonometry The arc-cosine (inverse cosine) -- takes an X coordinate and returns the angle (in radians) such that cos(angle)=X
  static double  asin(double Y) { return std::asin(Y); }
  // #CAT_Trigonometry The arc-sine (inverse sine) -- takes a Y coordinate and returns the angle (in radians) such that sin(angle)=Y
  static double  atan(double Y_over_X) { return std::atan(Y_over_X); }
  // #CAT_Trigonometry The arc-tangent (inverse tangent) -- takes a Y/X slope and returns angle (in radians) such that tan(angle)=Y/X.
  static double  atan2(double Y, double X) { return std::atan2(Y, X); }
  // #CAT_Trigonometry The arc-tangent (inverse tangent) -- takes a Y/X slope and returns angle (in radians) such that tan(angle)=Y/X.

  static double  cos(double angle) { return std::cos(angle); }
  // #CAT_Trigonometry The cosine (x-axis component) of angle (given in radians)
  static double  cos_deg(double angle) { return std::cos(angle / deg_per_rad); }
  // #CAT_Trigonometry The cosine (x-axis component) of angle (given in degrees)

  static double  sin(double angle) { return std::sin(angle); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in radians)
  static double  sin_deg(double angle) { return std::sin(angle / deg_per_rad); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in degrees)

  static double  tan(double angle) { return std::tan(angle); }
  // #CAT_Trigonometry The tangent (slope y over x) of angle (given in radians)
  static double  tan_deg(double angle) { return std::tan(angle / deg_per_rad); }
  // #CAT_Trigonometry The tangent (slope y over x) of angle (given in degrees)

//   static double  acosh(double X) { return std::acosh(X); }
//   // #CAT_Trigonometry The arc-hyperbolic-cosine (inverse hyperbolic cosine) -- takes an X coordinate and returns the angle (in radians) such that cosh(angle)=X
//   static double  asinh(double Y) { return std::asinh(Y); }
//   // #CAT_Trigonometry The arc-hyperbolic-sine (inverse hyperbolic sine) -- takes a Y coordinate and returns the angle (in radians) such that sinh(angle)=Y
//   static double  atanh(double Y_over_X) { return std::atanh(Y_over_X); }
//   // #CAT_Trigonometry The arc-hyperbolic-tangent (inverse hyperbolic tangent) -- takes Y over X slope and returns the angle (in radians) such that tanh(angle)=Y
  static double  cosh(double z) { return std::cosh(z); }
  // #CAT_Trigonometry The hyperbolic-cosine = e^z + e^-z / 2
  static double  sinh(double z) { return std::sinh(z); }
  // #CAT_Trigonometry The hyperbolic-sine = e^z - e^-z / 2
  static double  tanh(double z) { return std::tanh(z); }
  // #CAT_Trigonometry The hyperbolic-tangent = sinh(z) / cosh(z)

  /////////////////////////////////////////////////////////////////////////////////
  // Probability distributions and related functions

  static double fact_ln(int n);
  // #CAT_Probability natural log (ln) of n factorial (n!)
  static double bico_ln(int n, int j);
  // #CAT_Probability natural log (ln) of n choose j (binomial)
  static double hyperg(int j, int s, int t, int n);
  // #CAT_Probability hypergeometric (j t's of sample s in n) 
  static double gamma_ln(double z);
  // #CAT_Probability natural log (ln) of gamma function (not gamma distribution): generalization of (n-1)! to real values 
//   static double  lgamma(double z) { return std::lgamma(z); }
//   // #CAT_Probability natural log (ln) of gamma function (not gamma distribution): generalization of (n-1)! to real values 
//   static double tgamma(double z) { return std::tgamma(z); }
//   // #CAT_Probability true gamma function (not gamma distribution): generalization of (n-1)! to real values 

  static double gamma_p(double a, double x);
  // #CAT_Probability incomplete gamma function 
  static double gamma_q(double a, double x);
  // #CAT_Probability incomplete gamma function (complement of p)
  static double beta(double z, double w);
  // #CAT_Probability beta function 
  static double beta_i(double a, double b, double x);
  // #CAT_Probability incomplete beta function 

  static double binom_den(int n, int j, double p);
  // #CAT_Probability binomial probability function 
  static double binom_cum(int n, int j, double p);
  // #CAT_Probability cumulative binomial probability 
  static double binom_dev(int n, double p);
  // #CAT_Probability binomial deviate: p prob with n trials 

  static double poisson_den(int j, double l);
  // #CAT_Probability poisson distribution 
  static double poisson_cum(int j, double l);
  // #CAT_Probability cumulative Poisson P_l(<j) (0 thru j-1) 
  static double poisson_dev(double l);
  // #CAT_Probability poisson deviate:  mean is l 

  static double gamma_den(int j, double l, double t);
  // #CAT_Probability gamma probability distribution: j events, l lambda, t time 
  static double gamma_cum(int j, double l, double t);
  // #CAT_Probability gamma cumulative: j events, l lambda, t time 
  static double gamma_dev(int j);
  // #CAT_Probability gamma deviate: how long to wait until j events with  unit lambda 

  static double gauss_den(double x);
  // #CAT_Probability gaussian (normal) distribution 
  static double gauss_cum(double z);
  // #CAT_Probability cumulative gaussian (unit variance) to z 
  static double gauss_inv(double p);
  // #CAT_Probability inverse of the cumulative for p: z value for given p 
  static double gauss_dev();
  // #CAT_Probability gaussian deviate: normally distributed 
  static double erf(double x) { return erf(x); }
  // #CAT_Probability the error function: used for computing the normal distribution
  static double erfc(double x) { return erfc(x); }
  // #CAT_Probability complement of the error function (1.0 - erf(x))

  static double chisq_p(double X, double v);
  // #CAT_Probability P(X^2 | v) 
  static double chisq_q(double X, double v);
  // #CAT_Probability Q(X^2 | v) (complement) 
  static double students_cum(double t, double df);
  // #CAT_Probability cumulative student's distribution df deg of free t test
  static double students_den(double t, double df);
  // #CAT_Probability density fctn of student's distribution df deg of free t test
  static double Ftest_q(double F, double v1, double v2);
  // #CAT_Probability F distribution probability F | (v1 < v2) 

  /////////////////////////////////////////////////////////////////////////////////
  // Vector operations (operate on Matrix objects, treating as a single linear guy)

  ///////////////////////////////////////
  // arithmetic ops

  static bool	vec_check_same_size(const double_Matrix* a, const double_Matrix* b, bool quiet = false);
  // check that both vectors are the same size, and issue warning if not (unless quiet)

  static bool  vec_add(double_Matrix* a, const double_Matrix* b);
  // #CAT_Arithmetic add elements in two vectors: a(i) += b(i)
  static bool  vec_sub(double_Matrix* a, const double_Matrix* b);
  // #CAT_Arithmetic subtract elements in two vectors: a(i) += b(i)
  static bool  vec_mult_els(double_Matrix* a, const double_Matrix* b);
  // #CAT_Arithmetic multiply elements in two vectors: a(i) *= b(i)
  static bool  vec_div_els(double_Matrix* a, const double_Matrix* b);
  // #CAT_Arithmetic divide elements in two vectors: a(i) /= b(i)

  static bool	vec_simple_math(double_Matrix* vec, const SimpleMathSpec& math_spec);
  // #CAT_Arithmetic apply standard kinds of simple math operators to values in the vector

  static bool	vec_simple_math_arg(double_Matrix* vec, const double_Matrix* arg_vec,
				    const SimpleMathSpec& math_spec);
  // #CAT_Arithmetic apply simple math operators to values in vector, other vector provides 'arg' value for math_spec

  ///////////////////////////////////////
  // basic statistics

  static double	vec_max(const double_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the maximum value
  static double	vec_abs_max(const double_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the maximum absolute value
  static double	vec_min(const double_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the minimum value
  static double	vec_abs_min(const double_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the minimum value

  static double	vec_sum(const double_Matrix* vec);
  // #CAT_Statistics compute the sum of the values in the vector
  static double	vec_prod(const double_Matrix* vec);
  // #CAT_Statistics compute the product of the values in the vector
  static double	vec_mean(const double_Matrix* vec);
  // #CAT_Statistics compute the mean of the values in the vector
  static double	vec_var(const double_Matrix* vec, double mean=0, bool use_mean=false);
  // #CAT_Statistics compute the variance of the values, opt with given mean
  static double	vec_std_dev(const double_Matrix* vec, double mean=0, bool use_mean=false);
  // #CAT_Statistics compute the standard deviation of the values, opt with given mean
  static double	vec_sem(const double_Matrix* vec, double mean=0, bool use_mean=false);
  // #CAT_Statistics compute the standard error of the mean of the values, opt with given mean
  static double	vec_ss_len(const double_Matrix* vec);
  // #CAT_Statistics sum-of-squares length of the vector
  static void	vec_histogram(double_Matrix* hist_vec, const double_Matrix* src_vec,
			      double bin_size);
  // #CAT_Statistics gets a histogram (counts) of number of values within each bin size in source vector
  static double	vec_count(const double_Matrix* vec, CountParam& cnt);
  // #CAT_Statistics count number of times count relationship is true

  ///////////////////////////////////////
  // distance metrics (comparing two vectors)

  static double	vec_ss_dist(const double_Matrix* vec, const double_Matrix* oth_vec,
			    bool norm = false, double tolerance=0.0f);
  // #CAT_Distance compute sum-squares dist between this and the oth, tolerance is by element
  static double	vec_euclid_dist(const double_Matrix* vec, const double_Matrix* oth_vec,
				bool norm = false, double tolerance=0.0f);
  // #CAT_Distance compute Euclidian dist between this and the oth, tolerance is by element
  static double	vec_hamming_dist(const double_Matrix* vec, const double_Matrix* oth_vec,
				 bool norm = false, double tolerance=0.0f);
  // #CAT_Distance compute Hamming dist between this and the oth, tolerance is by element
  static double	vec_covar(const double_Matrix* vec, const double_Matrix* oth_vec);
  // #CAT_Distance compute the covariance of this vector the oth vector
  static double	vec_correl(const double_Matrix* vec, const double_Matrix* oth_vec);
  // #CAT_Distance compute the correlation of this vector with the oth vector
  static double	vec_inner_prod(const double_Matrix* vec, const double_Matrix* oth_vec,
			       bool norm = false);
  // #CAT_Distance compute the inner product of this vector and the oth vector
  static double	vec_cross_entropy(const double_Matrix* vec, const double_Matrix* oth_vec);
  // #CAT_Distance compute cross entropy between this and other vector, this is 'p' other is 'q'
  static double	vec_dist(const double_Matrix* vec, const double_Matrix* oth_vec,
			 DistMetric metric, bool norm = false, double tolerance=0.0f);
  // #CAT_Distance compute generalized distance metric with other vector (double_Matrix* vec, calls appropriate fun above)

  ///////////////////////////////////////
  // Normalization

  static double	vec_norm_len(double_Matrix* vec, double len=1.0f);
  // #CAT_Norm normalize vector to total given length (1.0), returns scaling factor
  static double	vec_norm_sum(double_Matrix* vec, double sum=1.0f, double min_val=0.0f);
  // #CAT_Norm normalize vector to total given sum (1.0) and min_val (0), returns scaling factor
  static double	vec_norm_max(double_Matrix* vec, double max=1.0f);
  // #CAT_Norm normalize vector to given maximum value, returns scaling factor
  static double	vec_norm_abs_max(double_Matrix* vec, double max=1.0f);
  // #CAT_Norm normalize vector to given absolute maximum value, returns scaling factor
  static int	vec_threshold(double_Matrix* vec, double thresh=.5f,
			      double low=0.0f, double high=1.0f);
  // #CAT_Norm threshold values in the vector, low vals go to low, etc; returns number of high values
  static double	vec_aggregate(const double_Matrix* vec, Aggregate& agg);
  // #CAT_Aggregate compute aggregate of values in this vector using aggregation params of agg

  /////////////////////////////////////////////////////////////////////////////////
  // Matrix operations

#ifdef HAVE_GSL
  static bool mat_get_gsl_fm_ta(double_Matrix* ta_mat, gsl_matrix* gsl_mat);
  // #IGNORE helper function to get a gsl-formatted matrix from a ta matrix

  static bool mat_add(double_Matrix* a, double_Matrix* b);
  // #CAT_Matrix add the elements of matrix b to the elements of matrix a: a(i,j) += b(i,j); the two matricies must have the same dimensions
  static bool mat_sub(double_Matrix* a, double_Matrix* b);
  // #CAT_Matrix subtract the elements of matrix b to the elements of matrix a: a(i,j) -= b(i,j); the two matricies must have the same dimensions
  static bool mat_mult_els(double_Matrix* a, double_Matrix* b);
  // #CAT_Matrix multiply the elements of matrix b with the elements of matrix a: a(i,j) *= b(i,j); the two matricies must have the same dimensions
  static bool mat_div_els(double_Matrix* a, double_Matrix* b);
  // #CAT_Matrix divide the elements of matrix b by the elements of matrix a: a(i,j) /= b(i,j); the two matricies must have the same dimensions
#endif
  
  void Initialize() { };
  void Destroy() { };
  TA_BASEFUNS(taMath_double);
};


/////////////////////////////////////////////
// 	float (single precision)
/////////////////////////////////////////////

class TA_API taMath_float : public taMath {
  // single-precision (float) math computations
public:

  /////////////////////////////////////////////////////////////////////////////////
  // Arithmetic: basic arithmetic

  static float  add(float x, float y) { return x + y; }
  // #CAT_Arithmetic add
  static float  sub(float x, float y) { return x - y; }
  // #CAT_Arithmetic subtract
  static float  mult(float x, float y) { return x * y; }
  // #CAT_Arithmetic multiply
  static float  div(float x, float y) { return x / y; }
  // #CAT_Arithmetic divide

  static float  fabs(float x) { return std::fabs(x); }
  // #CAT_Arithmetic absolute value
//   static float copysign(float x, float y) { return copysignf(x, y); }
//   // #CAT_Arithmetic copy sign from x to y

//   static float nextafter(float x, float y) { return nextafterf(x, y); }
//   // #CAT_Arithmetic return next largest distinct floating point number after x in direction of y

  static float  min(float x, float y) { return (x < y) ? x : y; }
  // #CAT_Arithmetic minimum of x and y
  static float  max(float x, float y) { return (x > y) ? x : y; }
  // #CAT_Arithmetic maximum of x and y
//   static float fmax(float x, float y) { return fmaxf(x,y); }
//   // #CAT_Arithmetic maximum of x and y
//   static float fmin(float x, float y) { return fminf(x,y); }
//   // #CAT_Arithmetic minimum of x and y

  static float ceil(float x) { return std::ceil(x); }
  // #CAT_Arithmetic ceiling of x: next largest integer value from x
  static float floor(float x) { return std::floor(x); }
  // #CAT_Arithmetic floor of x: next smallest integer value from x
//   static float rint(float x) { return rintf(x); }
//   // #CAT_Arithmetic round value to an integer using current rounding direction

//   static float round(float x) { return roundf(x); }
//   // #CAT_Arithmetic round value to an integer irrespective of current rounding direction
//   static float trunc(float x) { return truncf(x); }
//   // #CAT_Arithmetic round to truncated integer value (nearest lower magnitude integer) -- like floor but irrespective of sign

  static float fmod(float x, float y) { return std::fmod(x, y); }
  // #CAT_Arithmetic floating-point modulus function: remainder of x / y

//   static float remainder(float x, float y) { return remainderf(x, y); }
//   // #CAT_Arithmetic remainder r = x - ny where n is integer value nearest to x/y

  /////////////////////////////////////////////////////////////////////////////////
  // ExpLog: exponential and logarithmic functions

  static float e;
  // #CAT_Trigonometry #READ_ONLY e: the natural exponential number

  static float exp(float x) { return std::exp(x); }
  // #CAT_ExpLog The natural exponential (e to the power x: e^x)
//   static float exp2(float x) { return exp2f(x); }
//   // #CAT_ExpLog The base-2 expoenetial (2 to the power x: 2^x)
  static float log(float x) { return std::log(x); }
  // #CAT_ExpLog The natural logarithm of x: ln(x)
  static float log10(float x) { return std::log10(x); }
  // #CAT_ExpLog The logarithm of x, base 10
//   static float log2(float x) { return log2f(x); }
//   // #CAT_ExpLog The logarithm of x, base 2
  static float pow(float x, float p) { return std::pow(x, p); }
  // #CAT_ExpLog x to the power p (x^p)
  static float sqrt(float x) { return std::sqrt(x); }
  // #CAT_ExpLog square root of x (i.e., x^1/2)

  /////////////////////////////////////////////////////////////////////////////////
  // Trigonometry

  static float pi;
  // #CAT_Trigonometry #READ_ONLY pi: the ratio of circumference to diameter
  static float deg_per_rad;
  // #CAT_Trigonometry #READ_ONLY degrees per radian (180 / pi)

//   static float hypot(float x, float y) { return hypotf(x,y); }
//   // #CAT_Trigonometry the length of the hypotenuse (i.e., Euclidean distance): sqrt(x^2 + y^2)
  static float  acos(float X) { return std::acos(X); }
  // #CAT_Trigonometry The arc-cosine (inverse cosine) -- takes an X coordinate and returns the angle (in radians) such that cos(angle)=X
  static float  asin(float Y) { return std::asin(Y); }
  // #CAT_Trigonometry The arc-sine (inverse sine) -- takes a Y coordinate and returns the angle (in radians) such that sin(angle)=Y
  static float  atan(float Y_over_X) { return std::atan(Y_over_X); }
  // #CAT_Trigonometry The arc-tangent (inverse tangent) -- takes a Y/X slope and returns angle (in radians) such that tan(angle)=Y/X.
  static float  atan2(float Y, float X) { return std::atan2(Y, X); }
  // #CAT_Trigonometry The arc-tangent (inverse tangent) -- takes a Y/X slope and returns angle (in radians) such that tan(angle)=Y/X.

  static float  cos(float angle) { return std::cos(angle); }
  // #CAT_Trigonometry The cosine (x-axis component) of angle (given in radians)
  static float  cos_deg(float angle) { return std::cos(angle / deg_per_rad); }
  // #CAT_Trigonometry The cosine (x-axis component) of angle (given in degrees)

  static float  sin(float angle) { return std::sin(angle); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in radians)
  static float  sin_deg(float angle) { return std::sin(angle / deg_per_rad); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in degrees)

  static float  tan(float angle) { return std::tan(angle); }
  // #CAT_Trigonometry The tangent (slope y over x) of angle (given in radians)
  static float  tan_deg(float angle) { return std::tan(angle / deg_per_rad); }
  // #CAT_Trigonometry The tangent (slope y over x) of angle (given in degrees)

//   static float  acosh(float X) { return acoshf(X); }
//   // #CAT_Trigonometry The arc-hyperbolic-cosine (inverse hyperbolic cosine) -- takes an X coordinate and returns the angle (in radians) such that cosh(angle)=X
//   static float  asinh(float Y) { return asinhf(Y); }
//   // #CAT_Trigonometry The arc-hyperbolic-sine (inverse hyperbolic sine) -- takes a Y coordinate and returns the angle (in radians) such that sinh(angle)=Y
//   static float  atanh(float Y_over_X) { return atanhf(Y_over_X); }
//   // #CAT_Trigonometry The arc-hyperbolic-tangent (inverse hyperbolic tangent) -- takes Y over X slope and returns the angle (in radians) such that tanh(angle)=Y
  static float  cosh(float z) { return std::cosh(z); }
  // #CAT_Trigonometry The hyperbolic-cosine = e^z + e^-z / 2
  static float  sinh(float z) { return std::sinh(z); }
  // #CAT_Trigonometry The hyperbolic-sine = e^z - e^-z / 2
  static float  tanh(float z) { return std::tanh(z); }
  // #CAT_Trigonometry The hyperbolic-tangent = sinh(z) / cosh(z)

  /////////////////////////////////////////////////////////////////////////////////
  // Vector operations (operate on Matrix objects, treating as a single linear guy)

  ///////////////////////////////////////
  // arithmetic ops

  static bool	vec_check_same_size(const float_Matrix* a, const float_Matrix* b, bool quiet = false);
  // check that both vectors are the same size, and issue warning if not (unless quiet)

  static bool  vec_add(float_Matrix* a, const float_Matrix* b);
  // #CAT_Arithmetic add elements in two vectors: a(i) += b(i)
  static bool  vec_sub(float_Matrix* a, const float_Matrix* b);
  // #CAT_Arithmetic subtract elements in two vectors: a(i) += b(i)
  static bool  vec_mult_els(float_Matrix* a, const float_Matrix* b);
  // #CAT_Arithmetic multiply elements in two vectors: a(i) *= b(i)
  static bool  vec_div_els(float_Matrix* a, const float_Matrix* b);
  // #CAT_Arithmetic divide elements in two vectors: a(i) /= b(i)

  static bool	vec_simple_math(float_Matrix* vec, const SimpleMathSpec& math_spec);
  // #CAT_Arithmetic apply standard kinds of simple math operators to values in the vector

  static bool	vec_simple_math_arg(float_Matrix* vec, const float_Matrix* arg_vec,
				    const SimpleMathSpec& math_spec);
  // #CAT_Arithmetic apply simple math operators to values in vector, other vector provides 'arg' value for math_spec

  ///////////////////////////////////////
  // basic statistics

  static float	vec_max(const float_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the maximum value
  static float	vec_abs_max(const float_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the maximum absolute value
  static float	vec_min(const float_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the minimum value
  static float	vec_abs_min(const float_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the minimum value

  static float	vec_sum(const float_Matrix* vec);
  // #CAT_Statistics compute the sum of the values in the vector
  static float	vec_prod(const float_Matrix* vec);
  // #CAT_Statistics compute the product of the values in the vector
  static float	vec_mean(const float_Matrix* vec);
  // #CAT_Statistics compute the mean of the values in the vector
  static float	vec_var(const float_Matrix* vec, float mean=0, bool use_mean=false);
  // #CAT_Statistics compute the variance of the values, opt with given mean
  static float	vec_std_dev(const float_Matrix* vec, float mean=0, bool use_mean=false);
  // #CAT_Statistics compute the standard deviation of the values, opt with given mean
  static float	vec_sem(const float_Matrix* vec, float mean=0, bool use_mean=false);
  // #CAT_Statistics compute the standard error of the mean of the values, opt with given mean
  static float	vec_ss_len(const float_Matrix* vec);
  // #CAT_Statistics sum-of-squares length of the vector
  static void	vec_histogram(float_Matrix* hist_vec, const float_Matrix* src_vec,
			      float bin_size);
  // #CAT_Statistics gets a histogram (counts) of number of values within each bin size in source vector
  static float	vec_count(const float_Matrix* vec, CountParam& cnt);
  // #CAT_Statistics count number of times count relationship is true

  ///////////////////////////////////////
  // distance metrics (comparing two vectors)

  static float	vec_ss_dist(const float_Matrix* vec, const float_Matrix* oth_vec,
			    bool norm = false, float tolerance=0.0f);
  // #CAT_Distance compute sum-squares dist between this and the oth, tolerance is by element
  static float	vec_euclid_dist(const float_Matrix* vec, const float_Matrix* oth_vec,
				bool norm = false, float tolerance=0.0f);
  // #CAT_Distance compute Euclidian dist between this and the oth, tolerance is by element
  static float	vec_hamming_dist(const float_Matrix* vec, const float_Matrix* oth_vec,
				 bool norm = false, float tolerance=0.0f);
  // #CAT_Distance compute Hamming dist between this and the oth, tolerance is by element
  static float	vec_covar(const float_Matrix* vec, const float_Matrix* oth_vec);
  // #CAT_Distance compute the covariance of this vector the oth vector
  static float	vec_correl(const float_Matrix* vec, const float_Matrix* oth_vec);
  // #CAT_Distance compute the correlation of this vector with the oth vector
  static float	vec_inner_prod(const float_Matrix* vec, const float_Matrix* oth_vec,
			       bool norm = false);
  // #CAT_Distance compute the inner product of this vector and the oth vector
  static float	vec_cross_entropy(const float_Matrix* vec, const float_Matrix* oth_vec);
  // #CAT_Distance compute cross entropy between this and other vector, this is 'p' other is 'q'
  static float	vec_dist(const float_Matrix* vec, const float_Matrix* oth_vec,
			 DistMetric metric, bool norm = false, float tolerance=0.0f);
  // #CAT_Distance compute generalized distance metric with other vector (float_Matrix* vec, calls appropriate fun above)

  ///////////////////////////////////////
  // Normalization

  static float	vec_norm_len(float_Matrix* vec, float len=1.0f);
  // #CAT_Norm normalize vector to total given length (1.0), returns scaling factor
  static float	vec_norm_sum(float_Matrix* vec, float sum=1.0f, float min_val=0.0f);
  // #CAT_Norm normalize vector to total given sum (1.0) and min_val (0), returns scaling factor
  static float	vec_norm_max(float_Matrix* vec, float max=1.0f);
  // #CAT_Norm normalize vector to given maximum value, returns scaling factor
  static float	vec_norm_abs_max(float_Matrix* vec, float max=1.0f);
  // #CAT_Norm normalize vector to given absolute maximum value, returns scaling factor
  static int	vec_threshold(float_Matrix* vec, float thresh=.5f,
			      float low=0.0f, float high=1.0f);
  // #CAT_Norm threshold values in the vector, low vals go to low, etc; returns number of high values
  static float	vec_aggregate(const float_Matrix* vec, Aggregate& agg);
  // #CAT_Aggregate compute aggregate of values in this vector using aggregation params of agg

  /////////////////////////////////////////////////////////////////////////////////
  // Matrix operations

#ifdef HAVE_GSL
  static bool mat_get_gsl_fm_ta(float_Matrix* ta_mat, gsl_matrix_float* gsl_mat);
  // #IGNORE helper function to get a gsl-formatted matrix from a ta matrix

  static bool mat_add(float_Matrix* a, float_Matrix* b);
  // #CAT_Matrix add the elements of matrix b to the elements of matrix a: a(i,j) += b(i,j); the two matricies must have the same dimensions
  static bool mat_sub(float_Matrix* a, float_Matrix* b);
  // #CAT_Matrix subtract the elements of matrix b to the elements of matrix a: a(i,j) -= b(i,j); the two matricies must have the same dimensions
  static bool mat_mult_els(float_Matrix* a, float_Matrix* b);
  // #CAT_Matrix multiply the elements of matrix b with the elements of matrix a: a(i,j) *= b(i,j); the two matricies must have the same dimensions
  static bool mat_div_els(float_Matrix* a, float_Matrix* b);
  // #CAT_Matrix divide the elements of matrix b by the elements of matrix a: a(i,j) /= b(i,j); the two matricies must have the same dimensions
#endif

  // todo: copy over from float and replace all w/ float when double is all good..

  void Initialize() { };
  void Destroy() { };
  TA_BASEFUNS(taMath_float);
};

#endif // TA_MATH_H
