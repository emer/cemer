// Copyright 2017, Regents of the University of Colorado,
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

#ifndef ControlPanel_h
#define ControlPanel_h 1

// parent includes:
#include <taNBase>
#include <IRefListClient>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <ControlPanelMember_Group>
#include <ControlPanelMethod_Group>
#include <taBase_List>
#include <String_Array>
#include <taBase_RefList>
#include <taMarkUp>

// declare all other types mentioned but not required to include:
class taGroup_impl; // 
class ControlPanelMethod; // 
class DataTable; //
class ControlPanel_Group; //


taTypeDef_Of(ControlPanel);

class TA_API ControlPanel: public taNBase, public virtual IRefListClient {
  // #AKA_SelectEdit #STEM_BASE ##EXT_ctrl ##CAT_Display A user-customizable panel of parameters and functions from anywhere in the project, that are most important for controlling the simulation
  INHERITED(taNBase)
public:
  enum CPState {
    REGULAR,                 // just a regular control panel, not part of a master-and-clone setup
    MASTER,                  // master control panel: used as a template for other panels in the same group -- the first panel in the group, as orchestrated by ControlPanel_Group
    CLONE,                   // clone control panel: is updated from master panel in the same group, which is the first panel in the group, as orchestrated by ControlPanel_Group
  };

  CPState               cp_state; // #READ_ONLY #SHOW control panel state -- is this a master, clone, or regular control panel?  controlled by the owner control panel group
  bool                  updt_while_running; // #AKA_running_updt update the control panel while a program is running -- specs and other objects can be updated quite frequently and can slow things down considerably due to constant updating of the display -- only enable if you need it!
  String                desc;   // #EDIT_DIALOG description of what this control panel contains
  ControlPanelMember_Group     mbrs;   // the members of the control panel
  ControlPanelMethod_Group     mths;   // the methods of the control panel

  bool                  auto_edit; // #HIDDEN #NO_SAVE #OBSOLETE automatically activate control panel upon loading -- replaced with user data user_pinned 

  ControlPanelMember*   mbr(int i) const; // convenience accessor for flat access
  int                   mbrSize(int i) const {return mbrs.leaves;} // flat size
  ControlPanelMethod*   mth(int i) const; // convenience accessor for flat access
  int                   mthSize(int i) const {return mths.leaves;} // flat size

  String                GetTypeDecoKey() const override { return "ControlPanel"; }
  String                GetDesc() const override { return desc; } //
  int                   UpdatePointers_NewPar(taBase* old_par, taBase* new_par) override;
  String                GetToolbarName() const override { return "cntrl panel"; }
  int                   GetEnabled() const override;
  int                   GetSpecialState() const override;
  
  static void           StatSigEmit_Group(taGroup_impl* grp, int sls, void* op1, void* op2);

  
  SIMPLE_CUTLINKS(ControlPanel);
  void InitLinks() override;
  TA_BASEFUNS(ControlPanel);

public: // public API

  inline bool   IsRegular() const { return cp_state == REGULAR; }
  // #CAT_CtrlPanel is this a regular (non-master-clone) control panel?
  inline bool   IsMaster() const  { return cp_state == MASTER; }
  // #CAT_CtrlPanel is this a master
  inline bool   IsClone() const  { return cp_state == CLONE; }
  // #CAT_CtrlPanel is this a clone

  virtual void  RemoveMemberIdx(int idx);
  // #IGNORE remove control panel member at given index and update dialog
  virtual void  RemoveMember(taBase* base, MemberDef* md);
  // #IGNORE  remove control panel member and update dialog
  virtual void  MoveMemberToCtrlPanelIdx(int idx, ControlPanel* cp);
  // #IGNORE move member to control panel
  virtual void  MoveMemberToCtrlPanelGpIdx(int idx, ControlPanel_Group* cp);
  // #IGNORE move member to control panel group (i.e., master)
  virtual void  RemoveMethod(int idx);
  // #IGNORE remove method at given index
  virtual void  EditLabel(int idx);
  // #IGNORE edit the current label - use_default will generate the label

  virtual ParamSet*  Archive(const String& desc);
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel #MENU_SEP_BEFORE #DYN1 archive this set of parameters into a new archived_params parameter set, for later reference -- please provide a description of this archive
  virtual bool       IsArchived();
  // #CAT_CtrlPanel test if this is an archived item -- looks for a non-null owner of type ArchivedParams_Group
  virtual ParamSet*  CopyToParamSet(ParamSet* param_set = NULL);
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel #NULL_OK_0 #NULL_TEXT_0_NewParamSet copy all the members from this control panel into a (new if NULL) param set, and save all the current values in that param set -- provides a quick backup and checkpoint of a set of variables
  virtual void       CopyFromDataTable(DataTable* table, int row_num = -1);
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel copy member values into this control panel from same-named columns in a row of given data table -- if row_num is -1 then the row number is obtained by looking up the name of this control panel in the first column of the data table, which is typical for configuration tables -- if this is a ParamSet then values are copied into saved_value, otherwise goes directly into active values
  virtual void       CopyToDataTable(DataTable* table, int row_num = -1);
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel copy member values from this control panel into same-named columns in a row of given data table -- if row_num is -1 then the row number is obtained by looking up the name of this control panel in the first column of the data table -- a new row is added if not already present -- this is typical for configuration tables -- if this is a ParamSet then values are copied from saved_value, otherwise from current active value
  virtual void       AllStable();
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel #MENU_SEP_BEFORE #CONFIRM #DYN1 set all members to be STABLE -- you can then selectively mark a subset as EXPLORE -- in general don't want too many active fields
  virtual void       AllLocked();
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel #CONFIRM #DYN1 set all members to be LOCKED (not subject to editing of any sort) -- you can then selectively mark a subset as STABLE (editable but not recorded) or EXPLORE -- use this to lock down a very stable set of parameters and prevent further editing

  virtual bool  AddMember(taBase* base, MemberDef* md, const String& xtra_lbl = _nilString, const String& desc = _nilString, const String& sub_gp_nm = _nilString, bool short_label = false);
  // #CAT_CtrlPanel add new member to control panel if it isn't already here (returns true), optionally in a sub group, and optionally with an extra custom label, custom desc, or short label
  virtual bool  AddMemberNm
    (taBase* base, const String& md_nm, const String& xtra_lbl = _nilString, const String& desc = _nilString, const String& sub_gp_nm = _nilString, bool short_label = false);
  // #CAT_CtrlPanel add new member to control panel if it isn't already here (returns true), optionally in a sub group
  virtual bool  AddMemberPrompt(taBase* base, MemberDef* md, bool short_label = false);
  // #CAT_CtrlPanel add new member to control panel, prompting for exact label to use
  virtual bool  AddMethod(taBase* base, MethodDef* md, const String& xtra_lbl = _nilString, const String& sub_gp_nm = _nilString);
  // #CAT_CtrlPanel add new method to control panel if it isn't already here (returns true), optionally in a sub group
  virtual bool  AddMethodNm(taBase* base, const String& md, const String& xtra_lbl = _nilString, const String& desc = _nilString, const String& sub_gp_nm = _nilString);
  // #CAT_CtrlPanel add new method to control panel if it isn't already here (returns true), optionally in a sub group

  virtual int   FindMbrBase(const taBase* base, MemberDef* md);
  // #CAT_CtrlPanel find a given base and member, returns index
  virtual ControlPanelMember*  FindMbrName(const String& mbr_nm, const String& label = "")
  { return mbrs.FindMbrName(mbr_nm, label); }
  // #CAT_CtrlPanel find an item based on member name and, optionally if non-empty, the associated label
  virtual int   FindMethBase(const taBase* base, MethodDef* md);
  // #CAT_CtrlPanel find a given base and method, returns index
  virtual void  MbrUpdated(taBase* base, MemberDef* mbr);
  // #CAT_CtrlPanel let the panel know that a mbr has been updated

  virtual void  RemoveNullItems();
  // #IGNORE remove any null items (null bases or type info) in the control panel -- called automatically in UAE and UAM
  virtual void  MasterTriggerUpdate();
  // #IGNORE trigger an update of the clones because this master item has been updated
  virtual bool  UpdateCloneFromMaster(ControlPanel* master);
  // update this clone control panel from given master control panel -- ensures that the same structure and names of members are present
  static bool  UpdateCloneFromMaster_mbrlist
    (ControlPanelMember_Group* clone, ControlPanelMember_Group* master, ControlPanel* clone_panel);
  // #IGNORE update items in given list (not subgroups)
  static bool  UpdateCloneFromMaster_mbrgps
    (ControlPanelMember_Group* clone, ControlPanelMember_Group* master, ControlPanel* clone_panel);
  // #IGNORE update items in given subgroups
 
  
  virtual void  Reset();
  // #CAT_CtrlPanel #MENU #CONFIRM reset (remove all) current members and methods
  virtual String ToTable(taMarkUp::Format fmt);
  // #CAT_CtrlPanel #MENU #USE_RVAL save current parameters and notes to a text table in given format, suitable for pasting into a wiki or other document
  virtual DataTable* ToDataTable(DataTable* table = NULL);
  // #CAT_CtrlPanel #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable save current parameters and notes to given data table -- if null, a new one is automatically ceated in analysis group

  virtual String GetRecursiveVariableValue(String var_name, bool use_search_vals);
  // #CAT_CtrlPanel return the String value of a variable on a control panel. This recursively runs through paramsets, set on this control panel to find variables further down in the tree in case of nested paramsets
  
  virtual String ExploreMembersToString(bool use_search_vals = false);
  // #CAT_CtrlPanel return a String with a list of space-separated name=value pairs for the explore / search members in this control panel (if use_search_vals, records current search value -- members marked SEARCH will be saved regardless) -- any ControlPanel's that are linked within this one are recursively processed as well (but its members are not scoped in any way, so for this to be useful, names in general must be unique across the different control panels)
  
  virtual bool SearchMembersToList(ControlPanelMember_Group * m_params, ControlPanelMember_Group * m_yoked, int_PArray * m_counts);
  // #CAT_CtrlPanel Add control pannel members that are marked as SEARCH to a list, so that the cluster run search algorith can process them -- any ControlPanel's that are linked within this one are recursively processed as well (but its members are not scoped in any way, so for this to be useful, names in general must be unique across the different control panels)
  
  virtual void  SaveNameValueMembers_impl(ParamSet* param_set, String_Array& name_vals);
  // #IGNORE save name_vals name=value settings (generated by ExploreMembersToString, typically via ClusterRun, can be based on a prior set of param configs) to given param_set, creating members by copying from this, and recursively calling any linked param sets -- remove items from param_vals (each element is a name=value pair) when they have been processed -- outer loop will deal with the remainders
  virtual void  SaveNameValueMembers(ParamSet* param_set, const String& name_vals_str);
  // #CAT_CtrlPanel save name_vals_str name=value settings (generated by ExploreMembersToString, typically via ClusterRun, can be based on a prior set of param configs) to given param_set, creating members by copying from this, and recursively calling any linked param sets

  virtual void  ActivateAll(bool info_msg = true);
  // #CAT_CtrlPanel activate all of the parameter sets on this and any linked param set objects -- copies the saved_value values to be the active (live) values for all parameters -- this one call can be used on a master ClusterRun or ControlPanel to activate everything in Startup program, for example -- info_msg emits a message about each ParamSet as it is activated
  virtual String ParamSetNames(bool recursive = false, bool explore_only = true);
  // #CAT_CtrlPanel return a list of _ separated names of ParamSet (or any other type of ControlPanel) linked within this ControlPanel -- this is typically used in the startup program to obtain a "tag" for the job files that indicates the overall configuration used -- if the recursive flag is set, then each linked control panel also adds any sub-controls to the returned string (at the end), and so-on.. -- the explore_only option selects only items marked as EXPLORE if true

  virtual void  AddMembersAsArgs(bool explore_only = false, bool follow_control_panel_links = true);
  // #CAT_Args add single-valued members (optionally only those marked as EXPLORE for ControlPanel and ParamSet) as registered startup arguments that can be passed on the command line -- this is called automatically by the ControlPanelsFmArgs Program element -- optionally follows any members that are pointers to other control panels, to add them as well (default is true)
  virtual void  SetMembersFromArgs(bool explore_only = false, bool follow_control_panel_links = true);
  // #CAT_Args set single-valued members (optionally only those marked as EXPLORE for ControlPanel and ParamSet) from command-line arguments -- this is called automatically by the ControlPanelsFmArgs Program element -- optionally follows any members that are pointers to other control panels, to add them as well (default is true)

  bool         ReShowEdit(bool force = false);
  // #IGNORE this is just really a synonym for doing a SigEmit

 public: // IRefListClient i/f
  void*        This() override {return this;}
  void         SigDestroying_Ref(taBase_RefList* src, taBase* ta) override;
  // note: item will already have been removed from list
  void         SigEmit_Ref(taBase_RefList* src, taBase* ta,
                           int sls, void* op1, void* op2) override;

protected:
  int            m_changing; // flag so we don't recursively delete bases
  taBase_RefList base_refs; // all bases notify us via this list

  void           UpdateAfterEdit_impl() override;
  void           UpdateAfterMove_impl(taBase* old_owner) override;

  virtual void   SigEmit_Group(taGroup_impl* grp, int sls, void* op1, void* op2);
    // mostly for detecting asynchronous deletes
  virtual void   BaseAdded(ControlPanelItem* sei);
  virtual void   BaseRemoved(ControlPanelItem* sei);
  virtual void   RemoveMember_impl(int idx);
  virtual void   RemoveMethod_impl(int idx);
  virtual bool   AddMember_impl
    (taBase* base, MemberDef* md, const String& lbl, const String& desc, const String&
     sub_gp_nm = _nilString, bool custom_label = false, bool custom_desc = false,
     bool short_label = false);
  virtual bool   AddMethod_impl
    (taBase* base, MethodDef* md, const String& lbl, const String& desc, const String&
     sub_gp_nm = _nilString, bool custom_label = false, bool custom_desc = false);
 
private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const ControlPanel& cp); //
};

TA_SMART_PTRS(TA_API, ControlPanel); //

#endif // ControlPanel_h
