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

#include "MemberDef.h"
#include <TypeDef>
#include <EnumDef>
#include <taMisc>
#include <voidptr_PArray>

using namespace std;

void MemberDef::GetMembDesc(MemberDef* md, String& dsc_str, String indent) {
  String desc = md->desc;
  String defval = md->OptionAfter("DEF_");
  if(!defval.empty())
    desc = String("[Default: ") + defval + "] " + desc;
  else
    desc = desc;
  if(!indent.empty())
    desc = indent + md->GetLabel() + String(": ") + desc;
  if (!dsc_str.empty())
    dsc_str += "<br>";
  dsc_str += desc;
  if(md->type->InheritsFormal(TA_class) &&
     (md->type->HasOption("INLINE") || md->type->HasOption("EDIT_INLINE"))) {
    indent += "  ";
    for (int i=0; i < md->type->members.size; ++i) {
      MemberDef* smd = md->type->members.FastEl(i);
      if (!smd->ShowMember(taMisc::show_gui, TypeItem::SC_EDIT, TypeItem::SHOW_CHECK_MASK) ||
          smd->HasOption("HIDDEN_INLINE"))
        continue;
      GetMembDesc(smd, dsc_str, indent);
    }
  }
  else if (md->type->InheritsFormal(TA_enum)) {
    for (int i = 0; i < md->type->enum_vals.size; ++i) {
      EnumDef* ed = md->type->enum_vals.FastEl(i);
      if (ed->desc.empty() || (ed->desc == " ") || (ed->desc == "  ")) continue;
      if (ed->HasOption("NO_BIT") || ed->HasOption("NO_SHOW") || ed->HasOption("HIDDEN")) continue;
      desc = indent + " * " + ed->GetLabel() + String(": ") + ed->desc;
      if (!dsc_str.empty())
        dsc_str += "<br>";
      dsc_str += desc;
    }
  }
}

void MemberDef::Initialize() {
  off = NULL;
  base_off = 0;
  addr = NULL;
  fun_ptr = false;
}

MemberDef::MemberDef()
:inherited()
{
  Initialize();
}

MemberDef::MemberDef(const char* nm)
:inherited(nm)
{
  Initialize();
}

MemberDef::MemberDef(TypeDef* ty, const char* nm, const char* dsc,
  const char* op, const char* lis, ta_memb_ptr mptr, bool is_stat,
  void* maddr, bool funp)
:inherited(ty, nm, dsc, op, lis, is_stat)
{
  Initialize();
  off = mptr; addr = maddr;  fun_ptr = funp;
}

MemberDef::MemberDef(const MemberDef& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

void MemberDef::Copy(const MemberDef& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void MemberDef::Copy_(const MemberDef& cp) {
  off = cp.off;
  base_off = cp.base_off;
  addr = cp.addr;
  fun_ptr = cp.fun_ptr;
}

MemberDef::~MemberDef() {
}

void* MemberDef::GetOff(const void* base) const {
  void* rval = addr;
  if (!is_static)
    rval = (void*)&((ta_memb_ptr_class*)((char*)base+base_off)->*off);
  return rval;
}

void* MemberDef::GetOff_static(const void* base, int base_off_, ta_memb_ptr off_) {
  return (void*)&((ta_memb_ptr_class*)((char*)base+base_off_)->*off_);
}

const String MemberDef::GetPathName() const {
  String rval;
  TypeDef* owtp = GetOwnerType();
  if (owtp)
    rval = owtp->GetPathName();
  rval += "::" + name;
  return rval;
}

const Variant MemberDef::GetValVar(const void* base) const {
  return type->GetValVar(GetOff(base), this);
}

MemberDef::DefaultStatus MemberDef::GetDefaultStatus(const void* base) {
  String defval = OptionAfter("DEF_");
  if (defval.empty()) return NO_DEF;
  String cur_val = type->GetValStr(GetOff(base));
  // hack for , in real numbers in international settings
  // note: we would probably never have a variant member with float default
  if (type->DerivesFrom(TA_float) || type->DerivesFrom(TA_double)) {
    cur_val.gsub(",", "."); // does a makeUnique
  }
  if(defval.contains(';')) { // enumerated set of values specified
    String dv1 = defval.before(';');
    String dvr = defval.after(';');
    while(true) {
      if(cur_val == dv1) return EQU_DEF;
      if(dvr.contains(';')) {
        dv1 = dvr.before(';');
        dvr = dvr.after(';');
        continue;
      }
      return (cur_val == dvr) ? EQU_DEF : NOT_DEF;
    }
  }
  else if(defval.contains(':')) { // range of values specified -- must be numeric
    String dv1 = defval.before(':');
    String dvr = defval.after(':');
    double cv = (double)cur_val; // just use double because it should do the trick for anything
    double dvl = (double)dv1;
    double dvh = (double)dvr;
    return (cv >= dvl && cv <= dvh) ? EQU_DEF : NOT_DEF;
  }
  else {
    return (cur_val == defval) ? EQU_DEF : NOT_DEF;
  }
}


bool MemberDef::isReadOnly() const {
  return HasOption("READ_ONLY");
}

bool MemberDef::isGuiReadOnly() const {
  return (HasOption("READ_ONLY") || HasOption("GUI_READ_ONLY"));
}

void MemberDef::SetValVar(const Variant& val, void* base, void* par) {
  return type->SetValVar(val, GetOff(base), par, this);
}

bool MemberDef::ValIsDefault(const void* base, int for_show) const {
  String defval = OptionAfter("DEF_");
  void* mbr_base = GetOff(base);
  // if it has an explicit default, compare using the string
  // we do this regardless whether it is a simple value, or an object
  if (defval.nonempty()) {
    String act_val = type->GetValStr(mbr_base);
    return (act_val == defval);
  }
  // otherwise, delegate to the type -- objects recurse into us
  // if a default value was specified, compare and set the highlight accordingly
  return type->ValIsDefault(mbr_base, this, for_show);
}


String MemberDef::GetHTML(bool gendoc, bool short_fmt) const {
  STRING_BUF(rval, (short_fmt ? 100 : 300)); // extends if needed
  String own_typ;
  TypeDef* ot = GetOwnerType();
  if(ot) {
    own_typ.cat(ot->name).cat("::");
  }
  if(short_fmt) {
    rval.cat("<b><a href=#").cat(name).cat(">").cat(name).cat("</a></b> : ").cat(type->Get_C_Name());
    if(fun_ptr)   rval.cat("(*)"); // function pointer indicator
    if(is_static) rval.cat("&nbsp;&nbsp;<tt> [static]</tt>");
  }
  else {
    rval.cat("<h3 class=\"fn\"><a name=\"").cat(name).cat("\"></a>").cat(own_typ).cat(name).cat(" : ");
    rval.cat(type->GetHTMLLink(gendoc));
    if(fun_ptr)   rval.cat("(*)"); // function pointer indicator
    if(is_static) rval.cat("&nbsp;&nbsp;<tt> [static]</tt>");
    rval.cat("</h3>\n");
    rval.cat("<p>").cat(trim(desc).xml_esc()).cat("</p>\n");
    if(taMisc::help_detail >= taMisc::HD_DETAILS) {
      rval.cat("<p> Size: ").cat(String(type->size)).cat("</p>\n");
      if(opts.size > 0) {
        rval.cat("<p>").cat(GetOptsHTML()).cat("</p>\n");
      }
    }
  }
  return rval;
}

void MemberDef::CopyFromSameType(void* trg_base, void* src_base) {
  type->CopyFromSameType(GetOff(trg_base), GetOff(src_base), this);
}

void MemberDef::CopyOnlySameType(void* trg_base, void* src_base) {
  type->CopyOnlySameType(GetOff(trg_base), GetOff(src_base), this);
}

bool MemberDef::CompareSameType(Member_List& mds, TypeSpace& base_types,
                                voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                                TypeDef* base_typ, void* trg_base, void* src_base,
                                int show_forbidden, int show_allowed, bool no_ptrs,
                                bool test_only) {
  bool some_diff = false;
  if(type->InheritsFormal(TA_class)) {
    if(type->HasOption("EDIT_INLINE") || type->HasOption("INLINE")) {
      // check the members
      some_diff = type->CompareSameType(mds, base_types, trg_bases, src_bases,
                                        GetOff(trg_base), GetOff(src_base),
                                        show_forbidden, show_allowed,
                                        no_ptrs, true); // test only!
      if(some_diff && !test_only) {                     // not already testing, add it
        mds.Link(this); base_types.Link(base_typ);
        trg_bases.Add(trg_base); src_bases.Add(src_base);
      }
    }
    else {
      some_diff = type->CompareSameType(mds, base_types, trg_bases, src_bases,
                                        GetOff(trg_base), GetOff(src_base),
                                        show_forbidden, show_allowed, no_ptrs);
    }
  }
  else {
    // actually do the comparison, based on string value
    if(type->GetValStr(GetOff(trg_base), trg_base, this) !=
       type->GetValStr(GetOff(src_base), src_base, this)) {
      if(!test_only) {
        mds.Link(this); base_types.Link(base_typ);
        trg_bases.Add(trg_base); src_bases.Add(src_base);
      }
      some_diff = true;
    }
  }
  return some_diff;
}

void MemberDef::PrintType(String& col1, String& col2) const {
  col1 = "";
  if(is_static)
    col1 << "static ";
  col1 << type->Get_C_Name();
  if(fun_ptr)
    col2 = String("(*") + name + ")()";
  else
    col2 = name + ";";
  col2 << "  // ";
  if(taMisc::type_info_ == taMisc::MEMB_OFFSETS) {
    intptr_t ui_off = (intptr_t)GetOff((void*)0x100); // 0x100 is arbitrary non-zero number..
    ui_off -= 0x100;                          // now get rid of offset
    if(ui_off > 0) {
      col2 << "+" << ui_off;
      if(base_off > 0)
        col2 << "+" << base_off;
    }
    if(base_off > 0)
      col2 << "+" << base_off;
  }
  if(!desc.empty())
    col2 << " " << desc;
  PrintType_OptsLists(col2);
}

void MemberDef::Print(String& col1, String& col2, void* base, int indent) const {
  void* new_base = GetOff(base);
  col1 = type->Get_C_Name();
  col1 << "  ";
  if(fun_ptr) {
    col1 << String("(*") + name + ")()" << " = ";
  }
  else {
    col1 << name << " = ";
  }
  col2 = type->GetValStr(new_base, base, (MemberDef*)this, TypeDef::SC_DEFAULT, true);
  // force inline
}

#ifdef NO_TA_BASE

//////////////////////////////////////////////////////////
//  dummy versions of dump load/save: see ta_dump.cc    //
//////////////////////////////////////////////////////////

bool MemberDef::DumpMember(void* par) {
  return false;
}


int MemberDef::Dump_Save(ostream&, void*, void*, int) {
  return false;
}

int MemberDef::Dump_SaveR(ostream&, void*, void*, int) {
  return false;
}

int MemberDef::Dump_Save_PathR(ostream&, void*, void*, int) {
  return false;
}

int MemberDef::Dump_Load(istream&, void*, void*) {
  return false;
}

#endif // NO_TA_BASE
