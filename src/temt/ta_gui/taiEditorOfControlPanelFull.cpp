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

#include "taiEditorOfControlPanelFull.h"
#include <ClusterRun>
#include <ParamSet>
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
    lbl = new iLabel("Parameters --- Active (Current) Value --- State --- Saved Value --- ", body);
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
      
      bool active_disable = item->IsLocked() || item->IsParamSet();
      MemberDef* md = item->mbr;
      if (!md || (md->im == NULL))
        continue; // should only happen if created manually (Bad!)
      memb_set->memb_el.Add(md);

      // don't let the mashup do any uae by itself -- we retain full control over that!
      taiWidgetMashup* mash_widg = taiWidgetMashup::New(false, md->type, this, NULL, body);
                                                        // taiWidgetMashup::flgNoUAE);
      mash_widg->SetMemberDef(md);
      mash_widg->add_labels = false;
      mash_widg->SetLayType(taiWidgetComposite::LT_Grid);
      mash_widg->InitLayout();
      mash_widg->AddChildMember(md, 1);
      mash_widg->AddChildMember(data_md, 2);
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

      if(ctrlpan->InheritsFrom(&TA_ParamSet)) {
        if(!((ParamSet*)ctrlpan)->ActiveEqualsSaved(item->GetName())) {
          MarkRowException(dat_cnt);
        }
      }
    }
    def_grp = false;
    ++set_idx;
  }
}

void taiEditorOfControlPanelFull::DoRemoveFmCtrlPanel() {
   // removes the ctrl_panel_index item
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    ctrlpan->RemoveMemberIdx(ctrl_panel_index);
  }
  else {
    taMisc::DebugInfo("taiEditorOfControlPanelFull::DoRemoveFmCtrlPanel: could not find item");
  }
}

void taiEditorOfControlPanelFull::DoGoToObject() {
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    ctrlpan->GoToObject(ctrl_panel_index);
  }
  else {
    taMisc::DebugInfo("taiEditorOfControlPanelFull::DoGoToObject: could not find item");
  }
}

void taiEditorOfControlPanelFull::DoCopyActiveToSaved() {
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    ((ParamSet*)ctrlpan)->CopyActiveToSaved_item(ctrl_panel_index);
  }
  else {
    taMisc::DebugInfo("taiEditorOfControlPanelFull::DoCopyActiveToSaved: could not find item");
  }
}

void taiEditorOfControlPanelFull::DoCopySavedToActive() {
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    ((ParamSet*)ctrlpan)->CopySavedToActive_item(ctrl_panel_index);
  }
  else {
    taMisc::DebugInfo("taiEditorOfControlPanelFull::DoCopySavedToActive: could not find item");
  }
}

void taiEditorOfControlPanelFull::DoEditLabel() {
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_dat);
  if (ctrl_panel_index >= 0) {
    ctrlpan->EditLabel(ctrl_panel_index);
  }
  else {
    taMisc::DebugInfo("taiEditorOfControlPanelFull::EditLabel: could not find item");
  }
}

void taiEditorOfControlPanelFull::FillLabelContextMenu_CtrlPanel(QMenu* menu,
  int& last_id)
{
  int ctrl_panel_index = membs.GetFlatWidgetIndex(ctrl_panel_mbr, ctrl_panel_base);
  if (ctrl_panel_index < 0)
    return;
  if (ctrlpan->InheritsFrom(&TA_ParamSet)) {
    menu->addAction("Remove from ParamSet", this, SLOT(DoRemoveFmCtrlPanel()));
    menu->addAction("Save Current", this, SLOT(DoCopyActiveToSaved()));
    menu->addAction("Activate", this, SLOT(DoCopySavedToActive()));
  }
  else {
    menu->addAction("Remove from ControlPanel", this, SLOT(DoRemoveFmCtrlPanel()));
  }
  menu->addAction("Go To Object", this, SLOT(DoGoToObject()));
  menu->addAction("Edit Label", this, SLOT(DoEditLabel()));
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

  //  cur_row = 0;
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
          for (int el=1; el<mash_widg->memb_el.size; el++) {
            mash_widg->AddBase(item);
          }
          mash_widg->GetImage();
        }
        else {
          md->im->GetImage(mb_dat, item->base); // need to do this first, to affect visible
        }
      }
      ++itm_idx;
//      cur_row++
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
          mash_widg->GetValue();
          if(param_set) {
            item->ProgVarToSaved(); // copy back to string rep
            item->UpdateAfterEdit(); // further update to allow active to update..
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


