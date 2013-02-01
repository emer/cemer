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

#ifndef taiMemberOfDefaultToken_h
#define taiMemberOfDefaultToken_h 1

// parent includes:
#include <taiMemberOfTokenPtr>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDefault; //


TypeDef_Of(taiMemberOfDefaultToken);

class TA_API taiMemberOfDefaultToken : public taiMemberOfTokenPtr {
  // Special edit menu for the TDefault's token member
  TAI_MEMBER_SUBCLASS(taiMemberOfDefaultToken, taiMemberOfTokenPtr);
public:
  TypeDefault*  tpdflt;

  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IWidgetHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
protected:
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize()    { tpdflt = NULL; }
  void          Destroy() {}
};

#endif // taiMemberOfDefaultToken_h
