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



// xform.h -- coordinate transformation object

#ifndef XFORM_H
#define XFORM_H

#include "ta_stdef.h"
#include "ta_base.h"
#include "tamisc_TA_type.h"

// used for storing transforms on ta objects

class Xform : public taNBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER interviews transformer class
public:
  float a00;
  float a01;
  float a10;
  float a11;
  float a20;
  float a21;

  void 		Set(float b00,float b01, float b10, float b11, float b20, float b21);
  void 		Set(iTransformer*); // #IGNORE
  iTransformer* transformer();

  void Initialize();
  void Destroy()	{ };
  void Copy_(const Xform& cp);
  COPY_FUNS(Xform, taNBase);
  Xform(iTransformer* tr);
  TA_BASEFUNS(Xform);
};


class Xform_List : public taList<Xform> { 
public:
  void Initialize()	{ SetBaseType(&TA_Xform);}
  void Destroy()	{ };
  TA_BASEFUNS(Xform_List);
};

#endif
