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

#ifndef taiMemberOfTokenPtrFromGroup_h
#define taiMemberOfTokenPtrFromGroup_h 1

// parent includes:
#include <taiMemberOfTokenPtr>

// member includes:

// declare all other types mentioned but not required to include:
class taList_impl; //


taTypeDef_Of(taiMemberOfTokenPtrFromGroup);

class TA_API taiMemberOfTokenPtrFromGroup : public taiMemberOfTokenPtr {
  TAI_MEMBER_SUBCLASS(taiMemberOfTokenPtrFromGroup, taiMemberOfTokenPtr);
public:
  int           BidForMember(MemberDef* md, TypeDef* td) override;
  taiWidget*    GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr) override;
  void          GetImage_impl(taiWidget* dat, const void* base) override;
  void          GetMbrValue(taiWidget* dat, void* base, bool& first_diff) override;

private:
  void          Initialize() {}
  void          Destroy() {}
};

#endif // taiMemberOfTokenPtrFromGroup_h
