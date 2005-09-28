// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
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
			 dt->is_static, dt->addr, dt->stubp);
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
      // use a very simple adduniqnamenew here: all the hard work is already done
      int idx;
      if(tp.methods.FindName(md->name, idx) != NULL)
	tp.methods.Replace(idx, md);
      else
	tp.methods.Add(md);
    }
    dt++;
  }
}
