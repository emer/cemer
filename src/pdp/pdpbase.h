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

#include "pdpdeclare.h"
#include "tdgeometry.h"
#include "pdp_TA_type.h"


typedef  void (*init_proc_t)() ;	// initialization proc


class PDP_API InitProcRegistrar { // #IGNORE object used as a module static instance to register an init_proc
public:
  InitProcRegistrar(init_proc_t init_proc);
private:
  InitProcRegistrar(const InitProcRegistrar&);
  InitProcRegistrar& operator =(const InitProcRegistrar&);
};


// all of these functions are cached out in pdpshell.cc

class PDP_API pdpMisc {
  // #NO_TOKENS miscellaneous things for pdp
friend class InitProcRegistrar;
public:
  //note: this enum must be duplicated in Project
  enum ViewColors {		// indicies for view_colors
    TEXT,
    BACKGROUND,
    NETWORK,
    ENVIRONMENT, //note: also used for Events
    SCHED_PROC,
    STAT_GROUP,
    SUBPROC_GROUP,
    STAT_PROC,
    OTHER_PROC,
    PDPLOG,
    STAT_AGG,
    GEN_GROUP,
    INACTIVE,
    STOP_CRIT,
    AGG_STAT,
    CON_SPEC,
    UNIT_SPEC,
    PRJN_SPEC,
    LAYER_SPEC,
    WIZARD,
    COLOR_COUNT
  };

  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  static PDPRoot*	root;		// root of pdp class hierarchy
  static char*		defaults_str;
  // string representation of basic defaults compiled into executable
  static String_Array	proj_to_load;	// list of projects to load
  static taBase_List	post_load_opr;  // #HIDDEN objects that need to have operations performed on them after loading
  static TypeDef*	def_wizard; 	// default network wizard type to create (this should be set by the specific application Main function)
//  static void (*Init_Hook)();	 	// #READ_ONLY set this in user's main to init ta, etc.
  static float		pdpZScale; // amount by which to scale y dimension (inventor z) def is 4.0
  static float		pts_per_so_unit; // #DEF_36 equivalent font points per so unit
  static float		char_pts_per_so_unit; // #DEF_72 chars*pt size / so unit

  static taPtrList_impl* initHookList(); // need to use accessor to resolve module initialization ambiguity
  
  static int	Main(int argc, char* argv[]);
  // #IGNORE the pdp main startup function: call this from user's main
//NOTE: v4 waitprocs (aka event loop handling)
  static int	WaitProc();  		// waiting process
  static int	WaitProc_LoadProj(); // waiting process
  static int	WaitProc_PostLoadOpr(); // waiting process
  static void 	SaveRecoverFile(int err = 1);
  // error handling function that saves a recover file when system crashes
  static ColorScaleSpec* GetDefaultColor();  // gets the default color scale

  static const iColor* GetObjColor(Project* prj, ViewColors vc);
  // #IGNORE get default object color (for edit dialogs and project view)
  static const iColor* GetObjColor(Project* prj, TypeDef* typ);
  // get default object color (for edit dialogs and project view)


  static PDPLog* GetNewLog(Project* prj, TypeDef* typ);
  // get a temporary log of given type in given project (e.g., for pulling up a graph, etc)
  static NetConduit* GetNewConduit(Project* prj, TypeDef* typ = NULL);
  // get a temporary conduit of given type in given project
  static Network* GetNewNetwork(Project* prj, TypeDef* typ = NULL);
  // get a new network object
  static Network* GetDefNetwork(Project* prj);
  // get default network from project

  static BaseSpec_MGroup* FindMakeSpecGp(Project* prj, const char* nm, bool& nw_itm = nw_itm_def_arg);
  // find a given spec group and if not found, make it
  static BaseSpec* FindMakeSpec(Project* prj, const char* nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // find a given spec and if not found, make it
  static BaseSpec* FindSpecName(Project* prj, const char* nm);
  // find a given spec by name
  static BaseSpec* FindSpecType(Project* prj, TypeDef* td);
  // find a given spec by type

  static PDPLog* FindMakeLog(Project* prj, const char* nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // find a given log and if not found, make it
  static PDPLog*  FindLogName(Project* prj, const char* nm);
  // find a log of given name
  static PDPLog*  FindLogType(Project* prj, TypeDef* td);
  // find a log of given type

#ifdef TA_GUI
  static SelectEdit* FindSelectEdit(Project* prj);
  // find a select edit if it exists
  static SelectEdit* FindMakeSelectEdit(Project* prj);
  // find a select edit if it exists, otherwise make it
#endif
#ifdef DMEM_COMPILE
  static int 	DMem_SubEventLoop(); 	// #IGNORE for dmem sub-process (dmem_proc > 0), event-processing loop
  static int	DMem_WaitProc(bool send_stop_to_subs = false);
  // #IGNORE waiting process for dmem_proc = 0, if send_stop_to_subs, sends a stop command to sub procs so they bail out of sub loop
#endif
protected:
  static void	AddInitHook(init_proc_t init_proc); // #IGNORE called during module initialization, before main()
};

int get_unique_file_number(int st_no, const char* prefix, const char* suffix);

//////////////////////////////////////////////////
//	PosGroup/MGroup: groups with positions	//
//////////////////////////////////////////////////

/*nn
#ifdef USE_TEMPLATE_GROUPS

#define PosGroup_of(T)							      \
class T ## _Group : public taGroup<T> {				      \
public:									      \
  TDCoord	pos;		 	     		      \
  void	Initialize() 		{ };					      \
  void 	Destroy()		{ };					      \
  void	InitLinks()		{ taGroup<T>::InitLinks(); taBase::Own(pos,this); }\
  void  Copy_(const T ## _Group& cp)		{ pos = cp.pos; }		      \
  void  Copy(const T ## _Group& cp)		{ taGroup<T>::Copy(cp); Copy_(cp); } \
  TA_BASEFUNS(T ## _Group);						      \
}

#define PosMGroup_of(T)							      	\
class T ## _MGroup : public MenuGroup<T> {				      	\
public:									      	\
  TDCoord	pos;			     			\
  void	Initialize() 		{ };					      	\
  void 	Destroy()		{ };					      	\
  void	InitLinks()		{ MenuGroup<T>::InitLinks(); taBase::Own(pos,this); }\
  void  Copy_(const T ## _MGroup& cp)	{ pos = cp.pos; }		        	\
  void  Copy(const T ## _MGroup& cp)	{ MenuGroup<T>::Copy(cp); Copy_(cp); } 		\
  TA_BASEFUNS(T ## _MGroup);						      	\
}

#else // USE_TEMPLATE_GROUPS
*/
class PDP_API PosGroup : public taBase_Group { // ##NO_TOKENS
public:
  TDCoord	pos;		// Position of Group

  void	Initialize() 		{ };
  void 	Destroy()		{ };
  void	InitLinks()		{ taBase_Group::InitLinks(); taBase::Own(pos,this); }
  void  Copy_(const PosGroup& cp)	{ pos = cp.pos; }
  COPY_FUNS(PosGroup, taBase_Group);
  TA_BASEFUNS(PosGroup);
};

#define PosGroup_of(T)							      \
class PDP_API T ## _Group : public PosGroup {					      \
public:									      \
  void	Initialize() 		{ SetBaseType(&TA_ ## T); }		      \
  void 	Destroy()		{ };					      \
  TA_BASEFUNS(T ## _Group);						      \
}

#define PosMGroup_of(T)							      	\
class PDP_API T ## _MGroup : public taGroup<T> {				      	\
public:									      	\
  TDCoord	pos;			     			\
  void	Initialize() 		{ }					      	\
  void 	Destroy()		{ }					      	\
  void	InitLinks()		{ taGroup<T>::InitLinks(); taBase::Own(pos,this); }\
  void  Copy_(const T ## _MGroup& cp)	{ pos = cp.pos; }		        	\
  void  Copy(const T ## _MGroup& cp)	{ taGroup<T>::Copy(cp); Copy_(cp); } 		\
  TA_BASEFUNS(T ## _MGroup);						      	\
}


//#endif // USE_TEMPLATE_GROUPS


#endif // base_h

