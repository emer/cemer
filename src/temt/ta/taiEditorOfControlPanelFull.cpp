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
#include <taiWidgetMenuBar>
#include <taiWidgetMashup>
#include <iHiLightButton>

#include <taMisc>
#include <taiMisc>

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
  // delete all previous sele members (sele presumably stands for "SelectEdit" the old name for ControlPanel)
  String name;
  String help_text;
  
  dat_cnt = 0;  // keeps track of control count
  if (sele->InheritsFrom(&TA_ParamSet)) {
    MemberSpace& ms = sele->GetTypeDef()->members;
    for (int i = 0; i < ms.size; ++i) {
      MemberDef* md = ms.FastEl(i);
      if ((md->name == "name") || (md->name == "desc")) {
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
    lbl = new iLabel("Parameters", body);
    AddSectionLabel(-1, lbl, "");
  }
  
  int set_idx = 0;
  membs.ResetItems();
  taiMemberWidgets* memb_set = NULL;
  
  // note: iterates non-empty groups only
  FOREACH_SUBGROUP(EditMbrItem_Group, grp, sele->mbrs) {
    bool def_grp = (grp == &(sele->mbrs));// root group
    membs.SetMinSize(set_idx + 1);
    memb_set = membs.FastEl(set_idx);
    // make a group header
    if (!def_grp) {
      iLabel* lbl = new iLabel(grp->GetName(), body);
      AddSectionLabel(-1, lbl,
                      "");
    }
    for (int i = 0; i < grp->size; ++i) {
      EditMbrItem* item = grp->FastEl(i);
      MemberDef* md = item->mbr;
      if (!md || (md->im == NULL))
        continue; // should only happen if created manually (Bad!)
      memb_set->memb_el.Add(md);
      bool added_search = false;
      if(item->is_single && sele->InheritsFrom(&TA_ClusterRun)) {
        MemberDef* psmd = TA_EditMbrItem.members.FindName("param_search");
        if (psmd) {
          added_search = true;
          taiWidgetMashup* mash_widg = taiWidgetMashup::New(false, md->type, this, NULL, body);
          mash_widg->SetMemberDef(md);
          mash_widg->add_labels = false;
          mash_widg->InitLayout();
          mash_widg->AddChildMember(md);
          mash_widg->AddChildMember(psmd);
          mash_widg->EndLayout();
          
          memb_set->widget_el.Add(mash_widg);
          QWidget* data = mash_widg->GetRep();
          help_text = item->GetDesc();
          String new_lbl = item->caption();
          AddNameWidget(-1, new_lbl, help_text, data, mash_widg, md);
          ++dat_cnt;
        }
      }
      
      // if this panel is a param set panel
      bool added_param_set = false;
      if(sele->InheritsFrom(&TA_ParamSet)) {
        MemberDef* psmd = TA_EditMbrItem.members.FindName("param_set_value");
        if (psmd) {
          added_param_set = true;
          taiWidgetMashup* mash_widg = taiWidgetMashup::New(false, md->type, this, NULL, body);
          mash_widg->SetMemberDef(md);
          mash_widg->add_labels = false;
          mash_widg->InitLayout();
          mash_widg->AddChildMember(md);
          mash_widg->AddChildMember(psmd);
          mash_widg->EndLayout();
          
          memb_set->widget_el.Add(mash_widg);
          QWidget* data = mash_widg->GetRep();
          help_text = item->GetDesc();
          String new_lbl = item->caption();
          AddNameWidget(-1, new_lbl, help_text, data, mash_widg, md);
          ++dat_cnt;
        }
      }
      
      if(!added_search && !added_param_set) {
        taiWidget* mb_dat = md->im->GetWidgetRep(this, NULL, body);
        memb_set->widget_el.Add(mb_dat);
        QWidget* data = mb_dat->GetRep();
        help_text = item->GetDesc();
        String new_lbl = item->caption();
        AddNameWidget(-1, new_lbl, help_text, data, mb_dat, md);
        ++dat_cnt;
      }
    }
    def_grp = false;
    ++set_idx;
  }
}

void taiEditorOfControlPanelFull::DoRemoveSelEdit() {
   // removes the sel_item_index item
  int sel_item_index = membs.GetFlatWidgetIndex(sel_item_dat);
  if (sel_item_index >= 0) {
    sele->RemoveField(sel_item_index);
  }
  else
    taMisc::DebugInfo("taiEditorOfControlPanelFull::DoRemoveSelEdit: could not find item");
}

void taiEditorOfControlPanelFull::FillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id)
{
  int sel_item_index = membs.GetFlatWidgetIndex(sel_item_mbr, sel_item_base);
  if (sel_item_index < 0)
    return;
  if (sele->InheritsFrom(&TA_ParamSet)) {
    menu->addAction("Remove from ParamSet", this, SLOT(DoRemoveSelEdit()));
  }
  else {
    menu->addAction("Remove from ControlPanel", this, SLOT(DoRemoveSelEdit()));
  }
  
}

void taiEditorOfControlPanelFull::GetImage_Membs_def() {
  if (sele->InheritsFrom(&TA_ParamSet)) {
    for (int i = 0; i < prop_membs.widget_el.size; ++i) {
      taiWidget* mb_dat = prop_membs.widget_el.FastEl(i);
      MemberDef* md = prop_membs.memb_el.SafeEl(i);
      if (md) {
        md->im->taiType::GetImage(mb_dat, mb_dat->Base());
      }
    }
  }

  //  cur_row = 0;
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    taiMemberWidgets* ms = membs.FastEl(j);
    for (int i = 0; i < ms->widget_el.size; ++i) {
      taiWidget* mb_dat = ms->widget_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL) || item->mbr == NULL) {
        taMisc::DebugInfo("taiEditorOfControlPanelFull::GetImage_Membs_def(): unexpected md or mb_dat=NULL at i ", String(i));
      }
      else {
        taiWidgetMashup* mash_widg = dynamic_cast<taiWidgetMashup*>(mb_dat);
        if(mash_widg) {
          mash_widg->SetBases(item->base, item);
          mash_widg->GetImage();
        }
        else {
          md->im->GetImage(mb_dat, item->base); // need to do this first, to affect visible
        }
      }
      ++itm_idx;
//      cur_row++;
    }
  }
}

void taiEditorOfControlPanelFull::GetValue_Membs_def() {
  if (sele->InheritsFrom(&TA_ParamSet)) {
    for (int i = 0; i < prop_membs.widget_el.size; ++i) {
      taiWidget* mb_dat = prop_membs.widget_el.FastEl(i);
      MemberDef* md = prop_membs.memb_el.SafeEl(i);
      if (md) {
        bool first_diff = true;
        md->im->GetMbrValue(mb_dat, mb_dat->Base(), first_diff);
        mb_dat->Base()->UpdateAfterEdit();
      }
    }
  }
  
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    taiMemberWidgets* ms = membs.FastEl(j);
    for (int i = 0; i < ms->widget_el.size; ++i) {
      taiWidget* mb_dat = ms->widget_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL) || item->mbr == NULL) {
        taMisc::DebugInfo("taiEditorOfControlPanelFull::GetValue_Membs_def(): unexpected md or mb_dat=NULL at i ", String(i));
      }
      else {
        bool first_diff = true;
        taiWidgetMashup* mash_widg = dynamic_cast<taiWidgetMashup*>(mb_dat);
        if(mash_widg) {
          mash_widg->GetValue();
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


