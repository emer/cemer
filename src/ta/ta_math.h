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
#include "ta_mtrnd.h"
#include "ta_dmem.h"

#ifdef HAVE_LIBGSL
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_matrix_float.h>
#endif

class TA_API CountParam : public taBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math counting criteria params
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
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math Basic aggregation operations
  INHERITED(taOBase)
public:
  enum Operator {		// Aggregate Operators
    GROUP,			// group by this field
    FIRST,			// first item
    LAST,			// last item
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
    MEDIAN,			// middle item (note: requires sorting)
    MODE,			// most frequent item (note: requires sorting)
  };

  Operator      op;		// how to aggregate over the network
  CountParam	count;		// #CONDEDIT_ON_op:COUNT parameters for the COUNT aggregation

  virtual String GetAggName() const;  // get string representation of aggregation opr

  void 	Initialize();
  void 	Destroy();
  TA_SIMPLE_BASEFUNS(Aggregate);
};

class TA_API SimpleMathSpec : public taBase {
  // #INLINE #INLINE_DUMP #NO_UPDATE_AFTER ##NO_TOKENS ##CAT_Math params for std kinds of simple math operators
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

  static float flt_max;
  // #CAT_Arithmetic maximum value representable by a floating point number (typ 1e37)
  static float flt_max_10_exp;
  // #CAT_Arithmetic maximum base 10 exponent representable by a floating point number (typ 37)
  static float flt_max_exp;
  // #CAT_Arithmetic maximum radix (base of float, always 2) exponent
  
  static float flt_min;
  // #CAT_Arithmetic minimum value representable by a floating point number (typ 1e-37)
  static float flt_min_10_exp;
  // #CAT_Arithmetic minimum base 10 exponent representable by a floating point number (typ 37)
  static float flt_min_exp;
  // #CAT_Arithmetic minimum radix (base of float, always 2) exponent
  static float flt_epsilon;
  // #CAT_Arithmetic minimum positive floating point number such that 1.0f + flt_epsilon != 1.0f

  static double dbl_max;
  // #CAT_Arithmetic maximum value representable by a floating point number (typ 1e308)
  static double dbl_max_10_exp;
  // #CAT_Arithmetic maximum base 10 exponent representable by a floating point number (typ 308)
  static double dbl_max_exp;
  // #CAT_Arithmetic maximum radix (base of float, always 2) exponent

  static double dbl_min;
  // #CAT_Arithmetic minimum value representable by a floating point number (typ 1e-308)
  static double dbl_min_10_exp;
  // #CAT_Arithmetic minimum base 10 exponent representable by a floating point number (typ 308)
  static double dbl_min_exp;
  // #CAT_Arithmetic minimum radix (base of float, always 2) exponent
  static double dbl_epsilon;
  // #CAT_Arithmetic minimum positive floating point number such that 1.0 + dbl_epsilon != 1.0

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

  static void mat_cvt_double_to_float(float_Matrix* flt_dest, const double_Matrix* dbl_src);
  // #CAT_Convert convert double vector/matrix data to float
  static void mat_cvt_float_to_double(double_Matrix* dbl_dest, const float_Matrix* flt_src);
  // #CAT_Convert convert float vector/matrix data to double

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

  static double	vec_first(const double_Matrix* vec);
  // #CAT_Statistics first item in the vector
  static double	vec_last(const double_Matrix* vec);
  // #CAT_Statistics last item in the vector
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
  static double	vec_median(const double_Matrix* vec);
  // #CAT_Statistics compute the median of the values in the vector (middle value) -- requires sorting
  static double	vec_mode(const double_Matrix* vec);
  // #CAT_Statistics compute the mode (most frequent) of the values in the vector -- requires sorting

  static void	vec_sort(double_Matrix* vec, bool descending = false);
  // #CAT_Statistics sort the values from the source vector into the dest vector

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
  // Standard Matrix operations: operate on a 2-dimensional matrix

#ifdef HAVE_LIBGSL
  static bool mat_get_gsl_fm_ta(gsl_matrix* gsl_mat, const double_Matrix* ta_mat);
  // #IGNORE helper function to get a gsl-formatted matrix from a ta matrix
  static bool vec_get_gsl_fm_ta(gsl_vector* gsl_vec, const double_Matrix* ta_vec);
  // #IGNORE helper function to get a gsl-formatted vector from a one-dimensional ta matrix (vector)
#endif

  static bool mat_col(double_Matrix* col, const double_Matrix* mat, int col_no);
  // #CAT_Matrix get indicated column number from two-d matrix
  static bool mat_row(double_Matrix* row, const double_Matrix* mat, int row_no);
  // #CAT_Matrix get indicated row number from two-d matrix

  static bool mat_add(double_Matrix* a, const double_Matrix* b);
  // #CAT_Matrix add the elements of matrix b to the elements of matrix a: a(i,j) += b(i,j); the two matricies must have the same dimensions
  static bool mat_sub(double_Matrix* a, const double_Matrix* b);
  // #CAT_Matrix subtract the elements of matrix b to the elements of matrix a: a(i,j) -= b(i,j); the two matricies must have the same dimensions
  static bool mat_mult_els(double_Matrix* a, const double_Matrix* b);
  // #CAT_Matrix multiply the elements of matrix b with the elements of matrix a: a(i,j) *= b(i,j); the two matricies must have the same dimensions
  static bool mat_div_els(double_Matrix* a, const double_Matrix* b);
  // #CAT_Matrix divide the elements of matrix b by the elements of matrix a: a(i,j) /= b(i,j); the two matricies must have the same dimensions

  static bool mat_eigen_owrite(double_Matrix* A, double_Matrix* eigen_vals, double_Matrix* eigen_vecs);
  // #CAT_Matrix compute the eigenvalues and eigenvectors of matrix A, which must be a square symmetric n x n matrix. the matrix is overwritten by the operation.  eigen_vals and eigen_vecs are automatically configured to the appropriate size if they are not already. eigens are sorted from highest to lowest by magnitude (absolute value)
  static bool mat_eigen(const double_Matrix* A, double_Matrix* eigen_vals, double_Matrix* eigen_vecs);
  // #CAT_Matrix compute the eigenvalues and eigenvectors of matrix A, which must be a square symmetric (n x n) matrix. this matrix is not affected by the operation (it is copied first).  eigen_vals and eigen_vecs are automatically configured to the appropriate size if they are not already.  eigens are sorted from highest to lowest by magnitude (absolute value)

  static bool mat_svd_owrite(double_Matrix* A, double_Matrix* S, double_Matrix* V);
  // #CAT_Matrix compute the singular value decomposition (SVD) of MxN matrix A into an orthogonal MxN matrix U times a diagonal NxN matrix S (diagonals returned as n-item vector S) times the transpose of an NxN orthogonal square matrix V.  matrix A is replaced by MxN orthogonal matrix U.  S and V are automatically configured to the appropriate size if they are not already.
  static bool mat_svd(const double_Matrix* A, double_Matrix* U, double_Matrix* S, double_Matrix* V);
  // #CAT_Matrix compute the singular value decomposition (SVD) of MxN matrix A into an orthogonal MxN matrix U times a diagonal NxN matrix S (diagonals returned as n-item vector S) times the transpose of an NxN orthogonal square matrix V. matrix A is not affeced by the operation (it is copied first).  S and V are automatically configured to the appropriate size if they are not already

  static bool mat_mds_owrite(double_Matrix* A, double_Matrix* x_y_coords, int x_component = 0,
			     int y_component = 1);
  // perform multidimensional scaling of matrix A (must be square symmetric matrix, e.g., a distance matrix), returning two-dimensional coordinates that best capture the distance relationships among the items (rows, columns) in x,y coordinates using specified components --  overwrites the matrix A
  static bool mat_mds(const double_Matrix* A, double_Matrix* x_y_coords, int x_component = 0,
		      int y_component = 1);
  // perform multidimensional scaling of matrix A (must be square symmetric matrix, e.g., a distance matrix), returning two-dimensional coordinates that best capture the distance relationships among the items (rows, columns) in x,y coordinates using specified components -- first copies the matrix A so it is not overwritten

  /////////////////////////////////////////////////////////////////////////////////
  // higher-dimensional matrix frame-based operations (matrix = collection of matricies)

  static bool mat_cell_to_vec(double_Matrix* vec, const double_Matrix* mat, int cell_no);
  // #CAT_HighDimMatrix extract given cell element across frames of matrix, and put in vector vec (usueful for analyzing behavior of a given cell across time or whatever the frames represent)
  static bool mat_dist(double_Matrix* dist_mat, const double_Matrix* src_mat,
		       DistMetric metric, bool norm = false, double tolerance=0.0f);
  // #CAT_HighDimMatrix compute distance matrix of frames within matrix src_mat (must be dim >= 2) -- dist_mat is nframes x nframes
  static bool mat_cross_dist(double_Matrix* dist_mat, const double_Matrix* src_mat_a,
			       const double_Matrix* src_mat_b,
			       DistMetric metric, bool norm = false, double tolerance=0.0f);
  // #CAT_HighDimMatrix compute cross distance matrix between the frames within src_mat_a and src_mat_b (must be dim >= 2 and have same frame size) -- rows of dist_mat are a, cols are b
  static bool mat_correl(double_Matrix* correl_mat, const double_Matrix* src_mat);
  // #CAT_HighDimMatrix compute correlation matrix for cells across frames within src_mat (i.e., how does each cell co-vary across time/frames with each other cell). result is nxn matrix where n is number of cells in each frame of src_mat (i.e., size of sub-matrix), with each cell being correlation of that cell with other cell.
  static bool mat_prjn(double_Matrix* prjn_vec, const double_Matrix* src_mat,
		       const double_Matrix* prjn_mat, DistMetric metric=INNER_PROD,
		       bool norm = false, double tolerance=0.0f);
  // #CAT_HighDimMatrix compute projection of each frame of src_mat onto prjn_mat.  prjn_vec contains one value for each frame in src_mat, which is the inner/dot product (projection -- or other metric if selected) of that frame and the prjn_mat.

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

  static float	vec_first(const float_Matrix* vec);
  // #CAT_Statistics first item in the vector
  static float	vec_last(const float_Matrix* vec);
  // #CAT_Statistics last item in the vector
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
  static float	vec_median(const float_Matrix* vec);
  // #CAT_Statistics compute the median of the values in the vector (middle value) -- requires sorting
  static float	vec_mode(const float_Matrix* vec);
  // #CAT_Statistics compute the mode (most frequent) of the values in the vector -- requires sorting

  static void	vec_sort(float_Matrix* vec, bool descending = false);
  // #CAT_Statistics sort the values from the source vector into the dest vector

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

#ifdef HAVE_LIBGSL
  static bool mat_get_gsl_fm_ta(gsl_matrix_float* gsl_mat, const float_Matrix* ta_mat);
  // #IGNORE helper function to get a gsl-formatted matrix from a ta matrix
  static bool vec_get_gsl_fm_ta(gsl_vector_float* gsl_vec, const float_Matrix* ta_vec);
  // #IGNORE helper function to get a gsl-formatted vector from a one-dimensional ta matrix (vector)
#endif

  static bool mat_col(float_Matrix* col, const float_Matrix* mat, int col_no);
  // #CAT_Matrix get indicated column number from two-d matrix
  static bool mat_row(float_Matrix* row, const float_Matrix* mat, int row_no);
  // #CAT_Matrix get indicated row number from two-d matrix

  static bool mat_add(float_Matrix* a, const float_Matrix* b);
  // #CAT_Matrix add the elements of matrix b to the elements of matrix a: a(i,j) += b(i,j); the two matricies must have the same dimensions
  static bool mat_sub(float_Matrix* a, const float_Matrix* b);
  // #CAT_Matrix subtract the elements of matrix b to the elements of matrix a: a(i,j) -= b(i,j); the two matricies must have the same dimensions
  static bool mat_mult_els(float_Matrix* a, const float_Matrix* b);
  // #CAT_Matrix multiply the elements of matrix b with the elements of matrix a: a(i,j) *= b(i,j); the two matricies must have the same dimensions
  static bool mat_div_els(float_Matrix* a, const float_Matrix* b);
  // #CAT_Matrix divide the elements of matrix b by the elements of matrix a: a(i,j) /= b(i,j); the two matricies must have the same dimensions

  // note: the following all involve copying to/from double -- underlying computation is done in the double routines, because that is what gsl supports!

  static bool mat_eigen_owrite(float_Matrix* A, float_Matrix* eigen_vals, float_Matrix* eigen_vecs);
  // #CAT_Matrix compute the eigenvalues and eigenvectors of matrix A, which must be a square symmetric n x n matrix. the matrix is overwritten by the operation.  eigen_vals and eigen_vecs are automatically configured to the appropriate size if they are not already.    eigens are sorted from highest to lowest by magnitude (absolute value)
  static bool mat_eigen(const float_Matrix* A, float_Matrix* eigen_vals, float_Matrix* eigen_vecs);
  // #CAT_Matrix compute the eigenvalues and eigenvectors of matrix A, which must be a square symmetric (n x n) matrix. this matrix is not affected by the operation (it is copied first).  eigen_vals and eigen_vecs are automatically configured to the appropriate size if they are not already.  eigens are sorted from highest to lowest by magnitude (absolute value)

  static bool mat_svd_owrite(float_Matrix* A, float_Matrix* S, float_Matrix* V);
  // #CAT_Matrix compute the singular value decomposition (SVD) of MxN matrix A into an orthogonal MxN matrix U times a diagonal NxN matrix S (diagonals returned as n-item vector S) times the transpose of an NxN orthogonal square matrix V.  matrix A is replaced by MxN orthogonal matrix U.  S and V are automatically configured to the appropriate size if they are not already.
  static bool mat_svd(const float_Matrix* A, float_Matrix* U, float_Matrix* S, float_Matrix* V);
  // #CAT_Matrix compute the singular value decomposition (SVD) of MxN matrix A into an orthogonal MxN matrix U times a diagonal NxN matrix S (diagonals returned as n-item vector S) times the transpose of an NxN orthogonal square matrix V. matrix A is not affeced by the operation (it is copied first).  S and V are automatically configured to the appropriate size if they are not already

  static bool mat_mds_owrite(float_Matrix* A, float_Matrix* x_y_coords, int x_component = 0,
			     int y_component = 1);
  // perform multidimensional scaling of matrix A (must be square symmetric matrix, e.g., a distance matrix), returning two-dimensional coordinates that best capture the distance relationships among the items (rows, columns) in x,y coordinates using specified components --  overwrites the matrix A
  static bool mat_mds(const float_Matrix* A, float_Matrix* x_y_coords, int x_component = 0,
		      int y_component = 1);
  // perform multidimensional scaling of matrix A (must be square symmetric matrix, e.g., a distance matrix), returning two-dimensional coordinates that best capture the distance relationships among the items (rows, columns) in x,y coordinates using specified components -- first copies the matrix A so it is not overwritten

  /////////////////////////////////////////////////////////////////////////////////
  // higher-dimensional matrix frame-based operations (matrix = collection of matricies)

  static bool mat_cell_to_vec(float_Matrix* vec, const float_Matrix* mat, int cell_no);
  // #CAT_HighDimMatrix extract given cell element across frames of matrix, and put in vector vec (usueful for analyzing behavior of a given cell across time or whatever the frames represent)
  static bool mat_dist(float_Matrix* dist_mat, const float_Matrix* src_mat,
			 DistMetric metric, bool norm = false, float tolerance=0.0f);
  // #CAT_HighDimMatrix compute distance matrix of frames within matrix src_mat (must be dim >= 2) -- dist_mat is nframes x nframes
  static bool mat_cross_dist(float_Matrix* dist_mat, const float_Matrix* src_mat_a,
			       const float_Matrix* src_mat_b,
			       DistMetric metric, bool norm = false, float tolerance=0.0f);
  // #CAT_HighDimMatrix compute cross distance matrix between the frames within src_mat_a and src_mat_b (must be dim >= 2 and have same frame size) -- rows of dist_mat are a, cols are b
  static bool mat_correl(float_Matrix* correl_mat, const float_Matrix* src_mat);
  // #CAT_HighDimMatrix compute correlation matrix for cells across frames within src_mat (i.e., how does each cell co-vary across time/frames with each other cell).  result is nxn matrix where n is number of cells in each frame of src_mat (i.e., size of sub-matrix), with each cell being correlation of that cell with other cell.

  static bool mat_prjn(float_Matrix* prjn_vec, const float_Matrix* src_mat,
		       const float_Matrix* prjn_mat, DistMetric metric=INNER_PROD,
		       bool norm = false, float tolerance=0.0f);
  // #CAT_HighDimMatrix compute projection of each frame of src_mat onto prjn_mat.  prjn_vec contains one value for each frame in src_mat, which is the inner/dot product (projection -- or other metric if selected) of that frame and the prjn_mat.

  void Initialize() { };
  void Destroy() { };
  TA_BASEFUNS(taMath_float);
};



////////////////////////////////////////////////////////////////////////////
//			Random Number Generation 

class TA_API RndSeed : public taNBase {
  // ##CAT_Math random seeds: can control the random number generator to restart with the same pseudo-random sequence or get a new one
INHERITED(taNBase)
public:
  int_Array		seed;	// #READ_ONLY the seed, 624 elements long
  int			mti;	// #READ_ONLY the index into the seed, also needs to be saved

  virtual void	 NewSeed();
  // set the seed to a new random value (based on time and process id)
  virtual void	 OldSeed();
  // restore current seed to random num generator
  virtual void	 GetCurrent();
  // get the current seed in use by the generator
  virtual void   Init(ulong i);
  // initialize the seed based on given initializer

  virtual void	DMem_Sync(MPI_Comm comm);
   // synchronize seeds across all procs -- uses the first proc's seed

  void	Initialize();
  void	Destroy()		 { CutLinks(); }
  TA_SIMPLE_BASEFUNS(RndSeed);
};

class TA_API RndSeed_List : public taList<RndSeed> {
  // ##CAT_Math list of random seeds
INHERITED(taList<RndSeed>)
public:
  virtual void	 NewSeeds();
  // #MENU #MENU_ON_Actions #MENU_CONTEXT get new seeds for all items in the list
  virtual void	 UseSeed(int idx);
  // #MENU #MENU_ON_Actions #MENU_CONTEXT use seed at given index in the list (does OldSeed on it); wraps around (modulus) if idx is > list size (issues warning)

  TA_BASEFUNS(RndSeed_List);
private:
  void	Initialize() 		{ SetBaseType(&TA_RndSeed); }
  void 	Destroy()		{ };
};

class TA_API Random : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math Random Number Generation
INHERITED(taBase)
public:
  enum Type {
    UNIFORM,			// uniform with var = range on either side of the mean
    BINOMIAL,			// binomial with var = p, par = n
    POISSON,			// poisson with lambda = var
    GAMMA,			// gamma with var and par = stages
    GAUSSIAN,			// normal with var
    NONE 			// just the mean
  };

  Type		type;		// type of random variable to generate
  double	mean;		// mean of random distribution
  double	var;		// #CONDEDIT_OFF_type:NONE 'varibility' parameter for the random numbers (gauss = SD, not variance; uniform = half-range)
  double	par;		// #CONDEDIT_ON_type:GAMMA,BINOMIAL extra parameter for distribution (depends on each one)

  double 	Gen() const;
  // generate a random variable according to current parameters
  double	Density(double x) const;
  // get density of random variable according to current params

  ////////////////////////////////////////////////////////////////////////
  // various handy static random number generation functions:

  static int	IntZeroN(int n)
  { if(n > 0) return (int)(MTRnd::genrand_int32() % (uint)n); return 0; }
  // #CAT_Int uniform random integer in the range between 0 and n, exclusive of n: [0,n)
  static int	IntMinMax(int min, int max)
  { return min + IntZeroN(max - min); }
  // #CAT_Int uniform random integer in range between min and max, exclusive of max: [min,max)
  static int	IntMeanRange(int mean, int range)
  { return mean + (IntZeroN(2 * range + 1) - range); }
  // #CAT_Int uniform random integer with given range on either side of the mean: [mean - range, mean + range]

  static double ZeroOne() 		{ return MTRnd::genrand_res53(); }
  // #CAT_Float uniform random number between zero and one (inclusive of 1 due to rounding!)
  static double UniformMinMax(double min, double max)
  { return min + (max - min) * ZeroOne(); }
  // #CAT_Float uniform random number between min and max values (inclusive)
  static double UniformMeanRange(double mean, double range)
  { return mean + range * 2.0 * (ZeroOne() - 0.5); }
  // #CAT_Float uniform random number with given range on either size of the mean: [mean - range, mean + range]

  static double Binom(int n, double p) 	{ return taMath_double::binom_dev(n,p); }
  // #CAT_Float binomial with n trials (par) each of probability p (var)
  static double Poisson(double l)   	{ return taMath_double::poisson_dev(l); }
  // #CAT_Float poisson with parameter l (var)
  static double Gamma(double var, int j)  { return var * taMath_double::gamma_dev(j); }
  // #CAT_Float gamma with given variance, number of exponential stages (par)
  static double Gauss(double var)  	{ return var * taMath_double::gauss_dev(); }
  // #CAT_Float gaussian (normal) random number with given variance

  static double UniformDen(double x, double range)
  { double rval = 0.0; if(fabs(x) <= range) rval = 1.0 / (2.0 * range); return rval; }
  // #CAT_Float uniform density at x with given range on either size of 0 (subtr mean from x before)
  static double BinomDen(int n, int j, double p) { return taMath_double::binom_den(n,j,p); }
  // #CAT_Float binomial density at j with n trials (par) each of probability p (var)
  static double PoissonDen(int j, double l) { return taMath_double::poisson_den(j,l); }
  // #CAT_Float poisson density with parameter l (var)
  static double GammaDen(int j, double l, double t)  { return taMath_double::gamma_den(j,l,t); }
  // #CAT_Float gamma density at time t with given number of stages (par), lambda (var)
  static double GaussDen(double x, double var)  	{ return taMath_double::gauss_den(x / var); }
  // #CAT_Float gaussian (normal) density for given variance (0 mean)

  void	Initialize();
  void	Destroy()		 { };
  void	Copy_(const Random& cp);
  COPY_FUNS(Random, taBase);
  TA_BASEFUNS(Random); //
};

#endif // TA_MATH_H
