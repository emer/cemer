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

#ifndef ControlPanel_Group_h
#define ControlPanel_Group_h 1

// parent includes:
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class ControlPanel; //
class DataTable; // 

taTypeDef_Of(ControlPanel);
taTypeDef_Of(ControlPanel_Group);

class TA_API ControlPanel_Group : public taGroup<ControlPanel> {
  // #AKA_SelectEdit_Group  ##CAT_Display ##EXPAND_DEF_1  group of control panel dialog objects
INHERITED(taGroup<ControlPanel>)
public:
  virtual void  AddMember
  (taBase* base, MemberDef* md, const String& xtra_lbl = _nilString, const String& desc = _nilString, const String& sub_gp_nm = _nilString, bool short_label = false);
  // add new member to all control panels in group if it isn't already there, optionally in a sub group
  virtual void  RemoveMember(taBase* base, MemberDef* md);
  //  remove member from all control panels in group and update dialogs

  virtual void  RestorePanels();
  // if the panel was pinned when the program was saved redisplay it on project open

  virtual void CopyFromDataTable(DataTable* table);
  // #MENU #MENU_ON_ControlPanel copy member values into control panels within this group of panels from same-named columns in a row of given data table -- assumes that the first column of the table contains the names of the control panels, which is typical for configuration tables -- if this is a ParamSet then values are copied into saved_value, otherwise goes directly into active values -- at least one control panel must exist within the group already, with members set to appropriate variables -- the first panel is cloned in creating new panels for each row of the data table if they don't exist yet
  virtual void CopyToDataTable(DataTable* table);
  // #MENU #MENU_ON_ControlPanel copy member values from this control panel into same-named columns in a row of given data table -- a new row is added if not already present -- assumes that the first column of the table contains the names of the control panels, which is typical for configuration tables -- if this is a ParamSet then values are copied from saved_value, otherwise from current active value

  String       GetTypeDecoKey() const override { return "ControlPanel"; }

  TA_BASEFUNS(ControlPanel_Group);
private:
  NOCOPY(ControlPanel_Group)
  void  Initialize() { SetBaseType(&TA_ControlPanel);}
  void  Destroy()               { };
};

#endif // ControlPanel_Group_h
