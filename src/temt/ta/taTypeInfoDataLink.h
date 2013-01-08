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

#ifndef taTypeInfoDataLink_h
#define taTypeInfoDataLink_h 1

// parent includes:
#include <taClassDataLink>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taTypeInfoDataLink: public taClassDataLink { // DataLink for TypeInfo objects
INHERITED(taClassDataLink)
public:
  TypeItem*             data() {return (TypeItem*)m_data;} //
  TypeItem*             data() const {return (TypeItem*)m_data;} //

  override bool         HasChildItems();
  override String       GetName() const;
  override String       GetDisplayName() const;
  USING(inherited::ShowMember)
  override bool         ShowMember(MemberDef* md); // asks this type if we should show the md member

  taTypeInfoDataLink(taMisc::TypeInfoKind tik_, TypeItem* data_);  //
  DL_FUNS(taTypeInfoDataLink); //

protected:
  override iDataPanel*  CreateDataPanel_impl();
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
    iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags);
};

#endif // taTypeInfoDataLink_h
