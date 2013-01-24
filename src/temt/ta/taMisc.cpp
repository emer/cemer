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

#ifndef NO_TA_BASE
#include <taBase> 
#include <UserDataItemBase> 
#include <taFiler>
#include <css_machine.h>
#include <Program>
#include <taiMiscCore>
#include <UserDataItem_List>
#include <tabMisc>
#include <taRootBase>
#include <taStringDiff>
#include <taiChoiceDialog>

#include <ViewColor_List> 
#include <QDir>
#include <QPointer>
#include <QMainWindow>
#include <QDateTime>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostAddress>
#include <QList>
#endif

#ifdef TA_OS_WIN
#include "windows.h"
#include <errno.h>
#ifndef NO_TA_BASE
# include "shlobj.h"
#endif

#elif defined(TA_OS_UNIX)

#include <unistd.h>
#include <time.h>
#include <csignal>

#ifdef TA_OS_MAC
# include <sys/types.h>
# include <sys/sysctl.h>
#endif

#endif

#include <sstream>              // for FormatValue
#include <math.h>               // for NiceRoundNumber

using namespace std;

InitProcRegistrar::InitProcRegistrar(init_proc_t init_proc) {
  taMisc::AddInitHook(init_proc);
}

String  taMisc::app_name = "temt"; // must be set in main.cpp
String  taMisc::app_prefs_key; // must be set in main.cpp, else defaults to app_name
String  taMisc::default_app_install_folder_name = "Emergent";
String  taMisc::app_suffix;
String  taMisc::org_name = "ccnlab";

#ifndef SVN_REV // won't be defined if svnrev.h wasn't included
#define SVN_REV -1
#endif

String  taMisc::svn_rev = String(SVN_REV);
String  taMisc::version = String(VERSION);
taVersion taMisc::version_bin(String(VERSION));

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

taThreadDefaults::taThreadDefaults() {
  cpus = 1;
  n_threads = -1;
  alloc_pct = 0.0f;
  min_units = 3000;
  compute_thr = 0.5f;
  nibble_chunk = 8;
}

taThreadDefaults taMisc::thread_defaults;

bool    taMisc::save_old_fmt = false;

////////////////////////////////////////////////////////
//      TA GUI parameters

// parameters that are strictly platform specific
#ifdef TA_OS_MAC
String  taMisc::font_name = "Lucida Grande";
int     taMisc::font_size = 10;
String  taMisc::console_font_name = "Andale Mono";
int     taMisc::console_font_size = 10;
float   taMisc::doc_text_scale = 1.0f;
#elif defined(TA_OS_WIN)
String  taMisc::font_name = "Arial";
int     taMisc::font_size = 8;
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
String  taMisc::t3d_bg_color = "grey80";
String  taMisc::t3d_text_color = "black";

// parameters that differ between win and unix
taMisc::ConsoleType taMisc::console_type = CT_GUI;
taMisc::ConsoleOptions taMisc::console_options = CO_GUI_TRACKING;

taMisc::GuiStyle taMisc::gui_style = taMisc::GS_DEFAULT;
int     taMisc::display_width = 80;
int     taMisc::max_display_width = 180;
int     taMisc::indent_spc = 2;
int     taMisc::display_height = 25;
#ifdef TA_OS_MAC
bool    taMisc::emacs_mode = true;
#else
bool    taMisc::emacs_mode = false;
#endif
int     taMisc::undo_depth = 100;
int     taMisc::undo_data_max_cells = 10000;
float   taMisc::undo_new_src_thr = 0.3f;
bool    taMisc::undo_debug = false;
int     taMisc::auto_save_interval = 120;
int     taMisc::wait_proc_delay = 20;
int     taMisc::css_gui_event_interval = 200;
bool    taMisc::delete_prompts = false;
//bool  taMisc::delete_prompts = true;
int     taMisc::tree_indent = 12; // 12 is necessary in 4.7 -- otherwise widgets cut off
taMisc::HelpDetail taMisc::help_detail = taMisc::HD_DEFAULT;
int     taMisc::program_editor_width = 60;
int     taMisc::program_editor_lines = 5;
int     taMisc::max_menu = 1000; // no cost now in QT for making it large..
int     taMisc::search_depth = 4;
int     taMisc::color_scale_size = 128;
int     taMisc::jpeg_quality = 95;
taMisc::ColorHints taMisc::color_hints = (taMisc::ColorHints)(taMisc::CH_EDITS | taMisc::CH_BROWSER);
//note: we actually init this in ta_project, the first time, for the user
//taMisc::ProjViewPref taMisc::proj_view_pref = (taMisc::ProjViewPref)-1;//taMisc::PVP_2x2;
// nobody besides Brad uses 2x2..
taMisc::ProjViewPref taMisc::proj_view_pref = taMisc::PVP_3PANE;
taMisc::ViewerOptions   taMisc::viewer_options = (taMisc::ViewerOptions)(taMisc::VO_DOUBLE_CLICK_EXP_ALL | taMisc::VO_AUTO_SELECT_NEW | taMisc::VO_AUTO_EXPAND_NEW);
#ifndef NO_TA_BASE
ViewColor_List* taMisc::view_colors = NULL;
#endif
taMisc::EditStyle       taMisc::select_edit_style = taMisc::ES_ACTIVE_CONTROL;
taMisc::EditStyle       taMisc::std_edit_style = taMisc::ES_ALL_CONTROLS;

int     taMisc::antialiasing_level = 4;
float   taMisc::text_complexity = .2f;

TypeItem::ShowMembs     taMisc::show_gui = TypeItem::NORM_MEMBS;
taMisc::TypeInfo        taMisc::type_info_ = taMisc::NO_OPTIONS_LISTS;
taMisc::KeepTokens      taMisc::keep_tokens = taMisc::Tokens;
bool                    taMisc::auto_edit = false;
taMisc::AutoRevert      taMisc::auto_revert = taMisc::AUTO_APPLY;
taMisc::MatrixView      taMisc::matrix_view = taMisc::BOT_ZERO;
bool                    taMisc::beep_on_error = false;
short                   taMisc::num_recent_files = 10;
short                   taMisc::num_recent_paths = 10;
short                   taMisc::num_browse_history = 20;

////////////////////////////////////////////////////////
//      File/Path/Arg Info

int     taMisc::strm_ver = 2;
bool            taMisc::save_compress = false; // compression not the default in v4
#ifndef NO_TA_BASE
taLicense::StdLicense taMisc::license_def = taLicense::NO_LIC;
#endif
String          taMisc::license_owner;
String          taMisc::license_org;
taMisc::SaveFormat      taMisc::save_format = taMisc::PRETTY;
taMisc::LoadVerbosity   taMisc::verbose_load = taMisc::QUIET;

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

// note: app should set all these url's in its main or other app-specific code
String  taMisc::web_home = "http://grey.colorado.edu/emergent/index.php/Main_Page";
String  taMisc::web_help_wiki = "emergent";
String  taMisc::web_help_general = "http://grey.colorado.edu/emergent/index.php/User_hub";

NamedURL        taMisc::wiki1_url("emergent", "http://grey.colorado.edu/emergent");
NamedURL        taMisc::wiki2_url("CCN", "http://grey.colorado.edu/CompCogNeuro");
NamedURL        taMisc::wiki3_url;
NamedURL        taMisc::wiki4_url;
NamedURL        taMisc::wiki5_url;
NamedURL        taMisc::wiki6_url;

NameVar_PArray  taMisc::wikis;

String          taMisc::cluster1_name;
String          taMisc::cluster2_name;
String          taMisc::cluster3_name;
String          taMisc::cluster4_name;
String          taMisc::cluster5_name;
String          taMisc::cluster6_name;

String_PArray   taMisc::cluster_names;

NamedURL        taMisc::svn_repo1_url;
NamedURL        taMisc::svn_repo2_url;
NamedURL        taMisc::svn_repo3_url;
NamedURL        taMisc::svn_repo4_url;
NamedURL        taMisc::svn_repo5_url;
NamedURL        taMisc::svn_repo6_url;

NameVar_PArray  taMisc::svn_repos;

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
String  taMisc::plugin_make_env_cmd = "call \"C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\vcvarsall.bat\" x86";
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
QPointer<QMainWindow> taMisc::console_win = NULL;
#endif

void (*taMisc::WaitProc)() = NULL;
bool    taMisc::do_wait_proc = false;

void (*taMisc::ScriptRecordingGui_Hook)(bool) = NULL; // gui callback when script starts/stops; var is 'start'

String  taMisc::console_chars;
String  taMisc::LexBuf;
int taMisc::err_cnt;
fstream taMisc::log_stream;
String taMisc::log_fname;

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

namespace {
  void addUrl(NameVar_PArray &arr, const NamedURL &nurl) {
    if (nurl.name.nonempty() && nurl.url.nonempty()) {
      arr.Add(NameVar(nurl.name, nurl.url));
    }
  }
}

void taMisc::UpdateAfterEdit() {
#ifndef NO_TA_BASE
  wikis.Reset();
  addUrl(wikis, wiki1_url);
  addUrl(wikis, wiki2_url);
  addUrl(wikis, wiki3_url);
  addUrl(wikis, wiki4_url);
  addUrl(wikis, wiki5_url);
  addUrl(wikis, wiki6_url);

  cluster_names.Reset();
  if (cluster1_name.nonempty()) cluster_names.Add(taMisc::cluster1_name);
  if (cluster2_name.nonempty()) cluster_names.Add(taMisc::cluster2_name);
  if (cluster3_name.nonempty()) cluster_names.Add(taMisc::cluster3_name);
  if (cluster4_name.nonempty()) cluster_names.Add(taMisc::cluster4_name);
  if (cluster5_name.nonempty()) cluster_names.Add(taMisc::cluster5_name);
  if (cluster6_name.nonempty()) cluster_names.Add(taMisc::cluster6_name);

  svn_repos.Reset();
  addUrl(svn_repos, svn_repo1_url);
  addUrl(svn_repos, svn_repo2_url);
  addUrl(svn_repos, svn_repo3_url);
  addUrl(svn_repos, svn_repo4_url);
  addUrl(svn_repos, svn_repo5_url);
  addUrl(svn_repos, svn_repo6_url);
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

void taMisc::Warning(const char* a, const char* b, const char* c, const char* d,
  const char* e, const char* f, const char* g, const char* h, const char* i)
{
#if defined(DMEM_COMPILE)
//TODO: should provide a way to log these somehow
//  if(taMisc::dmem_proc > 0) return;
#endif
  taMisc::last_warn_msg = SuperCat(a, b, c, d, e, f, g, h, i);
#ifndef NO_TA_BASE
  if(cssMisc::cur_top && !taMisc::is_loading) {
    taMisc::last_warn_msg += String("\n") + cssMisc::GetSourceLoc(NULL);
    if(cssMisc::cur_top->own_program) {
      bool running = cssMisc::cur_top->state & cssProg::State_Run;
      cssMisc::cur_top->own_program->taWarning(cssMisc::GetSourceLn(NULL), running,
                                              taMisc::last_err_msg);
    }
  }
#endif
  String wmsg = "***WARNING: " + taMisc::last_warn_msg;
  taMisc::LogEvent(wmsg);
  taMisc::ConsoleOutput(wmsg, true, false);
}

void taMisc::Info(const char* a, const char* b, const char* c, const char* d,
  const char* e, const char* f, const char* g, const char* h, const char* i)
{
#if defined(DMEM_COMPILE)
//TODO: should provide a way to log these somehow
  if(taMisc::dmem_proc > 0) return;
#endif
  String msg = SuperCat(a, b, c, d, e, f, g, h, i);
  taMisc::LogEvent(msg);
  taMisc::ConsoleOutput(msg, false, false); // no pager
}

String taMisc::SuperCat(const char* a, const char* b, const char* c,
                      const char* d, const char* e, const char* f,
                      const char* g, const char* h, const char* i)
{
  STRING_BUF(s, 250);
  s.cat(a); if(b) s.cat(" ").cat(b);  if(c) s.cat(" ").cat(c);
  if(d) s.cat(" ").cat(d); if(e) s.cat(" ").cat(e); if(f) s.cat(" ").cat(f);
  if(g) s.cat(" ").cat(g); if(h) s.cat(" ").cat(h); if(i) s.cat(" ").cat(i);
  return s;
}

void taMisc::CheckError(const char* a, const char* b, const char* c, const char* d,
  const char* e, const char* f, const char* g, const char* h, const char* i)
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
}

#ifndef NO_TA_BASE
bool taMisc::TestError(const taBase* obj, bool test, const char* fun_name,
                       const char* a, const char* b, const char* c, const char* d,
                       const char* e, const char* f, const char* g, const char* h) {
  static taBase* prv_obj = NULL;
  static String prv_fun;
  static String prv_a;

  if(!test) return false;

#ifdef DMEM_COMPILE
  //  if(taMisc::dmem_proc > 0) return true;
#endif

  if(obj) {
    String objinfo = "Error in: " + obj->GetTypeDef()->name + " " + obj->GetDisplayName() + "::" + fun_name
      + "() (path: " + obj->GetPathNames() + ")\n";

    if((obj == prv_obj) && (prv_fun == fun_name) && (prv_a == a)) {
      // nogui version for repeat!
      taMisc::Error_nogui(objinfo, a, b, c, d, e, f, g, h);
    }
    else {
      // default gui version
      taMisc::Error(objinfo, a, b, c, d, e, f, g, h);
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

bool taMisc::TestWarning(const taBase* obj, bool test, const char* fun_name,
                         const char* a, const char* b, const char* c, const char* d,
                         const char* e, const char* f, const char* g, const char* h) {
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

void taMisc::Error_nogui(const char* a, const char* b, const char* c, const char* d,
                         const char* e, const char* f, const char* g, const char* h, const char* i)
{
  ++err_cnt;
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  // this is actually a bad idea -- can miss ideosynchratic errors, and if you
  // get an error these days it is terminal anyway, so just bail!!
//   if(taMisc::dmem_proc > 0) return;
#endif
  //  if (beep_on_error) cerr << '\a'; // BEL character
  taMisc::last_err_msg = SuperCat(a, b, c, d, e, f, g, h, i);
#if !defined(NO_TA_BASE)
  if(cssMisc::cur_top) {
    taMisc::last_err_msg += String("\n") + cssMisc::GetSourceLoc(NULL);
  }
#endif
  String fmsg = "***ERROR: " + taMisc::last_err_msg;
  taMisc::LogEvent(fmsg);
  taMisc::ConsoleOutput(fmsg, true, false);
#if !defined(NO_TA_BASE)
  if(cssMisc::cur_top) {
    if(cssMisc::cur_top->own_program) {
      bool running = cssMisc::cur_top->state & cssProg::State_Run;
      cssMisc::cur_top->own_program->taError(cssMisc::GetSourceLn(NULL), running,
                                              taMisc::last_err_msg);
    }
    cssMisc::cur_top->run_stat = cssEl::ExecError; // tell css that we've got an error
    cssMisc::cur_top->exec_err_msg = taMisc::last_err_msg;
  }
#endif
}

#ifdef TA_NO_GUI
// we put the no-gui versions here, to avoid dragging in all the gui stuff
// the gui versions are in ta_type_qt.cc

void taMisc::Error(const char* a, const char* b, const char* c, const char* d,
  const char* e, const char* f, const char* g, const char* h, const char* i)
{
  Error_nogui(a,b,c,d,e,f,g,h,i);
}

int taMisc::Choice(const char* text, const char* a, const char* b, const char* c,
  const char* d, const char* e, const char* f, const char* g, const char* h, const char* i)
{
  int m=-1;
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0) return -1;
#endif
  {
    int   chn = 0;
    String chstr = text;
    chstr += "\n";
    if (a) { chstr += String("0: ") + a + "\n"; chn++; }
    if (b) { chstr += String("1: ") + b + "\n"; chn++; }
    if (c) { chstr += String("2: ") + c + "\n"; chn++; }
    if (d) { chstr += String("3: ") + d + "\n"; chn++; }
    if (e) { chstr += String("4: ") + e + "\n"; chn++; }
    if (f) { chstr += String("5: ") + f + "\n"; chn++; }
    if (g) { chstr += String("6: ") + g + "\n"; chn++; }
    if (h) { chstr += String("7: ") + h + "\n"; chn++; }
    if (i) { chstr += String("8: ") + i + "\n"; chn++; }

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

void taMisc::Confirm(const char* a, const char* b, const char* c,
  const char* d, const char* e, const char* f, const char* g,
  const char* h, const char* i)
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

#endif // def TA_NO_GUI

void taMisc::DebugInfo(const char* a, const char* b, const char* c, const char* d,
       const char* e, const char* f, const char* g, const char* h, const char* i)
{
  String ad = String("*** DEBUG: ") + a;
#ifdef DEBUG
  taMisc::Info(ad, b, c, d, e, f, g, h, i);
#else
  String msg = SuperCat(ad, b, c, d, e, f, g, h, i);
  taMisc::LogEvent(msg);
#endif
}

void taMisc::LogInfo(const char* a, const char* b, const char* c, const char* d,
       const char* e, const char* f, const char* g, const char* h, const char* i)
{
  String ad = String("*** LOG: ") + a;
  String msg = SuperCat(ad, b, c, d, e, f, g, h, i);
  taMisc::LogEvent(msg);
}

void taMisc::LogEvent(const String& log_data) {
  time_t tmp = time(NULL);
  String tstamp = ctime(&tmp);
  tstamp = tstamp.before('\n');
  if(taMisc::log_stream.bad()) {
    if(taMisc::gui_active) {
      cout << tstamp << ": " << log_data << endl;
    }
  }
  else {
    taMisc::log_stream << tstamp << ": " << log_data << endl;
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
    String bkup_fn = user_log_dir + "/default_project_log.plog";
    taMisc::Error("taMisc::SetLogFile -- Could not open log stream as:", log_fn,
                  "reverting to default:", bkup_fn);
    taMisc::log_stream.close();
    taMisc::log_stream.clear();
    taMisc::log_stream.open(bkup_fn, ios::out);
    taMisc::log_fname = bkup_fn;
  }
  else {
    taMisc::LogEvent("taMisc::SetLogFile -- Log file opened for writing.");
  }
}

void taMisc::EditFile(const String& filename) {
  String edtr = taMisc::edit_cmd; //don't run gsub on the original string!
  edtr.gsub("%s", filename);
  taMisc::ExecuteCommand(edtr);
}

/////////////////////////////////////////////////
//      Global state management

TypeDef* taMisc::FindTypeName(const String& typ_nm) {
  TypeDef* td = taMisc::types.FindName(typ_nm);
  if(td != NULL) return td;
  for(int i=0; i<taMisc::aka_types.size; i++) {
    TypeDef* aka = taMisc::aka_types.FastEl(i);
    String aka_nm = aka->OptionAfter("AKA_");
    if(aka_nm == typ_nm) {
      return aka;
    }
  }
  taMisc::Warning("Unknown type:",typ_nm);
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
  if(pager) {
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
  if(!taMisc::interactive) pager = false;
  int pageln = 0;
  String rmdr = str;
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

        String curpt = longln.before(wrap_point);
        longln = longln.from(wrap_point);
        if(was_wrap)
          curpt = "->" + curpt;
        if(longln.nonempty())
          curpt += "->";
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



/////////////////////////////////////////////////
//      Startup

void taMisc::Initialize() {
  not_constr = false;
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

  prog_lib_paths.AddUnique(NameVar("SystemLib",
    (Variant)(app_dir + PATH_SEP + "prog_lib")));
  prog_lib_paths.AddUnique(NameVar("UserLib",
    (Variant)(user_app_dir + PATH_SEP + "prog_lib")));
  prog_lib_paths.AddUnique(NameVar("WebLib",
    (Variant)(web_home + "/prog_lib"))); //note: urls always use '/'

  proj_template_paths.AddUnique(NameVar("SystemLib",
    (Variant)(app_dir + PATH_SEP + "proj_templates")));
  proj_template_paths.AddUnique(NameVar("UserLib",
    (Variant)(user_app_dir + PATH_SEP + "proj_templates")));
  proj_template_paths.AddUnique(NameVar("WebLib",
    (Variant)(web_home + "/proj_templates"))); //note: urls always use '/'

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
    thread_defaults.n_threads = thread_defaults.cpus;

  int n_threads = FindArgByName("NThreads").toInt(); // 0 if doesn't exist
  if(n_threads > 0) {
    thread_defaults.n_threads = n_threads;
    taMisc::Info("Set n_threads to:", String(n_threads));
  }
  if(thread_defaults.n_threads > thread_defaults.cpus)
    thread_defaults.n_threads = thread_defaults.cpus;

  if(CheckArgByName("ThreadAllocPct")) {
    float alc_pct = FindArgByName("ThreadAllocPct").toFloat();
    thread_defaults.alloc_pct = alc_pct;
    taMisc::Info("Set threads alloc_pct to:", String(alc_pct));
  }

  int nib_chk = FindArgByName("ThreadNibbleChunk").toInt(); // 0 if doesn't exist
  if(nib_chk > 0) {
    thread_defaults.nibble_chunk = nib_chk;
    taMisc::Info("Set threads nibble_chunk to:", String(nib_chk));
  }

  if(CheckArgByName("ThreadComputeThr")) {
    float cmp_thr = FindArgByName("ThreadComputeThr").toFloat();
    thread_defaults.compute_thr = cmp_thr;
    taMisc::Info("Set threads compute_thr to:", String(cmp_thr));
  }

  int min_un = FindArgByName("ThreadMinUnits").toInt(); // 0 if doesn't exist
  if(min_un > 0) {
    thread_defaults.min_units = min_un;
    taMisc::Info("Set threads min_units to:", String(min_un));
  }

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

static void init_inventor_type(TypeDef* typ) {
  // look for an initClass method
  MethodDef* md = typ->methods.FindName("initClass");
  if (!md)
    md = typ->methods.FindName("InitClass");
  if (!md) return;
  if (!(md->is_static && md->addr && (md->arg_types.size == 0) )) return;
  // call the init function
  md->addr();
  md->addr = NULL;          // reset so it isn't run again!
}


void taMisc::Init_Types() {// called after all type info has been loaded into types
  // initialize all classes that have an initClass method (ex. Inventor subtypes)
  if(taMisc::use_gui) {
    for (int i = 0; i < types.size; ++i) {
      TypeDef* typ = types.FastEl(i);
      if ((typ->ptr > 0) || (typ->ref)) continue;
      init_inventor_type(typ);
    }
  }
  // add any Schema that couldn't be added earlier
  AddDeferredUserDataSchema();
  // other stuff could happen here..

  // go through all types and create list of AKA typedefs
  for (int i = 0; i < types.size; ++i) {
    TypeDef* typ = types.FastEl(i);
    if(typ->OptionAfter("AKA_").nonempty()) {
      aka_types.Link(typ);
    }
  }
}

void taMisc::AddUserDataSchema(const String& type_name, UserDataItemBase* item) {
  // calls could come very early in startup, even before .types or its contents
  // are created (static data gets created in "random" order in C++)
  // so we will add now if the type exists, otherwise we add to a deferred list
  TypeDef* typ = NULL;
  //TODO: gcc complains that &types is always true!
  if (&types) {
    typ = types.FindName(type_name);
  }
  if (typ) {
    typ->AddUserDataSchema(item);
  } else { // too early, or bad type_name
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
    TypeDef* typ = types.FindName(type_name);
    if (typ) {
      typ->AddUserDataSchema(item);
    } else {
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
  if(args.FindName(nm) < 0) return false;
  return true;
}

String taMisc::FindArgByName(const String& nm) {
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
  return true;
}

String taMisc::FindArgValContains(const String& vl) {
  int idx = args.FindValueContains(vl);
  if(idx < 0) return _nilString;
  return args.FastEl(idx).value.toString();
}

#endif // NO_TA_BASE for al startup/args functions

/////////////////////////////////////////////////
//      Commonly used utility functions on strings/arrays/values

void taMisc::CharToStrArray(String_PArray& sa, const char* ch) {
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
  DWORD retVal = GetTempPath(BUFSIZE, tmpbuf);
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

bool taMisc::RenameFile(const String& old_fn, const String& new_fn) {
  QDir d;
  return d.rename(old_fn, new_fn);
}

bool taMisc::RemoveFile(const String& fn) {
  QDir d;
  return d.remove(fn);
}

bool taMisc::MakeDir(const String& fn) {
  QDir d;
  return d.mkdir(fn);
}

bool taMisc::MakePath(const String& fn) {
  QDir d;
  return d.mkpath(fn);
}

bool taMisc::RemoveDir(const String& fn) {
  QDir d;
  return d.rmdir(fn);
}

bool taMisc::RemovePath(const String& fn) {
  QDir d;
  return d.rmpath(fn);
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
String taMisc::FindFileOnPath(String_PArray& paths, const char* fname) {
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

String taMisc::FindFileOnLoadPath(const char* fname) {
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

bool taMisc::CreateNewSrcFiles(const String& type_nm, const String& top_path,
			       const String& src_dir) {
  String src_path = top_path + "/" + src_dir + "/";
  String crfile = src_path + "COPYRIGHT.txt";
  String cmfile = src_path + "CMakeFiles.txt";
  String hfile = src_path + type_nm + ".h";
  String cppfile = src_path + type_nm + ".cpp";
  String incfile = top_path + "/include/" + type_nm;
  String incfileh = incfile + ".h";

  if(!FileExists(crfile)) {
    taMisc::Error("CreateNewSrcFiles: copyright file:", crfile, "not found -- paths must be wrong -- aborting");
    return false;
  }

  fstream crstrm;
  crstrm.open(crfile, ios::in);
  String crstr;
  crstr.Load_str(crstrm);
  crstrm.close();

  bool got_one = false;

  if(FileExists(hfile)) {
    taMisc::Warning("header file:", hfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n#ifndef " << type_nm << "_h\n"
        << "#define " << type_nm << "_h 1\n\n"
        << "// parent includes:\n"
        << "#include <taNBase>\n\n"
        << "// member includes:\n\n"
        << "// declare all other types mentioned but not required to include:\n\n"
        << "TypeDef_Of(" << type_nm << ");\n\n"
        << "class X_API " << type_nm << " : public taNBase {\n"
        << "  // <describe here in full detail in one extended line comment>\n"
        << "INHERITED(taNBase)\n"
        << "public:\n\n"
        << "  TA_SIMPLE_BASEFUNS(" << type_nm << ");\n"
        << "private:\n"
        << "  void Initialize()  { };\n"
        << "  void Destroy()     { };\n"
        << "};\n\n"
        << "#endif // " << type_nm << "_h\n";
    fstream strm;
    strm.open(hfile, ios::out);
    str.Save_str(strm);
    strm.close();
    ExecuteCommand("svn add " + hfile);
    got_one = true;
  }

  if(FileExists(cppfile)) {
    taMisc::Warning("cpp file:", cppfile, "already exists, not changing");
  }
  else {
    String str = crstr;
    str << "\n#include \"" << type_nm << ".h\"\n\n";
    fstream strm;
    strm.open(cppfile, ios::out);
    str.Save_str(strm);
    strm.close();
    ExecuteCommand("svn add " + cppfile);
    got_one = true;
  }

  if(FileExists(incfile)) {
    taMisc::Warning("include file:", incfile, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"../" << src_dir << "/" << type_nm << ".h\"\n";
    fstream strm;
    strm.open(incfile, ios::out);
    str.Save_str(strm);
    strm.close();
    ExecuteCommand("svn add " + incfile);
    got_one = true;
  }

  if(FileExists(incfileh)) {
    taMisc::Warning("include file:", incfileh, "already exists, not changing");
  }
  else {
    String str;
    str << "#include \"../" << src_dir << "/" << type_nm << ".h\"\n";
    fstream strm;
    strm.open(incfileh, ios::out);
    str.Save_str(strm);
    strm.close();
    ExecuteCommand("svn add " + incfileh);
    got_one = true;
  }

  fstream cmstrm;
  cmstrm.open(cmfile, ios::in);
  String cmstr;
  cmstr.Load_str(cmstrm);
  cmstrm.close();
  bool changed = false;

  if(!cmstr.contains(type_nm + ".h")) {
    String cmrest = cmstr.after(".h\n)");
    cmstr = cmstr.before(".h\n)");
    cmstr << ".h\n  " << type_nm << ".h\n)" << cmrest;
    changed = true;
  }
  if(!cmstr.contains(type_nm + ".cpp")) {
    String cmrest = cmstr.after(".cpp\n)");
    cmstr = cmstr.before(".cpp\n)");
    cmstr << ".cpp\n  " << type_nm << ".cpp\n)" << cmrest;
    changed = true;
  }

  if(changed) {
    fstream cmostrm;
    cmostrm.open(cmfile, ios::out);
    cmstr.Save_str(cmostrm);
  }

  return got_one;
}

void taMisc::CreateAllNewSrcFiles() {
#ifndef NO_TA_BASE
  //  TA_TimeUsed.CreateNewSrcFiles("/home/oreilly/emergent", "src/temt/ta");
  // TA_DataView_List.CreateNewSrcFiles("/home/oreilly/emergent", "src/temt/ta");

  int_Array iary;
  int i=0;
  while(i < types.size) {
    TypeDef* typ = types.FastEl(i);
    bool dbg = false;
    if(!typ->IsClass() || !typ->IsAnchor()) {
      if(dbg) taMisc::Info("fail class, anchor");
      i++;
      continue;
    }
    if(!typ->source_file.startsWith(typ->name)) {
      if(dbg) taMisc::Info("fail src file");
      i++;
      continue;
    }
    int chs = taMisc::Choice("Fix new source file for: " + typ->name,
			     "Yes", "No", "Back", "Cancel");
    if(chs == 3) break;
    if(chs == 2) {
      if(iary.size > 0) {
	i = iary.Pop();
      }
      continue;
    }
    iary.Add(i);
    if(chs == 1) {
      i++;
      continue;
    }
    if(chs == 0) {
      typ->CreateNewSrcFiles("/home/oreilly/emergent", "src/temt/ta");
      i++;
    }
  }
#endif
}

bool taMisc::CreateNewSrcFilesExisting(const String& type_nm, const String& top_path,
				       const String& src_dir) {
  TypeDef* td = types.FindName(type_nm);
  if(!td) {
    taMisc::Error("type not found:", type_nm);
    return false;
  }
  return td->CreateNewSrcFiles(top_path, src_dir);
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

bool taMisc::RecordScript(const char* cmd) {
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
      md->type->GetValStr(md->GetOff(tab)) << ";" << "\n";
  }
}
// Script Record Inline Assignment
void taMisc::SRIAssignment(taBase* tab,MemberDef* md){
  if(record_on)  {
    record_script << tab->GetPathNames() << "." << md->name << " = \"" <<
      md->type->GetValStr(md->GetOff(tab)) << "\";\n";
    record_script << tab->GetPathNames() << "." << "UpdateAfterEdit();" << "\n";
  }
}

// Script Record Enum Assignment
void taMisc::SREAssignment(taBase* tab,MemberDef* md){
  if(record_on)  {
    record_script << tab->GetPathNames() << "." << md->name << " = " <<
      tab->GetTypeDef()->name << "::" <<
      md->type->GetValStr(md->GetOff(tab)) << ";" << "\n";
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

#ifndef NO_TA_BASE

void taMisc::Error(const char* a, const char* b, const char* c, const char* d,
  const char* e, const char* f, const char* g, const char* h, const char* i)
{
  ++err_cnt;
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  //TODO: should log errors on nodes > 0!!!
  if(taMisc::dmem_proc > 0) return;
#endif
  taMisc::last_err_msg = SuperCat(a, b, c, d, e, f, g, h, i);
  String fmsg = "***ERROR: " + taMisc::last_err_msg;
  taMisc::LogEvent(fmsg);
#if !defined(NO_TA_BASE)
  if(taMisc::ErrorCancelCheck()) {
    return;			// cancel!
  }
#endif
  // we always output to console
  // if (beep_on_error) cerr << '\a'; // BEL character
#if !defined(NO_TA_BASE) 
  if(cssMisc::cur_top && !taMisc::is_loading) {
    taMisc::last_err_msg += String("\n") + cssMisc::GetSourceLoc(NULL);
  }
#endif
  taMisc::ConsoleOutput(fmsg, true, false);
#if !defined(NO_TA_BASE) 
  if(cssMisc::cur_top && !taMisc::is_loading) {
    if(cssMisc::cur_top->own_program) {
      bool running = cssMisc::cur_top->state & cssProg::State_Run;
      cssMisc::cur_top->own_program->taError(cssMisc::GetSourceLn(NULL), running,
					      taMisc::last_err_msg);
    }
    cssMisc::cur_top->run_stat = cssEl::ExecError; // tell css that we've got an error
    cssMisc::cur_top->exec_err_msg = taMisc::last_err_msg;
  }
  if (taMisc::gui_active && !taMisc::is_loading) {
    bool cancel = taiChoiceDialog::ErrorDialog(NULL, taMisc::last_err_msg);
    taMisc::ErrorCancelSet(cancel);
  }
#endif
}

int taMisc::Choice(const char* text, const char* a, const char* b, const char* c,
  const char* d, const char* e, const char* f, const char* g, const char* h, const char* i)
{
  int m=-1;
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0) return -1;
#endif
#if !defined(NO_TA_BASE)
  if (taMisc::gui_active) {
    String delimiter = taiChoiceDialog::delimiter;
    int   chn = 0;
    String chstr = delimiter;
    if (a) { chstr += String(a) + delimiter; chn++; }
    if (b) { chstr += String(b) + delimiter; chn++; }
    if (c) { chstr += String(c) + delimiter; chn++; }
    if (d) { chstr += String(d) + delimiter; chn++; }
    if (e) { chstr += String(e) + delimiter; chn++; }
    if (f) { chstr += String(f) + delimiter; chn++; }
    if (g) { chstr += String(g) + delimiter; chn++; }
    if (h) { chstr += String(h) + delimiter; chn++; }
    if (i) { chstr += String(i) + delimiter; chn++; }
    m = taiChoiceDialog::ChoiceDialog(NULL, text, chstr);
  } else
#endif
  {
    int   chn = 0;
    String chstr = text;
    chstr += "\n";
    if (a) { chstr += String("0: ") + a + "\n"; chn++; }
    if (b) { chstr += String("1: ") + b + "\n"; chn++; }
    if (c) { chstr += String("2: ") + c + "\n"; chn++; }
    if (d) { chstr += String("3: ") + d + "\n"; chn++; }
    if (e) { chstr += String("4: ") + e + "\n"; chn++; }
    if (f) { chstr += String("5: ") + f + "\n"; chn++; }
    if (g) { chstr += String("6: ") + g + "\n"; chn++; }
    if (h) { chstr += String("7: ") + h + "\n"; chn++; }
    if (i) { chstr += String("8: ") + i + "\n"; chn++; }

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

void taMisc::Confirm(const char* a, const char* b, const char* c,
  const char* d, const char* e, const char* f, const char* g,
  const char* h, const char* i)
{
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if (taMisc::dmem_proc > 0) return;
#endif
  String msg = SuperCat(a, b, c, d, e, f, g, h, i);
  taMisc::LogEvent("***CONFIRM: " + msg);
  taMisc::ConsoleOutput(msg, false, false);
#if !defined(NO_TA_BASE)
  if (taMisc::gui_active) {
    taiChoiceDialog::ConfirmDialog(NULL, msg);
  }
#endif
}

#endif
