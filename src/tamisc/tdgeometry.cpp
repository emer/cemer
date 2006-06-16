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



// tdgeometry.cc

#include "tdgeometry.h"

#ifdef TA_USE_INVENTOR
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoTransform.h>
#endif

TypeDef* TwoDCoord::StatTypeDef(int) 	{ return &TA_TwoDCoord; }

TwoDCoord::TwoDCoord(const FloatTwoDCoord& cp) {
  Register(); Initialize(); x = (int)cp.x; y = (int)cp.y;
}

void  TwoDCoord::UnSafeCopy(TAPtr cp) {
  if(cp->InheritsFrom(&TA_TwoDCoord)) Copy(*((TwoDCoord*)cp));
  if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this);
}

TwoDCoord& TwoDCoord::operator=(const FloatTwoDCoord& cp) {
  x = (int)cp.x; y = (int)cp.y;
  return *this;
}

void TwoDCoord::CopyToMatrixGeom(MatrixGeom& geom) {
// NOTE: TDCoord just replaces this whole routine, for simplicity
  geom.EnforceSize(2);
  geom.FastEl(0) = x;
  geom.FastEl(1) = y;
}

bool TwoDCoord::FitN(int n) {
  if((x * y) >= n)	return false;
  y = (int)sqrtf((float)n);
  if(y < 1)
    y = 1;
  x = n / y;
  while((x * y) < n)
    x++;
  return true;
}

bool TwoDCoord::WrapClipOne(bool wrap, int& c, int max) {
  if(c >= max) {
    if(wrap)	c = c % max;
    else	c = -1;
  }
  else if(c < 0) {
    if(wrap)    c = max + (c % max);
    else	c = -1;
  }
  if(c < 0)
    return true;
  return false;
}


void PosTwoDCoord::UpdateAfterEdit() {
  TwoDCoord::UpdateAfterEdit();
  SetGtEq(0);
}


TypeDef* TDCoord::StatTypeDef(int) 	{ return &TA_TDCoord; }

TDCoord::TDCoord(const FloatTDCoord& cp) {
  Register(); Initialize();
  x = (int)cp.x; y = (int)cp.y; z = (int)cp.z;
}

void  TDCoord::UnSafeCopy(TAPtr cp) {
  if(cp->InheritsFrom(&TA_TDCoord)) Copy(*((TDCoord*)cp));
  if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this);
}

TDCoord& TDCoord::operator=(const FloatTDCoord& cp) {
  x = (int)cp.x; y = (int)cp.y;	z = (int)cp.z;
  return *this;
}

void TDCoord::CopyToMatrixGeom(MatrixGeom& geom) {
// NOTE: TDCoord just replaces this whole routine, for simplicity
  geom.EnforceSize(3);
  geom.FastEl(0) = x;
  geom.FastEl(1) = y;
  geom.FastEl(2) = z;
}

bool TDCoord::FitNinXY(int n) {
  if((x * y) >= n)	return false;
  y = (int)sqrtf((float)n);
  if(y < 1)
    y = 1;
  x = n / y;
  while((x * y) < n)
    x++;
  return true;
}

void PosTDCoord::UpdateAfterEdit() {
  TDCoord::UpdateAfterEdit();
  SetGtEq(0);
}


TypeDef* FloatTwoDCoord::StatTypeDef(int) 	{ return &TA_FloatTwoDCoord; }

FloatTwoDCoord::FloatTwoDCoord(const TwoDCoord& cp) {
  Register(); Initialize(); x = (float)cp.x; y = (float)cp.y;
}

void FloatTwoDCoord::UnSafeCopy(TAPtr cp) {
  if(cp->InheritsFrom(&TA_FloatTwoDCoord)) Copy(*((FloatTwoDCoord*)cp));
  if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this);
}

FloatTwoDCoord& FloatTwoDCoord::operator=(const TwoDCoord& cp) {
  x = (float)cp.x; y = (float)cp.y;
  return *this;
}

TypeDef* FloatTDCoord::StatTypeDef(int) 	{ return &TA_FloatTDCoord; }

FloatTDCoord::FloatTDCoord(const TDCoord& cp) {
  Register(); Initialize();
  x = (float)cp.x; y = (float)cp.y; z = (float)cp.z;
}

void  FloatTDCoord::UnSafeCopy(TAPtr cp) {
  if(cp->InheritsFrom(&TA_FloatTDCoord)) Copy(*((FloatTDCoord*)cp));
  if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this);
}

FloatTDCoord& FloatTDCoord::operator=(const TDCoord& cp) {
  x = (float)cp.x; y = (float)cp.y; z = (float)cp.z;
  return *this;
}


#ifdef TA_USE_INVENTOR

void FloatTransform::CopyTo(SoTransform* txfm) {
  if (!txfm) return;
  txfm->translation.setValue(SbVec3f(translate.x, translate.y, translate.z));
  txfm->rotation.setValue(SbVec3f(rotate.x, rotate.y, rotate.z), rotate.rot);
  txfm->scaleFactor.setValue(SbVec3f(scale.x, scale.y, scale.z));
}

#endif
