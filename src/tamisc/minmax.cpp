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



// minmax.cc

#include "minmax.h"
#include "ta_matrix.h"

/* from xmgr, graphutils.c, copyright P. Turner
 * nicenum: find a "nice" number approximately equal to x
 * round if round=true, ceil if round=false
 */

double nicenum(double x, bool round) {
  double y;
  if(x <= 0.0)
     return 0.0;
  int exp = (int)floor(log10(x));
  double f = x / pow(10.0, (double) exp);	/* fraction between 1 and 10 */
  if (round)
    if (f < 1.5)
      y = 1.;
    else if (f < 3.)
      y = 2.;
    else if (f < 7.)
      y = 5.;
    else
      y = 10.;
  else if (f <= 1.)
    y = 1.;
  else if (f <= 2.)
    y = 2.;
  else if (f <= 5.)
    y = 5.;
  else
    y = 10.;
  return y * pow(10.0, (double)exp);
}

void MinMax::SetRange(float_Matrix& mat) {
  if (mat.size == 0) {
    Init(0.0f);
  } else {
    Init(mat.el[0]);
    for (int i = 1; i < mat.size; ++i)
      UpdateRange(mat.el[i]);
  }
} 


void FixedMinMax::Initialize() {
  min = max = 0.0f;
  fix_min = fix_max = false;
}

void FixedMinMax::Copy_(const FixedMinMax& cp) {
  min = cp.min; max = cp.max;
  fix_min = cp.fix_min; fix_max = cp.fix_max;
}

void Modulo::Initialize() {
  flag = true; m = 1; off = 0;
}

void Modulo::UpdateAfterEdit() {
  taOBase::UpdateAfterEdit();
  if(owner != NULL) owner->UpdateAfterEdit();
}

void Modulo::Copy_(const Modulo& cp) {
  flag = cp.flag; m = cp.m; off = cp.off;
}

