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

#include "LeabraUnGpData.h"

TA_BASEFUNS_CTORS_DEFN(LeabraUnGpData);

void LeabraUnGpData::Initialize() {
  Inhib_Initialize();
}

void LeabraUnGpData::InitLinks() {
  inherited::InitLinks();

  taBase::Own(i_val, this);
  
  taBase::Own(netin, this);
  taBase::Own(netin_raw, this);
  taBase::Own(acts, this);
  taBase::Own(acts_eq, this);
  taBase::Own(acts_raw, this);
  taBase::Own(acts_prvq, this);

  taBase::Own(acts_m, this);
  taBase::Own(acts_p, this);
  taBase::Own(acts_q0, this);

  taBase::Own(un_g_i, this);
  
  taBase::Own(am_deep_raw, this);
  taBase::Own(am_deep_raw_net, this);
  taBase::Own(am_deep_mod_net, this);
  taBase::Own(am_deep_ctxt, this);
}

void LeabraUnGpData::Copy_(const LeabraUnGpData& cp) {
  Inhib_Copy_(cp);
}

void LeabraUnGpData::Init_State() {
  // nop
}
