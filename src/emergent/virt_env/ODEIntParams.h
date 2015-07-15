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

#ifndef ODEIntParams_h
#define ODEIntParams_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(ODEIntParams);

class TA_API ODEIntParams : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv ODE integration parameters
INHERITED(taOBase)
public:
  float         erp;            // #DEF_0.2 (0-1, .1-.8 useful range) error reduction parameter: how much of the joint error is reduced at the next time step
  float         cfm;            // #DEF_1e-05 (0-1, 1e-9 - 1 useful range) constraint force mixing parameter: how "soft" is the constraint (0 = hard, 1 = soft)

  TA_SIMPLE_BASEFUNS(ODEIntParams);
protected:
  void  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // ODEIntParams_h
