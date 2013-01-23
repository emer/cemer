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

#ifndef taiEnumTypePtrMember_h
#define taiEnumTypePtrMember_h 1

// parent includes:
#include <taiTypePtrMember>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taiEnumTypePtrMember);

class TA_API taiEnumTypePtrMember : public taiTypePtrMember {
  // typedef ptrs that have member-comment directives, Enums
  TAI_MEMBER_SUBCLASS(taiEnumTypePtrMember, taiTypePtrMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
protected:
  taiData*     GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
private:
  void          Initialize() {}
  void          Destroy() {}
};

#endif // taiEnumTypePtrMember_h
