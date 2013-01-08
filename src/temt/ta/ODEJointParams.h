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

#ifndef ODEJointParams_h
#define ODEJointParams_h 1

// parent includes:
#include <ODEIntParams>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API ODEJointParams : public ODEIntParams {
  // ODE integration parameters for joints
INHERITED(ODEIntParams)
public:
  float         no_stop_cfm;    // #DEF_1e-05 (0-1, 1e-9 - 1 useful range) constraint force mixing parameter when not at a stop: how "soft" is the constraint (0 = hard, 1 = soft)
  float         fudge;          // #DEF_1 (0-1) fudge factor for reducing force when a motor is present -- reduce this value if there is excessive jumping at the joint

  void  Initialize();
  void  Destroy()       { };
  SIMPLE_COPY(ODEJointParams);
  TA_BASEFUNS(ODEJointParams);
// protected:
//   void       UpdateAfterEdit_impl();
};

#endif // ODEJointParams_h
