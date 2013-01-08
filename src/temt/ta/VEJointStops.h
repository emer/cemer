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

#ifndef VEJointStops_h
#define VEJointStops_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API VEJointStops : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env joint stop parameters
INHERITED(taOBase)
public:
  bool          stops_on;       // turn on stops -- otherwise not used
  float         lo;             // #CONDSHOW_ON_stops_on stop for low angle or position value of joint
  float         hi;             // #CONDSHOW_ON_stops_on stop for high angle or position value of joint
  float         def;            // #CONDSHOW_ON_stops_on default angle or position value of joint -- where it likes to be
  float         bounce;         // #CONDSHOW_ON_stops_on how bouncy is the joint (0 = no bounce, 1 = maximum bounce)
  float         def_force;      // #CONDSHOW_ON_stops_on how much force to apply to return joint to default position -- effectively adds springs to the joint that pull it back to the default position -- NOTE: must call ApplyForce to have this computed and updated

  inline float  Range() const           { return (hi - lo); }
  inline float  Scale() const
  { float rval = Range(); if(rval != 0.0f) rval = 1.0f / rval; return rval; }

  float Normalize(float val) const      { return (val - lo) * Scale(); }
  // normalize given value to 0-1 range given current in hi

  float Project(float val) const        { return lo + (val * Range()); }
  // project a normalized value into the current lo-hi range

  TA_SIMPLE_BASEFUNS(VEJointStops);
// protected:
//   void       UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // VEJointStops_h
