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

#include "taClassDataLink.h"


taClassDataLink::taClassDataLink(TypeItem::TypeInfoKinds tik_, void* data_, taDataLink* &link_ref_)
:inherited(data_, link_ref_), tik(tik_)
{
  switch (tik) {
  case TypeItem::TIK_ENUM: m_type = &TA_EnumDef; break;
  case TypeItem::TIK_MEMBER: m_type = &TA_MemberDef; break;
  case TypeItem::TIK_PROPERTY: m_type = &TA_PropertyDef; break;
  case TypeItem::TIK_METHOD: m_type = &TA_MethodDef; break;
  case TypeItem::TIK_TYPE: m_type = &TA_TypeDef; break;
  case TypeItem::TIK_ENUMSPACE: m_type = &TA_EnumSpace; break;
  case TypeItem::TIK_TOKENSPACE: m_type = &TA_TokenSpace; break;
  case TypeItem::TIK_MEMBERSPACE: m_type = &TA_MemberSpace; break;
  case TypeItem::TIK_PROPERTYSPACE: m_type = &TA_PropertySpace; break;
  case TypeItem::TIK_METHODSPACE: m_type = &TA_MethodSpace; break;
  case TypeItem::TIK_TYPESPACE: m_type = &TA_TypeSpace; break;
  default: m_type = NULL; // compiler food
  }
}

TypeDef* taClassDataLink::GetDataTypeDef() const {
  switch (tik) {
  case TypeItem::TIK_ENUM: return &TA_EnumDef;
  case TypeItem::TIK_MEMBER: return &TA_MemberDef;
  case TypeItem::TIK_PROPERTY: return &TA_PropertyDef;
  case TypeItem::TIK_METHOD: return &TA_MethodDef;
  case TypeItem::TIK_TYPE: return &TA_TypeDef;
  case TypeItem::TIK_ENUMSPACE: return &TA_EnumSpace;
  case TypeItem::TIK_TOKENSPACE: return &TA_TokenSpace;
  case TypeItem::TIK_MEMBERSPACE: return &TA_MemberSpace;
  case TypeItem::TIK_PROPERTYSPACE: return &TA_PropertySpace;
  case TypeItem::TIK_METHODSPACE: return &TA_MethodSpace;
  case TypeItem::TIK_TYPESPACE: return &TA_TypeSpace;
  default: return &TA_void; // compiler food; anything but NULL!!!
  }
}

