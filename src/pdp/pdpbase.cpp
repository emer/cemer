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

// for debugging the qconsole, uncomment this define and it will revert to qandd console
// #define QANDD_CONSOLE 1

#include "pdpbase.h"

#include "ta_type.h"
#include "ta_filer.h"
#include "ta_css.h"
#include "css_builtin.h"
#include "css_console.h"

#include "datatable.h"

#include "pdpshell.h"

#ifdef TA_GUI
# include "ta_qt.h"
# include "css_qt.h"
# include "ta_qtdata.h" // for taiObjChooser
# include "ta_qtviewer.h"
# include "ta_qtbrowse.h"
# include "pdp_qtso.h"
# include <qbitmap.h>
# include <QApplication>
# include <QFont>
#endif

#ifdef TA_USE_INVENTOR
  #include <Inventor/Qt/SoQt.h>
#endif

#include <QCoreApplication>

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif

#include <signal.h>
#include <memory.h>
//obs #include <malloc.h>
//nn?? #include <unistd.h>
//#include <sstream>
#include <time.h>

#include <QDir>
#include <ta_plugin.h>

#ifdef TA_GUI
#define pdp_bitmap_width 64
#define pdp_bitmap_height 64
static unsigned char pdp_bitmap_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x83, 0xff,
  0x1f, 0xfc, 0xff, 0x00, 0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00,
  0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00, 0x10, 0x1f, 0x82, 0xf8,
  0x10, 0xc4, 0x87, 0x00, 0x10, 0x32, 0x82, 0x10, 0x11, 0x84, 0x8c, 0x00,
  0x10, 0x22, 0x82, 0x10, 0x11, 0x84, 0x88, 0x00, 0x10, 0x32, 0x82, 0x10,
  0x11, 0x84, 0x8c, 0x00, 0x10, 0x1e, 0x82, 0x10, 0x11, 0x84, 0x87, 0x00,
  0x10, 0x02, 0x82, 0x10, 0x11, 0x84, 0x80, 0x00, 0x10, 0x02, 0x82, 0x10,
  0x11, 0x84, 0x80, 0x00, 0x10, 0x0f, 0x82, 0xf8, 0x10, 0xc4, 0x83, 0x00,
  0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00, 0x10, 0x00, 0x82, 0x00,
  0x10, 0x04, 0x80, 0x00, 0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00,
  0xf0, 0xff, 0x83, 0xff, 0x1f, 0xfc, 0xff, 0x00, 0x00, 0x78, 0x00, 0x50,
  0x00, 0xf0, 0x00, 0x00, 0x00, 0x90, 0x03, 0x88, 0x00, 0x4e, 0x00, 0x00,
  0x00, 0x20, 0x0c, 0x04, 0xc1, 0x21, 0x00, 0x00, 0x00, 0x40, 0x70, 0x02,
  0x3a, 0x10, 0x00, 0x00, 0x00, 0x40, 0x80, 0x03, 0x06, 0x08, 0x00, 0x00,
  0x00, 0x80, 0x80, 0xdc, 0x05, 0x04, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8,
  0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x22, 0x07, 0x13, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xf4, 0x00, 0xbc, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07,
  0xff, 0x3f, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
  0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0x20, 0x08, 0x04, 0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x08, 0x04,
  0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x3e, 0x04, 0xf1, 0x21, 0x00, 0x00,
  0x00, 0x20, 0x08, 0x04, 0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x08, 0x04,
  0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
  0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0xe0, 0xff, 0x07, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x01,
  0x7c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x42, 0x9e, 0x23, 0x06, 0x00, 0x00,
  0x00, 0x00, 0x81, 0xf9, 0x11, 0x08, 0x00, 0x00, 0x00, 0x80, 0x00, 0x07,
  0x1e, 0x10, 0x00, 0x00, 0x00, 0x40, 0xf0, 0x0c, 0xe2, 0x21, 0x00, 0x00,
  0x00, 0x20, 0x0e, 0x10, 0x01, 0xde, 0x00, 0x00, 0x00, 0xf0, 0x01, 0xe0,
  0x00, 0xe0, 0x01, 0x00, 0xe0, 0xff, 0x07, 0xff, 0x3f, 0xf8, 0xff, 0x01,
  0x20, 0x00, 0x04, 0x01, 0x20, 0x08, 0x00, 0x01, 0x20, 0x00, 0x04, 0x01,
  0x20, 0x08, 0x00, 0x01, 0x20, 0x3e, 0x04, 0xf1, 0x21, 0x88, 0x0f, 0x01,
  0x20, 0x64, 0x04, 0x21, 0x22, 0x08, 0x19, 0x01, 0x20, 0x44, 0x04, 0x21,
  0x22, 0x08, 0x11, 0x01, 0x20, 0x64, 0x04, 0x21, 0x22, 0x08, 0x19, 0x01,
  0x20, 0x3c, 0x04, 0x21, 0x22, 0x08, 0x0f, 0x01, 0x20, 0x04, 0x04, 0x21,
  0x22, 0x08, 0x01, 0x01, 0x20, 0x04, 0x04, 0x21, 0x22, 0x08, 0x01, 0x01,
  0x20, 0x1e, 0x04, 0xf1, 0x21, 0x88, 0x07, 0x01, 0x20, 0x00, 0x04, 0x01,
  0x20, 0x08, 0x00, 0x01, 0x20, 0x00, 0x04, 0x01, 0x20, 0x08, 0x00, 0x01,
  0x20, 0x00, 0x04, 0x01, 0x20, 0x08, 0x00, 0x01, 0xe0, 0xff, 0x07, 0xff,
  0x3f, 0xf8, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#endif


InitProcRegistrar::InitProcRegistrar(init_proc_t init_proc) {
  pdpMisc::initHookList()->Add_((void*)init_proc);
}

//////////////////////////
//	pdpMisc		//
//////////////////////////

bool 		pdpMisc::nw_itm_def_arg = false;
PDPRoot* 	pdpMisc::root = NULL;
String_Array	pdpMisc::proj_to_load;
String		pdpMisc::user_spec_def;
float		pdpMisc::pdpZScale = 4.0f;
float		pdpMisc::pts_per_so_unit = 36.0f;
float		pdpMisc::char_pts_per_so_unit = 72.0f;


taPtrList_impl* pdpMisc::initHookList() {
  static taPtrList_impl* p_initHookList = NULL;
  if (p_initHookList == NULL) {
    p_initHookList = new taPtrList_impl();
  }
  return p_initHookList;
}

// startup code common to several scenarios -- all might have local gui
void Startup_MakeMainWin() {
#ifdef TA_GUI
//TODO: need to better orchestrate the "OpenWindows" call below with
  // create the default application window
  MainWindowViewer* db = MainWindowViewer::NewBrowser(tabMisc::root, NULL, true);
#ifndef QANDD_CONSOLE
  ConsoleDockViewer* cdv = new ConsoleDockViewer;
  db->docks.Add(cdv);
#endif    
// create the console docked in the main project window
    
  db->ViewWindow();
  iMainWindowViewer* bw = db->viewerWindow();
  if (bw) {
    // resize to a custom size: 3/4 width, 3/4 height;
    bw->resize((taiM->scrn_s.w * 3)/4, (taiM->scrn_s.h * 3)/4);
    bw->show(); // when we start event loop
  }
//TODO: following prob not necessary
  if (taMisc::gui_active) taiMisc::OpenWindows();
#endif // TA_GUI
}

// startup code common to several scenarios -- all might have local gui
void Startup_InvokeShells() {
  if (cssMisc::gui) {
//TODO: we need event loop in gui AND non-gui, so check about Shell_NoGui_Rl
#ifndef QANDD_CONSOLE
    cssMisc::TopShell->StartupShellInit(cin, cout, cssCmdShell::CT_Qt_Console);
    cssMisc::TopShell->Shell_Qt_Console("pdp++> ");
#else
    // todo: only for debugging: remove later!
    cssMisc::TopShell->StartupShellInit(cin, cout, cssCmdShell::CT_QandD_Console);
    cssMisc::TopShell->Shell_QandD_Console("pdp++> ");
#endif
  } else {
    cssMisc::TopShell->StartupShellInit(cin, cout, cssCmdShell::CT_NoGui_Rl);
    cssMisc::TopShell->Shell_NoGui_Rl("pdp++> ");
  }
  qApp->exec();
}

// code only used for dmem
#ifdef DMEM_COMPILE
static void Startup_dmem() {
  if (taMisc::dmem_nprocs > 1) {
    if (cssMisc::gui) {
      if (taMisc::dmem_proc == 0) { // master dmem
	DMemShare::InitCmdStream();
	// need to have some initial string in the stream, otherwise it goes EOF and is bad!
	*(DMemShare::cmdstream) << "cerr << \"proc no: \" << taMisc::dmem_proc << endl;" << endl;
	taMisc::StartRecording((ostream*)(DMemShare::cmdstream));
	Startup_MakeMainWin();
	Startup_InvokeShells();
	DMemShare::CloseCmdStream();
	cerr << "proc: 0 quitting!" << endl;
      } else { // slave dmems
	cssMisc::gui = false;	// not for subguys
	cssMisc::init_interactive = false; // don't stay in startup shell
//TODO: dmem subs still need to use Qt event loop!!!
// 	if(taMisc::dmem_debug)
// 	  cerr << "proc " << taMisc::dmem_proc << " starting shell." << endl;
	// get rid of wait proc for rl -- we call it ourselves
	extern int (*rl_event_hook)(void);
 	rl_event_hook = NULL;
 	cssMisc::Top->StartupShellInit(cin, cout, cssCmdShell::CT_NoGui_Rl);
	//	cssMisc::Top->debug = 2;
	DMem_SubEventLoop();	// this is the "shell" for a dmem sub guy
	cerr << "proc: " << taMisc::dmem_proc << " quitting!" << endl;
      }
    } else {			// nogui
      Startup_InvokeShells();  
    }
  } else { // dmem, but only one guy, so pretty much like normal
    Startup_MakeMainWin();
    Startup_InvokeShells();
  }
}
#endif // DMEM_COMPILE
// this is the main that should be called..

/* NOTES
  Project autoloads were removed, because ALL versions of pdp must now
  have an event loop, so all can load in the event loop. BA 9/21/06

*/
int pdpMisc::Main(int argc, char *argv[]) {
// misc todos:
//TODO: should accept - or -- for switches

/* following creates QApplication and event loop -- these
   must get created first, and are needed even if we don't open a gui
   Set cssMisc::gui, according to command line switch and compilation mode (TA_GUI/NO_GUI)
*/
  //TODO: need a coherent way to establish path names etc.
  String app_root = "pdp4.0";
  
  cssMisc::PreInitialize(argc, argv);

#ifdef DMEM_COMPILE
  MPI_Init(&argc, &argv); // note mpi's extra level of indirection
  taMisc::DMem_Initialize();
#endif

  // check for special case of a project as first arg (no switch needed)
  String tmp;
  int idx_start = 1; // default start for searching for switches
  if (argc >= 2) {
    String tmp = argv[1];
    if (tmp.contains(".proj")) {
      idx_start = 2;
      proj_to_load.Add(tmp);
    }
  }
  // check for projects to load
  int idx = idx_start;
  while (idx < argc) {
    if (!cssMisc::CmdLineSwitchValue("-p", idx, tmp)) break;
    proj_to_load.Add(tmp);
  }
  
  // check for defaults
  String user_spec_def;
  idx = idx_start;
  cssMisc::CmdLineSwitchValue("-d", idx, user_spec_def, true);

  // initialize type system for us, followed by the various clients, ex. bp, leabra, etc.
  ta_Init_pdp();

  // load plugin system
  // TODO: Check if directory exists and allow for /usr/local/pdp++/plugins
  const QString& localdir = String(QDir::homeDirPath()).cat("/").cat(app_root).cat("/plugins") ;


  taPlugins::AddPluginFolder(localdir);
  taPlugins::LoadPlugins();

  taPtrList_impl* ihl = initHookList();
  for (int i = 0; i < ihl->size; ++i) {
    init_proc_t ip = (init_proc_t)ihl->FastEl_(i);
    ip();
  }

  ((taMisc*)TA_taMisc.GetInstance())->LoadConfig();
  // need this config to get mswin_scale (in taiMisc::Initialize) before opening root window.

  root = new PDPRoot();
  taBase::Ref(root); // ref=1
  root->InitLinks();	// normally the owner would do this, but..

  // tabMisc stuff
  tabMisc::root = root;
  taMisc::default_scope = &TA_ProjectBase;

  // cssMisc stuff
  cssMisc::HardVars.Push(cssBI::root = new cssTA_Base(root, 1, &TA_PDPRoot,"root"));
  cssMisc::Initialize();
  cssMisc::Top->name = app_root;	// changes prompt

#ifdef TA_GUI
  if(cssMisc::gui && (taMisc::dmem_proc == 0)) {
    taiM_ = taiMisc::New(cssMisc::gui);
    taiMC_ = taiM_;
    taiM->icon_bitmap = new QBitmap(pdp_bitmap_width,
    	pdp_bitmap_height, pdp_bitmap_bits);
//    qApp->setWindowIcon(QIcon(*(taiM->icon_bitmap)));
  } else
#endif // TA_GUI
  { 
    taiMC_ = taiMiscCore::New();
  }
  // create colorspecs even if nogui, since they are referred to in projects
  root->colorspecs.SetDefaultColor();	// set color after starting up..

   //always use our wait proc, since there is a predefined chain backwards anyways...
  taMisc::WaitProc = pdpMisc::WaitProc;

#if ((!defined(DMEM_COMPILE)) && (!defined(TA_OS_WIN))) 
  taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) SaveRecoverFile);
#endif

//TODO: these need to be fixed!!!!!
#if (defined(TA_OS_WIN))
  String pdp_dir("C:/").cat(app_root); // default pdp home directory
#else
//TODO: this is ALL HORRIBLE!!!!!! MUST FIX!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#ifdef DEBUG
  String pdp_dir = String("~/").cat(app_root).cat("/trunk"); // default pdp home directory
#else
  String pdp_dir = String("/usr/local/").cat(app_root); // default pdp home directory
#endif
#endif
  char* pdp_dir_env = getenv("PDP4DIR");
  if(pdp_dir_env != NULL)
    pdp_dir = pdp_dir_env;

  String home_dir;
  char* home_dir_env = getenv("HOME");
  if(home_dir_env != NULL)
    home_dir = home_dir_env;

  taMisc::include_paths.AddUnique(pdp_dir);
  taMisc::include_paths.AddUnique(pdp_dir+"/css/include");
  taMisc::include_paths.AddUnique(pdp_dir+"/defaults");
//TODO: NO!! can't have these during devel
/*  if(!home_dir.empty()) {
    taMisc::include_paths.AddUnique(home_dir+"/mypdp++");
    taMisc::include_paths.AddUnique(home_dir+"/pdp++");
    taMisc::include_paths.AddUnique(home_dir+"/mypdp++/defaults");
    taMisc::include_paths.AddUnique(home_dir+"/pdp++/defaults");
  } */

  String prognm = argv[0];
//TODO: this should just print the version and quit (so should be moved way earlier, before any init
  if (cssMisc::HasCmdLineSwitch("-version")) {
    root->Info();
  }

  // Initialize plugin system
  taPlugins::InitPlugins();

  root->LoadConfig();

  cssMisc::TopShell->cmd_prog->CompileRunClear(".pdpinitrc");

#ifdef DMEM_COMPILE
  Startup_dmem();
#else // NOT DMEM_COMPILE
  Startup_MakeMainWin();
  Startup_InvokeShells();
#endif // DMEM_COMPILE

  tabMisc::DeleteRoot();
  taMisc::types.RemoveAll();	// get rid of all the types before global dtor!

#ifdef TA_USE_INVENTOR
  SoQt::done();
#endif
#ifdef DMEM_COMPILE
  MPI_Finalize();
#endif // DMEM_COMPILE
  
  return 0;
}

#ifdef TA_GUI
SelectEdit* pdpMisc::FindSelectEdit(ProjectBase* prj) {
  return (SelectEdit*)prj->edits.DefaultEl();
}

SelectEdit* pdpMisc::FindMakeSelectEdit(ProjectBase* prj) {
  SelectEdit* rval = (SelectEdit*)prj->edits.DefaultEl();
  if(rval != NULL) return rval;
  rval = (SelectEdit*)prj->edits.New(1, &TA_SelectEdit);
  return rval;
}
#endif

const iColor* pdpMisc::GetObjColor(ProjectBase* proj, ViewColors vc) {
  if (proj == NULL) return NULL;
  return proj->GetObjColor((ProjectBase::ViewColors)vc);
}

const iColor* pdpMisc::GetObjColor(ProjectBase* proj, TypeDef* td) {
  if((proj == NULL) || (td == NULL)) return NULL;
  return proj->GetObjColor(td);
}

ColorScaleSpec* pdpMisc::GetDefaultColor() {
  if((root == NULL) || !taMisc::gui_active)	return NULL;
  return (ColorScaleSpec*)root->colorspecs.DefaultEl();
}

int get_unique_file_number(int st_no, const char* prefix, const char* suffix) {
  String prfx = prefix;
  String fname;
  int i;
  for(i=st_no; i<10000; i++) {	// stop at 10,000
    fname = prfx + String(i) + suffix;
    int acc = access(fname, R_OK);
    if(acc != 0)
      break;			// its ok..
  }
  fstream strm;
  strm.open(fname, ios::out);	// this should hold the place for the file
  strm.close();	strm.clear();		// while it is being saved, etc..
  return i;
}

void pdpMisc::WaitProc_LoadProj() {
  taFiler* gf = NULL;
  if(taMisc::gui_active) {
//obs    root->GetFileDlg();
//    gf = root->projects.ta_file;
  }
  if (gf == NULL) {
    gf = taFiler::New();
  }
  taRefN::Ref(gf); //refs regardless if our own, or external

  taMisc::Busy();
  int i;
  for(i=0; i<proj_to_load.size; i++) {
    String fnm = proj_to_load[i];
    gf->fname = fnm;
    istream* strm = gf->open_read();
    if((strm == NULL) || !(gf->open_file))
      taMisc::Error("ProjectBase Load: could not open file", fnm);
    else {
//obs      taiM->CreateLoadDialog();
      root->projects.Load(*strm);
      if(root->projects.size > i)
	((ProjectBase*)root->projects[i])->file_name = fnm;
//obs      taiM->RemoveLoadDialog();
    }
    gf->Close();
  }
  taRefN::unRefDone(gf);
  proj_to_load.Reset();
  taMisc::DoneBusy();
}

void pdpMisc::WaitProc_PostLoadOpr() {
  int i;
  for (i=0; i< tabMisc::post_load_opr.size; i++) {
    TAPtr obj = tabMisc::post_load_opr[i];
//     if(obj->InheritsFrom(TA_Script_Group)) {
//       ((Script_Group*)obj)->AutoRun();
//     }
    if(obj->InheritsFrom(TA_SelectEdit_Group)) {
      ((SelectEdit_Group*)obj)->AutoEdit();
    }
    else if(obj->InheritsFrom(TA_Wizard_Group)) {
      ((Wizard_Group*)obj)->AutoEdit();
    }
  }
  tabMisc::post_load_opr.Reset();
}

void pdpMisc::WaitProc() {
  if(proj_to_load.size > 0) {
    WaitProc_LoadProj();
  }
  else if(tabMisc::post_load_opr.size > 0) {
    WaitProc_PostLoadOpr();
  }

#ifdef DMEM_COMPILE
  if((taMisc::dmem_nprocs > 1) && (taMisc::dmem_proc == 0)) {
    DMem_WaitProc();
  }
#endif

#ifdef TA_GUI
  if(taMisc::gui_active) {
    taiMisc::OpenWindows();
  }
  taiMisc::WaitProc();
#else
  taiMiscCore::WaitProc();
#endif
}

Network* pdpMisc::GetNewNetwork(ProjectBase* prj, TypeDef* typ) {
  if(prj == NULL) return NULL;
  Network* rval = (Network*)prj->networks.New(1, typ);
#ifdef TA_GUI
  taiMisc::RunPending();
#endif
  return rval;
}

Network* pdpMisc::GetDefNetwork(ProjectBase* prj) {
  if(prj == NULL) return NULL;
  return (Network*)prj->networks.DefaultEl();
}




#ifdef DMEM_COMPILE

static cssProgSpace* dmem_space1 = NULL;
static cssProgSpace* dmem_space2 = NULL;

void pdpMisc::DMem_WaitProc(bool send_stop_to_subs) {
  if(dmem_space1 == NULL) dmem_space1 = new cssProgSpace;
  if(dmem_space2 == NULL) dmem_space2 = new cssProgSpace;

  if(DMemShare::cmdstream->bad() || DMemShare::cmdstream->eof()) {
    taMisc::Error("DMem: Error! cmstream is bad or eof.",
		  "Software will not respond to any commands, must quit!!");
  }
  while(DMemShare::cmdstream->tellp() > DMemShare::cmdstream->tellg()) {
    DMemShare::cmdstream->seekg(0, ios::beg);
    string str = DMemShare::cmdstream->str();
    String cmdstr = str.c_str();
    cmdstr = cmdstr.before((int)(DMemShare::cmdstream->tellp() - DMemShare::cmdstream->tellg()));
    // make sure to only get the part that is current -- other junk might be in there.
    cmdstr += '\n';
    if(taMisc::dmem_debug) {
      cerr << "proc 0 sending cmd: " << cmdstr;
    }
    DMemShare::cmdstream->seekp(0, ios::beg);

    int cmdlen = cmdstr.length();

    DMEM_MPICALL(MPI_Bcast((void*)&cmdlen, 1, MPI_INT, 0, MPI_COMM_WORLD),
		 "Proc 0 WaitProc", "MPI_Bcast - cmdlen");

    DMEM_MPICALL(MPI_Bcast((void*)(const char*)cmdstr, cmdlen, MPI_CHAR, 0, MPI_COMM_WORLD),
		 "Proc 0 WaitProc", "MPI_Bcast - cmd");

    if(taMisc::dmem_debug) {
      cerr << "proc 0 running cmd: " << cmdstr << endl;
    }
    // now run the command: it wasn't run before!
    cssProgSpace* sp = dmem_space1; // if first space is currently running, use another
    if(sp->state & (cssProg::State_Run | cssProg::State_Cont)) {
      if(taMisc::dmem_debug)
	cerr << "proc 0 using 2nd space!" << endl;
      sp = dmem_space2;
    }

    sp->CompileCode(cmdstr);
    sp->Run();
    sp->ClearAll();
  }
  if(send_stop_to_subs) {
    String cmdstr = "stop";
    int cmdlen = cmdstr.length();
    DMEM_MPICALL(MPI_Bcast((void*)&cmdlen, 1, MPI_INT, 0, MPI_COMM_WORLD),
		 "Proc 0 WaitProc, SendStop", "MPI_Bcast - cmdlen");
    DMEM_MPICALL(MPI_Bcast((void*)(const char*)cmdstr, cmdlen, MPI_CHAR, 0, MPI_COMM_WORLD),
		 "Proc 0 WaitProc, SendStop", "MPI_Bcast - cmdstr");
  }
}

int pdpMisc::DMem_SubEventLoop() {
  if(taMisc::dmem_debug) {
    cerr << "proc: " << taMisc::dmem_proc << " event loop start" << endl;
  }

  if(dmem_space1 == NULL) dmem_space1 = new cssProgSpace;
  if(dmem_space2 == NULL) dmem_space2 = new cssProgSpace;

  while(true) {
    int cmdlen;
    DMEM_MPICALL(MPI_Bcast((void*)&cmdlen, 1, MPI_INT, 0, MPI_COMM_WORLD),
		 "Proc n SubEventLoop", "MPI_Bcast");
    char* recv_buf = new char[cmdlen+2];
    DMEM_MPICALL(MPI_Bcast(recv_buf, cmdlen, MPI_CHAR, 0, MPI_COMM_WORLD),
		 "Proc n SubEventLoop", "MPI_Bcast");
    recv_buf[cmdlen] = '\0';
    String cmd = recv_buf;
    delete recv_buf;

    if(cmd.length() > 0) {
      if(taMisc::dmem_debug) {
       cerr << "proc " << taMisc::dmem_proc << " recv cmd: " << cmd << endl << endl;
      }
      if(cmd == "stop") {
	if(taMisc::dmem_debug)
	  cerr << "proc " << taMisc::dmem_proc << " got stop command, stopping out of sub event processing loop." << endl;
	return 1;
      }
      else if(!cmd.contains("Save(") && !cmd.contains("SaveAs(")) {
	if(taMisc::dmem_debug) {
	  cerr << "proc " << taMisc::dmem_proc << " running cmd: " << cmd << endl;
	}

	cssProgSpace* sp = dmem_space1; // if first space is currenntly running, use another
	if(sp->state & (cssProg::State_Run | cssProg::State_Cont)) {
	  if(taMisc::dmem_debug)
	    cerr << "proc " << taMisc::dmem_proc << " using 2nd space!" << endl;
	  sp = dmem_space2;
	}

	sp->CompileCode(cmd);
	sp->Run();
	sp->ClearAll();

	if(cmd.contains("Quit()")) {
	  if(taMisc::dmem_debug)
	    cerr << "proc " << taMisc::dmem_proc << " got quit command, quitting." << endl;
	  return 1;
	}
      }
    }
    else {
      cerr << "proc " << taMisc::dmem_proc << " received null command!" << endl;
    }
    // do basic wait proc here..
    tabMisc::WaitProc();
  }
  return 0;
}

#endif // DMEM

