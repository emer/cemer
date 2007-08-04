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

// The logical ordering is: enum, subtypes, members, methods
// For member options, it goes guys with nothing, then expert, then hidden
//  * Hidden also includes readonly

//////////////////////////////////
// 	     GenDoc		//
//////////////////////////////////

String_PArray* MTA::TypeDef_Get_Parents(TypeDef* td, String_PArray* bp) {
  TypeSpace* pp = &td->parents; // Potential parents
  for(int i=0;i<pp->size;i++) {
    TypeDef* this_par = pp->FastEl(i);
    if (!TypeDef_Filter_Type(this_par, pp)) {
	bp->Add(this_par->name);
	bp = TypeDef_Get_Parents(this_par, bp);
      }
  }
  return bp;
}

bool MTA::TypeDef_Filter_Type(TypeDef* td, TypeSpace* ts) {
  /////////////////////////////////////////////////////////////
  // 	Filters! Returns true if you should filter this TypeDef

  TypeDef* ta_base_def = ts->FindName("taBase");
  TypeDef* ta_smartref_def = ts->FindName("taSmartRef");
  TypeDef* ta_smartptr_def = ts->FindName("taSmartPtr");

  // We only want "actual" types, not pointers or references to types, etc...
  if (td->ptr || td->ref || td->formal || td->pre_parsed) 
    return true;

  if(td->InheritsFrom(&TA_const))
    return true;

  // exclude template instances (of any sort!)  //  && (td->children.size == 1)) 
  if (td->InheritsFormal(TA_templ_inst))
    return true;

  if(td->HasOption("IGNORE"))
    return true;

  // exclude low-level non-instance guys, except for the ones we want..
  if((td->HasOption("NO_INSTANCE") || td->HasOption("NO_CSS"))
     && !(td->HasOption("VIRT_BASE") || td->HasOption("SMART_POINTER")
	  || td->name == "taMisc"))
    return true;

  if(spc_builtin.FindName(td->name)) 
    return true;

  // no builtin guys
  if((td != ta_smartref_def && td->InheritsFrom(ta_smartref_def))
     || (td != ta_smartptr_def && td->InheritsFrom(ta_smartptr_def)))
    return true;

    TypeDef* main_parent = NULL;
    if(td->children.size >= 1)
      main_parent = td->children.FastEl(0);

    if(main_parent && main_parent->HasOption("IGNORE"))
      return true;
}

void MTA::GenDoc(TypeSpace* ths, fstream& strm) {

  //////////////////////
  //     TypeSpace    //
  //////////////////////

  TypeSpace* ts = ths;

  strm << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
  strm << "<TypeSpace>\n";
  strm << " <Name>" << ts->name.xml_esc() << "</Name>\n";

  TypeDef* ta_base_def = ts->FindName("taBase");
  TypeDef* ta_smartref_def = ts->FindName("taSmartRef");
  TypeDef* ta_smartptr_def = ts->FindName("taSmartPtr");

  for(int i=0;i<ts->size;i++) {
  
    //////////////////////
    //     TypeDef      //
    //////////////////////

    TypeDef* td = ts->FastEl(i);

    if (TypeDef_Filter_Type(td, ts)) continue;

    TypeDef* main_parent = NULL;
    if(td->children.size >= 1)
      main_parent = td->children.FastEl(0);

    if(td == ta_base_def) main_parent = NULL; // somehow parent of taBase is taBase!

    ///////////////////////////////////////////////////
    // now generate type info

    strm << " <TypeDef>\n";
    strm << "  <Name>" << td->name.xml_esc() << "</Name>\n";
    strm << (td->is_enum() ? "  <Type>enum</Type>\n":"");
    strm << (td->is_class() ? "  <Type>class</Type>\n":"");
    strm << (trim(td->desc).length() ? "  <Desc>"+trim(td->desc).xml_esc()+"</Desc>\n":"");

    if (td->opts.size) {  // Options of this TypeDef
      strm << "  <Options>\n";
      for (int k=0;k<td->opts.size;k++)
	strm << "   <Option>" << trim(td->opts.FastEl(k)).xml_esc() << "   </Option>\n";
      strm << "  </Options>\n";
    }
      
    String_PArray* bp = new String_PArray; // Biological parents
    bp = TypeDef_Get_Parents(td, bp);
    if(bp->size){
      strm << "  <Parents>\n";
      for (int l=0;l<bp->size;l++)
	strm << "   <Parent>" << trim(bp->FastEl(l)).xml_esc() << "</Parent>\n";
      strm << "  </Parents>\n";
    }

    TypeSpace* tsc = &td->children;
    if (tsc->size) {
      strm << "  <Children>\n";
      for (int j=0;j<tsc->size;j++) {
	TypeDef* tdc = tsc->FastEl(j);
	if (!TypeDef_Filter_Type(tdc, tsc))
	  strm << "   <Child>" << trim(tdc->name).xml_esc() << "</Child>\n";
      }
      strm << "  </Children>\n";
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
	    strm << "  <SubTypes>\n";
	    first_one = false;
	  }
	  strm << "   <EnumType>\n";
	  strm << "    <Name>" << st->name.xml_esc() << "</Name>\n";
	  EnumSpace* es = &st->enum_vals;
	  if(es->size) {
	    for (int j=0;j<es->size;j++) {
	      EnumDef* ed = es->FastEl(j);
	      strm << "    <EnumDef>\n";
	      strm << "     <Name>" << ed->name.xml_esc() << "</Name>\n";
	      strm << (trim(ed->desc).length() ? "     <Desc>"+trim(ed->desc).xml_esc()+"</Desc>\n":"");
	      strm << "     <Value>" << ed->enum_no << "</Value>\n";
	      strm << "    </EnumDef>\n";
	    }
	  }
	  strm << "   </EnumType>\n";
	}
      }
      if(!first_one)
	strm << "  </SubTypes>\n";
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
      for (int j=0;j<mems->size;j++) {
	MemberDef* md = mems->FastEl(j);

	if(!MemberSpace_Filter_Member(mems, md))
	  continue;

	if(md->HasOption("NO_SHOW") || md->HasOption("NO_SHOW_EDIT") ||
	   md->HasOption("HIDDEN") || md->HasOption("HIDDEN_TREE"))
	  continue;

	if(first_one) {
	  strm << "  <MemberSpace>\n";
	  strm << (mems->name != "members" ? "   <Name>"+mems->name.xml_esc()+"</Name>\n":"");
	  first_one = false;
	}

	strm << "    <MemberDef>\n";
	strm << "     <Name>" << md->name << "</Name>\n";
	strm << (trim(md->desc).length() ? "     <Desc>"+trim(md->desc).xml_esc()+"</Desc>\n":"");
	strm << "     <Type>"+md->type->Get_C_Name().xml_esc()+"</Type>\n";

	if (md->opts.size) {
	  strm << "     <Options>\n"; 
	  for (int k=0;k<md->opts.size;k++)
	    strm << "      <Option>" << trim(md->opts.FastEl(k)).xml_esc() << "   </Option>\n";
	  strm << "     </Options>\n"; 
	}

	strm << "    </MemberDef>\n";
      }
      if(!first_one)
	strm << "  </MemberSpace>\n";
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
      for (int j=0;j<mets->size;j++) {
	MethodDef* metd = mets->FastEl(j);

	if (!MethodSpace_Filter_Method(mets, metd)) {
	  continue;
	}

	if(metd->HasOption("EXPERT"))
	  continue;

	if(ta_base_def && td != ta_base_def && ta_base_def->methods.FindName(metd->name))
	  continue;		// firmly exclude any of the base guys, which tend to be
	                        // overwritten quite frequently in subclasses

	if(main_parent && main_parent->methods.FindName(metd->name)) {
	  continue;		// sometimes methods get re-owned, esp in multiple inheritance
	                        // and templates -- for docs, we can always exclude
	}

	if(metd->name.contains("__")) continue; // internal thing

	if(first_one) {
	  strm << "  <MethodSpace>\n";
	  first_one = false;
	}

	strm << "    <MethodDef>\n";
	strm << "     <Name>" << metd->name << "</Name>\n";
	strm << (trim(metd->desc).length() ? "     <Desc>"+trim(metd->desc).xml_esc()+"</Desc>\n":"");
	strm << "     <Prototype>"+String(metd->prototype()).xml_esc()+"</Prototype>\n";

	if (metd->opts.size) {
	  strm << "     <Options>\n"; 
	  for (int k=0;k<metd->opts.size;k++)
	    strm << "      <Option>" << trim(metd->opts.FastEl(k)).xml_esc() << "   </Option>\n";
	  strm << "     </Options>\n"; 
	}
	strm << "    </MethodDef>\n";
      }
      if(!first_one)
	strm << "  </MethodSpace>\n";
    }

    //////////////////////
    //    EnumSpace     //
    //////////////////////

    // Note: These are self-closing tags
    EnumSpace* es = &td->enum_vals;
    if(es->size) {
      strm << "  <EnumSpace>\n";

      //////////////////////
      //    EnumDef       //
      //////////////////////

      for (int j=0;j<es->size;j++) {
	EnumDef* ed = es->FastEl(j);
	strm << "   <EnumDef>\n";
	strm << "    <Name>" << ed->name.xml_esc() << "</Name>\n";
	strm << (trim(ed->desc).length() ? "    <Desc>"+trim(ed->desc).xml_esc()+"</Desc>\n":"");
	strm << "    <Value>" << ed->enum_no << "</Value>\n";
	strm << "   </EnumDef>\n";
      }
      strm << "  </EnumSpace>\n";
    }
    strm << " </TypeDef>\n";
  }
  strm << "</TypeSpace>\n";
}
