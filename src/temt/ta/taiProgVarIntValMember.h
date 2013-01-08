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

#ifndef taiProgVarIntValMember_h
#define taiProgVarIntValMember_h 1

// parent includes:
#include <taiMember>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiProgVarIntValMember : public taiMember {
  // the int_val member in a ProgVar -- switches from plain int to hard_enum
  TAI_MEMBER_SUBCLASS(taiProgVarIntValMember, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);
protected:
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize();
  void          Destroy() {};
};

#endif // taiProgVarIntValMember_h
