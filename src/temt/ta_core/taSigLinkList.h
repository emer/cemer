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
#include <taSigLinkTaOBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taSigLinkList);

class TA_API taSigLinkList: public taSigLinkTaOBase {
  // SigLink for taList objects -- note that it also manages the ListView nodes
INHERITED(taSigLinkTaOBase)
public:
  taList_impl*          list()  override {return (taList_impl*)m_data;}
  taList_impl*          list() const override {return (taList_impl*)m_data;}
  taiSigLink* listLink() override {return this;}

  taSigLinkList(taList_impl* data_);
  SL_FUNS(taSigLinkList) //
protected:
  taiTreeNode* CreateTreeDataNode_impl(MemberDef* md,
    taiTreeNode* nodePar, iTreeView* tvPar, taiTreeNode* after,
    const String& node_name, int dn_flags) override;
};

#endif // taSigLinkList_h
