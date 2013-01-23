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

#include "MemberDefBase_List.h"
#include <MemberDefBase>
#include <MemberDef>
#include <PropertyDef>
#include <taMisc>

#ifndef NO_TA_BASE
TypeDef_Of(taList_impl);
#include <taDataLink>
#endif

String  MemberDefBase_List::El_GetName_(void* it) const
  { return ((MemberDefBase*)it)->name; }
taPtrList_impl*  MemberDefBase_List::El_GetOwnerList_(void* it) const
  { return ((MemberDefBase*)it)->owner; }
void*   MemberDefBase_List::El_SetOwner_(void* it)
  { return ((MemberDefBase*)it)->owner = this; }
void    MemberDefBase_List::El_SetIndex_(void* it, int i)
  { ((MemberDefBase*)it)->idx = i; }

void*   MemberDefBase_List::El_Ref_(void* it)
  { taRefN::Ref((MemberDefBase*)it); return it; }
void*   MemberDefBase_List::El_unRef_(void* it)
  { taRefN::unRef((MemberDefBase*)it); return it; }
void    MemberDefBase_List::El_Done_(void* it)
  { taRefN::Done((MemberDefBase*)it); }


void*   MemberDefBase_List::El_MakeToken_(void* it) {
  switch (((MemberDefBase*)it)->TypeInfoKind()) {
  case TypeItem::TIK_MEMBER:
    return (void*)((MemberDef*)it)->MakeToken();
  case TypeItem::TIK_PROPERTY:
    return (void*)((PropertyDef*)it)->MakeToken();
  default: return NULL; // shouldn't happen!
  }
  return NULL; // compiler food
}

// note: this use the "pseudo-virtual" type guy
void*   MemberDefBase_List::El_Copy_(void* trg, void* src)
  { ((MemberDefBase*)trg)->Copy(((MemberDefBase*)src));
  return trg;
}

void MemberDefBase_List::Initialize() {
  owner = NULL;
#ifndef NO_TA_BASE
  data_link = NULL;
#endif
}

MemberDefBase_List::~MemberDefBase_List() {
  Reset();
#ifndef NO_TA_BASE
  if (data_link) {
    data_link->DataDestroying(); // link NULLs our pointer
  }
#endif
}
