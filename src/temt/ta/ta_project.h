// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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
#include "colorscale.h"
#include "ta_engine.h"
#include "ta_seledit.h"

class taDoc;
class taWizard;
class taProject;
class taRootBase;

class TA_API taDoc : public taNBase {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 ##CAT_Docs document for providing information on projects and other objects
INHERITED(taNBase)
public:
  bool			auto_open;	// open this document upon startup
  //note: a specialized taEdit is used to show this guy
  String		text; // #NO_SHOW the text of the document (in html/mediawiki format)
  String		html_text; // #READ_ONLY #HIDDEN #NO_SAVE #EDIT_DIALOG wiki conversion of html text -- use this for actual display

  static String		WikiParse(const String& in_str);
  // convert very basic wiki syntax to html format -- == headers ==, * bulleted lists, [[ ]] links, etc

  virtual void		UpdateText();
  // update the html_text from the user-entered text with wiki syntax by calling WikiParse

  override String 	GetTypeDecoKey() const { return "Doc"; }

  TA_BASEFUNS(taDoc);
protected:
  override void		UpdateAfterEdit_impl();
  static const String	init_text; // ##IGNORE initial text

private:
  SIMPLE_COPY(taDoc);
  void 	Initialize();
  void 	Destroy() { }
};
TA_SMART_PTRS(taDoc);

class TA_API UserData_DocLink: public UserDataItemBase {
  // a link to a doc -- when added to the userdata of an obj, will auto display the doc
INHERITED(UserDataItemBase)
public:
  taDocRef		doc; // the doc
  
  override void		SmartRef_DataDestroying(taSmartRef* ref, taBase* obj);
  TA_BASEFUNS(UserData_DocLink)
private:
  void Copy_(const UserData_DocLink& cp){doc = cp.doc;}
  void Initialize();
  void Destroy() {}
};

class TA_API Doc_Group : public taGroup<taDoc> {
  // ##CAT_Docs group of doc objects
INHERITED(taGroup<taDoc>)
public:
  virtual void		AutoEdit();

  override String 	GetTypeDecoKey() const { return "Doc"; }
  TA_BASEFUNS(Doc_Group);
private:
  NOCOPY(Doc_Group)
  void	Initialize() 		{ SetBaseType(&TA_taDoc); }
  void 	Destroy()		{ };
};

class TA_API taWizard : public taNBase {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 ##CAT_Wizard wizard for automating construction of simulation objects
INHERITED(taNBase)
public:
  bool		auto_open;	// open this wizard upon startup

  override String 	GetTypeDecoKey() const { return "Wizard"; }
  TA_BASEFUNS(taWizard);
private:
  SIMPLE_COPY(taWizard);
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

class TA_API Wizard_Group : public taGroup<taWizard> {
  // ##CAT_Wizard group of wizard objects
INHERITED(taGroup<taWizard>)
public:
  virtual void		AutoEdit();

  override String 	GetTypeDecoKey() const { return "Wizard"; }

  TA_BASEFUNS(Wizard_Group);
private:
  NOCOPY(Wizard_Group)
  void	Initialize() 		{ SetBaseType(&TA_taWizard); }
  void 	Destroy()		{ };
};

class TA_API SelectEdit_Group : public taGroup<SelectEdit> {
  // ##CAT_Display group of select edit dialog objects
INHERITED(taGroup<SelectEdit>)
public:
  virtual void	AutoEdit();

  override String 	GetTypeDecoKey() const { return "SelectEdit"; }

  TA_BASEFUNS(SelectEdit_Group);
private:
  NOCOPY(SelectEdit_Group)
  void	Initialize() { SetBaseType(&TA_SelectEdit);}
  void 	Destroy()		{ };
};

class TA_API taProject : public taFBase {
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #VIRT_BASE ##DUMP_LOAD_POST ##DEF_NAME_ROOT_Project ##CAT_Project Base class for a project object containing all relevant info for a given instance -- all ta GUI-based systems should have one..
INHERITED(taFBase)
public:
  String 		tags;	   // #EDIT_DIALOG list of comma separated tags that indicate the basic function of this project -- should be listed in hierarchical order, with most important/general tags first -- these are used for searching the online project library if this project is uploaded
  taBase_Group		templates; // #HIDDEN templates for new objects -- copy new objects from here
  Doc_Group		docs; // documents, typically linked to other objects
  Wizard_Group    	wizards; // Wizards for automatically configuring simulation objects
  SelectEdit_Group	edits;	// special edit dialogs for selected elements
  DataTable_Group	data;	// data, such as patterns for network input
  taBase_Group		data_proc; // objects that perform data processing operations (functions collected on objects for different kinds of operations)
  Program_Group		programs; // Gui-based programs to run simulations and other processing
  DataViewer_List	viewers; // a list of the viewers that have been made and saved in the project; choose one, right click, and OpenViewer to view if not open

  bool			m_dirty; // #HIDDEN #READ_ONLY #NO_SAVE
  bool			m_no_save; // #HIDDEN #READ_ONLY #NO_SAVE -- flag to prevent double user query on exiting; cleared when undirtying
  bool			use_change_log;  // #AKA_use_sim_log record project changes in a ChangeLog docs item -- you will be prompted whenver the project is saved to a different name, and can always use the UpdateChangeLog button to add a new entry prior to saving
  String		last_change_desc; // #EXPERT description of the last change made to the project -- used for change log

  override bool		isDirty() const {return m_dirty;}
  override void 	setDirty(bool value); 

  MainWindowViewer*	GetDefaultProjectBrowser(); // gets one if there is, else NULL
  MainWindowViewer*	GetDefaultProjectViewer(); // gets one if there is, else NULL
  virtual MainWindowViewer* AssertDefaultProjectBrowser(bool auto_opn); 
    // make sure the default project browser is made, and optionally open it
  virtual void 		AssertDefaultWiz(bool auto_opn) {} 
    // make sure the default wizard(s) are made, and optionally open them
  virtual void		OpenNewProjectBrowser(String proj_browser_name = "(default name)");
    // #MENU #MENU_ON_View #MENU_CONTEXT open a new browser, either 3-pane or 2-2-pane (tree + viewer)
  virtual void		OpenNewProjectViewer(String proj_browser_name = "(default name)");
    // #MENU #MENU_ON_View #MENU_CONTEXT open a new 3D viewer (with panels)


  virtual DataTable*	GetNewInputDataTable(const String& nw_nm="", bool msg=false);
  // createa a new data table in data.InputData (used for data generation functions).  nw_nm = name for new table, msg = issue a warning message about the creation of this table
  virtual DataTable*	GetNewOutputDataTable(const String& nw_nm="", bool msg=false);
  // createa a new data table in data.OutputData (used for monitoring and logging functions).  nw_nm = name for new table, msg = issue a warning message about the creation of this table
  virtual DataTable*	GetNewAnalysisDataTable(const String& nw_nm="", bool msg=false);
  // createa a new data table in data.AnalysisData (used for various data processing and graphing functions).  nw_nm = name for new table, msg = issue a warning message about the creation of this table
  virtual taBase*	FindMakeNewDataProc(TypeDef* typ, const String& nm);
  // find existing data processing object of given type, or else make one and give it nm
  virtual SelectEdit* FindMakeSelectEdit(const String& seledit_name);
  // get select edit object of given name, or make one if not found

  virtual MainWindowViewer* NewProjectBrowser(); 
    // create a new, empty viewer -- note: window not opened yet
  virtual void		UpdateChangeLog();
  // #BUTTON update change log for this project, stored as a ChangeLog item in docs on the project -- you will be prompted to enter a description of recent changes, and the date, user, and file names will be recorded

  virtual void		SaveRecoverFile();
  // Save a recover file of this project, usually called when a signal is received indicating a crash condition
  virtual void		SaveRecoverFile_strm(ostream& strm) { Save_strm(strm); }
  // #IGNORE underlying save function to use when saving a recover file -- might want to do something special here

  override bool		SetFileName(const String& val);
  override int 		Save_strm(ostream& strm, TAPtr par=NULL, int indent=0);

  override void		Dump_Load_pre();
  override void		PostLoadAutos();
  // perform post-loading automatic functions
  void			OpenViewers(); // open any yet unopen viewers

  void	UpdateAfterEdit();
  virtual void		InitLinks_impl(); // #IGNORE use this instead of InitLinks in subclasses
  virtual void		CutLinks_impl(); // #IGNORE use this instead of CutLinks in subclasses -- you can call this first to nuke the viewers etc. before your own stuff
  TA_BASEFUNS(taProject);
  
protected:
  virtual void 		InitLinks_post(); // #IGNORE called after all _impls: does LoadDefaults and launches wiz
  void 	CutLinks(); // don't override this -- use _impl instead
  virtual MainWindowViewer* MakeProjectBrowser_impl(); // make a standard viewer for this project type
  override int		GetOwnerEditableState_impl(int mask) const
    {return 0;} // the readonly stops here!
  
private:
  void	Copy_(const taProject& cp);
  void 	InitLinks(); // don't try to use this -- use _impl instead
  void	Initialize();
  void 	Destroy();
};
TA_SMART_PTRS(taProject)

class TA_API Project_Group : public taGroup<taProject> {
  //  ##CAT_Project group of projects
INHERITED(taGroup<taProject>)
friend class taProject;
public:
  override int	 	Load_strm(istream& strm, TAPtr par=NULL, taBase** loaded_obj_ptr = NULL);
  override int	 	Load(const String& fname="", taBase** loaded_obj_ptr = NULL);

  TA_BASEFUNS(Project_Group);
private:
  NOCOPY(Project_Group)
  void	Initialize() 		{SetBaseType(&TA_taProject);} // upclassed in pdp
  void 	Destroy()		{ };
};


class TA_API taRootBaseAdapter: public QObject {
  // ##IGNORE QObject for dispatching startup routines in event loop
INHERITED(QObject)
friend class taRootBase;
  Q_OBJECT
public:
  taRootBaseAdapter(): QObject(NULL) {}
  ~taRootBaseAdapter() {}
  
protected slots:
  void 	Startup_ProcessArgs();
  void 	Startup_RunStartupScript();
#ifdef DMEM_COMPILE
  void 	DMem_SubEventLoop();
#endif
};


class TA_API taRootBase: public taFBase {
  // ##CAT_Project base class for the root of the structural hierarchy (root. or . in css / paths)
INHERITED(taFBase)
public:
  static TypeDef*	root_type; // set in Startup_Main
  static taRootBase*	instance();
  
  String		version; 	// #READ_ONLY #SHOW current version number
  taBase_List		templates;	// #NO_SAVE #READ_ONLY objects used as templates -- do not use or mess with these!
  Wizard_Group		wizards; // #NO_SAVE global wizards -- see each project for project-specific wizards
  Project_Group		projects; 	// #NO_SAVE The projects
  DataViewer_List	viewers;	// #NO_SAVE global viewers (not saved)
  taPlugin_List		plugins; //  available plugins
  taBase_List		plugin_state; // #NO_SAVE #HIDDEN #HIDDEN_TREE state objs of plugins -- created/managed by plugin system; state saved as {name}.state in user data
  taPluginBase_List	plugin_deps; // #SHOW_TREE #EXPERT_TREE #NO_SAVE  dynamic list, populated in presave
  taiMimeFactory_List	mime_factories; // #NO_SAVE #HIDDEN_TREE extensible list of mime factories
  ColorScaleSpec_Group 	colorspecs;	// Color Specs
  taEngine_Group	engines; // #NO_SAVE calculation engines (highly machine-specific; some are added by plugins)
  taBase_List		objs;  // #SHOW_TREE #EXPERT_TREE #NO_SAVE misc place for app-global objs, usually for system use, ex. tcp server
  String_Array		recent_files; // #NO_SHOW recently loaded files
  String_Array		recent_paths; // #NO_SHOW recently used paths
#ifdef DEBUG
  taBase_Group		test_group; // #EXPERT #NO_SAVE #LINK_GROUP group for testing clip ops etc.
#endif
  
  void		OpenRemoteServer(ushort port = 5360);
  // #MENU #MENU_ON_Server Open a Server for remote TCP-based control of this application
  void		CloseRemoteServer();
  // #MENU #MENU_ON_Server Close an open Server, if any
  
  virtual void  Options();
  // edit global settings/parameters (taMisc)
  virtual void	About();
  // #MENU #MENU_ON_Object get information/copyright notice
  virtual void	SaveAll();
  // saves all the projects
  
  void		AddRecentFile(const String& value, bool no_save = false); // #IGNORE add this file to the recent list (also adds the path to recent paths)
  void		AddRecentPath(const String& value, bool no_save = false); // #IGNORE add this path to the recent list
  
  taBase*	FindGlobalObject(TypeDef* base_type = &TA_taBase, 
    const String& name = _nilString);
    // find an object deriving from base_type, with given name, or any name if blank

  virtual void  MonControl(bool on);
  // #MENU #MENU_ON_Object set profile monitoring: on=true = on, else off.  starts out off..
  
  taBase*	GetTemplateInstance(TypeDef* typ);
  // get an instance of the indicated tab type, or NULL if not found
  
  ///////////////////////////////////////////////////////////////////////////////
  //		Startup Code	(in order of calling by Startup_Main)

  static bool	Startup_Main(int& argc, const char* argv[], ta_void_fun ta_init_fun = NULL,
			     TypeDef* root_typ = &TA_taRootBase);
  // #IGNORE this is the main function call to startup the software -- creates a root object of given type, processes args, sets global state vars, starts up css, and opens a browser window on the root object if in gui mode

  static bool	Startup_InitDMem(int& argc, const char* argv[]);
  // #IGNORE init distributed memory (MPI) stuff
  static bool	Startup_ProcessGuiArg(int argc, const char* argv[]);
  // #IGNORE process the -gui/-nogui arg
  static bool	Startup_InitApp(int& argc, const char* argv[]);
  // #IGNORE init application stuff (qapp etc) -- note: twiddles cmdline args
  static bool	Startup_InitArgs(int& argc, const char* argv[]);
  // #IGNORE process args into more usable form
  static bool	Startup_InitTA(ta_void_fun ta_init_fun);
  // #IGNORE basic type-access system intializaton
  static bool	Startup_InitTypes();
  // #IGNORE final init of typedefs
  static bool	Startup_EnumeratePlugins();
  // #IGNORE enumeration of plugins 
  static bool	Startup_LoadPlugins();
  // #IGNORE final initialize of plugins 
  static bool	Startup_EnumerateEngines();
  // #IGNORE make a list of all available engines, both native and plugins 
  static bool	Startup_InitCss();
  // #IGNORE initialize css script system
  static bool	Startup_InitGui();
  // #IGNORE initialize gui system
  static bool	Startup_InitViewColors();
  // #IGNORE initialize default view colors
  static bool	Startup_ConsoleType();
  // #IGNORE arbitrate type of console, based on user options, and app context
  static bool	Startup_MakeWizards();
  // #IGNORE make the global wizards, including from plugins
  static bool	Startup_MakeMainWin();
  // #IGNORE open the main window (browser of root object) (returns success)
  static bool	Startup_Console();
  // #IGNORE start the console shell (returns success)
  static bool	Startup_RegisterSigHandler();
  // #IGNORE register signal handler routine (i.e., cleanup routine to save recover file upon crashing)

  static bool	Startup_Run();
  // #IGNORE go ahead and run the main event loop

  // these following two guys are run in the event loop, after Startup_Run
  static bool	Startup_ProcessArgs();
  // #IGNORE process general args
  static bool	Startup_RunStartupScript();
  // #IGNORE process general args

  static void	Cleanup_Main();
  // #IGNORE after init, or running, do final cleanups (called by StartupInit on fail, or Startup_Run)

#ifdef DMEM_COMPILE
  static bool 	Run_GuiDMem();
  // #IGNORE run the gui under dmem: requires special code.. 
  //protected:
  static int 	DMem_SubEventLoop();
  // #IGNORE for dmem sub-process (dmem_proc > 0), event-processing loop -- note, called from event loop
  static void	DMem_WaitProc(bool send_stop_to_subs = false);
  // #IGNORE waiting process for dmem_proc = 0, if send_stop_to_subs, sends a stop command to sub procs so they bail out of sub loop
public:
#endif
  	
  bool		CheckAddPluginDep(TypeDef* td); // add a plugin dependency, if this type is a  type defined in a plugin; true if it was
  bool		VerifyHasPlugins(); // check the current plugin_deps w/ loaded plugins, return true if all needed plugins loaded OR user says to continue loading anyway
  void		MakeWizards();
  static void 	SaveRecoverFileHandler(int err = 1);
  // error handling function that saves a recover file when system crashes

  int		SavePluginState(); // save current state for plugins in user data
  int		LoadPluginState(); // load state for plugins from user data
  int	Save();
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(taRootBase)
protected:
  enum StartupMilestones { // #BIT successively marked, so we can shutdown cleanly
    SM_MPI_INIT		= 0x002,
    SM_QAPP_OBJ		= 0x004,
    SM_SOQT_INIT	= 0x008,
    SM_TYPES_INIT	= 0x020,
    SM_ROOT_CREATE	= 0x040,
    SM_APP_OBJ		= 0x080,
    SM_REG_SIG		= 0x100
  };
  
  static int		milestone; // StartupMilestones
  static taMisc::ConsoleType console_type; // #IGNORE 
  static int console_options; //#IGNORE taMisc::ConsoleOptions 
  static ContextFlag	in_init; // suppresses spurious Saves
  
  static bool 	Startup_InitTA_AppFolders();  // #IGNORE Share, Plugins
  static bool 	Startup_InitTA_InitUserAppDir();  // #IGNORE once found
  static void	Startup_EnumerateEnginesR(TypeDef* typ);
  // #IGNORE recursively enumerate from typ 
  bool		AddRecentFile_impl(const String& value); // #IGNORE add this file to the recent list (also adds the path to recent paths)
  bool		AddRecentPath_impl(const String& value); // #IGNORE add this path to the recent list;
  virtual void		AddTemplates(); // called in InitLinks -- extend to add new templates
  virtual taBase* 	GetTemplateInstance_impl(TypeDef* typ, taBase* base);
  virtual void		MakeWizards_impl();
private:
  SIMPLE_COPY(taRootBase)
  void	Initialize();
  void	Destroy();
};


class TA_API PluginWizard : public taWizard {
  // #STEM_BASE ##CAT_Wizard wizard to create a new Plugin
INHERITED(taWizard)
public:
  enum PluginType { // the type of plugin
    UserPlugin,	// created in your emergent_user/plugins folder and only available for your login on your computer
    SystemPlugin, // created in your computer's emergent/plugins folder -- makes plugin available to everyone, but may require Administrator/root access on your system
  };
  
  String		plugin_name; // the name, which must be a valid C identifier, and cannot cause name clashes with existing classes or loaded plugins (this will be checked during Validate)
  PluginType		plugin_type; // the type -- this controls the visibility of the plugin (just you, or everyone on your system) -- on Unix and some Windows installations, you will need administrator rights to install a system plugin	
  bool			default_location; // #DEF_true create the plugin in the default location for the type RECOMMENDED
  bool			validated; // #NO_SHOW
  String		plugin_location; // folder where to create the plugin (folder name should usually be same as plugin_name)
  String		desc;
  String		uniqueId;
  String		url;
  taVersion		version;
  
  bool			Validate();
  // #BUTTON validate all the provided parameters, prior to making the Plugin
  bool			MakePlugin();
  // #BUTTON #CONDEDIT_ON_validated  create the plugin -- must be validated first

  TA_BASEFUNS_NOCOPY(PluginWizard);
protected:
  String 		src_dir;
  String_PArray 	files;
  override void		UpdateAfterEdit_impl();
  override void		CheckThisConfig_impl(bool quiet, bool& ok);
  virtual void		AddFiles(bool upgrade_only); // populate the file list
  virtual void		CreateDestFile(const String& src_file, 
    const String& dst_file, bool& ok);	
  virtual void		TemplatizeFile(const String& src_file, 
    const String& src, String& dst, bool& ok);
private:
  void 	Initialize();
  void 	Destroy()	{ };
};


#endif
