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

#include "MethodSpace.h"
#include <MethodDef>
#include <taMisc>

#ifndef NO_TA_BASE
#include <taDataLink>
#endif

String  MethodSpace::El_GetName_(void* it) const { return ((MethodDef*)it)->name; }
taPtrList_impl*  MethodSpace::El_GetOwnerList_(void* it) const { return ((MethodDef*)it)->owner; }
void*   MethodSpace::El_SetOwner_(void* it) { return ((MethodDef*)it)->owner = this; }
void    MethodSpace::El_SetIndex_(void* it, int i){ ((MethodDef*)it)->idx = i; }

void*   MethodSpace::El_Ref_(void* it)    { taRefN::Ref((MethodDef*)it); return it; }
void*   MethodSpace::El_unRef_(void* it)  { taRefN::unRef((MethodDef*)it); return it; }
void    MethodSpace::El_Done_(void* it)   { taRefN::Done((MethodDef*)it); }
void*   MethodSpace::El_MakeToken_(void* it)  { return (void*)((MethodDef*)it)->MakeToken(); }
void*   MethodSpace::El_Copy_(void* trg, void* src)
{ ((MethodDef*)trg)->Copy(*((MethodDef*)src)); return trg; }

void MethodSpace::Initialize() {
  owner = NULL;
#ifndef NO_TA_BASE
  data_link = NULL;
#endif
}

MethodSpace::~MethodSpace() {
  Reset();
#ifndef NO_TA_BASE
  if (data_link) {
    data_link->DataDestroying(); // link NULLs our pointer
  }
#endif
}

bool MethodSpace::AddUniqNameNew(MethodDef *it) {
  bool result = false;
  {
/*NOTE:
  At present, typea is only able to manage a single method per name.
  We only retain the last one scanned (so we always replace previously
    encountered ones with new ones.)
  It is HIGHLY recommended to write source with unique names, and use
  variants on names or default parameters to achieve overloading.
*/
  MethodDef* rval = NULL;
  bool replace = false; // we set true if we should replace the one we find
  int idx;
  // we first see if it is an override of a virtual base...
  if (!it->is_static) // of course statics can't be virtual
    rval = FindVirtualBase(it, idx); //note: only finds virtuals, not non-virtuals
  if (rval) {
    if (it == rval) {result = false; goto end;} // could be the same one..
    it->is_virtual = true; // may not have been set for implicit override
    it->is_override = true; // this is our job to set
    // the overload count will be same for an override
    it->fun_overld = rval->fun_overld;
    // because we are the same method as the one encounters, we inherit its opts
    it->lists.DupeUnique(rval->lists);
// automatically inherit regular options
    it->opts.DupeUnique(rval->opts);
// but not comments or inherited options (which would be redundant)
//    it->opts.DupeUnique(rval->inh_opts);
//    it->inh_opts.DupeUnique(rval->inh_opts);
    if ((it->desc.empty()) || (it->desc == " "))
      it->desc = rval->desc;    // get the comment if we don't actually have one now..
    replace = true; // always replace
  } else {
    // ok, we are not a virtual override, but check if we are an overload or lexical hide
    // since we aren't an override, we are a new entity, so will replace
    idx = FindNameIdx(it->name);
    if(idx >= 0) rval = FastEl(idx);
    if (rval) {
      replace = true;
      if (it == rval) {result = false; goto end;}// could be the same one..
      it->fun_overld = rval->fun_overld;
      // if the args are identical, then we are lexically hiding the previous
      // one -- we will therefore replace it, but of course not set "override"
      if (it->CompareArgs(rval)) {
        it->is_lexhide = true; // note: this is not often done, and could be an error
      } else {
        it->fun_overld++; // normal overload
      }
/*NOTE: in v3.2, this would seem to have applied mostly for virtual overrides
      // never overloaded, not adding any new options
      if (!it->is_static && (it->fun_overld == 0) && (it->opts.size == 0) && (it->lists.size == 0))
      return false; */
    }
  }
  if (replace) {
    ReplaceIdx(idx, it);                // new one replaces old if overloaded or overridden
    result = false;
    goto end;
  }
  inherited::Add(it);
  result = true;                        // yes, its unique
  } // block, for jump
end:
  return result;
}

MethodDef* MethodSpace::FindVirtualBase(MethodDef* it, int& idx) {
  idx = -1;
  for (int i = 0; i < size; ++i) {
    MethodDef* rval = FastEl(i);
    if (!rval->is_virtual) continue;
    if (rval->name != it->name) continue;
    if(!rval->CompareArgs(it)) continue;
    idx = i;
    return rval;
  }
  return NULL;
}

String& MethodSpace::PrintType(String& strm, int indent) const {
  taMisc::IndentString(strm, indent) << "// methods\n";
  String_PArray col1;
  String_PArray col2;
  for(int i=0; i<size; i++) {
    String c1; String c2;
    FastEl(i)->PrintType(c1, c2);
    col1.Add(c1); col2.Add(c2);
  }
  return taMisc::FancyPrintTwoCol(strm, col1, col2, indent);
}

