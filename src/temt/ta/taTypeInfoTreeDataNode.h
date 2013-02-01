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

#ifndef taTypeInfoTreeDataNode_h
#define taTypeInfoTreeDataNode_h 1

// parent includes:
#include <taiTreeDataNode>

// member includes:
#include <TypeItem>
#include <taSigLinkTypeItem>

// declare all other types mentioned but not required to include:


TypeDef_Of(taTypeInfoTreeDataNode);

class TA_API taTypeInfoTreeDataNode: public taiTreeDataNode { // node for type info, like type, enum, method, etc.
INHERITED(taiTreeDataNode)
public:
  const TypeItem::TypeInfoKinds    tik;

  USING(inherited::data)
  TypeItem*             data() {return ((taSigLinkTypeItem*)m_link)->data();}
  taSigLinkTypeItem*   link() const {return (taSigLinkTypeItem*)m_link;}


  taTypeInfoTreeDataNode(taSigLinkTypeItem* link_, MemberDef* md, taiTreeDataNode* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  taTypeInfoTreeDataNode(taSigLinkTypeItem* link_, MemberDef* md, iTreeView* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  ~taTypeInfoTreeDataNode();
public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_taTypeInfoTreeDataNode;}
protected:
  override void         willHaveChildren_impl(bool& will) const;
  override void         CreateChildren_impl(); // called by the Node when it needs to create
//  override void               DataChanged_impl(int dcr, void* op1, void* op2);
private:
  void                  init(taSigLinkTypeItem* link_, int flags_); // #IGNORE
};

#endif // taTypeInfoTreeDataNode_h
