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

#ifndef taiDynEnumMember_h
#define taiDynEnumMember_h 1

// parent includes:
#include <taiMember>

// member includes:

// declare all other types mentioned but not required to include:
class taiComboBox; // #IGNORE
class taiBitBox; // #IGNORE


class TA_API taiDynEnumMember : public taiMember {
  // an int member with #DYNENUM_ON_xxx flag indicating DynEnumType guy
  TAI_MEMBER_SUBCLASS(taiDynEnumMember, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);

  static void   UpdateDynEnumCombo(taiComboBox* cb, DynEnum& de);
  // helper function for populating combobox with dyn enum values
  static void   UpdateDynEnumBits(taiBitBox* cb, DynEnum& de);
  // helper function for populating bit box with dyn enum values

protected:
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);

  bool          isBit; // true if a bits type enum

private:
  void          Initialize();
  void          Destroy() {};
};

#endif // taiDynEnumMember_h
