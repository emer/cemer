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

#ifndef taiTypeOfColor_h
#define taiTypeOfColor_h 1

// parent includes:
#include <taiTypeOfClass>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taiTypeOfColor);

class TA_API taiTypeOfColor : public taiTypeOfClass { // special editor for colors, add dialog
  TAI_TYPEBASE_SUBCLASS(taiTypeOfColor, taiTypeOfClass);
public:
  override bool handlesReadOnly() const { return true; }
  override bool requiresInline() const  { return true; }
  int           BidForType(TypeDef* td);
protected:
  taiData*      GetDataRepInline_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
};

#endif // taiTypeOfColor_h
