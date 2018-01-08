// Co2018ght 2017-2017, Regents of the University of Colorado,
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

#include "taiMemberOfProgVarValue.h"
#include <ProgVar>
#include <taiWidgetProgVarValue>

void taiMemberOfProgVarValue::Initialize() {
}

int taiMemberOfProgVarValue::BidForMember(MemberDef* md, TypeDef* td){
  if(md->type->InheritsFrom(&TA_ProgVar) && md->HasOption("EDIT_VALUE"))
    return taiMember::BidForMember(md,td)+1;
  return 0;
}

taiWidget* taiMemberOfProgVarValue::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_) {
  taiWidgetProgVarValue* rval = new taiWidgetProgVarValue(NULL, host_, par, gui_parent_, flags_);
  return rval;
}

void taiMemberOfProgVarValue::GetImage_impl(taiWidget* dat, const void* base) {
  ProgVar* pv = (ProgVar*)mbr->GetOff(base);
  taiWidgetProgVarValue* rval = (taiWidgetProgVarValue*)dat;
  rval->GetImageProgVar(pv);
}

void taiMemberOfProgVarValue::GetMbrValue_impl(taiWidget* dat, void* base) {
  ProgVar* pv = (ProgVar*)mbr->GetOff(base);
  taiWidgetProgVarValue* rval = (taiWidgetProgVarValue*)dat;
  rval->GetValueProgVar(pv);
}
