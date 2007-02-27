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

// ta_project.h

#ifndef TA_PROJECT_H
#define TA_PROJECT_H

#include "ta_group.h"
#include "ta_defaults.h"
#include "ta_datatable.h"
#include "ta_viewspec.h"
#include "ta_viewer.h"
#include "ta_program.h"
#include "ta_plugin_p.h"
#include "ta_qtclipdata.h"

#ifdef TA_GUI
  #include "ta_seledit.h"
#endif

class taProject;

class TA_API taWizard : public taNBase {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 ##CAT_Wizard wizard for automating construction of simulation objects
INHERITED(taNBase)
public:
  bool		auto_open;	// open this wizard upon startup

  override String 	GetTypeDecoKey() const { return "Wizard"; }

  void 	InitLinks();
  void	CutLinks(); 
  SIMPLE_COPY(taWizard);
  COPY_FUNS(taWizard, inherited);
  TA_BASEFUNS(taWizard);
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

class TA_API Wizard_Group : public taGroup<taWizard> {
  // ##CAT_Wizard group of wizard objects
INHERITED(taGroup<taWizard>)
public:
  virtual void	AutoEdit();

  override String 	GetTypeDecoKey() const { return "Wizard"; }

  void	Initialize() 		{ SetBaseType(&TA_taWizard); }
  void 	Destroy()		{ };
  TA_BASEFUNS(Wizard_Group);
};

class TA_API SelectEdit_Group : public taGroup<SelectEdit> {
  // ##CAT_Display group of select edit dialog objects
INHERITED(taGroup<SelectEdit>)
public:
  virtual void	AutoEdit();

  override String 	GetTypeDecoKey() const { return "SelectEdit"; }

  void	Initialize() 		{ SetBaseType(&TA_SelectEdit); }
  void 	Destroy()		{ };
  TA_BASEFUNS(SelectEdit_Group);
};

class TA_API taProject : public taFBase {
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #VIRT_BASE ##DEF_NAME_ROOT_Project ##CAT_Project Base class for a project object containing all relevant info for a given instance -- all ta GUI-based systems should have one..
INHERITED(taFBase)
public:
  taBase_Group		templates; // #HIDDEN templates for new objects -- copy new objects from here
  Wizard_Group    	wizards; // Wizards for automatically configuring simulation objects
  SelectEdit_Group	edits;	// special edit dialogs for selected elements
  DataTable_Group	data;	// data, such as patterns for network input
  taBase_Group		data_proc; // objects that perform data processing operations (functions collected on objects for different kinds of operations)
  Program_Group		programs; // Gui-based programs to run simulations and other processing
  DataViewer_List	viewers; // #EXPERT viewer objects: contains configuration information (though this is typically more easily accessed through the view control panel)

  bool			m_dirty; // #HIDDEN #READ_ONLY #NO_SAVE
  bool			m_no_save; // #HIDDEN #READ_ONLY #NO_SAVE -- flag to prevent double user query on exiting; cleared when undirtying
  bool			use_sim_log; 	// record project changes in the SimLog file

  override bool		isDirty() const {return m_dirty;}
  override void 	setDirty(bool value); 

  MainWindowViewer*	GetDefaultProjectBrowser(); // gets one if there is, else NULL
  virtual void 		AssertDefaultProjectBrowser(bool auto_opn); 
    // make the default project browser is made, and optionally open it
  virtual void 		AssertDefaultWiz(bool auto_opn) {} 
    // make sure the default wizard(s) are made, and optionally open them
  virtual void		OpenNewProjectBrowser(String proj_browser_name = "(default name)");
    // #MENU #MENU_ON_View #MENU_CONTEXT

  virtual DataTable*	GetNewAnalysisDataTable(const String& nw_nm="", bool msg=false);
  // createa a new data table in data.AnalysisData (used for various data processing and graphing functions).  nw_nm = name for new table, msg = issue a warning message about the creation of this table
  virtual taBase*	FindMakeNewDataProc(TypeDef* typ, const String& nm);
  // find existing data processing object of given type, or else make one and give it nm

  virtual MainWindowViewer* NewProjectBrowser(); 
    // create a new, empty viewer -- note: window not opened yet
  virtual void	UpdateSimLog();
  // #MENU update simulation log (SimLog) for this project, storing the name of the project and the description as entered here.  click off use_simlog if you are not using this feature

  override bool		SetFileName(const String& val);
  override int 		Save_strm(ostream& strm, TAPtr par=NULL, int indent=0);
  override int	 	Load_strm(istream& strm, TAPtr par=NULL, taBase** loaded_obj_ptr = NULL);

  virtual void		PostLoadAutos();
  // perform post-loading automatic functions

  void	UpdateAfterEdit();
  virtual void		InitLinks_impl(); // #IGNORE use this instead of InitLinks in subclasses
  virtual void		CutLinks_impl(); // #IGNORE use this instead of CutLinks in subclasses -- you can call this first to nuke the viewers etc. before your own stuff
  void	Copy_(const taProject& cp);
  COPY_FUNS(taProject, taFBase);
  TA_BASEFUNS(taProject);
  
protected:
  virtual void 		InitLinks_post(); // #IGNORE called after all _impls: does LoadDefaults and launches wiz
  void 	CutLinks(); // don't override this -- use _impl instead
  virtual MainWindowViewer* MakeProjectBrowser_impl(); // make a standard viewer for this project type
  
private:
  void 	InitLinks(); // don't try to use this -- use _impl instead
  void	Initialize();
  void 	Destroy() { CutLinks(); }
};


class TA_API Project_Group : public taGroup<taProject> {
  //  ##CAT_Project group of projects
INHERITED(taGroup<taProject>)
friend class taProject;
public:
  override int	 	Load_strm(istream& strm, TAPtr par=NULL, taBase** loaded_obj_ptr = NULL);

  void	Initialize() 		{SetBaseType(&TA_taBase);} // upclassed in pdp
  void 	Destroy()		{ };
  TA_BASEFUNS(Project_Group);
};


class TA_API taRootBase: public taFBase {
  // ##CAT_Project base class for the root of the structural hierarchy (root. or . in css / paths)
INHERITED(taFBase)
public:
  static TypeDef*	root_type; // set in Startup_Main
  static taRootBase*	instance();
  
  String		version; 	// #READ_ONLY #SHOW current version number
  taBase_List		templates;	// #NO_SAVE #READ_ONLY objects used as templates -- do not use or mess with these!
  Project_Group		projects; 	// #NO_SAVE The projects
  DataViewer_List	viewers;	// #NO_SAVE global viewers (not saved)
  taPlugin_List		plugins; //  available plugins
  taPluginBase_List	plugin_deps; // #SHOW_TREE EXPERT_TREE #NO_SAVE  dynamic list, populated in presave
  taiMimeFactory_List	mime_factories; // #NO_SAVE extensible list of mime factories
  String_Array		recent_files; // #NO_SHOW recently loaded files
  String_Array		recent_paths; // #NO_SHOW recently used paths
  
  
  SArg_Array		last_dirs; // #NO_SHOW #NO_SAVE #OBS obsolete, nuke
  
  virtual void  Options();
  // edit global settings/parameters (taMisc)
  virtual void	About();
  // #MENU get information/copyright notice
  virtual void	SaveAll();
  // saves all the projects
  
  void		AddRecentFile(const String& value); // #IGNORE add this file to the recent list (also adds the path to recent paths)
  void		AddRecentPath(const String& value); // #IGNORE add this path to the recent list

#ifdef GPROF			// turn on for profiling
  virtual void  MonControl(bool on);
  // #MENU set profile monitoring: on=true = on, else off.  starts out off..
#endif
  
  taBase*	GetTemplateInstance(TypeDef* typ);
  // get an instance of the indicated tab type, or NULL if not found
  
  ///////////////////////////////////////////////////////////////////////////////
  //		Startup Code	(in order of calling by Startup_Main)

  static bool	Startup_Main(int& argc, const char* argv[], ta_void_fun ta_init_fun = NULL,
			     TypeDef* root_typ = &TA_taRootBase);
  // #IGNORE this is the main function call to startup the software -- creates a root object of given type, processes args, sets global state vars, starts up css, and opens a browser window on the root object if in gui mode

  static bool	Startup_InitApp(int& argc, const char* argv[]);
  // #IGNORE init application stuff (qapp etc)
  static bool	Startup_InitTA(ta_void_fun ta_init_fun);
  // #IGNORE basic type-access system intializaton
  static bool	Startup_InitArgs(int& argc, const char* argv[]);
  // #IGNORE process args into more usable form
  static bool	Startup_InitDMem(int& argc, const char* argv[]);
  // #IGNORE init distributed memory (MPI) stuff
  static bool	Startup_ProcessGuiArg();
  // #IGNORE process the -gui/-nogui arg
  static bool	Startup_EnumeratePlugins();
  // #IGNORE enumeration of plugins 
  static bool	Startup_LoadPlugins();
  // #IGNORE final initialize of plugins 
  static bool	Startup_InitTypes();
  // #IGNORE final init of typedefs
  static bool	Startup_InitCss();
  // #IGNORE initialize css script system
  static bool	Startup_InitGui();
  // #IGNORE initialize gui system
  static bool	Startup_InitViewColors();
  // #IGNORE initialize default view colors
  static bool	Startup_MakeMainWin();
  // #IGNORE open the main window (browser of root object) (returns success)
  static bool	Startup_Console();
  // #IGNORE start the console shell (returns success)
  static bool	Startup_ProcessArgs();
  // #IGNORE process general args

  static bool	Startup_Run();
  // #IGNORE go ahead and run the main event loop
  static bool	Cleanup_Main();
  // #IGNORE after running, do final cleanups (called by Startup_Run)

#ifdef DMEM_COMPILE
  static bool 	Run_GuiDMem();
  // #IGNORE run the gui under dmem: requires special code.. 
  static int 	DMem_SubEventLoop();
  // #IGNORE for dmem sub-process (dmem_proc > 0), event-processing loop
  static void	DMem_WaitProc(bool send_stop_to_subs = false);
  // #IGNORE waiting process for dmem_proc = 0, if send_stop_to_subs, sends a stop command to sub procs so they bail out of sub loop
#endif
  	
  bool		CheckAddPluginDep(TypeDef* td); // add a plugin dependency, if this type is a  type defined in a plugin; true if it was
  bool		VerifyHasPlugins(); // check the current plugin_deps w/ loaded plugins, return true if all needed plugins loaded OR user says to continue loading anyway
  
  int	Save();
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(taRootBase)
protected:
  bool		AddRecentFile_impl(const String& value); // #IGNORE add this file to the recent list (also adds the path to recent paths)
  bool		AddRecentPath_impl(const String& value); // #IGNORE add this path to the recent list;
  virtual void		AddTemplates(); // called in InitLinks -- extend to add new templates
  virtual taBase* 	GetTemplateInstance_impl(TypeDef* typ, taBase* base);
private:
  void	Initialize();
  void	Destroy();
};



#endif
