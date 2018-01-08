// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef ControlPanelMember_Group_h
#define ControlPanelMember_Group_h 1

// parent includes:
#include <ControlPanelMember>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class TypeDef; // 
class MemberDef; // 
class DataTable; // 


taTypeDef_Of(ControlPanelMember_Group);

class TA_API ControlPanelMember_Group : public taGroup<ControlPanelMember> {
  // #AKA_EditMbrItem_Group ##CAT_Display group of control panel dialog members
INHERITED(taGroup<ControlPanelMember>)
public:
  taBase*               GetBase_Flat(int idx) const;
// gets the flat (leaf) base -- NULL if out of bounds or doesn't exist
  ControlPanelMember*          FindItemBase(taBase* base, MemberDef* md, int& idx) const
  { return (ControlPanelMember*)ControlPanelItem::StatFindItemBase(this, base, md, idx); }
  // find a given base and member, returns index
  ControlPanelMember*          FindMbrName(const String& mbr_nm, const String& label);
  // #CAT_Access find an item based on member name and, optionally if non-empty, the associated label

  int          NumListCols() const override {return 5;}
  // base name, base type, memb name, memb type, memb label
  String       GetColHeading(const KeyString& key) const override;
  // header text for the indicated column
  const KeyString GetListColKey(int col) const override;
  String       GetTypeDecoKey() const override { return "ControlPanel"; }
  void         SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) override;
  TA_BASEFUNS_NOCOPY(ControlPanelMember_Group);
#ifdef __MAKETA__ // dummy to supress New
  taBase*       New_gui(int n_objs=1, TypeDef* typ = NULL); // #HIDDEN
#endif

private:
  void  Initialize() { SetBaseType(&TA_ControlPanelMember);}
  void  Destroy()    { };
};

#endif // ControlPanelMember_Group_h
