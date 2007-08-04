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


#include "itransformer.h"

#include <qwmatrix.h>


iTransformer::iTransformer(){
  mat = new QWMatrix();
}

//iTransformer::iTransformer(const Transformer*){}
iTransformer::iTransformer (float a00, float a01, float a10, float a11, float a20, float a21){
  mat = new QWMatrix(a00, a01, a10, a11, a20, a21);
}

iTransformer::iTransformer(const iTransformer& val){
  mat = new QWMatrix(val.mat->m11(), val.mat->m12(), val.mat->m21(), val.mat->m22(), val.mat->dx(), val.mat->dy());
}
iTransformer::iTransformer(const QWMatrix& val){//void iTransformer::matrix(floata00, floata01, floata10, floata11, floata20, floata21) {}

  mat = new QWMatrix(val.m11(), val.m12(), val.m21(), val.m22(), val.dx(), val.dy());
}

iTransformer::~iTransformer(){
  delete mat;
  mat = NULL;
}

void iTransformer::matrix(float a00, float a01, float a10, float a11, float a20, float a21){
  mat->setMatrix(a00, a01, a10, a11, a20, a21);
}

iTransformer& iTransformer::operator=(const iTransformer& val){
  mat->setMatrix(val.mat->m11(), val.mat->m12(), val.mat->m21(), val.mat->m22(), val.mat->dx(), val.mat->dy());
  return *this;
}

iTransformer& iTransformer::operator=(const QWMatrix& val){
  mat->setMatrix(val.m11(), val.m12(), val.m21(), val.m22(), val.dx(), val.dy());
  return *this;
}

//iTransformer::operator QWMatrix() const{}

bool iTransformer::identity() const {return mat->isIdentity();}
//void iTransformer::premultiply(const iTransformer&) {}
//void iTransformer::postmultiply(const iTransformer&) {}
void iTransformer::invert() {
  bool invertible;
  QWMatrix val = mat->invert(&invertible);
  if (invertible) {
    matrix(val.m11(), val.m12(), val.m21(), val.m22(), val.dx(), val.dy());
  }
}

void iTransformer::translate(float dx, float dy) {mat->translate(dx, dy);}
void iTransformer::scale(float sx, float sy) {mat->scale(sx, sy);}
void iTransformer::rotate(float angle) {mat->rotate(angle);}
void iTransformer::skew(float sx, float sy) {mat->shear(sx, sy);}
//void iTransformer::transform(float x, float y) {}
//void iTransformer::transform(float x, float y, floattx, floatty) {}
//void iTransformer::inverse_transform(floatx, floaty) {}
//void iTransformer::inverse_transform(float tx, float ty, floatx, floaty) {}


