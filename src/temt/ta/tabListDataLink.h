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

#ifndef tabListDataLink_h
#define tabListDataLink_h 1

// parent includes:
#include <tabODataLink>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(tabListDataLink);

class TA_API tabListDataLink: public tabODataLink {
  // DataLink for taList objects -- note that it also manages the ListView nodes
INHERITED(tabODataLink)
public:
  taList_impl*          list() {return (taList_impl*)m_data;}
  taList_impl*          list() const {return (taList_impl*)m_data;}
  override taiDataLink* listLink() {return this;}

  tabListDataLink(taList_impl* data_);
  DL_FUNS(tabListDataLink) //
protected:
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md,
    taiTreeDataNode* nodePar, iTreeView* tvPar, taiTreeDataNode* after,
    const String& node_name, int dn_flags);
};

#endif // tabListDataLink_h
