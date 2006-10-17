// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


// ta_type_constr.cc: Type Access Self-Construction Code

#include "ta_type_constr.h"
#include "ta_TA_type.h"

static TypeDef* tac_GetTypeFmName(TypeDef& cur_tp, char* nm) {
  String full_nm = nm;
  TypeDef* rval;
  String sub_nm = full_nm.after("::");
  String tp_nm;
  if(full_nm(0,2) == "::") {
    tp_nm = cur_tp.name;
    rval = cur_tp.sub_types.FindName(sub_nm);
  }
  else {
    tp_nm = full_nm.before("::");
    TypeDef* td = taMisc::types.FindName(tp_nm);
    if(td == NULL) {
      // todo: should probably have "undefined ref" thing that then goes back
      // afterwards like in loading/saving and fills things in
      return &TA_int;	// this happens very rarely, and is always an enum (int)
//       taMisc::Error("tac_GetTypeFmName(): type:",tp_nm,"was not found, check include declare order");
//       return NULL;
    }
    rval = td->sub_types.FindName(sub_nm);
  }
  if(rval == NULL) {
    taMisc::Error("tac_GetTypeFmName(): sub_type:",sub_nm,"was not found on type:",
		    tp_nm, "check class definition");
  }
  return rval;
}

void tac_AddEnum(TypeDef& tp, char* name, char* desc, char* opts,
		 char* inh_opts, char* lists, EnumDef_data* dt) {
  if(dt == NULL) return;
  TypeDef* enm = new TypeDef(name, desc, inh_opts, opts, lists, sizeof(int), (void**)0);
  enm->AddParFormal(&TA_enum);
  while(dt->name != NULL) {
    enm->enum_vals.Add(dt->name, dt->desc, dt->opts, dt->val);
    dt++;
  }
  tp.sub_types.Add(enm);
}

void tac_ThisEnum(TypeDef& tp, EnumDef_data* dt) {
  while(dt->name != NULL) {
    tp.enum_vals.Add(dt->name, dt->desc, dt->opts, dt->val);
    dt++;
  }
}


void tac_AddMembers(TypeDef& tp, MemberDef_data* dt) {
  while((dt != NULL) && (dt->type != NULL) || (dt->type_nm != NULL)) {
    if(dt->type == NULL)
      dt->type = tac_GetTypeFmName(tp, dt->type_nm);
    if(dt->type != NULL) {
      MemberDef* md;
      md = new MemberDef(dt->type, dt->name, dt->desc, dt->opts, dt->lists,
			 dt->off, dt->is_static, dt->addr, dt->fun_ptr);
      tp.members.AddUniqNameNew(md);
    }
    dt++;
  }
}

void tac_AddMethods(TypeDef& tp, MethodDef_data* dt) {
  while((dt != NULL) && (dt->type != NULL) || (dt->type_nm != NULL)) {
    if(dt->type == NULL)
      dt->type = tac_GetTypeFmName(tp, dt->type_nm);

    if(dt->type != NULL) {
      MethodDef* md;
      md = new MethodDef(dt->type, dt->name, dt->desc, dt->opts, dt->lists,
			 dt->fun_overld, dt->fun_argc, dt->fun_argd,
			 dt->is_static, dt->addr, dt->stubp, dt->is_virtual);
      MethodArgs_data* fa = dt->fun_args;
      while((fa != NULL) && ((fa->type != NULL) || (fa->type_nm != NULL))) {
	if(fa->type == NULL)
	  fa->type = tac_GetTypeFmName(tp, fa->type_nm);

	md->arg_types.Link(fa->type);
	md->arg_names.Add(fa->name);
	md->arg_defs.Add(fa->def);
	md->arg_vals.Add(fa->def); // initial val is default
	fa++;
      }
      tp.methods.AddUniqNameNew(md);
/*nuke      // use a very simple adduniqnamenew here: all the hard work is already done
      int idx;
      if(tp.methods.FindName(md->name, idx) != NULL)
	tp.methods.Replace(idx, md);
      else
	tp.methods.Add(md);*/
    }
    dt++;
  }
}
