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

#include "taTypeInfoDataLink.h"
#include <taTypeInfoTreeDataNode>
#include <TypeDef>
#include <taiEditDataHost>
#include <taiTypeItemDataHost>
#include <EditDataPanel>

taTypeInfoDataLink::taTypeInfoDataLink(TypeItem::TypeInfoKinds tik_, TypeItem* data_)
:inherited(tik_, data_, data_->data_link)
{
}

taiTreeDataNode* taTypeInfoDataLink::CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  taiTreeDataNode* rval = NULL;
  if (nodePar)
    rval = new taTypeInfoTreeDataNode(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new taTypeInfoTreeDataNode(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}

bool taTypeInfoDataLink::HasChildItems() {
  bool rval = false;
  switch(tik) {
  case TypeItem::TIK_ENUM: break;
  case TypeItem::TIK_MEMBER: break;
  case TypeItem::TIK_PROPERTY: break;
  case TypeItem::TIK_METHOD: break;
  case TypeItem::TIK_TYPE: {
    TypeDef* td = static_cast<TypeDef*>(data());
    // if (td->internal && !td->InheritsFormal(TA_template)) {
    //   break;
    // }
    rval = true;
    }
  default:break; // compiler food
  }
  return rval;
}

String taTypeInfoDataLink::GetName() const {
  return data()->name;
}

String taTypeInfoDataLink::GetDisplayName() const {
  return data()->name;
}

iDataPanel* taTypeInfoDataLink::CreateDataPanel_impl() {
  taiTypeItemDataHost* tidh = 
    new taiTypeItemDataHost(data(), tik, true, false);
  tidh->Constr("", "", taiDataHost::HT_PANEL);
  EditDataPanel* dp = tidh->EditPanel(this);
  return dp;
} 


bool taTypeInfoDataLink::ShowMember(MemberDef* md) {
  return false; // na
}


