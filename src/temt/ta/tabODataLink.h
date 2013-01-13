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

#ifndef tabODataLink_h
#define tabODataLink_h 1

// parent includes:
#include <tabDataLink>

// member includes:
#include <taOBase>
#include <taList_impl>

// declare all other types mentioned but not required to include:


class TA_API tabODataLink: public tabDataLink { // DataLink for taOBase objects
INHERITED(tabDataLink)
public:
  taOBase*              data() {return (taOBase*)m_data;}
  taOBase*              data() const {return (taOBase*)m_data;}

  override MemberDef*   GetDataMemberDef() const;

  tabODataLink(taOBase* data_);
  DL_FUNS(tabODataLink); //

public: // for taLists, and default children (where defined) in taOBase
  virtual taList_impl*  list() {return ((taOBase*)m_data)->children_();}
  virtual taList_impl*  list() const {return ((taOBase*)m_data)->children_();}
  virtual taiDataLink*  listLink() {taList_impl* lst = list();
    return (lst) ? (taiDataLink*)lst->GetDataLink() : NULL;}

  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md,
    taiTreeDataNode* nodePar, iTreeView* tvPar, taiTreeDataNode* after,
    const String& node_name, int dn_flags);
  override taiDataLink* GetListChild(int itm_idx);
  override taiDataLink* GetListChild(void* el);
   override int         NumListCols() const; // number of columns in a list view for this item type
  override const KeyString GetListColKey(int col) const; // #IGNORE
  override String       GetColHeading(const KeyString& key) const; // #IGNORE
  override String       ChildGetColText(taDataLink* child, const KeyString& key,
    int itm_idx = -1) const;    // #IGNORE
//   override bool              HasChildItems() {return true;} // at very least, has the 'items' subnode

};

#endif // tabODataLink_h
