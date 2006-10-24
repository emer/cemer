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

#ifndef TA_VIEWSPEC_H
#define TA_VIEWSPEC_H

#include "ta_base.h"
#include "ta_group.h"

#include "ta_TA_type.h"

class TA_API ViewSpec: public taDataView {
  // generic specification for a view of some item
INHERITED(taDataView)
public:

  TA_BASEFUNS(ViewSpec) //
private:
  void Initialize() {}
  void Destroy() {}
};

  
class TA_API ViewSpec_Group: public taGroup<ViewSpec> {
public:
  TA_BASEFUNS(ViewSpec_Group) //
private:
  void Initialize() {SetBaseType(&TA_ViewSpec);}
  void Destroy() {}
};

#endif
