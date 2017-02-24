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
#include <ControlPanelConfig>
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
  // #AKA_SelectEdit #STEM_BASE ##EXT_edit ##CAT_Display A user-customizable panel of parameters and functions from anywhere in the project, that are most important for controlling the simulation
  INHERITED(taNBase)
public:
  static void           StatSigEmit_Group(taGroup_impl* grp, int sls, void* op1, void* op2);

  bool                  updt_while_running; // #AKA_running_updt update the control panel while a program is running -- specs and other objects can be updated quite frequently and can slow things down considerably due to constant updating of the display -- only enable if you need it!
  String                desc;   // #EDIT_DIALOG description of what this edit contains
  ControlPanelMember_Group     mbrs;   // the members of the control panel
  ControlPanelMethod_Group     mths;   // the methods of the control panel

  bool                  auto_edit; // #HIDDEN #NO_SAVE #OBSOLETE automatically activate control panel upon loading -- replaced with user data user_pinned 

  ControlPanelMember*          mbr(int i) const; // convenience accessor for flat access
  int                   mbrSize(int i) const {return mbrs.leaves;} // flat size
  ControlPanelMethod*          mth(int i) const; // convenience accessor for flat access
  int                   mthSize(int i) const {return mths.leaves;} // flat size

  virtual bool          autoEdit() const {return auto_edit;}

  String                GetTypeDecoKey() const override { return "ControlPanel"; }
  String                GetDesc() const override { return desc; } //
  int                   UpdatePointers_NewPar(taBase* old_par, taBase* new_par) override;
  String                GetToolbarName() const override { return "cntrl panel"; }
  
  SIMPLE_LINKS(ControlPanel);
  TA_BASEFUNS(ControlPanel);

public: // public API
  virtual void  RemoveField(int idx);
  //  remove edit data item at given index and update dialog
  virtual void  RemoveFun(int idx);
  //  remove function at given index
  virtual void  GoToObject(int idx);
  // select object for given member field for editing in the gui
  virtual void  EditLabel(int idx);
  // edit the current label - use_default will generate the label

  virtual ParamSet*  CopyToParamSet(ParamSet* param_set = NULL);
  // #MENU #MENU_ON_ControlPanel #MENU_SEP_BEFORE #NULL_OK_0 #NULL_TEXT_0_NewParamSet copy all the members from this control panel into a (new if NULL) param set, and save all the current values in that param set -- provides a quick backup and checkpoint of a set of variables

  virtual bool  SelectMember(taBase* base, MemberDef* md, const String& xtra_lbl = _nilString,
                             const String& desc = _nilString, const String& sub_gp_nm = _nilString);
  // add new member to edit if it isn't already here (returns true), optionally in a sub group
  virtual bool  SelectMemberPrompt(taBase* base, MemberDef* md, const String& desc = _nilString);
  // add new member to edit, prompting for exact label to us
  virtual bool  SelectMemberNm(taBase* base, const String& md_nm,
                               const String& xtra_lbl, const String& desc = _nilString,
                               const String& sub_gp_nm = _nilString);
  // add new member to edit if it isn't already here (returns true), optionally in a sub group
  virtual bool  SelectMethod(taBase* base, MethodDef* md, const String& desc = _nilString);
  // add new method to edit if it isn't already here (returns true), optionally in a sub group
  virtual bool  SelectMethodNm(taBase* base, const String& md, const String& desc = _nilString);
  // add new method to edit if it isn't already here (returns true), optionally in a sub group

  virtual int   FindMbrBase(taBase* base, MemberDef* md);
  // find a given base and member, returns index

  virtual ControlPanelMember*  FindMbrName(const String& mbr_nm, const String& label = "")
  { return mbrs.FindMbrName(mbr_nm, label); }
  // #CAT_Access find an item based on member name and, optionally if non-empty, the associated label

  virtual int   FindMethBase(taBase* base, MethodDef* md);
  // find a given base and method, returns index

  virtual void  MbrUpdated(taBase* base, MemberDef* mbr);
  // let the panel know that a mbr has been updated
  
  virtual void  Reset();
  // #MENU #CONFIRM reset (remove all) current members and methods
  virtual String  ToWikiTable();
  // #MENU #CONFIRM #USE_RVAL save current parameters and notes to a mediawiki-formatted table, suitable for pasting into a wiki for recording params and notes

  bool                  ReShowEdit(bool force = false); // #IGNORE this is just really a synonym for doing a SigEmit

 public: // IRefListClient i/f
  void*        This() override {return this;}
  void         SigDestroying_Ref(taBase_RefList* src, taBase* ta) override;
    // note: item will already have been removed from list
  void         SigEmit_Ref(taBase_RefList* src, taBase* ta,
    int sls, void* op1, void* op2) override;

protected:
  int                   m_changing; // flag so we don't recursively delete bases
  taBase_RefList        base_refs; // all bases notify us via this list

  void         UpdateAfterEdit_impl() override;

  virtual void          SigEmit_Group(taGroup_impl* grp, int sls, void* op1, void* op2);
    // mostly for detecting asynchronous deletes
  virtual void          BaseAdded(ControlPanelItem* sei);
  virtual void          BaseRemoved(ControlPanelItem* sei);
  virtual void          RemoveField_impl(int idx);
  virtual void          RemoveFun_impl(int idx);
  virtual bool          SelectMember_impl(taBase* base, MemberDef* md,
                  const String& lbl, const String& desc, const String& sub_gp_nm = _nilString, bool custom_label = false);
  virtual bool          SelectMethod_impl(taBase* base, MethodDef* md, const String& desc);

private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const ControlPanel& cp); //
};

TA_SMART_PTRS(TA_API, ControlPanel); //

#endif // ControlPanel_h
