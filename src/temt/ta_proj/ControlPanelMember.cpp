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

#include "ControlPanelMember.h"
#include <BuiltinTypeDefs>
#include <DynEnum>
#include <ControlPanel>
#include <ParamSet>
#include <ClusterRun>
#include <taObjDiffRec>
#include <taObjDiff_List>
#include <ArchivedParams_Group>

#include <taMisc>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(ControlPanelMemberData);
TA_BASEFUNS_CTORS_DEFN(ControlPanelMember);

TA_BASEFUNS_CTORS_DEFN(EditParamSearch); // OBSOLETE
TA_BASEFUNS_CTORS_DEFN(ParamSetItem); // OBSOLETE
TA_BASEFUNS_CTORS_DEFN(ControlItemNote); // OBSOLETE


void ControlPanelMemberData::Initialize() {
  ctrl_type = CONTROL;
  state = STABLE;
  is_numeric = false;
  is_single = false;
  obs_record = false;
  obs_search = false;
}

void ControlPanelMemberData::Destroy() {
}

void ControlPanelMemberData::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  SetCtrlType();
  if(taMisc::is_loading) {
    taVersion v807(8, 0, 7);
    if (taMisc::loading_version < v807) {
      if(IsClusterRun()) {
        if(obs_search)
          state = SEARCH;
        else if(obs_record)
          state = EXPLORE;
      }
      else {
        state = STABLE;         // default to stable for everything else
      }
      saved_value.gsub(".param_sets", ".active_params"); // update pointers to new name
    }
    obs_search = false;
    obs_record = false;
  }
  if (state == SEARCH) {
    ParseRange();
  }
}

void ControlPanelMemberData::SetCtrlType() {
  ControlPanel* panel = GET_MY_OWNER(ControlPanel);
  if(!panel) return;
  if(panel->InheritsFrom(&TA_ClusterRun)) {
    ctrl_type = CLUSTER_RUN;
  }
  else if(panel->InheritsFrom(&TA_ParamSet)) {
    ctrl_type = PARAM_SET;
  }
  else {
    ctrl_type = CONTROL;
    state = EXPLORE;             // all control panel members are active by definition!
  }
}


/**
 * Parse a string of the form "1;2;3:1:5;10:2:20" into a list of ranges.
 * Allocates a variety of memory that needs to be freed again.
 * We need to use a list of pointers as the objects aren't immutable.
 */
bool ControlPanelMemberData::ParseRange() {
  int idx, idx2;
  int start_pos = 0;
  int start_pos2 = 0;

  srch_vals.Reset();

  range = trim(range);
  
  if(range.startsWith('%')) {
    return true;                     // deal with it at higher level
  }
  
  idx = range.index(',', start_pos);

  //Iterate over all ; in the range string
  while (idx >= 0) {
    String sub_range = range.at(start_pos, idx - start_pos);
    bool ok = ParseSubRange(sub_range);
    if(!ok) return false;
    start_pos = idx + 1;
    idx = range.index(',', start_pos);
  }
  if(start_pos < range.length()) {
    String sub_range = range.after(start_pos - 1);
    bool ok = ParseSubRange(sub_range);
    if(!ok) return false;
  }
  return (srch_vals.size > 0);  // not ok if no vals.
}

/**
 * Parse a string of the form minValue:maxValue:increment into a structure.
 */
bool ControlPanelMemberData::ParseSubRange(const String& sub_range) {
  int start_pos = 0;
  int idx = sub_range.index(':', start_pos);

  //We assume that we can either have a sigular value, or we need to specify a start value, end value and increment.
  if (idx >= 0) {
    String start_s = sub_range.at(start_pos, idx - start_pos);
    String step_s = "1";
    String end_s;
    start_pos = idx + 1;
    int sidx = sub_range.index(':', start_pos);
    if (sidx > 0) {
      end_s = sub_range.at(start_pos, sidx - start_pos);
      step_s = sub_range.after(sidx);
    }
    else {
      end_s = sub_range.after(idx); // original index
    }
    bool ok;
    double start = start_s.toDouble(&ok);
    if(TestError(!ok, "ParseSubRange",
                 "could not convert string to floating point value:", start_s)) {
      return false;
    }
    double end = end_s.toDouble(&ok);
    if(TestError(!ok, "ParseSubRange",
                 "could not convert string to floating point value:", end_s)) {
      return false;
    }
    double increment = step_s.toDouble(&ok);
    if(TestError(!ok, "ParseSubRange",
                 "could not convert string to floating point value:", step_s)) {
      return false;
    }

    if(TestError(((end < start) || ((end - start) / increment <= 0.0)),
                 "ParseSubRange",
                 "Range specified does not provide a valid set of values: " + sub_range)) {
      return false;
    }

    for(double val = start; val <= end; val += increment) {
      srch_vals.Add(val);
    }
  }
  else {
    bool ok;
    double start = sub_range.toDouble(&ok);
    if(TestError(!ok, "ParseSubRange",
                 "could not convert string to floating point value:", sub_range)) {
      return false;
    }
    srch_vals.Add(start);
  }
  return true;
}


//////////////////////////////////////////////////////
//              Member

void ControlPanelMember::Initialize() {
  mbr = NULL;
}

void ControlPanelMember::Destroy() {
}

void ControlPanelMember::InitLinks() {
  inherited::InitLinks();
  taBase::Own(data, this);

  taBase::Own(obs_param_srch, this);
  taBase::Own(obs_ps_value, this);
  taBase::Own(obs_notes, this);

  SetCtrlType();
}

void ControlPanelMember::Copy_(const ControlPanelMember& cp) {
  mbr = cp.mbr;
  data = cp.data;
  data.SetCtrlType();
}

void ControlPanelMember::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  String def_long_label;
  String def_short_label;
  if(base && mbr) {
    base->GetControlPanelLabel(mbr, def_long_label, "", false);
    base->GetControlPanelLabel(mbr, def_short_label, "", true); // short
  }

  if(taMisc::is_loading) {
    data.SetCtrlType();

    taVersion v806(8, 0, 6);
    if (taMisc::loading_version < v806) {
      if(obs_param_srch.search) {
        SetToSearch();
      }
      else if(obs_param_srch.record) {
        SetToExplore();
      }
      obs_param_srch.search = false;
      obs_param_srch.record = false;
      data.range = obs_param_srch.range;
      data.saved_value = obs_ps_value.saved_value;
      data.notes = obs_notes.notes;
    }

    // update label flags and status on load -- we know obj name didn't just change
    // so it should be safe to update at this point -- not so during other updates
    if(base && mbr) {
      if(label == def_long_label) {
        cust_label = false;
        short_label = false;
      }
      else if(label == def_short_label) {
        cust_label = false;
        short_label = true;
      }
      else {
        cust_label = true;
      }
    }
  }

  data.is_numeric = false;
  data.is_single = false;
  if(mbr && !mbr->HasOption("READ_ONLY") && !mbr->HasOption("GUI_READ_ONLY")) {
    if(mbr->type->IsAtomic() || mbr->type->IsAtomicEff()) {
      data.is_single = true;
      if(mbr->type->IsInt() || mbr->type->IsFloat()) {
        data.is_numeric = true;
      }
      if(base && base->InheritsFrom(&TA_DynEnum)) {
        data.is_numeric = false;     // nix
      }
    }
    else if(mbr->type->IsBasePointerType()) {
      data.is_single = true;    // counts as single!
    }
  }

  // update label and desc during operation now
  if(!taMisc::is_loading && base && mbr) {
    if(!cust_desc && desc != prv_desc) { // we just changed -- might be custom
      cust_desc = true;
    }
    if(!cust_label && label != prv_label) {
      cust_label = true;
    }

    if(!cust_label) {
      if(short_label) {
        if(label != def_short_label) {
          label = def_short_label;
        }
      }
      else {
        if(label != def_long_label) {
          label = def_long_label;
        }
      }
    }
    if(!cust_desc) {
      String def_desc;
      base->GetControlPanelDesc(mbr, def_desc); // regenerate
      if(desc != def_desc) {
        desc = def_desc;
      }
    }
  }


  label = taMisc::StringCVar(label); // keep as safe c variables at all times..
  // all updates over by now!
  prv_desc = desc;
  prv_label = label;

  if(!IsControl()) {
    if(IsSearch()) {
      if(!data.is_single || !data.is_numeric) {
        taMisc::Warning("ControlPanelMember:",label,
                        "cannot SEARCH on parameters that are not elemental numeric values -- reverting to EXPLORE");
        SetToExplore();
      }
    }
    if(IsExplore()) {
      if(!data.is_single) {
        taMisc::Warning("ControlPanelMember:",label,
                        "cannot have EXPLORE parameters that are not elemental values (i.e., no composite objects are allowed) -- reverting to STABLE");
        SetToStable();
      }
    }
  }
}

bool ControlPanelMember::MbrUpdated() {
  // called by control panel when a member is updated -- update our label, desc..
  if(!base || !mbr) return false;
  bool updted = false;
  if(!cust_label) {
    String new_label;
    base->GetControlPanelLabel(mbr, new_label, "", short_label);
    if(label != new_label) {
      label = new_label;
      updted = true;
      prv_label = label;
    }
  }
  if(!cust_desc) {
    String def_desc;
    base->GetControlPanelDesc(mbr, def_desc); // regenerate
    if(desc != def_desc) {
      desc = def_desc;
      updted = true;
      prv_desc = desc;
    }
  }

  if(!IsParamSet() || IsInactive()) {
    return updted;
  }

  ParamSet* psown = GET_MY_OWNER(ParamSet);
  if(!psown) {
    return updted;              // shouldn't happen
  }
  if(!psown->last_activated) {  // only if active guy..
    return updted;
  }

  if(!ActiveEqualsSaved() && base->IsMemberEditable(mbr->name)) {
    String cur_val = CurValAsString();
    if(cur_val != data.last_prompt_val) { // prevent multiple queries on same value!
      data.last_prompt_val = cur_val;
      int chs = taMisc::Choice
        ("ParamSet: " + psown->name + " Member: " + label +
         " has been edited and is different from the saved value\n" +
         "cur value: " + cur_val + "\n" +
         "saved val: " + data.saved_value, "Apply (ParamSet unchanged)", "Apply and Save To ParamSet", "Revert from ParamSet Saved Val");
      if(chs == 0) {
        return updted;
      }
      if(chs == 1) {
        SaveCurrent();
        return true;
      }
      if(chs == 2) {
        Activate();
        data.last_prompt_val = CurValAsString();
        return true;
      }
    }
  }
  return updted;
}

void ControlPanelMember::ActivateAfterEdit() {
  if(!mbr || !base) return;
  if(IsParamSet() && IsExplore()) {
    Activate();
  }
}

String ControlPanelMember::GetColText(const KeyString& key, int itm_idx) const {
  if (key == "mbr_type")
    return (mbr) ? mbr->type->name : String("NULL");
  else return inherited::GetColText(key, itm_idx);
}

String ControlPanelMember::CurValAsString() const {
  if(!mbr) return _nilString;
  
  if(base && base->InheritsFrom(&TA_DynEnum)) {
    String nmval = ((DynEnum*)base)->NameVal();
    if(nmval.nonempty())
      return nmval;             // special case for program enum -- use string
  }
  return mbr->GetValStr(base, TypeDef::SC_VALUE, true);
}

bool ControlPanelMember::SetCurVal(const Variant& cur_val) {
  ControlPanel* panel = GET_MY_OWNER(ControlPanel);
  if(!panel) return false;
  if(TestError(!mbr, "SetCurVal", "item does not have member def set -- not valid control panel item", "label:", label, "in panel:", panel->name))
    return false;
  if(TestError(!data.is_single, "SetCurVal", "item is not a single atomic value and thus not a valid control panel item to set from a command line.  member name:", mbr->name, "label:", label))
    return false;
  if (base && (base->GetTypeDef()->InheritsFrom(&TA_DynEnum)) && (mbr->name == "value") &&
      (cur_val.isStringType())) {
    ((DynEnum*)base)->SetNameVal(cur_val.toString());
  }
  else {
    mbr->type->SetValVar(cur_val, mbr->GetOff(base), NULL, mbr);
  }
  base->UpdateAfterEdit();
  return true;
}

bool ControlPanelMember::SetCurValFmString(const String& cur_val, bool warn_no_match,
                                           bool info_msg) {
  ControlPanel* panel = GET_MY_OWNER(ControlPanel);
  if(!panel) return false;
  if(TestError(!mbr, "SetCurValFmString", "item does not have member def set -- not valid control panel item", "label:", label, "in panel:", panel->name))
    return false;
  // if(TestError(!data.is_single, "SetCurValFmString", "item is not a single atomic value and thus not a valid control panel item to set from a command line.  member name:", mbr->name, "label:", label, "in panel:", panel->name))
  //   return false;
  if (base && (base->GetTypeDef()->InheritsFrom(&TA_DynEnum)) && (mbr->name == "value")) {
    ((DynEnum*)base)->SetNameVal(cur_val);
  }
  else {
    mbr->SetValStr(cur_val, base);
  }
  if((taMisc::dmem_proc > 0) || (!info_msg && !warn_no_match))
    return true;
  String act_val = CurValAsString();
  if(info_msg) {
    taMisc::Info("Set Control Panel Member:",label,"in Panel:", panel->name, "to value:",
                 act_val, "from string:", cur_val);
  }
  if(warn_no_match && act_val != cur_val) {
    taMisc::Warning("Control Panel Member:",label, "in Panel:", panel->name,
                    "DOES NOT MATCH the string value used to set its value in SetCurValFmString, actual value:", act_val, "set from string:", cur_val);
  }
  return true;
}

void ControlPanelMember::GoToObject() {
  if(!mbr || !base) return;
  if(!taMisc::gui_active) return;
  taBase* mbrown = base->GetMemberOwner(true);
  if(!mbrown) 
    mbrown = base;       // must be object itself
  taMisc::Info("Going to:", mbrown->DisplayPath());
  tabMisc::DelayedFunCall_gui(mbrown, "BrowserSelectMe");
}

void ControlPanelMember::MoveTo(ControlPanel* ctrl_panel) {
  if(!ctrl_panel) return;
  ctrl_panel->mbrs.Transfer(this);
}

void ControlPanelMember::MoveToTop() {
  if(!owner || !owner->InheritsFrom(&TA_taGroup_impl)) return;

  if(IsParamSet()) {
    ParamSet_Group* owngp = GET_MY_OWNER(ParamSet_Group);
    if(owngp && owngp->master_and_clones) {
      ParamSet* mstr = (ParamSet*)owngp->GetMaster();
      ParamSet* ps = GET_MY_OWNER(ParamSet);
      if(mstr && (ps != mstr)) {
        ControlPanelMember* mmbr = mstr->mbrs.FindLeafName(label);
        if(mmbr && mmbr != this) {
          mmbr->MoveToTop();
          return;
        }
      }
    }
  }
  taGroup_impl* owngp = (taGroup_impl*)owner;
  int my_idx = owngp->FindEl(this);
  if(my_idx < 0) return;
  owngp->MoveIdx(my_idx, 0);
}

void ControlPanelMember::MoveToBottom() {
  if(!owner || !owner->InheritsFrom(&TA_taGroup_impl)) return;
  if(IsParamSet()) {
    ParamSet_Group* owngp = GET_MY_OWNER(ParamSet_Group);
    if(owngp && owngp->master_and_clones) {
      ParamSet* mstr = (ParamSet*)owngp->GetMaster();
      ParamSet* ps = GET_MY_OWNER(ParamSet);
      if(mstr && (ps != mstr)) {
        ControlPanelMember* mmbr = mstr->mbrs.FindLeafName(label);
        if(mmbr && mmbr != this) {
          mmbr->MoveToBottom();
          return;
        }
      }
    }
  }
  taGroup_impl* owngp = (taGroup_impl*)owner;
  int my_idx = owngp->FindEl(this);
  if(my_idx < 0) return;
  owngp->MoveIdx(my_idx, owngp->size-1);
}

void ControlPanelMember::CopyToAllInGroup() {
  if(!IsParamSet()) return;
  ParamSet_Group* owngp = GET_MY_OWNER(ParamSet_Group);
  if(!owngp) return;
  owngp->SetSavedValue(label, data.saved_value, true);
}

void ControlPanelMember::CopyStateToAllInGroup() {
  if(!IsParamSet()) return;
  ParamSet_Group* owngp = GET_MY_OWNER(ParamSet_Group);
  if(!owngp) return;
  owngp->SetMemberState(label, data.state);
}

void ControlPanelMember::CopyActiveToSaved() {
  if(!mbr || !base) return;
  data.saved_value = CurValAsString();
  SavedToProgVar();
}

bool ControlPanelMember::ActiveEqualsSaved() const {
  if(!IsParamSet()) return true; // only active..
  String active_value = CurValAsString();
  String saved_value = data.saved_value;
  return (active_value == saved_value);
}

void ControlPanelMember::SavedToProgVar() {
  if(!mbr || !base) return;
  TypeDef* mbr_td = mbr->type;
  if(base->InheritsFrom(&TA_DynEnum)) {
    DynEnum* den = (DynEnum*)base;
    data.saved.var_type = ProgVar::T_DynEnum;
    data.saved.dyn_enum_val.CopyFrom(den);
    data.saved.dyn_enum_val.SetNameVal(data.saved_value);
  }
  else if(mbr_td->IsBool()) {
    data.saved.SetBool(data.saved_value.toBool());
  }
  else if(mbr_td->IsInt()) {
    data.saved.SetInt(data.saved_value.toInt());
  }
  else if(mbr_td->IsFloat()) {
    data.saved.SetReal(data.saved_value.toDouble());
  }
  else if(mbr_td->IsEnum()) {
    String enum_tp_nm;
    data.saved.SetHardEnum(mbr_td, mbr_td->GetEnumVal(data.saved_value, enum_tp_nm));
    // this is required for full bits processing:
    mbr_td->SetValStr_enum(data.saved_value, (void*)&(data.saved.int_val), mbr_td);
  }
  else if(mbr_td->IsString()) {
    data.saved.SetString(data.saved_value);
  }
  else if(mbr_td->IsBasePointerType()) {
    if(base->InheritsFrom(&TA_ProgVar)) {
      data.saved.object_type = ((ProgVar*)base)->object_type;
      data.saved.object_scope = ((ProgVar*)base)->object_scope;
    }
    MemberDef* ret_md = NULL;
    taBase* rbase = tabMisc::RootFindFromPath(data.saved_value, ret_md);
    data.saved.SetObject(rbase);
  }
  else {
    data.saved.SetString(data.saved_value);
  }
}

void ControlPanelMember::ProgVarToSaved() {
  if(!mbr || !base) return;
  data.saved_value = data.saved.GetStringVal();
}

void ControlPanelMember::CopySavedToActive() {
  if(!mbr || !base) return;
  if(IsInactive()) return;
  SetCurValFmString(data.saved_value, true, false);
  base->UpdateAfterEdit();
}

void ControlPanelMember::CopySavedToActive_nouae() {
  if(!mbr || !base) return;
  if(IsInactive()) return;
  SetCurValFmString(data.saved_value, false, false);
}

void ControlPanelMember::BaseUpdateAfterEdit() {
  if(!mbr || !base) return;
  base->MemberUpdateAfterEdit(mbr, true); // edit dialog context
  base->UpdateAfterEdit();
}

bool ControlPanelMember::RecordValue() {
  if(!data.is_single) return false;
  return (IsExplore() || IsSearch());
}

String ControlPanelMember::RecordValueString(bool use_search_vals) {
  if(use_search_vals && IsSearch()) {
    return data.next_val;
  }
  if(IsParamSet()) {
    return data.saved_value;
  }
  else {
    return CurValAsString();
  }
}

bool ControlPanelMember::IsArchived() {
  taBase* agp = GetOwner(&TA_ArchivedParams_Group);
  return (agp != NULL);
}

ControlPanel* ControlPanelMember::GetControlPanelPointer() const {
  if(!base) return NULL;
  TypeDef* mbr_td = mbr->type;
  if(!mbr_td->IsBasePointerType()) return NULL;
  taBase* rbase = NULL;
  if(IsParamSet()) {
    MemberDef* ret_md;
    rbase = tabMisc::RootFindFromPath(data.saved_value, ret_md);
  }
  else {
    void* addr = mbr->GetOff(base);
    if((mbr_td->IsPointer()) && mbr_td->IsTaBase()) rbase = *((taBase**)addr);
    else if(mbr_td->InheritsFrom(TA_taSmartRef)) rbase = ((taSmartRef*)addr)->ptr();
    else if(mbr_td->InheritsFrom(TA_taSmartPtr)) rbase = ((taSmartPtr*)addr)->ptr();
  }
  if(rbase && rbase->InheritsFrom(&TA_ControlPanel)) {
    return (ControlPanel*)rbase;
  }
  return NULL;
}

taObjDiffRec* ControlPanelMember::GetObjDiffRec(taObjDiff_List& odl, int nest_lev, MemberDef* memb_def,
                                    const void* par, TypeDef* par_typ, taObjDiffRec* par_od) const {
  // always just add a record for this guy
  taObjDiffRec* odr = new taObjDiffRec(odl, nest_lev, GetTypeDef(), memb_def, (void*)this,
                                       (void*)par, par_typ, par_od);
  odl.Add(odr);
  if(GetOwner()) {
    odr->tabref = new taBaseRef;
    ((taBaseRef*)odr->tabref)->set((taBase*)this);
  }
  // do NOT do sub-classes -- only the main obj -- uses listing text!
  // GetTypeDef()->GetObjDiffRec_class(odl, nest_lev, this, memb_def, par, par_typ, odr);
  return odr;
}

void ControlPanelMember::GetObjDiffValue(taObjDiffRec* rec, taObjDiff_List& odl, bool ptr) const {
  if(ptr) {
    inherited::GetObjDiffValue(rec, odl, ptr);
    return;
  }
  else {
    if(IsParamSet()) {
      rec->value = data.saved_value;
    }
    else {
      rec->value = CurValAsString();
    }
  }
}

