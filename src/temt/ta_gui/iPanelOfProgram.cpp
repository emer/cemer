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
}

void iPanelOfProgram::items_CustomExpandFilter(iTreeViewItem* item,
  int level, bool& expand)
{
  if (level < 1) return; // always expand root level
  // by default, expand code guys throughout, plus top-level args, vars and objs
  taiSigLink* dl = item->link();
  TypeDef* typ = dl->GetDataTypeDef();
  if((level <= 1) && (typ->InheritsFrom(&TA_ProgVar_List) ||
                      typ->InheritsFrom(&TA_ProgType_List))) return; // only top guys: args, vars
  if(typ->DerivesFrom(&TA_ProgEl_List) || typ->DerivesFrom(&TA_ProgObjList)
     || typ->DerivesFrom(&TA_Function_List))
    return;                     // expand
  if(typ->InheritsFrom(&TA_ProgEl)) {
    String mbr = typ->OptionAfter("DEF_CHILD_");
    if(!mbr.empty()) {
      MemberDef* md = typ->members.FindName(mbr);
      if(md) {
        if(md->type->InheritsFrom(&TA_ProgEl_List)) return; // expand
      }
      expand = false;           // don't expand any non-progel def childs
    }
    else {
      return;                   // ok to expand
    }
  }
  // otherwise, nada
  expand = false;
}

void iPanelOfProgram::OnWindowBind_impl(iPanelViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // make sure the Program toolbar is created
  MainWindowViewer* mvw = itv->viewerWindow()->viewer();
  mvw->FindToolBarByType(&TA_ProgramToolBar,"Program");
}


