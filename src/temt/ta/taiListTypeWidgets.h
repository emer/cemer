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

#ifndef taiListTypeWidgets_h
#define taiListTypeWidgets_h 1

// parent includes:
#include "ta_def.h"

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef;
class taBase;
class taiWidget;

class TA_API taiListTypeWidgets {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS contains data_els for one member of List
public:
  TypeDef*      typ;
  taBase*       cur_base;
  taiWidget*      data_el;        // data element provided by the typ->it

  taiListTypeWidgets(TypeDef* tp, taBase* base, taiWidget* data_el = NULL);
  virtual ~taiListTypeWidgets();
};

#endif // taiListTypeWidgets_h
