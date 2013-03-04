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

#include "MemberDefBase.h"
#include <MemberDef>
#include <PropertyDef>
#include <TypeDef>
#include <MemberDefBase_List>
#include <taMisc>

#ifndef NO_TA_BASE
taTypeDef_Of(taList_impl);
#include <taiMember>
#endif

void MemberDefBase::Initialize() {
  owner = NULL;
  type = NULL;
  is_static = false;
#ifdef TA_GUI
  im = NULL;
#endif
  show_any = 0; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  show_edit = 0;
  show_tree = 0;
}

MemberDefBase::MemberDefBase()
:inherited()
{
  Initialize();
}

MemberDefBase::MemberDefBase(const char* nm)
:inherited()
{
  Initialize();
  name = nm;
}

MemberDefBase::MemberDefBase(TypeDef* ty, const char* nm, const char* dsc,
  const char* op, const char* lis, bool is_stat)
:inherited()
{
  Initialize();
  type = ty; name = nm; desc = dsc;
  taMisc::CharToStrArray(opts,op);
  taMisc::CharToStrArray(lists,lis);
  is_static = is_stat;
}

MemberDefBase::MemberDefBase(const MemberDefBase& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

void MemberDefBase::Copy(const MemberDefBase& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void MemberDefBase::Copy(const MemberDefBase* cp) {
  if (TypeInfoKind() == cp->TypeInfoKind())
  switch (TypeInfoKind()) {
  case TIK_MEMBER:
    ((MemberDef*)this)->Copy(*(const MemberDef*)cp);
    return;
  case TIK_PROPERTY:
    ((PropertyDef*)this)->Copy(*(const PropertyDef*)cp);
    return;
  default: break; // compiler food
  }
  Copy(*cp); // should never happen!
}

void MemberDefBase::Copy_(const MemberDefBase& cp) {
  type = cp.type;
  inh_opts = cp.inh_opts;
  is_static = cp.is_static;
// don't copy because delete is not ref counted
//  im = cp.im;
// always invalidate show bits, so they get redone in our new context
  show_any = 0; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  show_edit = 0;
  show_tree = 0;
}

MemberDefBase::~MemberDefBase() {
#ifndef NO_TA_BASE
# ifndef NO_TA_GUI
  taRefN::SafeUnRefDone(im);
  im = NULL;
# endif
#endif
}

TypeDef* MemberDefBase::GetOwnerType() const {
  TypeDef* rval=NULL; 
  if((owner) && (owner->owner))
    rval=owner->owner;
  return rval;
}

bool MemberDefBase::CheckList(const String_PArray& lst) const {
  int i;
  for(i=0; i<lists.size; i++) {
    if(lst.FindEl(lists.FastEl(i)) >= 0)
      return true;
  }
  return false;
}

bool MemberDefBase::ShowMember(
  int show_forbidden,
  TypeItem::ShowContext show_context,
  int show_allowed) const
{
  //note: require exact match to special flag, so boolean operations don't match it
  if (show_forbidden == TypeItem::USE_SHOW_GUI_DEF)
    show_forbidden = taMisc::show_gui;

  // check if cache has been done yet
  if (show_any == 0) ShowMember_CalcCache();
  byte show_eff = 0;
  // default viewability for edit is "any OR edit"
  // default viewability for tree is "tree", ie, any not good enough
  switch (show_context) {
  case SC_ANY: show_eff = show_any; break;
  case SC_EDIT: show_eff = show_edit; break;
  case SC_TREE: show_eff = show_tree; break;
  //note: should be no default, let compiler complain if any added
  }
  // our show_eff is the positives (what it is) so if there is nothing there, then
  // we clearly can't show
  // if there is something (a positive) then bit-AND with the
  // show, which is negatives (what not to show), and if anything remains, don't show!
//  show_eff &= TypeItem::SHOW_CHECK_MASK;
//  return (show_eff) && !(show_eff & show);
  return 0 != (show_eff & show_allowed & ~show_forbidden);
}

void MemberDefBase::ShowMember_CalcCache() const {
#ifndef NO_TA_BASE
  // default children are never shown
  TypeDef* par_typ = GetOwnerType();
  if (par_typ && par_typ->DerivesFrom(&TA_taOBase) &&
   !par_typ->DerivesFrom(&TA_taList_impl))
  {
    String mbr = par_typ->OptionAfter("DEF_CHILD_");
    if (mbr.nonempty() && (mbr == name)) {
    show_tree = 0x80; // set the "done" flag
    show_any = 0x80;
    show_edit = 0x80;
    return;
    }
  }
#endif

  // note that "normal" is a special case, which depends both on context and
  // on whether other bits are set, so we calc those individually
  show_any = TypeItem::IS_NORMAL; // the default for any
  ShowMember_CalcCache_impl(show_any, _nilString);

  show_edit = show_any; // start with the "any" settings
  ShowMember_CalcCache_impl(show_edit, "_EDIT");

#ifndef NO_TA_BASE
  //  show_tree = show_any;
  show_tree = TypeItem::IS_NORMAL;
  // for trees, we only browse lists/groups by default
  if (!type->DerivesFrom(&TA_taList_impl))
    show_tree &= ~TypeItem::NO_NORMAL;
#endif
  ShowMember_CalcCache_impl(show_tree, "_TREE");
  //NOTE: lists/groups, we only show by default in lists/groups, embedded lists/groups
}

void MemberDefBase::ShowMember_CalcCache_impl(int& show, const String& suff) const {
  show |= 0x80; // set the "done" flag

  //note: keep in mind that these show bits are the opposite of the show flags,
  // i.e show flags are all negative, whereas these are all positive (bit = is that type)

  //note: member flags should generally trump type flags, so you can SHOW a NO_SHOW type
  //note: NO_SHOW is special, since it negates, so we check for base NO_SHOW everywhere
  bool typ_show = type->HasOption("MEMB_SHOW" + suff);
  bool typ_no_show = type->HasOption("MEMB_NO_SHOW") || type->HasOption("MEMB_NO_SHOW" + suff);
  bool mbr_show = HasOption("SHOW" + suff);
  bool mbr_no_show = HasOption("NO_SHOW") || HasOption("NO_SHOW" + suff);

  // the following are all cumulative, not mutually exclusive
  if (HasOption("HIDDEN" + suff) || type->HasOption("MEMB_HIDDEN" + suff))
    show |= TypeItem::IS_HIDDEN;
  // RO are HIDDEN unless explicitly marked SHOW
  // note: no type-level, makes no sense
  if ((HasOption("READ_ONLY") || HasOption("GUI_READ_ONLY")) && !HasOption("SHOW"))
    show |= TypeItem::IS_HIDDEN;
  if (HasOption("EXPERT" + suff) || type->HasOption("MEMB_EXPERT" + suff))
    show |= TypeItem::IS_EXPERT;

  // if NO_SHOW and no SHOW or explicit other, then never shows
  if (mbr_no_show || (typ_no_show && (!mbr_show || (show & TypeItem::NORM_MEMBS)))) {
    show &= (0x80 | ~TypeItem::SHOW_CHECK_MASK);
    return;
  }

  // if any of the special guys are set, we unset NORMAL (which may
  //   or may not have been already set by default)
  if (show & TypeItem::NORM_MEMBS) // in "any" context, default is "normal"
    show &= ~TypeItem::IS_NORMAL;
  else // no non-NORMAL set
    // SHOW is like an explicit NORMAL if nothing else applies
    if (mbr_show || typ_show)
      show |= TypeItem::IS_NORMAL;
}

