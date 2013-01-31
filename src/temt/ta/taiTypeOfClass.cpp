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

#include "taiTypeOfClass.h"
#include <taiData>
#include <taiPolyData>
#include <taiEditButton>


int taiTypeOfClass::BidForType(TypeDef* td) {
  if(td->IsActualClassNoEff()) 
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiTypeOfClass::GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
                                  taiType* parent_type_, int flags_, MemberDef* mbr)
{
  if (typ->HasOption("INLINE") || typ->HasOption("EDIT_INLINE"))
    flags_ |= taiData::flgInline;
  return inherited::GetDataRep(host_, par, gui_parent_, parent_type_, flags_, mbr);
}

taiData* taiTypeOfClass::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  taiEditButton* rval = taiEditButton::New(NULL, NULL, typ, host_, par, gui_parent_, flags_);
  return rval;
}

taiData* taiTypeOfClass::GetDataRepInline_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  taiPolyData* rval = taiPolyData::New(true, typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiTypeOfClass::GetImage_impl(taiData* dat, const void* base) {
  dat->GetImage_(base);
}

void taiTypeOfClass::GetValue_impl(taiData* dat, void* base) {
  dat->GetValue_(base); //noop for taiEditButton
}

bool taiTypeOfClass::CanBrowse() const {
  //TODO: add additionally supported base types
  return (typ->IsActualTaBase()  && !typ->HasOption("HIDDEN"));
}
