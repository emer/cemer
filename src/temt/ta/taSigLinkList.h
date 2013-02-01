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

#ifndef taSigLinkList_h
#define taSigLinkList_h 1

// parent includes:
#include <taSigLinkOBase>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taSigLinkList);

class TA_API taSigLinkList: public taSigLinkOBase {
  // SigLink for taList objects -- note that it also manages the ListView nodes
INHERITED(taSigLinkOBase)
public:
  taList_impl*          list() {return (taList_impl*)m_data;}
  taList_impl*          list() const {return (taList_impl*)m_data;}
  override taiSigLink* listLink() {return this;}

  taSigLinkList(taList_impl* data_);
  SL_FUNS(taSigLinkList) //
protected:
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md,
    taiTreeDataNode* nodePar, iTreeView* tvPar, taiTreeDataNode* after,
    const String& node_name, int dn_flags);
};

#endif // taSigLinkList_h
