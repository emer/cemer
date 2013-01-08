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

#ifndef ODEWorldParams_h
#define ODEWorldParams_h 1

// parent includes:
#include <ODEIntParams>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API ODEWorldParams : public ODEIntParams {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv ODE integration parameters
  INHERITED(ODEIntParams)
public:
  float         max_cor_vel;    // #DEF_1e+06 maximum correcting velocity for contacts (how quickly they can pop things out of contact)
  float         contact_depth;  // #DEF_0.001 depth of the surface layer arond all geometry objects -- allows things to go this deep into a surface before actual contact is made -- increased value can help prevent jittering
  int           max_col_pts;    // #DEF_4 maximum number of collision points to get (must be less than 64, which is a hard maximum)

  void  Initialize();
  void  Destroy()       { };
  SIMPLE_COPY(ODEWorldParams);
  TA_BASEFUNS(ODEWorldParams);
protected:
  void  UpdateAfterEdit_impl();
};

#endif // ODEWorldParams_h
