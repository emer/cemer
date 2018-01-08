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

#ifndef ControlPanelMember_h
#define ControlPanelMember_h 1

// parent includes:
#include <ControlPanelItem>

// member includes:
#include <MemberDef>
#include <double_Array>
#include <ProgVar>

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
    EXPLORE,          // #COLOR_green #AKA_ACTIVE this parameter is being actively explored -- for param set items, edits to saved values will automatically be activated to the live current values -- this value will also be saved in a ClusterRun and passed as a startup arg on the command line
    STABLE,          // #COLOR_darkblue this is a stable parameter that generally should not change -- it is not recorded specifically in the ClusterRun, but you CAN edit the saved_value if you want
    LOCKED,          // #COLOR_black stronger form of STABLE: the value of this parameter cannot be edited directly -- it is locked down -- change to STABLE to allow editing
    INACTIVE,        // #COLOR_grey the value of this parameter cannot be edited directly,  and the saved value is not copied to the live (active) values for a param set when it is activated -- essentially as if this item was removed from the list -- useful for temporary inactivation instead of removing and then re-adding
    SEARCH,          // #COLOR_red the values of this parameter are determined by the parameter searching mechanism in ClusterRun, according to the range specification -- each searched value will be passed as a command-line argument, and saved in the list of paramters shown in the cluster run
  };

  ControlType           ctrl_type;      // #READ_ONLY type of control panel item
  bool                  is_numeric;     // #READ_ONLY is this a single numeric type (float, double, int)
  bool                  is_single;      // #READ_ONLY is this a single item, not a composite item

  ParamState            state;          // #LABEL_ #CONDSHOW_OFF_ctrl_type:CONTROL what state is this parameter in?  determines many features of how the parameter is used

  String                saved_value;    // #HIDDEN saved value of this parameter -- the value that was in use for a saved parameter set (ParamSet) or the value to be used for a parameter set that is specified to be used -- this value will be copied to the active current value of this object when it is used
  ProgVar               saved;          // #CONDSHOW_ON_ctrl_type:PARAM_SET&&is_single #CONDEDIT_OFF_state:LOCKED,INACTIVE #LABEL_ #EDIT_VALUE #NO_SAVE #NO_CHOOSER gui editor version of saved value of this parameter -- the value that was in use for a saved parameter set (ParamSet) or the value to be used for a parameter set that is specified to be used -- this value will be copied to the active current value of this object when it is used
  taBasePtr             saved_obj;      // #HIDDEN #NO_SAVE #NO_CHOOSER when member is a taBase object that cannot be handled by the ProgVar, we create an object here and use it for editing
  
  String                range;          // #CONDSHOW_ON_state:SEARCH specify the values over which to search this parameter -- specific values can be listed separated by commas , and ranges can be specified using start:stop:increment (increment is optional, defaults to 1) notation as used in Matrix code -- e.g. 1,2,3:10:1,10:20:2 -- however here the stop value here is INCLUSIVE: 1:3:1 = 1,2,3 as that is more often useful -- can also specify %paramname to yoke this parameter to the paramname parameter -- it will not be searched independently, but rather will have the same values as paramname
  double                next_val;       // #HIDDEN #NO_SAVE computed next value to assign to this item in the parameter search
  double_Array          srch_vals;      // #HIDDEN #NO_SAVE full list of search values, parsed from range expression

  String                notes;          // #EDIT_DIALOG #EDIT_WIDTH_40 #CONDEDIT_OFF_state:INACTIVE a place to comment on the effect of parameter or any other comment -- please document early and often!
  String                last_prompt_val; // #IGNORE the last current value of stimulus that user was prompted about regarding differences from saved value
  
  bool                  obs_record;     // #AKA_record #HIDDEN #READ_ONLY #NO_SAVE #OBSOLETE just for loading obsolete data -- now EXPLORE
  bool                  obs_search;     // #AKA_search #HIDDEN #READ_ONLY #NO_SAVE #OBSOLETE just for loading obsolete data -- now SEARCH


  virtual bool          ParseRange();
  // parse the range expression into srch_vals list of explicit values to search over
  virtual bool          ParseSubRange(const String& sub_range);
  // parse the sub-range expression into srch_vals list of explicit values to search over
  virtual void          SetCtrlType(); // based on owner, set ctrl_type field

  inline bool           IsControl() const  { return ctrl_type == CONTROL; }
  // #CAT_CtrlPanel is this a member of a ControlPanel
  inline bool           IsParamSet() const { return ctrl_type == PARAM_SET; }
  // #CAT_CtrlPanel is this a member of a ParamSet
  inline bool           IsClusterRun() const { return ctrl_type == CLUSTER_RUN; }
  // #CAT_CtrlPanel is this a member of a ClusterRun

  inline bool          IsExplore() const { return state == EXPLORE; }
  // is state == EXPLORE?
  inline bool          IsStable() const { return state == STABLE; }
  // is state == STABLE?
  inline bool          IsLocked() const { return state == LOCKED; }
  // is state == LOCKED?
  inline bool          IsInactive() const { return state == INACTIVE; }
  // is state == INACTIVE?
  inline bool          IsSearch() const { return state == SEARCH; }
  // is state == SEARCH?

  inline void          SetToExplore() { state = EXPLORE; }
  // set the state to EXPLORE
  inline void          SetToStable() { state = STABLE; }
  // set the state to STABLE
  inline void          SetToLocked() { state = LOCKED; }
  // set the state to LOCKED
  inline void          SetToInactive() { state = INACTIVE; }
  // set the state to INACTIVE
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
  static bool           member_activating; // #IGNORE flag for when a member is activating -- prevents any prompting about that..
  
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

  virtual void          GoToObject();
  // #CAT_CtrlPanel #BUTTON go in the gui to the object where this member lives 
  virtual void          MoveTo(ControlPanel* ctrl_panel);
  // #CAT_CtrlPanel #BUTTON #ITEM_FILTER_ControlPanelStdItemFilter move this member to other control panel
  virtual void          MoveToTop();
  // #CAT_CtrlPanel #BUTTON move this member to the top of list of members in its group -- automatically does this in Master if a ParamSet in a group
  virtual void          MoveToBottom();
  // #CAT_CtrlPanel #BUTTON move this member to the bottom of list of members in its group -- automatically does this in Master if a ParamSet in a group
  inline void           SaveCurrent() { CopyActiveToSaved(); }
  // #CAT_CtrlPanel #BUTTON #ENABLE_ON_data.ctrl_type:PARAM_SET only for ParamSet elements: copy the current active (live) values on the objects to the saved values
  inline void           Activate() { if(IsParamSet()) CopySavedToActive(); }
  // #CAT_CtrlPanel #BUTTON #ENABLE_ON_data.ctrl_type:PARAM_SET only for ParamSet elements: copy the save_value to be active (live) values on the objects
  virtual void          CopyToAllInGroup();
  // #CAT_CtrlPanel #BUTTON #CONFIRM #ENABLE_ON_data.ctrl_type:PARAM_SET only for ParamSet elements: copy the save_value of this member to all other ParamSet's within the same group as this one -- does NOT copy any ones with state of LOCKED or INACTIVE, so you can protect certain ones with that
  virtual void          CopyStateToAllInGroup();
  // #CAT_CtrlPanel #BUTTON #CONFIRM #ENABLE_ON_data.ctrl_type:PARAM_SET only for ParamSet elements: copy state (EXPLORE, STABLE, etc) of this member to all other ParamSet's within the same group as this one

  virtual void          ActivateAfterEdit();
  // #CAT_CtrlPanel for EXPLORE paramset members, activate after editing -- called via the gui editor 
  
  virtual void          CopyActiveToSaved();
  // #CAT_CtrlPanel for ParamSet elements: copy the current active (live) values on the objects to the saved values
  virtual void          CopySavedToActive();
  // #CAT_CtrlPanel for ParamSet elements: copy the save_value to be active (live) values on the objects
  virtual bool          ActiveEqualsSaved() const;
  // #CAT_CtrlPanel does the current active value equal the saved_value?
  virtual void          CopySavedToActive_nouae();
  // #IGNORE -- no update after edit -- just set the value
  virtual void          BaseUpdateAfterEdit();
  // #CAT_CtrlPanel call MemberUpdateAfterEdit and UpdateAfterEdit on the base object

  virtual bool          RecordValue();
  // #CAT_CtrlPanel whether this member value should be recorded in MembersToString record (e.g., for ClusterRun) -- only single-valued EXPLORE or SEARCH members are so recorded
  virtual String        RecordValueString(bool use_search_vals);
  // #CAT_CtrlPanel a string representation of the value to record for this member -- if use_search_vals then data.next_val is used, otherwise if a ParamSet member then use the saved_value, otherwise the current active value as a string

  virtual void          SavedToProgVar();
  // #IGNORE set the saved progvar from the saved_value string
  virtual void          ProgVarToSaved();
  // #IGNORE set the saved_value string from saved progvar
  
  inline void           SetCtrlType()   { data.SetCtrlType(); }
  // #CAT_CtrlPanel update the ctrl_type based on owner type

  inline bool           IsControl() const  { return data.IsControl(); }
  // #CAT_CtrlPanel is this a member of a ControlPanel
  inline bool           IsParamSet() const { return data.IsParamSet(); }
  // #CAT_CtrlPanel is this a member of a ParamSet
  inline bool           IsClusterRun() const { return data.IsClusterRun(); }
  // #CAT_CtrlPanel is this a member of a ClusterRun
  virtual bool          IsArchived();
  // #CAT_CtrlPanel test if this is an archived item -- looks for a non-null owner of type ArchivedParams_Group

  inline bool          IsExplore() const { return data.IsExplore(); }
  // #CAT_CtrlPanel is state == EXPLORE?
  inline bool          IsStable() const { return data.IsStable(); }
  // #CAT_CtrlPanel is state == STABLE?
  inline bool          IsLocked() const { return data.IsLocked(); }
  // #CAT_CtrlPanel is state == LOCKED?
  inline bool          IsInactive() const { return data.IsInactive(); }
  // #CAT_CtrlPanel is state == INACTIVE?
  inline bool          IsSearch() const { return data.IsSearch(); }
  // #CAT_CtrlPanel is state == SEARCH?

  inline void          SetToExplore() { data.SetToExplore(); SigEmitUpdated(); }
  // #CAT_CtrlPanel #DYN1 set the state to EXPLORE
  inline void          SetToStable() { data.SetToStable(); SigEmitUpdated(); }
  // #CAT_CtrlPanel #DYN1 set the state to STABLE
  inline void          SetToLocked() { data.SetToLocked(); SigEmitUpdated(); }
  // #CAT_CtrlPanel #DYN1 set the state to LOCKED
  inline void          SetToInactive() { data.SetToInactive(); SigEmitUpdated(); }
  // #CAT_CtrlPanel #DYN1 set the state to INACTIVE
  inline void          SetToSearch() { data.SetToSearch(); SigEmitUpdated(); }
  // #CAT_CtrlPanel #DYN1 set the state to SEARCH

  virtual ControlPanel* GetControlPanelPointer() const;
  // #CAT_CtrlPanel if this is a control panel pointer, return the control panel we point to (uses saved_value if a param set item, otherwise live base value)

  virtual bool          MbrUpdated();
  // #IGNORE called by owner when a member sig emit updates -- return true if we updated something
  
  FlatTreeEl*   GetFlatTree(FlatTreeEl_List& ftl, int nest_lev, FlatTreeEl* par_el,
                            const taBase* par_obj, MemberDef* md) const override;
  void          GetFlatTreeValue(FlatTreeEl_List& ftl, FlatTreeEl* ft, bool ptr = false) const override;
  String        BrowserEditString() const override;

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
