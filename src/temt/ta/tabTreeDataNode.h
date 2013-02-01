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

#ifndef tabTreeDataNode_h
#define tabTreeDataNode_h 1

// parent includes:
#include <taiTreeDataNode>

// member includes:
#include <taSigLinkBase>

// declare all other types mentioned but not required to include:

TypeDef_Of(tabTreeDataNode);

class TA_API tabTreeDataNode: public taiTreeDataNode {
INHERITED(taiTreeDataNode)
public:
  taBase*               tadata() {return ((taSigLinkBase*)m_link)->data();}
  taSigLinkBase*          link() const {return (taSigLinkBase*)m_link;}

  tabTreeDataNode(taSigLinkBase* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  tabTreeDataNode(taSigLinkBase* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  ~tabTreeDataNode();
public: // ISigLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_tabTreeDataNode;}
protected:
  override void         SigEmit_impl(int sls, void* op1, void* op2);
private:
  void                  init(taSigLinkBase* link_, int dn_flags_); // #IGNORE
};

#endif // tabTreeDataNode_h
