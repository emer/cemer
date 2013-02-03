// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "LeabraProject.h"

#include <taMisc>

TypeDef_Of(LeabraNetwork);
TypeDef_Of(LeabraWizard);

static void leabra_converter_init() {
  DumpFileCvt* cvt = new DumpFileCvt("Leabra", "LeabraUnit");
  cvt->repl_strs.Add(NameVar("_MGroup", "_Group"));
  cvt->repl_strs.Add(NameVar("Project", "V3LeabraProject"));
  cvt->repl_strs.Add(NameVar("V3LeabraProject_Group", "Project_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("V3LeabraProjection", "LeabraPrjn"));       // new prjn type!
  cvt->repl_strs.Add(NameVar("LeabraPrjn_Group", "Projection_Group"));
  cvt->repl_strs.Add(NameVar("Network", "LeabraNetwork"));
  cvt->repl_strs.Add(NameVar("LeabraNetwork_Group", "Network_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("LeabraWiz", "LeabraWizard"));
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

void leabra_module_init() {
  leabra_converter_init();      // configure converter
}

// module initialization
InitProcRegistrar mod_init_leabra(leabra_module_init);


void LeabraProject::Initialize() {
  networks.SetBaseType(&TA_LeabraNetwork);
  wizards.SetBaseType(&TA_LeabraWizard);
}

