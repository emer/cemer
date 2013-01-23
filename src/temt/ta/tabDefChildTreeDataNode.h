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
#include <tabParTreeDataNode>

// member includes:
#include <tabDefChildRef>

// declare all other types mentioned but not required to include:


TypeDef_Of(tabDefChildTreeDataNode);

class TA_API tabDefChildTreeDataNode: public tabParTreeDataNode {
INHERITED(tabParTreeDataNode)
friend class tabDefChildRef;
public:
  tabDefChildTreeDataNode(tabODataLink* link_, MemberDef* md_,
    taiTreeDataNode* parent_, taiTreeDataNode* after,
    const String& tree_name, int dn_flags_ = 0);
  tabDefChildTreeDataNode(tabODataLink* link_, MemberDef* md_,
    iTreeView* parent_, taiTreeDataNode* after,
    const String& tree_name, int dn_flags_ = 0);
  ~tabDefChildTreeDataNode();
public: // IDataLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_tabDefChildTreeDataNode;}
protected:
  tabDefChildRef        m_def_child; // for list notifies
  virtual void          DefChild_DataChanged(int dcr, void* op1, void* op2);
private:
  void                  init(tabODataLink* link_, int dn_flags_); // #IGNORE
};

#endif // tabDefChildTreeDataNode_h
