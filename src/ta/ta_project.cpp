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
# include <QFileDialog>
# include <QMessageBox>
# include <QWidgetList>
#endif

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QTimer>

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
    bool rval = (md->ShowMember(show()) && (md->im != NULL));
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
  m_no_save = false;
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
  taBase::Own(data_proc, this);
  taBase::Own(programs, this);
  taBase::Own(viewers, this);

  // note: any derived programs should install additional guys..
  FindMakeNewDataProc(&TA_taDataProc, "data_base");
  FindMakeNewDataProc(&TA_taDataAnal, "data_anal");
  FindMakeNewDataProc(&TA_taDataGen, "data_gen");
  FindMakeNewDataProc(&TA_taImageProc, "image_proc");
  // not actually useful to have these guys visible..  no user-accessible matrix objs
  // if in datatable, it should be accessible in above
//   FindMakeNewDataProc(&TA_taMath_float, "math_float");
//   FindMakeNewDataProc(&TA_taMath_double, "math_double");
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
  programs.CutLinks();
  data_proc.CutLinks();
  data.CutLinks();
  edits.CutLinks();
  wizards.CutLinks();
  templates.CutLinks();
}

void taProject::Copy_(const taProject& cp) {
  // delete things first, to avoid dangling references
  programs.Reset();
  viewers.Reset();
  data.Reset();

  edits.Reset();
  
  templates = cp.templates;
  wizards = cp.wizards;
  edits = cp.edits;
  data = cp.data;
  data_proc = cp.data_proc;
  viewers = cp.viewers;
  programs = cp.programs;
  // NOTE: once a derived project has all the relevant stuff copied, it needs to call this:
  // UpdatePointers_NewPar(&cp, this); // update pointers within entire project..
  setDirty(true);
}

void taProject::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}

taBase* taProject::FindMakeNewDataProc(TypeDef* typ, const String& nm) {
  taBase* obj = data_proc.FindType(typ);
  if(obj) return obj;
  obj = data_proc.NewEl(1, typ);
  obj->SetName(nm);
  return obj;
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
  if (viewer_name != "(default name)")
    vwr->SetName(viewer_name);
  vwr->ViewWindow();
  
}

DataTable* taProject::GetNewAnalysisDataTable(const String& nw_nm, bool msg) {
  DataTable_Group* dgp = (DataTable_Group*)data.FindMakeGpName("AnalysisData");
  DataTable* rval = dgp->NewEl(1, &TA_DataTable);
  if(!nw_nm.empty())
    rval->name = nw_nm;
  if(msg)
    taMisc::Info("Note: created new data table named:", rval->name, "in .data.AnalysisData");
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
  // always set current dir for project!
  if(!val.empty()) {
    QDir::setCurrent(taMisc::GetDirFmPath(val));	
  }
  return true;
}

int taProject::Save_strm(ostream& strm, TAPtr par, int indent) {
#ifdef TA_GUI
  if (use_sim_log) {
    UpdateSimLog();
  }
#endif
  int rval = inherited::Save_strm(strm, par, indent);
  return rval;
}

int taProject::Load_strm(istream& strm, TAPtr par, taBase** loaded_obj_ptr) { 
  int rval = inherited::Load_strm(strm, par, loaded_obj_ptr);
  return rval;
}

void taProject::setDirty(bool value) {
  // note: inherited only forwards 'dirty' up the chain, not '!dirty'
  inherited::setDirty(value);
  m_dirty = value;
  if (!value) m_no_save = false;
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

    String user = taPlatform::userName();
    String host = taPlatform::hostName();
    if (host.nonempty()) user += String("@") + host;

    String fnm = taMisc::GetDirFmPath(file_name) + "/SimLog";
    fstream fh;
    fh.open(fnm, ios::out | ios::app);
    fh << endl << endl;
    fh << file_name << " <- " << GetFileName() << "\t" << tstamp << "\t" << user << endl;
    if(!desc.empty()) fh << "\t" << desc << endl;
    fh.close(); fh.clear();
  }
#endif
}

void taProject::SaveRecoverFile() {
  bool tmp_sim_log = use_sim_log;
  use_sim_log = false;		// don't pop up any dialogs..

  String prfx;
  String sufx = ".proj";
  String recv = "_recover";
  if(file_name.empty()) {
    if(name.empty()) {
      prfx = GetTypeDef()->name;
    }
    else {
      prfx = name;
    }
  }
  else {
    if(file_name.contains(sufx)) {
      prfx = file_name.before(sufx, -1);
    }
    else {
      prfx = file_name;		// whatever
    }
  }
  if(prfx.contains(recv))
    prfx = prfx.through(recv, -1);
  else
    prfx += recv;
  int cnt = taMisc::GetUniqueFileNumber(0, prfx, sufx);
  String fnm = prfx + String(cnt) + sufx;
  taFiler* flr = GetSaveFiler(fnm, _nilString, -1, _nilString);
  bool saved = false;
  if(flr->ostrm) {
    SaveRecoverFile_strm(*flr->ostrm);
    saved = true;
  }
  else {
    cerr << "Error saving recover file: " << fnm << endl;
    String old_fnm = fnm;
    String fnm = taMisc::user_dir + "/" + taMisc::GetFileFmPath(old_fnm);
    flr->setFileName(fnm);
    flr->Save();
    if(flr->ostrm) {
      cerr << "Now saving in user directory: " << fnm << endl;
      use_sim_log = false;
      SaveRecoverFile_strm(*flr->ostrm);
      saved = true;
    }
  }
  flr->Close();
  taRefN::unRefDone(flr);

  use_sim_log = tmp_sim_log;

#ifdef HAVE_QT_CONSOLE
  // now try to save console
  if(saved) {
    if(cssMisc::TopShell->console_type == taMisc::CT_GUI) {
      String cfnm = fnm;
      cfnm.gsub("_recover", "_console");
      cfnm.gsub((const char*)sufx, ".txt");
      QcssConsole* qcons = QcssConsole::getInstance();
      if(qcons)
	qcons->saveContents(cfnm);
    }
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
//   taRootBaseAdapter	//
//////////////////////////

void taRootBaseAdapter::Startup_ProcessArgs() {
  taRootBase::Startup_ProcessArgs();
}

void taRootBaseAdapter::Startup_RunStartupScript() {
  taRootBase::Startup_RunStartupScript();
}

#ifdef DMEM_COMPILE
void taRootBaseAdapter::DMem_SubEventLoop() {
  taRootBase::DMem_SubEventLoop();
}
#endif // DMEM_COMPILE

//////////////////////////
//   taRoot		//
//////////////////////////

int taRootBase::milestone;
TypeDef* taRootBase::root_type;
taMisc::ConsoleType taRootBase::console_type;
int taRootBase::console_options;

// note: not static class to avoid need qpointer in header
QPointer<taRootBaseAdapter> root_adapter;

taRootBase* taRootBase::instance() {
  if (!tabMisc::root) {
    taRootBase* rb = (taRootBase*)root_type->GetInstance();
    if (!rb) {
      taMisc::Error("Startup_MakeRoot: Error -- no instance of root type!");
      return NULL;
    }
    tabMisc::root = (taRootBase*)rb->MakeToken();
    tabMisc::root->SetName("root");
    taBase::Ref(tabMisc::root);
    tabMisc::root->InitLinks();
  }
  return tabMisc::root;
}

void taRootBase::Initialize() {
  version = taMisc::version;
  projects.SetName("projects");
  plugin_deps.SetBaseType(&TA_taPluginDep);
  console_type = taMisc::console_type;
  console_options = taMisc::console_options;
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
  taBase::Own(colorspecs, this);
  // create colorspecs even if nogui, since they are referenced in projects
  colorspecs.SetDefaultColor();	
  taBase::Own(recent_files, this);
  taBase::Own(recent_paths, this);
  taiMimeFactory_List::setInstance(&mime_factories);
  AddTemplates(); // note: ok that this will be called here, before subclass has finished its own
#ifdef DEBUG
  taBase::Own(test_group, this);
#endif
}

void taRootBase::CutLinks() {
#ifdef DEBUG
  test_group.CutLinks();
#endif
  recent_paths.CutLinks();
  recent_files.CutLinks();
  colorspecs.CutLinks();
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
  bool save = AddRecentFile_impl(value);
  QFileInfo fi(value);
  String path = fi.path();
  if (AddRecentPath_impl(path))
    save = true;
  if (save)
    Save();
}

bool taRootBase::AddRecentFile_impl(const String& value) {
  // first, see if already there, if so, then just move it to the top
  if (taMisc::num_recent_files <= 0) {
    if (recent_files.size > 0) {
      recent_files.Reset();
      return true;
    } else return false;
  }
  int idx = recent_files.FindEl(value);
  if (idx == 0) return false; // already at top, no need to save either!
  else if (idx > 0) {
    recent_files.MoveIdx(idx, 0);
  } else {
    // not there; if full, then nuke a guy
    if (recent_files.size >= taMisc::num_recent_files)
      recent_files.SetSize(taMisc::num_recent_files - 1);
    // insert it
    recent_files.Insert(value, 0);
  }
  return true;
}

void taRootBase::AddRecentPath(const String& value) {
  if (AddRecentPath_impl(value))
    Save();
}

bool taRootBase::AddRecentPath_impl(const String& value) {
  // first, see if already there, if so, then just move it to the top
  if (taMisc::num_recent_paths <= 0) {
    if (recent_paths.size > 0) {
      recent_paths.Reset();
      return true;
    } else return false;
  }
  int idx = recent_paths.FindEl(value);
  if (idx == 0) return false; // already at top, no need to save either!
  else if (idx > 0) {
    recent_paths.MoveIdx(idx, 0);
  } else {
    // not there; if full, then nuke a guy(s)
    if (recent_paths.size >= taMisc::num_recent_paths)
      recent_paths.SetSize(taMisc::num_recent_paths - 1);
    // insert it
    recent_paths.Insert(value, 0);
  }
  return true;
}

int taRootBase::Save() {
  ++taFiler::no_save_last_fname;
  int rval = inherited::Save();
  --taFiler::no_save_last_fname;
  return rval;
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
      static_cast<taPluginBase*>(pl_dep)->Copy(*pl);
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
  milestone |= SM_MPI_INIT;
#endif
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

  taMisc::AddArgName("-a", "AppDir");
  taMisc::AddArgName("--app_dir", "AppDir");
  taMisc::AddArgName("app_dir=", "AppDir");
  taMisc::AddArgNameDesc("AppDir", "\
 -- explicitly specifies location of the app directory (prog libs, plugins, etc.)");

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

  taMisc::AddArgName("-u", "UserDir");
  taMisc::AddArgName("--user_dir", "UserDir");
  taMisc::AddArgName("user_dir=", "UserDir");
  taMisc::AddArgNameDesc("UserDir", "\
 -- explicitly specifies location of user home folder (should normally not need to override)");

  taMisc::AddArgName("-ua", "UserAppDir");
  taMisc::AddArgName("--user_app_dir", "UserAppDir");
  taMisc::AddArgName("user_app_dir=", "UserAppDir");
  taMisc::AddArgNameDesc("UserAppDir", "\
 -- explicitly specifies location of user app folder (should normally not need to override)");

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

bool taRootBase::Startup_ProcessGuiArg(int argc, const char* argv[]) {
#ifdef TA_GUI
  taMisc::use_gui = true;
#else
  taMisc::use_gui = false;
#endif

  // process gui flag right away -- has other implications
  // we will just take the last one found on cmd line
  for (int i = argc - 1; i > 0; --i) {
    String arg = argv[i];
     if (arg.endsWith("-nogui")) {
      taMisc::use_gui = false; 
      break;
    } else if (arg.endsWith("-gui")) {
      taMisc::use_gui = true; 
      break;
    }
  }

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
    milestone |= (SM_QAPP_OBJ | SM_SOQT_INIT);
# else
    new QApplication(argc, (char**)argv); // accessed as qApp
    milestone |= SM_QAPP_OBJ;
# endif
  } else 
#endif
  {
    new QCoreApplication(argc, (char**)argv); // accessed as qApp
    QFileInfo fi(argv[0]);
    milestone |= SM_QAPP_OBJ;
  }    
  QCoreApplication::instance()->setApplicationName(taMisc::app_name);
  return true;
}

bool taRootBase::isAppDir(String path) {
//NOTE: this is a test that is supposed to confirm a dir is a tacss dir
// our first version checks for the prog_lib folder
  path = taPlatform::finalSep(path) + "prog_lib";
  QDir dir(path);
  bool rval = dir.exists();
#ifdef DEBUG // don't clutter with success, just failures
  if (!rval)
    taMisc::Info("Did not find app_dir as:", path);
#endif
  return rval;
}

// hairy, modal, issue-prone -- we put in its own routine
bool taRootBase::Startup_InitTA_folders() {
  // explicit cmdline override has highest priority
  String app_dir = taMisc::FindArgByName("AppDir");
  if (app_dir.nonempty() && isAppDir(app_dir))
    goto have_app_dir;
  
/* NOTE: in development, binaries can tend to get created deep
   inside the source code structure -- therefore, we give priority
   to the PDP4DIR variable first, before checking dynamically.
   Below, we list the likely or supported folder structure.
*/
  app_dir = getenv("PDP4DIR");
  if (app_dir.nonempty() && isAppDir(app_dir))
    goto have_app_dir;

  app_dir = QCoreApplication::applicationDirPath();
#ifdef TA_OS_WIN
/*
  {app_dir}\bin
*/
  // note: Qt docs say it returns the '/' version...
  if (app_dir.endsWith("/bin") || app_dir.endsWith("\bin")) {
    app_dir = app_dir.at(0, app_dir.length() - 4);
    if (isAppDir(app_dir)) goto have_app_dir;
  }
#elif defined(TA_OS_MAC)
/*
  {app_dir}/{appname.app}/Contents/MacOS (bundle in app root)
  {app_dir}/bin/{appname.app}/Contents/MacOS (bundle in app bin)
  {app_dir}/bin (typically non-gui only, since gui must run from bundle)
*/
  { // because of goto
  String probe = "/Contents/MacOS";
  if (app_dir.endsWith(probe)) {
    app_dir = app_dir.before(probe);
    QDir dir(app_dir);
    if (dir.cdUp()) {
      app_dir = dir.absolutePath();
      // if bundle is in bin folder, strip that
      if (app_dir.endsWith("/bin")) {
        app_dir = app_dir.at(0, app_dir.length() - 4);
      }
      if (isAppDir(app_dir)) goto have_app_dir;
    }
  }
  }
  // seemingly not in a bundle, so try raw bin
  if (app_dir.endsWith("/bin")) {
    app_dir = app_dir.at(0, app_dir.length() - 4);
    if (isAppDir(app_dir)) goto have_app_dir;
  }
    
#else // non-Mac Unix
/*
  {app_dir}/bin
*/
  if (app_dir.endsWith("/bin")) {
    app_dir = app_dir.at(0, app_dir.length() - 4);
    if (isAppDir(app_dir)) goto have_app_dir;
  }
#endif

  // common code for failure to grok the app path
  app_dir = QCoreApplication::applicationDirPath();
  // first, maybe it is actually the exe's folder itself? -- probe with known subfolder
  if (isAppDir(app_dir)) goto have_app_dir;
  
  // is it the current directory? -- probe with known subfolder
  app_dir = QDir::currentPath();
  if (isAppDir(app_dir)) goto have_app_dir;
  
#ifdef TA_OS_UNIX
  // on Unix platforms, check the usually folders
  app_dir = "/usr/local/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/usr/local/share/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/usr/share/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/usr/local/src/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/usr/share/src/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
#endif

#ifdef DEBUG
  cerr << "NOTE: default app_dir logic did not find the app_dir.\n";
#endif  
  
  app_dir = _nilString; // try finding override, or else prompt user
  
have_app_dir:

  // initialize the key folders
  taMisc::app_dir = app_dir;
  // cmd line override of UserDir takes preference
  taMisc::user_dir = taMisc::FindArgByName("UserDir");;
  if (taMisc::user_dir.empty())
    taMisc::user_dir = taPlatform::getHomePath();
  
  taMisc::user_app_dir = taMisc::FindArgByName("UserAppDir");;
  if (taMisc::user_app_dir.empty())
    taMisc::user_app_dir = taMisc::user_dir + PATH_SEP + taMisc::app_name + "_user";
  taMisc::prefs_dir = taPlatform::getAppDataPath(taMisc::app_name);
  // make sure it exists
  taPlatform::mkdir(taMisc::prefs_dir);
  return true;
}

bool taRootBase::Startup_InitTA_getMissingAppDir() {
//TODO: if gui, prompt user to find the app path, must be valid
  // start with a default, and then loop validating /fetching
  bool prompted = false;
  bool new_one = false; // assume it existed before
  String app_dir(taMisc::app_dir_default);
  while (true) {
    // validate current override, if any -- first one comes from options
    if (app_dir.nonempty()) {
      if (isAppDir(app_dir)) {
        taMisc::app_dir_default = app_dir; // save for next time
        taMisc::app_dir = app_dir;
        // save the manual default so user doesn't get bothered every time
        if (new_one)
          ((taMisc*)TA_taMisc.GetInstance())->SaveConfig();
        return true;
      }
    }
    new_one = true;
    // if in gui mode, prompt, otherwise just fail
    if (!taMisc::use_gui) return false;
    
    //ask user to supply the application folder
    // note: 1) we can't use our own gui stuff yet; 2) we may be in nogui mode!
    String msg = "The folder where the application was installed could not be found -- would you like to find it yourself? (the application will close otherwise)";
    
    if (taMisc::use_gui) {
      if (!prompted) {
        //note: following will be non-gui
        int chs = QMessageBox::question(NULL, "Can't find app folder", msg,
          (QMessageBox::Yes | QMessageBox::No), QMessageBox::Yes);
        if (chs != QMessageBox::Yes) return false;
      }
      app_dir = QFileDialog::getExistingDirectory(NULL, "Find root application folder");
    } else {
      if (!prompted) {
        //note: following will be non-gui
        int chs = taMisc::Choice(msg, "Yes (Find)", "No (Close)");
        if (chs != 0) return false;
      }
      cout << "Enter path to root application folder (blank to cancel): ";
      cout.flush();
      cin >> app_dir;
    }
    prompted = true;
    if (app_dir.empty()) return false;
    // strip trailing slash
    String last = app_dir.right(1);
    if ((last == "/") || (last == "\\"))
      app_dir.truncate(app_dir.length() - 1);
  } 
  return false;
}

bool taRootBase::Startup_InitTA_initUserAppDir() {
  // make sure the folder exists
  // make sure the standard user subfolders exist:
  QDir dir(taMisc::user_app_dir);
  if (!dir.exists()) {
    if (!dir.mkdir(taMisc::user_app_dir)) {
  //TODO: this is too harsh -- should prompt user for one, like for app dir
      taMisc::Error("Could not find or make the user dir:", taMisc::user_app_dir,
       "Please make sure this directory exists and is readable, and try again.");
      return false;
    }
  }
//NOTE: we could get excessively anal, and check all of these, but if we
// can make/read the user folder, then very unlikely will these fail
  // make user prog_lib
  dir.mkdir(taMisc::user_app_dir + PATH_SEP + "prog_lib");
  // make user css_lib
  dir.mkdir(taMisc::user_app_dir + PATH_SEP + "css_lib");
  // TODO: make user plugin folders
  
  return true;
}

bool taRootBase::Startup_InitTA(ta_void_fun ta_init_fun) {
  // first initialize the types
  if(ta_init_fun)
    (*ta_init_fun)();
  taMisc::Init_Hooks();	// client dlls register init hooks -- this calls them!
  milestone |= SM_TYPES_INIT;
    
  if (!Startup_InitTA_folders()) return false;

  // then load configuration info: sets lots of user-defined config info
  taMisc::Init_Defaults_PreLoadConfig();
  ((taMisc*)TA_taMisc.GetInstance())->LoadConfig();
//TEMP
if (taMisc::proj_view_pref == -1) {
  if (taMisc::user_dir.contains("oreilly")) 
    taMisc::proj_view_pref = taMisc::PVP_3PANE;
  else taMisc::proj_view_pref = taMisc::PVP_2x2;
}
// / TEMP

  // init user dir first time
  if (taMisc::user_app_dir.empty()) {
    taMisc::user_app_dir = taMisc::user_dir + PATH_SEP + taMisc::app_name + "_user";
  }

  if (!Startup_InitTA_initUserAppDir()) return false;
  
// if we still hadn't found an app_dir, need to find one now!
  if (taMisc::app_dir.empty()) {
    if (Startup_InitTA_getMissingAppDir() ) {
    } else {
      taMisc::Error("Could not find application folder, shutting down.");
      return false;
    }
  }
  taMisc::Init_Defaults_PostLoadConfig();

  console_type = taMisc::console_type;
  console_options = taMisc::console_options;

  taMisc::default_scope = &TA_taProject; // this is general default
  
  // load prefs values for us
  taRootBase* inst = instance();
  milestone |= SM_ROOT_CREATE;
  inst->SetFileName(taMisc::prefs_dir + "/root");
  if (QFile::exists(inst->GetFileName())) {
    ++taFiler::no_save_last_fname;
    inst->Load();
    --taFiler::no_save_last_fname;
  }
  return true;
}
  	
bool taRootBase::Startup_EnumeratePlugins() {
#ifdef TA_OS_WIN
  String plug_dir = "\\bin"; 
#else
  String plug_dir = "/lib"; 
#endif
  String plug_log;
  String plug_sub; // subdirectory, if any, for debug, mpi, etc.
  if (taMisc::build_str.empty()) {
    plug_log = "plugins.log";
  } else {
    plug_log = "plugins_" + taMisc::build_str + ".log";
    plug_sub = PATH_SEP + taMisc::build_str;
  }
  // add basic tacss plugin folders, for 
  taPlugins::AddPluginFolder(taMisc::app_dir + plug_dir + PATH_SEP + "plugins_tacss" + plug_sub);
  taPlugins::AddPluginFolder(taMisc::user_app_dir + plug_dir + PATH_SEP + "plugins_tacss" + plug_sub);
  // add for the application lib, ex. pdp
  if (taMisc::app_lib_name.nonempty()) {
    taPlugins::AddPluginFolder(taMisc::app_dir + plug_dir + 
      PATH_SEP + "plugins_" + taMisc::app_lib_name + plug_sub);
    taPlugins::AddPluginFolder(taMisc::user_app_dir + plug_dir +
      PATH_SEP + "plugins_" + taMisc::app_lib_name + plug_sub);
  }
  taPlugins::InitLog(taMisc::prefs_dir + PATH_SEP + plug_log);
  taPlugins::EnumeratePlugins();

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
  milestone |= SM_APP_OBJ;
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
  taMisc::view_colors->FindMakeViewColor("ProgType", "Program type",
					 true, "lime green", true, "lime green");
  taMisc::view_colors->FindMakeViewColor("ProgVar", "Program variable",
					 true, "dark goldenrod");
  taMisc::view_colors->FindMakeViewColor("ProgArg", "Program argument",
					 true, "goldenrod");
  taMisc::view_colors->FindMakeViewColor("Program", "Program itself",
					 true, "coral2", true, "coral1");
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

bool taRootBase::Startup_ConsoleType() {
  // arbitrate console options
  // first, make sure requested console_type is a legal value for this platform
  
  // note: is_batch could be extended to include "headless" cmd line invocation
  //   it would also include contexts such as piping or other stdin/out redirects
  bool is_batch = !cssMisc::init_interactive;
#ifdef DMEM_COMPILE
  if(taMisc::use_gui) {
    if((taMisc::dmem_nprocs > 1) && (taMisc::dmem_proc > 0)) // non-first procs batch
      is_batch = true;
  }
  else {
    if(taMisc::dmem_nprocs > 1)	// nogui dmem is *ALWAYS* batch for all procs
      is_batch = true;
  }
#endif  

  if (is_batch) {
    console_type = taMisc::CT_NONE;
    console_options &= ~(taMisc::CO_USE_PAGING_GUI | taMisc::CO_USE_PAGING_NOGUI); // damn well better not use paging!!!
  } else if (taMisc::use_gui) {
#ifdef HAVE_QT_CONSOLE
    if (!((console_type == taMisc::CT_OS_SHELL) ||
         (console_type == taMisc::CT_GUI) ||
         (console_type == taMisc::CT_NONE))
    ) console_type = taMisc::CT_GUI;
#else
    if (!((console_type == taMisc::CT_OS_SHELL) ||
         (console_type == taMisc::CT_NONE))
    ) console_type = taMisc::CT_OS_SHELL;
#endif
  } else { // not a gui context, can only use a non-gui console
    if (!((console_type == taMisc::CT_OS_SHELL) ||
         (console_type == taMisc::CT_NONE))
    ) console_type = taMisc::CT_OS_SHELL;
  }  
  return true; // always works
}

bool taRootBase::Startup_MakeMainWin() {
  if(!taMisc::gui_active) return true;
#ifdef TA_GUI
  // TODO: need to better orchestrate the "OpenWindows" call below with
  // create the default application window
  MainWindowViewer* db = MainWindowViewer::NewBrowser(tabMisc::root, NULL, true);
  // try to size fairly large to avoid scrollbars -- values obtained empirically
  iSize s(1024, 480); // no console
  if ((console_type == taMisc::CT_GUI) && (!(console_options & taMisc::CO_GUI_TRACKING))) {
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
#ifdef HAVE_QT_CONSOLE
  if (console_type == taMisc::CT_GUI) {  
    //note: nothing else to do here for gui_dockable
    if (console_options & taMisc::CO_GUI_TRACKING) {
      QcssConsole* con = QcssConsole::getInstance(NULL, cssMisc::TopShell);
      QMainWindow* cwin = new QMainWindow();
      cwin->setCentralWidget((QWidget*)con);
      cwin->resize((int)(.95 * taiM->scrn_s.w), (int)(.25 * taiM->scrn_s.h));
      cwin->move((int)(.025 * taiM->scrn_s.w), (int)(.7 * taiM->scrn_s.h));
      cwin->show();
      taMisc::console_win = cwin; // note: uses a guarded QPointer

      MainWindowViewer* db = (MainWindowViewer*)tabMisc::root->viewers[0];
      db->ViewWindow();		// make sure root guy is on top
    }
  }
#endif
  cssMisc::TopShell->StartupShellInit(cin, cout, console_type);

  return true;
}

bool taRootBase::Startup_RegisterSigHandler() {
// #if (!defined(DMEM_COMPILE)) 
  // let's see if this works now!
  taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) SaveRecoverFileHandler);
  milestone |= SM_REG_SIG;
// #endif
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

  if(!proj_ld.empty()) {
    tabMisc::root->projects.Load(proj_ld);
  }

  // chain the next step -- this will hopefully happen *after* any post-loading
  // events triggered by the projects.load 
  QTimer::singleShot(0, root_adapter, SLOT(Startup_RunStartupScript()));

  return true;
}

bool taRootBase::Startup_RunStartupScript() {
  cssMisc::TopShell->RunStartupScript();

  if(!cssMisc::init_interactive) taiMC_->Quit();

  return true;
}

bool taRootBase::Startup_Main(int& argc, const char* argv[], ta_void_fun ta_init_fun, 
			      TypeDef* root_typ) 
{
  root_type = root_typ;
#ifdef GPROF
  moncontrol(0);		// turn off at start
#endif
  // just create the adapter obj, whether needed or not
  root_adapter = new taRootBaseAdapter;
  cssMisc::prompt = taMisc::app_name; // the same
  if(!Startup_InitDMem(argc, argv)) goto startup_failed;
  if(!Startup_ProcessGuiArg(argc, argv)) goto startup_failed;
  if(!Startup_InitArgs(argc, argv)) goto startup_failed;
  if(!Startup_InitApp(argc, argv)) goto startup_failed;
  if(!Startup_InitTA(ta_init_fun)) goto startup_failed;
  if(!Startup_InitTypes()) goto startup_failed;
  if(!Startup_EnumeratePlugins()) goto startup_failed;
  if(!Startup_LoadPlugins()) goto startup_failed; // loads those enabled, and does type integration
  if(!Startup_InitCss()) goto startup_failed;
  if(!Startup_InitGui()) goto startup_failed; // note: does the taiType bidding
  if(!Startup_ConsoleType()) goto startup_failed;
  if(!Startup_MakeMainWin()) goto startup_failed;
  if(!Startup_Console()) goto startup_failed;
  if(!Startup_RegisterSigHandler()) goto startup_failed;
  // note: Startup_ProcessArgs() is called after having entered the event loop
  // note: don't call event loop yet, because we haven't initialized main event loop
  // happens in Startup_Run()
  instance()->Save(); 
  return true;
  
startup_failed:
  Cleanup_Main();
  return false;
}

///////////////////////////////////////////////////////////////////////////
//	Run & Cleanup

bool taRootBase::Startup_Run() {
#ifdef DMEM_COMPILE
//TODO: make Run_GuiDMem dispatched after event loop, and fall through
  if((taMisc::dmem_nprocs > 1) && taMisc::gui_active) {
    Run_GuiDMem(); // does its own eventloop dispatch
    return true;
  }
#endif

  // first thing to do upon entering event loop:
  QTimer::singleShot(0, root_adapter, SLOT(Startup_ProcessArgs()));

  if (taMisc::gui_active || cssMisc::init_interactive) {
    // next thing is to start the console if interactive
    if (console_type == taMisc::CT_NONE) {
      QTimer::singleShot(0, cssMisc::TopShell, SLOT(Shell_NoConsole_Run()));
    }
  }

  // now everyone goes into the event loop
  taiMC_->Exec();

  Cleanup_Main();
  return true;
}

// todo: could partition these out into separate guys..  	
void taRootBase::Cleanup_Main() {
  // remove sig handler -- very nasty when baddies happen after this point
  if (milestone & SM_REG_SIG) {
    taMisc::Register_Cleanup(SIG_DFL); // replace back to using default
  }
  if (milestone & SM_ROOT_CREATE)
    tabMisc::DeleteRoot();
  if (milestone & SM_TYPES_INIT)
    taMisc::types.RemoveAll();	// get rid of all the types before global dtor!

#ifdef TA_USE_INVENTOR
  if(taMisc::gui_active && (milestone & SM_SOQT_INIT))
    SoQt::done();
#endif
#ifdef DMEM_COMPILE
  if (milestone & SM_MPI_INIT)
    MPI_Finalize();
#endif
}

#ifdef DMEM_COMPILE

bool taRootBase::Run_GuiDMem() {
  if (taMisc::dmem_proc == 0) { // master dmem
    DMemShare::InitCmdStream();
    // need to have some initial string in the stream, otherwise it goes EOF and is bad!
    *(DMemShare::cmdstream) << "cerr << \"proc no: \" << taMisc::dmem_proc << endl;" << endl;
    taMisc::StartRecording((ostream*)(DMemShare::cmdstream));
    taiMC_->Exec();  // normal run..
    DMemShare::CloseCmdStream();
    cerr << "proc: 0 quitting!" << endl;
  } else { // slave dmems
    cssMisc::init_interactive = false; // don't stay in startup shell
    QTimer::singleShot(0, root_adapter, SLOT(DMem_SubEventLoop()));
    taiMC_->Exec();  // event loop
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
    delete[] recv_buf;
    recv_buf = NULL; // defensive

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
	  taiMiscCore::Quit(); // unconditional
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

//////////////////////////////////////////////////////////////////////////////
// 		Recover File Handler

//#ifndef TA_OS_WIN

#include <signal.h>
#include <memory.h>
#include <sstream>

// for saving a recovery file if program crashes, is killed, etc.
void taRootBase::SaveRecoverFileHandler(int err) {
  static bool has_crashed = false;
  signal(err, SIG_DFL);		// disable catcher

  if(has_crashed) {
    cerr << "Unable to save recover file (multiple errors)...sorry" << endl;
    exit(err);
  }
  has_crashed = true;		// to prevent recursive crashing..

#ifdef TA_GUI
  taiMisc::Cleanup(err);	// cleanup stuff in tai
#endif
#ifndef TA_OS_WIN // MS CRT doesn't handle these signals...
  if((err == SIGUSR1) || (err == SIGUSR2) || (err == SIGALRM)) {
    cerr << "Saving project file(s) from signal: ";
  } else
#endif // !TA_OS_WIN
  {
    cerr << "Saving recover file(s) and exiting from signal: ";
  }
  taMisc::Decode_Signal(err);
  cerr << endl;

  if (tabMisc::root) for (int i = 0; i < tabMisc::root->projects.size; ++i) {
    taProject* prj = tabMisc::root->projects.FastEl(i);
    prj->SaveRecoverFile();
  }

#ifdef TA_OS_WIN // MS CRT doesn't handle these signals...
  exit(err);
#else // TA_OS_WIN // MS CRT doesn't handle these signals...
  if((err == SIGALRM) || (err == SIGUSR1) || (err == SIGUSR2)) {
    taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) SaveRecoverFileHandler);
    has_crashed = false;
  } else {
    kill(getpid(), err);		// activate signal
  }
#endif // 
 
}

//#endif // TA_OS_WIN
