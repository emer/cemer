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

#include "NetViewObj.h"

void NetViewObj::Initialize() {
  obj_type = TEXT;
  text = "Select, Context Menu to Edit";
  scale = 1.0f;
  font_size = .05f;
  set_color = false;
  color.Set(0.0f, 0.0f, 0.0f);
}

void NetViewObj::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // anything?
}

