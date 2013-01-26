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

#include "taiVariantType.h"
#include <taiVariant>
#include <QVariant>


int taiVariantType::BidForType(TypeDef* td){
  if(td->IsVariant())
    return(taiType::BidForType(td) + 2); // outbid taiClass type
  return 0;
}

taiData* taiVariantType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef* md) {
  // todo: this needs to be in GetImage I guess -- and variant needs meth to not show type
  // AND it needs to be a taiVariantMember instead of taiVariantType.  ugh.
//   if(md) {
//     String fixtyp = md->OptionAfter("FIXTYPE_ON_");
//     if(fixtyp.nonempty()) {
//       TypeDef* own_td = typ;
//       ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
//       MemberDef* tdmd = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off,
//                                                    fixtyp, false); // no warn
//       if (tdmd && (tdmd->type == &TA_bool)) {
//      if(*((bool*)(MemberDef::GetOff_static(base, net_base_off, net_mbr_off)))) {
//        flags_ |= taiVariantBase::flgFixedType;
//      }
//       }

//     }
//   }
  taiVariant* rval = new taiVariant(host_, par, gui_parent_, flags_);
  return rval;
}

void taiVariantType::GetImage_impl(taiData* dat, const void* base) {
  if (!base) return; // error
  taiVariant* rval = (taiVariant*)dat;
  rval->GetImage(*(Variant*)base);
}

void taiVariantType::GetValue_impl(taiData* dat, void* base) {
  if (!base) return; // error
  taiVariant* rval = (taiVariant*)dat;

  rval->GetValue(*(Variant*)base);
}

