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

#include "taiDynEnumMember.h"
#include <DynEnum>
#include <taiDataDeck>
#include <taiField>
#include <taiComboBox>
#include <taiBitBox>
#include <iLineEdit>
#include <iComboBox>
#include <BuiltinTypeDefs>


#include <QHBoxLayout>


void taiDynEnumMember::Initialize() {
  isBit = false;
}

int taiDynEnumMember::BidForMember(MemberDef* md, TypeDef* td){
  TypeDef* mtd = md->type;
  if(td->InheritsFrom(&TA_DynEnum) && mtd->InheritsFrom(&TA_int) &&
     md->OptionAfter("DYNENUM_ON_").nonempty())
    return taiMember::BidForMember(md,td)+1;
  return 0;
}

taiData* taiDynEnumMember::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_) {
  flags_ |= taiData::flgAutoApply; // always auto-apply
  isBit = false;                // oops -- we don't have base and can't find out!
  taiDataDeck* rval = new taiDataDeck(NULL, host_, par, gui_parent_, flags_);
  rval->InitLayout();
  gui_parent_ = rval->GetRep();
  taiBitBox* bit_rep = new taiBitBox(typ, host_, rval, gui_parent_, flags_);
  taiField*  field_rep = new taiField(typ, host_, rval, gui_parent_, flags_); // read only
  taiComboBox* combo_rep = new taiComboBox(true, NULL, host_, rval, gui_parent_, flags_);
  rval->data_el.Add(bit_rep);
  rval->AddChildWidget(bit_rep->rep());
  rval->data_el.Add(field_rep);
  rval->AddChildWidget(field_rep->rep());
  rval->data_el.Add(combo_rep);
  rval->AddChildWidget(combo_rep->rep());
  rval->EndLayout();
  return rval;
}

void taiDynEnumMember::UpdateDynEnumCombo(taiComboBox* cb, DynEnum& de) {
  cb->Clear();
  if(!de.enum_type) return;
  for (int i = 0; i < de.enum_type->enums.size; ++i) {
    const DynEnumItem* dei = de.enum_type->enums.FastEl(i);
    //note: dynenums store the index of the value, not the value
    cb->AddItem(dei->name, i); //TODO: desc in status bar or such would be nice!
  }
  if(de.value < 0) de.value = 0; // un-init -- init!
  cb->GetImage(de.value);
}

void taiDynEnumMember::UpdateDynEnumBits(taiBitBox* cb, DynEnum& de) {
  cb->Clear();
  if(!de.enum_type) return;
  for (int i = 0; i < de.enum_type->enums.size; ++i) {
    const DynEnumItem* dei = de.enum_type->enums.FastEl(i);
    //note: dynenums store the index of the value, not the value
    cb->AddBoolItem(true, dei->name, dei->value, dei->desc, false);
  }
  cb->lay->addStretch();
  if(de.value < 0) de.value = 0; // un-init -- init!
  cb->GetImage(de.value);
}

void taiDynEnumMember::GetImage_impl(taiData* dat, const void* base) {
  DynEnum* dye = (DynEnum*)base;
  taiDataDeck* rval = (taiDataDeck*)dat;
  if(!isBit && dye->enum_type && dye->enum_type->bits) {
    isBit = true;
  }
  if(isBit) {
    rval->GetImage(0);
    taiBitBox* bit_rep = dynamic_cast<taiBitBox*>(rval->data_el.SafeEl(0));
    if(!bit_rep) return;
    UpdateDynEnumBits(bit_rep, *dye);
  }
  else if (isReadOnly(dat)) {
    rval->GetImage(1);
    taiField* field_rep = dynamic_cast<taiField*>(rval->data_el.SafeEl(1));
    if(!field_rep) return;
    String str = dye->NameVal();
    field_rep->GetImage(str);
  }
  else {
    rval->GetImage(2);
    taiComboBox* combo_rep = dynamic_cast<taiComboBox*>(rval->data_el.SafeEl(2));
    if(!combo_rep) return;
    UpdateDynEnumCombo(combo_rep, *dye);
  }
}

void taiDynEnumMember::GetMbrValue_impl(taiData* dat, void* base) {
  DynEnum* dye = (DynEnum*)base;
  taiDataDeck* rval = (taiDataDeck*)dat;
  if(!isReadOnly(dat)) {
    if(isBit) {
      taiBitBox* bit_rep = dynamic_cast<taiBitBox*>(rval->data_el.SafeEl(0));
      if(!bit_rep) return;
      bit_rep->GetValue(dye->value);
    }
    else {
      taiComboBox* combo_rep = dynamic_cast<taiComboBox*>(rval->data_el.SafeEl(2));
      if(!combo_rep) return;
      combo_rep->GetValue(dye->value);
    }
  }
}
