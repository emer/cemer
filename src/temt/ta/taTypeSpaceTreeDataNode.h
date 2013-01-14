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

#ifndef taTypeSpaceTreeDataNode_h
#define taTypeSpaceTreeDataNode_h 1

// parent includes:
#include <taiTreeDataNode>

// member includes:
#include <TypeItem>
#include <taTypeSpaceDataLink_Base>


// declare all other types mentioned but not required to include:
class taTypeInfoDataLink;


class TA_API taTypeSpaceTreeDataNode: public taiTreeDataNode { // node for spaces, ex. enumspace, typespace, etc.
INHERITED(taiTreeDataNode)
public:
  const TypeItem::TypeInfoKinds    tik;

  USING(inherited::data)
  taPtrList_impl*       data() {return ((taTypeSpaceDataLink_Base*)m_link)->data();}
  taTypeInfoDataLink*   child_link(int idx);
  taTypeSpaceDataLink_Base*     link() const {return (taTypeSpaceDataLink_Base*)m_link;}
  bool                  ShowItem(TypeItem* ti) const;
    // determine whether to show, ex. based on a filter
  bool                  ShowType(TypeDef* td) const;
    // determine whether to show, ex. based on a filter
  bool                  ShowMember(MemberDef* md) const;
    // determine whether to show, ex. based on a filter
  bool                  ShowMethod(MethodDef* md) const;
    // determine whether to show, ex. based on a filter

  taTypeSpaceTreeDataNode(taTypeSpaceDataLink_Base* link_, MemberDef* md, taiTreeDataNode* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  taTypeSpaceTreeDataNode(taTypeSpaceDataLink_Base* link_, MemberDef* md, iTreeView* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  ~taTypeSpaceTreeDataNode();
public: // IDataLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_taTypeSpaceTreeDataNode;}
protected:
  override void         willHaveChildren_impl(bool& will) const;
  override void         CreateChildren_impl(); // called by the Node when it needs to create its children
  void                  CreateListItem(taiTreeDataNode* par_node, taiTreeDataNode* after_node, void* el);
private:
  void                  init(taTypeSpaceDataLink_Base* link_, int flags_); // #IGNORE
};

#endif // taTypeSpaceTreeDataNode_h
