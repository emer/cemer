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

#include "taiEditorOfProgramCtrl.h"
#include <Program>
#include <Program_Group>
#include <ProgVar>
#include <DynEnum>

#include <iLabel>
#include <taiMember>
#include <taiWidgetProgVarValue>

#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>

TypeInst_Of(DynEnum);           // TAI_DynEnum

taiEditorOfProgramCtrl::taiEditorOfProgramCtrl(Program* prog, bool read_only_,
                                           bool modal_, QObject* parent)
: taiEditorOfClass(prog, prog->GetTypeDef(), read_only_, modal_, parent)
{
  membs.SetMinSize(MS_CNT); // note: our own MS_CNT
  membs.def_size = 0; // we handle everything
  //  use_show = false;
  refs.setOwner(this); // these update frequently
  refs_struct.setOwner(this); // these are same for prog lifetime
  refs_struct.Add(prog);
  // note: we deftly solve the problem of reacting to new vars/args
  // by simply putting those lists on our ref list, which notifies us
  refs_struct.Add(&(prog->args));
  refs_struct.Add(&(prog->vars));
  Program_Group* pg = GET_OWNER(prog, Program_Group);
  if (pg) { // better exist!
    refs_struct.Add(pg);
  }
  ctrl_panel_mbrs = true;
}

taiEditorOfProgramCtrl::~taiEditorOfProgramCtrl() {
}

bool taiEditorOfProgramCtrl::ShowMember(MemberDef* md) const {
  return false;
}

void taiEditorOfProgramCtrl::Cancel_impl() {
  refs.setOwner(NULL);
  refs.Reset(); // release all the guys we are linked to
  refs_struct.setOwner(NULL);
  refs_struct.Reset(); // release all the guys we are linked to
  inherited::Cancel_impl();
}

void taiEditorOfProgramCtrl::Constr_Members() {
  // we show all sections
  for (int j = 0; j < MS_CNT; ++j) {
    show_set(j) = true;
  }
}


void taiEditorOfProgramCtrl::Constr_Widget_Labels() {
  dat_cnt = 0;
  Program* prog = this->prog();
  refs.Reset();
  membs.ResetItems(); // all Meths and data
  if (!prog) return; // defensive
  String nm;
  String help_text;
  
  // Program guys (just a few key guys), no group label
  {
    MemberSpace& ms = prog->GetTypeDef()->members;
    for (int i = 0; i < ms.size; ++i) {
      MemberDef* md = ms.FastEl(i);
      if (md->im == NULL) continue; // this puppy won't show nohow!set_grp
      // just have a fixed list of guys we show
      if ((md->name == "name") || (md->name == "desc") || (md->name == "short_nm")
          || (md->name == "flags")) {
        memb_el(MS_PROG).Add(md);
      }
      if (md->name == "stop_step_cond") {
        if (md->GetCondOptTest("CONDSHOW", prog->GetTypeDef(), prog)) {
          memb_el(MS_PROG).Add(md);
        }
      }
    }
    int idx = 0;
    // we can just use the default worker bee routine
    Constr_Widget_Labels_impl(idx, &memb_el(MS_PROG), &widget_el(MS_PROG));
  }
  
//   taiMemberWidgets* ms = membs.SafeEl(MS_GP);
//   ms->text = "Items from Program_Group";
//   ms->desc = "useful items from the Program_Group to which this Program belongs";
//   iLabel* lbl = new iLabel(ms->text.chars(), body);
//   AddSectionLabel(-1, lbl,ms->desc.chars());

  taiMemberWidgets* ms = NULL;
  iLabel* lbl = NULL;
  // args and vars
  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    ms = membs.SafeEl(j);
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS: {
      ms->text = "Program args";
      ms->desc = "the arguments to the program";
      pvl = &prog->args;
      lbl = new iLabel(ms->text.chars(), body);
      AddSectionLabel(-1, lbl,ms->desc.chars());
      break;
    }
    case MS_VARS: {
      ms->text = "Program vars";
      ms->desc = "the variables used inside the program";
      pvl = &prog->vars;
      lbl = new iLabel(ms->text.chars(), body);
      AddSectionLabel(-1, lbl, ms->desc.chars());
      } break;
    }

    for (int i = 0; i < pvl->size; ++i) {
      ProgVar* pv = pvl->FastEl(i);
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL)) continue;
      MemberDef* md = pv->GetValMemberDef();
      if(!md) continue;
      memb_el(j).Add(md);
      int flags_ = 0;
      taiWidget* mb_dat = new taiWidgetProgVarValue(&TA_ProgVar, this, NULL, body, flags_);
      widget_el(j).Add(mb_dat);
      QWidget* data = mb_dat->GetRep();
      nm = pv->name;
      help_text = pv->desc;
      AddNameWidget(-1, nm, help_text, data, mb_dat/*, md*/);
      refs.Add(pv);
      ++dat_cnt;
    }
  } // j == set
}

void taiEditorOfProgramCtrl::SigDestroying_Ref(taBase_RefList* ref, taBase* base) {
  // we need to rebuild...
  if (ref == &refs)
    ReShow_Async();
  // otherwise, pgp or prog are destroying, so don't bother
}

void taiEditorOfProgramCtrl::SigEmit_Ref(taBase_RefList* ref, taBase* base,
    int sls, void* op1, void* op2)
{
  if (ignoreSigEmit()) return; // not visible, so ignore!
  Program* prog = this->prog(); //cache
  if (!prog) return;
  // ignore list delete msgs, since the obj itself should notify
  if (ref == &refs_struct) {
    if ((base == &(prog->args)) ||(base == &(prog->vars))) {
      if ((sls <= SLS_LIST_INIT) ||  (sls == SLS_LIST_ITEM_REMOVE) ||
        (sls > SLS_LIST_SORTED)
      ) return;
    }
    Program_Group* pg = GET_OWNER(prog, Program_Group);
    // for step, only interested in group-as-object item update
    if (base == pg) {
      if ((sls > SLS_ITEM_UPDATED_ND))
        return;
    }
  }

  //note: don't need to check for is_loading because we defer until after
  // we need to do a fullblown reshow, to handle things like name changes of vars, etc.
  if(!apply_req)                // already doing delayed apply
    ReShow_Async();
}

MemberDef* taiEditorOfProgramCtrl::GetMemberPropsForSelect(int sel_idx, taBase** base,
    String& lbl, String& desc)
{
  return NULL;
/*
  MemberDef* md = NULL;
  if (!(membs.GetFlatWidgetItem(sel_idx, &md) && md))
    return NULL;
  if (base) *base = cur_base;
  String tlbl = ((taBase*)cur_base)->GetName().elidedTo(16);
  lbl = tlbl;
  return md;*/
}


void taiEditorOfProgramCtrl::GetValue_Membs_def() {
  Program* prog = this->prog(); //cache
  if (!prog) return;

  // prog stuff
  if (show_set(MS_PROG) && (widget_el(MS_PROG).size > 0)) {
    GetValue_impl(&memb_el(MS_PROG), widget_el(MS_PROG), prog);
  }

  // group stuff
//   if (show_set(MS_GP) && (widget_el(MS_GP).size > 0)) {
//     Program_Group* pg = GET_OWNER(prog, Program_Group);
//     GetValue_impl(&memb_el(MS_GP), widget_el(MS_GP), pg);
//   }

  bool first_diff = true;
  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    if (!show_set(j)) continue;
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS: pvl = &prog->args; break;
    case MS_VARS: pvl = &prog->vars; break;
    default: continue; // shouldn't happen!
    }

    int cnt = 0;
    for (int i = 0; i < pvl->size; ++i) {
      ProgVar* pv = pvl->FastEl(i);
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL))
        continue;
      MemberDef* md = memb_el(j).SafeEl(cnt);
      taiWidget* mb_dat = widget_el(j).SafeEl(cnt++);
      if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) continue; // do this after the cnt++!
      if (!md || !mb_dat) {
        taMisc::DebugInfo("taiEditorOfProgramCtrl:GetValue_impl: ran out of controls!");
        break;
      }
      ((taiWidgetProgVarValue*)mb_dat)->GetValueProgVar(pv);
      pv->UpdateAfterEdit();
      if(!first_diff) {         // always reset!
        taiMember::EndScript((void*)pv);
        first_diff = true;
      }
    }
  }
}

void taiEditorOfProgramCtrl::GetImage_Membs()
{
  Program* prog = this->prog(); //cache
  if (!prog) return;

  // prog stuff
  if (show_set(MS_PROG) && (widget_el(MS_PROG).size > 0)) {
    GetImage_impl(&memb_el(MS_PROG), widget_el(MS_PROG), prog);
  }

  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    if (!show_set(j)) continue;
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS: pvl = &prog->args; break;
    case MS_VARS: pvl = &prog->vars; break;
    default: continue; // shouldn't happen!
    }

    int cnt = 0;
    for (int i = 0; i < pvl->size; ++i) {
      ProgVar* pv = pvl->FastEl(i);
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL)) continue;
      MemberDef* md = memb_el(j).SafeEl(cnt);
      taiWidget* mb_dat = widget_el(j).SafeEl(cnt++);
      if (!md || !mb_dat) {
        taMisc::DebugInfo("taiEditorOfProgramCtrl:GetImage_impl: ran out of controls!");
        break;
      }
      ((taiWidgetProgVarValue*)mb_dat)->GetImageProgVar(pv);
    }
  }
}
