/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

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


