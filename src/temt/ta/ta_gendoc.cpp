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

// ta_gendoc.cpp

#include "ta_gendoc.h"

// The logical ordering is: enum, subtypes, members, methods
// For member options, it goes guys with nothing, then expert, then hidden
//  * Hidden also includes readonly

//////////////////////////////////
// 	     GenDoc		//
//////////////////////////////////

// bool taGenDoc::MemberSpace_Filter_Member(MemberSpace* ths, MemberDef* md) {
//   if((md->owner == ths) && !md->HasOption("IGNORE"))
//     return true;
//   return false;
// }

// bool taGenDoc::MethodSpace_Filter_Method(MethodSpace* ths, MethodDef* md) {
//   // always ignore these
//   if((md->name == "Copy_") || md->HasOption("IGNORE")
//      || ths->owner->IgnoreMeth(md->name))
//     return false;

//   if((md->owner != ths) && !ths->owner->HasOption("MULT_INHERIT"))
//     return false;		// don't reproduce owners functions (except multi inh)

//   // is_static is a problem in mult_inherit cases for compiler
//   if(ths->owner->HasOption("MULT_INHERIT")) {
//     if(!md->is_static)
//       return true;
//     return false;
//   }

//   return true;
// }

bool taGenDoc::TypeDef_Filter_Type(TypeDef* td, TypeSpace* ts) {
  /////////////////////////////////////////////////////////////
  // 	Filters! Returns true if you should filter this TypeDef

  if(!td->InheritsNonAtomicClass()) return true;

//   TypeDef* ta_base_def = ts->FindName("taBase");
  TypeDef* ta_smartref_def = ts->FindName("taSmartRef");
  TypeDef* ta_smartptr_def = ts->FindName("taSmartPtr");

  // We only want "actual" types, not pointers or references to types, etc...
  if (td->ptr || td->ref || td->formal)
    return true;

  if(!td->InheritsFormal(TA_class))
    return true;
  if(td->InheritsFrom(&TA_const))
    return true;

  // exclude template instances (of any sort!)  //  && (td->children.size == 1)) 
  if (td->InheritsFormal(TA_templ_inst))
    return true;

  // exclude low-level non-instance guys, except for the ones we want..
  if((td->HasOption("NO_INSTANCE") || td->HasOption("NO_CSS"))
     && !(td->HasOption("VIRT_BASE") || td->HasOption("SMART_POINTER")
	  || td->name == "taMisc"))
    return true;

  // no builtin guys
  if((td != ta_smartref_def && td->InheritsFrom(ta_smartref_def))
     || (td != ta_smartptr_def && td->InheritsFrom(ta_smartptr_def)))
    return true;

  return false;
}

void taGenDoc::GenDoc(TypeSpace* ts, int detail_level) {

  for(int i=0;i<ts->size;i++) {
    TypeDef* td = ts->FastEl(i);
    if (TypeDef_Filter_Type(td, ts)) continue;

//     TypeDef* main_parent = NULL;
//     if(td->parents.size >= 1)
//       main_parent = td->parents.FastEl(0);

//     if(td == ta_base_def) main_parent = NULL; // somehow parent of taBase is taBase!

    String fnm = td->name + ".html";
    String html = td->GetHTML(detail_level);
    fstream strm;
    strm.open(fnm, ios::out);
    strm << html << endl;
    strm.close();
  }
}
