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

#ifndef taiTreeNodeProgEl_h
#define taiTreeNodeProgEl_h 1

// parent includes:
#include <taiTreeNodeTaBase>

// member includes:

// declare all other types mentioned but not required to include:
class iLineEdit; //

taTypeDef_Of(taiTreeNodeProgEl);

class TA_API taiTreeNodeProgEl : public taiTreeNodeTaBase {
  // tree node for ProgEl items, which can be directly edited within the tree
INHERITED(taiTreeNodeTaBase)
public:

  taiTreeNodeProgEl(taSigLinkTaBase* link_, MemberDef* md_, taiTreeNode* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  taiTreeNodeProgEl(taSigLinkTaBase* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeNode* after, const String& tree_name, int dn_flags_ = 0);
  ~taiTreeNodeProgEl();

  void          itemEdited(int column) override;
  void          lookupKeyPressed(iLineEdit* le, int column) override;

public: // ISigLinkClient interface
//  void*      This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_taiTreeNodeTaBase;}

private:
  void                  init(taSigLinkTaBase* link_, int dn_flags_); // #IGNORE
};

#endif // taiTreeNodeProgEl_h
