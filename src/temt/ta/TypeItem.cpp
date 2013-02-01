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

#include "TypeItem.h"
#include <Variant>
#include <taMisc>
#include <TypeDef>
#include <MemberDef>

#ifndef NO_TA_BASE
#include <taSigLink>
#endif

const String TypeItem::opt_show("SHOW");
const String TypeItem::opt_no_show("NO_SHOW");
const String TypeItem::opt_hidden("HIDDEN");
const String TypeItem::opt_read_only("READ_ONLY");
const String TypeItem::opt_expert("EXPERT");
const String TypeItem::opt_edit_show("EDIT_SHOW");
const String TypeItem::opt_edit_no_show("EDIT_NO_SHOW");
const String TypeItem::opt_edit_hidden("EDIT_HIDDEN");
const String TypeItem::opt_edit_read_only("EDIT_READ_ONLY");
const String TypeItem::opt_edit_detail("EDIT_DETAIL");
const String TypeItem::opt_edit_expert("EDIT_EXPERT");
const String TypeItem::opt_APPLY_IMMED("APPLY_IMMED");
const String TypeItem::opt_NO_APPLY_IMMED("NO_APPLY_IMMED");
const String TypeItem::opt_inline("INLINE");
const String TypeItem::opt_edit_inline("EDIT_INLINE");
const String TypeItem::opt_EDIT_DIALOG("EDIT_DIALOG");

const String TypeItem::opt_bits("BITS");
const String TypeItem::opt_instance("INSTANCE");

TypeItem::TypeItem()
:inherited()
{
  init();
}

TypeItem::TypeItem(const TypeItem& cp) {
  init();
  Copy_(cp);
}

void TypeItem::init()
{
  idx = 0;
  data_link = NULL;
}

TypeItem::~TypeItem() {
#ifndef NO_TA_BASE
  if (data_link != NULL) {
    data_link->DataDestroying(); // link NULLs our pointer
  }
#endif
}

void TypeItem::Copy(const TypeItem& cp) {
  Copy_(cp);
}

void TypeItem::Copy_(const TypeItem& cp) {
  name          = cp.name;
  desc          = cp.desc;
  opts          = cp.opts;
  lists         = cp.lists;
}

String TypeItem::OptionAfter(const String& op) const {
  String tmp_label;
  int opt = opts.FindStartsWith(op, -1);
  if (opt >= 0) { // search bckwrds for overrides..
    tmp_label = opts.FastEl(opt).after(op);
  }
  return tmp_label;
}

bool TypeItem::HasOptionAfter(const String& prefix, const String& op) const {
  int idx = -1;
  do {
    idx = opts.FindStartsWith(prefix, idx + 1);
    if (idx >= 0) {
      if (opts.FastEl(idx).after(prefix) == op) {
        return true;
      }
    }
  }
  while (idx >= 0);
  return false;
}

bool TypeItem::NextOptionAfter(const String& pre, int& itr, String& res) const
{
  if (itr < 0) itr = 0; // sanity
  while (itr < opts.size) {
    String opt = opts.FastEl(itr);
    ++itr;
    if (opt.matches(pre)) {
      res = opt.after(pre);
      return true;
    }
  }
  return false;
}


String TypeItem::GetLabel() const {
  String rval =  OptionAfter("LABEL_");
  if((rval.empty()) && !HasOption("LABEL_")) {  // not intentionally blank
    rval = name;
    taMisc::SpaceLabel(rval);
  } else                                // do translate spaces..
    rval.gsub('_', ' ');
  return rval;
}

String TypeItem::GetOptsHTML() const {
  if(opts.size == 0) return _nilString;
  STRING_BUF(rval, 32); // extends if needed
  rval.cat("Options: ");
  for(int i=0;i<opts.size;i++) {
    if(i > 0) rval.cat(", ");
    rval.cat(trim(opts[i]).xml_esc());
  }
  return rval;
}

bool TypeItem::GetCondOpt(const String condkey, const TypeDef* base_td, const void* base,
                          bool& is_on, bool& val_is_eq) const {
  // format: [CONDEDIT|CONDSHOW|GHOST]_[ON|OFF]_member[:value{,value}[&&,||member[:value{,value}...]] -- must all be && or || for logic
  String optedit = OptionAfter(condkey + "_");
  if (optedit.empty()) return false;

  String onoff = optedit.before('_');
  is_on = (onoff == "ON"); //NOTE: should probably verify if OFF, otherwise it is an error
  optedit = optedit.after('_');

  val_is_eq = false;
  int curlogic = 0;             // 0 = nothing, 1 = AND, 2 = OR,

  while(true) {                 // iterate on logical operations
    String nextedit;
    int nextlogic = 0;          // 0 = nothing, 1 = AND, 2 = OR,
    int andloc = optedit.index("&&");
    int orloc = optedit.index("||");
    if(andloc > 0 && orloc > 0) {
      if(andloc < orloc) {
        nextlogic = 1;
        nextedit = optedit.after(andloc+1);
        optedit = optedit.before(andloc);
      }
      else {
        nextlogic = 2;
        nextedit = optedit.after(orloc+1);
        optedit = optedit.before(orloc);
      }
    }
    else if(andloc > 0) {
      nextlogic = 1;
      nextedit = optedit.after(andloc+1);
      optedit = optedit.before(andloc);
    }
    else if(orloc > 0) {
      nextlogic = 2;
      nextedit = optedit.after(orloc+1);
      optedit = optedit.before(orloc);
    }

    String val = optedit.after(':');

    // find the member name -- depends on mode, see below
    String mbr;
    if (val.empty())
      mbr = optedit; // entire thing is the member, implied boolean
    else
      mbr = optedit.before(':');

    void* mbr_base = NULL;      // base for conditionalizing member itself
    ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
    TypeDef* eff_td = (TypeDef*)base_td;
    MemberDef* md = TypeDef::FindMemberPathStatic(eff_td, net_base_off, net_mbr_off,
                                                  mbr, false); // no warn..
    if (md) {
      mbr_base = MemberDef::GetOff_static(base, net_base_off, net_mbr_off);
    }

    if (!md || !mbr_base) {
      // this can happen in valid cases (selectedit), and the msg is annoying
      //    taMisc::Warning("taiType::CheckProcessCondMembMeth: conditionalizing member", mbr, "not found!");
      return false;
    }

    bool tmp_val_is_eq = false;
    if (val.empty()) {
      // implied boolean member mode
      Variant mbr_val(md->type->GetValVar(mbr_base, md));
      tmp_val_is_eq = mbr_val.toBool();
    }
    else {
      // explicit value mode
      String mbr_val = md->type->GetValStr(mbr_base, NULL, md, TypeDef::SC_DEFAULT, true); // inline
      while (true) {
        String nxtval;
        if (val.contains(',')) {
          nxtval = val.after(',');
          val = val.before(',');
        }
        if(md->type->IsEnum() && mbr_val.contains('|')) { // bits!
          if(mbr_val.contains(val)) {
            String aft = mbr_val.after(val);
            String bef = mbr_val.before(val);
            if((aft.empty() || aft.firstchar() == '|') && (bef.empty() || bef.lastchar() == '|')) {
              // make sure it is not just a subset of something else..
              tmp_val_is_eq = true;
              break;
            }
          }
        }
        else {
          if (val == mbr_val) {
            tmp_val_is_eq = true;
            break;
          }
        }
        if (!nxtval.empty())
          val = nxtval;
        else
          break;
      } // value while
    } // if

    if(curlogic == 0)
      val_is_eq = tmp_val_is_eq;
    else if(curlogic == 1)
      val_is_eq = val_is_eq && tmp_val_is_eq;
    else if(curlogic == 2)
      val_is_eq = val_is_eq || tmp_val_is_eq;

    if(nextlogic > 0 && nextedit.nonempty()) {
      optedit = nextedit;
      curlogic = nextlogic;
    }
    else {
      break;                    // done!
    }
  } // outer logic while
  return true;
}

bool TypeItem::GetCondOptTest(const String condkey, const TypeDef* base_td, const void* base) const {
  bool is_on = false;   // defaults here make it return true if no opt spec'd at all
  bool val_is_eq = false;
  bool rval = GetCondOpt(condkey, base_td, base, is_on, val_is_eq);
  return ((is_on && val_is_eq) || (!is_on && !val_is_eq));
}

void TypeItem::PrintType_OptsLists(String& strm) const {
  if((opts.size > 0) && ((taMisc::type_info_ == taMisc::ALL_INFO) ||
                         (taMisc::type_info_ == taMisc::NO_LISTS))) {
    for(int i=0; i<opts.size; i++)
      strm << " #" << opts.FastEl(i);
  }
  if((lists.size > 0) && ((taMisc::type_info_ == taMisc::ALL_INFO) ||
                          (taMisc::type_info_ == taMisc::NO_OPTIONS))) {
    strm << " LISTS:";
    for(int i=0; i<lists.size; i++)
      strm << " " << lists.FastEl(i);
  }
}


