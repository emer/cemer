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

#include "taGuiWidget_List.h"

TA_BASEFUNS_CTORS_DEFN(taGuiWidget_List);

void taGuiWidget_List::Initialize() {
  SetBaseType(&TA_taGuiWidget);
}

void taGuiWidget_List::GetImage() {
  for(int i=0; i<size; i++) {
    taGuiWidget* w = FastEl(i);
    w->GetImage();
  }
}

void taGuiWidget_List::GetValue() {
  for(int i=0; i<size; i++) {
    taGuiWidget* w = FastEl(i);
    w->GetValue();
  }
}

void taGuiWidget_List::FixAllUrl(const String& url_tag, const String& path) {
  for(int i=0; i<size; i++) {
    taGuiWidget* w = FastEl(i);
    w->FixUrl(url_tag, path);
  }
}
