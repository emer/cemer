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

#include "taiTreeNodeDefChild.h"

#include <SigLinkSignal>
#include <ProgEl>


taiTreeNodeDefChild::taiTreeNodeDefChild(taSigLinkTaOBase* link_, MemberDef* md_,
  taiTreeNode* parent_, taiTreeNode* last_child_,
  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name,
  dn_flags_ | DNF_LAZY_CHILDREN)
{
  init(link_, dn_flags_);
}

taiTreeNodeDefChild::taiTreeNodeDefChild(taSigLinkTaOBase* link_, MemberDef* md_,
  iTreeView* parent_, taiTreeNode* last_child_,
  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void taiTreeNodeDefChild::init(taSigLinkTaOBase*, int) {
  m_def_child.Init(this, list());
  // todo: could modulate by some kind of settings
  ProgEl* pel = dynamic_cast<ProgEl*>(tadata());
  if(pel) {
    setFlags(flags() | Qt::ItemIsEditable);
  }
}

taiTreeNodeDefChild::~taiTreeNodeDefChild()
{
}

void taiTreeNodeDefChild::DefChild_SigEmit(int sls, void* op1, void* op2) {
  // we only pass on the List notifies
  if ((!(sls >= SLS_LIST_MIN) && (sls <= SLS_LIST_MAX))) return;
  SigEmit(sls, op1, op2);
}

