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
#include <taiData>
#include <taiComboBox>
#include <iComboBox>
#include <taiBitBox>
#include <taiField>


void taiTypeOfEnum::Initialize() {
}

int taiTypeOfEnum::BidForType(TypeDef* td){
  if (td->IsEnum())
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiTypeOfEnum::GetDataRep_impl(IDataHost* host_, taiData* par,
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
    flags_ |= taiData::flgAutoApply; // default is to auto-apply!
  }
  if (isBit) {
    return new taiBitBox(true, typ, host_, par, gui_parent_, flags_);
  }
  else if (flags_ & taiData::flgReadOnly) {
    return new taiField(typ, host_, par, gui_parent_, flags_);
  }
  else {
    taiComboBox* rval = new taiComboBox(true, typ,host_, par, gui_parent_, flags_);
    return rval;
  }
}

void taiTypeOfEnum::GetImage_impl(taiData* dat, const void* base) {
  if (isBit) {
    taiBitBox* rval = (taiBitBox*)dat;
    rval->m_par_obj_base = GetCurParObjBase(); // note: hack to pass things to bitbox for condshow
    rval->GetImage(*((int*)base));
  }
  else if (isReadOnly(dat)) {
    taiField* rval = (taiField*)(dat);
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
    taiComboBox* rval = (taiComboBox*)dat;
    int enum_val = *((int*)base);
    rval->GetEnumImage(enum_val);
  }
}

void taiTypeOfEnum::GetValue_impl(taiData* dat, void* base) {
  if (isBit) {
    taiBitBox* rval = dynamic_cast<taiBitBox*>(dat);
    if (rval) rval->GetValue(*((int*)base));
  } else if (!isReadOnly(dat)) {
    taiComboBox* rval = dynamic_cast<taiComboBox*>(dat);
    if (rval) rval->GetEnumValue(*((int*)base));
  }
}
