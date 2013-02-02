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

#include "taiTypeOfArray.h"
#include <taiWidgetArrayEditButton>

TypeDef_Of(taArray);

int taiTypeOfArray::BidForType(TypeDef* td) {
  if (td->InheritsFrom(TA_taArray)) { // bid higher than the class  type
    return (taiTypeOfClass::BidForType(td) +1);
  }
  return 0;
}

taiWidget* taiTypeOfArray::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  taiWidgetArrayEditButton *rval =
    new taiWidgetArrayEditButton(NULL, typ, host_, par, gui_parent_, flags_);
  return rval;
}
