// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_type_constr.cc: Type Access Self-Construction Code

#include "ta_type_constr.h"
#include <taString>
#include <TypeDef>
#include <EnumDef>
#include <MethodDef>
#include <MemberDef>
#include <BuiltinTypeDefs>


#include <taMisc>


///////////////////////////////////////////////////////////////////
//        Registrar for keeping track of all init funs

TypeDefInitRegistrar_PtrList* TypeDefInitRegistrar::instances = NULL;

TypeDefInitRegistrar::TypeDefInitRegistrar(TypeDefInitFun types_init_fun_,
                                           TypeDefInitFun data_init_fun_,
                                           TypeDefInitFun inst_init_fun_)
  : types_init_fun(types_init_fun_),
    data_init_fun(data_init_fun_),
    inst_init_fun(inst_init_fun_)
{
  if(!instances)
    instances = new TypeDefInitRegistrar_PtrList;
  instances->Add(this);
}

bool TypeDefInitRegistrar::CallAllTypeInitFuns() {
  if(!instances) {
    taMisc::Error("TypeDefInitRegistrar: no instances found -- something badly wrong!");
    return false;
  }
  for(int i=0; i<instances->size; i++) {
    TypeDefInitRegistrar* it = instances->FastEl(i);
    (*(it->types_init_fun))();  // call method
  }
  return true;
}

bool TypeDefInitRegistrar::CallAllDataInitFuns() {
  if(!instances) {
    taMisc::Error("TypeDefInitRegistrar: no instances found -- something badly wrong!");
    return false;
  }
  for(int i=0; i<instances->size; i++) {
    TypeDefInitRegistrar* it = instances->FastEl(i);
    (*(it->data_init_fun))();  // call method
  }
  return true;
}

bool TypeDefInitRegistrar::CallAllInstInitFuns() {
  if(!instances) {
    taMisc::Error("TypeDefInitRegistrar: no instances found -- something badly wrong!");
    return false;
  }
  for(int i=0; i<instances->size; i++) {
    TypeDefInitRegistrar* it = instances->FastEl(i);
    (*(it->inst_init_fun))();  // call method
  }
  return true;
}


//////////////////////////////////////////////////////////////////////
//    methods for actually initializing things from data structures


static TypeDef* tac_GetTypeFmName(TypeDef& tp, const char* nm) {
  static String_PArray already_warned;

  String snm = nm;
  TypeDef* typ = TypeDef::FindGlobalTypeName(snm, false);
  if(!typ) { // not found
    if(snm.endsWith("_ary")) {  // we dynamically construct arrays
      typ = new TypeDef(nm);
      typ->AddNewGlobalType(false);
      TypeDef* par = tac_GetTypeFmName(tp, typ->name.before("_ary"));
      typ->type = par->type;
      typ->SetType(TypeDef::ARRAY);
      typ->AddParent(par);
    }
    else {
#ifdef DEBUG      
      String anm = snm;
      if(anm.startsWith("const_")) anm = anm.after("const_");
      if(already_warned.FindEl(anm) < 0) {
        if(!(anm.startsWith("Q") || anm.startsWith("So") || anm.startsWith("Sb")
             || anm.startsWith("i") || anm.startsWith("I"))) {
          taMisc::Info("tac_GetTypeFmName(): unknown type named:", nm,
                       "not found -- anything involving this type will not be constructed");
        }
        already_warned.Add(anm);
      }
#endif
    }
  }
  return typ;
}

void tac_AddEnum(TypeDef& tp, const char* name, const char* desc,
		 const char* opts, const char* inh_opts, const char* lists,
		 const char* src_file, int src_st, int src_ed,
		 EnumDef_data* dt) {
  if(dt == NULL) return;
  TypeDef* enm = new TypeDef(name, desc, inh_opts, opts, lists,
			     src_file, src_st, src_ed, 
                             TypeDef::ENUM | TypeDef::SUBTYPE,
                             sizeof(int), (void**)0);
  while(dt->name != NULL) {
    enm->enum_vals.Add(dt->name, dt->desc, dt->opts, dt->val);
    dt++;
  }
#ifdef DEBUG
  if(tp.name != "taiMisc") {	// taimisc has intentional duplicates
    for(int i=1; i<enm->enum_vals.size; i++) {
      EnumDef* ed = enm->enum_vals.FastEl(i);
      for(int j=0; j<i; j++) {
	EnumDef* edo = enm->enum_vals.FastEl(j);
	if(ed->enum_no == edo->enum_no) {
	  taMisc::Warning("two enums have the same value", ed->name, edo->name,
			  "in type:", tp.name);
	}
      }
    }
  }
#endif

  tp.sub_types.Add(enm);
}

void tac_ThisEnum(TypeDef& tp, EnumDef_data* dt) {
  while(dt->name != NULL) {
    tp.enum_vals.Add(dt->name, dt->desc, dt->opts, dt->val);
    dt++;
  }
}


void tac_AddMembers(TypeDef& tp, MemberDef_data* dt) {
  while((dt != NULL) && (dt->type != NULL)) {
    TypeDef* typ = tac_GetTypeFmName(tp, dt->type);
    if(typ != NULL) {
      MemberDef* md;
      md = new MemberDef(typ, dt->name, dt->desc, dt->opts, dt->lists,
			 dt->off, dt->is_static, dt->addr, dt->fun_ptr);
      tp.members.AddUniqNameNew(md);
    }
    dt++;
  }
}

void tac_AddProperties(TypeDef& tp, PropertyDef_data* dt) {
  while((dt != NULL) && (dt->type != NULL)) {
    TypeDef* typ = tac_GetTypeFmName(tp, dt->type);
    if(typ != NULL) {
      PropertyDef* md;
      md = new PropertyDef(typ, dt->name, dt->desc, dt->opts, dt->lists,
                           dt->prop_get, dt->prop_set, dt->is_static);
      tp.properties.AddUniqNameNew(md);
    }
    dt++;
  }
}


void tac_AddMethods(TypeDef& tp, MethodDef_data* dt) {
  while((dt != NULL) && (dt->type != NULL)) {
    TypeDef* typ = tac_GetTypeFmName(tp, dt->type);
    if(typ != NULL) {
      bool some_invalid_type = false;
      MethodArgs_data* fa = dt->fun_args;
      while((fa != NULL) && (fa->type != NULL)) {
        TypeDef* fatyp = tac_GetTypeFmName(tp, fa->type);
        if(fatyp == NULL) {
          some_invalid_type = true;
          break;
        }
	fa++;
      }
      if(!some_invalid_type) {
        MethodDef* md;
        md = new MethodDef(typ, dt->name, dt->desc, dt->opts, dt->lists,
                           dt->fun_overld, dt->fun_argc, dt->fun_argd,
                           dt->is_static, dt->addr, dt->stubp, dt->is_virtual);

        MethodArgs_data* fa = dt->fun_args;
        while((fa != NULL) && (fa->type != NULL)) {
          TypeDef* fatyp = tac_GetTypeFmName(tp, fa->type);
          if(fatyp != NULL) {
            md->arg_types.Link(fatyp);
            md->arg_names.Add(fa->name);
            md->arg_defs.Add(fa->def);
            md->arg_vals.Add(fa->def); // initial val is default
          }
          fa++;
        }
        tp.methods.AddUniqNameNew(md);
      }
    }
    dt++;
  }
}
