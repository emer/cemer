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

#include "tabGroupDataLink.h"


tabGroupDataLink::tabGroupDataLink(taGroup_impl* data_)
  : inherited((taList_impl*)data_)
{
}

taiTreeDataNode* tabGroupDataLink::CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  taiTreeDataNode* rval = NULL;
  if (nodePar)
    rval = new tabGroupTreeDataNode(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new tabGroupTreeDataNode(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}

