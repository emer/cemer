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

#include "taiTreeNodeTaBase.h"

#include <SigLinkSignal>
#include <iLineEdit>

#include <taMisc>

taiTreeNodeTaBase::taiTreeNodeTaBase(taSigLinkTaBase* link_, MemberDef* md_, taiTreeNode* parent_,
  taiTreeNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taiSigLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

taiTreeNodeTaBase::taiTreeNodeTaBase(taSigLinkTaBase* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taiSigLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void taiTreeNodeTaBase::init(taSigLinkTaBase* link_, int dn_flags_) {
  taBase* tab = tadata();
  if(tab && tab->BrowserEditEnable()) {
    setFlags(flags() | Qt::ItemIsEditable);
  }
}

taiTreeNodeTaBase::~taiTreeNodeTaBase()
{
}

void taiTreeNodeTaBase::SigEmit_impl(int sls, void* op1_, void* op2_) {
  inherited::SigEmit_impl(sls, op1_, op2_);
  bool do_updt = false;
  taBase* tab = tadata();
  if(tab) {
    TypeDef* base_typ = tab->GetTypeDef();
    if(base_typ->HasOption("HAS_CONDTREE")) {
      if(sls == SLS_ITEM_UPDATED)
        do_updt = true;
    }
  }
  if(do_updt || sls == SLS_STRUCT_UPDATE_ALL) { // special case for post-loading update
    takeChildren();
    CreateChildren();
    iTreeView* itv = treeView();
    if(!itv) return;
    if(itv->itemCount() > 0) {
      if(itv->item(0) == this)
        itv->ExpandDefault();
      else if(do_updt) {
        itv->ExpandDefaultUnder(this);
      }
    }
  }
}

void taiTreeNodeTaBase::itemEdited(int column, int move_after) {
  taBase* tab = tadata();
  if(!tab) return;
  String new_val = text(column);
  tab->BrowserEditSet(new_val, move_after);
}

void taiTreeNodeTaBase::lookupKeyPressed(iLineEdit* le, int column) {
  taBase* tab = tadata();
  if(!tab) return;

  TypeDef* td = tab->GetTypeDef();
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("BROWSER_EDIT_LOOKUP")) continue;
    if(!md->type->IsTaBase()) {
      taMisc::Warning("Programmer error in taiTreeNodeTaBase::lookupKeyPressed -- #BROWSER_EDIT_LOOKUP on member:", md->name, "type must be a taBase");
      return;
    }      
    taBase* bel = (taBase*)md->GetOff(tab);
    int cur_pos = le->cursorPosition();
    int new_pos = -1;
    String rval = bel->StringFieldLookupFun(le->text(), cur_pos, "", new_pos);
#ifdef TA_OS_MAC
  // per this bug with 2.8.x on mac, we need to regain focus:  https://bugreports.qt-project.org/browse/QTBUG-22911
    le->window()->setFocus();
    le->setFocus();
#endif
    if(rval.nonempty()) {
      le->setText(rval);
      if(new_pos >= 0)
        le->setCursorPosition(new_pos); // go back to orig pos
      else
        le->setCursorPosition(cur_pos); // go back to orig pos
    }
  }
}
