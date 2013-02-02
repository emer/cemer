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

#ifndef tabListTreeDataNode_h
#define tabListTreeDataNode_h 1

// parent includes:
#include <taiTreeNodeTaBasePar>

// member includes:

// declare all other types mentioned but not required to include:
class taSigLinkList;


TypeDef_Of(tabListTreeDataNode);

class TA_API tabListTreeDataNode: public taiTreeNodeTaBasePar {
INHERITED(taiTreeNodeTaBasePar)
public:
  tabListTreeDataNode(taSigLinkList* link_, MemberDef* md_, taiTreeNode* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  tabListTreeDataNode(taSigLinkList* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  ~tabListTreeDataNode();
public: // ISigLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_tabListTreeDataNode;}
private:
  void                  init(taSigLinkList* link_, int dn_flags_); // #IGNORE
};

#endif // tabListTreeDataNode_h
