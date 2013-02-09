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

#ifndef taiTypeOfList_h
#define taiTypeOfList_h 1

// parent includes:
#include <taiTypeOfClass>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiTypeOfList);

class TA_API taiTypeOfList : public taiTypeOfClass {
  TAI_TYPEBASE_SUBCLASS(taiTypeOfList, taiTypeOfClass);
public:
  int           BidForType(TypeDef* td);
protected:
  taiWidget*      GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
};

#endif // taiTypeOfList_h
