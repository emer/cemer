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

#include "taTypeInfoViewType.h"
#include <taTypeInfoDataLink>
#include <taMemberSpaceDataLink>
#include <taPropertySpaceDataLink>
#include <taMethodSpaceDataLink>
#include <taTypeSpaceDataLink>

#include <taMisc>


int taTypeInfoViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_TypeItem) ||
    td->InheritsFrom(&TA_EnumSpace) ||
    td->InheritsFrom(&TA_TokenSpace) ||
    td->InheritsFrom(&TA_MemberSpace) ||
    td->InheritsFrom(&TA_PropertySpace) ||
    td->InheritsFrom(&TA_MethodSpace) ||
    td->InheritsFrom(&TA_TypeSpace)
  )
    return (inherited::BidForView(td) + 1);
  return 0;
}


taiDataLink* taTypeInfoViewType::GetDataLink(void* el, TypeDef* td) {
  if (!el) return NULL; 

  TypeItem::TypeInfoKinds tik = taMisc::TypeToTypeInfoKind(td);

  switch (tik) {
  case TypeItem::TIK_ENUM:
  case TypeItem::TIK_MEMBER:
  case TypeItem::TIK_PROPERTY:
  case TypeItem::TIK_METHOD:
  case TypeItem::TIK_TYPE: {
    TypeItem* ti = static_cast<TypeItem*>(el);
    if (ti->data_link) return static_cast<taiDataLink*>(ti->data_link);
    else return new taTypeInfoDataLink(tik, ti);
    }
  case TypeItem::TIK_MEMBERSPACE: {
    MemberSpace* s = static_cast<MemberSpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taMemberSpaceDataLink(s);
    }
  case TypeItem::TIK_PROPERTYSPACE: {
    PropertySpace* s = static_cast<PropertySpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taPropertySpaceDataLink(s);
    }
  case TypeItem::TIK_METHODSPACE: {
    MethodSpace* s = static_cast<MethodSpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taMethodSpaceDataLink(s);
    }
  case TypeItem::TIK_TYPESPACE: {
    TypeSpace* s = static_cast<TypeSpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taTypeSpaceDataLink(s);
    }
  case TypeItem::TIK_TOKENSPACE:
  default:
    return NULL;
  }
  return NULL; //compiler food
}
