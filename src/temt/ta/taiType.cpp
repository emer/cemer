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

#include "taiType.h"
#include <taiData>
#include <taiField>
#include <IDataHost>


void taiType::Initialize() {
  m_par_obj_base = NULL;
  m_par_obj_type = NULL;
}

void taiType::Destroy() {
}

// Add "this" type to the td->it slot, or link in lower slot as per the bid.
void taiType::AddToType(TypeDef* td) {
  // td->it is the head-pointer of a linked list of taiTypeBase objects.
  // Objects in the list are sorted by their bid values for the td type,
  // so that the object with the highest bid for the type is the one directly
  // pointed to by td->it.  The next item in the list is pointed to by
  // next_lower_bidder.
  InsertThisIntoBidList(td->it);
}

// Based on various flags, this function calls one of the following:
//   GetDataRep_impl()       // from this class, as a default
//   GetDataRep_impl()       // virtual
//   GetDataRepInline_impl() // virtual
taiData* taiType::GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
                             taiType* parent_type_, int flags_, MemberDef* mbr)
{
  // Note: user can pass in flgReadOnly to force readonly, but we can also set it.
  // Must also take account of whether parent_type is read only.
  if (isReadOnly(par, host_)
      || (parent_type_ && parent_type_->isReadOnly(par)))
  {
    flags_ |= taiData::flgReadOnly;
  }
  if (requiresInline())
  {
    flags_ |= taiData::flgInline;
  }
  taiData* rval = NULL;
  if ((flags_ & taiData::flgReadOnly) && !handlesReadOnly()) {
    // The field needs to be displayed read-only, but the subclass isn't able
    // to handle it as a read-only field, so let taiType take care of things.
    // Note: this is not a virtual function call due to the taiType:: scoping.
    rval = taiType::GetDataRep_impl(host_, par, gui_parent_, flags_, mbr);
  }
  else if ((flags_ & taiData::flgInline) && allowsInline()) {
    rval = GetDataRepInline_impl(host_, par, gui_parent_, flags_, mbr);
  }
  else {
    rval = GetDataRep_impl(host_, par, gui_parent_, flags_, mbr);
  }
  rval->mbr = mbr;
  return rval;
}

taiData* taiType::GetDataRep_impl(IDataHost* host_, taiData* par,
                                  QWidget* gui_parent_, int flags_,
                                  MemberDef*)
{
  // taiField: your friend when all else fails...
  taiField* rval = new taiField(typ, host_, par, gui_parent_, flags_);
  return rval;
}

taiData* taiType::GetDataRepInline_impl(IDataHost* host_, taiData* par,
                                        QWidget* gui_parent, int flags_,
                                        MemberDef* mbr_)
{
  // base type doesn't know what to do for inline, so just returns the basic guy
  return GetDataRep_impl(host_, par, gui_parent, flags_, mbr_);
}

void taiType::GetImage(taiData* dat, const void* base) {
  // Use similar critera as in GetDataRep() to determine whether the
  // subclass can handle this field as read-only.
  //
  // TODO: The old comment said:
  //   // use the exact criteria we used in the GetRep
  // The logic in this function isn't *exactly* the same, since it
  // doesn't call isReadOnly(dat), which would additionally check if
  // the taiData's parent or host are read only.
  bool ro = dat->HasFlag(taiData::flgReadOnly);
  if (ro && !handlesReadOnly()) {
    taiType::GetImage_impl(dat, base);
  }
  else {
    GetImage_impl(dat, base);
  }
}

void taiType::GetImage_impl(taiData* dat, const void* base) {
  //TODO: shouldn't this strval follow same logic as the one in GetDataRep
  String strval = typ->GetValStr(base);
  taiField* rval = (taiField*)dat;
  rval->GetImage(strval);
}

void taiType::GetValue(taiData* dat, void* base) {
  // TODO: see comment in taiType::GetImage().
//  bool ro = isReadOnly(dat);
  // use the exact criteria we used in the GetRep
  bool ro = dat->HasFlag(taiData::flgReadOnly);
  if (!ro || handlesReadOnly()) {
    GetValue_impl(dat, base);
  }
  //note: we don't do anything if ro and type doesn't handle it!
}

void taiType::GetValue_impl(taiData* dat, void* base) {
  taiField* rval = (taiField*)dat;
  String strval(rval->GetValue());
  typ->SetValStr(strval, base);
}

bool taiType::isReadOnly(taiData* dat, IDataHost* host_) {
  // ReadOnly if host_ is RO, OR par is RO, OR directives state RO
  if (dat && dat->readOnly()) {
    return true;
  }
  if (!host_ && dat) {
    host_ = dat->host;
  }
  if (host_ && host_->isReadOnly()) {
    return true;
  }
  return typ->HasOption("READ_ONLY");
}

