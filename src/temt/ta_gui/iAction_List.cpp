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

#include "iAction_List.h"

void iAction_List::El_Done_(void* it_) {
  iAction* it = (iAction*)it_;
  if (it->nref == 0)
    delete it; //NB: don't deleteLater, because taiWidget->parent will be invalid by then
//     it->deleteLater();
}

iAction* iAction_List::PeekNonSep() {
  iAction* rval;
  for (int i = size - 1; i >= 0; --i) {
    rval = FastEl(i);
    if (!rval->isSeparator())
      return rval;
  }
  return NULL;
}
