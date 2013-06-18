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

#include "taMath.h"
#include <taMisc>
#include <float_Matrix>
#include <double_Matrix>
#include <double_Array>
#include <MTRnd>

#include <limits.h>
#include <float.h>

#ifdef HAVE_LIBGSL
// gsl error handler -- needed to prevent default from raising abort!
void ta_gsl_err_handler(const char * reason,
  const char * file, int line, int gsl_errno)
{
  // note, the rest of the info is really not helpful to users, so we
  // only print the reason -- Emergent programs will then print the
  // CSS line number, which can be helpful
  taMisc::Error("taMath: GSL library error:", reason);
}

void ta_gsl_module_init() {
  // note: we don't save result, because it will be NULL, and we won't reset it
  //gsl_error_handler_t* =
  gsl_set_error_handler(ta_gsl_err_handler);
}
// initialize error handler
InitProcRegistrar mod_init_gsl(ta_gsl_module_init);
#endif // HAVE_LIBGSL


/////////////////////////////////////////////////////////////////////////////////
// Params: parameters controlling various math functions

int taMath::max_iterations = 100;
double taMath::err_tolerance = 3.0e-7;

/////////////////////////////////////////////////////////////////////////////////
// Arithmetic: basic arithmatic

float taMath::flt_max = FLT_MAX;
float taMath::flt_max_10_exp = FLT_MAX_10_EXP;
float taMath::flt_max_exp = FLT_MAX_EXP;

float taMath::flt_min = FLT_MIN;
float taMath::flt_min_10_exp = FLT_MIN_10_EXP;
float taMath::flt_min_exp = FLT_MIN_EXP;
float taMath::flt_epsilon = FLT_EPSILON;

double taMath::dbl_max = DBL_MAX;
double taMath::dbl_max_10_exp = DBL_MAX_10_EXP;
double taMath::dbl_max_exp = DBL_MAX_EXP;

double taMath::dbl_min = DBL_MIN;
double taMath::dbl_min_10_exp = DBL_MIN_10_EXP;
double taMath::dbl_min_exp = DBL_MIN_EXP;
double taMath::dbl_epsilon = DBL_EPSILON;

// double taMath::INT_MAX = INT_MAX;
// double taMath::INT_MIN = INT_MIN;
// double taMath::LONG_MAX = LONG_MAX;
// double taMath::LONG_MIN = LONG_MIN;

bool taMath::dist_larger_further(DistMetric metric) {
  switch(metric) {
  case SUM_SQUARES:
    return true;
  case EUCLIDIAN:
    return true;
  case HAMMING:
    return true;
  case COVAR:
    return false;
  case CORREL:
    return false;
  case INNER_PROD:
    return false;
  case CROSS_ENTROPY:
    return false;
  }
  return false;
}

void taMath::mat_cvt_double_to_float(float_Matrix* flt_dest, const double_Matrix* dbl_src) {
  flt_dest->SetGeomN(dbl_src->geom);
  for (int i = 0; i < flt_dest->size; ++i) {
    flt_dest->FastEl_Flat(i) = (float)dbl_src->FastEl_Flat(i);
  }
  if(dbl_src->ElView())
    flt_dest->SetElView(dbl_src->ElView(), dbl_src->el_view_mode);
}

void taMath::mat_cvt_float_to_double(double_Matrix* dbl_dest, const float_Matrix* flt_src) {
  dbl_dest->SetGeomN(flt_src->geom);
  for (int i = 0; i < dbl_dest->size; ++i) {
    dbl_dest->FastEl_Flat(i) = (double)flt_src->FastEl_Flat(i);
  }
  if(flt_src->ElView())
    dbl_dest->SetElView(flt_src->ElView(), flt_src->el_view_mode);
}

