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

#include "taTypeSpaceTreeDataNode.h"
#include <taiViewType>
#include <taSigLinkTypeItem>
#include <MethodDef>
#include <taSigLinkTypeSpace>


taTypeSpaceTreeDataNode::taTypeSpaceTreeDataNode(taSigLinkTypeItemSpace* link_, 
  MemberDef* md, taiTreeDataNode* parent_, 
  taiTreeDataNode* last_child_, const String& tree_name, int flags_)
:inherited(link_, md, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

taTypeSpaceTreeDataNode::taTypeSpaceTreeDataNode(taSigLinkTypeItemSpace* link_, 
  MemberDef* md, iTreeView* parent_, 
  taiTreeDataNode* last_child_, const String& tree_name, int flags_)
:inherited(link_, md, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

void taTypeSpaceTreeDataNode::init(taSigLinkTypeItemSpace* link_, int flags_) {
}

taTypeSpaceTreeDataNode::~taTypeSpaceTreeDataNode() {
}

taSigLinkTypeItem* taTypeSpaceTreeDataNode::child_link(int idx) {
  taiSigLink* dl = NULL;
  TypeItem* ti = static_cast<TypeItem*>(data()->PosSafeEl_(idx));
  if (ti != NULL) {
    dl = taiViewType::StatGetDataLink(ti, ti->GetTypeDef());
  }
  return static_cast<taSigLinkTypeItem*>(dl);
}

void taTypeSpaceTreeDataNode::CreateChildren_impl() {
//  inherited::CreateChildren();
  String tree_nm;
  taiTreeDataNode* last_child_node = NULL;
  int flags = 0;
    switch (tik) {
    case TypeItem::TIK_MEMBERSPACE:
      flags = iTreeViewItem::DNF_NO_CAN_DROP;
      break;
    case TypeItem::TIK_METHODSPACE:
      flags = iTreeViewItem::DNF_NO_CAN_DROP;
      break;
    case TypeItem::TIK_TYPESPACE: 
      flags = iTreeViewItem::DNF_CAN_BROWSE | iTreeViewItem::DNF_NO_CAN_DROP;
      break;
    default: break;
    }
  for (int i = 0; i < data()->size; ++i) {
    // do global processing for the type item
//none    
    // do any modal type-specific processing for the item
    switch (tik) {
    case TypeItem::TIK_MEMBERSPACE: {
      MemberDef* md = static_cast<MemberDef*>(data()->SafeEl_(i));
      if (!ShowMember(md)) continue;
    } break;
    case TypeItem::TIK_METHODSPACE: {
      MethodDef* md = static_cast<MethodDef*>(data()->SafeEl_(i));
      if (!ShowMethod(md)) continue;
    } break;
    case TypeItem::TIK_TYPESPACE: {
      TypeDef* td = static_cast<TypeDef*>(data()->SafeEl_(i));
      if (!ShowType(td)) continue;
    } break;
    default: break;
    }
    
    taSigLinkTypeItem* dl = child_link(i);
    if (dl == NULL) continue; // shouldn't happen...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
    //TODO  tree_nm = link()->AnonymousItemName(typ->name, i);
    }
    int flags = iTreeViewItem::DNF_CAN_BROWSE;
    last_child_node = dl->CreateTreeDataNode((MemberDef*)NULL, this, 
      last_child_node, tree_nm, flags);
  }
}

bool taTypeSpaceTreeDataNode::ShowItem(TypeItem* ti) const {
return true;
  // default doesn't show hidden items
  if (ti->HasOption("HIDDEN") || ti->HasOption("NO_SHOW"))
    return false;
  return true;
}

bool taTypeSpaceTreeDataNode::ShowMember(MemberDef* md) const {
return true;
//  if (!ShowItem(md)) return false;
//  return true;
  return md->ShowMember(treeView()->show(), 
    TypeItem::SC_ANY); // note: don't say "tree" because this is a meta context
}

bool taTypeSpaceTreeDataNode::ShowMethod(MethodDef* md) const {
return true;
//  if (!ShowItem(md)) return false;
//  return true;
  return md->ShowMethod(treeView()->show());
}
 
bool taTypeSpaceTreeDataNode::ShowType(TypeDef* td) const {
  // first, check with dm of link
  taSigLinkTypeSpace* tsdl = static_cast<taSigLinkTypeSpace*>(link());
  if (!tsdl->ShowChild(td)) return false;
  if (!ShowItem(td)) return false;
  // basic behavior is that we don't show derivitive types, ex. consts, refs, ptrs, etc.
  if (td->IsNotActual())
    return false;
  
  return true;
}

void taTypeSpaceTreeDataNode::willHaveChildren_impl(bool& will) const 
{
  will = true;
}

