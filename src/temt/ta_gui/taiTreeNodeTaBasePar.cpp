// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "taiTreeNodeTaBasePar.h"
#include <taiViewType>
#include <iMainWindowViewer>

#include <SigLinkSignal>
#include <taMisc>

taiTreeNodeTaBasePar::taiTreeNodeTaBasePar(taSigLinkTaOBase* link_, MemberDef* md_,
  taiTreeNode* parent_, taiTreeNode* last_child_,
    const String& tree_name, int dn_flags_)
:inherited((taSigLinkTaBase*)link_, md_, parent_, last_child_, tree_name,
  dn_flags_ | DNF_LAZY_CHILDREN)
{
  init(link_, dn_flags_);
}

taiTreeNodeTaBasePar::taiTreeNodeTaBasePar(taSigLinkTaOBase* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taSigLinkTaBase*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void taiTreeNodeTaBasePar::init(taSigLinkTaOBase* link_, int dn_flags_) {
  save_exp_state = false;
  last_list_items_node = NULL;
}

taiTreeNodeTaBasePar::~taiTreeNodeTaBasePar()
{
}

void taiTreeNodeTaBasePar::AssertLastListItem() {
  void* el = list()->Peek_();
  if (el == NULL) {
    last_list_items_node = last_member_node;
    return;
  }
  int idx;
  last_list_items_node = this->FindChildForData(el, idx);
}

void taiTreeNodeTaBasePar::CreateChildren_impl() {
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

void taiTreeNodeTaBasePar::willHaveChildren_impl(bool& will) const {
  if (list()->size > 0) will = true;
  if (!will)
    inherited::willHaveChildren_impl(will);
}

taiTreeNode* taiTreeNodeTaBasePar::CreateListItem(taiTreeNode* par_node,
  taiTreeNode* after, taBase* el)
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
  taiTreeNode* dn = dl->CreateTreeDataNode((MemberDef*)NULL,
    par_node, after, _nilString, dn_flags_tmp);
  return dn;
}


bool taiTreeNodeTaBasePar::RebuildChildrenIfNeeded() {
  int st_idx = 0;
  if(last_member_node)
    st_idx = MAX(indexOfChild(last_member_node)+1, 0);
  if(childCount() != list()->size + st_idx) {
    taMisc::DebugInfo("taiTreeNodeTaBasePar::RebuildChildrenIfNeeded() running");
    takeChildren();
    CreateChildren();
    return true;
  }
  return false;
}

void taiTreeNodeTaBasePar::SigEmit_impl(int sls, void* op1_, void* op2_) {
  inherited::SigEmit_impl(sls, op1_, op2_);
  if (!this->children_created) {
    if ((sls == SLS_LIST_ITEM_INSERT) || (sls == SLS_LIST_ITEM_REMOVE) ||
        (sls == SLS_STRUCT_UPDATE_END)) {
      // taMisc::DebugInfo("UpdateLazyChildren sls", String(sls));
      UpdateLazyChildren(); // updates
    }
    return;
  }

  if(sls == SLS_ITEM_UPDATED || sls == SLS_STRUCT_UPDATE_END) {
    // taMisc::Info("SLS item update or struct update end", String(sls));
    if(!RebuildChildrenIfNeeded()) {
      // taMisc::Info("UpdateListNames", String(sls));
      UpdateListNames();
    }
    return;
  }

  int idx;
  switch (sls) {
  case SLS_LIST_INIT: break;
  case SLS_LIST_ITEM_INSERT: {  // op1=item, op2=item_after, null=at beginning
    taiTreeNode* after_node = this->FindChildForData(op2_, idx); //null if not found
    if (!after_node) after_node = last_member_node; // insert, after
    iTreeView* tv = treeView();
    if(tv) {
      tv->TreeStructUpdate(true);
    }
    taiTreeNode* new_node = CreateListItem(this, after_node, (taBase*)op1_);
    //EVIL    tv->expandItem(new_node);
    // only scroll to it if parent is visible
    if(tv) {
      tv->TreeStructUpdate(false);
      if (isExpandedLeaf() && !taMisc::in_gui_multi_action)
        tv->scrollTo(new_node);
    }
    return;                     // don't update names
  }
  case SLS_LIST_ITEM_REMOVE: {  // op1=item -- note, item not DisOwned yet, but has been removed from list
    taiTreeNode* gone_node = this->FindChildForData(op1_, idx); //null if not found
    if (gone_node) {
      iTreeView* tv = treeView();
      if(tv) {
        // taMisc::DebugInfo("SLS_LIST_ITEM_REMOVE");
        tv->TreeStructUpdate(true);
        // bool is_exp = this->isExpanded();
        // if(is_exp)
        //   this->setExpanded(false);
        tv->SelectNextLogicalItem(gone_node);  // do while we still have the current item
        takeChild(idx);
        delete gone_node;
        // this->setExpanded(is_exp);
        tv->TreeStructUpdate(false);
      }
    }
    return;                     // don't update names
  }
  case SLS_LIST_ITEM_MOVED: {   // op1=item, op2=item_after, null=at beginning
    int fm_idx;
    taiTreeNode* moved_node = this->FindChildForData(op1_, fm_idx); //null if not found
    if (!moved_node) break; // shouldn't happen
    int to_idx;
    taiTreeNode* after_node = this->FindChildForData(op2_, to_idx); //null if not found
    if (!after_node) to_idx = indexOfChild(last_member_node); // insert, after
    ++to_idx; // after
    iTreeView* tv = treeView();
    if(tv) {
      tv->TreeStructUpdate(true);
    }
    moveChild(fm_idx, to_idx);

    if(tv) {
      tv->TreeStructUpdate(false);
      // only scroll to it if parent is visible
      // if (isExpandedLeaf() && !taMisc::in_gui_multi_action)
      //   tv->scrollTo(moved_node);
    }
    break;
  }
  case SLS_LIST_ITEMS_SWAP: {   // op1=item1, op2=item2
    int n1_idx, n2_idx;
    taiTreeNode* node1 = this->FindChildForData(op1_, n1_idx); //null if not found
    taiTreeNode* node2 = this->FindChildForData(op2_, n2_idx); //null if not found
    if ((!node1) || (!node2)) break; // shouldn't happen
    iTreeView* tv = treeView();
    if(tv) {
      tv->TreeStructUpdate(true);
    }
    swapChildren(n1_idx, n2_idx);
    if(tv) {
      tv->TreeStructUpdate(false);
    }
    break;
  }
  case SLS_LIST_RESET_START: {     // no ops
    this->save_exp_state = isExpanded();
    this->setExpanded(false);
    iTreeView* tv = treeView();
    if(tv) {
      // taMisc::DebugInfo("SLS_LIST_RESET_START");
      tv->TreeStructUpdate(true);
    }
    return;                     // don't rename!
  }
  case SLS_LIST_RESET_END: {     // no ops
    // actually, no point in restoring!
    // this->setExpanded(save_exp_state);
    iTreeView* tv = treeView();
    if(tv) {
#ifdef TA_OS_MAC
      // this is key for preventing crash on delete of groups!
      iMainWindowViewer* imw = tv->mainWindow();
      if(imw)
        imw->skip_next_update_refresh = 1; // 2 causes noticible lack of update -- 1 ok
#endif      
      // taMisc::DebugInfo("SLS_LIST_RESET_END");
      tv->TreeStructUpdate(false);
    }
    break;                      // ok to rename
  }
  case SLS_LIST_SORTED: {       // no ops
    int nd_idx; // index of the node
    taList_impl* list = this->list(); // cache
    iTreeView* tv = treeView();
    if(tv) {
      tv->TreeStructUpdate(true);
    }
    for (int i = 0; i < list->size; ++i) {
      taBase* tab = (taBase*)list->FastEl_(i);
      FindChildForData(tab, nd_idx, i);
      if (i == nd_idx) continue; // in right place already
      moveChild(nd_idx, i);
    }
    if(tv) {
      tv->TreeStructUpdate(false);
    }
    break;
  }
  default: return; // don't update names
  }
  UpdateListNames();
}

void taiTreeNodeTaBasePar::UpdateChildNames() {
  inherited::UpdateChildNames();
  UpdateListNames();
}

void taiTreeNodeTaBasePar::UpdateListNames() {
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
    taiTreeNode* node1 = this->FindChildForData(el, idx, i); //efficient search from i
    if (node1 != NULL)
      node1->DecorateDataNode();
  }
}
