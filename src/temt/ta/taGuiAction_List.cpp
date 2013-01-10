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

#include "taGuiAction_List.h"

void taGuiAction_List::Initialize() {
  SetBaseType(&TA_taGuiAction);
}

void taGuiAction_List::FixAllUrl(const String& url_tag, const String& path) {
  for(int i=0; i<size; i++) {
    taGuiAction* w = FastEl(i);
    w->FixUrl(url_tag, path);
  }
}
