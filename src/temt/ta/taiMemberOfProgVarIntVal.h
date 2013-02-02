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

#ifndef taiMemberOfProgVarIntVal_h
#define taiMemberOfProgVarIntVal_h 1

// parent includes:
#include <taiMember>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taiMemberOfProgVarIntVal);

class TA_API taiMemberOfProgVarIntVal : public taiMember {
  // the int_val member in a ProgVar -- switches from plain int to hard_enum
  TAI_MEMBER_SUBCLASS(taiMemberOfProgVarIntVal, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiWidget*      GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);
protected:
  override void GetImage_impl(taiWidget* dat, const void* base);
  override void GetMbrValue_impl(taiWidget* dat, void* base);
private:
  void          Initialize();
  void          Destroy() {};
};

#endif // taiMemberOfProgVarIntVal_h
