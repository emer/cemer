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

// declare all other types mentioned but not required to include:
class taGroup_impl; // 
class ControlPanelMember; // 
class ControlPanelMethod; // 
class DataTable; // 


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
  // #CAT_CtrlPanel remove control panel member at given index and update dialog
  virtual void  RemoveMember(taBase* base, MemberDef* md);
  // #CAT_CtrlPanel  remove control panel member and update dialog
  virtual void  RemoveMethod(int idx);
  // #CAT_CtrlPanel  remove method at given index
  virtual void  GoToObject(int idx);
  // #CAT_CtrlPanel select object for given member field for editing in the gui
  virtual void  EditLabel(int idx);
  // #CAT_CtrlPanel edit the current label - use_default will generate the label

  virtual ParamSet*  CopyToParamSet(ParamSet* param_set = NULL);
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel #MENU_SEP_BEFORE #NULL_OK_0 #NULL_TEXT_0_NewParamSet copy all the members from this control panel into a (new if NULL) param set, and save all the current values in that param set -- provides a quick backup and checkpoint of a set of variables
  virtual void CopyFromDataTable(DataTable* table, int row_num = -1);
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel copy member values into this control panel from same-named columns in a row of given data table -- if row_num is -1 then the row number is obtained by looking up the name of this control panel in the first column of the data table, which is typical for configuration tables -- if this is a ParamSet then values are copied into saved_value, otherwise goes directly into active values
  virtual void CopyToDataTable(DataTable* table, int row_num = -1);
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel copy member values from this control panel into same-named columns in a row of given data table -- if row_num is -1 then the row number is obtained by looking up the name of this control panel in the first column of the data table -- a new row is added if not already present -- this is typical for configuration tables -- if this is a ParamSet then values are copied from saved_value, otherwise from current active value

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

  virtual int   FindMbrBase(taBase* base, MemberDef* md);
  // #CAT_CtrlPanel find a given base and member, returns index
  virtual ControlPanelMember*  FindMbrName(const String& mbr_nm, const String& label = "")
  { return mbrs.FindMbrName(mbr_nm, label); }
  // #CAT_CtrlPanel find an item based on member name and, optionally if non-empty, the associated label
  virtual int   FindMethBase(taBase* base, MethodDef* md);
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
    (ControlPanelMember_Group* clone, ControlPanelMember_Group* master);
  // #IGNORE update items in given list (not subgroups)
  static bool  UpdateCloneFromMaster_mbrgps
    (ControlPanelMember_Group* clone, ControlPanelMember_Group* master);
  // #IGNORE update items in given subgroups
 
  
  virtual void  Reset();
  // #CAT_CtrlPanel #MENU #CONFIRM reset (remove all) current members and methods
  virtual String ToWikiTable();
  // #CAT_CtrlPanel #MENU #CONFIRM #USE_RVAL save current parameters and notes to a mediawiki-formatted table, suitable for pasting into a wiki for recording params and notes

  virtual String MembersToString(bool use_search_vals = false);
  // #CAT_CtrlPanel return a String with a list of space-separated name=value pairs for the members in this control panel -- if this is a ClusterRun control panel, then only the members marked as search or record are saved (and if use_search_vals, search records current search value) -- any ControlPanel's that are linked within this one are recursively processed as well (but its members are not scoped in any way, so for this to be useful, names in general must be unique across the different control panels)
  
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
     sub_gp_nm = _nilString, bool custom_label = false, bool custom_desc = false);
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
