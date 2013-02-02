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

#include "taSigLinkTypeItemSpace.h"
#include <taiTreeNodeTypeSpace>


taSigLinkTypeItemSpace::taSigLinkTypeItemSpace(TypeItem::TypeInfoKinds tik_, 
  taPtrList_impl* data_, taSigLink* &link_ref_)
:inherited(tik_, data_, link_ref_)
{
}

taiTreeNode* taSigLinkTypeItemSpace::CreateTreeDataNode_impl(MemberDef* md, 
  taiTreeNode* nodePar,
  iTreeView* tvPar, taiTreeNode* after, const String& node_name, int dn_flags)
{
  taiTreeNode* rval = NULL;
  if (nodePar)
    rval = new taiTreeNodeTypeSpace(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new taiTreeNodeTypeSpace(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}

