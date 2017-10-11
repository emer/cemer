// Copyright 2017, Regents of the University of Colorado,
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

#include "BpProject.h"

#include <DumpFileCvt>
#include <taMisc>

eTypeDef_Of(BpNetwork);

TA_BASEFUNS_CTORS_DEFN(BpProject);

static void bp_converter_init() {
  DumpFileCvt* cvt = new DumpFileCvt("Bp", "BpUnit");
  cvt->repl_strs.Add(NameVar("_MGroup", "_Group"));
  cvt->repl_strs.Add(NameVar("Project", "V3BpProject"));
  cvt->repl_strs.Add(NameVar("V3BpProject_Group", "Project_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("V3BpProjection", "Projection"));
  cvt->repl_strs.Add(NameVar("BpPrjn_Group", "Projection_Group"));
  cvt->repl_strs.Add(NameVar("Network", "BpNetwork"));
  cvt->repl_strs.Add(NameVar("BpNetwork_Group", "Network_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("BpWiz", "BpWizard"));
  cvt->repl_strs.Add(NameVar("Layer", "BpLayer"));
  // obsolete types get replaced with taBase..
  cvt->repl_strs.Add(NameVar("WinView_Group", "taBase_Group"));
  cvt->repl_strs.Add(NameVar("ProjViewState_List", "taBase_List"));
  cvt->repl_strs.Add(NameVar("NetView", "taNBase"));
  cvt->repl_strs.Add(NameVar("DataTable", "taNBase"));
  cvt->repl_strs.Add(NameVar("EnviroView", "taNBase"));
  cvt->repl_strs.Add(NameVar("Xform", "taBase"));
  cvt->repl_strs.Add(NameVar("ImageEnv", "ScriptEnv"));
  cvt->repl_strs.Add(NameVar("unique/w=", "unique"));
  taMisc::file_converters.Add(cvt);
}

void bp_module_init() {
  bp_converter_init();          // configure converter
}

// module initialization
InitProcRegistrar mod_init_bp(bp_module_init);

void BpProject::Initialize() {
  networks.el_typ = &TA_BpNetwork;
  // wizards.el_typ = &TA_BpWizard;
}
