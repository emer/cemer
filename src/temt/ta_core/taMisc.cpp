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

#include "taMisc.h"

#include <TypeDef> 
#include <MemberDef> 
#include <MethodDef>
#include <BuiltinTypeDefs>
taTypeDef_Of(PropertyDef);
taTypeDef_Of(EnumDef);

#ifndef NO_TA_BASE

// this gets around cmake's internal dependency checking mechanisms, to break
// interdependency on svnrev.h
# define CMAKE_DEPENDENCY_HACK(a) #a
#  include CMAKE_DEPENDENCY_HACK(svnrev.h)
# undef CMAKE_DEPENDENCY_HACK

#include <taBase> 
#include <UserDataItemBase> 
#include <taFiler>
#include <css_machine.h>
#include <Program>
#include <taProject>
#include <taiMiscCore>
#include <UserDataItem_List>
#include <tabMisc>
#include <taRootBase>
#include <taStringDiff>
#include <iDialogChoice>
#include <iDialogLineEdit>

#include <taiMisc>
#include <taiType>
#include <taiEdit>
#include <taiMember>
#include <taiMethod>
#include <taiArgType>
#include <taiViewType>

#include "ta_type_constr.h"
#include "ta_vector_ops.h"

#include <ViewColor_List> 
#include <QFile>
#include <QDir>
#include <QPointer>
#include <cssConsoleWindow>
#include <QDateTime>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostAddress>
#include <QList>
#include <QThread>
#include <QApplication>
#endif

#include <ctime>
#include <csignal>

#ifdef TA_OS_WIN
#include "windows.h"
#include <errno.h>

#ifndef NO_TA_BASE
#include <taProject>
# include "shlobj.h"
#endif

#elif defined(TA_OS_UNIX)

#include <unistd.h>
#include <time.h>


#ifdef TA_OS_MAC
# include <sys/types.h>
# include <sys/sysctl.h>
#endif

#endif

#include <sstream>              // for FormatValue
#include <math.h>               // for NiceRoundNumber

#include "temt_version.h"

using namespace std;

InitProcRegistrar::InitProcRegistrar(init_proc_t init_proc) {
  taMisc::AddInitHook(init_proc);
}

String  taMisc::app_name = "temt"; // must be set in main.cpp
String  taMisc::app_prefs_key; // must be set in main.cpp, else defaults to app_name
String  taMisc::default_app_install_folder_name = "Emergent";
String  taMisc::app_suffix;

String          taMisc::project_author = "";
String          taMisc::author_email = "";
#ifndef NO_TA_BASE
taLicense::StdLicense taMisc::license_def = taLicense::NO_LIC;
#endif
String          taMisc::license_owner;
String          taMisc::license_org;

String  taMisc::org_name = "ccnlab";

#ifndef SVN_REV // won't be defined if svnrev.h wasn't included
#define SVN_REV -1
#endif

String  taMisc::svn_rev = String(SVN_REV);
String  taMisc::version = String(TEMT_VERSION);
taVersion taMisc::version_bin(String(TEMT_VERSION));

int64_t taMisc::exe_mod_time_int = 0;
String  taMisc::exe_mod_time;

// ugh! but easiest way to just statically set the build_type and official extender string
#ifdef DEBUG
# ifdef DMEM_COMPILE
#   ifdef NO_TA_GUI
const taMisc::BuildType taMisc::build_type = (taMisc::BuildType)(taMisc::BT_DEBUG | taMisc::BT_DMEM | taMisc::BT_NO_GUI);
const String             taMisc::build_str("nogui_dbg_mpi");
#   else
const taMisc::BuildType taMisc::build_type = (taMisc::BuildType)(taMisc::BT_DEBUG | taMisc::BT_DMEM);
const String             taMisc::build_str("dbg_mpi");
#   endif
# else
#   ifdef NO_TA_GUI
const taMisc::BuildType taMisc::build_type = (taMisc::BuildType)(taMisc::BT_DEBUG | taMisc::BT_NO_GUI);
const String             taMisc::build_str("nogui_dbg");
#   else
const taMisc::BuildType taMisc::build_type = taMisc::BT_DEBUG ;
const String             taMisc::build_str("dbg");
#   endif
# endif
#else
# ifdef DMEM_COMPILE
#   ifdef NO_TA_GUI
const taMisc::BuildType taMisc::build_type = taMisc::BT_DMEM + taMisc::BT_NO_GUI;
const String             taMisc::build_str("nogui_mpi");
#   else
const taMisc::BuildType taMisc::build_type = taMisc::BT_DMEM;
const String             taMisc::build_str("mpi");
#   endif
# else
#   ifdef NO_TA_GUI
const taMisc::BuildType taMisc::build_type = taMisc::BT_NO_GUI;
const String             taMisc::build_str("nogui");
#   else
const taMisc::BuildType taMisc::build_type = taMisc::BT_0; // default release version
const String             taMisc::build_str;
#   endif
# endif
#endif

// compiler info

#if TA_VEC_USE
#define STRINGIFY(s) XSTRINGIFY(s)
#define XSTRINGIFY(s) #s
#pragma message ("INSTRSET: " STRINGIFY(INSTRSET))
#pragma message ("TA_VEC_SIZE: " STRINGIFY(TA_VEC_SIZE))
#endif

#ifdef DEBUG
#pragma message ("DEBUG: ON")
#endif


String taMisc::compile_info;

taThreadDefaults::taThreadDefaults() {
  cpus = 1;
  n_threads = -1;
  alt_mpi = false;
}

taThreadDefaults taMisc::thread_defaults;
bool    taMisc::dmem_output_all = false;

taExpandDefaultsProject::taExpandDefaultsProject() {
  docs = 1;
  wizards = 1;
  ctrl_panels = 1;
  param_sets = 0;
  data = 2;
  programs = 2;
  viewers = 0;
  networks = 1;
  network = 1;
  specs = 1;
  layers = 0;
}
taExpandDefaultsProject taMisc::expand_defaults_project;

taExpandDefaultsEditor::taExpandDefaultsEditor() {
  objs = 1;
  types = 1;
  args = 1;
  vars = 1;
  functions = 7;
  init_code = 7;
  prog_code = 7;
  call_args = 0;
}
taExpandDefaultsEditor taMisc::expand_defaults_editor;

taExpandDefaultsNavigator::taExpandDefaultsNavigator() {
  objs = 1;
  types = 0;
  args = 0;
  vars = 0;
  functions = 1;
  init_code = 0;
  prog_code = 0;
  call_args = 0;
}
taExpandDefaultsNavigator taMisc::expand_defaults_navigator;


////////////////////////////////////////////////////////
//      TA GUI parameters

// parameters that are strictly platform specific
#ifdef TA_OS_MAC
String  taMisc::font_name = "Lucida Grande";
int     taMisc::font_size = 12;
String  taMisc::console_font_name = "Andale Mono";
int     taMisc::console_font_size = 12;
float   taMisc::doc_text_scale = 1.0f;
#elif defined(TA_OS_WIN)
String  taMisc::font_name = "Arial";
int     taMisc::font_size = 10;
String  taMisc::console_font_name = "Fixed";
int     taMisc::console_font_size = 10;
float   taMisc::doc_text_scale = 1.5f;
#else // Linux or some Unix variant
String  taMisc::font_name = "Nimbus Sans";
int     taMisc::font_size = 10;
String  taMisc::console_font_name = "LucidaTypewriter";
int     taMisc::console_font_size = 10;
float   taMisc::doc_text_scale = 1.0f;
#endif

String  taMisc::t3d_font_name = "Arial";
String  taMisc::t3d_bg_color = "white"; // was: grey80 -- white is brighter :)
String  taMisc::t3d_text_color = "black";

int     taMisc::table_font_size = taMisc::font_size;
int     taMisc::navigator_font_size = taMisc::font_size;
int     taMisc::program_font_size = taMisc::font_size;

// parameters that differ between win and unix
taMisc::ConsoleOptions taMisc::console_options = CO_PAGER;

taMisc::GuiStyle taMisc::gui_style = taMisc::GS_DEFAULT;
taMisc::AppToolbarStyle taMisc::app_toolbar_style = taMisc::TB_TEXT_UNDER_ICON;


int     taMisc::display_width = 120;
int     taMisc::max_display_width = 255;
int     taMisc::indent_spc = 2;
int     taMisc::display_height = 25;
int     taMisc::undo_depth = 100;
int     taMisc::undo_data_max_cells = 10000;
int     taMisc::auto_save_data_max_cells = 1000000;
float   taMisc::undo_new_src_thr = 0.3f;
bool    taMisc::undo_debug = false;
int     taMisc::auto_save_interval = 120;
int     taMisc::wait_proc_delay = 20;
int     taMisc::css_gui_event_interval = 200;
bool    taMisc::delete_prompts = false;
//bool  taMisc::delete_prompts = true;
int     taMisc::tree_indent = 12; // 12 is necessary in 4.7 -- otherwise widgets cut off
taMisc::HelpDetail taMisc::help_detail = taMisc::HD_DEFAULT;
taMisc::NoviceExpert taMisc::program_editor_mode = taMisc::NOVICE;
int     taMisc::program_editor_lines = 5;
int     taMisc::max_menu = 1000; // no cost now in QT for making it large..
int     taMisc::search_depth = 4;
int     taMisc::color_scale_size = 128;
int     taMisc::jpeg_quality = 95;
taMisc::ColorHints taMisc::color_hints = (taMisc::ColorHints)(taMisc::CH_EDITS | taMisc::CH_BROWSER);
//note: we actually init this in ta_project, the first time, for the user
taMisc::ViewerOptions   taMisc::viewer_options = (taMisc::ViewerOptions)(taMisc::VO_DOUBLE_CLICK_EXP_ALL | taMisc::VO_AUTO_SELECT_NEW | taMisc::VO_AUTO_EXPAND_NEW);
taMisc::EditOptions     taMisc::edit_options = EO_0;
#ifndef NO_TA_BASE
ViewColor_List* taMisc::view_colors = NULL;
ViewBackground_List* taMisc::view_backgrounds = NULL;
KeyBindings_List* taMisc::key_binding_lists = NULL;
#endif

int     taMisc::antialiasing_level = 4;
float   taMisc::text_complexity = .2f;

TypeItem::ShowMembs     taMisc::show_gui = TypeItem::NORM_MEMBS;
taMisc::TypeInfo        taMisc::type_info_ = taMisc::NO_OPTIONS_LISTS;
taMisc::KeepTokens      taMisc::keep_tokens = taMisc::Tokens;
bool                    taMisc::auto_edit = false;
taMisc::MatrixView      taMisc::matrix_view = taMisc::BOT_ZERO;
bool                    taMisc::beep_on_error = false;
short                   taMisc::num_recent_files = 10;
short                   taMisc::num_recent_paths = 10;
short                   taMisc::num_browse_history = 20;

bool                    taMisc::tree_spring_loaded = true;
short                   taMisc::spring_loaded_delay = 1000;  // milliseconds
taMisc::KeyBindingSet   taMisc::current_key_bindings = taMisc::KEY_BINDINGS_CUSTOM;


////////////////////////////////////////////////////////
//    Logging settings

bool                    taMisc::project_log = false;
bool                    taMisc::ext_messages = false;
taMisc::LoadVerbosity   taMisc::verbose_load = taMisc::QUIET;


////////////////////////////////////////////////////////
//      File/Path/Arg Info

int     taMisc::strm_ver = 2;
bool            taMisc::save_compress = false; // compression not the default in v4
taMisc::SaveFormat      taMisc::save_format = taMisc::PRETTY;

String  taMisc::app_dir; // set early in startup, algorithmically to find app dir
String  taMisc::app_plugin_dir;
String  taMisc::app_dir_default; // emergency override, obtained from user
String  taMisc::user_dir;                       // this will be set in init call
String  taMisc::prefs_dir; // this must be set at startup!
String  taMisc::user_app_dir;
String  taMisc::user_plugin_dir;
String  taMisc::user_log_dir;
String  taMisc::exe_cmd;
String  taMisc::exe_path;
String  taMisc::custom_key_file;

// note: app should set all these url's in its main or other app-specific code
String  taMisc::web_home = "https://grey.colorado.edu/emergent/index.php/Main_Page";
String  taMisc::web_help_wiki = "emergent";
String  taMisc::web_help_general = "https://grey.colorado.edu/emergent/index.php/User_hub";
String  taMisc::plib_app_wiki = "emergent";
String  taMisc::plib_sci_wiki = "CCN";
String  taMisc::plib_user_wiki;

NamedURL        taMisc::wiki1_url("emergent", "https://grey.colorado.edu/emergent");
NamedURL        taMisc::wiki2_url("CCN", "https://grey.colorado.edu/CompCogNeuro");
NamedURL        taMisc::wiki3_url;
NamedURL        taMisc::wiki4_url;
NamedURL        taMisc::wiki5_url;
NamedURL        taMisc::wiki6_url;

String  taMisc::pub_proj_page = "PublishedProjectList";
String  taMisc::pub_prog_page = "PublishedProgramList";

NameVar_PArray  taMisc::wikis;

#ifndef NO_TA_BASE
ClusterSpecs          taMisc::cluster1;
ClusterSpecs          taMisc::cluster2;
ClusterSpecs          taMisc::cluster3;
ClusterSpecs          taMisc::cluster4;
ClusterSpecs          taMisc::cluster5;
ClusterSpecs          taMisc::cluster6;
ClusterSpecs          taMisc::cluster7;
ClusterSpecs          taMisc::cluster8;
ClusterSpecs          taMisc::cluster9;

String_PArray         taMisc::cluster_names;
ClusterSpecs_PArray   taMisc::clusters;
#endif

String          taMisc::cluster_svn_path = "~/svn_clusterun";

NamedURL        taMisc::svn_repo1_url;
NamedURL        taMisc::svn_repo2_url;
NamedURL        taMisc::svn_repo3_url;
NamedURL        taMisc::svn_repo4_url;
NamedURL        taMisc::svn_repo5_url;
NamedURL        taMisc::svn_repo6_url;

NameVar_PArray  taMisc::svn_repos;
NameVar_PArray  taMisc::svn_wc_dirs;

String_PArray   taMisc::css_include_paths;
String_PArray   taMisc::load_paths;
NameVar_PArray  taMisc::prog_lib_paths;
NameVar_PArray  taMisc::proj_template_paths;
NameVar_PArray  taMisc::named_paths;

DumpFileCvtList taMisc::file_converters;

String          taMisc::compress_sfx = ".gz";
bool            taMisc::record_on = false;
String          taMisc::record_script;

// NOTE: we quote all filenames in case they have spaces
#ifdef TA_OS_WIN
//NOTE: Notepad could possibly really screw up files, because of crlf
// 'start' is the Windows equivalent of '&'.
String  taMisc::edit_cmd = "start Notepad.exe \"%s\"";
String  taMisc::plugin_make_env_cmd = "call \"C:\\Program Files\\Microsoft Visual Studio ta9.0\\VC\\vcvarsall.bat\" x86";
#else
#ifdef TA_OS_MAC
String  taMisc::edit_cmd = "emacs \"%s\" &";
#else // prob Linux, or some Unix for sure
String  taMisc::edit_cmd = "emacs \"%s\" &";
#endif
#endif

////////////////////////////////////////////////////////
//      Args

String_PArray   taMisc::args_raw;
NameVar_PArray  taMisc::arg_names;
NameVar_PArray  taMisc::arg_name_descs;
NameVar_PArray  taMisc::args;
String_PArray   taMisc::args_tmp;
int_PArray      taMisc::args_used;

////////////////////////////////////////////////////////
//      DMEM: Distributed Memory

int     taMisc::dmem_proc = 0;
int     taMisc::dmem_nprocs = 1;
bool    taMisc::dmem_debug = false;

////////////////////////////////////////////////////////
//      Global State, Flags Etc

// give the main typespace a big hash table..
TypeSpace taMisc::types("taMisc::types", 10000);
TypeSpace taMisc::aka_types("taMisc::aka_types", 100);
TypeSpace taMisc::reg_funs("taMisc::reg_funs", 100);
TypeDef*        taMisc::default_scope = NULL;

taPtrList_impl* taMisc::init_hook_list = NULL;

bool    taMisc::in_init = false;
bool    taMisc::in_event_loop = false;
signed char     taMisc::quitting = QF_RUNNING;
bool    taMisc::not_constr = true;
bool    taMisc::use_gui = false; // set to default in Init_Gui
bool    taMisc::in_dev_exe = false;
bool    taMisc::use_plugins = true;
bool    taMisc::gui_active = false;
bool    taMisc::interactive = true;
bool    taMisc::gui_no_win = false;
bool    taMisc::server_active = false; // true while connected
bool    taMisc::cluster_run = false;
ContextFlag     taMisc::is_loading;
taVersion       taMisc::loading_version;
ContextFlag     taMisc::is_post_loading;
ContextFlag     taMisc::is_saving;
bool            taMisc::save_use_name_paths;
ContextFlag     taMisc::is_undo_loading;
ContextFlag     taMisc::is_undo_saving;
ContextFlag     taMisc::is_duplicating;
ContextFlag     taMisc::is_changing_type;
ContextFlag     taMisc::is_checking;
ContextFlag     taMisc::in_gui_call;
ContextFlag     taMisc::in_gui_multi_action;
ContextFlag     taMisc::in_plugin_init;
ContextFlag     taMisc::in_shutdown;
ContextFlag     taMisc::in_waitproc;
ContextFlag     taMisc::no_auto_expand;
TypeDef*        taMisc::plugin_loading;

bool    taMisc::err_cancel = false;
int64_t taMisc::err_cancel_time = 0;
int     taMisc::err_cancel_time_thr = 2;
int     taMisc::err_waitproc_cnt = 0;
int     taMisc::err_waitproc_thr = 100;

String  taMisc::last_err_msg;
String  taMisc::last_warn_msg;

String  taMisc::last_check_msg;
bool taMisc::check_quiet = false;
bool taMisc::check_confirm_success = true;
bool taMisc::check_ok = true;

#ifndef NO_TA_BASE
String_PArray* taMisc::deferred_schema_names;
UserDataItem_List* taMisc::deferred_schema_items;
#endif

#ifdef TA_GUI
QPointer<cssConsoleWindow> taMisc::console_win = NULL;
#endif

void (*taMisc::WaitProc)() = NULL;
bool    taMisc::do_wait_proc = false;

void (*taMisc::ScriptRecordingGui_Hook)(bool) = NULL; // gui callback when script starts/stops; var is 'start'

String  taMisc::console_chars;
String  taMisc::console_hold;
bool    taMisc::console_hold_on = true;
String  taMisc::LexBuf;
int     taMisc::err_cnt;
fstream taMisc::log_stream;
String  taMisc::log_fname;

/////////////////////////////////////////////////////////////////
//              taMisc funs

/////////////////////////////////////////////////
//      Configuration -- object as settings

void taMisc::SaveConfig() {
#ifndef NO_TA_BASE
  UpdateAfterEdit();
  ++taFiler::no_save_last_fname;
  String cfgfn = prefs_dir + PATH_SEP + "options" + app_suffix;
  fstream strm;
  strm.open(cfgfn, ios::out);
  TA_taMisc.Dump_Save_Value(strm, (void*)this);
  strm.close(); strm.clear();
  --taFiler::no_save_last_fname;
#endif
}

void taMisc::LoadConfig() {
#ifndef NO_TA_BASE
  ++taFiler::no_save_last_fname;
  String cfgfn = prefs_dir + PATH_SEP + "options" + app_suffix;
  if(!QFile::exists(cfgfn)) {   // try without app suffix!
    cfgfn = prefs_dir + PATH_SEP + "options";
  }
  if(!QFile::exists(cfgfn))
    return;
  if(taMisc::dmem_proc == 0)
    std::cout << "Loading config (preferences, options) from: " << cfgfn << std::endl;
  fstream strm;
  strm.open(cfgfn, ios::in);
  if(!strm.bad() && !strm.eof())
    TA_taMisc.Dump_Load_Value(strm, (void*)this);
  strm.close();
  strm.clear();
  --taFiler::no_save_last_fname;
  UpdateAfterEdit();
#endif
}

#ifndef NO_TA_BASE
namespace {
  void addCluster(ClusterSpecs &cl) {
    if (cl.name.nonempty()) {
      cl.UpdateProcs();
      taMisc::cluster_names.Add(cl.name);
      taMisc::clusters.Add(cl);
    }
  }
  void addUrl(NameVar_PArray &arr, const NamedURL &nurl) {
    if (nurl.name.nonempty() && nurl.url.nonempty()) {
      arr.Add(NameVar(nurl.name, nurl.url));
    }
  }
  void addWcDir(NameVar_PArray &arr, const NamedURL &nurl) {
    if (nurl.name.nonempty() && nurl.url.nonempty()) {
      arr.Add(NameVar(nurl.name, nurl.local_dir)); // even if empty, keep in corresp with url
    }
  }
}
#endif

void taMisc::UpdateAfterEdit() {
#ifndef NO_TA_BASE
  if(font_name == "LucidaGrande") {
    font_name = "Lucida Grande";
  }
  if(console_font_name == "AndaleMono") {
    console_font_name = "Andale Mono";
  }

  wikis.Reset();
  addUrl(wikis, wiki1_url);
  addUrl(wikis, wiki2_url);
  addUrl(wikis, wiki3_url);
  addUrl(wikis, wiki4_url);
  addUrl(wikis, wiki5_url);
  addUrl(wikis, wiki6_url);

  cluster_names.Reset();
  clusters.Reset();
  addCluster(cluster1);
  addCluster(cluster2);
  addCluster(cluster3);
  addCluster(cluster4);
  addCluster(cluster5);
  addCluster(cluster6);
  addCluster(cluster7);
  addCluster(cluster8);
  addCluster(cluster9);

  svn_repos.Reset();
  addUrl(svn_repos, svn_repo1_url);
  addUrl(svn_repos, svn_repo2_url);
  addUrl(svn_repos, svn_repo3_url);
  addUrl(svn_repos, svn_repo4_url);
  addUrl(svn_repos, svn_repo5_url);
  addUrl(svn_repos, svn_repo6_url);

  svn_wc_dirs.Reset();
  addWcDir(svn_wc_dirs, svn_repo1_url);
  addWcDir(svn_wc_dirs, svn_repo2_url);
  addWcDir(svn_wc_dirs, svn_repo3_url);
  addWcDir(svn_wc_dirs, svn_repo4_url);
  addWcDir(svn_wc_dirs, svn_repo5_url);
  addWcDir(svn_wc_dirs, svn_repo6_url);

#endif
}

/////////////////////////////////////////////////
//      Errors, Warnings, Simple Dialogs

bool taMisc::ErrorCancelCheck() {
#ifndef NO_TA_BASE
  if(taMisc::err_cancel) {
    QDateTime tm = QDateTime::currentDateTime();
    QDateTime st;
    st.setTime_t(err_cancel_time);
    if(st.secsTo(tm) < err_cancel_time_thr) {
      taMisc::ConsoleOutputChars("+", true, false);
      err_waitproc_cnt = 0;     // reset counter and start counting again
    }
    else {
      taMisc::ConsoleOutputChars(".", true, false);
    }
    err_cancel_time = tm.toTime_t();
  }
#endif
  return taMisc::err_cancel;
}

bool taMisc::ErrorCancelSet(bool on) {
  if(on) {
    taMisc::err_cancel = true;
#ifndef NO_TA_BASE
    QDateTime tm = QDateTime::currentDateTime();
    err_cancel_time = tm.toTime_t();
#endif
    taMisc::Info("Cancelling remaining error messages in this batch");
  }
  else {
    taMisc::err_cancel = false;
    taMisc::err_waitproc_cnt = 0;
  }
  return taMisc::err_cancel;
}

int taMisc::CheckClearErrCnt() {
  int rval = err_cnt;
  err_cnt = 0;
  return rval;
}

void taMisc::Warning(const String& a, const String& b, const String& c, const String& d,
  const String& e, const String& f, const String& g, const String& h, const String& i)
{
#if defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0 && !taMisc::dmem_output_all) {
    return;
  }
#endif
  String msg = SuperCat(a, b, c, d, e, f, g, h, i);
  String wmsg;
  if(msg == taMisc::last_warn_msg) {
    wmsg = ".";
  }
  else {
    if(taMisc::InMainThread()) {
      taMisc::last_warn_msg = msg; // can only save to global for same-thread guys
    }
#ifndef NO_TA_BASE
    if(cssMisc::cur_top && cssMisc::cur_top->own_program && !taMisc::is_loading) {
      msg += String("\n") + cssMisc::GetSourceLoc(NULL);
      if(cssMisc::cur_top->own_program) {
        bool running = cssMisc::cur_top->state & cssProg::State_Run;
        cssMisc::cur_top->own_program->taWarning(cssMisc::GetSourceLn(NULL), running,
                                                 msg);
      }
    }
#endif
    wmsg = "***WARNING: " + msg;
  }
  taMisc::LogEvent(wmsg);
  taMisc::ConsoleOutput(wmsg, true, false);
}

void taMisc::Info(const String& a, const String& b, const String& c, const String& d,
  const String& e, const String& f, const String& g, const String& h, const String& i)
{
#if defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0 && !taMisc::dmem_output_all) {
    return;
  }
#endif
  String msg = SuperCat(a, b, c, d, e, f, g, h, i);
  taMisc::LogEvent(msg);
  taMisc::ConsoleOutput(msg, false, false); // no pager
}

String taMisc::SuperCat(const String& a, const String& b, const String& c,
                      const String& d, const String& e, const String& f,
                      const String& g, const String& h, const String& i)
{
  STRING_BUF(s, 250);
  s.cat(a); if(b.nonempty()) s.cat(" ").cat(b); if(c.nonempty()) s.cat(" ").cat(c);
  if(d.nonempty()) s.cat(" ").cat(d); if(e.nonempty()) s.cat(" ").cat(e);
  if(f.nonempty()) s.cat(" ").cat(f); if(g.nonempty()) s.cat(" ").cat(g);
  if(h.nonempty()) s.cat(" ").cat(h); if(i.nonempty()) s.cat(" ").cat(i);
  return s;
}

void taMisc::CheckError(const String& a, const String& b, const String& c, const String& d,
  const String& e, const String& f, const String& g, const String& h, const String& i)
{
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  //  if(taMisc::dmem_proc > 0) return;
#endif
  // always send to console
  String msg = SuperCat(a, b, c, d, e, f, g, h, i);
  String fmsg = "***CHECK ERROR: " + msg;
  taMisc::LogEvent(fmsg);
  taMisc::ConsoleOutput(fmsg, true, false); // no pager
  if (is_checking) {
    last_check_msg.cat(msg).cat("\n");
  } else {
    last_check_msg = msg;
  }
#if !defined(NO_TA_BASE)
#if defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0) return;
#endif
  if(!taMisc::interactive) {
    taMisc::Info("Quitting non-interactive job on error");
    taiMiscCore::Quit();        // all errors are *fatal* for non-interactive jobs!!!
  }
#endif
}

#ifndef NO_TA_BASE
bool taMisc::TestError_impl(const taBase* obj, bool test, const String& fun_name,
                       const String& a, const String& b, const String& c, const String& d,
                       const String& e, const String& f, const String& g, const String& h) {
  static taBase* prv_obj = NULL;  // prv means previous
  static String prv_fun;
  static String prv_a;
  
  if(!test) return false;
  
#ifdef DMEM_COMPILE
  //  if(taMisc::dmem_proc > 0) return true;
#endif
  
  if(obj) {
    // objinfo is the old style msg now just used for no_gui console output
    String pth = obj->GetPathNames();

    String objinfo = "Error in: " + obj->GetTypeDef()->name + " " + obj->GetDisplayName() + "::" + fun_name
    + "() (path: " + pth + ")\n";

    // path and method are used for the user friendly message
    String path_method = "\n\nPath: " + pth + "\n\nFrom: "
      + obj->GetTypeDef()->name + " " + obj->GetDisplayName() + "::" + fun_name + "()";
    
    if((obj == prv_obj) && (prv_fun == fun_name) && (prv_a == a)) {
      // nogui version for repeat!
      taMisc::Error_nogui(objinfo, a, b, c, d, e, f, g, h);
    }
    else {
      // default gui version
      taMisc::Error(a, b, c, d, e, f, g, h, path_method);
    }
  }
  else {
    String fn = String("Function: ") + fun_name + "()\n";
    if((prv_fun == fun_name) && (prv_a == a)) {
      // nogui version for repeat!
      taMisc::Error_nogui(fn, a, b, c, d, e, f, g, h);
    }
    else {
      // default gui version
      taMisc::Error(fn, a, b, c, d, e, f, g, h);
    }
  }
  prv_obj = const_cast<taBase*>(obj);
  prv_fun = fun_name;
  prv_a = a;
  return true;
}

bool taMisc::TestWarning_impl(const taBase* obj, bool test, const String& fun_name,
                         const String& a, const String& b, const String& c, const String& d,
                         const String& e, const String& f, const String& g, const String& h) {
  if(!test) return false;
  if(obj) {
    String objinfo = obj->GetTypeDef()->name + " " + obj->GetDisplayName() + "::" + fun_name
      + "() (path: " + obj->GetPathNames() + ")\n";
    taMisc::Warning(objinfo, a, b, c, d, e, f, g, h);
  }
  else {
    String fn = String("Function: ") + fun_name + "()\n";
    taMisc::Warning(fn, a, b, c, d, e, f, g, h);
  }
  return true;
}
#endif

void taMisc::Error_nogui(const String& a, const String& b, const String& c, const String& d,
                         const String& e, const String& f, const String& g, const String& h, const String& i)
{
  ++err_cnt;
  //  if (beep_on_error) cerr << '\a'; // BEL character
  String msg = SuperCat(a, b, c, d, e, f, g, h, i);
  String emsg;
  if(msg == taMisc::last_err_msg) {
    emsg = ".";
  }
  else {
    if(taMisc::InMainThread()) {
      taMisc::last_err_msg = msg; // can only save to global for same-thread guys
    }
#ifndef NO_TA_BASE
    if(cssMisc::cur_top) {
      msg += String("\n") + cssMisc::GetSourceLoc(NULL);
    }
#endif
    emsg = "***ERROR: " + msg;
  }
  taMisc::LogEvent(emsg);
  taMisc::ConsoleOutput(emsg, true, false);
#if !defined(NO_TA_BASE)
  if(cssMisc::cur_top) {
    if(cssMisc::cur_top->own_program) {
      bool running = cssMisc::cur_top->state & cssProg::State_Run;
      cssMisc::cur_top->own_program->taError(cssMisc::GetSourceLn(NULL), running, msg);
    }
    cssMisc::cur_top->run_stat = cssEl::ExecError; // tell css that we've got an error
    cssMisc::cur_top->exec_err_msg = msg;
  }
  if(!taMisc::interactive) {
    taMisc::Info("Quitting non-interactive job on error");
    taiMiscCore::Quit();        // all errors are *fatal* for non-interactive jobs!!!
  }
#endif
}

#ifdef TA_NO_GUI

void taMisc::Error(const String& a, const String& b, const String& c, const String& d,
  const String& e, const String& f, const String& g, const String& h, const String& i)
{
  Error_nogui(a,b,c,d,e,f,g,h,i);
}

int taMisc::Choice(const String& text, const String& a, const String& b, const String& c,
  const String& d, const String& e, const String& f, const String& g, const String& h, const String& i)
{
  int m=-1;
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0) return -1;
#endif
  {
    int   chn = 0;
    String chstr = text;
    chstr += "\n";
    if(a.nonempty()) { chstr += String("0: ") + a + "\n"; chn++; }
    if(b.nonempty()) { chstr += String("1: ") + b + "\n"; chn++; }
    if(c.nonempty()) { chstr += String("2: ") + c + "\n"; chn++; }
    if(d.nonempty()) { chstr += String("3: ") + d + "\n"; chn++; }
    if(e.nonempty()) { chstr += String("4: ") + e + "\n"; chn++; }
    if(f.nonempty()) { chstr += String("5: ") + f + "\n"; chn++; }
    if(g.nonempty()) { chstr += String("6: ") + g + "\n"; chn++; }
    if(h.nonempty()) { chstr += String("7: ") + h + "\n"; chn++; }
    if(i.nonempty()) { chstr += String("8: ") + i + "\n"; chn++; }

    int   choiceval = -1;
    while((choiceval < 0) ||  (choiceval > chn) ) {
      cout << chstr;
      String choice;
      cin >> choice;
      choiceval = atoi(choice);
    }
    m = choiceval;
  }
  return m;
}

void taMisc::Confirm(const String& a, const String& b, const String& c,
  const String& d, const String& e, const String& f, const String& g,
  const String& h, const String& i)
{
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if (taMisc::dmem_proc > 0) return;
#endif
  String msg = SuperCat(a, b, c, d, e, f, g, h, i);
  {
    taMisc::LogEvent(msg);
    taMisc::ConsoleOutput(msg, false, false);
  }
}

bool taMisc::StringPrompt(String& str_val, const String& prompt,
     const String& ok_txt, const String cancel_txt)
{
  int m=-1;
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0) return -1;
#endif
  {
    cout << prompt << endl;
    cin >> str_val;
  }
  return str_val.nonempty();
}

#else // def TA_NO_GUI

#ifndef NO_TA_BASE

void taMisc::Error(const String& a, const String& b, const String& c, const String& d,
  const String& e, const String& f, const String& g, const String& h, const String& i)
{
  ++err_cnt;
  String msg = SuperCat(a, b, c, d, e, f, g, h, i);
  String emsg;
  if(msg == taMisc::last_err_msg) {
    emsg = ".";
  }
  else {
    if(taMisc::InMainThread()) {
      taMisc::last_err_msg = msg; // can only save to global for same-thread guys
    }
#if !defined(NO_TA_BASE) 
    if(cssMisc::cur_top && cssMisc::cur_top->own_program && !taMisc::is_loading) {
      msg += String("\n\n") + cssMisc::GetSourceLoc(NULL);
    }
#endif
    emsg = "***ERROR: " + msg;
  }
  taMisc::LogEvent(emsg);
#if !defined(NO_TA_BASE)
  taMisc::ErrorCancelCheck();   // sets taMisc::err_cancel
#endif
  // if (beep_on_error) cerr << '\a'; // BEL character

  if(!taMisc::err_cancel) {
    taMisc::ConsoleOutput(emsg, true, false);
  }
#if !defined(NO_TA_BASE) 
  if(cssMisc::cur_top && !taMisc::is_loading) {
    if(cssMisc::cur_top->own_program) {
      bool running = cssMisc::cur_top->state & cssProg::State_Run;
      cssMisc::cur_top->own_program->taError(cssMisc::GetSourceLn(NULL), running, msg);
    }
    cssMisc::cur_top->run_stat = cssEl::ExecError; // tell css that we've got an error
    cssMisc::cur_top->exec_err_msg = msg;
  }
  if(!taMisc::interactive) {
    taMisc::Info("Quitting non-interactive job on error");
    taiMiscCore::Quit();        // all errors are *fatal* for non-interactive jobs!!!
  }
  else {
    if (taMisc::gui_active && !taMisc::is_loading && taMisc::InMainThread()) {
      bool cancel = iDialogChoice::ErrorDialog(NULL, msg);
      taMisc::ErrorCancelSet(cancel);
    }
  }
#endif
}

int taMisc::Choice(const String& text, const String& a, const String& b, const String& c,
  const String& d, const String& e, const String& f, const String& g, const String& h, const String& i)
{
  int m=-1;
  if(!taMisc::InMainThread()) return -1;
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0) return -1;
#endif
#if !defined(NO_TA_BASE)
  if (taMisc::gui_active) {
    String delimiter = iDialogChoice::delimiter;
    int   chn = 0;
    String chstr = delimiter;
    if(a.nonempty()) { chstr += String(a) + delimiter; chn++; }
    if(b.nonempty()) { chstr += String(b) + delimiter; chn++; }
    if(c.nonempty()) { chstr += String(c) + delimiter; chn++; }
    if(d.nonempty()) { chstr += String(d) + delimiter; chn++; }
    if(e.nonempty()) { chstr += String(e) + delimiter; chn++; }
    if(f.nonempty()) { chstr += String(f) + delimiter; chn++; }
    if(g.nonempty()) { chstr += String(g) + delimiter; chn++; }
    if(h.nonempty()) { chstr += String(h) + delimiter; chn++; }
    if(i.nonempty()) { chstr += String(i) + delimiter; chn++; }
    m = iDialogChoice::ChoiceDialog(NULL, text, chstr);
  } else
#endif
  {
    int   chn = 0;
    String chstr = text;
    chstr += "\n";
    if(a.nonempty()) { chstr += String("0: ") + a + "\n"; chn++; }
    if(b.nonempty()) { chstr += String("1: ") + b + "\n"; chn++; }
    if(c.nonempty()) { chstr += String("2: ") + c + "\n"; chn++; }
    if(d.nonempty()) { chstr += String("3: ") + d + "\n"; chn++; }
    if(e.nonempty()) { chstr += String("4: ") + e + "\n"; chn++; }
    if(f.nonempty()) { chstr += String("5: ") + f + "\n"; chn++; }
    if(g.nonempty()) { chstr += String("6: ") + g + "\n"; chn++; }
    if(h.nonempty()) { chstr += String("7: ") + h + "\n"; chn++; }
    if(i.nonempty()) { chstr += String("8: ") + i + "\n"; chn++; }

    int   choiceval = -1;
    while((choiceval < 0) ||  (choiceval > chn) ) {
      cout << chstr;
      String choice;
      cin >> choice;
      choiceval = atoi(choice);
    }
    m = choiceval;
  }
  return m;
}

void taMisc::Confirm(const String& a, const String& b, const String& c,
  const String& d, const String& e, const String& f, const String& g,
  const String& h, const String& i)
{
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if (taMisc::dmem_proc > 0) return;
#endif
  String msg = SuperCat(a, b, c, d, e, f, g, h, i);
  taMisc::LogEvent("***CONFIRM: " + msg);
  taMisc::ConsoleOutput(msg, false, false);
#if !defined(NO_TA_BASE)
  if (taMisc::gui_active) {
    iDialogChoice::ConfirmDialog(NULL, msg);
  }
#endif
}

bool taMisc::StringPrompt(String& str_val, const String& prompt, 
     const String& ok_txt, const String cancel_txt)
{
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0) return false;
#endif
  QString qval = str_val;
  bool rval = iDialogLineEdit::LineEditDialog(qval, prompt, ok_txt, cancel_txt);
  str_val = qval;
  return rval;
}

#endif // NO_TA_BASE
#endif // else TA_NO_GUI

void taMisc::DebugInfo(const String& a, const String& b, const String& c, const String& d,
       const String& e, const String& f, const String& g, const String& h, const String& i)
{
  String ad = String("*** DEBUG: ") + a;
#ifdef DEBUG
  taMisc::Info(ad, b, c, d, e, f, g, h, i);
#else
  String msg = SuperCat(ad, b, c, d, e, f, g, h, i);
  taMisc::LogEvent(msg);
#endif
}

void taMisc::LogInfo(const String& a, const String& b, const String& c, const String& d,
       const String& e, const String& f, const String& g, const String& h, const String& i)
{
  String ad = String("*** LOG: ") + a;
  String msg = SuperCat(ad, b, c, d, e, f, g, h, i);
  taMisc::LogEvent(msg);
}

void taMisc::LogEvent(const String& log_data) {
#if defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0 && !taMisc::dmem_output_all) {
    return;
  }
#endif
  String log_full;
  if(log_data.empty() || log_data == ".") {
    log_full = log_data;
  }
  else {
    time_t tmp = time(NULL);
    String tstamp = ctime(&tmp);
    tstamp = tstamp.before('\n');

    String thread_info;
    if(!taMisc::InMainThread()) {
      thread_info = String("thread ") << taMisc::CurrentThreadName() << ": ";
    }
    log_full = tstamp + ": " + thread_info + log_data;
  }
  if(taMisc::log_stream.bad()) {
    if(taMisc::gui_active) {
      cout << log_full << endl;
    }
  }
  else {
    taMisc::log_stream << log_full << endl;
  }
}

void taMisc::SetLogFile(const String& log_fn) {
  if(taMisc::log_fname == log_fn) return;
  taMisc::LogEvent("taMisc::SetLogFile -- Setting log file to: " + log_fn);
  taMisc::log_fname = log_fn;
  taMisc::log_stream.close();
  taMisc::log_stream.clear();
  taMisc::log_stream.open(log_fn, ios::out);
  if(taMisc::log_stream.bad()) {
    SetLogFileToDefault();
    taMisc::Error("taMisc::SetLogFile -- Could not open log stream as:", log_fn,
                  "reverting to default:", log_fname);
  }
  else {
    taMisc::LogEvent("taMisc::SetLogFile -- Log file opened for writing.");
  }
}

void taMisc::SetLogFileToDefault() {
  String bkup_fn = user_log_dir + "/default_project_log.plog";
  if(taMisc::log_fname == bkup_fn) return;
  taMisc::log_stream.close();
  taMisc::log_stream.clear();
  taMisc::log_stream.open(bkup_fn, ios::out);
  taMisc::log_fname = bkup_fn;
}

void taMisc::EditFile(const String& filename) {
  String edtr = taMisc::edit_cmd; //don't run gsub on the original string!
  edtr.gsub("%s", filename);
  taMisc::ExecuteCommand(edtr);
}

/////////////////////////////////////////////////
//      Global state management

TypeDef* taMisc::FindTypeName(const String& nm, bool err_not_found) {
  String use_nm = trim(nm);
  if(use_nm.startsWith("&TA_") || use_nm.startsWith("TA_")) {
    use_nm = use_nm.after("TA_");
  }
  if(use_nm.contains("::")) {
    String typnm = use_nm.before("::");
    String subnm = use_nm.after("::");
    TypeDef* typ = FindTypeName(typnm, err_not_found);
    if(!typ) {
      if(err_not_found) {
        taMisc::Error("taMisc::FindTypeName: type named:", typnm, "not found!");
      }
      return NULL;
    }
    TypeDef* sub = typ->FindSubType(subnm);
    if(!sub) {
      if(err_not_found) {
        taMisc::Error("taMisc::FindTypeName: sub type named:", subnm,
                      "not found in parent class of type:", typnm);
      }
      return NULL;
    }
    return sub;
  }
  TypeDef* typ = taMisc::types.FindName(use_nm);
  if(typ != NULL) return typ;
  for(int i=0; i<taMisc::aka_types.size; i++) {
    TypeDef* aka = taMisc::aka_types.FastEl(i);
    String aka_nm = aka->OptionAfter("AKA_");
    if(aka_nm == use_nm) {
      return aka;
    }
  }
  if(!typ && err_not_found) {
    if(!(use_nm.contains("null") || use_nm.contains("NULL"))) {
      taMisc::Error("FindGlobalTypeName: type named:", use_nm, "not found!");
    }
  }
  return NULL;
}

MethodDef* taMisc::FindRegFunName(const String& name, int& idx) {
  idx = -1;
  for(int i=0; i<reg_funs.size; i++) {
    TypeDef* rftp = taMisc::reg_funs.FastEl(i);
    if(!rftp->IsFunction() || rftp->methods.size != 1) continue; // shouldn't happen
    if(rftp->name != name) continue;
    MethodDef* fun = rftp->methods[0];
    idx = i;
    return fun;
  }
  return NULL;
}

MethodDef* taMisc::FindRegFunAddr(ta_void_fun funa, int& idx) {
  idx = -1;
  for(int i=0; i<reg_funs.size; i++) {
    TypeDef* rftp = taMisc::reg_funs.FastEl(i);
    if(!rftp->IsFunction() || rftp->methods.size != 1) continue; // shouldn't happen
    MethodDef* fun = rftp->methods[0];
    if(fun->addr == funa) {
      idx = i;
      return fun;
    }
  }
  return NULL;
}

MethodDef* taMisc::FindRegFunListAddr(ta_void_fun funa, const String_PArray& lst,
                                      int& lidx) {
  // lidx is "index" in space for items on same list
  lidx = 0;
  for(int i=0; i<reg_funs.size; i++) {
    TypeDef* rftp = taMisc::reg_funs.FastEl(i);
    if(!rftp->IsFunction() || rftp->methods.size != 1) continue; // shouldn't happen
    MethodDef* fun = rftp->methods[0];
    if(fun->CheckList(lst)) {
      if(fun->addr == funa) {
        return fun;
      }
      lidx++;
    }
  }
  lidx = -1;
  return NULL;
}

MethodDef* taMisc::FindRegFunListIdx(int lidx, const String_PArray& lst) {
  int chk = 0;
  for(int i=0; i<reg_funs.size; i++) {
    TypeDef* rftp = taMisc::reg_funs.FastEl(i);
    if(!rftp->IsFunction() || rftp->methods.size != 1) continue; // shouldn't happen
    MethodDef* fun = rftp->methods[0];
    if(fun->CheckList(lst)) {
      if(chk == lidx) {
        return fun;
      }
      chk++;
    }
  }
  return NULL;
}

void taMisc::FlushConsole() {
#ifndef NO_TA_BASE
  if(!cssMisc::TopShell) return;
  cssMisc::TopShell->FlushConsole();
#endif
}

// internal: output one line
static bool ConsoleOutputLine(const String& oneln, bool err, bool& pager, int& pageln) {
#ifndef NO_TA_BASE
  const char* prompt = "---Press Any Key to Continue, Except q = Quit, c = Continue without Paging ---";

  if(cssMisc::TopShell) {
    cssMisc::TopShell->OutputLine(oneln, err);
  }
  else {
    if(err)
      cerr << oneln << endl;
    else
      cout << oneln << endl;
  }
  if(pager && (taMisc::console_options & taMisc::CO_PAGER)) {
    pageln++;
    if(pageln >= taMisc::display_height) {
      int resp = 0;
      if(cssMisc::TopShell) {
        resp = cssMisc::TopShell->QueryForKeyResponse(prompt);
      }
      else {
        cout << prompt << endl;
        resp = cin.get();
      }
      if(resp == 'q' || resp == 'Q') {
        return false;
      }
      if(resp == 'c' || resp == 'C') {
        pager = false;
      }
      pageln = 0;               // start over
    }
  }
#else
  if(err)
    cerr << oneln << endl;
  else
    cout << oneln << endl;
#endif
  return true;
}

bool taMisc::ConsoleOutput(const String& str, bool err, bool pager) {
  // cannot directly output to console from a thread!
  // todo: need to figure out a better workaround?
  if(!taMisc::InMainThread()) {
    if(err)
      cerr << "thread " << taMisc::CurrentThreadName() << ": " << str << endl;
    else
      cout << "thread " << taMisc::CurrentThreadName() << ": " << str << endl;
    return true;
  }

  // console_hold is a String, can only do this in current thread!
  if (console_hold_on) {
    console_hold << str;
  }

  if(str.empty() || str == ".") {
    ConsoleOutputChars(str, err, pager);
  }
  
  if(!taMisc::interactive) pager = false;
  int pageln = 0;
  String rmdr;
  if(taMisc::gui_active)
    rmdr = PathsToLinks(str);
  else
    rmdr = str;

  const int min_dw = taMisc::display_width / 2;

#if defined(DMEM_COMPILE)
  if(taMisc::dmem_nprocs > 1) {
    if(!err && (taMisc::dmem_proc > 0) && !taMisc::dmem_output_all) {
      return false;
    }
    rmdr = "P" + String(taMisc::dmem_proc) + ": " + rmdr;
  }
#endif
  do {
    String curln;
    if(rmdr.contains("\n")) {
      curln = rmdr.before("\n");
      rmdr = rmdr.after("\n");
    }
    else {
      curln = rmdr;
      rmdr = _nilString;
    }
    if(curln.length() > taMisc::display_width) {
      String longln = curln;
      bool was_wrap = false;
      do {
        // Reserve two characters for the end-of-line wrap marker, plus
        // (possibly) two characters for the beginning-of line wrap marker.
        int wrap_point = taMisc::display_width - 2;
        if (was_wrap) wrap_point -= 2;
        wrap_point = MIN(wrap_point, longln.length());

        String curpt = longln.before(wrap_point);
        if(curpt.contains("<a ") && longln.contains("<a href=") &&
           longln.contains("</a>")) {
          curpt = longln.through("</a>");
          wrap_point = curpt.length();
          int ref_st = curpt.index("<a href");
          if(ref_st >= min_dw) {
            wrap_point = ref_st;
          }
        }
        else {
          if(wrap_point < longln.length() && isspace(longln[wrap_point])) {
            // good to go..
          }
          else if(wrap_point > min_dw) {
            wrap_point--;
            while(wrap_point > min_dw) {
              char c = longln[wrap_point];
              if(isspace(c))
                break;
              wrap_point--;
            }
          }
        }
        curpt = longln.before(wrap_point);
        longln = longln.from(wrap_point);
        if(was_wrap)
          curpt = "  " + curpt; // just indent -- no confusing wrap symbols
        // if(longln.nonempty())
        //   curpt += "->";
        if(!ConsoleOutputLine(curpt, err, pager, pageln))
          return false;         // user hit quit
        was_wrap = true;
      } while(longln.nonempty());
    }
    else {
      if(!ConsoleOutputLine(curln, err, pager, pageln))
        return false;           // user hit quit
    }
  } while(rmdr.nonempty());
  return true;
}

bool taMisc::ConsoleOutputChars(const String& str, bool err, bool pager) {
  console_chars << str;
  if((console_chars.length() >= taMisc::display_width-2) || str.contains("\n")) {
    ConsoleOutput(console_chars, err, pager);
    console_chars = _nilString; // reset
    return true;
  }
  return false;
}

void taMisc::SetConsoleHoldState(bool state) {
  console_hold_on = state;
}

String taMisc::GetConsoleHold() {
  return console_hold;
}

void taMisc::ClearConsoleHold() {
  console_hold = "";
}

int taMisc::ProcessEvents() {
#ifndef NO_TA_BASE
  return taiMiscCore::ProcessEvents();
#else
  return 0;
#endif
}

int taMisc::RunPending() {
#ifndef NO_TA_BASE
  return taiMiscCore::RunPending();
#else
  return 0;
#endif
}


void taMisc::Busy(bool busy) {
#ifndef NO_TA_BASE
  if (taiMC_) taiMC_->Busy_(busy);
#endif
}

void taMisc::CheckConfigStart(bool confirm_success, bool quiet) {
  // if first entry, do init stuff
  if (!taMisc::is_checking) {
    // always clear last msg, so there is no confusion after running Check
    taMisc::last_check_msg = _nilString;
    check_ok = true;
    check_quiet = quiet;
    check_confirm_success = confirm_success;
    taMisc::Busy();
  };
  ++taMisc::is_checking;
}

void taMisc::CheckConfigEnd(bool ok) {
#ifndef NO_TA_BASE
  // failure always cumulative for all nestings
  if (!ok) check_ok = false;
  // if last exit, do notify stuff
  if (--taMisc::is_checking) return; // still checking

  taMisc::DoneBusy();
  if (!check_quiet && (!check_ok || check_confirm_success)) {
    taiMC_->CheckConfigResult_(check_ok);
  }
#endif
}

#if 1
// don't other with mallinfo for
String& taMisc::MallocInfo(String& strm) {
  strm << "Sorry memory usage statistics not available for this machine\n";
  return strm;
}
#else
String& taMisc::MallocInfo(String& strm) {
  static struct mallinfo prv_mi;
#if defined(SUN4) && !defined(__CLCC__)
  struct mallinfo mi = mallinfo(0);
#else
  struct mallinfo mi = mallinfo();
#endif
  strm << "Memory Allocation Information (and change since last call):\n" <<
"arena          total space in arena            "
<<      mi.arena   << "\t(" << mi.arena - prv_mi.arena << ")\n" <<
"ordblks                number of ordinary blocks       "
<<      mi.ordblks << "\t(" << mi.ordblks - prv_mi.ordblks << ")\n" <<
"uordblks       space in ordinary blocks in use "
<<      mi.uordblks << "\t(" << mi.uordblks - prv_mi.uordblks << ")\n" <<
"fordblks       space in free ordinary blocks   "
<<      mi.fordblks << "\t(" << mi.fordblks - prv_mi.fordblks << ")\n" <<
"smblks         number of small blocks          "
<<      mi.smblks  << "\t(" << mi.smblks - prv_mi.smblks << ")\n" <<
"usmblks                space in small blocks in use    "
<<      mi.usmblks << "\t(" << mi.usmblks - prv_mi.usmblks << ")\n" <<
"fsmblks                space in free small blocks      "
<<      mi.fsmblks << "\t(" << mi.fsmblks - prv_mi.fsmblks << ")\n" <<
"hblks          number of holding blocks        "
<<      mi.hblks   << "\t(" << mi.hblks - prv_mi.hblks << ")\n" <<
"hblkhd         space in holding block headers  "
<<      mi.hblkhd  << "\t(" << mi.hblkhd - prv_mi.hblkhd << ")\n" <<
"keepcost       space penalty if keep option    "
<<      mi.keepcost << "\t(" << mi.keepcost - prv_mi.keepcost << ")\n";
  prv_mi = mi;
  return strm;
}
#endif

String& taMisc::PrintAllTokens(String& strm) {
  return types.PrintAllTokens(strm);
}

TypeItem::TypeInfoKinds taMisc::TypeToTypeInfoKind(TypeDef* td) {
  if (!td) return TypeItem::TIK_UNKNOWN;

  TypeItem::TypeInfoKinds tik;
#ifndef NO_TA_BASE
  if (td->InheritsFrom(&TA_TypeDef)) tik = TypeItem::TIK_TYPE;
  else if (td->InheritsFrom(&TA_MemberDef)) tik = TypeItem::TIK_MEMBER;
  else if (td->InheritsFrom(&TA_PropertyDef)) tik = TypeItem::TIK_PROPERTY;
  else if (td->InheritsFrom(&TA_MethodDef)) tik = TypeItem::TIK_METHOD;
  else if (td->InheritsFrom(&TA_MemberSpace)) tik = TypeItem::TIK_MEMBERSPACE;
  else if (td->InheritsFrom(&TA_MethodSpace)) tik = TypeItem::TIK_METHODSPACE;
  else if (td->InheritsFrom(&TA_PropertySpace)) tik = TypeItem::TIK_PROPERTYSPACE;
  else if (td->InheritsFrom(&TA_TypeSpace)) tik = TypeItem::TIK_TYPESPACE;
  else if (td->InheritsFrom(&TA_EnumDef)) tik = TypeItem::TIK_ENUM;
  else if (td->InheritsFrom(&TA_EnumSpace)) tik = TypeItem::TIK_ENUMSPACE;
  else if (td->InheritsFrom(&TA_TokenSpace)) tik = TypeItem::TIK_TOKENSPACE;
  else
#endif
    tik = TypeItem::TIK_UNKNOWN; // shouldn't happen
  return tik;
}

void taMisc::Register_Cleanup(SIGNAL_PROC_FUN_ARG(fun)) {
  // this should be the full set of terminal signals
//  signal(SIGHUP,  fun); // 1
//NOTE: SIGABRT is only ansi signal MS CRT really supports
  signal(SIGABRT, fun); // 6
#ifndef TA_OS_WIN
  signal(SIGILL,  fun); // 4
  signal(SIGSEGV, fun); // 11
  //  signal(SIGINT,  fun);     // 2 -- this is caught by css!!
  signal(SIGQUIT, fun); // 3
  signal(SIGBUS,  fun); // 7
  signal(SIGUSR1, fun); // 10
  signal(SIGUSR2, fun); // 12
# ifndef LINUX
  signal(SIGSYS,  fun);
# endif
  signal(SIGPIPE, fun); // 13
  signal(SIGALRM, fun); // 14
  signal(SIGTERM, fun); // 15
  signal(SIGFPE, fun);
#endif //!TA_OS_WIN
}

void taMisc::Decode_Signal(int err) {
  String emsg;
  switch(err) {
  case SIGABRT: emsg << "abort"; break;
#ifndef TA_OS_WIN
  case SIGHUP:  emsg << "hangup"; break;
  case SIGQUIT: emsg << "quit"; break;
  case SIGILL:  emsg << "illegal instruction"; break;
  case SIGBUS:  emsg << "bus error"; break;
  case SIGSEGV: emsg << "segmentation violation"; break;
# ifndef LINUX
  case SIGSYS:  emsg << "bad argument to system call"; break;
# endif
  case SIGPIPE: emsg << "broken pipe"; break;
  case SIGALRM: emsg << "alarm clock"; break;
  case SIGTERM: emsg << "software termination signal"; break;
  case SIGFPE:  emsg << "floating point exception"; break;
  case SIGUSR1: emsg << "user signal 1"; break;
  case SIGUSR2: emsg << "user signal 2"; break;
#endif  //!TA_OS_WIN
  default:      emsg << "unknown"; break;
  }
  cerr << emsg;                 // nothing better to do in this situation -- don't go to console..
}

bool taMisc::InMainThread() {
#ifndef NO_TA_BASE
  return (QThread::currentThread() == QApplication::instance()->thread());
#else
  return true;
#endif  
}

String taMisc::CurrentThreadName() {
#ifndef NO_TA_BASE
  return (String)QThread::currentThread()->objectName();
#else
  return "MainThread";
#endif
}

/////////////////////////////////////////////////
//      Startup

void taMisc::Initialize() {
  not_constr = false;

  String vec_info;
#ifdef TA_VEC_USE
  vec_info << " using SSE/AVX vectorizing, instrset: " << INSTRSET
           << " vec size: " << TA_VEC_SIZE;
#else
  vec_info << " not using vectorizing";
#endif

#ifdef TA_OS_WIN
  compile_info = "Windows Compiler: TODO need more info here!";
#else
  // linux or mac
  compile_info 
#ifdef __clang__
               << "Clang Compiler version: " << __clang_version__
#else
               << "GCC Compiler version: " << __VERSION__
#endif
               << " arch:"
#ifdef __x86_64__
               << " x86_64 64bit"
#endif
#ifdef __corei7__
               << " corei7"
#endif
#ifdef __nehalem__
               << " nehalem"
#endif
#ifdef __westmere__
               << " westmere"
#endif
#ifdef __sandybridge__
               << " sandybridge"
#endif
#ifdef __ivybridge__
               << " ivybridge"
#endif
#ifdef __haswell__
               << " haswell"
#endif
#ifdef __broadwell__
               << " broadwell"
#endif
#ifdef __athlon__
               << " althlon"
#endif
#ifdef __opteron__
               << " opteron"
#endif
#ifdef __barcelona__
               << " barcelona"
#endif
               << vec_info;
#endif // TA_OS_WIN
}

void taMisc::AddInitHook(init_proc_t init_proc) {
  if(!init_hook_list)
    init_hook_list = new taPtrList_impl;
  init_hook_list->Add_((void*)init_proc);
}

#ifndef NO_TA_BASE

void taMisc::Init_Hooks() {
  if(!init_hook_list) return;
  for (int i = 0; i < init_hook_list->size; ++i) {
    init_proc_t ip = (init_proc_t)init_hook_list->FastEl_(i);
    ip();
  }
}

void taMisc::Init_Defaults_PreLoadConfig() {
  thread_defaults.cpus = taMisc::CpuCount();
}

void taMisc::Init_Defaults_PostLoadConfig() {
  // set any default settings after loading config file (ensures certain key settings in place)
  css_include_paths.AddUnique(app_dir + PATH_SEP + "css_lib");
  css_include_paths.AddUnique(user_app_dir + PATH_SEP + "css_lib");
  css_include_paths.AddUnique(user_app_dir); // for .init files in user app dir
  css_include_paths.AddUnique(user_dir); // needed for .init files **DEPRECATED**

  prog_lib_paths.AddUnique(NameVar("UserLib",
    (Variant)(user_app_dir + PATH_SEP + "prog_lib")));
  prog_lib_paths.AddUnique(NameVar("SystemLib",
    (Variant)(app_dir + PATH_SEP + "prog_lib")));
  prog_lib_paths.AddUnique(NameVar("WebAppLib",
    (Variant)(user_app_dir + PATH_SEP + "web_app_prog_lib")));
  prog_lib_paths.AddUnique(NameVar("WebSciLib",
    (Variant)(user_app_dir + PATH_SEP + "web_sci_prog_lib")));
  prog_lib_paths.AddUnique(NameVar("WebUserLib",
    (Variant)(user_app_dir + PATH_SEP + "web_user_prog_lib")));

  proj_template_paths.AddUnique(NameVar("UserLib",
    (Variant)(user_app_dir + PATH_SEP + "proj_templates")));
  proj_template_paths.AddUnique(NameVar("SystemLib",
    (Variant)(app_dir + PATH_SEP + "proj_templates")));
  proj_template_paths.AddUnique(NameVar("WebAppLib",
    (Variant)(user_app_dir + PATH_SEP + "web_app_proj_templates")));
  proj_template_paths.AddUnique(NameVar("WebSciLib",
    (Variant)(user_app_dir + PATH_SEP + "web_sci_proj_templates")));
  proj_template_paths.AddUnique(NameVar("WebUserLib",
    (Variant)(user_app_dir + PATH_SEP + "web_user_proj_templates")));

  String curdir = QDir::currentPath();
  taMisc::load_paths.AddUnique(curdir);

  // max_cpu
  int max_cpus = FindArgByName("MaxCpus").toInt(); // 0 if doesn't exist
  if ((max_cpus > 0) && (max_cpus <= taMisc::CpuCount())) {
    thread_defaults.cpus = max_cpus;
    taMisc::Info("Set threads cpus:", String(max_cpus));
  }

  if(thread_defaults.cpus > taMisc::CpuCount())
    thread_defaults.cpus = taMisc::CpuCount();

  if(thread_defaults.n_threads == -1)
    thread_defaults.n_threads = MIN(thread_defaults.cpus, 4); // max of 4 by default

  int n_threads = FindArgByName("NThreads").toInt(); // 0 if doesn't exist
  if(n_threads > 0) {
    thread_defaults.n_threads = n_threads;
    taMisc::Info("Set n_threads to:", String(n_threads));
  }
  if(thread_defaults.n_threads > thread_defaults.cpus)
    thread_defaults.n_threads = thread_defaults.cpus;

  UpdateAfterEdit();
}

void taMisc::Init_Args(int argc, const char* argv[]) {
  for(int i=0;i<argc;i++) {
    String av = argv[i];
    if(av.length() == 0) continue;
    while(av.contains(" -")) { // sometimes multiple flag args get munged together in scripts
      String frst = av.before(" -");
      av = av.from(" -");
      av = av.after(0);         // skip space
      args_raw.Add(frst);
    }
    args_raw.Add(av);
  }
  UpdateArgs();
}

void taMisc::Init_Types() {
  // does full loading of types -- first check for first run
  if(TypeDefInitRegistrar::instances_already_processed == 0) {
    taMisc::Initialize();
    tac_AddBuiltinTypeDefs();    // adds to taMisc::types
  }

  TypeDefInitRegistrar::CallAllTypeInitFuns();
  TypeDefInitRegistrar::CallAllDataInitFuns();

  for(int i=TypeDefInitRegistrar::types_list_last_size; i< taMisc::types.size; i++) {
    TypeDef* td = taMisc::types[i];
    td->AddParentData();        // recursive, adds in right order..
  }

  // only call inst once all the type information is fully in place!
  TypeDefInitRegistrar::CallAllInstInitFuns();

  // finally, once the base offsets are in place, update the member base offsets
  for(int i=TypeDefInitRegistrar::types_list_last_size; i< taMisc::types.size; i++) {
    TypeDef* td = taMisc::types[i];
    if(!td->IsActualClass()) continue;
    td->ComputeMembBaseOff();
  }

  // initialize all classes that have an initClass method (ex. Inventor subtypes)
  // todo: remove this after TA_QT3D is only thing used
  if(taMisc::use_gui) {
    for (int i = TypeDefInitRegistrar::types_list_last_size; i < types.size; ++i) {
      TypeDef* typ = types.FastEl(i);
      if(!typ->IsActualClassNoEff()) continue;
      typ->CallInitClass();
    }
  }

  // add any Schema that couldn't be added earlier
  AddDeferredUserDataSchema();
  // other stuff could happen here..

  // go through all types and create list of AKA typedefs, and optimize stuff
  for (int i = TypeDefInitRegistrar::types_list_last_size; i < types.size; ++i) {
    TypeDef* typ = types.FastEl(i);
    String aka = typ->OptionAfter("AKA_");
    if(aka.nonempty()) {
      // taMisc::Info("aka type: ", typ->name, "aka:", aka);
      aka_types.Link(typ);
    }

    typ->members.BuildHashTable(typ->members.size);
    typ->methods.BuildHashTable(typ->methods.size);
    typ->CacheParents();
  }

  Init_Types_Gui(taMisc::use_gui);
  
  TypeDefInitRegistrar::types_list_last_size = types.size;
  TypeDefInitRegistrar::instances_already_processed =
    TypeDefInitRegistrar::instances->size;
}

void taMisc::Init_Types_Gui(bool gui) {
  int i,j,k;
  TypeDef* td;

  TypeSpace i_type_space;
  TypeSpace v_type_space;
  TypeSpace i_memb_space;
  TypeSpace i_meth_space;
  TypeSpace i_edit_space;

  for (i=0; i < taMisc::types.size; ++i) {
    td = taMisc::types.FastEl(i);

    // generate a list of all the qt types
    if (td->instance) {
      if (gui && td->InheritsFrom(TA_taiType)
         && !(td->InheritsFrom(TA_taiMember) || td->InheritsFrom(TA_taiMethod) ||
              td->InheritsFrom(TA_taiArgType) || td->InheritsFrom(TA_taiEdit)))
        i_type_space.Link(td);

      // generate a list of all the view types
      if (td->InheritsFrom(TA_taiViewType))
        v_type_space.Link(td);

      // generate a list of all the member_i types
      if (gui && td->InheritsFrom(TA_taiMember))
        i_memb_space.Link(td);

      // generate a list of all the method_i types
      if (gui && td->InheritsFrom(TA_taiMethod))
        i_meth_space.Link(td);

      // generate a list of all the method arg types to be used later
      if (gui && td->InheritsFrom(TA_taiArgType))
        taiMisc::arg_types.Link(td);

      // generate a list of all the ie types (edit dialogs)
      if (gui && td->InheritsFrom(TA_taiEdit))
        i_edit_space.Link(td);
    }
  }

  if (gui && (i_type_space.size == 0))
    taMisc::Error("taMisc::Init_Types_Gui: warning: no taiType's found with instance != NULL");
  if (gui && (i_memb_space.size == 0))
    taMisc::Error("taMisc::Init_Types_Gui: warning: no taiMembers's found with instance != NULL");
  if (gui && (i_edit_space.size == 0))
    taMisc::Error("taMisc::Init_Types_Gui: warning: no taiEdit's found with instance != NULL");

  // go through all the types and assign the highest bid for the it, iv, and ie
  int bid;
  for (i=TypeDefInitRegistrar::types_list_last_size; i < taMisc::types.size; ++i) {
    td = taMisc::types.FastEl(i);
    if (gui) {
      for (j=0; j <i_type_space.size; ++j) {
        taiType* tit_i = (taiType*) i_type_space.FastEl(j)->GetInstance();
        bid = tit_i->BidForType(td);
        if (bid > 0) {
          taiType* tit = tit_i->TypeInst(td); // make one
          tit->bid = bid;
          tit->AddToType(td);             // add it
        }
      }
    }

    for (j=0; j < v_type_space.size; ++j) {
      taiViewType* tit_v = (taiViewType*) v_type_space.FastEl(j)->GetInstance();
      bid = tit_v->BidForView(td);
      if (bid > 0) {
        taiViewType* tiv = tit_v->TypeInst(td); // make one
        tiv->bid = bid;
        tiv->AddView(td);               // add it
      }
    }

    if (gui) {
      for (j=0; j < i_edit_space.size; ++j) {
        taiEdit* tie_i = (taiEdit*) i_edit_space.FastEl(j)->GetInstance();
        bid = tie_i->BidForEdit(td);
        if (bid > 0) {
          taiEdit* tie = tie_i->TypeInst(td);
          tie->bid = bid;
          tie->AddEdit(td);
        }
      }
    }

    // go though all the types and find the ones that are classes
    // for each class type go through the members and assign
    // the highest bid for the member's it (may be based on opts field)
    // and do the enum types since they are not global and only on members

    if (gui && td->IsActualClassNoEff()) {
      for (j=0; j < td->members.size; ++j) {
        MemberDef* md = td->members.FastEl(j);
        if (md->owner->owner != td) continue; // if we do not own this mdef, skip
        for (k=0; k < i_memb_space.size; ++k) {
          taiMember* tim_i = (taiMember*) i_memb_space.FastEl(k)->GetInstance();
          bid = tim_i->BidForMember(md,td);
          if (bid > 0) {
            taiMember* tim = tim_i->MembInst(md,td);
            tim->bid = bid;
            tim->AddMember(md);
          }
        }
      }

      for(j=0; j < td->sub_types.size; ++j) {
        TypeDef* subt = td->sub_types.FastEl(j);
        for(k=0; k < i_type_space.size; ++k) {
          taiType* tit_i = (taiType*) i_type_space.FastEl(k)->GetInstance();
          bid = tit_i->BidForType(subt);
          if (bid > 0) {
            taiType* tit = tit_i->TypeInst(subt); // make one
            tit->bid = bid;
            tit->AddToType(subt);               // add it
          }
        }
      }

      // only assign method im's to those methods that do better than the default
      // (which has a value of 0).  Thus, most methods don't generate a new object here

      for (j=0; j < td->methods.size; ++j) {
        MethodDef* md = td->methods.FastEl(j);
        if (md->owner->owner != td) continue; // if we do not own this mdef, skip
        for (k=0; k < i_meth_space.size; ++k) {
          taiMethod* tim_i = (taiMethod*) i_meth_space.FastEl(k)->GetInstance();
          bid = tim_i->BidForMethod(md,td);
          if (bid > 0) {
            taiMethod* tim = tim_i->MethInst(md,td);
            tim->bid = bid;
            tim->AddMethod(md);
          }
        }
      }

#if 0
      // do check for out-of-order methods or members -- key for using index-based optimization -- now fixed..
      for(int pi=0; pi<MIN(1,td->parents.size); pi++) { // only first parent
        TypeDef* par = td->parents.FastEl(pi);
        bool got_mm = false;
        for(int j=0; j<td->methods.size; j++) {
          String mm = td->methods.FastEl(j)->name;
          if(j < par->methods.size) {
            String pm = par->methods.FastEl(j)->name;
            if(pm != mm) {
              taMisc::Info("meth mismatch:", td->name, par->name, String(j), mm, pm);
              got_mm = true;
            }
          }
          else if(got_mm) {
            taMisc::Info("meth mismatch:", td->name, par->name, String(j), mm, "<none>");
          }
        }

        got_mm = false;
        for(int j=0; j<td->members.size; j++) {
          String mm = td->members.FastEl(j)->name;
          if(j < par->members.size) {
            String pm = par->members.FastEl(j)->name;
            if(pm != mm) {
              taMisc::Info("memb mismatch:", td->name, par->name, String(j), mm, pm);
              got_mm = true;
            }
          }
          else if(got_mm) {
            taMisc::Info("memb mismatch:", td->name, par->name, String(j), mm, "<none>");
          }
        }

      }
#endif

    } // td->IsActualClassNoEff()

  } // for each Type

  // link in compatible members as properties of every type
  // "compatible" means that it isn't already a property, and is otherwise accessible
  for (i=TypeDefInitRegistrar::types_list_last_size; i < taMisc::types.size; ++i) {
    td->SetInitFlag(TypeDef::IF_GUI_INIT);
    td = taMisc::types.FastEl(i);
    for (int j = 0; j < td->members.size; ++j) {
      MemberDef* md = td->members.FastEl(j);
      MemberDefBase* pd = td->properties.FindName(md->name);
      if (pd) continue; // already exists
      // ok, see if it is "compatible"
      if (!md->type->IsVarCompat()) continue; // can't read/write by Variant
      // TODO: anything????
      td->properties.Link(md);
    }
  }
}

void taMisc::AddUserDataSchema(const String& type_name, UserDataItemBase* item) {
  // calls could come very early in startup, even before .types or its contents
  // are created (static data gets created in "random" order in C++)
  // so we will add now if the type exists, otherwise we add to a deferred list
  TypeDef* typ = NULL;
  if (types.size > 0) {
    typ = FindTypeName(type_name);
  }
  if (typ) {
    typ->AddUserDataSchema(item);
  }
  else { // too early, or bad type_name
    if (!deferred_schema_names) {
      deferred_schema_names = new String_PArray;
      deferred_schema_items = new UserDataItem_List;
    }
    taBase::Ref(item); // extra ref so doesn't get deleted later
    deferred_schema_names->Add(type_name);
    deferred_schema_items->Link(item); // don't do the owner thang...
  }
}

void taMisc::AddDeferredUserDataSchema() {
  if (!deferred_schema_names) return;
  String type_name;
  while (deferred_schema_names->size > 0) {
    type_name = deferred_schema_names->Pop();
    //note: has an extra refn, so not deleted on Pop
    UserDataItemBase* item = deferred_schema_items->Pop();
    TypeDef* typ = FindTypeName(type_name);
    if (typ) {
      typ->AddUserDataSchema(item);
    }
    else {
      Warning("Type:", type_name, " not found trying to add UserDataSchema for key:",
        item->name);
    }
    taBase::UnRef(item); // only deleted if not added to new list
  }
  // don't need these anymore
  delete deferred_schema_names;
  deferred_schema_names = NULL;
  delete deferred_schema_items;
  deferred_schema_items = NULL;
}

void taMisc::Init_DMem(int& argc, const char* argv[]) {
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  MPI_Init(&argc, (char***)&argv); // note mpi's extra level of indirection
  MPI_Comm_size(MPI_COMM_WORLD, &dmem_nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &dmem_proc);
  MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
  if(dmem_proc == 0) {
    cerr << "DMEM Running on " << dmem_nprocs << " processors." << endl;
  }
#endif
}

void taMisc::SetKeyBindingSet(KeyBindingSet kb_set) {
  current_key_bindings = kb_set;
}

void taMisc::HelpMsg(String& strm) {
  strm << "TA/CSS Help Info, version: " << version << "\n";
  strm << "Startup arguments: " << "\n";
  for(int i=0;i<arg_names.size; i++) {
    NameVar nv = arg_names.FastEl(i);
    Variant dvar = arg_name_descs.GetVal(nv.value.toString());
    String desc = dvar.toString();
    strm << "  " << nv.name << desc << "\n";
  }
}

void taMisc::UpdateArgs() {
  args.Reset();
  for(int i=0;i<args_raw.size;i++) {
    String av = args_raw[i];
    NameVar nv;
    if(av[0] == '-') { // a flag
      Variant vl = arg_names.GetVal(av);
      if(vl.isNull()) {
        nv.name = av;
      }
      else {
        nv.name = vl.toString();
      }
      if(i < args_raw.size-1) {
        String nxt = args_raw[i+1];
        if((nxt[0] != '-') && !nxt.contains('=')) {
          // not another flag or n=v; treat as value for this guy
          nv.value = nxt;
        }
        // don't consume this arg in any case: it will show up as argv[i+1] too
      }
    }
    else if(av.contains('=')) { // name=value arg
      nv.name = av.before('=');
      nv.value = av.after('=');
      Variant vl = arg_names.GetVal(nv.name + "="); // register "flag=" to convert to names
      if(!vl.isNull())
        nv.name = vl.toString();
    }
    else {                      // regular arg: enter name as argv[x]
      nv.name = "argv[" + String(i) + "]";
      nv.value = av;
    }
    args.Add(nv);
  }
  args_used.SetSize(args.size); // always start out w/ 0 -- don't reset b/c called multiple times
}

void taMisc::AddArgName(const String& flag, const String& nm) {
  taMisc::arg_names.Add(NameVar(flag, (Variant)nm));
}

void taMisc::AddEqualsArgName(const String& arg_nm) {
  taMisc::arg_names.Add(NameVar(arg_nm + "=", (Variant)arg_nm));
}

void taMisc::AddArgNameDesc(const String& nm, const String& desc) {
  taMisc::arg_name_descs.Add(NameVar(nm, (Variant)desc));
}

String taMisc::FullArgString() {
  return args_raw.AsString(" ");
}

bool taMisc::FullArgStringToFile(const String& fname) {
  if(taMisc::dmem_proc != 0) return true;
  String ars = args_raw.AsString(" ");
  fstream strm;
  strm.open(fname, ios::out);
  if(strm.bad()) {
    strm.close();
    strm.clear();
    return false;
  }
  strm << ars << endl;
  strm.close(); strm.clear();
  return true;
}


String taMisc::FullArgStringName(bool exclude_flags, const String& exclude_names,
                                 bool shorten_names, int max_len, int seg_len,
                                 int rm_vowels_thr,
                                 const String& nm_val_sep, const String& arg_sep,
                                 const String& space_repl, const String& period_repl,
                                 const String& slash_repl) {

  String_PArray excludes;
  excludes.SetFromString(exclude_names, ", ");

  String act_arg_sep = arg_sep;
  if(act_arg_sep.empty())
    act_arg_sep = "#";          // makea barfs if this is in the default args!

  String rval;
  for(int i=1; i<args.size; i++) {
    NameVar nv = args[i];
    if(exclude_flags && nv.value.isNull()) continue;
    bool excl = false;
    for(int j=0; j<excludes.size; j++) {
      if(nv.name.contains(excludes[j])) {
        excl = true;
        break;
      }
    }
    if(excl) continue;
    String nm = nv.name;
    if(shorten_names) {
      nm = ShortName(nm, max_len, seg_len, rm_vowels_thr);
    }
    String vl = nv.value.toString();
    String nvs = nm + nm_val_sep + vl;
    nvs.gsub(" ", space_repl);
    nvs.gsub(".", period_repl);
    nvs.gsub("/", slash_repl);
    nvs.gsub("\\", slash_repl);
    if(rval.empty())
      rval = nvs;
    else
      rval.cat(act_arg_sep).cat(nvs);
  }
  return rval;
}


bool taMisc::CheckArgByName(const String& nm) {
  int idx = args.FindName(nm);
  if(idx < 0) return false;
  args_used.SafeEl(idx)++;
  return true;
}

String taMisc::FindArgByName(const String& nm) {
  if(!CheckArgByName(nm)) return _nilString; // updates use count..
  Variant vl = args.GetVal(nm);
  if(vl.isNull()) return _nilString;
  return vl.toString();
}

bool taMisc::GetAllArgsNamed(const String& nm, String_PArray& vals) {
  return taMisc::args.GetAllVals(nm, vals);
}

bool taMisc::CheckArgValContains(const String& vl) {
  int idx = args.FindValueContains(vl);
  if(idx < 0) return false;
  args_used.SafeEl(idx)++;
  return true;
}

String taMisc::FindArgValContains(const String& vl) {
  int idx = args.FindValueContains(vl);
  if(idx < 0) return _nilString;
  args_used.SafeEl(idx)++;
  return args.FastEl(idx).value.toString();
}

bool taMisc::ReportUnusedArgs(bool err) {
  String_PArray unused;
  for(int i=1; i<args.size; i++) {
    int used = args_used.SafeEl(i);
    if(used > 0) continue;
    String nm = args.SafeEl(i).name;
    String val = args.SafeEl(i).value.toString();
    if(val.contains(".proj")) continue;
    unused.Add(nm + " = " + val);
  }
  if(unused.size == 0) return false;
  String uns = unused.AsString(" ");
  if(err) {
    taMisc::Error("Some Args were Unused -- check for typos:", uns);
  }
  else {
    taMisc::Warning("Some Args were Unused -- check for typos:", uns);
  }
  return true;
}

#endif // NO_TA_BASE for all startup/args functions

/////////////////////////////////////////////////
//      Commonly used utility functions on strings/arrays/values

void taMisc::CharToStrArray(String_PArray& sa, const String& ch) {
  String tmp = ch;
  while (!tmp.empty()) {
    sa.AddUnique(tmp.before(" "));
    tmp = tmp.after(" ");
  }
}

String taMisc::StrArrayToChar(const String_PArray& sa) {
//NOTE: for historical reasons, this adds an extra sep on the end
// more efficient to know the length, so we don't resize...
  uint xlen = sa.size; // for seps
  int i;
  for (i=0; i < sa.size; i++) xlen += sa.FastEl(i).length();
  String tmp(0, xlen, '\0');
  for (i = 0; i < sa.size; i++) {
    tmp += sa.FastEl(i);
    tmp += " ";
  }
  return tmp;
}

void taMisc::ToNameValuePairs(const String& str, NameVar_PArray& nv_array) {
  int pos;
  String strCopy(str);
  strCopy = trimr(strCopy); // remove any tailing white space

  bool parse = true;
  while (parse) {
    pos = 0; // start at beginning of string or what is remaining
    pos = strCopy.index('=', pos);
    if (pos == -1) {  // no more pairs
      parse = false;
      break;
    }
    String name = strCopy.before(pos);
    name = trim(name);

    strCopy = strCopy.after(pos); // lop off what we processed
    strCopy = triml(strCopy);  // trim leading on remaining

    String value;
    pos = 0;  // reset before each search
    pos = strCopy.index(' ', pos); // find white space between name/value pairs (none if last n/v pair)
    if (pos == -1) {  // no space
      value = strCopy;  // last n/v pair
    }
    else {
      value = strCopy.before(pos);  // get chars up to next space
    }
    value = trim(value);
    nv_array.Add(NameVar(name, value));

    if (pos != -1) {  // more pairs - lop off what we got
      strCopy = strCopy.after(pos);
    }
    else {
      parse = false;
    }
  }
}

// add spaces to a label
void taMisc::SpaceLabel(String& lbl) {
  String tmp = lbl;
  lbl = "";
  int i;
  for(i=0; i<(int)tmp.length(); i++) {
    if((i > 0) && isupper(tmp[i]) && islower(tmp[i-1]))
      lbl += " ";
    else if(tmp[i] == '_') {
      lbl += " ";
      continue;
    }
    lbl += tmp[i];
  }
}

String taMisc::LeadingZeros(int num, int len) {
  String rval = num;
  int rval_len = rval.length();
  String lead_zeros;
  int i;
  for(i=len; i>=1; i--) {
    if(i > rval_len)
      lead_zeros += "0";
    else
      break;
  }
  return lead_zeros + rval;
}

String taMisc::GetSizeString(int64_t size_in_bytes, int precision, bool power_of_two) {
  double gb;
  double mb;
  double kb;
  if(power_of_two) {
    gb = 1073741824.0;
    mb = 1048576.0;
    kb = 1024.0;
  }
  else {
    gb = 1000000000.0;
    mb = 1000000.0;
    kb = 1000.0;
  }

  String fmt = "%." + String(precision) + "g";
  String szstr;
  if(size_in_bytes > gb) {
    szstr.convert((double)size_in_bytes / gb, fmt);
    szstr += " GB";
  }
  else if(size_in_bytes > mb) {
    szstr.convert((double)size_in_bytes / mb, fmt);
    szstr += " MB";
  }
  else if(size_in_bytes > kb) {
    szstr.convert((double)size_in_bytes / kb, fmt);
    szstr += " KB";
  }
  else 
    szstr = String(size_in_bytes) + " B";
  return szstr;
}

String taMisc::FormatValue(float val, int width, int precision) {
  stringstream ss;
  ss.precision(precision);
  ss.width(width);
  ss << val;
  string str = ss.str();
  String sval = str.c_str();
  if((int)sval.length() > width)
    sval = sval.before(width);
  while((int)sval.length() < width) sval += " ";
  return sval;
}

void taMisc::NormalizeRealString(String &str) {
  // Make NaN and infinity representations consistent.
  // Windows may use "1.#QNAN" or "-1.#IND" for nan.
  if (str.contains_ci("nan") || str.contains_ci("ind")) {
    str = "nan";
    return;
  }

  // Windows uses "1.#INF" or "-1.#INF" for infinities.
  if (str.contains_ci("inf")) {
    str = (str.elem(0) == '-') ? "-inf" : "inf";
    return;
  }

  // Get rid of leading zeros in the exponent, since mac/win aren't
  // consistent in how many they output for padding.
  int exponent = str.index_ci("e+");
  if (exponent == -1) exponent = str.index_ci("e-");
  if (exponent > 0) {
    exponent += 2;
    int first_non_zero = exponent;
    while (str.elem(first_non_zero) == '0') ++first_non_zero;
    if (first_non_zero > exponent) {
      str.del(exponent, first_non_zero - exponent);
    }
  }

  // Convert , to . for intl contexts so files are always uniform
  str.repl(",", ".");
}

String taMisc::StreamFormatFloat(float f, TypeDef::StrContext sc) {
  switch (sc) {
  case TypeDef::SC_STREAMING: {
    String ret(f, "%.7g");
    NormalizeRealString(ret);
    return ret;
  }
  default:
    return String(f);
  }
}

String taMisc::StreamFormatDouble(double d, TypeDef::StrContext sc) {
  switch (sc) {
  case TypeDef::SC_STREAMING: {
    String ret(d, "%.16lg");
    NormalizeRealString(ret);
    return ret;
  }
  default:
    return String(d);
  }
}


/* from xmgr, graphutils.c, copyright P. Turner
 * nicenum: find a "nice" number approximately equal to x
 * round if round=true, ceil if round=false
 */

double taMisc::NiceRoundNumber(double x, bool round) {
  double y;
  if(x <= 0.0)
     return 0.0;
  int exp = (int)floor(log10(x));
  double f = x / pow(10.0, (double) exp);	/* fraction between 1 and 10 */
  if (round)
    if (f < 1.5)
      y = 1.;
    else if (f < 3.)
      y = 2.;
    else if (f < 7.)
      y = 5.;
    else
      y = 10.;
  else if (f <= 1.)
    y = 1.;
  else if (f <= 2.)
    y = 2.;
  else if (f <= 5.)
    y = 5.;
  else
    y = 10.;
  return y * pow(10.0, (double)exp);
}

String taMisc::StringMaxLen(const String& str, int len) {
  if((int)str.length() <= len) return str;
  String rval = ((String)str).before(len);
  return rval;
}

String taMisc::StringEnforceLen(const String& str, int len) {
  if((int)str.length() >= len) {
    String rval = ((String)str).before(len);
    return rval;
  }
  String rval = str;
  while((int)rval.length() < len) rval += " ";
  return rval;
}

String taMisc::ShortName(const String& name, int max_len, int seg_len, int rm_vowels_thr) {
  int ln = name.length();
  if(ln <= max_len) return name;
  String nm = name;
  if(ln >= rm_vowels_thr) {
    nm = RemoveVowels(name);
    ln = nm.length();
  }
  int_PArray brks;              // locations of break points
  for(int i = 1; i < ln; i++) {
    char c = nm[i];
    if(c == ' ' || c == '_') {
      brks.Add(i);
      continue;
    }
    if(isupper(c) && islower(nm[i-1])) {
      brks.Add(i-1);
      continue;
    }
  }
  if(brks.size == 0)
    return StringMaxLen(nm, max_len);
  String rval;
  if(brks.size == 1) {
    String seg1 = nm.at(0, brks[0]);
    rval.cat(StringMaxLen(seg1, seg_len));
    int mln = max_len - rval.length();
    if(mln > 0) {
      String seg2 = nm.after(brks[0]);
      rval.cat(StringMaxLen(seg2, mln));
    }
  }
  if(brks.size > 1) {
    String seg1 = nm.at(0, brks[0]);
    rval.cat(StringMaxLen(seg1, seg_len));
    String seg2 = nm.at(brks[0]+1, brks[1]-brks[0]-1);
    rval.cat(StringMaxLen(seg2, seg_len));
    int mln = max_len - rval.length();
    if(mln > 0) {
      String seg2 = nm.at(brks[1]+1, ln-brks[1]-1);
      rval.cat(StringMaxLen(seg2, mln));
    }
  }
  return rval;
}

String taMisc::RemoveVowels(const String& str) {
  String rval;
  int len = str.length();
  for(int i=0; i<len; i++) {
    char c = str[i];
    char lc = tolower(c);
    if(lc == 'a' || lc == 'e' || lc == 'i' || lc == 'o' || lc == 'u') continue;
    rval.cat(c);
  }
  return rval;
}

String taMisc::PathsToLinks(const String& str) {
  String rval;
  int len = str.length();
  int lenm1 = len-1;
  bool prv_ws = true;           // allow at start
  int pos = 0;
  while(pos < len) {
    char c = str[pos];
    if(prv_ws && c == '.' && pos < lenm1) {    // now starting path
      char n = str[pos+1];                     // lookahead for alpha
      if(!isalpha(n)) {
        rval.cat(c); pos++;
        continue;
      }
      String path;
      path.cat(c);
      pos++;
      prv_ws = false;
      while(pos < len) {
        c = str[pos];
        if(isspace(c)) {
          break;
        }
        path.cat(c);
        pos++;
      }
      String qpath = path;
      qpath.gsub("\"", "%22");
      rval << "<a href=\"ta:" << qpath << "\">" << path << "</a>";
      if(pos >= len) break;
    }
    else if(isspace(c)) {
      prv_ws = true;
    }
    else {
      prv_ws = false;
    }
    rval.cat(c);
    pos++;
  }
  return rval;
}

bool taMisc::IsLegalCVar(const String& str) {
  for(int i=0;i<str.length();i++) {
    char c = str[i];
    if(isalnum(c) || c == '_') {
      if(i==0 && isdigit(c))
        return false;
    }
    else {
      return false;
    }
  }
  return true;
}

String taMisc::StringCVar(const String& str) {
  String rval;
  for(int i=0;i<str.length();i++) {
    char c = str[i];
    if(isalnum(c) || c == '_') {
      if(!(isdigit(c) && (rval.length()==0)))
        rval += c;
    }
    else {
      if((rval.length() > 0) && (rval.lastchar() != '_'))
        rval += '_';            // use _ to replace all strange chars
    }
  }
  return rval;
}

String& taMisc::FancyPrintList(String& strm, const String_PArray& strs,
                               int indent, int max_col_width, int n_per_line) {
  int max_wd = 0;
  for(int i=0; i<strs.size; i++) {
    max_wd = MAX(max_wd, strs[i].length());
  }
  max_wd = MIN(max_wd, max_col_width);
  max_wd = MAX(2, max_wd);

  int max_wd_sp = max_wd + taMisc::indent_spc;  // include spacing per item

  int isp = taMisc::indent_spc*indent;
  if(n_per_line < 0) {
    n_per_line = (taMisc::display_width - isp) / max_wd_sp; // n per line
    if(n_per_line <= 0) n_per_line = 1;
  }

  IndentString(strm, indent); // start indented
  int cur_wd = isp;
  int prln = 0;
  for(int i=0; i<strs.size; i++) {
    String& it = strs[i];
    int sln = it.length();
    if(MAX(sln,max_wd_sp) + cur_wd > taMisc::display_width || prln >= n_per_line) {
      // cr if we're going over
      prln = 0;
      CrIndentString(strm, indent);
      cur_wd = isp;
    }
    strm << it;
    prln++;
    if(prln < n_per_line) {
      int trg_wd_sp = (int(sln / max_wd_sp)+1) * max_wd_sp;
      while(sln < trg_wd_sp) { strm << ' '; ++sln; }
      cur_wd += trg_wd_sp;
    }
  }
  strm << "\n";                 // always terminate list
  return strm;
}

String& taMisc::FancyPrintTwoCol(String& strm, const String_PArray& col1_strs,
                                  const String_PArray& col2_strs, int indent) {
  int max_wd = 0;
  for(int i=0; i<col1_strs.size; i++) {
    max_wd = MAX(max_wd, col1_strs[i].length());
  }
  max_wd = MAX(2, max_wd);

  int max_wd_sp = max_wd + taMisc::indent_spc;  // include spacing per item

  int isp = taMisc::indent_spc*indent;
  for(int i=0; i<col1_strs.size; i++) {
    taMisc::IndentString(strm, indent); // start indented
    String& it = col1_strs[i];
    int sln = it.length();
    strm << it;
    while(sln < max_wd_sp) { strm << ' '; ++sln; }
    strm << col2_strs[i];
    strm << "\n";
  }
  return strm;
}

/////////////////////////////////////////////////
//      File Paths etc

#ifdef TA_OS_WIN
const String taMisc::path_sep('\\'); 
#else 
const String taMisc::path_sep('/'); 
#endif

String taMisc::FinalPathSep(const String& in) {
  if (in.length() == 0)
    return String();
  else {
    //NOTE: don't use [] below, because of insane MS VC++ ambiguity
    char c = in.elem(in.length() - 1);
    // note: need to check both seps, because of whacky Win/Cygwin mixtures
    if (( c == '\\') || (c == '/'))
      return in;
  } 
  return in + path_sep;
}

String taMisc::NoFinalPathSep(const String& in) {
  String rval = in;
  char c;
  while ((c = rval.lastchar()) && 
    ( (c == '/') || (c == '\\')))
    rval.truncate(rval.length() - 1);
  return rval;
}

int taMisc::PosFinalPathSep(const String& in) {
  int rval = in.length() - 1;
  char c;
  while (rval >= 0) {
    c = in.elem(rval);
    if ((c == '/') || (c == '\\')  || (c == ':'))
      break;
    --rval;
  }
  return rval;
}

String taMisc::UnescapeBackslash(const String& in) {
  // convert lexical double backslash to actual single backslash
  // i.e. you are reading C source, and want to convert to the string 
  String rval = in;
  rval.gsub("\\\\", "\\");
  return rval;
}

String taMisc::GetFileFmPath(const String& path) {
  int pfs = PosFinalPathSep(path);
  if (pfs < 0) return path;
  else return path.after(pfs);
}

String taMisc::GetDirFmPath(const String& path, int n_up) {
#ifdef NO_TA_BASE
  String pth = path;
#else
  String pth = QDir::fromNativeSeparators(path);
#endif
  if(!pth.contains('/')) return _nilString;
  String dir = pth.before('/',-1);
  for(int i=0;i<n_up;i++) {
    if(!dir.contains('/')) return _nilString;
    dir = dir.before('/',-1);
  }
#ifdef NO_TA_BASE
  return dir;
#else
  return QDir::toNativeSeparators(dir);
#endif
}

String taMisc::PathToUnixSep(const String& path) {
#ifdef TA_OS_WIN
#ifdef NO_TA_BASE
  String pth = UnescapeBackslash(path);
  pth.gsub("\\", "/");
#else
  String pth = QDir::fromNativeSeparators(path);
#endif
  return pth;
#else
  return path;
#endif
}


bool taMisc::IsQualifiedPath(const String& fname) {
  //NOTE: we just check the union of Unix and Win -- we aren't asked if it 
  // is valid for the platform, so ok to check all cases here
  if (fname.empty()) return false;
  char c = fname.elem(0);
  // handles all relative, and Unix absolute, and Win share paths
  if ((c == '.') || (c == '\\') || (c == '/')) return true; 
#ifdef TA_OS_WIN
  // Win x: drive letter path
  if ((fname.length() >= 2) && (fname.elem(1) == ':')) return true;
#endif
  return false;
}

bool taMisc::FileExists(const String& fname) {
  bool rval = false;
  fstream fin;
  fin.open(fname, ios::in);
  if (fin.is_open()) {
    rval = true;
  }
  fin.close();
  return rval;
}

String taMisc::GetTemporaryPath() {
#ifndef NO_TA_BASE
  return QDir::tempPath();
#else

  static char tmpbuf[1024];
#ifdef TA_OS_WIN
  String rval;
  DWORD retVal = GetTempPath(1024, tmpbuf);
  if (retVal != 0)
    rval = String(tmpbuf);
  return rval;
#else
  String rval = "/tmp";
  return rval;
#endif

#endif
}

/////////////////////////////////////////////////////////////////////////
//		Start of TA_BASE-only section

#ifndef NO_TA_BASE

String taMisc::ExpandFilePath(const String& path, taProject* proj) {
  String ep = path;
  if(ep.startsWith('~')) {
    ep = GetHomePath() + ep.after('~');
  }
  if(proj) {                    // only avail if project provided
    if(ep.startsWith("CRR:")) {
      ep = proj->GetClusterRunPath() + path_sep + "results" + path_sep + ep.after("CRR:");
    }
    else if(ep.startsWith("CRM:")) {
      ep = proj->GetClusterRunPath() + path_sep + "models" + path_sep + ep.after("CRM:");
    }
    else if(ep.startsWith("CR:")) {
      ep = proj->GetClusterRunPath() + path_sep + ep.after("CR:");
    } else if(ep.startsWith("CRRM:")) { //Choose the file, either local or in the cluster_run directory, that actually exists
      String test_path;
      test_path = proj->proj_dir + path_sep + ep.after("CRRM:");
      if (FileExists(test_path)) {
        return test_path;
      } else {
        test_path = proj->GetClusterRunPath() + path_sep + "results" + path_sep + ep.after("CRRM:");
        return test_path;
      }

    }
  }
  return ep;
}

String taMisc::CompressFilePath(const String& path, taProject* proj) {
  String ep = path;
  String hp = GetHomePath();
  if(ep.startsWith(hp)) {
    ep = String("~") + ep.after(hp);
  }
  if(proj) {                    // only avail if project provided
    String cr = proj->GetClusterRunPath();
    if(ep.startsWith(cr)) {
      String acr = ep.after(cr);
      if(acr.startsWith(path_sep + "results" + path_sep)) {
        ep = String("CRR:") + acr.after("results" + path_sep);
      }
      else if(acr.startsWith(path_sep + "models" + path_sep)) {
        ep = String("CRM:") + acr.after("models" + path_sep);
      }
      else {
        ep = String("CR:") + acr;
      }
    }
  }
  return ep;
}


int64_t taMisc::FileSize(const String& fname) {
  QFileInfo fi(fname);
  return fi.size();
}

bool taMisc::FileWritable(const String& fname) {
  QFileInfo fi(fname);
  return fi.isWritable();
}

bool taMisc::FileReadable(const String& fname) {
  QFileInfo fi(fname);
  return fi.isReadable();
}

bool taMisc::FileExecutable(const String& fname) {
  QFileInfo fi(fname);
  return fi.isExecutable();
}

bool taMisc::SetFilePermissions(const String& fname, bool user, bool group,
                                   bool other, bool readable, bool writable,
                                   bool executable) {
  QFile fi(fname);
  QFile::Permissions perm = 0;
  if(user && readable) perm |= QFile::ReadUser;
  if(user && writable) perm |= QFile::WriteUser;
  if(user && executable) perm |= QFile::ExeUser;
  if(group && readable) perm |= QFile::ReadGroup;
  if(group && writable) perm |= QFile::WriteGroup;
  if(group && executable) perm |= QFile::ExeGroup;
  if(other && readable) perm |= QFile::ReadOther;
  if(other && writable) perm |= QFile::WriteOther;
  if(other && executable) perm |= QFile::ExeOther;
  return fi.setPermissions(perm);
}

#ifdef CopyFile
#undef CopyFile
#endif

bool taMisc::CopyFile(const String& old_fn, const String& new_fn) {
  return QFile::copy(old_fn, new_fn);
}

bool taMisc::RenameFile(const String& old_fn, const String& new_fn) {
  QDir d;
  return d.rename(old_fn, new_fn);
}

bool taMisc::RemoveFile(const String& fn) {
  QDir d;
  return d.remove(fn);
}

bool taMisc::DirExists(const String& fn) {
  QDir d(fn);
  return d.exists();
}

bool taMisc::MakeDir(const String& fn) {
  QDir d;
  return d.mkdir(fn);
}

bool taMisc::MakePath(const String& fn) {
  QDir d;
  return d.mkpath(fn);
}

bool taMisc::MakeSymLink(const String& file_name, const String& link_name) {
  String fnm = taMisc::ExpandFilePath(file_name);
  return QFile::link(fnm, link_name);
}

bool taMisc::RemoveDir(const String& fn) {
  QDir d;
  return d.rmdir(fn);
}

bool taMisc::RemovePath(const String& fn) {
  QDir d;
  return d.rmpath(fn);
}

int taMisc::ReplaceStringInFile(const String& filename, const String& search_str,
                                 const String& repl_str) {
  if(!FileExists(filename)) {
    taMisc::Error("ReplaceStringInFile: file named:",filename,"does not exist");
    return -1;
  }

  String fstr;
  fstr.LoadFromFile(filename);
  int rval = fstr.gsub(search_str, repl_str);
  if(rval > 0) {
    fstr.SaveToFile(filename);
  }
  return rval;
}

/////////////////////////////////////////
//		Various standard paths		

String taMisc::GetCurrentPath() {
  // before any projects have been opened, the current path is not really valid per QDir
  // so we use the first of the recent paths -- this works well for the file chooser for
  // example.
  if(tabMisc::root->projects.size == 0) {
    String_Array& rpth = tabMisc::root->recent_paths;
    if(rpth.size > 0)
      return rpth.SafeEl(0);
  }
  return QDir::currentPath();
}

bool taMisc::SetCurrentPath(const String& path) {
  return QDir::setCurrent(path);
}

String taMisc::GetHomePath() {
#ifdef TA_OS_WIN
  return String(getenv("USERPROFILE"));
#else
 // return getenv("HOME");
 return QDir::homePath();
#endif
}

String taMisc::GetUserPluginDir() {
#ifdef TA_OS_WIN
  // Use a separate directory for 64-bit plugins.  Otherwise, if the user has
  // both 32-bit emergent and 64-bit emergent installed, it can cause problems
  // with the Qt Plugin Cache, see:
  //   http://doc.qt.nokia.com/stable/deployment-plugins.html#the-plugin-cache
  // The problem is that the 32-bit version of emergent might mark a 64-bit
  // plugin DLL as "bad" in the cache, thus preventing it from working with
  // the 64-bit version of emergent (and vice versa).  Separating the DLLs
  // prevents 32-bit emergent from ever seeing a 64-bit DLL, so this can't
  // happen.
  if (sizeof(void *) >= 8) {
    return "plugins64";
  }
#endif

  return "plugins";
}

String taMisc::GetSysPluginDir() {
  // Not necessary to separate 32/64 here, since this directory only exists
  // within the directory emergent was installed to, which is already distinct
  // for 32-bit and 64-bit installations.  Also, this directory is created by
  // the installer/CMake/CPack system, and would be more difficult to name
  // differently on Windows for 32 vs. 64.
  return "plugins";
}

String taMisc::GetDocPath() {
#ifdef TA_OS_WIN
  //NOTE: we don't want the "home" folder, we really want the user's My Documents folder
  // since this can be moved from its default place, and/or renamed, we have to do this:
  TCHAR szPath[MAX_PATH];
  if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath))) {
    return String(szPath);
  }
  else {
    taMisc::Warning("taMisc::GetDocPath(): SHGetFolderPath() failed unexpectedly, guessing at user's home folder...");
    return const_cast<const char*>(getenv("USERPROFILE")) + PATH_SEP + "My Documents";
  }
#else
  return GetHomePath() + "/Documents";
#endif
}

String taMisc::GetAppDataPath(const String& appname) {
#ifdef TA_OS_WIN
  // note: sleazy, we should use the complicated way, but this seems to be correct...
  // note: for Windows, we add the orgname, to keep it compliant
  return getenv("APPDATA") + PATH_SEP + taMisc::org_name + PATH_SEP + appname;
#else
  return GetHomePath() + "/." + appname;
#endif
}

String taMisc::GetAppDocPath(const String& appname) {
#ifdef TA_OS_WIN
  return GetHomePath() + "\\" + capitalize(appname);
#elif defined(TA_OS_MAC)
  return GetHomePath() + "/Library/" + capitalize(appname);
#else
  return GetHomePath() + "/lib/" + capitalize(appname);
#endif
}

String taMisc::GetCustomKeyFilename() {
 return taMisc::prefs_dir + PATH_SEP + "custom_keys";
}

String taMisc::GetConsoleHistoryFilename() {
  return taMisc::prefs_dir + PATH_SEP + "console_history";
}

String taMisc::FileDiff(const String& fname_a, const String& fname_b,
                        bool trimSpace, bool ignoreSpace, bool ignoreCase) {
  String str_a, str_b;
  taStringDiff diff;
  diff.DiffFiles(fname_a, fname_b, str_a, str_b, trimSpace, ignoreSpace, ignoreCase);
  return diff.GetDiffStr(str_a, str_b);
}

#endif // NO_TA_BASE

// 		End of TA_BASE only 
///////////////////////////////////////////////////////////////////////


// try to find file fnm in one of the include paths -- returns complete path to file
String taMisc::FindFileOnPath(String_PArray& paths, const String& fname) {
  int acc = access(fname, F_OK);
  if (acc == 0) {
    return fname;
  }

  for(int i=0; i<paths.size; i++) {
    String trynm = paths.FastEl(i) + "/" + fname;
    int acc = access(trynm, F_OK);
    if (acc == 0) {
      return trynm;
    }
  }
  return "";
}

String taMisc::FindFileOnLoadPath(const String& fname) {
  return FindFileOnPath(load_paths, fname);
}

int taMisc::GetUniqueFileNumber(int st_no, const String& prefix, const String& suffix) {
  String fname;
  int i;
  for(i=st_no; i<10000; i++) {  // stop at 10,000
    fname = prefix + String(i) + suffix;
    int acc = access(fname, R_OK);
    if(acc != 0)
      break;                    // its ok..
  }
  fstream strm;
  strm.open(fname, ios::out);   // this should hold the place for the file
  strm.close(); strm.clear();           // while it is being saved, etc..
  return i;
}

String taMisc::GetWikiURL(const String& wiki_name, bool add_index) {
  Variant rval = wikis.GetVal(wiki_name);
  if(rval.isNull()) return _nilString;
  String url = rval.toString();
  if(add_index)
    url += "/index.php/";
  return url;
}

String taMisc::FixURL(const String& urltxt) {
  String nwurl = urltxt;
  // This is not a fix, it's breakage: it prevents using relative URLs to
  // link to pages stored in the same directory as the project file.
  // if(!urltxt.contains(':') && !urltxt.startsWith('.')) {
  //   nwurl = String("http://") + urltxt; // assume http as default
  // }
  if(nwurl.startsWith("http://")) { // now fixup http links..
    if(!nwurl.contains('.')) nwurl += ".com"; // assume .com
  }
  if(nwurl.startsWith("https://")) { // now fixup http links..
    if(!nwurl.contains('.')) nwurl += ".com"; // assume .com
  }
  return nwurl;
}

String taMisc::ExtraAppSuffix() {
  if(app_suffix.empty()) return app_suffix;
  String rval = app_suffix;
  // remove standard suffixes
  rval.gsub("_dbg", "");
  rval.gsub("_mpi", "");
  return rval;
}


bool taMisc::InternetConnected() {
  bool any_valid = false;
#ifndef NO_TA_BASE
  const bool DEBUG_LOG = false;
  QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
  if (DEBUG_LOG) cerr << "Inspecting network interfaces..." << endl;
  for (int i = 0; i < ifaces.size(); ++i) {
    if (DEBUG_LOG) cerr << " Looking at interface #" << i << endl;
    const QNetworkInterface& ifc = ifaces.at(i);
    if(!ifc.isValid()) {
      if (DEBUG_LOG) cerr << "  Skipping, not valid" << endl;
      continue;
    }
    if(ifc.flags() & (QNetworkInterface::IsLoopBack | QNetworkInterface::IsPointToPoint)) {
      if (DEBUG_LOG) cerr << "  Skipping, isloopback or ptp" << endl;
      continue;
    }
    if(ifc.flags() & (QNetworkInterface::IsRunning | QNetworkInterface::IsUp)) {

      QList<QNetworkAddressEntry> addrs = ifc.addressEntries();
      if(addrs.size() == 0) {
        if (DEBUG_LOG) cerr << "  Skipping, addrs.size()==0" << endl;
        continue;
      }

      String nm = ifc.name();
      if(nm.startsWith("vm")) {
        // This check isn't necessarily reliable: a VirtualBox VM
        // interface may be named with a GUID.
        if (DEBUG_LOG) cerr << "  Skipping, VM name: " << nm << endl;
        continue; // avoid virtual machine guys!
      }
      for (int j = 0; j < addrs.size(); ++j) {
        QHostAddress ip = addrs.at(j).ip();
        if(ip.isNull()) {
          if (DEBUG_LOG) cerr << "   Skipping address #" << j << ", IP is null" << endl;
        }
        else {
          String adr = ip.toString();
          if (DEBUG_LOG) cerr << "   Up and running: " << nm << " with addr:" << adr << endl;
          any_valid = true;
          // could break/return here, but for logging purposes, keep going.
        }
      }
    }
    else
    {
      if (DEBUG_LOG) cerr << "intf #" << i << " is not up" << endl;
    }
  }
#endif
  return any_valid;
}


///////////////////////////////////////////////////////////////////////
//     System info etc

int taMisc::CpuCount() {
#ifdef TA_OS_WIN
  SYSTEM_INFO info;
  info.dwNumberOfProcessors = 0;
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors;
#elif defined(TA_OS_MAC)
  int mib[2] = {CTL_HW, HW_NCPU};
  int ncpu;
  size_t len = sizeof(ncpu);
  sysctl(mib, 2, &ncpu, &len, NULL, 0);
  return ncpu;
#else
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

String taMisc::HostName() {
#ifdef TA_OS_WIN
  return String(getenv("COMPUTERNAME"));
#else
  return String(getenv("HOSTNAME"));
#endif
}

String taMisc::UserName() {
#ifdef TA_OS_WIN
  return String(getenv("USERNAME"));
#else
  return String(getenv("USER"));
#endif
}

int taMisc::ProcessId() {
#ifdef TA_OS_WIN
  return (int)GetCurrentProcessId();
#else
  return (int)getpid();
#endif
}

int taMisc::TickCount() {
#ifdef TA_OS_WIN
  return (int)GetTickCount(); // is in ms
#else
  return (int)clock();
#endif
}

void taMisc::SleepS(int sec) {
  SleepMs(sec * 1000);
}

void taMisc::SleepMs(int msec) {
#ifdef TA_OS_WIN
  Sleep(msec);
#else
  //note: specs say max usleep value is 1s, so we loop if necessary
  while (msec > 1000) {
    usleep(1000000);
    msec -= 1000;
  }
  usleep(msec * 1000);
#endif
}

int taMisc::ExecuteCommand(const String& cmd) {
  int rval = system(cmd.chars());
  // if allegedly successful, still need to test for error
#ifdef TA_OS_WIN
  if (rval == 0) { 
    if (errno == ENOENT)
      rval = -1;
  }
#endif
  return rval;
}


/////////////////////////////////////////////////
//      Recording GUI actions to css script

void taMisc::StartRecording() {
  record_on = true;
  if (ScriptRecordingGui_Hook)
    ScriptRecordingGui_Hook(true);
}

void taMisc::StopRecording(){
  record_on = false;
  if (ScriptRecordingGui_Hook)
    ScriptRecordingGui_Hook(false);
}

bool taMisc::RecordScript(const String& cmd) {
  if(!record_on) return false;
  record_script << cmd;
  if(cmd[strlen(cmd)-1] != '\n') {
    taMisc::Warning("*** Warning: cmd must end in a newline, but doesn't -- should be fixed:",
                    cmd);
    record_script << '\n';
  }
  return true;
}

#ifndef NO_TA_BASE
// normal non quoted members
void taMisc::ScriptRecordAssignment(taBase* tab,MemberDef* md){
  if(record_on)  {
    record_script << tab->GetPathNames() << "." << md->name << " = " <<
      md->GetValStr(tab) << ";" << "\n";
  }
}
// Script Record Inline Assignment
void taMisc::SRIAssignment(taBase* tab,MemberDef* md){
  if(record_on)  {
    record_script << tab->GetPathNames() << "." << md->name << " = \"" <<
      md->GetValStr(tab) << "\";\n";
    record_script << tab->GetPathNames() << "." << "UpdateAfterEdit();" << "\n";
  }
}

// Script Record Enum Assignment
void taMisc::SREAssignment(taBase* tab,MemberDef* md){
  if(record_on)  {
    record_script << tab->GetPathNames() << "." << md->name << " = " <<
      tab->GetTypeDef()->name << "::" <<
      md->GetValStr(tab) << ";" << "\n";
  }
}
#endif

////////////////////////////////////////////////////////////////////////
//      File Parsing Stuff for Dump routines

int taMisc::skip_white(istream& strm, bool peek) {
  int c;
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c=strm.peek()) == ' ') || (c == '\t') || (c == '\n') || (c == '\r')) {
      strm.get();
      ConsoleOutputChars((char)c, true);
    }
    if(!peek && (c != EOF)) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c=strm.peek()) == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
      strm.get();
  }
  if(!peek)
    strm.get();
  return c;
}

int taMisc::skip_white_noeol(istream& strm, bool peek) {
  int c;
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c=strm.peek()) == ' ') || (c == '\t') || (c == '\r')) {
      strm.get();
      ConsoleOutputChars((char)c, true);
    }
    if(!peek && (c != EOF)) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c=strm.peek()) == ' ') || (c == '\t') || (c == '\r'))
      strm.get();
  }
  if(!peek)
    strm.get();
  return c;
}

int taMisc::read_word(istream& strm, bool peek) {
  int c = skip_white(strm, true); // use peek mode
  LexBuf = "";
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && (isalnum(c) || (c == '_'))) {
      ConsoleOutputChars((char)c, true);
      LexBuf += (char)c; strm.get();
    }
    if(c != EOF) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c = strm.peek()) != EOF) && (isalnum(c) || (c == '_'))) {
      LexBuf += (char)c; strm.get();
    }
  }
  if(!peek)
    strm.get();
  return c;
}

int taMisc::read_nonwhite(istream& strm, bool peek) {
  int c = skip_white(strm, true);
  LexBuf = "";
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && (c!=' ') && (c!='\t') && (c!='\n') && (c!='\r')) {
      ConsoleOutputChars((char)c, true);
      LexBuf += (char)c; strm.get();
    }
    if(c != EOF) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c = strm.peek()) != EOF) && (c!=' ') && (c!='\t') && (c!='\n') && (c!='\r')) {
      LexBuf += (char)c; strm.get();
    }
  }
  if(!peek)
    strm.get();
  return c;
}

int taMisc::read_nonwhite_noeol(istream& strm, bool peek) {
  int c = skip_white_noeol(strm,true);
  taMisc::LexBuf = "";
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && (c!=' ') && (c!='\t') && (c!='\n') && (c!='\r')) {
      ConsoleOutputChars((char)c, true);
      taMisc::LexBuf += (char)c; strm.get();
    }
    if(c != EOF) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c = strm.peek()) != EOF) && (c!=' ') && (c!='\t') && (c!='\n') && (c!='\r')) {
      taMisc::LexBuf += (char)c; strm.get();
    }
  }
  if(!peek)
    strm.get();
  return c;
}

int taMisc::read_till_eol(istream& strm, bool peek) {
  int c = skip_white_noeol(strm, true);
  LexBuf = "";
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && !((c == '\n'))) {
      ConsoleOutputChars((char)c, true);
      if(c != '\r') LexBuf += (char)c;
      strm.get();
    }
    if(c != EOF) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c = strm.peek()) != EOF) && !((c == '\n'))) {
      if(c != '\r') LexBuf += (char)c;
      strm.get();
    }
  }
  if(!peek)
    strm.get();
  return c;
}

int taMisc::read_till_semi(istream& strm, bool peek) {
  int c = skip_white(strm, true);
  LexBuf = "";
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && !((c == ';'))) {
      ConsoleOutputChars((char)c, true);
      LexBuf += (char)c; strm.get();
    }
    if(c != EOF) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c = strm.peek()) != EOF) && !((c == ';'))) {
      LexBuf += (char)c; strm.get();
    }
  }
  if(!peek)
    strm.get();
  return c;
}

int taMisc::read_till_lbracket(istream& strm, bool peek) {
  int c = skip_white(strm, true);
  LexBuf = "";
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && !((c == '{'))) {
      ConsoleOutputChars((char)c, true);
      LexBuf += (char)c; strm.get();
    }
    if(c != EOF) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c = strm.peek()) != EOF) && !((c == '{'))) {
      LexBuf += (char)c; strm.get();
    }
  }
  if(!peek)
    strm.get();
  return c;
}

int taMisc::read_till_lb_or_semi(istream& strm, bool peek) {
  int c = skip_white(strm, true);
  LexBuf = "";
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && !((c == '{') || (c == ';') || (c == '='))) {
      ConsoleOutputChars((char)c, true);
      LexBuf += (char)c; strm.get();
    }
    if(c != EOF) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c = strm.peek()) != EOF) && !((c == '{') || (c == ';') || (c == '='))) {
      LexBuf += (char)c; strm.get();
    }
  }
  if(!peek)
    strm.get();
  return c;
}

int taMisc::read_till_rbracket(istream& strm, bool peek) {
  int c = skip_white(strm, true);
  LexBuf = "";
  int depth = 0;
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && !((c == '}') && (depth <= 0))) {
      ConsoleOutputChars((char)c, true);
      LexBuf += (char)c;
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
    if(c != EOF) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c = strm.peek()) != EOF) && !((c == '}') && (depth <= 0))) {
      LexBuf += (char)c;
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
  }
  if(!peek)
    strm.get();
  return c;
}

int taMisc::read_till_rb_or_semi(istream& strm, bool peek) {
  int c = skip_white(strm, true);
  LexBuf = "";
  int depth = 0;
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && !(((c == '}') || (c == ';')) && (depth <= 0))) {
      ConsoleOutputChars((char)c, true);
      LexBuf += (char)c;
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
    if(c != EOF) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c = strm.peek()) != EOF) && !(((c == '}') || (c == ';')) && (depth <= 0))) {
      LexBuf += (char)c;
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
  }
  if(!peek) {
    strm.get();
    if((c == '}') && (strm.peek() == ';')) strm.get(); // skip next semi
  }
  return c;
}

int taMisc::skip_till_start_quote_or_semi(istream& strm, bool peek) {
// read in the stream until a start quote or a semi
  int c = skip_white(strm, true);
  LexBuf = "";
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && (c != '\"') && (c != ';')) {
      ConsoleOutputChars((char)strm.get(), true);
    }
  }
  else {
    while (((c = strm.peek()) != EOF) && (c != '\"') && (c != ';'))
      strm.get(); // consume it
  }
  if(!peek)
    strm.get();
  return c;
}


int taMisc::read_till_end_quote(istream& strm, bool peek) {
  // don't skip initial whitespace because we're presumably reading a string literal
  int c;
  bool bs = false;
  LexBuf = "";
  if (taMisc::verbose_load >= taMisc::SOURCE) {
    while (true) {
      while (((c = strm.peek()) != EOF) && (bs || ((c != '\"') && (c != '\\')) ) )
      {  // "
        bs = false;
        ConsoleOutputChars((char)c, true);
        LexBuf += (char)c; strm.get();
      }
      if (c == EOF) break;
      //NOTE: don't echo escape chars
      if (c == '\\') {
        strm.get();             // get the backslash (don't put in LexBuf)
        bs = true;              // backslash-quoted character coming
      } else // if (c == '\"')
        break;
    }
  }
  else {
    while (true) {
      while (((c = strm.peek()) != EOF) && (bs || ((c != '\"') && (c != '\\')))) {  // "
        bs = false;
        LexBuf += (char)c; strm.get();
      }
      if (c == EOF) break;
      if (c == '\\') {
        strm.get();             // get the backslash (don't put in LexBuf)
        bs = true;              // backslash-quoted character coming
      } else // if (c == '\"')
        break;
    }
  }
  if (!peek)
    strm.get(); // consume the "
  return c;
}

int taMisc::read_till_end_quote_semi(istream& strm, bool peek) {
  int c = read_till_end_quote(strm, peek);
  if (c == EOF) return c;
  if (peek) strm.get(); // consume the "
  while (((c = strm.peek()) != EOF) && (c != ';'))
    c = strm.get();
  if (c == EOF) return c;
  if (!peek)
    strm.get(); // consume the ;
  return c;
}

int taMisc::skip_past_err(istream& strm, bool peek) {
  int c;
  int depth = 0;
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    int cur_pos = 0;
    ConsoleOutputChars("<<err_skp ->>", true);
    while (((c = strm.peek()) != EOF) && !(((c == '}') || (c == ';')) && (depth <= 0))) {
      ConsoleOutputChars((char)c, true); cur_pos++;
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
    if(c != EOF) ConsoleOutputChars(String((char)c) + "<<- err_skp>>", true);
  }
  else {
    while (((c = strm.peek()) != EOF) && !(((c == '}') || (c == ';')) && (depth <= 0))) {
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
  }
  if(!peek) {
    strm.get();
    if((c != EOF) && (strm.peek() == ';')) strm.get(); // skip next semi
  }
  return c;
}

int taMisc::skip_past_err_rb(istream& strm, bool peek) {
  int c;
  int depth = 0;
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    ConsoleOutputChars("<<err_skp ->>", true);
    while (((c = strm.peek()) != EOF) && !((c == '}') && (depth <= 0))) {
      ConsoleOutputChars((char)c, true);
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
    if(c != EOF) ConsoleOutputChars(String((char)c) + "<<- err_skp>>", true);
  }
  else {
    while (((c = strm.peek()) != EOF) && !((c == '}') && (depth <= 0))) {
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
  }
  if(!peek) {
    strm.get();
    if((c != EOF) && (strm.peek() == ';')) strm.get(); // skip next semi
  }
  return c;
}


int taMisc::find_not_in_quotes(const String& str, char c, int start) {
  int len = str.length();
  bool in_str = false;
  if(start < 0) {
    for(int i=len+start; i>=0; i--) {
      char cv = str[i];
      if(cv == '\"') {
        in_str = !in_str;
        continue;
      }
      if(!in_str && cv == c) return i;
    }
  }
  else {
    for(int i=start; i<len; i++) {
      char cv = str[i];
      if(cv == '\"') {
        in_str = !in_str;
        continue;
      }
      if(!in_str && cv == c) return i;
    }
  }
  return -1;
}

int taMisc::replace_strings(istream& istrm, ostream& ostrm, NameVar_PArray& repl_list) {
  int n_repl = 0;
  int c;
  String nwln;
  while((c = read_till_eol(istrm)) != EOF) {
    nwln = LexBuf;
    for(int i=0;i<repl_list.size;i++) {
      String repl = repl_list[i].name;
      if(repl.contains("/w")) { // special whitespace flag
        String pre = repl.before("/w");
        if(!nwln.contains(pre)) continue;
        String post = repl.after("/w");
        if(!nwln.contains(post)) continue;
        String nwpre = nwln.before(pre);
        String nwpost = nwln.after(post);
        nwln = nwpre + repl_list[i].value.toString() + nwpost;
      }
      else {
        n_repl += nwln.gsub(repl, repl_list[i].value.toString());
      }
    }
    ostrm << nwln << endl;
  }
  return n_repl;
}

int taMisc::find_strings(istream& istrm, String_PArray& strs) {
  int c;
  String nwln;
  while((c = read_till_eol(istrm)) != EOF) {
    for(int i=0;i<strs.size;i++) {
      if(LexBuf.contains(strs[i])) return i;
    }
  }
  return -1; // none found
}

////////////////////////////////////////////////////////////////////////
//      String parsing

String taMisc::ParseStr_CName(const String& str) {
  int len = str.length();
  int st_pos = 0;
  while(st_pos < len && isspace(str[st_pos])) st_pos++;
  int ed_pos = st_pos;
  while(ed_pos < len && (isalnum(str[ed_pos]) || str[ed_pos] == '_')) ed_pos++;
  if(ed_pos > st_pos) {
    return str.at(st_pos, ed_pos - st_pos);
  }
  return _nilString;
}

////////////////////////////////////////////////////////////////////////
//      HTML-style tags

taMisc::ReadTagStatus taMisc::read_tag(istream& strm, String& tag, String& val) {
  int c = skip_white(strm, true);
  if(c == EOF) return TAG_EOF;
  if(c != '<') return TAG_NONE;
  strm.get();
  c = strm.peek();
  if(c == EOF) return TAG_EOF;
  ReadTagStatus rval = TAG_GOT;
  if(c == '/') {
    strm.get();
    rval = TAG_END;
  }
  read_till_rangle(strm, false);
  if(LexBuf.contains(' ')) {
    tag = LexBuf.before(' ');
    val = LexBuf.after(' ');
  }
  else {
    tag = LexBuf;
    val = "";
  }
  c = strm.peek();
  if(c == '\r' || c == '\n') strm.get(); // absorb an immediate cr after tag, which is common
  if(c == '\r') {
    c = strm.peek();
    if(c == '\n') strm.get();   // absorb an immediate cr after tag, which is common
  }
  return rval;
}

int taMisc::read_till_rangle(istream& strm, bool peek) {
  int c;
  LexBuf = "";
  int depth = 0;
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && !((c == '>') && (depth <= 0))) {
      ConsoleOutputChars((char)c, true);
      LexBuf += (char)c;
      if(c == '<')      depth++;
      if(c == '>')      depth--;
      strm.get();
    }
    if(c != EOF) ConsoleOutputChars((char)c, true);
  }
  else {
    while (((c = strm.peek()) != EOF) && !((c == '>') && (depth <= 0))) {
      LexBuf += (char)c;
      if(c == '<')      depth++;
      if(c == '>')      depth--;
      strm.get();
    }
  }
  if(!peek)
    strm.get();
  return c;
}

////////////////////////////////////////////////////////////////////////
//      File Parsing Stuff for Dump routines: Output

ostream& taMisc::indent(ostream& strm, int indent, int tsp) {
  // don't do any fancy things for undo saving -- just wastes space!
  if(is_saving && ((save_format == PLAIN) || is_undo_saving)) return strm;
  int i;
  int itabs = (indent * tsp) / 8;
  int ispcs = (indent * tsp) % 8;
  for(i=0; i<itabs; i++)
    strm << "\t";
  for(i=0; i<ispcs; i++)
    strm << " ";
  return strm;
}

ostream& taMisc::write_quoted_string(ostream& strm, const String& str, bool write_if_empty) {
  if (!write_if_empty && str.empty()) return strm;

  strm << '\"';
  int l = str.length();
  for (int i = 0; i < l; ++i) {
    char c = str.elem(i);
    // we slash escape slashes and quotes
    //TODO: perhaps we should escape other control chars as well
    if ((c == '\"') || (c == '\\'))
      strm << '\\';
    //else
    strm << c;
  }
  strm << '\"';
  return strm;
}


/////////////////////////

