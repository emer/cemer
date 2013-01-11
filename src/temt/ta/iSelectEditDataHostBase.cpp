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

#include "iSelectEditDataHostBase.h"

void iSelectEditDataHostBase::iSelectEditDataHostBase(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, false, parent)
{
  Initialize();
  sele = (SelectEdit*)base;
}

iSelectEditDataHostBase::~iSelectEditDataHostBase() {
}

void iSelectEditDataHostBase::Initialize()
{
  no_meth_menu = true; // only show them on outer menu, by way of Propertiesguy
  sele = NULL;
  sel_edit_mbrs = true; // note: we don't actually select members, just for removal
}

void iSelectEditDataHostBase::Constr_Body() {
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

void iSelectEditDataHostBase::Constr_Methods_impl() {
  inherited::Constr_Methods_impl();
  Insert_Methods();

  FOREACH_SUBGROUP(EditMthItem_Group, grp, sele->mths) {
    //note: root group uses only buttons (hard wired)
    EditMthItem_Group::MthGroupType group_type = grp->group_type;

    // make a menu or button group if needed
    String men_nm = grp->GetDisplayName(); // blank for default
    switch (group_type) {
    case EditMthItem_Group::GT_MENU: {
      SetCurMenu_Name(men_nm); // default is "Actions"
    } break;
    case EditMthItem_Group::GT_MENU_BUTTON: {
      if (men_nm.empty()) // shouldn't happen
        men_nm = "Actions";
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
      if (!md || (md->im == NULL) || (base == NULL)) continue;
      taiMethod* im = md->im;
      if (im == NULL) continue;

      //NOTE: for seledit functions, we never place them on the last menu or button, because that may
      // make no sense -- the label specifies the place, or Actions if no label
      String mth_cap = item->caption();
      String statustip = item->desc;
      taiMethodData* mth_rep = NULL;
      switch (group_type) {
      case EditMthItem_Group::GT_BUTTONS:  {
        mth_rep = im->GetButtonMethodRep(base, this, NULL, frmMethButtons);
        AddMethButton(mth_rep, mth_cap);
      } break;
      case EditMthItem_Group::GT_MENU: {
        mth_rep = im->GetMenuMethodRep(base, this, NULL, NULL/*frmMethButtons*/);
//        mth_rep->AddToMenu(cur_menu);
        taiAction* act = cur_menu->AddItem(mth_cap, taiMenu::use_default,
              taiAction::action, mth_rep, SLOT(CallFun()) );
        if (statustip.nonempty())
          act->setStatusTip(statustip);
      } break;
      case EditMthItem_Group::GT_MENU_BUTTON: {
        mth_rep = im->GetMenuMethodRep(base, this, NULL, NULL/*frmMethButtons*/);
//        mth_rep->AddToMenu(cur_menu_but);
        taiAction* act = cur_menu_but->AddItem(mth_cap, taiMenu::use_default,
              taiAction::action, mth_rep, SLOT(CallFun()) );
        if (statustip.nonempty())
          act->setStatusTip(statustip);
      } break;
      } // switch group_type
      if (mth_rep)
        meth_el.Add(mth_rep);
    }
  } // groups
}

taBase* iSelectEditDataHostBase::GetMembBase_Flat(int idx) {
  return sele->mbrs.GetBase_Flat(idx);
}

taBase* iSelectEditDataHostBase::GetMethBase_Flat(int idx) {
  return sele->mths.GetBase_Flat(idx);
}


void iSelectEditDataHostBase::mnuRemoveMember_select(int idx) {
  sele->RemoveField(idx);
}

void iSelectEditDataHostBase::mnuRemoveMethod_select(int idx) {
  sele->RemoveFun(idx);
}


