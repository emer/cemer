/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// this is the main that is included to make pdplib executable
//

#include <pdp/base.h>
#include "cs_TA_type.h"

static char* my_defaults = 
"// ta_Dump File v1.0\n\
TypeDefault_MGroup .projects[0].defaults { \n\
TypeDefault_MGroup .projects[0].defaults {\n\
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
NameValue @[1] { };\n\
};\n\
};\n\
TypeDefault @[4] { \n\
taBase_Group @.active_membs {\n\
NameValue @[0] { };\n\
};\n\
};\n\
TypeDefault @[5] { \n\
taBase_Group @.active_membs {\n\
NameValue @[0] { };\n\
};\n\
};\n\
};\n\
};\n\
TypeDefault_MGroup .projects[0].defaults {\n\
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
value=\"CsCon\";\n\
};\n\
NameValue @[2] {\n\
name=\"con_gp_type\";\n\
value=\"CsCon_Group\";\n\
};\n\
NameValue @[3] {\n\
name=\"con_spec\";\n\
value=\"{type=CsConSpec: spec=NULL: }\";\n\
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
value=\"Size: 0 (CsUnit)\";\n\
};\n\
NameValue @[1] {\n\
name=\"unit_spec\";\n\
value=\"{type=SigmoidUnitSpec: spec=NULL: }\";\n\
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
value=\"CsTrial\";\n\
};\n\
};\n\
};\n\
TypeDefault @[3] {\n\
name=\"SyncEpochProc\";\n\
default_type=SyncEpochProc;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"\";\n\
NameValue @[0] {\n\
name=\"sub_proc_type\";\n\
value=\"CsTrial\";\n\
};\n\
NameValue @[1] {\n\
name=\"second_proc_type\";\n\
value=\"CsTrial\";\n\
};\n\
};\n\
};\n\
TypeDefault @[4] {\n\
name=\"SequenceProcess\";\n\
default_type=SequenceProcess;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"\";\n\
NameValue @[0] {\n\
name=\"sub_proc_type\";\n\
value=\"CsTrial\";\n\
};\n\
};\n\
};\n\
TypeDefault @[5] {\n\
name=\"CsUnit\";\n\
default_type=CsUnit;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"\";\n\
NameValue @[0] {\n\
name=\"spec\";\n\
value=\"{type=SigmoidUnitSpec: spec=NULL: }\";\n\
};\n\
};\n\
};\n\
};\n\
TypeDefault .projects[0].defaults[0] {\n\
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
value=\"CsCon\";\n\
};\n\
NameValue @[2] {\n\
name=\"con_gp_type\";\n\
value=\"CsCon_Group\";\n\
};\n\
NameValue @[3] {\n\
name=\"con_spec\";\n\
value=\"{type=CsConSpec: spec=NULL: }\";\n\
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
value=\"Size: 0 (CsUnit)\";\n\
};\n\
NameValue @[1] {\n\
name=\"unit_spec\";\n\
value=\"{type=SigmoidUnitSpec: spec=NULL: }\";\n\
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
value=\"CsTrial\";\n\
};\n\
};\n\
};\n\
TypeDefault .projects[0].defaults[3] {\n\
name=\"SyncEpochProc\";\n\
default_type=SyncEpochProc;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"\";\n\
NameValue @[0] {\n\
name=\"sub_proc_type\";\n\
value=\"CsTrial\";\n\
};\n\
NameValue @[1] {\n\
name=\"second_proc_type\";\n\
value=\"CsTrial\";\n\
};\n\
};\n\
};\n\
TypeDefault .projects[0].defaults[4] {\n\
name=\"SequenceProcess\";\n\
default_type=SequenceProcess;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"\";\n\
NameValue @[0] {\n\
name=\"sub_proc_type\";\n\
value=\"CsTrial\";\n\
};\n\
};\n\
};\n\
TypeDefault .projects[0].defaults[5] {\n\
name=\"CsUnit\";\n\
default_type=CsUnit;\n\
active_membs {\n\
el_typ=NameValue;\n\
el_def=0;\n\
name=\"\";\n\
NameValue @[0] {\n\
name=\"spec\";\n\
value=\"{type=SigmoidUnitSpec: spec=NULL: }\";\n\
};\n\
};\n\
};\n\
";

int main(int argc, char* argv[]) {
  pdpMisc::Init_Hook = ta_Init_cs;
  pdpMisc::defaults_str = my_defaults;
  return pdpMisc::Main(argc, argv);
}
