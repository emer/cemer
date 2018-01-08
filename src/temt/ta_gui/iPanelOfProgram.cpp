// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "iPanelOfProgram.h"
#include <Program>
#include <ProgCode>
#include <Function>
#include <MethodCall>
#include <StaticMethodCall>
#include <ProgramCall>
#include <FunctionCall>
#include <taMisc>
#include <taiMisc>

#include <iMainWindowViewer>
#include <ProgramToolBar>
#include <taSigLinkItr>
#include <iPanelSet>
#include <iProgramEditor>
#include <iTreeViewItem>

iPanelOfProgram::iPanelOfProgram(taiSigLink* dl_)
:inherited(dl_)
{
  Program* prog_ = prog();
  if (prog_) {
    taiSigLink* dl = (taiSigLink*)prog_->GetSigLink();
    if (dl) {
      dl->CreateTreeDataNode(NULL, pe->items, NULL, dl->GetName());
    }
  }
  pe->items->ctxt_name = "PROG";
  pe->items->setDecorateEnabled(true); //TODO: maybe make this an app option
}

void iPanelOfProgram::OnWindowBind_impl(iPanelViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // make sure the Program toolbar is created
  MainWindowViewer* mvw = itv->viewerWindow()->viewer();
  mvw->FindToolBarByType(&TA_ProgramToolBar,"Program");
}


