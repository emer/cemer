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



#ifndef process_h
#define process_h

#include "ta_script.h"
#include "tarandom.h"
#include "minmax.h"
#include "datatable.h"
#include "pdpbase.h"

//#include "igeometry.h"

class ProcessDialog;	// #IGNORE

#ifdef TA_GUI
class CtrlPanelData : public taOBase {
  // ##NO_TOKENS #INLINE data for the control panel
public:
  ProcessDialog* ctrl_panel;	// #IGNORE pointer to current ctrl panel
  bool		active;		// is panel active (save this state)
  float		lft;		// panel window left coord
  float		top;		// panel window top coord

  virtual void	GetPanel();	// get pointer to current dialog for owner proc
  virtual void	GetWinPos();	// get current window position
  virtual void	ScriptWinPos(ostream& strm = cout); // script current window position
  virtual void	SetWinPos();	// set window position from lft, bot
  virtual void	Place(float left = 0.0f, float top = 0.0f); // place window in given position
  virtual void	Revert();	// revert (update) the display

  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy()		{ };
  void 	Copy_(const CtrlPanelData& cp);
  COPY_FUNS(CtrlPanelData, taOBase);
  TA_BASEFUNS(CtrlPanelData);
};
#endif

class Process : public taNBase, public ScriptBase {
  // ##EXT_proc simple processes for controlling and coordinating execution
public:
  enum Type {
    C_CODE,			// C code (builtin)
    SCRIPT 			// Script (user-defined)
  };

  TypeDef* 	min_network;	// #HIDDEN #NO_SAVE #TYPE_Network Minimum acceptable Network type
  TypeDef* 	min_layer;	// #HIDDEN #NO_SAVE #TYPE_Layer Minimum acceptable Layer type
  TypeDef* 	min_unit;	// #HIDDEN #NO_SAVE #TYPE_Unit Minimum acceptable Unit type
  TypeDef* 	min_con_group; 	// #HIDDEN #NO_SAVE #TYPE_Con_Group Min acceptable Con_Group type
  TypeDef* 	min_con; 	// #HIDDEN #NO_SAVE #TYPE_Connection Min acceptable Con type

  RndSeed	rndm_seed;	// #HIDDEN random seed, for NewRun(), ReRun()
  TimeUsed	time_used;	// #HIDDEN accumulated time used during the Run() of this process
  LogData	log_data;	// #HIDDEN #NO_SAVE log data object

  Type    	type;			// process can be builtin c-code or a script
  Modulo	mod;			// flag=run this process or not, m=modulus, run process every m times, off=start runing at this offset
  Project*	project;  		// #READ_ONLY #NO_SAVE project to act on
  Network*	network;  		// #CONTROL_PANEL network to act on
  Environment*  environment;		// #CONTROL_PANEL environmnent to act in
#ifdef TA_GUI
  CtrlPanelData	ctrl_panel;		// #HIDDEN data for the control panel display
#endif
  virtual void	NewInit();
  // #BUTTON #GHOST_OFF_running initialize the process using new random seed
  virtual void	ReInit();
  // #BUTTON #GHOST_OFF_running initialize the process using old random seed
  virtual void	Init();
  // actually perform the initialization
  virtual void	Run();
  // run the process: this can be called by any kind of code to run the process, but does not have support for interactive stopping
  virtual void	Run_gui();
  // #BUTTON #LABEL_Run #NO_SCRIPT run the process in an interactive fashion: can be stopped and sets flags for only running one process

  virtual void	NewSeed();
  // get a new random seed (i.e., rndm_seed.NewSeed(), but is also DMEM safe)
  virtual void	OldSeed();
  // get the old random seed (i.e., rndm_seed.OldSeed(), but is also DMEM safe)
#ifdef DMEM_COMPILE
  virtual void	DMem_SyncSameNetSeeds();
  // #IGNORE synchronize random seeds across dmem processors that operate on the same network: these must always have same seed!
  virtual void	DMem_SyncAllSeeds();
  // #IGNORE synchronize random seeds across all dmem processors
#endif

#ifdef TA_GUI
  virtual void	ControlPanel(float left = 0.0f, float top = 0.0f);
  // #MENU #ARGC_0 brings up a small control dialog for running the process (in given location)
#endif
  virtual LogData& GenLogData(LogData* ld=NULL); // #IGNORE generate log data

  virtual void	C_Code() 	{ };	// this is the actual stuff to do in C++

  // validate a given type using Min_ types (and all below it)
  virtual void	CheckResetCache(); // reset the cache of types already checked
  virtual bool	CheckNetwork();
  // validate all objects in network as being of the correct type
  virtual bool	CheckLayer(Layer* ck);		// #IGNORE
  virtual bool	CheckUnit(Unit* ck);		// #IGNORE
  virtual bool	CheckConGroup(Con_Group* ck);	// #IGNORE
  virtual void	CheckError(TAPtr ck, TypeDef* td); // #IGNORE report type check error

  // stuff for script_base
  TypeDef*	GetThisTypeDef()	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }
  void		LoadScript(const char* file_nm = NULL);
  bool		RunScript();

  virtual void	CopyPNEPtrs(Network* net, Environment* env);
  // #IGNORE copy the project, network, env ptrs from
  virtual void	SetDefaultPNEPtrs();
  // #IGNORE get default project, network, env ptrs
  virtual void	SetEnv(Environment* env); // set environment to given environment
  virtual void	SetNet(Network* net); // set network to given network

  // the following are to facilitate script programming
  virtual SchedProcess* GetMySchedProc(); // gets the schedule process that owns me
  virtual SchedProcess* GetMySProcOfType(TypeDef* proc_type); // get the schedule process of given type for my process hierarchy
  virtual TrialProcess* GetMyTrialProc(); // gets the trial process for my process hierarchy
  virtual EpochProcess* GetMyEpochProc(); // gets the epoch process for my process hierarchy
  virtual Event* GetMyCurEvent();  // get the current event being processed
  virtual Event_MGroup* GetMyCurEventGp();  // get the current event group being processed
  virtual Event* GetMyNextEvent();  // get the next event to be processed (e.g., in case you want to alter it)
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Process); }
#endif
  void	UpdateAfterEdit();
  void 	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const Process& cp);
  COPY_FUNS(Process, taNBase);
  TA_BASEFUNS(Process);
};

class Process_Group : public taBase_Group {
  // ##NO_TOKENS a regular group of processes
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

  bool		Close_Child(TAPtr obj);

  virtual Process* FindMakeProc(const char* nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // find a process of given type and name -- if not found, make it
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Process); }
#endif
  void	Initialize() 		{ SetBaseType(&TA_Process); }
  void 	Destroy()		{ };
  TA_BASEFUNS(Process_Group);
};

// assumes no USE_TEMPLATE_GROUPS

class Process_MGroup : public taGroup<Process> {
  // ##NO_TOKENS a menu group for processes
public:
  static bool nw_itm_def_arg;	// #IGNORE default arg val for FindMake..

//obs  void		GetAllWinPos();	// get window positions for control panels
//obs  void		ScriptAllWinPos(); // script window positions for control panels

  virtual int	ReplaceEnvPtrs(Environment* old_ev, Environment* new_ev);
  // replace environment pointers in all processes with new environment pointer
  virtual int	ReplaceNetPtrs(Network* old_net, Network* new_net);
  // replace network pointers in all processes with new network pointer

  virtual Process* FindMakeProc(const char* nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg);
  // find a process of given type and name -- if not found, make it

  // support special structure of sched procs here
  bool		Close_Child(TAPtr obj);
  bool		DuplicateEl(TAPtr obj);
#ifdef TA_GUI
  const iColor* GetEditColor() { return pdpMisc::GetObjColor(GET_MY_OWNER(Project),&TA_Process); }
#endif
  void	Initialize();
  void 	Destroy()		{ };
  TA_BASEFUNS(Process_MGroup);
#ifdef TA_GUI
public:
  virtual void	ControlPanel_mc(taiMenuEl* sel);
#endif
};


#endif // process_h
