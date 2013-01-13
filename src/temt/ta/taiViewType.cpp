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

#include "taiViewType.h"
#include <iDataPanelSet>
#include <iDataPanelFrame>


taiDataLink* taiViewType::StatGetDataLink(void* el, TypeDef* el_typ) {
  if (!el || !el_typ) return NULL; // subclass will have to grok

  // handle ptrs by derefing the type and the el
  if (el_typ->ptr > 0) {
    int ptr = el_typ->ptr; // need to deref this many times
    el_typ = el_typ->GetNonPtrType(); // gets base type in one step
    while (el && ptr) {
      el = *((void**)el);
      --ptr;
    }
  }

  if (!el || !el_typ) return NULL;
  if (!el_typ->iv) return NULL;
  taiDataLink* rval = NULL;
  taiViewType* tiv = el_typ->iv;
  rval = tiv->GetDataLink(el, el_typ);
  return rval; //NULL if not taBase
}

void taiViewType::Initialize() {
  m_dp = NULL;
  m_dps = NULL;
  m_need_set = false;
}

void taiViewType::AddView(TypeDef* td) {
  InsertThisIntoBidList(td->iv);
}

iDataPanel* taiViewType::CreateDataPanel(taiDataLink* dl_) {
  m_dp = NULL;
  m_dps = NULL;
  m_need_set = false;
  CreateDataPanel_impl(dl_);
  if (m_dps) {
      m_dps->AllSubPanelsAdded();
      m_dps->setCurrentPanelId(0);
      return m_dps;
  } else return m_dp;
}

void taiViewType::DataPanelCreated(iDataPanelFrame* dp) {
  // we will need to create a set if > 1 panel, or 1st-only uses minibar
  m_need_set = m_need_set || (m_dp != NULL) || dp->hasMinibarCtrls();
  if (!m_dp) {
    m_dp = dp;
    if (!needSet()) return; // only applies to first guy
  }
  // using or need a set
  if (!m_dps) {
//     // note: use first link for set, in case, ex. this is a Doc panel, which is nonstandard
    m_dps = new iDataPanelSet(m_dp->link());
    m_dps->AddSubPanel(m_dp);
    // if adding first because of minibar, don't add again
    if (m_dp == dp) return;
  }
  m_dps->AddSubPanel(dp);
}
