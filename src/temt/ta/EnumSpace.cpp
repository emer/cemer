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

#include "EnumSpace.h"
#include <EnumDef>
#include <taMisc>

#ifndef NO_TA_BASE
#include <taSigLink>
#endif

String  EnumSpace::El_GetName_(void* it) const { return ((EnumDef*)it)->name; }
taPtrList_impl*  EnumSpace::El_GetOwnerList_(void* it) const { return ((EnumDef*)it)->owner; }
void*   EnumSpace::El_SetOwner_(void* it) { return ((EnumDef*)it)->owner = this; }
void    EnumSpace::El_SetIndex_(void* it, int i){ ((EnumDef*)it)->idx = i; }

void*   EnumSpace::El_Ref_(void* it)      { taRefN::Ref((EnumDef*)it); return it; }
void*   EnumSpace::El_unRef_(void* it)    { taRefN::unRef((EnumDef*)it); return it; }
void    EnumSpace::El_Done_(void* it)     { taRefN::Done((EnumDef*)it); }
void*   EnumSpace::El_MakeToken_(void* it)  { return (void*)((EnumDef*)it)->MakeToken(); }
void*   EnumSpace::El_Copy_(void* trg, void* src)
{ ((EnumDef*)trg)->Copy(*((EnumDef*)src)); return trg; }

void EnumSpace::Initialize() {
  owner = NULL;
#ifndef NO_TA_BASE
  data_link = NULL;
#endif
}

EnumSpace::~EnumSpace() {
  Reset();
#ifndef NO_TA_BASE
  if (data_link) {
    data_link->DataDestroying(); // link NULLs our pointer
  }
#endif
}

// default enum no is last + 1
void  EnumSpace::Add(EnumDef* it) {
  taPtrList<EnumDef>::Add(it);
  if((it->idx == 0) || (it->idx > size+1))
    it->enum_no = 0;
  else
    it->enum_no = FastEl(it->idx - 1)->enum_no + 1;
}

EnumDef*  EnumSpace::Add(const char* nm, const char* dsc, const char* op, int eno) {
  EnumDef* rval = new EnumDef(nm);
  Add(rval);
  rval->desc = dsc;
  taMisc::CharToStrArray(rval->opts, op);
  rval->enum_no = eno;
  return rval;
}

EnumDef* EnumSpace::FindNo(int eno) const {
  int i;
  for(i=0; i < size; i++) {
    if(FastEl(i)->enum_no == eno)
      return FastEl(i);
  }
  return NULL;
}


String& EnumSpace::PrintType(String& strm, int indent) const {
  EnumDef* enm;
  String_PArray col1;
  String_PArray col2;
  for(int i=0; i<size; i++) {
    enm = FastEl(i);
    col1.Add(enm->name);
    String c2;
    c2 << "= " << enm->enum_no << ";";
    if(!enm->desc.empty())
      c2 << "\t//" << enm->desc;
    enm->PrintType_OptsLists(c2);
    col2.Add(c2);
  }
  return taMisc::FancyPrintTwoCol(strm, col1, col2, indent);
}
