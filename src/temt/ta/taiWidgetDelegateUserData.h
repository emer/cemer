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

#ifndef taiWidgetDelegateUserData_h
#define taiWidgetDelegateUserData_h 1

// parent includes:
#include <taiWidgetDelegate>

// member includes:

// declare all other types mentioned but not required to include:
class UserDataItem_List;
class taiEditorOfUserData;


class TA_API taiWidgetDelegateUserData: public taiWidgetDelegate {
INHERITED(taiWidgetDelegate)
Q_OBJECT
public:
  UserDataItem_List*    udil;
  taiEditorOfUserData*    uddh;

  bool         IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const CPP11_OVERRIDE;

  QWidget* createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const CPP11_OVERRIDE;

  taiWidgetDelegateUserData(UserDataItem_List* udil_, taiEditorOfUserData* uddh_);
};

#endif // taiWidgetDelegateUserData_h
