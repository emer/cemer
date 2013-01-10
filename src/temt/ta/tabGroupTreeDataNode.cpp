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

#include "tabGroupTreeDataNode.h"

tabGroupTreeDataNode::tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((tabListDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

tabGroupTreeDataNode::tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((tabListDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabGroupTreeDataNode::init(tabGroupDataLink* link_, int dn_flags_) {
}

tabGroupTreeDataNode::~tabGroupTreeDataNode()
{
}

void tabGroupTreeDataNode::CreateChildren_impl() {
  inherited::CreateChildren_impl();
  String tree_nm;
  for (int i = 0; i < tadata()->gp.size; ++i) {
    // the subgroups are themselves taGroup items
    taBase* el = tadata()->gp.FastEl(i);
    if (!el) continue;
    TypeDef* typ = el->GetTypeDef();
    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    if (!dl) continue; // shouldn't happen unless null...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = "(subgroup " + String(i) + ")";
    }
    last_child_node = dl->CreateTreeDataNode(NULL, this, last_child_node, tree_nm,
      (iTreeViewItem::DNF_UPDATE_NAME | iTreeViewItem::DNF_CAN_DRAG));
    //TODO: maybe this isn't right -- we really want the root group's md, because that is the only
          // one that has the defs
  }
}

taiTreeDataNode* tabGroupTreeDataNode::CreateSubGroup(taiTreeDataNode* after_node,
  void* el)
{
  taSubGroup* gp = &tadata()->gp;
  TypeDef* typ = gp->GetElType();
  // the subgroups are themselves taGroup items
  if (!typ) return NULL; //TODO: maybe we should put a marker item in list???
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
  if (!dl) return NULL; // shouldn't happen unless null...

  taiTreeDataNode* dn = dl->CreateTreeDataNode(NULL, this, after_node, "",
    (iTreeViewItem::DNF_UPDATE_NAME | iTreeViewItem::DNF_CAN_DRAG));
     //gets its name in rename
  return dn;
}

bool tabGroupTreeDataNode::RebuildChildrenIfNeeded() {
  int st_idx = 0;
  if(last_member_node)
    st_idx = MAX(indexOfChild(last_member_node)+1, 0);
  taSubGroup* gp = &tadata()->gp;
  if(childCount() != (list()->size + gp->size + st_idx)) {
    takeChildren();
    CreateChildren();
    AssertLastListItem();
    return true;
  }
  return false;
}

void tabGroupTreeDataNode::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  if (!this->children_created) {
    if ((dcr == DCR_GROUP_INSERT) || (dcr == DCR_GROUP_REMOVE))
      UpdateLazyChildren(); // updates
    return;
  }
  AssertLastListItem();
  int idx;
  switch (dcr) {
  case DCR_GROUP_INSERT: {      // op1=item, op2=item_after, null=at beginning
    taiTreeDataNode* after_node = this->FindChildForData(op2_, idx); //null if not found
    if (after_node == NULL) after_node = last_list_items_node; // insert, after lists
    iTreeView* tv = treeView();
    if(tv) {
      tv->EmitTreeStructToUpdate();
    }
    taiTreeDataNode* new_node = CreateSubGroup(after_node, op1_);
    // only scroll to it if parent is visible
    if(tv) {
      if (isExpandedLeaf() && !taMisc::in_gui_multi_action)
	tv->scrollTo(new_node);
      tv->EmitTreeStructUpdated();
    }
    break;
  }
  case DCR_GROUP_REMOVE: {      // op1=item -- note, item not DisOwned yet, but has been removed from list
    taiTreeDataNode* gone_node = this->FindChildForData(op1_, idx); //null if not found
    if (gone_node) {
      iTreeView* tv = treeView();
      if(tv) {
	tv->EmitTreeStructToUpdate();
      }
      delete gone_node; // goodbye!
      if(tv) {
	tv->EmitTreeStructUpdated();
      }
    }
    break;
  }
  case DCR_GROUP_MOVED: {       // op1=item, op2=item_after, null=at beginning
    int fm_idx;
    taiTreeDataNode* moved_node = this->FindChildForData(op1_, fm_idx); //null if not found
    if (!moved_node) break; // shouldn't happen
    int to_idx;
    taiTreeDataNode* after_node = this->FindChildForData(op2_, to_idx); //null if not found
    if (!after_node) to_idx = indexOfChild(last_list_items_node); // insert, after
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
  case DCR_GROUPS_SWAP: {       // op1=item1, op2=item2
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
  case DCR_GROUPS_SORTED: {     // no ops
    int gp0_idx = indexOfChild(last_list_items_node) + 1; // valid if llin=NULL
    int nd_idx; // index of the node
    taGroup_impl* gp = this->tadata(); // cache
    iTreeView* tv = treeView();
    if(tv) {
      tv->EmitTreeStructToUpdate();
    }
    for (int i = 0; i < gp->gp.size; ++i) {
      taBase* tab = (taBase*)gp->FastGp_(i);
      FindChildForData(tab, nd_idx);
      if ((gp0_idx+i) == nd_idx) continue; // in right place already
      moveChild(nd_idx, (gp0_idx+i));
    }
    if(tv) {
      tv->EmitTreeStructUpdated();
    }
    break;
  }
  default: return; // don't update names
  }
  UpdateGroupNames();
}

void tabGroupTreeDataNode::UpdateChildNames() {
  inherited::UpdateChildNames();
  UpdateGroupNames();
}

void tabGroupTreeDataNode::UpdateGroupNames() {
  String tree_nm;
  for (int i = 0; i < tadata()->gp.size; ++i) {
    // the subgroups are themselves taGroup items
    taBase* el = tadata()->gp.FastEl(i);
    if (!el) continue;
    TypeDef* typ = el->GetTypeDef();
    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    if (!dl) continue; // shouldn't happen unless null...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = "(subgroup " + String(i) + ")";
    }
    int idx;
    iTreeViewItem* node1 = this->FindChildForData(el, idx); //null if not found
    if (node1 != NULL)
      node1->setText(0, tree_nm);
  }
}

void tabGroupTreeDataNode::willHaveChildren_impl(bool& will) const {
  if (tadata()->gp.size > 0) will = true;
  if (!will)
    inherited::willHaveChildren_impl(will);
}

