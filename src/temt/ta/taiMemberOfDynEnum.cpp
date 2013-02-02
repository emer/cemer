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

#include "taiMemberOfDynEnum.h"
#include <DynEnum>
#include <taiWidgetDeck>
#include <taiWidgetField>
#include <taiWidgetComboBox>
#include <taiWidgetBitBox>
#include <iLineEdit>
#include <iComboBox>
#include <BuiltinTypeDefs>


#include <QHBoxLayout>


void taiMemberOfDynEnum::Initialize() {
  isBit = false;
}

int taiMemberOfDynEnum::BidForMember(MemberDef* md, TypeDef* td){
  TypeDef* mtd = md->type;
  if(td->InheritsFrom(&TA_DynEnum) && mtd->InheritsFrom(&TA_int) &&
     md->OptionAfter("DYNENUM_ON_").nonempty())
    return taiMember::BidForMember(md,td)+1;
  return 0;
}

taiWidget* taiMemberOfDynEnum::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_) {
  flags_ |= taiWidget::flgAutoApply; // always auto-apply
  isBit = false;                // oops -- we don't have base and can't find out!
  taiWidgetDeck* rval = new taiWidgetDeck(NULL, host_, par, gui_parent_, flags_);
  rval->InitLayout();
  gui_parent_ = rval->GetRep();
  taiWidgetBitBox* bit_rep = new taiWidgetBitBox(typ, host_, rval, gui_parent_, flags_);
  taiWidgetField*  field_rep = new taiWidgetField(typ, host_, rval, gui_parent_, flags_); // read only
  taiWidgetComboBox* combo_rep = new taiWidgetComboBox(true, NULL, host_, rval, gui_parent_, flags_);
  rval->data_el.Add(bit_rep);
  rval->AddChildWidget(bit_rep->rep());
  rval->data_el.Add(field_rep);
  rval->AddChildWidget(field_rep->rep());
  rval->data_el.Add(combo_rep);
  rval->AddChildWidget(combo_rep->rep());
  rval->EndLayout();
  return rval;
}

void taiMemberOfDynEnum::UpdateDynEnumCombo(taiWidgetComboBox* cb, DynEnum& de) {
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

void taiMemberOfDynEnum::UpdateDynEnumBits(taiWidgetBitBox* cb, DynEnum& de) {
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

void taiMemberOfDynEnum::GetImage_impl(taiWidget* dat, const void* base) {
  DynEnum* dye = (DynEnum*)base;
  taiWidgetDeck* rval = (taiWidgetDeck*)dat;
  if(!isBit && dye->enum_type && dye->enum_type->bits) {
    isBit = true;
  }
  if(isBit) {
    rval->GetImage(0);
    taiWidgetBitBox* bit_rep = dynamic_cast<taiWidgetBitBox*>(rval->data_el.SafeEl(0));
    if(!bit_rep) return;
    UpdateDynEnumBits(bit_rep, *dye);
  }
  else if (isReadOnly(dat)) {
    rval->GetImage(1);
    taiWidgetField* field_rep = dynamic_cast<taiWidgetField*>(rval->data_el.SafeEl(1));
    if(!field_rep) return;
    String str = dye->NameVal();
    field_rep->GetImage(str);
  }
  else {
    rval->GetImage(2);
    taiWidgetComboBox* combo_rep = dynamic_cast<taiWidgetComboBox*>(rval->data_el.SafeEl(2));
    if(!combo_rep) return;
    UpdateDynEnumCombo(combo_rep, *dye);
  }
}

void taiMemberOfDynEnum::GetMbrValue_impl(taiWidget* dat, void* base) {
  DynEnum* dye = (DynEnum*)base;
  taiWidgetDeck* rval = (taiWidgetDeck*)dat;
  if(!isReadOnly(dat)) {
    if(isBit) {
      taiWidgetBitBox* bit_rep = dynamic_cast<taiWidgetBitBox*>(rval->data_el.SafeEl(0));
      if(!bit_rep) return;
      bit_rep->GetValue(dye->value);
    }
    else {
      taiWidgetComboBox* combo_rep = dynamic_cast<taiWidgetComboBox*>(rval->data_el.SafeEl(2));
      if(!combo_rep) return;
      combo_rep->GetValue(dye->value);
    }
  }
}
