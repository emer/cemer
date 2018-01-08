// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "taiEditorOfControlPanelFull.h"
#include <ClusterRun>
#include <ParamSet>
#include <taProject>
#include <iLabel>
#include <iFlowLayout>
#include <taiMember>
#include <taiWidgetMashup>
#include <iHiLightButton>
#include <iStripeWidget>

#include <taMisc>
#include <taiMisc>

#include <QColor>

taiEditorOfControlPanelFull::taiEditorOfControlPanelFull(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, parent)
{
  Initialize();
}

taiEditorOfControlPanelFull::~taiEditorOfControlPanelFull() {
}

void taiEditorOfControlPanelFull::Initialize()
{
  help_but = NULL;
  apply_but = NULL;
  revert_but = NULL;
}

void taiEditorOfControlPanelFull::ClearBody_impl() {
  // we also clear all the methods, and then rebuild them
  prop_membs.memb_el.Reset();
  prop_membs.widget_el.Reset();
  ta_menus.Reset();
  ta_menu_buttons.Reset();
//  meth_el.Reset(); // must defer deletion of these, because the MethodWidget objects are used in menu calls, so can't be
  if(frmMethButtons) {
    if(layMethButtons && (layMethButtons != (iFlowLayout*)frmMethButtons->layout())) {
      delete layMethButtons;
    }
    if(frmMethButtons->layout()) {
      delete frmMethButtons->layout();
    }
    layMethButtons = NULL;
    taiMisc::DeleteChildrenLater(frmMethButtons);
  }
  show_meth_buttons = false;

  // note: no show menu in this class
  cur_menu = NULL;
  cur_menu_but = NULL;
  if (menu) {
    menu->Reset();
  }
  inherited::ClearBody_impl();
}

void taiEditorOfControlPanelFull::Constr_Widget_Labels() {
  // delete all previous ctrlpan members 
  String name;
  String help_text;
    
  MemberDef* data_md = TA_ControlPanelMember.members.FindName("data");

  bool param_set = false;
  dat_cnt = 0;  // keeps track of control count
  if(ctrlpan->InheritsFrom(&TA_ParamSet)) { // other types have more advanced edits
    param_set = true;
    MemberSpace& ms = ctrlpan->GetTypeDef()->members;
    for (int i = 0; i < ms.size; ++i) {
      MemberDef* md = ms.FastEl(i);
      if ((md->name == "name") || (md->name == "desc") ||
          md->HasOption("CONTROL_PANEL_SHOW")) {
        prop_membs.memb_el.Add(md);
        // Create data widget
        taiWidget* mb_dat = md->im->GetWidgetRep(this, NULL, body);
        prop_membs.widget_el.Add(mb_dat);
        QWidget* rep = mb_dat->GetRep();
        // create label
        GetName(md, name, help_text);
        AddNameWidget(-1, name, help_text, rep, mb_dat, md);
        ++dat_cnt;
      }
    }
  }
  
  if (dat_cnt > 0) {  // no increment of data_cnt
    iLabel* lbl = NULL;
    lbl = new iLabel("Parameters --- Active (Current) Value ----- State ------- Saved Value --- ", body);
    // only for ParamSet
    AddSectionLabel(-1, lbl, "");
  }
  
  int set_idx = 0;
  membs.ResetItems();
  taiMemberWidgets* memb_set = NULL;
  
  // note: iterates non-empty groups only
  FOREACH_SUBGROUP(ControlPanelMember_Group, grp, ctrlpan->mbrs) {
    bool def_grp = (grp == &(ctrlpan->mbrs));// root group
    membs.SetMinSize(set_idx + 1);
    memb_set = membs.FastEl(set_idx);
    // make a group header
    if (!def_grp) {
      iLabel* lbl = new iLabel(grp->GetName(), body);
      AddSectionLabel(-1, lbl, "");
      dat_cnt++;                // these count toward rows!!
    }
    for (int i = 0; i < grp->size; ++i) {
      ControlPanelMember* item = grp->FastEl(i);
      
      if(param_set) {
        item->SavedToProgVar(); // update prog var rep
      }
      
      bool active_disable = item->IsLocked() || item->IsInactive() || item->IsParamSet();
      MemberDef* md = item->mbr;
      if (!md || (md->im == NULL))
        continue; // should only happen if created manually (Bad!)
      memb_set->memb_el.Add(md);

      taiWidgetMashup* mash_widg = taiWidgetMashup::New(false, md->type, this, NULL, body);
      mash_widg->SetMemberDef(md);
      mash_widg->add_labels = false;
      mash_widg->SetLayType(taiWidgetComposite::LT_Grid);
      mash_widg->InitLayout();
      mash_widg->AddChildMember(md, 1);
      if(item->data.saved_obj.ptr()) { // we're using a saved_obj rep
        mash_widg->AddChildMember(md, 2, true); // true = type only!  key!
        mash_widg->AddChildMember(data_md, 3);
        if (item->IsLocked() || item->IsInactive()) {
          mash_widg->widgets(1)->setEnabled(false); // inactivated
        }
      }
      else {
        mash_widg->AddChildMember(data_md, 2);
      }
      mash_widg->EndLayout();
 
      //          taiWidget* mb_dat = md->im->GetWidgetRep(this, NULL, body);
      memb_set->widget_el.Add(mash_widg);
      QWidget* data = mash_widg->GetRep();
      if (active_disable) {
        mash_widg->widgets(0)->setEnabled(false); // first one is active guy
      }
        
      help_text = item->GetDesc();
      String new_lbl = item->caption();
      AddNameWidget(-1, new_lbl, help_text, data, mash_widg, md);
      ++dat_cnt;

      if(item->IsParamSet() && !item->IsInactive()) {
        if(!item->ActiveEqualsSaved()) {
          MarkRowException(dat_cnt);
        }
      }
    }
    def_grp = false;
    ++set_idx;
  }
}

void taiEditorOfControlPanelFull::DoRemoveFmCtrlPanel() {
  taProject* proj = (taProject*)((taBase*)root)->GetThisOrOwner(&TA_taProject);
  if (!proj)
    return;
  
   // removes the ctrl_panel_index item
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    proj->undo_mgr.SaveUndo(ctrlpan, "RemoveMemberIdx", ctrlpan);
    ctrlpan->RemoveMemberIdx(ctrl_panel_index);
  }
}

void taiEditorOfControlPanelFull::DoGoToObject() {
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    ControlPanelMember* mbr = ctrlpan->mbrs.Leaf(ctrl_panel_index);
    if(mbr) {
      mbr->GoToObject();
    }
  }
}

void taiEditorOfControlPanelFull::DoCopyToAllInGroup() {
  taProject* proj = (taProject*)((taBase*)root)->GetThisOrOwner(&TA_taProject);
  if (!proj)
    return;
  
   // removes the ctrl_panel_index item
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    proj->undo_mgr.SaveUndo(ctrlpan, "DoCopyToAllInGroup", ctrlpan);
    ControlPanelMember* mbr = ctrlpan->mbrs.Leaf(ctrl_panel_index);
    if(mbr) {
      mbr->CopyToAllInGroup();
    }
  }
}

void taiEditorOfControlPanelFull::DoCopyStateToAllInGroup() {
  taProject* proj = (taProject*)((taBase*)root)->GetThisOrOwner(&TA_taProject);
  if (!proj)
    return;
  
   // removes the ctrl_panel_index item
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    proj->undo_mgr.SaveUndo(ctrlpan, "DoCopyStateToAllInGroup", ctrlpan);
    ControlPanelMember* mbr = ctrlpan->mbrs.Leaf(ctrl_panel_index);
    if(mbr) {
      mbr->CopyStateToAllInGroup();
    }
  }
}

void taiEditorOfControlPanelFull::DoMoveToTop() {
  taProject* proj = (taProject*)((taBase*)root)->GetThisOrOwner(&TA_taProject);
  if (!proj)
    return;
  
   // removes the ctrl_panel_index item
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    proj->undo_mgr.SaveUndo(ctrlpan, "DoMoveToTop", ctrlpan);
    ControlPanelMember* mbr = ctrlpan->mbrs.Leaf(ctrl_panel_index);
    if(mbr) {
      mbr->MoveToTop();
    }
  }
}

void taiEditorOfControlPanelFull::DoMoveToBottom() {
  taProject* proj = (taProject*)((taBase*)root)->GetThisOrOwner(&TA_taProject);
  if (!proj)
    return;
  
   // removes the ctrl_panel_index item
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    proj->undo_mgr.SaveUndo(ctrlpan, "DoMoveToBottom", ctrlpan);
    ControlPanelMember* mbr = ctrlpan->mbrs.Leaf(ctrl_panel_index);
    if(mbr) {
      mbr->MoveToBottom();
    }
  }
}

void taiEditorOfControlPanelFull::DoMoveToCtrlPanel(QAction* act) {
  taProject* proj = (taProject*)((taBase*)root)->GetThisOrOwner(&TA_taProject);
  if (!proj)
    return;
  
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index < 0) {
    return;
  }

  void* vval = act->data().value<void*>();
  if(!vval) return;
  taBase* bval = (taBase*)vval;
  if(bval->InheritsFrom(&TA_ControlPanel)) {
    ControlPanel* cp = (ControlPanel*)bval;
    proj->undo_mgr.SaveUndo(ctrlpan, "MoveMemberToCtrlPanelIdx"); // proj save
    ctrlpan->MoveMemberToCtrlPanelIdx(ctrl_panel_index, cp);
  }
  else {                        // must be a group
    ControlPanel_Group* cp = (ControlPanel_Group*)bval;
    proj->undo_mgr.SaveUndo(ctrlpan, "MoveMemberToCtrlPanelGpIdx"); // proj save
    ctrlpan->MoveMemberToCtrlPanelGpIdx(ctrl_panel_index, cp);
  }
}

void taiEditorOfControlPanelFull::DoSaveCurrent() {
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    ((ParamSet*)ctrlpan)->CopyActiveToSaved_item(ctrl_panel_index);
  }
}

void taiEditorOfControlPanelFull::DoActivate() {
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    ((ParamSet*)ctrlpan)->CopySavedToActive_item(ctrl_panel_index);
  }
}

void taiEditorOfControlPanelFull::DoEditLabel() {
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    ctrlpan->EditLabel(ctrl_panel_index);
  }
}

void taiEditorOfControlPanelFull::FillLabelContextMenu_CtrlPanel(QMenu* menu,
  int& last_id)
{
  taProject* proj = (taProject*)ctrlpan->GetThisOrOwner(&TA_taProject);
  if (!proj) return;
  
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_mbr, ctrl_panel_base);
  if (ctrl_panel_index < 0)
    return;
  if (ctrlpan->InheritsFrom(&TA_ParamSet)) {
    menu->addAction("Save Current", this, SLOT(DoSaveCurrent()));
    menu->addAction("Activate", this, SLOT(DoActivate()));
    menu->addAction("Copy To All In Group", this, SLOT(DoCopyToAllInGroup()));
    menu->addAction("Copy State To All In Group", this, SLOT(DoCopyStateToAllInGroup()));
  }
  menu->addAction("Remove", this, SLOT(DoRemoveFmCtrlPanel()));
  menu->addAction("Go To Object", this, SLOT(DoGoToObject()));
  menu->addAction("Edit Label", this, SLOT(DoEditLabel()));
  menu->addAction("Move To Top", this, SLOT(DoMoveToTop()));
  menu->addAction("Move To Bottom", this, SLOT(DoMoveToBottom()));
  QMenu* move_sub = menu->addMenu("Move To");
  connect(move_sub, SIGNAL(triggered(QAction*)), this, SLOT(DoMoveToCtrlPanel(QAction*)));
  move_sub->setFont(menu->font());

  // todo: would be cleaner to generalize this -- also used in taiEditorWidgets
  const int n_types = 7;
  const char* type_names[n_types] = {"ControlPanel", "ClusterRun", "ParamSet",
                                     "ParamStep", "ControlPanel_Group", "ParamSet_Group",
                                     "ParamStep_Group"
  };

  for(int ti=0; ti<n_types; ti++) {
    TypeDef* cptd = taMisc::FindTypeName(type_names[ti], false);
    if(cptd) {
      for (int i = 0; i < cptd->tokens.size; ++i) {
        void* tok = cptd->tokens[i];
        if(!tok) continue;
        taBase* cp = (taBase*)tok;
        if(!cp->SameScope(proj, &TA_taProject)) continue;
        taBase* agp = cp->GetOwner(&TA_ArchivedParams_Group);
        if(agp) continue;       // skip all archived elements
        String nm = cp->GetName();
        bool is_cp = false;
        ControlPanel* cpr = NULL;
        if(cpr == ctrlpan) continue; // don't include self!
        ControlPanel_Group* gp = NULL;
        if(cp->InheritsFrom(&TA_ControlPanel)) {
          is_cp = true;
          cpr = (ControlPanel*)cp;
          if(cpr->IsClone()) continue; // don't clutter with clones!
        }
        else {                  // group
          gp = (ControlPanel_Group*)cp; // could omit master/clone groups but may be easier
          // to think in terms of group or master element, depending..
          if(gp == &(proj->active_params)) continue;
          nm += " (Group)";
        }
        
        QAction* move_act = move_sub->addAction(nm);
        move_act->setData(QVariant::fromValue((void*)cp));
      }
    }
  }
}

void taiEditorOfControlPanelFull::GetImage_Membs_def() {
  bool param_set = false;
  if(ctrlpan->InheritsFrom(&TA_ParamSet)) { // other types have more advanced edits
    param_set = true;
    taBase* rbase = Base();
    for (int i = 0; i < prop_membs.widget_el.size; ++i) {
      taiWidget* mb_dat = prop_membs.widget_el.FastEl(i);
      MemberDef* md = prop_membs.memb_el.SafeEl(i);
      if (md) {
        mb_dat->SetBase(rbase); // used for things like Ctrlpandit context menu
        md->im->GetImage(mb_dat, rbase);
      }
    }
  }

  int itm_idx = 0;;
  for (int j = 0; j < membs.size; ++j) {
    taiMemberWidgets* ms = membs.FastEl(j);
    for (int i = 0; i < ms->widget_el.size; ++i) {
      taiWidget* mb_dat = ms->widget_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      ControlPanelMember* item = ctrlpan->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL) || item->mbr == NULL) {
        // taMisc::DebugInfo("taiEditorOfControlPanelFull::GetImage_Membs_def(): unexpected md or mb_dat=NULL at i ", String(i));
      }
      else {
        
        if(param_set) {
          item->SavedToProgVar(); // update prog var rep
        }
      
        taiWidgetMashup* mash_widg = dynamic_cast<taiWidgetMashup*>(mb_dat);
        if(mash_widg) {
          mash_widg->SetBases(NULL);
          mash_widg->AddBase(item->base);
          if(item->data.saved_obj.ptr()) { // we're using a saved_obj rep
            mash_widg->AddBase(item->data.saved_obj.ptr());
          }
          mash_widg->AddBase(item);
          mash_widg->GetImage();
        }
        else {
          md->im->GetImage(mb_dat, item->base); // need to do this first, to affect visible
        }
      }
      ++itm_idx;
    }
  }
}

void taiEditorOfControlPanelFull::GetValue_Membs_def() {
  bool param_set = false;
  if(ctrlpan->InheritsFrom(&TA_ParamSet)) { // other types have more advanced edits
    param_set = true;
    taBase* rbase = Base();
    for (int i = 0; i < prop_membs.widget_el.size; ++i) {
      taiWidget* mb_dat = prop_membs.widget_el.FastEl(i);
      MemberDef* md = prop_membs.memb_el.SafeEl(i);
      if (md) {
        bool first_diff = true;
        md->im->GetMbrValue(mb_dat, rbase, first_diff);
        rbase->UpdateAfterEdit();
      }
    }
  }
  
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    taiMemberWidgets* ms = membs.FastEl(j);
    for (int i = 0; i < ms->widget_el.size; ++i) {
      taiWidget* mb_dat = ms->widget_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      ControlPanelMember* item = ctrlpan->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL) || item->mbr == NULL) {
        // taMisc::DebugInfo("taiEditorOfControlPanelFull::GetValue_Membs_def(): unexpected md or mb_dat=NULL at i ", String(i));
      }
      else {
        bool first_diff = true;
        taiWidgetMashup* mash_widg = dynamic_cast<taiWidgetMashup*>(mb_dat);
        if(mash_widg) {
          item->member_activating = true; // prevent updates from UAE here!
          mash_widg->GetValue();
          item->member_activating = false;
          if(param_set) {
            item->ProgVarToSaved(); // copy back to string rep
            item->ActivateAfterEdit();
          }
        }
        else {
          md->im->GetMbrValue(mb_dat, item->base, first_diff);
          if (!first_diff)
            taiMember::EndScript(item->base);
          item->base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
        }
      }
      ++itm_idx;
    }
  }
}


