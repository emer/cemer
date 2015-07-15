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

#ifndef taiTreeNode_h
#define taiTreeNode_h 1

// parent includes:
#include <iTreeViewItem>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiTreeNode);

class TA_API taiTreeNode: public iTreeViewItem {
INHERITED(iTreeViewItem)
public:
  taiTreeNode*      parent() const
  { return (taiTreeNode*) QTreeWidgetItem::parent(); }
  // note: NULL for root item

  taiTreeNode*      FindChildForData(void* data, int& idx);
  // find the Child Node (if any) that has data as the data of its link; NULL/-1 if not found
  virtual void          UpdateChildNames() {} // #IGNORE update child names of this node

  taiTreeNode(taiSigLink* link_, MemberDef* md_, taiTreeNode* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  taiTreeNode(taiSigLink* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  ~taiTreeNode();

public: // ITypedObject interface
  TypeDef*     GetTypeDef() const override {return &TA_taiTreeNode;}
public: // ISelectable interface
protected:
  taiTreeNode*      last_member_node;
  // #IGNORE last member node created, so we know where to start list/group items
  taiTreeNode*      last_child_node;
  // #IGNORE last child node created, so we can pass to createnode
  void         willHaveChildren_impl(bool& will) const override;
  void         CreateChildren_impl() override;
private:
  void                  init(taiSigLink* link_, int dn_flags_); // #IGNORE
};

#endif // taiTreeNode_h
