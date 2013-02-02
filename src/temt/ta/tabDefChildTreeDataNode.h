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

#ifndef tabDefChildTreeDataNode_h
#define tabDefChildTreeDataNode_h 1

// parent includes:
#include <taiTreeNodeTaBasePar>

// member includes:
#include <tabDefChildRef>

// declare all other types mentioned but not required to include:


TypeDef_Of(tabDefChildTreeDataNode);

class TA_API tabDefChildTreeDataNode: public taiTreeNodeTaBasePar {
INHERITED(taiTreeNodeTaBasePar)
friend class tabDefChildRef;
public:
  tabDefChildTreeDataNode(taSigLinkTaOBase* link_, MemberDef* md_,
    taiTreeNode* parent_, taiTreeNode* after,
    const String& tree_name, int dn_flags_ = 0);
  tabDefChildTreeDataNode(taSigLinkTaOBase* link_, MemberDef* md_,
    iTreeView* parent_, taiTreeNode* after,
    const String& tree_name, int dn_flags_ = 0);
  ~tabDefChildTreeDataNode();
public: // ISigLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_tabDefChildTreeDataNode;}
protected:
  tabDefChildRef        m_def_child; // for list notifies
  virtual void          DefChild_SigEmit(int sls, void* op1, void* op2);
private:
  void                  init(taSigLinkTaOBase* link_, int dn_flags_); // #IGNORE
};

#endif // tabDefChildTreeDataNode_h
