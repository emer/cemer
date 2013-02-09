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

#ifndef taSigLinkClass_h
#define taSigLinkClass_h 1

// parent includes:
#include <taiSigLink>

// member includes:
#include <TypeItem>

// declare all other types mentioned but not required to include:


taTypeDef_Of(taSigLinkClass);

class TA_API taSigLinkClass: public taiSigLink { // SigLink for TypeInfo objects
INHERITED(taiSigLink)
public:
  const TypeItem::TypeInfoKinds    tik;
  TypeDef*              type() const {return m_type;}

  override TypeDef*     GetDataTypeDef() const;

  SL_FUNS(taSigLinkClass); //

protected:
  TypeDef*              m_type;
  taSigLinkClass(TypeItem::TypeInfoKinds tik_, void* data_, taSigLink* &link_ref_);  //
};

#endif // taSigLinkClass_h
