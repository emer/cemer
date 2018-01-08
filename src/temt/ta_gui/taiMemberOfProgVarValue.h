// Copyright 2017-2017, Regents of the University of Colorado,
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

#ifndef taiMemberOfProgVarValue_h
#define taiMemberOfProgVarValue_h 1

// parent includes:
#include <taiMember>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(taiMemberOfProgVarValue);

class TA_API taiMemberOfProgVarValue : public taiMember {
  // a program var with #EDIT_VALUE comment directive to only edit the value (as in control panel)
  TAI_MEMBER_SUBCLASS(taiMemberOfProgVarValue, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td) override;
  taiWidget*    GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_) override;
protected:
  void          GetImage_impl(taiWidget* dat, const void* base) override;
  void          GetMbrValue_impl(taiWidget* dat, void* base) override;
private:
  void          Initialize();
  void          Destroy() {};
};

#endif // taiMemberOfProgVarValue_h
