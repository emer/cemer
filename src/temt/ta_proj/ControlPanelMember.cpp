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

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ControlPanelMemberData);
TA_BASEFUNS_CTORS_DEFN(ControlPanelMember);

TA_BASEFUNS_CTORS_DEFN(EditParamSearch); // OBSOLETE
TA_BASEFUNS_CTORS_DEFN(ParamSetItem); // OBSOLETE
TA_BASEFUNS_CTORS_DEFN(ControlItemNote); // OBSOLETE


void ControlPanelMemberData::Initialize() {
  ctrl_type = CONTROL;
  is_numeric = false;
  is_single = false;
  record = true;
  search = false;
}

void ControlPanelMemberData::Destroy() {
}

void ControlPanelMemberData::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  SetCtrlType();
  if (search) {
    record = true;
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

  if(taMisc::is_loading) {
    data.SetCtrlType();
  }
  
  if(taMisc::is_loading) {
    taVersion v806(8, 0, 6);
    if (taMisc::loading_version < v806) {
      data.record = obs_param_srch.record;
      data.search = obs_param_srch.search;
      data.range = obs_param_srch.range;
      data.saved_value = obs_ps_value.saved_value;
      data.notes = obs_notes.notes;
      if(base && mbr) {
        String def_label;
        base->GetControlPanelLabel(mbr, def_label);
        if(label == def_label) {
          cust_label = false;
        }
        else {
          cust_label = true;
        }
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
  }
  if(!data.is_single) {
    data.search = false;
    data.record = false;
  }
  else if(!data.is_numeric && data.search) {
    data.search = false;
    data.record = true;
  }

  if(base && mbr) {
    if(!cust_label) {
      label = "";
      base->GetControlPanelLabel(mbr, label); // regenerate
      prv_label = label;
    }
    if (!cust_desc) {
      desc = "";
      base->GetControlPanelDesc(mbr, desc); // regenerate
      prv_desc = desc;
    }
  }
}

String ControlPanelMember::GetColText(const KeyString& key, int itm_idx) const {
  if (key == "mbr_type")
    return (mbr) ? mbr->type->name : String("NULL");
  else return inherited::GetColText(key, itm_idx);
}

String ControlPanelMember::CurValAsString() {
  if(!mbr) return _nilString;
  
  if(base && base->InheritsFrom(&TA_DynEnum)) {
    String nmval = ((DynEnum*)base)->NameVal();
    if(nmval.nonempty())
      return nmval;             // special case for program enum -- use string
  }
  return mbr->GetValStr(base, TypeDef::SC_STREAMING, true);
}

bool ControlPanelMember::SetCurVal(const Variant& cur_val) {
  if(TestError(!mbr, "SetCurVal", "item does not have member def set -- not valid control panel item"))
    return false;
  if(TestError(!data.is_single, "SetCurVal", "item is not a single atomic value and thus not a valid control panel item to set from a command line.  member name:", mbr->name, "label:", label))
    return false;
  // rohrlich - 1/25/16 - special case to handle setting value when it comes
  // in as string as for instance as a command line arg
  if (base && (base->GetTypeDef()->DerivesFromName("DynEnum")) && (mbr->name == "value") && (cur_val.isStringType())) {
    ((DynEnum*)base)->SetNameVal(cur_val.toString());
  }
  else {
    mbr->type->SetValVar(cur_val, mbr->GetOff(base), NULL, mbr);
  }
  base->UpdateAfterEdit();
  return true;
}

void ControlPanelMember::CopyActiveToSaved() {
  if(!mbr || !base) return;
  data.saved_value = mbr->GetValStr(base);
}

void ControlPanelMember::CopySavedToActive() {
  if(!mbr || !base) return;
  mbr->SetValStr(data.saved_value, base);
  base->UpdateAfterEdit();
}

bool ControlPanelMember::RecordValue() {
  if(!data.is_single) return false;
  if(IsClusterRun()) {
    return (data.record || data.search);
  }
  else {
    return true;
  }
}

bool ControlPanelMember::IsControlPanelPointer() {
  if(!base) return false;
  TypeDef* mbr_td = mbr->type;
  if(mbr_td->IsBasePointerType()) {
    void* addr = mbr->GetOff(base);
    taBase* rbase = NULL;
    if((mbr_td->IsPointer()) && mbr_td->IsTaBase()) rbase = *((taBase**)addr);
    else if(mbr_td->InheritsFrom(TA_taSmartRef)) rbase = ((taSmartRef*)addr)->ptr();
    else if(mbr_td->InheritsFrom(TA_taSmartPtr)) rbase = ((taSmartPtr*)addr)->ptr();
    if(rbase && rbase->InheritsFrom(&TA_ControlPanel)) {
      return true;
    }
  }
  return false;
}

ControlPanel* ControlPanelMember::GetControlPanelPointer() {
  if(!base) return NULL;
  TypeDef* mbr_td = mbr->type;
  if(mbr_td->IsBasePointerType()) {
    void* addr = mbr->GetOff(base);
    taBase* rbase = NULL;
    if((mbr_td->IsPointer()) && mbr_td->IsTaBase()) rbase = *((taBase**)addr);
    else if(mbr_td->InheritsFrom(TA_taSmartRef)) rbase = ((taSmartRef*)addr)->ptr();
    else if(mbr_td->InheritsFrom(TA_taSmartPtr)) rbase = ((taSmartPtr*)addr)->ptr();
    if(rbase && rbase->InheritsFrom(&TA_ControlPanel)) {
      return (ControlPanel*)rbase;
    }
  }
  return NULL;
}

