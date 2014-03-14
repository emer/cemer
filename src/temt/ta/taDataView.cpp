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

#include "taDataView.h"

#include <SigLinkSignal>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(taDataView);
SMARTREF_OF_CPP(taDataView);


void taDataView::Initialize() {
  m_data = NULL;
  data_base = &TA_taBase;
  m_dbu_cnt = 0;
  m_parent = NULL;
  m_index = -1;
  m_vis_cnt = 0;
  m_defer_refresh = 0;
}

void taDataView::CutLinks() {
  m_parent = NULL;
  SetData(NULL);
  inherited::CutLinks();
}

void taDataView::Copy_(const taDataView& cp) {
  SetData(cp.m_data);
}

void taDataView::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  if (isMapped())
    Render_impl();
}

void taDataView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (taMisc::is_loading) {
    if (m_data)
      m_data->AddSigClient(this);
  }
}

void taDataView::ChildAdding(taDataView* child) {
  child->m_vis_cnt = m_vis_cnt;
}

// arbitrary number, not likely to be higher than this
#define MAX_VIS_CNT 6

void taDataView::SigRecvUpdateView_impl() {
  if(taMisc::gui_active) Render_impl();
} 

void taDataView::SetVisible(bool showing) {
  DataViewAction act = (showing) ? SHOWING_IMPL : HIDING_IMPL;
  DoActions(act);
}

void taDataView::SetVisible_impl(DataViewAction act) {
  int do_defer_refresh = 0;
  if (act & SHOWING_IMPL) {
    if (++m_vis_cnt == 1) {
      // first hide->show: do a defered struct guy if appl
      do_defer_refresh = m_defer_refresh;
      if (do_defer_refresh > 0)
        SigLinkRecv(NULL, SLS_STRUCT_UPDATE_BEGIN, NULL, NULL);
      else if (do_defer_refresh < 0)
        SigLinkRecv(NULL, SLS_DATA_UPDATE_BEGIN, NULL, NULL);
    }
    if(m_vis_cnt > MAX_VIS_CNT) {
      DebugInfo("taDataView::SetVisible_impl",
                "m_vis_cnt > likely max, may indicate show/hide issues (is:",
                String(m_vis_cnt), ")");
    }
  }
  else { // act & HIDING_IMPL
    if (TestError((--m_vis_cnt < 0), "taDataView::SetVisible_impl",
      "m_vis_cnt went -ve, indicates show/hide issues"))
      m_vis_cnt = 0;
  }

  DoActionChildren_impl(act);

  if (do_defer_refresh == 0) return;

  DebugInfo("doing deferred refresh:", String(do_defer_refresh), " on: ",
            data()->GetName());
  m_defer_refresh = 0;
  if (do_defer_refresh > 0)
    SigLinkRecv(NULL, SLS_STRUCT_UPDATE_END, NULL, NULL);
  else // (do_defer_rebuild < 0)
    SigLinkRecv(NULL, SLS_DATA_UPDATE_END, NULL, NULL);
}


void taDataView::IgnoredSigEmit(taSigLink*, int sls, void* op1_, void* op2_) {
  // note: should not need to track anything during loading
  if (taMisc::is_loading) return;

  // keep track if we need to update -- struct has priority, and overrides data
  if ((sls == SLS_STRUCT_UPDATE_BEGIN) ||
    (sls == SLS_REBUILD_VIEWS))
    m_defer_refresh = 1;
  else if ((m_defer_refresh == 0) && ((sls == SLS_DATA_UPDATE_BEGIN) ||
    (sls <= SLS_ITEM_UPDATED_ND) || (sls == SLS_UPDATE_VIEWS)))
    m_defer_refresh = -1;
}

// set this to emit debug messages for the following code..
// #define DATA_DATA_DEBUG 1

void taDataView::SigLinkRecv(taSigLink*, int sls, void* op1_, void* op2_) {
  // detect the implicit DATA_UPDATE_END
#ifdef DATA_DATA_DEBUG
  if(sls <= SLS_ITEM_UPDATED_ND) {
    taMisc::Info(GetName(),"iu:", String(m_dbu_cnt));
  }
#endif
  if ((m_dbu_cnt == -1) && (sls <= SLS_ITEM_UPDATED_ND))
    sls = SLS_DATA_UPDATE_END;
  // we need to reinterpret a ITEM_UPDATED if we are in datamode with count=1
  // that is sent instead of the terminal DATA_UPDATE_END
  if (sls == SLS_STRUCT_UPDATE_BEGIN) { // forces us to be in struct state
    if (m_dbu_cnt < 0) m_dbu_cnt *= -1; // switch state if necessary
    ++m_dbu_cnt;
#ifdef DATA_DATA_DEBUG
    taMisc::Info(GetName(),"stru start:", String(m_dbu_cnt));
#endif
    return;
  }
  else if (sls == SLS_DATA_UPDATE_BEGIN) { // stay in struct state if struct state
    if (m_dbu_cnt > 0) ++m_dbu_cnt;
    else               --m_dbu_cnt;
#ifdef DATA_DATA_DEBUG
    taMisc::Info(GetName(),"data start:", String(m_dbu_cnt));
#endif
    return;
  }
  else if ((sls == SLS_STRUCT_UPDATE_END) || (sls == SLS_DATA_UPDATE_END)) {
    bool stru = false;
    if (m_dbu_cnt < 0) ++m_dbu_cnt;
    else {stru = true; --m_dbu_cnt;}
#ifdef DATA_DATA_DEBUG
    if(sls == SLS_DATA_UPDATE_END)
      taMisc::Info(GetName(),"data end:", String(m_dbu_cnt));
    else
      taMisc::Info(GetName(),"stru end:", String(m_dbu_cnt));
#endif
    if (m_dbu_cnt == 0) {
      int pdbu = parDbuCnt();
      // we will only signal if no parent update, or if parent is data and we are structural
      if (pdbu == 0) {
        if (stru) {
          SigRecvStructUpdateEnd_impl();
          SigLinkRecv_impl(SLS_STRUCT_UPDATE_END, NULL, NULL);
        }       
        else {
          SigRecvUpdateView_impl();
          SigLinkRecv_impl(SLS_DATA_UPDATE_END, NULL, NULL);
        }
      }
      else if ((pdbu < 0) && stru) {
          SigRecvStructUpdateEnd_impl();
          SigLinkRecv_impl(SLS_STRUCT_UPDATE_END, NULL, NULL);
      }
    }
    return;
  }
  if ((m_dbu_cnt > 0) || (parDbuCnt() > 0)) {
    return;
  }
  if (sls <= SLS_ITEM_UPDATED_ND) {
    SigRecvUpdateAfterEdit();
  }
  else if (sls == SLS_UPDATE_VIEWS) {
    SigRecvUpdateView_impl();
    SigLinkRecv_impl(SLS_UPDATE_VIEWS, NULL, NULL);
  }
  else if (sls == SLS_REBUILD_VIEWS) {
    SigRecvRebuildView_impl();
    SigLinkRecv_impl(SLS_REBUILD_VIEWS, NULL, NULL);
  }
  else {
    SigLinkRecv_impl(sls, op1_, op2_);
  }
}

void taDataView::SigLinkDestroying(taSigLink*) {
  m_data = NULL;
  SigDestroying();
}

void taDataView::SigRecvUpdateAfterEdit() {
  SigRecvUpdateAfterEdit_impl();
  taDataView* par = parent();
  if (par)
    par->SigRecvUpdateAfterEdit_Child(this);
}

void taDataView::DoActions(DataViewAction acts) {
  if (acts & SHOWING_HIDING_MASK) {
    SetVisible_impl(acts); // note: only 1 will be called
    return; // NEVER combined with any other action
  }
  // never do any rendering during load or copying,
  if (!(taMisc::is_loading || taMisc::is_duplicating)) {

    if (acts & CONSTR_POST) {
      // note: only ever called manually
      Constr_post();
    }
    if (acts & UNBIND_IMPL) {
      Unbind_impl();
    }
  }
  if (acts & CLEAR_IMPL) {
    // must be mapped to do clear
    if (isMapped())
      Clear_impl();
  }
  if (acts & RESET_IMPL) {
    Reset_impl();
  }

  if (taMisc::is_loading || taMisc::is_duplicating) return;
  // no rendering should ever get done if not in gui mode, incl during late shutdown
  if (taMisc::gui_active || taMisc::gui_no_win) {
    if (acts & RENDER_PRE) {
/*obs      // must not already be constructed
      if (!isMapped())*/
      Render_pre();
    }
    // must be mapped for other render steps
    if (isMapped()) {
      if (acts & RENDER_IMPL) {
        Render_impl();
      }
      if (acts & RENDER_POST) {
        Render_post();
      }
    }
  }
  if (acts & CLOSE_WIN_IMPL) {
    if (isMapped())
      CloseWindow_impl();
  }
}


String taDataView::GetLabel() const {
  return GetTypeDef()->GetLabel();
}

int taDataView::parDbuCnt() {
  taDataView* par = parent();
  if (par) {
    int pdbu = par->dbuCnt();
    if (pdbu > 0) return pdbu; //optimization -- don't need to go up the chain if parent is struct
    int ppdbu = par->parDbuCnt();
    int rval = abs(pdbu) + abs(ppdbu); // gives number of nestings... now is it structural or data?
    // both parent and grandparent have to not be structural, for result to be data
    if ((pdbu <= 0) && (ppdbu <= 0)) rval *= -1;
    return rval;
  } else
    return 0;
}

void taDataView::SetData(taBase* ta) {
  if (m_data == ta) return;
  if (m_data) {
    m_data->RemoveSigClient(this);
    m_data = NULL;
  }
  if (!ta) return;
  if (!ta->GetTypeDef()->InheritsFrom(data_base)) {
    taMisc::Warning("taDataView::m_data must inherit from ", data_base->name);
  } else {
    ta->AddSigClient(this);
    m_data = ta;
  }
}

taBase* taDataView::SetOwner(taBase* own) {
  taBase* rval = inherited::SetOwner(own);
  // note: we have to do this here and in InitLinks
  m_parent = (taDataView*)GetOwner(parentType()); // NULL if no owner, or no compatible type
  return rval;
}

