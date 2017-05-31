// Copyright 2015, Regents of the University of Colorado,
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

#include "taiTreeNodeList.h"

taiTreeNodeList::taiTreeNodeList(taSigLinkList* link_, MemberDef* md_,
  taiTreeNode* parent_, taiTreeNode* last_child_,
  const String& tree_name, int dn_flags_)
:inherited((taSigLinkTaOBase*)link_, md_, parent_, last_child_, tree_name,
  dn_flags_ | DNF_LAZY_CHILDREN)
{
  init(link_, dn_flags_);
}

taiTreeNodeList::taiTreeNodeList(taSigLinkList* link_, MemberDef* md_,
  iTreeView* parent_, taiTreeNode* last_child_,
  const String& tree_name, int dn_flags_)
:inherited((taSigLinkTaOBase*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void taiTreeNodeList::init(taSigLinkList*, int) {
}

taiTreeNodeList::~taiTreeNodeList()
{
}

