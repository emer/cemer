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

#ifndef ActrMotorStyle_h
#define ActrMotorStyle_h 1

// parent includes:
#include <taNBase>
#include "network_def.h"

// member includes:
#include <NameVar_Array>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrMotorStyle);

class E_API ActrMotorStyle : public taNBase {
  // A motor style -- defines parameters for how motor actions are performed -- used to represent prepared and executed motor actions
INHERITED(taNBase)
public:
  float         fprep_time;  // actual feature prep time for the current movement
  float         exec_time;   // actual execution time for the current movement
  float         finish_time; // actual finish time for the current movement
  bool          exec_immediate; // execute this action next -- defaults to true -- if false, then it is prepared only but not executed -- for the prepare request only
  NameVar_Array features; // named movement style features for this movement style

  TA_SIMPLE_BASEFUNS(ActrMotorStyle);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrMotorStyle_h
