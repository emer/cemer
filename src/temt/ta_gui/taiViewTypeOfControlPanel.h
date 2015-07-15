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

#ifndef taiViewTypeOfControlPanel_h
#define taiViewTypeOfControlPanel_h 1

// parent includes:
#include <taiViewTypeOftaOBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiViewTypeOfControlPanel);

class TA_API taiViewTypeOfControlPanel: public taiViewTypeOftaOBase {
  TAI_TYPEBASE_SUBCLASS(taiViewTypeOfControlPanel, taiViewTypeOftaOBase)
public:
  int          BidForView(TypeDef*) override;
  void                  Initialize() {}
  void                  Destroy() {}
protected:
  void         CreateDataPanel_impl(taiSigLink* dl_) override;
};

#endif // taiViewTypeOfControlPanel_h
