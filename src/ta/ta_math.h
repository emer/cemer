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

#include <math.h>
#include "ta_matrix.h"

//#ifdef TA_GSL
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_matrix_float.h>
//#endif

class TA_API taMath : public taBase {
  // collection of commonly-used math functions
public:

  /////////////////////////////////////////////////////////////////////////////////
  // Params: parameters controlling various math functions
  
  static int max_iterations;
  // #DEF_100 #CAT_Params maximum number of iterations for iterative functions
  static double err_tolerance;
  // #DEF_3.0e-7 #CAT_Params error tolerance for iterative functions to stop

  /////////////////////////////////////////////////////////////////////////////////
  // Arithmetic: basic arithmatic

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

  static double  add(double x, double y) { return x + y; }
  // #CAT_Arithmetic add
  static float   addf(float x, float y) { return x + y; }
  // #CAT_Arithmetic add
  static double  sub(double x, double y) { return x - y; }
  // #CAT_Arithmetic subtract
  static float  subf(float x, float y) { return x - y; }
  // #CAT_Arithmetic subtract
  static double  mult(double x, double y) { return x * y; }
  // #CAT_Arithmetic multiply
  static float   multf(float x, float y) { return x * y; }
  // #CAT_Arithmetic multiply
  static double  div(double x, double y) { return x / y; }
  // #CAT_Arithmetic divide
  static float   divf(float x, float y) { return x / y; }
  // #CAT_Arithmetic divide

  static double  fabs(double x) { return fabs(x); }
  // #CAT_Arithmetic absolute value
  static float   fabsf(float x) { return fabsf(x); }
  // #CAT_Arithmetic absolute value
  static double copysign(double x, double y) { return copysign(x, y); }
  // #CAT_Arithmetic copy sign from x to y
  static float copysignf(float x, float y) { return copysignf(x, y); }
  // #CAT_Arithmetic copy sign from x to y

  static double nextafter(double x, double y) { return nextafter(x, y); }
  // #CAT_Arithmetic return next largest distinct floating point number after x in direction of y
  static float nextafterf(float x, float y) { return nextafterf(x, y); }
  // #CAT_Arithmetic return next largest distinct floating point number after x in direction of y

  static double  min(double x, double y) { return (x < y) ? x : y; }
  // #CAT_Arithmetic minimum of x and y
  static float   minf(float x, float y) { return (x < y) ? x : y; }
  // #CAT_Arithmetic minimum of x and y
  static double  max(double x, double y) { return (x > y) ? x : y; }
  // #CAT_Arithmetic maximum of x and y
  static float   maxf(float x, float y) { return (x > y) ? x : y; }
  // #CAT_Arithmetic maximum of x and y

  static double fmax(double x, double y) { return fmax(x,y); }
  // #CAT_Arithmetic maximum of x and y
  static float  fmaxf(float x, float y) { return fmaxf(x,y); }
  // #CAT_Arithmetic maximum of x and y

  static double fmin(double x, double y) { return fmin(x,y); }
  // #CAT_Arithmetic minimum of x and y
  static float  fminf(float x, float y)  { return fminf(x,y); }
  // #CAT_Arithmetic minimum of x and y

  static double ceil(double x) { return ceil(x); }
  // #CAT_Arithmetic ceiling of x: next largest integer value from x
  static float ceilf(float x) { return ceilf(x); }
  // #CAT_Arithmetic ceiling of x: next largest integer value from x

  static double floor(double x) { return floor(x); }
  // #CAT_Arithmetic floor of x: next smallest integer value from x
  static float floorf(float x) { return floorf(x); }
  // #CAT_Arithmetic floor of x: next smallest integer value from x

  static double rint(double x) { return rint(x); }
  // #CAT_Arithmetic round value to an integer using current rounding direction
  static float rintf(float x) { return rintf(x); }
  // #CAT_Arithmetic round value to an integer using current rounding direction

  static double round(double x) { return round(x); }
  // #CAT_Arithmetic round value to an integer irrespective of current rounding direction
  static float roundf(float x) { return roundf(x); }
  // #CAT_Arithmetic round value to an integer irrespective of current rounding direction

  static double trunc(double x) { return trunc(x); }
  // #CAT_Arithmetic round to truncated integer value (nearest lower magnitude integer) -- like floor but irrespective of sign
  static float truncf(float x) { return truncf(x); }
  // #CAT_Arithmetic round to truncated integer value (nearest lower magnitude integer) -- like floor but irrespective of sign

  static double fmod(double x, double y) { return fmod(x, y); }
  // #CAT_Arithmetic floating-point modulus function: remainder of x / y
  static float fmodf(float x, float y) { return fmodf(x, y); }
  // #CAT_Arithmetic floating-point modulus function: remainder of x / y

  static double remainder(double x, double y) { return remainder(x, y); }
  // #CAT_Arithmetic remainder r = x - ny where n is integer value nearest to x/y
  static float remainderf(float x, float y) { return remainderf(x, y); }
  // #CAT_Arithmetic remainder r = x - ny where n is integer value nearest to x/y

  /////////////////////////////////////////////////////////////////////////////////
  // ExpLog: exponential and logarithmic functions

  static double e;
  // #CAT_Trigonometry #READ_ONLY e: the natural exponential number

  static double exp(double x) { return exp(x); }
  // #CAT_ExpLog The natural exponential (e to the power x: e^x)
  static float  expf(float x) { return expf(x); }
  // #CAT_ExpLog The natural exponential (e to the power x: e^x)
  static double exp2(double x) { return exp2(x); }
  // #CAT_ExpLog The base-2 expoenetial (2 to the power x: 2^x)
  static float  exp2f(float x) { return exp2f(x); }
  // #CAT_ExpLog The base-2 expoenetial (2 to the power x: 2^x)

  static double log(double x) { return log(x); }
  // #CAT_ExpLog The natural logarithm of x: ln(x)
  static float  logf(float x) { return logf(x); }
  // #CAT_ExpLog The natural logarithm of x: ln(x)

  static double log10(double x) { return log10(x); }
  // #CAT_ExpLog The logarithm of x, base 10
  static float  log10f(float x) { return log10f(x); }
  // #CAT_ExpLog The logarithm of x, base 10

  static double log2(double x) { return log2(x); }
  // #CAT_ExpLog The logarithm of x, base 2
  static float  log2f(float x) { return log2f(x); }
  // #CAT_ExpLog The logarithm of x, base 2

  static double pow(double x, double p) { return pow(x, p); }
  // #CAT_ExpLog x to the power p (x^p)
  static float  powf(float x, float p) { return powf(x, p); }
  // #CAT_ExpLog x to the power p (x^p)

  static double sqrt(double x) { return sqrt(x); }
  // #CAT_ExpLog square root of x (i.e., x^1/2)
  static float  sqrtf(float x) { return sqrtf(x); }
  // #CAT_ExpLog square root of x (i.e., x^1/2)

  /////////////////////////////////////////////////////////////////////////////////
  // Trigonometry

  static double pi;
  // #CAT_Trigonometry #READ_ONLY pi: the ratio of circumference to diameter
  static double deg_per_rad;
  // #CAT_Trigonometry #READ_ONLY degrees per radian (180 / pi)

  static double  acos(double X) { return acos(X); }
  // #CAT_Trigonometry The arc-cosine (inverse cosine) -- takes an X coordinate and returns the angle (in radians) such that cos(angle)=X
  static float   acosf(float X) { return acosf(X); }
  // #CAT_Trigonometry The arc-cosine (inverse cosine) -- takes an X coordinate and returns the angle (in radians) such that cos(angle)=X

  static double  asin(double Y) { return asin(Y); }
  // #CAT_Trigonometry The arc-sine (inverse sine) -- takes a Y coordinate and returns the angle (in radians) such that sin(angle)=Y
  static float   asinf(float Y) { return asinf(Y); }
  // #CAT_Trigonometry The arc-sine (inverse sine) -- takes a Y coordinate and returns the angle (in radians) such that sin(angle)=Y

  static double  atan(double Y_over_X) { return atan(Y_over_X); }
  // #CAT_Trigonometry The arc-tangent (inverse tangent) -- takes a Y/X slope and returns angle (in radians) such that tan(angle)=Y/X.
  static float   atanf(float Y_over_X) { return atanf(Y_over_X); }
  // #CAT_Trigonometry The arc-tangent (inverse tangent) -- takes a Y/X slope and returns angle (in radians) such that tan(angle)=Y/X.

  static double  atan2(double Y, double X) { return atan2(Y, X); }
  // #CAT_Trigonometry The arc-tangent (inverse tangent) -- takes a Y/X slope and returns angle (in radians) such that tan(angle)=Y/X.
  static float   atan2f(float Y, float X) { return atan2f(Y, X); }
  // #CAT_Trigonometry The arc-tangent (inverse tangent) -- takes a Y/X slope and returns angle (in radians) such that tan(angle)=Y/X.

  static double  cos(double angle) { return cos(angle); }
  // #CAT_Trigonometry The cosine (x-axis component) of angle (given in radians)
  static double  cos_deg(double angle) { return cos(angle / deg_per_rad); }
  // #CAT_Trigonometry The cosine (x-axis component) of angle (given in degrees)
  static float   cosf(float angle) { return cosf(angle); }
  // #CAT_Trigonometry The cosine (x-axis component) of angle (given in radians).

  static double  sin(double angle) { return sin(angle); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in radians)
  static double  sin_deg(double angle) { return sin(angle / deg_per_rad); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in degrees)
  static float   sinf(float angle) { return sinf(angle); }
  // #CAT_Trigonometry The sine (y-axis component) of angle (given in radians)

  static double  tan(double angle) { return tan(angle); }
  // #CAT_Trigonometry The tangent (slope y over x) of angle (given in radians)
  static double  tan_deg(double angle) { return tan(angle / deg_per_rad); }
  // #CAT_Trigonometry The tangent (slope y over x) of angle (given in degrees)
  static float   tanf(float angle) { return tanf(angle); }
  // #CAT_Trigonometry The tangent (slope y over x) of angle (given in radians)

  static double  acosh(double X) { return acosh(X); }
  // #CAT_Trigonometry The arc-hyperbolic-cosine (inverse hyperbolic cosine) -- takes an X coordinate and returns the angle (in radians) such that cosh(angle)=X
  static float   acoshf(float X) { return acoshf(X); }
  // #CAT_Trigonometry The arc-hyperbolic-cosine (inverse hyperbolic cosine) -- takes an X coordinate and returns the angle (in radians) such that cosh(angle)=X

  static double  asinh(double Y) { return asinh(Y); }
  // #CAT_Trigonometry The arc-hyperbolic-sine (inverse hyperbolic sine) -- takes a Y coordinate and returns the angle (in radians) such that sinh(angle)=Y
  static float   asinhf(float Y) { return asinhf(Y); }
  // #CAT_Trigonometry The arc-hyperbolic-sine (inverse hyperbolic sine) -- takes a Y coordinate and returns the angle (in radians) such that sinh(angle)=Y

  static double  atanh(double Y_over_X) { return atanh(Y_over_X); }
  // #CAT_Trigonometry The arc-hyperbolic-tangent (inverse hyperbolic tangent) -- takes Y over X slope and returns the angle (in radians) such that tanh(angle)=Y
 static float    atanhf(float Y_over_X) { return atanhf(Y_over_X); }
  // #CAT_Trigonometry The arc-hyperbolic-tangent (inverse hyperbolic tangent) -- takes Y over X slope and returns the angle (in radians) such that tanh(angle)=Y

  static double  cosh(double z) { return cosh(z); }
  // #CAT_Trigonometry The hyperbolic-cosine = e^z + e^-z / 2
  static float  coshf(float z) { return coshf(z); }
  // #CAT_Trigonometry The hyperbolic-cosine = e^z + e^-z / 2

  static double  sinh(double z) { return sinh(z); }
  // #CAT_Trigonometry The hyperbolic-sine = e^z - e^-z / 2
  static float  sinhf(float z) { return sinhf(z); }
  // #CAT_Trigonometry The hyperbolic-sine = e^z - e^-z / 2

  static double  tanh(double z) { return tanh(z); }
  // #CAT_Trigonometry The hyperbolic-tangent = sinh(z) / cosh(z)
  static float  tanhf(float z) { return tanhf(z); }
  // #CAT_Trigonometry The hyperbolic-tangent = sinh(z) / cosh(z)

  static double hypot(double x, double y) { return hypot(x,y); }
  // #CAT_Trigonometry the length of the hypotenuse (i.e., Euclidean distance): sqrt(x^2 + y^2)
  static float hypotf(float x, float y) { return hypotf(x,y); }
  // #CAT_Trigonometry the length of the hypotenuse (i.e., Euclidean distance): sqrt(x^2 + y^2)

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
  static double  lgamma(double z) { return lgamma(z); }
  // #CAT_Probability natural log (ln) of gamma function (not gamma distribution): generalization of (n-1)! to real values 
  static float  lgammaf(float z) { return lgammaf(z); }
  // #CAT_Probability natural log (ln) of gamma function (not gamma distribution): generalization of (n-1)! to real values 
  static double tgamma(double z) { return tgamma(z); }
  // #CAT_Probability true gamma function (not gamma distribution): generalization of (n-1)! to real values 
  static float  tgammaf(float z) { return tgammaf(z); }
  // #CAT_Probability true gamma function (not gamma distribution): generalization of (n-1)! to real values 

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
  static float  erff(float x) { return erff(x); }
  // #CAT_Probability the error function: used for computing the normal distribution
  static double erfc(double x) { return erfc(x); }
  // #CAT_Probability complement of the error function (1.0 - erf(x))
  static float  erfcf(float x) { return erfc(x); }
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
  // Vector operations

  
  /////////////////////////////////////////////////////////////////////////////////
  // Matrix operations

  // todo: support this?
//   static bool get_gsl_matrix_fm_ta(double_Matrix* ta_mat, gsl_matrix* gsl_mat);
//   // #IGNORE helper function to get a gsl-formatted matrix from a ta matrix
  static bool get_gsl_matrix_fm_ta_f(float_Matrix* ta_mat, gsl_matrix_float* gsl_mat);
  // #IGNORE helper function to get a gsl-formatted matrix from a ta matrix

  static bool matrix_add(float_Matrix* a, float_Matrix* b);
  // #CAT_Matrix add the elements of matrix b to the elements of matrix a: a(i,j) += b(i,j); the two matricies must have the same dimensions
  static bool matrix_sub(float_Matrix* a, float_Matrix* b);
  // #CAT_Matrix subtract the elements of matrix b to the elements of matrix a: a(i,j) -= b(i,j); the two matricies must have the same dimensions
  static bool matrix_mult_els(float_Matrix* a, float_Matrix* b);
  // #CAT_Matrix multiply the elements of matrix b with the elements of matrix a: a(i,j) *= b(i,j); the two matricies must have the same dimensions
  static bool matrix_div_els(float_Matrix* a, float_Matrix* b);
  // #CAT_Matrix divide the elements of matrix b by the elements of matrix a: a(i,j) /= b(i,j); the two matricies must have the same dimensions

  
};

#endif // TA_MATH_H
