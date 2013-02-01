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

#include "taiTypeOfEnum.h"
#include <EnumDef>
#include <taiWidget>
#include <taiWidgetComboBox>
#include <iComboBox>
#include <taiWidgetBitBox>
#include <taiWidgetField>


void taiTypeOfEnum::Initialize() {
}

int taiTypeOfEnum::BidForType(TypeDef* td){
  if (td->IsEnum())
    return (taiType::BidForType(td) +1);
  return 0;
}

taiWidget* taiTypeOfEnum::GetDataRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef*)
{
  isBit = ((typ != NULL) && (typ->HasOption("BITS")));
  m_is_cond = false;
  // determine if has any CONDxxx guys
  if (isBit) {
    for (int i = 0; i < typ->enum_vals.size; ++i) {
      EnumDef* ed = typ->enum_vals.FastEl(i);
      if (ed->HasOption("NO_BIT") || ed->HasOption("IGNORE") ||
        ed->HasOption("NO_SHOW"))
        continue;
      if (ed->OptionAfter("COND").nonempty()) {
        m_is_cond = true;
        break;
      }
    }
  }
  if(!typ->HasOption(TypeItem::opt_NO_APPLY_IMMED)) {
    flags_ |= taiWidget::flgAutoApply; // default is to auto-apply!
  }
  if (isBit) {
    return new taiWidgetBitBox(true, typ, host_, par, gui_parent_, flags_);
  }
  else if (flags_ & taiWidget::flgReadOnly) {
    return new taiWidgetField(typ, host_, par, gui_parent_, flags_);
  }
  else {
    taiWidgetComboBox* rval = new taiWidgetComboBox(true, typ,host_, par, gui_parent_, flags_);
    return rval;
  }
}

void taiTypeOfEnum::GetImage_impl(taiWidget* dat, const void* base) {
  if (isBit) {
    taiWidgetBitBox* rval = (taiWidgetBitBox*)dat;
    rval->m_par_obj_base = GetCurParObjBase(); // note: hack to pass things to bitbox for condshow
    rval->GetImage(*((int*)base));
  }
  else if (isReadOnly(dat)) {
    taiWidgetField* rval = (taiWidgetField*)(dat);
    String str;
    EnumDef* ed = typ->enum_vals.FindNo(*((int*)base));
    if (ed != NULL) {
      str = ed->GetLabel();
    }
    else {
      str = String(*((int*)base));
    }
    rval->GetImage(str);
  }
  else {
    taiWidgetComboBox* rval = (taiWidgetComboBox*)dat;
    int enum_val = *((int*)base);
    rval->GetEnumImage(enum_val);
  }
}

void taiTypeOfEnum::GetValue_impl(taiWidget* dat, void* base) {
  if (isBit) {
    taiWidgetBitBox* rval = dynamic_cast<taiWidgetBitBox*>(dat);
    if (rval) rval->GetValue(*((int*)base));
  } else if (!isReadOnly(dat)) {
    taiWidgetComboBox* rval = dynamic_cast<taiWidgetComboBox*>(dat);
    if (rval) rval->GetEnumValue(*((int*)base));
  }
}
