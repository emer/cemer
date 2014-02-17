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

#ifndef taMath_h
#define taMath_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class float_Matrix; // 
class double_Matrix; // 

#ifndef __MAKETA__
# include <cmath>
#endif

#if (defined(HAVE_LIBGSL) && !defined(__MAKETA__))
# include "gsl/gsl_matrix_double.h"
# include "gsl/gsl_matrix_float.h"
#endif

#ifdef TA_OS_WIN
// following for msvc
# ifdef min
#   undef min
# endif
# ifdef max
#   undef max
# endif
# ifdef isnan
#   undef isnan
# endif
# ifdef isinf
#   undef isinf
# endif
#endif

taTypeDef_Of(taMath);

class TA_API taMath : public taNBase {
  // #STEM_BASE ##CAT_Math ##NO_TOKENS ##INSTANCE collection of commonly-used math functions
INHERITED(taNBase)
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

  enum DistMetric {             // generalized distance metrics
    SUM_SQUARES,                // sum of squares:  sum[(x-y)^2]
    EUCLIDIAN,                  // Euclidian distance (sqrt of sum of squares)
    HAMMING,                    // Hamming distance: sum[abs(x-y)]
    COVAR,                      // covariance: sum[(x-<x>)(y-<y>)]
    CORREL,                     // correlation: sum[(x-<x>)(y-<y>)] / sqrt(sum[x^2 y^2])
    INNER_PROD,                 // inner product: sum[x y]
    CROSS_ENTROPY               // cross entropy: sum[x ln(x/y) + (1-x)ln((1-x)/(1-y))]
  };

  static bool   dist_larger_further(DistMetric metric);
  // returns true if a larger value of given distance metric means further apart

  static void mat_cvt_double_to_float(float_Matrix* flt_dest, const double_Matrix* dbl_src);
  // #CAT_Convert convert double vector/matrix data to float
  static void mat_cvt_float_to_double(double_Matrix* dbl_dest, const float_Matrix* flt_src);
  // #CAT_Convert convert float vector/matrix data to double

  void Initialize() { };
  void Destroy() { };
  TA_ABSTRACT_BASEFUNS_NOCOPY(taMath) //
};

#endif // taMath_h
