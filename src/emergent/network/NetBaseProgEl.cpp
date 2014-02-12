// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "NetBaseProgEl.h"
#include <ProgVar>

TA_BASEFUNS_CTORS_DEFN(NetBaseProgEl);

eTypeDef_Of(Network);

bool NetBaseProgEl::NetProgVarFilter(void* base_, void* var_) {
  bool rval = ObjProgVarFilter(base_, var_);
  if(!rval) return false; // doesn't pass basic test

  ProgVar* var = static_cast<ProgVar*>(var_);
  return (var->object_type && var->object_type->InheritsFrom(&TA_Network));
}


