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

#ifndef taiActuatorMethod_h
#define taiActuatorMethod_h 1

// parent includes:
#include <taiMethod>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiActuatorMethod : public taiMethod {
  TAI_METHOD_SUBCLASS(taiActuatorMethod, taiMethod);
public:
  int                   BidForMethod(MethodDef* md, TypeDef* td);

protected:
  taiMethodData*        GetButtonMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  taiMethodData*        GetMenuMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};

#endif // taiActuatorMethod_h
