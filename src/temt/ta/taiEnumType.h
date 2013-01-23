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

#ifndef taiEnumType_h
#define taiEnumType_h 1

// parent includes:
#include <taiType>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taiEnumType);

class TA_API taiEnumType : public taiType {
  TAI_TYPEBASE_SUBCLASS(taiEnumType, taiType);
public:
  override bool handlesReadOnly() const { return true; } // uses a RO iLineEdit w/ enum name

  inline bool   isCond() const {return m_is_cond;} // true if a BIT, and has any CONDxxxx bits
  int           BidForType(TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
protected:
  void          Initialize();

  bool          isBit; // true if a BIT type enum
  bool          m_is_cond; // true if a BIT, and has any CONDxxxx bits
};

#endif // taiEnumType_h
