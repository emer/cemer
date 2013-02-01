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

#ifndef taiMemberOfFileDialog_h
#define taiMemberOfFileDialog_h 1

// parent includes:
#include <taiMember>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taiMemberOfFileDialog);

class TA_API taiMemberOfFileDialog : public taiMember {
  // string with FILE_DIALOG_xxx directive
  TAI_MEMBER_SUBCLASS(taiMemberOfFileDialog, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IWidgetHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
protected:
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

#endif // taiMemberOfFileDialog_h
