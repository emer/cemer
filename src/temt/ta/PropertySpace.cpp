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

#include "PropertySpace.h"
#include <PropertyDef>
#include <MemberDef>
#include <MethodDef>
#include <TypeDef>
#include <taMisc>

void PropertyDef::setType(TypeDef* typ) {
  if (!typ) {
    taMisc::Error("PropertyDef::", name, ": attempt to set NULL type!");
    return;
  }
  if (type) {
    if (type != typ)
      taMisc::Warning("PropertyDef::", name, ": changed type from ",
        type->name, " to ", typ->name);
  }
  type = typ;
}

#ifdef NO_TA_BASE
PropertyDef* PropertySpace::AssertProperty_impl(const char* nm, bool& is_new,
    bool get_nset, MemberDef* mbr, MethodDef* mth)
{
  MemberDefBase* md = FindName(nm);
  PropertyDef* pd = dynamic_cast<PropertyDef*>(md);
  //note: following actually won't happen, because maketa doesn't add members
  if (md && !pd) {
    taMisc::Warning("PropertySpace::AssertProperty: attempt to find PropertyDef '",
    nm, "' but MemberDef already exists\n");
    return NULL;
  }
  TypeItem* ti = NULL;
  if (!pd) {
    pd = new PropertyDef(nm);
    Add(pd);
    is_new = true;
  } else
    is_new = false;
  if (mbr) {
    ti = mbr;
    if (get_nset) {
      pd->setType(mbr->type);
      taRefN::SetRefDone(*((taRefN**)&pd->get_mbr), mbr);
    } else {
      pd->setType(mbr->type);
      taRefN::SetRefDone(*((taRefN**)&pd->set_mbr), mbr);
    }
  }
  if (mth) {
    ti = mth;
    if (get_nset) {
      TypeDef* td = mth->type;
      if (td)
        td = td->GetNonConstType();
      pd->setType(td);
      taRefN::SetRefDone(*((taRefN**)&pd->get_mth), mth);
    } else {
      TypeDef* td = mth->arg_types.SafeEl(0);
      if (td)
        td = td->GetNonConstNonRefType();
      pd->setType(td);
      taRefN::SetRefDone(*((taRefN**)&pd->set_mth), mth);
    }
  }
  // new getters control all the opts, desc, etc.
  if (is_new && get_nset) {
    pd->desc = ti->desc;
    pd->opts = ti->opts;
    pd->lists = ti->lists;
  }
  return pd;
}
#endif

//////////////////////////////////
// PropertySpace: Find By Name  //
//////////////////////////////////

int PropertySpace::FindNameOrType(const char *nm) const {       // lookup by name
  int rval = 0; //init just to keep msvc happy
  // first check names
  if(FindName(nm),rval)
    return rval;

  // then type names
  return FindTypeName(nm);
}

int PropertySpace::FindTypeName(const char* nm) const {
  for(int i=0; i<size; i++) {
    if(FastEl(i)->type->InheritsFrom(nm))
      return i;
  }
  return -1;
}

MemberDefBase* PropertySpace::FindNameR(const char* nm) const {
  if (MemberDefBase *rval = FindName(nm)) {
    return rval;
  }

  for (int i = 0; i < size; i++) {
    if (FastEl(i)->type->ptr == 0) {
      if (MemberDefBase *rval = FastEl(i)->type->properties.FindNameR(nm)) {
        return rval;
      }
    }
  }
  return NULL;
}

