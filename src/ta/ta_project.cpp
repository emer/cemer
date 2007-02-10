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

#include "ta_project.h"
#include "ta_platform.h"
#include "ta_dump.h"
#include "ta_plugin.h"

#include "css_ta.h"
#include "css_console.h"

#include <QCoreApplication>
#include <QFileInfo>

#ifdef TA_GUI
# include "ta_qt.h"
# include "ta_qtdata.h" // for taiObjChooser
# include "ta_qtdialog.h"
# include "ta_qttype_def.h"
# include "colorscale.h"
# include "css_qt.h"
# ifdef HAVE_QT_CONSOLE
#   include "css_qtconsole.h"
# endif
# include <QApplication>
# include <QWidgetList>
#endif

#include <time.h>
#include <locale.h>

#ifdef DMEM_COMPILE
#include "ta_dmem.h"
#endif

#ifdef TA_USE_INVENTOR
  #include <Inventor/Qt/SoQt.h>
#endif

#ifdef GPROF			// turn on for profiling
extern "C" void moncontrol(int mode);
#endif

//////////////////////////////////
//	SelectEdit_Group	//
//////////////////////////////////

void SelectEdit_Group::AutoEdit() {
  taLeafItr i;
  SelectEdit* se;
  FOR_ITR_EL(SelectEdit, se, this->, i) {
    if(se->config.auto_edit)
      se->Edit();
  }
}


//////////////////////////
//   taWizard		//
//////////////////////////

void taWizard::Initialize() {
  auto_open = false;
}

void taWizard::InitLinks() {
  inherited::InitLinks();
}

void taWizard::CutLinks() {
  inherited::CutLinks();
}

//////////////////////////////////
// 	Wizard_Group		//
//////////////////////////////////

void Wizard_Group::AutoEdit() {
  taWizard* wz;
  taLeafItr i;
  FOR_ITR_EL(taWizard, wz, this->, i) {
    if (wz->auto_open)
      wz->Edit();
  }
}

//////////////////////////
//  taProject		//
//////////////////////////

#ifdef TA_GUI
class SimLogEditDialog: public taiEditDataHost {
public:
  bool	ShowMember(MemberDef* md) {
    // just show a small subset of the members
    bool rval = (md->ShowMember(show) && (md->im != NULL));
    if (!rval) return rval;
// note: we also include a couple of members we know are in taProject
    if (!(md->name.contains("desc") || (md->name == "use_sim_log") || (md->name == "save_rmv_units")
	 || (md->name == "file_name"))) return false;
    return true;
  }
  override void	Constr_Methods_impl() { }	// suppress methods

  SimLogEditDialog(void* base, TypeDef* tp, bool read_only_,
  	bool modal_) : taiEditDataHost(base, tp, read_only_, modal_) { };
};
#endif

void taProject::Initialize() {
  m_dirty = false;
  use_sim_log = true;
  viewers.SetBaseType(&TA_TopLevelViewer);
}

void taProject::InitLinks() {
  //note: this routine is private, so subclasses must use _impl or _post
  inherited::InitLinks();
  InitLinks_impl();
  InitLinks_post();
}

void taProject::InitLinks_impl() {
  taBase::Own(templates, this);
  taBase::Own(wizards, this);
  taBase::Own(edits, this);
  taBase::Own(data, this);
  taBase::Own(programs, this);
  taBase::Own(viewspecs, this);
  taBase::Own(viewers, this);
}

void taProject::InitLinks_post() {
  if (!taMisc::is_loading) {
    AssertDefaultProjectBrowser(true);
    AssertDefaultWiz(true);	// make default and edit it
  } else {
    AssertDefaultProjectBrowser(false);
    AssertDefaultWiz(false);	// make default and don't edit it
  }
}

void taProject::CutLinks() {
  CutLinks_impl();
  inherited::CutLinks();
}

void taProject::CutLinks_impl() {
  viewers.CutLinks(); 
  viewspecs.CutLinks();
  programs.CutLinks();
  data.CutLinks();
  edits.CutLinks();
  wizards.CutLinks();
  templates.CutLinks();
}

void taProject::Copy_(const taProject& cp) {
  // delete things first, to avoid dangling references
  programs.Reset();
  viewers.Reset();
  viewspecs.Reset();
  data.Reset();
  edits.Reset();
  
  templates = cp.templates;
  wizards = cp.wizards;
  edits = cp.edits;
  data = cp.data;
  viewspecs = cp.viewspecs;
  viewers = cp.viewers;
  programs = cp.programs;
  // NOTE: once a derived project has all the relevant stuff copied, it needs to call this:
  // UpdatePointers_NewPar(&cp, this); // update pointers within entire project..
  setDirty(true);
}

void taProject::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
//   if(taMisc::is_loading) {
//     PostLoadAutos();
//   }
}


MainWindowViewer* taProject::GetDefaultProjectBrowser() {
  // try official default first
  MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.DefaultEl()); 
  if (vwr) return vwr;
  // otherwise iterate
  for (int i = 0; i < viewers.size; ++i) {
    vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
  }
  return NULL;
}

void taProject::PostLoadAutos() {
  // todo: have option or make default to open a panel instead of dialog
  if (taMisc::gui_active) {
    AssertDefaultProjectBrowser(true);
    wizards.AutoEdit();
    edits.AutoEdit();
//     tabMisc::post_load_opr.Link(&wizards);
    //       pdpMisc::post_load_opr.Link(&programs);
//     tabMisc::post_load_opr.Link(&edits);
  }
}

void taProject::AssertDefaultProjectBrowser(bool auto_open) {
  MainWindowViewer* vwr = NULL;
  // get the default one, if there is one unopened
  vwr = GetDefaultProjectBrowser();
  if (!vwr) {
    vwr = MakeProjectBrowser_impl();
    vwr->SetName("DefaultProjectBrowser");
  }
  else {
    vwr->SetData(this);
  }
  if (auto_open) {
    vwr->ViewWindow();
  }
}

MainWindowViewer* taProject::MakeProjectBrowser_impl() {
  MainWindowViewer* vwr =  MainWindowViewer::NewProjectBrowser(this); // added to viewers
  return vwr;
}

MainWindowViewer* taProject::NewProjectBrowser() {
  MainWindowViewer* vwr = NULL;
  // get the default one, if there is one unopened
  vwr = GetDefaultProjectBrowser();
  if (!vwr || vwr->isMapped()) {
    vwr = MakeProjectBrowser_impl();
  }
  return vwr;
}

void taProject::OpenNewProjectBrowser(String viewer_name) {
  MainWindowViewer* vwr =  MakeProjectBrowser_impl();
  if (viewer_name.nonempty())
    vwr->SetName(viewer_name);
  vwr->ViewWindow();
  
}

DataTable* taProject::GetNewAnalysisDataTable(const String& nw_nm, bool msg) {
  DataTable_Group* dgp = (DataTable_Group*)data.FindMakeGpName("AnalysisData");
  DataTable* rval = dgp->NewEl(1, &TA_DataTable);
  if(!nw_nm.empty())
    rval->name = nw_nm;
  if(msg)
    taMisc::Warning("Note: created new data table named:", rval->name, "in .data.AnalysisData");
  return rval;
}

bool taProject::SetFileName(const String& val) {
  if (GetFileName() == val) return true;
  inherited::SetFileName(val);
  MainWindowViewer* vwr = GetDefaultProjectBrowser();
  if(vwr) {
    vwr->SetWinName();
  }
  tabMisc::root->AddRecentFile(val);
  return true;
}

int taProject::Save_strm(ostream& strm, TAPtr par, int indent) {
#ifdef TA_GUI
  if (use_sim_log) {
    UpdateSimLog();
  }
#endif
  return inherited::Save_strm(strm, par, indent);
}

int taProject::Load_strm(istream& strm, TAPtr par, taBase** loaded_obj_ptr) { 
  int rval = inherited::Load_strm(strm, par, loaded_obj_ptr);
  PostLoadAutos();
  return rval;
}

void taProject::setDirty(bool value) {
  // note: inherited only forwards 'dirty' up the chain, not '!dirty'
  inherited::setDirty(value);
  m_dirty = value;
}


void taProject::UpdateSimLog() {
#ifdef TA_GUI
  SimLogEditDialog* dlg = new SimLogEditDialog(this, GetTypeDef(), false, true);
  dlg->Constr("Update simulation log (SimLog) for this project,\n\
 storing the name of the project and the description as entered here.\n\
 Click off use_sim_log if you are not using this feature");
  if(dlg->Edit(true) && use_sim_log) {
    time_t tmp = time(NULL);
    String tstamp = ctime(&tmp);
    tstamp = tstamp.before('\n');

    String user;
    char* user_c = getenv("USER");
    if(user_c != NULL) user = user_c;
    char* host_c = getenv("HOSTNAME");
    if(host_c != NULL) user += String("@") + String(host_c);

    fstream fh;
    fh.open("SimLog", ios::out | ios::app);
    fh << endl << endl;
    fh << file_name << " <- " << GetFileName() << "\t" << tstamp << "\t" << user << endl;
    if(!desc.empty()) fh << "\t" << desc << endl;
    fh.close(); fh.clear();
  }
#endif
}

//////////////////////////
//   Project_Group	//
//////////////////////////

int Project_Group::Load_strm(istream& strm, TAPtr par, taBase** loaded_obj_ptr) {
  int prj_sz = leaves;
  int rval = inherited::Load_strm(strm, par, loaded_obj_ptr);
  for(int i=prj_sz;i<leaves;i++) {
    taProject* prj = Leaf(i);
    prj->PostLoadAutos();
  }
  return rval;
}


//////////////////////////
//   taRoot		//
//////////////////////////

TypeDef* taRootBase::root_type;

taRootBase* taRootBase::instance() {
  if (!tabMisc::root) {
    taRootBase* rb = (taRootBase*)root_type->GetInstance();
    if (!rb) {
      taMisc::Error("Startup_MakeRoot: Error -- no instance of root type!");
      return NULL;
    }
    tabMisc::root = (taRootBase*)rb->MakeToken();
    taBase::Ref(tabMisc::root);
    tabMisc::root->InitLinks();
  }
  return tabMisc::root;
}

void taRootBase::Initialize() {
  version = taMisc::version;
  SetName("root");
  projects.SetName("projects");
  plugin_deps.SetBaseType(&TA_taPluginDep);
}

void taRootBase::Destroy() {
  CutLinks(); // note: will prob already have been done
  bool we_are_root = (tabMisc::root == this); // 'true' if we are the one and only root app object
  if (we_are_root) {
    tabMisc::root = NULL; //TODO: maybe for cleanness we should do a SetPointer thingy, since we set it that way...
#ifdef DMEM_COMPILE
    if(taMisc::dmem_nprocs > 1) {
      taMisc::RecordScript(".Quit();\n");
    }
#endif
  }
}

void taRootBase::InitLinks() {
  inherited::InitLinks();
  taBase::Own(templates, this);
  taBase::Own(projects, this);
  taBase::Own(viewers, this);
  taBase::Own(plugins, this);
  taBase::Own(plugin_deps, this);
  taBase::Own(mime_factories, this);
  taBase::Own(recent_files, this);
  taBase::Own(last_dirs, this);
  taiMimeFactory_List::setInstance(&mime_factories);
  AddTemplates(); // note: ok that this will be called here, before subclass has finished its own
}

void taRootBase::CutLinks() {
  last_dirs.CutLinks();
  recent_files.CutLinks();
  mime_factories.CutLinks();
  plugin_deps.CutLinks();
  plugins.CutLinks();
  viewers.CutLinks();
  projects.CutLinks();
  templates.CutLinks();
  inherited::CutLinks();
}

#ifdef GPROF			// turn on for profiling
void taRootBase::MonControl(bool on) {
  moncontrol(on);
}
#endif

void taRootBase::AddRecentFile(const String& value) {
  // first, see if already there, if so, then just move it to the top
  int idx = recent_files.FindEl(value);
  if (idx >= 0) {
    recent_files.SwapIdx(0, idx);
    return;
  }
  // not there; if full, then nuke a guy
  if (recent_files.size >= taMisc::num_recent_files)
    recent_files.SetSize(taMisc::num_recent_files - 1);
  // insert it
  recent_files.Insert(value, 0);
}

bool taRootBase::CheckAddPluginDep(TypeDef* td) {
  if (!td) return false;
  TypeDef* pl_td = td->plugin;
  if (!pl_td) return false;
  bool rval = false;
  for (int i = 0; i < plugins.size; ++i) {
    taPlugin* pl = plugins.FastEl(i);
    if (!pl->plugin ) continue; // not loaded
    IPlugin* ipl = pl->plugin->plugin();
    if (!ipl) continue; // not loaded
    if (ipl->GetTypeDef() == pl_td) {
      // this is the guy!
      rval = true;
      // see if already listedbool		VerifyHasPlugins()
      if (plugin_deps.FindName(pl->GetName())) break;
      // otherwise, clone a dep, and add
      taPluginDep* pl_dep = new taPluginDep;
      pl_dep->Copy(*(taPluginBase*)pl);
      plugin_deps.Add(pl_dep);
      break;
    }
  }
  
  return rval;
}

bool taRootBase::VerifyHasPlugins() {
  int miss_cnt = 0;
  for (int i = 0; i < plugin_deps.size; ++i) {
    taPluginDep* pl_dep = (taPluginDep*)plugin_deps.FastEl(i);
    taPlugin* pl = (taPlugin*)plugins.FindName(pl_dep->GetName());
    if (pl) {
      if (!pl->loaded)
        pl_dep->dep_check = taPluginDep::DC_NOT_LOADED;
      //else if...
      else continue; // ok
    } else {
      pl_dep->dep_check = taPluginDep::DC_MISSING;
    }
    ++miss_cnt;
  }
  if (miss_cnt == 0) return true;
  // highlight load issues
  plugin_deps.CheckConfig();
  String msg = "Would you like to Abort or Continue loading?";
  int chs = taMisc::Choice(msg, "Abort", "Continue");
  return (chs == 1);

}

void taRootBase::About() {
  String info;
  info += "TA/CSS Info\n";
  info += "This is the TA/CSS software package, version: ";
  info += taMisc::version;
  info += "\n\n";
  info += "Mailing List:       http://psych.colorado.edu/~oreilly/PDP++/pdp-discuss.html\n";
  info += "WWW Page:           http://psych.colorado.edu/~oreilly/PDP++/PDP++.html\n";
  info += "Anonymous FTP Site: ftp://grey.colorado.edu/pub/oreilly/pdp++/\n";
  info += "\n\n";

  info += "Copyright (c) 1995-2006, Regents of the University of Colorado,\n\
 Carnegie Mellon University, Princeton University.\n\
 \n\
 TA/CSS is free software; you can redistribute it and/or modify\n\
 it under the terms of the GNU General Public License as published by\n\
 the Free Software Foundation; either version 2 of the License, or\n\
 (at your option) any later version.\n\
 \n\
 TA/CSS is distributed in the hope that it will be useful,\n\
 but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
 GNU General Public License for more details.\n\
 \n\
 Note that the taString class was derived from the GNU String class\n\
 Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and\n\
 is covered by the GNU General Public License, see ta_string.h\n";
  taMisc::Choice(info, "Ok");
}

void taRootBase::AddTemplates() {
  templates.Add(Program::MakeTemplate());
}

taBase* taRootBase::GetTemplateInstance(TypeDef* typ) {
  return GetTemplateInstance_impl(typ, &templates);
} 

taBase* taRootBase::GetTemplateInstance_impl(TypeDef* typ, taBase* base) {
  taBase* rval = NULL;
  TypeDef* btyp = base->GetTypeDef();
  if (btyp->name == typ->name) return base;
  
  // if it is a list, check its children first (vastly more likely than member pointers)
  if (btyp->InheritsFrom(&TA_taList_impl)) {
    taList_impl* lst = (taList_impl*)base;
    // check all the children
    for (int j = 0; j < lst->size; ++j) {
      taBase* chld = (taBase*)lst->FastEl_(j);
      if (!chld) continue;
      rval = GetTemplateInstance_impl(typ, chld);
      if (rval) return rval;
    }
  }
  
  // check all taBase* members (but NOT embedded objects) and list children
  for (int i = 0; i < btyp->members.size; ++i) {
    MemberDef* md = btyp->members.FastEl(i);
    // if base is an embedded list, then check all its children
    if (md->type->InheritsFrom(&TA_taList_impl) && 
      (md->type->ptr == 0))
    { 
      taList_impl* lst = (taList_impl*)md->GetOff(base);
      rval = GetTemplateInstance_impl(typ, lst);
      if (rval) return rval;
    }
    
    //TODO: if we find it is needed, then also check taBase* ptrs, or ptrs to lists
  }
  return NULL;
    
} 

void taRootBase::SaveAll() {
  taLeafItr i;
  taProject* pr;
  FOR_ITR_EL(taProject, pr, projects., i) {
    pr->Save(); // does SaveAs if no filename
  }
}

void taRootBase::Options() {
#ifdef TA_GUI
  if (!taMisc::gui_active) return;
  taiEdit* ie =  TA_taMisc.ie;
  if (!ie) return;
  taMisc* inst = (taMisc*)TA_taMisc.GetInstance();
  int accepted = ie->EditDialog(inst, false, NULL, true); // r/w, defcolor, modal
  if (accepted) {
    inst->SaveConfig();
  }
#endif
}



/////////////////////////////////////////
// 	startup code


bool taRootBase::Startup_InitDMem(int& argc, const char* argv[]) {
#ifdef DMEM_COMPILE
  taMisc::Init_DMem(argc, argv);
#endif
  return true;
}

bool taRootBase::Startup_InitTA(ta_void_fun ta_init_fun) {
  // first initialize the types
  if(ta_init_fun)
    (*ta_init_fun)();

  // initialize the key folders
  taMisc::home_dir = taPlatform::getHomePath();
  taMisc::prefs_dir = taPlatform::getAppDataPath(taMisc::app_name);
  // make sure it exists
  taPlatform::mkdir(taMisc::prefs_dir);
  
  // then load configuration info: sets lots of user-defined config info
  taMisc::Init_Defaults_PreLoadConfig();
  ((taMisc*)TA_taMisc.GetInstance())->LoadConfig();
  taMisc::Init_Defaults_PostLoadConfig();

  taMisc::default_scope = &TA_taProject; // this is general default
  
  // load prefs values for us
  taRootBase* inst = instance();
  inst->SetFileName(taMisc::prefs_dir + "/root");
  if (QFile::exists(inst->GetFileName())) {
    inst->Load();
  }
  return true;
}
  	
bool taRootBase::Startup_EnumeratePlugins() {
  taMisc::Init_Hooks();		// plugins register init hooks -- this calls them!
  taPlugins::AddPluginFolder(taMisc::pkg_home + "/lib/plugins");
  taPlugins::AddPluginFolder(taMisc::home_dir + "/ta_plugins"); //TODO: should be pdpuserhome
  taPlugins::InitLog(taMisc::prefs_dir + "/plugins.log");
  taPlugins::EnumeratePlugins();

  return true;
}

bool taRootBase::Startup_InitArgs(int& argc, const char* argv[]) {
  taMisc::AddArgName("-nogui", "NoGui");
  taMisc::AddArgName("--nogui", "NoGui");
  taMisc::AddArgNameDesc("NoGui", "\
 -- Disables the GUI (graphical user interface), for running in background");

  taMisc::AddArgName("-gui", "Gui");
  taMisc::AddArgName("--gui", "Gui");
  taMisc::AddArgNameDesc("Gui", "\
 -- Enables the GUI (graphical user interface) -- it is on by default in most programs except css");

  taMisc::AddArgName("-version", "Version");
  taMisc::AddArgName("--version", "Version");
  taMisc::AddArgNameDesc("Version", "\
 -- Prints out version and other information");

  taMisc::AddArgName("-h", "Help");
  taMisc::AddArgName("--help", "Help");
  taMisc::AddArgNameDesc("Help", "\
 -- Prints out help on startup arguments and other usage information");

  taMisc::AddArgName("-p", "Project");
  taMisc::AddArgName("--proj", "Project");
  taMisc::AddArgName("proj=", "Project");
  taMisc::AddArgNameDesc("Project", "\
 <projname.proj> -- Specifies a project file to be loaded upon startup");

  taMisc::AddArgName("-f", "CssScript");
  taMisc::AddArgName("--file", "CssScript");
  taMisc::AddArgName("file=", "CssScript");
  taMisc::AddArgName("-s", "CssScript");
  taMisc::AddArgName("--script", "CssScript");
  taMisc::AddArgName("script=", "CssScript");
  taMisc::AddArgNameDesc("CssScript", "\
 <scriptname.css> -- Specifies a css script file to be loaded and executed upon startup");

  taMisc::AddArgName("-e", "CssCode");
  taMisc::AddArgName("--exec", "CssCode");
  taMisc::AddArgName("exec=", "CssCode");
  taMisc::AddArgNameDesc("CssCode", "\
 <scriptcode> Specifies css script code to be executed upon startup");

  taMisc::AddArgName("-i", "CssInteractive");
  taMisc::AddArgName("--interactive", "CssInteractive");
  taMisc::AddArgNameDesc("CssInteractive", "\
 -- Specifies that the css console should remain active after running a css script file upon startup");

  taMisc::AddArgName("-v", "CssDebug");
  taMisc::AddArgName("--verbose", "CssDebug");
  taMisc::AddArgName("verbose=", "CssDebug");
  taMisc::AddArgNameDesc("CssDebug", "\
 -- Specifies an initial debug level for css upon startup");

  taMisc::AddArgName("-b", "CssBreakpoint");
  taMisc::AddArgName("--breakpoint", "CssBreakpoint");
  taMisc::AddArgName("breakpoint=", "CssBreakpoint");
  taMisc::AddArgNameDesc("CssBreakpoint", "\
 <line_no> -- Specifies an initial breakpoint at given line number of the startup script file");

  taMisc::AddArgName("-rct", "CssRefCountTrace");
  taMisc::AddArgName("--ref_count_trace", "CssRefCountTrace");
  taMisc::AddArgNameDesc("CssRefCountTrace", "\
 -- Specifies that css reference count tracing should be performed (debugging tool)");

  taMisc::Init_Args(argc, argv);
  return true;
}

bool taRootBase::Startup_ProcessGuiArg() {
#ifndef TA_GUI
  taMisc::use_gui = false;
#endif

  // process gui flag right away -- has other implications
  if(taMisc::CheckArgByName("NoGui"))
    taMisc::use_gui = false;
  if(taMisc::CheckArgByName("Gui"))
    taMisc::use_gui = true;

#ifndef TA_GUI
  if(taMisc::use_gui) {
    taMisc::Error("Startup_InitArgs: cannot specify '-gui' switch when compiled without gui support");
    return false;
  }
#endif
  return true;
}
  	
bool taRootBase::Startup_InitApp(int& argc, const char* argv[]) {
  setlocale(LC_ALL, "");

#ifdef TA_GUI
  if(taMisc::use_gui) {
# ifdef TA_USE_INVENTOR
new QApplication(argc, (char**)argv); // accessed as qApp
    SoQt::init(argc, (char**)argv, cssMisc::prompt.chars()); // creates a special Coin QApplication instance
# else
    new QApplication(argc, (char**)argv); // accessed as qApp
# endif
  } else 
#endif
  {
    new QCoreApplication(argc, (char**)argv); // accessed as qApp
    QFileInfo fi(argv[0]);
    QCoreApplication::instance()->setApplicationName(fi.baseName()); // just the name part w/o path or suffix
  }
  return true;
}

bool taRootBase::Startup_InitTypes() {
  taMisc::Init_Types();
  return true;
}
  	
bool taRootBase::Startup_InitCss() {
  return cssMisc::Initialize();
}
  	
bool taRootBase::Startup_InitGui() {
#ifdef TA_GUI
  if(taMisc::use_gui && (taMisc::dmem_proc == 0)) {
    taiM_ = taiMisc::New(taMisc::use_gui);
    taiMC_ = taiM_;
//     taiM->icon_bitmap = new QBitmap(pdp_bitmap_width,
//     	pdp_bitmap_height, pdp_bitmap_bits);
//    qApp->setWindowIcon(QIcon(*(taiM->icon_bitmap)));
    taMisc::gui_active = true;	// officially active!
    Startup_InitViewColors();
  }
  else
#endif // TA_GUI
  { 
    taiMC_ = taiMiscCore::New();
  }
  return true;
}

/* emacs colors:
 comment	Firebrick
 string		RosyBrown
 keyword	Purple
 builtin	Orchid (also preprocessor)
 function-name	Blue1
 variable-name	DarkGoldenrod
 type		ForestGreen
 constant	CadetBlue
 warning	Red1
*/

bool taRootBase::Startup_InitViewColors() {
  if(!taMisc::view_colors) {
    taMisc::view_colors = new ViewColor_List;
    taMisc::view_colors->BuildHashTable(100); // speed this one up
  }
  taMisc::view_colors->FindMakeViewColor("NotEnabled", "State: !isEnabled",
					 false, _nilString, true, "grey80");
  taMisc::view_colors->FindMakeViewColor("ThisInvalid", "State: item not valid according to CheckConfig",
					 false, _nilString, true, "red1");
  taMisc::view_colors->FindMakeViewColor("ChildInvalid", "State: child not valid according to CheckConfig",
					 false, _nilString, true, "orange1");
  taMisc::view_colors->FindMakeViewColor("ProgElNonStd", "State: program element is not standard",
					 false, _nilString, true, "yellow1");
  taMisc::view_colors->FindMakeViewColor("ProgElNewEl", "State: program element is newly added",
					 false, _nilString, true, "SpringGreen1");
  taMisc::view_colors->FindMakeViewColor("Comment", "Program comment",
					 true, "firebrick", true, "firebrick1");
  taMisc::view_colors->FindMakeViewColor("ProgCtrl", "Program keyword",
					 true, "purple2", true, "MediumPurple1");
  taMisc::view_colors->FindMakeViewColor("Function", "Program function",
					 true, "blue1", true, "LightBlue2");
  taMisc::view_colors->FindMakeViewColor("ProgVar", "Program variable",
					 true, "dark goldenrod");
  taMisc::view_colors->FindMakeViewColor("ProgArg", "Program argument",
					 true, "goldenrod");
  taMisc::view_colors->FindMakeViewColor("Program", "Program itself",
					 true, "red2", true, "red1");
  taMisc::view_colors->FindMakeViewColor("DataTable", "DataTable and associated objects",
					 true, "forest green", true, "pale green");
  taMisc::view_colors->FindMakeViewColor("Wizard", "Wizard and associated objects",
					 true, "azure4", true, "azure1");
  taMisc::view_colors->FindMakeViewColor("SelectEdit", "SelectEdit -- editor for selected variables across different objects",
					 true, "azure4", true, "azure1");
  return true;
}
  	
bool taRootBase::Startup_LoadPlugins() {
  if (!tabMisc::root) return false; // should be made
  tabMisc::root->plugins.LoadPlugins();
  return true;
}

bool taRootBase::Startup_MakeMainWin() {
  if(!taMisc::gui_active) return true;
#ifdef TA_GUI
  // TODO: need to better orchestrate the "OpenWindows" call below with
  // create the default application window
  MainWindowViewer* db = MainWindowViewer::NewBrowser(tabMisc::root, NULL, true);
  // try to size fairly large to avoid scrollbars -- values obtained empirically
  iSize s(1024, 480); // no console
  if (taMisc::console_style == taMisc::CS_GUI_DOCKABLE) {
    s.h = 720; // console
    ConsoleDockViewer* cdv = new ConsoleDockViewer;
    db->docks.Add(cdv);
  }
  db->ViewWindow();
  iMainWindowViewer* bw = db->viewerWindow();
  if (bw) { //note: already constrained to max screen size, so we don't have to check
    bw->resize(s.w, s.h);
    bw->show(); // when we start event loop
  }
  //TODO: following prob not necessary
  if (taMisc::gui_active) taiMisc::OpenWindows();
#endif // TA_GUI
  return true;
}

bool taRootBase::Startup_Console() {
  if (taMisc::use_gui) {
//TODO: we need event loop in gui AND non-gui, so check about Shell_NoGui_Rl
   
#if (!defined(QANDD_CONSOLE) && defined(HAVE_QT_CONSOLE))
    if ((taMisc::console_style == taMisc::CS_GUI_DOCKABLE) ||
     (taMisc::console_style == taMisc::CS_GUI_TRACKING))
    {  //note: nothing else to do here for gui_dockable
      if (taMisc::console_style == taMisc::CS_GUI_TRACKING) {
        QcssConsole* con = QcssConsole::getInstance(NULL, cssMisc::TopShell);
        QMainWindow* cwin = new QMainWindow();
        cwin->setCentralWidget((QWidget*)con);
        cwin->resize((int)(.95 * taiM->scrn_s.w), (int)(.25 * taiM->scrn_s.h));
        cwin->move((int)(.025 * taiM->scrn_s.w), (int)(.7 * taiM->scrn_s.h));
        cwin->show();
        taMisc::console_win = cwin;
      }
      cssMisc::TopShell->StartupShellInit(cin, cout, cssCmdShell::CT_Qt_Console);
      return true;
    } else
#endif
    // otherwise, (esp Windows) we'll use the older type shell
    cssMisc::TopShell->StartupShellInit(cin, cout, cssCmdShell::CT_QandD_Console);
  } else {
    cssMisc::TopShell->StartupShellInit(cin, cout, cssCmdShell::CT_NoGui_Rl);
  }
  return true;
}

bool taRootBase::Startup_ProcessArgs() {
  if(taMisc::CheckArgByName("Version")) {
    tabMisc::root->About();
  }
  if(taMisc::CheckArgByName("Help")) {
    taMisc::HelpMsg();
  }

  // just load the thing!?
  String proj_ld = taMisc::FindArgByName("Project");
  if(proj_ld.empty())
    proj_ld = taMisc::FindArgValContains(".proj");

  if(!proj_ld.empty())
    tabMisc::root->projects.Load(proj_ld);

  cssMisc::TopShell->RunStartupScript();

  return true;
}

bool taRootBase::Startup_Main(int& argc, const char* argv[], ta_void_fun ta_init_fun, 
			      TypeDef* root_typ) 
{
  root_type = root_typ;
#ifdef GPROF
  moncontrol(0);		// turn off at start
#endif
  cssMisc::prompt = taMisc::app_name; // the same
  if(!Startup_InitDMem(argc, argv)) return false;
  if(!Startup_InitTA(ta_init_fun)) return false;
  if(!Startup_InitArgs(argc, argv)) return false;
  if(!Startup_ProcessGuiArg()) return false;
  if(!Startup_InitApp(argc, argv)) return false;
  if(!Startup_InitTypes()) return false;
  if(!Startup_EnumeratePlugins()) return false;
  if(!Startup_LoadPlugins()) return false; // loads those enabled, and does type integration
  if(!Startup_InitCss()) return false;
  if(!Startup_InitGui()) return false; // note: does the taiType bidding
  if(!Startup_MakeMainWin()) return false;
  if(!Startup_Console()) return false;
  if(!Startup_ProcessArgs()) return false;
//TODO: shouldn't call event loop yet, because we haven't initialized main event loop!
//  taiMiscCore::ProcessEvents();;
  instance()->Save(); 
  return true;
}

///////////////////////////////////////////////////////////////////////////
//	Run & Cleanup

bool taRootBase::Startup_Run() {
#ifdef DMEM_COMPILE
  if((taMisc::dmem_nprocs > 1) && taMisc::gui_active) {
    Run_GuiDMem();
  }
  else {
#endif
//TODO: THIS IS WRONG!!!!!!!!!!!!!!!!!!!!!!
// We ABSOLUTELY must enter the event loop in EVERY configuration
  if(taMisc::gui_active) {
    taiM->Exec();		// gui version is always interactive
  }
  else {
    if(cssMisc::init_interactive)
      cssMisc::TopShell->Shell_NoGui_Rl_Run();
  }
#ifdef DMEM_COMPILE
  }
#endif
  return Cleanup_Main();
}

// todo: could partition these out into separate guys..  	
bool taRootBase::Cleanup_Main() {
  tabMisc::DeleteRoot();
  taMisc::types.RemoveAll();	// get rid of all the types before global dtor!

#ifdef TA_USE_INVENTOR
  if(taMisc::gui_active)
    SoQt::done();
#endif
#ifdef DMEM_COMPILE
  MPI_Finalize();
#endif
  return true;
}

#ifdef DMEM_COMPILE

bool taRootBase::Run_GuiDMem() {
  if (taMisc::dmem_proc == 0) { // master dmem
    DMemShare::InitCmdStream();
    // need to have some initial string in the stream, otherwise it goes EOF and is bad!
    *(DMemShare::cmdstream) << "cerr << \"proc no: \" << taMisc::dmem_proc << endl;" << endl;
    taMisc::StartRecording((ostream*)(DMemShare::cmdstream));
    qApp->exec();  // normal run..
    DMemShare::CloseCmdStream();
    cerr << "proc: 0 quitting!" << endl;
  } else { // slave dmems
    cssMisc::init_interactive = false; // don't stay in startup shell
    DMem_SubEventLoop();	// this is the "shell" for a dmem sub guy
    cerr << "proc: " << taMisc::dmem_proc << " quitting!" << endl;
  }
  return true;
}
  	
static cssProgSpace* dmem_space1 = NULL;
static cssProgSpace* dmem_space2 = NULL;

void taRootBase::DMem_WaitProc(bool send_stop_to_subs) {
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
    if(sp->state & cssProg::State_Run) {
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

int taRootBase::DMem_SubEventLoop() {
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
	if(sp->state & cssProg::State_Run) {
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
