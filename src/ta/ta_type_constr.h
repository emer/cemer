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


#ifndef ta_type_constr_h
#define ta_type_constr_h

#include "ta_type.h"

// new type information construction system
// this includes the code to load the information from the _TA.cc file
// _TA.cc must include this header, but libtypea does not need to include
// the code generation code (mta_constr.cc)

// the typedefs come first, and are initialized just as before,
// same with the stubs

// the member data is now in static data structures, that are then
// read in by the _init function.

class TA_API MemberDef_data {
public:
  TypeDef* 	type;		// either the type is known
  char*		type_nm;	// or its a type::subtype, given by this name
  char*		name;
  char*		desc;
  char*		opts;
  char*		lists;
  ta_memb_ptr	off;		// offset if not static
  bool		is_static;
  void*		addr;		// address static absolute addr
  bool		fun_ptr;
};

class TA_API MethodArgs_data {
public:
  TypeDef*	type;		// either the type is known
  char*		type_nm;	// or its a type::subtype, given by this name
  char*		name;
  char*		def;		// default value
};

class TA_API MethodDef_data {
public:
  TypeDef* 	type;		// either the type is known
  char*		type_nm;	// or its a type::subtype, given by this name
  char*		name;
  char*		desc;
  char*		opts;
  char*		lists;
  int		fun_overld;	// number of times overloaded
  int		fun_argc;	// nofun, or # of parameters to the function
  int		fun_argd;	// indx for start of the default args (-1 if none)
  bool		is_static;
  ta_void_fun 	addr;		// address (static or reg_fun only)
  css_fun_stub_ptr stubp; 	// css function pointer
  MethodArgs_data* fun_args;	// args to the function
};

class TA_API EnumDef_data {
public:
  char* 	name;
  char*		desc;
  char*		opts;
  int		val;
};

extern TA_API void tac_AddEnum(TypeDef& tp, char* name, char* desc, char* inh_opts,
			char* opts, char* lists, EnumDef_data* dt);
extern TA_API void tac_ThisEnum(TypeDef& tp, EnumDef_data* dt);
extern TA_API void tac_AddMembers(TypeDef& tp, MemberDef_data* dt);
extern TA_API void tac_AddMethods(TypeDef& tp, MethodDef_data* dt);

#endif // ta_type_constr_h

