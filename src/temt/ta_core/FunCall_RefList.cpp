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

#include "FunCall_RefList.h"

#include <IRefListClient>
#include <taMisc>

void FunCall_RefList::AddWithFunName(taBase* obj, String fun_name) {
  delayed_funcalls_name.Add(fun_name);
  inherited::Add(obj);
}

bool FunCall_RefList::RemoveIdxAndFunName(int index) {
  if (delayed_funcalls_name.RemoveIdx(index)) {
    return inherited::RemoveIdx(index);
  }
  return false;
}

void FunCall_RefList::Reset() {
  delayed_funcalls_name.Reset();
  inherited::Reset();
}

String FunCall_RefList::GetFunName(int index) {
  return delayed_funcalls_name.SafeEl(index);
}

void FunCall_RefList::SigLinkDestroying(taSigLink* dl) {
  // note: dl has already done a RemoveSigLink on us
  taBase* tab = dl->taData();
  if (tab) { // should exist!
    // note: we need to remove all instances, in case multiply-added
    while (int index = FindEl(tab)) {
      RemoveIdxAndFunName(index);
    }
    if (m_own) {
      m_own->SigDestroying_Ref(this, tab);
    }
  }
  else {
    taMisc::DebugInfo("Unexpected taData() NULL in taBase_RefList::SigLinkDestroying()");
  }
}
