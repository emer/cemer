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

#ifndef taProject_h
#define taProject_h 1

// parent includes:
#include <taFBase>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <taProjVersion>
#include <taLicense>
#include <taWikiURL>
#include <taBase_Group>
#include <Doc_Group>
#include <Wizard_Group>
#include <SelectEdit_Group>
#include <DataTable_Group>
#include <Program_Group>
#include <DataViewer_List>
#include <taUndoMgr>
#include <TimeUsed>

// declare all other types mentioned but not required to include:
class MainWindowViewer; // 
class DataTable; // 
class TypeDef; // 
class SelectEdit; // 
class taDoc; // 


class TA_API taProject : public taFBase {
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #VIRT_BASE ##DUMP_LOAD_POST ##DEF_NAME_ROOT_Project ##CAT_Project ##UNDO_BARRIER ##EXPAND_DEF_2 Base class for a project object containing all relevant info for a given instance -- all ta GUI-based systems should have one..
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
    USER_LIB,                   // user's personal library
    SYSTEM_LIB,                 // local system library
    WEB_LIB,                    // web-based library
    SEARCH_LIBS,                // search through the libraries (for loading)
  };

  String		proj_dir;  // #NO_SAVE #READ_ONLY #SHOW current working directory for the project -- automatically set when loading or saving the project
  String                tags;      // #EDIT_DIALOG list of comma separated tags that indicate the basic function of this project -- should be listed in hierarchical order, with most important/general tags first -- these are used for searching the online project library if this project is uploaded
  taProjVersion         version;
  // project version numbering information -- useful for keeping track of changes over time (recorded in change log automatically with SaveNoteChanges)
  taLicense             license; // license for this project -- to be determined by the original creator of the project -- if this is not you, then you must abide by the constraints of the original license, if specified -- do ViewLicense button to see the applicable license information for this project
  taWikiURL             wiki_url; // url to synchronize project to/from wiki -- see taMisc::wiki_url for base url in case of relative location
  taBase_Group          templates; // #HIDDEN templates for new objects -- copy new objects from here
  Doc_Group             docs; // documents, typically linked to other objects
  Wizard_Group          wizards; // Wizards for automatically configuring simulation objects
  SelectEdit_Group      edits;  // special edit dialogs for selected elements
  DataTable_Group       data;   // data, such as patterns for network input
  taBase_Group          data_proc; // objects that perform data processing operations (functions collected on objects for different kinds of operations)
  Program_Group         programs; // Gui-based programs to run simulations and other processing
  DataViewer_List       viewers; // a list of the viewers that have been made and saved in the project; choose one, right click, and OpenViewer to view if not open
  DataViewer_List       viewers_tmp; // #READ_ONLY #HIDDEN #NO_SAVE temporary viewers (edit dialogs etc -- not saved)
  taUndoMgr             undo_mgr; // #READ_ONLY #HIDDEN #NO_SAVE undo manager

  bool                  auto_name; // #DEF_true automatically update the name of the project based on the file name used when saving -- useful because this name is typically neglected yet useful for distinguishing different projects when comparing or choosing objects
  bool                  m_dirty; // #HIDDEN #READ_ONLY #NO_SAVE
  bool                  m_no_save; // #HIDDEN #READ_ONLY #NO_SAVE -- flag to prevent double user query on exiting; cleared when undirtying
  String                last_change_desc; // #EXPERT description of the last change made to the project -- used for change log
  String                view_plog;
  // #READ_ONLY #NO_SAVE current view of project log data

  override bool         isDirty() const {return m_dirty;}
  override void         setDirty(bool value);  //

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
  virtual SelectEdit*   FindMakeSelectEdit(const String& seledit_name,
					   TypeDef* type = &TA_SelectEdit);
  // #CAT_Edit get select edit object of given name, or make one if not found
  virtual taDoc*        FindMakeDoc(const String& doc_name, const String& wiki_nm = "",
                                    const String& web_url = "");
  // #CAT_Doc get doc document object of given name, or make one if not found -- also set the wiki name and web url if specified

  ///////////////////////////////////////////////////////////////////
  //    misc

  virtual void          UpdateChangeLog();
  // #BUTTON #CAT_File update change log for this project, stored as a ChangeLog item in docs on the project -- you will be prompted to enter a description of recent changes, and the date, user, and file names will be recorded
  virtual void          UndoStats(bool show_list = false, bool show_diffs = false);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_File report to css Console the current undo statistics in terms of # of records and total amount of RAM taken, etc -- if show_list, show full list of current undo info, if show_diffs, then show full diffs of changes from orig source data (requires show_list too)
  virtual void          ReplaceString(const String& srch, const String& repl);
  // #BUTTON #CAT_File replace string in this project -- WARNING: this is potentially highly destructive and dangerous -- it just does a literal string replace for ANYTHING that matches srch in the ENTIRE project file (types, paths, values everything) -- this could end up rendering your project completely brain dead, which you will soon see due to error messages: it converts entire project to a string object, does the replace on that, and then attempts to load the new project string back over the top of the current one -- errors will typically be evident during this load process.  If you know what you're doing, this can be a very convenient way to update types or other such things globally.

  virtual void		ProjDirToCurrent();
  // #CAT_File set the proj_dir for this project to be the current working directory for the file system (whenever this project enters scope, this should be called, so files are always loaded appropriately)
  virtual void          SaveRecoverFile();
  // #CAT_File Save a recover file of this project, usually called when a signal is received indicating a crash condition
  virtual void          SaveRecoverFile_strm(ostream& strm);
  // #IGNORE underlying save function to use when saving a recover file -- might want to do something special here
  virtual String        GetAutoFileName(const String& suffix, const String& ftype_ext = ".proj");
  // #CAT_File get a file name to save project to, with suffix but file extension *removed*, based on any existing file name, project name, and type
  virtual bool          AutoSave(bool force = false);
  // #CAT_File called automatically by the wait process -- if enough time has passed or force is true, save current project to an auto save backup file (file name + _autosave)

  override bool         SetFileName(const String& val);
  override int          Save_strm(ostream& strm, taBase* par=NULL, int indent=0);
  override int          Save();
  override int          SaveAs(const String& fname = "");

  virtual void          PublishDocsOnWeb(const String &repositoryName);

  override int          Load(const String& fname="", taBase** loaded_obj_ptr = NULL);
  virtual void          OpenProjectLog();
  // #CAT_File set the system event logging to record to the project name + .plog -- called for SaveAs and Load -- uses project file_name value

  virtual bool          CleanFiles();
  // #BUTTON #CAT_File remove autosave and recover files based on current file_name -- this is done by default after saving a non-recover or autosave version of the project

  virtual void          ViewLicense();
  // #BUTTON view the applicable license information for this project

  virtual int           SaveNoteChanges();
  // #CAT_File saves the project to a file using current file name, but first prompts for a text note of changes that have been made, which are registered in the ChangeLog document within the project prior to saving
  virtual int           SaveAsNoteChanges(const String& fname = "");
  // #CAT_File Saves object data to a new file -- if fname is empty, it prompts the user, but first prompts for a text note of changes that have been made, which are registered in the ChangeLog document within the project prior to saving

  virtual void          ViewProjLog();
  // #MENU #MENU_ON_View #MENU_CONTEXT #MENU_SEP_BEFORE #BUTTON #CAT_File view the current project log file in internal text viewer display
  virtual void          ViewProjLog_Editor();
  // #MENU #MENU_ON_View #MENU_CONTEXT #CAT_File view the current project log file in an external editor as specified in the preferences

  override void         Dump_Load_post();
  void                  OpenViewers(); // open any yet unopen viewers

  void  UpdateAfterEdit();
  virtual void          InitLinks_impl(); // #IGNORE use this instead of InitLinks in subclasses
  virtual void          CutLinks_impl(); // #IGNORE use this instead of CutLinks in subclasses -- you can call this first to nuke the viewers etc. before your own stuff
  TA_BASEFUNS(taProject);

protected:
  TimeUsed              auto_save_timer; // #IGNORE timer used for auto saving

  virtual void          InitLinks_post(); // #IGNORE called after all _impls (not called for undo_loading): assert things like default wizards in here
  void  CutLinks(); // don't override this -- use _impl instead
  virtual MainWindowViewer* MakeProjectBrowser_impl(); // make a standard viewer for this project type
  override int          GetOwnerEditableState_impl(int mask) const
    {return 0;} // the readonly stops here!
  void                  DoView();

private:
  void  Copy_(const taProject& cp);
  void  InitLinks(); // don't try to use this -- use _impl instead
  void  Initialize();
  void  Destroy();
};

TA_SMART_PTRS(taProject)

#endif // taProject_h
