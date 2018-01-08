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

#include "iPanelOfProgramGroup.h"
#include <Program_Group>
#include <iTreeViewItem>
#include <iProgramEditor>
#include <Program>


iPanelOfProgramGroup::iPanelOfProgramGroup(taiSigLink* dl_)
:inherited(dl_)
{
  Program_Group* prog_ = progGroup();
  if (prog_) {
    taiSigLink* dl = (taiSigLink*)prog_->GetSigLink();
    if (dl) {
      dl->CreateTreeDataNode(NULL, pe->items, NULL, dl->GetName());
    }
  }
  pe->items->ctxt_name = "PRGP";
}


