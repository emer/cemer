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

#include "DynMethod_PtrList.h"
#include <ISelectable_PtrList>
#include <TypeDef>
#include <MethodDef>
#include <taiDataLink>
#include <taiObjectsMimeItem>
#include <taiMimeSource>

#include <taMisc>


DynMethod_PtrList::~DynMethod_PtrList() {
  Reset();
}

DynMethodDesc* DynMethod_PtrList::AddNew(int dmd_type, MethodDef* md) {
  DynMethodDesc* rval = new DynMethodDesc();
  rval->dmd_type = dmd_type;
  rval->md = md;
  Add(rval);
  return rval;
}

void DynMethod_PtrList::Fill(ISelectable_PtrList& sel_items,
  ISelectable::GuiContext gc_typ)
{
  if (sel_items.size == 0) return;

  TypeDef* t1n = sel_items.CommonSubtype1N(gc_typ); // greatest common subtype of items 1-N
  if (t1n == NULL) return; // typically for non-taBase types, ex Class browsing
  MethodDef* md;
  // Type_N methods (common to all)
  for (int i = 0; i < t1n->methods.size; ++i) {
    md = t1n->methods.FastEl(i);
    if (!md->HasOption("DYN1")) continue;
    AddNew(Type_1N, md);
  }

  if (sel_items.size == 1) return;
  taiDataLink* link = sel_items.FastEl(0)->effLink(gc_typ);
  if (!link) return; // gui only obj
  TypeDef* t1 = link->GetDataTypeDef(); // type of 1st item
  TypeDef* t2n = sel_items.CommonSubtype2N(gc_typ); // greatest common subtype of items 2-N
  if (!t2n) return;

  TypeDef* arg1_typ;
  // Type_1_2N
  for (int i = 0; i < t1->methods.size; ++i) {
    md = t1->methods.FastEl(i);
    if (!md->HasOption("DYN12N")) continue;
    if (md->arg_types.size == 0) {
      taMisc::Warning("method:", md->name, "should have had an arg1 in class* form.");
      continue;
    }
    arg1_typ = md->arg_types.FastEl(0);
    // must be a pointer to a class type
    if (arg1_typ->ptr != 1) {
      taMisc::Warning("method:", md->name, "should have had arg1 in class* form.");
      continue;
    }
    // now get the non-pointer type
    arg1_typ = arg1_typ->GetNonPtrType();
    if (!t2n->InheritsFrom(arg1_typ)) continue;
    AddNew(Type_1_2N, md);
  }

  // Type_2N_1
  for (int i = 0; i < t2n->methods.size; ++i) {
    md = t2n->methods.FastEl(i);
    if ((md->arg_types.size == 0) || !md->HasOption("DYN2N1")) continue;
    arg1_typ = md->arg_types.FastEl(0);
    // must be a pointer to a class type
    if (arg1_typ->ptr != 1) {
      taMisc::Warning("method:", md->name, "should have had class* form.");
      continue;
    }
    // now get the non-pointer type
    arg1_typ = arg1_typ->GetNonPtrType();
    if (!t1->InheritsFrom(arg1_typ)) continue;
    AddNew(Type_2N_1, md);
  }

}

void DynMethod_PtrList::FillForDrop(const taiMimeSource& ms,
    ISelectable* drop_item)
{
  taiObjectsMimeItem* mi = ms.objects();
  if (!mi || (mi->count() == 0)) return;
  TypeDef* tms = mi->CommonSubtype(); // greatest common subtype of source object(s)
  TypeDef* tdi = drop_item->GetEffDataTypeDef();
  if (!tdi) return;

  for (int i = 0; i < tdi->methods.size; ++i) {
    MethodDef* md = tdi->methods.FastEl(i);
    //look for all DROP methods with compatible arg0 type
    if (md->arg_types.size == 0) continue;
    TypeDef* arg0_typ = md->arg_types.FastEl(0);
    // must be a pointer to a class type
    if (arg0_typ->ptr != 1) {
      continue;
    }
    // meth must be marked for drop
    if (!(md->HasOption("DROPN") ||
      ((mi->count() == 1) && md->HasOption("DROP1")))) continue;

    // now get the non-pointer type
    arg0_typ = arg0_typ->GetNonPtrType();
    if (!tms->InheritsFrom(arg0_typ)) continue;
    AddNew(Type_MimeN_N, md);
  }

}

