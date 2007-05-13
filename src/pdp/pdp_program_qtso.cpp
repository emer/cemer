// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "pdp_program_qtso.h"

#include "pdp_program.h"
#include "ta_project.h"
#include "ta_qtviewer.h"

void PDPProgramToolBoxProc(iToolBoxDockViewer* tb) {
  int sec = tb->AssertSection("Network"); //note: need to keep it short
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("init nm units",
    tabMisc::root->GetTemplateInstance(&TA_InitNamedUnits)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("set units lit",
    tabMisc::root->GetTemplateInstance(&TA_SetUnitsLit)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("set units var",
    tabMisc::root->GetTemplateInstance(&TA_SetUnitsVar)));
  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("net ctr init",
    tabMisc::root->GetTemplateInstance(&TA_NetCounterInit)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("net ctr inc",
    tabMisc::root->GetTemplateInstance(&TA_NetCounterIncr)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("net updt view",
    tabMisc::root->GetTemplateInstance(&TA_NetCounterIncr)));
  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("data loop",
    tabMisc::root->GetTemplateInstance(&TA_NetDataLoop)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("gp data lp",
    tabMisc::root->GetTemplateInstance(&TA_NetGroupedDataLoop)));
}

ToolBoxRegistrar pdp_ptb(PDPProgramToolBoxProc);
