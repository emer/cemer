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

#ifndef complex_float_Matrix_h
#define complex_float_Matrix_h 1

// parent includes:
#include <float_Matrix>

// member includes:

// declare all other types mentioned but not required to include:
class taMatrix; // 
class MatrixGeom; // 


taTypeDef_Of(complex_float_Matrix);

class TA_API complex_float_Matrix: public float_Matrix {
  // #INSTANCE matrix of complex numbers -- inner-most dimension is always size 2 and contains real, imag number pair
INHERITED(float_Matrix)
public:

  static bool           CheckComplexGeom(const MatrixGeom& gm, bool err = true);
  // #CAT_Complex check to make sure given geometry is correct for representing complex numbers (inner-most dimension size = 2, 2+ dims overall)
  static MatrixGeom     NonComplexGeom(const MatrixGeom& gm);
  // #CAT_Complex #IGNORE return the equivalent geometry for a non-complex matrix of the same size as given geometry (i.e., remove the inner-most dimension)
  static MatrixGeom     ComplexGeom(const MatrixGeom& gm);
  // #CAT_Complex #IGNORE return the equivalent geometry for complex matrix of the same size as given geometry (i.e., add the inner-most dimension)

  virtual  float_Matrix* SqMag() const;
  // #CAT_Complex return a new float matrix that contains the squared magnitudes of the complex numbers in this matrix (real * real + imag * imag)
  virtual  float_Matrix* Mag() const;
  // #CAT_Complex return a new float matrix that contains the magnitudes of the complex numbers in this matrix: sqrt(real * real + imag * imag) -- otherwise known as the complex modulus or the absolute value of the complex number
  taMatrix*     Abs() const override { return Mag(); }
  // #CAT_Complex return a new float matrix that contains the absolute values or magnitudes of the complex numbers in this matrix: sqrt(real * real + imag * imag) (also called the modulus)
  virtual  float_Matrix* Angle() const;
  // #CAT_Complex return a new float matrix that contains the angles of the complex numbers in this matrix: atan2(imag, real) -- see Expi function for inverse, which takes angles and produces cos(ang) + i sin(ang)
  virtual  float_Matrix* Real() const;
  // #CAT_Complex return a new float matrix that contains the real-valued elements from this complex matrix
  virtual  float_Matrix* Imag() const;
  // #CAT_Complex return a new float matrix that contains the imaginary-valued elements from this complex matrix
  virtual  complex_float_Matrix* Conj() const;
  // #CAT_Complex return a new complex matrix that contains the complex conjugate of this complex matrix, where the imaginary component is replaced with its negative (i.e., real - imag)

  virtual  void         Complex(const float_Matrix& reals, const float_Matrix& imags,
                                bool copy_geom=true);
  // #CAT_Complex set both the real and imaginary components of this complex matrix from source matricies, which both must have the same geometry, and if copy_geom then we set our geometry based on that geometry
  virtual  void         SetReal(const float_Matrix& reals, bool copy_geom=true);
  // #CAT_Complex set the real-valued components of this complex matrix from given matrix of real values -- if copy_geom then we set our geometry based on the reals source matrix
  virtual  void         SetImag(const float_Matrix& imags, bool copy_geom=true);
  // #CAT_Complex set the imaginary-valued components of this complex matrix from given matrix of imaginary values -- if copy_geom then we set our geometry based on the reals source matrix
  virtual  void         Expi(const float_Matrix& angles, bool copy_geom=true);
  // #CAT_Complex sets complex numbers in this matrix from angles in input matrix, using exponential of i * angle = cos(angle) + i sin(angle) (Euler's formula)

  virtual  void         ComplexAll(float real, float imag);
  // #CAT_Complex set both the real and imaginary components of this complex matrix from source values -- initializes all matrix values to the same numbers
  virtual  void         SetRealAll(float real);
  // #CAT_Complex set the real-valued components of this complex matrix from given real value -- initializes all matrix values to the same number
  virtual  void         SetImagAll(float real);
  // #CAT_Complex set the imaginary-valued components of this complex matrix from given imaginary value -- initializes all matrix values to the same number
  virtual  void         ExpiAll(float angle);
  // #CAT_Complex sets complex numbers in this matrix from angle in input matrix, using exponential of i * angle = cos(angle) + i sin(angle) (Euler's formula) -- initializes all matrix values to the same numbers

  taMatrix* Transpose() const override;

  using taMatrix::operator*;
  taMatrix* operator*(const taMatrix& t) const override;

  using taMatrix::operator/;
  taMatrix* operator/(const taMatrix& t) const override;

  using taMatrix::operator*=;
  void      operator*=(const taMatrix& t) override;

  using taMatrix::operator/=;
  void      operator/=(const taMatrix& t) override;

  TA_MATRIX_FUNS_DERIVED(complex_float_Matrix, float);
private:
  void          Initialize() {}
  void          Destroy() {}
};


#endif // complex_float_Matrix_h
