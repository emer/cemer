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

#include "complex_float_Matrix.h"
#include <taMisc>

#include <cmath>

TA_BASEFUNS_CTORS_DEFN(complex_float_Matrix);

bool complex_float_Matrix::CheckComplexGeom(const MatrixGeom& gm, bool err) {
  if(gm.dims() < 2 || gm.dim(0) != 2) {
    if(!err) return false;
    taMisc::Error("CheckComplexGeom: geometry is not correct for representing complex numbers -- inner-most dimension must be size 2, and there must be 2 or more dimensions",
                  gm.PrintStr());
    return false;
  }
  return true;
}

MatrixGeom complex_float_Matrix::NonComplexGeom(const MatrixGeom& gm) {
  MatrixGeom ngm;
  if(!CheckComplexGeom(gm)) return ngm;
  ngm.SetDims(gm.dims() - 1); // reduce 1 dim
  for (int i = 1; i < gm.dims(); ++i) {
    ngm.Set(i-1, gm.dim(i));
  }
  return ngm;
}

MatrixGeom complex_float_Matrix::ComplexGeom(const MatrixGeom& gm) {
  MatrixGeom ngm;
  ngm.SetDims(gm.dims() + 1); // add 1 dim
  ngm.Set(0,2);               // inner = 2
  for (int i = 0; i < gm.dims(); ++i) {
    ngm.Set(i+1, gm.dim(i));
  }
  return ngm;
}

float_Matrix* complex_float_Matrix::SqMag() const {
  if(!CheckComplexGeom(geom)) return NULL;
  float_Matrix* rval = new float_Matrix(NonComplexGeom(geom));
  for(int i=0; i< rval->size; ++i) {
    float r = FastEl_Flat(2*i);
    float j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = r*r + j*j;
  }
  return rval;
}

float_Matrix* complex_float_Matrix::Mag() const {
  if(!CheckComplexGeom(geom)) return NULL;
  float_Matrix* rval = new float_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    float r = FastEl_Flat(2*i);
    float j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = sqrt(r*r + j*j);
  }
  return rval;
}

float_Matrix* complex_float_Matrix::Angle() const {
  if(!CheckComplexGeom(geom)) return NULL;
  float_Matrix* rval = new float_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    float r = FastEl_Flat(2*i);
    float j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = atan2(j, r);
  }
  return rval;
}

float_Matrix* complex_float_Matrix::Real() const {
  if(!CheckComplexGeom(geom)) return NULL;
  float_Matrix* rval = new float_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    float r = FastEl_Flat(2*i);
    rval->FastEl_Flat(i) = r;
  }
  return rval;
}

float_Matrix* complex_float_Matrix::Imag() const {
  if(!CheckComplexGeom(geom)) return NULL;
  float_Matrix* rval = new float_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    float j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = j;
  }
  return rval;
}

complex_float_Matrix* complex_float_Matrix::Conj() const {
  if(!CheckComplexGeom(geom)) return NULL;
  complex_float_Matrix* rval = new complex_float_Matrix(this->geom);
  for(int i=0; i < size; i+=2) {
    float tr = FastEl_Flat(i);
    float tj = FastEl_Flat(i+1);
    rval->FastEl_Flat(i) = tr;
    rval->FastEl_Flat(i+1) = -tj;
  }
  return rval;
}

void complex_float_Matrix::SetReal(const float_Matrix& reals, bool copy_geom) {
  MatrixGeom cgm = ComplexGeom(reals.geom);
  if(copy_geom) {
    SetGeomN(cgm);
  }
  else {
    if(!CheckComplexGeom(geom)) return;
    if(TestError(cgm != geom, "SetReal", "input matrix does not have same geometry as destination matrix",reals.geom.PrintStr()))
      return;
  }
  for(int i=0; i < reals.size; ++i) {
    FastEl_Flat(2*i) = reals.FastEl_Flat(i);
  }
}

void complex_float_Matrix::SetImag(const float_Matrix& imags, bool copy_geom) {
  MatrixGeom cgm = ComplexGeom(imags.geom);
  if(copy_geom) {
    SetGeomN(cgm);
  }
  else {
    if(!CheckComplexGeom(geom)) return;
    if(TestError(cgm != geom, "SetImag", "input matrix does not have same geometry as destination matrix",imags.geom.PrintStr()))
      return;
  }
  for(int i=0; i < imags.size; ++i) {
    FastEl_Flat(2*i+1) = imags.FastEl_Flat(i);
  }
}

void complex_float_Matrix::Complex(const float_Matrix& reals, const float_Matrix& imags,
                             bool copy_geom) {
  if(TestError(reals.geom != imags.geom, "Complex", "input reals and imags matricies do not have same geometry.  reals:",reals.geom.PrintStr(), "imags:", imags.geom.PrintStr()))
    return;
  MatrixGeom cgm = ComplexGeom(reals.geom);
  if(copy_geom) {
    SetGeomN(cgm);
  }
  else {
    if(!CheckComplexGeom(geom)) return;
    if(TestError(cgm != geom, "Complex", "input matrices do not have same geometry as destination matrix",reals.geom.PrintStr()))
      return;
  }
  for(int i=0; i < reals.size; ++i) {
    FastEl_Flat(2*i) = reals.FastEl_Flat(i);
    FastEl_Flat(2*i+1) = imags.FastEl_Flat(i);
  }
}

void complex_float_Matrix::Expi(const float_Matrix& angles, bool copy_geom) {
  MatrixGeom cgm = ComplexGeom(angles.geom);
  if(copy_geom) {
    SetGeomN(cgm);
  }
  else {
    if(!CheckComplexGeom(geom)) return;
    if(TestError(cgm != geom, "Expi", "input matrix does not have same geometry as destination matrix",angles.geom.PrintStr()))
      return;
  }
  for(int i=0; i < angles.size; ++i) {
    FastEl_Flat(2*i) = cos(angles.FastEl_Flat(i));
    FastEl_Flat(2*i+1) = sin(angles.FastEl_Flat(i));
  }
}


void complex_float_Matrix::SetRealAll(float real) {
  if(!CheckComplexGeom(geom)) return;
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i) = real;
  }
}

void complex_float_Matrix::SetImagAll(float imag) {
  if(!CheckComplexGeom(geom)) return;
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i+1) = imag;
  }
}

void complex_float_Matrix::ComplexAll(float real, float imag) {
  if(!CheckComplexGeom(geom)) return;
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i) = real;
    FastEl_Flat(2*i+1) = imag;
  }
}

void complex_float_Matrix::ExpiAll(float angle) {
  if(!CheckComplexGeom(geom)) return;
  float cosv = cos(angle);
  float sinv = sin(angle);
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i) = cosv;
    FastEl_Flat(2*i+1) = sinv;
  }
}


//////////// op *
taMatrix* complex_float_Matrix::operator*(const taMatrix& t) const {
  MatrixGeom ncg = NonComplexGeom(geom);
  if(t.geom == ncg) {
    if(t.GetDataValType() == VT_FLOAT) {
      complex_float_Matrix* rval = new complex_float_Matrix(this->geom);
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        float tr = FastEl_Flat(ci);
        float tj = FastEl_Flat(ci+1);
        float orr = ((float_Matrix*)&t)->FastEl_Flat(i);
        float nr = tr * orr;   // just straight mult both factors
        float nj = tj * orr;
        rval->FastEl_Flat(ci) = nr;
        rval->FastEl_Flat(ci+1) = nj;
      }
      return rval;
    }
    else {                      // use variants -- no need to optimize
      complex_float_Matrix* rval = new complex_float_Matrix(this->geom);
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        float tr = FastEl_Flat(ci);
        float tj = FastEl_Flat(ci+1);
        float orr = t.FastElAsVar_Flat(i).toFloat();
        float nr = tr * orr;
        float nj = tj * orr;
        rval->FastEl_Flat(ci) = nr;
        rval->FastEl_Flat(ci+1) = nj;
      }
      return rval;
    }
  }
  else if(t.geom == geom) {
    if(t.GetDataValType() == VT_FLOAT) {
      complex_float_Matrix* rval = new complex_float_Matrix(this->geom);
      for(int i=0; i < size; i+=2) {
        float tr = FastEl_Flat(i);
        float tj = FastEl_Flat(i+1);
        float orr = ((float_Matrix*)&t)->FastEl_Flat(i);
        float oj = ((float_Matrix*)&t)->FastEl_Flat(i+1);
        float nr = tr * orr - tj * oj;
        float nj = tr * oj + tj * orr;
        rval->FastEl_Flat(i) = nr;
        rval->FastEl_Flat(i+1) = nj;
      }
      return rval;
    }
    else {                      // use variants -- no need to optimize
      complex_float_Matrix* rval = new complex_float_Matrix(this->geom);
      for(int i=0; i < size; i+=2) {
        float tr = FastEl_Flat(i);
        float tj = FastEl_Flat(i+1);
        float orr = t.FastElAsVar_Flat(i).toFloat();
        float oj = t.FastElAsVar_Flat(i+1).toFloat();
        float nr = tr * orr - tj * oj;
        float nj = tr * oj + tj * orr;
        rval->FastEl_Flat(i) = nr;
        rval->FastEl_Flat(i+1) = nj;
      }
      return rval;
    }
  }
  else {
    TestError(true, "*", "the geometry of the two matricies is not equal -- must be for element-wise operation");
  }
  return NULL;
}

//////////// op /
taMatrix* complex_float_Matrix::operator/(const taMatrix& t) const {
  MatrixGeom ncg = NonComplexGeom(geom);
  if(t.geom == ncg) {
    if(t.GetDataValType() == VT_FLOAT) {
      complex_float_Matrix* rval = new complex_float_Matrix(this->geom);
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        float tr = FastEl_Flat(ci);
        float tj = FastEl_Flat(ci+1);
        float orr = ((float_Matrix*)&t)->FastEl_Flat(i);
        float nr = 0.0;
        float nj = 0.0;
        if(!TestError(orr == 0.0, "/", "Floating Point Exception: Division by Zero")) {
          nr = tr / orr;        // just straight mult both factors
          nj = tj / orr;
        }
        rval->FastEl_Flat(ci) = nr;
        rval->FastEl_Flat(ci+1) = nj;
      }
      return rval;
    }
    else {                      // use variants -- no need to optimize
      complex_float_Matrix* rval = new complex_float_Matrix(this->geom);
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        float tr = FastEl_Flat(ci);
        float tj = FastEl_Flat(ci+1);
        float orr = t.FastElAsVar_Flat(i).toFloat();
        float nr = 0.0;
        float nj = 0.0;
        if(!TestError(orr == 0.0, "/", "Floating Point Exception: Division by Zero")) {
          nr = tr / orr;        // just straight mult both factors
          nj = tj / orr;
        }
        rval->FastEl_Flat(ci) = nr;
        rval->FastEl_Flat(ci+1) = nj;
      }
      return rval;
    }
  }
  else if(t.geom == geom) {
    if(t.GetDataValType() == VT_FLOAT) {
      complex_float_Matrix* rval = new complex_float_Matrix(this->geom);
      for(int i=0; i < size; i+=2) {
        float tr = FastEl_Flat(i);
        float tj = FastEl_Flat(i+1);
        float orr = ((float_Matrix*)&t)->FastEl_Flat(i);
        float oj = ((float_Matrix*)&t)->FastEl_Flat(i+1);
        float n = orr*orr + oj*oj;
        float nr = 0.0f;
        float nj = 0.0f;
        if(!TestError(n == 0.0, "/", "Floating Point Exception: Division by Zero")) {
          nr = (tr * orr + tj * oj) / n;
          nj = (tj * orr - tr * oj) / n;
        }
        rval->FastEl_Flat(i) = nr;
        rval->FastEl_Flat(i+1) = nj;
      }
      return rval;
    }
    else {                      // use variants -- no need to optimize
      complex_float_Matrix* rval = new complex_float_Matrix(this->geom);
      for(int i=0; i < size; i+=2) {
        float tr = FastEl_Flat(i);
        float tj = FastEl_Flat(i+1);
        float orr = t.FastElAsVar_Flat(i).toFloat();
        float oj = t.FastElAsVar_Flat(i+1).toFloat();
        float n = orr*orr + oj*oj;
        float nr = 0.0f;
        float nj = 0.0f;
        if(!TestError(n == 0.0, "/", "Floating Point Exception: Division by Zero")) {
          nr = (tr * orr + tj * oj) / n;
          nj = (tj * orr - tr * oj) / n;
        }
        rval->FastEl_Flat(i) = nr;
        rval->FastEl_Flat(i+1) = nj;
      }
      return rval;
    }
  }
  else {
    TestError(true, "/", "the geometry of the two matricies is not equal -- must be for element-wise operation");
  }
  return NULL;
}

//////////// op *=
void complex_float_Matrix::operator*=(const taMatrix& t) {
  MatrixGeom ncg = NonComplexGeom(geom);
  if(t.geom == ncg) {
    if(t.GetDataValType() == VT_FLOAT) {
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        float tr = FastEl_Flat(ci);
        float tj = FastEl_Flat(ci+1);
        float orr = ((float_Matrix*)&t)->FastEl_Flat(i);
        float nr = tr * orr;
        float nj = tj * orr;
        FastEl_Flat(ci) = nr;
        FastEl_Flat(ci+1) = nj;
      }
    }
    else {                      // use variants -- no need to optimize
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        float tr = FastEl_Flat(ci);
        float tj = FastEl_Flat(ci+1);
        float orr = t.FastElAsVar_Flat(i).toFloat();
        float nr = tr * orr;
        float nj = tj * orr;
        FastEl_Flat(ci) = nr;
        FastEl_Flat(ci+1) = nj;
      }
    }
  }
  else if(t.geom == geom) {
    if(t.GetDataValType() == VT_FLOAT) {
      for(int i=0; i < size; i++) {
        float tr = FastEl_Flat(i);
        float tj = FastEl_Flat(i+1);
        float orr = ((float_Matrix*)&t)->FastEl_Flat(i);
        float oj = ((float_Matrix*)&t)->FastEl_Flat(i+1);
        float nr = tr * orr - tj * oj;
        float nj = tr * oj + tj * orr;
        FastEl_Flat(i) = nr;
        FastEl_Flat(i+1) = nj;
      }
    }
    else {                      // use variants -- no need to optimize
      for(int i=0; i < size; i+=2) {
        float tr = FastEl_Flat(i);
        float tj = FastEl_Flat(i+1);
        float orr = t.FastElAsVar_Flat(i).toFloat();
        float oj = t.FastElAsVar_Flat(i+1).toFloat();
        float nr = tr * orr - tj * oj;
        float nj = tr * oj + tj * orr;
        FastEl_Flat(i) = nr;
        FastEl_Flat(i+1) = nj;
      }
    }
  }
  else {
    TestError(true, "*", "the geometry of the two matricies is not equal -- must be for element-wise operation");
  }
}

//////////// op /
void complex_float_Matrix::operator/=(const taMatrix& t) {
  MatrixGeom ncg = NonComplexGeom(geom);
  if(t.geom == ncg) {
    if(t.GetDataValType() == VT_FLOAT) {
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        float tr = FastEl_Flat(ci);
        float tj = FastEl_Flat(ci+1);
        float orr = ((float_Matrix*)&t)->FastEl_Flat(i);
        float nr = 0.0;
        float nj = 0.0;
        if(!TestError(orr == 0.0, "/", "Floating Point Exception: Division by Zero")) {
          nr = tr / orr;        // just straight mult both factors
          nj = tj / orr;
        }
        FastEl_Flat(ci) = nr;
        FastEl_Flat(ci+1) = nj;
      }
    }
    else {                      // use variants -- no need to optimize
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        float tr = FastEl_Flat(ci);
        float tj = FastEl_Flat(ci+1);
        float orr = t.FastElAsVar_Flat(i).toFloat();
        float nr = 0.0;
        float nj = 0.0;
        if(!TestError(orr == 0.0, "/", "Floating Point Exception: Division by Zero")) {
          nr = tr / orr;        // just straight mult both factors
          nj = tj / orr;
        }
        FastEl_Flat(ci) = nr;
        FastEl_Flat(ci+1) = nj;
      }
    }
  }
  else if(t.geom == geom) {
    if(t.GetDataValType() == VT_FLOAT) {
      for(int i=0; i < size; i+=2) {
        float tr = FastEl_Flat(i);
        float tj = FastEl_Flat(i+1);
        float orr = ((float_Matrix*)&t)->FastEl_Flat(i);
        float oj = ((float_Matrix*)&t)->FastEl_Flat(i+1);
        float n = orr*orr + oj*oj;
        float nr = 0.0f;
        float nj = 0.0f;
        if(!TestError(n == 0.0, "/", "Floating Point Exception: Division by Zero")) {
          nr = (tr * orr + tj * oj) / n;
          nj = (tj * orr - tr * oj) / n;
        }
        FastEl_Flat(i) = nr;
        FastEl_Flat(i+1) = nj;
      }
    }
    else {                      // use variants -- no need to optimize
      for(int i=0; i < size; i+=2) {
        float tr = FastEl_Flat(i);
        float tj = FastEl_Flat(i+1);
        float orr = t.FastElAsVar_Flat(i).toFloat();
        float oj = t.FastElAsVar_Flat(i+1).toFloat();
        float n = orr*orr + oj*oj;
        float nr = 0.0f;
        float nj = 0.0f;
        if(!TestError(n == 0.0, "/", "Floating Point Exception: Division by Zero")) {
          nr = (tr * orr + tj * oj) / n;
          nj = (tj * orr - tr * oj) / n;
        }
        FastEl_Flat(i) = nr;
        FastEl_Flat(i+1) = nj;
      }
    }
  }
  else {
    TestError(true, "/", "the geometry of the two matricies is not equal -- must be for element-wise operation");
  }
}

taMatrix* complex_float_Matrix::Transpose() const {
  if(!CheckComplexGeom(geom)) return NULL;
  if(TestError(dims() != 3, "Transpose", "can only transpose a 2D matrix"))
    return NULL;
  int d0 = dim(1);
  int d1 = dim(2);
  complex_float_Matrix* rval = new complex_float_Matrix(3,2,d1,d0);
  for(int i=0;i<d0;i++) {
    for(int j=0;j<d1;j++) {
      rval->FastEl3d(0,j,i) = this->FastEl3d(0,i,j);
      rval->FastEl3d(1,j,i) = this->FastEl3d(1,i,j);
    }
  }
  return rval;
}
