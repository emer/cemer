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

#include "complex_Matrix.h"
#include <taMisc>

#include <cmath>

TA_BASEFUNS_CTORS_DEFN(complex_Matrix);

bool complex_Matrix::CheckComplexGeom(const MatrixGeom& gm, bool err) {
  if(gm.dims() < 2 || gm.dim(0) != 2) {
    if(!err) return false;
    taMisc::Error("CheckComplexGeom: geometry is not correct for representing complex numbers -- inner-most dimension must be size 2, and there must be 2 or more dimensions",
                  gm.PrintStr());
    return false;
  }
  return true;
}

MatrixGeom complex_Matrix::NonComplexGeom(const MatrixGeom& gm) {
  MatrixGeom ngm;
  if(!CheckComplexGeom(gm)) return ngm;
  ngm.SetDims(gm.dims() - 1); // reduce 1 dim
  for (int i = 1; i < gm.dims(); ++i) {
    ngm.Set(i-1, gm.dim(i));
  }
  return ngm;
}

MatrixGeom complex_Matrix::ComplexGeom(const MatrixGeom& gm) {
  MatrixGeom ngm;
  ngm.SetDims(gm.dims() + 1); // add 1 dim
  ngm.Set(0,2);               // inner = 2
  for (int i = 0; i < gm.dims(); ++i) {
    ngm.Set(i+1, gm.dim(i));
  }
  return ngm;
}

double_Matrix* complex_Matrix::SqMag() const {
  if(!CheckComplexGeom(geom)) return NULL;
  double_Matrix* rval = new double_Matrix(NonComplexGeom(geom));
  for(int i=0; i< rval->size; ++i) {
    double r = FastEl_Flat(2*i);
    double j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = r*r + j*j;
  }
  return rval;
}

double_Matrix* complex_Matrix::Mag() const {
  if(!CheckComplexGeom(geom)) return NULL;
  double_Matrix* rval = new double_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    double r = FastEl_Flat(2*i);
    double j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = sqrt(r*r + j*j);
  }
  return rval;
}

double_Matrix* complex_Matrix::Angle() const {
  if(!CheckComplexGeom(geom)) return NULL;
  double_Matrix* rval = new double_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    double r = FastEl_Flat(2*i);
    double j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = atan2(j, r);
  }
  return rval;
}

double_Matrix* complex_Matrix::Real() const {
  if(!CheckComplexGeom(geom)) return NULL;
  double_Matrix* rval = new double_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    double r = FastEl_Flat(2*i);
    rval->FastEl_Flat(i) = r;
  }
  return rval;
}

double_Matrix* complex_Matrix::Imag() const {
  if(!CheckComplexGeom(geom)) return NULL;
  double_Matrix* rval = new double_Matrix(NonComplexGeom(geom));
  for(int i=0; i < rval->size; ++i) {
    double j = FastEl_Flat(2*i+1);
    rval->FastEl_Flat(i) = j;
  }
  return rval;
}

complex_Matrix* complex_Matrix::Conj() const {
  if(!CheckComplexGeom(geom)) return NULL;
  complex_Matrix* rval = new complex_Matrix(this->geom);
  for(int i=0; i < size; i+=2) {
    double tr = FastEl_Flat(i);
    double tj = FastEl_Flat(i+1);
    rval->FastEl_Flat(i) = tr;
    rval->FastEl_Flat(i+1) = -tj;
  }
  return rval;
}

void complex_Matrix::SetReal(const double_Matrix& reals, bool copy_geom) {
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

void complex_Matrix::SetImag(const double_Matrix& imags, bool copy_geom) {
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

void complex_Matrix::Complex(const double_Matrix& reals, const double_Matrix& imags,
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

void complex_Matrix::Expi(const double_Matrix& angles, bool copy_geom) {
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


void complex_Matrix::SetRealAll(double real) {
  if(!CheckComplexGeom(geom)) return;
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i) = real;
  }
}

void complex_Matrix::SetImagAll(double imag) {
  if(!CheckComplexGeom(geom)) return;
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i+1) = imag;
  }
}

void complex_Matrix::ComplexAll(double real, double imag) {
  if(!CheckComplexGeom(geom)) return;
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i) = real;
    FastEl_Flat(2*i+1) = imag;
  }
}

void complex_Matrix::ExpiAll(double angle) {
  if(!CheckComplexGeom(geom)) return;
  double cosv = cos(angle);
  double sinv = sin(angle);
  int sz = size/2;
  for(int i=0; i < sz; ++i) {
    FastEl_Flat(2*i) = cosv;
    FastEl_Flat(2*i+1) = sinv;
  }
}


//////////// op *
taMatrix* complex_Matrix::operator*(const taMatrix& t) const {
  MatrixGeom ncg = NonComplexGeom(geom);
  if(t.geom == ncg) {
    if(t.GetDataValType() == VT_DOUBLE) {
      complex_Matrix* rval = new complex_Matrix(this->geom);
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        double tr = FastEl_Flat(ci);
        double tj = FastEl_Flat(ci+1);
        double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
        double nr = tr * orr;   // just straight mult both factors
        double nj = tj * orr;
        rval->FastEl_Flat(ci) = nr;
        rval->FastEl_Flat(ci+1) = nj;
      }
      return rval;
    }
    else {                      // use variants -- no need to optimize
      complex_Matrix* rval = new complex_Matrix(this->geom);
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        double tr = FastEl_Flat(ci);
        double tj = FastEl_Flat(ci+1);
        double orr = t.FastElAsVar_Flat(i).toDouble();
        double nr = tr * orr;
        double nj = tj * orr;
        rval->FastEl_Flat(ci) = nr;
        rval->FastEl_Flat(ci+1) = nj;
      }
      return rval;
    }
  }
  else if(t.geom == geom) {
    if(t.GetDataValType() == VT_DOUBLE) {
      complex_Matrix* rval = new complex_Matrix(this->geom);
      for(int i=0; i < size; i+=2) {
        double tr = FastEl_Flat(i);
        double tj = FastEl_Flat(i+1);
        double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
        double oj = ((double_Matrix*)&t)->FastEl_Flat(i+1);
        double nr = tr * orr - tj * oj;
        double nj = tr * oj + tj * orr;
        rval->FastEl_Flat(i) = nr;
        rval->FastEl_Flat(i+1) = nj;
      }
      return rval;
    }
    else {                      // use variants -- no need to optimize
      complex_Matrix* rval = new complex_Matrix(this->geom);
      for(int i=0; i < size; i+=2) {
        double tr = FastEl_Flat(i);
        double tj = FastEl_Flat(i+1);
        double orr = t.FastElAsVar_Flat(i).toDouble();
        double oj = t.FastElAsVar_Flat(i+1).toDouble();
        double nr = tr * orr - tj * oj;
        double nj = tr * oj + tj * orr;
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
taMatrix* complex_Matrix::operator/(const taMatrix& t) const {
  MatrixGeom ncg = NonComplexGeom(geom);
  if(t.geom == ncg) {
    if(t.GetDataValType() == VT_DOUBLE) {
      complex_Matrix* rval = new complex_Matrix(this->geom);
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        double tr = FastEl_Flat(ci);
        double tj = FastEl_Flat(ci+1);
        double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
        double nr = 0.0;
        double nj = 0.0;
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
      complex_Matrix* rval = new complex_Matrix(this->geom);
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        double tr = FastEl_Flat(ci);
        double tj = FastEl_Flat(ci+1);
        double orr = t.FastElAsVar_Flat(i).toDouble();
        double nr = 0.0;
        double nj = 0.0;
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
    if(t.GetDataValType() == VT_DOUBLE) {
      complex_Matrix* rval = new complex_Matrix(this->geom);
      for(int i=0; i < size; i+=2) {
        double tr = FastEl_Flat(i);
        double tj = FastEl_Flat(i+1);
        double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
        double oj = ((double_Matrix*)&t)->FastEl_Flat(i+1);
        double n = orr*orr + oj*oj;
        double nr = 0.0f;
        double nj = 0.0f;
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
      complex_Matrix* rval = new complex_Matrix(this->geom);
      for(int i=0; i < size; i+=2) {
        double tr = FastEl_Flat(i);
        double tj = FastEl_Flat(i+1);
        double orr = t.FastElAsVar_Flat(i).toDouble();
        double oj = t.FastElAsVar_Flat(i+1).toDouble();
        double n = orr*orr + oj*oj;
        double nr = 0.0f;
        double nj = 0.0f;
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
void complex_Matrix::operator*=(const taMatrix& t) {
  MatrixGeom ncg = NonComplexGeom(geom);
  if(t.geom == ncg) {
    if(t.GetDataValType() == VT_DOUBLE) {
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        double tr = FastEl_Flat(ci);
        double tj = FastEl_Flat(ci+1);
        double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
        double nr = tr * orr;
        double nj = tj * orr;
        FastEl_Flat(ci) = nr;
        FastEl_Flat(ci+1) = nj;
      }
    }
    else {                      // use variants -- no need to optimize
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        double tr = FastEl_Flat(ci);
        double tj = FastEl_Flat(ci+1);
        double orr = t.FastElAsVar_Flat(i).toDouble();
        double nr = tr * orr;
        double nj = tj * orr;
        FastEl_Flat(ci) = nr;
        FastEl_Flat(ci+1) = nj;
      }
    }
  }
  else if(t.geom == geom) {
    if(t.GetDataValType() == VT_DOUBLE) {
      for(int i=0; i < size; i++) {
        double tr = FastEl_Flat(i);
        double tj = FastEl_Flat(i+1);
        double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
        double oj = ((double_Matrix*)&t)->FastEl_Flat(i+1);
        double nr = tr * orr - tj * oj;
        double nj = tr * oj + tj * orr;
        FastEl_Flat(i) = nr;
        FastEl_Flat(i+1) = nj;
      }
    }
    else {                      // use variants -- no need to optimize
      for(int i=0; i < size; i+=2) {
        double tr = FastEl_Flat(i);
        double tj = FastEl_Flat(i+1);
        double orr = t.FastElAsVar_Flat(i).toDouble();
        double oj = t.FastElAsVar_Flat(i+1).toDouble();
        double nr = tr * orr - tj * oj;
        double nj = tr * oj + tj * orr;
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
void complex_Matrix::operator/=(const taMatrix& t) {
  MatrixGeom ncg = NonComplexGeom(geom);
  if(t.geom == ncg) {
    if(t.GetDataValType() == VT_DOUBLE) {
      for(int i=0; i < t.size; i++) {
        int ci = i*2;
        double tr = FastEl_Flat(ci);
        double tj = FastEl_Flat(ci+1);
        double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
        double nr = 0.0;
        double nj = 0.0;
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
        double tr = FastEl_Flat(ci);
        double tj = FastEl_Flat(ci+1);
        double orr = t.FastElAsVar_Flat(i).toDouble();
        double nr = 0.0;
        double nj = 0.0;
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
    if(t.GetDataValType() == VT_DOUBLE) {
      for(int i=0; i < size; i+=2) {
        double tr = FastEl_Flat(i);
        double tj = FastEl_Flat(i+1);
        double orr = ((double_Matrix*)&t)->FastEl_Flat(i);
        double oj = ((double_Matrix*)&t)->FastEl_Flat(i+1);
        double n = orr*orr + oj*oj;
        double nr = 0.0f;
        double nj = 0.0f;
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
        double tr = FastEl_Flat(i);
        double tj = FastEl_Flat(i+1);
        double orr = t.FastElAsVar_Flat(i).toDouble();
        double oj = t.FastElAsVar_Flat(i+1).toDouble();
        double n = orr*orr + oj*oj;
        double nr = 0.0f;
        double nj = 0.0f;
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

taMatrix* complex_Matrix::Transpose() const {
  if(!CheckComplexGeom(geom)) return NULL;
  if(TestError(dims() != 3, "Transpose", "can only transpose a 2D matrix"))
    return NULL;
  int d0 = dim(1);
  int d1 = dim(2);
  complex_Matrix* rval = new complex_Matrix(3,2,d1,d0);
  for(int i=0;i<d0;i++) {
    for(int j=0;j<d1;j++) {
      rval->FastEl3d(0,j,i) = this->FastEl3d(0,i,j);
      rval->FastEl3d(1,j,i) = this->FastEl3d(1,i,j);
    }
  }
  return rval;
}
