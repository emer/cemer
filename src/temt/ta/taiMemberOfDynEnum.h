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

#ifndef taiMemberOfDynEnum_h
#define taiMemberOfDynEnum_h 1

// parent includes:
#include <taiMember>

// member includes:

// declare all other types mentioned but not required to include:
class taiWidgetComboBox; // #IGNORE
class taiWidgetBitBox; // #IGNORE
class DynEnum; //


taTypeDef_Of(taiMemberOfDynEnum);

class TA_API taiMemberOfDynEnum : public taiMember {
  // an int member with #DYNENUM_ON_xxx flag indicating DynEnumType guy
  TAI_MEMBER_SUBCLASS(taiMemberOfDynEnum, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiWidget*      GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);

  static void   UpdateDynEnumCombo(taiWidgetComboBox* cb, DynEnum& de);
  // helper function for populating combobox with dyn enum values
  static void   UpdateDynEnumBits(taiWidgetBitBox* cb, DynEnum& de);
  // helper function for populating bit box with dyn enum values

protected:
  override void GetImage_impl(taiWidget* dat, const void* base);
  override void GetMbrValue_impl(taiWidget* dat, void* base);

  bool          isBit; // true if a bits type enum

private:
  void          Initialize();
  void          Destroy() {};
};

#endif // taiMemberOfDynEnum_h
