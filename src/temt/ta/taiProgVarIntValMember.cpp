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

#include "taiProgVarIntValMember.h"


void taiProgVarIntValMember::Initialize() {
}

int taiProgVarIntValMember::BidForMember(MemberDef* md, TypeDef* td){
  if(td->InheritsFrom(&TA_ProgVar) && (md->name == "int_val"))
    return taiMember::BidForMember(md,td)+10;
  return 0;
}

taiData* taiProgVarIntValMember::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_) {
  taiDataDeck* rval = new taiDataDeck(NULL, host_, par, gui_parent_, flags_);
  rval->InitLayout();
  gui_parent_ = rval->GetRep();
  taiIncrField* int_rep = new taiIncrField(typ, host_, rval, gui_parent_, flags_);
  int_rep->setMinimum(INT_MIN);
  taiComboBox*  enum_rep = new taiComboBox(true, NULL, host_, rval, gui_parent_, flags_);
  taiBitBox* bit_rep = new taiBitBox(typ, host_, rval, gui_parent_, flags_);
  rval->data_el.Add(int_rep);
  rval->AddChildWidget(int_rep->rep());
  rval->data_el.Add(enum_rep);
  rval->AddChildWidget(enum_rep->rep());
  rval->data_el.Add(bit_rep);
  rval->AddChildWidget(bit_rep->rep());
  rval->EndLayout();
  return rval;
}

void taiProgVarIntValMember::GetImage_impl(taiData* dat, const void* base) {
  ProgVar* pv = (ProgVar*)base;
  int val =  *((int*)mbr->GetOff(base));
  taiDataDeck* rval = (taiDataDeck*)dat;

  if(pv->var_type == ProgVar::T_HardEnum && pv->hard_enum_type) {
    if(pv->hard_enum_type->HasOption("BITS")) {
      rval->GetImage(2);
      taiBitBox* bit_rep = dynamic_cast<taiBitBox*>(rval->data_el.SafeEl(2));
      if (!bit_rep) return; // shouldn't happen
      bit_rep->SetEnumType(pv->hard_enum_type);
      bit_rep->GetImage(val);
    }
    else {
      rval->GetImage(1);
      taiComboBox* enum_rep = dynamic_cast<taiComboBox*>(rval->data_el.SafeEl(1));
      if (!enum_rep) return; // shouldn't happen
      enum_rep->SetEnumType(pv->hard_enum_type);
      enum_rep->GetEnumImage(val);
    }
  }
  else {
    rval->GetImage(0);
    taiIncrField* int_rep = dynamic_cast<taiIncrField*>(rval->data_el.SafeEl(0));
    if (!int_rep) return; // shouldn't happen
    int_rep->GetImage(val);
  }
}

void taiProgVarIntValMember::GetMbrValue_impl(taiData* dat, void* base) {
  ProgVar* pv = (ProgVar*)base;
  int& val =  *((int*)mbr->GetOff(base));
  taiDataDeck* rval = (taiDataDeck*)dat;

  if(pv->var_type == ProgVar::T_HardEnum && pv->hard_enum_type) {
    if(pv->hard_enum_type->HasOption("BITS")) {
      taiBitBox* bit_rep = dynamic_cast<taiBitBox*>(rval->data_el.SafeEl(2));
      if (!bit_rep) return; // shouldn't happen
      bit_rep->GetValue(val);
    }
    else {
      int itm_no = -1;
      taiComboBox* enum_rep = dynamic_cast<taiComboBox*>(rval->data_el.SafeEl(1));
      if (!enum_rep) return; // shouldn't happen
      enum_rep->GetEnumValue(val);
    }
  }
  else {
    taiIncrField* int_rep = dynamic_cast<taiIncrField*>(rval->data_el.SafeEl(0));
    if (!int_rep) return; // shouldn't happen
    val = int_rep->GetValue();
  }
}
