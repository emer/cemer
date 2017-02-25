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

  ControlType           ctrl_type;      // #READ_ONLY type of control panel item
  bool                  is_numeric;     // #READ_ONLY is this a single numeric type (float, double, int)
  bool                  is_single;      // #READ_ONLY is this a single item, not a composite item

  String                saved_value;    // #CONDSHOW_ON_ctrl_type:PARAM_SET saved value of this parameter -- the value that was in use for a saved parameter set (ParamSet) or the value to be used for a parameter set that is specified to be used -- this value will be copied to the active current value of this object when it is used

  bool                  record;         // #CONDSHOW_ON_ctrl_type:CLUSTER_RUN&&!search #DEF_true whether we should record the parameter in the parameters set for for the run -- this is automatically true of anything that is being searched -- otherwise the project is saved and run with all current paramters in any case, so this option only determines which parameters are specifically recorded as being manipulated in the current run, as recorded in cluster run jobs tables, etc
  bool                  search;         // #CONDSHOW_ON_ctrl_type:CLUSTER_RUN Use this parameter in a parameter search function, with values that are specified in range -- only applies when a search algorithm is also selected!  automatically records this parameter as well because it will be set by the command line and is NOT the same as that saved with the project
  String                range;          // #CONDSHOW_ON_ctrl_type:CLUSTER_RUN&&search specify the values over which to search this parameter -- specific values can be listed separated by commas , and ranges can be specified using start:stop:increment (increment is optional, defaults to 1) notation as used in Matrix code -- e.g. 1,2,3:10:1,10:20:2 -- however here the stop value here is INCLUSIVE: 1:3:1 = 1,2,3 as that is more often useful -- can also specify %paramname to yoke this parameter to the paramname parameter -- it will not be searched independently, but rather will have the same values as paramname
  double                next_val;       // #HIDDEN #NO_SAVE computed next value to assign to this item in the parameter search
  double_Array          srch_vals;      // #HIDDEN #NO_SAVE full list of search values, parsed from range expression

  String                notes;          // #EDIT_DIALOG #EDIT_WIDTH_40 a place to comment on the effect of parameter or any other comment -- please document early and often!
  
  virtual bool          ParseRange();
  // parse the range expression into srch_vals list of explicit values to search over
  virtual bool          ParseSubRange(const String& sub_range);
  // parse the sub-range expression into srch_vals list of explicit values to search over
  virtual void          SetCtrlType(); // based on owner, set ctrl_type field

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

  virtual String        CurValAsString();
  // #CAT_ControlPanel get current value of item, as a string -- not subject to validity for parameter searching
  virtual bool          SetCurVal(const Variant& cur_val);
  // #CAT_ControlPanel set current value of item from a variant

  virtual void          CopyActiveToSaved();
  // #BUTTON for ParamSet elements: copy the current active (live) values on the objects to the saved values
  virtual void          CopySavedToActive();
  // #BUTTON for ParamSet elements: copy the previously-saved values to be active (live) values on the objects

  inline void           SetCtrlType()   { data.SetCtrlType(); }
  // update the ctrl_type based on owner type

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
