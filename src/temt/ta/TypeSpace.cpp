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

#include "TypeSpace.h"
#include <TypeDef>
#include <taMisc>

#ifndef NO_TA_BASE
#include <taSigLink>
#endif

String  TypeSpace::El_GetName_(void* it) const { return ((TypeDef*)it)->name; }
taPtrList_impl*  TypeSpace::El_GetOwnerList_(void* it) const { return ((TypeDef*)it)->owner; }
void*   TypeSpace::El_SetOwner_(void* it_) {
  if (!it_) return it_;
  TypeDef* it = (TypeDef*)it_;
  it->owner = this;
  //if this type is being added to anything during a plugin init, then
  // it is a plugin class, and we stamp it as such
#ifndef NO_TA_BASE
  if (taMisc::in_plugin_init) {
    it->plugin = taMisc::plugin_loading;
  }
#endif
  return it_;

}
void    TypeSpace::El_SetIndex_(void* it, int i){ ((TypeDef*)it)->idx = i; }

void*   TypeSpace::El_Ref_(void* it)      { taRefN::Ref((TypeDef*)it); return it; }
void*   TypeSpace::El_unRef_(void* it)    { taRefN::unRef((TypeDef*)it); return it; }
void    TypeSpace::El_Done_(void* it)     { taRefN::Done((TypeDef*)it); }
void*   TypeSpace::El_MakeToken_(void* it)  { return (void*)((TypeDef*)it)->MakeToken(); }
void*   TypeSpace::El_Copy_(void* trg, void* src)
{ ((TypeDef*)trg)->Copy(*((TypeDef*)src)); return trg; }

void TypeSpace::Initialize() {
  owner = NULL;
#ifndef NO_TA_BASE
  sig_link = NULL;
#endif
}

TypeSpace::~TypeSpace() {
  Reset();
#ifndef NO_TA_BASE
  if (sig_link) {
    sig_link->SigDestroying(); // link NULLs our pointer
  }
#endif
}

TypeDef* TypeSpace::FindTypeR(const String& fqname) const {
  if (fqname.contains("::")) {
    TypeDef* td = FindName(fqname.before("::"));
    if (!td) return NULL;
    return td->sub_types.FindTypeR(fqname.after("::"));
  } else {
    return FindName(fqname);
  }
}


bool TypeSpace::ReplaceLinkAll(TypeDef* ol, TypeDef* nw) {
  bool rval = false;
  int i;
  for(i=0; i<size; i++) {
    if(FastEl(i) == ol) {
      rval = true;
      ReplaceLinkIdx(i, nw);
    }
  }
  return rval;
}

bool TypeSpace::ReplaceParents(const TypeSpace& ol, const TypeSpace& nw) {
  bool rval = false;
  int i;
  for(i=0; i<size; i++) {
    int j;
    for(j=0; j<ol.size; j++) {
      TypeDef* old_st = ol.FastEl(j);
      TypeDef* new_st = nw.FastEl(j);   // assumes one-to-one correspondence

      if(FastEl(i)->ReplaceParent(old_st, new_st))
        rval = true;
    }
  }
  return rval;
}

String& TypeSpace::PrintAllTokens(String& strm) const {
  for(int i=0; i<size; i++) {
    TypeDef* td = FastEl(i);
    if(!td->tokens.keep)
      continue;
    strm << td->name << ": \t" << td->tokens.size << " (sub: " << td->tokens.sub_tokens << ")\n";
  }
  return strm;
}
