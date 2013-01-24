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

#include "gpiTAPtrArgType.h"
#include <taList_impl>

#include <taMisc>

int gpiTAPtrArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (td->InheritsFrom(TA_taList_impl) &&
      (argt->IsPointer()) && argt->DerivesFrom(TA_taBase))
    return taiTokenPtrArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* gpiTAPtrArgType::GetElFromArg(const char* nm, void* base) {
  taList_impl* lst = (taList_impl*)base;
  if ((lst != NULL) &&
     (arg_typ->DerivesFrom(lst->el_base) || lst->el_base->DerivesFrom(arg_typ->GetNonPtrType()))) {
    String ptrnm = lst->el_base->name + "_ptr";
    TypeDef* ntd = taMisc::types.FindName(ptrnm);
    if (ntd != NULL)
      arg_typ = ntd;    // search in el_base (if args are compatible)
  }
  return taiTokenPtrArgType::GetElFromArg(nm,base);
}
