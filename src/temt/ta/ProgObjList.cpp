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

TypeDef_Of(DataTable);


DataTable* ProgObjList::NewDataTable(int n_tables) {
  return (DataTable*)New_gui(n_tables, &TA_DataTable); // this is a gui op
}

void ProgObjList::GetVarsForObjs() {
  Program* prog = GET_MY_OWNER(Program);
  if(!prog) return;
  for(int i = 0; i < size; ++i) {
    taBase* obj = FastEl(i);
    String nm = obj->GetName();
    if(nm.empty()) continue;
    ProgVar* var = prog->vars.FindName(nm);
    if(var) {
      if((var->var_type != ProgVar::T_Object) || (var->object_val != obj)) {
        taMisc::Error("Program error: variable named:", nm,
                      "exists, but does not refer to object in objs list -- rename either to avoid conflict");
      }
      else {
        var->objs_ptr = true;   // make sure
        var->object_type = obj->GetTypeDef();
        var->DataItemUpdated();
      }
    }
    else {
      bool found_it = false;
      for(int j=0;j<prog->vars.size; j++) {
        ProgVar* tv = prog->vars[j];
        if((tv->var_type == ProgVar::T_Object) && (tv->object_val.ptr() == obj)) {
          found_it = true;
          tv->name = nm;        // update the name
          tv->objs_ptr = true;  // make sure
          tv->object_type = obj->GetTypeDef();
          tv->UpdateAfterEdit(); // need UAE to update schema sig to cascade to progvar
          //      tv->DataItemUpdated();
          break;
        }
      }
      if(!found_it) {
        var = (ProgVar*)prog->vars.New(1, &TA_ProgVar);
        var->name = nm;
        var->var_type = ProgVar::T_Object;
        var->object_val = obj;
        var->objs_ptr = true;
        var->object_type = obj->GetTypeDef();
        var->ClearVarFlag(ProgVar::CTRL_PANEL); // don't show in ctrl panel by default
        var->DataItemUpdated();
      }
    }
  }
  // now cleanup any orphaned
  for(int i = prog->vars.size-1; i >= 0; --i) {
    ProgVar* var = prog->vars[i];
    if(!var->objs_ptr) continue;
    taBase* obj = FindName(var->name);
    if(obj == NULL)
      prog->vars.RemoveIdx(i);          // get rid of it
  }
}

void ProgObjList::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if(!taMisc::is_loading && !taMisc::is_duplicating && !isDestroying())
    GetVarsForObjs();
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

