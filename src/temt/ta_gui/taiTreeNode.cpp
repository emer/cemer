// Copyright 2015-2017, Regents of the University of Colorado,
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

#include "taiTreeNode.h"
#include <taiSigLink>
#include <taiViewType>

#include <taMisc>
#include <taiMisc>



taiTreeNode::taiTreeNode(taiSigLink* link_, MemberDef* md_, taiTreeNode* parent_,
  taiTreeNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

taiTreeNode::taiTreeNode(taiSigLink* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeNode* last_child_, const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void taiTreeNode::init(taiSigLink* link_, int dn_flags_) {
  last_child_node = NULL;
  last_member_node = NULL;
}


taiTreeNode::~taiTreeNode() {
}

void taiTreeNode::CreateChildren_impl() {
//NOTE: keep willHaveChildren_impl in sync with this code
  TypeDef* base_typ = link()->GetDataTypeDef();
  MemberSpace* ms = &(base_typ->members);
  iTreeView* tree = treeView(); //cache
  if(!tree) return;
  for (int i = 0; i < ms->size; ++ i) {
    MemberDef* md = ms->FastEl(i);
/*TODO: replace filters, or nuke    // check for member/type-based filter
    if (tree->HasFilter(md->type)) continue;
    if (tree->HasFilter(md)) continue; */
    if (md->IsTreeHidden()) {
      continue;
    }
    if(!md->GetCondOptTest("CONDTREE", base_typ, linkData())) continue;
    TypeDef* typ = md->type;
    void* el = md->GetOff(linkData()); //note: GetSigLink automatically derefs typ and el if pointers
    taiSigLink* dl = taiViewType::StatGetSigLink(el, typ);
    //note: we still can't get links for some types, ex. ptrs to NULL
    if (dl) {
      String tree_nm = md->GetLabel();
      last_child_node = dl->CreateTreeDataNode(md, this, last_child_node, tree_nm,
        (iTreeViewItem::DNF_IS_MEMBER));
      if (!tree->ShowNode(last_child_node)) {
        last_child_node->setHidden(true);
      }
    }
  }
  last_member_node = last_child_node; //note: will be NULL if no members issued
}

void taiTreeNode::willHaveChildren_impl(bool& will) const {
//NOTE: keep CreateChildren_impl in sync with this code
//NOTE: this typically doesn't execute for listish nodes with children
  TypeDef* base_typ = link()->GetDataTypeDef();
  MemberSpace* ms = &(base_typ->members);
  for (int i = 0; i < ms->size; ++ i) {
    MemberDef* md = ms->FastEl(i);
    if (md->IsTreeHidden()) continue;
    if(!md->GetCondOptTest("CONDTREE", base_typ, linkData())) continue;
    will = true;
    break;
  }
  if (!will)
    inherited::willHaveChildren_impl(will);
}

taiTreeNode* taiTreeNode::FindChildForData(void* data, int& idx, int guess_idx) {
  // do a more efficient lo/hi-search from guess if given a guess
  if(guess_idx == 0) {
    const int cnt = childCount();
    for(int i=0; i<cnt; i++) {
      taiTreeNode* rval = (taiTreeNode*)child(i);
      if (rval->link()->data() == data) {
        idx = i;
        return rval;
      }
    }
  }
  else {
    int lo_idx = guess_idx;
    int hi_idx = guess_idx+1;
    const int cnt = childCount();
    while(lo_idx >= 0 || hi_idx < cnt) {
      if(lo_idx >= 0) {
        taiTreeNode* rval = (taiTreeNode*)child(lo_idx);
        if (rval->link()->data() == data) {
          idx = lo_idx;
          return rval;
        }
        lo_idx--;
      }
      if(hi_idx < cnt) {
        taiTreeNode* rval = (taiTreeNode*)child(hi_idx);
        if (rval->link()->data() == data) {
          idx = hi_idx;
          return rval;
        }
        hi_idx++;
      }
    }
  }
  idx = -1;
  return NULL;
}

/*nn taiSigLink* taiTreeNode::par_link() const {
  taiTreeNode* par = parent();
  return (par) ? par->link() : NULL;
} */

/*MemberDef* taiTreeNode::par_md() const {
//TODO: this is a fairly broken concept -- i don't even think the clip system
// needs to know about par mds
//TODO: following is the gui one, we should be returning the data one
  taiTreeNode* par = parent();
  return (par) ? par->md() : NULL;
} */

