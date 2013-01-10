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

#include "MemberSpace.h"
#include <MemberDef>
#include <TypeDef>
#include <taMisc>

using namespace std;

int MemberSpace::FindNameOrType(const char *nm) const { // lookup by name
  int rval = FindNameIdx(nm);
  if(rval >= 0)
    return rval;

  // then type names
  return FindTypeName(nm);
}

int MemberSpace::FindTypeName(const char* nm) const {
  for(int i=0; i<size; i++) {
    if(FastEl(i)->type->InheritsFrom(nm))
      return i;
  }
  return -1;
}

MemberDef* MemberSpace::FindNameR(const char* nm) const {
  if (MemberDef *rval = FindName(nm)) {
    return rval;
  }

  for (int i = 0; i < size; i++) {
    if (FastEl(i)->type->ptr == 0) {
      if (MemberDef *rval = FastEl(i)->type->members.FindNameR(nm)) {
        return rval;
      }
    }
  }
  return NULL;
}

MemberDef* MemberSpace::FindNameAddr(const char* nm, void* base, void*& ptr) const {
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
    if (FastEl(i)->type->ptr == 0) {
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
    if((md->type->ptr == 1) && (mbr == *((void **)md->GetOff(base))))
      return i;
    // Variants: just check for equivalence to contained pointer,
    //  if doesn't contain a ptr, test will return null
    if (md->type->InheritsFrom(TA_Variant)) {
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

bool MemberSpace::CompareSameType(Member_List& mds, TypeSpace& base_types,
                                  voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                                  TypeDef* base_typ, void* trg_base, void* src_base,
                                  int show_forbidden, int show_allowed, bool no_ptrs,
                                  bool test_only) {
  bool some_diff = false;
  int i;
  for(i=0; i<size; i++) {
    MemberDef* md = FastEl(i);
    if(md->ShowMember(show_forbidden, TypeItem::SC_ANY, show_allowed)) {
      if(no_ptrs && (md->type->ptr > 0 || md->type->HasOption("SMART_POINTER"))) continue;
      some_diff |= md->CompareSameType(mds, base_types, trg_bases, src_bases,
                                       base_typ, trg_base, src_base,
                                       show_forbidden, show_allowed, no_ptrs, test_only);
    }
  }
  return some_diff;
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
    if(!md->ShowMember(taMisc::USE_SHOW_GUI_DEF, TypeItem::SC_ANY,
                       taMisc::SHOW_CHECK_MASK)) continue;
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
