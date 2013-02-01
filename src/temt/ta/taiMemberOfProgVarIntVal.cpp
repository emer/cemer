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

#include "taiMemberOfProgVarIntVal.h"
#include <ProgVar>

#include <taiWidgetDeck>
#include <taiWidgetIncrField>
#include <taiWidgetComboBox>
#include <taiWidgetBitBox>
#include <iComboBox>
#include <iSpinBox>



void taiMemberOfProgVarIntVal::Initialize() {
}

int taiMemberOfProgVarIntVal::BidForMember(MemberDef* md, TypeDef* td){
  if(td->InheritsFrom(&TA_ProgVar) && (md->name == "int_val"))
    return taiMember::BidForMember(md,td)+10;
  return 0;
}

taiWidget* taiMemberOfProgVarIntVal::GetDataRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_) {
  taiWidgetDeck* rval = new taiWidgetDeck(NULL, host_, par, gui_parent_, flags_);
  rval->InitLayout();
  gui_parent_ = rval->GetRep();
  taiWidgetIncrField* int_rep = new taiWidgetIncrField(typ, host_, rval, gui_parent_, flags_);
  int_rep->setMinimum(INT_MIN);
  taiWidgetComboBox*  enum_rep = new taiWidgetComboBox(true, NULL, host_, rval, gui_parent_, flags_);
  taiWidgetBitBox* bit_rep = new taiWidgetBitBox(typ, host_, rval, gui_parent_, flags_);
  rval->data_el.Add(int_rep);
  rval->AddChildWidget(int_rep->rep());
  rval->data_el.Add(enum_rep);
  rval->AddChildWidget(enum_rep->rep());
  rval->data_el.Add(bit_rep);
  rval->AddChildWidget(bit_rep->rep());
  rval->EndLayout();
  return rval;
}

void taiMemberOfProgVarIntVal::GetImage_impl(taiWidget* dat, const void* base) {
  ProgVar* pv = (ProgVar*)base;
  int val =  *((int*)mbr->GetOff(base));
  taiWidgetDeck* rval = (taiWidgetDeck*)dat;

  if(pv->var_type == ProgVar::T_HardEnum && pv->hard_enum_type) {
    if(pv->hard_enum_type->HasOption("BITS")) {
      rval->GetImage(2);
      taiWidgetBitBox* bit_rep = dynamic_cast<taiWidgetBitBox*>(rval->data_el.SafeEl(2));
      if (!bit_rep) return; // shouldn't happen
      bit_rep->SetEnumType(pv->hard_enum_type);
      bit_rep->GetImage(val);
    }
    else {
      rval->GetImage(1);
      taiWidgetComboBox* enum_rep = dynamic_cast<taiWidgetComboBox*>(rval->data_el.SafeEl(1));
      if (!enum_rep) return; // shouldn't happen
      enum_rep->SetEnumType(pv->hard_enum_type);
      enum_rep->GetEnumImage(val);
    }
  }
  else {
    rval->GetImage(0);
    taiWidgetIncrField* int_rep = dynamic_cast<taiWidgetIncrField*>(rval->data_el.SafeEl(0));
    if (!int_rep) return; // shouldn't happen
    int_rep->GetImage(val);
  }
}

void taiMemberOfProgVarIntVal::GetMbrValue_impl(taiWidget* dat, void* base) {
  ProgVar* pv = (ProgVar*)base;
  int& val =  *((int*)mbr->GetOff(base));
  taiWidgetDeck* rval = (taiWidgetDeck*)dat;

  if(pv->var_type == ProgVar::T_HardEnum && pv->hard_enum_type) {
    if(pv->hard_enum_type->HasOption("BITS")) {
      taiWidgetBitBox* bit_rep = dynamic_cast<taiWidgetBitBox*>(rval->data_el.SafeEl(2));
      if (!bit_rep) return; // shouldn't happen
      bit_rep->GetValue(val);
    }
    else {
      int itm_no = -1;
      taiWidgetComboBox* enum_rep = dynamic_cast<taiWidgetComboBox*>(rval->data_el.SafeEl(1));
      if (!enum_rep) return; // shouldn't happen
      enum_rep->GetEnumValue(val);
    }
  }
  else {
    taiWidgetIncrField* int_rep = dynamic_cast<taiWidgetIncrField*>(rval->data_el.SafeEl(0));
    if (!int_rep) return; // shouldn't happen
    val = int_rep->GetValue();
  }
}
