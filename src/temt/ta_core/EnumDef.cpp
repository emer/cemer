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

#include "EnumDef.h"
#include <EnumSpace>
#include <taMisc>

EnumDef::EnumDef()
:inherited()
{
  Initialize();
}

EnumDef::EnumDef(const char* nm)
:inherited()
{
  Initialize();
  name = nm;
}

EnumDef::EnumDef(const char* nm, const char* dsc, int eno, const char* op, const char* lis)
:inherited()
{
  name = nm;
  desc = dsc;
  enum_no = eno;
  taMisc::CharToStrArray(opts, op);
  taMisc::CharToStrArray(lists,lis);
}

EnumDef::EnumDef(const EnumDef& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

void EnumDef::Initialize() {
  owner = NULL;
  enum_no = 0;
}

void EnumDef::Copy(const EnumDef& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void EnumDef::Copy_(const EnumDef& cp) {
  enum_no = cp.enum_no;
}

bool EnumDef::CheckList(const String_PArray& lst) const {
  int i;
  for(i=0; i<lists.size; i++) {
    if(lst.FindEl(lists.FastEl(i)) >= 0)
      return true;
  }
  return false;
}

TypeDef* EnumDef::GetOwnerType() const {
  TypeDef* rval=NULL;
  if((owner) && (owner->owner))
    rval=owner->owner;
  return rval;
}
