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

#include "taiTypeOfTokenPtr.h"
#include <taiData>
#include <taiTokenPtrButton>
#include <taSmartPtr>

#include <taMisc>

int taiTypeOfTokenPtr::BidForType(TypeDef* td) {
  if(td->IsBasePointerType())
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiTypeOfTokenPtr::GetDataRep_impl(IWidgetHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef*)
{
  // setting mode now is good for rest of life
  if (typ->IsTaBase())
    mode = MD_BASE;
  else if (typ->DerivesFrom(TA_taSmartPtr))
    mode = MD_SMART_PTR;
  else if (typ->DerivesFrom(TA_taSmartRef))
    mode = MD_SMART_REF;

  TypeDef* npt = GetMinType(NULL); // only a min type
  bool ro = isReadOnly(par, host_);
  if (ro)
    flags_ |= taiData::flgReadOnly;
  else
    flags_ |= (taiData::flgEditOk | taiData::flgEditDialog);
  if (!npt->tokens.keep)
    flags_ |= taiData::flgNoTokenDlg; // no dialog
  flags_ |= (taiData::flgNullOk);

  if(!typ->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiData::flgAutoApply; // default is to auto-apply!

  taiTokenPtrButton* rval = new taiTokenPtrButton(npt, host_, par, gui_parent_, flags_);
  return rval;
}

TypeDef* taiTypeOfTokenPtr::GetMinType(const void* base) {
  // the min type is at least the type of the member, but can be more derived
  TypeDef* rval = NULL;
  // first, we'll try to get a bare minimum type, from the member type itself
  switch (mode) {
  case MD_BASE: {
    rval = typ->GetNonPtrType();
  } break;
  case MD_SMART_PTR: {
    rval = taSmartPtr::GetBaseType(typ);
  } break;
  case MD_SMART_REF: {
    //note: don't know anything about the type w/o an instance
    if (base) {
      taSmartRef& ref = *((taSmartRef*)base);
      rval = ref.GetBaseType();
    } else {
      rval = &TA_taBase;
    }
  } break;
  }
  return rval;
}

void taiTypeOfTokenPtr::GetImage_impl(taiData* dat, const void* base) {
  TypeDef* npt = typ->GetNonPtrType();
  // this seems out of date:
//   bool ro = isReadOnly(dat);
//   if (ro || !npt->tokens.keep) {
//     taiEditButton *ebrval = (taiEditButton*) dat;
//     ebrval->GetImage_(*((void**) base));
//   }
//   else {
    taiTokenPtrButton* rval = (taiTokenPtrButton*)dat;
    rval->GetImage(*((taBase**)base), npt); // default typ, no scope
//   }
}

void taiTypeOfTokenPtr::GetValue_impl(taiData* dat, void* base) {
  TypeDef* npt = typ->GetNonPtrType();
  bool ro = isReadOnly(dat);
  if(ro) {
    // do nothing
  }
  else if(!npt->tokens.keep) {
    taMisc::Warning("taiTypeOfTokenPtr::GetValue_impl: Value not set for type:",npt->name,
                    "because it is not keeping tokens");
  }
  else {
    taiTokenPtrButton* rval = (taiTokenPtrButton*)dat;
    if(!no_setpointer)
      taBase::SetPointer((taBase**)base, (taBase*)rval->GetValue());
    else
      *((void**)base) = rval->GetValue();
/*type must derive from taBase, otherwise we wouldn't have bid!!!
        if(!no_setpointer && typ->IsTaBase())
      taBase::SetPointer((taBase**)base, (taBase*)rval->GetValue());
    else
      *((void**)base) = rval->GetValue(); */
  }
}

taBase* taiTypeOfTokenPtr::GetTokenPtr(const void* base) const {
  taBase* tok_ptr = NULL; // this is the addr of the token, in the member
  switch (mode) {
  case MD_BASE:
  case MD_SMART_PTR:  // is binary compatible
  {
    tok_ptr = *((taBase**)base);
  } break;
  case MD_SMART_REF: {
    taSmartRef& ref = *((taSmartRef*)base);
    tok_ptr = ref.ptr();
  } break;
  }
  return tok_ptr;
}
