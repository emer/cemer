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

#include "PropertyDef.h"
#include <Variant>
#include <TypeDef>
#include <taMisc>

void PropertyDef::Initialize() {
#ifdef NO_TA_BASE
  get_mbr = NULL;
  get_mth = NULL;
  set_mbr = NULL;
  set_mth = NULL;
#endif
  prop_get = NULL;
  prop_set = NULL;
}

PropertyDef::PropertyDef()
:inherited()
{
  Initialize();
}

PropertyDef::PropertyDef(const char* nm)
:inherited(nm)
{
  Initialize();
}

PropertyDef::PropertyDef(TypeDef* ty, const char* nm, const char* dsc,
  const char* op, const char* lis, ta_prop_get_fun get, ta_prop_set_fun set,
  bool is_stat)
:inherited(ty, nm, dsc, op, lis, is_stat)
{
  Initialize();
  prop_get = get;
  prop_set = set;
}

PropertyDef::PropertyDef(const PropertyDef& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

PropertyDef::~PropertyDef() {
#ifdef NO_TA_BASE
  SetRefDone(*((taRefN**)&get_mbr), NULL);
  SetRefDone(*((taRefN**)&get_mth), NULL);
  SetRefDone(*((taRefN**)&set_mbr), NULL);
  SetRefDone(*((taRefN**)&set_mth), NULL);
#endif
}

void PropertyDef::Copy(const PropertyDef& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void PropertyDef::Copy_(const PropertyDef& cp) {
  prop_get = cp.prop_get;
  prop_set =cp.prop_set;
}

/*nn?? const String PropertyDef::GetPathName() const {
  String rval;
  TypeDef* owtp = GetOwnerType();
  if (owtp)
    rval = owtp->GetPathName();
  rval += "::" + name;
  return rval;
} */

const Variant PropertyDef::GetValVar(const void* base) const {
  if (prop_get)
    return prop_get(base);
  else return _nilVariant;
}

bool PropertyDef::isReadOnly() const {
  return (prop_set == NULL);
}

bool PropertyDef::isGuiReadOnly() const {
  return isReadOnly();
}

void PropertyDef::SetValVar(const Variant& val, void* base, void* par) {
  if (prop_set)
    prop_set(base, val);
}

bool PropertyDef::ValIsDefault(const void* base, int for_show) const {
  String defval = OptionAfter("DEF_");
  Variant val = GetValVar(base);
  // if it has an explicit default, compare using the string
  // we do this regardless whether it is a simple value, or an object
  if (defval.nonempty()) {
    return (val.toString() == defval);
  }
  // otherwise, just accept if it is the simple type
  return val.isDefault();
}

String PropertyDef::GetHTML(bool gendoc, bool short_fmt) const {
  // todo: accessor functions, etc
  // todo: fun_ptr!
  STRING_BUF(rval, (short_fmt ? 100 : 300)); // extends if needed
  String own_typ;
  TypeDef* ot = GetOwnerType();
  if(ot) {
    own_typ.cat(ot->name).cat("::");
  }
  if(short_fmt) {
    rval.cat("<b><a href=#").cat(name).cat(">").cat(name).cat("</a></b> : ").cat(type->Get_C_Name());
  }
  else {
    rval.cat("<h3 class=\"fn\"><a name=\"").cat(name).cat("\"></a>").cat(own_typ).cat(name).cat(" : ");
    rval.cat(type->GetHTMLLink(gendoc)).cat("</h3>\n");
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
