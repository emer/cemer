// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_type.cc: Type Access Main code

#include "ta_type.h"
#include "ta_platform.h"
#include "ta_variant.h"

#ifndef NO_TA_BASE
# include "ta_group.h"
# include "ta_dump.h"
# include "ta_project.h" // for taRootBase
# include "ta_program.h"
# include "colorscale.h"
# include "ta_thread.h"
# ifdef DMEM_COMPILE
#   include "ta_dmem.h"
# endif
# include "ta_TA_type.h"
# include <QDir>
# include <QCoreApplication>
# include <QTimer>
# include <QNetworkInterface>
# include <QNetworkAddressEntry>
# include <QHostAddress>
# include <QList>
# include "css_machine.h"	// for setting error code in taMisc::Error
# ifdef TA_GUI
#   include "ta_qtdata.h"
#   include "ta_qttype.h"
#   include <QMainWindow> 
#   include "igeometry.h"
# endif // TA_GUI
#endif // NO_TA_BASE


#include <sstream>		// for FormatValue
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#ifndef TA_OS_WIN
# include <sys/time.h>
# include <sys/times.h>
#endif

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif

const String String_PArray::def_sep(", ");

const String String_PArray::AsString(const String& sep) const {
  if (size == 0) return _nilString;
  // more efficient to know the length, so we don't resize...
  int xlen = MAX(0, (size - 1) * sep.length()); // for seps
  int i;
  for (i=0; i < size; ++i) xlen += FastEl(i).length();
  String rval(0, xlen, '\0');
  for (i = 0; i < size; ++i) {
    if (i > 0) 
      rval.cat(sep);
    rval.cat(FastEl(i));
  }
  return rval;
}

int String_PArray::FindContains(const String& op, int start) const {
  int i;
  if(start < 0) {		// search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).contains(op))
	return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).contains(op))
	return i;
    }
  }
  return -1;
}

int String_PArray::FindStartsWith(const String& op, int start) const {
  int i;
  if(start < 0) {		// search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).startsWith(op))
	return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).startsWith(op))
	return i;
    }
  }
  return -1;
}

void String_PArray::SetFromString(String str, const String& sep) {
  Reset();
  int pos = str.index(sep);
  while (pos >= 0) {
    Add(str.left(pos)); // could be empty
    str = str.after(pos + sep.length() - 1);
    pos = str.index(sep);
  }
  if (str.nonempty())
    Add(str);
}

///////////////////////////////////////
// NameVar_PArray

const String NameVar_PArray::def_sep(", ");

const String NameVar_PArray::AsString(const String& sep) const {
  if (size == 0) return _nilString;
  int i;
  String rval;
  for (i = 0; i < size; ++i) {
    if (i > 0) 
      rval.cat(sep);
    rval.cat(FastEl(i).GetStr());
  }
  return rval;
}

int NameVar_PArray::FindName(const String& op, int start) const {
  int i;
  if(start < 0) {		// search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).name == op)
	return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).name == op)
	return i;
    }
  }
  return -1;
}

int NameVar_PArray::FindNameContains(const String& op, int start) const {
  int i;
  if(start < 0) {		// search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).name.contains(op))
	return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).name.contains(op))
	return i;
    }
  }
  return -1;
}

int NameVar_PArray::FindValue(const Variant& op, int start) const {
  int i;
  if(start < 0) {		// search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).value == op)
	return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).value == op)
	return i;
    }
  }
  return -1;
}

int NameVar_PArray::FindValueContains(const String& op, int start) const {
  int i;
  if(start < 0) {		// search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).value.toString().contains(op))
	return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).value.toString().contains(op))
	return i;
    }
  }
  return -1;
}

Variant NameVar_PArray::GetVal(const String& nm) {
  int idx = FindName(nm);
  if(idx < 0) return _nilVariant;
  return FastEl(idx).value;
}

Variant NameVar_PArray::GetValDef(const String& nm, const Variant& def) {
  int idx = FindName(nm);
  if (idx < 0) return def;
  return FastEl(idx).value;
}


bool NameVar_PArray::GetAllVals(const String& nm, String_PArray& vals) {
  for(int i=0;i<size; i++) {
    NameVar& nv = FastEl(i);
    if(nv.name != nm) continue;
    vals.Add(nv.value.toString());
  }
  return (vals.size == 0);
}

bool NameVar_PArray::SetVal(const String& nm, const Variant& vl) {
  int idx = FindName(nm);
  if(idx < 0) {
    Add(NameVar(nm, vl));
    return true;
  }
  else {
    FastEl(idx).value = vl;
    return false;
  }
}

//////////////////////////
//  taiMiscCore		//
//////////////////////////

extern "C" {
  extern int rl_done;
}

#ifndef NO_TA_BASE
TA_API taiMiscCore* taiMC_ = NULL; 

int taiMiscCore::rl_callback() {
  QCoreApplication* app = QCoreApplication::instance();
  if (taMisc::quitting) goto quit_exit;
  if (app)
    app->processEvents();
  if (taMisc::quitting) goto quit_exit;
  return 0; // ???
  
quit_exit:
  rl_done = 1;
  return 0;
}

taiMiscCore* taiMiscCore::New(QObject* parent) {
  taiMiscCore* rval = new taiMiscCore(parent);
  rval->Init();
  return rval;
}

void taiMiscCore::Quit(CancelOp cancel_op) {
  // good place to save config, regardless what happens
  if (tabMisc::root)
    tabMisc::root->Save();

#ifdef DMEM_COMPILE
  MPI_Finalize();
#endif
  taThreadMgr::TerminateAllThreads(); // don't leave any active threads lying around

  taMisc::quitting = (cancel_op == CO_NOT_CANCELLABLE) ? 
    taMisc::QF_FORCE_QUIT : taMisc::QF_USER_QUIT;
  OnQuitting(cancel_op); // saves changes
  if (cancel_op != CO_CANCEL) {
    if (taiMC_) {
      taiMC_->Quit_impl(cancel_op);
    }
  }
  if (cancel_op == CO_CANCEL)
    taMisc::quitting = taMisc::QF_RUNNING;
}

void taiMiscCore::OnQuitting(CancelOp& cancel_op) {
  if (taiMC_) {
    taiMC_->OnQuitting_impl(cancel_op);
  }
  if (cancel_op == CO_CANCEL) {
    taMisc::quitting = taMisc::QF_RUNNING;
  }
}

int taiMiscCore::ProcessEvents() {
  if (taMisc::in_event_loop) {
    QCoreApplication::processEvents();
  }
  return 0;
}

int taiMiscCore::RunPending() {
  if (taMisc::in_event_loop) {
    if(QCoreApplication::hasPendingEvents()) {
      QCoreApplication::processEvents();
      return true;
    }
  }
  return false;
}

void taiMiscCore::WaitProc() {
  tabMisc::WaitProc();
}

taiMiscCore::taiMiscCore(QObject* parent)
:inherited(parent) 
{
}

taiMiscCore::~taiMiscCore() {
  if (taiMC_ == this)
    taiMC_ = NULL;
}
  
void taiMiscCore::app_aboutToQuit() {
//NOTE: Qt will not process any more events at this point!
  if (timer)
    timer->stop();
  // do wait proc, to do final deferred deletes
  WaitProc();
}

void taiMiscCore::CheckConfigResult_(bool ok) {
//note: only called if !quiet, and if !ok only if confirm_success
  if (ok) {
    taMisc::Warning("No configuration errors were found.");
  } else {
    cerr << "/n" << "/n"; // helps group this block together
    taMisc::Warning("Configuration errors were found:\n");
    cerr << taMisc::last_check_msg << endl;
  }
}

const String taiMiscCore::classname() {
  return String(QCoreApplication::instance()->applicationName());
}

void taiMiscCore::customEvent(QEvent* ev) {
  if ((int)ev->type() == (int)CE_QUIT)
    Quit(CO_NOT_CANCELLABLE);
  else inherited::customEvent(ev);
}

int taiMiscCore::Exec() {
  if (taMisc::in_event_loop) {
    taMisc::Error("Attempt to enter event loop a second time!");
    return 0;
  }
  taMisc::in_event_loop = true;
  int rval = 0;
  try {
    rval = Exec_impl();
  } catch(...) {
    taMisc::in_event_loop = false;
    raise(SIGABRT);
  }
  taMisc::in_event_loop = false;
  return rval;
}

int taiMiscCore::Exec_impl() {
  return QCoreApplication::instance()->exec();
}

void taiMiscCore::Init(bool gui) {
  taMisc::gui_active = gui;
  
  connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()),
    this, SLOT(app_aboutToQuit()) );

  // initialize the type system
  taiTypeBase::InitializeTypes(gui);

  // does idle processing in Qt
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
  timer->start(50);
  
}

void taiMiscCore::OnQuitting_impl(CancelOp& cancel_op) {
  // nothing in nongui
}

void taiMiscCore::PostUpdateAfter() {
#ifndef NO_TA_BASE
  dumpMisc::PostUpdateAfter();
#endif
}  

void taiMiscCore::Quit_impl(CancelOp cancel_op) {
  // only for nogui
  QCoreApplication::instance()->quit();
}

void taiMiscCore::timer_timeout() {
  if (taMisc::WaitProc)
    taMisc::WaitProc();
}


#endif // NO_TA_BASE

InitProcRegistrar::InitProcRegistrar(init_proc_t init_proc) {
  taMisc::AddInitHook(init_proc);
}

void taVersion::setFromString(String ver) {
  Clear();
  // parse, mj.mn.st-build -- just blindly go through, harmless if missings
  major = BeforeOrOf('.', ver);
  minor = BeforeOrOf('.', ver);
  step = BeforeOrOf('-', ver);
  build = BeforeOrOf(' ', ver); //dummy
}

int taVersion::BeforeOrOf(char sep, String& in) {
  String s = in.before(sep);
  if (s.nonempty()) {
    in = in.after(sep);
  } else {
    s = in;
    in = _nilString;
  }
  // strip any gunk, which may be at end, ex "b1" etc.
  int i = 0;
  while (i < s.length()) {
    char c = s[i];
    if (!isdigit(c)) break;
    ++i;
  }
  return s.left(i).toInt();
}

//////////////////////////////////
// 	     taMisc		//
//////////////////////////////////

String	taMisc::app_name = "temt"; // must be set in main.cpp
String	taMisc::app_prefs_key; // must be set in main.cpp, else defaults to app_name
String	taMisc::default_app_install_folder_name = "Emergent";

String	taMisc::org_name = "ccnlab"; 

#ifdef SVN_REV
String	taMisc::svn_rev = String(SVN_REV);
#endif

String	taMisc::version = String(VERSION);
taVersion taMisc::version_bin(String(VERSION)); 

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

////////////////////////////////////////////////////////
// 	TA GUI parameters

// parameters that are strictly platform specific
#ifdef TA_OS_MAC
String  taMisc::font_name = "Lucida Grande";
int  	taMisc::font_size = 10;
String  taMisc::console_font_name = "Andale Mono";
int  	taMisc::console_font_size = 10;
#elif defined(TA_OS_WIN)
String  taMisc::font_name = "Verdana"; // looks nice on Win
int  	taMisc::font_size = 11;
String  taMisc::console_font_name = "Fixed";
int  	taMisc::console_font_size = 10;
#else // Linux or some Unix variant
String  taMisc::font_name = "Nimbus Sans";
int  	taMisc::font_size = 10;
String  taMisc::console_font_name = "LucidaTypewriter";
int  	taMisc::console_font_size = 10;
#endif

bool	taMisc::log_console_out = false;
String  taMisc::t3d_font_name = "Arial";

// parameters that differ between win and unix
#ifdef TA_OS_WIN
taMisc::ConsoleType taMisc::console_type = CT_OS_SHELL;
taMisc::ConsoleOptions taMisc::console_options = CO_USE_PAGING_NOGUI; // none
#else
taMisc::ConsoleType taMisc::console_type = CT_GUI;
taMisc::ConsoleOptions taMisc::console_options = (taMisc::ConsoleOptions)(CO_USE_PAGING_NOGUI | CO_GUI_TRACKING);
#endif

taMisc::GuiStyle taMisc::gui_style = taMisc::GS_DEFAULT;
int	taMisc::display_width = 80;
int	taMisc::undo_depth = 100;
float	taMisc::undo_new_src_thr = 0.3f;
bool	taMisc::delete_prompts = false;
//bool	taMisc::delete_prompts = true;
int	taMisc::tree_indent = 10; // 12 used to be default, but 10 seems good
int	taMisc::program_editor_width = 60;
int	taMisc::program_editor_lines = 5;
int	taMisc::max_menu = 1000; // no cost now in QT for making it large..
int 	taMisc::search_depth = 4;
int	taMisc::color_scale_size = 128;
int	taMisc::jpeg_quality = 95;
taMisc::ColorHints taMisc::color_hints = (taMisc::ColorHints)(taMisc::CH_EDITS | taMisc::CH_BROWSER);
//note: we actually init this in ta_project, the first time, for the user
//taMisc::ProjViewPref taMisc::proj_view_pref = (taMisc::ProjViewPref)-1;//taMisc::PVP_2x2;
// nobody besides Brad uses 2x2.. 
taMisc::ProjViewPref taMisc::proj_view_pref = taMisc::PVP_3PANE;
taMisc::ViewerOptions	taMisc::viewer_options = (taMisc::ViewerOptions)(taMisc::VO_DOUBLE_CLICK_EXP_ALL | taMisc::VO_AUTO_SELECT_NEW | taMisc::VO_AUTO_EXPAND_NEW);
#ifndef NO_TA_BASE
ViewColor_List* taMisc::view_colors = NULL;
#endif
taMisc::EditStyle	taMisc::select_edit_style = taMisc::ES_ACTIVE_CONTROL;
taMisc::EditStyle	taMisc::std_edit_style = taMisc::ES_ALL_CONTROLS;

int	taMisc::antialiasing_level = 4;
float	taMisc::text_complexity = .2f;

taMisc::ShowMembs  	taMisc::show_gui = taMisc::NORM_MEMBS;
taMisc::TypeInfo  	taMisc::type_info_ = taMisc::NO_OPTIONS_LISTS;
taMisc::KeepTokens 	taMisc::keep_tokens = taMisc::Tokens;
bool			taMisc::auto_edit = false;
taMisc::AutoRevert 	taMisc::auto_revert = taMisc::AUTO_APPLY;
taMisc::MatrixView	taMisc::matrix_view = taMisc::BOT_ZERO;	
bool 			taMisc::beep_on_error = false;
short			taMisc::num_recent_files = 10;
short			taMisc::num_recent_paths = 10;
short			taMisc::num_browse_history = 20;
 
////////////////////////////////////////////////////////
// 	File/Path/Arg Info

int	taMisc::strm_ver = 2;
bool 		taMisc::save_compress = false; // compression not the default in v4
TypeDef*	taMisc::default_proj_type = NULL;
#ifdef DEBUG
taMisc::SaveFormat	taMisc::save_format = taMisc::PRETTY;
#else
taMisc::SaveFormat	taMisc::save_format = taMisc::PLAIN;
#endif
taMisc::LoadVerbosity	taMisc::verbose_load = taMisc::QUIET;

String	taMisc::app_dir; // set early in startup, algorithmically to find app dir
String	taMisc::app_plugin_dir; 
String	taMisc::app_dir_default; // emergency override, obtained from user
String  taMisc::user_dir;			// this will be set in init call
String	taMisc::prefs_dir; // this must be set at startup!
String	taMisc::user_app_dir; 
String	taMisc::user_plugin_dir; 
String	taMisc::user_log_dir; 

// note: app should set all these url's in its main or other app-specific code
String	taMisc::web_home = "http://grey.colorado.edu/emergent/index.php/Main_Page"; 
String	taMisc::web_help_wiki = "emergent";
String	taMisc::web_help_general = "http://grey.colorado.edu/emergent/index.php/User_hub";

NamedURL	taMisc::wiki1_url("emergent", "http://grey.colorado.edu/emergent");
NamedURL	taMisc::wiki2_url("CCN", "http://grey.colorado.edu/CompCogNeuro");
NamedURL	taMisc::wiki3_url;
NamedURL	taMisc::wiki4_url;
NamedURL	taMisc::wiki5_url;
NamedURL	taMisc::wiki6_url;

NameVar_PArray	taMisc::wikis;

String_PArray	taMisc::css_include_paths;
String_PArray	taMisc::load_paths;
NameVar_PArray	taMisc::prog_lib_paths;
NameVar_PArray	taMisc::named_paths;

DumpFileCvtList taMisc::file_converters; 

String	taMisc::compress_sfx = ".gz";
ostream*	taMisc::record_script = NULL;

// NOTE: we quote all filenames in case they have spaces
#ifdef TA_OS_WIN
//NOTE: Notepad could possibly really screw up files, because of crlf
String	taMisc::edit_cmd = "Notepad.exe \"%s\"";
#else
#ifdef TA_OS_MAC
String	taMisc::edit_cmd = "emacs \"%s\" &";
#else // prob Linux, or some Unix for sure
String	taMisc::edit_cmd = "emacs \"%s\" &";
#endif
#endif

////////////////////////////////////////////////////////
// 	Args

String_PArray	taMisc::args_raw;
NameVar_PArray	taMisc::arg_names;
NameVar_PArray	taMisc::arg_name_descs;
NameVar_PArray	taMisc::args;
String_PArray	taMisc::args_tmp;

////////////////////////////////////////////////////////
// 	DMEM: Distributed Memory

int	taMisc::dmem_proc = 0;
int	taMisc::dmem_nprocs = 1;
bool	taMisc::dmem_debug = false;

////////////////////////////////////////////////////////
// 	Global State, Flags Etc

// give the main typespace a big hash table..
TypeSpace taMisc::types("taMisc::types", 10000);
TypeDef*	taMisc::default_scope = NULL;

taPtrList_impl*	taMisc::init_hook_list = NULL;

bool	taMisc::in_init = false;
bool	taMisc::in_event_loop = false;
signed char	taMisc::quitting = QF_RUNNING;
bool	taMisc::not_constr = true;
bool	taMisc::use_gui = false; // set to default in Init_Gui
bool	taMisc::use_plugins; // set to default in Init_Gui
bool 	taMisc::gui_active = false;
bool 	taMisc::gui_no_win = false;
bool 	taMisc::server_active = false; // true while connected
ContextFlag	taMisc::is_loading;
taVersion 	taMisc::loading_version; 
ContextFlag	taMisc::is_post_loading;
ContextFlag	taMisc::is_saving;
ContextFlag	taMisc::is_undo_loading;
ContextFlag	taMisc::is_undo_saving;
ContextFlag	taMisc::is_duplicating;
ContextFlag	taMisc::is_checking;
ContextFlag	taMisc::in_gui_call;
ContextFlag	taMisc::in_plugin_init;
ContextFlag	taMisc::no_auto_expand;
TypeDef*	taMisc::plugin_loading;

String	taMisc::last_err_msg;
String	taMisc::last_warn_msg;

String	taMisc::last_check_msg;
bool taMisc::check_quiet;
bool taMisc::check_confirm_success;
bool taMisc::check_ok;

#ifndef NO_TA_BASE
String_PArray* taMisc::deferred_schema_names; 
UserDataItem_List* taMisc::deferred_schema_items; 
#endif

#ifdef TA_GUI
QPointer<QMainWindow> taMisc::console_win = NULL;
#endif
taMisc::SimageAvail taMisc::simage_avail; 

void (*taMisc::WaitProc)() = NULL;
void (*taMisc::ScriptRecordingGui_Hook)(bool) = NULL; // gui callback when script starts/stops; var is 'start'

String 	taMisc::LexBuf;
int taMisc::err_cnt;

/////////////////////////////////////////////////////////////////
// 		taMisc funs

/////////////////////////////////////////////////
//	Configuration -- object as settings

void taMisc::SaveConfig() {
#ifndef NO_TA_BASE
  UpdateAfterEdit();
  ++taFiler::no_save_last_fname;
  String cfgfn = prefs_dir + PATH_SEP + "options";
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
  String cfgfn = prefs_dir + PATH_SEP + "options";
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

void taMisc::UpdateAfterEdit() {
#ifndef NO_TA_BASE
  wikis.Reset();
  if(wiki1_url.name.nonempty() && wiki1_url.url.nonempty()) {
    wikis.Add(NameVar(wiki1_url.name, wiki1_url.url));
  }
  if(wiki2_url.name.nonempty() && wiki2_url.url.nonempty()) {
    wikis.Add(NameVar(wiki2_url.name, wiki2_url.url));
  }
  if(wiki3_url.name.nonempty() && wiki3_url.url.nonempty()) {
    wikis.Add(NameVar(wiki3_url.name, wiki3_url.url));
  }
  if(wiki4_url.name.nonempty() && wiki4_url.url.nonempty()) {
    wikis.Add(NameVar(wiki4_url.name, wiki4_url.url));
  }
  if(wiki5_url.name.nonempty() && wiki5_url.url.nonempty()) {
    wikis.Add(NameVar(wiki5_url.name, wiki5_url.url));
  }
  if(wiki6_url.name.nonempty() && wiki6_url.url.nonempty()) {
    wikis.Add(NameVar(wiki6_url.name, wiki6_url.url));
  }
#endif
}

/////////////////////////////////////////////////
//	Errors, Warnings, Simple Dialogs

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
  if(cssMisc::cur_top) {
    taMisc::last_warn_msg += String("\n") + cssMisc::GetSourceLoc(NULL);
  }
#endif
  cerr << "***WARNING: " << taMisc::last_warn_msg << endl;
  FlushConsole();
}

void taMisc::Info(const char* a, const char* b, const char* c, const char* d,
  const char* e, const char* f, const char* g, const char* h, const char* i)
{
#if defined(DMEM_COMPILE)
//TODO: should provide a way to log these somehow
  if(taMisc::dmem_proc > 0) return;
#endif
  cout << SuperCat(a, b, c, d, e, f, g, h, i) << endl;
  FlushConsole();
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
  cerr << "***CHECK ERROR: " << msg << endl;
  FlushConsole();
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
      + "() (path: " + obj->GetPath_Long() + ")\n";

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
      + "() (path: " + obj->GetPath_Long() + ")\n";
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
  if (beep_on_error) cerr << '\a'; // BEL character
  taMisc::last_err_msg = SuperCat(a, b, c, d, e, f, g, h, i);
#if !defined(NO_TA_BASE) 
  if(cssMisc::cur_top) {
    taMisc::last_err_msg += String("\n") + cssMisc::GetSourceLoc(NULL);
  }
#endif
  cerr << "***ERROR: " << taMisc::last_err_msg  << endl;
  FlushConsole();
#if !defined(NO_TA_BASE) 
  if(cssMisc::cur_top) {
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
    cout << msg << "\n";
    FlushConsole();
  }
}

#endif // def TA_NO_GUI

void taMisc::EditFile(const String& filename) {
  String edtr = taMisc::edit_cmd; //don't run gsub on the original string!
  edtr.gsub("%s", filename);
  taPlatform::exec(edtr);
}

/////////////////////////////////////////////////
//	Global state management

void taMisc::FlushConsole() {
#ifndef NO_TA_BASE
  if(!cssMisc::TopShell) return;
  cssMisc::TopShell->FlushConsole();
#endif
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
void taMisc::MallocInfo(ostream& strm) {
  strm << "Sorry memory usage statistics not available for this machine\n";
}
#else
void taMisc::MallocInfo(ostream& strm) {
  static struct mallinfo prv_mi;
#if defined(SUN4) && !defined(__CLCC__)
  struct mallinfo mi = mallinfo(0);
#else
  struct mallinfo mi = mallinfo();
#endif
  strm << "Memory Allocation Information (and change since last call):\n" <<
"arena		total space in arena 		"
<<	mi.arena   << "\t(" << mi.arena - prv_mi.arena << ")\n" <<
"ordblks		number of ordinary blocks	"
<<	mi.ordblks << "\t(" << mi.ordblks - prv_mi.ordblks << ")\n" <<
"uordblks	space in ordinary blocks in use	"
<<	mi.uordblks << "\t(" << mi.uordblks - prv_mi.uordblks << ")\n" <<
"fordblks	space in free ordinary blocks	"
<<	mi.fordblks << "\t(" << mi.fordblks - prv_mi.fordblks << ")\n" <<
"smblks		number of small blocks		"
<<	mi.smblks  << "\t(" << mi.smblks - prv_mi.smblks << ")\n" <<
"usmblks		space in small blocks in use	"
<<	mi.usmblks << "\t(" << mi.usmblks - prv_mi.usmblks << ")\n" <<
"fsmblks		space in free small blocks	"
<<	mi.fsmblks << "\t(" << mi.fsmblks - prv_mi.fsmblks << ")\n" <<
"hblks		number of holding blocks	"
<<	mi.hblks   << "\t(" << mi.hblks - prv_mi.hblks << ")\n" <<
"hblkhd		space in holding block headers	"
<<	mi.hblkhd  << "\t(" << mi.hblkhd - prv_mi.hblkhd << ")\n" <<
"keepcost	space penalty if keep option	"
<<	mi.keepcost << "\t(" << mi.keepcost - prv_mi.keepcost << ")\n";
  prv_mi = mi;
}
#endif

void taMisc::ListAllTokens(ostream& strm) {
  types.ListAllTokens(strm);
}

taMisc::TypeInfoKind taMisc::TypeToTypeInfoKind(TypeDef* td) {
  if (!td) return TIK_UNKNOWN; 

  TypeInfoKind tik;
#ifndef NO_TA_BASE
  if (td->InheritsFrom(&TA_TypeDef)) tik = TIK_TYPE;
  else if (td->InheritsFrom(&TA_MemberDef)) tik = TIK_MEMBER;
  else if (td->InheritsFrom(&TA_PropertyDef)) tik = TIK_PROPERTY;
  else if (td->InheritsFrom(&TA_MethodDef)) tik = TIK_METHOD;
  else if (td->InheritsFrom(&TA_MemberSpace)) tik = TIK_MEMBERSPACE;
  else if (td->InheritsFrom(&TA_MethodSpace)) tik = TIK_METHODSPACE;
  else if (td->InheritsFrom(&TA_PropertySpace)) tik = TIK_PROPERTYSPACE;
  else if (td->InheritsFrom(&TA_TypeSpace)) tik = TIK_TYPESPACE;
  else if (td->InheritsFrom(&TA_EnumDef)) tik = TIK_ENUM;
  else if (td->InheritsFrom(&TA_EnumSpace)) tik = TIK_ENUMSPACE;
  else if (td->InheritsFrom(&TA_TokenSpace)) tik = TIK_TOKENSPACE;
  else 
#endif
    tik = TIK_UNKNOWN; // shouldn't happen
  return tik;
}

void taMisc::Register_Cleanup(SIGNAL_PROC_FUN_ARG(fun)) {
  // this should be the full set of terminal signals
//  signal(SIGHUP,  fun); // 1
//NOTE: SIGABRT is only ansi signal MS CRT really supports
  signal(SIGABRT, fun);	// 6
#ifndef TA_OS_WIN
  signal(SIGILL,  fun);	// 4
  signal(SIGSEGV, fun); // 11
  //  signal(SIGINT,  fun);	// 2 -- this is caught by css!!
  signal(SIGQUIT, fun);	// 3
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
  switch(err) {
  case SIGABRT:	cerr << "abort"; break;
#ifndef TA_OS_WIN
  case SIGHUP:	cerr << "hangup"; break;
  case SIGQUIT:	cerr << "quit";	break;
  case SIGILL:	cerr << "illegal instruction"; break;
  case SIGBUS:	cerr << "bus error"; break;
  case SIGSEGV:	cerr << "segmentation violation"; break;
# ifndef LINUX
  case SIGSYS:	cerr << "bad argument to system call"; break;
# endif
  case SIGPIPE:	cerr << "broken pipe"; break;
  case SIGALRM:	cerr << "alarm clock"; break;
  case SIGTERM:	cerr << "software termination signal"; break;
  case SIGFPE:	cerr << "floating point exception"; break;
  case SIGUSR1:	cerr << "user signal 1"; break;
  case SIGUSR2:	cerr << "user signal 2"; break;
#endif  //!TA_OS_WIN
  default:	cerr << "unknown"; break;
  }
}



/////////////////////////////////////////////////
//	Startup

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
  thread_defaults.cpus = taPlatform::cpuCount();
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

  String curdir = GetCurrentPath();
  taMisc::load_paths.AddUnique(curdir);

  // max_cpu
  int max_cpus = FindArgByName("MaxCpus").toInt(); // 0 if doesn't exist
  if ((max_cpus > 0) && (max_cpus <= taPlatform::cpuCount())) {
    thread_defaults.cpus = max_cpus;
    taMisc::Info("Set threads cpus:", String(max_cpus));
  }

  if(thread_defaults.cpus > taPlatform::cpuCount())
    thread_defaults.cpus = taPlatform::cpuCount();

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
      av = av.after(0);		// skip space
      args_raw.Add(frst);
    }
    args_raw.Add(av);
  }
  UpdateArgs();
}

void taMisc::Init_Types() {// called after all type info has been loaded into types
  // initialize all classes that have an initClass method (ex. Inventor subtypes)
  if(taMisc::use_gui) {
    for (int i = 0; i < types.size; ++i) {
      TypeDef* typ = types.FastEl(i);
      if ((typ->ptr > 0) || (typ->ref)) continue;
      // look for an initClass method
      MethodDef* md = typ->methods.FindName("initClass");
      if (!md)
	md = typ->methods.FindName("InitClass");
      if (!md) continue;
      if (!(md->is_static && md->addr && (md->arg_types.size == 0) )) continue;
      // call the init function
      md->addr();
      md->addr = NULL;		// reset so it isn't run again!
    }
  }
  // add any Schema that couldn't be added earlier
  AddDeferredUserDataSchema();
  // other stuff could happen here..
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

void taMisc::HelpMsg(ostream& strm) {
  strm << "TA/CSS Help Info, version: " << version << endl;
  strm << "Startup arguments: " << endl;
  for(int i=0;i<arg_names.size; i++) {
    NameVar nv = arg_names.FastEl(i);
    Variant dvar = arg_name_descs.GetVal(nv.value.toString());
    String desc = dvar.toString();
    strm << "  " << nv.name << desc << endl;
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
    else if(av.contains('=')) {	// name=value arg
      nv.name = av.before('=');
      nv.value = av.after('=');
      Variant vl = arg_names.GetVal(nv.name + "="); // register "flag=" to convert to names
      if(!vl.isNull())
	nv.name = vl.toString();
    }
    else {			// regular arg: enter name as argv[x]
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
  strm.close();	strm.clear();
  return true;
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
//	Commonly used utility functions on strings/arrays/values

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
//    String sval(val);
//    if(sval.contains('.')) {
//      String digs = sval.after('.');
//      sval = sval.through('.') + digs.at(0,MIN(precision,(int)digs.length()));
//      if(digs.contains("e"))
//        sval += digs.from('e');
//    }
//    if((int)sval.length() > width)
//      sval = sval.before(width);
//    return sval;
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
	rval += '_';		// use _ to replace all strange chars
    }
  }
  return rval;
}


/////////////////////////////////////////////////
//	File Paths etc

String taMisc::GetFileFmPath(const String& path) {
  if(path.contains('/')) return path.after('/',-1);
  return path;
}

String taMisc::GetDirFmPath(const String& path, int n_up) {
  if(!path.contains('/')) return _nilString;
  String dir = path.before('/',-1);
  for(int i=0;i<n_up;i++) {
    if(!dir.contains('/')) return _nilString;
    dir = dir.before('/',-1);
  }
  return dir;
}

String taMisc::GetHomePath() {
  return taPlatform::getHomePath();
}

bool taMisc::FileExists(const String& fname) {
  return taPlatform::fileExists(fname);
}

#ifndef NO_TA_BASE
String taMisc::GetCurrentPath() {
  return QDir::currentPath();
}

bool taMisc::SetCurrentPath(const String& path) {
  return QDir::setCurrent(path);
}
#endif

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
  for(i=st_no; i<10000; i++) {	// stop at 10,000
    fname = prefix + String(i) + suffix;
    int acc = access(fname, R_OK);
    if(acc != 0)
      break;			// its ok..
  }
  fstream strm;
  strm.open(fname, ios::out);	// this should hold the place for the file
  strm.close();	strm.clear();		// while it is being saved, etc..
  return i;
}

String taMisc::FileDiff(const String& fname_a, const String& fname_b,
			bool trimSpace, bool ignoreSpace, bool ignoreCase) {
  String str_a, str_b;
  taStringDiff diff;
  diff.DiffFiles(fname_a, fname_b, str_a, str_b, trimSpace, ignoreSpace, ignoreCase);
  return diff.GetDiffStr(str_a, str_b);
}

String taMisc::GetWikiURL(const String& wiki_name, bool add_index) {
  Variant rval = wikis.GetVal(wiki_name);
  if(rval.isNull()) return _nilString;
  String url = rval.toString();
  if(add_index)
    url += "/index.php/";
  return url;
}

bool taMisc::InternetConnected() {
#ifndef NO_TA_BASE
  QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
  for (int i = 0; i < ifaces.size(); ++i) {
    const QNetworkInterface& ifc = ifaces.at(i);
    if(!ifc.isValid()) continue;
    if(ifc.flags() & (QNetworkInterface::IsLoopBack | QNetworkInterface::IsPointToPoint))
      continue;
    if(ifc.flags() & (QNetworkInterface::IsRunning | QNetworkInterface::IsUp)) {

      QList<QNetworkAddressEntry> addrs = ifc.addressEntries();
      if(addrs.size() == 0) continue;

      String nm = ifc.name();
      if(nm.startsWith("vm")) continue; // avoid virtual machine guys!
//      bool any_valid = false;
      for (int j = 0; j < addrs.size(); ++j) {
	QHostAddress ip = addrs.at(j).ip();
	if(ip.isNull()) continue;
	String adr = ip.toString();
// 	cerr << nm << " has addr:" << adr << endl;
      }
//       cerr << nm << " is up and running" << endl;
      return true;
    }
  }
#endif
  return false;
}

/////////////////////////////////////////////////
//	Recording GUI actions to css script

void taMisc::StartRecording(ostream* strm){
  record_script = strm;
  if (ScriptRecordingGui_Hook)
    ScriptRecordingGui_Hook(true);
}

void taMisc::StopRecording(){
  record_script = NULL;
  if (ScriptRecordingGui_Hook)
    ScriptRecordingGui_Hook(false);
}

bool taMisc::RecordScript(const char* cmd) {
  if (record_script == NULL)
    return false;
  if (record_script->bad() || record_script->eof()) {
    taMisc::Warning("*** Error: recording script is bad or eof, no script command recorded!!",
		  cmd);
    return false;
  }
  *record_script << cmd;
  if(cmd[strlen(cmd)-1] != '\n') {
    taMisc::Warning("*** Warning: cmd must end in a newline, but doesn't -- should be fixed:",
		  cmd);
    *record_script << '\n';
  }
  record_script->flush();
  return true;
}

#ifndef NO_TA_BASE
// normal non quoted members
void taMisc::ScriptRecordAssignment(taBase* tab,MemberDef* md){
  if(taMisc::record_script != NULL)  {
    *taMisc::record_script << tab->GetPath() << "." << md->name << " = " <<
      md->type->GetValStr(md->GetOff(tab)) << ";" << endl;
  }
}
// Script Record Inline Assignment
void taMisc::SRIAssignment(taBase* tab,MemberDef* md){
  if(taMisc::record_script != NULL)  {
    *taMisc::record_script << tab->GetPath() << "." << md->name << " = \"" <<
      md->type->GetValStr(md->GetOff(tab)) << "\";\n";
    *taMisc::record_script << tab->GetPath() << "." << "UpdateAfterEdit();" << endl;
  }
}

// Script Record Enum Assignment
void taMisc::SREAssignment(taBase* tab,MemberDef* md){
  if(taMisc::record_script != NULL)  {
    *taMisc::record_script << tab->GetPath() << "." << md->name << " = " <<
      tab->GetTypeDef()->name << "::" <<
      md->type->GetValStr(md->GetOff(tab)) << ";" << endl;
  }
}
#endif

////////////////////////////////////////////////////////////////////////
// 	File Parsing Stuff for Dump routines

int taMisc::skip_white(istream& strm, bool peek) {
  int c;
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c=strm.peek()) == ' ') || (c == '\t') || (c == '\n') || (c == '\r')) {
      strm.get();
      cerr << (char)c;
    }
    if(!peek && (c != EOF)) cerr << (char)c;
    taMisc::FlushConsole();
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
      cerr << (char)c;
    }
    if(!peek && (c != EOF)) cerr << (char)c;
    taMisc::FlushConsole();
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
      cerr << (char)c;
      LexBuf += (char)c; strm.get();
    }
    if(c != EOF) cerr << (char)c;
    taMisc::FlushConsole();
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
      cerr << (char)c;
      LexBuf += (char)c; strm.get();
    }
    if(c != EOF) cerr << (char)c;
    taMisc::FlushConsole();
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
      cerr << (char)c;
      taMisc::LexBuf += (char)c; strm.get();
    }
    if(c != EOF) cerr << (char)c;
    taMisc::FlushConsole();
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
      cerr << (char)c;
      if(c != '\r') LexBuf += (char)c;
      strm.get();
    }
    if(c != EOF) cerr << (char)c;
    taMisc::FlushConsole();
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
      cerr << (char)c;
      LexBuf += (char)c; strm.get();
    }
    if(c != EOF) cerr << (char)c;
    taMisc::FlushConsole();
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
      cerr << (char)c;
      LexBuf += (char)c; strm.get();
    }
    if(c != EOF) cerr << (char)c;
    taMisc::FlushConsole();
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
      cerr << (char)c;
      LexBuf += (char)c; strm.get();
    }
    if(c != EOF) cerr << (char)c;
    taMisc::FlushConsole();
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
      cerr << (char)c;
      if(c == '\n') 	taMisc::FlushConsole();
      LexBuf += (char)c;
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
    if(c != EOF) cerr << (char)c;
    taMisc::FlushConsole();
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
      cerr << (char)c;
      if(c == '\n') 	taMisc::FlushConsole();
      LexBuf += (char)c;
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
    if(c != EOF) cerr << (char)c;
    taMisc::FlushConsole();
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
    while (((c = strm.peek()) != EOF) && (c != '\"') && (c != ';')) 
      cerr << strm.get(); // consume it
    taMisc::FlushConsole();
  } else {
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
        cerr << (char)c;
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
    taMisc::FlushConsole();
  } else {
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
    int lst_flush = 0;
    int cur_pos = 0;
    cerr << "<<err_skp ->>";
    while (((c = strm.peek()) != EOF) && !(((c == '}') || (c == ';')) && (depth <= 0))) {
      cerr << (char)c; cur_pos++;
      if((c == '\n') || ((cur_pos - lst_flush) > taMisc::display_width * 4)) { taMisc::FlushConsole(); lst_flush = cur_pos; }
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
    if(c != EOF) cerr << (char)c << "<<- err_skp>>";
    taMisc::FlushConsole();
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
    cerr << "<<err_skp ->>";
    while (((c = strm.peek()) != EOF) && !((c == '}') && (depth <= 0))) {
      cerr << (char)c;
      if(c == '\n') 	taMisc::FlushConsole();
      if(c == '{')      depth++;
      if(c == '}')      depth--;
      strm.get();
    }
    if(c != EOF) cerr << (char)c << "<<- err_skp>>";
    taMisc::FlushConsole();
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
//	HTML-style tags

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
  if(c == '\n') strm.get();	// absorb an immediate cr after tag, which is common
  return rval;
}
  
int taMisc::read_till_rangle(istream& strm, bool peek) {
  int c;
  LexBuf = "";
  int depth = 0;
  if(taMisc::verbose_load >= taMisc::SOURCE) {
    while (((c = strm.peek()) != EOF) && !((c == '>') && (depth <= 0))) {
      cerr << (char)c;
      if(c == '\n') 	taMisc::FlushConsole();
      LexBuf += (char)c;
      if(c == '<')      depth++;
      if(c == '>')      depth--;
      strm.get();
    }
    if(c != EOF) cerr << (char)c;
    taMisc::FlushConsole();
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
// 	File Parsing Stuff for Dump routines: Output

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

ostream& taMisc::fancy_list(ostream& strm, const String& itm, int no, int prln, int tabs) {
  strm << itm << " ";
  if((no+1) % prln == 0) {
    strm << "\n";
    return strm;
  }
  int len = itm.length() + 1;
  int i;
  for(i=tabs; i>=0; i--) {
    if(len < i * 8)
      strm << "\t";
  }
  return strm;
}

// no == 0 = indent
ostream& taMisc::fmt_sep(ostream& strm, const String& itm, int no, int indent, int tsp) {
  int i;
  int itabs = (indent * tsp) / 8;
  int ispcs = (indent * tsp) % 8;
  if(no == 0) {			// indent
    for(i=0; i<itabs; i++)
      strm << "\t";
    for(i=0; i<ispcs; i++)
      strm << " ";
  }

  strm << itm << " ";

  int len = itm.length() + 1 + ispcs;
  for(i=2; i>=0; i--) {		// sep_tabs = 2
    if(len < i * 8)
      strm << "\t";
  }
  for(i=0; i<ispcs; i++)
    strm << " ";
  return strm;
}


//////////////////////////
//   IDataLinkProxy 	//
//////////////////////////

TypeDef* IDataLinkProxy::GetDataTypeDef() const {
  taDataLink* link_ = (taDataLink*)link();
  return (link_) ? link_->GetDataTypeDef() : NULL;
}

//////////////////////////
//   IDataLinkClient 	//
//////////////////////////

IDataLinkClient::~IDataLinkClient() {
  if (m_link) {
    //NOTE: since this is destructor, the 'this' we pass is our *own* virtual v-table
    // version, therefore, RemoveDataClient may NOT use any of our virtual or pure-virtual methods
    m_link->RemoveDataClient(this); //nulls our ref
  }
}

bool IDataLinkClient::AddDataLink(taDataLink* dl) {
#ifdef DEBUG
  if (m_link && (m_link != dl)) {
    taMisc::Error("IDataLinkClient::AddDataLink: a link has already been set!");
  }
/*  if (m_link) {
    taMisc::Error("IDataLinkClient::AddDataLink: a link has already been set!");
  }*/
#endif
  bool r = (!m_link);
  m_link = dl;
  return r;
} 

bool IDataLinkClient::RemoveDataLink(taDataLink* dl) {
  bool r = (m_link);
  m_link = NULL;
  return r;
} 

//////////////////////////////////
//   IMultiDataLinkClient 	//
//////////////////////////////////

IMultiDataLinkClient::~IMultiDataLinkClient() {
  while (dls.size > 0) {
    taDataLink* dl = dls.Pop();
    dl->RemoveDataClient(this);
  }
}

bool IMultiDataLinkClient::AddDataLink(taDataLink* dl) {
  return dls.AddUnique(dl);
} 

bool IMultiDataLinkClient::RemoveDataLink(taDataLink* dl) {
  return dls.RemoveEl(dl);
}

//////////////////////////
//   taDataLink 	//
//////////////////////////

const KeyString taDataLink::key_name("name");

taDataLink::taDataLink(void* data_, taDataLink* &link_ref_)
{
  // save data items reference, and set us in it
  m_data = data_;
  m_link_ref = &link_ref_;
  link_ref_ = this;
  m_dbu_cnt = 0;
}

taDataLink::~taDataLink() {
  *m_link_ref = NULL; //note: m_link_ref is always valid, because the constructor passed it by reference
#ifdef DEBUG
  if (clients.size > 0) {
    taMisc::Error("taDataLink::~taDataLink: clients.size not zero!");
  }
#endif
}

bool taDataLink::AddDataClient(IDataLinkClient* dlc) {
  if (!clients.AddUnique(dlc)) return false; // already added
  return dlc->AddDataLink(this);
}

void taDataLink::DataDestroying() { //note: linklist will automatically remove us
  IDataLinkClient* dlc;
  while (clients.size > 0) {
    //NOTE: client could destroy, so we have to remove it now
    dlc = clients.Pop();
    dlc->RemoveDataLink(this);
    //NOTE: client can still refer to us, but must do so through the ref we pass it
    dlc->DataLinkDestroying(this);
  }
}

void taDataLink::DoNotify(int dcr, void* op1_, void* op2_) {
#if defined(DEBUG) && !defined(NO_TA_BASE)
 // check for dispatch in a thread, which is Very Bad(TM)!!!
  if (!taTaskThread::inMainThread()) {
    taMisc::Error_nogui("A non-main thread has caused a taDataLink::DoNotify -- not allowed, notify will not be sent");
    return;
  }
#endif
  for (int i = 0; i < clients.size; ++i) {
    IDataLinkClient* dlc = clients.FastEl(i);
    if ((dcr == DCR_REBUILD_VIEWS) && !dlc->isDataView()) continue;
    if (dlc->ignoreDataChanged())
      dlc->IgnoredDataChanged(this, dcr, op1_, op2_);
    else 
      dlc->DataDataChanged(this, dcr, op1_, op2_);
  }
}

// set this to emit debug messages for the following code..
// #define DATA_DATA_DEBUG 1

void taDataLink::DataDataChanged(int dcr, void* op1_, void* op2_) {
/*
  m_dbu_cnt = 0: idle state
  m_dbu_cnt < 0: in a DATA_UPDATE context
  m_dbu_cnt > 0: in a STRUCT_UPDATE context

  If we only ever issue DATA BEGIN/ENDs, we remain in DATA state;
  If we ever issue a STRUCT BEGIN, we get forced into STRUCT state,
    and any subsequent DATA commands get interpreted as STRUCT.
    
  We try to suppress unnecessary guys.
  If we start with STRUCT or DATA and do all the same (no different)
  then we only issue the first and last; but if DATA->STRUCT,
  then we also need to issue the STRUCT, however to maintain
  balance, we issue a semi-spurious DATA END, so we have equal
  numbers of + and -; NOTE: This situation is very unlikely to
  actually occur, since Struct and Data ops are typically mutually
  exclusive, and even then, the most likely is DATA ops nested inside
  a STRUCT update (not the other way around.)
  
    we need to send out all further STRUCT ops, and the final DATA one
*/
  bool send_iu = false; // set true if we should send a synthetic ITEM_UPDATED
  bool suppress = false; // set it if we should supress forwarding
  bool dummy_end = false;
  //we translate the NoDirty guy, since it is only for the sender's use
  if (dcr == DCR_ITEM_UPDATED_ND) {
    dcr = DCR_ITEM_UPDATED;
  }
  if (dcr == DCR_STRUCT_UPDATE_BEGIN) { // forces us to be in struct state
    // only forward the first one (ex some clients do a reset step)
    // OR the first one where DATA->STRUCT
    suppress = (m_dbu_cnt > 0); // send if first, or we were in DATA state 
    if (m_dbu_cnt < 0) { // switch state if in DATA state
      m_dbu_cnt = -m_dbu_cnt; 
      dummy_end = true;
    }
    ++m_dbu_cnt;
#ifdef DATA_DATA_DEBUG    
    cerr << (String)(int)this << " stru beg: " << m_dbu_cnt << endl;
    taMisc::FlushConsole();
#endif
  } 
  else if (dcr == DCR_DATA_UPDATE_BEGIN) { 
    suppress = (m_dbu_cnt != 0);
    if (m_dbu_cnt > 0) ++m_dbu_cnt; // stay in STRUCT state if STRUCT state
    else               --m_dbu_cnt;
#ifdef DATA_DATA_DEBUG    
    cerr << (String)(int)this << " data beg: " << m_dbu_cnt << endl;
    taMisc::FlushConsole();
#endif
  } 
  else if ((dcr == DCR_STRUCT_UPDATE_END) || (dcr == DCR_DATA_UPDATE_END)) {
#ifdef DATA_DATA_DEBUG    
    bool was_stru = false;	// debug only
    if(dcr == DCR_STRUCT_UPDATE_END)
      was_stru = true;
#endif
    if (m_dbu_cnt < 0) {
      ++m_dbu_cnt;
    } else if (m_dbu_cnt > 0) {
      --m_dbu_cnt;
      dcr = DCR_STRUCT_UPDATE_END; // force to be struct end, in case we notify
    } 
    // this situation might theoretically arise if some updating action 
    // mid-update causes a link to an item to get created, which will then
    // not have been tracking all the BEGINs -- but it should be safe
    // to just keep letting them happen, doing gui updates, which should be harmless
#ifdef DEBUG // just make sure these are harmless, and don't scare users...
    else {
      cerr << "WARNING: Datalink for object name: "
        << GetName() << " unexpectedly received a DATA or STRUCT END (cnt was 0)\n";
    }
#endif
#ifdef DATA_DATA_DEBUG    
    if(was_stru)
      cerr << (String)(int)this << " stru end: " << m_dbu_cnt << endl;
    else
      cerr << (String)(int)this << " data end: " << m_dbu_cnt << endl;
    taMisc::FlushConsole();
#endif
    // at the end, also send a IU
    if (m_dbu_cnt == 0) {
      if (dcr == DCR_DATA_UPDATE_END) { // just turn it into an IU
        //NOTE: clients who count (ex taDataView) must detect this implicit
        // DATA_UPDATE_END as occurring when:
        // State=DATA, Count=1
        dcr = DCR_ITEM_UPDATED;
#ifdef DATA_DATA_DEBUG    
	cerr << (String)(int)this << " cvt to iu: " << m_dbu_cnt << endl;
	taMisc::FlushConsole();
#endif
      }
      else {// otherwise, we send both
        send_iu = true;
      }
    } else suppress = true;
  }
  else if (dcr == DCR_ITEM_UPDATED) {
    // if we are already updating, then ignore IUs, since we'll send one eventually
    if (m_dbu_cnt != 0) suppress = true;
  }
  
  if (!suppress) {
#ifdef DATA_DATA_DEBUG    
    cerr << (String)(int)this << " sending: " << dcr << endl;
    taMisc::FlushConsole();
#endif
    DoNotify(dcr, op1_, op2_);
  }
  if (dummy_end)
    DoNotify(DCR_DATA_UPDATE_END, NULL, NULL);
  if (send_iu) 
    DoNotify(DCR_ITEM_UPDATED, NULL, NULL);
}

String taDataLink::GetDisplayName() const {
  MemberDef* md = GetDataMemberDef();
  if (md) return md->name;
  else    return GetName();
}

TypeDef* taDataLink::GetTypeDef() const {
#if !defined(TA_NO_GUI) && !defined(NO_TA_BASE)
  return &TA_taDataLink;
#else
  return NULL;
#endif
}

bool taDataLink::RemoveDataClient(IDataLinkClient* dlc) {
  //WARNING: dlc calls this in its destructor, therefore 'dlc' is IDataLinkClient virtual
  // version, therefore, RemoveDataClient may NOT use any IDataLinkClient virtual methods
  // that are intended to be overloaded
  // exception: IMultiDataLinkClient calls from its own destructor, so its virtuals are ok
  dlc->RemoveDataLink(this);
  return clients.RemoveEl(dlc);
}

void* taDataLinkItr::NextEl(taDataLink* dl, const TypeDef* typ) {
  void* rval = NULL;
  while (dl && (i < dl->clients.size)) {
    IDataLinkClient* dlc = dl->clients.FastEl(i);
    ++i;
    TypeDef* dlc_typ = dlc->GetTypeDef();
    if (dlc_typ) {
      if (dlc_typ->InheritsFrom(typ)) {
        rval = dlc->This();
        break;
      }
    }
  }
  return rval;
}


//////////////////////////////////
// 	     TypeSpace		//
//////////////////////////////////

TypeSpace::~TypeSpace() { 
  Reset();
  if (data_link) {
    data_link->DataDestroying(); // link NULLs our pointer
  }
}

String	TypeSpace::El_GetName_(void* it) const { return ((TypeDef*)it)->name; }
TALPtr 	TypeSpace::El_GetOwnerList_(void* it) const { return ((TypeDef*)it)->owner; }
void*	TypeSpace::El_SetOwner_(void* it_) { 
  if (!it_) return it_;
  TypeDef* it = (TypeDef*)it_;
  it->owner = this; 
  //if this type is being added to anything during a plugin init, then
  // it is a plugin class, and we stamp it as such
#ifndef NO_TA_BASE
  if (taMisc::in_plugin_init) {
    it->plugin = taMisc::plugin_loading;
  }
#endif
  return it_;
  
}
void	TypeSpace::El_SetIndex_(void* it, int i){ ((TypeDef*)it)->idx = i; }

void*	TypeSpace::El_Ref_(void* it)   	  { taRefN::Ref((TypeDef*)it); return it; }
void* 	TypeSpace::El_unRef_(void* it) 	  { taRefN::unRef((TypeDef*)it); return it; }
void	TypeSpace::El_Done_(void* it)	  { taRefN::Done((TypeDef*)it); }
void*	TypeSpace::El_MakeToken_(void* it)  { return (void*)((TypeDef*)it)->MakeToken(); }
void*	TypeSpace::El_Copy_(void* trg, void* src)
{ ((TypeDef*)trg)->Copy(*((TypeDef*)src)); return trg; }

TypeDef* TypeSpace::FindTypeR(const String& fqname) const {
  if (fqname.contains("::")) {
    TypeDef* td = FindName(fqname.before("::"));
    if (!td) return NULL;
    return td->sub_types.FindTypeR(fqname.after("::"));
  } else {
    return FindName(fqname);
  }
}


bool TypeSpace::ReplaceLinkAll(TypeDef* ol, TypeDef* nw) {
  bool rval = false;
  int i;
  for(i=0; i<size; i++) {
    if(FastEl(i) == ol) {
      rval = true;
      ReplaceLinkIdx(i, nw);
    }
  }
  return rval;
}

bool TypeSpace::ReplaceParents(const TypeSpace& ol, const TypeSpace& nw) {
  bool rval = false;
  int i;
  for(i=0; i<size; i++) {
    int j;
    for(j=0; j<ol.size; j++) {
      TypeDef* old_st = ol.FastEl(j);
      TypeDef* new_st = nw.FastEl(j); 	// assumes one-to-one correspondence

      if(FastEl(i)->ReplaceParent(old_st, new_st))
	rval = true;
    }
  }
  return rval;
}

void TypeSpace::ListAllTokens(ostream& strm) {
  int i;
  for(i=0; i<size; i++) {
    TypeDef* td = FastEl(i);
    if(!td->tokens.keep)
      continue;
    strm << td->name << ": \t" << td->tokens.size << " (sub: " << td->tokens.sub_tokens << ")\n";
  }
}

//////////////////////////////////
// 	     EnumSpace		//
//////////////////////////////////

String	EnumSpace::El_GetName_(void* it) const { return ((EnumDef*)it)->name; }
TALPtr 	EnumSpace::El_GetOwnerList_(void* it) const { return ((EnumDef*)it)->owner; }
void*	EnumSpace::El_SetOwner_(void* it) { return ((EnumDef*)it)->owner = this; }
void	EnumSpace::El_SetIndex_(void* it, int i){ ((EnumDef*)it)->idx = i; }

void*	EnumSpace::El_Ref_(void* it)   	  { taRefN::Ref((EnumDef*)it); return it; }
void* 	EnumSpace::El_unRef_(void* it) 	  { taRefN::unRef((EnumDef*)it); return it; }
void	EnumSpace::El_Done_(void* it)	  { taRefN::Done((EnumDef*)it); }
void*	EnumSpace::El_MakeToken_(void* it)  { return (void*)((EnumDef*)it)->MakeToken(); }
void*	EnumSpace::El_Copy_(void* trg, void* src)
{ ((EnumDef*)trg)->Copy(*((EnumDef*)src)); return trg; }

EnumSpace::~EnumSpace() { 
  Reset(); 
  if (data_link) {
    data_link->DataDestroying(); // link NULLs our pointer
  }
}

// default enum no is last + 1
void  EnumSpace::Add(EnumDef* it) {
  taPtrList<EnumDef>::Add(it);
  if((it->idx == 0) || (it->idx > size+1))
    it->enum_no = 0;
  else
    it->enum_no = FastEl(it->idx - 1)->enum_no + 1;
}

EnumDef*  EnumSpace::Add(const char* nm, const char* dsc, const char* op, int eno) {
  EnumDef* rval = new EnumDef(nm);
  Add(rval);
  rval->desc = dsc;
  taMisc::CharToStrArray(rval->opts, op);
  rval->enum_no = eno;
  return rval;
}

EnumDef* EnumSpace::FindNo(int eno) const {
  int i;
  for(i=0; i < size; i++) {
    if(FastEl(i)->enum_no == eno)
      return FastEl(i);
  }
  return NULL;
}


//////////////////////////////////
// 	     TokenSpace		//
//////////////////////////////////

String TokenSpace::tmp_el_name;

void TokenSpace::Initialize() {
  owner = NULL;
  keep = false;
  sub_tokens = 0;
  data_link = NULL;
}

TokenSpace::~TokenSpace() { 
//  Reset(); //note: TokenSpace never had a Reset, but maybe it should...
  if (data_link) {
    data_link->DataDestroying(); // link NULLs our pointer
  }
}

String TokenSpace::El_GetName_(void* it) const {
#ifndef NO_TA_BASE
  if((owner == NULL) || !(owner->InheritsFrom(TA_taBase))) {
#else
  if(owner == NULL) {
#endif
    tmp_el_name = String((ta_intptr_t)it);
    return tmp_el_name;
  }
#ifndef NO_TA_BASE
  TAPtr tmp = (TAPtr)it;
  return tmp->GetName();
#else
  return _nilString;
#endif
}

void TokenSpace::List(ostream &strm) const {
  if(owner == NULL) return;

  strm << "\nTokens of type: " << owner->name << " (" << size
       << "), sub-tokens: " << sub_tokens;
  if(keep)
    strm << "\n";
  else
    strm << " (not keeping tokens)\n";

  taPtrList<void>::List(strm);
}


//////////////////////////////////
// 	    MemberSpace		//
//////////////////////////////////

String	MemberDefBase_List::El_GetName_(void* it) const 
  { return ((MemberDefBase*)it)->name; }
TALPtr 	MemberDefBase_List::El_GetOwnerList_(void* it) const 
  { return ((MemberDefBase*)it)->owner; }
void*	MemberDefBase_List::El_SetOwner_(void* it)
  { return ((MemberDefBase*)it)->owner = this; }
void	MemberDefBase_List::El_SetIndex_(void* it, int i)
  { ((MemberDefBase*)it)->idx = i; }

void*	MemberDefBase_List::El_Ref_(void* it)
  { taRefN::Ref((MemberDefBase*)it); return it; }
void* 	MemberDefBase_List::El_unRef_(void* it)
  { taRefN::unRef((MemberDefBase*)it); return it; }
void	MemberDefBase_List::El_Done_(void* it)
  { taRefN::Done((MemberDefBase*)it); }


void*	MemberDefBase_List::El_MakeToken_(void* it) { 
  switch (((MemberDefBase*)it)->typeInfoKind()) {
  case taMisc::TIK_MEMBER: 
    return (void*)((MemberDef*)it)->MakeToken();
  case taMisc::TIK_PROPERTY:
    return (void*)((PropertyDef*)it)->MakeToken();
  default: return NULL; // shouldn't happen!
  }
  return NULL; // compiler food
}

// note: this use the "pseudo-virtual" type guy
void*	MemberDefBase_List::El_Copy_(void* trg, void* src)
  { ((MemberDefBase*)trg)->Copy(((MemberDefBase*)src)); 
  return trg; 
}

MemberDefBase_List::~MemberDefBase_List() { 
  Reset();
  if (data_link) {
    data_link->DataDestroying(); // link NULLs our pointer
  }
}

//////////////////////////////////
// MemberSpace: Find By Name	//
//////////////////////////////////

int MemberSpace::FindNameOrType(const char *nm) const {	// lookup by name
  int rval = FindNameIdx(nm);
  if(rval >= 0)
    return rval;

  // then type names
  return FindTypeName(nm);
}

int MemberSpace::FindTypeName(const char* nm) const {
  for(int i=0; i<size; i++) {
    if(FastEl(i)->type->InheritsFrom(nm))
      return i;
  }
  return -1;
}

MemberDef* MemberSpace::FindNameR(const char* nm) const {
  MemberDef* rval;
  if((rval = FindName(nm)))
    return rval;

  int i;
  for(i=0; i < size; i++) {
    if((FastEl(i)->type->ptr == 0) &&
       ((rval = FastEl(i)->type->members.FindNameR(nm)) != NULL))
      return rval;
  }
  return NULL;
}

MemberDef* MemberSpace::FindNameAddr(const char* nm, void* base, void*& ptr) const {
  MemberDef* rval;
  if((rval = FindName(nm))) {
    ptr = rval->GetOff(base);
    return rval;
  }
  ptr = NULL;
  return NULL;
}

//////////////////////////////////
// MemberSpace: Find By Type	//
//////////////////////////////////

MemberDef* MemberSpace::FindType(TypeDef* it) const {
  int i;
  for(i=0; i<size; i++) {
    if(FastEl(i)->type->InheritsFrom(it)) {
      return FastEl(i);
    }
  }
  return NULL;
}

MemberDef* MemberSpace::FindTypeR(TypeDef* it) const {
  MemberDef* rval;
  if((rval = FindType(it)))
    return rval;

  int i;
  for(i=0; i < size; i++) {
    if((FastEl(i)->type->ptr == 0) &&
       ((rval = FastEl(i)->type->members.FindTypeR(it)) != NULL))
      return rval;
  }
  return NULL;
}

MemberDef* MemberSpace::FindTypeAddr(TypeDef* it, void* base, void*& ptr) const {
  MemberDef* rval;
  if((rval = FindType(it))) {
    ptr = rval->GetOff(base);
    return rval;
  }
  ptr = NULL;
  return NULL;
}

//////////////////////////////////
// MemberSpace: other Find 	//
//////////////////////////////////

int MemberSpace::FindDerives(TypeDef* it) const {
  int i;
  for(i=0; i<size; i++) {
    if(FastEl(i)->type->DerivesFrom(it))
      return i;
  }
  return -1;
}

MemberDef* MemberSpace::FindTypeDerives(TypeDef* it) const {
  int idx = FindDerives(it);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

MemberDef* MemberSpace::FindAddr(void* base, void* mbr, int& idx) const {
  int i;
  for(i=0; i<size; i++) {
    if(mbr == FastEl(i)->GetOff(base)) {
      idx = i;
      return FastEl(i);
    }
  }
  idx = -1;
  return NULL;
}

int MemberSpace::FindPtr(void* base, void* mbr) const {
  int i;
  for(i=0; i<size; i++) {
    MemberDef* md = FastEl(i);
    // check conventional pointers of any type
    if((md->type->ptr == 1) && (mbr == *((void **)md->GetOff(base))))
      return i;
    // Variants: just check for equivalence to contained pointer,
    //  if doesn't contain a ptr, test will return null
    if (md->type->InheritsFrom(TA_Variant)) {
      Variant& var = *((Variant*)md->GetOff(base));
      if (mbr == var.toPtr())
        return i;
    }
      
  }
  return -1;
}

MemberDef* MemberSpace::FindAddrPtr(void* base, void* mbr, int& idx) const {
  idx = FindPtr(base, mbr);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}



//////////////////////////////////
//  PropertySpace		//
//////////////////////////////////

void PropertyDef::setType(TypeDef* typ) {
  if (!typ) {
    taMisc::Error("PropertyDef::", name, ": attempt to set NULL type!");
    return;
  }
  if (type) {
    if (type != typ)
      taMisc::Warning("PropertyDef::", name, ": changed type from ",
        type->name, " to ", typ->name);
  }
  type = typ;
}

#ifdef NO_TA_BASE  
PropertyDef* PropertySpace::AssertProperty_impl(const char* nm, bool& is_new,
    bool get_nset, MemberDef* mbr, MethodDef* mth)
{
  MemberDefBase* md = FindName(nm);
  PropertyDef* pd = dynamic_cast<PropertyDef*>(md);
  //note: following actually won't happen, because maketa doesn't add members
  if (md && !pd) {
    taMisc::Warning("PropertySpace::AssertProperty: attempt to find PropertyDef '",
    nm, "' but MemberDef already exists\n");
    return NULL;
  }
  TypeItem* ti = NULL;
  if (!pd) {
    pd = new PropertyDef(nm);
    Add(pd);
    is_new = true;
  } else 
    is_new = false;
  if (mbr) {
    ti = mbr;
    if (get_nset) {
      pd->setType(mbr->type);
      taRefN::SetRefDone(*((taRefN**)&pd->get_mbr), mbr);
    } else {
      pd->setType(mbr->type);
      taRefN::SetRefDone(*((taRefN**)&pd->set_mbr), mbr);
    }
  }
  if (mth) {
    ti = mth;
    if (get_nset) {
      TypeDef* td = mth->type;
      if (td)
        td = td->GetNonConstType();
      pd->setType(td);
      taRefN::SetRefDone(*((taRefN**)&pd->get_mth), mth);
    } else {
      TypeDef* td = mth->arg_types.SafeEl(0);
      if (td)
        td = td->GetNonConstNonRefType();
      pd->setType(td);
      taRefN::SetRefDone(*((taRefN**)&pd->set_mth), mth);
    }
  }
  // new getters control all the opts, desc, etc.
  if (is_new && get_nset) {
    pd->desc = ti->desc;
    pd->opts = ti->opts;
    pd->lists = ti->lists;
  }
  return pd;
}
#endif

//////////////////////////////////
// PropertySpace: Find By Name	//
//////////////////////////////////

int PropertySpace::FindNameOrType(const char *nm) const {	// lookup by name
  int rval = 0; //init just to keep msvc happy
  // first check names
  if(FindName(nm),rval)
    return rval;

  // then type names
  return FindTypeName(nm);
}

int PropertySpace::FindTypeName(const char* nm) const {
  for(int i=0; i<size; i++) {
    if(FastEl(i)->type->InheritsFrom(nm))
      return i;
  }
  return -1;
}

MemberDefBase* PropertySpace::FindNameR(const char* nm) const {
  MemberDefBase* rval;
  if((rval = FindName(nm)))
    return rval;

  int i;
  for(i=0; i < size; i++) {
    if((FastEl(i)->type->ptr == 0) &&
       ((rval = FastEl(i)->type->properties.FindNameR(nm)) != NULL))
      return rval;
  }
  return NULL;
}


//////////////////////////////////
// 	    MethodSpace		//
//////////////////////////////////

String	MethodSpace::El_GetName_(void* it) const { return ((MethodDef*)it)->name; }
TALPtr 	MethodSpace::El_GetOwnerList_(void* it) const { return ((MethodDef*)it)->owner; }
void*	MethodSpace::El_SetOwner_(void* it) { return ((MethodDef*)it)->owner = this; }
void	MethodSpace::El_SetIndex_(void* it, int i){ ((MethodDef*)it)->idx = i; }

void*	MethodSpace::El_Ref_(void* it)    { taRefN::Ref((MethodDef*)it); return it; }
void* 	MethodSpace::El_unRef_(void* it)  { taRefN::unRef((MethodDef*)it); return it; }
void	MethodSpace::El_Done_(void* it)	  { taRefN::Done((MethodDef*)it); }
void*	MethodSpace::El_MakeToken_(void* it)  { return (void*)((MethodDef*)it)->MakeToken(); }
void*	MethodSpace::El_Copy_(void* trg, void* src)
{ ((MethodDef*)trg)->Copy(*((MethodDef*)src)); return trg; }

MethodSpace::~MethodSpace() { 
  Reset();
  if (data_link) {
    data_link->DataDestroying(); // link NULLs our pointer
  }
}

bool MethodSpace::AddUniqNameNew(MethodDef *it) {
  bool result = false;
  {
/*NOTE:
  At present, typea is only able to manage a single method per name.
  We only retain the last one scanned (so we always replace previously
    encountered ones with new ones.)
  It is HIGHLY recommended to write source with unique names, and use
  variants on names or default parameters to achieve overloading.
*/
  MethodDef* rval = NULL;
  bool replace = false; // we set true if we should replace the one we find 
  int idx;
  // we first see if it is an override of a virtual base...
  if (!it->is_static) // of course statics can't be virtual
    rval = FindVirtualBase(it, idx); //note: only finds virtuals, not non-virtuals
  if (rval) {
    if (it == rval) {result = false; goto end;} // could be the same one..
    it->is_virtual = true; // may not have been set for implicit override
    it->is_override = true; // this is our job to set
    // the overload count will be same for an override
    it->fun_overld = rval->fun_overld;
    // because we are the same method as the one encounters, we inherit its opts
    it->lists.DupeUnique(rval->lists);
// automatically inherit regular options
    it->opts.DupeUnique(rval->opts);
// but not comments or inherited options (which would be redundant)
//    it->opts.DupeUnique(rval->inh_opts);
//    it->inh_opts.DupeUnique(rval->inh_opts);
    if ((it->desc.empty()) || (it->desc == " "))
      it->desc = rval->desc;	// get the comment if we don't actually have one now..
    replace = true; // always replace
  } else {
    // ok, we are not a virtual override, but check if we are an overload or lexical hide
    // since we aren't an override, we are a new entity, so will replace
    idx = FindNameIdx(it->name);
    if(idx >= 0) rval = FastEl(idx);
    if (rval) {
      replace = true; 
      if (it == rval) {result = false; goto end;}// could be the same one..
      it->fun_overld = rval->fun_overld;
      // if the args are identical, then we are lexically hiding the previous
      // one -- we will therefore replace it, but of course not set "override"
      if (it->CompareArgs(rval)) {
        it->is_lexhide = true; // note: this is not often done, and could be an error
      } else {
        it->fun_overld++; // normal overload
      }
/*NOTE: in v3.2, this would seem to have applied mostly for virtual overrides
      // never overloaded, not adding any new options
      if (!it->is_static && (it->fun_overld == 0) && (it->opts.size == 0) && (it->lists.size == 0))
      return false; */
    }
  }
  if (replace) {
    ReplaceIdx(idx, it);		// new one replaces old if overloaded or overridden
    result = false;
    goto end;
  }
  inherited::Add(it);
  result = true;			// yes, its unique
  } // block, for jump
end:
  return result;
}

MethodDef* MethodSpace::FindAddr(ta_void_fun funa, int& idx) const {
  int i;
  idx = 0;
  for(i=0; i<size; i++) {
    if(FastEl(i)->addr == funa) {
      idx = i;
      return FastEl(i);
    }
  }
  return NULL;
}


// lidx is "index" in space for items on same list
MethodDef* MethodSpace::FindOnListAddr(ta_void_fun funa, const String_PArray& lst, int& lidx) const {
  int i;
  lidx = 0;
  for(i=0; i<size; i++) {
    if(FastEl(i)->CheckList(lst)) {
      if(FastEl(i)->addr == funa)
	return FastEl(i);
      lidx++;
    }
  }
  return NULL;
}

MethodDef* MethodSpace::FindOnListIdx(int lidx, const String_PArray& lst) const {
  int i, chk = 0;
  for(i=0; i<size; i++) {
    if(FastEl(i)->CheckList(lst)) {
      if(chk == lidx)
	return FastEl(i);
      chk++;
    }
  }
  return NULL;
}

MethodDef* MethodSpace::FindVirtualBase(MethodDef* it, int& idx) {
  idx = -1;
  for (int i = 0; i < size; ++i) {
    MethodDef* rval = FastEl(i);
    if (!rval->is_virtual) continue;
    if (rval->name != it->name) continue;
    if(!rval->CompareArgs(it)) continue;
    idx = i;
    return rval;
  }
  return NULL;
}


//////////////////////////
//   TypeItem		//
//////////////////////////
const String TypeItem::opt_show("SHOW");
const String TypeItem::opt_no_show("NO_SHOW");
const String TypeItem::opt_hidden("HIDDEN");
const String TypeItem::opt_read_only("READ_ONLY");
const String TypeItem::opt_expert("EXPERT");
const String TypeItem::opt_edit_show("EDIT_SHOW");
const String TypeItem::opt_edit_no_show("EDIT_NO_SHOW");
const String TypeItem::opt_edit_hidden("EDIT_HIDDEN");
const String TypeItem::opt_edit_read_only("EDIT_READ_ONLY");
const String TypeItem::opt_edit_detail("EDIT_DETAIL");
const String TypeItem::opt_edit_expert("EDIT_EXPERT");
const String TypeItem::opt_APPLY_IMMED("APPLY_IMMED");
const String TypeItem::opt_NO_APPLY_IMMED("NO_APPLY_IMMED");
const String TypeItem::opt_inline("INLINE");
const String TypeItem::opt_edit_inline("EDIT_INLINE");
const String TypeItem::opt_EDIT_DIALOG("EDIT_DIALOG");

const String TypeItem::opt_bits("BITS");
const String TypeItem::opt_instance("INSTANCE");

TypeItem::TypeItem()
:inherited()
{
  init();
}

TypeItem::TypeItem(const TypeItem& cp) {
  init();
  Copy_(cp);
}

void TypeItem::init()
{
  idx = 0;
  data_link = NULL;
}

TypeItem::~TypeItem() {
  if (data_link != NULL) {
    data_link->DataDestroying(); // link NULLs our pointer
  }
}

void TypeItem::Copy(const TypeItem& cp) {
  Copy_(cp);
}

void TypeItem::Copy_(const TypeItem& cp) {
  name 		= cp.name;
  desc		= cp.desc;
  opts		= cp.opts;
  lists		= cp.lists;
}

String TypeItem::OptionAfter(const String& op) const {
  int opt;
  String tmp_label;
  if((opt = opts.FindStartsWith(op,-1)) >= 0) { // search bckwrds for overrides..
    tmp_label = opts.FastEl(opt).after(op);
  }
  return tmp_label;
}

bool TypeItem::HasOptionAfter(const String& prefix, const String& op) const {
  int idx = -1;
  do {
    if ((idx = opts.FindStartsWith(prefix, idx + 1)) >= 0) {
      if (opts.FastEl(idx).after(prefix) == op) return true;
    }
  } while (idx >= 0);
  return false;
}

bool TypeItem::NextOptionAfter(const String& pre, int& itr, String& res) const
{
  if (itr < 0) itr = 0; // sanity
  while (itr < opts.size) {
    String opt = opts.FastEl(itr);
    ++itr;
    if (opt.matches(pre)) {
      res = opt.after(pre);
      return true;
    }
  }
  return false;
}


String TypeItem::GetLabel() const {
  String rval =  OptionAfter("LABEL_");
  if((rval.empty()) && !HasOption("LABEL_")) {	// not intentionally blank
    rval = name;
    taMisc::SpaceLabel(rval);
  } else				// do translate spaces..
    rval.gsub('_', ' ');
  return rval;
}

String TypeItem::GetOptsHTML() const {
  if(opts.size == 0) return _nilString;
  STRING_BUF(rval, 32); // extends if needed
  rval.cat("Options: ");
  for(int i=0;i<opts.size;i++) {
    if(i > 0) rval.cat(", ");
    rval.cat(trim(opts[i]).xml_esc());
  }
  return rval;
}


//////////////////////////
//   EnumDef		//
//////////////////////////


EnumDef::EnumDef()
:inherited()
{
  Initialize();
}

EnumDef::EnumDef(const char* nm)
:inherited()
{
  Initialize();
  name = nm;
}

EnumDef::EnumDef(const char* nm, const char* dsc, int eno, const char* op, const char* lis)
:inherited()
{
  name = nm;
  desc = dsc;
  enum_no = eno;
  taMisc::CharToStrArray(opts, op);
  taMisc::CharToStrArray(lists,lis);
}

EnumDef::EnumDef(const EnumDef& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

void EnumDef::Initialize() {
  owner = NULL;
  enum_no = 0;
}

void EnumDef::Copy(const EnumDef& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void EnumDef::Copy_(const EnumDef& cp) {
  enum_no = cp.enum_no;
}

bool EnumDef::CheckList(const String_PArray& lst) const {
  int i;
  for(i=0; i<lists.size; i++) {
    if(lst.FindEl(lists.FastEl(i)) >= 0)
      return true;
  }
  return false;
}


//////////////////////////////////
//  MemberDefBase		//
//////////////////////////////////

void MemberDefBase::Initialize() {
  owner = NULL;
  type = NULL;
  is_static = false;
#ifdef TA_GUI
  im = NULL;
#endif
  show_any = 0; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  show_edit = 0;
  show_tree = 0;
}

MemberDefBase::MemberDefBase()
:inherited()
{
  Initialize();
}

MemberDefBase::MemberDefBase(const char* nm)
:inherited()
{
  Initialize();
  name = nm;
}

MemberDefBase::MemberDefBase(TypeDef* ty, const char* nm, const char* dsc,
  const char* op, const char* lis, bool is_stat)
:inherited()
{
  Initialize();
  type = ty; name = nm; desc = dsc;
  taMisc::CharToStrArray(opts,op);
  taMisc::CharToStrArray(lists,lis);
  is_static = is_stat;
}

MemberDefBase::MemberDefBase(const MemberDefBase& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

void MemberDefBase::Copy(const MemberDefBase& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void MemberDefBase::Copy(const MemberDefBase* cp) {
  if (typeInfoKind() == cp->typeInfoKind())
  switch (typeInfoKind()) {
  case taMisc::TIK_MEMBER:
    ((MemberDef*)this)->Copy(*(const MemberDef*)cp);
    return;
  case taMisc::TIK_PROPERTY:
    ((PropertyDef*)this)->Copy(*(const PropertyDef*)cp);
    return;
  default: break; // compiler food
  }
  Copy(*cp); // should never happen!
}

void MemberDefBase::Copy_(const MemberDefBase& cp) {
  type = cp.type;
  inh_opts = cp.inh_opts;
  is_static = cp.is_static;
// don't copy because delete is not ref counted
//  im = cp.im;
// always invalidate show bits, so they get redone in our new context
  show_any = 0; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  show_edit = 0;
  show_tree = 0;
}

MemberDefBase::~MemberDefBase() {
#ifndef NO_TA_BASE
# ifndef NO_TA_GUI
  taRefN::SafeUnRefDone(im); 
  im = NULL;
# endif
#endif
}

bool MemberDefBase::CheckList(const String_PArray& lst) const {
  int i;
  for(i=0; i<lists.size; i++) {
    if(lst.FindEl(lists.FastEl(i)) >= 0)
      return true;
  }
  return false;
}

bool MemberDefBase::ShowMember(
  int show_forbidden,
  TypeItem::ShowContext show_context,
  int show_allowed) const 
{
  //note: require exact match to special flag, so boolean operations don't match it
  if (show_forbidden == taMisc::USE_SHOW_GUI_DEF)
    show_forbidden = taMisc::show_gui;
  
  // check if cache has been done yet
  if (show_any == 0) ShowMember_CalcCache();
  byte show_eff = 0;
  // default viewability for edit is "any OR edit"
  // default viewability for tree is "tree", ie, any not good enough
  switch (show_context) {
  case SC_ANY: show_eff = show_any; break;
  case SC_EDIT: show_eff = show_edit; break;
  case SC_TREE: show_eff = show_tree; break;
  //note: should be no default, let compiler complain if any added
  }
  // our show_eff is the positives (what it is) so if there is nothing there, then
  // we clearly can't show
  // if there is something (a positive) then bit-AND with the
  // show, which is negatives (what not to show), and if anything remains, don't show!
//  show_eff &= (byte)taMisc::SHOW_CHECK_MASK;
//  return (show_eff) && !(show_eff & (byte)show);
  return (show_eff & (byte)show_allowed & ~(byte)show_forbidden);
}
  
void MemberDefBase::ShowMember_CalcCache() const {
#ifndef NO_TA_BASE
  // default children are never shown
  TypeDef* par_typ = GetOwnerType();
  if (par_typ && par_typ->DerivesFrom(&TA_taOBase) && 
   !par_typ->DerivesFrom(&TA_taList_impl)) 
  {
    String mbr = par_typ->OptionAfter("DEF_CHILD_");
    if (mbr.nonempty() && (mbr == name)) {
    show_tree = 0x80; // set the "done" flag
    show_any = 0x80; 
    show_edit = 0x80;
    return;
    } 
  }
#endif
  
  // note that "normal" is a special case, which depends both on context and
  // on whether other bits are set, so we calc those individually
  show_any = taMisc::IS_NORMAL; // the default for any
  ShowMember_CalcCache_impl(show_any, _nilString);
  
  show_edit = show_any; // start with the "any" settings
  ShowMember_CalcCache_impl(show_edit, "_EDIT");
  
#ifndef NO_TA_BASE
  show_tree = show_any;
  // for trees, we only browse lists/groups by default
  if (!type->DerivesFrom(&TA_taList_impl))
    show_tree &= ~(byte)taMisc::NO_NORMAL;
#endif
  ShowMember_CalcCache_impl(show_tree, "_TREE");
  //NOTE: lists/groups, we only show by default in lists/groups, embedded lists/groups
}

void MemberDefBase::ShowMember_CalcCache_impl(byte& show, const String& suff) const {
  show |= 0x80; // set the "done" flag
  
  //note: keep in mind that these show bits are the opposite of the show flags,
  // i.e show flags are all negative, whereas these are all positive (bit = is that type)
  
  //note: member flags should generally trump type flags, so you can SHOW a NO_SHOW type
  //note: NO_SHOW is special, since it negates, so we check for base NO_SHOW everywhere
  bool typ_show = type->HasOption("MEMB_SHOW" + suff);
  bool typ_no_show = type->HasOption("MEMB_NO_SHOW") || type->HasOption("MEMB_NO_SHOW" + suff);
  bool mbr_show = HasOption("SHOW" + suff);
  bool mbr_no_show = HasOption("NO_SHOW") || HasOption("NO_SHOW" + suff);
  
  // the following are all cumulative, not mutually exclusive
  if (HasOption("HIDDEN" + suff) || type->HasOption("MEMB_HIDDEN" + suff))
    show |= (byte)taMisc::IS_HIDDEN;
  // RO are HIDDEN unless explicitly marked SHOW
  // note: no type-level, makes no sense
  if ((HasOption("READ_ONLY") || HasOption("GUI_READ_ONLY")) && !HasOption("SHOW")) 
    show |= (byte)taMisc::IS_HIDDEN;
  if (HasOption("EXPERT" + suff) || type->HasOption("MEMB_EXPERT" + suff))
    show |= (byte)taMisc::IS_EXPERT;

  // if NO_SHOW and no SHOW or explicit other, then never shows
  if (mbr_no_show || (typ_no_show && (!mbr_show || (show & (byte)taMisc::NORM_MEMBS)))) {
    show &= (byte)(0x80 | ~taMisc::SHOW_CHECK_MASK);
    return; 
  }
  
  // if any of the special guys are set, we unset NORMAL (which may
  //   or may not have been already set by default)
  if (show & (byte)taMisc::NORM_MEMBS) // in "any" context, default is "normal"
    show &= ~(byte)taMisc::IS_NORMAL;
  else // no non-NORMAL set
    // SHOW is like an explicit NORMAL if nothing else applies
    if (mbr_show || typ_show)
      show |= (byte)taMisc::IS_NORMAL;
}



//////////////////////////////////
// 	     MemberDef		//
//////////////////////////////////

void MemberDef::GetMembDesc(MemberDef* md, String& dsc_str, String indent) {
  String desc = md->desc;
  String defval = md->OptionAfter("DEF_");
  if(!defval.empty())
    desc = String("[Default: ") + defval + "] " + desc;
  else
    desc = desc;
  if(!indent.empty())
    desc = indent + md->GetLabel() + String(": ") + desc;
  if (!dsc_str.empty())
    dsc_str += "<br>";
  dsc_str += desc;
  if(md->type->InheritsFormal(TA_class) &&
     (md->type->HasOption("INLINE") || md->type->HasOption("EDIT_INLINE"))) {
    indent += "  ";
    for (int i=0; i < md->type->members.size; ++i) {
      MemberDef* smd = md->type->members.FastEl(i);
      if (!smd->ShowMember(taMisc::show_gui, TypeItem::SC_EDIT) ||
        smd->HasOption("HIDDEN_INLINE"))
	continue;
      GetMembDesc(smd, dsc_str, indent);
    }
  } else if (md->type->InheritsFormal(TA_enum)) {
    for (int i = 0; i < md->type->enum_vals.size; ++i) {
      EnumDef* ed = md->type->enum_vals.FastEl(i);
      if (ed->desc.empty() || (ed->desc == " ") || (ed->desc == "  ")) continue;
      desc = indent + "  " + ed->GetLabel() + String(": ") + ed->desc;
      if (!dsc_str.empty())
        dsc_str += "<br>";
      dsc_str += desc;
    }
  }
}

void MemberDef::Initialize() {
  off = NULL;
  base_off = 0;
  addr = NULL;
  fun_ptr = false;
}

MemberDef::MemberDef()
:inherited()
{
  Initialize();
}

MemberDef::MemberDef(const char* nm)
:inherited(nm)
{
  Initialize();
}

MemberDef::MemberDef(TypeDef* ty, const char* nm, const char* dsc,
  const char* op, const char* lis, ta_memb_ptr mptr, bool is_stat,
  void* maddr, bool funp)
:inherited(ty, nm, dsc, op, lis, is_stat)
{
  Initialize();
  off = mptr; addr = maddr;  fun_ptr = funp;
}

MemberDef::MemberDef(const MemberDef& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

void MemberDef::Copy(const MemberDef& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void MemberDef::Copy_(const MemberDef& cp) {
  off = cp.off;
  base_off = cp.base_off;
  addr = cp.addr;
  fun_ptr = cp.fun_ptr;
}

MemberDef::~MemberDef() {
}

void* MemberDef::GetOff(const void* base) const {
  void* rval = addr;
  if (!is_static)
    rval = (void*)&((ta_memb_ptr_class*)((char*)base+base_off)->*off);
  return rval;
}

void* MemberDef::GetOff_static(const void* base, int base_off_, ta_memb_ptr off_) {
  return (void*)&((ta_memb_ptr_class*)((char*)base+base_off_)->*off_);
}

const String MemberDef::GetPathName() const {
  String rval; 
  TypeDef* owtp = GetOwnerType();
  if (owtp) 
    rval = owtp->GetPathName();
  rval += "::" + name;
  return rval;
} 

const Variant MemberDef::GetValVar(const void* base) const {
  return type->GetValVar(GetOff(base), this);
}

MemberDef::DefaultStatus MemberDef::GetDefaultStatus(const void* base) {
  String defval = OptionAfter("DEF_");
  if (defval.empty()) return NO_DEF;
  String cur_val = type->GetValStr(GetOff(base));
  // hack for , in real numbers in international settings
  // note: we would probably never have a variant member with float default
  if (type->DerivesFrom(TA_float) || type->DerivesFrom(TA_double)) {
    cur_val.gsub(",", "."); // does a makeUnique
  }
  if(defval.contains(';')) { // enumerated set of values specified
    String dv1 = defval.before(';');
    String dvr = defval.after(';');
    while(true) {
      if(cur_val == dv1) return EQU_DEF;
      if(dvr.contains(';')) {
	dv1 = dvr.before(';');
	dvr = dvr.after(';');
	continue;
      }
      return (cur_val == dvr) ? EQU_DEF : NOT_DEF;
    }
  }
  else if(defval.contains(':')) { // range of values specified -- must be numeric
    String dv1 = defval.before(':');
    String dvr = defval.after(':');
    double cv = (double)cur_val; // just use double because it should do the trick for anything
    double dvl = (double)dv1;
    double dvh = (double)dvr;
    return (cv >= dvl && cv <= dvh) ? EQU_DEF : NOT_DEF;
  }
  else {
    return (cur_val == defval) ? EQU_DEF : NOT_DEF;
  }
}


bool MemberDef::isReadOnly() const {
  return HasOption("READ_ONLY");
}

bool MemberDef::isGuiReadOnly() const {
  return (HasOption("READ_ONLY") || HasOption("GUI_READ_ONLY"));
}

void MemberDef::SetValVar(const Variant& val, void* base, void* par) {
  return type->SetValVar(val, GetOff(base), par, this);
}

bool MemberDef::ValIsDefault(const void* base, int for_show) const {
  String defval = OptionAfter("DEF_");
  void* mbr_base = GetOff(base);
  // if it has an explicit default, compare using the string
  // we do this regardless whether it is a simple value, or an object
  if (defval.nonempty()) {
    String act_val = type->GetValStr(mbr_base);
    return (act_val == defval);
  }
  // otherwise, delegate to the type -- objects recurse into us
  // if a default value was specified, compare and set the highlight accordingly
  return type->ValIsDefault(mbr_base, this, for_show);
}


String MemberDef::GetHTML(int detail_level) const {
  STRING_BUF(rval, (detail_level == 0 ? 100 : 300)); // extends if needed
  String own_typ;
  TypeDef* ot = GetOwnerType();
  if(ot) {
    own_typ.cat(ot->name).cat("::");
  }
  if(detail_level >= 1) {
    rval.cat("<h3 class=\"fn\"><a name=\"").cat(name).cat("\"></a>").cat(own_typ).cat(name).cat(" : ");
    rval.cat(type->GetHTMLLink());
    if(fun_ptr)   rval.cat("(*)"); // function pointer indicator
    if(is_static) rval.cat("&nbsp;&nbsp;<tt> [static]</tt>");
    rval.cat("</h3>\n");
    rval.cat("<p>").cat(trim(desc).xml_esc()).cat("</p>\n");
    if(detail_level >= 2) {
      rval.cat("<p> Size: ").cat(String(type->size)).cat("</p>\n");
      if(opts.size > 0) {
	rval.cat("<p>").cat(GetOptsHTML()).cat("</p>\n");
      }
    }
  }
  else {
    rval.cat("<b><a href=#").cat(name).cat(">").cat(name).cat("</a></b> : ").cat(type->Get_C_Name());
    if(fun_ptr)   rval.cat("(*)"); // function pointer indicator
    if(is_static) rval.cat("&nbsp;&nbsp;<tt> [static]</tt>");
  }
  return rval;
}

//////////////////////////////////
// 	    PropertyDef		//
//////////////////////////////////

void PropertyDef::Initialize() {
#ifdef NO_TA_BASE
  get_mbr = NULL;
  get_mth = NULL;
  set_mbr = NULL;
  set_mth = NULL;
#endif
  prop_get = NULL;
  prop_set = NULL;
}

PropertyDef::PropertyDef()
:inherited()
{
  Initialize();
}

PropertyDef::PropertyDef(const char* nm)
:inherited(nm)
{
  Initialize();
}

PropertyDef::PropertyDef(TypeDef* ty, const char* nm, const char* dsc,
  const char* op, const char* lis, ta_prop_get_fun get, ta_prop_set_fun set,
  bool is_stat)
:inherited(ty, nm, dsc, op, lis, is_stat)
{
  Initialize();
  prop_get = get; 
  prop_set = set;
}

PropertyDef::PropertyDef(const PropertyDef& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

PropertyDef::~PropertyDef() {
#ifdef NO_TA_BASE
  SetRefDone(*((taRefN**)&get_mbr), NULL);
  SetRefDone(*((taRefN**)&get_mth), NULL);
  SetRefDone(*((taRefN**)&set_mbr), NULL);
  SetRefDone(*((taRefN**)&set_mth), NULL);
#endif
}

void PropertyDef::Copy(const PropertyDef& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void PropertyDef::Copy_(const PropertyDef& cp) {
  prop_get = cp.prop_get; 
  prop_set =cp.prop_set;
}

/*nn?? const String PropertyDef::GetPathName() const {
  String rval; 
  TypeDef* owtp = GetOwnerType();
  if (owtp) 
    rval = owtp->GetPathName();
  rval += "::" + name;
  return rval;
} */

const Variant PropertyDef::GetValVar(const void* base) const {
  if (prop_get)
    return prop_get(base);
  else return _nilVariant;
}

bool PropertyDef::isReadOnly() const {
  return (prop_set == NULL);
}

bool PropertyDef::isGuiReadOnly() const {
  return isReadOnly();
}

void PropertyDef::SetValVar(const Variant& val, void* base, void* par) {
  if (prop_set)
    prop_set(base, val);
}

bool PropertyDef::ValIsDefault(const void* base, int for_show) const {
  String defval = OptionAfter("DEF_");
  Variant val = GetValVar(base);
  // if it has an explicit default, compare using the string
  // we do this regardless whether it is a simple value, or an object
  if (defval.nonempty()) {
    return (val.toString() == defval);
  }
  // otherwise, just accept if it is the simple type
  return val.isDefault();
}

String PropertyDef::GetHTML(int detail_level) const {
  // todo: accessor functions, etc
  // todo: fun_ptr!
  STRING_BUF(rval, (detail_level == 0 ? 100 : 300)); // extends if needed
  String own_typ;
  TypeDef* ot = GetOwnerType();
  if(ot) {
    own_typ.cat(ot->name).cat("::");
  }
  if(detail_level >= 1) {
    rval.cat("<h3 class=\"fn\"><a name=\"").cat(name).cat("\"></a>").cat(own_typ).cat(name).cat(" : ");
    rval.cat(type->GetHTMLLink()).cat("</h3>\n");
    rval.cat("<p>").cat(trim(desc).xml_esc()).cat("</p>\n");
    if(detail_level >= 2) {
      rval.cat("<p> Size: ").cat(String(type->size)).cat("</p>\n");
      if(opts.size > 0) {
	rval.cat("<p>").cat(GetOptsHTML()).cat("</p>\n");
      }
    }
  }
  else {
    rval.cat("<b><a href=#").cat(name).cat(">").cat(name).cat("</a></b> : ").cat(type->Get_C_Name());
  }
  return rval;
}

//////////////////////////////////
// 	     MethodDef		//
//////////////////////////////////

void MethodDef::Initialize() {
  owner = NULL;
  type = NULL;
  is_static = false;
  is_virtual = false;
  is_override = false;
  is_lexhide = false;
  addr = NULL;
#ifdef TA_GUI
  im = NULL;
#endif
  fun_overld = 0;
  fun_argc = 0;
  fun_argd = -1;
  stubp = NULL;

  arg_types.name = "arg_types";
  arg_types.owner = (TypeDef*)this; // this isn't quite right, is it..
  show_any = 0;
}

MethodDef::MethodDef()
:inherited()
{
  Initialize();
}

MethodDef::MethodDef(const char* nm)
:inherited()
{
  Initialize();
  name = nm;
}

MethodDef::MethodDef(TypeDef* ty, const char* nm, const char* dsc, const char* op, const char* lis,
		     int fover, int farc, int fard, bool is_stat, ta_void_fun funa,
		     css_fun_stub_ptr stb, bool is_virt)
:inherited()
{
  Initialize();
  type = ty; name = nm; desc = dsc;
  taMisc::CharToStrArray(opts,op);
  taMisc::CharToStrArray(lists,lis);
  fun_overld = fover; fun_argc = farc; fun_argd = fard;
  is_static = is_stat; addr = funa; stubp = stb;
  is_virtual = is_virt; // note: gets further processed, will get set if this is an override
  // without explicit virtual keyword
}

MethodDef::MethodDef(const MethodDef& cp)
:inherited(cp)
{
  Initialize();
  Copy(cp);
}

MethodDef::~MethodDef() {
#ifndef NO_TA_BASE
# ifndef NO_TA_GUI
  taRefN::SafeUnRefDone(im); 
  im = NULL;
# endif
#endif
}

void MethodDef::Copy(const MethodDef& cp) {
  inherited::Copy(cp);
  type = cp.type;
  is_static = cp.is_static;
  is_virtual = cp.is_virtual;
  is_override = cp.is_override;
  is_lexhide = cp.is_lexhide;
  addr = cp.addr;
  inh_opts = cp.inh_opts;
  // don't delete because delete is not ref counted
//  im = cp.im;
  fun_overld = cp.fun_overld;
  fun_argc = cp.fun_argc;
  fun_argd = cp.fun_argd;
  arg_types = cp.arg_types;
  arg_names = cp.arg_names;
  arg_defs = cp.arg_defs;
  arg_vals = cp.arg_vals;
  stubp = cp.stubp;
  show_any = 0; //rebuild
}

bool MethodDef::CheckList(const String_PArray& lst) const {
  int i;
  for(i=0; i<lists.size; i++) {
    if(lst.FindEl(lists.FastEl(i)) >= 0)
      return true;
  }
  return false;
}

bool MethodDef::CompareArgs(MethodDef* it) const {
  if(fun_argc != it->fun_argc)
    return false;
  int i;
  for(i=0; i<fun_argc; i++) {
    if(arg_types[i] != it->arg_types[i])
      return false;
  }
  return true;
}

void MethodDef::CallFun(void* base) const {
#if !defined(NO_TA_BASE) && defined(TA_GUI)
  taiMethodData* mth_rep = NULL;
  if (taMisc::gui_active && (im != NULL)) {
    //TODO: following may not work, because it doesn't have enough context to pass to the routine
    mth_rep = im->GetGenericMethodRep(base, NULL);
  }
  if(mth_rep != NULL) {
    ++taMisc::in_gui_call;
    mth_rep->CallFun();
    --taMisc::in_gui_call;
    delete mth_rep;
  }
  else {
    if((fun_argc == 0) || (fun_argd == 0)) {
      ++taMisc::in_gui_call;
      // cssEl* rval = 
      (*(stubp))(base, 0, (cssEl**)NULL);
      --taMisc::in_gui_call;
    }
    else {
      taMisc::Warning("*** CallFun Error: function:", name,
		    "not available, because args are required and no dialog requestor can be opened",
		    "(must be gui, and function must have #MENU or #BUTTON");
      return;
    }
  }
#endif
}

const String MethodDef::GetPathName() const {
  String rval; 
  TypeDef* owtp = GetOwnerType();
  if (owtp) 
    rval = owtp->GetPathName();
  rval += "::" + name;
  return rval;
} 

const String MethodDef::ParamsAsString() const {
  if (arg_types.size == 0) return _nilString;
  STRING_BUF(rval, arg_types.size * 20); 
  String arg_def;
  for (int i = 0; i < arg_types.size; ++i) {
    if (i > 0) rval += ", ";
    TypeDef* arg_typ = arg_types.FastEl(i);
    rval += arg_typ->Get_C_Name() + " ";
    rval += arg_names.FastEl(i);
    arg_def = arg_defs.FastEl(i); //note: same string used in original definition
    if (arg_def.length() > 0) {
      rval += " = " + arg_def;
    }
  } 
  return rval;
}

const String MethodDef::prototype() const {
  STRING_BUF(rval, 80); // expands if necessary
  rval.cat(type->name).cat(' ').cat(name).cat('(');
  for (int i = 0; i < arg_names.size; ++i) {
    if (i > 0) rval.cat(", ");
    rval.cat(arg_types[i]->Get_C_Name()).cat(' ');
    rval.cat(arg_names[i]);
    String def = arg_defs[i];
    if (def.nonempty())
      rval.cat(" = ").cat(def);
  }
  rval.cat(')');
  return rval;
}

bool MethodDef::ShowMethod(taMisc::ShowMembs show) const 
{
  if (show & taMisc::USE_SHOW_GUI_DEF)
    show = taMisc::show_gui;
  
  // check if cache has been done yet
  if (show_any == 0) ShowMethod_CalcCache();
  byte show_eff = show_any;
  
  // our show_eff is the positives (what it is) so if there is nothing there, then
  // we clearly can't show
  // if there is something (a positive) then bit-AND with the
  // show, which is negatives (what not to show), and if anything remains, don't show!
  show_eff &= (byte)taMisc::SHOW_CHECK_MASK;
  return (show_eff) && !(show_eff & (byte)show);
}
  
void MethodDef::ShowMethod_CalcCache() const {
  // note that "normal" is a special case, which depends both on context and
  // on whether other bits are set, so we calc those individually
  show_any = taMisc::IS_NORMAL; // the default for any
  ShowMethod_CalcCache_impl(show_any);
  
}

void MethodDef::ShowMethod_CalcCache_impl(byte& show) const {
  show |= 0x80; // set the "done" flag
  
  //note: keep in mind that these show bits are the opposite of the show flags,
  // i.e show flags are all negative, whereas these are all positive (bit = is that type)
  
  //note: member flags should generally trump type flags, so you can SHOW a NO_SHOW type
  //note: NO_SHOW is special, since it negates, so we check for base NO_SHOW everywhere
  //MethodDef note: SHOW is implied in any of: MENU BUTTON or MENU_CONTEXT
  bool typ_show = type->HasOption("METH_SHOW");
  bool typ_no_show = type->HasOption("METH_NO_SHOW") || type->HasOption("METH_NO_SHOW");
  bool mth_show = HasOption("SHOW") || HasOption("MENU") ||
    HasOption("BUTTON") || HasOption("MENU_CONTEXT") || HasOption("MENU_BUTTON");
  bool mth_no_show = HasOption("NO_SHOW") || HasOption("NO_SHOW");
  
  // ok, so no explicit SHOW or NO_SHOW, so we do the special checks
  // you can't "undo" type-level specials, but you can always mark SHOW on the mth

  // the following are all cumulative, not mutually exclusive
  if (HasOption("HIDDEN") || type->HasOption("METH_HIDDEN"))
    show |= (byte)taMisc::IS_HIDDEN;
  if ((HasOption("READ_ONLY") || HasOption("GUI_READ_ONLY")) && !mth_show) 
    show |= (byte)taMisc::IS_HIDDEN;
  if (HasOption("EXPERT") || type->HasOption("METH_EXPERT"))
    show |= (byte)taMisc::IS_EXPERT;

  String cat = OptionAfter("CAT_");
  if(cat.contains("Xpert"))	// special code for expert -- no need for redundant #EXPERT flag
    show |= (byte)taMisc::IS_EXPERT;

  // if NO_SHOW and no SHOW or explicit other, then never shows
  if (mth_no_show || (typ_no_show && (!mth_show || (show & (byte)taMisc::NORM_MEMBS)))) {
    show &= (byte)(0x80 | ~taMisc::SHOW_CHECK_MASK);
    return; 
  }
  
  // if any of the special guys are set, we unset NORMAL (which may
  //   or may not have been already set by default)
  if (show & (byte)taMisc::NORM_MEMBS) // in "any" context, default is "normal"
    show &= ~(byte)taMisc::IS_NORMAL;
  else // no non-NORMAL set
    // SHOW is like an explicit NORMAL if nothing else applies
    if (mth_show || typ_show)
      show |= (byte)taMisc::IS_NORMAL;
}

String MethodDef::GetHTML(int detail_level) const {
  STRING_BUF(rval, (detail_level == 0 ? 100 : 300)); // extends if needed
  String own_typ;
  TypeDef* ot = GetOwnerType();
  if(ot) {
    own_typ.cat(ot->name).cat("::");
  }
  if(detail_level >= 1) {
    rval.cat("<h3 class=\"fn\"><a name=\"").cat(name).cat("\"></a>");
    rval.cat(type->GetHTMLLink()).cat(" ").cat(own_typ).cat(name).cat(" ( ");
    for(int i = 0; i < arg_names.size; ++i) {
      if (i > 0) rval.cat(", ");
      rval.cat(arg_types[i]->GetHTMLLink()).cat(' ');
      rval.cat("<i>").cat(arg_names[i]).cat("</i>");
      String def = arg_defs[i];
      if (def.nonempty())
	rval.cat(" = ").cat(trim(def).xml_esc());
    }
    rval.cat(" )");
    if(is_static) rval.cat("&nbsp;&nbsp;<tt> [static]</tt>");
    rval.cat("</h3>\n");
    rval += "<p>" + trim(desc).xml_esc() + "</p>\n";
    if(detail_level >= 2) {
      if(opts.size > 0) {
	rval.cat("<p>").cat(GetOptsHTML()).cat("</p>\n");
      }
    }
  }
  else {
    rval.cat(type->Get_C_Name()).cat(" <b><a href=#").cat(name).cat(">").cat(own_typ).cat(name).cat("</a></b> ( ");
    for(int i = 0; i < arg_names.size; ++i) {
      if (i > 0) rval.cat(", ");
      rval.cat(arg_types[i]->Get_C_Name()).cat(' ');
      rval.cat("<i>").cat(arg_names[i]).cat("</i>");
      String def = arg_defs[i];
      if (def.nonempty())
	rval.cat(" = ").cat(trim(def).xml_esc());
    }
    rval.cat(" )");
    if(is_static) rval.cat("&nbsp;&nbsp;<tt> [static]</tt>");
  }
  return rval;
}

//////////////////////////
//    TypeDef		//
//////////////////////////

TypeDef* TypeDef::GetCommonSubtype(TypeDef* typ1, TypeDef* typ2) {
  // search up typ1's tree until a common subtype is found
  // note: doesn't matter which obj's tree we go up, so we just pick typ1
  TypeDef* rval = typ1;
  while (rval && (!typ2->InheritsFrom(rval))) {
    // note: we only search up the primary inheritance hierarchy
    rval = rval->parents.SafeEl(0);
  }
  return rval;
}

void TypeDef::Initialize() {
  owner = NULL;
  size = 0;
  ptr = 0;
  ref = false;
  internal = false;
  formal = false;

#ifdef TA_GUI
  it = NULL;
  ie = NULL;
  iv = NULL;
#endif
#ifdef NO_TA_BASE
  pre_parsed = false;	// true if previously parsed by maketa
#else
  is_subclass = false;
  plugin = NULL; // set true by TypeSpace::SetOwner if initing a plugin
  instance = NULL;
  defaults = NULL;
  schema = NULL;
#endif

  parents.name = "parents";
  parents.owner = this;
  par_formal.name = "par_formal";
  par_formal.owner = this;
  par_cache.name = "par_cache";
  par_cache.owner = this;
  children.name = "children";
  children.owner = this;
  tokens.name = "tokens";
  tokens.owner = this;

  enum_vals.name = "enum_vals";
  enum_vals.owner = this;
  sub_types.name = "sub_types";
  sub_types.owner = this;
  members.name = "members";
  members.owner = this;
  properties.name = "properties";
  properties.owner = this;
  methods.name = "methods";
  methods.owner = this;
  templ_pars.name = "templ_pars";
  templ_pars.owner = this;
  m_cacheInheritsNonAtomicClass = 0;
#if (!defined(NO_TA_BASE) && defined(DMEM_COMPILE))
  dmem_type = NULL;
#endif
}

TypeDef::TypeDef()
:inherited()
{
  Initialize();
}

TypeDef::TypeDef(const char* nm)
:inherited()
{
  Initialize();
  name = nm;
}

#ifdef NO_TA_BASE
TypeDef::TypeDef(const char* nm, const char* dsc, const char* inop, const char* op, const char* lis,
  uint siz, int ptrs, bool refnc, bool global_obj)
#else
TypeDef::TypeDef(const char* nm, const char* dsc, const char* inop, const char* op, const char* lis,
  uint siz, void** inst, bool toks, int ptrs, bool refnc, bool global_obj)
#endif
:inherited()
{
  Initialize();
#ifndef NO_TA_BASE
  instance = inst;
  tokens.keep = toks;
#endif  
  name = nm; desc = dsc;
  c_name = nm;
  taMisc::CharToStrArray(opts,op);
  taMisc::CharToStrArray(inh_opts,inop);
  taMisc::CharToStrArray(lists,lis);
#ifndef NO_TA_BASE
  CleanupCats(true);		// save the last one for initialization
#endif
  size = siz; 
  ptr = ptrs;
  ref = refnc;
  if(global_obj)
    taRefN::Ref(this);		// reference if static (non-new'ed) global object
}

TypeDef::TypeDef(const char* nm, bool intrnl, int ptrs, bool refnc, bool forml,
		 bool global_obj, uint siz, const char* c_nm
)
:inherited()
{
  Initialize();
  name = nm; internal = intrnl; ptr = ptrs; ref = refnc; formal = forml; 
  size = siz; // note: may get updated later
  if (c_nm) c_name = c_nm;
  else c_name = name;
  if(ptr > 0) size = sizeof(void*);
  if(global_obj)
    taRefN::Ref(this);		// reference if static (non-new'ed) global object
}

TypeDef::TypeDef(const TypeDef& cp)
:inherited(cp)
{
  Initialize();
  Copy_(cp);
}

void TypeDef::Copy(const TypeDef& cp) {
  inherited::Copy(cp);
  Copy_(cp);
}

void TypeDef::Copy_(const TypeDef& cp) {
#ifdef NO_TA_BASE
  pre_parsed	= cp.pre_parsed;
#else
  is_subclass	= cp.is_subclass;
  plugin = cp.plugin;
  instance = cp.instance ;
  //TODO: copy the schema
// don't copy the tokens..
#endif
  c_name	= cp.c_name;
  size		= cp.size    ;
  ptr		= cp.ptr     ;
  ref		= cp.ref     ;
  internal	= cp.internal;
  formal	= cp.formal  ;

  inh_opts	= cp.inh_opts ;

  parents	= cp.parents  ;
  par_formal 	= cp.par_formal;
  par_cache	= cp.par_cache;
  children	= cp.children ;	// not sure about this one..

// don't copy the it's
//   it	 	= cp.it       ;
//   ie	   	= cp.ie      ;
// or the defaults
//  defaults 	= cp.defaults ;

  enum_vals	= cp.enum_vals;
  //  sub_types 	= cp.sub_types;
  sub_types.Duplicate(cp.sub_types);// important: add to subtypes..
  members	= cp.members;
  properties	= cp.properties;
  methods	= cp.methods;
  templ_pars	= cp.templ_pars;

  sub_types.ReplaceParents(cp.sub_types, sub_types); // make our sub types consistent
  DuplicateMDFrom(&cp);		// duplicate members owned by source
  UpdateMDTypes(cp.sub_types, sub_types); // since sub-types are new, point to them
}

TypeDef::~TypeDef() {
#ifndef NO_TA_BASE 
# ifdef DMEM_COMPILE
  if (dmem_type) {
    delete (MPI_Datatype_PArray*)dmem_type;
    dmem_type = NULL;
  }
# endif
  if (defaults) {
    taBase::UnRef(defaults);
    defaults = NULL;
  }
  if (schema) {
    delete schema;
    schema = NULL;
  }
# ifndef NO_TA_GUI
  taRefN::SafeUnRefDone(it); it = NULL;
  if (ie) {delete ie; ie = NULL;}
  if (iv) {delete iv; iv = NULL;}
# endif // !NO_TA_GUI
#endif // !NO_TA_BASE
  if((owner == &taMisc::types) && !taMisc::not_constr) // destroying..
    taMisc::not_constr = true;
}

#ifndef NO_TA_BASE
void TypeDef::AddUserDataSchema(UserDataItemBase* item) {
  if (!schema) {
    schema = new UserDataItem_List;
  }
  schema->Add(item);
}
#endif

void TypeDef::CleanupCats(bool save_last) {
  if(save_last) {
    bool got_op = false;
    for(int i=opts.size-1; i>=0; i--) {
      String op = opts[i];
      if(!op.contains("CAT_")) continue;
      if(got_op) {
	opts.RemoveIdx(i);
      }	// remove all other previous ones
      else got_op = true;
    }
    got_op = false;
    for(int i=inh_opts.size-1; i>=0; i--) {
      String op = inh_opts[i];
      if(!op.contains("CAT_")) continue;
      if(got_op) {
	inh_opts.RemoveIdx(i);
      } // remove all other previous ones
      else got_op = true;
    }
  }
  else {			// save first
    bool got_op = false;
    for(int i=0; i< opts.size;i++) {
      String op = opts[i];
      if(!op.contains("CAT_")) continue;
      if(got_op) { opts.RemoveIdx(i); i--; }	// remove all other previous ones
      else got_op = true;
    }
    got_op = false;
    for(int i=0; i<inh_opts.size;i++) {
      String op = inh_opts[i];
      if(!op.contains("CAT_")) continue;
      if(got_op) {inh_opts.RemoveIdx(i); i--; } // remove all other previous ones
      else got_op = true;
    }
  }
}

void TypeDef::DuplicateMDFrom(const TypeDef* old) {
  int i;
  for(i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    if(md->owner == &(old->members))
      members.ReplaceIdx(i, md->Clone());
  }
  for(i=0; i<methods.size; i++) {
    MethodDef* md = methods.FastEl(i);
    if(md->owner == &(old->methods))
      methods.ReplaceIdx(i, md->Clone());
  }
}

bool TypeDef::InheritsNonAtomicClass() const {
  if (m_cacheInheritsNonAtomicClass == 0) {
    // set cache
    m_cacheInheritsNonAtomicClass = (InheritsFormal(TA_class) 
        && !InheritsFrom(TA_taString) 
        && !InheritsFrom(TA_Variant)
#ifndef NO_TA_BASE
        && !InheritsFrom(TA_taSmartPtr)
        && !InheritsFrom(TA_taSmartRef)
# if TA_USE_QT
        && !InheritsFrom(TA_QAtomicInt)
# endif
#endif
    ) ? 1 : -1;
  }
  return (m_cacheInheritsNonAtomicClass == 1);
}

TypeDef* TypeDef::GetStemBase() const {
  if(HasOption("STEM_BASE")) return const_cast<TypeDef*>(this);
  // first breadth
  for(int i=0; i < parents.size; i++) {
    TypeDef* par = parents.FastEl(i);
    if(par->HasOption("STEM_BASE"))
      return par;
  }
  // then depth recursion
  for(int i=0; i < parents.size; i++) {
    TypeDef* rval = parents.FastEl(i)->GetStemBase();
    if(rval) return rval;
  }
  return NULL;
}

void TypeDef::UpdateMDTypes(const TypeSpace& ol, const TypeSpace& nw) {
  int i;
  for(i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    if(md->owner != &members)	// only for members we own
      continue;

    int j;
    for(j=0; j<ol.size; j++) {
      TypeDef* old_st = ol.FastEl(j);
      TypeDef* new_st = nw.FastEl(j); 	// assumes one-to-one correspondence

      if(md->type == old_st)
	md->type = new_st;
    }
  }
  for(i=0; i<methods.size; i++) {
    MethodDef* md = methods.FastEl(i);
    if(md->owner != &methods)
      continue;

    int j;
    for(j=0; j<ol.size; j++) {
      TypeDef* old_st = ol.FastEl(j);
      TypeDef* new_st = nw.FastEl(j); 	// assumes one-to-one correspondence

      if(md->type == old_st)
	md->type = new_st;

      md->arg_types.ReplaceLinkAll(old_st, new_st);
    }
  }
}

TypeDef*  TypeDef::FindTypeWithMember(const char* nm, MemberDef** md){
  if((*md = members.FindName(nm)) != NULL) return this;
  TypeDef* td;
  int i;
  for(i=0;i<children.size;i++){
    if((td = children[i]->FindTypeWithMember(nm,md)) != NULL)
      return td;
  }
  return NULL;
}

bool TypeDef::CheckList(const String_PArray& lst) const {
  int i;
  for(i=0; i<lists.size; i++) {
    if(lst.FindEl(lists.FastEl(i)) >= 0)
      return true;
  }
  return false;
}


TypeDef* TypeDef::GetNonPtrType() const {
  if(ptr == 0)    return (TypeDef*)this;

  TypeDef* rval = (TypeDef*)this;
  while((rval = rval->GetParent()) != NULL) {
    if(rval->ptr == 0)
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetNonRefType() const {
  if(!ref)    return (TypeDef*)this;

  TypeDef* rval = (TypeDef*)this;
  while((rval = rval->GetParent()) != NULL) {
    if(!rval->ref)
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetTemplType() const {
  if(InheritsFormal(TA_template)) return (TypeDef*)this;

  TypeDef* rval = (TypeDef*)this;
  while((rval = rval->GetParent()) != NULL) {
    if(rval->InheritsFormal(TA_template))
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetTemplInstType() const {
  if(InheritsFormal(TA_templ_inst)) return (TypeDef*)this;

  TypeDef* rval = (TypeDef*)this;
  while((rval = rval->GetParent()) != NULL) {
    if(rval->InheritsFormal(TA_templ_inst))
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetNonConstType() const {
  if(!DerivesFrom(TA_const))    return (TypeDef*)this;

  TypeDef* rval = (TypeDef*)this;
  while((rval = rval->parents.Peek()) != NULL) { // use the last parent, not the 1st
    if(!rval->DerivesFrom(TA_const))
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetNonConstNonRefType() const {
  if(!(DerivesFrom(TA_const) || ref))  return (TypeDef*)this;

  TypeDef* rval = (TypeDef*)this;
  while((rval = rval->parents.Peek()) != NULL) { // use the last parent, not the 1st
    if (!(rval->DerivesFrom(TA_const) || rval->ref))
      return rval;
  }
  return NULL;
}

TypeDef* TypeDef::GetPluginType() const {
#ifdef NO_TA_BASE
  return NULL;
#else
  return plugin;
#endif
}

String TypeDef::GetPtrString() const {
  String rval; int i;
  for(i=0; i<ptr; i++) rval += "*";
  return rval;
}


String TypeDef::Get_C_Name() const {
  String rval;
  if(ref) {
    TypeDef* nrt;
    if((nrt = GetNonRefType()) == NULL) {
//       taMisc::Error("Null NonRefType in TypeDef::Get_C_Name()", name);
      return name;
    }
    rval = nrt->Get_C_Name() + "&";
    return rval;
  }

  if (ptr > 0) {
    TypeDef* npt;
    if((npt = GetNonPtrType()) == NULL) {
//       taMisc::Error("Null NonPtrType in TypeDef::Get_C_Name()", name);
      return name;
    }
    rval = npt->Get_C_Name() + GetPtrString();
    return rval;
  }

  // combo type
  if ((parents.size > 1) && !InheritsFormal(TA_class)) {
    int i;
    for(i=0; i<parents.size; i++) {
      TypeDef* pt = parents.FastEl(i);
      rval += pt->Get_C_Name();
      if(i < parents.size-1) rval += " ";
    }
    return rval;
  }

  // on some local list and not internal
  // (which were not actually delcared at this scope anyway)
  if (!(internal) && (owner != NULL) && (owner->owner != NULL)) {
    rval = owner->owner->Get_C_Name() + "::";
  }

  if (InheritsFormal(TA_templ_inst) && (templ_pars.size > 0)) {
    int i;
    TypeDef* tmpar = GetTemplParent();
    if (tmpar == NULL) {
      taMisc::Error("Null TemplParent in TypeDef::Get_C_Name()", name);
      return name;
    }
    rval += tmpar->name + "<"; // note: name is always its valid c_name
    for (i=0; i<templ_pars.size; i++) {
      rval += templ_pars.FastEl(i)->Get_C_Name();
      if(i < templ_pars.size-1)
	rval += ",";
    }
    rval += ">";
    return rval;
  }

  //note: normally, c_name should be valid, but may be cases, ex. templates, dynamic types, etc
  // where c_name was not set or updated, so most contexts the name is the same
  if (c_name.empty())
    rval += name;			// the default
  else 
    rval += c_name;			// the default
  return rval;
}

TypeDef* TypeDef::AddParent(TypeDef* it, int p_off) {
  if(parents.LinkUnique(it))
    par_off.Add(p_off);		// it was unique, add offset
  // only add to children if not internal (except when parent is)
  bool templ = InheritsFormal(TA_template); // cache
  if (templ || (!internal || it->internal))
    it->children.Link(this);
  // since templs don't call AddParClass, we have to determine
#ifndef NO_TA_BASE
  // if it is a subclass here...
  if (templ && it->InheritsFormal(TA_class))
    is_subclass = true;
#endif

  opts.DupeUnique(it->inh_opts);
  inh_opts.DupeUnique(it->inh_opts);	// and so on

  if(InheritsFrom(TA_taBase))
    opts.AddUnique(opt_instance);	// ta_bases always have an instance

#ifndef NO_TA_BASE
  CleanupCats(false);		// save first guy for add parent!
#endif

  // no need to get all this junk for internals
  if(internal && !templ) return it;

  // use the old one because the parent does not have precidence over existing
  enum_vals.BorrowUniqNameOld(it->enum_vals);
  sub_types.BorrowUniqNameOld(it->sub_types);
  members.BorrowUniqNameOld(it->members);
  properties.BorrowUniqNameOld(it->properties);
  methods.BorrowUniqNameOld(it->methods);
  return it;
}

void TypeDef::AddParents(TypeDef* p1, TypeDef* p2, TypeDef* p3, TypeDef* p4,
			 TypeDef* p5, TypeDef* p6) {
  if(p1 != NULL)    AddParent(p1);
  if(p2 != NULL)    AddParent(p2);
  if(p3 != NULL)    AddParent(p3);
  if(p4 != NULL)    AddParent(p4);
  if(p5 != NULL)    AddParent(p5);
  if(p6 != NULL)    AddParent(p6);
}

void TypeDef::AddClassPar(TypeDef* p1, int p1_off, TypeDef* p2, int p2_off,
			  TypeDef* p3, int p3_off, TypeDef* p4, int p4_off,
			  TypeDef* p5, int p5_off, TypeDef* p6, int p6_off)
{
#ifndef NO_TA_BASE
  is_subclass = true;
#endif
  bool mi = false;
  if(p1 != NULL)    AddParent(p1,p1_off);
  if(p2 != NULL)    { AddParent(p2,p2_off); mi = true; }
  if(p3 != NULL)    AddParent(p3,p3_off);
  if(p4 != NULL)    AddParent(p4,p4_off);
  if(p5 != NULL)    AddParent(p5,p5_off);
  if(p6 != NULL)    AddParent(p6,p6_off);

  if(mi)	    ComputeMembBaseOff();
}

void TypeDef::AddParFormal(TypeDef* p1, TypeDef* p2, TypeDef* p3, TypeDef* p4,
			   TypeDef* p5, TypeDef* p6) {
  if(p1 != NULL)    par_formal.LinkUnique(p1);
  if(p2 != NULL)    par_formal.LinkUnique(p2);
  if(p3 != NULL)    par_formal.LinkUnique(p3);
  if(p4 != NULL)    par_formal.LinkUnique(p4);
  if(p5 != NULL)    par_formal.LinkUnique(p5);
  if(p6 != NULL)    par_formal.LinkUnique(p6);
}

void TypeDef::CacheParents() {
  par_cache.Reset();		// justin case
  for(int i=0; i<parents.size; i++) {
    parents.FastEl(i)->CacheParents_impl(this);
  }
  par_cache.BuildHashTable(par_cache.size + 2, taHashTable::KT_PTR); // little extra, cache on pointer vals
}

void TypeDef::CacheParents_impl(TypeDef* src_typ) {
  src_typ->par_cache.LinkUnique(this);
  for(int i=0; i<parents.size; i++) {
    parents.FastEl(i)->CacheParents_impl(src_typ);
  }
}

void TypeDef::ComputeMembBaseOff() {
  int i;
  for(i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    TypeDef* mo = md->GetOwnerType();

    if((mo == this) || (mo == NULL))
      continue;

    int base_off = GetParOff(mo);
    if(base_off > 0) {		// only those that need it!
      MemberDef* nmd = md->Clone();
      nmd->base_off = base_off;
      members.ReplaceIdx(i, nmd);
    }
    else if(base_off < 0) {
      taMisc::Error("ComputeMembBaseOff(): parent type not found:",mo->name,
		     "in type of:", name);
    }
  }
}

bool TypeDef::FindChildName(const char* nm) const {
  if(children.FindName(nm))
    return true;
  int i;
  for(i=0; i < children.size; i++) {
    if(children.FastEl(i)->FindChildName(nm))
      return true;
  }
  return false;
}
bool TypeDef::FindChild(TypeDef* it) const {
  if(children.FindEl(it) >= 0)
    return true;
  int i;
  for(i=0; i < children.size; i++) {
    if(children.FastEl(i)->FindChild(it))
      return true;
  }
  return false;
}

void* TypeDef::GetParAddr(const char* it, void* base) const {
  if(name == it) return base;	// you are it!
  int anidx = parents.FindNameIdx(it);
  if(anidx >= 0)
    return (void*)((char*)base + par_off[anidx]);
  int i;
  for(i=0; i < parents.size; i++) {
    void* nw_base = (void*)((char*)base + par_off[i]);
    void* rval;
    if((rval=parents.FastEl(i)->GetParAddr(it, nw_base)) != NULL)
      return rval;
  }
  return NULL;
}

void* TypeDef::GetParAddr(TypeDef* it, void* base) const {
  if(it==this) return base;	// you are it!
  int anidx = parents.FindEl(it);
  if(anidx >= 0)
    return (void*)((char*)base + par_off[anidx]);
  int i;
  for(i=0; i < parents.size; i++) {
    void* nw_base = (void*)((char*)base + par_off[i]);
    void* rval;
    if((rval=parents.FastEl(i)->GetParAddr(it, nw_base)) != NULL)
      return rval;
  }
  return NULL;
}


int TypeDef::GetParOff(TypeDef* it, int boff) const {
  int use_boff=0;
  if(boff >= 0)
    use_boff = boff;
  if(it==this) return use_boff;	// you are it!
  int anidx;
  if((anidx = parents.FindEl(it)) >= 0)
    return use_boff + par_off[anidx];
  int i;
  for(i=0; i < parents.size; i++) {
    int rval;
    if((rval=parents.FastEl(i)->GetParOff(it, use_boff + par_off[i])) >= 0)
      return rval;
  }
  return -1;
}

const String TypeDef::GetPathName() const {
//TODO: this routine may not even be used!
  // are we owned?
  // are we an EnumDef?
  
  String rval; 
  TypeDef* owtp = GetOwnerType();
  if (owtp) { 
    rval = owtp->GetPathName() + "::"; 
  }
  rval += name; 
  return rval;
}

TypeDef* TypeDef::GetPtrType() const {
  TypeDef* rval = children.FindName(name + "_ptr");
  if (rval) {
    // make sure its ptr count is one more than ours!
    if (rval->ptr != (ptr + 1)) {
      rval = NULL;
    }
  }
  if (!rval) {
    // need to make one, we use same pattern as maketa 
    rval = new TypeDef(name + "_ptr", internal, ptr + 1, 0, 0, 0);
    taMisc::types.Add(rval);
    // unconstify us, this is an internal operation, still considered "const" access
    rval->AddParent((TypeDef*)this);
  }
  return rval;
}

String TypeDef::GetTemplName(const TypeSpace& inst_pars) const {
  String rval = name;
  int i;
  for(i=0; i<inst_pars.size; i++) {
    rval += String("_") + inst_pars.FastEl(i)->name + "_";
  }
  return rval;
}

TypeDef* TypeDef::GetTemplParent() const {
  int i;
  for(i=0; i<parents.size; i++) {
    if(parents.FastEl(i)->InheritsFormal(TA_template))
      return parents.FastEl(i);
  }
  return NULL;
}

bool TypeDef::IgnoreMeth(const String& nm) const {
  if(!InheritsFormal(TA_class))
    return false;
  if(ignore_meths.FindEl(nm) >= 0)
    return true;

  int i;
  for(i=0; i<parents.size; i++) {
    if(parents.FastEl(i)->IgnoreMeth(nm))
      return true;
  }
  return false;
}

bool TypeDef::is_anchor() const {
  return ((ptr == 0) && !ref && !DerivesFrom(TA_const)
    && (enum_vals.size == 0));
}

bool TypeDef::is_class() const {
  return InheritsFormal(TA_class);
}

bool TypeDef::is_enum() const {
  return (enum_vals.size > 0);
}

MemberDef* TypeDef::FindMemberPathStatic(TypeDef*& own_td, int& net_base_off,
					 ta_memb_ptr& net_mbr_off, 
					 const String& path, bool warn) {
  void* cur_base_off = NULL;
  net_mbr_off = 0;
  if(!own_td || path.empty()) {
    return NULL;		// no warning..
  }
  String pth = path;
  while(pth.contains('.')) {
    String bef = pth.before('.');
    pth = pth.after('.');
    MemberDef* md = own_td->members.FindName(bef);
    if(!md) {
      if(warn) {
	taMisc::Warning("FindMemberPathStatic: member:", bef, "not found in object type:",
			own_td->name);
      }
      return NULL;
    }
    else {
      own_td = md->type;
      cur_base_off = md->GetOff(cur_base_off);
      net_mbr_off = *((ta_memb_ptr*)&cur_base_off);
    }
  }
  MemberDef* md = own_td->members.FindName(pth);
  if(!md) {
    if(warn) {
      taMisc::Warning("FindMemberPathStatic: member:", pth, "not found in object type:",
		      own_td->name);
    }
    return NULL;
  }
  cur_base_off = md->GetOff(cur_base_off);
  net_mbr_off = *((ta_memb_ptr*)&cur_base_off);
  return md;
}

EnumDef* TypeDef::FindEnum(const String& nm) const {
  EnumDef* rval;
  if((rval = enum_vals.FindName(nm)) != NULL)
    return rval;

  int i;
  for(i=0; i < sub_types.size; i++) {
    TypeDef* td = sub_types.FastEl(i);
    if(td->InheritsFormal(TA_enum)) {
      if((rval = td->FindEnum(nm)) != NULL)
	return rval;
    }
  }

  // try to look for an aka now..
  for(i=0;i<enum_vals.size;i++) {
    EnumDef* ed = enum_vals.FastEl(i);
    String aka = ed->OptionAfter("AKA_");
    if(aka.empty()) continue;
    if(aka == nm) return ed;
  }

  return NULL;
}

int TypeDef::GetEnumVal(const String& nm, String& enum_tp_nm) const {
  EnumDef* rval = FindEnum(nm);
  if(rval != NULL) {
    if((rval->owner != NULL) && (rval->owner->owner != NULL))
      enum_tp_nm = rval->owner->owner->name;
    return rval->enum_no;
  }
  return -1;
}

String TypeDef::GetEnumString(const String& enum_tp_nm, int enum_val) const {
  EnumDef* rval;
  if(enum_vals.size > 0) {
    rval = enum_vals.FindNo(enum_val);
    if(rval != NULL) return rval->name;
  }
  int i;
  for(i=0; i < sub_types.size; i++) {
    TypeDef* td = sub_types.FastEl(i);
    if(td->InheritsFormal(TA_enum) && (enum_tp_nm.empty() || (td->name == enum_tp_nm))) {
      rval = td->enum_vals.FindNo(enum_val);
      if(rval != NULL) return rval->name;
    }
  }
  return "";
}

const String TypeDef::Get_C_EnumString(int enum_val) const {
  // note: the containing type for an enumtypedef is the owner
  TypeDef* par_td = GetOwnerType();
  if (!par_td) return _nilString;
  String par_typnm = par_td->GetPathName();
  STRING_BUF(rval, 80); // extends if needed
  
  bool made = false; // indicates we succeeded
  if (HasOption(opt_bits)) {
    // compose the result from bits
//TODO:
  } else { // no bits
    EnumDef* ed = enum_vals.FindNo(enum_val);
    if (ed) {
      rval.cat(par_typnm).cat("::").cat(ed->name);
      made = true;
    }
  }
  
  if (!made) {
    // ok, no joy, so winge out and just cast
    rval.cat("((").cat(par_typnm).cat(")").cat(String(enum_val)).cat(")");
  }
  return rval;
}

#ifndef NO_TA_BASE
int TypeDef::FindTokenR(void* addr, TypeDef*& aptr) const {
  int rval = tokens.FindEl(addr);
  if(rval >= 0) {
    aptr = (TypeDef*)this;
    return rval;
  }

  // depth-first...
  int i;
  for(i=0; i<children.size; i++) {
    if((rval = children.FastEl(i)->FindTokenR(addr, aptr)) >= 0)
      return rval;
  }
  aptr = NULL;
  return -1;
}

int TypeDef::FindTokenR(const char* nm, TypeDef*& aptr) const {
  int rval = tokens.FindNameIdx(nm);
  if(rval >= 0) {
    aptr = (TypeDef*)this;
    return rval;
  }

  // depth-first...
  int i;
  for(i=0; i<children.size; i++) {
    if((rval = children.FastEl(i)->FindTokenR(nm, aptr)) >= 0)
      return rval;
  }
  aptr = NULL;
  return -1;
}
#endif // ndef NO_TA_BASE

bool TypeDef::HasEnumDefs() const {
  for (int i = 0; i < sub_types.size; ++i) {
    TypeDef* td = sub_types.FastEl(i);
    if (td->enum_vals.size > 0) return true;
  }
  return false;
}

bool TypeDef::HasSubTypes() const {
  bool rval = false;
  for (int i = 0; i < sub_types.size; ++i) {
    TypeDef* td = sub_types.FastEl(i);
    if (td->enum_vals.size == 0) {
      rval = true;
      break;
    }
  }
  return rval;
}

/*obs void TypeDef::Register(void* it) {
  if(taMisc::in_init)		// don't register the instance tokens
    return;
  if((taMisc::keep_tokens != taMisc::ForceTokens) &&
     (!tokens.keep || (taMisc::keep_tokens == taMisc::NoTokens)))
    return;

  TypeDef* par = GetParent();	// un-register from parent..
  int pos;
  if(par && (par->tokens.keep ||
	     (taMisc::keep_tokens == taMisc::ForceTokens))
     && ((pos = par->tokens.FindEl(it)) >= 0))
  {
    par->tokens.RemoveIdx(pos);
    par->tokens.sub_tokens.ref();	// sub class got a new token..
  }
  if(par)			// only register if you have a parent...
    tokens.Link(it);
}*/

void TypeDef::RegisterFinal(void* it) {
  if(taMisc::in_init)		// don't register the instance tokens
    return;
  if((taMisc::keep_tokens != taMisc::ForceTokens) &&
     (!tokens.keep || (taMisc::keep_tokens == taMisc::NoTokens)))
    return;

  TypeDef* par = GetParent();
  if (!par) return; // we only register if have parent
#if defined(DEBUG) && !defined(NO_TA_BASE) // semi-TEMP, until 100% verified
if (par->tokens.FindEl(it) >= 0) {
cerr << "attempt to reregister token of type(addr): " << ((taBase*)it)->GetTypeDef()->name << "(" << it << ")\n";
return;
}
#endif
  tokens.Link(it);
  while (par) {
    par->tokens.sub_tokens.ref();	// sub class got a new token..
    par = par->GetParent();
  }
}

bool TypeDef::ReplaceParent(TypeDef* old_tp, TypeDef* new_tp) {
  int anidx;
  if((anidx = parents.FindEl(old_tp)) >= 0) {
    parents.ReplaceLinkIdx(anidx, new_tp);
    name.gsub(old_tp->name, new_tp->name);
    return true;
  }
  bool rval = false;
  int i;
  for(i=0; i<parents.size; i++) {
    if(parents.FastEl(i)->ReplaceParent(old_tp, new_tp)) {
      rval = true;
      name.gsub(old_tp->name, new_tp->name); // updt name at all levels
    }
  }
  return rval;
}

void TypeDef::SetTemplType(TypeDef* templ_par, const TypeSpace& inst_pars) {
  if(inst_pars.size != templ_pars.size) {
    String defn_no(templ_pars.size);
    String inst_no(inst_pars.size);
    taMisc::Error("Template",name,"defined with",defn_no,"parameters, instantiated with",
		   inst_no);
    cerr << "Defined with parameters: ";
    templ_pars.List(cerr);
    cerr << "\nInstantiated with parameters: ";
    inst_pars.List(cerr);
    return;
  }

  parents.Reset();			// bag the template's parents
  parents.LinkUnique(templ_par);	// parent is the templ_par
  par_formal.RemoveEl(&TA_template);
  par_formal.Link(&TA_templ_inst); 	// now a template instantiation
  templ_par->children.LinkUnique(this);
  internal = false;			// not internal any more
  children.Reset();			// don't have any real children..

  // todo: need to add support for arbitrary strings here, which are not just types

  int i;
  for(i=0; i<inst_pars.size; i++) {
    TypeDef* defn_tp = templ_par->templ_pars.FastEl(i); // type as defined
    TypeDef* inst_tp = inst_pars.FastEl(i);  // type as instantiated

    templ_pars.ReplaceLinkIdx(i, inst_tp); // actually replace it

    // update sub-types based on defn_tp (go backwards to get most extended types 1st)
    int j;
    for(j=sub_types.size-1; j>=0; j--) {
      sub_types.FastEl(j)->ReplaceParent(defn_tp, inst_tp);
    }
  }

  // update to use new types
  UpdateMDTypes(templ_par->templ_pars, templ_pars);
}

/*obs void TypeDef::unRegister(void* it) {
  if((taMisc::keep_tokens != taMisc::ForceTokens) &&
     (!tokens.keep || (taMisc::keep_tokens == taMisc::NoTokens)))
    return;

  if(!tokens.RemoveEl(it)) {	// if we couldn't find this one, must be a sub-tok..
    int subt = (int)(tokens.sub_tokens) - 1;
    tokens.sub_tokens = MAX(subt, 0); // might blow down..
  }
}*/

void TypeDef::unRegisterFinal(void* it) {
  if((taMisc::keep_tokens != taMisc::ForceTokens) &&
     (!tokens.keep || (taMisc::keep_tokens == taMisc::NoTokens)))
    return;

  if (tokens.RemoveEl(it)) {
    TypeDef* par = GetParent();
    while (par) {
      par->tokens.sub_tokens.deref();
      par = par->GetParent();
    }
  }
}


//////////////////////////////////
// 	Get/SetVal		//
//////////////////////////////////

String TypeDef::GetValStr_enum(const void* base, void* par, MemberDef* memb_def,
			       StrContext sc, bool force_inline) const 
{
  int enval = *((int*)base);

  if(HasOption("BITS")) {
    String bits;
    for(int i=0; i<enum_vals.size; i++) {
      EnumDef* ed = enum_vals[i];
      if(ed->HasOption("NO_BIT") || ed->HasOption("NO_SAVE")) continue;
      if(enval & ed->enum_no) {
	if(bits.empty()) bits = ed->name;
	else bits += "|" + ed->name;
      }
    }
    return bits;
  }

  EnumDef* ed = enum_vals.FindNo(enval);
  if(ed != NULL) return ed->name;
  else return String(*((int*)base));
}

String TypeDef::GetValStr_class_inline(const void* base_, void* par, MemberDef* memb_def,
				       StrContext sc, bool force_inline) const 
{
  void* base = (void*)base_; // hack to avoid having to go through entire code below and fix
  String rval;
  if (sc != SC_DISPLAY) rval = "{";
  for(int i=0; i<members.size; i++) {
    MemberDef* md = members.FastEl(i);
    // if streaming, do full save check, else just check for NO_SAVE
    if (sc == SC_STREAMING) {
      if (!md->DumpMember(base))
	continue;
    } else if (sc == SC_DISPLAY) {
      if (!md->ShowMember(taMisc::USE_SHOW_GUI_DEF, SC_EDIT))
	continue;
    } else {
      if(md->HasOption("NO_SAVE"))
	continue;
    }
    if(sc == SC_DISPLAY) {
      bool non_def = false;
      if(md->GetDefaultStatus(base) == MemberDef::NOT_DEF) {
	non_def = true;
	rval += "<font style=\"background-color: yellow\">&nbsp;";
      }
      rval += md->name + "&nbsp;";
      if(non_def) {
	rval += "</font>";
      }
      if(md->type->DerivesFormal(TA_enum) || md->type->ptr > 0
#ifndef NO_TA_BASE
	 || md->type->DerivesFrom(TA_taSmartPtr) || md->type->DerivesFrom(TA_taSmartRef)
#endif
	 )
	rval += "<font style=\"background-color: LightGrey\">&nbsp;&nbsp;";
      else
	rval += "<font style=\"background-color: white\">&nbsp;&nbsp;";
      if(md->type->InheritsFrom(TA_taString))	  rval += "\"";
      rval += md->type->GetValStr(md->GetOff(base), base, md, sc, force_inline);
      if(md->type->InheritsFrom(TA_taString))	  rval += "\"";
      rval += "&nbsp;&nbsp;</font>&nbsp;&nbsp;&nbsp;&nbsp;";
    }
    else {
      rval += md->name + "=";
      if(md->type->InheritsFrom(TA_taString))	  rval += "\"";
      rval += md->type->GetValStr(md->GetOff(base), base, md, sc, force_inline);
      if(md->type->InheritsFrom(TA_taString))	  rval += "\"";
      rval += ": ";
    }
  }
  if (sc != SC_DISPLAY) rval += "}";
  return rval;
}

String TypeDef::GetValStr(const void* base_, void* par, MemberDef* memb_def,
			  StrContext sc, bool force_inline) const 
{
  if(sc == SC_DEFAULT) 
    sc = (taMisc::is_saving) ? SC_STREAMING : SC_VALUE;
  void* base = (void*)base_; // hack to avoid having to go through entire code below and fix
  // if its void, odds are its a function..
  if (InheritsFrom(TA_void) || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun;
    if(memb_def != NULL)
      fun = TA_taRegFun.methods.FindOnListAddr(*((ta_void_fun*)base),
						 memb_def->lists, lidx);
    else
      fun = TA_taRegFun.methods.FindAddr(*((ta_void_fun*)base), lidx);
    if(fun != NULL)
      return fun->name;
    else if(*((void**)base) == NULL)
      return String::con_NULL;
    return String((ta_intptr_t)*((void**)base));
  }
  if (ptr == 0) {
    if (DerivesFrom(TA_bool)) {
      bool b = *((bool*)base);
      switch (sc) {
      case SC_STREAMING: return (b) ? String::con_1 : String::con_0;
      case SC_DISPLAY: return (b) ? String("+") : String("-");
      default:
        return String(b);
      }
    }
    // note: char is generic, and typically we won't use signed char
    else if (DerivesFrom(TA_char)) {
      switch (sc) {
      case SC_STREAMING: return String((int)*((char*)base));
      default:
        return String(*((char*)base));
      }
    }
    // note: explicit use of signed char is treated like a number
    else if ((DerivesFrom(TA_signed_char))) {
      return String((int)*((signed char*)base)); // treat as numbers
    }
    // note: explicit use of unsigned char is "byte" in ta/pdp
    else if ((DerivesFrom(TA_unsigned_char))) {
      return String((uint)*((unsigned char*)base)); // treat bytes as numbers
    }
    else if(DerivesFrom(TA_short)) {
      return String((int)*((short*)base));
    }
    else if(DerivesFrom(TA_unsigned_short)) {
      return String((uint)*((unsigned short*)base));
    }
    else if(DerivesFrom(TA_int)) {
#ifndef NO_TA_BASE
      if(sc == SC_DISPLAY && par && memb_def && memb_def->HasOption("DYNENUM_ON_enum_type")) {
	// shameless hack to get dyn enum to display text value
	DynEnum* dye = (DynEnum*)par;
	return dye->NameVal();
      }
      else
#endif
	return String(*((int*)base));
    }
    else if(DerivesFrom(TA_unsigned_int)) {
      return String(*((uint*)base));
    }
    else if(DerivesFrom(TA_int64_t)) {
      return String(*((int64_t*)base));
    }
    else if(DerivesFrom(TA_uint64_t)) {
      return String(*((uint64_t*)base));
    }
    // note: we convert , to . for intl contexts so files are always uniform
    else if(DerivesFrom(TA_float)) {
      switch (sc) {
      case SC_STREAMING: return String(*((float*)base), "%.7g").repl(",", ".");
      default:
        return String(*((float*)base));
      }
    }
    else if(DerivesFrom(TA_double)) {
      switch (sc) {
      case SC_STREAMING: return String(*((double*)base), "%.16lg").repl(",", ".");
      default:
        return String(*((double*)base));
      }
    }
    else if(DerivesFormal(TA_enum)) {
      return GetValStr_enum(base, par, memb_def, sc, force_inline);
    }
    else if(DerivesFrom(TA_taString))
      return *((String*)base);
    // in general, Variant is handled by recalling this routine on its rep's typdef
    else if (DerivesFrom(TA_Variant)) {
      TypeDef* typ;
      void* var_base;
      Variant& var = *((Variant*)base);
      //note: TA_void does not deal with this properly, so don't indirect...
      if (var.type() == Variant::T_Invalid)
        return _nilString;
        //NOTE: maybe we should indirect, rather than return NULL directly...
      if (var.isNull()) return String::con_NULL;
      var.GetRepInfo(typ, var_base);
      return typ->GetValStr(var_base, NULL, memb_def, sc, force_inline);
    }
#ifndef NO_TA_BASE
# ifdef TA_USE_QT
    else if(DerivesFrom(TA_QAtomicInt)) {
      return String((int)(*((QAtomicInt*)base)));
    }
# endif
    else if(DerivesFrom(TA_taBase)) {
      TAPtr rbase = (TAPtr)base;
      if(rbase)
	return rbase->GetValStr(par, memb_def, sc, force_inline);
    }
    else if(DerivesFrom(TA_taArray_impl)) {
      taArray_impl* gp = (taArray_impl*)base;
      if(gp != NULL) {
	String nm = " Size: ";
	nm += String(gp->size);
	nm += String(" (") + name + ")";
	return nm;
      }
      return name;
    }
    else if (DerivesFrom(TA_taSmartPtr)) {
      // we just delegate to taBase* since we are binary compatible
      return TA_taBase_ptr.GetValStr(base_, par, memb_def, sc, force_inline);
    }
    else if (DerivesFrom(TA_taSmartRef)) {
      taSmartRef& ref = *((taSmartRef*)base);
      TAPtr rbase = ref;
      if (rbase) {
        if ((rbase->GetOwner() != NULL) || (rbase == tabMisc::root)) {
          switch (sc) {
          case SC_STREAMING:
            return dumpMisc::path_tokens.GetPath(rbase);	// use path tokens when saving..
          case SC_DISPLAY:
            return rbase->GetName();
          default:
            return rbase->GetPath();
          }
        } else
          return String((intptr_t)rbase);
      } else  return String::con_NULL;
    }
#endif
    else if(DerivesFormal(TA_class) && 
	    (force_inline || HasOption("INLINE") || HasOption("INLINE_DUMP"))) 
    {
      return GetValStr_class_inline(base_, par, memb_def, sc, force_inline);
    }
    else if(DerivesFormal(TA_struct))
      return "struct " + name;
    else if(DerivesFormal(TA_union))
      return "union " + name;
    else if(DerivesFormal(TA_class))
      return "class " + name;
    else if(DerivesFrom(TA_void))
      return "void";
  }
  else if(ptr == 1) {
#ifndef NO_TA_BASE
    if(DerivesFrom(TA_taBase)) {
      return taBase::GetValStr_ptr(this, base_, par, memb_def, sc, force_inline);
    }
    else
#endif
    if (DerivesFrom(TA_TypeDef)) {
      TypeDef* td = *((TypeDef**)base);
      if (td) {
        return td->GetPathName();
      } else
	return String::con_NULL;
    }
    else if (DerivesFrom(TA_MemberDef)) {
      MemberDef* md = *((MemberDef**)base);
      if (md) {
        return md->GetPathName();
      } else
	return String::con_NULL;
    }
    else if (DerivesFrom(TA_MethodDef)) {
      MethodDef* md = *((MethodDef**)base);
      if (md) {
        return md->GetPathName();
      } else
	return String::con_NULL;
    }
  }
  return name;
}

void TypeDef::SetValStr_enum(const String& val, void* base, void* par, MemberDef* memb_def,
			     StrContext sc, bool force_inline) {
  String strval = val;
  if(strval.contains(')')) {
    strval = strval.after(')');
    if(strval.empty())	// oops
      strval = val;
  }
  strval.gsub(" ",""); strval.gsub("\t",""); strval.gsub("\n","");
  if(strval.contains("::")) {
    String tp_nm = strval.before("::");
    String en_nm = strval.after("::");
    TypeDef* td = taMisc::types.FindName(tp_nm);
    if(!td) {
      taMisc::Warning("Enum type name:", tp_nm, "not found in list of registered types -- cannot set value for string:", strval);
      return;
    }
    td->SetValStr_enum(en_nm, base, par, memb_def, sc, force_inline);
    return;
  }
  if(strval.contains('|')) { // bits
    int bits = 0;
    while(strval.nonempty()) {
      String curstr = strval;
      if(strval.contains('|')) {
	curstr = strval.before('|');
	strval = strval.after('|');
      }
      else
	strval = _nilString;
      EnumDef* ed = FindEnum(curstr);
      if(ed) {
	bits |= ed->enum_no;
      }
      else {
	taMisc::Warning("Enum named:", curstr, "not found in enum type:", name);
      }
    }
    *((int*)base) = bits;
    return;
  }
  else {
    EnumDef* ed = FindEnum(strval);
    if(ed) {
      *((int*)base) = ed->enum_no;
      return;
    }
    int intval = (int)strval;
    *((int*)base) = intval;
    return;
  }
}

void TypeDef::SetValStr_class_inline(const String& val, void* base, void* par, 
				     MemberDef* memb_def, StrContext sc, bool force_inline) {
  String rval = val;
  rval = rval.after('{');
  while(rval.contains(':')) {
    int st_pos = rval.index('=');
    String mb_nm = rval.before(st_pos);
    String next_val = rval.after(st_pos);
    int pos = 0;
    int next_val_len = next_val.length();
    int c = next_val[pos];
    if(c == '\"') {		// "
      st_pos++;
      next_val = next_val.after(0);
      next_val_len--;
      c = next_val[pos];
      while((c != '\"') && (pos < next_val_len)) c = next_val[++pos]; // "
    }
    else {
      int depth = 0;
      while(!(((c == ':') || (c == '}')) && (depth <= 0)) && (pos < next_val_len)) {
	if(c == '{')  depth++;
	if(c == '}')  depth--;
	c = next_val[++pos];
      }
    }
    String mb_val = next_val.before(pos);
    rval = rval.after(pos + st_pos + 1); // next position
    if(c == '\"')			     // "
      rval = rval.after(':');	// skip the semi-colon which was not groked
    mb_nm.gsub(" ", "");
    MemberDef* md = members.FindName(mb_nm);
    if(md == NULL) {		// try to find a name with an aka..
      int a;
      for(a=0;a<members.size;a++) {
	MemberDef* amd = members.FastEl(a);
	String aka = amd->OptionAfter("AKA_");
	if(aka.empty()) continue;
	if(aka == mb_nm) {
	  md = amd;
	  break;
	}
      }
    }
    if((md != NULL) && !mb_val.empty()) { // note: changed par to base here.. 
      md->type->SetValStr(mb_val, md->GetOff(base), base /* par */, md, sc, true);
      // force inline!
    }
  }
}

void TypeDef::SetValStr(const String& val, void* base, void* par, MemberDef* memb_def, 
			StrContext sc, bool force_inline) 
{
  if (sc == SC_DEFAULT) 
    sc = (taMisc::is_loading) ? SC_STREAMING : SC_VALUE;

  if(InheritsFrom(TA_void) || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    MethodDef* fun = TA_taRegFun.methods.FindName(val);
    if((fun != NULL) && (fun->addr != NULL))
      *((ta_void_fun*)base) = fun->addr;
    return;
  }
  if (ptr == 0) {
    if(DerivesFrom(TA_bool)) {
      *((bool*)base) = val.toBool();
    }
    else if(DerivesFrom(TA_int))
      *((int*)base) = val.toInt();
    else if(DerivesFrom(TA_float))
      *((float*)base) = val.toFloat();
    else if(DerivesFormal(TA_enum)) {
      SetValStr_enum(val, base, par, memb_def, sc, force_inline);
    }
    else if(DerivesFrom(TA_taString))
      *((String*)base) = val;
    // in general, Variant is handled by recalling this routine on its rep's typdef, then fixing null
    else if (DerivesFrom(TA_Variant)) {
      TypeDef* typ;
      void* var_base;
      Variant& var = *((Variant*)base);
      // if it doesn't have a type, then it will just become a string
      // (we can't let TA_void get processed...)
      if (var.type() == Variant::T_Invalid) {
        // don't do anything for empty string
        if (!val.empty())
          var = val;
        return;
      }
      var.GetRepInfo(typ, var_base);
      typ->SetValStr(val, var_base, par, memb_def, sc, force_inline);
      var.UpdateAfterLoad();
    }
    // note: char is treated as an ansi character
    else if (DerivesFrom(TA_char))
    //TODO: char conversion heuristics
      *((char*)base) = val.toChar();
    // signed char is treated like a number
    else if (DerivesFrom(TA_signed_char))
      *((signed char*)base) = (signed char)val.toShort();
    // unsigned char is "byte" in ta/pdp and treated like a number
    else if (DerivesFrom(TA_unsigned_char))
      *((unsigned char*)base) = (unsigned char)val.toUShort();
    else if(DerivesFrom(TA_short))
      *((short*)base) = val.toShort();
    else if(DerivesFrom(TA_unsigned_short))
      *((unsigned short*)base) = val.toUShort();
    else if(DerivesFrom(TA_unsigned_int))
      *((uint*)base) = val.toUInt();
    else if(DerivesFrom(TA_int64_t))
      *((int64_t*)base) = val.toInt64();
    else if(DerivesFrom(TA_uint64_t))
      *((uint64_t*)base) = val.toUInt64();
    else if(DerivesFrom(TA_double))
      *((double*)base) = val.toDouble();
#ifndef NO_TA_BASE
# ifdef TA_USE_QT
    else if(DerivesFrom(TA_QAtomicInt)) {
       (*((QAtomicInt*)base)) = val.toInt();
    }
# endif
    else if(DerivesFrom(TA_taBase)) {
      TAPtr rbase = (TAPtr)base;
      if(rbase)
	rbase->SetValStr(val, par, memb_def, sc, force_inline);
    }
    else if(DerivesFrom(TA_taArray_impl)) {
      taArray_impl* gp = (taArray_impl*)base;
      if(gp != NULL)
	gp->InitFromString(val);
    }
    else if (DerivesFrom(TA_taSmartPtr)) {
      // we just delegate, since we are binary compat
      TA_taBase_ptr.SetValStr(val, base, par, memb_def, sc, force_inline);
      return;
    }
    else if(DerivesFrom(TA_taSmartRef) && (tabMisc::root)) {
      TAPtr bs = NULL;
      if ((val != String::con_NULL) && (val != "Null")) {
        String tmp_val(val); // FindFromPath can change it
	if (sc == SC_STREAMING) {
	  bs = dumpMisc::path_tokens.FindFromPath(tmp_val, this, base, par, memb_def);
	  if (!bs)return;	// indicates deferred
	} else {
	  MemberDef* md = NULL;
	  bs = tabMisc::root->FindFromPath(tmp_val, md);
	  if(!bs) {
	    taMisc::Warning("*** Invalid Path in SetValStr:",val);
	    return;
	  }
	  if(md) {
	    if (md->type->ptr == 1) {
	      bs = *((TAPtr*)bs);
	      if(bs == NULL) {
		taMisc::Warning("*** Null object at end of path in SetValStr:",val);
		return;
	      }
	    } else if(md->type->ptr != 0) {
	      taMisc::Warning("*** ptr count greater than 1 in path:", val);
	      return;
	    }
	  }
	}
      }
      taSmartRef& ref = *((taSmartRef*)base);
      ref = bs;
    }
#endif
    else if(DerivesFormal(TA_class) &&
	    (force_inline || HasOption("INLINE") || HasOption("INLINE_DUMP"))) {
      SetValStr_class_inline(val, base, par, memb_def, sc, force_inline);
    }
  }
  else if(ptr == 1) {
    bool is_null = ((val == "NULL") || (val == "(NULL)"));
#ifndef NO_TA_BASE
    if (DerivesFrom(TA_taBase)) {
      if (tabMisc::root) {
        if (is_null) 
          taBase::DelPointer((taBase**)base);
        else
          taBase::SetValStr_ptr(val, this, base, par, memb_def, sc, force_inline);
      }
    }
    else
#endif
    if (is_null) {
      *((void**)base) = NULL;
    }
    else if(DerivesFrom(TA_TypeDef)) {
      TypeDef* td = taMisc::types.FindTypeR(val);
      if(td != NULL)
	*((TypeDef**)base) = td;
    }
    else if(DerivesFrom(TA_MemberDef)) {
      String fqtypnm = val.before("::", -1); // before final ::
      String mbnm = val.after("::", -1); // after final ::
      if(!fqtypnm.empty() && !mbnm.empty()) {
	TypeDef* td = taMisc::types.FindTypeR(fqtypnm);
	if(td != NULL) {
	  MemberDef* md = td->members.FindName(mbnm);
	  if(md != NULL)
	    *((MemberDef**)base) = md;
	}
      }
    }
    else if(DerivesFrom(TA_MethodDef)) {
      String fqtypnm = val.before("::", -1); // before final ::
      String mthnm = val.after("::", -1);
      if(!fqtypnm.empty() && !mthnm.empty()) {
	TypeDef* td = taMisc::types.FindTypeR(fqtypnm);
	if(td != NULL) {
	  MethodDef* md = td->methods.FindName(mthnm);
	  if(md != NULL)
	    *((MethodDef**)base) = md;
	}
      }
    }
  }
}

bool TypeDef::isVarCompat() const {
  // a few "blockers"
  if (ref || (ptr > 1) || InheritsFrom(TA_void)) return false;
#ifndef NO_TA_BASE
  if ((ptr ==1) && !InheritsFrom(TA_taBase)) return false;
#endif
  if (InheritsNonAtomicClass()) return false;
  // ok, hopefully the rest are ok!
  return true;
}

const Variant TypeDef::GetValVar(const void* base_, const MemberDef* memb_def) const 
{
  void* base = (void*)base_; // hack to avoid having to go through entire code below and fix
  // if its void, odds are its a function..
  if (InheritsFrom(TA_void) || ((memb_def) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun;
    if(memb_def != NULL)
      fun = TA_taRegFun.methods.FindOnListAddr(*((ta_void_fun*)base),
						 memb_def->lists, lidx);
    else
      fun = TA_taRegFun.methods.FindAddr(*((ta_void_fun*)base), lidx);
    if (fun != NULL)
      return fun->name;
    else if(*((void**)base) == NULL)
      return Variant((void*)NULL);//String::con_NULL;
    else 
      return String((ta_intptr_t)*((void**)base)); //TODO: is this the best??
  }
  if (ptr == 0) {
    if (DerivesFrom(TA_bool)) {
      bool b = *((bool*)base);
      return b; //T_Bool
    }
    // note: char is generic char, and typically we won't use signed char
    else if (DerivesFrom(TA_char)) {
      return *((char*)base); // T_Char
    }
    // note: explicit use of signed char is treated like a number
    else if ((DerivesFrom(TA_signed_char))) {
      return (int)*((signed char*)base); // T_Int
    }
    // note: explicit use of unsigned char is "byte" in ta/pdp
    else if ((DerivesFrom(TA_unsigned_char))) {
      return (uint)*((unsigned char*)base);  // T_UInt
    }
    else if(DerivesFrom(TA_short)) {
      return (int)*((short*)base);  // T_Int
    }
    else if(DerivesFrom(TA_unsigned_short)) {
      return (uint)*((unsigned short*)base);  // T_UInt
    }
    else if(DerivesFrom(TA_int)) {
      return *((int*)base);  // T_Int
    }
    else if(DerivesFrom(TA_unsigned_int)) {
      return *((uint*)base);  // T_UInt
    }
    else if(DerivesFrom(TA_int64_t)) {
      return *((int64_t*)base);  // T_Int64
    }
    else if(DerivesFrom(TA_uint64_t)) {
      return *((uint64_t*)base);  // T_UInt64
    }
    else if(DerivesFrom(TA_float)) {
      return *((float*)base); // T_Double
    }
    else if(DerivesFrom(TA_double)) {
      return *((double*)base); // T_Double
    }
    else if(DerivesFormal(TA_enum)) {
      return *((int*)base); // T_Int
    }
    else if(DerivesFrom(TA_taString))
      return *((String*)base); // T_String
    else if(DerivesFrom(TA_Variant)) {
      return *((Variant*)base);
    }
#ifndef NO_TA_BASE
    //WARNING: there could be ref-count issues if base has not been ref'ed at least once!
    else if(DerivesFrom(TA_taBase)) {
      TAPtr rbase = (TAPtr)base;
      //WARNING: there could be ref-count issues if base has not been ref'ed at least once!
      return rbase; // T_Base
    }
    else if (DerivesFrom(TA_taSmartPtr)) {
      TAPtr rbase = (TAPtr)base;
      return rbase; // T_Base
    }
    else if (DerivesFrom(TA_taSmartRef)) {
      TAPtr rbase = *((taSmartRef*)base);
      return rbase; // T_Base
    }
#endif
    // NOTE: other value types are not really supported, just fall through to return invalid
  }
  else if (ptr == 1) {
    if (DerivesFrom(TA_char)) {
      return *((char**)base); // T_String
    }
#ifndef NO_TA_BASE
    else if (DerivesFrom(TA_taBase)) {
      //NOTE: strictly speaking, we should be returning a generic ptr which points to the
      // base value, but in practice, this is never what we want, since members that
      // are TAPtr's are ubiquitous, what we actually want is a reference to the thing
      // being pointed to, ie., the content of the variable
      TAPtr rbase = *((TAPtr*)base);
      return rbase; // T_Base
    }
#endif
  }
  // other types and degress of indirection not really supported
  return _nilVariant; 
}

bool TypeDef::ValIsDefault(const void* base, const MemberDef* memb_def, 
    int for_show) const 
{
  // some cases are simple, for non-class values
  if ((InheritsFrom(TA_void) || ((memb_def) && (memb_def->fun_ptr != 0))) ||
    (ptr > 0) || 
    !InheritsNonAtomicClass()
  ){
    return ValIsEmpty(base, memb_def); // note: show not used for single guy
  } else { // instance of a class, so must recursively determine
    // just find all eligible guys, and return true if none fail
    for (int i = 0; i < members.size; ++i) {
      MemberDef* md = members.FastEl(i);
      if (!md || !md->ShowMember(taMisc::USE_SHOW_GUI_DEF, TypeItem::SC_ANY,
        for_show)) continue;
      if (!md->ValIsDefault(base, for_show))
        return false;
    }
    return true;
  }
}

bool TypeDef::ValIsEmpty(const void* base_, const MemberDef* memb_def) const 
{
  void* base = (void*)base_; // hack to avoid having to go through entire code below and fix
  // if its void, odds are its a function..
  if (InheritsFrom(TA_void) || ((memb_def) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun;
    if(memb_def != NULL)
      fun = TA_taRegFun.methods.FindOnListAddr(*((ta_void_fun*)base),
						 memb_def->lists, lidx);
    else
      fun = TA_taRegFun.methods.FindAddr(*((ta_void_fun*)base), lidx);
    if (fun)
      return false;
    else 
      return !(*((void**)base)); 
  }
  if (ptr == 0) {
    if (DerivesFrom(TA_bool)) {
      bool b = *((bool*)base);
      return !b; //T_Bool
    }
    // note: char is generic char, and typically we won't use signed char
    else if (DerivesFrom(TA_char)) {
      return (*((char*)base) == '\0');
    }
    // note: explicit use of signed char is treated like a number
    else if ((DerivesFrom(TA_signed_char))) {
      return (*((signed char*)base) == 0);
    }
    // note: explicit use of unsigned char is "byte" in ta/pdp
    else if ((DerivesFrom(TA_unsigned_char))) {
      return (*((unsigned char*)base) == 0);
    }
    else if(DerivesFrom(TA_short)) {
      return (*((short*)base) == 0); 
    }
    else if(DerivesFrom(TA_unsigned_short)) {
      return (*((unsigned short*)base) == 0);  
    }
    else if(DerivesFrom(TA_int)) {
      return (*((int*)base) == 0);
    }
    else if(DerivesFrom(TA_unsigned_int)) {
      return (*((uint*)base) == 0);
    }
    else if(DerivesFrom(TA_int64_t)) {
      return (*((int64_t*)base) == 0);
    }
    else if(DerivesFrom(TA_uint64_t)) {
      return (*((uint64_t*)base) == 0);  // T_UInt64
    }
    else if(DerivesFrom(TA_float)) {
      return (*((float*)base) == 0); // T_Double
    }
    else if(DerivesFrom(TA_double)) {
      return (*((double*)base) == 0); // T_Double
    }
    else if(DerivesFormal(TA_enum)) {
      return (*((int*)base) == 0); // T_Int
    }
    else if(DerivesFrom(TA_taString))
      return ((*((String*)base)).empty()); // T_String
    else if(DerivesFrom(TA_Variant)) {
      return ((*((Variant*)base)).isDefault());
    }
#ifndef NO_TA_BASE
    else if (DerivesFrom(TA_taSmartPtr)) {
      TAPtr rbase = (TAPtr)base;
      return !(rbase); // T_Base
    }
    else if (DerivesFrom(TA_taSmartRef)) {
      TAPtr rbase = *((taSmartRef*)base);
      return !(rbase); // T_Base
    }
#endif
    // must be some other value or a class -- default to saying no to empty
    else return false; 
    // NOTE: other value types are not really supported, just fall through to return invalid
  }
  else // (ptr >= 1) 
    return !(*((void**)base)); // only empty if NULL
}

void TypeDef::SetValVar(const Variant& val, void* base, void* par,
			MemberDef* memb_def) 
{
  if(InheritsFrom(TA_void) || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    MethodDef* fun = TA_taRegFun.methods.FindName(val.toString());
    if((fun != NULL) && (fun->addr != NULL))
      *((ta_void_fun*)base) = fun->addr;
    return;
  }
  if (ptr == 0) {
    if(DerivesFrom(TA_bool)) {
      *((bool*)base) = val.toBool(); return;
    }
    else if(DerivesFrom(TA_int)) {
      *((int*)base) = val.toInt(); return;}
    else if(DerivesFrom(TA_float)) {
      *((float*)base) = val.toFloat(); return;}
    else if(DerivesFormal(TA_enum)) {
      // if it is a number, assume direct value, otherwise it is a string
      if (val.isNumeric()) {
        *((int*)base) = val.toInt();
        return;
      }
      SetValStr_enum(val.toString(), base, par, memb_def);
    }
    else if(DerivesFrom(TA_taString)) {
      *((String*)base) = val.toString(); return;}
    // in general, Variant is handled by recalling this routine on its rep's typdef, then fixing null
    else if (DerivesFrom(TA_Variant)) {
      *((Variant*)base) = val; return;
    }
    // note: char is treated as an ansi character
    else if (DerivesFrom(TA_char)) {
    //TODO: char conversion heuristics
      *((char*)base) = val.toChar(); return;}
    // signed char is treated like a number
    else if (DerivesFrom(TA_signed_char)) {
      *((signed char*)base) = (signed char)val.toInt(); return;}
    // unsigned char is "byte" in ta/pdp and treated like a number
    else if (DerivesFrom(TA_unsigned_char)) {
      *((unsigned char*)base) = val.toByte(); return;}
    else if(DerivesFrom(TA_short)) {
      *((short*)base) = (short)val.toInt(); return;}
    else if(DerivesFrom(TA_unsigned_short)) {
      *((unsigned short*)base) = (unsigned short)val.toUInt(); return;}
    else if(DerivesFrom(TA_unsigned_int)) {
      *((uint*)base) = val.toUInt(); return;}
    else if(DerivesFrom(TA_int64_t)) {
      *((int64_t*)base) = val.toInt64(); return;}
    else if(DerivesFrom(TA_uint64_t)) {
      *((uint64_t*)base) = val.toUInt64(); return;}
    else if(DerivesFrom(TA_double)) {
      *((double*)base) = val.toDouble(); return;}
#ifndef NO_TA_BASE
    else if(DerivesFrom(TA_taList_impl)) {
      //TODO: not handled!
    }
    else if(DerivesFrom(TA_taArray_base)) {
      taArray_base* gp = (taArray_base*)base;
      if(gp != NULL)
	gp->InitFromString(val.toString());
      return;
    }
    else if(DerivesFrom(TA_taArray_impl)) {
      taArray_impl* gp = (taArray_impl*)base;
      if (gp != NULL) {
	gp->InitFromString(val.toString());
      }
      return;
    }
    else if (DerivesFrom(TA_taSmartPtr)) {
      // we just delegate, since we are binary compat
      TA_taBase_ptr.SetValVar(val, base, par, memb_def);
      return;
    }
    else if(DerivesFrom(TA_taSmartRef)) {
      //VERY DANGEROUS!!!! No type checking!!!!
      taSmartRef& ref = *((taSmartRef*)base);
      ref = val.toBase();
      return;
    }
#endif
  }
  else if(ptr == 1) {
#ifndef NO_TA_BASE
    if (DerivesFrom(TA_taBase)) {
      TAPtr bs = val.toBase();
      if (bs && !bs->GetTypeDef()->DerivesFrom(this)) {
        taMisc::Warning("Attempt to set member of type", this->name, " from ",
          bs->GetTypeDef()->name);
        return;
      
      }
      if (memb_def  && memb_def->HasOption("OWN_POINTER")) {
	if(par == NULL)
	  taMisc::Warning("*** NULL parent for owned pointer:");
	else
	  taBase::OwnPointer((TAPtr*)base, bs, (TAPtr)par);
      }
      else {
        if (memb_def && memb_def->HasOption("NO_SET_POINTER"))
	  (*(TAPtr*)base) = bs;
        else
	  taBase::SetPointer((TAPtr*)base, bs);
      }
      return;
    }
#endif
  }
  // if we get to here, the value can't be assigned
  taMisc::Warning("Type value was not assigned from Variant.");
}


#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
#define DMEM_TYPE (*(MPI_Datatype_PArray*)(dmem_type))
void TypeDef::AssertDMem_Type() {
  if (dmem_type) return;
  dmem_type = new MPI_Datatype_PArray;
}

MPI_Datatype TypeDef::GetDMemType(int share_set) {
  AssertDMem_Type();
  if(DMEM_TYPE.size > share_set) return DMEM_TYPE[share_set];
  if(DMEM_TYPE.size < share_set) GetDMemType(share_set-1);

  MPI_Datatype primitives[members.size];
  MPI_Aint byte_offsets[members.size];
  int block_lengths[members.size];

  int curr_prim = 0;
  for (int m = 0; m < members.size; m++) {
    MemberDef* md = members.FastEl(m);
    String shrset = md->OptionAfter("DMEM_SHARE_SET_");
    if(shrset.empty()) continue;
    if (md->type->ptr > 0) {
      if(taMisc::dmem_proc == 0) {
	taMisc::Error("WARNING: DMEM_SHARE_SET Specified for a pointer.",
		      "Pointers can not be shared.");
	continue;
      }
    }
    if (md->type->InheritsFormal(TA_class)) {
      primitives[curr_prim] = md->type->GetDMemType(share_set);
    }
    else if (md->type->InheritsFrom(TA_double)) {
      primitives[curr_prim] = MPI_DOUBLE;
    }
    else if (md->type->InheritsFrom(TA_float)) {
      primitives[curr_prim] = MPI_FLOAT;
    }
    else if (md->type->InheritsFrom(TA_int)) {
      primitives[curr_prim] = MPI_INT;
    }
    else if (md->type->InheritsFrom(TA_enum)) {
      primitives[curr_prim] = MPI_INT;
    }
    else if (md->type->InheritsFrom(TA_long)) {
      primitives[curr_prim] = MPI_LONG;
    }
    else {
      taMisc::Error("WARNING: DMEM_SHARE_SET Specified for an unrecognized type.",
		    "unrecoginized types can not be shared.");
      continue;
    }

    byte_offsets[curr_prim] = (ulong)md->GetOff((void *)0x100) - 0x100;
    block_lengths[curr_prim] = 1;
    curr_prim++;
  }
  MPI_Datatype new_type;
  if (!curr_prim) {
    new_type = MPI_DATATYPE_NULL;
    taMisc::Error("Null type, set:", String(share_set),", type:",name);
  }
  else {
    MPI_Type_struct(curr_prim, block_lengths, byte_offsets, primitives, &new_type);
    MPI_Type_commit(&new_type);
  }
  DMEM_TYPE.Add(new_type);
  return new_type;
}

#endif

//////////////////////////////////
// 	CopyFromSameType	//
//////////////////////////////////

void MemberSpace::CopyFromSameType(void* trg_base, void* src_base) {
  int i;
  for(i=0; i<size; i++) {
    MemberDef* md = FastEl(i);
    if(!md->HasOption("NO_COPY"))
      md->CopyFromSameType(trg_base, src_base);
  }
}

void MemberSpace::CopyOnlySameType(void* trg_base, void* src_base) {
  int i;
  for(i=0; i<size; i++) {
    MemberDef* md = FastEl(i);
    if((md->owner == this) && !md->HasOption("NO_COPY")) // only same type provision
      md->CopyFromSameType(trg_base, src_base); 	// then copy whole thing..
  }
}

void MemberDef::CopyFromSameType(void* trg_base, void* src_base) {
  type->CopyFromSameType(GetOff(trg_base), GetOff(src_base), this);
}

void MemberDef::CopyOnlySameType(void* trg_base, void* src_base) {
  type->CopyOnlySameType(GetOff(trg_base), GetOff(src_base), this);
}

void TypeDef::CopyFromSameType(void* trg_base, void* src_base,
			       MemberDef* memb_def)
{
  // if its void, odds are it is a fun pointer
  if(InheritsFrom(TA_void) || ((memb_def != NULL) && (memb_def->fun_ptr != 0))) {
    int lidx;
    MethodDef* fun;
    if(memb_def != NULL)
      fun = TA_taRegFun.methods.FindOnListAddr(*((ta_void_fun*)src_base),
						 memb_def->lists, lidx);
    else
      fun = TA_taRegFun.methods.FindAddr(*((ta_void_fun*)src_base), lidx);
    if((fun != NULL) || (memb_def != NULL))
      *((ta_void_fun*)trg_base) = *((ta_void_fun*)src_base); // must be a funptr
    else
      *((void**)trg_base) = *((void**)src_base); // otherwise just a voidptr
    return;
  }
  if (ptr == 0) {
    // internal types can simply be bit copied
    if (internal) {
      memcpy(trg_base, src_base, size);
    }
    else if(DerivesFormal(&TA_enum)) {
      memcpy(trg_base, src_base, size); // bit copy
    }
    else if (DerivesFrom(TA_Variant))
      *((Variant*)trg_base) = *((Variant*)src_base);
    else if (DerivesFrom(TA_taString))
      *((String*)trg_base) = *((String*)src_base);
#ifndef NO_TA_BASE
    else if(DerivesFrom(TA_taSmartRef))
      *((taSmartRef*)trg_base) = *((taSmartRef*)src_base);
    else if(DerivesFrom(TA_taSmartPtr))
      *((taSmartPtr*)trg_base) = *((taSmartPtr*)src_base);
    else if(DerivesFrom(TA_taBase)) {
      TAPtr rbase = (TAPtr)trg_base;
      TAPtr sbase = (TAPtr)src_base;
      if(sbase->InheritsFrom(rbase->GetTypeDef()) || rbase->InheritsFrom(sbase->GetTypeDef())) // makin it safe..
	rbase->UnSafeCopy(sbase);
    }
#endif
    else if(DerivesFormal(TA_class))
      members.CopyFromSameType(trg_base, src_base);
  }
  else if(ptr >= 1) {
#ifndef NO_TA_BASE
    if((ptr == 1) && DerivesFrom(TA_taBase)) {
      TAPtr* rbase = (TAPtr*)trg_base;
      TAPtr* sbase = (TAPtr*)src_base;
      taBase::SetPointer(rbase, *sbase);
    }
    else {
#endif
      *((void**)trg_base) = *((void**)src_base);
#ifndef NO_TA_BASE
    }
#endif
  }
}

void TypeDef::CopyOnlySameType(void* trg_base, void* src_base,
			       MemberDef* memb_def)
{
  if(InheritsFormal(TA_class)) {
#ifndef NO_TA_BASE
    if(InheritsFrom(TA_taBase)) {
      TAPtr src = (TAPtr)src_base;
      // I actually inherit from the other guy, need to use their type for copying!
      if((src->GetTypeDef() != this) && InheritsFrom(src->GetTypeDef())) {
	src->GetTypeDef()->CopyOnlySameType(trg_base, src_base);
	return;
      }
    }
#endif // NO_TA_BASE
    members.CopyOnlySameType(trg_base, src_base);
  }
  else
    CopyFromSameType(trg_base, src_base, memb_def);
}

void TypeDef::MemberCopyFrom(int memb_no, void* trg_base, void* src_base) {
  if(memb_no < members.size)
    members[memb_no]->CopyFromSameType(trg_base, src_base);
}

//////////////////////////////////
// 	CompareSameType		//
//////////////////////////////////

bool MemberSpace::CompareSameType(Member_List& mds, TypeSpace& base_types,
				  void_PArray& trg_bases, void_PArray& src_bases,
				  TypeDef* base_typ, void* trg_base, void* src_base,
				  int show_forbidden, int show_allowed, bool no_ptrs,
				  bool test_only) {
  bool some_diff = false;
  int i;
  for(i=0; i<size; i++) {
    MemberDef* md = FastEl(i);
    if(md->ShowMember(show_forbidden, TypeItem::SC_ANY, show_allowed)) {
      if(no_ptrs && (md->type->ptr > 0 || md->type->HasOption("SMART_POINTER"))) continue;
      some_diff |= md->CompareSameType(mds, base_types, trg_bases, src_bases, 
				       base_typ, trg_base, src_base,
				       show_forbidden, show_allowed, no_ptrs, test_only);
    }
  }
  return some_diff;
}

bool MemberDef::CompareSameType(Member_List& mds, TypeSpace& base_types,
				void_PArray& trg_bases, void_PArray& src_bases,
				TypeDef* base_typ, void* trg_base, void* src_base,
				int show_forbidden, int show_allowed, bool no_ptrs,
				bool test_only) {
  bool some_diff = false;
  if(type->InheritsFormal(TA_class)) {
    if(type->HasOption("EDIT_INLINE") || type->HasOption("INLINE")) {
      // check the members 
      some_diff = type->CompareSameType(mds, base_types, trg_bases, src_bases,
					GetOff(trg_base), GetOff(src_base),
					show_forbidden, show_allowed,
					no_ptrs, true); // test only!
      if(some_diff && !test_only) {			// not already testing, add it
	mds.Link(this); base_types.Link(base_typ); 
	trg_bases.Add(trg_base); src_bases.Add(src_base);
      }
    }
    else {
      some_diff = type->CompareSameType(mds, base_types, trg_bases, src_bases,
					GetOff(trg_base), GetOff(src_base),
					show_forbidden, show_allowed, no_ptrs);
    }
  }
  else {
    // actually do the comparison, based on string value
    if(type->GetValStr(GetOff(trg_base), trg_base, this) !=
       type->GetValStr(GetOff(src_base), src_base, this)) {
      if(!test_only) {
	mds.Link(this); base_types.Link(base_typ); 
	trg_bases.Add(trg_base); src_bases.Add(src_base);
      }
      some_diff = true;
    }
  }
  return some_diff;
}

bool TypeDef::CompareSameType(Member_List& mds, TypeSpace& base_types,
			      void_PArray& trg_bases, void_PArray& src_bases,
			      void* trg_base, void* src_base,
			      int show_forbidden, int show_allowed, bool no_ptrs,
			      bool test_only) {
  if(InheritsFormal(TA_class)) {
    return members.CompareSameType(mds, base_types, trg_bases, src_bases,
				   this, trg_base, src_base,
				   show_forbidden, show_allowed, no_ptrs, test_only);
  }
  else {
    taMisc::Error("CompareSameType called on non-class object -- does not work!");
    return false;
  }
}

//////////////////////////////////////////////////////////
// 			OutputType			//
//////////////////////////////////////////////////////////

static void OutputType_OptsLists(ostream& strm, const String_PArray& opts,
			    const String_PArray& lists)
{
  if((opts.size > 0) && (taMisc::type_info_ == taMisc::ALL_INFO) ||
     (taMisc::type_info_ == taMisc::NO_LISTS)) {
    int i;
    for(i=0; i<opts.size; i++)
      strm << " #" << opts.FastEl(i);
    taMisc::FlushConsole();
  }
  if((lists.size > 0) && (taMisc::type_info_ == taMisc::ALL_INFO) ||
     (taMisc::type_info_ == taMisc::NO_OPTIONS)) {
    strm << " LISTS:";
    int i;
    for(i=0; i<lists.size; i++)
      strm << " " << lists.FastEl(i);
  }
}


ostream& TypeDef::OutputInherit_impl(ostream& strm) const {
  int i;
  for(i=0; i<parents.size; i++) {
    TypeDef* par = parents.FastEl(i);
    strm << par->name;
    if(par_off[i] > 0)
      strm << " +" << par_off[i];
    if(par->parents.size > 0)
      strm << " : ";
    par->OutputInherit_impl(strm);
    if(i < parents.size-1)
      strm << ", ";
  }
  return strm;
}

ostream& TypeDef::OutputInherit(ostream& strm) const {
  int i;
  for(i=0; i<par_formal.size; i++)
    strm << par_formal.FastEl(i)->name << " ";

  strm << name;
  if(parents.size > 0)
    strm << " : ";
  OutputInherit_impl(strm);
  return strm;
}

ostream& TypeDef::OutputType(ostream& strm, int indent) const {
  taMisc::indent(strm, indent);
  OutputInherit(strm);
//  if(taMisc::type_info_ == taMisc::ALL_INFO)
  strm << " (Sz: " << size << ")";
  if(InheritsFormal(TA_class) || InheritsFormal(TA_enum))
    strm << " {";
  else
    strm << ";";
  if(!desc.empty()) {
    if(InheritsFormal(TA_class)) {
      strm << "\n";
      taMisc::indent(strm, indent) << "//" << desc ;
    }
    else
      strm << "\t//" << desc ;
  }

  OutputType_OptsLists(strm, opts, lists);
  strm << "\n";
  if(InheritsFormal(TA_class)) {
    if(sub_types.size > 0) {
      strm << "\n";
      taMisc::indent(strm, indent+1) << "// sub-types\n";
      int i;
      for(i=0; i<sub_types.size; i++) {
	if(!(sub_types.FastEl(i)->internal))
	  sub_types.FastEl(i)->OutputType(strm,indent+1) << "\n";
      }
    }
    members.OutputType(strm, indent+1);
    methods.OutputType(strm, indent+1);
    taMisc::indent(strm, indent) << "} ";
    if(children.size > 0) {
      strm << " children: ";
      int i;
      for(i=0; i<children.size; i++) {
	strm << children.FastEl(i)->name;
	if(i < children.size - 1)
	  strm << ", ";
      }
    }
    strm << "\n";
  }
  else if(InheritsFormal(TA_enum)) {
    enum_vals.OutputType(strm, indent+1);
    taMisc::indent(strm, indent) << "}\n";
  }
  taMisc::FlushConsole();
  return strm;
}

ostream& EnumSpace::OutputType(ostream& strm, int indent) const {
  EnumDef* enm;
  int i;
  for(i=0; i<size; i++) {
    enm = FastEl(i);
    taMisc::fmt_sep(strm, enm->name, 0, indent);
    strm << " = " << enm->enum_no << ";";
    if(!enm->desc.empty())
      strm << "\t//" << enm->desc;
    OutputType_OptsLists(strm, enm->opts, enm->lists);
    strm << "\n";
    taMisc::FlushConsole();
  }
  return strm;
}

ostream& MemberSpace::OutputType(ostream& strm, int indent) const {
  strm << "\n";
  taMisc::indent(strm, indent) << "// members\n";
  int i;
  for(i=0; i<size; i++) {
    FastEl(i)->OutputType(strm, indent) << "\n";
    taMisc::FlushConsole();
  }
  return strm;
}

ostream& MethodSpace::OutputType(ostream& strm, int indent) const {
  strm << "\n";
  taMisc::indent(strm, indent) << "// functions\n";
  int i;
  for(i=0; i<size; i++) {
    FastEl(i)->OutputType(strm, indent) << "\n";
    taMisc::FlushConsole();
  }
  return strm;
}

ostream& MemberDef::OutputType(ostream& strm, int indent) const {
  String cnm = type->Get_C_Name();
  if(is_static)
    cnm = String("static ") + cnm;
  taMisc::fmt_sep(strm, cnm, 0, indent);
  String nwnm;
  if(fun_ptr)
    nwnm = String("(*") + name + ")()";
  else
    nwnm = name + ";";
  taMisc::fmt_sep(strm, nwnm, 1, indent);
  strm << "// ";
  if(taMisc::type_info_ == taMisc::MEMB_OFFSETS) {
    intptr_t ui_off = (intptr_t)GetOff((void*)0x100); // 0x100 is arbitrary non-zero number..
    ui_off -= 0x100;			      // now get rid of offset
    if(ui_off > 0) {
      strm << "+" << ui_off;
      if(base_off > 0)
	strm << "+" << base_off;
    }
    if(base_off > 0)
      strm << "+" << base_off;
  }
  if(!desc.empty())
    strm << " " << desc;
  OutputType_OptsLists(strm, opts, lists);
  return strm;
}

ostream& MethodDef::OutputType(ostream& strm, int indent) const {
  String cnm = type->Get_C_Name();
  if(is_static)
    cnm = String("static ") + cnm;
  taMisc::fmt_sep(strm, cnm, 0, indent);
  strm << name << "(";
  if(fun_argc > 0) {
    int i;
    for(i=0; i<fun_argc; i++) {
      strm << arg_types[i]->Get_C_Name() << " " << arg_names[i];
      if((fun_argd >= 0) && (i >= fun_argd))	// indicate a default
	strm << "=" << arg_defs[i];
      if(i+1 < fun_argc)
	strm << ", ";
    }
  }
  strm << ");";
  if(!desc.empty())
    strm << "\t//" + desc;
  OutputType_OptsLists(strm, opts, lists);
  return strm;
}


//////////////////////////////////////////////////////////
// 			output/R 			//
//////////////////////////////////////////////////////////

ostream& MemberSpace::Output(ostream& strm, void* base, int indent) const {
  int i;
  for(i=0; i<size; i++)
    FastEl(i)->Output(strm, base, indent);
  return strm;
}
ostream& MemberSpace::OutputR(ostream& strm, void* base, int indent) const {
  int i;
  for(i=0; i<size; i++)
    FastEl(i)->OutputR(strm, base, indent);
  return strm;
}

ostream& MemberDef::Output(ostream& strm, void* base, int indent) const {
  if(!ShowMember())
    return strm;
  void* new_base = GetOff(base);
  String cnm = type->Get_C_Name();
  taMisc::fmt_sep(strm, cnm, 0, indent);
  if(fun_ptr) {
    String nnm = String("(*") + name + ")()";
    taMisc::fmt_sep(strm, nnm, 1, indent) << "= ";
  }
  else {
    taMisc::fmt_sep(strm, name, 1, indent) << "= ";
  }
  strm << type->GetValStr(new_base, base, (MemberDef*)this);
  strm << ";\n";

  return strm;
}

ostream& MemberDef::OutputR(ostream& strm, void* base, int indent) const {
  if(!ShowMember())
    return strm;
#ifndef NO_TA_BASE
  if((type->DerivesFormal(TA_class)) && !(type->DerivesFrom(TA_taString))) {
    String cnm = type->Get_C_Name();
    taMisc::fmt_sep(strm, cnm, 0, indent);
    taMisc::fmt_sep(strm, name, 1, indent);
    if(type->ptr == 0) {
      if(type->DerivesFrom(TA_taBase)) {
	TAPtr rbase = (TAPtr)GetOff(base);
	rbase->OutputR(strm, indent);
      }
      else
	type->OutputR(strm, GetOff(base), indent);
    }
    else
      strm << "= " << type->GetValStr(GetOff(base), base, (MemberDef*)this) << ";\n";
  }
  else
#endif
    Output(strm, base, indent);
  return strm;
}

ostream& TypeDef::Output(ostream& strm, void* base, int indent) const {
  taMisc::indent(strm, indent);
#ifndef NO_TA_BASE
  if(DerivesFrom(TA_taBase)) {
    TAPtr rbase;
    if(ptr == 0)
      rbase = (TAPtr)base;
    else if(ptr == 1)
      rbase = *((TAPtr*)base);
    else
      rbase = NULL;

    if(rbase)
      strm << rbase->GetPath();
    else
      strm << Get_C_Name();
  }
  else
#endif
    strm << Get_C_Name();

#ifndef NO_TA_BASE
  if(InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)base;
    strm << " " << rbase->GetName()
	 << " (refn=" << taBase::GetRefn(rbase) << ")";
  }
#endif

  if(InheritsFormal(TA_class)) {
    strm << " {\n";
    members.Output(strm, base, indent+1);
    taMisc::indent(strm, indent) << "}\n";
  }
  else {
    strm << " = " << GetValStr(base) << "\n";
  }
  return strm;
}

ostream& TypeDef::OutputR(ostream& strm, void* base, int indent) const {
  taMisc::indent(strm, indent);
#ifndef NO_TA_BASE
  if(DerivesFrom(TA_taBase)) {
    TAPtr rbase;
    if(ptr == 0)
      rbase = (TAPtr)base;
    else if(ptr == 1)
      rbase = *((TAPtr*)base);
    else
      rbase = NULL;

    if(rbase)
      strm << rbase->GetPath();
    else
      strm << Get_C_Name();
  }
  else
#endif
    strm << Get_C_Name();

#ifndef NO_TA_BASE
  if(InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)base;
    strm << " " << rbase->GetName()
	 << " (refn=" << taBase::GetRefn(rbase) << ")";
  }
#endif

  if(InheritsFormal(TA_class)) {
    strm << " {\n";
    members.OutputR(strm, base, indent+1);
    taMisc::indent(strm, indent) << "}\n";
  }
  else {
    strm << " = " << GetValStr(base) << "\n";
  }
  return strm;
}

String TypeDef::GetHTMLLink() const {
  STRING_BUF(rval, 32); // extends if needed
  TypeDef* npt = (TypeDef*)this;
  if(npt->ptr > 0)
    npt = npt->GetNonPtrType();
  if(npt && npt->ref)
    npt = npt->GetNonRefType();
  if(npt && npt->DerivesFrom(TA_const))
    npt = npt->GetNonConstType();
  if(npt) {
    if(npt->InheritsFormal(TA_class)) {
      rval.cat("<a href=\"").cat(npt->name).cat(".html\">").cat(Get_C_Name()).cat("</a>");
    }
    else if(npt->InheritsFormal(TA_enum)) {
      rval.cat("<a href=\"#").cat(npt->name).cat("\">").cat(Get_C_Name()).cat("</a>");
    }
    else {
      rval.cat(Get_C_Name());
    }
  }
  else {
    rval.cat(Get_C_Name());
  }
  return rval;
}

String TypeDef::GetHTMLSubType(int detail_level) const {
  STRING_BUF(rval, 100); // extends if needed
  String own_typ;
  TypeDef* ot = GetOwnerType();
  if(ot) {
    own_typ.cat(ot->name).cat("::");
  }
  if(InheritsFormal(TA_class)) {
    // todo: not yet supported by maketa..
  }
  else if(InheritsFormal(TA_enum)) {
    if(detail_level == 0) {
      rval.cat("enum <b><a href=\"#").cat(name).cat("\">").cat(own_typ).cat(name).cat("</a></b> { ");
      for(int i=0;i<enum_vals.size;i++) {
	EnumDef* ed = enum_vals[i];
	if(i > 0) rval.cat(", ");
	rval.cat(ed->name);
      }
      rval.cat(" }");
    }
    else {
      rval.cat("<h3 class=\"flags\"><a name=\"").cat(name).cat("\"></a>");
      rval.cat("enum ").cat(own_typ).cat(name).cat("</h3>\n");
      rval.cat("<p>").cat(trim(desc).xml_esc()).cat("</p>\n");
      rval.cat("<p><table border=\"1\" cellpadding=\"2\" cellspacing=\"1\" width=\"100%\">\n");
      rval.cat("<tr><th width=\"25%\">Constant</th><th width=\"15%\">Value</th><th width=\"60%\">Description</th></tr>\n");
      for(int i=0;i<enum_vals.size;i++) {
	EnumDef* ed = enum_vals[i];
	rval.cat("<tr><td valign=\"top\"><tt>").cat(own_typ).cat(ed->name).cat("</tt></td>");
	rval.cat("<td align=\"center\" valign=\"top\"><tt>0x").cat(String(ed->enum_no,"%08X")).cat("</tt></td>");
	rval.cat("<td valign=\"top\">").cat(trim(ed->desc).xml_esc()).cat("</td></tr>\n");
      }
      rval.cat("</table></p>\n");
    }
  }
  else {
    rval.cat("typedef ");
    for(int i=0;i<parents.size;i++) {
      rval.cat(parents[i]->Get_C_Name()).cat(" ");
    }
    rval.cat(name);
  }
  return rval;
}

String TypeDef::GetHTML(int detail_level) const {
  if(!InheritsFormal(TA_class)) return GetHTMLLink();

  STRING_BUF(rval, 9096); // extends if needed

  int sub_detail = MAX(1, detail_level); // what we pass onto our sub-guys

  String wiki_help_url = taMisc::GetWikiURL(taMisc::web_help_wiki, true); // add index

  // preamble
  rval.cat("<head>\n");
  rval.cat("<title>").cat(taMisc::app_name).cat(" ").cat(taMisc::version).cat(" ").cat(name).cat(" Class Reference</title>\n");
  rval.cat("<link href=\"classic.css\" rel=\"stylesheet\" type=\"text/css\" />\n");
  rval.cat("</head>\n");
  rval.cat("<body>\n");
  rval.cat("<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"100%\">\n");
  rval.cat("<tr>\n");
  rval.cat("<td align=\"left\" valign=\"top\" width=\"32\"><a href=\"").cat(taMisc::web_home).cat("\">");
  rval.cat(taMisc::app_name).cat("</a> ").cat(taMisc::version).cat("</td>\n");
  rval.cat("<td width=\"1\">&nbsp;&nbsp;</td><td class=\"postheader\" valign=\"center\">");
  rval.cat("<a href=\"").cat(taMisc::web_home).cat("\"><font color=\"#004faf\">Home</font></a>&nbsp;&middot; ");
  rval.cat("<a href=\"").cat(wiki_help_url).cat(name).cat("\"><font color=\"#004faf\">Wiki Docs For: ").cat(name).cat("</font></a>&nbsp;&middot; ");
  rval.cat("<a href=\"ta:class_browser\"><font color=\"#004faf\">Class&nbsp;Browser</font></a>&nbsp\n");

  rval.cat("<td align=\"right\" valign=\"top\" width=\"230\"></td></tr></table><h1 class=\"title\">");
  rval.cat(name).cat(" Class Reference<br />\n");
  // <span class=\"small-subtitle\">[<a href="qtgui.html">QtGui</a> module]</span>
  rval.cat("</h1>\n\n");

  rval.cat("<p>").cat(trim(desc).xml_esc()).cat("</p>\n");
  rval.cat("<p>See for more info: <a href=\"").cat(wiki_help_url).cat(name).cat("\">Wiki Docs For: ").cat(name).cat("</a></p>\n\n");

  // NOTE: for include file tracking -- not a bad idea to actually record this info in maketa!!
  //  rval.cat("<pre> #include &lt;QTextDocument&gt;</pre>\n");

  if(par_cache.size > 0) {
    int inhi = 0;
    rval.cat("<p>Inherits From: ");
    for(int i=0;i<par_cache.size; i++) {
      TypeDef* par = par_cache[i];
      if(par->InheritsFormal(TA_templ_inst)) continue; // none of those
      if(inhi > 0) rval.cat(", ");
      rval.cat(par->GetHTMLLink());
      inhi++;
    }
    rval.cat("</p>\n\n");	
  }

  if(children.size > 0) {
    int inhi = 0;
    rval.cat("<p>Inherited By: ");
    for(int i=0;i<children.size; i++) {
      TypeDef* par = children[i];
      if(par->InheritsFormal(TA_templ_inst)) continue; // none of those
      if(inhi > 0) rval.cat(", ");
      rval.cat(par->GetHTMLLink());
      inhi++;
    }
    rval.cat("</p>\n\n");	
  }

  if(detail_level >= 2) {
    if(opts.size > 0) {
      rval.cat("<p>").cat(GetOptsHTML()).cat("</p>\n");
    }
  }

  rval.cat("<p>Index: <a href=\"#subtypes\">SubTypes</a>, ");
  rval.cat("<a href=\"#members\">Members</a>, ");
  rval.cat("<a href=\"#methods\">Methods</a></p>");

  rval.cat("<a name=\"subtypes\"></a>\n");
  rval.cat("<h3>Sub Types</h3>\n");
  if(sub_types.size > 0) {
    rval.cat("<ul>\n");
    for(int i=0;i<sub_types.size;i++) {
      TypeDef* st = sub_types[i];
      if(!st->InheritsFormal(&TA_enum)) continue;
      if((this != &TA_taBase) && (st->GetOwnerType() == &TA_taBase)) continue;
      if(st->GetOwnerType()->InheritsFormal(TA_templ_inst)) continue;
      rval.cat("<li>").cat(st->GetHTMLSubType(0)).cat("</li>\n");
    }
    rval.cat("</ul>\n");
  }
  
  /////////////////////////////////////////////////////////////////////
  // collect the members and methods into string arrays for sorting 
  String_PArray memb_idx;
  for(int i=0;i<members.size;i++) {
    MemberDef* md = members[i];
    if(detail_level < 2) {
      if(md->HasOption("NO_SHOW") || md->HasOption("HIDDEN")) continue;
    }
    if((this != &TA_taBase) && (md->GetOwnerType() == &TA_taBase)) continue;
    String cat = md->GetCat();
    if(cat.empty()) cat = "_NoCategory";
    String key = cat + ":" + md->name;
    memb_idx.Add(key);
  }
  memb_idx.Sort();		// sorts by category then key, in effect

  String_PArray meth_idx;
  for(int i=0;i<methods.size;i++) {
    MethodDef* md = methods[i];
    // expert methods are fine for this kind of doc rendering, given that they are grouped at bottom typically
//     if(detail_level < 2) {
//       if(md->HasOption("EXPERT")) continue;
//     }
    if((this != &TA_taBase) && (md->GetOwnerType() == &TA_taBase)) continue;
    String cat = md->GetCat();
    if(cat.empty()) cat = "_NoCategory";
    String key = cat + ":" + md->name;
    meth_idx.Add(key);
  }
  meth_idx.Sort();		// sorts by category then key, in effect

  //////////////////////////////////////////
  // then do first pass output

  rval.cat("<a name=\"members\"></a>\n");
  rval.cat("<h3>Members</h3>\n");
  if(memb_idx.size > 0) {
    String prv_cat;
    for(int i=0;i<memb_idx.size;i++) {
      String key = memb_idx[i];
      String cat = key.before(':');
      String mnm = key.after(':');
      MemberDef* md = members.FindName(mnm);
      if(cat != prv_cat) {
	if(prv_cat.nonempty()) rval.cat("</ul>\n"); // terminate prev
	rval.cat("<h4>Member Category: <a href=\"#MbrCat-").cat(cat).cat("\">").cat(cat).cat("</a></h4>\n");
	rval.cat("<ul>\n");
	prv_cat = cat;
      }
      rval.cat("<li>").cat(md->GetHTML(0)).cat("</li>\n");
    }
    rval.cat("</ul>\n");
  }

  rval.cat("<a name=\"methods\"></a>\n");
  rval.cat("<h3>Methods</h3>\n");
  if(meth_idx.size > 0) {
    String prv_cat;
    for(int i=0;i<meth_idx.size;i++) {
      String key = meth_idx[i];
      String cat = key.before(':');
      String mnm = key.after(':');
      MethodDef* md = methods.FindName(mnm);
      if(cat != prv_cat) {
	if(prv_cat.nonempty()) rval.cat("</ul>\n"); // terminate prev
	rval.cat("<h4>Method Category: <a href=\"#MthCat-").cat(cat).cat("\">").cat(cat).cat("</a></h4>\n");
	rval.cat("<ul>\n");
	prv_cat = cat;
      }
      rval.cat("<li>").cat(md->GetHTML(0)).cat("</li>\n");
    }
    rval.cat("</ul>\n");
  }

  /////////////////////////////////////////////////////
  // now for full detail render

  if(sub_types.size > 0) {
    rval.cat("<hr />\n");
    rval.cat("<h2>SubType Documentation</h2>\n");

    for(int i=0;i<sub_types.size;i++) {
      TypeDef* st = sub_types[i];
      if(!st->InheritsFormal(&TA_enum)) continue;
      if((this != &TA_taBase) && (st->GetOwnerType() == &TA_taBase)) continue;
      if(st->GetOwnerType()->InheritsFormal(TA_templ_inst)) continue;
      rval.cat(st->GetHTMLSubType(sub_detail));
    }
  }

  if(memb_idx.size > 0) {
    rval.cat("<hr />\n");
    rval.cat("<h2>Member Documentation</h2>\n");

    String prv_cat;
    for(int i=0;i<memb_idx.size;i++) {
      String key = memb_idx[i];
      String cat = key.before(':');
      String mnm = key.after(':');
      MemberDef* md = members.FindName(mnm);
      if(cat != prv_cat) {
	rval.cat("<a name=\"MbrCat-").cat(cat).cat("\"></a>\n");
	rval.cat("<h3>Member Category: ").cat(cat).cat("</h3>\n");
	prv_cat = cat;
      }
      rval.cat(md->GetHTML(sub_detail)).cat("\n"); // extra cr for good readability
    }
  }

  if(methods.size > 0) {
    rval.cat("<hr />\n");
    rval.cat("<h2>Method Documentation</h2>\n");

    String prv_cat;
    for(int i=0;i<meth_idx.size;i++) {
      String key = meth_idx[i];
      String cat = key.before(':');
      String mnm = key.after(':');
      MethodDef* md = methods.FindName(mnm);
      if(cat != prv_cat) {
	rval.cat("<a name=\"MthCat-").cat(cat).cat("\"></a>\n");
	rval.cat("<h3>Method Category: ").cat(cat).cat("</h3>\n");
	prv_cat = cat;
      }
      rval.cat(md->GetHTML(sub_detail)).cat("\n"); // extra cr for good readability
    }
  }

  /////////////////////////
  // postscript/footer
  rval.cat("<p /><address><hr /><div align=\"center\">\n");
  rval.cat("<table width=\"100%\" cellspacing=\"0\" border=\"0\"><tr class=\"address\">\n");
  rval.cat("<td width=\"70%\" align=\"left\">Copyright &copy; 2009 Regents of the University of Colorado, Carnegie Mellon University, Princeton University.</td>\n");
  //  rval.cat("<td width=\"40%\" align=\"center\"><a href=\"trademarks.html\">Trademarks</a></td>\n");
  rval.cat("<td width=\"30%\" align=\"right\"><div align=\"right\"> ").cat(taMisc::app_name).cat(" ").cat(taMisc::version).cat("</div></td>\n");
  rval.cat("</tr></table></div></address></body>\n");
  rval.cat("</html>\n");
  return rval;
}

#ifdef NO_TA_BASE

//////////////////////////////////////////////////////////
//  dummy versions of dump load/save: see ta_dump.cc	//
//////////////////////////////////////////////////////////

int MemberSpace::Dump_Save(ostream&, void*, void*, int) {
  return false;
}
int MemberSpace::Dump_SaveR(ostream&, void*, void*, int) {
  return false;
}

int MemberSpace::Dump_Save_PathR(ostream&, void*, void*, int) {
  return false;
}

bool MemberDef::DumpMember(void* par) {
  return false;
}


int MemberDef::Dump_Save(ostream&, void*, void*, int) {
  return false;
}

int MemberDef::Dump_SaveR(ostream&, void*, void*, int) {
  return false;
}

int MemberDef::Dump_Save_PathR(ostream&, void*, void*, int) {
  return false;
}

int TypeDef::Dump_Save_Path(ostream&, void*, void*, int) {
  return true;
}

int TypeDef::Dump_Save_PathR(ostream&, void*, void*, int) {
  return false;
}

int TypeDef::Dump_Save_Value(ostream&, void*, void*, int) {
  return true;
}

int TypeDef::Dump_Save_impl(ostream&, void*, void*, int) {
  return true;
}

int TypeDef::Dump_Save_inline(ostream&, void*, void*, int) {
  return true;
}

int TypeDef::Dump_Save(ostream&, void*, void*, int) {
  return true;
}

int TypeDef::Dump_SaveR(ostream&, void*, void*, int) {
  return false;
}


//////////////////////////////////////////////////////////
// 			dump load			//
//////////////////////////////////////////////////////////

int MemberSpace::Dump_Load(istream&, void*, void*, const char*, int) {
  return true;
}

int MemberDef::Dump_Load(istream&, void*, void*) {
  return false;
}

int TypeDef::Dump_Load_Path(istream&, void*&, void*, TypeDef*&, String&, const char*) {
  return true;
}

int TypeDef::Dump_Load_Path_impl(istream&, void*&, void*, String) {
  return true;
}

int TypeDef::Dump_Load_Value(istream&, void*, void*) {
  return true;
}

int TypeDef::Dump_Load_impl(istream&, void*, void*, const char*) {
  return true;
}

int TypeDef::Dump_Load(istream&, void*, void*, void**) {
  return true;
}

#endif // NO_TA_BASE

