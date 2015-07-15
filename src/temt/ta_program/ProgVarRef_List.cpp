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

#include <Program>
#include "ProgVarRef_List.h"


void ProgVarRef_List::Initialize() {
}

ProgVarRef_List::~ProgVarRef_List() {
  Reset();
}

ProgVarRef* ProgVarRef_List::FindVar(ProgVar* var, int& idx) const {
  for(int i=0;i<size;i++) {
    ProgVarRef* vrf = FastEl(i);
    if(vrf->ptr() == var) {
      idx = i;
      return vrf;
    }
  }
  idx = -1;
  return NULL;
}

ProgVarRef* ProgVarRef_List::FindVarName(const String& var_nm, int& idx) const {
  for(int i=0;i<size;i++) {
    ProgVarRef* vrf = FastEl(i);
    if(vrf->ptr() && ((ProgVar*)vrf->ptr())->name == var_nm) {
      idx = i;
      return vrf;
    }
  }
  idx = -1;
  return NULL;
}

int ProgVarRef_List::UpdatePointers_NewPar(taBase* lst_own, taBase* old_par, taBase* new_par) {
  int nchg = 0;
  for(int i=size-1; i>=0; i--) {
    ProgVarRef* vrf = FastEl(i);
    nchg += lst_own->UpdatePointers_NewPar_Ref(*vrf, old_par, new_par);
  }
  return nchg;
}

int ProgVarRef_List::UpdatePointers_NewParType(taBase* lst_own, TypeDef* par_typ, taBase* new_par) {
  int nchg = 0;
  for(int i=size-1; i>=0; i--) {
    ProgVarRef* vrf = FastEl(i);
    nchg += lst_own->UpdatePointers_NewParType_Ref(*vrf, par_typ, new_par);
  }
  return nchg;
}

int ProgVarRef_List::UpdatePointers_NewObj(taBase* lst_own, taBase* ptr_owner, taBase* old_ptr, taBase* new_ptr) {
  int nchg = 0;
  for(int i=size-1; i>=0; i--) {
    ProgVarRef* vrf = FastEl(i);
    nchg += lst_own->UpdatePointers_NewObj_Ref(*vrf, ptr_owner, old_ptr, new_ptr);
  }
  return nchg;
}

