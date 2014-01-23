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

#ifndef taSigLinkTypeItemSpace_h
#define taSigLinkTypeItemSpace_h 1

// parent includes:
#include <taSigLinkClass>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taSigLinkTypeItemSpace);

class TA_API taSigLinkTypeItemSpace: public taSigLinkClass {
  // SigLink for XxxSpace objects -- note that it also manages the ListView nodes
INHERITED(taSigLinkClass)
public:
  taPtrList_impl*       data() {return (taPtrList_impl*)m_data;}
  bool         HasChildItems() CPP11_OVERRIDE {return true;}

  taSigLinkTypeItemSpace(TypeItem::TypeInfoKinds tik_, taPtrList_impl* data_,
    taSigLink* &link_ref_);
  SL_FUNS(taSigLinkTypeItemSpace) //
protected:
  taiTreeNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeNode* nodePar,
    iTreeView* tvPar, taiTreeNode* after, const String& node_name, int dn_flags) CPP11_OVERRIDE;
};

#endif // taSigLinkTypeItemSpace_h
