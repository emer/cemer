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

#ifndef ControlPanelMember_h
#define ControlPanelMember_h 1

// parent includes:
#include <ControlPanelItem>

// member includes:
#include <MemberDef>
#include <double_Array>

// declare all other types mentioned but not required to include:

taTypeDef_Of(ControlPanelMemberData);

class TA_API ControlPanelMemberData: public taOBase {
  // #INLINE Data for control panel members, including parameter search, notes, etc
  INHERITED(taOBase)
public:
  enum ControlType { // what type of control panel do we live in?
    CONTROL,         // regular ControlPanel
    PARAM_SET,       // ParamSet: has editable saved value
    CLUSTER_RUN,     // ClusterRun: has search functionality
  };

  enum ParamState {  // what state is the parameter in?
    ACTIVE,          // this parameter is being actively explored -- any edit made to the active value of the parameter will automatically be saved in the saved_value field immediately, so you can just set it directly once and that is what will be used -- this value will also be saved in a ClusterRun and passed as a startup arg on the command line
    STABLE,          // this is a stable parameter that generally should not change -- it is not recorded specifically in the ClusterRun, but you CAN edit the saved_value if you want
    LOCKED,          // a stronger form of STABLE: the saved_value of this parameter cannot be edited directly -- it is locked down -- change to STABLE to allow editing
    SEARCH,          // the values of this parameter are determined by the parameter searching mechanism in ClusterRun, according to the range specification -- each searched value will be passed as a command-line argument, and saved in the list of paramters shown in the cluster run
  };

  ControlType           ctrl_type;      // #READ_ONLY type of control panel item
  bool                  is_numeric;     // #READ_ONLY is this a single numeric type (float, double, int)
  bool                  is_single;      // #READ_ONLY is this a single item, not a composite item

  ParamState            state;          // #LABEL_ what state is this parameter in?  determines many features of how the parameter is used
  
  String                saved_value;    // #HIDDEN saved value of this parameter -- the value that was in use for a saved parameter set (ParamSet) or the value to be used for a parameter set that is specified to be used -- this value will be copied to the active current value of this object when it is used -- gui editor automatically shows this saved value as the live value that you edit, so 
  String                active;         // #CONDSHOW_ON_ctrl_type:PARAM_SET #NO_SAVE #READ_ONLY #SHOW current active value in string form (only valid while editing -- not otherwise updated) -- the editor automatically shows the saved_value for param sets as the thing you edit, so this shows you the current active value for comparison purposes

  String                range;          // #CONDSHOW_ON_state:SEARCH specify the values over which to search this parameter -- specific values can be listed separated by commas , and ranges can be specified using start:stop:increment (increment is optional, defaults to 1) notation as used in Matrix code -- e.g. 1,2,3:10:1,10:20:2 -- however here the stop value here is INCLUSIVE: 1:3:1 = 1,2,3 as that is more often useful -- can also specify %paramname to yoke this parameter to the paramname parameter -- it will not be searched independently, but rather will have the same values as paramname
  double                next_val;       // #HIDDEN #NO_SAVE computed next value to assign to this item in the parameter search
  double_Array          srch_vals;      // #HIDDEN #NO_SAVE full list of search values, parsed from range expression

  String                notes;          // #EDIT_DIALOG #EDIT_WIDTH_40 a place to comment on the effect of parameter or any other comment -- please document early and often!
  
  bool                  obs_record;     // #AKA_record #HIDDEN #READ_ONLY #NO_SAVE #OBSOLETE just for loading obsolete data -- now ACTIVE
  bool                  obs_search;     // #AKA_search #HIDDEN #READ_ONLY #NO_SAVE #OBSOLETE just for loading obsolete data -- now SEARCH


  virtual bool          ParseRange();
  // parse the range expression into srch_vals list of explicit values to search over
  virtual bool          ParseSubRange(const String& sub_range);
  // parse the sub-range expression into srch_vals list of explicit values to search over
  virtual void          SetCtrlType(); // based on owner, set ctrl_type field

  inline bool          IsActive() const { return state == ACTIVE; }
  // is state == ACTIVE?
  inline bool          IsStable() const { return state == STABLE; }
  // is state == STABLE?
  inline bool          IsLocked() const { return state == LOCKED; }
  // is state == LOCKED?
  inline bool          IsSearch() const { return state == SEARCH; }
  // is state == SEARCH?

  inline void          SetToActive() { state = ACTIVE; }
  // set the state to ACTIVE
  inline void          SetToStable() { state = STABLE; }
  // set the state to STABLE
  inline void          SetToLocked() { state = LOCKED; }
  // set the state to LOCKED
  inline void          SetToSearch() { state = SEARCH; }
  // set the state to SEARCH
  
  TA_SIMPLE_BASEFUNS(ControlPanelMemberData);
protected:
  void                  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy();
};

taTypeDef_Of(ParamSetItem);

class TA_API ParamSetItem : public taOBase {
// #INLINE #OBSOLETE only for loading old projects
  INHERITED(taOBase)
public:
  String            saved_value;
  
  TA_SIMPLE_BASEFUNS(ParamSetItem);
private:
  void  Initialize()  { };
  void  Destroy()     { };
};

taTypeDef_Of(ControlItemNote);

class TA_API ControlItemNote : public taOBase {
  // #INLINE #OBSOLETE only for loading old projects
  INHERITED(taOBase)
public:
  String            notes;

  TA_SIMPLE_BASEFUNS(ControlItemNote);
private:
  void Initialize()  { };
  void Destroy()     { };
};

taTypeDef_Of(EditParamSearch);

class TA_API EditParamSearch: public taOBase {
  // #INLINE #OBSOLETE only for loading old projects
  INHERITED(taOBase)
public:
  bool                  record;
  bool                  search;
  String                range;

  TA_SIMPLE_BASEFUNS(EditParamSearch);
private:
  void Initialize()  { };
  void Destroy()     { };
};

taTypeDef_Of(ControlPanelMember);

class TA_API ControlPanelMember: public ControlPanelItem {
  // #AKA_EditMbrItem a member control panel item -- allows editing of just one member from a class
  INHERITED(ControlPanelItem)
public:
  MemberDef*            mbr; // #READ_ONLY #SHOW the mbr type
  ControlPanelMemberData data; // all the data associated with this member, including param search, saved value, notes
  // note: the above must be in a separate subclass due to the way that the taiWidgetMashup works -- it requires a MemberDef to operate on -- the MemberDef to this data member is it..

  EditParamSearch       obs_param_srch; // #AKA_param_search #HIDDEN #READ_ONLY #NO_SAVE #OBSOLETE just for loading obsolete data
  ParamSetItem          obs_ps_value; // #AKA_param_set_value #HIDDEN #READ_ONLY #NO_SAVE #OBSOLETE just for loading obsolete data
  ControlItemNote       obs_notes;    // #AKA_notes #HIDDEN #READ_ONLY #NO_SAVE #OBSOLETE just for loading obsolete data
  
  TypeItem*    typeItem() const override {return mbr;} // the mbr or mth

  virtual String        CurValAsString() const;
  // #CAT_CtrlPanel get current value of item, as a string -- not subject to validity for parameter searching
  virtual bool          SetCurVal(const Variant& cur_val);
  // #CAT_CtrlPanel set current value of item from a variant
  virtual bool          SetCurValFmString(const String& cur_val, bool warn_no_match = true,
                                          bool info_msg = false);
  // #CAT_CtrlPanel set current value of item from a string, optionally warning if string rep of value does not match the value passed in (possibly indicating an error in the string, such as a bad enum value) -- if info_msg is true, then an message is emitted about this value being set

  void                  SaveActive() { CopyActiveToSaved(); }
  // #CAT_CtrlPanel #BUTTON for ParamSet elements: copy the current active (live) values on the objects to the saved values
  void                  Activate() { CopySavedToActive(); }
  // #CAT_CtrlPanel #BUTTON for ParamSet elements: copy the save_value to be active (live) values on the objects
  
  virtual void          CopyActiveToSaved();
  // #CAT_CtrlPanel for ParamSet elements: copy the current active (live) values on the objects to the saved values
  virtual void          CopyToActiveString();
  // #CAT_CtrlPanel for ParamSet elements: copy the current active (live) values on the objects to the active_value string
  virtual void          CopySavedToActive();
  // #CAT_CtrlPanel for ParamSet elements: copy the save_value to be active (live) values on the objects
  virtual void          CopySavedToActive_nouae();
  // #IGNORE -- no update after edit -- just set the value
  virtual void          ActivateActiveString_nouae();
  // #IGNORE -- copy active_value to actual live active state -- no update after edit -- just set the value
  virtual void          BaseUpdateAfterEdit();
  // #CAT_CtrlPanel call MemberUpdateAfterEdit and UpdateAfterEdit on the base object

  virtual bool          RecordValue();
  // #CAT_CtrlPanel whether this member value should be recorded in MembersToString record (e.g., for ClusterRun) -- only single-valued ACTIVE members are so recorded

  inline void           SetCtrlType()   { data.SetCtrlType(); }
  // #CAT_CtrlPanel update the ctrl_type based on owner type

  inline bool           IsControl() const
  { return data.ctrl_type == ControlPanelMemberData::CONTROL; }
  // #CAT_CtrlPanel is this a member of a ControlPanel
  inline bool           IsParamSet() const
  { return data.ctrl_type == ControlPanelMemberData::PARAM_SET; }
  // #CAT_CtrlPanel is this a member of a ParamSet
  inline bool           IsClusterRun() const
  { return data.ctrl_type == ControlPanelMemberData::CLUSTER_RUN; }
  // #CAT_CtrlPanel is this a member of a ClusterRun

  inline bool          IsActive() const { return data.IsActive(); }
  // #CAT_CtrlPanel is state == ACTIVE?
  inline bool          IsStable() const { return data.IsStable(); }
  // #CAT_CtrlPanel is state == STABLE?
  inline bool          IsLocked() const { return data.IsLocked(); }
  // #CAT_CtrlPanel is state == LOCKED?
  inline bool          IsSearch() const { return data.IsSearch(); }
  // #CAT_CtrlPanel is state == SEARCH?

  inline void          SetToActive() { data.SetToActive(); }
  // #CAT_CtrlPanel #DYN1 set the state to ACTIVE
  inline void          SetToStable() { data.SetToStable(); }
  // #CAT_CtrlPanel #DYN1 set the state to STABLE
  inline void          SetToLocked() { data.SetToLocked(); }
  // #CAT_CtrlPanel #DYN1 set the state to LOCKED
  inline void          SetToSearch() { data.SetToSearch(); }
  // #CAT_CtrlPanel #DYN1 set the state to SEARCH

  virtual bool          IsControlPanelPointer() const;
  // #CAT_CtrlPanel is this member a pointer to another control panel?
  virtual ControlPanel* GetControlPanelPointer() const;
  // #CAT_CtrlPanel if this is a control panel pointer, return the current control panel we point to

  taObjDiffRec*  GetObjDiffRec
    (taObjDiff_List& odl, int nest_lev, MemberDef* memb_def=NULL, const void* par=NULL,
     TypeDef* par_typ=NULL, taObjDiffRec* par_od=NULL) const override;
  void         GetObjDiffValue(taObjDiffRec* rec, taObjDiff_List& odl, bool ptr = false)
    const override;
  
  String       GetColText(const KeyString& key, int itm_idx = -1) const override;
  TA_BASEFUNS(ControlPanelMember);
  void  InitLinks() override;
protected:
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const ControlPanelMember& cp);
};

#endif // ControlPanelMember_h
