// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "FrameViewer.h"
#include <iFrameViewer>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(FrameViewer);


void FrameViewer::Initialize() {
}

void FrameViewer::GetWinState_impl() {
  iFrameViewer* wid = widget();
  if (!wid) return; // not supposed to happen
  wid->GetWinState();
}

void FrameViewer::SetWinState_impl() {
  iFrameViewer* wid = widget();
  if (!wid) return; // not supposed to happen
  wid->SetWinState();
}

void FrameViewer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading) {
    // revision 8066 renamed the project window frames to Navigator, Editor, Visualizer
    // this code updates old project files when reloaded
    if (name == "Tree") {
      SetName("Navigator");
    }
    else if (name == "Panels") {
      SetName("Editor");
    }
    else if (name == "T3Frames") {
      SetName("Visualizer");
    }
  }
}
