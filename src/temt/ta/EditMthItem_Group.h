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

#ifndef EditMthItem_Group_h
#define EditMthItem_Group_h 1

// parent includes:
#include <EditMthItem>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class TypeDef; // 


taTypeDef_Of(EditMthItem_Group);

class TA_API EditMthItem_Group : public taGroup<EditMthItem> {
  // ##CAT_Display group of select edit dialog objects
INHERITED(taGroup<EditMthItem>)
public:
  enum MthGroupType {
    GT_BUTTONS,         // make a buttons in the dialog for aach method
    GT_MENU_BUTTON,     // make a button group for the methods (subgroups only)
    GT_MENU             // make a menu for the methods (subgroups only)
  };

  bool                  is_root; // #NO_SHOW #READ_ONLY #NO_SAVE
  MthGroupType          group_type; // #GHOST_ON_is_root how to organize and display the methods in this group

  void                  SetGroupType(MthGroupType group_type);
   // #MENU #MENU_CONTEXT set how the methods will be displayed in the SelectEdit dialog

  taBase*               GetBase_Flat(int idx) const;
    // gets the flat (leaf) base -- NULL if out of bounds or doesn't exist

  int          NumListCols() const override {return 4;}
  // base name, base type, meth name, memb label
  String       GetColHeading(const KeyString& key) const override;
  // header text for the indicated column
  const KeyString GetListColKey(int col) const override;
  String       GetTypeDecoKey() const override { return "SelectEdit"; }
  void         SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) override;
  void  InitLinks();
  TA_BASEFUNS(EditMthItem_Group);
#ifdef __MAKETA__ // dummy to supress New
  taBase*       New_gui(int n_objs=1, TypeDef* typ = NULL); // #NO_SHOW
#endif
private:
  void  Initialize();
  void  Destroy() { };
  void  Copy_(const EditMthItem_Group& cp);
};

#endif // EditMthItem_Group_h
