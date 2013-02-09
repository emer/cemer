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

#ifndef taSigLinkTaOBase_h
#define taSigLinkTaOBase_h 1

// parent includes:
#include <taSigLinkTaBase>

// member includes:
#include <taOBase>
#include <taList_impl>

// declare all other types mentioned but not required to include:


taTypeDef_Of(taSigLinkTaOBase);

class TA_API taSigLinkTaOBase: public taSigLinkTaBase { // SigLink for taOBase objects
INHERITED(taSigLinkTaBase)
public:
  taOBase*              data() {return (taOBase*)m_data;}
  taOBase*              data() const {return (taOBase*)m_data;}

  override MemberDef*   GetDataMemberDef() const;

  taSigLinkTaOBase(taOBase* data_);
  SL_FUNS(taSigLinkTaOBase); //

public: // for taLists, and default children (where defined) in taOBase
  virtual taList_impl*  list() {return ((taOBase*)m_data)->children_();}
  virtual taList_impl*  list() const {return ((taOBase*)m_data)->children_();}
  virtual taiSigLink*  listLink() {taList_impl* lst = list();
    return (lst) ? (taiSigLink*)lst->GetSigLink() : NULL;}

  override taiTreeNode* CreateTreeDataNode_impl(MemberDef* md,
    taiTreeNode* nodePar, iTreeView* tvPar, taiTreeNode* after,
    const String& node_name, int dn_flags);
  override taiSigLink* GetListChild(int itm_idx);
  override taiSigLink* GetListChild(void* el);
   override int         NumListCols() const; // number of columns in a list view for this item type
  override const KeyString GetListColKey(int col) const; // #IGNORE
  override String       GetColHeading(const KeyString& key) const; // #IGNORE
  override String       ChildGetColText(taSigLink* child, const KeyString& key,
    int itm_idx = -1) const;    // #IGNORE
//   override bool              HasChildItems() {return true;} // at very least, has the 'items' subnode

};

#endif // taSigLinkTaOBase_h
