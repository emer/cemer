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

#ifndef taiMemberOfTypeDefault_h
#define taiMemberOfTypeDefault_h 1

// parent includes:
#include <taiMember>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDefault; //


TypeDef_Of(taiMemberOfTypeDefault);

class TA_API taiMemberOfTypeDefault : public taiMember {
  // special for the TypeDefault member (add the "active" box) -- this doesn't use the default handling, and just provides its own directly
  TAI_MEMBER_SUBCLASS(taiMemberOfTypeDefault, taiMember);
public:
  TypeDefault*  tpdflt;

  virtual int           BidForMember(MemberDef* md, TypeDef* td);//
  override taiWidget*     GetDataRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL);
  override void         GetImage(taiWidget* dat, const void* base);
  override void         GetMbrValue(taiWidget* dat, void* base, bool& first_diff);

private:
  void          Initialize()    { tpdflt = NULL; }
  void          Destroy()       { next_lower_bidder = NULL; } // prevent from being destroyed
};

#endif // taiMemberOfTypeDefault_h
