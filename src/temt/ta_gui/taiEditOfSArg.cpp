// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "taiEditOfSArg.h"
#include <taiEditorOfSArg>

taTypeDef_Of(SArg_Array);

int taiEditOfSArg::BidForEdit(TypeDef* td){
  if(td->InheritsFrom(TA_SArg_Array))
    return (taiEditOfArray::BidForType(td) +1);
  return 0;
}

taiEditorOfClass* taiEditOfSArg::CreateDataHost(void* base, bool readonly, bool modal) {
  return new taiEditorOfSArg(base, typ, readonly, modal);
}
