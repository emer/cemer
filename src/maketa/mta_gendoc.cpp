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

  for(int i=0; i < ts->size; i++) {
  
    //////////////////////
    //     TypeDef      //
    //////////////////////

    TypeDef* td = ts->FastEl(i);

    // We only want "actual" types, not pointers or references to types, etc...
    if (td->ptr || td->ref || td->formal || td->pre_parsed) 
      continue;

    // exclude template instances that have a single further subclass
    if (td->InheritsFormal(TA_templ_inst) && (td->children.size == 1))
     continue;
    
    strm << "  <TypeDef name=\"" << td->name;
    strm << (td->is_enum() ? "\" type=\"enum" : "");
    strm << (td->is_class() ? "\" type=\"class" : "");
    strm << "\">\n";

    // Options of this TypeDef
    //strm << "   <option>" << td->opts.AsString() << "</option>\n";
      
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
    //    MemberSpace   //
    //////////////////////

    MemberSpace* mems = &td->members;

    if (mems->size) {
      strm << "   <MemberSpace name=\"" << mems->name << "\">\n";

      //////////////////////
      //    MemberDef     //
      //////////////////////

      for (int j=0; j < mems->size; j++) {
	MemberDef* md = mems->FastEl(j);

	// Shortened src/ta.xml by !5k lines
	if (MemberSpace_Filter_Member(mems, md))
	  continue;

	strm << "     <MemberDef name=\"" << md->name << ">\n";

	strm << (md->desc.length() > 2 ? "      <desc>" + md->desc + "</desc>\n" : "");
	if (md->opts.size) {
	  for (int k=0; k < md->opts.size; k++ ) {
	    strm << "      <options>" << md->opts.FastEl(k) << "</options>\n";
	  }
	}
	strm << "     </MemberDef>\n";
      }
      strm << "   </MemberSpace>\n";
    }

    //////////////////////
    //    MethodSpace   //
    //////////////////////

    MethodSpace* mets = &td->methods;
    if (mets->size) {
      strm << "   <MethodSpace\">\n";

      //////////////////////
      //    MethodDef     //
      //////////////////////

      for (int j=0; j < mets->size; j++) {
	MethodDef* metd = mets->FastEl(j);

	// Shortened src/ta.xml by ~20k lines
	if (MethodSpace_Filter_Method(mets, metd))
	  continue;

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
	strm << "     <EnumDef name=\"" << ed->name << "\" " << "value=\"" << ed->enum_no << "\"/>\n";
      }
      strm << "   </EnumSpace>\n";
    }
    strm << "  </TypeDef>\n";
  }
  strm << " </TypeSpace>\n";
  strm << "</xml>\n";
}
