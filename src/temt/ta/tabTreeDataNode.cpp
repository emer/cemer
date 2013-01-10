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

#include "tabTreeDataNode.h"

tabTreeDataNode::tabTreeDataNode(tabDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taiDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

tabTreeDataNode::tabTreeDataNode(tabDataLink* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taiDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabTreeDataNode::init(tabDataLink* link_, int dn_flags_) {
}

tabTreeDataNode::~tabTreeDataNode()
{
}

void tabTreeDataNode::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  bool do_updt = false;
  taBase* tab = tadata();
  if(tab) {
    TypeDef* base_typ = tab->GetTypeDef();
    if(base_typ->HasOption("HAS_CONDTREE")) {
      if(dcr == DCR_ITEM_UPDATED)
        do_updt = true;
    }
  }
  if(do_updt || dcr == DCR_STRUCT_UPDATE_ALL) { // special case for post-loading update
    takeChildren();
    CreateChildren();
    iTreeView* itv = treeView();
    if(!itv) return;
    if(itv->itemCount() > 0) {
      if(itv->item(0) == this)
        itv->ExpandDefault();
      else if(do_updt) {
        itv->ExpandDefaultUnder(this);
      }
    }
  }
}
