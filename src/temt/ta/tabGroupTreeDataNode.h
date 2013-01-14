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

#ifndef tabGroupTreeDataNode_h
#define tabGroupTreeDataNode_h 1

// parent includes:
#include <tabListTreeDataNode>

// member includes:
#include <tabGroupDataLink>

// declare all other types mentioned but not required to include:
class taGroup_impl; //


class TA_API tabGroupTreeDataNode: public tabListTreeDataNode {
INHERITED(tabListTreeDataNode)
public:
  taGroup_impl*         tadata() const {return ((tabGroupDataLink*)m_link)->data();}
  tabGroupDataLink*     link() const {return (tabGroupDataLink*)m_link;}

  override bool         RebuildChildrenIfNeeded();

  taiTreeDataNode*      CreateSubGroup(taiTreeDataNode* after, void* el);
    // for dynamic changes to tree
  override void         UpdateChildNames(); // #IGNORE

  tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  ~tabGroupTreeDataNode();
public: // IDataLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_tabGroupTreeDataNode;}
protected:
  override void         CreateChildren_impl();
  override void         DataChanged_impl(int dcr, void* op1, void* op2); // handle DCR_GROUP_xxx ops
  void                  UpdateGroupNames(); // #IGNORE updates names after inserts/deletes etc.
  override void         willHaveChildren_impl(bool& will) const;
private:
  void                  init(tabGroupDataLink* link_, int dn_flags_); // #IGNORE
};

#endif // tabGroupTreeDataNode_h
