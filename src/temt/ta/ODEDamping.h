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

#ifndef ODEDamping_h
#define ODEDamping_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API ODEDamping : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv ODE damping parameters
INHERITED(taOBase)
public:
  bool          on;             // #DEF_false whether to use any of these damping parameters
  float         lin;            // #DEF_0 #CONDSHOW_ON_on The body's linear damping scale.
  float         lin_thr;        // #DEF_0 #CONDSHOW_ON_on The body's linear damping threshold. Damping will be applied only if the linear speed is above the threshold limit
  float         ang;            // #DEF_0 #CONDSHOW_ON_on The body's angular damping scale.
  float         ang_thr;        // #DEF_0 #CONDSHOW_ON_on The body's angular damping threshold. Damping will be applied only if the angular speed is above the threshold limit
  float         ang_speed;         // #DEF_0 #CONDSHOW_ON_on You can also limit the maximum angular speed. In contrast to the damping functions, the angular velocity is affected before the body is moved. This means that it will introduce errors in joints that are forcing the body to rotate too fast. Some bodies have naturally high angular velocities (like cars' wheels), so you may want to give them a very high (like the default, infinity) limit.

  TA_SIMPLE_BASEFUNS(ODEDamping);
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // ODEDamping_h
