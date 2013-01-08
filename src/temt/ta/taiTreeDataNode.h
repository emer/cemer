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

#ifndef taiTreeDataNode_h
#define taiTreeDataNode_h 1

// parent includes:
#include <iTreeViewItem>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiTreeDataNode: public iTreeViewItem {
INHERITED(iTreeViewItem)
public:
  taiTreeDataNode*      parent() const {return (taiTreeDataNode*) QTreeWidgetItem::parent();} //note: NULL for root item

  taiTreeDataNode*      FindChildForData(void* data, int& idx); // find the Child Node (if any) that has data as the data of its link; NULL/-1 if not found
  virtual void          UpdateChildNames() {} // #IGNORE update child names of this node

  taiTreeDataNode(taiDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  taiTreeDataNode(taiDataLink* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  ~taiTreeDataNode();
public: // ITypedObject interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_taiTreeDataNode;}
public: // ISelectable interface
//obs  override taiDataLink* par_link() const;
//obs  override MemberDef*      par_md() const; //provided at create time
//  override ISelectableHost* host() const;
protected:
  taiTreeDataNode*      last_member_node; // #IGNORE last member node created, so we know where to start list/group items
  taiTreeDataNode*      last_child_node; // #IGNORE last child node created, so we can pass to createnode
  override void         willHaveChildren_impl(bool& will) const;
  override void         CreateChildren_impl();
private:
  void                  init(taiDataLink* link_, int dn_flags_); // #IGNORE
};

#endif // taiTreeDataNode_h
