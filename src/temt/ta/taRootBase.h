// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef taRootBase_h
#define taRootBase_h 1

// parent includes:
#include <taFBase>

// member includes:
#include <taBase_List>
#include <Doc_Group>
#include <Wizard_Group>
#include <Project_Group>
#include <taViewer_List>
#include <taPlugin_List>
#include <taPluginBase_List>
#include <taiMimeFactory_List>
#include <ColorScaleSpec_Group>
#include <String_Array>
#include <char_Array>
#include <taVector2i>
#include <taVector2f>
#include <ContextFlag>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class taBase; // 
class taDoc; // 


taTypeDef_Of(taRootBase);

class TA_API taRootBase: public taFBase {
  // ##CAT_Project ##EXPAND_DEF_2 base class for the root of the structural hierarchy (root. or . in css / paths)
INHERITED(taFBase)
public:
  static TypeDef*       root_type;      // set in Startup_Main
  static taRootBase*    instance();
  static bool           openProject;    // #NO_SHOW #NO_SAVE set to true if any project gets opened

  String                version;        // #READ_ONLY #SHOW current version number
  taBase_List           templates;      // #NO_SAVE #READ_ONLY objects used as templates -- do not use or mess with these!
  Doc_Group             docs;           // #NO_SAVE documents, typically linked to other objects
  Wizard_Group          wizards;        // #NO_SAVE global wizards -- see each project for project-specific wizards
  Project_Group         projects;       // #NO_SAVE The projects
  taViewer_List         viewers;        // #NO_SAVE viewer display settings for the root object only
  taViewer_List         viewers_tmp;    // #READ_ONLY #HIDDEN #NO_SAVE temporary viewers
  taPlugin_List         plugins;        // available plugins
  taBase_List           plugin_state;   // #NO_SAVE #HIDDEN #HIDDEN_TREE state objs of plugins -- created/managed by plugin system; state saved as {name}.state in user data
  taPluginBase_List     plugin_deps;    // #SHOW_TREE #EXPERT_TREE #NO_SAVE  dynamic list, populated in presave
  taiMimeFactory_List   mime_factories; // #NO_SAVE #HIDDEN_TREE extensible list of mime factories
  ColorScaleSpec_Group  colorspecs;     // global list of color specs for displaying values in terms of colors
  taBase_List           objs;           // #SHOW_TREE #EXPERT_TREE #NO_SAVE misc place for app-global objs, usually for system use, ex. tcp server
  String_Array          recent_files;   // #NO_SHOW recently loaded files
  String_Array          recent_paths;   // #NO_SHOW recently used paths
  String_Array          sidebar_paths;  // #NO_SHOW for the file chooser dialog -- sidebar paths that are saved and reloaded
  taVector2f            rootview_pos;   // #NO_SHOW position for the root viewer
  taVector2f            rootview_size;   // #NO_SHOW size for the root viewer
  String                rootview_splits; // #NO_SHOW splitter config in root viewer
  char_Array            filedlg_setary; // #NO_SHOW settings for the file dialog -- persisted
  taVector2i            filedlg_size;   // #NO_SHOW size for the file dialog -- persisted

  void          OpenRemoteServer(ushort port = 5360);
  // #MENU #MENU_ON_Server Open a Server for remote TCP-based control of this application
  void          CloseRemoteServer();
  // #MENU #MENU_ON_Server Close an open Server, if any

  virtual void  Options();
  // edit global settings/parameters (taMisc)
  virtual void  About();
  // #MENU #MENU_ON_Object get information/copyright notice
  virtual void  SaveAll();
  // saves all the projects
  void          WindowShowHook() override;

  void          AddRecentFile(const String& value, bool no_save = false); // #IGNORE add this file to the recent list (also adds the path to recent paths)
  void          ClearRecentFiles();
  // #IGNORE
  void          AddRecentPath(const String& value, bool no_save = false); // #IGNORE add this path to the recent list

  taBase*       FindGlobalObject(TypeDef* base_type = &TA_taBase,
    const String& name = _nilString);
    // find an object deriving from base_type, with given name, or any name if blank

  virtual taDoc* FindMakeDoc(const String& doc_name, const String& wiki_nm = "",
                             const String& web_url = "");
  // #CAT_Doc get doc document object of given name, or make one if not found -- also set the wiki name and web url if specified

  virtual void  MonControl(bool on);
  // #MENU #MENU_ON_Object set profile monitoring: on=true = on, else off.  starts out off..

  taBase*       GetTemplateInstance(TypeDef* typ);
  // get an instance of the indicated tab type, or NULL if not found

  ///////////////////////////////////////////////////////////////////////////////
  //            Startup Code    (in order of calling by Startup_Main)

  static bool   Startup_Main(int& argc, const char* argv[], 
                             TypeDef* root_typ = &TA_taRootBase);
  // #IGNORE this is the main function call to startup the software -- creates a root object of given type, processes args, sets global state vars, starts up css, and opens a browser window on the root object if in gui mode

  static bool   Startup_InitDMem(int& argc, const char* argv[]);
  // #IGNORE init distributed memory (MPI) stuff
  static bool   Startup_ProcessGuiArg(int argc, const char* argv[]);
  // #IGNORE process the -gui/-nogui arg
  static bool   Startup_InitApp(int& argc, const char* argv[]);
  // #IGNORE init application stuff (qapp etc) -- note: twiddles cmdline args
  static bool   Startup_InitArgs(int& argc, const char* argv[]);
  // #IGNORE process args into more usable form
  static bool   Startup_InitTA();
  // #IGNORE basic type-access system intializaton
  static bool   Startup_EnumeratePlugins();
  // #IGNORE enumeration of plugins
  static bool   Startup_LoadPlugins();
  // #IGNORE load and init types of plugins
  static bool   Startup_InitCss();
  // #IGNORE initialize css script system
  static bool   Startup_InitGui();
  // #IGNORE initialize gui system
  static bool   Startup_InitViewColors();
  // #IGNORE initialize default view colors
  static bool   Startup_InitViewBackgrounds();
  // #IGNORE initialize default view backgrounds - brushes
  static bool   Startup_ConsoleType();
  // #IGNORE arbitrate type of console, based on user options, and app context
  static bool   Startup_MakeWizards();
  // #IGNORE make the global wizards
  static bool   Startup_InitPlugins();
  // #IGNORE create and restore plugin state, and final initialize
  static bool   Startup_MakeMainWin();
  // #IGNORE open the main window (browser of root object) (returns success)
  static bool   Startup_Console();
  // #IGNORE start the console shell (returns success)
  static bool   Startup_RegisterSigHandler();
  // #IGNORE register signal handler routine (i.e., cleanup routine to save recover file upon crashing)

  static bool   Startup_Run();
  // #IGNORE go ahead and run the main event loop

  // these following two guys are run in the event loop, after Startup_Run
  static bool   Startup_ProcessArgs();
  // #IGNORE process general args
  static bool   Startup_RunStartupScript();
  // #IGNORE process general args

  static void   ProjectOpened();
  // #IGNORE called when a project is opened so we can minimize the root window if this is the first project to be opened

  static void   Cleanup_Main();
  // #IGNORE after init, or running, do final cleanups (called by StartupInit on fail, or Startup_Run)

  virtual void  ConsoleNewStdin(int n_lines);
  // notification that the console has received new input lines

#ifdef DMEM_COMPILE
  static bool   Run_GuiDMem();
  // #IGNORE run the gui under dmem: requires special code..
  //protected:
  static int    DMem_SubEventLoop();
  // #IGNORE for dmem sub-process (dmem_proc > 0), event-processing loop -- note, called from event loop
  static void   DMem_WaitProc(bool send_stop_to_subs = false);
  // #IGNORE waiting process for dmem_proc = 0, if send_stop_to_subs, sends a stop command to sub procs so they bail out of sub loop
public:
#endif

  bool          CheckAddPluginDep(TypeDef* td); // add a plugin dependency, if this type is a  type defined in a plugin; true if it was
  bool          VerifyHasPlugins(); // check the current plugin_deps w/ loaded plugins, return true if all needed plugins loaded OR user says to continue loading anyway
  void          MakeWizards();
  static void   SaveRecoverFileHandler(int err = 1);
  // error handling function that saves a recover file when system crashes

  int           SavePluginState(); // save current state for plugins in user data
  int           LoadPluginState(); // load state for plugins from user data
  int           Save() override;

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(taRootBase)
protected:
  enum StartupMilestones { // #BIT successively marked, so we can shutdown cleanly
    SM_MPI_INIT         = 0x002,
    SM_QAPP_OBJ         = 0x004,
    SM_SOQT_INIT        = 0x008,
    SM_TYPES_INIT       = 0x020,
    SM_ROOT_CREATE      = 0x040,
    SM_APP_OBJ          = 0x080,
    SM_REG_SIG          = 0x100
  };

  static int            milestone; // StartupMilestones
  static int            console_type; // #IGNORE taMisc::ConsoleType
  static int            console_options; //#IGNORE taMisc::ConsoleOptions
  static ContextFlag    in_init; // suppresses spurious Saves

  static bool           Startup_InitTA_AppFolders();  // #IGNORE Share, Plugins
  static bool           Startup_InitTA_InitUserAppDir();  // #IGNORE once found
  static void           Startup_EnumerateEnginesR(TypeDef* typ);
  void         UpdateAfterEdit_impl() override;
  // #IGNORE recursively enumerate from typ
  bool                  AddRecentFile_impl(const String& value); // #IGNORE add this file to the recent list (also adds the path to recent paths)
  bool                  AddRecentPath_impl(const String& value); // #IGNORE add this path to the recent list;
  virtual void          AddTemplates(); // called in InitLinks -- extend to add new templates
  virtual void          AddDocs(); // called in InitLinks -- extend to add new docs
  virtual taBase*       GetTemplateInstance_impl(TypeDef* typ, taBase* base);
  virtual void          MakeWizards_impl();

#ifdef TA_OS_LINUX
public: // debuggy stuff
  enum FPExceptFlags { // #BITS floating point exception masks -- helps to debug nan issues etc.
   FPE_0                = 0, // #IGNORE
   FPE_INEXACT          = 0x001, // inexact result
   FPE_DIVBYZERO        = 0x002, // division by zero
   FPE_UNDERFLOW        = 0x004, // result not representable due to underflow
   FPE_OVERFLOW         = 0x008, // result not representable due to overflow
   FPE_INVALID          = 0x010, // invalid operation
  };

  FPExceptFlags         fpe_enable; // #EXPERT #NO_SAVE (DEBUG builds only) enable floating point exceptions; useful to help locate nan and similar issues
protected:
  int                   GetFEFlags(FPExceptFlags fpef); // convert FPE flags to FE flags
  FPExceptFlags         GetFPEFlags(int fef); // convert FE flags to FPE flags
#endif

private:
  SIMPLE_COPY(taRootBase)
  void  Initialize();
  void  Destroy();
};

#endif // taRootBase_h
