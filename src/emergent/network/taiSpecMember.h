// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef taiSpecMember_h
#define taiSpecMember_h 1

// parent includes:
#include "network_def.h"
#include <taiMember>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(taiSpecMember);

class EMERGENT_API taiSpecMember : public taiMember {
  // special for the spec type member (adds the unique box)
  TAI_MEMBER_SUBCLASS(taiSpecMember, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);

  void          CmpOrigVal(taiData* dat, const void* base, bool& first_diff); // replaces

  virtual bool  NoCheckBox(IDataHost* host_) const; // deterimine if check box should be used

protected:
  override taiData*     GetArbitrateDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef* mbr_);
  override void         GetArbitrateImage(taiData* dat, const void* base);
  override void         GetArbitrateMbrValue(taiData* dat, void* base, bool& first_diff);
};

#endif // taiSpecMember_h
