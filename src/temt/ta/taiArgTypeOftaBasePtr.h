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

#ifndef taiArgTypeOftaBasePtr_h
#define taiArgTypeOftaBasePtr_h 1

// parent includes:
#include <taiArgTypeOfTokenPtr>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taiArgTypeOftaBasePtr);

class TA_API taiArgTypeOftaBasePtr : public taiArgTypeOfTokenPtr {
  // for taBase pointers in groups, sets the typedef to be the right one..
  TAI_ARGTYPE_SUBCLASS(taiArgTypeOftaBasePtr, taiArgTypeOfTokenPtr);
public:
  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

#endif // taiArgTypeOftaBasePtr_h
