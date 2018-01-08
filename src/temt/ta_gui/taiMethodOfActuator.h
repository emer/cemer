// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef taiMethodOfActuator_h
#define taiMethodOfActuator_h 1

// parent includes:
#include <taiMethod>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiMethodOfActuator);

class TA_API taiMethodOfActuator : public taiMethod {
  TAI_METHOD_SUBCLASS(taiMethodOfActuator, taiMethod);
public:
  int                   BidForMethod(MethodDef* md, TypeDef* td) override;

protected:
  taiWidgetMethod*        GetButtonMethodRep_impl(void* base, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_) override;
  taiWidgetMethod*        GetMenuMethodRep_impl(void* base, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_) override;
};

#endif // taiMethodOfActuator_h
