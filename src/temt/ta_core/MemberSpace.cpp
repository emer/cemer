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

#include "MemberSpace.h"
#include <MemberDef>
#include <TypeDef>
#include <taMisc>

using namespace std;

int MemberSpace::FindNameOrType(const String& nm) const { // lookup by name
  int rval = FindNameIdx(nm);
  if(rval >= 0)
    return rval;

  // then type names
  return FindTypeName(nm);
}

int MemberSpace::FindTypeName(const String& nm) const {
  for(int i=0; i<size; i++) {
    if(FastEl(i)->type->InheritsFrom(nm))
      return i;
  }
  return -1;
}

MemberDef* MemberSpace::FindNameR(const String& nm) const {
  if (MemberDef *rval = FindName(nm)) {
    return rval;
  }

  for (int i = 0; i < size; i++) {
    if (FastEl(i)->type->IsNotPtr()) {
      if (MemberDef *rval = FastEl(i)->type->members.FindNameR(nm)) {
        return rval;
      }
    }
  }
  return NULL;
}

MemberDef* MemberSpace::FindNameAddr(const String& nm, void* base, void*& ptr) const {
  if (MemberDef *rval = FindName(nm)) {
    ptr = rval->GetOff(base);
    return rval;
  }
  ptr = NULL;
  return NULL;
}

//////////////////////////////////
// MemberSpace: Find By Type    //
//////////////////////////////////

MemberDef* MemberSpace::FindType(TypeDef* it) const {
  for (int i = 0; i<size; i++) {
    if (FastEl(i)->type->InheritsFrom(it)) {
      return FastEl(i);
    }
  }
  return NULL;
}

MemberDef* MemberSpace::FindTypeR(TypeDef* it) const {
  if (MemberDef *rval = FindType(it)) {
    return rval;
  }

  for (int i = 0; i < size; i++) {
    if (FastEl(i)->type->IsNotPtr()) {
      if (MemberDef *rval = FastEl(i)->type->members.FindTypeR(it)) {
        return rval;
      }
    }
  }
  return NULL;
}

MemberDef* MemberSpace::FindTypeAddr(TypeDef* it, void* base, void*& ptr) const {
  if (MemberDef *rval = FindType(it)) {
    ptr = rval->GetOff(base);
    return rval;
  }
  ptr = NULL;
  return NULL;
}

//////////////////////////////////
// MemberSpace: other Find      //
//////////////////////////////////

int MemberSpace::FindDerives(TypeDef* it) const {
  int i;
  for(i=0; i<size; i++) {
    if(FastEl(i)->type->DerivesFrom(it))
      return i;
  }
  return -1;
}

MemberDef* MemberSpace::FindTypeDerives(TypeDef* it) const {
  int idx = FindDerives(it);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

MemberDef* MemberSpace::FindAddr(void* base, void* mbr, int& idx) const {
  int i;
  for(i=0; i<size; i++) {
    if(mbr == FastEl(i)->GetOff(base)) {
      idx = i;
      return FastEl(i);
    }
  }
  idx = -1;
  return NULL;
}

int MemberSpace::FindPtr(void* base, void* mbr) const {
  int i;
  for(i=0; i<size; i++) {
    MemberDef* md = FastEl(i);
    // check conventional pointers of any type
    if(md->type->IsPointer() && (mbr == *((void **)md->GetOff(base))))
      return i;
    // Variants: just check for equivalence to contained pointer,
    //  if doesn't contain a ptr, test will return null
    if (md->type->IsVariant()) {
      Variant& var = *((Variant*)md->GetOff(base));
      if (mbr == var.toPtr())
        return i;
    }

  }
  return -1;
}

MemberDef* MemberSpace::FindAddrPtr(void* base, void* mbr, int& idx) const {
  idx = FindPtr(base, mbr);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}


void MemberSpace::CopyFromSameType(void* trg_base, void* src_base) {
  int i;
  for(i=0; i<size; i++) {
    MemberDef* md = FastEl(i);
    if(!md->HasOption("NO_COPY"))
      md->CopyFromSameType(trg_base, src_base);
  }
}

void MemberSpace::CopyOnlySameType(void* trg_base, void* src_base) {
  int i;
  for(i=0; i<size; i++) {
    MemberDef* md = FastEl(i);
    if((md->owner == this) && !md->HasOption("NO_COPY")) // only same type provision
      md->CopyFromSameType(trg_base, src_base);         // then copy whole thing..
  }
}

String& MemberSpace::PrintType(String& strm, int indent) const {
  taMisc::IndentString(strm, indent) << "// members\n";
  String_PArray col1;
  String_PArray col2;
  for(int i=0; i<size; i++) {
    String c1; String c2;
    FastEl(i)->PrintType(c1, c2);
    col1.Add(c1); col2.Add(c2);
  }
  return taMisc::FancyPrintTwoCol(strm, col1, col2, indent);
}

String& MemberSpace::Print(String& strm, void* base, int indent) const {
  String_PArray col1;
  String_PArray col2;
  for(int i=0; i<size; i++) {
    MemberDef* md = FastEl(i);
    if(!md->IsVisible()) continue;
    String c1; String c2;
    md->Print(c1, c2, base, indent);
    col1.Add(c1); col2.Add(c2);
  }
  return taMisc::FancyPrintTwoCol(strm, col1, col2, indent);
}

#ifdef NO_TA_BASE

//////////////////////////////////////////////////////////
//  dummy versions of dump load/save: see ta_dump.cc    //
//////////////////////////////////////////////////////////

int MemberSpace::Dump_Save(ostream&, void*, void*, int) {
  return false;
}
int MemberSpace::Dump_SaveR(ostream&, void*, void*, int) {
  return false;
}

int MemberSpace::Dump_Save_PathR(ostream&, void*, void*, int) {
  return false;
}

int MemberSpace::Dump_Load(istream&, void*, void*, const char*, int) {
  return true;
}

#endif // NO_TA_BASE
