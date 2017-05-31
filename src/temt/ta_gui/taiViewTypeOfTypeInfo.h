// Copyright 2015, Regents of the University of Colorado,
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

#ifndef taiViewTypeOfTypeInfo_h
#define taiViewTypeOfTypeInfo_h 1

// parent includes:
#include <taiViewType>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiViewTypeOfTypeInfo);

class TA_API taiViewTypeOfTypeInfo: public taiViewType { // for TypeItem types and their spaces
  TAI_TYPEBASE_SUBCLASS(taiViewTypeOfTypeInfo, taiViewType)
public:
  int          BidForView(TypeDef*) override;
  taiSigLink* GetSigLink(void* data_, TypeDef* el_typ) override;
  void                  Initialize() {}
  void                  Destroy() {}
};//

#endif // taiViewTypeOfTypeInfo_h
