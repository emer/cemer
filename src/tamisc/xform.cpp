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



#include "xform.h"

#include "itransformer.h"


/////////////////
//   Xform     //
/////////////////

Xform::Xform(iTransformer * tr){
  Initialize();
  if(tr == NULL) return;
  Set(tr);
}

void Xform::Initialize() {
  a00 = a01 = a10 = a11 = a20 = a21 = 0.0f;
}

void Xform::Copy_(const Xform& cp) {
  a00 = cp.a00; a01 = cp.a01; a10 = cp.a10; a11 = cp.a11;
  a20 = cp.a20; a21 = cp.a21;
}

void Xform::Set(float b00,float b01, float b10, float b11, float b20, float b21) {
  a00 = b00; a01 = b01; a10 = b10; a11 = b11; a20 = b20; a21 = b21;
}

void Xform::Set(iTransformer* tr){
  tr->matrix(a00,a01,a10,a11,a20,a21);
}

iTransformer* Xform::transformer() {
  return new iTransformer(a00,a01,a10,a11,a20,a21);
}

