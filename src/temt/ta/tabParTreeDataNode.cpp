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

#include "tabParTreeDataNode.h"
#include <taiViewType>

#include <SigLinkSignal>
#include <taMisc>


tabParTreeDataNode::tabParTreeDataNode(taSigLinkOBase* link_, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_,
    const String& tree_name, int dn_flags_)
:inherited((taSigLinkBase*)link_, md_, parent_, last_child_, tree_name,
  dn_flags_ | DNF_LAZY_CHILDREN)
{
  init(link_, dn_flags_);
}

tabParTreeDataNode::tabParTreeDataNode(taSigLinkOBase* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taSigLinkBase*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabParTreeDataNode::init(taSigLinkOBase* link_, int dn_flags_) {
  last_list_items_node = NULL;
}

tabParTreeDataNode::~tabParTreeDataNode()
{
}

void tabParTreeDataNode::AssertLastListItem() {
  void* el = list()->Peek_();
  if (el == NULL) {
    last_list_items_node = last_member_node;
    return;
  }
  int idx;
  last_list_items_node = this->FindChildForData(el, idx);
}

void tabParTreeDataNode::CreateChildren_impl() {
  inherited::CreateChildren_impl();
  String tree_nm;
  taList_impl* list = this->list(); // cache
  for (int i = 0; i < list->size; ++i) {
    taBase* el = (taBase*)list->FastEl_(i);
    if (!el) continue; // generally shouldn't happen
    TypeDef* typ = el->GetTypeDef();
    taiSigLink* dl = taiViewType::StatGetSigLink(el, typ);
    if (!dl) continue; // shouldn't happen... unless null

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = link()->AnonymousItemName(typ->name, i);
    }
    int dn_flags_tmp = DNF_UPDATE_NAME | DNF_CAN_BROWSE | DNF_CAN_DRAG;
    // check if this is a link
    taBase* own = el->GetOwner(); //note: own=NULL generally means <taOBase items
    if (own && (own != list))
      dn_flags_tmp |= DNF_IS_LINK;
    last_child_node = dl->CreateTreeDataNode((MemberDef*)NULL, this, last_child_node, tree_nm, dn_flags_tmp);
  }

  last_list_items_node = last_child_node;
}

void tabParTreeDataNode::willHaveChildren_impl(bool& will) const {
  if (list()->size > 0) will = true;
  if (!will)
    inherited::willHaveChildren_impl(will);
}

taiTreeDataNode* tabParTreeDataNode::CreateListItem(taiTreeDataNode* par_node,
  taiTreeDataNode* after, taBase* el)
{
  if (!el) return NULL;
  taList_impl* list = this->list(); // cache
  TypeDef* typ = el->GetTypeDef();
  taiSigLink* dl = taiViewType::StatGetSigLink(el, typ);
  if (!dl) return NULL; // shouldn't happen unless null...
  //note: we don't make name because it is updated anyway
  int dn_flags_tmp = DNF_UPDATE_NAME | DNF_CAN_BROWSE | DNF_CAN_DRAG;
  // check if this is a link
  taBase* own = el->GetOwner(); //note: own=NULL generally means <taOBase items
  if (own && (own != list))
    dn_flags_tmp |= DNF_IS_LINK;
  taiTreeDataNode* dn = dl->CreateTreeDataNode((MemberDef*)NULL,
    par_node, after, _nilString, dn_flags_tmp);
  return dn;
}


bool tabParTreeDataNode::RebuildChildrenIfNeeded() {
  int st_idx = 0;
  if(last_member_node)
    st_idx = MAX(indexOfChild(last_member_node)+1, 0);
  if(childCount() != list()->size + st_idx) {
    takeChildren();
    CreateChildren();
    return true;
  }
  return false;
}

void tabParTreeDataNode::SigEmit_impl(int dcr, void* op1_, void* op2_) {
  inherited::SigEmit_impl(dcr, op1_, op2_);
  if (!this->children_created) {
    if ((dcr == SLS_LIST_ITEM_INSERT) || (dcr == SLS_LIST_ITEM_REMOVE) ||
        (dcr == SLS_STRUCT_UPDATE_END)) {
      UpdateLazyChildren(); // updates
    }
    return;
  }

  if(dcr == SLS_ITEM_UPDATED || dcr == SLS_STRUCT_UPDATE_END) {
    if(!RebuildChildrenIfNeeded())
      UpdateListNames();
    return;
  }

  int idx;
  switch (dcr) {
  case SLS_LIST_INIT: break;
  case SLS_LIST_ITEM_INSERT: {  // op1=item, op2=item_after, null=at beginning
    taiTreeDataNode* after_node = this->FindChildForData(op2_, idx); //null if not found
    if (!after_node) after_node = last_member_node; // insert, after
    iTreeView* tv = treeView();
    if(tv) {
      tv->EmitTreeStructToUpdate();
    }
    taiTreeDataNode* new_node = CreateListItem(this, after_node, (taBase*)op1_);
//EVIL    tv->expandItem(new_node);
    // only scroll to it if parent is visible
    if(tv) {
      tv->EmitTreeStructUpdated();
      if (isExpandedLeaf() && !taMisc::in_gui_multi_action)
	tv->scrollTo(new_node);
    }
    break;
  }
  case SLS_LIST_ITEM_REMOVE: {  // op1=item -- note, item not DisOwned yet, but has been removed from list
    taiTreeDataNode* gone_node = this->FindChildForData(op1_, idx); //null if not found
    if (gone_node) {
      iTreeView* tv = treeView();
      if(tv) {
	tv->EmitTreeStructToUpdate();
        tv->SaveScrollPos();
        tv->setAutoScroll(false);       // auto scroll is very bad for this in 4.7.0 -- scrolls to top..
      }
      delete gone_node; // goodbye!
      iTreeView* tv2 = treeView(); // make sure it still exists!
      if(tv2) {
        tv2->RestoreScrollPos();
        tv2->setAutoScroll(true);
	tv2->EmitTreeStructUpdated();
      }
    }
    break;
  }
  case SLS_LIST_ITEM_MOVED: {   // op1=item, op2=item_after, null=at beginning
    int fm_idx;
    taiTreeDataNode* moved_node = this->FindChildForData(op1_, fm_idx); //null if not found
    if (!moved_node) break; // shouldn't happen
    int to_idx;
    taiTreeDataNode* after_node = this->FindChildForData(op2_, to_idx); //null if not found
    if (!after_node) to_idx = indexOfChild(last_member_node); // insert, after
    ++to_idx; // after
    iTreeView* tv = treeView();
    if(tv) {
      tv->EmitTreeStructToUpdate();
    }
    moveChild(fm_idx, to_idx);
    // only scroll to it if parent is visible
    if(tv) {
      if (isExpandedLeaf() && !taMisc::in_gui_multi_action)
	tv->scrollTo(moved_node);
      tv->EmitTreeStructUpdated();
    }
    break;
  }
  case SLS_LIST_ITEMS_SWAP: {   // op1=item1, op2=item2
    int n1_idx, n2_idx;
    taiTreeDataNode* node1 = this->FindChildForData(op1_, n1_idx); //null if not found
    taiTreeDataNode* node2 = this->FindChildForData(op2_, n2_idx); //null if not found
    if ((!node1) || (!node2)) break; // shouldn't happen
    iTreeView* tv = treeView();
    if(tv) {
      tv->EmitTreeStructToUpdate();
    }
    swapChildren(n1_idx, n2_idx);
    if(tv) {
      tv->EmitTreeStructUpdated();
    }
    break;
  }
  case SLS_LIST_SORTED: {       // no ops
    int nd_idx; // index of the node
    taList_impl* list = this->list(); // cache
    iTreeView* tv = treeView();
    if(tv) {
      tv->EmitTreeStructToUpdate();
    }
    for (int i = 0; i < list->size; ++i) {
      taBase* tab = (taBase*)list->FastEl_(i);
      FindChildForData(tab, nd_idx);
      if (i == nd_idx) continue; // in right place already
      moveChild(nd_idx, i);
    }
    if(tv) {
      tv->EmitTreeStructUpdated();
    }
    break;
  }
  default: return; // don't update names
  }
  UpdateListNames();
}

void tabParTreeDataNode::UpdateChildNames() {
  inherited::UpdateChildNames();
  UpdateListNames();
}

void tabParTreeDataNode::UpdateListNames() {
  String tree_nm;
  taList_impl* list = this->list(); //cache
  for (int i = 0; i < list->size; ++i) {
    // the subgroups are themselves taGroup items
    taBase* el = (taBase*)list->FastEl_(i);
    if(!el) continue;
    TypeDef* typ = el->GetTypeDef();
    taiSigLink* dl = taiViewType::StatGetSigLink(el, typ);
    if (!dl) continue; // shouldn't happen unless null...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = link()->AnonymousItemName(typ->name, i);
    }
    int idx;
    taiTreeDataNode* node1 = this->FindChildForData(el, idx); //null if not found
    if (node1 != NULL)
      node1->DecorateDataNode();
  }
}
