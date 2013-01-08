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

#ifndef cssMath_h
#define cssMath_h 1

// parent includes:
#include <taMath_double>

// member includes:

// declare all other types mentioned but not required to include:
class taMatrix; // 
class int_Matrix; // 
class double_Matrix; // 
class slice_Matrix; // 
class byte_Matrix; // 
class complex_Matrix; // 


class cssMath : public taMath_double {
  // css specific math operations, including all of taMath_double -- this is parsed to provide css math functionality
INHERITED(taMath_double)
public:
  // extra functions that return matrix objects (standard for taMath is to always pass matrix objects as args, which avoids all the associated memory management issues)

  static int ndims(const taMatrix* mat);
  // #CAT_Matrix returns number of dimensions of given matrix
  static int ndim(const taMatrix* mat) { return ndims(mat); }
  // #CAT_Matrix returns number of dimensions of given matrix
  static int_Matrix* shape(const taMatrix* mat);
  // #CAT_Matrix returns shape (geometry, size along each dimension) of the matrix
  static taMatrix* flatten(const taMatrix* mat);
  // #CAT_Matrix returns elements of matrix, filtered according to current view, as a 1D vector list of values
  static double_Matrix* zeros(const int_Matrix* shape);
  // #CAT_Matrix returns a matrix of geometry (shape) given by shape argument (containing x,y,z,... dimension sizes) of floating point numbers all initialized to 0
  static double_Matrix* ones(const int_Matrix* shape);
  // #CAT_Matrix returns a matrix of geometry (shape) given by shape argument (containing x,y,z,... dimension sizes) of floating point numbers all initialized to 1
  static double_Matrix* eye(int size);
  // #CAT_Matrix returns a square 2D matrix of given size of floating point numbers, initialized to the identity matrix (1s along the diagonal, 0's elsewhere)
  static double_Matrix* rand(const int_Matrix* shape);
  // #CAT_Matrix returns a matrix of geometry (shape) given by shape argument (containing x,y,z,... dimension sizes) of floating point numbers all initialized to uniform random numbers between 0 and 1, exclusive of 1
  static double_Matrix* diag(const double_Matrix* mat, int mat_zero=-1);
  // #CAT_Matrix if mat_zero == 0, then returns a 2D square diagonal matrix whose diagonal elements are all the elements of the given matrix (treated like a simple vector of numbers), otherwise returns a 1D matrix (vector) containing the diagonal elements of given matrix
  static double_Matrix* linspace(double start, double end, int n_vals);
  // #CAT_Matrix returns a 1D matrix (vector) containing n_vals linearly spaced values between start and end
  static double_Matrix* meshgrid(const slice_Matrix* dims);
  // #CAT_Matrix returns an n-dimensional matrix from slice inputs that specify start, end, and increment spacing for values along each dimension -- defaults for unspecified slice values are start=0:end=start+100:step=1
  static double_Matrix* transpose(const double_Matrix* a);
  // #CAT_Matrix returns a new matrix that is a transpose of the given 2D matrix (works for complex matricies too)
  static double_Matrix* mat_mult_css(const double_Matrix* a, const double_Matrix* b);
  // #CAT_Matrix #LABEL_mat_mult matrix multiplication (not element-wise -- see mat_mult_els) -- c = a * b -- number of columns of a must be same as number of rows of b, and vice-versa
  static double_Matrix* dot(const double_Matrix* a, const double_Matrix* b)
  { return mat_mult_css(a, b); }
  // #CAT_Matrix matrix multiplication (not element-wise -- see mat_mult_els) -- c = a * b -- number of columns of a must be same as number of rows of b, and vice-versa
  static int_Matrix* find(const byte_Matrix* mat);
  // #CAT_Matrix (synonym for nonzero) returns a 1D matrix of coordinates into given matrix mat for all values that are non-zero -- the resulting coordinates can be used as a more efficient view onto a matrix of the same shape (the original matrix can also be used directly as a mask view, but it is less efficient, especially as the number of non-zero values is relatively small
  static int_Matrix* nonzero(const byte_Matrix* mat) { return find(mat); }
  // #CAT_Matrix (synonym for find) returns a 1D matrix of coordinates into given matrix mat for all values that are non-zero -- the resulting coordinates can be used as a more efficient view onto a matrix of the same shape (the original matrix can also be used directly as a mask view, but it is less efficient, especially as the number of non-zero values is relatively small

  static double_Matrix* angle(const complex_Matrix* cmat);
  // #CAT_Complex return a new double matrix that contains the angles of the complex numbers in this matrix: atan2(imag, real)
  static double_Matrix* real(const complex_Matrix* cmat);
  // #CAT_Complex return a new double matrix that contains the real-valued elements from this complex matrix
  static double_Matrix* imag(const complex_Matrix* cmat);
  // #CAT_Complex return a new double matrix that contains the imaginary-valued elements from this complex matrix
  static complex_Matrix* conj(const complex_Matrix* cmat);
  // #CAT_Complex return a new complex matrix that contains the complex conjugate of this complex matrix, where the imaginary component is replaced with its negative (i.e., real - imag)
  static complex_Matrix* complex(const double_Matrix* reals, const double_Matrix* imags);
  // #CAT_Complex create a new complex matrix composed of real and imaginary components given by the input matricies
  static complex_Matrix* expi(const double_Matrix* angles);
  // #CAT_Complex returns complex matrix from angles in input matrix, using exponential of i * angle = cos(angle) + i sin(angle) (Euler's formula)


  static complex_Matrix* fft(const double_Matrix* in_mat);
  // #CAT_Complex returns complex matrix from forward fast fourier transform (FFT) of the data in in_mat (can be either a real or complex matrix), writing the complex output to out_mat. results are much faster if size is divisible by many small factors, but any size can be used (radix-N). complex numbers stored in out_mat as inner-most dimension of matrix, of size 2, first is real, 2nd is imag.  if there are more than 1 dims in in_mat, the FFT is computed for all frames of d0 (assuming a time series of 1d frames -- see fft2 for 2d); 
  static complex_Matrix* ffti(const complex_Matrix* in_mat);
  // #CAT_Complex returns complex matrix from inverse (backward) fast fourier transform (FFT) of the data in in_mat (must be a complex matrix), writing the complex output to out_mat. results are much faster if size is divisible by many small factors, but any size can be used (radix-N). complex numbers stored in out_mat as inner-most dimension of matrix, of size 2, first is real, 2nd is imag.  if there are more than 1 dims in in_mat, the FFT is computed for all frames of d0 (assuming a time series of 1d frames -- see ffti2 for 2d); 
  static complex_Matrix* fft2(const double_Matrix* in_mat);
  // #CAT_Complex returns complex matrix from forward 2D fast fourier transform (FFT) of the 2D data in in_mat (can be either a real or complex matrix), writing the complex output to out_mat. results are much faster if size is divisible by many small factors, but any size can be used (radix-N). complex numbers stored in out_mat as inner-most dimension of matrix, of size 2, first is real, 2nd is imag. if there are more than 2 dims in in_mat, the FFT is computed for all frames 2D data
  static complex_Matrix* ffti2(const complex_Matrix* in_mat);
  // #CAT_Complex returns complex matrix from inverse (backward) 2D fast fourier transform (FFT) of the data in in_mat (must be a complex matrix), writing the complex output to out_mat. results are much faster if size is divisible by many small factors, but any size can be used (radix-N). complex numbers stored in out_mat as inner-most dimension of matrix, of size 2, first is real, 2nd is imag.  if there are more than 2 dims in in_mat, the FFT is computed for all frames 2D data



  TA_ABSTRACT_BASEFUNS_NOCOPY(cssMath);
private:
  void Initialize() { };
  void Destroy() { };
};

#endif // cssMath_h
