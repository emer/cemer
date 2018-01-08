// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef taiViewTypeOfProgramGroup_h
#define taiViewTypeOfProgramGroup_h 1

// parent includes:
#include <taiViewTypeOfGroup>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiViewTypeOfProgramGroup);

class TA_API taiViewTypeOfProgramGroup: public taiViewTypeOfGroup {
  TAI_TYPEBASE_SUBCLASS(taiViewTypeOfProgramGroup, taiViewTypeOfGroup) //
public:
  int          BidForView(TypeDef*) override;
  void                  Initialize() {}
  void                  Destroy() {}
protected:
  void         CreateDataPanel_impl(taiSigLink* dl_) override;
};

#endif // taiViewTypeOfProgramGroup_h
