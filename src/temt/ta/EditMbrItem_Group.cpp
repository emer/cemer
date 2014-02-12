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

#include "EditMbrItem_Group.h"
#include <SelectEdit>
#include <DataTable>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(EditMbrItem_Group);


void EditMbrItem_Group::SigEmit(int sls, void* op1, void* op2)
{
  inherited::SigEmit(sls, op1, op2);
  SelectEdit::StatSigEmit_Group(this, sls, op1, op2);
}

taBase* EditMbrItem_Group::GetBase_Flat(int idx) const {
  taBase* rval = NULL;
  SelectEditItem::StatGetBase_Flat(this, idx, rval);
  return rval;
}

String EditMbrItem_Group::GetColHeading(const KeyString& key) const {
  if (key == "base_name") return "Base Name";
  else if (key == "base_type") return "Base Type";
  else if (key == "item_name") return "Member Name";
  else if (key == "mbr_type") return "Member Type";
  else if (key == "label") return "Label";
  else return inherited::GetColHeading(key);
}

const KeyString EditMbrItem_Group::GetListColKey(int col) const {
  switch (col) {
  case 0: return "base_name";
  case 1: return "base_type";
  case 2: return "item_name"; // mbr or mth
  case 3: return "mbr_type";
  case 4: return "label";
  default: break;
  }
  return inherited::GetListColKey(col);
}

EditMbrItem* EditMbrItem_Group::FindMbrName(const String& mbr_nm, const String& label) {
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr) continue;
    if(sei->mbr->name == mbr_nm) {
      if(label.nonempty()) {
        if(sei->label.contains(label)) {
          return sei;
        }
      }
    }
  }
  return NULL;
}

EditMbrItem* EditMbrItem_Group::PSearchFind(const String& mbr_nm, const String& label) {
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr || !sei->is_numeric) continue;
    if(sei->mbr->name == mbr_nm) {
      if(label.nonempty()) {
        if(sei->label.contains(label)) {
          return sei;
        }
      }
    }
  }
  TestError(true, "PSearchFind", "could not find member named:", mbr_nm, "label:", label);
  return NULL;
}

EditMbrItem* EditMbrItem_Group::PSearchNext(int& st_idx) {
  while(st_idx < leaves) {
    EditMbrItem* sei = Leaf(st_idx);
    if(!sei->mbr || !sei->is_numeric || sei->param_search.srch != EditParamSearch::SRCH) {
      st_idx++;
      continue;
    }
    return sei;
  }
  return NULL;
}


bool EditMbrItem_Group::PSearchOn(const String& mbr_nm, const String& label) {
  static bool no_val = false;
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return no_val;
  return (sei->param_search.srch == EditParamSearch::SRCH);
}

bool EditMbrItem_Group::PSearchOn_Set(bool psearch, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  if(psearch)
    sei->param_search.srch = EditParamSearch::SRCH;
  else
    sei->param_search.srch = EditParamSearch::NO;
  return true;
}

double& EditMbrItem_Group::PSearchMinVal(const String& mbr_nm, const String& label) {
  static double no_val = 0;
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return no_val;
  return sei->param_search.min_val;
}

bool EditMbrItem_Group::PSearchMinVal_Set(double min_val, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  sei->param_search.min_val = min_val;
  return true;
}

double& EditMbrItem_Group::PSearchMaxVal(const String& mbr_nm, const String& label) {
  static double no_val = 0;
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return no_val;
  return sei->param_search.max_val;
}

bool EditMbrItem_Group::PSearchMaxVal_Set(double max_val, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  sei->param_search.max_val = max_val;
  return true;
}

double& EditMbrItem_Group::PSearchNextVal(const String& mbr_nm, const String& label) {
  static double no_val = 0;
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return no_val;
  return sei->param_search.next_val;
}

bool EditMbrItem_Group::PSearchNextVal_Set(double next_val, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  sei->param_search.next_val = next_val;
  return true;
}

double& EditMbrItem_Group::PSearchIncrVal(const String& mbr_nm, const String& label) {
  static double no_val = 0;
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return no_val;
  return sei->param_search.incr;
}
bool EditMbrItem_Group::PSearchIncrVal_Set(double incr_val, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  sei->param_search.incr = incr_val;
  return true;
}

Variant EditMbrItem_Group::PSearchCurVal(const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return _nilVariant;
  return sei->PSearchCurVal();
}

bool EditMbrItem_Group::PSearchCurVal_Set(const Variant& cur_val, const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  return sei->PSearchCurVal_Set(cur_val);
}

bool EditMbrItem_Group::PSearchNextToCur(const String& mbr_nm, const String& label) {
  EditMbrItem* sei = PSearchFind(mbr_nm, label);
  if(!sei)
    return false;
  return sei->PSearchNextToCur();
}

bool EditMbrItem_Group::PSearchMinToCur_All() {
  bool rval = false;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr || !sei->is_numeric || sei->param_search.srch != EditParamSearch::SRCH)
      continue;
    bool psr = sei->PSearchMinToCur();
    rval |= psr;
  }
  return rval;
}

bool EditMbrItem_Group::PSearchNextIncr_Grid() {
  int cur_idx = 0;
  while(true) {
    EditMbrItem* sei = PSearchNext(cur_idx);
    if(!sei) return false;
    if(sei->PSearchNextIncr())
      return true;
    cur_idx++;
  }
  return false;                 // never gets here..
}

bool EditMbrItem_Group::PSearchNextToCur_All() {
  bool rval = false;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr || !sei->is_numeric || sei->param_search.srch != EditParamSearch::SRCH)
      continue;
    bool psr = sei->PSearchNextToCur();
    rval |= psr;
  }
  return rval;
}

void EditMbrItem_Group::PSearchConfigTable(DataTable* dat, bool all_nums, bool add_eval) {
  if(TestError(!dat, "PSearchConfigTable", "data table is NULL"))
    return;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr || !sei->is_numeric) continue;
    if(!all_nums && sei->param_search.srch != EditParamSearch::SRCH) continue;
    String nm = taMisc::StringCVar(sei->label);
    dat->FindMakeCol(nm, VT_DOUBLE);
  }
  if(add_eval)
    dat->FindMakeCol("eval", VT_DOUBLE);
}

void EditMbrItem_Group::PSearchRecordData(DataTable* dat, bool all_nums, bool add_eval,
                                          double eval_val) {
  if(TestError(!dat, "PSearchRecord", "data table is NULL"))
    return;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, sei, *this) {
    if(!sei->mbr || !sei->is_numeric) continue;
    if(!all_nums && sei->param_search.srch != EditParamSearch::SRCH) continue;
    String nm = taMisc::StringCVar(sei->label);
    dat->SetValColName(sei->PSearchCurVal(), nm, -1);
  }
  if(add_eval)
    dat->SetValColName(eval_val, "eval", -1);
  dat->WriteClose();
}
