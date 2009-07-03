// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "emergent_program_qtso.h"

#include "emergent_program.h"
#include "ta_project.h"
#include "ta_qtviewer.h"

static void ptbp_add_widget(iToolBoxDockViewer* tb, int sec, TypeDef* td) {
  ProgEl* obj = (ProgEl*)tabMisc::root->GetTemplateInstance(td);
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction(obj->GetToolbarName(), obj));
}

void PDPProgramToolBoxProc(iToolBoxDockViewer* tb) {
  int sec = tb->AssertSection("Network"); //note: need to keep it short
  ptbp_add_widget(tb, sec, &TA_InitNamedUnits);
  ptbp_add_widget(tb, sec, &TA_SetUnitsLit);
  ptbp_add_widget(tb, sec, &TA_SetUnitsVar);
  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_NetCounterInit);
  ptbp_add_widget(tb, sec, &TA_NetCounterIncr);
  ptbp_add_widget(tb, sec, &TA_NetUpdateView);
  ptbp_add_widget(tb, sec, &TA_WtInitPrompt);
  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_NetDataLoop);
  ptbp_add_widget(tb, sec, &TA_NetGroupedDataLoop);
}

ToolBoxRegistrar emergent_ptb(PDPProgramToolBoxProc);
