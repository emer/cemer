// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ta_seledit_qt.h"

#include "ta_qt.h"
#include "ta_qtdialog.h" // for Hilight button
#include "ta_TA_inst.h"

#include <QColor>

//////////////////////////////////
//  tabSelectEditViewType 	//
/////////////////////////////////

int tabSelectEditViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_SelectEdit))
    return (inherited::BidForView(td) +1);
  return 0;
}

void tabSelectEditViewType::CreateDataPanel_impl(taiDataLink* dl_)
{
  // we create ours first, because it should be the default
  iSelectEditPanel* bldp = new iSelectEditPanel(dl_);
  DataPanelCreated(bldp);
  inherited::CreateDataPanel_impl(dl_);
}



//////////////////////////
//  iSelectEdit		//
//////////////////////////

iSelectEditDataHost::iSelectEditDataHost(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, false, parent)
{
  Initialize();
  sele = (SelectEdit*)base;
}

iSelectEditDataHost::~iSelectEditDataHost() {
}

void iSelectEditDataHost::Initialize()
{
  sele = NULL;
  sel_edit_mbrs = false; // not for us!!!
}

void iSelectEditDataHost::Constr_Body() {
  if (rebuild_body) {
    meth_el.Reset();
  }
  inherited::Constr_Body();
  // we deleted the normally not-deleted methods, so redo them here
  if (rebuild_body) {
    Constr_Methods();
    frmMethButtons->setHidden(!showMethButtons());
  }
}
  
void iSelectEditDataHost::ClearBody_impl() {
  // we also clear all the methods, and then rebuild them
  ta_menus.Reset();
  ta_menu_buttons.Reset();
//  meth_el.Reset(); // must defer deletion of these, because the MethodData objects are used in menu calls, so can't be
  layMethButtons = NULL;
  DeleteChildrenLater(frmMethButtons);
  show_meth_buttons = false;

  // note: no show menu in this class
  cur_menu = NULL;
  cur_menu_but = NULL;
  if (menu) {
    menu->Reset();
  }
  inherited::ClearBody_impl();
}

void iSelectEditDataHost::Constr_Data_Labels() {
  // delete all previous sele members
  membs.ResetItems();
  dat_cnt = 0;
  // mark place
  String nm;
  String help_text;
  MembSet* memb_set = NULL;
  
  bool def_grp = true; // first one
  taGroupItr itr;
  EditMbrItem_Group* grp;
  int set_idx = 0;
  FOR_ITR_GP(EditMbrItem_Group, grp, sele->mbrs., itr) {
    if (grp->size == 0) {def_grp = false; continue; }
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
      if (md->im == NULL) continue; // shouldn't happen
      taiData* mb_dat = md->im->GetDataRep(this, NULL, body);
      memb_set->memb_el.Add(md);
      memb_set->data_el.Add(mb_dat);
      QWidget* data = mb_dat->GetRep();
      int row = AddData(-1, data);
  
      help_text = item->GetDesc();
      String new_lbl = item->caption();
      //obs GetName(md, nm, help_text); //note: we just call this to get the help text
      AddName(row, new_lbl, help_text, mb_dat, md);
      ++dat_cnt;
    }
    def_grp = false;
    ++set_idx;
  }
}

void iSelectEditDataHost::Constr_Methods() {
  inherited::Constr_Methods();
  Insert_Methods();
  if (cur_menu != NULL) {// for safety... cur_menu should be the SelectEdit menu
    cur_menu->AddSep();
  }

  taGroupItr itr;
  EditMthItem_Group* grp;
  //int set_idx = 0;
  FOR_ITR_GP(EditMthItem_Group, grp, sele->mths., itr) {
    if (grp->size == 0) continue;
    //note: root group uses only buttons (hard wired)
    EditMthItem_Group::MthGroupType group_type = grp->group_type;
    
    // make a menu or button group if needed
    String men_nm = grp->GetDisplayName();
    if (men_nm.empty()) // shouldn't happen
      men_nm = "Actions";
    switch (group_type) {
    case EditMthItem_Group::GT_MENU: {
      cur_menu = ta_menus.FindName(men_nm);
      if (cur_menu == NULL) {
        cur_menu = menu->AddSubMenu(men_nm);
        ta_menus.Add(cur_menu);
      }
    } break;
    case EditMthItem_Group::GT_MENU_BUTTON: { 
      cur_menu_but = ta_menu_buttons.FindName(men_nm);
      if (cur_menu_but == NULL) {
        cur_menu_but = taiActions::New
          (taiMenu::buttonmenu, taiMenu::normal, taiMisc::fonSmall,
                  NULL, this, NULL, widget());
        cur_menu_but->setLabel(men_nm);
        DoAddMethButton((QPushButton*)cur_menu_but->GetRep()); // rep is the button for buttonmenu
        ta_menu_buttons.Add(cur_menu_but);
      }
    } break;
    default: break; // nothing for butts
    } // switch group_type
          
    for (int i = 0; i < grp->size; ++i) {
      EditMthItem* item = grp->FastEl(i);
      MethodDef* md = item->mth;
      taBase* base = item->base;
      if ((md->im == NULL) || (base == NULL)) continue;
      taiMethodData* mth_rep = md->im->GetMethodRep(base, this, NULL, frmMethButtons);
      if (mth_rep == NULL) continue;
      meth_el.Add(mth_rep);
  
      //NOTE: for seledit functions, we never place them on the last menu or button, because that may
      // make no sense -- the label specifies the place, or Actions if no label
      String mth_cap = item->caption();
      switch (group_type) {
      case EditMthItem_Group::GT_BUTTONS:  {
        AddMethButton(mth_rep, mth_cap);
      } break;
      case EditMthItem_Group::GT_MENU: {
        mth_rep->AddToMenu(cur_menu);
      } break;
      case EditMthItem_Group::GT_MENU_BUTTON: { 
        mth_rep->AddToMenu(cur_menu_but);
      } break;
      } // switch group_type
    }
  } // groups
}

void iSelectEditDataHost::DoRemoveSelEdit() {
   // removes the sel_item_index item 
  int sel_item_index = sel_item_idx;
  if (sel_item_index >= 0) {
    sele->RemoveField(sel_item_index);
  }
#ifdef DEBUG
  else
    taMisc::Error("iSelectEditDataHost::DoRemoveSelEdit: could not find item");
#endif
}

void iSelectEditDataHost::FillLabelContextMenu_SelEdit(iLabel* sender,
  QMenu* menu, int& last_id)
{
  int sel_item_index = sel_item_idx;
  if (sel_item_index < 0) return; 
  menu->insertItem("Remove from SelectEdit", this, SLOT(DoRemoveSelEdit()), 0, ++last_id);
}

QMenu* iSelectEditDataHost::FindMenuItem(QMenu* par_menu, const char* label) {
  int id = 0;
  for (uint i = 0; i < par_menu->count(); ++i) {
    id = par_menu->idAt(i);
    if (par_menu->text(id) == label)
      return (QMenu*)par_menu->findItem(id);
  }
  return NULL;
}

void iSelectEditDataHost::GetImage_Membs_def() {
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    MembSet* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      taiData* mb_dat = ms->data_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL))
        taMisc::Error("iSelectEditDataHost::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i), "\n");
      else {
        md->im->GetImage(mb_dat, item->base); // need to do this first, to affect visible
      }
      ++itm_idx;
    }
  }
}

void iSelectEditDataHost::GetValue_Membs_def() {
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    MembSet* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      taiData* mb_dat = ms->data_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL))
        taMisc::Error("iSelectEditDataHost::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i), "\n");
      else {
        bool first_diff = true;
        md->im->GetMbrValue(mb_dat, item->base, first_diff); 
        if (!first_diff)
          taiMember::EndScript(item->base);
	item->base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
      }
      ++itm_idx;
    }
  }
}

void iSelectEditDataHost::MakeMenuItem(QMenu* menu, const char* name, int index, int param, const char* slot) {
    menu->insertItem(name, this, slot, 0, index);
    menu->setItemParameter(index, param);
}

void iSelectEditDataHost::mnuRemoveMember_select(int idx) {
  sele->RemoveField(idx);
}

void iSelectEditDataHost::mnuRemoveMethod_select(int idx) {
  sele->RemoveFun(idx);
}



//////////////////////////
//   iSelectEditPanel 	//
//////////////////////////

iSelectEditPanel::iSelectEditPanel(taiDataLink* dl_)
:inherited(dl_)
{
  SelectEdit* se_ = sele();
  se = NULL;
  if (se_) {
    se = new iSelectEditDataHost(se_, se_->GetTypeDef());
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = se_->GetEditColorInherit(ok);
      if (ok) se->setBgColor(bgcol);
    }
/*    se->ConstrEditControl();
    setCentralWidget(se->widget()); //sets parent
    setButtonsWidget(se->widButtons);*/
  }
}

iSelectEditPanel::~iSelectEditPanel() {
  if (se) {
    delete se;
    se = NULL;
  }
}

void iSelectEditPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
}

bool iSelectEditPanel::HasChanged() {
  return se->HasChanged();
}

bool iSelectEditPanel::ignoreDataChanged() const {
  return !isVisible();
}

void iSelectEditPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  se->ConstrEditControl();
  setCentralWidget(se->widget()); //sets parent
  setButtonsWidget(se->widButtons);
}

void iSelectEditPanel::UpdatePanel_impl() {
  if (se) se->ReShow_Async();
}

void iSelectEditPanel::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (se && se->HasChanged()) {
    se->Apply();
  }
}


