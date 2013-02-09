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

#ifndef taiTreeNodeGroup_h
#define taiTreeNodeGroup_h 1

// parent includes:
#include <taiTreeNodeList>

// member includes:
#include <taSigLinkGroup>

// declare all other types mentioned but not required to include:
class taGroup_impl; //


taTypeDef_Of(taiTreeNodeGroup);

class TA_API taiTreeNodeGroup: public taiTreeNodeList {
INHERITED(taiTreeNodeList)
public:
  taGroup_impl*         tadata() const {return ((taSigLinkGroup*)m_link)->data();}
  taSigLinkGroup*     link() const {return (taSigLinkGroup*)m_link;}

  override bool         RebuildChildrenIfNeeded();

  taiTreeNode*      CreateSubGroup(taiTreeNode* after, void* el);
    // for dynamic changes to tree
  override void         UpdateChildNames(); // #IGNORE

  taiTreeNodeGroup(taSigLinkGroup* link_, MemberDef* md_, taiTreeNode* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  taiTreeNodeGroup(taSigLinkGroup* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  ~taiTreeNodeGroup();
public: // ISigLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_taiTreeNodeGroup;}
protected:
  override void         CreateChildren_impl();
  override void         SigEmit_impl(int sls, void* op1, void* op2); // handle SLS_GROUP_xxx ops
  void                  UpdateGroupNames(); // #IGNORE updates names after inserts/deletes etc.
  override void         willHaveChildren_impl(bool& will) const;
private:
  void                  init(taSigLinkGroup* link_, int dn_flags_); // #IGNORE
};

#endif // taiTreeNodeGroup_h
