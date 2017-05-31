// Copyright 2016, Regents of the University of Colorado,
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

#include "taSigLinkTypeItem.h"
#include <taiTreeNodeTypeItem>
#include <TypeDef>
#include <taiEditorOfClass>
#include <taiEditorOfTypeItem>
#include <iPanelOfEditor>

taSigLinkTypeItem::taSigLinkTypeItem(TypeItem::TypeInfoKinds tik_, TypeItem* data_)
:inherited(tik_, data_, data_->sig_link)
{
}

taiTreeNode* taSigLinkTypeItem::CreateTreeDataNode_impl(MemberDef* md, taiTreeNode* nodePar,
  iTreeView* tvPar, taiTreeNode* after, const String& node_name, int dn_flags)
{
  taiTreeNode* rval = NULL;
  if (nodePar)
    rval = new taiTreeNodeTypeItem(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new taiTreeNodeTypeItem(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}

bool taSigLinkTypeItem::HasChildItems() {
  bool rval = false;
  switch(tik) {
  case TypeItem::TIK_ENUM: break;
  case TypeItem::TIK_MEMBER: break;
  case TypeItem::TIK_PROPERTY: break;
  case TypeItem::TIK_METHOD: break;
  case TypeItem::TIK_TYPE: {
    rval = true;
  }
  default:break; // compiler food
  }
  return rval;
}

String taSigLinkTypeItem::GetName() const {
  return data()->name;
}

String taSigLinkTypeItem::GetDisplayName(bool desc_ok) const {
  return data()->name;
}

iPanelBase* taSigLinkTypeItem::CreateDataPanel_impl() {
  taiEditorOfTypeItem* tidh = 
    new taiEditorOfTypeItem(data(), tik, true, false);
  tidh->Constr("", "", taiEditorWidgetsMain::HT_PANEL);
  iPanelOfEditor* dp = tidh->EditPanel(this);
  return dp;
} 


bool taSigLinkTypeItem::ShowMember(MemberDef* md) {
  return false; // na
}


