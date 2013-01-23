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

#ifndef taitaColorType_h
#define taitaColorType_h 1

// parent includes:
#include <taiColorType>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taitaColorType);

class TA_API taitaColorType : public taiColorType { // special editor for colors, add dialog
  TAI_TYPEBASE_SUBCLASS(taitaColorType, taiColorType);
public:
  int           BidForType(TypeDef* td);
protected:
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
};

#endif // taitaColorType_h
