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

#ifndef taiTreeNodeTypeSpace_h
#define taiTreeNodeTypeSpace_h 1

// parent includes:
#include <taiTreeNode>

// member includes:
#include <TypeItem>
#include <taSigLinkTypeItemSpace>


// declare all other types mentioned but not required to include:
class taSigLinkTypeItem;


TypeDef_Of(taiTreeNodeTypeSpace);

class TA_API taiTreeNodeTypeSpace: public taiTreeNode { // node for spaces, ex. enumspace, typespace, etc.
INHERITED(taiTreeNode)
public:
  const TypeItem::TypeInfoKinds    tik;

  USING(inherited::data)
  taPtrList_impl*       data() {return ((taSigLinkTypeItemSpace*)m_link)->data();}
  taSigLinkTypeItem*   child_link(int idx);
  taSigLinkTypeItemSpace*     link() const {return (taSigLinkTypeItemSpace*)m_link;}
  bool                  ShowItem(TypeItem* ti) const;
    // determine whether to show, ex. based on a filter
  bool                  ShowType(TypeDef* td) const;
    // determine whether to show, ex. based on a filter
  bool                  ShowMember(MemberDef* md) const;
    // determine whether to show, ex. based on a filter
  bool                  ShowMethod(MethodDef* md) const;
    // determine whether to show, ex. based on a filter

  taiTreeNodeTypeSpace(taSigLinkTypeItemSpace* link_, MemberDef* md, taiTreeNode* parent_,
    taiTreeNode* last_child_, const String& tree_name, int flags_ = 0);
  taiTreeNodeTypeSpace(taSigLinkTypeItemSpace* link_, MemberDef* md, iTreeView* parent_,
    taiTreeNode* last_child_, const String& tree_name, int flags_ = 0);
  ~taiTreeNodeTypeSpace();
public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_taiTreeNodeTypeSpace;}
protected:
  override void         willHaveChildren_impl(bool& will) const;
  override void         CreateChildren_impl(); // called by the Node when it needs to create its children
  void                  CreateListItem(taiTreeNode* par_node, taiTreeNode* after_node, void* el);
private:
  void                  init(taSigLinkTypeItemSpace* link_, int flags_); // #IGNORE
};

#endif // taiTreeNodeTypeSpace_h
