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

#include "DynEnumItem_List.h"
#include <DynEnumType>
#include <Program>

#include <SigLinkSignal>

void DynEnumItem_List::Initialize() {
  SetBaseType(&TA_DynEnumItem);
}

int DynEnumItem_List::FindNumIdx(int val) const {
  for(int i=0;i<size;i++)
    if(FastEl(i)->value == val) return i;
  return -1;
}

void DynEnumItem_List::OrderItems() {
  if(size == 0 || !owner) return;
  DynEnumType* own = dynamic_cast<DynEnumType*>(owner);
  if(!own) return;
  if(own->bits) {
    int prval = FastEl(0)->value;
    for(int i=1;i<size;i++) {
      DynEnumItem* it = FastEl(i);
      if(it->value <= prval) {
	it->value = prval << 1;
	it->SigEmit(SLS_ITEM_UPDATED);
      }
      prval = it->value;
    }
  }
  else {
    int prval = FastEl(0)->value;
    for(int i=1;i<size;i++) {
      DynEnumItem* it = FastEl(i);
      if(it->value <= prval) {
	it->value = prval + 1;
	it->SigEmit(SLS_ITEM_UPDATED);
      }
      prval = it->value;
    }
  }
}

void DynEnumItem_List::SigEmit(int dcr, void* op1, void* op2) {
  OrderItems();
  // we notify owner, so editing items causes related things to update,
  // typically used by ProgVar to make sure the enum list gets updated in gui
  taBase* own = GetOwner();
  if(own) {
    if(dcr <= SLS_CHILD_ITEM_UPDATED)
      own->SigEmit(SLS_CHILD_ITEM_UPDATED, (void*)this);
  }
  inherited::SigEmit(dcr, op1, op2);
}

bool DynEnumItem_List::BrowserSelectMe() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserSelectMe_ProgItem(this);
}

bool DynEnumItem_List::BrowserExpandAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserExpandAll_ProgItem(this);
}

bool DynEnumItem_List::BrowserCollapseAll() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return false;
  return prog->BrowserCollapseAll_ProgItem(this);
}
