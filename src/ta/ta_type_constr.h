/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

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

