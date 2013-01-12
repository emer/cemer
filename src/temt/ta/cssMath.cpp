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

#include "cssMath.h"
#include <double_Matrix>
#include <int_Matrix>
#include <slice_Matrix>
#include <complex_Matrix>
#include <byte_Matrix>
#include <MatrixIndex>
#include <Random>
#include <taMisc>


int cssMath::ndims(const taMatrix* cr) {
  if(!cr) {
    taMisc::Error("ndims -- input matrix NULL");
    return 0;
  }
  return cr->dims();
}

int_Matrix* cssMath::shape(const taMatrix* cr) {
  if(!cr) {
    taMisc::Error("shape -- input matrix NULL");
    return NULL;
  }
  return cr->Shape();
}

taMatrix* cssMath::flatten(const taMatrix* mat) {
  if(!mat) {
    taMisc::Error("flatten -- matrix is NULL");
    return NULL;
  }
  return mat->Flatten();
}

double_Matrix* cssMath::zeros(const int_Matrix* shape) {
  if(!shape || shape->size < 1) {
    taMisc::Error("zeros -- shape matrix NULL or does not have at least 1 entry");
    return NULL;
  }
  double_Matrix* rval = new double_Matrix;
  rval->SetShape(shape);	// automatically zeros
  return rval;
}

double_Matrix* cssMath::ones(const int_Matrix* shape) {
  if(!shape || shape->size < 1) {
    taMisc::Error("ones -- shape matrix NULL or does not have at least 1 entry");
    return NULL;
  }
  double_Matrix* rval = new double_Matrix;
  rval->SetShape(shape);	// automatically zeros
  rval->InitVals(1.0);
  return rval;
}

double_Matrix* cssMath::eye(int size) {
  if(size < 1) {
    taMisc::Error("eye -- size must be >= 1");
    return NULL;
  }
  double_Matrix* rval = new double_Matrix;
  rval->SetGeom(2, size, size);
  for(int i=0;i<size; i++) {
    rval->FastEl(i,i) = 1.0;
  }
  return rval;
}

double_Matrix* cssMath::rand(const int_Matrix* shape) {
  if(!shape || shape->size < 1) {
    taMisc::Error("rand -- shape matrix NULL or does not have at least 1 entry");
    return NULL;
  }
  double_Matrix* rval = new double_Matrix;
  rval->SetShape(shape);	// automatically zeros
  for(int i=0;i<rval->size;i++) {
    rval->FastEl_Flat(i) = Random::ZeroOne();
  }
  return rval;
}

double_Matrix* cssMath::diag(const double_Matrix* mat, int mat_zero) {
  if(!vec_check_type(mat)) return NULL;
  if(mat_zero == 0) {
    double_Matrix* rval = new double_Matrix;
    rval->SetGeom(2, mat->size, mat->size);
    for(int i=0;i<mat->size; i++) {
      rval->FastEl(i,i) = mat->FastEl_Flat(i);
    }
    return rval;
  }
  else {
    if(mat->dim(0) != mat->dim(1)) {
      taMisc::Error("diag -- input matrix is not a square matrix");
      return NULL;
    }
    double_Matrix* rval = new double_Matrix;
    int sz = mat->dim(0);
    rval->SetGeom(1, sz);
    for(int i=0;i<sz; i++) {
      rval->FastEl_Flat(i) = mat->FastEl(i,i);
    }
    return rval;
  }
}

double_Matrix* cssMath::linspace(double start, double end, int n_vals) {
  if(n_vals <= 2) n_vals = 2;
  if(end <= start) {
    taMisc::Error("linspace -- end must be > start");
    return NULL;
  }
  double_Matrix* rval = new double_Matrix;
  rval->SetGeom(1, n_vals);
  double incr = (end - start) / (n_vals-1);
  for(int i=0;i<n_vals; i++) {
    rval->FastEl_Flat(i) = start + (double)i * incr;
  }
  return rval;
}

double_Matrix* cssMath::meshgrid(const slice_Matrix* dims) {
  if(!dims || dims->dims() != 2 || dims->dim(0) != 3) {
    taMisc::Error("meshgrid -- dims matrix NULL or is not properly configured slice matrix with 2 dimensions, inner = 3 (start, stop, end) and outer = number of dimensions");
    return NULL;
  }
  int_Matrix fixsmat(*dims);
  int dm = dims->dim(1);
  MatrixGeom sliceg(dm);		// slice geometry
  for(int i=0; i < dm; i++) {
    int start = dims->FastEl(0,i);
    int end = dims->FastEl(1,i);
    int step = dims->FastEl(2,i);
    if(end <= start) end = start + 100;
    if(step == 0) step = 1;
    int my_n = (end-start) / ABS(step); // number of guys in my slice
    sliceg.Set(i, my_n);
    fixsmat.FastEl(0,i) = start; 
    fixsmat.FastEl(1,i) = end; 
    fixsmat.FastEl(2,i) = step;
  }
  int tot_n = sliceg.Product();
  sliceg.AddDim(dm);		// one more outer dim to hold each coord set
  double_Matrix* rval = new double_Matrix(sliceg);

  MatrixIndex sidx;				   // slice idx
  for(int i=0;i<tot_n; i++) {
    sliceg.DimsFmIndex(i, sidx); // get index into slice vals
    for(int d=0; d<dm; d++) {
      int start = fixsmat.FastEl(0,d);
      int end = fixsmat.FastEl(1,d);
      int step = fixsmat.FastEl(2,d);
      int sc;
      if(step > 0) {
	sc = start + step * sidx[d];
      }
      else {
	sc = end-1 + step * sidx[d];
      }
      sidx[dm] = d;
      rval->FastElN(sidx) = (double)sc;
    }
  }
  return rval;
}

double_Matrix* cssMath::mat_mult_css(const double_Matrix* a, const double_Matrix* b) {
  double_Matrix* rval = new double_Matrix;
  mat_mult(rval, a, b);
  return rval;
}

double_Matrix* cssMath::transpose(const double_Matrix* a) {
  if(!a) { taMisc::Error("transpose -- matrix is NULL"); return NULL; }
  if(a->InheritsFrom(&TA_complex_Matrix)) {
    complex_Matrix* rval = new complex_Matrix;
    mat_transpose(rval, a);
    return rval;
  }
  double_Matrix* rval = new double_Matrix;
  mat_transpose(rval, a);
  return rval;
}

int_Matrix* cssMath::find(const byte_Matrix* mat) {
  if(!mat) {
    taMisc::Error("find -- matrix is NULL");
    return NULL;
  }
  return mat->Find();
}

double_Matrix* cssMath::angle(const complex_Matrix* cmat) {
  if(!cmat || !cmat->InheritsFrom(&TA_complex_Matrix)) {
    taMisc::Error("angle -- input matrix is NULL or not a complex_Matrix type");
    return NULL;
  }
  return cmat->Angle();
}

double_Matrix* cssMath::real(const complex_Matrix* cmat) {
  if(!cmat || !cmat->InheritsFrom(&TA_complex_Matrix)) {
    taMisc::Error("real -- input matrix is NULL or not a complex_Matrix type");
    return NULL;
  }
  return cmat->Real();
}

double_Matrix* cssMath::imag(const complex_Matrix* cmat) {
  if(!cmat || !cmat->InheritsFrom(&TA_complex_Matrix)) {
    taMisc::Error("imag -- input matrix is NULL or not a complex_Matrix type");
    return NULL;
  }
  return cmat->Imag();
}

complex_Matrix* cssMath::conj(const complex_Matrix* cmat) {
  if(!cmat || !cmat->InheritsFrom(&TA_complex_Matrix)) {
    taMisc::Error("conj -- input matrix is NULL or not a complex_Matrix type");
    return NULL;
  }
  return cmat->Conj();
}

complex_Matrix* cssMath::complex(const double_Matrix* reals, const double_Matrix* imags) {
  if(!vec_check_type(reals) || !vec_check_type(imags)) return NULL;
  complex_Matrix* rval = new complex_Matrix;
  rval->Complex(*reals, *imags, true); // set geom
  return rval;
}

complex_Matrix* cssMath::expi(const double_Matrix* angles) {
  if(!vec_check_type(angles)) return NULL;
  complex_Matrix* rval = new complex_Matrix;
  rval->Expi(*angles, true); // set geom
  return rval;
}

complex_Matrix* cssMath::fft(const double_Matrix* in_mat) {
  if(!vec_check_type(in_mat)) return NULL;
  complex_Matrix* rval = new complex_Matrix;
  if(in_mat->InheritsFrom(&TA_complex_Matrix)) {
    rval->Copy(in_mat);
    fft_complex(rval);
  }
  else {
    fft_real(rval, in_mat);
  }
  return rval;
}

complex_Matrix* cssMath::ffti(const complex_Matrix* in_mat) {
  if(!vec_check_type(in_mat)) return NULL;
  complex_Matrix* rval = new complex_Matrix;
  rval->Copy(in_mat);
  ffti_complex(rval);
  return rval;
}

complex_Matrix* cssMath::fft2(const double_Matrix* in_mat) {
  if(!vec_check_type(in_mat)) return NULL;
  complex_Matrix* rval = new complex_Matrix;
  if(in_mat->InheritsFrom(&TA_complex_Matrix)) {
    rval->Copy(in_mat);
    fft2_complex(rval);
  }
  else {
    fft2_real(rval, in_mat);
  }
  return rval;
}

complex_Matrix* cssMath::ffti2(const complex_Matrix* in_mat) {
  if(!vec_check_type(in_mat)) return NULL;
  complex_Matrix* rval = new complex_Matrix;
  rval->Copy(in_mat);
  ffti2_complex(rval);
  return rval;
}
