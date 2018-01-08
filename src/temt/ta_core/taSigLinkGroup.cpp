// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "taSigLinkGroup.h"
#include <taiTreeNodeGroup>


taSigLinkGroup::taSigLinkGroup(taGroup_impl* data_)
  : inherited((taList_impl*)data_)
{
}

taiTreeNode* taSigLinkGroup::CreateTreeDataNode_impl(MemberDef* md, taiTreeNode* nodePar,
  iTreeView* tvPar, taiTreeNode* after, const String& node_name, int dn_flags)
{
  taiTreeNode* rval = NULL;
  if (nodePar)
    rval = new taiTreeNodeGroup(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new taiTreeNodeGroup(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}

