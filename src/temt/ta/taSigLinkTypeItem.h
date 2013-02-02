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

#ifndef taSigLinkTypeItem_h
#define taSigLinkTypeItem_h 1

// parent includes:
#include <taSigLinkClass>

// member includes:
#include <TypeItem>

// declare all other types mentioned but not required to include:


TypeDef_Of(taSigLinkTypeItem);

class TA_API taSigLinkTypeItem: public taSigLinkClass { // SigLink for TypeInfo objects
INHERITED(taSigLinkClass)
public:
  TypeItem*             data() {return (TypeItem*)m_data;} //
  TypeItem*             data() const {return (TypeItem*)m_data;} //

  override bool         HasChildItems();
  override String       GetName() const;
  override String       GetDisplayName() const;
  USING(inherited::ShowMember)
  override bool         ShowMember(MemberDef* md); // asks this type if we should show the md member

  taSigLinkTypeItem(TypeItem::TypeInfoKinds tik_, TypeItem* data_);  //
  SL_FUNS(taSigLinkTypeItem); //

protected:
  override iPanelBase*  CreateDataPanel_impl();
  override taiTreeNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeNode* nodePar,
    iTreeView* tvPar, taiTreeNode* after, const String& node_name, int dn_flags);
};

#endif // taSigLinkTypeItem_h
