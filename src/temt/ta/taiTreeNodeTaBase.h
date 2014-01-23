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

#ifndef taiTreeNodeTaBase_h
#define taiTreeNodeTaBase_h 1

// parent includes:
#include <taiTreeNode>

// member includes:
#include <taSigLinkTaBase>

// declare all other types mentioned but not required to include:

taTypeDef_Of(taiTreeNodeTaBase);

class TA_API taiTreeNodeTaBase: public taiTreeNode {
INHERITED(taiTreeNode)
public:
  taBase*               tadata() {return ((taSigLinkTaBase*)m_link)->data();}
  taSigLinkTaBase*          link() const {return (taSigLinkTaBase*)m_link;}

  taiTreeNodeTaBase(taSigLinkTaBase* link_, MemberDef* md_, taiTreeNode* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  taiTreeNodeTaBase(taSigLinkTaBase* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  ~taiTreeNodeTaBase();
public: // ISigLinkClient interface
//  void*      This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_taiTreeNodeTaBase;}
protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) override;
private:
  void                  init(taSigLinkTaBase* link_, int dn_flags_); // #IGNORE
};

#endif // taiTreeNodeTaBase_h
