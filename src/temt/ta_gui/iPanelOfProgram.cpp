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
  pe->items->setDefaultExpandLevels(12); // shouldn't generally be more than this
  pe->items->setDecorateEnabled(true); //TODO: maybe make this an app option
  connect(pe->items, SIGNAL(CustomExpandFilter(iTreeViewItem*, int, bool&)),
    this, SLOT(items_CustomExpandFilter(iTreeViewItem*, int, bool&)) );
  
  cur_expand_depth = -1;
}

void iPanelOfProgram::items_CustomExpandFilter(iTreeViewItem* item, int level, bool& expand)
{
  if (level < 1) {
    return; // always expand root level
  }
  
  taiSigLink* dl = item->link();
  int depth = taiMisc::GetProgramDefaultExpand(dl->GetName());  // get user's preference for top level proogram groups
  if (depth > -1) {  // must be one of the program groups (objs, types, vars, etc)
    cur_expand_depth = depth;
    if (depth == 0) {
      expand = false;
      return;
    }
    else if (depth >= 1) {
      return;
    }
  }
  else if (level <= cur_expand_depth) {
    if (!taiMisc::GetCallDefaultExpand()) {
      if (dl->taData()->GetTypeDef()->DerivesFrom(&TA_MethodCall) || dl->taData()->GetTypeDef()->DerivesFrom(&TA_StaticMethodCall) ||
          dl->taData()->GetTypeDef()->DerivesFrom(&TA_FunctionCall) || dl->taData()->GetTypeDef()->DerivesFrom(&TA_ProgramCall)) {
        expand = false;
        return;
      }
    }
    else {
      return;
    }
  }
  else {
    expand = false;
    return;
  }
}

void iPanelOfProgram::OnWindowBind_impl(iPanelViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // make sure the Program toolbar is created
  MainWindowViewer* mvw = itv->viewerWindow()->viewer();
  mvw->FindToolBarByType(&TA_ProgramToolBar,"Program");
}


