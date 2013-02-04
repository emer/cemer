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


#ifndef ta_type_constr_h
#define ta_type_constr_h

#include "ta_def.h"
#include <taPtrList>
#include <PropertyDef>

// type information construction system -- for type information generated
// by maketa -- produced in maketa/mta_constr.cpp
// this code loads the information from the _TA.cpp file
// _TA.cpp must include this header

// this is the type of a function that initializes typedef's,
// as defined in the _TA.cpp file
typedef void (*TypeDefInitFun)();

///////////////////////////////////////////////////////////////////
//        Registrar for keeping track of all init funs

class TypeDefInitRegistrar; //

class TA_API TypeDefInitRegistrar_PtrList: public taPtrList<TypeDefInitRegistrar> {
INHERITED(taPtrList<TypeDefInitRegistrar>)
public:
  TypeDefInitRegistrar_PtrList() {}
};

class TA_API TypeDefInitRegistrar {
  // static class used to register typedef initialization functions
public:
  static TypeDefInitRegistrar_PtrList* instances;
  // this is the list of all the instances of this class, which get added to the list automatically in their constructors
  static int    instances_already_processed;
  // how many instances have already been processed (e.g., at startup) -- allows multiple iterative calls, e.g., when loading plugins, etc.
  static int    types_list_last_size;
  // corresponding last size in taMisc::types for the last round of instances processed

  static bool CallAllTypeInitFuns();
  // call all the type initialization functions that have been registered in instances
  static bool CallAllDataInitFuns();
  // call all the data initialization functions that have been registered in instances
  static bool CallAllInstInitFuns();
  // call all the inst initialization functions that have been registered in instances

  TypeDefInitFun        types_init_fun;
  // the initialization function for type defs for this instance
  TypeDefInitFun        data_init_fun;
  // the initialization function for member, property, method, enum data for this instance
  TypeDefInitFun        inst_init_fun;
  // the initialization function for instances of types 

  TypeDefInitRegistrar(TypeDefInitFun types_init_fun_, TypeDefInitFun data_init_fun_,
                       TypeDefInitFun inst_init_fun_);
  // constructor that adds this object to the list of instances, and sets the init_fun
};

///////////////////////////////////////////////////////////////////
//       Data structures for holding all the type init info

class TA_API MemberDef_data {
public:
  const char*  	type;
  const char*	name;
  const char*	desc;
  const char*	opts;
  const char*	lists;
  ta_memb_ptr	off;		// offset if not static
  bool		is_static;
  void*		addr;		// address static absolute addr
  bool		fun_ptr;
};

class TA_API PropertyDef_data {
public:
  const char* 	type;
  const char*	name;
  const char*	desc;
  const char*	opts;
  const char*	lists;
  bool		is_static;
  ta_prop_get_fun prop_get; // stub function to get the property (as Variant)
  ta_prop_set_fun prop_set; // stub function to set the property (as Variant)
};

class TA_API MethodArgs_data {
public:
  const char* 	type;
  const char*	name;
  const char*	def;		// default value
};

class TA_API MethodDef_data {
public:
  const char*   type;
  const char*	name;
  const char*	desc;
  const char*	opts;
  const char*	lists;
  short		fun_overld;	// number of times overloaded
  short		fun_argc;	// nofun, or # of parameters to the function
  short		fun_argd;	// indx for start of the default args (-1 if none)
  bool		is_virtual;	// note: only true when 'virtual' keyword was present
  bool		is_static;
  ta_void_fun 	addr;		// address (static or reg_fun only)
  css_fun_stub_ptr stubp; 	// css function pointer
  MethodArgs_data* fun_args;	// args to the function
};

class TA_API EnumDef_data {
public:
  const char* 	name;
  const char*	desc;
  const char*	opts;
  int		val;
};

//////////////////////////////////////////////////////////////////////
//    methods for actually initializing things from data structures

extern TA_API void tac_AddEnum(TypeDef& tp, const char* name, const char* desc,
			       const char* inh_opts, const char* opts, const char* lists,
			       const char* src_file, int src_st, int src_ed,
			       EnumDef_data* dt);
extern TA_API void tac_ThisEnum(TypeDef& tp, EnumDef_data* dt);
extern TA_API void tac_AddMembers(TypeDef& tp, MemberDef_data* dt);
extern TA_API void tac_AddProperties(TypeDef& tp, PropertyDef_data* dt);
extern TA_API void tac_AddMethods(TypeDef& tp, MethodDef_data* dt);
extern TA_API void tac_AddRegFun(TypeDef& tp);

#endif // ta_type_constr_h

