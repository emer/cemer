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

#include "taiSigLink.h"
#include <taSigLink_QObj>
#include <TypeDef>
#include <taiViewType>
#include <iTreeViewItem>
#include <iTreeView>
#include <taiTreeNode>

String taiSigLink::AnonymousItemName(const String& type_name, int index) {
  // makes name in form [index]:TypeName
  String rval = "[" + String(index) + "]:" + type_name;
  return rval;
}

taiSigLink::taiSigLink(void* data_, taSigLink* &link_ref_)
  : inherited(data_, link_ref_)
{
  qobj = 0;
}

taiSigLink::~taiSigLink() {
  delete qobj;
  qobj = 0;
}

void taiSigLink::Assert_QObj() {
  if (!qobj) {
    qobj = new taSigLink_QObj(this);
  }
}

iPanelBase* taiSigLink::CreateDataPanel() {
  return CreateDataPanel_impl();
}

iPanelBase* taiSigLink::CreateDataPanel_impl() {
  iPanelBase* rval = NULL;
  TypeDef* typ = GetDataTypeDef();
  //typ can be null for non-taBase classes
  if ((typ == NULL) || (typ->iv == NULL)) return NULL;
  taiViewType* tiv = typ->iv;
  rval = tiv->CreateDataPanel(this);
  return rval;
}


taiTreeNode* taiSigLink::CreateTreeDataNode(MemberDef* md, taiTreeNode* parent,
  taiTreeNode* after, const String& node_name, int dn_flags)
{
  return CreateTreeDataNode(md, parent, NULL, after, node_name, dn_flags);
}

taiTreeNode* taiSigLink::CreateTreeDataNode(MemberDef* md, iTreeView* parent,
  taiTreeNode* after, const String& node_name, int dn_flags)
{
  return CreateTreeDataNode(md, NULL, parent, after, node_name, dn_flags);
}

taiTreeNode* taiSigLink::CreateTreeDataNode(MemberDef* md, taiTreeNode* nodePar,
  iTreeView* tvPar, taiTreeNode* after, const String& node_name, int dn_flags)
{
  if (HasChildItems()) {
    //note: the list item automatically enables lazy children
    dn_flags |= iTreeViewItem::DNF_LAZY_CHILDREN;
  }
  taiTreeNode* rval = CreateTreeDataNode_impl(md, nodePar, tvPar, after, node_name, dn_flags);
  if (rval) {
    rval->DecorateDataNode();
  }
  return rval;
}

void taiSigLink::FillContextMenu(taiWidgetActions* menu) {
  FillContextMenu_impl(menu);
}

