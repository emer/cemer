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



#ifndef base_h
#define base_h

// everything includes base.h, this is the minimal include

#include "ta_group.h"

#ifdef TA_GUI
//#include "xform.h"
#endif

#include "ta_geometry.h"
#include "pdpdeclare.h"
#include "pdp_TA_type.h"

// all of these functions are cached out in pdp_project.cc

class PDP_API pdpMisc { 
  // #NO_TOKENS miscellaneous things for pdp, extension of basic tabMisc app stuff
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  static PDPRoot*	root;		// root of pdp class hierarchy
  static String		user_spec_def;	// provided by user using -d option
  static float		pdpZScale; // amount by which to scale y dimension (inventor z) def is 4.0
  
  static int	Main(int argc, const char* argv[]);
  // #IGNORE the pdp main startup function: call this from user's main
//NOTE: v4 waitprocs (aka event loop handling)
  static void	WaitProc();  		// waiting process
  static void 	SaveRecoverFile(int err = 1);
  // error handling function that saves a recover file when system crashes
  static ColorScaleSpec* GetDefaultColor();  // gets the default color scale

  static Network* GetNewNetwork(ProjectBase* prj, TypeDef* typ = NULL);
  // get a new network object
  static Network* GetDefNetwork(ProjectBase* prj);
  // get default network from project

#ifdef TA_GUI
  static SelectEdit* FindSelectEdit(ProjectBase* prj);
  // find a select edit if it exists
  static SelectEdit* FindMakeSelectEdit(ProjectBase* prj);
  // find a select edit if it exists, otherwise make it
#endif

  // todo: move these guys to ta_project
#ifdef DMEM_COMPILE
  static int 	DMem_SubEventLoop(); 	// #IGNORE for dmem sub-process (dmem_proc > 0), event-processing loop
  static int	DMem_WaitProc(bool send_stop_to_subs = false);
  // #IGNORE waiting process for dmem_proc = 0, if send_stop_to_subs, sends a stop command to sub procs so they bail out of sub loop
#endif
};

int get_unique_file_number(int st_no, const char* prefix, const char* suffix);

#endif // base_h

