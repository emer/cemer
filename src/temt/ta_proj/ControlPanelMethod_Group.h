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

#ifndef ControlPanelMethod_Group_h
#define ControlPanelMethod_Group_h 1

// parent includes:
#include <ControlPanelMethod>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class TypeDef; // 


taTypeDef_Of(ControlPanelMethod_Group);

class TA_API ControlPanelMethod_Group : public taGroup<ControlPanelMethod> {
  // #AKA_EditMthItem_Group ##CAT_Display group of control panel methods 
INHERITED(taGroup<ControlPanelMethod>)
public:
  enum MthGroupType {
    GT_BUTTONS,         // make a buttons in the dialog for aach method
    GT_MENU_BUTTON,     // make a button group for the methods (subgroups only)
    GT_MENU             // make a menu for the methods (subgroups only)
  };

  bool                  is_root; // #HIDDEN #READ_ONLY #NO_SAVE is this the first group within the ControlPanel?
  MthGroupType          group_type; // #ENABLE_OFF_is_root how to organize and display the methods in this group

  void                  SetGroupType(MthGroupType group_type);
   // #MENU #MENU_CONTEXT set how the methods will be displayed in the ControlPanel dialog

  taBase*               GetBase_Flat(int idx) const;
    // gets the flat (leaf) base -- NULL if out of bounds or doesn't exist

  int          NumListCols() const override {return 4;}
  // base name, base type, meth name, memb label
  String       GetColHeading(const KeyString& key) const override;
  // header text for the indicated column
  const KeyString GetListColKey(int col) const override;
  String       GetTypeDecoKey() const override { return "ControlPanel"; }
  void         SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) override;
  void  InitLinks() override;
  TA_BASEFUNS(ControlPanelMethod_Group);
#ifdef __MAKETA__ // dummy to supress New
  taBase*       New_gui(int n_objs=1, TypeDef* typ = NULL); // #HIDDEN
#endif
private:
  void  Initialize();
  void  Destroy() { };
  void  Copy_(const ControlPanelMethod_Group& cp);
};

#endif // ControlPanelMethod_Group_h
