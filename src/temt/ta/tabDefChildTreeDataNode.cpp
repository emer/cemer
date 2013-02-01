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

#include "tabDefChildTreeDataNode.h"

#include <SigLinkSignal>

tabDefChildTreeDataNode::tabDefChildTreeDataNode(taSigLinkOBase* link_, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_,
  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name,
  dn_flags_ | DNF_LAZY_CHILDREN)
{
  init(link_, dn_flags_);
}

tabDefChildTreeDataNode::tabDefChildTreeDataNode(taSigLinkOBase* link_, MemberDef* md_,
  iTreeView* parent_, taiTreeDataNode* last_child_,
  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabDefChildTreeDataNode::init(taSigLinkOBase*, int) {
  m_def_child.Init(this, list());
}

tabDefChildTreeDataNode::~tabDefChildTreeDataNode()
{
}

void tabDefChildTreeDataNode::DefChild_SigEmit(int dcr, void* op1, void* op2) {
  // we only pass on the List notifies
  if ((!(dcr >= SLS_LIST_MIN) && (dcr <= SLS_LIST_MAX))) return;
  SigEmit(dcr, op1, op2);
}

