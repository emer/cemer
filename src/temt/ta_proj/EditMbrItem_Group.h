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

#ifndef EditMbrItem_Group_h
#define EditMbrItem_Group_h 1

// parent includes:
#include <EditMbrItem>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class TypeDef; // 
class MemberDef; // 
class DataTable; // 


taTypeDef_Of(EditMbrItem_Group);

class TA_API EditMbrItem_Group : public taGroup<EditMbrItem> {
  // ##CAT_Display group of control panel dialog objects
INHERITED(taGroup<EditMbrItem>)
public:
  taBase*               GetBase_Flat(int idx) const;
// gets the flat (leaf) base -- NULL if out of bounds or doesn't exist
  EditMbrItem*          FindItemBase(taBase* base, MemberDef* md, int& idx) const
  { return (EditMbrItem*)ControlPanelItem::StatFindItemBase(this, base, md, idx); }
  // find a given base and member, returns index
  EditMbrItem*          FindMbrName(const String& mbr_nm, const String& label);
  // #CAT_Access find an item based on member name and, optionally if non-empty, the associated label

  int          NumListCols() const override {return 5;}
  // base name, base type, memb name, memb type, memb label
  String       GetColHeading(const KeyString& key) const override;
  // header text for the indicated column
  const KeyString GetListColKey(int col) const override;
  String       GetTypeDecoKey() const override { return "ControlPanel"; }
  void         SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) override;
  TA_BASEFUNS_NOCOPY(EditMbrItem_Group);
#ifdef __MAKETA__ // dummy to supress New
  taBase*       New_gui(int n_objs=1, TypeDef* typ = NULL); // #NO_SHOW
#endif

private:
  void  Initialize() { SetBaseType(&TA_EditMbrItem);}
  void  Destroy()               { };
};

#endif // EditMbrItem_Group_h
