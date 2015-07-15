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

#ifndef taiTreeNodeTaBasePar_h
#define taiTreeNodeTaBasePar_h 1

// parent includes:
#include <taiTreeNodeTaBase>

// member includes:
#include <taSigLinkTaOBase>

// declare all other types mentioned but not required to include:
class taiTreeNode; //


taTypeDef_Of(taiTreeNodeTaBasePar);

class TA_API taiTreeNodeTaBasePar: public taiTreeNodeTaBase {
INHERITED(taiTreeNodeTaBase)
public:
  taList_impl*          list() const {return ((taSigLinkTaOBase*)m_link)->list();}

  void                  AssertLastListItem(); // #IGNORE updates last_list_items_node -- called by Group node before dynamic inserts/updates etc.
  void         UpdateChildNames() override; // #IGNORE update child names of the indicated node

  virtual bool          RebuildChildrenIfNeeded();
  // checks if child count != list count, and rebuilds children if so

  taiTreeNodeTaBasePar(taSigLinkTaOBase* link_, MemberDef* md_, taiTreeNode* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  taiTreeNodeTaBasePar(taSigLinkTaOBase* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  ~taiTreeNodeTaBasePar();
public: // ISigLinkClient interface
//  void*      This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_taiTreeNodeTaBasePar;}
protected:
  taiTreeNode*      last_list_items_node; // #IGNORE last list member node created, so we know where to start group items
  void         SigEmit_impl(int sls, void* op1, void* op2) override;
  void         CreateChildren_impl() override;
  taiTreeNode*      CreateListItem(taiTreeNode* par_node,
    taiTreeNode* after, taBase* el);
  void                  UpdateListNames(); // #IGNORE updates names after inserts/deletes etc.
  void         willHaveChildren_impl(bool& will) const override;
private:
  void                  init(taSigLinkTaOBase* link_, int dn_flags_); // #IGNORE
};

#endif // taiTreeNodeTaBasePar_h
