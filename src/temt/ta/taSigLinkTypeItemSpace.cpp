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
#include <taTypeSpaceTreeDataNode>


taSigLinkTypeItemSpace::taSigLinkTypeItemSpace(TypeItem::TypeInfoKinds tik_, 
  taPtrList_impl* data_, taSigLink* &link_ref_)
:inherited(tik_, data_, link_ref_)
{
}

taiTreeDataNode* taSigLinkTypeItemSpace::CreateTreeDataNode_impl(MemberDef* md, 
  taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  taiTreeDataNode* rval = NULL;
  if (nodePar)
    rval = new taTypeSpaceTreeDataNode(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new taTypeSpaceTreeDataNode(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}

