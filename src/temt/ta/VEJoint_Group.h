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

#ifndef VEJoint_Group_h
#define VEJoint_Group_h 1

// parent includes:
#include <VEJoint>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API VEJoint_Group : public taGroup<VEJoint> {
  // ##CAT_VirtEnv a group of virtual environment joints
INHERITED(taGroup<VEJoint>)
public:
  virtual void  Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void  SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done

  virtual void  CurFromODE(bool updt_disp = false);
  // #CAT_ODE get the updated values from ODE after computing
  virtual void  DestroyODE();
  // #CAT_ODE destroy ODE objs for these items

  TA_BASEFUNS_NOCOPY(VEJoint_Group);
private:
  void  Initialize()            { SetBaseType(&TA_VEJoint); }
  void  Destroy()               { };
};

#endif // VEJoint_Group_h
