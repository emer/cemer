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

#include "taDataLinkItr.h"
#include <taDataLink>
#include <TypeDef>

void* taDataLinkItr::NextEl(taDataLink* dl, const TypeDef* typ) {
  void* rval = NULL;
  while (dl && (i < dl->clients.size)) {
    IDataLinkClient* dlc = dl->clients.FastEl(i);
    ++i;
    TypeDef* dlc_typ = dlc->GetTypeDef();
    if (dlc_typ) {
      if (dlc_typ->InheritsFrom(typ)) {
        rval = dlc->This();
        break;
      }
    }
  }
  return rval;
}


