// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef ActrVisionModule_h
#define ActrVisionModule_h 1

// parent includes:
#include <ActrModule>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrVisionModule);

class E_API ActrVisionModule : public ActrModule {
  // <describe here in full detail in one extended line comment>
INHERITED(ActrModule)
public:

  override void  InitModule();
  override void  ProcessEvent(ActrEvent& event);
  override void  Init();

  TA_SIMPLE_BASEFUNS(ActrVisionModule);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrVisionModule_h
