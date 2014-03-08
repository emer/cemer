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

#ifndef taMisc_h
#define taMisc_h 1

// parent includes:

// member includes:
#include <TypeDef>
#include <taVersion>
#include <taThreadDefaults>
#include <NamedURL>
#include <NameVar_PArray>
#include <String_PArray>
#include <DumpFileCvtList>
#include <ContextFlag>

#ifndef NO_TA_BASE
#include <taLicense>
#include <ClusterSpecs>
#endif

#ifndef __MAKETA__
#include <iostream>
#include <fstream>
#ifndef NO_TA_BASE
#include <QPointer>
#endif
#endif

// declare all other types mentioned but not required to include:
class ViewColor_List; // 
class ViewBackground_List; //
class TypeDef; // 
class taBase; // 
class UserDataItemBase; // 
class MemberDef; // 
class UserDataItem_List; //
class taProject; //

#ifndef NO_TA_BASE
class QMainWindow;
#endif

typedef  void (*init_proc_t)() ;        // initialization proc

class TA_API InitProcRegistrar {
  // #IGNORE object used as a module static instance to register an init_proc
public:
  InitProcRegistrar(init_proc_t init_proc);
private:
  InitProcRegistrar(const InitProcRegistrar&);
  InitProcRegistrar& operator =(const InitProcRegistrar&);
};

#define PATH_SEP taMisc::path_sep

taTypeDef_Of(taMisc);

class TA_API taMisc {
  // #NO_TOKENS #INSTANCE global parameters and functions for the application
friend class InitProcRegistrar;
public:


  enum TypeInfo {
    MEMB_OFFSETS,               // display all including member offsets
    ALL_INFO,                   // display all type information
    NO_OPTIONS,                 // don't display options
    NO_LISTS,                   // don't display lists
    NO_OPTIONS_LISTS            // don't display options or lists
  };

  enum KeepTokens {
    Tokens,                     // keep tokens as specified by the type
    NoTokens,                   // don't keep any tokens
    ForceTokens                 // force to keep all tokens
  };

  enum SaveFormat {
    PLAIN,                      // dump files are not formatted for easy reading
    PRETTY                      // dump files should be more readable by humans
  };

  enum NoviceExpert {
    NOVICE,                      // novice mode -- default, for starting out
    EXPERT,                      // expert mode -- select this to enable expert behavior 
  };

  enum LoadVerbosity {
    QUIET,                      // don't say anything except errors
    VERSION_SKEW,               // display mismatches in names of objects in loading file that are likely due to changes in newer versions of the software
    MESSAGES,                   // display informative messages on css console during load
    TRACE,                      // and show a trace of objects loaded
    SOURCE                      // and show the source of the load as its loaded
  };

  enum AutoRevert {
    AUTO_APPLY,                 // automatically apply changes before auto-reverting
    AUTO_REVERT,                // automatically revert, losing changes
    CONFIRM_REVERT              // put up a confirmatory message before reverting
  };

  enum QuitFlag { // helps during shutdown to know whether we can cancel
    QF_RUNNING          = 0, //
    QF_USER_QUIT,       // we can still cancel while in this state of quitting
    QF_FORCE_QUIT       // too late to turn back now...
  };

  enum ConsoleType { // the type of console window and how to show it; ignored in non-gui mode (either uses OS shell, or no console, depending on startup mode)
    CT_OS_SHELL = 0, // #LABEL_OS_Shell use the operating system's shell or console (with readline library on unix)
    CT_GUI = 1, // #LABEL_Gui uses a gui-based console, either docked in the main app window, or floating (see console_options)
    CT_NONE = 4 // #NO_SHOW no console, usually only used internally, such as for batch or dmem operation
  };

  enum ConsoleOptions { // #BITS options that can be used with the console
    CO_0 = 0, // #NO_BIT #IGNORE dummy item, and to clear
    CO_GUI_TRACKING     = 0x0001, // #LABEL_Gui_Tracking in GUI mode, the console floats below the active project -- this only operates if DockRoot is not checked
    CO_GUI_DOCK         = 0x0002, // #LABEL_Gui_DockRoot in GUI mode, dock the console in the root window -- if this checked, then tracking is not done
  };

  enum ColorHints { // #BITS what types of color hinting to use in the application
    CH_EDITS            = 0x0001, // color the background of property editors according to the type of the item
    CH_BROWSER          = 0x0002 // color browser tree text according to the type of the item
  };

  enum BuildType { // #BITS what type of build this is
    BT_0                = 0x00, // #NO_BIT constant for when no other flags set
    BT_DEBUG            = 0x01, // a debug build
    BT_DMEM             = 0x02, // compiled for MPI (clustered use)
    BT_NO_GUI           = 0x04  // compiled without gui support
  };

  enum ClickStyle { // how to select editable items, such as in T3
    CS_SINGLE,  // #LABEL_Single single click opens properties
    CS_CONTEXT  // #LABEL_Context choose from context menu
  };

  enum MatrixView {     // order of display for matrix cols
    DEF_ZERO    = -1, // #IGNORE use the current global default
    BOT_ZERO,   // row zero is displayed at bottom of view (default)
    TOP_ZERO    // row zero is displayed at top of view (ex. for images)
  };

  enum ProjViewPref { // project viewing preference
    PVP_2x2,    // #LABEL_2x2_Panes one window has a browser and edit pane, the second has an edit pane and a 3-d viewer
    PVP_3PANE   // #LABEL_3_Pane one window with a browser, edit pane, and a 3d-viewer
  };

  enum ViewerOptions { // #BITS options for the viewer
    VO_0                = 0, // #IGNORE
    VO_DOUBLE_CLICK_EXP_ALL = 0X001, // #LABEL_DoubleClickExpAll double click expands or contracts all tree items -- use at your own risk on big projects...
    VO_AUTO_SELECT_NEW  = 0x002, // #LABEL_AutoSelectNew automatically select (the first) of a new tree item that is made with New or similar menu commands
    VO_AUTO_EXPAND_NEW  = 0x004, // #LABEL_AutoExpandNew automatically expand new tree items that are made with New or similar menu commands
    VO_NO_TOOLBAR = 0x008,       // #LABEL_NoToolbar do not turn on the toolbar by default in new project views
    VO_NO_TOOLBOX = 0x010,       // #LABEL_NoToolbox do not turn on the program toolbox by default in new projectd views
  };

  enum  GuiStyle {      // style options provided by the gui system (not all are available on all platforms)
    GS_DEFAULT,         // #LABEL_Default use the default style for whatever platform you're running on
    GS_PLASTIQUE,       // #LABEL_Plastique default style on linux -- a good choice to try
    GS_CLEANLOOKS,      // #LABEL_CleanLooks similar overall to plastique, with a bit more of a windows look
    GS_MOTIF,           // #LABEL_Motif a classic look from the 90's -- one of the first 3d-looks
    GS_CDE,             // #LABEL_CDE Common Desktop Environment -- a bit lighter and cleaner than MOTIF, but overall very similar to it
#ifdef TA_OS_MAC
    GS_MACINTOSH,       // #LABEL_Macintosh only available on a macintosh!
#else
    GS_MACINTOSH,       // #NO_SHOW
#endif
    GS_WINDOWS,         // #LABEL_Windows standard old-school Microsoft Windows (pre XP)
#ifdef TA_OS_WIN
    GS_WINDOWSXP,       // #LABEL_WindowsXP Windows XP look -- only available on Windows XP or higher
    GS_WINDOWSVISTA,    // #LABEL_WindowsVista Windows Vista look -- only available on Windows Vista or higher
#else
    GS_WINDOWSXP,       // #NO_SHOW
    GS_WINDOWSVISTA,    // #NO_SHOW
#endif
  };

  enum HelpDetail { // level of detail for generated help information
    HD_DEFAULT,         // standard level of help detail
    HD_DETAILS,         // full programming details -- offsets, comments, etc
  };

  static String         app_name;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_App the root name of the app, ex. "emergent" or "css"
  static String         app_prefs_key;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_App the string used for the prefs folders etc. -- emergent and css should share this, but external apps (ex physics sim) should make a new one
  static String         default_app_install_folder_name;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_App the default folder name for installation, ex. "Emergent"
  static String         org_name;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_App the name of the organization, ex. ccnlab (used mostly in Windows paths)
  static String         version;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_App version number of ta/css
  static taVersion      version_bin;
  //  #READ_ONLY #NO_SAVE #EXPERT #CAT_App version number of ta/css
  static String         svn_rev;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_App svn revision number
  static int64_t        exe_mod_time_int; // #READ_ONLY #NO_SAVE #NO_SHOW time stamp for executable file last modification date (internal seconds since jan 1 1970 time units) -- this is used as a trigger for determining when to rebuild plugins, for example
  static String         exe_mod_time; // #READ_ONLY #NO_SAVE #SHOW date and time when the executable file was last modified (installed)
  static const BuildType build_type;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_App build type, mostly for determining plugin subfolders to search
  static const String    build_str;
  // #READ_ONLY #NO_SAVE #EXPERT #CAT_App an extension string based on build type -- no extension for "release gui no-dmem" configuration -- this reflects the actual compile-time parameters, not the suffix -- see app_sufix for empirical parameters
  static String          app_suffix;
  // #READ_ONLY #NO_SAVE #EXPERT #CAT_App actual empirical suffix on the exe_cmd -- this is the official key used for plugins and prefs and other things like that
  static bool            save_old_fmt;
  // #SAVE #CAT_File save project and other 'dump' files in the old emergent format (version 2 of the file format) which was used up through version 5.0.1 -- files are unlikely to be very functional in older versions anyway, but this would allow them to at least be loaded -- only use this on a temporary basis when absolutely necessary! (note: old format files can be loaded without problem in current version)

  ////////////////////////////////////////////////////////
  //    User-tunable compute params

  static taThreadDefaults thread_defaults;
  // #SAVE #CAT_MultiProc defaults for parallel threading -- these are used to initialize values in any specific parallel threading context, as they should be specific to a given machine, not to a given model or project
  static bool           dmem_output_all;
  // #SAVE #CAT_MultiProc #EXPERT all dmem (mpi) processors generate output (with processor number as PX:) for the standard output routines (Error, Warning, Info), including program traces and verbose output -- useful for debugging dmem issues, but otherwise can be too much output

  ////////////////////////////////////////////////////////
  //    TA GUI parameters

  static GuiStyle       gui_style;      // #SAVE #CAT_GUI #DEF_GS_DEFAULT style options provided by the gui system, affecting how the widgets are drawn, etc (not all are available on all platforms) -- change only takes effect on restarting the program
  static String         font_name;      // #SAVE #CAT_GUI default font name to use
  static int            font_size;      // #SAVE #CAT_GUI default font size to use
  static String         t3d_bg_color;   // #SAVE #CAT_GUI default background color for 3d view -- standard X11 color names are supported, most of which are also web/html standard color names
  static String         t3d_text_color; // #SAVE #CAT_GUI default text color for 3d view  -- standard X11 color names are supported, most of which are also web/html standard color names
  static String         t3d_font_name;  // #SAVE #CAT_GUI #EXPERT default font name to use in the 3D display (default is Arial -- not many options supported depending on platform -- set the environment variable COIN_DEBUG_FONTSUPPORT to debug)
  static ConsoleType    console_type; // #SAVE #CAT_GUI style of the console to display -- **REQUIRES APP RESTART
  static ConsoleOptions console_options; // #SAVE #CAT_GUI options for the console **REQUIRES APP RESTART
  static String         console_font_name; // #SAVE #CAT_GUI font name for the css console
  static int            console_font_size; // #SAVE #CAT_GUI font size for the css console
  static float          doc_text_scale; // #SAVE #CAT_GUI scale factor for text displayed in doc objects (including web pages) -- multiplies base setting from font_size parameter (above), plus any doc-specific text_size parameter -- values > 1 make the text bigger, < 1 = smaller
  static int            display_width;  // #SAVE #HIDDEN #CAT_GUI width of console display (in chars) -- set automatically by gui console -- affects all Print routines, which generate strings that also show up in tool tips, dialogs, and other places
  static int            max_display_width;  // #SAVE #EXPERT #MIN_10 #CAT_GUI maximum width of console display (in chars) -- affects all Print routines, which generate strings that also show up in tool tips, dialogs, and other places -- may not want this to get too big
  static int            indent_spc;     // #SAVE #EXPERT #MIN_1 #MAX_8 #DEF_2 #CAT_GUI how many spaces to use per indent level
  static int            display_height;  // #SAVE #HIDDEN #CAT_GUI height of console display (in rows) -- set automatically by gui console -- used for paging

  static bool           emacs_mode;     // #SAVE #CAT_GUI use full emacs key bindings -- all non-conflicting emacs keys are available regardless, but with this turned on, Ctrl+V is page down instead of Paste -- use Ctrl+Y (emacs yank) for paste instead -- on a Mac, Command+V is usually paste, so Ctrl+V can be used for page down without conflict, so this setting defaults to ON for macs
  static int            undo_depth;     // #SAVE #CAT_GUI #MIN_10 how many steps of undo are maintained -- the system is very efficient so large numbers (default 100) are usually acceptable -- see Project UndoStats menu item for memory usage statistics
  static int            undo_data_max_cells; // #SAVE #CAT_GUI maximum number of cells in a data table to save an undo copy -- if above this number of cells, it won't be saved for undo (only the column structure will be retained)
  static bool           undo_debug; // #NO_SAVE #CAT_GUI #EXPERT display undo debug messages to css console and project log -- can help determine what is causing excessive latencies and unresponsiveness in the application
  static float          undo_new_src_thr; // #SAVE #CAT_GUI #EXPERT threshold for how big (as a proportion of total file size) the diff's need to get before a new undo source record is created (default of around .3 is usually fine)
  static int            auto_save_interval;     // #SAVE #CAT_GUI #EXPERT how many seconds to wait between automatic saves of opened projects that have been modified?  auto save files go to project file name + _autosave
  static int            wait_proc_delay; // #SAVE #CAT_GUI #DEF_20 #EXPERT delay in milliseconds before starting the wait processing function to process misc stuff after all of the current gui events have been processed -- a smaller number makes the system more responsive but also consumes a bit more CPU -- setting to 0 consumes a lot of CPU as the wait processing loop is constantly revisited
  static int            css_gui_event_interval; // #SAVE #CAT_GUI #DEF_200 #EXPERT how many milliseconds between processing of gui events in css -- lower number = more responsive interface, but worse performance, while things are running
  static bool           delete_prompts;  //  #SAVE #CAT_GUI #EXPERT should a prompt be provided to confirm when deleting an item?  with the undo system available, this is not neccessary
  static int            tree_indent;    // #SAVE #CAT_GUI #EXPERT number of pixels to indent in the tree browser gui interface
  static NoviceExpert   program_editor_mode;  // #SAVE #CAT_GUI #EXPERT mode for program editor behavior -- EXPERT mode hides the top mini-editor by default (just double-click on the splitter to get it back) and other advanced settings and behavior
  static int            program_editor_lines;   // #SAVE #CAT_GUI #EXPERT #MIN_4 #MAX_20 default number of lines in the mini editor within the program editor, where program elements and other objects are edited.

  static HelpDetail     help_detail;    // #SAVE #CAT_GUI #EXPERT level of detail to display in the help system

  static int            max_menu;       // #SAVE #CAT_GUI #EXPERT maximum number of items in a menu -- largely obsolete at this point
  static int            search_depth;   // #SAVE #CAT_GUI #EXPERT depth recursive find will search for a path object
  static int            color_scale_size; // #SAVE #CAT_GUI #EXPERT number of colors to put in a color scale
  static int            jpeg_quality;   // #SAVE #CAT_GUI #EXPERT jpeg quality for dumping jpeg files (1-100; 95 default) -- in general it is better to use PNG format which is lossless and yields small compressed file sizes for saved view images, etc
  static ColorHints     color_hints; // #SAVE #CAT_GUI #EXPERT what types of color hinting to use in the application
  static ProjViewPref   proj_view_pref; // #SAVE #CAT_GUI #EXPERT the default way to view projects
  static ViewerOptions  viewer_options; // #SAVE #CAT_GUI #EXPERT misc options for the viewer
#ifndef NO_TA_BASE
//NOTE: following not keeping tokens so cannot be viewed in any mode
  static ViewColor_List* view_colors;   // #NO_SAVE #NO_SHOW colors to use in the view displays -- looked up by name emitted by GetTypeDecoKey and GetStateDecoKey on objects
  static ViewBackground_List* view_backgrounds;   // #NO_SAVE #NO_SHOW backgrounds to use in the view displays -- looked up by name emitted by GetTypeDecoKey and GetStateDecoKey on objects
#endif

  static int            antialiasing_level; // #SAVE #CAT_GUI level of smoothing to perform in the 3d display -- values depend on hardware acceleration, but 2 or 4 are typical values.  1 or lower disables entirely.  modern hardware can do typically do level 4 with little slowdown in speed.
  static float          text_complexity;     // #SAVE #CAT_GUI #EXPERT complexity value (between 0 and 1) for rendering 3D text -- values above .5 are usually not noticibly better and slow rendering
  static TypeItem::ShowMembs show_gui;       // #IGNORE #CAT_GUI #EXPERT what to show in the gui -- set in main window viewer
  static TypeInfo       type_info_;     // #SAVE #CAT_GUI #EXPERT #LABEL_type_info what to show when displaying type information
  //note: 'type_info' is a reserved word in C++, it is the type of rtti data
  static KeepTokens     keep_tokens;    // #SAVE #CAT_GUI #EXPERT default for keeping tokens
  static SaveFormat     save_format;    // #SAVE #CAT_GUI #EXPERT format to use when saving things (dump files)
  static bool           auto_edit;      // #SAVE #CAT_GUI #EXPERT automatic edit dialog after creation?
  static AutoRevert     auto_revert;    // #SAVE #CAT_GUI #EXPERT when dialogs are automatically updated (reverted), what to do about changes?
  static MatrixView     matrix_view;    // #SAVE #CAT_GUI #EXPERT #DEF_BOT_ZERO whether to show matrices with 0 row at top or bottom of view
  static bool           beep_on_error; // #SAVE #DEF_false #CAT_GUI #EXPERT beep when an error message is printed on the console
  static short          num_recent_files; // #SAVE #DEF_10 #MIN_0 #MAX_50 number of recent files to save
  static short          num_recent_paths; // #SAVE #DEF_10 #MIN_0 #MAX_50 number of recent paths to save
  static short          num_browse_history; // #SAVE #DEF_20 #MIN_10 #MAX_50 number of browse history items to keep

  static bool           tree_spring_loaded; // #SAVE #CAT_GUI do the tree view folders expand during drag and drop
  static short          spring_loaded_delay; // #SAVE #DEF_1000 #MIN_500 #MAX_2000 delay in milliseconds for expanding
 ////////////////////////////////////////////////////////
  //    File/Paths Info

  static int            strm_ver;       // #READ_ONLY #NO_SAVE during dump or load, version # (app v4.x=v2 stream)
  static bool           save_compress;  // #SAVE #DEF_false #CAT_File compress by default for files that support it (ex .proj, .net)\nNOTE: starting with v4.0, compression is no longer recommended except for large weight files or large nets with saved units
#ifndef NO_TA_BASE
  static taLicense::StdLicense license_def;       // #SAVE #CAT_File default license to use for new projects that are created -- can change for specific projects -- see license field on projects
#endif
  static String         license_owner;     // #SAVE #CAT_File default legal owner of new projects that are created by this user (e.g., Regents of University of xyz) -- used for copyright and licensing information -- see project license field for where to change or update on existing projects
  static String         license_org;       // #SAVE #CAT_File default organization that actually created the project for new projects that are created by this user (e.g., MyLab at University of xyz) -- used for copyright and licensing information -- see project license field for where to change or update on existing projects -- defaults to license_owner if left blank

  static LoadVerbosity  verbose_load;   // #SAVE #CAT_File #EXPERT report the names of things during loading -- for debugging

  static String         app_dir;
  // #SHOW #READ_ONLY #CAT_File base of installed app directory -- override with "-a <path>" command line switch
  static String         app_plugin_dir;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_File location of installed system plugins
  static String         app_dir_default;
  // #OBSOLETE #NO_SHOW #READ_ONLY #NO_SAVE #CAT_File obs as of 4.0.19 -- nuke at some point
  static String         user_dir;
  // #SHOW #READ_ONLY #CAT_File location of user's home directory -- override with "-u <path>" command line switch
  static String         prefs_dir;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_File location of preference files
  static String         user_app_dir;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_File user's location of application, for Program libraries, etc. can be overridden in {APPNAME}_USER_APP_DIR env variable
  static String         user_plugin_dir;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_File location of installed user plugins
  static String         user_log_dir;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_File location of log files, such as plugin log
  static String         exe_cmd;
  // #SHOW #READ_ONLY #CAT_File executable command path and filename -- how was this program invoked (from argv0)
  static String         exe_path;
  // #SHOW #READ_ONLY #CAT_File full absoluate path to executable

  static String         web_home;
  // #NO_SAVE #READ_ONLY #SHOW #EXPERT #CAT_File url for location of main web home page for this application
  static String         web_help_wiki;
  // #NO_SAVE #READ_ONLY #SHOW #EXPERT #CAT_File wiki name for web application help such that appending the name of the object in question will produce help for that specific object or topic
  static String         web_help_general;
  // #NO_SAVE #READ_ONLY #SHOW #EXPERT #CAT_File url for general web application help, not associated with a specific object

  static NamedURL       wiki1_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NamedURL       wiki2_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NamedURL       wiki3_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NamedURL       wiki4_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NamedURL       wiki5_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NamedURL       wiki6_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NameVar_PArray wikis;
  // #NO_SAVE #READ_ONLY #HIDDEN the non-empty wiki names and urls from the above set of wikis -- for actual programmatic use -- above list is just for simple gui editing in preferences/options

#ifndef NO_TA_BASE
  static ClusterSpecs   cluster1; // #SAVE #CAT_File specifications for an available compute cluster or other similar remote computing resource
  static ClusterSpecs   cluster2; // #SAVE #CAT_File specifications for an available compute cluster or other similar remote computing resource
  static ClusterSpecs   cluster3; // #SAVE #CAT_File specifications for an available compute cluster or other similar remote computing resource
  static ClusterSpecs   cluster4; // #SAVE #CAT_File specifications for an available compute cluster or other similar remote computing resource
  static ClusterSpecs   cluster5; // #SAVE #CAT_File specifications for an available compute cluster or other similar remote computing resource
  static ClusterSpecs   cluster6; // #SAVE #CAT_File specifications for an available compute cluster or other similar remote computing resource
  static ClusterSpecs   cluster7; // #SAVE #CAT_File specifications for an available compute cluster or other similar remote computing resource
  static ClusterSpecs   cluster8; // #SAVE #CAT_File specifications for an available compute cluster or other similar remote computing resource
  static ClusterSpecs   cluster9; // #SAVE #CAT_File specifications for an available compute cluster or other similar remote computing resource
  static String_PArray  cluster_names; // #NO_SAVE #READ_ONLY #HIDDEN non-empty cluster names
  static ClusterSpecs_PArray  clusters; // #NO_SAVE #READ_ONLY #HIDDEN non-empty cluster specs
#endif

  static String         cluster_svn_path; // #SAVE #CAT_File path to the local directory where cluster run svn repositories should be checked out -- typically ~/svn_clusterun/ or something like that -- ~/ will be replaced by user's home directory -- this is where all the local copies of cluster data in the svn repository will reside

  static NamedURL       svn_repo1_url; // #SAVE #CAT_File short name and url for Subversion repository, including local directory = working copy checkout location
  static NamedURL       svn_repo2_url; // #SAVE #CAT_File short name and url for Subversion repository, including local directory = working copy checkout location
  static NamedURL       svn_repo3_url; // #SAVE #CAT_File short name and url for Subversion repository, including local directory = working copy checkout location
  static NamedURL       svn_repo4_url; // #SAVE #CAT_File short name and url for Subversion repository, including local directory = working copy checkout location
  static NamedURL       svn_repo5_url; // #SAVE #CAT_File short name and url for Subversion repository, including local directory = working copy checkout location
  static NamedURL       svn_repo6_url; // #SAVE #CAT_File short name and url for Subversion repository, including local directory = working copy checkout location
  static NameVar_PArray svn_repos;
  // #NO_SAVE #READ_ONLY #HIDDEN the non-empty repo names and urls from the above set of repos -- for actual programmatic use -- above list is just for simple gui editing in preferences/options
  static NameVar_PArray svn_wc_dirs;
  // #NO_SAVE #READ_ONLY #HIDDEN the non-empty repo names and local_dirs = working copy checkout directories from the above set of repos -- for actual programmatic use -- above list is just for simple gui editing in preferences/options

  // don't save these paths: they are generated from above which are saved, and can
  // be modified more reliably in a .cssinitrc or similar..
  static String_PArray  css_include_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths to be used for finding css files (e.g., in #include or load statements -- searched in order)
  static String_PArray  load_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths to be used for loading object files for the ta dump file system
  static NameVar_PArray prog_lib_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths/url's for specific categories of program library files (e.g., System, User, Web)
  static NameVar_PArray proj_template_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths/url's for collections of project template files (e.g., System, User, Web)
  static NameVar_PArray named_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths/url's for misc purposes -- search by name, value = path

  static DumpFileCvtList file_converters;   // #CAT_File #HIDDEN conversion parameters (from v.3 to v.4)

  static String         compress_sfx;   // #SAVE #CAT_File #EXPERT suffix to use for compressing files

  static bool           record_on;      // #CAT_File #NO_SAVE are we recording?
  static String         record_script;  // #CAT_File #NO_SAVE string to use for recording a script of interface activity (NULL if no record)

  static String         edit_cmd;       // #SAVE #CAT_File how to run editor
#ifdef TA_OS_WIN
  static String         plugin_make_env_cmd;
  // #SAVE #CAT_File command to set the environment for making (compiling) a plugin -- default is: call \"C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\vcvarsall.bat\" x86 -- change last arg to amd64 for a 64bit platform
#endif

  ////////////////////////////////////////////////////////
  //    Args

  static String_PArray  args_raw;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args raw list of arguments passed to program at startup (in order, no filtering or modification)
  static NameVar_PArray arg_names;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args conversions between arg flags (as a String in name field, e.g., -f or --file) and a canonical functional name (in value field, e.g., CssScript)
  static NameVar_PArray arg_name_descs;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args descriptions of arg names for help -- name is canonical functional name (e.g., CssScript) and value is string describing what this arg does
  static NameVar_PArray args;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args startup arguments processed by arg_names into name/value pairs -- this is the list that should be used!
  static int_PArray     args_used;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args counter for when args were actually used used -- warn about unused args
  static String_PArray  args_tmp;
  // #NO_SAVE #HIDDEN #CAT_Args temporary list of args; can be passed to GetAllArgsNamed in scripts..

  ////////////////////////////////////////////////////////
  //    DMEM: Distributed Memory

  static int            dmem_proc;
  // #READ_ONLY #EXPERT #NO_SAVE #SHOW #CAT_DMem distributed memory process number (rank in MPI, always 0 for no dmem)
  static int            dmem_nprocs;
  // #READ_ONLY #EXPERT #NO_SAVE #SHOW #CAT_DMem distributed memory number of processes (comm_size in MPI, 1 for no dmem)
  static bool           dmem_debug;
  // #SAVE #EXPERT #CAT_DMem turn on debug messages for distributed memory processing

  ////////////////////////////////////////////////////////
  //    Global State, Flags Etc

  static TypeSpace      types;          // #READ_ONLY #NO_SAVE #EXPERT list of all the active types
  static TypeSpace      aka_types;      // #READ_ONLY #NO_SAVE #EXPERT list of types that have AKA for other types that are no longer supported
  static TypeSpace      reg_funs;       // #READ_ONLY #NO_SAVE #EXPERT registered global functions that have been marked with the REG_FUN directive -- contains links to corresponding types TypeDef entries with a static MethodDef that points to the function -- just a call-out list for easy searching of only these functions
  static TypeDef*       default_scope;  // #READ_ONLY #NO_SAVE type of object to use to determine if two objects are in the same scope

  static taPtrList_impl* init_hook_list; // #IGNORE list of init hook's to call during initialization

  static bool           in_init;        // #READ_ONLY #NO_SAVE #NO_SHOW true if in ta initialization function
  static bool           in_event_loop;  // #READ_ONLY #NO_SAVE #NO_SHOW true when in the main event loop (ex. now ok to do ProcessEvents)
  static signed char    quitting;       // #READ_ONLY #NO_SAVE #NO_SHOW true, via one of QuitFlag values, once we are quitting
  static bool           not_constr;     // #READ_ONLY #NO_SAVE #NO_SHOW true if ta types are not yet constructed (or are destructed)

  static bool           use_gui;        // #READ_ONLY #NO_SAVE #NO_SHOW #EXPERT  whether the user has specified to use the gui or not (default = true)
  static bool           gui_active;     // #READ_ONLY #NO_SAVE #NO_SHOW #EXPERT if gui has actually been started up or not -- this is the one that should be checked for gui modality in all non-startup code
  static bool           interactive;    // #READ_ONLY #NO_SAVE #NO_SHOW #EXPERT if the system is in an interactive mode of operation, otherwise in batch mode running in the background -- determines whether to prompt user ever
  static bool           gui_no_win;     // #READ_ONLY #NO_SAVE #NO_SHOW #EXPERT an intermediate form of gui operation where the gui system is fully initialized, but no windows are created, and gui_active remains false -- this is useful for batch (background) jobs that need to do offscreen rendering or other gui-dependent functions
  static bool           in_dev_exe;     // #READ_ONLY #NO_SAVE #NO_SHOW are we running a development executable -- running out of the build directory of the source code -- do some things differently in this case (e.g., no plugins)
  static bool           use_plugins;    // #READ_ONLY #NO_SAVE #NO_SHOW whether to use plugins
  static bool           server_active;  // #READ_ONLY #NO_SAVE #NO_SHOW #EXPERT if remote server has been started up or not
  static ContextFlag    is_loading;     // #READ_ONLY #NO_SAVE #NO_SHOW true if currently loading an object
  static taVersion      loading_version;
  //  #READ_ONLY #NO_SAVE #EXPERT version number associated with file currently being loaded
  static ContextFlag    is_post_loading;// #READ_ONLY #NO_SAVE #NO_SHOW true if currently in the post load routine (DUMP_POST_LOAD)
  static ContextFlag    is_saving;      // #READ_ONLY #NO_SAVE #NO_SHOW true if currently saving an object
  static bool           save_use_name_paths; // #READ_ONLY #NO_SAVE #NO_SHOW use name-based paths (GetPathNames) for saving paths
  static ContextFlag    is_undo_saving; // #READ_ONLY #NO_SAVE #NO_SHOW true if currently saving an object for undo data -- objects with extensive "leaf" level data (i.e., having no signficant undoable data under them, e.g., data table rows) should NOT save that data in this context
  static ContextFlag    is_undo_loading;// #READ_ONLY #NO_SAVE #NO_SHOW true if currently loading an object from undo data
  static ContextFlag    is_duplicating; // #READ_ONLY #NO_SAVE #NO_SHOW true if currently duplicating an object
  static ContextFlag    is_changing_type;       // #READ_ONLY #NO_SAVE #NO_SHOW true if currently doing a ChangeType on object
  static ContextFlag    is_checking;    // #READ_ONLY #NO_SAVE #NO_SHOW true if currently doing batch CheckConfig on objects
  static ContextFlag    in_gui_call;    // #READ_ONLY #NO_SAVE #NO_SHOW true if we are running a function call from the gui (used to modalize warning dialogs)
  static ContextFlag    in_gui_multi_action; // #READ_ONLY #NO_SAVE #NO_SHOW we are currently in a gui multiple item action (e.g., drag/drop or cut/paste multiple items) -- good to suspend various update actions that might otherwise occur at this time.  The last item in the sequence does NOT have this flag set, so it can trigger relevant updates etc
  static ContextFlag    in_plugin_init; // #READ_ONLY #NO_SAVE #NO_SHOW true if currently loading typeinfo for a plugin
  static ContextFlag    in_shutdown;    // #READ_ONLY #NO_SAVE #NO_SHOW true if currently shutting down and cleaning up
  static ContextFlag    no_auto_expand; // #READ_ONLY #NO_SAVE #NO_SHOW true to suppress auto-expanding (esp during code that makes a lot of objs)
  static TypeDef*       plugin_loading; // #READ_ONLY #NO_SAVE #NO_SHOW the TypeDef of the plugin currently loading -- we stamp this into all formal classes

  static bool           err_cancel;     // #READ_ONLY #NO_SAVE #NO_SHOW true if currently canceling error messages
  static int64_t        err_cancel_time; // #READ_ONLY #NO_SAVE #NO_SHOW time point at which error cancel was last activated (internal seconds since jan 1 1970 time units)
  static int            err_cancel_time_thr; // #NO_SAVE #NO_SHOW threshold for how long to wait in seconds between error events to consider it part of the same sequence of errors, and thus reset the err_waitproc_cnt back to 0
  static int            err_waitproc_cnt; // #READ_ONLY #NO_SAVE #NO_SHOW count of number of times through the waitproc during err_cancel -- if enough times through, then we lift the err cancel (see err_waitproc_thr)
  static int            err_waitproc_thr; // #NO_SAVE #NO_SHOW threshold number of times through the waitproc to lift an err cancel
  static String         last_err_msg;
  // #READ_ONLY #NO_SAVE #NO_SHOW #EXPERT last message from the taMisc::Error function
  static String         last_warn_msg;
  // #READ_ONLY #NO_SAVE #NO_SHOW #EXPERT last message from the taMisc::Warning function

  static String         last_check_msg; // #READ_ONLY #NO_SAVE #EDIT_DIALOG last error, or last batch of errors (if checking) by CheckConfig
  static bool           check_quiet;    // #IGNORE mode we are in; set by CheckConfigStart
  static bool           check_confirm_success; // #IGNORE mode we are in; set by CheckConfigStart
  static bool           check_ok;       // #IGNORE cumulative AND of all nested oks
  static int            err_cnt; //  #READ_ONLY #NO_SAVE cumulative error count; can be used/reset by Server to detect for errors after it calls a routine
  static int            CheckClearErrCnt(); // gets current value, and clears

  static std::fstream   log_stream; // #IGNORE current logging output stream -- updated to project name + .plog extension whenever a program is opened or saved with a new name -- all significant events are logged to this stream via logging interface functions below
  static String         log_fname;  // #READ_ONLY #NO_SAVE current log file output name

  static String         console_chars; // #NO_SAVE #HIDDEN buffer of current console chars output -- when this gets longer than a display line, it is output

#if (defined(TA_GUI) && !(defined(__MAKETA__) || defined(NO_TA_BASE)))
  static QPointer<QMainWindow>  console_win;    // #IGNORE the console window
#endif

  static void   (*WaitProc)();
  // #IGNORE set this to a work process for idle time processing
  static bool   do_wait_proc;
  // #IGNORE any case where something is added to the wait processing queue MUST set this flag -- it is reset again at the START of the wait proc so that subsequent stuff within the waitproc can request another waitproc visit the next time through
  static void   (*ScriptRecordingGui_Hook)(bool); // #IGNORE gui callback when script starts/stops; var is 'start'

  /////////////////////////////////////////////////
  //    Configuration -- object as settings

  void  SaveConfig();
  // #CAT_Config save configuration defaults to <appdata>/taconfig file that is loaded automatically at startup
  void  LoadConfig();
  // #CAT_Config load configuration defaults from <appdata>/.taconfig file (which is loaded automatically at startup)
  static void  UpdateAfterEdit();
  // #CAT_Config called before saving and before loading -- updates any derived fields based on current settings

  /////////////////////////////////////////////////
  //    Errors, Warnings, Simple Dialogs

  static bool   ErrorCancelCheck();
  // #CAT_Dialog check if error messages have been canceled by the user
  static bool   ErrorCancelSet(bool on = true);
  // #CAT_Dialog turn on (or off) canceling of errors

  static String SuperCat(const char* a, const char* b, const char* c,
                      const char* d, const char* e, const char* f,
                      const char* g, const char* h, const char* i);
  // #CAT_Dialog concatenate strings with spaces between

  static void   Error(const char* a, const char* b=0, const char* c=0,
                      const char* d=0, const char* e=0, const char* f=0,
                      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays error either in a window+stderr if gui_active or to stderr only
  static void   Error_nogui(const char* a, const char* b=0, const char* c=0,
                            const char* d=0, const char* e=0, const char* f=0,
                            const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog explicit no-gui version of error: displays error to stderr only

#ifndef NO_TA_BASE
  static bool   TestError(const taBase* obj, bool test, const char* fun_name,
                          const char* a, const char* b=0, const char* c=0,
                          const char* d=0, const char* e=0, const char* f=0,
                          const char* g=0, const char* h=0);
  // #CAT_Dialog if test, then report error, including object name, type, and path information if non-null; returns test -- use e.g. if(taMisc::TestError(this, (condition), "fun", "msg")) return false;
  static bool   TestWarning(const taBase* obj, bool test, const char* fun_name,
                            const char* a, const char* b=0, const char* c=0,
                            const char* d=0, const char* e=0, const char* f=0,
                            const char* g=0, const char* h=0);
  // #CAT_Dialog if test, then report warning, including object name, type, and path information if non-null; returns test -- use e.g. if(taMisc::TestWarning(this, (condition), "fun", "msg")) return false;
#endif

  static void   CheckError(const char* a, const char* b=0, const char* c=0,
                      const char* d=0, const char* e=0, const char* f=0,
                      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog called by CheckConfig routines; enables batching up of errors for display

  static void   Warning(const char* a, const char* b=0, const char* c=0,
                      const char* d=0, const char* e=0, const char* f=0,
                      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays warning to stderr and/or other logging mechanism

  static void   Info(const char* a, const char* b=0, const char* c=0,
                      const char* d=0, const char* e=0, const char* f=0,
                      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays informative msg to stdout and/or other logging mechanism
  static void   DebugInfo(const char* a, const char* b=0, const char* c=0,
                          const char* d=0, const char* e=0, const char* f=0,
                          const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays informative msg to stdout and/or other logging mechanism -- always save to the log, but only displays to stdout if DEBUG mode is active
  static void   LogInfo(const char* a, const char* b=0, const char* c=0,
                        const char* d=0, const char* e=0, const char* f=0,
                        const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog easy method for calling LogEvent with discrete args like the rest of the dialog options -- just cats the args with LOG: tag and sends resulting string to LogEvent

  static int    Choice(const char* text="Choice", const char* a="Ok", const char* b=0,
                       const char* c=0, const char* d=0, const char* e=0,
                       const char* f=0, const char* g=0, const char* h=0,
                       const char* i=0);
  // #CAT_Dialog allows user to choose among different options in a popup dialog window if gui_active (else stdin/out) -- option a returns 0, b = 1, etc.

  static void   Confirm(const char* a, const char* b=0, const char* c=0,
                      const char* d=0, const char* e=0, const char* f=0,
                      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays informative msg in a dialog -- use this instead of Choice for such confirmations

  static void   LogEvent(const String& log_data);
  // #CAT_Log record data to current log stream file -- log all significant data using this (errors and warnings above are logged for example)
  static void   SetLogFile(const String& log_fname);
  // #CAT_Log set log file to given file name -- always overwrites any existing log file

  static void   EditFile(const String& filename);
  // #CAT_Dialog edit the file in the external editor

  /////////////////////////////////////////////////
  //    Global state management

  static TypeDef* FindTypeName(const String& typ_nm);
  // #CAT_GlobalState looks up typedef by name on global list of types, using AKA to find replacement types if original name not found
  static MethodDef* FindRegFunName(const String& name, int& idx);
  // #CAT_GlobalState find registered function on given list by name, idx is index of fun within list of registered functions (reg_funs)
  static MethodDef* FindRegFunAddr(ta_void_fun funa, int& idx);
  // #CAT_GlobalState #IGNORE find registered function on given list by addr, idx is index of funs within list of registered functions (reg_funs)
  static MethodDef* FindRegFunListAddr(ta_void_fun funa, const String_PArray& lst, int& lidx);
  // #CAT_GlobalState #IGNORE find registered function on given list by addr, lidx is 'index' of funs on same list
  static MethodDef* FindRegFunListIdx(int lidx, const String_PArray& lst);
  // #CAT_GlobalState #IGNORE find registered function on given list by lidx, which is 'index' of funs on same list (as returned by FindRegFunListAddr)
  static void   FlushConsole();
  // #CAT_GlobalState flush any pending console output (cout, cerr) -- call this in situations that generate a lot of console output (NOTE: output to cout, cerr is deprecated and should be avoided -- use ConsoleOutput instead)
  static bool   ConsoleOutput(const String& str, bool err = false, bool pager = true);
  // #CAT_Utility send the string one line at a time to console, optionaly using paging control to output only one page at a time to the user.  err means send to cerr or mark in red on gui console. returns true if full output was sent, false if user hit quit on pager
  static bool   ConsoleOutputChars(const String& str, bool err = false, bool pager = false);
  // #CAT_Utility send the string one line at a time to console, optionaly using paging control to output only one page at a time to the user.  err means send to cerr or mark in red on gui console. returns true if full output was sent, false if user hit quit on pager

  static int    ProcessEvents();
  // #CAT_GlobalState run any pending qt events that might need processed
  static int    RunPending();
  // #CAT_GlobalState check to see if any events are pending, and run if true -- MUCH faster than processevents, but also likely to miss some events along the way.

  static void   Busy(bool busy = true);
  // #CAT_GlobalState puts system in a 'busy' state
  static inline void    DoneBusy() {Busy(false);}
  // #CAT_GlobalState when no longer busy, call this function

  static void   CheckConfigStart(bool confirm_success = true, bool quiet = false);
  // #CAT_GlobalState we are starting checkconfig, nestable, 1st guy controls params
  static void   CheckConfigEnd(bool ok = true);
  // #CAT_GlobalState ending checkconfig, last exit handles display etc.

  static String& MallocInfo(String& strm);
  // #CAT_GlobalState generate malloc memory statistic information to given stream
  static String& PrintAllTokens(String& strm);
  // #CAT_GlobalState generate a list and count of all types that keep tokens, with a count of tokens
  static TypeItem::TypeInfoKinds TypeToTypeInfoKind(TypeDef* typ);
  // #IGNORE translate typedef to internal type info kind

#ifndef __MAKETA__
  static void   Register_Cleanup(SIGNAL_PROC_FUN_ARG(fun));
  // #IGNORE register a cleanup process in response to all terminal signals
#endif
  static void   Decode_Signal(int err);
  // #IGNORE printout translation of signal on cerr

  /////////////////////////////////////////////////
  //    Startup/Args

  static void   Initialize();
  // #IGNORE very first initialize of type system prior to loading _TA.cpp information (called by ta_TA.cpp file -- hardcoded into maketa

  static void   AddInitHook(init_proc_t init_proc);
  // #IGNORE add an init hook -- invoked by InitProccalled during module initialization, before main()

#ifndef NO_TA_BASE
  static void   Init_Hooks();
  // #IGNORE calls initialization hooks for plugins
  static void   Init_Defaults_PreLoadConfig();
  // #IGNORE sets up default parameters for taMisc settings, prior to loading from config file
  static void   Init_Defaults_PostLoadConfig();
  // #IGNORE sets up default parameters for taMisc settings, after loading from config file
  static void   Init_Args(int argc, const char* argv[]);
  // #IGNORE initialize taMisc startup argument information (note: arg_names must be initialized prior to this!)
  static void   Init_Types();
  // #IGNORE orchestrates the loading of type info (for both first startup and plugin loading) -- calls initClass methods on So Coin3d classes (and possibly other type post-init info)
  static void   Init_Types_Gui(bool gui);
  // #IGNORE does the taiType* bidding and allocating process to assign gui representations to types, members, methods, args, editors, and viewers for each different type
  static void   Init_DMem(int& argc, const char* argv[]);
  // #IGNORE initialize distributed memory stuff

  static void   HelpMsg(String& strm);
  // #CAT_Args generate a help message about program args, usage, etc

  static void   AddArgName(const String& flag, const String& name);
  // #CAT_Args add an argument flag name to be processed from startup args (e.g., flag = -f, name = CssScript; see arg_names)
  static void   AddEqualsArgName(const String& arg_name);
  // #CAT_Args add an argument that uses equals to set value, as in <arg_name>=<value> -- uses arg_name also for the logical name of the argument for later reference (without the ='s sign)
  static void   AddArgNameDesc(const String& name, const String& desc);
  // #CAT_Args add a description of an argument flag name (see arg_name_descs)

  static void   UpdateArgs();
  // #CAT_Args update arg information after adding new arg names

  static String FullArgString();
  // #CAT_Args return the full string of arguments passed to the program by the user
  static bool   FullArgStringToFile(const String& fname);
  // #CAT_Args write the full string of arguments passed to the program by the user to given file name
  static String FullArgStringName(bool exclude_flags=true, const String& exclude_names="",
                                  bool shorten_names=true, int max_len=6, int seg_len=3,
                                  int rm_vowels_thr=8,
                                  const String& nm_val_sep="_", const String& arg_sep="",
                                  const String& space_repl="", const String& period_repl="",
                                  const String& slash_repl="%");
  // #CAT_Args return user-provided args as a concatenation of name_value pairs, in a form suitable for use as a file name -- exclude_flags = do not include args that didn't have a value passed (tend to be system control flags) -- exclude_names is a comma-separated list of name strings that will be exlcuded (uses contains, so can be part of a name) -- lots of options to control output, including shortening arg names (see ShortName for meaning of max_len and seg_len, and rm_vowels_thr), separators between name and value, and between args (note: for obscure reasons, the separator for an empty string is actually the pound sign), and what to replace spaces, periods and slashes (either direction) with (these are important file-system separators)

  static bool   CheckArgByName(const String& nm);
  // #CAT_Args was the given arg name set?  updates arg used count
  static String FindArgByName(const String& nm);
  // #CAT_Args get the value for given named argument (argv[x] for unnamed args) -- updates arg used count
  static bool   GetAllArgsNamed(const String& nm, String_PArray& vals);
  // #CAT_Args get the values for all args with given name tag -- does NOT update arg used count
  static bool   CheckArgValContains(const String& vl);
  // #CAT_Args check if there is an arg that contains string fragment in its value -- updates arg used count
  static String FindArgValContains(const String& vl);
  // #CAT_Args get full arg value that contains string fragment -- updates arg used count
  static bool   ReportUnusedArgs(bool err = true);
  // #CAT_Args report all the args that have a used count of 0 -- very helpful to determine if args were passed that were not processed -- returns true if some args were unused -- if err then report using a taMisc::Error (else taMisc::Warning)
  static void   AddUserDataSchema(const String& type_name, UserDataItemBase* item);
  // #CAT_UserData adds the item as schema, putting on deferred list if type not avail yet
  static void   AddDeferredUserDataSchema(); // #IGNORE call during init to resolve
protected:
  static String_PArray* deferred_schema_names; // for early startup if type not defined yet
  static UserDataItem_List* deferred_schema_items; // for early startup if type not defined yet
public:
#endif // NO_TA_BASE

  /////////////////////////////////////////////////
  //    Commonly used utility functions on strings/arrays/values

  static void   CharToStrArray(String_PArray& sa, const char* ch);
  // #CAT_Utility convert space-delimeted character string to a string array
  static String StrArrayToChar(const String_PArray& sa);
  // #CAT_Utility convert a string array to a space-delimeted character string
  static void   ToNameValuePairs(const String& params, NameVar_PArray& nv_array);
  // #CAT_Utility takes a string and creates name/value pairs where '=' is the character between name and value

  static void   SpaceLabel(String& lbl);
  // #CAT_Utility add spaces to a label in place of _'s and upper-lower transitions

  static String LeadingZeros(int num, int len);
  // #CAT_Utility returns num converted to a string with leading zeros up to len
  static String GetSizeString(int64_t size_in_bytes, int precision = 3,
                              bool power_of_two = true);
  // #CAT_Utility returns a human-readable size value for given raw size number in bytes -- e.g., 3.2 GB for a 3.2 gigabyte size value -- if power_of_two then uses 2^n definitions of GB, MB, KB -- else uses standard SI power of ten definitions

  static String FormatValue(float val, int width, int precision);
  // #CAT_Utility format output of value according to width and precision
  static String StreamFormatFloat(float val, TypeDef::StrContext sc);
  // #CAT_Utility #IGNORE format a floating number for given streaming output context
  static String StreamFormatDouble(double val, TypeDef::StrContext sc);
  // #CAT_Utility #IGNORE format a double floating number for given streaming output context
  static void	NormalizeRealString(String& str);
  // make NaN and infinity representations consistent across platforms in real string

  static double NiceRoundNumber(double x, bool round);
  // #CAT_Utility returns a nice round number close to x -- useful for graphing axes, etc -- rounds if round is true, otherwise does ceil(ing)

  static String StringMaxLen(const String& str, int len);
  // #CAT_Utility returns string up to maximum length given (enforces string to be len or less in length)
  static String StringEnforceLen(const String& str, int len);
  // #CAT_Utility returns string enforced to given length (spaces added to make length)
  static String ShortName(const String& full_name, int max_len=6, int seg_len=3, int rm_vowels_thr=8);
  // #CAT_Utility return a shortened version of a full name -- looks for lower-upper case transitions, _'s as ways to segment names, then abbreviates segments to given segment len (only enforced if multiple segments) -- rm_vowels_thr is length above which RemoveVowels is called first
  static String RemoveVowels(const String& str);
  // #CAT_Utility remove vowels from given string -- useful for shortening while still producing legible text

  static String StringCVar(const String& str);
  // #CAT_Utility make return string in a form that would be valid as a variable name in C (i.e., alpha + numeric (not at start) + _

  static String& IndentString(String& strm, int indent)
  { return strm << String(MAX(indent,0) * indent_spc, 0, ' '); }
  // #CAT_Utility add indent to a string, using default indent spacing
  static String& CrIndentString(String& strm, int indent)
  { return strm << "\n" << String(MAX(indent,0) * indent_spc, 0, ' '); }
  // #CAT_Utility add carriage return (newline) and indent to a string, using default indent spacing
  static String& FancyPrintList(String& strm, const String_PArray& strs,
                                int indent=0, int max_col_width=20, int n_per_line = -1);
  // #CAT_Utility generate a print string from an array of strings, where all the items are lined up in columns based on the maximum width of items in the array -- if an item exceeds the max_col_width it gets multiple columns as needed -- keeps things overall more compact if there are a few outliers with very long names -- can specify per line or if -1 it is auto-computed
  static String& FancyPrintTwoCol(String& strm, const String_PArray& col1_strs,
                                  const String_PArray& col2_strs, int indent=0);
  // #CAT_Utility generate a print string from two columns of strings (must be equal in size), where the 2nd column items are all aligned with proper spacing after the first column items, based on the maximum width of items in the first columnn

  /////////////////////////////////////////////////
  //    File Paths etc

  static const String   path_sep;
  // #CAT_File #READ_ONLY #NO_SAVE normal file path separator character, ex / -- use this for construction of paths only (use qt parsing routines to parse paths)
  static String FinalPathSep(const String& in);
  // #CAT_File return string that has a valid final path separator
  static String NoFinalPathSep(const String& in);
  // #CAT_File return string that has no final path separator
  static int	PosFinalPathSep(const String& in);
  // #CAT_File position of final path separator, -1 if not found
  static String UnescapeBackslash(const String& in);
  // #CAT_File change double-backslash to a single-backslash in string

  static String GetFileFmPath(const String& path);
  // #CAT_File get file name component from full path
  static String GetDirFmPath(const String& path, int n_up = 0);
  // #CAT_File get directory component from full path, n_up is number of directories to go up from the final directory
  static String PathToUnixSep(const String& path);
  // #CAT_File convert path to unix-style / separators, which can be read on windows in any case, and don't cause escaping issues
  static bool	IsQualifiedPath(const String& fname);
  // #CAT_File true if the fname is already an absolute or qualified relative path

  static bool   FileExists(const String& filename);
  // #CAT_File returns true if the file exists in current working directory (or absolute path) -- see also DirExists for checking on directories
  static String GetTemporaryPath();
  // #CAT_File return path to system temporary file directory (e.g., /tmp) that user can write to

#ifndef NO_TA_BASE		// all of these are not for maketa

  static String ExpandFilePath(const String& path, taProject* proj = NULL);
  // #CAT_File expand any special variables or shortcuts in the file path -- CR: unpacks to the current cluster run svn directory for this project (if available), CRR:, CRM: are results and models subdirs of CR, ~ is the current user's home directory (see CompressFilePath for inverse) -- called by all standard file operations
  static String CompressFilePath(const String& path, taProject* proj = NULL);
  // #CAT_File replace path elements with special variables or shortcuts -- CR: = the current cluster run svn directory for this project (if available), CRR:, CRM: are results and models subdirs of CR, ~ is the current user's home directory (see ExpandFilePath for inverse)

  static int64_t FileSize(const String& filename);
  // #CAT_File returns size of given file (0 if it does not exist -- see also FileExists)
  static bool   FileWritable(const String& filename);
  // #CAT_File returns true if file is writable according to file system permissions
  static bool   FileReadable(const String& filename);
  // #CAT_File returns true if file is readable according to file system permissions
  static bool   FileExecutable(const String& filename);
  // #CAT_File returns true if file is executable according to file system permissions
  static bool   SetFilePermissions(const String& filename, bool user=true,
                   bool group=false, bool other=false, bool readable=true,
                   bool writable=true, bool executable=false);
  // #CAT_File set file permissions for different classes of users
  static bool   RenameFile(const String& old_filename, const String& new_filename);
  // #CAT_File rename file from old to new name in current working directory (or absolute path) -- returns success
  static bool   RemoveFile(const String& filename);
  // #CAT_File remove file with given name in current working directory (or absolute path) -- returns success
  static bool   DirExists(const String& dir);
  // #CAT_File does the directory (either full path or relative to current working directory) exist?
  static bool   MakeDir(const String& dir);
  // #CAT_File make new subdirectory in current working directory -- returns success
  static bool   MakePath(const String& path);
  // #CAT_File make full path relative to current working directory (or absolute path) including all intermediate directories along the way as needed
  static bool   MakeSymLink(const String& file_name, const String& link_name);
  // #CAT_File make a symbolic link to given file with given link name
  static bool   RemoveDir(const String& dir);
  // #CAT_File remove subdirectory in current working directory -- must be empty -- returns success
  static bool   RemovePath(const String& path);
  // #CAT_File remove full path relative to current working directory (or absolute path) including all *empty* intermediate directories along the way -- only removes directories that are empty -- returns success

  static int    ReplaceStringInFile(const String& filename, const String& search_str,
                                    const String& repl_str);
  // #CAT_File replace all occurrences of search_str with repl_str in given file -- loads file into a String, does gsub, and saves back -- may not be suitable for very large files -- returns number of replacements actually made, or -1 if there was an error in loading the file

  static String GetCurrentPath();
  // #CAT_File get current working directory path
  static bool   SetCurrentPath(const String& path);
  // #CAT_File set current working directory to given path -- returns success
  static String GetHomePath();
  // #CAT_File get user's home directory path
  static String GetUserPluginDir();
  // #CAT_File get the directory where user plugins are stored (just the dir name, not full path)
  static String GetSysPluginDir();
  // #CAT_File get the directory where system plugins are stored (just the dir name, not full path)
  static String GetDocPath();
  // #CAT_File the user's Documents path, (or home, if none defined)
  static String GetAppDataPath(const String& appname);
  // #CAT_File root for preference data, typically hidden from user
  static String GetAppDocPath(const String& appname);
  // #CAT_File root for user-visible application files
  static String FileDiff(const String& fname_a, const String& fname_b,
                         bool trimSpace = false, bool ignoreSpace = false,
                         bool ignoreCase = false);
  // #CAT_File return a string showing the differences between two files -- uses taStringDiff

#endif	// NO_TA_BASE

  static String FindFileOnPath(String_PArray& paths, const char* fname);
  // #CAT_File helper function: try to find file fnm in one of the load_include paths -- returns complete path to file (or empty str if not found)
  static String FindFileOnLoadPath(const char* fname);
  // #CAT_File try to find file fnm in one of the load_include paths -- returns complete path to file  (or empty str if not found)
  static int    GetUniqueFileNumber(int st_no, const String& prefix, const String& suffix);
  // #CAT_File get a unique file number by adding numbers in between prefix and suffix until such a file does not exist

  static String GetWikiURL(const String& wiki_name, bool add_index=true);
  // #CAT_File get the url for a given wiki name, optionally adding /index.php/ if add_index is true
  static String FixURL(const String& url_str);
  // #CAT_File do some basic things to fix a url to make it at least somewhat viable (e.g., add http:// if no 'scheme' already there, add .com if no . present in an http:// url)
  static String ExtraAppSuffix();
  // #CAT_File return any extra app suffix beyond the standard app suffixes (dbg, mpi) -- empty string if none -- parses the app_suffix member

  static bool   InternetConnected();
  // #CAT_File determine if the system has at least one active network interface -- i.e., is it connected to the internet?

  /////////////////////////////////////////////////
  // Computer system info, and timing

  static int	CpuCount();
  // #CAT_System number of physical cpus
  static String	HostName();
  // #CAT_System name of the computer
  static String	UserName();
  // #CAT_System name of user of the computer
  static int	ProcessId();
  // #CAT_System returns a process-specific Id
  static int	TickCount();
  // #CAT_System ticks since system started -- def of a 'tick' is system dependent
  static void	SleepS(int sec);
  // #CAT_System sleep the specified number of seconds
  static void	SleepMs(int msec);
  // #CAT_System sleep the specified number of milliseconds
  static int    ExecuteCommand(const String& cmd);
  // #CAT_System execute given command -- currently just uses the "system" function call on all platforms, which seems to work well


  /////////////////////////////////////////////////
  //    Recording GUI actions to css script

  static void   StartRecording();
  // #CAT_Script sets record_strm and record_cursor
  static void   StopRecording();
  // #CAT_Script unsets record_strm and record_cursor
  static bool   RecordScript(const char* cmd);
  // #CAT_Script record the given script command, if the script is open (just sends cmd to stream)
#ifndef NO_TA_BASE
  static void   ScriptRecordAssignment(taBase* tab,MemberDef* md);
  // #CAT_Script record last script assignment of tab's md value;
  static void   SRIAssignment(taBase* tab,MemberDef* md);
  // #CAT_Script record inline md assignment
  static void   SREAssignment(taBase* tab,MemberDef* md);
  // #CAT_Script record enum md assignment
#endif

  ////////////////////////////////////////////////////////////////////////
  //    File Parsing Stuff for Dump routines: Input

  static String LexBuf; // #NO_SAVE #HIDDEN a buffer, contains last thing read by read_ funs

  // return value is the next character in the stream
  // peek=true means that return value was not read, but was just peek'd

  static int    skip_white(std::istream& strm, bool peek = false);
  // #CAT_Parse skip over all whitespace
  static int    skip_white_noeol(std::istream& strm, bool peek = false);
  // #CAT_Parse don't skip end-of-line
  static int    skip_till_start_quote_or_semi(std::istream& strm, bool peek = false);
  // #CAT_Parse used to seek up to an opening " for a string; will terminate on a ;
  static int    read_word(std::istream& strm, bool peek = false);
  // #CAT_Parse reads only contiguous 'isalnum' and _ -- does skip_white first
  static int    read_nonwhite(std::istream& strm, bool peek = false);
  // #CAT_Parse read any contiguous non-whitespace string -- does skip_white first
  static int    read_nonwhite_noeol(std::istream& strm, bool peek = false);
  // #CAT_Parse read any contiguous non-whitespace string, does skip_white_noeol first (string must be on this line)
  static int    read_till_eol(std::istream& strm, bool peek = false);
  // #CAT_Parse eol = end of line
  static int    read_till_semi(std::istream& strm, bool peek = false);
  // #CAT_Parse semi = ;
  static int    read_till_lbracket(std::istream& strm, bool peek = false);
  // #CAT_Parse lbracket = {
  static int    read_till_lb_or_semi(std::istream& strm, bool peek = false);
  // #CAT_Parse lb = { or ;
  static int    read_till_rbracket(std::istream& strm, bool peek = false);
  // #CAT_Parse rbracket = } -- does depth counting to skip over intervening paired { }
  static int    read_till_rb_or_semi(std::istream& strm, bool peek = false);
  // #CAT_Parse rbracket } or ; -- does depth counting to skip over intervening paired { }
  static int    read_till_end_quote(std::istream& strm, bool peek = false); // #CAT_Parse
  // #CAT_Parse read-counterpart to write_quoted_string; read-escaping, until "
  static int    read_till_end_quote_semi(std::istream& strm, bool peek = false);
  // #CAT_Parse read-counterpart to write_quoted_string; read-escaping, until "; (can be ws btwn " and ;)
  static int    skip_past_err(std::istream& strm, bool peek = false);
  // #CAT_Parse skips to next rb or semi (robust)
  static int    skip_past_err_rb(std::istream& strm, bool peek = false);
  // #CAT_Parse skips to next rbracket (

  static int    find_not_in_quotes(const String& str, char c, int start = 0);
  // #CAT_Parse find character c in the string, starting at given index (- = from end), making sure that the character is not contained within a quoted string within the overall string

  static int    replace_strings(std::istream& istrm, std::ostream& ostrm, NameVar_PArray& repl_list);
  // #CAT_File replace a list of strings (no regexp) in input file istrm to output file ostrm (name -> value) -- reads one line at a time; returns number replaced
  static int    find_strings(std::istream& istrm, String_PArray& strs);
  // #CAT_File find first occurrence of any of the given strings in file (reading one line at a time); returns index of string or -1 if none found


  ////////////////////////////////////////////////////////////////////////
  //    HTML-style tags

  enum ReadTagStatus {
    TAG_GOT,                    // got a starting tag <xxx...>
    TAG_END,                    // got an ending tag </xx>
    TAG_NONE,                   // no start of < tag there
    TAG_EOF,                    // got an EOF
  };

  static ReadTagStatus read_tag(std::istream& strm, String& tag, String& val);
  // #CAT_Parse read an html-style tag from the file: <XXX ...> tag = XXX, val = ... (optional)
  static int    read_till_rangle(std::istream& strm, bool peek = false);
  // #CAT_Parse rangle = >

  ////////////////////////////////////////////////////////////////////////
  //    File Parsing Stuff for Dump routines: Output

  static std::ostream& indent(std::ostream& strm, int indent, int tsp=2);
  // #CAT_File generate indentation
  static std::ostream& write_quoted_string(std::ostream& strm, const String& str,
                                           bool write_if_empty = false);
  // #CAT_File writes the string, including enclosing quotes, escaping so we can read back using read_till_end_quote funcs

};

#endif // taMisc_h
