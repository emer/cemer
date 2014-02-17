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

#ifndef taMath_float_h
#define taMath_float_h 1

// parent includes:
#include <taMath>

// member includes:

// declare all other types mentioned but not required to include:
class float_Matrix; // 
class int_Matrix; // 
class SimpleMathSpec; // 
class Relation; // 
class RelationFloat; // 
class Aggregate; // 


taTypeDef_Of(taMath_float);

class TA_API taMath_float : public taMath {
  // single-precision (float) math computations
INHERITED(taMath)
public:

  /////////////////////////////////////////////////////////////////////////////////
  // Basic number checking

  static bool   isnan(float x);
  // #CAT_Number returns true if the number is a 'not a number' nan value
  static float  no_nan(float x) 
  { if(isnan(x)) return 0.0; return x; }
  // #CAT_Number returns 0.0 if the number is a nan, otherwise just the number

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

  static float  abs(float x) { return std::fabs(x); }
  // #CAT_Arithmetic absolute value
  static float  fabs(float x) { return std::fabs(x); }
  // #CAT_Arithmetic absolute value
//   static float copysign(float x, float y) { return copysignf(x, y); }
//   // #CAT_Arithmetic copy sign from x to y

//   static float nextafter(float x, float y) { return nextafterf(x, y); }
//   // #CAT_Arithmetic return next largest distinct floating point number after x in direction of y

// The windows macros min and max need to be undefined before code will compile on 
// that platform. This is already done in taMath.h, but somehow doesn't "take" (maybe 
// the first inclusion of taMath.h happens before the macros are defined; maybe Qt is
// responsible for including windef.h, and we don't know where)
// See also: http://stackoverflow.com/questions/5004858/stdmin-gives-error
#ifdef TA_OS_WIN
# ifdef min
#   undef min
# endif
# ifdef max
#   undef max
# endif
#endif //TA_OS_WIN

  static float  min(float x, float y) { return (x < y) ? x : y; }
  // #CAT_Arithmetic minimum of x and y
  static float  max(float x, float y) { return (x > y) ? x : y; }
  // #CAT_Arithmetic maximum of x and y

  static float ceil(float x) { return /*std*/::ceilf(x); }
  // #CAT_Arithmetic ceiling of x: next largest integer value from x
  static float floor(float x) { return /*std*/::floorf(x); }
  // #CAT_Arithmetic floor of x: next smallest integer value from x

  static float round(float x) { return floor(x+0.5f); }
  // #CAT_Arithmetic round value to an integer irrespective of current rounding direction
  static int    rint(float x) { return (int)round(x); }
  // #CAT_Arithmetic round value to an integer using current rounding direction

  static float fmod(float x, float y) { return std::fmod(x, y); }
  // #CAT_Arithmetic floating-point modulus function: remainder of x / y
  static float quantize(float x, float grid) { return floor(x / grid) * grid; }
  // #CAT_Arithmetic quantize the value of x to be an integer multiple of grid size value

  /////////////////////////////////////////////////////////////////////////////////
  // ExpLog: exponential and logarithmic functions

  static float e;
  // #CAT_Trigonometry #READ_ONLY e: the natural exponential number

  static float pow(float x, float p) { return std::pow(x, p); }
  // #CAT_ExpLog x to the power p (x^p)
  static float sqrt(float x) { return std::sqrt(x); }
  // #CAT_ExpLog square root of x (i.e., x^1/2)
  static float exp(float x) { return std::exp(x); }
  // #CAT_ExpLog The natural exponential (e to the power x: e^x)
  static float exp2(float x) { return pow(2.0f, x); }
  // #CAT_ExpLog The base-2 expoenetial (2 to the power x: 2^x)
  static float exp_fast(float x);
  // #CAT_ExpLog a fast approximation to the exponential function from Nicol Schraudolph Neural Computation, 1999
  static float log(float x) { return std::log(x); }
  // #CAT_ExpLog The natural logarithm of x: ln(x)
  static float log10(float x) { return std::log10(x); }
  // #CAT_ExpLog The logarithm of x, base 10
  static float log2(float x) { return log(x) / 0.69314718f; }
  // #CAT_ExpLog The logarithm of x, base 2
  static float logistic(float x, float gain=1.0, float off=0.0)
  { return 1.0 / (1.0 + exp(-gain*(x-off))); }
  // #CAT_ExpLog logistic (sigmoid) function of x: 1/(1 + e^(-gain*(x-off)))
  static float logistic_fast(float x, float gain=1.0, float off=0.0)
  { return 1.0 / (1.0 + exp_fast(-gain*(x-off))); }
  // #CAT_ExpLog logistic (sigmoid) function of x: 1/(1 + e^(-gain*(x-off))) -- using exp_fast function

  /////////////////////////////////////////////////////////////////////////////////
  // Trigonometry

  static float pi;
  // #CAT_Trigonometry #READ_ONLY pi: the ratio of circumference to diameter
  static float two_pi;
  // #CAT_Trigonometry #READ_ONLY 2 pi -- commonly used factor
  static float oneo_two_pi;
  // #CAT_Trigonometry #READ_ONLY 1 / 2 pi -- commonly used factor
  static float deg_per_rad;
  // #CAT_Trigonometry #READ_ONLY degrees per radian (180 / pi)
  static float rad_per_deg;
  // #CAT_Trigonometry #READ_ONLY radians per degree (pi / 180)

  static float  deg_to_rad(float deg)
  { return deg * rad_per_deg; }
  // #CAT_Trigonometry convert degrees to radians
  static float  rad_to_deg(float rad)
  { return rad * deg_per_rad; }
  // #CAT_Trigonometry convert radians to degrees
  static float  norm_to_rad(float norm)
  { return norm * two_pi; }
  // #CAT_Trigonometry normalized angle (1 = 360 = 2pi, .5 = 180 = pi) to radians
  static float  rad_to_norm(float rad)
  { return rad / two_pi; }
  // #CAT_Trigonometry normalized angle (1 = 360 = 2pi, .5 = 180 = pi) from radians

  static float  euc_dist_sq(float x1, float y1, float x2, float y2)
  { return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)); }
  // #CAT_Trigonometry the squared Euclidean distance between two coordinates ((x1-x2)^2 + (y1-y2)^2)
  static float  euc_dist(float x1, float y1, float x2, float y2)
  { return sqrt(euc_dist_sq(x1, y1, x2, y2)); }
  // #CAT_Trigonometry the Euclidean distance between two coordinates ((x1-x2)^2 + (y1-y2)^2)
  static float  hypot_sq(float x, float y)  { return x*x + y*y; }
  // #CAT_Trigonometry the squared length of the hypotenuse (i.e., Euclidean distance): (x^2 + y^2)
  static float  hypot(float x, float y)     { return sqrt(hypot_sq(x,y)); }
  // #CAT_Trigonometry the length of the hypotenuse (i.e., Euclidean distance): sqrt(x^2 + y^2)

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
  static float  cos_deg(float angle) { return std::cos(angle * rad_per_deg); }
  // #CAT_Trigonometry The cosine (x-axis component) of angle (given in degrees)

  static float  sin(float angle) { return std::sin(angle); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in radians)
  static float  sin_deg(float angle) { return std::sin(angle * rad_per_deg); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in degrees)

  static float  tan(float angle) { return std::tan(angle); }
  // #CAT_Trigonometry The tangent (slope y over x) of angle (given in radians)
  static float  tan_deg(float angle) { return std::tan(angle * rad_per_deg); }
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
  // Probability distributions and related functions

  static float fact_ln(int n);
  // #CAT_Probability natural log (ln) of n factorial (n!)
  static float fact(int n);
  // #CAT_Probability factorial of n
  static float bico_ln(int n, int j);
  // #CAT_Probability natural log (ln) of n choose j (binomial)
  static float hyperg(int j, int s, int t, int n);
  // #CAT_Probability hypergeometric (j t's of sample s in n)
  static float gamma_ln(float z);
  // #CAT_Probability natural log (ln) of gamma function (not gamma distribution): generalization of (n-1)! to real values
//   static float  lgamma(float z) { return std::lgamma(z); }
//   // #CAT_Probability natural log (ln) of gamma function (not gamma distribution): generalization of (n-1)! to real values
//   static float tgamma(float z) { return std::tgamma(z); }
//   // #CAT_Probability true gamma function (not gamma distribution): generalization of (n-1)! to real values

  static float gamma_p(float a, float x);
  // #CAT_Probability incomplete gamma function
  static float gamma_q(float a, float x);
  // #CAT_Probability incomplete gamma function (complement of p)
  static float beta(float z, float w);
  // #CAT_Probability beta function
  static float beta_i(float a, float b, float x);
  // #CAT_Probability incomplete beta function

  static float binom_den(int n, int j, float p);
  // #CAT_Probability binomial probability function
  static float binom_cum(int n, int j, float p);
  // #CAT_Probability cumulative binomial probability
  static float binom_dev(int n, float p);
  // #CAT_Probability binomial deviate: p prob with n trials

  static float poisson_den(int j, float l);
  // #CAT_Probability poisson distribution
  static float poisson_cum(int j, float l);
  // #CAT_Probability cumulative Poisson P_l(<j) (0 thru j-1)
  static float poisson_dev(float l);
  // #CAT_Probability poisson deviate:  mean is l

  static float gamma_den(int j, float l, float t);
  // #CAT_Probability gamma probability distribution: j events, l lambda, t time
  static float gamma_cum(int j, float l, float t);
  // #CAT_Probability gamma cumulative: j events, l lambda, t time
  static float gamma_dev(int j);
  // #CAT_Probability gamma deviate: how long to wait until j events with  unit lambda

  static float gauss_den(float x);
  // #CAT_Probability gaussian (normal) distribution with uniform standard deviation: 1 / sqrt(2 * PI) * exp(-x^2 / 2)
  static float gauss_den_sig(float x, float sigma);
  // #CAT_Probability gaussian (normal) distribution with explicit sigma: 1 / (sigma * sqrt(2 * PI)) * exp(-x^2 / (2 * sigma^2))
  static float gauss_den_sq_sig(float x_sq, float sigma);
  // #CAT_Probability gaussian (normal) distribution with x already squared and explicit sigma: 1 / (sigma * sqrt(2 * PI)) * exp(-x_sq / (2 * sigma^2))
  static float gauss_den_nonorm(float x, float sigma);
  // #CAT_Probability non-normalized gaussian (normal) distribution with uniform standard deviation: exp(-x^2 / (2 * sigma^2))
  static float gauss_cum(float z);
  // #CAT_Probability cumulative gaussian (unit variance) to z
  static float gauss_inv(float p);
  // #CAT_Probability inverse of the cumulative for p: z value for given p
  static float gauss_inv_lim(float p);
  // #CAT_Probability inverse of the cumulative for p: z value for given p , returns nonzero value for p==0 or p==1
  static float gauss_dev();
  // #CAT_Probability gaussian deviate: normally distributed
  static float erf(float x);
  // #CAT_Probability the error function: used for computing the normal distribution
  static float erfc(float x);
  // #CAT_Probability complement of the error function (1.0 - erf(x)) */

  static float dprime(float mean_signal, float stdev_signal,
		       float mean_noise, float stdev_noise);
  // #CAT_Probability d' signal detection sensitivity measure (actually d_a for unequal variances) -- = (mean_signal - mean_noise) / sqrt(0.5 * (stdev_signal^2 + stdev_noise^2)) -- difference in means divided by the average standard deviations
  static float hits_fa(float dprime, float& hits, float& false_alarms, 
		       float crit_z=0.5);
  // #CAT_Probability returns hits and false alarms (reference args), and overall percent correct (return value: hits + correct rejections / 2) associated with a given dprime level and z-normalized criterion (0.5 = half way between the means, 0 = right at the noise mean, 1 = right at the signal mean) -- results are just the area under the normal curve relative to the criterion

  static float chisq_p(float X, float v);
  // #CAT_Probability P(X^2 | v)
  static float chisq_q(float X, float v);
  // #CAT_Probability Q(X^2 | v) (complement)
  static float students_cum(float t, float df);
  // #CAT_Probability area between -t and t of cumulative student's distribution df deg of free t test
  static float students_cum_cum(float t, float df);
 // #CAT_Probability cumulative student's distribution df deg of free t test
  static float students_den(float t, float df);
  // #CAT_Probability density fctn of student's distribution df deg of free t test
  static float Ftest_q(float F, float v1, float v2);
  // #CAT_Probability F distribution probability F | (v1 < v2)
  static float d_sub_a(float_Matrix* vec_signal, float_Matrix* vec_noise);
  // #CAT_Probability Computes d_sub_a, which reduces to d' in the equal variance case. See: http://psych.colorado.edu/~lharvey/P4165/P4165_2004_Fall/2004_Fall_pdf/P4165_SDT.pdf

  /////////////////////////////////////////////////////////////////////////////////
  // Vector operations (operate on Matrix objects, treating as a single linear guy)

  ///////////////////////////////////////
  // arithmetic ops

  static void   vec_fm_ints(float_Matrix* float_mat, const int_Matrix* int_mat);
  // initialize given float matrix (which is completely overwritten) from integer matrix data
  static void   vec_to_ints(int_Matrix* int_mat, const float_Matrix* float_mat);
  // copy float matrix data back to integer matrix

  static bool   vec_check_type(const float_Matrix* a);
  // check that matrix is actually a float type -- issues Error if not and returns false
  static bool   vec_check_same_size(const float_Matrix* a, const float_Matrix* b,
                                    bool quiet = false, bool flex = true);
  // check that both vectors are the same size, and issue warning if not (unless quiet) -- if flex then use flexible test (assumes use of iterators for going through operators)

  static bool  vec_add(float_Matrix* a, const float_Matrix* b);
  // #CAT_Arithmetic add elements in two vectors: a(i) += b(i) -- uses entire matrix, ignoring any view of sub-elements
  static bool  vec_sub(float_Matrix* a, const float_Matrix* b);
  // #CAT_Arithmetic subtract elements in two vectors: a(i) -= b(i) -- uses entire matrix, ignoring any view of sub-elements
  static bool  vec_mult_els(float_Matrix* a, const float_Matrix* b);
  // #CAT_Arithmetic multiply elements in two vectors: a(i) *= b(i) -- uses entire matrix, ignoring any view of sub-elements
  static bool  vec_div_els(float_Matrix* a, const float_Matrix* b);
  // #CAT_Arithmetic divide elements in two vectors: a(i) /= b(i) -- uses entire matrix, ignoring any view of sub-elements

  static bool  vec_add_scalar(float_Matrix* a, float b);
  // #CAT_Arithmetic add scalar value b to elements in vector a: a(i) += b
  static bool  vec_sub_scalar(float_Matrix* a, float b);
  // #CAT_Arithmetic subtract scalar value b to elements in vector a: a(i) -= b
  static bool  vec_mult_scalar(float_Matrix* a, float b);
  // #CAT_Arithmetic multiply scalar value b to elements in vector a: a(i) *= b
  static bool  vec_div_scalar(float_Matrix* a, float b);
  // #CAT_Arithmetic divide scalar value b to elements in vector a: a(i) /= b
  static bool   vec_quantize(float_Matrix* vec, float grid);
  // #CAT_Arithmetic quantize the values of vector vec to be an integer multiple of grid size value

  static bool   vec_simple_math(float_Matrix* vec, const SimpleMathSpec& math_spec);
  // #CAT_Arithmetic apply standard kinds of simple math operators to values in the vector

  static bool   vec_simple_math_arg(float_Matrix* vec, const float_Matrix* arg_vec,
                                    const SimpleMathSpec& math_spec);
  // #CAT_Arithmetic apply simple math operators to values in vector, other vector provides 'arg' value for math_spec

  static bool vec_students_cum(float_Matrix* t,const float_Matrix* df);
  // #CAT_Probability element wise area between -t and t of student's distribution df deg of free t test
  static bool vec_students_cum_cum( float_Matrix* t,const float_Matrix* df);
  // #CAT_Probability element wise cumulative student's distribution (a is tval, b is df)
  static bool vec_gauss_inv(float_Matrix* p);
  // #CAT_Probability element-wise inverse of the cumulative for p: z value for given p 
  static bool vec_gauss_inv_lim(float_Matrix* p);
  // #CAT_Probability element-wise inverse of the cumulative for p: z value for given p , returns nonzero values for  p==0 or p==1

  ///////////////////////////////////////
  // basic statistics

  static float  vec_first(const float_Matrix* vec);
  // #CAT_Statistics first item in the vector
  static float  vec_last(const float_Matrix* vec);
  // #CAT_Statistics last item in the vector
  static int    vec_find_first(const float_Matrix* vec, Relation& rel);
  // #CAT_Statistics find first element in the vector that meets relationship rel -- returns index in vector or -1 if not found
  static int    vec_find_last(const float_Matrix* vec, Relation& rel);
  // #CAT_Statistics find first element in the vector that meets relationship rel -- returns index in vector or -1 if not found 
  static float  vec_max(const float_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the maximum value
  static float  vec_abs_max(const float_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the maximum absolute value
  static float  vec_next_max(const float_Matrix* vec, int max_idx, int& idx);
  // #CAT_Statistics value and index of the element that has the next-largest value, excluding the max item which is at max_idx -- uses entire matrix, ignoring any view of sub-elements
  static float  vec_min(const float_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the minimum value
  static float  vec_abs_min(const float_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the minimum value
  static float  vec_next_min(const float_Matrix* vec, int min_idx, int& idx);
  // #CAT_Statistics value and index of the element that has the next-smallest value, excluding the min item which is at min_idx -- uses entire matrix, ignoring any view of sub-elements

  static float  vec_sum(const float_Matrix* vec);
  // #CAT_Statistics compute the sum of the values in the vector
  static float  vec_sum_range(const float_Matrix* vec, int start=0, int end=-1);
  // #CAT_Statistics compute the sum of the values in the vector from el=startpos to el=endpos-1 -- uses entire matrix, ignoring any view of sub-elements
  static float  vec_prod(const float_Matrix* vec);
  // #CAT_Statistics compute the product of the values in the vector
  static float  vec_mean(const float_Matrix* vec);
  // #CAT_Statistics compute the mean of the values in the vector
  static float  vec_var(const float_Matrix* vec, float mean=0, bool use_mean=false, bool use_est=false);
  // #CAT_Statistics compute the variance of the values, opt with given mean; if use_est == true, then divides by N-1 rather than N
  static float  vec_std_dev(const float_Matrix* vec, float mean=0, bool use_mean=false, bool use_est=false);
  // #CAT_Statistics compute the standard deviation of the values, opt with given mean; if use_est == true, then divides by N-1 rather than N
  static float  vec_sem(const float_Matrix* vec, float mean=0, bool use_mean=false);
  // #CAT_Statistics compute the standard error of the mean of the values, opt with given mean
  static float  vec_ss_len(const float_Matrix* vec);
  // #CAT_Statistics sum-of-squares length of the vector
  static float  vec_norm(const float_Matrix* vec);
  // #CAT_Statistics square root of the sum-of-squares length of the vector -- its norm or euclidean length
  static float  vec_ss_mean(const float_Matrix* vec);
  // #CAT_Statistics sum-of-squares around the mean of the vector
  static void   vec_histogram(float_Matrix* hist_vec, const float_Matrix* src_vec,
                              float bin_size, float min_val = 0.0, float max_val = 0.0);
  // #CAT_Statistics gets a histogram (counts) of number of values within each bin size in source vector -- min and maximum ranges to compute within are also optional args -- only used min != max
  static float  vec_count(const float_Matrix* vec, Relation& rel);
  // #CAT_Statistics count number of times relationship is true
  static float  vec_percent(const float_Matrix* vec, Relation& rel);
  // #CAT_Statistics compute percent (proportion) of times relationship is true
  static float  vec_count_float(const float_Matrix* vec, RelationFloat& rel);
  // #CAT_Statistics count number of times relationship is true -- optimized float version
  static float  vec_median(const float_Matrix* vec);
  // #CAT_Statistics compute the median of the values in the vector (middle value) -- requires sorting
  static float  vec_mode(const float_Matrix* vec);
  // #CAT_Statistics compute the mode (most frequent) of the values in the vector -- requires sorting
  static float  vec_quantile(const float_Matrix* vec, float quant_pos);
  // #CAT_Statistics compute arbitrary quantile according to quant_pos value, which is a proportion 0-1 from start to end of sorted list of values, e.g., .5 = median, .25 = first quartile, etc
  static float  vec_kwta(float_Matrix* vec, int k, bool descending = true);
  // #CAT_Statistics perform an optimized k-winners-take-all sort, returning the value of the item that is k from the highest (lowest if !descending) on the list -- this can be much faster than vec_quantile, which does a full sort -- uses entire matrix, ignoring any view of sub-elements
  static void   vec_kwta_avg(float& top_k_avg, float& bot_k_avg,
                             float_Matrix* vec, int k, bool descending = true);
  // #CAT_Statistics perform an optimized k-winners-take-all sort, returning the average of the top k values, and average of the bottom k (reversed if !descending) on the list -- uses entire matrix, ignoring any view of sub-elements

  static String vec_stats(const float_Matrix* vec);
  // #CAT_Statistics compute standard descriptive statistics on given vector data, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).

  static int   vec_prob_choose(float_Matrix* vec);
  // #CAT_Statistics given a vector of probability values, choose an index according to its corresponding probability

  static void   vec_sort(float_Matrix* vec, bool descending = false);
  // #CAT_Statistics sort the given vector values in numerical order (in place)

  static float  vec_dprime(const float_Matrix* signal_vec,
			    const float_Matrix* noise_vec);
  // #CAT_Statistics computes the d-prime (d') statistic from signal and noise vectors -- just computes the mean and standard deviation of each vector and then computes d' from there using standard formula (see plain dprime function)

  static bool   vec_regress_lin(const float_Matrix* x_vec, const float_Matrix* y_vec,
                                float& b, float& m, float& cov00, float& cov01,
                                float& cov11, float& sum_sq);
  // #CAT_Statistics computes the best-fit linear regression coefficients (b,m) of the model y = b + mx for the dataset (x, y). The variance-covariance matrix for the parameters (c0, c1) is estimated from the scatter of the points around the best-fit line and returned via the parameters (cov00, cov01, cov11). The sum of squares of the residuals from the best-fit line is returned in sum_sq.  See vec_correl to compute the correlation coefficient. -- uses entire matrix, ignoring any view of sub-elements

  static bool vec_jitter_gauss(float_Matrix* vec, float stdev);
  // #CAT_Statistics jitters all the non-zero elements of vec by a gaussian with stdev. jittered indices below zero or above the length of the vector are rejittered until they fall inside. there must be at least one zero element. method is clobber safe - the number of elements after jittering is guaranteed to be the same as the number of elements before jittering. see also: http://en.wikipedia.org/wiki/Jitter#Random_jitter -- uses entire matrix, ignoring any view of sub-elements

  ///////////////////////////////////////
  // distance metrics (comparing two vectors)

  static float  vec_ss_dist(const float_Matrix* vec, const float_Matrix* oth_vec,
                            bool norm = false, float tolerance=0.0f);
  // #CAT_Distance compute sum-squares dist between this and the oth, tolerance is by element
  static float  vec_euclid_dist(const float_Matrix* vec, const float_Matrix* oth_vec,
                                bool norm = false, float tolerance=0.0f);
  // #CAT_Distance compute Euclidian dist between this and the oth, tolerance is by element
  static float  vec_hamming_dist(const float_Matrix* vec, const float_Matrix* oth_vec,
                                 bool norm = false, float tolerance=0.0f);
  // #CAT_Distance compute Hamming dist between this and the oth, tolerance is by element
  static float  vec_covar(const float_Matrix* vec, const float_Matrix* oth_vec);
  // #CAT_Distance compute the covariance of this vector the oth vector
  static float  vec_correl(const float_Matrix* vec, const float_Matrix* oth_vec);
  // #CAT_Distance compute the correlation of this vector with the oth vector
  static float  vec_inner_prod(const float_Matrix* vec, const float_Matrix* oth_vec,
                               bool norm = false);
  // #CAT_Distance compute the inner product of this vector and the oth vector
  static float  vec_cross_entropy(const float_Matrix* vec, const float_Matrix* oth_vec);
  // #CAT_Distance compute cross entropy between this and other vector, this is 'p' other is 'q'
  static float  vec_dist(const float_Matrix* vec, const float_Matrix* oth_vec,
                         DistMetric metric, bool norm = false, float tolerance=0.0f);
  // #CAT_Distance compute generalized distance metric with other vector (float_Matrix* vec, calls appropriate fun above)
  static float  scalar_dist(float v1, float v2,
                            DistMetric metric, float tolerance=0.0);
  // #CAT_Distance compute distance metric on scalar values -- not really sensible for COVAR or CORREL (which reduce to SUM_SQUARES and EUCLIDIAN, respectively)

  ///////////////////////////////////////
  // Normalization

  static float  vec_norm_len(float_Matrix* vec, float len=1.0f);
  // #CAT_Norm normalize vector to total given length (1.0), returns scaling factor
  static float  vec_norm_sum(float_Matrix* vec, float sum=1.0f, float min_val=0.0f);
  // #CAT_Norm normalize vector to total given sum (1.0) and min_val (0), returns scaling factor
  static float  vec_norm_max(float_Matrix* vec, float max=1.0f);
  // #CAT_Norm normalize vector to given maximum value, returns scaling factor
  static float  vec_norm_abs_max(float_Matrix* vec, float max=1.0f);
  // #CAT_Norm normalize vector to given absolute maximum value, returns scaling factor
  static int    vec_threshold(float_Matrix* vec, float thresh=.5f,
                              float low=0.0f, float high=1.0f);
  // #CAT_Norm threshold values in the vector, low vals go to low, etc; returns number of high values
  static int    vec_threshold_low(float_Matrix* vec, float thresh=.5,
                                  float low=0.0);
  // #CAT_Norm threshold values in the vector, low vals go to low; returns number of low vals
  static int    vec_threshold_high(float_Matrix* vec, float thresh=.5,
                                   float high=0.0);
  // #CAT_Norm threshold values in the vector, high vals go to high; returns number of high values
  static int    vec_replace(float_Matrix* vec, float find1, float repl1,
                            bool do2 = false, float find2=0.0, float repl2=0.0,
                            bool do3 = false, float find3=0.0, float repl3=0.0,
                            bool do4 = false, float find4=0.0, float repl4=0.0,
                            bool do5 = false, float find5=0.0, float repl5=0.0,
                            bool do6 = false, float find6=0.0, float repl6=0.0);
  // #CAT_Norm find and replace values -- replace find val with repl replacement val -- do flags activate usage of multiple replacements -- more efficient to do in parallel -- returns number of replacments made
  static float  vec_aggregate(const float_Matrix* vec, Aggregate& agg);
  // #CAT_Aggregate compute aggregate of values in this vector using aggregation params of agg

  ///////////////////////////////////////
  // Convolution

  static bool   vec_kern_uniform(float_Matrix* kernel, int half_sz,
                                 bool neg_tail=true, bool pos_tail=true);
  // #CAT_Convolution create a unit-sum-normalized uniform kernel of given half-size (size set to 2* half_sz + 1) in given vector.  if given tail direction is false, it is replaced with zeros -- uses entire matrix, ignoring any view of sub-elements
  static bool   vec_kern_gauss(float_Matrix* kernel, int half_sz, float sigma,
                               bool neg_tail=true, bool pos_tail=true);
  // #CAT_Convolution create a unit-sum-normalized gaussian kernel of given half-size (size set to 2* half_sz + 1) in given vector, with given sigma (standard deviation) value.  if given tail direction is false, it is replaced with zeros -- uses entire matrix, ignoring any view of sub-elements
  static bool   vec_kern_exp(float_Matrix* kernel, int half_sz, float exp_mult,
                             bool neg_tail=true, bool pos_tail=true);
  // #CAT_Convolution create a unit-sum-normalized symmetric exponential kernel of given half-size (size set to 2* half_sz + 1) in given vector, with given exponential multiplier.  symmetric means abs(i-ctr) is used for value.  if given tail direction is false, it is replaced with zeros -- uses entire matrix, ignoring any view of sub-elements
  static bool   vec_kern_pow(float_Matrix* kernel, int half_sz, float pow_exp,
                             bool neg_tail=true, bool pos_tail=true);
  // #CAT_Convolution create a unit-sum-normalized power-function kernel of given half-size (size set to 2* half_sz + 1) in given vector, with given exponential multiplier.  if given tail direction is false, it is replaced with zeros -- uses entire matrix, ignoring any view of sub-elements
  static bool   vec_convolve(float_Matrix* out_vec, const float_Matrix* in_vec,
                             const float_Matrix* kernel, bool keep_edges = false);
  // #CAT_Convolution convolve in_vec with kernel to produce out_vec.  out_vec_i = sum_j kernel_j * in_vec_[i+j-off] (where off is 1/2 width of kernel).  normally, out_vec is indented by the offset and width of the kernel so that the full kernel is used for all out_vec points.  however, if keep_edges is true, it keeps these edges by clipping and renormalizing the kernel all the way to both edges -- uses entire matrix, ignoring any view of sub-elements

  static bool   vec_kern2d_gauss(float_Matrix* kernel, int half_sz_x, int half_sz_y,
                                 float sigma_x, float sigma_y);
  // #CAT_Convolution create a unit-sum-normalized 2D gaussian kernel of given half-size in each axis (size set to 2* half_sz + 1) in given vector, with given sigma (standard deviation) value in each axis -- uses entire matrix, ignoring any view of sub-elements

  /////////////////////////////////////////////////////////////////////////////////
  // Matrix operations

#if (defined(HAVE_LIBGSL) && !defined(__MAKETA__))
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

  static bool mat_transpose(float_Matrix* dest, const float_Matrix* src);
  // #CAT_Matrix transpose a 2d matrix -- swap rows and columns

  // note: the following all involve copying to/from double -- underlying computation is done in the double routines, because that is what gsl supports!
  static bool mat_mult(float_Matrix* c, const float_Matrix* a, const float_Matrix* b);
  // #CAT_Matrix #NO_CSS_MATH matrix multiplication (not element-wise -- see mat_mult_els) -- c = a * b -- number of columns of a must be same as number of rows of b, and vice-versa -- does NOT support complex_Matrix type
  static float mat_det(const float_Matrix* a);
  // #CAT_Matrix #LABEL_det return the determinant of the given square matrix

  static bool mat_vec_product(const float_Matrix* A, const float_Matrix* x,
                              float_Matrix* y);
  // #CAT_Statistics computes the matrix-vector product y=Ax -- uses entire matrix, ignoring any view of sub-elements


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
  static bool mat_slice(float_Matrix* dest, float_Matrix* src, int d0_start = 0, int d0_end = -1, int d1_start = 0, int d1_end = -1);
  // #CAT_Matrix See http://en.wikipedia.org/wiki/Array_slicing. Copies a 2d slice out of the first 2 dimensions of src into dest. If d0_end or d1_end are -1 (default) they will be set to the size of that dimension. See also taDataProc::Slice2D.
  static bool mat_trim(float_Matrix* dest, float_Matrix* src, RelationFloat& thresh, int intol_within = 0, int intol_between = 0,
                       bool left = true, bool right = true, bool top = true, bool bottom = true);
  // #CAT_Matrix Trim all consecutive rows/columns starting from top, bottom, left and/or right that satisfy thresh. The intolerance params control how greedy the threshold operation is. intol_within controls how many values in this row/col that don't satisfy thresh can be found and the row/col will still be trimmed. intol_between controls how many consecutive rows/cols that don't satisfy thresh can be found, followed by a row/col that DOES satisfy thresh, that will still be trimmed. Returns false if the entire matrix matches so be sure to check the return value.

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

  /////////////////////////////////////////////////////////////////////////////////
  // frame-compatible versions of various functions

  static bool mat_time_avg(float_Matrix* a, float avg_dt);
  // #CAT_HighDimMatrix replace matrix values with their running (exponentially-weighted) time-averaged values (new_val = avg_dt * running_avg + (1-avg_dt) * old_val
  static bool mat_frame_convolve(float_Matrix* out_mat, const float_Matrix* in_mat,
                                 const float_Matrix* kernel);
  // #CAT_Convolution convolve in_mat with kernel to produce out_mat, in a cell-by-cell manner across frames.  always keeps the edges by clipping and renormalizing the kernel all the way to both edges

  /////////////////////  Statistics

  static bool   mat_fmt_out_frame(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix format out_mat as one frame (inner dimensions) of in_mat (i.e., out_mat has n-1 dimensions relative to in_mat)

  static bool   mat_frame_set_n(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix set out_mat to the number of frames for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_first(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute first item for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_last(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute last item for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_find_first(float_Matrix* out_mat, const float_Matrix* in_mat, Relation& rel);
  // #CAT_HighDimMatrix compute index of first item that meets relationship rel for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_find_last(float_Matrix* out_mat, const float_Matrix* in_mat, Relation& rel);
  // #CAT_HighDimMatrix compute index of last item that meets relationship rel for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_max(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute maximum for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_min(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute minimum for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_abs_max(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute maximum of absolute values for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_abs_min(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute minimum of absolute values for each value within inner frame dimensions across matrix frames (last outer dimension)

  static bool   mat_frame_sum(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute sum for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_prod(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute product for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_mean(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute mean for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_var(float_Matrix* out_mat, const float_Matrix* in_mat, bool use_est=false);
  // #CAT_HighDimMatrix compute variance for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_std_dev(float_Matrix* out_mat, const float_Matrix* in_mat, bool use_est=false);
  // #CAT_HighDimMatrix compute standard deviation for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_sem(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute standard error of the mean for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_ss_len(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute sum-of-squares length for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_ss_mean(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute sum of squares around the mean for each value within inner frame dimensions across matrix frames (last outer dimension)

  static bool   mat_frame_count(float_Matrix* out_mat, const float_Matrix* in_mat, Relation& rel);
  // #CAT_HighDimMatrix compute count number of times relationship rel is true for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_percent(float_Matrix* out_mat, const float_Matrix* in_mat, Relation& rel);
  // #CAT_HighDimMatrix compute percent (proportion) of times relationship rel is true for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_median(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute median (middle value -- requires sorting) for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_mode(float_Matrix* out_mat, const float_Matrix* in_mat);
  // #CAT_HighDimMatrix compute mode (most frequent) value (requires sorting) for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_quantile(float_Matrix* out_mat, const float_Matrix* in_mat, float quant_pos);
  // #CAT_HighDimMatrix compute arbitrary quantile according to quant_pos value, which is a proportion 0-1 from start to end of sorted list of values, e.g., .5 = median, .25 = first quartile, etc, for each value within inner frame dimensions across matrix frames (last outer dimension)

  static bool   mat_frame_aggregate(float_Matrix* out_mat, const float_Matrix* in_mat,
                                    Aggregate& agg);
  // #CAT_Aggregate compute aggregate across matrix frames (last outer dimension) for each value within inner frame dimensions, using aggregation params of agg

  /////////////////////////////////////////////////////////////////////////////////
  // fft routines: note fft NOT supported for float type -- complex only avail for double

  TA_ABSTRACT_BASEFUNS_NOCOPY(taMath_float) //
private:
  void Initialize() { };
  void Destroy() { };
};

#endif // taMath_float_h
