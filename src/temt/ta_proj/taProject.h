// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef taProject_h
#define taProject_h 1

// this is useful for seeing who is including this and why..
// #warning "project included"

// parent includes:
#include <taFBase>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <taProjVersion>
#include <taLicense>
#include <taWikiSpec>
#include <taBase_Group>
#include <Patch_Group>
#include <Doc_Group>
#include <Wizard_Group>
#include <ControlPanel_Group>
#include <ParamSet_Group>
#include <ArchivedParams_Group>
#include <DataTable_Group>
#include <Program_TopGroup>
#include <ProgramRef>
#include <taViewer_List>
#include <taUndoMgr>
#include <TimeUsed>
#include <String_Array>

// declare all other types mentioned but not required to include:
class MainWindowViewer; // 
class DataTable; // 
class TypeDef; // 
class ControlPanel; // 
class taDoc; // 


taTypeDef_Of(taProject);

class TA_API taProject : public taFBase {
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #VIRT_BASE ##DUMP_LOAD_POST ##DEF_NAME_ROOT_Project ##CAT_Project ##UNDO_BARRIER Base class for a project object containing all relevant info for a given instance -- all ta GUI-based systems should have one..
INHERITED(taFBase)
public:
  enum  StdLicense {
    GPL2,                       // GNU General Public License (GPL), version 2
    GPL3,                       // GNU General Public License (GPL), version 3
    BSD,                        // Berkeley Software Distribution License
    OSL3,                       // Open Software License, version 3
    AFL3,                       // Academic Free License, version 3
    MIT,                        // MIT License
    APACHE,                     // Apache License, version 2.0
    MOZILLA,                    // Mozilla Public License 1.1 (MPL)
    RESEARCH,                   // Research License based on CU Boulder template
  };

  enum ProjLibs {
    USER_LIB,                   // user's personal library -- located in app user dir (~/lib/emergent or ~/Library/Emergent proj_templates)
    SYSTEM_LIB,                 // local system library, installed with software, in /usr/local/share/Emergent/proj_templates
    WEB_APP_LIB,                // web-based application-specific library (e.g., emergent)
    WEB_SCI_LIB,                // web-based scientifically oriented library (e.g., CCN)
    WEB_USER_LIB,               // web-based user's library (e.g., from lab wiki)
  };

  static taProject*     cur_proj; // #NO_SAVE #HIDDEN #READ_ONLY current active project -- gui actions and program running updates this and also updates the active directory to the proj_dir

#ifdef __MAKETA__
  String      name; // #CONDEDIT_OFF_base_flags:NAME_READONLY #NO_DIFF #CAT_taBase name of the project
#endif
  
  String                proj_dir;  // #NO_SAVE #READ_ONLY #SHOW current working directory for the project -- automatically set when loading or saving the project
  String                tags;      // #EDIT_DIALOG list of comma separated tags (use initial letter uppercase) that indicate the basic function of this project -- should be listed in hierarchical order, with most important/general tags first -- these are used for searching the online project library if this project is uploaded
  taProjVersion         version;
  // #NO_DIFF project version numbering information -- useful for keeping track of changes over time (recorded in change log automatically with SaveNoteChanges)
  String                author;
  // #NO_DIFF the project author - by default the one saved in emergent preferences
  String                email;
  // #NO_DIFF email address for comments, questions about the project - typically the author's email address - by default the one saved in emergent preferences
  taLicense             license; // license for this project -- to be determined by the original creator of the project -- if this is not you, then you must abide by the constraints of the original license, if specified -- do ViewLicense button to see the applicable license information for this project
  String                pub_cite; // for a project that has been described in a publication, this is the citation key for that publication, in the form of Author1[Author2][Author3][EtAl]YY where Author is last name of given author -- e.g., OReillyMunakataFrankEtAl12 is the citation key for the online wiki textbook: http://ccnbook.colorado.edu
  taWikiSpec            wiki; // wiki info, for projects that have been saved to a wiki
  Patch_Group           patches; // #NO_DIFF #NO_SAVE patches (collections of edit changes) that can be used for updating projects based on changes made to other projects -- can be generated by DiffCompare function, and loaded from files or web
  Doc_Group             docs; // #NO_DIFF documents, typically linked to other objects
  Wizard_Group          wizards; // #NO_DIFF Wizards for automatically configuring simulation objects
  ControlPanel_Group    ctrl_panels;  // #AKA_edits special edit dialogs for selected elements
  ParamSet_Group        active_params; // #AKA_param_sets actively used sets of parameters with values that can be used to restore a particular set of parameters -- best to organize into groups, which default to a Master / Clone relationship to enforce consistent set of parameters across multiple different sets that can be used
  ArchivedParams_Group  archived_params; // archived sets of parameters with values to record the history of different parameters over time -- one use the Archive button on active params to automatically save a copy here -- typically organized by date with newest on top
  DataTable_Group       data;   // data, such as patterns for network input
  taBase_Group          data_proc; // #NO_DIFF #TREE_HIDDEN #NO_SAVE objects that perform data processing operations (functions collected on objects for different kinds of operations)
  Program_TopGroup      programs; // Gui-based programs to run simulations and other processing
  taViewer_List         viewers; // #NO_DIFF a list of the viewers that have been made and saved in the project; choose one, right click, and OpenViewer to view if not open
  taViewer_List         viewers_tmp; // #NO_DIFF #READ_ONLY #HIDDEN #NO_SAVE temporary viewers (edit dialogs etc -- not saved)
  taUndoMgr             undo_mgr; // #NO_DIFF #READ_ONLY #HIDDEN #NO_SAVE undo manager

  bool                  save_view; // project specific setting determines if view state saved when project is saved
  bool                  save_as_only;  // prevent the project from being changed, force save_as
  bool                  auto_name; // #DEF_true automatically update the name of the project based on the file name used when saving -- useful because this name is typically neglected yet useful for distinguishing different projects when comparing or choosing objects
  bool                  m_dirty; // #HIDDEN #READ_ONLY #NO_SAVE
  bool                  m_no_save; // #HIDDEN #READ_ONLY #NO_SAVE -- flag to prevent double user query on exiting; cleared when undirtying
  bool                  no_dialogs; // #HIDDEN #NO_SAVE -- UpdateAfterEdit methods can check to see if this is a good time to pop a dialog or if it should be skipped
  String                last_change_desc; // #NO_DIFF #EXPERT description of the last change made to the project -- used for change log
  String                view_plog;
  // #READ_ONLY #NO_SAVE current view of project log data
  String_Array          tree_state;  // #HIDDEN #READ_ONLY save!! this is the expand/collapse state of the navigator tree nodes
  ProgramRef            last_step_prog;
  // #READ_ONLY #NO_SAVE the last program to be single-stepped -- set by the Step call of the program that was last run
  ProgramRef            last_run_prog;
  // #READ_ONLY #NO_SAVE the last program to have been run by the user -- top-level run call
  ProgramRef            last_stop_prog;
  // #READ_ONLY #NO_SAVE the last program that was stopped from a stop request of any sort

  bool                  isDirty() const override {return m_dirty;}
  void                  setDirty(bool value) override;  //

  static String         GetProjTemplatePath(ProjLibs library);
  // #CAT_ProjTemplates get path to given project template library

  virtual void          SaveAsTemplate(const String& template_name, const String& desc,
                          const String& tags, ProjLibs library = USER_LIB);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #CAT_Project save the project to given project library -- please add a useful description and set of comma separated tags (especially if this is a new template) so others will know what it is for

  ///////////////////////////////////////////////////////////////////
  //    View/Browser Stuff

  MainWindowViewer*     GetDefaultProjectBrowser();
  // #CAT_Display gets one if there is, else NULL
  MainWindowViewer*     GetDefaultProjectViewer();
  // #CAT_Display gets one if there is, else NULL
  virtual MainWindowViewer* AssertDefaultProjectBrowser(bool auto_opn);
    // #CAT_Display make sure the default project browser is made, and optionally open it
  virtual void          OpenNewProjectBrowser(String proj_browser_name = "(default name)");
    // #MENU #MENU_ON_View #MENU_CONTEXT #CAT_Display open a new browser, either 3-pane or 2-2-pane (tree + viewer)
  virtual void          OpenNewProjectViewer(String proj_browser_name = "(default name)");
    // #MENU #MENU_ON_View #MENU_CONTEXT #CAT_Display open a new 3D viewer (with panels)
  virtual MainWindowViewer* NewProjectBrowser();
    // create a new, empty viewer -- note: window not opened yet
  virtual void          RefreshAllViews();
  // #CAT_Display manual refresh of all view information in the project -- equivalent to the View/Refresh (F5 key) menu -- should not be necessary but sometimes comes in handy..
  virtual void          UpdateUi();
  // #CAT_Display manual call to update user interface enabled/disabled settings -- usually done through signals and slots, but this can be useful for non-gui driven changes that might affect enabling
  virtual void          DelayedUpdateUi();
  // #CAT_Display manual call to update user interface enabled/disabled settings -- usually done through signals and slots, but this can be useful for non-gui driven changes that might affect enabling -- delayed version only happens in event loop
  virtual void          SelectT3ViewTabNo(int tab_idx);
  // activate given 3D view tab, specified by number (0..n-1, left to right) -- see also SelectT3ViewTabName
  virtual void          SelectT3ViewTabName(const String& tab_nm);
  // activate given 3D view tab, specified by the name of the tab -- see also SelectT3ViewTabNo

  ///////////////////////////////////////////////////////////////////
  //    Get new proj objects

  virtual DataTable*    GetNewInputDataTable(const String& nw_nm="", bool msg=false);
  // #CAT_Data create a new data table in data.InputData (used for data generation functions).  nw_nm = name for new table, msg = issue a warning message about the creation of this table
  virtual DataTable*    GetNewOutputDataTable(const String& nw_nm="", bool msg=false);
  // #CAT_Data create a new data table in data.OutputData (used for monitoring and logging functions).  nw_nm = name for new table, msg = issue a warning message about the creation of this table
  virtual DataTable*    GetNewAnalysisDataTable(const String& nw_nm="", bool msg=false);
  // #CAT_Data create a new data table in data.AnalysisData (used for various data processing and graphing functions).  nw_nm = name for new table, msg = issue a warning message about the creation of this table
  virtual taBase*       FindMakeNewDataProc(TypeDef* typ, const String& nm);
  // #CAT_Data find existing data processing object of given type, or else make one and give it nm
  virtual ControlPanel*   FindMakeControlPanel(const String& seledit_name,
                                           TypeDef* type = &TA_ControlPanel);
  // #CAT_Edit get control panel object of given name, or make one if not found
  virtual taDoc*        FindMakeDoc(const String& doc_name, const String& wiki_nm = "",
                                    const String& web_url = "");
  // #CAT_Doc get doc document object of given name, or make one if not found -- also set the wiki name and web url if specified
  virtual String        GetClusterRunPath();
  // #CAT_File get the full path to the current cluster run repository associated with this project ---- if taMisc::cluster_run flag set, then we're actually running on the cluster, in which case the path is one up from the current project directory -- otherwise it is the svn repository path from the ClusterRun object -- if no such path is available, it just returns the current project directory path
  virtual bool          GetClusterRunJob(int updt_interval_mins = 10);
  // #CAT_File if we are a taMisc::cluster_run job, get all the current job data into ClusterRunJob::cur_job -- returns true if cur_job updated -- if cur_job does not yet exist, then it always tries to load data, otherwise it updates every 10 minutes by default (which corresponds with how frequently the cluster run script auto-updates running job info)
  virtual String        CheckClusterRunCmd();
  // #CAT_File if we are a taMisc::cluster_run job, see if we have received a command in jobs_running_cmd.dat file -- if so, returns command (status field of table), and removes this command from the table and saves it
  
  ///////////////////////////////////////////////////////////////////
  //    misc

  virtual void          SvnBrowser();
  // #MENU_BUTTON #MENU_ON_Svn #CAT_File open a subversion browser for the directory that this project is in
  virtual void          SvnCommit(bool project_file_only);
  // #MENU_BUTTON #MENU_ON_Svn #CAT_File commit this project file (saves first) into svn (only if already added to svn repository -- use SvnBrowser to do that if necessary)
  virtual bool          SvnAdd();
  // #MENU_BUTTON #MENU_ON_Svn #CAT_File add a single file to the svn repository
  virtual bool          SvnCommitDialog(String& commit_msg, bool& updt_change_log,
                                        const String& com_itm_str);
  // #IGNORE popup an svn commit dialog, return false if canceled.  commit_msg = msg
  virtual void          SvnDiff();
  // #MENU_BUTTON #MENU_ON_Svn #MENU_SEP_BEFORE #CAT_File #CONFIRM compare differences between current working copy of project and the immediate prior svn checkin -- checks out the prior svn version of the project as a tmp project file, loads it, and runs a diff compare which then pulls up a browser of the differences -- you can select actions to generate a patch that will capture your changes so that others can replicate your changes on their own projects
  virtual void          SvnPrevDiff();
  // #MENU_BUTTON #MENU_ON_Svn #CAT_File #CONFIRM compare differences between last svn revision of project and the one immediately before that -- checks out the prior svn versions of the project as tmp project files, loads them, and runs a diff compare which then pulls up a browser of the differences -- you can select actions to generate a patch that will capture your changes so that others can replicate your changes on their own projects
  virtual void          CleanSvnProjs();
  // #MENU_BUTTON #MENU_ON_Svn #CAT_File #CONFIRM remove old svn versions of project file -- looks for files with current name followed by _number.proj where number is the revision number, and removes them -- console shows the files removed
  virtual void          DiffMerge(taProject* vers1, taProject* vers2);
  // #MENU_BUTTON #MENU_ON_Svn #MENU_SEP_BEFORE #INIT_ARGVAL_ON_file_name #EXT_proj #CAT_File #FILE_DIALOG_LOAD merge two versions of the current project, by computing diffs (and generating patches) from each version against this project which must be a common base version that these two are descended from -- both diff browsers will appear at same time -- see top text for which is which.  When you are done, resulting patch records (stored in patches on this project) must then be merged -- do MergePatches in patch group of this project, which will flag any apparent conflicts affecting the same target object, and remove duplicates -- once the conflicts are manually resolved (remove one or the other) then the resulting patch can be applied to the base project to combine the two versions
  virtual void          DiffMergeFiles(const String& vers1_fname, const String& vers2_fname);
  // #INIT_ARGVAL_ON_file_name #EXT_proj #CAT_File #FILE_DIALOG_LOAD merge two versions of the current project, by computing diffs (and generating patches) from each version against this project which must be a common base version that these two are descended from (both files must be present on local file system) -- both diff browsers will appear at same time -- see top text for which is which.  When you are done, resulting patch records (stored in patches on this project) must then be merged -- do MergePatches in patch group of this project, which will flag any apparent conflicts affecting the same target object, and remove duplicates -- once the conflicts are manually resolved (remove one or the other) then the resulting patch can be applied to the base project to combine the two versions
  
  bool                  DiffCompare(taBase* cmp_obj) override;
  // #BUTTON #MENU_ON_Svn #CAT_File compare this project against another project, pulling up a diff browser where the differences can be viewed, and then any changes selected to generate a Pach for each project that will convert from one into the other -- this is very robust and operates on separate major groups of objects -- strongly recommended for recording and communicating changes between projects
  virtual void          UpdateChangeLog();
  // #BUTTON #ARGC_0 #CAT_File update change log for this project, stored as a ChangeLog item in docs on the project -- you will be prompted to enter a description of recent changes, and the date, user, and file names will be recorded -- increments version step by 1
  virtual void          RecordChangeLog(const String& msg);
  // #CAT_File record a new change log entry with given message text -- increments version step by 1
  virtual void          UndoStats(bool show_list = false, bool show_diffs = false);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #NO_SAVE_UNDO #CAT_File report to css Console the current undo statistics in terms of # of records and total amount of RAM taken, etc -- if show_list, show full list of current undo info, if show_diffs, then show full diffs of changes from orig source data (requires show_list too)
  virtual void          UndoSaveCurRec();
  // #MENU #MENU_ON_Object #NO_SAVE_UNDO #CAT_File #CONFIRM save the current undo save file to 'undo_cur_src_rec.proj' file in current directory -- useful for determining what is being saved in the undo state when it seems bigger than it should be!
  virtual void          ReplaceString(const String& srch, const String& repl);
  // #BUTTON #CAT_File replace string in this project -- WARNING: this is potentially highly destructive and dangerous -- it just does a literal string replace for ANYTHING that matches srch in the ENTIRE project file (types, paths, values everything) -- this could end up rendering your project completely brain dead, which you will soon see due to error messages: it converts entire project to a string object, does the replace on that, and then attempts to load the new project string back over the top of the current one -- errors will typically be evident during this load process.  If you know what you're doing, this can be a very convenient way to update types or other such things globally.

  virtual String        GetDir();
  // #CAT_File get the project directory
  virtual void          SetProjAsCurrent();
  // #CAT_File set the current project to be the current active project -- this also sets the proj_dir for this project to be the current working directory for the file system (whenever this project enters scope, this should be called, so files are always loaded appropriately)
  virtual void          SaveRecoverFile();
  // #CAT_File Save a recover file of this project, usually called when a signal is received indicating a crash condition
  virtual void          SaveRecoverFile_strm(std::ostream& strm);
  // #IGNORE underlying save function to use when saving a recover file -- might want to do something special here
  virtual String        GetAutoFileName(const String& suffix, const String& ftype_ext = ".proj");
  // #CAT_File get a file name to save project to, with suffix but file extension *removed*, based on any existing file name, project name, and type
  virtual bool          AutoSave(bool force = false);
  // #CAT_File called automatically by the wait process -- if enough time has passed or force is true, save current project to an auto save backup file (file name + _autosave)
  virtual void          AutoNameProj(const String& fname);
  // #CAT_File update the name of the project based on given file name

  virtual String        ProgGlobalStatus();
  // #CAT_Code #IGNORE get a global status string for programs for display in status bar

  
  bool                  SetFileName(const String& val) override;
  int                   Save_strm(std::ostream& strm, taBase* par=NULL, int indent=0) override;
  int                   Save() override;
  int                   SaveAs(const String& fname = "") override;
  virtual int           SaveCopy(const String& fname = "");
  // #MENU #ARGC_0 #EDIT_READ_ONLY #CAT_File Save a copy of the object without changing current file name -- if fname is empty, the user is prompted with a file dialog
  void                  SetSaveView(bool value);
  // #CAT_File determines whether or not the current view settings will be saved with the project
  virtual void          SaveViewState();
  // #CAT_File save the window sizes / positions (including css console) and other view state if save_view true

  virtual bool          PublishProjectOnWeb(const String& wiki_name);
  // #CAT_File publish project on the web (wiki) to given wiki name -- menu chooser for wikis available in gui version in iMainWindowViewer
  virtual bool          UpdateProjectOnWeb(const String& wiki_name);
  // #CAT_File update (upload latest version) of project on the web (wiki) to given wiki name -- menu chooser for wikis available in gui version in iMainWindowViewer
  virtual bool          UploadFilesForProjectOnWeb(const String& wiki_name);
  // #CAT_File upload a list of files associated with project to the web (wiki) to given wiki name -- menu chooser for wikis available in gui version in iMainWindowViewer
  virtual bool          OpenProjectFromWeb(const String& proj_file_name, const String& wiki_name);
  // #CAT_File upload a list of files associated with project to the web (wiki) to given wiki name -- menu chooser for wikis available in gui version in iMainWindowViewer

  int                   Load(const String& fname="", taBase** loaded_obj_ptr = NULL) override;
  virtual void          OpenProjectLog();
  // #CAT_File set the system event logging to record to the project name + .plog -- called for SaveAs and Load -- uses project file_name value

  virtual bool          CleanFiles();
  // #BUTTON #CAT_File remove autosave and recover files based on current file_name -- this is done by default after saving a non-recover or autosave version of the project

  virtual void          ViewLicense();
  // #BUTTON view the applicable license information for this project
  virtual void          AddMeAsAuthor(bool sole_author, bool updt_license);
  // #BUTTON update the author, email, and (optionally) license information with your default information as set in the preferences (update preferences first if not otherwise set!) -- if sole_author then updt_licence automatically
  virtual void          SaveSetAuthor();
  // #IGNORE during save, set author if information is otherwise blank

  virtual int           SaveNoteChanges();
  // #CAT_File saves the project to a file using current file name, but first prompts for a text note of changes that have been made, which are registered in the ChangeLog document within the project prior to saving
  virtual int           SaveAsNoteChanges(const String& fname = "");
  // #CAT_File Saves object data to a new file -- if fname is empty, it prompts the user, but first prompts for a text note of changes that have been made, which are registered in the ChangeLog document within the project prior to saving

  virtual void          ViewProjLog();
  // #MENU #MENU_ON_View #MENU_CONTEXT #MENU_SEP_BEFORE #BUTTON #CAT_File view the current project log file in internal text viewer display
  virtual void          ViewProjLog_Editor();
  // #MENU #MENU_ON_View #MENU_CONTEXT #CAT_File view the current project log file in an external editor as specified in the preferences

  virtual void          BgRunKilled() { };
  // #IGNORE called when program is quitting prematurely and is not in an interactive mode -- override to save relevant state information
  
  void                  Dump_Load_post() override;
  void                  OpenViewers(); // open any yet unopen viewers
  void                  CloseLater() override;
  
  virtual void          ParamSetComparePeers(ParamSet* key_set, ParamSet* peer_set);
  // creates a table with a column of values for key_paramam_set and column for peer_set - values for peer_set are shown if different from key_spec - if table with key_spec exists a call with a new peer adds a column to the table
  virtual void          WriteParamMbrNamesToTable(DataTable* param_table, ParamSet* param_set);
  // #IGNORE writes ParamSet member names to a param_set compare table
  virtual void          WriteParamSavedValsToTable(DataTable* param_table, ParamSet* param_set);
  // #IGNORE writes param member values to a param_set compare table
  virtual void          AddPeerToParamCompareTable(DataTable* param_table, ParamSet* param_set);
  // #IGNORE called to add a column for a single param_set to param compare data table -- this table is not automatically updated -- call again if you change params!
  void                  UpdateAfterEdit() override;
  virtual void          InitLinks_impl(); // #IGNORE use this instead of InitLinks in subclasses
  virtual void          CutLinks_impl(); // #IGNORE use this instead of CutLinks in subclasses -- you can call this first to nuke the viewers etc. before your own stuff
  TA_BASEFUNS(taProject);

protected:
  TimeUsed              auto_save_timer; // #IGNORE timer used for auto saving

  virtual void          InitLinks_post(); // #IGNORE called after all _impls (not called for undo_loading): assert things like default wizards in here
  void                  CutLinks() override; // don't override this -- use _impl instead
  virtual MainWindowViewer* MakeProjectBrowser_impl(); // make a standard viewer for this project type
  int                   GetOwnerEditableState_impl(int mask) const override
    {return 0;} // the readonly stops here!
  void                  DoView();

private:
  void  Copy_(const taProject& cp);
  void  InitLinks() override; // don't try to use this -- use _impl instead
  void  Initialize();
  void  Destroy();
};

TA_SMART_PTRS(TA_API, taProject);

#endif // taProject_h
