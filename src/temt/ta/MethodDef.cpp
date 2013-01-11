// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "MethodDef.h"
#include <MethodSpace>
#include <TypeDef>
#include <taMisc>

#ifndef NO_TA_BASE
#include <taiMethod>
#include <taiMethodData>
#include <css_machine.h>
#endif

void MethodDef::Initialize() {
  owner = NULL;
  type = NULL;
  is_static = false;
  is_virtual = false;
  is_override = false;
  is_lexhide = false;
  addr = NULL;
#ifdef TA_GUI
  im = NULL;
#endif
  fun_overld = 0;
  fun_argc = 0;
  fun_argd = -1;
  stubp = NULL;

  arg_types.name = "arg_types";
  arg_types.owner = (TypeDef*)this; // this isn't quite right, is it..
  show_any = 0;
}

MethodDef::MethodDef()
:inherited()
{
  Initialize();
}

MethodDef::MethodDef(const char* nm)
:inherited()
{
  Initialize();
  name = nm;
}

MethodDef::MethodDef(TypeDef* ty, const char* nm, const char* dsc, const char* op, const char* lis,
                     int fover, int farc, int fard, bool is_stat, ta_void_fun funa,
                     css_fun_stub_ptr stb, bool is_virt)
:inherited()
{
  Initialize();
  type = ty; name = nm; desc = dsc;
  taMisc::CharToStrArray(opts,op);
  taMisc::CharToStrArray(lists,lis);
  fun_overld = fover; fun_argc = farc; fun_argd = fard;
  is_static = is_stat; addr = funa; stubp = stb;
  is_virtual = is_virt; // note: gets further processed, will get set if this is an override
  // without explicit virtual keyword
}

MethodDef::MethodDef(const MethodDef& cp)
:inherited(cp)
{
  Initialize();
  Copy(cp);
}

MethodDef::~MethodDef() {
#ifndef NO_TA_BASE
# ifndef NO_TA_GUI
  taRefN::SafeUnRefDone(im);
  im = NULL;
# endif
#endif
}

void MethodDef::Copy(const MethodDef& cp) {
  inherited::Copy(cp);
  type = cp.type;
  is_static = cp.is_static;
  is_virtual = cp.is_virtual;
  is_override = cp.is_override;
  is_lexhide = cp.is_lexhide;
  addr = cp.addr;
  inh_opts = cp.inh_opts;
  // don't delete because delete is not ref counted
//  im = cp.im;
  fun_overld = cp.fun_overld;
  fun_argc = cp.fun_argc;
  fun_argd = cp.fun_argd;
  arg_types = cp.arg_types;
  arg_names = cp.arg_names;
  arg_defs = cp.arg_defs;
  arg_vals = cp.arg_vals;
  stubp = cp.stubp;
  show_any = 0; //rebuild
}

bool MethodDef::CheckList(const String_PArray& lst) const {
  int i;
  for(i=0; i<lists.size; i++) {
    if(lst.FindEl(lists.FastEl(i)) >= 0)
      return true;
  }
  return false;
}

TypeDef* MethodDef::GetOwnerType() const {
  TypeDef* rval=NULL;
  if((owner) && (owner->owner))
    rval=owner->owner;
  return rval;
}

bool MethodDef::CompareArgs(MethodDef* it) const {
  if(fun_argc != it->fun_argc)
    return false;
  int i;
  for(i=0; i<fun_argc; i++) {
    if(arg_types[i] != it->arg_types[i])
      return false;
  }
  return true;
}

void MethodDef::CallFun(void* base) const {
#if !defined(NO_TA_BASE) && defined(TA_GUI)
  taiMethodData* mth_rep = NULL;
  if (taMisc::gui_active && (im != NULL)) {
    //TODO: following may not work, because it doesn't have enough context to pass to the routine
    mth_rep = im->GetGenericMethodRep(base, NULL);
  }
  if(mth_rep != NULL) {
    ++taMisc::in_gui_call;
    mth_rep->CallFun();
    --taMisc::in_gui_call;
    delete mth_rep;
  }
  else {
    if((fun_argc == 0) || (fun_argd == 0)) {
      ++taMisc::in_gui_call;
      cssEl* rval = (*(stubp))(base, 0, (cssEl**)NULL);
      if(rval) {
        cssEl::Ref(rval);
        cssEl::unRefDone(rval);
      }
      --taMisc::in_gui_call;
    }
    else {
      taMisc::Warning("*** CallFun Error: function:", name,
                    "not available, because args are required and no dialog requestor can be opened",
                    "(must be gui, and function must have #MENU or #BUTTON");
      return;
    }
  }
#endif
}

const String MethodDef::GetPathName() const {
  String rval;
  TypeDef* owtp = GetOwnerType();
  if (owtp)
    rval = owtp->GetPathName();
  rval += "::" + name;
  return rval;
}

const String MethodDef::ParamsAsString() const {
  if (arg_types.size == 0) return _nilString;
  STRING_BUF(rval, arg_types.size * 20);
  String arg_def;
  for (int i = 0; i < arg_types.size; ++i) {
    if (i > 0) rval += ", ";
    TypeDef* arg_typ = arg_types.FastEl(i);
    rval += arg_typ->Get_C_Name() + " ";
    rval += arg_names.FastEl(i);
    arg_def = arg_defs.FastEl(i); //note: same string used in original definition
    if (arg_def.length() > 0) {
      rval += " = " + arg_def;
    }
  }
  return rval;
}

const String MethodDef::prototype() const {
  STRING_BUF(rval, 80); // expands if necessary
  rval.cat(type->name).cat(' ').cat(name).cat('(');
  for (int i = 0; i < arg_names.size; ++i) {
    if (i > 0) rval.cat(", ");
    rval.cat(arg_types[i]->Get_C_Name()).cat(' ');
    rval.cat(arg_names[i]);
    String def = arg_defs[i];
    if (def.nonempty())
      rval.cat(" = ").cat(def);
  }
  rval.cat(')');
  return rval;
}

bool MethodDef::ShowMethod(int show) const {
  if (show & taMisc::USE_SHOW_GUI_DEF)
    show = taMisc::show_gui;

  // check if cache has been done yet
  if (show_any == 0) ShowMethod_CalcCache();
  byte show_eff = show_any;

  // our show_eff is the positives (what it is) so if there is nothing there, then
  // we clearly can't show
  // if there is something (a positive) then bit-AND with the
  // show, which is negatives (what not to show), and if anything remains, don't show!
  show_eff &= (byte)taMisc::SHOW_CHECK_MASK;
  return (show_eff) && !(show_eff & (byte)show);
}

void MethodDef::ShowMethod_CalcCache() const {
  // note that "normal" is a special case, which depends both on context and
  // on whether other bits are set, so we calc those individually
  show_any = taMisc::IS_NORMAL; // the default for any
  ShowMethod_CalcCache_impl(show_any);

}

void MethodDef::ShowMethod_CalcCache_impl(byte& show) const {
  show |= 0x80; // set the "done" flag

  //note: keep in mind that these show bits are the opposite of the show flags,
  // i.e show flags are all negative, whereas these are all positive (bit = is that type)

  //note: member flags should generally trump type flags, so you can SHOW a NO_SHOW type
  //note: NO_SHOW is special, since it negates, so we check for base NO_SHOW everywhere
  //MethodDef note: SHOW is implied in any of: MENU BUTTON or MENU_CONTEXT
  bool typ_show = type->HasOption("METH_SHOW");
  bool typ_no_show = type->HasOption("METH_NO_SHOW") || type->HasOption("METH_NO_SHOW");
  bool mth_show = HasOption("SHOW") || HasOption("MENU") ||
    HasOption("BUTTON") || HasOption("MENU_CONTEXT") || HasOption("MENU_BUTTON");
  bool mth_no_show = HasOption("NO_SHOW") || HasOption("NO_SHOW");

  // ok, so no explicit SHOW or NO_SHOW, so we do the special checks
  // you can't "undo" type-level specials, but you can always mark SHOW on the mth

  // the following are all cumulative, not mutually exclusive
  if (HasOption("HIDDEN") || type->HasOption("METH_HIDDEN"))
    show |= (byte)taMisc::IS_HIDDEN;
  if ((HasOption("READ_ONLY") || HasOption("GUI_READ_ONLY")) && !mth_show)
    show |= (byte)taMisc::IS_HIDDEN;
  if (HasOption("EXPERT") || type->HasOption("METH_EXPERT"))
    show |= (byte)taMisc::IS_EXPERT;

//   String cat = OptionAfter("CAT_");
//   if(cat.contains("Xpert"))  // special code for expert -- no need for redundant #EXPERT flag
//     show |= (byte)taMisc::IS_EXPERT;

  // if NO_SHOW and no SHOW or explicit other, then never shows
  if (mth_no_show || (typ_no_show && (!mth_show || (show & (byte)taMisc::NORM_MEMBS)))) {
    show &= (byte)(0x80 | ~taMisc::SHOW_CHECK_MASK);
    return;
  }

  // if any of the special guys are set, we unset NORMAL (which may
  //   or may not have been already set by default)
  if (show & (byte)taMisc::NORM_MEMBS) // in "any" context, default is "normal"
    show &= ~(byte)taMisc::IS_NORMAL;
  else // no non-NORMAL set
    // SHOW is like an explicit NORMAL if nothing else applies
    if (mth_show || typ_show)
      show |= (byte)taMisc::IS_NORMAL;
}

String MethodDef::GetHTML(bool gendoc, bool short_fmt) const {
  STRING_BUF(rval, (short_fmt ? 100 : 300)); // extends if needed
  String own_typ;
  TypeDef* ot = GetOwnerType();
  if(ot) {
    own_typ.cat(ot->name).cat("::");
  }
  if(short_fmt) {
    rval.cat(type->Get_C_Name()).cat(" <b><a href=#").cat(name).cat(">").cat(own_typ).cat(name).cat("</a></b> ( ");
    for(int i = 0; i < arg_names.size; ++i) {
      if (i > 0) rval.cat(", ");
      rval.cat(arg_types[i]->Get_C_Name()).cat(' ');
      rval.cat("<i>").cat(arg_names[i]).cat("</i>");
      String def = arg_defs[i];
      if (def.nonempty())
        rval.cat(" = ").cat(trim(def).xml_esc());
    }
    rval.cat(" )");
    if(is_static) rval.cat("&nbsp;&nbsp;<tt> [static]</tt>");
  }
  else {
    rval.cat("<h3 class=\"fn\"><a name=\"").cat(name).cat("\"></a>");
    rval.cat(type->GetHTMLLink(gendoc)).cat(" ").cat(own_typ).cat(name).cat(" ( ");
    for(int i = 0; i < arg_names.size; ++i) {
      if (i > 0) rval.cat(", ");
      rval.cat(arg_types[i]->GetHTMLLink(gendoc)).cat(' ');
      rval.cat("<i>").cat(arg_names[i]).cat("</i>");
      String def = arg_defs[i];
      if (def.nonempty())
        rval.cat(" = ").cat(trim(def).xml_esc());
    }
    rval.cat(" )");
    if(is_static) rval.cat("&nbsp;&nbsp;<tt> [static]</tt>");
    rval.cat("</h3>\n");
    rval += "<p>" + trim(desc).xml_esc() + "</p>\n";
    if(taMisc::help_detail >= taMisc::HD_DETAILS) {
      if(opts.size > 0) {
        rval.cat("<p>").cat(GetOptsHTML()).cat("</p>\n");
      }
    }
    rval.cat("<span class=\"fakelink\" onclick='writesrc(this,\"");
    rval.cat(type->Get_C_Name()).cat(" ").cat(own_typ).cat(name).cat("\")'>Show Source Code</span><br>\n");
  }
  return rval;
}

void MethodDef::PrintType(String& col1, String& col2) const {
  col1 = "";
  if(is_static)
    col1 << "static ";
  col1 << type->Get_C_Name();
  col2 = "";
  col2 << name << "(";
  if(fun_argc > 0) {
    for(int i=0; i<fun_argc; i++) {
      col2 << arg_types[i]->Get_C_Name() << " " << arg_names[i];
      if((fun_argd >= 0) && (i >= fun_argd))    // indicate a default
        col2 << "=" << arg_defs[i];
      if(i+1 < fun_argc)
        col2 << ", ";
    }
  }
  col2 << ");";
  if(!desc.empty())
    col2 << "  // " + desc;
  PrintType_OptsLists(col2);
}

