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

#include "taSigLinkTypeItem.h"
#include <taTypeInfoTreeDataNode>
#include <TypeDef>
#include <taiEditorOfWidgetsClass>
#include <taiEditorWidgetsOfTypeItem>
#include <EditDataPanel>

taSigLinkTypeItem::taSigLinkTypeItem(TypeItem::TypeInfoKinds tik_, TypeItem* data_)
:inherited(tik_, data_, data_->sig_link)
{
}

taiTreeDataNode* taSigLinkTypeItem::CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  taiTreeDataNode* rval = NULL;
  if (nodePar)
    rval = new taTypeInfoTreeDataNode(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new taTypeInfoTreeDataNode(this, md, tvPar, after, node_name, dn_flags);
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
    TypeDef* td = static_cast<TypeDef*>(data());
    // if (td->internal && !td->IsTemplate()) {
    //   break;
    // }
    rval = true;
    }
  default:break; // compiler food
  }
  return rval;
}

String taSigLinkTypeItem::GetName() const {
  return data()->name;
}

String taSigLinkTypeItem::GetDisplayName() const {
  return data()->name;
}

iDataPanel* taSigLinkTypeItem::CreateDataPanel_impl() {
  taiEditorWidgetsOfTypeItem* tidh = 
    new taiEditorWidgetsOfTypeItem(data(), tik, true, false);
  tidh->Constr("", "", taiEditorOfWidgetsMain::HT_PANEL);
  EditDataPanel* dp = tidh->EditPanel(this);
  return dp;
} 


bool taSigLinkTypeItem::ShowMember(MemberDef* md) {
  return false; // na
}


