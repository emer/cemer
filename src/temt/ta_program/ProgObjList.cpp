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

#include "ProgObjList.h"
#include <Program>
#include <ProgVar>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(ProgObjList);

taTypeDef_Of(DataTable);


DataTable* ProgObjList::OneNewTable() {
  return (DataTable*)New(1, &TA_DataTable);
}

DataTable* ProgObjList::NewDataTable(int n_tables) {
  return (DataTable*)New_gui(n_tables, &TA_DataTable); // this is a gui op
}

void ProgObjList::GetVarsForObjs() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return;
  for(int i = 0; i < size; ++i) {
    taBase* obj = FastEl(i);
    String nm = obj->GetName();
    nm = nm.CamelToSnake();
    ProgVar* var = prog->vars.FindObjVar(obj);
    if(var) {
      var->objs_ptr = true;   // make sure
      if(var->object_type != obj->GetTypeDef() || var->name != nm) {
        var->object_type = obj->GetTypeDef();
        var->SetName(nm);
        var->UpdateAfterEdit(); // trigger more updating..
      }
    }
    else {
      var = (ProgVar*)prog->vars.New(1, &TA_ProgVar);
      var->name = nm;
      var->var_type = ProgVar::T_Object;
      var->object_val = obj;
      var->objs_ptr = true;
      var->object_type = obj->GetTypeDef();
      var->ClearVarFlag(ProgVar::CTRL_PANEL); // don't show in ctrl panel by default
      var->UpdateAfterEdit();
    }
  }
  // now cleanup any orphaned 
  for(int i = prog->vars.size-1; i >= 0; --i) {
    ProgVar* var = prog->vars[i];
    if(!(var->objs_ptr && var->var_type == ProgVar::T_Object)) continue;
    int obj_idx = FindEl(var->object_val);
    if(obj_idx < 0) {
      if(is_transfer) {           // flag on taPtrList_impl -- means transferring list not deleting items
        var->objs_ptr = false;
      }
      else {
        prog->vars.RemoveIdx(i); // get rid of it -- not clear if this ever happens...
      }
    }
  }
}

void ProgObjList::SigEmit(int sls, void* op1, void* op2) {
  inherited::SigEmit(sls, op1, op2);
  
  Program* prog = GET_MY_OWNER(Program);
  // don't do if copying - vars get copied anyway as part of parent copy
  if(prog && !prog->HasBaseFlag(COPYING) && !taMisc::is_loading && !taMisc::is_duplicating && !isDestroying()) {
    // must be a move - pass true
    GetVarsForObjs();
  }
}

void ProgObjList::StructUpdateEls(bool begin) {
  for(int i=0;i<size;i++) {
    taBase* itm = FastEl(i);
    itm->StructUpdate(begin);
  }
}

void* ProgObjList::El_Own_(void* it_) {
  // note: gen the name first, so we don't need to do another notify
  // note: setting default name is mostly for things like taMatrix
  // that are normally anon, but that would like a name here in objs
  taBase* it = (taBase*)it_;
  if (it && it->GetName().empty()) {
    // first, try the default token-based naming
    it->SetDefaultName_();
    // if still empty, we could gen a synthetic name
    if (it->GetName().empty()) {
      it->SetName("Obj_" + String(size + 1));
    }
  }
  void* rval = inherited::El_Own_(it_);
  return rval;
}

