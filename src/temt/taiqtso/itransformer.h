// Copyright, 1995-2005, Regents of the University of Colorado,
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

/*
  The iTransformer class provides an interface compatible with the Iv Transform class.
  It is implemented on top of the Qt QWMatrix class, which is similar to the Iv class.

*/
#ifndef ITRANSFORMER_H
#define ITRANSFORMER_H

#include "taiqtso_def.h"

class TAIQTSO_API iTransformer {
public:
  iTransformer();
//  iTransformer(const Transformer*);
  iTransformer (float a00, float a01, float a10, float a11, float a20, float a21);
  ~iTransformer();

  iTransformer(const iTransformer& val);
  iTransformer& operator=(const iTransformer& val);

  bool identity() const;
  void premultiply(const iTransformer&);
  void postmultiply(const iTransformer&);
  void invert();
  void translate(float dx, float dy);
  void scale(float sx, float sy);
  void rotate(float angle);
  void skew(float sx, float sy);
//  void transform(floatx, floaty);
//  void transform(float x, float y, floattx, floatty);
//  void inverse_transform(floatx, floaty);
//  void inverse_transform(float tx, float ty, floatx, floaty);
  void matrix(float a00, float a01, float a10, float a11, float a20, float a21);

  iTransformer(const QWMatrix& val);
  iTransformer& operator=(const QWMatrix& val);
//  operator QWMatrix() const;
protected:
  QWMatrix* mat; // actual font data is encapsulated
};

#endif
