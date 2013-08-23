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

#include "ActrAction.h"
#include <ActrProceduralModule>
#include <ActrModel>
#include <ActrSlot>

#include <MemberDef>

#include <taMisc>

void ActrAction::Initialize() {
  flags = AF_NONE;
  action = UPDATE;
  dest_type = &TA_ActrBuffer;
  dt_row = -1;
  dt_cell = 0;
}

void ActrAction::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ActrAction);
  ActrProduction* prod = GET_MY_OWNER(ActrProduction);
  if(prod) {
    prod->UpdateNames();
  }
}

void ActrAction::CutLinks() {
  CutLinks_taAuto(&TA_ActrAction);
  inherited::CutLinks();
}

void ActrAction::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  switch(action) {
  case UPDATE:
  case OVERWRITE:
  case REQUEST: 
  case REQUEST_DIR:
  case CLEAR:
    dest_type = &TA_ActrBuffer;
    break;
  case STOP:
    break;
  case OUTPUT:
    break;
  case PROG_RUN:
    dest_type = &TA_Program;
    break;
  case PROG_VAR:
    dest_type = &TA_ProgVar;
    break;
  case OBJ_MEMBER:
    break;
  case DATA_CELL:
    dest_type = &TA_DataTable;
    break;
  }
  SetChunkTypeFromCond();
}

bool ActrAction::SetChunkTypeFromCond() {
  if(dest_type != &TA_ActrBuffer || !(bool)dest) return false;
  if(chunk.chunk_type) return false; // we're ok -- todo: should always override??
  ActrProduction* prod = GET_MY_OWNER(ActrProduction);
  if(!prod) return false;
  ActrCondition* cnd = prod->FindCondOnBuffer((ActrBuffer*)dest.ptr());
  if(cnd) {
    if(cnd->cmp_chunk.chunk_type) {
      chunk.SetChunkType(cnd->cmp_chunk.chunk_type.ptr());
      return true;
    }
  }
  return false;
}

bool ActrAction::SetBangAction(const String& act) {
  if(act == "output") {
    action = OUTPUT;
  }
  else if(act == "stop") {
    action = STOP;
  }
  else if(act == "eval") {
    taMisc::Info("Note: eval expressions are not supported in C++!");
  }
  else if(act == "safe_eval") {
    taMisc::Info("Note: safe_eval expressions are not supported in C++!");
  }
  else if(act == "bind") {
    taMisc::Info("Note: bind expressions are not supported in C++!");
  }
  else if(act == "mv_bind") {
    taMisc::Info("Note: mv_bind expressions are not supported in C++!");
  }
  else if(act == "safe_bind") {
    taMisc::Info("Note: safe_bind expressions are not supported in C++!");
  }
  else {
    TestError(true, "SetBangAction", "action value: !" + act + "!  not recognized");
    return false;
  }
  return true;
}

void ActrAction::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(action != OUTPUT && action != STOP) {
    CheckError(!dest, quiet, rval, "destination dest for action is NULL");
    if(dest) {
      CheckError(!dest->InheritsFrom(dest_type), quiet, rval,
                 "destination dest for action is not of right type:", dest_type->name,
                 "is type:", dest->GetTypeDef()->name);
    }
  }
  switch(action) {
  case UPDATE:
  case REQUEST:
    CheckError(!chunk.chunk_type, quiet, rval,
               "chunk type not set for update or request");
    break;
  case OVERWRITE:
    break;
  case REQUEST_DIR:
    break;
  case CLEAR:
    break;
  case STOP:
    break;
  case PROG_RUN:
    break;
  case OUTPUT:
    break;
  case PROG_VAR:
    break;
  case OBJ_MEMBER: {
    CheckError(obj_path.empty(), quiet, rval,
               "object path obj_path is empty -- specify path to member to set");
    if(dest) {
      taBase* obj = dest.ptr();
      void* mbr_base = NULL;      // base for conditionalizing member itself
      ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
      TypeDef* eff_td = (TypeDef*)obj->GetTypeDef();
      MemberDef* md = TypeDef::FindMemberPathStatic(eff_td, net_base_off, net_mbr_off,
                                                    obj_path, true); // yes warn..
      CheckError(!md, quiet, rval,
                 "object path is not valid -- could not find path:",obj_path,
                 "on object:", obj->GetPathNames());
    }
    break;
  }
  case DATA_CELL: {
    CheckError(dt_col_name.empty(), quiet, rval,
               "data table column name dt_col_name is empty -- specify column name");
    if(dest && dt_col_name.nonempty()) {
      DataTable* dt = (DataTable*)dest.ptr();
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
  if(action >= OUTPUT) {
    CheckError(val.empty(), quiet, rval,
               "val is empty -- must specify value to set or output");
  }
  ActrProduction* prod = GET_MY_OWNER(ActrProduction);
  if(prod) {
    String vstr = val;
    while(vstr.nonempty() && vstr.contains('=')) {
      String vnm = vstr.after('=');
      vstr = vnm;
      if(vnm.contains(' '))
        vnm = vnm.before(' ');
      vstr = vstr.after(vnm);
      ActrSlot* var = prod->vars.FindName(vnm);
      CheckError(!var, quiet, rval,
                 "could not find variable name:", vnm,
                 "in production variables -- variables must originate in conditions");
    }
    if((bool)chunk.chunk_type) {
      for(int i=0; i<chunk.slots.size; i++) {
        ActrSlot* sl = chunk.slots.FastEl(i);
        if(!sl->CondIsVar()) continue;
        String vnm = sl->GetVarName();
        ActrSlot* var = prod->vars.FindName(vnm);
        CheckError(!var, quiet, rval,
                   "could not find variable name:", vnm,
                   "in production variables -- variables must originate in conditions");
      }
    }
  }
}

String& ActrAction::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << GetDisplayName() << " ";
  switch(action) {
  case UPDATE:
  case REQUEST: 
    chunk.Print(strm);
    break;
  case CLEAR:
    break;
  case STOP:
    break;
  case PROG_RUN:
    break;
  case OUTPUT:
    strm << " " << val;
    if(chunk.chunk_type)
      chunk.Print(strm);
    break;
  case PROG_VAR:
  case OBJ_MEMBER:
  case OVERWRITE:
  case REQUEST_DIR:
  case DATA_CELL:
    strm << " <- " << val;
    break;
  }
  return strm;
}

String ActrAction::GetDisplayName() const {
  String strm;
  switch(action) {
  case UPDATE:
  case OVERWRITE:
    strm << "=";
  case REQUEST:
  case REQUEST_DIR:
    if(action >= REQUEST) strm << "+"; // fall thru possible
  case CLEAR:
    if(action == CLEAR) strm << "-"; // fall thru possible
    if(!dest) {
      strm << "<no buffer!>";
    }
    else {
      strm << ((ActrBuffer*)dest.ptr())->GetName() << " ";
    }
    if(params.nonempty())
      strm << ":" << params;
    break;
  case STOP:
    strm << "!stop!";
    break;
  case PROG_RUN:
    strm << "run ";
    if(dest) {
      strm << dest->GetName() << "()";
    }
    else {
      strm << "<no program!>";
    }
    break;
  case OUTPUT:
    strm << "!output!";
    break;
  case PROG_VAR:
    strm << "var ";
    if(dest) {
      strm << dest->GetName();
    }
    else {
      strm << "<no var!>";
    }
    break;
  case OBJ_MEMBER:
    strm << "obj ";
    if(dest) {
      strm << dest->GetName() + "." << obj_path;
    }
    else {
      strm << "<no obj!>";
    }
    break;
  case DATA_CELL:
    strm << "data table ";
    if(dest) {
      strm << dest->GetName() << "[" << dt_col_name << "]["
           << dt_cell << "," << dt_row << "]";
    }
    else {
      strm << "<no obj!>";
    }
    break;
  }
  return strm;
}

String ActrAction::GetDesc() const {
  return PrintStr();
}

void ActrAction::SetVarsChunk(ActrProduction& prod, ActrChunk* ck) {
  for(int i=0; i<chunk.slots.size; i++) {
    ActrSlot* sl = chunk.slots.FastEl(i);
    if(!sl->CondIsVar()) continue;
    ActrSlot* var = prod.vars.FindName(sl->GetVarName());
    ActrSlot* cs = ck->slots.FastEl(i);
    if(var && cs) {
      cs->CopyValFrom(*var); 
    }
  }    
}

void ActrAction::SetVarsString(ActrProduction& prod, String& str) {
  for(int i=0; i<prod.vars.size; i++) {
    ActrSlot* var = prod.vars.FastEl(i);
    str.gsub("=" + var->name, var->val);
  }    
}

bool ActrAction::DoAction(ActrProduction& prod, 
                          ActrProceduralModule* proc_mod, ActrModel* model) {
  if(IsOff()) return false;
  // todo: what priorities on these??
  if(!dest && action != STOP && action != OUTPUT) return false;
  ActrBuffer* buffer = NULL;
  if(action <= CLEAR) {
    buffer = (ActrBuffer*)dest.ptr();
  }
  switch(action) {
  case UPDATE: {
    // todo: add a flag to action if it has variables or not
    ActrChunk* new_chunk = new ActrChunk;
    new_chunk->CopyFrom(&chunk);
    SetVarsChunk(prod, new_chunk);
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod,
                         buffer->module, buffer,
                         "MOD_BUFFER_CHUNK", params, this, new_chunk);
    break;
  }
  case OVERWRITE: {
    // todo: get new chunk from val expression!
    // ActrChunk* new_chunk = new ActrChunk;
    // new_chunk->CopyFrom(&chunk);
    // SetVarsChunk(prod, new_chunk);
    TestError(true, "DoAction::OVERWRITE", "not yet supported!");
    // model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod,
    //                      buffer->module, buffer,
    //                      "MOD_BUFFER_CHUNK", params, this, new_chunk);
    break;
  }
  case REQUEST: {
    ActrChunk* new_chunk = new ActrChunk;
    new_chunk->CopyFrom(&chunk);
    SetVarsChunk(prod, new_chunk);
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod,
                         buffer->module, buffer,
                         "MODULE_REQUEST", params, this, new_chunk);
    break;
  }
  case REQUEST_DIR: {
    // todo: get new chunk from val expression!
    // ActrChunk* new_chunk = new ActrChunk;
    // new_chunk->CopyFrom(&chunk);
    // SetVarsChunk(prod, new_chunk);
    TestError(true, "DoAction::REQUEST_DIR", "not yet supported!");
    // model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod,
    //                      buffer->module, buffer,
    //                      "MOD_BUFFER_CHUNK", params, this, new_chunk);
    break;
  }
  case CLEAR:
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod,
                         buffer->module, buffer,
                         "CLEAR_BUFFER", buffer->name, this);
    break;
  case STOP:
    model->ScheduleEvent(0.0f, ActrEvent::min_pri, proc_mod, NULL, NULL,
                         "!stop!", "", this);
    break;
  case PROG_RUN: {
    Program* prg = (Program*)dest.ptr();
    if(prg->run_state == Program::NOT_INIT) {
      prg->CallInit(NULL);
    }
    if(TestError(prg->run_state == Program::NOT_INIT,
                 "cannot run program:", prg->name,
                 "because it cannot be initialized -- see console for errors")) {
    }
    else {
      prg->Call(NULL);
    }
    break;
  }
  case OUTPUT: {
    String out = val;
    SetVarsString(prod, out);
    if(chunk.chunk_type) {
      ActrChunk* new_chunk = new ActrChunk;
      new_chunk->CopyFrom(&chunk);
      SetVarsChunk(prod, new_chunk);
      new_chunk->Print(out);
      delete new_chunk;
    }
    taMisc::ConsoleOutput(out);
    break;
  }
  case PROG_VAR: {
    String sval = val;
    SetVarsString(prod, sval);
    ((ProgVar*)dest.ptr())->SetVar(sval);
    break;
  }
  case OBJ_MEMBER: {
    String sval = val;
    SetVarsString(prod, sval);
    taBase* obj = dest.ptr();
    void* mbr_base = NULL;      // base for conditionalizing member itself
    ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
    TypeDef* eff_td = (TypeDef*)obj->GetTypeDef();
    MemberDef* md = TypeDef::FindMemberPathStatic(eff_td, net_base_off, net_mbr_off,
                                                  obj_path, true); // yes warn..
    if(md) {
      mbr_base = MemberDef::GetOff_static(obj, net_base_off, net_mbr_off);
      md->type->SetValStr(sval, mbr_base, NULL, md);
    }
    break;
  }
  case DATA_CELL: {
    String sval = val;
    SetVarsString(prod, sval);
    DataTable* dt = (DataTable*)dest.ptr();
    DataCol* dc = dt->GetColData(dt_col_name);
    if(dc) {
      if(dc->isMatrix()) {
        dc->SetMatrixFlatVal(sval, dt_row, dt_cell);
      }
      else {
        dc->SetVal(sval, dt_row);
      }
    }
    break;
  }
  }
  return true;                  // todo: need error condition tracking etc
}

void ActrAction::InitProg() {
  if(IsOff()) return;
  if(action != PROG_RUN) return;
  Program* prg = (Program*)dest.ptr();
  if(!prg) return;
  prg->CallInit(NULL);
}
