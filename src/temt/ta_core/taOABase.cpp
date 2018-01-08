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

#include "taOABase.h"
#include <taBase_QObj>

TA_BASEFUNS_CTORS_DEFN(taOABase);

void taOABase::CutLinks() {
#ifdef TA_GUI
  SetAdapter(NULL);
#endif
  inherited::CutLinks();
}

void taOABase::SetAdapter(taBase_QObj* adapter_) {
  if (adapter == adapter_) return;
  if (adapter_) { // setting adapter
    if (adapter) { // chaining
      adapter->setParent(adapter_);
    }
    adapter = adapter_;
  } else { // deleting adapter
    if (adapter) {
      adapter->owner = NULL; // prevents callback delete
      delete adapter; // will delete chained adapters
      adapter = NULL;
    }
  }
}


