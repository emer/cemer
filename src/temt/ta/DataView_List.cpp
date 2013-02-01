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

#include "DataView_List.h"

void DataView_List::SigEmit(int sls, void* op1, void* op2) {
  inherited::SigEmit(sls, op1, op2);
  taDataView* own = GET_MY_OWNER(taDataView);
  if (own)
    own->SigEmit_Child(this, sls, op1, op2);
}

void DataView_List::El_disOwn_(void* it) {
  if (data_view) {
    data_view->ChildRemoving((taDataView*)it);
  }
  inherited::El_disOwn_(it);
}

void* DataView_List::El_Own_(void* it) {
  inherited::El_Own_(it);
  if (data_view)
    data_view->ChildAdding((taDataView*)it);
  return it;
}

taBase* DataView_List::SetOwner(taBase* own) {
  if (own && own->GetTypeDef()->InheritsFrom(&TA_taDataView))
    data_view = (taDataView*)own;
  else data_view = NULL;
  return inherited::SetOwner(own);
}

void DataView_List::DoAction(taDataView::DataViewAction acts) {
  if (acts & taDataView::CONSTR_MASK) {
    for (int i = 0; i < size; ++i) {
      taDataView* dv = FastEl(i);
      dv->DoActions(acts);
    }
  } else { // DESTR_MASK
    for (int i = size - 1; i >= 0 ; --i) {
      taDataView* dv = FastEl(i);
      dv->DoActions(acts);
    }
    if (acts & taDataView::RESET_IMPL)
      Reset();
  }
}
