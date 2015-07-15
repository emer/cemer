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

#include "ISelectable_PtrList.h"
#include <taiSigLink>
#include <TypeDef>


taPtrList_impl* ISelectable_PtrList::insts;

ISelectable_PtrList::ISelectable_PtrList(const ISelectable_PtrList& cp)
  : taPtrList<ISelectable>(cp)
{
  Initialize();
}

void ISelectable_PtrList::Initialize() {
  // add to managed list
  if (!insts) {
    insts = new taPtrList_impl;
  }
  insts->Add_(this);
}

ISelectable_PtrList::~ISelectable_PtrList() {
  // remove from managed list
  if (insts) {
    insts->RemoveEl_(this);
    if (insts->size == 0) {
      delete insts;
      insts = NULL;
    }
  }
}

// greatest common subtype of items 1-N
TypeDef* ISelectable_PtrList::CommonSubtype1N(ISelectable::GuiContext gc_typ)
{
  if (size == 0) return NULL;
  taiSigLink* link = FastEl(0)->effLink(gc_typ);
  if (!link) return NULL; // gui-only object, no ref
  TypeDef* rval = link->GetDataTypeDef();
  for (int i = 1; (rval && (i < size)); ++i) {
    link = FastEl(i)->effLink(gc_typ);
    if (!link) return NULL; // gui-only, not commensurable
    rval = TypeDef::GetCommonSubtype(rval, link->GetDataTypeDef());
  }
  return rval;
}

// greatest common subtype of items 2-N
TypeDef* ISelectable_PtrList::CommonSubtype2N(ISelectable::GuiContext gc_typ)
{
  if (size <= 1) return NULL;
  taiSigLink* link = FastEl(1)->effLink(gc_typ);
  if (!link) return NULL; // gui-only object, no ref
  TypeDef* rval = link->GetDataTypeDef();
  for (int i = 2; (rval && (i < size)); ++i) {
    link = FastEl(i)->effLink(gc_typ);
    if (!link) return NULL; // gui-only, not commensurable
    rval = TypeDef::GetCommonSubtype(rval, link->GetDataTypeDef());
  }
  return rval;
}

TypeDef* ISelectable_PtrList::Type1(ISelectable::GuiContext gc_typ) {
  if (size == 0) return NULL;
  else {
    taiSigLink* link = FastEl(0)->effLink(gc_typ);
    if (link) return link->GetDataTypeDef();
    else      return NULL; // gui-only object, no ref
  }
}

