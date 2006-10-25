// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "ta_viewspec.h"


void ViewSpec::Initialize() {
  visible = true;
}

void ViewSpec::Copy_(const ViewSpec& cp) {
  name = cp.name;
  display_name = cp.display_name;
  visible = cp.visible;
}

bool ViewSpec::SetName(const String& value) {
  name = value; 
  return true;
}

String ViewSpec::GetDisplayName() const {
  if (display_name.nonempty())
    return display_name;
  else return name;
}
