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
  // #AKA_SelectEdit_Group  ##CAT_Display ##TOKENS group of control panels 
INHERITED(taGroup<ControlPanel>)
public:
  bool          master_and_clones;
  // if true, the first control panel serves as a master control panel for the rest of the panels in the group, which are updated from the master whenever any items are added, removed, renamed, etc
  
  virtual void  AddMember
  (taBase* base, MemberDef* md, const String& xtra_lbl = _nilString, const String& desc = _nilString, const String& sub_gp_nm = _nilString, bool short_label = false);
  // #CAT_ControlPanel add new member to all control panels in group if it isn't already there, optionally in a sub group
  virtual void  RemoveMember(taBase* base, MemberDef* md);
  // #CAT_ControlPanel remove member from all control panels in group and update dialogs

  virtual void  RestorePanels();
  // #CAT_ControlPanel ifthe panel was pinned when the program was saved redisplay it on project open

  virtual void CopyFromDataTable(DataTable* table);
  // #CAT_ControlPanel #MENU #MENU_ON_ControlPanel #BUTTON copy member values into control panels within this group of panels from same-named columns in a row of given data table -- assumes that the first column of the table contains the names of the control panels, which is typical for configuration tables -- if this is a ParamSet then values are copied into saved_value, otherwise goes directly into active values -- at least one control panel must exist within the group already, with members set to appropriate variables -- the first panel is cloned in creating new panels for each row of the data table if they don't exist yet
  virtual void CopyToDataTable(DataTable* table);
  // #CAT_ControlPanel #MENU #MENU_ON_ControlPanel #BUTTON copy member values from this control panel into same-named columns in a row of given data table -- a new row is added if not already present -- assumes that the first column of the table contains the names of the control panels, which is typical for configuration tables -- if this is a ParamSet then values are copied from saved_value, otherwise from current active value
  virtual void SetMasterAndClones(bool use_master_and_clones);
  // #CAT_ControlPanel #MENU #MENU_ON_ControlPanel #BUTTON configure this group to use the master-and-clones setup or not -- sets the master_and_clones flag and, if true, updates all the clones based on the current configuration of the first control panel in the group, which becomes the master
  virtual void AllStable();
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel #MENU_SEP_BEFORE #BUTTON #CONFIRM set all members to be STABLE -- you can then selectively mark a subset as EXPLORE -- in general don't want too many active fields
  virtual void AllLocked();
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel #CONFIRM #BUTTON #BUTTON set all members to be LOCKED (not subject to editing of any sort) -- you can then selectively mark a subset as STABLE (editable but not recorded) or EXPLORE -- use this to lock down a very stable set of parameters and prevent further editing

  String       GetTypeDecoKey() const override { return "ControlPanel"; }
  void         SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) override;

  virtual void  MasterClonesUpdate();
  // #CAT_ControlPanel update all the control panels for the master-and-clones configuration
  virtual ControlPanel* GetMaster();
  // #CAT_ControlPanel get the first control panel in group

  void  InitLinks() override;
  TA_BASEFUNS(ControlPanel_Group);
  SIMPLE_COPY(ControlPanel_Group);
protected:
  void  UpdateAfterEdit_impl() override;
  
private:
  void  Initialize();
  void  Destroy()               { };
};

#endif // ControlPanel_Group_h
