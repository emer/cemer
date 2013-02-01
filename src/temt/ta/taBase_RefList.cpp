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

#include "taBase_RefList.h"

#include <taSigLink>
#include <IRefListClient>
#include <taMisc>

void taBase_RefList::Initialize() {
  m_own = NULL;
}

taBase_RefList::~taBase_RefList() {
  m_own = NULL; // conservative, even though removing items shouldn't trigger anything
  Reset();
}

void taBase_RefList::SigLinkDestroying(taSigLink* dl) {
  // note: dl has already done a RemoveSigLink on us
  taBase* tab = dl->taData();
  if (tab) { // should exist!
    // note: we need to remove all instances, in case multiply-added
    while (RemoveEl(tab)) {;}
    if (m_own) {
      m_own->SigDestroying_Ref(this, tab);
    }
  }
  else {
    taMisc::DebugInfo("Unexpected taData() NULL in taBase_RefList::SigLinkDestroying()");
  }
}

void taBase_RefList::SigLinkRecv(taSigLink* dl, int dcr, void* op1, void* op2) {
  if (!m_own) return;
  taBase* tab = dl->taData();
  m_own->SigEmit_Ref(this, tab, dcr, op1, op2);
}

void* taBase_RefList::El_Ref_(void* it_) {
  taBase* it = (taBase*)it_;
  it->AddSigClient(this);
  return it_;
}

void* taBase_RefList::El_unRef_(void* it_) {
  taBase* it = (taBase*)it_;
  it->RemoveSigClient(this);
  return it_;
}

void taBase_RefList::setOwner(IRefListClient* own_) {
  m_own = own_;
}

int taBase_RefList::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int nchg = 0;
  for(int i=size-1; i>=0; i--) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    taBase* old_own = itm->GetOwner(old_par->GetTypeDef());
    if(old_own != old_par) continue;
    String old_path = itm->GetPath(NULL, old_par);
    MemberDef* md;
    taBase* nitm = new_par->FindFromPath(old_path, md);
    if(nitm) {
      ReplaceIdx(i, nitm);
      nchg++;
    }
    else {
      RemoveIdx(i);
    }
  }
  return nchg;
}

