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

#include "taiTreeNodeProgEl.h"
#include <ProgEl>
#include <ProgExpr>
#include <iLineEdit>

#include <taMisc>

taiTreeNodeProgEl::taiTreeNodeProgEl(taSigLinkTaBase* link_, MemberDef* md_, taiTreeNode* parent_,
  taiTreeNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

taiTreeNodeProgEl::taiTreeNodeProgEl(taSigLinkTaBase* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void taiTreeNodeProgEl::init(taSigLinkTaBase* link_, int dn_flags_) {
  // todo: could modulate by some kind of settings
  setFlags(flags() | Qt::ItemIsEditable);
}

taiTreeNodeProgEl::~taiTreeNodeProgEl()
{
}

void taiTreeNodeProgEl::itemEdited(int column) {
  ProgEl* pel = dynamic_cast<ProgEl*>(tadata());
  if(!pel) return;
  String new_val = text(column);
  pel->CodeFromText(new_val);
}

void taiTreeNodeProgEl::lookupKeyPressed(iLineEdit* le, int column) {
  ProgEl* pel = dynamic_cast<ProgEl*>(tadata());
  if(!pel) return;

  taMisc::Info("lookup key pressed on:", pel->GetDisplayName());

  TypeDef* td = pel->GetTypeDef();
  MemberDef* pem = td->members.FindType(&TA_ProgExpr);
  if(pem) {
    ProgExpr* pe = (ProgExpr*)pem->GetOff(pel);
    int cur_pos = le->cursorPosition();
    int new_pos = -1;
    String rval = pe->StringFieldLookupFun(le->text(), cur_pos, "", new_pos);
    // TODO: oops -- the editor is destroyed at this point!
    return;
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
  //                                         lookupfun_md->name, new_pos);
}
