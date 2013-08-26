// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ActrCondition.h"
#include <ActrBuffer>
#include <ActrModel>
#include <ActrProceduralModule>
#include <ActrSlot>

#include <ProgVar>
#include <Layer>
#include <MemberDef>

#include <taMisc>

void ActrCondition::Initialize() {
  flags = CF_NONE;
  cond_src = BUFFER_EQ;
  src_type = &TA_ActrBuffer;
  rel = Relation::EQUAL;
  unit_val = "act";
  dt_row = -1;
  dt_cell = 0;
}

void ActrCondition::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ActrCondition);
  ActrProduction* prod = GET_MY_OWNER(ActrProduction);
  if(prod) {
    prod->UpdateNames();
  }
}

void ActrCondition::CutLinks() {
  CutLinks_taAuto(&TA_ActrCondition);
  inherited::CutLinks();
}

void ActrCondition::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  switch(cond_src) {
  case BUFFER_EQ:
  case BUFFER_QUERY:
    src_type = &TA_ActrBuffer;
    break;
  case PROG_VAR:
    src_type = &TA_ProgVar;
    break;
  case NET_UNIT:
    src_type = &TA_Layer;
    break;
  case OBJ_MEMBER:
    break;
  case DATA_CELL:
    src_type = &TA_DataTable;
    break;
  }
  for(int i=0; i< cmp_chunk.slots.size; i++) {
    ActrSlot* sl = cmp_chunk.slots[i];
    sl->SetSlotFlag(ActrSlot::COND); // must be in cond mode!
    sl->val_type = ActrSlot::LITERAL;
  }
}

void ActrCondition::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(IsOff()) return;
  CheckError(!src, quiet, rval, "source src for condition matching is NULL");
  switch(cond_src) {
  case BUFFER_EQ: {
    CheckError(cmp_chunk.slots.size == 0, quiet, rval,
               "for buffer compare: no slots in cmp_chunk to match");
    break;
  }
  case BUFFER_QUERY:
    break;
  case PROG_VAR:
    break;
  case NET_UNIT: {
    CheckError(unit_name.empty(), quiet, rval,
               "network unit name unit_name is empty -- specify name");
    if(src && unit_name.nonempty()) {
      Layer* lay = (Layer*)src.ptr();
      Unit* un = lay->FindUnitNamed(unit_name, true); // true = error if not found
      if(un) {
        MemberDef* md = un->FindMember(unit_val);
        CheckError(!md, quiet, rval,
                   "network unit unit variable:", unit_val, "not found in unit");
      }
      else {
        CheckError(!un, quiet, rval,
                   "unit name not found:", unit_name, "in layer:",
                   lay->name);
      }
    }
    break;
  }
  case OBJ_MEMBER: {
    CheckError(obj_path.empty(), quiet, rval,
               "object path obj_path is empty -- specify path to comparison value");
    taBase* obj = src.ptr();
    void* mbr_base = NULL;      // base for conditionalizing member itself
    ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
    TypeDef* eff_td = (TypeDef*)obj->GetTypeDef();
    MemberDef* md = TypeDef::FindMemberPathStatic(eff_td, net_base_off, net_mbr_off,
                                                  obj_path, true); // yes warn..
    CheckError(!md, quiet, rval,
	       "object path is not valid -- could not find path:",obj_path,
	       "on object:", obj->GetPathNames());
    break;
  }
  case DATA_CELL: {
    CheckError(dt_col_name.empty(), quiet, rval,
               "data table column name dt_col_name is empty -- specify column name");
    if(src && dt_col_name.nonempty()) {
      DataTable* dt = (DataTable*)src.ptr();
      DataCol* dc = dt->GetColData(dt_col_name);
      CheckError(!dc, quiet, rval,
                 "data column named:", dt_col_name, "not found in data table:",
                 dt->name);
      if(dc) {
        if(dc->isMatrix()) {
          Variant tval = dc->GetMatrixFlatVal(dt_row, dt_cell);
          CheckError(tval.isInvalid(), quiet, rval,
                     "could not access row:", String(dt_row), "cell:", String(dt_cell),
                     "in column:", dc->name,"in table:", dt->name);
            
        }
        else {
          Variant tval = dc->GetVal(dt_row);
          CheckError(tval.isInvalid(), quiet, rval,
                     "could not access row:", String(dt_row),
                     "in column:", dc->name,"in table:", dt->name);
            
        }
      }
    }
    break;
    }
  }

  if(cond_src >= BUFFER_QUERY) {
    CheckError(cmp_val.empty(), quiet, rval,
               "comparison value cmp_val is empty -- must specify value to compare against");
  }
}

String& ActrCondition::Print(String& strm, int indent) const {
  String relstr = TA_Relation.GetEnumString("Relations", rel);
  taMisc::IndentString(strm, indent);
  strm << GetDisplayName() << " ";
  switch(cond_src) {
  case BUFFER_EQ:
    strm << relstr << " ";
    cmp_chunk.Print(strm);
    break;
  case BUFFER_QUERY:
    strm << cmp_val;
    break;
  case PROG_VAR:
  case NET_UNIT:
  case OBJ_MEMBER:
  case DATA_CELL:
    strm << " " << relstr << " " << cmp_val;
    break;
  }
  return strm;
}

String ActrCondition::GetDisplayName() const {
  String strm;
  switch(cond_src) {
  case BUFFER_EQ:
    if(!src) {
      strm << "=<no buffer!>";
    }
    else {
      strm << "=" << src->GetName();
    }
    break;
  case BUFFER_QUERY:
    if(!src) {
      strm << "?<no buffer!>";
    }
    else {
      strm << "?" << src->GetName();
    }
    break;
  case PROG_VAR:
    if(!src) {
      strm << "=<no prog var!>";
    }
    else {
      strm << "=<prog var>" << src->GetName();
    }
    break;
  case NET_UNIT:
    if(!src) {
      strm << "=<no layer!>";
    }
    else {
      strm << "=<unit>" << src->GetName() << "." << unit_name;
    }
    break;
  case OBJ_MEMBER:
    if(!src) {
      strm << "=<no object!>";
    }
    else {
      strm << "=<object>" << src->GetName() + "." << obj_path;
    }
    break;
  case DATA_CELL:
    if(!src) {
      strm << "=<no data table!>";
    }
    else {
      strm << "=<data cell>" << src->GetName() << "[" << dt_col_name << "]["
           << dt_cell << "," << dt_row << "]";
    }
    break;
  }
  return strm;
}

String ActrCondition::GetDesc() const {
  return PrintStr();
}

void ActrCondition::UpdateVars(ActrProduction& prod) {
  if(IsOff()) return;
  if(cond_src != BUFFER_EQ) return;
  for(int i=0; i< cmp_chunk.slots.size; i++) {
    ActrSlot* sl = cmp_chunk.slots[i];
    sl->SetSlotFlag(ActrSlot::COND); // must be in cond mode!
    if(!sl->CondIsVar()) continue;
    String var = sl->GetVarName();
    bool made_new = false;
    prod.vars.FindMakeNameType(var, NULL, made_new); // make sure one exists
  }
}

bool ActrCondition::MatchVarVal(const Variant& var, bool why_not) {
  bool match = false;
  if(rel >= Relation::CONTAINS) {
    match = var.toString().contains(cmp_val);
    if(rel == Relation::NOT_CONTAINS)
      match = !match;
  }
  else if(var.isNumeric() || (rel >= Relation::LESSTHAN)) {
    Relation rl;
    rl.rel = rel;
    rl.val = (double)cmp_val;
    match = rl.Evaluate(var.toDouble());
  }
  else {
    match = (cmp_val == var.toString());
    if(rel == Relation::NOTEQUAL)
      match = !match;
  }
  if(!match && why_not) {
    taMisc::Info("condition:", PrintStr(), "value:", var.toString(),
                 "doesn't match conditions");
  }
  return match;
}

bool ActrCondition::Matches(ActrProduction& prod, bool why_not) {
  if(IsOff()) return true;
  if(!src) {
    if(why_not) {
      taMisc::Info("source to match against is NULL");
    }
    return false;
  }

  bool match = false;
  switch(cond_src) {
  case BUFFER_EQ: {
    ActrBuffer* buf = (ActrBuffer*)src.ptr();
    if(!buf->IsFull()) {
      if(why_not) {
        taMisc::Info("condition:", GetDisplayName(), "buffer:", buf->name, "is empty");
      }
      return false;
    }
    ActrChunk* bc = buf->CurChunk();
    match = cmp_chunk.MatchesProd(prod, bc, false, why_not); // false = not exact
    return match;
    break;
  }
  case BUFFER_QUERY: {
    ActrBuffer* buf = (ActrBuffer*)src.ptr();
    match = buf->QueryMatches(cmp_val, why_not);
    return match;
    break;
  }
  case PROG_VAR: {
    ProgVar* pv = (ProgVar*)src.ptr();
    Variant tval = pv->GetVar();
    match = MatchVarVal(tval, why_not);
    break;
  }
  case NET_UNIT: {
    Layer* lay = (Layer*)src.ptr();
    Unit* un = lay->FindUnitNamed(unit_name, true); // true = error if not found
    if(un) {
      MemberDef* md = un->FindMember(unit_val);
      if(md) {
	Variant tval = md->GetValVar(un);
	match = MatchVarVal(tval, why_not);
      }
      else {
	if(why_not) {
	  taMisc::Info("net unit:", GetDisplayName(), "member path not found:",
		       obj_path);
	}
      }
    }
    else {
      if(why_not) {
        taMisc::Info("net unit:", GetDisplayName(), "unit name not found:",
                     unit_name);
      }
    }
    break;
  }
  case OBJ_MEMBER: {
    taBase* obj = src.ptr();
    void* mbr_base = NULL;      // base for conditionalizing member itself
    ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
    TypeDef* eff_td = (TypeDef*)obj->GetTypeDef();
    MemberDef* md = TypeDef::FindMemberPathStatic(eff_td, net_base_off, net_mbr_off,
                                                  obj_path, true); // yes warn..
    if (md) {
      mbr_base = MemberDef::GetOff_static(obj, net_base_off, net_mbr_off);
      Variant tval = md->type->GetValVar(mbr_base, md);
      match = MatchVarVal(tval, why_not);
    }
    else {
      if(why_not) {
        taMisc::Info("condition:", GetDisplayName(), "member path not found:",
                     obj_path);
      }
    }
    break;
  }
  case DATA_CELL: {
    DataTable* dt = (DataTable*)src.ptr();
    DataCol* dc = dt->GetColData(dt_col_name);
    if(dc) {
      if(dc->isMatrix()) {
        Variant tval = dc->GetMatrixFlatVal(dt_row, dt_cell);
        match = MatchVarVal(tval, why_not);
      }
      else {
        Variant tval = dc->GetVal(dt_row);
        match = MatchVarVal(tval, why_not);
      }
    }
    else {
      if(why_not) {
        taMisc::Info("condition:", GetDisplayName(), "column not found:",
                     dt_col_name);
      }
    }
    break;
  }
  }
  return match;
}

bool ActrCondition::MatchVars(ActrProduction& prod, bool why_not) {
  if(IsOff()) return true;
  if(cond_src != BUFFER_EQ) return true;

  ActrBuffer* buf = (ActrBuffer*)src.ptr();
  if(!buf->IsFull()) return true; // should not fail
  if(why_not) {
    taMisc::Info("condition:", GetDisplayName(),
                 "starting variable-based matching, vars:", prod.PrintVars());
  }
  ActrChunk* bc =buf->CurChunk();
  for(int i=0; i< cmp_chunk.slots.size; i++) {
    ActrSlot* sl = cmp_chunk.slots[i];
    if(!sl->CondIsVar()) continue;
    ActrSlot* var = prod.vars.FindName(sl->GetVarName());
    if(var) {
      ActrSlot* os = bc->slots.SafeEl(i);
      if(os) {
        if(sl->CondIsNeg()) {
          // true = use exact match
          if(var->MatchesProd(prod, os, true, false)) { // this report will be meaningless
            if(why_not) {
              taMisc::Info("condition:", GetDisplayName(),
                           "negative variable test actually matches!",
                           sl->GetDisplayName());
            }
            return false; // only nonmatch!
          }
        }
        else {
          if(!var->MatchesProd(prod, os, true, why_not)) { // true = exact match
            return false; // only nonmatch!
          }
        }
      }
    }
  }
  return true;
}

void ActrCondition::SendCondActions(ActrProceduralModule* proc_mod, ActrModel* model) {
  if(IsOff()) return;

  if(cond_src == BUFFER_EQ) {
    if(TestError(!src, "SendCondActions",
                 "no buffer specified as the source to match against!"))
      return;
    ActrBuffer* buf = (ActrBuffer*)src.ptr();
    if(HasCondFlag(BUF_UPDT_ACT)) {
      model->LogEvent(-1.0f, "procedural", "BUFFER_READ_ACTION", buf->name, "will updt, is null");
      return;
    }
    // todo: not sure about pri here
    model->ScheduleEvent(0.0f, ActrEvent::max_pri, proc_mod, buf->module, buf,
                         "BUFFER_READ_ACTION", buf->name);
  }
  else if(cond_src == BUFFER_QUERY) {
    // note: this already happened so is kinda superflous -- not sure what point is,
    // but matches act-r log
    if(TestError(!src, "SendCondActions",
                 "no buffer specified as the source to match against!"))
      return;
    ActrBuffer* buf = (ActrBuffer*)src.ptr();
    model->LogEvent(-1.0f, "procedural", "QUERY_BUFFER_ACTION", buf->name);
  }
}

bool ActrCondition::SetVal(ActrSlot* slt, const String& val,
                           Relation::Relations rl) {
  if(slt) {
    if(TestError(slt->val.nonempty(), "SetVal",
                 "chunk comparison value had already been set -- to test multiple values of the same slot, please duplicate the condition and divide the tests across them -- can only have one test per slot per condition in this implementation")) {
      slt->val += " ";
    }
    slt->val += val;
    slt->rel = rl;
  }
  else {
    cmp_val = val;              // todo: could test that this is ok..
  }
  return true;
}

bool ActrCondition::SetQuery(const String& sys, const String& val, bool neg) {
  if(cmp_val.nonempty()) {
    cmp_val += "; ";
  }
  cmp_val += sys + " " + val;
  if(neg)
    cmp_val += "-";
  return true;
}
 
