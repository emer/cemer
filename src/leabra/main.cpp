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


/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than sale or distribution is hereby granted without  //
// fee, provided  that the above copyright notice and this permission notice  //
// appear in all copies of the software and related documentation.            //
//									      //
// PERMISSION TO MARKET, DISTRIBUTE, OR SELL THIS SOFTWARE OR ITS             //
// DOCUMENTATION IS EXPRESSLY *NOT* GRANTED AND IS RETAINED BY THE COPYRIGHT  //
// HOLDERS.					       			      //
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
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR		      //
// ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,    //
// OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,     //
// WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF  //
// LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE     //
// OF THIS SOFTWARE.							      //
//     									      //
==============================================================================*/

// this is the main that is included to make pdplib executable
//

#include "pdpbase.h"
#include "leabra_TA_type.h"

static char* my_defaults =
"// ta_Dump File v1.0\n\
TypeDefault_MGroup .projects[0].defaults { \n\
 TypeDefault .projects[0].defaults[0] { \n\
  NameValue @.active_membs[0] { };\n\
  NameValue @.active_membs[1] { };\n\
  NameValue @.active_membs[2] { };\n\
  NameValue @.active_membs[3] { };\n\
 };\n\
 TypeDefault .projects[0].defaults[1] { \n\
  NameValue @.active_membs[0] { };\n\
  NameValue @.active_membs[1] { };\n\
 };\n\
 TypeDefault .projects[0].defaults[2] { \n\
  NameValue @.active_membs[0] { };\n\
 };\n\
 TypeDefault .projects[0].defaults[3] { \n\
  NameValue @.active_membs[0] { };\n\
  NameValue @.active_membs[1] { };\n\
  NameValue @.active_membs[2] { };\n\
  NameValue @.active_membs[3] { };\n\
 };\n\
 TypeDefault .projects[0].defaults[4] { \n\
  NameValue @.active_membs[0] { };\n\
 };\n\
 TypeDefault .projects[0].defaults[5] { \n\
  NameValue @.active_membs[0] { };\n\
 };\n\
 TypeDefault .projects[0].defaults[6] { \n\
  NameValue @.active_membs[0] { };\n\
  NameValue @.active_membs[1] { };\n\
  NameValue @.active_membs[2] { };\n\
  NameValue @.active_membs[3] { };\n\
 };\n\
};\n\
TypeDefault_MGroup .projects[0].defaults {\n\
 el_typ = TypeDefault;\n\
 el_def = 0;\n\
 name = \"\";\n\
};\n\
TypeDefault .projects[0].defaults[0] {\n\
 name = \"Projection\";\n\
 default_type = Projection;\n\
 active_membs  taBase_Group  {\n\
  el_typ = NameValue;\n\
  el_def = 0;\n\
  name = \"\";\n\
 };\n\
 NameValue @.active_membs[0] {\n\
  name = \"spec\";\n\
  value = \"{type=FullPrjnSpec: spec=NULL: }\";\n\
 };\n\
 NameValue @.active_membs[1] {\n\
  name = \"con_type\";\n\
  value = \"LeabraCon\";\n\
 };\n\
 NameValue @.active_membs[2] {\n\
  name = \"con_gp_type\";\n\
  value = \"LeabraCon_Group\";\n\
 };\n\
 NameValue @.active_membs[3] {\n\
  name = \"con_spec\";\n\
  value = \"{type=LeabraConSpec: spec=NULL: }\";\n\
 };\n\
};\n\
TypeDefault .projects[0].defaults[1] {\n\
 name = \"LeabraLayer\";\n\
 default_type = LeabraLayer;\n\
 active_membs  taBase_Group  {\n\
  el_typ = NameValue;\n\
  el_def = 0;\n\
  name = \"\";\n\
 };\n\
 NameValue @.active_membs[0] {\n\
  name = \"units\";\n\
  value = \"Size: 0 (LeabraUnit)\";\n\
 };\n\
 NameValue @.active_membs[1] {\n\
  name = \"unit_spec\";\n\
  value = \"{type=LeabraUnitSpec: spec=NULL: }\";\n\
 };\n\
};\n\
TypeDefault .projects[0].defaults[2] {\n\
 name = \"Network\";\n\
 default_type = Network;\n\
 active_membs  taBase_Group  {\n\
  el_typ = NameValue;\n\
  el_def = 0;\n\
  name = \"\";\n\
 };\n\
 NameValue @.active_membs[0] {\n\
  name = \"layers\";\n\
  value = \"LeabraLayer\";\n\
 };\n\
};\n\
TypeDefault .projects[0].defaults[3] {\n\
 name = \"EpochProcess\";\n\
 default_type = EpochProcess;\n\
 active_membs  taBase_Group  {\n\
  el_typ = NameValue;\n\
  el_def = 0;\n\
  name = \"\";\n\
 };\n\
 NameValue @.active_membs[0] {\n\
  name = \"sub_proc_type\";\n\
  value = \"LeabraTrial\";\n\
 };\n\
 NameValue @.active_membs[1] {\n\
  name = \"order\";\n\
  value = \"PERMUTED\";\n\
 };\n\
 NameValue @.active_membs[2] {\n\
  name = \"wt_update\";\n\
  value = \"ON_LINE\";\n\
 };\n\
 NameValue @.active_membs[3] {\n\
  name = \"between_events\";\n\
  value = \"INIT_STATE\";\n\
 };\n\
};\n\
TypeDefault .projects[0].defaults[4] {\n\
 name = \"SequenceProcess\";\n\
 default_type = SequenceProcess;\n\
 active_membs  taBase_Group  {\n\
  el_typ = NameValue;\n\
  el_def = 0;\n\
  name = \"\";\n\
 };\n\
 NameValue @.active_membs[0] {\n\
  name = \"sub_proc_type\";\n\
  value = \"LeabraTrial\";\n\
 };\n\
};\n\
TypeDefault .projects[0].defaults[5] {\n\
 name = \"SE_Stat\";\n\
 default_type = SE_Stat;\n\
 active_membs  taBase_Group  {\n\
  el_typ = NameValue;\n\
  el_def = 0;\n\
  name = \"\";\n\
 };\n\
 NameValue @.active_membs[0] {\n\
  name = \"tolerance\";\n\
  value = \"0.5\";\n\
 };\n\
};\n\
TypeDefault .projects[0].defaults[6] {\n\
 name = \"SyncEpochProc\";\n\
 default_type = SyncEpochProc;\n\
 active_membs  taBase_Group  {\n\
  el_typ = NameValue;\n\
  el_def = 0;\n\
  name = \"\";\n\
 };\n\
 NameValue @.active_membs[0] {\n\
  name = \"sub_proc_type\";\n\
  value = \"LeabraTrial\";\n\
 };\n\
 NameValue @.active_membs[1] {\n\
  name = \"order\";\n\
  value = \"PERMUTED\";\n\
 };\n\
 NameValue @.active_membs[2] {\n\
  name = \"wt_update\";\n\
  value = \"ON_LINE\";\n\
 };\n\
 NameValue @.active_membs[3] {\n\
  name = \"second_proc_type\";\n\
  value = \"LeabraTrial\";\n\
 };\n\
};\n\
";

int main(int argc, char* argv[]) {
  pdpMisc::Init_Hook = ta_Init_leabra;
  pdpMisc::defaults_str = my_defaults;
  pdpMisc::def_wizard = &TA_LeabraWiz;
  return pdpMisc::Main(argc, argv);
}
