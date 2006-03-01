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



#include "pdpbase.h"

#include "ta_type.h"
#include "ta_filer.h"
#include "ta_css.h"
#include "css_builtin.h"

#include "datatable.h"

//obs #include "process.h"
#include "pdplog.h"
#include "pdpshell.h"

#ifdef TA_GUI
#include "ta_qt.h"
#include "css_qt.h"
#include "ta_qtdata.h" // for taiObjChooser
#include "ta_qtviewer.h"
#include "ta_qtbrowse.h"
#include <qbitmap.h>
#endif

#ifdef TA_USE_INVENTOR
  #include <Inventor/Qt/SoQt.h>
#endif


#ifdef DMEM_COMPILE
#include <mpi.h>
#endif

#include <signal.h>
#include <memory.h>
//obs #include <malloc.h>
//nn?? #include <unistd.h>
//#include <sstream>
#include <time.h>


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
char* 		pdpMisc::defaults_str = NULL;
String_Array	pdpMisc::proj_to_load;
taBase_List	pdpMisc::post_load_opr;

float		pdpMisc::pdpZScale = 4.0f;
float		pdpMisc::pts_per_so_unit = 36.0f;
float		pdpMisc::char_pts_per_so_unit = 72.0f;

//void (*pdpMisc::Init_Hook)() = NULL;

taPtrList_impl* pdpMisc::initHookList() {
  static taPtrList_impl* p_initHookList = NULL;
  if (p_initHookList == NULL) {
    p_initHookList = new taPtrList_impl();
  }
  return p_initHookList;
}

// this is the main that should be called..

int pdpMisc::Main(int argc, char *argv[]) {
//<TEMP>
  //fix for unparented DT_ViewSpec
#ifdef TA_GUI
//NOTE: not showing up in nogui compiles, but that could possibly change if it
//starts getting referenced
  if (TA_DT_ViewSpec_ptr.parents.size == 0)
    TA_DT_ViewSpec_ptr.AddParents(&TA_DT_ViewSpec);
#endif
//</TEMP>

/* following creates QApplication and event loop -- these
   must get created first, and are needed even if we don't open a gui
   Set cssMisc::gui, according to command line switch and compilation mode (TA_GUI/NO_GUI)
*/
  cssMisc::PreInitialize(argc, argv);

#ifdef DMEM_COMPILE
  MPI_Init(&argc, &argv); // note mpi's extra level of indirection
  taMisc::DMem_Initialize();
#endif

  String user_spec_def;
  for (int ac = 1; ac < argc; ac++) {
    String tmp = argv[ac];
    // project is either flagged with a -p flag or the first or second argument
    if((tmp == "-p") || ((ac <= 2) && tmp.contains(".proj"))) {
      if(tmp == "-p") {
	if(argc > ac+1) {
	  proj_to_load.Add(argv[ac+1]);
	  ac++;			// skip over it so as to not load 2x
	}
      }
      else
	proj_to_load.Add(tmp);
    }
    if((tmp == "-d") || (tmp == "-def")) {
      if(argc > ac+1)
	user_spec_def = argv[ac+1];
    }
  }

/*obs  if (Init_Hook != NULL)
    (*Init_Hook)();	// call the user's init function (which will call pdp)
  else
    ta_Init_pdp();		// always has to be first */
  // initialize type system for us, followed by the various clients, ex. bp, leabra, etc.
  ta_Init_pdp();
  taPtrList_impl* ihl = initHookList();
  for (int i = 0; i < ihl->size; ++i) {
    init_proc_t ip = (init_proc_t)ihl->FastEl_(i);
    ip();
  }

/*obs  if (cssMisc::gui && (taMisc::dmem_proc == 0))
    new ivSession("PDP++", argc, argv, PDP_options, PDP_defs); */

  root = new PDPRoot();
  taBase::Ref(root); // ref=1
  root->InitLinks();	// normally the owner would do this, but..
  root->SetName("root");

  // tabMisc stuff
  tabMisc::root = (TAPtr)root;
  taMisc::app = (IApp*)root;
  taMisc::default_scope = &TA_Project;

  // cssMisc stuff
  cssMisc::HardVars.Push(cssBI::root = new cssTA_Base(root, 1, &TA_PDPRoot,"root"));
  cssMisc::Initialize();

  ((taMisc*)TA_taMisc.GetInstance())->LoadConfig();
  // need this config to get mswin_scale (in taiMisc::Initialize) before opening root window.

#ifdef TA_GUI
  if(cssMisc::gui && (taMisc::dmem_proc == 0)) {
    taiMisc::Initialize(cssMisc::gui, "PDP++");
    taiM->icon_bitmap = new QBitmap(pdp_bitmap_width,
    	pdp_bitmap_height, pdp_bitmap_bits);

/*    root->WinInit();
    root->OpenNewWindow();
    taiMisc::SetMainWindow(root->window); */
    //NOTE: we root the browser at the projects, to minimize unnecessary browsing levels
    // root.colorspecs is aliased in root->colorspecs, as a HIDDEN linked variable
    DataBrowser* db = DataBrowser::New(&root->projects, root->GetTypeDef()->members.FindName("projects"),
      root->projects.GetTypeDef(), true);
    db->InitLinks(); // no one else to do it!
    db->ViewWindow();
//nn    bw->browser->setRoot(root, root->GetTypeDef());
    taiMisc::SetMainWindow(db->browser_win());

    // set the update action (taken after Ok or Apply in the Edit dialog)
//temp    taiMisc::Update_Hook = taMisc::DelayedMenuUpdate;
//obs    winbMisc::group_leader = root->window;
//temp    taMisc::DelayedMenuUpdate(root); 	// get menus after startup..
  }
#endif //TA_GUI
  // create colorspecs even if nogui, since they are referred to in projects
  root->colorspecs.SetDefaultColor();	// set color after starting up..

//3.2a:  if(cssiSession::WaitProc == NULL)
   //always use our wait proc, since there is a predefined chain backwards anyways...
#ifdef TA_GUI
   cssiSession::WaitProc = pdpMisc::WaitProc;
#endif

#if ((!defined(DMEM_COMPILE)) && (!defined(TA_OS_WIN))) 
  taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) SaveRecoverFile);
#endif

#if (defined(TA_OS_WIN))
  String pdp_dir = "C:/PDP++"; // default pdp home directory
#else
  String pdp_dir = "/usr/local/pdp++"; // default pdp home directory
#endif
  char* pdp_dir_env = getenv("PDPDIR");
  if(pdp_dir_env != NULL)
    pdp_dir = pdp_dir_env;

  String home_dir;
  char* home_dir_env = getenv("HOME");
  if(home_dir_env != NULL)
    home_dir = home_dir_env;

  taMisc::include_paths.AddUnique(pdp_dir);
  taMisc::include_paths.AddUnique(pdp_dir+"/css/include");
  taMisc::include_paths.AddUnique(pdp_dir+"/defaults");
  if(!home_dir.empty()) {
    taMisc::include_paths.AddUnique(home_dir+"/mypdp++");
    taMisc::include_paths.AddUnique(home_dir+"/pdp++");
    taMisc::include_paths.AddUnique(home_dir+"/mypdp++/defaults");
    taMisc::include_paths.AddUnique(home_dir+"/pdp++/defaults");
  }

  String prognm = argv[0];
  if(prognm.contains('/'))
    prognm = prognm.after('/',-1);
  if(prognm.contains("++"))	// the distribution version of an executable
    prognm = prognm.before("++");
  if(prognm.contains('.'))	// some kind of extention
    prognm = prognm.before('.', -1);

  // set the defaults to be the given executable's name
  if((prognm == "pdpshell") || (prognm == "pdp") || (prognm == "bp"))
    root->default_file = "bp.def";
  else
    root->default_file = prognm + ".def";

  if(!user_spec_def.empty()) {
    root->default_file = user_spec_def;
    if(!root->default_file.contains(".def"))
      root->default_file += ".def"; // justin case
  }

  String vers = "-version";
  for (int i=1; i<=argc; i++) {
    //if(vers == static_cast<const char*>(argv[i])) {
    if(vers == argv[i]) {
	root->Info();
      break;
    }
  }

  root->LoadConfig();

  cssMisc::Top->CompileRunClear(".pdpinitrc");

  if((proj_to_load.size > 0) && !cssMisc::gui)
    pdpMisc::WaitProc_LoadProj();	// load file manually, since it won't go thru waitproc
#ifdef TA_GUI
  if (taiMisc::gui_active) taiMisc::OpenWindows();
#endif

#ifdef DMEM_COMPILE
  if(taMisc::dmem_nprocs > 1) {
    if (cssMisc::gui) {
      if(taMisc::dmem_proc == 0) {
	DMemShare::InitCmdStream();
	// need to have some initial string in the stream, otherwise it goes EOF and is bad!
	*(DMemShare::cmdstream) << "cerr << \"proc no: \" << taMisc::dmem_proc << endl;" << endl;
	taMisc::StartRecording((ostream*)(DMemShare::cmdstream));
	cssMisc::Top->StartupShell(cin, cout);
	DMemShare::CloseCmdStream();
	cerr << "proc: 0 quitting!" << endl;
      }
      else {
	cssMisc::gui = false;	// not for subguys
	if(proj_to_load.size > 0) {
	  if(taMisc::dmem_debug)
	    cerr << "proc " << taMisc::dmem_proc << " loading projects." << endl;
	  pdpMisc::WaitProc_LoadProj();	// load file manually, since it won't go thru waitproc
	}

	cssMisc::init_interactive = false; // don't stay in startup shell
// 	if(taMisc::dmem_debug)
// 	  cerr << "proc " << taMisc::dmem_proc << " starting shell." << endl;
	// get rid of wait proc for rl -- we call it ourselves
	extern int (*rl_event_hook)(void);
 	rl_event_hook = NULL;
 	cssMisc::Top->StartupShell(cin, cout);
	//	cssMisc::Top->debug = 2;
	DMem_SubEventLoop();
	cerr << "proc: " << taMisc::dmem_proc << " quitting!" << endl;
      }
    }
    else {
      cssMisc::Top->StartupShell(cin, cout);
    }
  }
  else {
    cssMisc::Top->StartupShell(cin, cout);
#ifdef TA_USE_INVENTOR
    SoQt::done();
#endif
  }
  MPI_Finalize();

#else
  cssMisc::Top->StartupShell(cin, cout);
#ifdef TA_USE_INVENTOR
  SoQt::done();
#endif
#endif

  //note: new 4.0 behavior is for root deletion to be the app end, so following is probably redundant
  if (tabMisc::root) {
    delete tabMisc::root;
    tabMisc::root = NULL;
  }
#ifdef TA_GUI
  taiMisc::RunPending(); // do Qt defered deletes, if any
#endif
  taMisc::types.RemoveAll();	// get rid of all the types before global dtor!
  return 0;
}

BaseSpec_MGroup* pdpMisc::FindMakeSpecGp(ProjectBase* prj, const char* nm, bool& nw_itm) {
  return prj->FindMakeSpecGp(nm, nw_itm);
}

BaseSpec* pdpMisc::FindMakeSpec(ProjectBase* prj, const char* nm, TypeDef* td, bool& nw_itm) {
  return (BaseSpec*)prj->specs.FindMakeSpec(nm, td, nw_itm);
}

BaseSpec* pdpMisc::FindSpecName(ProjectBase* prj, const char* nm) {
  BaseSpec* rval = (BaseSpec*)prj->specs.FindSpecName(nm);
  if(rval == NULL) {
    taMisc::Error("Error: could not find spec named:", nm);
  }
  return rval;
}

BaseSpec* pdpMisc::FindSpecType(ProjectBase* prj, TypeDef* td) {
  BaseSpec* rval = (BaseSpec*)prj->specs.FindSpecType(td);
  if(rval == NULL) {
    taMisc::Error("Error: could not find spec of type:", td->name);
  }
  return rval;
}

PDPLog* pdpMisc::FindMakeLog(ProjectBase* prj, const char* nm, TypeDef* td, bool& nw_itm) {
  return (PDPLog*)prj->logs.FindMakeLog(nm, td, nw_itm);
}

PDPLog* pdpMisc::FindLogName(ProjectBase* prj, const char* nm) {
  PDPLog* rval = (PDPLog*)prj->logs.Find(nm);
  if(rval == NULL) {
    taMisc::Error("Error: could not find log named:", nm);
  }
  return rval;
}

PDPLog* pdpMisc::FindLogType(ProjectBase* prj, TypeDef* td) {
  PDPLog* rval = (PDPLog*)prj->logs.Find(td);
  if(rval == NULL) {
    taMisc::Error("Error: could not find log of type:", td->name);
  }
  return rval;
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

int pdpMisc::WaitProc_LoadProj() {
  taFiler* gf = NULL;
  if(taMisc::gui_active) {
//obs    root->GetFileDlg();
//    gf = root->projects.ta_file;
  }
  if (gf == NULL) {
    gf = taFiler_CreateInstance();
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
    if(taMisc::gui_active) {
      taMisc::DelayedMenuUpdate(&(root->projects));
    }
  }
  taRefN::unRefDone(gf);
  proj_to_load.Reset();
  taMisc::DoneBusy();
  return 0;
}

int pdpMisc::WaitProc_PostLoadOpr() {
  int i;
  for(i=0;i<post_load_opr.size;i++) {
    TAPtr obj = post_load_opr[i];
    if(obj->InheritsFrom(TA_Script_MGroup)) {
      ((Script_MGroup*)obj)->AutoRun();
    }
#ifdef TA_GUI
    else if(obj->InheritsFrom(TA_SelectEdit_MGroup)) {
      ((SelectEdit_MGroup*)obj)->AutoEdit();
    }
#endif
    else if(obj->InheritsFrom(TA_Wizard_MGroup)) {
      ((Wizard_MGroup*)obj)->AutoEdit();
    }
  }
  post_load_opr.Reset();
  return 0;
}

int pdpMisc::WaitProc() {
  if(proj_to_load.size > 0) {
    WaitProc_LoadProj();
  }
  else if(post_load_opr.size > 0) {
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
    return taiMisc::WaitProc();
  }
  else {
    return tabMisc::WaitProc();
  }
#else
  return 0; // TODO: check that this is a correct value
#endif
}

PDPLog* pdpMisc::GetNewLog(ProjectBase* prj, TypeDef* typ) {
  if((prj == NULL) || (typ == NULL)) return NULL;
  PDPLog* rval = (PDPLog*)prj->logs.New(1, typ);
#ifdef TA_GUI
  taiMisc::RunPending();
  taMisc::DelayedMenuUpdate(prj);
#endif
  return rval;
}

NetConduit* pdpMisc::GetNewConduit(ProjectBase* prj, TypeDef* typ) {
  if(prj == NULL) return NULL;
  NetConduit* rval = NULL;
  if (typ->InheritsFrom(TA_NetWriter))
    rval = (NetConduit*)prj->net_writers.New(1, typ);
  else if (typ->InheritsFrom(TA_NetReader))
    rval = (NetConduit*)prj->net_readers.New(1, typ);
#ifdef TA_GUI
  taiMisc::RunPending();
  taMisc::DelayedMenuUpdate(prj);
#endif
  return rval;
}

Network* pdpMisc::GetNewNetwork(ProjectBase* prj, TypeDef* typ) {
  if(prj == NULL) return NULL;
  Network* rval = (Network*)prj->networks.New(1, typ);
#ifdef TA_GUI
  taiMisc::RunPending();
  taMisc::DelayedMenuUpdate(prj);
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

int pdpMisc::DMem_WaitProc(bool send_stop_to_subs) {
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
  return 0;
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

