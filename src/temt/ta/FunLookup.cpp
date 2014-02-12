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

#include "FunLookup.h"

TA_BASEFUNS_CTORS_DEFN(FunLookup);


void FunLookup::Initialize() {
  res = .001f;
  res_inv = 1.0f / res;
  x_range.min = 0.0f;
  x_range.max = 1.0f;
  x_range.UpdateAfterEdit_NoGui();
}

void FunLookup::InitLinks() {
  float_Array::InitLinks();
  taBase::Own(x_range, this);
}

void FunLookup::Copy_(const FunLookup& cp) {
  x_range = cp.x_range;
  res = cp.res;
  res_inv = cp.res_inv;
}

void FunLookup::UpdateAfterEdit_impl() {
  float_Array::UpdateAfterEdit_impl();
  res_inv = 1.0f / res;
  x_range.UpdateAfterEdit_NoGui();
}

// float FunLookup::Eval(float x) {
//   int idx = (int) floor((x - x_range.min) * res_inv);
//   if(idx < 0) return FastEl(0);
//   if(idx >= size-1) return FastEl(size-1);
//   float x_0 = x_range.min + (res * (float)idx);
//   float y_0 = FastEl(idx);
//   float	y_1 = FastEl(idx+1);
//   return y_0 + (y_1 - y_0) * ((x - x_0) * res_inv);
// }

  // the NRC polint polynomial interpolation routine using Neville's algorithm
  // works only a tiny bit better than linear for the wt_sig function in leabra
  // so not using it..
//   if((idx <= 1) || (idx == size-2)) return y_0 + (y_1 - y_0) * ((x - x_0) / res);
//   static const int order = 5;
//   static float xa[order+1];
//   static float ya[order+1];
//   static float c[order+1];
//   static float d[order+1];

//   float stx = x_0 - 2.0 * res;
//   int i;
//   for(i=1;i<=order;i++) {
//     ya[i] = FastEl(idx-3+i);
//     xa[i] = stx;
//     stx += res;
//   }

//   float dif=fabs(x-xa[1]);
//   float dift;			// x - closest point
//   int ns = 0;			// idx of closest point
//   for(i=1;i<=order;i++) {
//     if((dift=fabs(x-xa[i])) < dif) {
//       ns=i;
//       dif=dift;
//     }
//     c[i]=ya[i];
//     d[i]=ya[i];
//   }
//   float y=ya[ns--];		// initial approximation to y
//   float dy;
//   int m;
//   for(m=1;m<order;m++) {
//     for(i=1;i<=order-m;i++) {
//       float ho=xa[i]-x;
//       float hp=xa[i+m]-x;
//       float w=c[i+1]-d[i];
//       float den = w/(ho-hp);
//       d[i]=hp*den;
//       c[i]=ho*den;
//     }
//     dy=(2*ns < (order-m) ? c[ns+1] : d[ns--]);
//     y += dy;
//   }
//   return y;

  // my attempted 2nd order algorithm, doesn't work any better than linear:
//   float xd = (x - x_0) / res;
//   float firstord = y_0 + (y_1 - y_0) * xd;
//   if((idx == 0) || (idx == size-1)) return firstord;
//   float y_m = FastEl(idx-1);
//   float y_2 = FastEl(idx+2);
//   float secord = .5f * ((y_0 + (y_0 - y_m) * xd) + (y_1 - (y_2 - y_1) * (1.0 - xd)));
//   return .5f * (firstord + secord);

void FunLookup::AllocForRange() {
  // range is inclusive -- add some extra..
  UpdateAfterEdit_impl();
  int sz = (int) (x_range.range / res) + 2;
  Alloc(sz);
  size = sz;
}

void FunLookup::Plot(std::ostream& strm) {
  strm << "_H:\tx\ty\n";
  int i;
  for(i=0;i<size;i++) {
    float xval = x_range.min + ((float)i * res);
    strm << "_D:\t" << xval << "\t" << SafeEl(i) << "\n";
  }
}

void FunLookup::Convolve(const FunLookup& src, const FunLookup& con) {
  x_range = src.x_range;
  res = src.res;
  x_range.min -= con.x_range.min;
  x_range.max -= con.x_range.max;
  AllocForRange();
  int ti;
  for(ti=0; ti < size; ti++) {
    float val = 0;
    int ci;
    for(ci=0; ci<con.size; ci++)
      val += src.SafeEl(ti + ci) * con.FastEl(ci);
    FastEl(ti) = val;
  }
}
