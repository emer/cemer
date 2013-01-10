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

#include "ODEIntParams.h"

#include <ode/ode.h>

void ODEIntParams::Initialize() {
  erp = 0.2f;
  cfm = 1.0e-5f;
}

void ODEIntParams::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(erp < .01f) erp = .01f;
  if(erp > .99f) erp = .99f;
  if(cfm < 1.0e-9f) cfm = 1.0e-9f;
  if(cfm > 1.0f) cfm = 1.0f;
}

