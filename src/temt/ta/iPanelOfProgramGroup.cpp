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

#include "iPanelOfProgramGroup.h"
#include <Program_Group>
#include <iTreeViewItem>
#include <iProgramEditor>



void iPanelOfProgramGroup::items_CustomExpandFilter(iTreeViewItem* item,
  int level, bool& expand)
{
  if (level < 1) return; // always expand root level
  // by default, we don't expand code and objs,  but do expand
  // the args, and vars.
  taiSigLink* dl = item->link();
  TypeDef* typ = dl->GetDataTypeDef();
  if (typ->DerivesFrom(&TA_ProgEl_List) ||
    typ->DerivesFrom(&TA_ProgObjList)
  )  {
    expand = false;
  }
}

iPanelOfProgramGroup::iPanelOfProgramGroup(taiSigLink* dl_)
:inherited(dl_)
{
  pe->items->AddFilter("ProgGp");
  Program_Group* prog_ = progGroup();
  if (prog_) {
    taiSigLink* dl = (taiSigLink*)prog_->GetSigLink();
    if (dl) {
      dl->CreateTreeDataNode(NULL, pe->items, NULL, dl->GetName());
    }
  }
  pe->items->setDefaultExpandLevels(2);
  connect(pe->items, SIGNAL(CustomExpandFilter(iTreeViewItem*, int, bool&)),
    this, SLOT(items_CustomExpandFilter(iTreeViewItem*, int, bool&)) );
}


