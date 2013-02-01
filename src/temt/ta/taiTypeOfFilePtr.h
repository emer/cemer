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

#ifndef taiTypeOfFilePtr_h
#define taiTypeOfFilePtr_h 1

// parent includes:
#include <taiType>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taiTypeOfFilePtr);

class TA_API taiTypeOfFilePtr : public taiType {// ptr to tai_getFiles
  TAI_TYPEBASE_SUBCLASS(taiTypeOfFilePtr, taiType);
public:
  int           BidForType(TypeDef* td);
  taiWidget*      GetDataRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiWidget* dat, const void* base);
  void          GetValue_impl(taiWidget* dat, void* base);
};

#endif // taiTypeOfFilePtr_h
