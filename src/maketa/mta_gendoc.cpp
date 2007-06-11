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


// mta_gendoc.cc

#include "mta_constr.h"

#include "ta/ta_platform.h"
#include "ta/ta_type.h"

// TODO: Put everything/methods/members/etc... into wiki categories based on CAT_
// The logical ordering is: enum, subtypes, members, methods
// For member options, it goes guys with nothing, then expert, then hidden
//  * Hidden also includes readonly

//////////////////////////////////
// 	     GenDoc		//
//////////////////////////////////

void MTA::GenDoc(TypeSpace* ths, fstream& strm) {

  //////////////////////
  //     TypeSpace    //
  //////////////////////

  TypeSpace* ts = ths;

  strm << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
  strm << " <TypeSpace name=\"" << ths->name << "\">\n";


  TypeDef* ta_base_def = ts->FindName("taBase");
  TypeDef* ta_smartref_def = ts->FindName("taSmartRef");
  TypeDef* ta_smartptr_def = ts->FindName("taSmartPtr");

  for(int i=0; i < ts->size; i++) {
  
    //////////////////////
    //     TypeDef      //
    //////////////////////

    TypeDef* td = ts->FastEl(i);

    ///////////////////////////////////////////////////
    // 	Filters!

    // We only want "actual" types, not pointers or references to types, etc...
    if (td->ptr || td->ref || td->formal || td->pre_parsed) 
      continue;

    if(td->InheritsFrom(&TA_const))
      continue;

    // exclude template instances (of any sort!)
    if (td->InheritsFormal(TA_templ_inst)) //  && (td->children.size == 1))
     continue;

    if(td->HasOption("IGNORE")) continue;

    // exclude low-level non-instance guys, except for the ones we want..
    if((td->HasOption("NO_INSTANCE") || td->HasOption("NO_CSS"))
       && !(td->HasOption("VIRT_BASE") || td->HasOption("SMART_POINTER")
	    || td->name == "taMisc"))
      continue;

    if(spc_builtin.FindName(td->name)) continue;
    // no builtin guys

    if((td != ta_smartref_def && td->InheritsFrom(ta_smartref_def))
       || (td != ta_smartptr_def && td->InheritsFrom(ta_smartptr_def))) continue;

    TypeDef* main_parent = NULL;
    if(td->children.size >= 1)
      main_parent = td->children.FastEl(0);

    if(main_parent && main_parent->HasOption("IGNORE")) continue;

    ///////////////////////////////////////////////////
    // now generate type info

    strm << "  <TypeDef name=\"" << td->name;
    strm << (td->is_enum() ? "\" type=\"enum" : "");
    strm << (td->is_class() ? "\" type=\"class" : "");
    strm << "\">\n";

    if(td->desc.length() > 2)
      strm << "   <desc>" + td->desc + "</desc>\n";

    // Options of this TypeDef
    if (td->opts.size) {
      for (int k=0; k < td->opts.size; k++ ) {
	strm << "   <options>" << td->opts.FastEl(k) << "</options>\n";
      }
    }
      
    // Parents of this TypeDef
    TypeSpace* tsp = &td->parents;
    for (int j=0; j < tsp->size; j++) {
      TypeDef* tdp = tsp->FastEl(j);
      strm << "   <parent>" << tdp->name << "</parent>\n";
    }

    // Children of this TypeDef
    TypeSpace* tsc = &td->children;
    for (int j=0; j < tsc->size; j++) {
      TypeDef* tdc = tsc->FastEl(j);
      strm << "   <children>" << tdc->name << "</children>\n";
    }

    //////////////////////
    //    Sub-types     //
    //////////////////////

    TypeSpace* subs = &td->sub_types;
    if(subs->size) {
      bool first_one = true;
      for(int q=0;q<subs->size;q++) {
	TypeDef* st = subs->FastEl(q);
	if(st->GetOwnerType() != td) continue;
	if(st->is_enum()) {
	  if(first_one) {
	    strm << "   <SubTypes>\n";
	    first_one = false;
	  }
	  strm << "     <EnumType name=\"" << st->name << "\">\n";
	  EnumSpace* es = &st->enum_vals;
	  if(es->size) {
	    for (int j=0; j < es->size; j++) {
	      EnumDef* ed = es->FastEl(j);
	      strm << "      <EnumDef name=\"" << ed->name << "\" " << "value=\"" << ed->enum_no
		   << "\"";
	      if(ed->desc.length() > 2)
		strm << " desc=\"" << ed->desc << "\"";
	      strm << "/>\n";
	    }
	  }
	  strm << "     </EnumType>\n";
	}
      }
      if(!first_one)
	strm << "   </SubTypes>\n";
    }

    //////////////////////
    //    MemberSpace   //
    //////////////////////

    MemberSpace* mems = &td->members;
    if (mems->size) {
      //////////////////////
      //    MemberDef     //
      //////////////////////

      bool first_one = true;
      for (int j=0; j < mems->size; j++) {
	MemberDef* md = mems->FastEl(j);

	// Shortened src/ta.xml by !5k lines
	if(!MemberSpace_Filter_Member(mems, md))
	  continue;

	if(md->HasOption("NO_SHOW") || md->HasOption("NO_SHOW_EDIT") ||
	   md->HasOption("HIDDEN") || md->HasOption("HIDDEN_TREE"))
	  continue;

	if(first_one) {
	  strm << "   <MemberSpace name=\"" << mems->name << "\">\n";
	  first_one = false;
	}

	strm << "     <MemberDef name=\"" << md->name << "\">\n";

	if(md->desc.length() > 2)
	  strm << "      <desc>" + md->desc + "</desc>\n";

	strm << "      <type>" + md->type->Get_C_Name() + "</type>\n";

	if (md->opts.size) {
	  for (int k=0; k < md->opts.size; k++ ) {
	    strm << "      <options>" << md->opts.FastEl(k) << "</options>\n";
	  }
	}
	strm << "     </MemberDef>\n";
      }
      if(!first_one)
	strm << "   </MemberSpace>\n";
    }

    //////////////////////
    //    MethodSpace   //
    //////////////////////

    MethodSpace* mets = &td->methods;
    if (mets->size) {

      //////////////////////
      //    MethodDef     //
      //////////////////////

      bool first_one = true;
      for (int j=0; j < mets->size; j++) {
	MethodDef* metd = mets->FastEl(j);

	if (!MethodSpace_Filter_Method(mets, metd))
	  continue;

	if(metd->HasOption("EXPERT"))
	  continue;

	if(ta_base_def && td->name != "taBase" && ta_base_def->methods.FindName(metd->name))
	  continue;		// firmly exclude any of the base guys, which tend to be
	                        // overwritten quite frequently in subclasses

	if(main_parent && main_parent->methods.FindName(metd->name))
	  continue;		// sometimes methods get re-owned, esp in multiple inheritance
	                        // and templates -- for docs, we can always exclude

	if(metd->name.contains("__")) continue; // internal thing

	if(first_one) {
	  strm << "   <MethodSpace>\n";
	  first_one = false;
	}

	int metd_desc_len = metd->desc.length();
	int metd_opts_size = metd->opts.size;
	int metd_args_size = metd->arg_types.size;
	bool metd_cont = 1;
	strm << "     <MethodDef name=\"" << metd->name << "\"";

	// Make this a self-closing tag if it has no xml children
	if (metd_desc_len < 2 && !metd_opts_size) {
	  strm << " />\n";
	  metd_cont = 0;
	}
	  
	if (metd_cont) {
	  strm << ">\n";

	  // Get the prototype
	    strm << "      <prototype>" + metd->prototype() + "</prototype>\n";
	  // Get the description
	  if (metd_desc_len > 2) {
	    strm << "      <desc>" + metd->desc + "</desc>\n";
	  }
	  // Get the options
	  if (metd_opts_size) {
	    for (int k=0; k < metd->opts.size; k++ ) {
	      strm << "      <option>" << metd->opts.FastEl(k) << "</option>\n";
	    }
	  }
	  strm << "     </MethodDef>\n";
	}
      }
      if(!first_one)
	strm << "   </MethodSpace>\n";
    }

    //////////////////////
    //    EnumSpace     //
    //////////////////////

    // Note: These are self-closing tags
    EnumSpace* es = &td->enum_vals;
    if(es->size) {
      strm << "   <EnumSpace>\n";

      //////////////////////
      //    EnumDef       //
      //////////////////////

      for (int j=0; j < es->size; j++) {
	EnumDef* ed = es->FastEl(j);
	strm << "     <EnumDef name=\"" << ed->name << "\" " << "value=\"" << ed->enum_no
	     << "\"";
	if(ed->desc.length() > 2)
	  strm << " desc=\"" << ed->desc << "\"";
	strm << "/>\n";
      }
      strm << "   </EnumSpace>\n";
    }
    strm << "  </TypeDef>\n";
  }
  strm << " </TypeSpace>\n";
  strm << "</xml>\n";
}
