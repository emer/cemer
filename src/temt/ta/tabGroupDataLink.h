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

#ifndef tabGroupDataLink_h
#define tabGroupDataLink_h 1

// parent includes:
#include <tabListDataLink>

// member includes:

// declare all other types mentioned but not required to include:
class taGroup_impl; //


TypeDef_Of(tabGroupDataLink);

class TA_API tabGroupDataLink: public tabListDataLink {
  // DataLink for taGroup objects -- adds additional 'subgroups' node under the 'items' node, for any subgroups
INHERITED(tabListDataLink)
public:
  taGroup_impl*         data() {return (taGroup_impl*)m_data;}
  const taGroup_impl*   data() const {return (taGroup_impl*)m_data;}

  tabGroupDataLink(taGroup_impl* data_);
  DL_FUNS(tabGroupDataLink)
protected:
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
    iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags);
};

#endif // tabGroupDataLink_h
