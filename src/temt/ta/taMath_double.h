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

#ifndef taMath_double_h
#define taMath_double_h 1

// parent includes:
#include <taMath>

// member includes:

// declare all other types mentioned but not required to include:
class double_Matrix; // 
class int_Matrix; // 
class SimpleMathSpec; // 
class Relation; // 
class taMatrix; // 
class Aggregate; // 
class complex_Matrix; // 

taTypeDef_Of(taMath_double);

class TA_API taMath_double : public taMath {
  // double precision math computations
INHERITED(taMath)
public:

  /////////////////////////////////////////////////////////////////////////////////
  // Arithmetic: basic arithmetic

  static double  add(double x, double y) { return x + y; }
  // #CAT_Arithmetic #NO_CSS_MATH add
  static double  sub(double x, double y) { return x - y; }
  // #CAT_Arithmetic #NO_CSS_MATH subtract
  static double  mult(double x, double y) { return x * y; }
  // #CAT_Arithmetic #NO_CSS_MATH multiply
  static double  div(double x, double y) { return x / y; }
  // #CAT_Arithmetic #NO_CSS_MATH divide

  static double  abs(double x) { return std::fabs(x); }
  // #CAT_Arithmetic #NO_CSS_MATH absolute value
  static double  fabs(double x) { return std::fabs(x); }
  // #CAT_Arithmetic absolute value
//   static double copysign(double x, double y) { return std::copysign(x, y); }
//   // #CAT_Arithmetic copy sign from x to y

//   static double nextafter(double x, double y) { return std::nextafter(x, y); }
//   // #CAT_Arithmetic return next largest distinct floating point number after x in direction of y

  static double  min(double x, double y) { return (x < y) ? x : y; }
  // #CAT_Arithmetic #NO_CSS_MATH minimum of x and y
  static double  max(double x, double y) { return (x > y) ? x : y; }
  // #CAT_Arithmetic #NO_CSS_MATH maximum of x and y

  static double ceil(double x) { return std::ceil(x); }
  // #CAT_Arithmetic ceiling of x: next largest integer value from x
  static double floor(double x) { return std::floor(x); }
  // #CAT_Arithmetic floor of x: next smallest integer value from x
  static double round(double x) { return floor(x+0.5); }
  // #CAT_Arithmetic round value to an integer irrespective of current rounding direction
  static int    rint(double x) { return (int)round(x); }
  // #CAT_Arithmetic round value to an integer using current rounding direction

  static double fmod(double x, double y) { return std::fmod(x, y); }
  // #CAT_Arithmetic #CSS_MATRIX_ARG_1 floating-point modulus function: remainder of x / y
  static double quantize(double x, double grid) { return floor(x / grid) * grid; }
  // #CAT_Arithmetic #CSS_MATRIX_ARG_1 quantize the value of x to be an integer multiple of grid size value

  /////////////////////////////////////////////////////////////////////////////////
  // ExpLog: exponential and logarithmic functions

  static double e;
  // #CAT_Trigonometry #READ_ONLY e: the natural exponential number

  static double pow(double x, double p) { return std::pow(x, p); }
  // #CAT_ExpLog #CSS_MATRIX_ARG_1 x to the power p (x^p)
  static double sqrt(double x) { return std::sqrt(x); }
  // #CAT_ExpLog square root of x (i.e., x^1/2)
  static double exp(double x) { return std::exp(x); }
  // #CAT_ExpLog The natural exponential (e to the power x: e^x)
  static double exp2(double x) { return pow(2.0, x); }
  // #CAT_ExpLog The base-2 expoenetial (2 to the power x: 2^x)
  static double exp_fast(double x);
  // #CAT_ExpLog a fast approximation to the exponential function from Nicol Schraudolph Neural Computation, 1999
  static double log(double x) { return std::log(x); }
  // #CAT_ExpLog The natural logarithm of x: ln(x)
  static double log10(double x) { return std::log10(x); }
  // #CAT_ExpLog The logarithm of x, base 10
  static double log2(double x) { return log(x) / 0.69314718; }
  // #CAT_ExpLog The logarithm of x, base 2
  static double logistic(double x, double gain=1.0, double off=0.0)
  { return 1.0 / (1.0 + exp(-gain*(x-off))); }
  // #CAT_ExpLog #CSS_MATRIX_ARG_1 logistic (sigmoid) function of x: 1/(1 + e^(-gain*(x-off)))
  static double logistic_fast(double x, double gain=1.0, double off=0.0)
  { return 1.0 / (1.0 + exp_fast(-gain*(x-off))); }
  // #CAT_ExpLog #CSS_MATRIX_ARG_1 logistic (sigmoid) function of x: 1/(1 + e^(-gain*(x-off))) -- using exp_fast function

  /////////////////////////////////////////////////////////////////////////////////
  // Trigonometry

  static double pi;
  // #CAT_Trigonometry #READ_ONLY pi: the ratio of circumference to diameter
  static double two_pi;
  // #CAT_Trigonometry #READ_ONLY 2 pi -- commonly used factor
  static double oneo_two_pi;
  // #CAT_Trigonometry #READ_ONLY 1 / 2 pi -- commonly used factor
  static double deg_per_rad;
  // #CAT_Trigonometry #READ_ONLY degrees per radian (180 / pi)
  static double rad_per_deg;
  // #CAT_Trigonometry #READ_ONLY radians per degree (pi / 180)

  static double  deg_to_rad(double deg)
  { return deg * rad_per_deg; }
  // #CAT_Trigonometry convert degrees to radians
  static double  rad_to_deg(double rad)
  { return rad * deg_per_rad; }
  // #CAT_Trigonometry convert radians to degrees
  static double  norm_to_rad(double norm)
  { return norm * two_pi; }
  // #CAT_Trigonometry normalized angle (1 = 360 = 2pi, .5 = 180 = pi) to radians
  static double  rad_to_norm(double rad)
  { return rad / two_pi; }
  // #CAT_Trigonometry normalized angle (1 = 360 = 2pi, .5 = 180 = pi) from radians

  static double  euc_dist_sq(double x1, double y1, double x2, double y2)
  { return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)); }
  // #CAT_Trigonometry the squared Euclidean distance between two coordinates ((x1-x2)^2 + (y1-y2)^2)
  static double  euc_dist(double x1, double y1, double x2, double y2)
  { return sqrt(euc_dist_sq(x1, y1, x2, y2)); }
  // #CAT_Trigonometry the Euclidean distance between two coordinates ((x1-x2)^2 + (y1-y2)^2)
  static double  hypot_sq(double x, double y)  { return x*x + y*y; }
  // #CAT_Trigonometry the squared length of the hypotenuse (i.e., Euclidean distance): (x^2 + y^2)
  static double  hypot(double x, double y)     { return sqrt(hypot_sq(x,y)); }
  // #CAT_Trigonometry the length of the hypotenuse (i.e., Euclidean distance): sqrt(x^2 + y^2)
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
  static double  cos_deg(double angle) { return std::cos(angle * rad_per_deg); }
  // #CAT_Trigonometry The cosine (x-axis component) of angle (given in degrees)

  static double  sin(double angle) { return std::sin(angle); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in radians)
  static double  sin_deg(double angle) { return std::sin(angle * rad_per_deg); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in degrees)

  static double  tan(double angle) { return std::tan(angle); }
  // #CAT_Trigonometry The tangent (slope y over x) of angle (given in radians)
  static double  tan_deg(double angle) { return std::tan(angle * rad_per_deg); }
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
  static double fact(int n);
  // #CAT_Probability factorial of n
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
  // #CAT_Probability gaussian (normal) distribution with uniform standard deviation: 1 / sqrt(2 * PI) * exp(-x^2 / 2)
  static double gauss_den_sig(double x, double sigma);
  // #CAT_Probability gaussian (normal) distribution with explicit sigma: 1 / (sigma * sqrt(2 * PI)) * exp(-x^2 / (2 * sigma^2))
  static double gauss_den_sq_sig(double x_sq, double sigma);
  // #CAT_Probability gaussian (normal) distribution with x already squared and explicit sigma: 1 / (sigma * sqrt(2 * PI)) * exp(-x_sq / (2 * sigma^2))
  static double gauss_den_nonorm(double x, double sigma);
  // #CAT_Probability non-normalized gaussian (normal) distribution with uniform standard deviation: exp(-x^2 / (2 * sigma^2))
  static double gauss_cum(double z);
  // #CAT_Probability cumulative gaussian (unit variance) to z
  static double gauss_inv(double p);
  // #CAT_Probability inverse of the cumulative for p: z value for given p
  static double gauss_inv_lim(double p);
  // #CAT_Probability inverse of the cumulative for p: z value for given p , returns a non-zero value for p==0 or p==1
  static double gauss_dev();
  // #CAT_Probability gaussian deviate: normally distributed
  static double erf(double x);
  // #CAT_Probability the error function: used for computing the normal distribution
  static double erfc(double x);
  // #CAT_Probability complement of the error function (1.0 - erf(x)) */

  static double dprime(double mean_signal, double stdev_signal,
		       double mean_noise, double stdev_noise);
  // #CAT_Probability d' signal detection sensitivity measure (actually d_a for unequal variances) -- = (mean_signal - mean_noise) / sqrt(0.5 * (stdev_signal^2 + stdev_noise^2)) -- difference in means divided by the average standard deviations
  static double hits_fa(double dprime, double& hits, double& false_alarms, 
		       double crit_z=0.5);
  // #CAT_Probability returns hits and false alarms (reference args), and overall percent correct (return value: hits + correct rejections / 2) associated with a given dprime level and z-normalized criterion (0.5 = half way between the means, 0 = right at the noise mean, 1 = right at the signal mean) -- results are just the area under the normal curve relative to the criterion

  static double chisq_p(double X, double v);
  // #CAT_Probability P(X^2 | v)
  static double chisq_q(double X, double v);
  // #CAT_Probability Q(X^2 | v) (complement)
  static double students_cum(double t, double df);
  // #CAT_Probability area between -t and t of the student's distribution df deg of free t test
  static double students_cum_cum(double t, double df);
  // #CAT_Probability cumulative student's distribution df deg of free t test
  static double students_den(double t, double df);
  // #CAT_Probability density fctn of student's distribution df deg of free t test
  static double Ftest_q(double F, double v1, double v2);
  // #CAT_Probability F distribution probability F | (v1 < v2)
  static double d_sub_a(double_Matrix* vec_signal, double_Matrix* vec_noise);
  // #CAT_Probability Computes d_sub_a, which reduces to d' in the equal variance case. See: http://psych.colorado.edu/~lharvey/P4165/P4165_2004_Fall/2004_Fall_pdf/P4165_SDT.pdf
  static double cdf_inv(double x);
  // #CAT_Probability Computes the inverse cumulative distribution function for the unit Gaussian distribution.
  static double integrate_polynomial(double_Matrix* coef, double min, double max);
  // #CAT_Probability Computes the inverse cumulative distribution function for the unit Gaussian distribution.

  /////////////////////////////////////////////////////////////////////////////////
  // Vector operations (operate on Matrix objects, treating as a single linear guy)

  ///////////////////////////////////////
  // arithmetic ops

  static void   vec_fm_ints(double_Matrix* double_mat, const int_Matrix* int_mat);
  // #NO_CSS_MATH initialize given double matrix (which is completely overwritten) from integer matrix data
  static void   vec_to_ints(int_Matrix* int_mat, const double_Matrix* double_mat);
  // #NO_CSS_MATH copy double matrix data back to integer matrix

  static bool   vec_check_type(const double_Matrix* a);
  // #NO_CSS_MATH check that matrix is actually a double type -- issues Error if not and returns false
  static bool   vec_check_same_size(const double_Matrix* a, const double_Matrix* b,
                                    bool quiet = false, bool flex = true);
  // check that both vectors are the same size, and issue warning if not (unless quiet) -- if flex then use flexible test (assumes use of iterators for going through operators)

  static bool  vec_add(double_Matrix* a, const double_Matrix* b);
  // #CAT_Arithmetic add elements in two vectors: a(i) += b(i)
  static bool  vec_sub(double_Matrix* a, const double_Matrix* b);
  // #CAT_Arithmetic subtract elements in two vectors: a(i) -= b(i)
  static bool  vec_mult_els(double_Matrix* a, const double_Matrix* b);
  // #CAT_Arithmetic multiply elements in two vectors: a(i) *= b(i)
  static bool  vec_div_els(double_Matrix* a, const double_Matrix* b);
  // #CAT_Arithmetic divide elements in two vectors: a(i) /= b(i)

  static bool  vec_add_scalar(double_Matrix* a, double b);
  // #CAT_Arithmetic add scalar value b to elements in vector a: a(i) += b
  static bool  vec_sub_scalar(double_Matrix* a, double b);
  // #CAT_Arithmetic subtract scalar value b to elements in vector a: a(i) -= b
  static bool  vec_mult_scalar(double_Matrix* a, double b);
  // #CAT_Arithmetic multiply scalar value b to elements in vector a: a(i) *= b
  static bool  vec_div_scalar(double_Matrix* a, double b);
  // #CAT_Arithmetic divide scalar value b to elements in vector a: a(i) /= b
  static bool   vec_quantize(double_Matrix* vec, double grid);
  // #CAT_Arithmetic quantize the values of vector vec to be an integer multiple of grid size value

  static bool   vec_simple_math(double_Matrix* vec, const SimpleMathSpec& math_spec);
  // #CAT_Arithmetic apply standard kinds of simple math operators to values in the vector

  static bool   vec_simple_math_arg(double_Matrix* vec, const double_Matrix* arg_vec,
                                    const SimpleMathSpec& math_spec);
  // #CAT_Arithmetic apply simple math operators to values in vector, other vector provides 'arg' value for math_spec

  static bool vec_students_cum(double_Matrix* t,const double_Matrix* df);
  // #CAT_Probability element wise area between -t and t of student's distribution df deg of free t test
  static bool vec_students_cum_cum(double_Matrix* t,const double_Matrix* df);
  // #CAT_Probability element wise cumulative student's distribution df deg of free t test (area from 0 to t)
  static bool vec_gauss_inv(double_Matrix* p);
  // #CAT_Probability element-wise inverse of the cumulative for p: z value for given p
  static bool vec_gauss_inv_lim(double_Matrix* p);
  // #CAT_Probability element-wise inverse of the cumulative for p: z value for given p . returns non-0 values for p==0 or p ==1

  ///////////////////////////////////////
  // basic statistics

  static double vec_first(const double_Matrix* vec);
  // #CAT_Statistics first item in the vector
  static double vec_last(const double_Matrix* vec);
  // #CAT_Statistics last item in the vector
  static int    vec_find_first(const double_Matrix* vec, Relation& rel);
  // #CAT_Statistics find first element in the vector that meets relationship rel -- returns index in vector or -1 if not found
  static int    vec_find_last(const double_Matrix* vec, Relation& rel);
  // #CAT_Statistics find first element in the vector that meets relationship rel -- returns index in vector or -1 if not found 
  static double vec_max(const double_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the maximum value
  static double vec_abs_max(const double_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the maximum absolute value
  static double vec_next_max(const double_Matrix* vec, int max_idx, int& idx);
  // #CAT_Statistics value and index of the element that has the next-largest value, excluding the max item which is at max_idx -- uses entire matrix, ignoring any view of sub-elements
  static double vec_min(const double_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the minimum value
  static double vec_abs_min(const double_Matrix* vec, int& idx);
  // #CAT_Statistics value and index of the (first) element that has the minimum value
  static double vec_next_min(const double_Matrix* vec, int min_idx, int& idx);
  // #CAT_Statistics value and index of the element that has the next-smallest value, excluding the min item which is at min_idx -- uses entire matrix, ignoring any view of sub-elements

  static double vec_sum(const double_Matrix* vec);
  // #CAT_Statistics compute the sum of the values in the vector
  static double vec_sum_range(const double_Matrix* vec, int start=0, int end=-1);
  // #CAT_Statistics compute the sum of the values in the vector from el=startpos to el=endpos-1 -- uses entire matrix, ignoring any view of sub-elements
  static double vec_prod(const double_Matrix* vec);
  // #CAT_Statistics compute the product of the values in the vector
  static double vec_mean(const double_Matrix* vec);
  // #CAT_Statistics compute the mean of the values in the vector
  static double vec_var(const double_Matrix* vec, double mean=0, bool use_mean=false, bool use_est=false);
  // #CAT_Statistics compute the variance of the values, opt with given mean, if use_est is true, divides by N-1 rather than N
  static double vec_std_dev(const double_Matrix* vec, double mean=0, bool use_mean=false, bool use_est=false);
  // #CAT_Statistics compute the standard deviation of the values, opt with given mean, if use_est is true, divides by N-1 rather than N
  static double vec_sem(const double_Matrix* vec, double mean=0, bool use_mean=false);
  // #CAT_Statistics compute the standard error of the mean of the values, opt with given mean
  static double vec_ss_len(const double_Matrix* vec);
  // #CAT_Statistics sum-of-squares length of the vector
  static double vec_norm(const double_Matrix* vec);
  // #CAT_Statistics square root of the sum-of-squares length of the vector -- its norm or euclidean length
  static double vec_ss_mean(const double_Matrix* vec);
  // #CAT_Statistics sum-of-squares around the mean of the vector
  static void   vec_histogram(double_Matrix* hist_vec, const double_Matrix* src_vec,
                              double bin_size, double min_val = 0.0, double max_val = 0.0);
  // #CAT_Statistics gets a histogram (counts) of number of values within each bin size in source vector -- min and maximum ranges to compute within are also optional args -- only used if min != max
  static double vec_count(const double_Matrix* vec, Relation& rel);
  // #CAT_Statistics count number of times relationship is true
  static double vec_median(const double_Matrix* vec);
  // #CAT_Statistics compute the median of the values in the vector (middle value) -- requires sorting
  static double vec_mode(const double_Matrix* vec);
  // #CAT_Statistics compute the mode (most frequent) of the values in the vector -- requires sorting
  static double vec_quantile(const double_Matrix* vec, double quant_pos);
  // #CAT_Statistics compute arbitrary quantile according to quant_pos value, which is a proportion 0-1 from start to end of sorted list of values, e.g., .5 = median, .25 = first quartile, etc
  static double vec_kwta(double_Matrix* vec, int k, bool descending = true);
  // #CAT_Statistics perform an optimized k-winners-take-all sort, returning the value of the item that is k from the highest (lowest if !descending) on the list -- this can be much faster than vec_quantile, which does a full sort -- uses entire matrix, ignoring any view of sub-elements
  static void   vec_kwta_avg(double& top_k_avg, double& bot_k_avg,
                             double_Matrix* vec, int k, bool descending = true);
  // #CAT_Statistics perform an optimized k-winners-take-all sort, returning the average of the top k values, and average of the bottom k (reversed if !descending) on the list -- uses entire matrix, ignoring any view of sub-elements

  static String vec_stats(const double_Matrix* vec);
  // #CAT_Statistics compute standard descriptive statistics on given vector data, returning result as a string of name=value; pairs (e.g., mean=3.2; etc).

  static int   vec_prob_choose(double_Matrix* vec);
  // #CAT_Statistics given a vector of probability values, choose an index according to its corresponding probability

  static void   vec_sort(double_Matrix* vec, bool descending = false);
  // #CAT_Statistics sort the given vector values in numerical order (in place) -- uses entire matrix, ignoring any view of sub-elements

  static bool   vec_uniq(const taMatrix* src_vec, taMatrix* dest_vec, const bool& sort_first = false);
  // #CAT_Statistics Uniqify the vector, removing all consecutive elements that are the same. use vec_sort first to get a vector of unique elements. -- uses entire matrix, ignoring any view of sub-elements

  static bool   mat_concat(const taMatrix* src_a_mat, const taMatrix* src_b_mat,
                           taMatrix* dest_mat, const int& dim = 0);
  // #CAT_Statistics Concatenate a and b into dest. If dim = 0 (default) concatenate the columns. If dim = 1 concatenate the rows. -- uses entire matrix, ignoring any view of sub-elements


  static double  vec_dprime(const double_Matrix* signal_vec,
			    const double_Matrix* noise_vec);
  // #CAT_Statistics computes the d-prime (d') statistic from signal and noise vectors -- just computes the mean and standard deviation of each vector and then computes d' from there using standard formula (see plain dprime function)

  static bool   vec_regress_lin(const double_Matrix* x_vec, const double_Matrix* y_vec,
                                double& b, double& m, double& cov00, double& cov01,
                                double& cov11, double& sum_sq);
  // #CAT_Statistics computes the best-fit linear regression coefficients (b,m) of the model y = b + mx for the dataset (x, y). The variance-covariance matrix for the parameters (c0, c1) is estimated from the scatter of the points around the best-fit line and returned via the parameters (cov00, cov01, cov11). The sum of squares of the residuals from the best-fit line is returned in sum_sq.  See vec_correl to compute the correlation coefficient. -- uses entire matrix, ignoring any view of sub-elements

  static bool vec_regress_multi_lin(double_Matrix* X, double_Matrix* Y, double_Matrix* C,
                                    double_Matrix* cov, double& chisq);
  // #CAT_Statistics This function computes the best-fit parameters c of the model y = X c for the observations y and the matrix of predictor variables X. The variance-covariance matrix of the model parameters cov is estimated from the scatter of the observations about the best-fit. The sum of squares of the residuals from the best-fit, chi^2, is returned in chisq. The best-fit is found by singular value decomposition of the matrix X using the preallocated workspace provided in work. The modified Golub-Reinsch SVD algorithm is used, with column scaling to improve the accuracy of the singular values. Any components which have zero singular value (to machine precision) are discarded from the fit. -- uses entire matrix, ignoring any view of sub-elements

  static bool vec_regress_multi_lin_polynomial(double_Matrix* dx, double_Matrix* dy,
                                               double_Matrix* coef,  double_Matrix* cov,
                                               int degree, double& chisq);
    // #CAT_Statistics This function computes the best-fit parameters c of the model y = X c for the observations y and the matrix of predictor variables X. The variance-covariance matrix of the model parameters cov is estimated from the scatter of the observations about the best-fit. The sum of squares of the residuals from the best-fit, chi^2, is returned in chisq. The best-fit is found by singular value decomposition of the matrix X using the preallocated workspace provided in work. The modified Golub-Reinsch SVD algorithm is used, with column scaling to improve the accuracy of the singular values. Any components which have zero singular value (to machine precision) are discarded from the fit. -- uses entire matrix, ignoring any view of sub-elements

  static bool vec_jitter_gauss(double_Matrix* vec, double stdev);
  // #CAT_Statistics jitters all the non-zero elements of vec by a gaussian with stdev rounded to the nearest int. jittered indices below zero or above the length of the vector are rejittered until they fall inside. there must be at least one zero element. method is clobber safe - the number of elements after jittering is guaranteed to be the same as the number of elements before jittering. see also: http://en.wikipedia.org/wiki/Jitter#Random_jitter -- uses entire matrix, ignoring any view of sub-elements

  ///////////////////////////////////////
  // distance metrics (comparing two vectors)

  static double vec_ss_dist(const double_Matrix* vec, const double_Matrix* oth_vec,
                            bool norm = false, double tolerance=0.0);
  // #CAT_Distance compute sum-squares dist between this and the oth, tolerance is by element
  static double vec_euclid_dist(const double_Matrix* vec, const double_Matrix* oth_vec,
                                bool norm = false, double tolerance=0.0);
  // #CAT_Distance compute Euclidian dist between this and the oth, tolerance is by element
  static double vec_hamming_dist(const double_Matrix* vec, const double_Matrix* oth_vec,
                                 bool norm = false, double tolerance=0.0);
  // #CAT_Distance compute Hamming dist between this and the oth, tolerance is by element 
  static double vec_covar(const double_Matrix* vec, const double_Matrix* oth_vec);
  // #CAT_Distance compute the covariance of this vector the oth vector
  static double vec_correl(const double_Matrix* vec, const double_Matrix* oth_vec);
  // #CAT_Distance compute the correlation of this vector with the oth vector
  static double vec_inner_prod(const double_Matrix* vec, const double_Matrix* oth_vec,
                               bool norm = false);
  // #CAT_Distance compute the inner product of this vector and the oth vector
  static double vec_cross_entropy(const double_Matrix* vec, const double_Matrix* oth_vec);
  // #CAT_Distance compute cross entropy between this and other vector, this is 'p' other is 'q'
  static double vec_dist(const double_Matrix* vec, const double_Matrix* oth_vec,
                         DistMetric metric, bool norm = false, double tolerance=0.0);
  // #CAT_Distance compute generalized distance metric with other vector (double_Matrix* vec, calls appropriate fun above)
  static double scalar_dist(double v1, double v2,
                            DistMetric metric, double tolerance=0.0);
  // #CAT_Distance compute distance metric on scalar values -- not really sensible for COVAR or CORREL (which reduce to SUM_SQUARES and EUCLIDIAN, respectively)

  ///////////////////////////////////////
  // Normalization

  static double vec_norm_len(double_Matrix* vec, double len=1.0);
  // #CAT_Norm normalize vector to total given length (1.0), returns scaling factor
  static double vec_norm_sum(double_Matrix* vec, double sum=1.0, double min_val=0.0);
  // #CAT_Norm normalize vector to total given sum (1.0) and min_val (0), returns scaling factor
  static double vec_norm_max(double_Matrix* vec, double max=1.0);
  // #CAT_Norm normalize vector to given maximum value, returns scaling factor
  static double vec_norm_abs_max(double_Matrix* vec, double max=1.0);
  // #CAT_Norm normalize vector to given absolute maximum value, returns scaling factor
  static int    vec_threshold(double_Matrix* vec, double thresh=.5,
                              double low=0.0, double high=1.0);
  // #CAT_Norm threshold values in the vector, low vals go to low, etc; returns number of high values
  static int    vec_threshold_low(double_Matrix* vec, double thresh=.5,
                                  double low=0.0);
  // #CAT_Norm threshold values in the vector, low vals go to low; returns number of low vals
  static int    vec_threshold_high(double_Matrix* vec, double thresh=.5,
                                  double high=0.0);
  // #CAT_Norm threshold values in the vector, high vals go to high; returns number of high values

  static int    vec_replace(double_Matrix* vec, double find1, double repl1,
                            bool do2 = false, double find2=0.0, double repl2=0.0,
                            bool do3 = false, double find3=0.0, double repl3=0.0,
                            bool do4 = false, double find4=0.0, double repl4=0.0,
                            bool do5 = false, double find5=0.0, double repl5=0.0,
                            bool do6 = false, double find6=0.0, double repl6=0.0);
  // #CAT_Norm find and replace values -- replace find val with repl replacement val -- do flags activate usage of multiple replacements -- more efficient to do in parallel -- returns number of replacments made
  static double vec_aggregate(const double_Matrix* vec, Aggregate& agg);
  // #CAT_Aggregate compute aggregate of values in this vector using aggregation params of agg

  ///////////////////////////////////////
  // Convolution

  static bool   vec_kern_uniform(double_Matrix* kernel, int half_sz,
                                 bool neg_tail=true, bool pos_tail=true);
  // #CAT_Convolution create a unit-sum-normalized uniform kernel of given half-size (size set to 2* half_sz + 1) in given vector.  if given tail direction is false, it is replaced with zeros -- uses entire matrix, ignoring any view of sub-elements
  static bool   vec_kern_gauss(double_Matrix* kernel, int half_sz, double sigma,
                               bool neg_tail=true, bool pos_tail=true);
  // #CAT_Convolution create a unit-sum-normalized gaussian kernel of given half-size (size set to 2* half_sz + 1) in given vector, with given sigma (standard deviation) value.  if given tail direction is false, it is replaced with zeros -- uses entire matrix, ignoring any view of sub-elements
  static bool   vec_kern_exp(double_Matrix* kernel, int half_sz, double exp_mult,
                             bool neg_tail=true, bool pos_tail=true);
  // #CAT_Convolution create a unit-sum-normalized symmetric exponential kernel of given half-size (size set to 2* half_sz + 1) in given vector, with given exponential multiplier.  symmetric means abs(i-ctr) is used for value.  if given tail direction is false, it is replaced with zeros -- uses entire matrix, ignoring any view of sub-elements
  static bool   vec_kern_pow(double_Matrix* kernel, int half_sz, double pow_exp,
                             bool neg_tail=true, bool pos_tail=true);
  // #CAT_Convolution create a unit-sum-normalized power-function kernel of given half-size (size set to 2* half_sz + 1) in given vector, with given exponential multiplier.  if given tail direction is false, it is replaced with zeros -- uses entire matrix, ignoring any view of sub-elements
  static bool   vec_convolve(double_Matrix* out_vec, const double_Matrix* in_vec,
                             const double_Matrix* kernel, bool keep_edges = false);
  // #CAT_Convolution convolve in_vec with kernel to produce out_vec.  out_vec_i = sum_j kernel_j * in_vec_[i+j-off] (where off is 1/2 width of kernel).  normally, out_vec is indented by the offset and width of the kernel so that the full kernel is used for all out_vec points.  however, if keep_edges is true, it keeps these edges by clipping and renormalizing the kernel all the way to both edges -- uses entire matrix, ignoring any view of sub-elements

  static bool   vec_kern2d_gauss(double_Matrix* kernel, int sz_x, int sz_y,
                                 double sigma_x, double sigma_y);
  // #CAT_Convolution create a unit-sum-normalized 2D gaussian kernel of given size in each axis (total size, not half size) in given vector, with given sigma (standard deviation) value in each axis -- uses entire matrix, ignoring any view of sub-elements

  /////////////////////////////////////////////////////////////////////////////////
  // Standard Matrix operations: operate on a 2-dimensional matrix

#if (defined(HAVE_LIBGSL) && !defined(__MAKETA__))
  static bool mat_get_gsl_fm_ta(gsl_matrix* gsl_mat, const double_Matrix* ta_mat);
  // #IGNORE helper function to get a gsl-formatted matrix from a ta matrix
  static bool vec_get_gsl_fm_ta(gsl_vector* gsl_vec, const double_Matrix* ta_vec);
  // #IGNORE helper function to get a gsl-formatted vector from a one-dimensional ta matrix (vector)
#endif

  static bool mat_col(double_Matrix* col, const double_Matrix* mat, int col_no);
  // #CAT_Matrix #NO_CSS_MATH get indicated column number (from two-d matrix
  static bool mat_row(double_Matrix* row, const double_Matrix* mat, int row_no);
  // #CAT_Matrix #NO_CSS_MATH get indicated row number from two-d matrix

  static bool mat_add(double_Matrix* a, const double_Matrix* b);
  // #CAT_Matrix #NO_CSS_MATH add the elements of matrix b to the elements of matrix a: a(i,j) += b(i,j); the two matricies must have the same dimensions -- can also be achieved by just using the + operator
  static bool mat_sub(double_Matrix* a, const double_Matrix* b);
  // #CAT_Matrix #NO_CSS_MATH subtract the elements of matrix b to the elements of matrix a: a(i,j) -= b(i,j); the two matricies must have the same dimensions -- can also be achieved by just using the - operator
  static bool mat_mult_els(double_Matrix* a, const double_Matrix* b);
  // #CAT_Matrix #NO_CSS_MATH multiply the elements of matrix b with the elements of matrix a: a(i,j) *= b(i,j); the two matricies must have the same dimensions -- can also be achieved by just using the * operator
  static bool mat_div_els(double_Matrix* a, const double_Matrix* b);
  // #CAT_Matrix #NO_CSS_MATH divide the elements of matrix b by the elements of matrix a: a(i,j) /= b(i,j); the two matricies must have the same dimensions -- can also be achieved by just using the / operator

  static bool mat_transpose(double_Matrix* dest, const double_Matrix* src);
  // #CAT_Matrix #NO_CSS_MATH transpose a 2d matrix, swapping rows and columns (also works for complex_Matrix args)
  static bool mat_mult(double_Matrix* c, const double_Matrix* a, const double_Matrix* b);
  // #CAT_Matrix #NO_CSS_MATH matrix multiplication (not element-wise -- see mat_mult_els) -- c = a * b -- number of columns of a must be same as number of rows of b, and vice-versa -- does NOT support complex_Matrix type
  static double mat_det(const double_Matrix* a);
  // #CAT_Matrix #LABEL_det return the determinant of the given square matrix

  static bool mat_vec_product(const double_Matrix* A, const double_Matrix* x,
                              double_Matrix* y);
  // #CAT_Statistics computes the matrix-vector product y=Ax -- uses entire matrix, ignoring any view of sub-elements

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
  static bool mat_slice(double_Matrix* dest, double_Matrix* src, int d0_start = 0, int d0_end = -1, int d1_start = 0, int d1_end = -1);
  // #CAT_Matrix #NO_CSS_MATH See http://en.wikipedia.org/wiki/Array_slicing. Copies a 2d slice out of the first 2 dimensions of src into dest. If d0_end or d1_end are -1 (default) they will be set to the size of that dimension. See also taDataProc::Slice2D.
  static bool mat_trim(double_Matrix* dest, double_Matrix* src, Relation& thresh, int intol_within = 0, int intol_between = 0,
                       bool left = true, bool right = true, bool top = true, bool bottom = true);
  // #CAT_Matrix #NO_CSS_MATH Trim all consecutive rows/columns starting from top, bottom, left and/or right that satisfy thresh. The intolerance params control how greedy the threshold operation is. intol_within controls how many values in this row/col that don't satisfy thresh can be found and the row/col will still be trimmed. intol_between controls how many consecutive rows/cols that don't satisfy thresh can be found, followed by a row/col that DOES satisfy thresh, that will still be trimmed. Returns false if the entire matrix matches so be sure to check the return value.

  /////////////////////////////////////////////////////////////////////////////////
  // higher-dimensional matrix frame-based operations (matrix = collection of matricies)

  static bool mat_cell_to_vec(double_Matrix* vec, const double_Matrix* mat, int cell_no);
  // #CAT_HighDimMatrix extract given cell element across frames of matrix, and put in vector vec (usueful for analyzing behavior of a given cell across time or whatever the frames represent)
  static bool mat_dist(double_Matrix* dist_mat, const double_Matrix* src_mat,
                       DistMetric metric, bool norm = false, double tolerance=0.0);
  // #CAT_HighDimMatrix compute distance matrix of frames within matrix src_mat (must be dim >= 2) -- dist_mat is nframes x nframes
  static bool mat_cross_dist(double_Matrix* dist_mat, const double_Matrix* src_mat_a,
                               const double_Matrix* src_mat_b,
                               DistMetric metric, bool norm = false, double tolerance=0.0);
  // #CAT_HighDimMatrix compute cross distance matrix between the frames within src_mat_a and src_mat_b (must be dim >= 2 and have same frame size) -- rows of dist_mat are a, cols are b
  static bool mat_correl(double_Matrix* correl_mat, const double_Matrix* src_mat);
  // #CAT_HighDimMatrix compute correlation matrix for cells across frames within src_mat (i.e., how does each cell co-vary across time/frames with each other cell). result is nxn matrix where n is number of cells in each frame of src_mat (i.e., size of sub-matrix), with each cell being correlation of that cell with other cell.
  static bool mat_prjn(double_Matrix* prjn_vec, const double_Matrix* src_mat,
                       const double_Matrix* prjn_mat, DistMetric metric=INNER_PROD,
                       bool norm = false, double tolerance=0.0);
  // #CAT_HighDimMatrix compute projection of each frame of src_mat onto prjn_mat.  prjn_vec contains one value for each frame in src_mat, which is the inner/dot product (projection -- or other metric if selected) of that frame and the prjn_mat.

  /////////////////////////////////////////////////////////////////////////////////
  // frame-compatible versions of various functions

  static bool mat_time_avg(double_Matrix* a, double avg_dt);
  // #CAT_HighDimMatrix replace matrix values with their running (exponentially-weighted) time-averaged values (new_val = avg_dt * running_avg + (1-avg_dt) * old_val
  static bool mat_frame_convolve(double_Matrix* out_mat, const double_Matrix* in_mat,
                                 const double_Matrix* kernel);
  // #CAT_Convolution convolve in_mat with kernel to produce out_mat, in a cell-by-cell manner across frames.  always keeps the edges by clipping and renormalizing the kernel all the way to both edges

  /////////////////////  Statistics

  static bool   mat_fmt_out_frame(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix format out_mat as one frame (inner dimensions) of in_mat (i.e., out_mat has n-1 dimensions relative to in_mat)

  static bool   mat_frame_set_n(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix set out_mat to the number of frames for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_first(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute first item for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_last(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute last item for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_find_first(double_Matrix* out_mat, const double_Matrix* in_mat, Relation& rel);
  // #CAT_HighDimMatrix compute index of first item that meets relationship rel for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_find_last(double_Matrix* out_mat, const double_Matrix* in_mat, Relation& rel);
  // #CAT_HighDimMatrix compute index of last item that meets relationship rel for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_max(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute maximum for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_min(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute minimum for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_abs_max(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute maximum of absolute values for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_abs_min(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute minimum of absolute values for each value within inner frame dimensions across matrix frames (last outer dimension)

  static bool   mat_frame_sum(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute sum for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_prod(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute product for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_mean(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute mean for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_var(double_Matrix* out_mat, const double_Matrix* in_mat, bool use_est=false);
  // #CAT_HighDimMatrix compute variance for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_std_dev(double_Matrix* out_mat, const double_Matrix* in_mat, bool use_est=false);
  // #CAT_HighDimMatrix compute standard deviation for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_sem(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute standard error of the mean for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_ss_len(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute sum-of-squares length for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_ss_mean(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute sum of squares around the mean for each value within inner frame dimensions across matrix frames (last outer dimension)

  static bool   mat_frame_count(double_Matrix* out_mat, const double_Matrix* in_mat, Relation& rel);
  // #CAT_HighDimMatrix compute count number of times relationship rel is true for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_median(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute median (middle value -- requires sorting) for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_mode(double_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_HighDimMatrix compute mode (most frequent) value (requires sorting) for each value within inner frame dimensions across matrix frames (last outer dimension)
  static bool   mat_frame_quantile(double_Matrix* out_mat, const double_Matrix* in_mat, double quant_pos);
  // #CAT_HighDimMatrix compute arbitrary quantile according to quant_pos value, which is a proportion 0-1 from start to end of sorted list of values, e.g., .5 = median, .25 = first quartile, etc, for each value within inner frame dimensions across matrix frames (last outer dimension)

  static bool   mat_frame_aggregate(double_Matrix* out_mat, const double_Matrix* in_mat,
                                    Aggregate& agg);
  // #CAT_Aggregate compute aggregate across matrix frames (last outer dimension) for each value within inner frame dimensions, using aggregation params of agg


  /////////////////////////////////////////////////////////////////////////////////
  // fft routines -- css friendly versions in cssMath

  static bool fft_real(complex_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_FFT #NO_CSS_MATH compute the fast fourier transform (FFT) of the real data in in_mat, writing the complex output to out_mat. results are much faster if size is divisible by many small factors, but any size can be used (radix-N).  complex numbers stored in out_mat as inner-most dimension of matrix, of size 2, first is real, 2nd is imag.  if there are more than 1 dims in in_mat, the FFT is computed for all frames of d0 (assuming a time series of 1d frames -- see fft2 for 2d)
  static bool fft_complex(complex_Matrix* mat);
  // #CAT_FFT #NO_CSS_MATH compute the fast fourier transform (FFT) of the complex data in the matrix, in place (overwrites contents of mat). results are much faster if size is divisible by many small factors, but any size can be used (radix-N).  complex numbers stored as inner-most dimension of matrix, of size 2, first is real, 2nd is imag. if there are more than 1 dims in in_mat, the FFT is computed for all frames of d0 (assuming a time series of 1d frames -- see fft2 for 2d)
  static bool ffti_complex(complex_Matrix* mat);
  // #CAT_FFT #NO_CSS_MATH compute the inverse fast fourier transform (FFT) of the complex data in the matrix, in place (overwrites contents of mat). results are much faster if size is divisible by many small factors, but any size can be used (radix-N).  complex numbers stored as inner-most dimension of matrix, of size 2, first is real, 2nd is imag.  if there are more than 1 dims in in_mat, the FFT is computed for all frames of d0 (assuming a time series of 1d frames -- see fft2 for 2d)

  static bool fft2_real(complex_Matrix* out_mat, const double_Matrix* in_mat);
  // #CAT_FFT #NO_CSS_MATH compute the 2D fast fourier transform (FFT) of the real data in in_mat, writing the complex output to out_mat. results are much faster if size is divisible by many small factors, but any size can be used (radix-N).  complex numbers are stored in out_mat as inner-most dimension of matrix, of size 2, first is real, 2nd is imag.  if there are more than 2D dims in in_mat, the FFT is computed for each 2D frame
  static bool fft2_complex(complex_Matrix* mat);
  // #CAT_FFT #NO_CSS_MATH compute the 2D fast fourier transform (FFT) of the complex data in the matrix, in place (overwrites contents of mat). results are much faster if size is divisible by many small factors, but any size can be used (radix-N).  complex numbers stored as inner-most dimension of matrix, of size 2, first is real, 2nd is imag.  if there are more than 2D dims in in_mat, the FFT is computed for each 2D frame
  static bool ffti2_complex(complex_Matrix* mat);
  // #CAT_FFT #NO_CSS_MATH compute the 2D inverse fast fourier transform (FFT) of the complex data in the matrix, in place (overwrites contents of mat). results are much faster if size is divisible by many small factors, but any size can be used (radix-N).  complex numbers stored as inner-most dimension of matrix, of size 2, first is real, 2nd is imag. if there are more than 2D dims in in_mat, the FFT is computed for each 2D frame

  void Initialize() { };
  void Destroy() { };
  TA_ABSTRACT_BASEFUNS_NOCOPY(taMath_double);
};

#endif // taMath_double_h
