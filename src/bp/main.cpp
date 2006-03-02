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



// this is the main that is included to make pdplib executable

#include "pdpbase.h"
#include "bp_TA_type.h"

static char* my_defaults =
"// ta_Dump File v1.0\n\
TypeDefault_Group .projects[0].defaults { \n\
TypeDefault_Group .projects[0].defaults {\n\
TypeDefault @[0] { \n\
taBase_Group @.active_membs {\n\
NameValue @[0] { };\n\
NameValue @[1] { };\n\
NameValue @[2] { };\n\
NameValue @[3] { };\n\
};\n\
};\n\
TypeDefault @[1] { \n\
taBase_Group @.active_membs {\n\
NameValue @[0] { };\n\
NameValue @[1] { };\n\
};\n\
};\n\
TypeDefault @[2] { \n\
taBase_Group @.active_membs {\n\
NameValue @[0] { };\n\
};\n\
};\n\
TypeDefault @[3] { \n\
taBase_Group @.active_membs {\n\
NameValue @[0] { };\n\
};\n\
};\n\
TypeDefault @[4] { \n\
taBase_Group @.active_membs {\n\
NameValue @[0] { };\n\
};\n\
};\n\
};\n\
};\n\
TypeDefault_Group .projects[0].defaults {\n\
el_typ=TypeDefault;\n\
el_def=0;\n\
name=\"\";\n\
TypeDefault @[0] {\n\
name=\"Projection\";\n\
default_type=Projection;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"taBase_Group_6\";\n\
NameValue @[0] {\n\
name=\"spec\";\n\
value=\"{type=FullPrjnSpec: spec=NULL: }\";\n\
};\n\
NameValue @[1] {\n\
name=\"con_type\";\n\
value=\"BpCon\";\n\
};\n\
NameValue @[2] {\n\
name=\"con_gp_type\";\n\
value=\"BpCon_Group\";\n\
};\n\
NameValue @[3] {\n\
name=\"con_spec\";\n\
value=\"{type=BpConSpec: spec=NULL: }\";\n\
};\n\
};\n\
};\n\
TypeDefault @[1] {\n\
name=\"Layer\";\n\
default_type=Layer;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"taBase_Group_1\";\n\
NameValue @[0] {\n\
name=\"units\";\n\
value=\"Size: 0 (BpUnit)\";\n\
};\n\
NameValue @[1] {\n\
name=\"unit_spec\";\n\
value=\"{type=BpUnitSpec: spec=NULL: }\";\n\
};\n\
};\n\
};\n\
TypeDefault @[2] {\n\
name=\"EpochProcess\";\n\
default_type=EpochProcess;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"taBase_Group_2\";\n\
NameValue @[0] {\n\
name=\"sub_proc_type\";\n\
value=\"BpTrial\";\n\
};\n\
};\n\
};\n\
TypeDefault @[3] {\n\
name=\"SequenceProcess\";\n\
default_type=SequenceProcess;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"\";\n\
NameValue @[0] {\n\
name=\"sub_proc_type\";\n\
value=\"BpTrial\";\n\
};\n\
};\n\
};\n\
TypeDefault @[4] {\n\
name=\"SyncEpochProc\";\n\
default_type=SyncEpochProc;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"\";\n\
NameValue @[0] {\n\
name=\"sub_proc_type\";\n\
value=\"BpTrial\";\n\
};\n\
};\n\
};\n\
};\n\
TypeDefault .projects[0].defaults[0] {\n\
name=\"Projection\";\n\
default_type=Projection;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n"
"name=\"taBase_Group_6\";\n\
NameValue @[0] {\n\
name=\"spec\";\n\
value=\"{type=FullPrjnSpec: spec=NULL: }\";\n\
};\n\
NameValue @[1] {\n\
name=\"con_type\";\n\
value=\"BpCon\";\n\
};\n\
NameValue @[2] {\n\
name=\"con_gp_type\";\n\
value=\"BpCon_Group\";\n\
};\n\
NameValue @[3] {\n\
name=\"con_spec\";\n\
value=\"{type=BpConSpec: spec=NULL: }\";\n\
};\n\
};\n\
};\n\
TypeDefault .projects[0].defaults[1] {\n\
name=\"Layer\";\n\
default_type=Layer;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"taBase_Group_1\";\n\
NameValue @[0] {\n\
name=\"units\";\n\
value=\"Size: 0 (BpUnit)\";\n\
};\n\
NameValue @[1] {\n\
name=\"unit_spec\";\n\
value=\"{type=BpUnitSpec: spec=NULL: }\";\n\
};\n\
};\n\
};\n\
TypeDefault .projects[0].defaults[2] {\n\
name=\"EpochProcess\";\n\
default_type=EpochProcess;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"taBase_Group_2\";\n\
NameValue @[0] {\n\
name=\"sub_proc_type\";\n\
value=\"BpTrial\";\n\
};\n\
};\n\
};\n\
TypeDefault .projects[0].defaults[3] {\n\
name=\"SequenceProcess\";\n\
default_type=SequenceProcess;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"\";\n\
NameValue @[0] {\n\
name=\"sub_proc_type\";\n\
value=\"BpTrial\";\n\
};\n\
};\n\
};\n\
TypeDefault .projects[0].defaults[4] {\n\
name=\"SyncEpochProc\";\n\
default_type=SyncEpochProc;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"\";\n\
NameValue @[0] {\n\
name=\"sub_proc_type\";\n\
value=\"BpTrial\";\n\
};\n\
};\n\
};\n\
";

int main(int argc, char* argv[]) {
//  pdpMisc::Init_Hook = ta_Init_bp;
//  pdpMisc::defaults_str = my_defaults;
//  pdpMisc::def_wizard = &TA_BpWizard;
  return pdpMisc::Main(argc, argv);
}

