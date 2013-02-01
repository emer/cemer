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

#ifndef taiEditorWidgetsOfTypeItem_h
#define taiEditorWidgetsOfTypeItem_h 1

// parent includes:
#include <taiEditorWidgetsOfClass>

// member includes:
#include <TypeItem>

// declare all other types mentioned but not required to include:


class TA_API taiEditorWidgetsOfTypeItem: public taiEditorWidgetsOfClass {
// #IGNORE displays data on a TypeItem item
INHERITED(taiEditorWidgetsOfClass)
public:
  TypeItem*             ti; // #IGNORE
  TypeItem::TypeInfoKinds          tik;

  taiEditorWidgetsOfTypeItem(TypeItem* ti_, TypeItem::TypeInfoKinds tik, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0);
protected:
  override void         Constr_Data_Labels();
};

#endif // taiEditorWidgetsOfTypeItem_h
