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

#include "taRootBase.h"
#include <taRootBase_QObj>
#include <taFiler>
#include <taPlugin>
#include <taPluginInst>
#include <taPluginDep>
#include <IPlugin>
#include <taPlugins>
#include <TemtServer>
#include <taiEdit>
#include <taApplication>
#include <ViewColor_List>
#include <ViewBackground_List>
#include <MainWindowViewer>
#include <iMainWindowViewer>
#include <iNetworkAccessManager>
#include <ConsoleDockViewer>
#include <cssConsoleWindow>
#include <taGenDoc>
#include <taCodeUtils>
#include <Program>
#include <taDoc>
#include <taiWidgetTokenChooser>
#include <KeyBindings_List>
#include <MTRnd>
#include <DynEnumType>
#include <ControlPanel>
#include <DataTable>
#include <iDialogChoice>
#include <taMediaWiki>

taTypeDef_Of(PluginWizard);
taTypeDef_Of(StartupWizard);

#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taiMisc>
#include <DMemShare>

#include <QPointer>
#include <QFileInfo>
#include <QApplication>
#include <QDir>
#include <QDesktopServices>
#include <QPixmap>
#include <QIcon>
#include <QTimer>
#include <QGLFormat>
#include <QBrush>
#include <QLocale>
#include <QFile>
#if (QT_VERSION >= 0x050000)
#include <QStandardPaths>
#endif
#include <QDesktopServices>

#if (QT_VERSION >= 0x050600)
#include <QSurfaceFormat>
#endif

#include <css_machine.h>
#include <css_qtconsole.h>

#ifdef TA_QT3D

#else // TA_QT3D
  #include <Quarter/Quarter.h>
  #include <Quarter/QuarterWidget.h>
  #include <SoImageEx.h>
  #include <Inventor/SbImage.h>
  #include <Inventor/nodes/SoTexture2.h>
  #include <Inventor/SbLinear.h>
#endif // TA_QT3D
#include <ode/ode.h>

#ifdef GPROF                    // turn on for profiling
extern "C" void moncontrol(int mode);
#endif
#ifdef HPCPROF_COMPILE
#include <hpctoolkit.h>
#endif
#ifdef CUDA_COMPILE
#include "cuda_profiler_api.h"
#endif

#ifdef TA_OS_LINUX
# include <fenv.h>
#endif

using namespace std;

class DataTable;
class DynEnumType;
class ControlPanel;

int taRootBase::milestone = 0;
TypeDef* taRootBase::root_type = NULL;
int taRootBase::console_options = 0;
ContextFlag taRootBase::in_init;
bool taRootBase::openProject = false;

// note: not static class to avoid need qpointer in header
QPointer<taRootBase_QObj> root_adapter;

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
  console_options = taMisc::console_options;
  openProject = false;
  rootview_pos.SetXY(0.0f, 0.0f);
  rootview_size.SetXY(0.9f, 0.6f);
  console_locked = true;
#ifdef TA_OS_LINUX
  fpe_enable = FPE_0; //GetFPEFlags(fegetexcept());
#endif
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
  version = taMisc::version;
  taBase::Own(templates, this);
  taBase::Own(docs, this);
  taBase::Own(wizards, this);
  taBase::Own(projects, this);
  taBase::Own(viewers, this);
  taBase::Own(viewers_tmp, this);
  taBase::Own(plugins, this);
  taBase::Own(plugin_state, this);
  taBase::Own(plugin_deps, this);
  taBase::Own(mime_factories, this);
  taBase::Own(colorspecs, this);
  // create colorspecs even if nogui, since they are referenced in projects
  colorspecs.SetDefaultColor();
  taBase::Own(objs, this);
  taBase::Own(recent_files, this);
  taBase::Own(recent_paths, this);
  taBase::Own(sidebar_paths, this);
  taiMimeFactory_List::setInstance(&mime_factories);
  AddTemplates(); // note: ok that this will be called here, before subclass has finished its own
  AddDocs(); // note: ok that this will be called here, before subclass has finished its own
}

void taRootBase::CutLinks() {
  sidebar_paths.CutLinks();
  recent_paths.CutLinks();
  recent_files.CutLinks();
  objs.CutLinks();
  colorspecs.CutLinks();
  mime_factories.CutLinks();
  plugin_deps.CutLinks();
//TODO: we should save the plugin state!
  plugin_state.CutLinks();
  plugins.CutLinks();
  viewers_tmp.CutLinks();
  viewers.CutLinks();
  projects.CutLinks();
  wizards.CutLinks();
  templates.CutLinks();
  inherited::CutLinks();
}

#ifdef TA_OS_LINUX
int taRootBase::GetFEFlags(FPExceptFlags fpef) {
  int rval = 0;
  if (fpef & FPE_INEXACT) rval |= FE_INEXACT;
  if (fpef & FPE_DIVBYZERO) rval |= FE_DIVBYZERO;
  if (fpef & FPE_UNDERFLOW) rval |= FE_UNDERFLOW;
  if (fpef & FPE_OVERFLOW) rval |= FE_OVERFLOW;
  if (fpef & FPE_INVALID) rval |= FE_INVALID;
  return rval;
}

taRootBase::FPExceptFlags taRootBase::GetFPEFlags(int fef) {
  int rval = 0;
  if (fef & FE_INEXACT) rval |= FPE_INEXACT;
  if (fef & FE_DIVBYZERO) rval |= FPE_DIVBYZERO;
  if (fef & FE_UNDERFLOW) rval |= FPE_UNDERFLOW;
  if (fef & FE_OVERFLOW) rval |= FPE_OVERFLOW;
  if (fef & FE_INVALID) rval |= FPE_INVALID;
  return (FPExceptFlags)rval;
}
#endif

void taRootBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
#ifdef TA_OS_LINUX
  // just remove all, then set selected ones
  fedisableexcept(FE_ALL_EXCEPT);
  if (fpe_enable) {
    feenableexcept(GetFEFlags(fpe_enable));
  }
#endif
}

void taRootBase::MonControl(bool on) {
#ifdef GPROF                    // turn on for profiling
  taMisc::Info("Turning gprof profiler monitoring:", String(on));
  moncontrol(on);
}
#else
#ifdef HPCPROF_COMPILE
  if(on) {
    taMisc::Info("Turning on HPCToolkit profiler sampling");
    hpctoolkit_sampling_start();
  }
  else {
    hpctoolkit_sampling_stop();
    taMisc::Info("Turning off HPCToolkit profiler sampling");
  }
#else
  // nop
#endif
#ifdef CUDA_COMPILE
  if(on) {
    taMisc::Info("Turning on CUDA profiler sampling");
    cudaProfilerStart();
  }
  else {
    cudaProfilerStop();
    taMisc::Info("Turning off CUDA profiler sampling");
  }
#endif
#endif
}

void taRootBase::ClearRecentFiles() {
  String msg = "Remove all files listed in the recent files menu list?";
  String buttons = "Proceed" + iDialogChoice::delimiter + "Cancel";
  int chs = iDialogChoice::ChoiceDialog(NULL, msg, buttons);
  if (chs == 0) {
    recent_files.Reset();
  }
}

void taRootBase::CleanRecentFiles() {
  String msg = "Proceeding will remove all recover and autosave files in the recents list, the actual files as well as the entries in the recents list. Use with caution.";
  String buttons = "Proceed" + iDialogChoice::delimiter + "Cancel";
  int chs = iDialogChoice::ChoiceDialog(NULL, msg, buttons);
  if (chs == 0) {
    CleanNonexistentRecentFiles();
    for (int i=recent_files.size - 1; i >= 0; i--) {
      String file_fullpath = recent_files[i];
      if (file_fullpath.contains("_recover") || file_fullpath.contains("_autosave")) {
        if (QFile::remove(file_fullpath.toQString())) {  // only remove path from recents if file actually removed
          recent_files.RemoveIdx(i);
        }
      }
    }
  }
}

void taRootBase::CleanNonexistentRecentFiles() {
  for (int i=recent_files.size - 1; i >= 0; i--) {
    String file_fullpath = recent_files[i];
    String dir = taMisc::GetDirFmPath(file_fullpath);
    if(!taMisc::FileExists(file_fullpath) && taMisc::DirExists(dir)) {
      // note: only remove if dir exists b/c it could be on removable media and so
      // you don't want to nuke those guys
      recent_files.RemoveIdx(i);
    }
  }
}

void taRootBase::AddRecentFile(const String& value, bool no_save) {
  if (value.empty()) return; // oops...
  // never save for dmem>0
  no_save = no_save || (taMisc::dmem_proc > 0);
  bool save = AddRecentFile_impl(value);
  QFileInfo fi(value);
  String path = fi.path();
  if (AddRecentPath_impl(path))
    save = true;
  if (save && !no_save)
    Save();
}

void taRootBase::RemoveRecentFile(const String& value) {
  if (value.empty()) return; // oops...
  // never save for dmem>0
  RemoveRecentFile_impl(value); // but don't remove recent path
}

bool taRootBase::AddRecentFile_impl(const String& value) {
  // first, see if already there, if so, then just move it to the top
  if (taMisc::num_recent_files <= 0) {
    if (recent_files.size > 0) {
      recent_files.Reset();
      return true;
    } else return false;
  }

  CleanNonexistentRecentFiles();
  
  int idx = recent_files.FindEl(value);
  if (idx == 0) return false; // already at top, no need to save either!
  else if (idx > 0) {
    recent_files.MoveIdx(idx, 0);
  }
  else {
    // not there; if full, then nuke a guy
    if (recent_files.size >= taMisc::num_recent_files)
      recent_files.SetSize(taMisc::num_recent_files - 1);
    // insert it
    recent_files.Insert(value, 0);
  }
  return true;
}

void taRootBase::RemoveRecentFile_impl(const String& value) {
  int idx = recent_files.FindEl(value);
  if (idx > -1) {
    recent_files.RemoveIdx(idx);
  }
}

void taRootBase::AddRecentPath(const String& value, bool no_save) {
  if (value.empty()) return; // oops...
// never save for dmem>0
  no_save = no_save || (taMisc::dmem_proc > 0);
  if (AddRecentPath_impl(value) && !no_save)
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
  if (in_init) return false; // no spurious, and also suppresses for dmem
  ++taFiler::no_save_last_fname;

  // save the view state info into us..
  if(taMisc::gui_active) {
    if(taMisc::console_win) {
      taMisc::console_win->SaveGeom();
    }
    for (int i = 0; i < viewers.size; ++i) {
      MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
      if (!(vwr && vwr->isRoot())) continue;
      vwr->GetWinState();
      float lft = vwr->GetUserDataDef("view_win_lft", 0.0f).toFloat();
      float top = vwr->GetUserDataDef("view_win_top", 0.0f).toFloat();
      float wd = vwr->GetUserDataDef("view_win_wd", 1.0f).toFloat();
      float ht = vwr->GetUserDataDef("view_win_ht", 1.0f).toFloat();
      // these frame vals will not be in place when
      // window is first shown, so do it here..
      wd -= ((float)taiM->frame_s.w / (float)taiM->scrn_s.w);
      ht -= ((float)taiM->frame_s.h / (float)taiM->scrn_s.h);
      rootview_pos.SetXY(lft, top);
      rootview_size.SetXY(wd, ht);
      rootview_splits = vwr->GetUserDataAsString("view_splitter_state");
      iMainWindowViewer* imwv = vwr->widget();
    }
  }

  int rval = inherited::Save();
  --taFiler::no_save_last_fname;
  return rval;
}

int taRootBase::SavePluginState() {
//TODO:
// iterate the plugin_state collection, making a file for each guy
// in the user data area
  return 0;
}

int taRootBase::LoadPluginState() {
//TODO:
// iterate the plugin_state collection, loading data for each guy
// from the user data area
  return 0;
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
      // see if already listedbool              VerifyHasPlugins()
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
  info += "The Emergent Toolbox (TEMT) Info\n";
  info += "This is the TEMT software package, version: ";
  info += taMisc::version;
  info += "\n\n";
  info += "WWW Page: https://grey.colorado.edu/temt\n";
  info += "\n\n";

  info += "Copyright (c) 1995-2017, Regents of the University of Colorado,\n\
 Carnegie Mellon University, Princeton University.\n\
 \n\
 TEMT is free software; you can redistribute it and/or modify\n\
 it under the terms of the GNU General Public License as published by\n\
 the Free Software Foundation; either version 2 of the License, or\n\
 (at your option) any later version.\n\
 \n\
 TEMT is distributed in the hope that it will be useful,\n\
 but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
 GNU General Public License for more details.\n\
 \n\
 Note that the taString class was derived from the GNU String class\n\
 Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and\n\
 is covered by the GNU General Public License, see ta_string.h\n";
  if(taMisc::interactive)
    taMisc::Choice(info, "Ok");
  else
    taMisc::Info(info);
}
 
void taRootBase::AddTemplatesOfType(TypeDef* td) {
  if(!td->HasOption("VIRT_BASE") && td->GetInstance() != NULL) {
    taBase* tok = ((taBase*)td->GetInstance())->MakeToken();
    templates.Add(tok);
  }
  for(int i=0; i<td->children.size; i++) {
    TypeDef* chtd = td->children.FastEl(i);
    if(chtd->IsNotActual()) continue;
    AddTemplatesOfType(chtd);
  }
}

void taRootBase::AddTemplates() {
  templates.Add(new Program);
  AddTemplatesOfType(&TA_ProgEl); // get all prog els
  templates.Add(new ProgVar);
  templates.Add(new ControlPanel);
  templates.Add(new DataTable);
  templates.Add(new DynEnumType);
}

void taRootBase::AddDocs() {
  taDoc* doc = FindMakeDoc("web_home", "", taMisc::web_home);
  doc->auto_open = false; // true;
}

taDoc* taRootBase::FindMakeDoc(const String& nm, const String& wiki_nm, const String& web_url) {
  taDoc* rval = docs.FindName(nm);
  if(!rval) {
    rval = (taDoc*)docs.New(1);
    rval->SetName(nm);
  }
  if(web_url.nonempty()) {
    if(wiki_nm.nonempty()) {
      rval->wiki = wiki_nm;
      rval->url = web_url;
    }
    else {
      rval->SetURL(web_url);
    }
  }
  rval->UpdateAfterEdit();
  return rval;
}

taBase* taRootBase::FindGlobalObject(TypeDef* base_type,
    const String& name)
{
  for (int i = 0; i < objs.size; ++i) {
    taBase* rval = objs.FastEl(i);
    if (!rval || !rval->InheritsFrom(base_type))
      continue;
    if (name.empty() || (rval->GetName() == name))
      return rval;
  }
  return NULL;
}

void taRootBase::OpenRemoteServer(ushort port) {
  TemtServer* srv = (TemtServer*)FindGlobalObject(&TA_TemtServer);
  if (!srv) {
    srv = (TemtServer*)objs.New(1, &TA_TemtServer);
  }
  if (srv->isOpen()) return;
  srv->port = port;
  if (!srv->OpenServer()) {
    taMisc::Error("Could not open server");
  }

}

void taRootBase::CloseRemoteServer() {
  TemtServer* srv = (TemtServer*)FindGlobalObject(&TA_TemtServer);
  if (!srv) return; // if doesn't exist, don't create!
  if (!srv->isOpen()) return;
  srv->CloseServer();
}

bool taRootBase::IsServerOpen() {
  TemtServer* srv = (TemtServer*)FindGlobalObject(&TA_TemtServer);
  if (!srv)
    return false;
  if (!srv->isOpen())
    return false;
  return true;
}

taBase* taRootBase::GetTemplateInstance(TypeDef* typ) {
  return GetTemplateInstance_impl(typ, &templates);
}

taBase* taRootBase::GetTemplateInstance_impl(TypeDef* typ, taBase* base) {
  taBase* rval = NULL;
  TypeDef* btyp = base->GetTypeDef();
  if (btyp->name == typ->name)
    return base;

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
        (md->type->IsNotPtr()))
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
  FOREACH_ELEM_IN_GROUP(taProject, pr, projects) {
    pr->Save(); // does SaveAs if no filename
  }
}

void taRootBase::Options() {
#ifdef TA_GUI
  if (!taMisc::gui_active)
    return;
  taiEdit* ie =  TA_taMisc.ie;
  if (!ie) return;
  taMisc* inst = (taMisc*)TA_taMisc.GetInstance();
  
  taMisc::KeyBindingSet old_key_bindings = taMisc::current_key_bindings;
  
  int accepted = ie->EditDialog(inst, false, true, taiTypeBase::def_color,
                                800, 600); // r/w, modal, min width, height
  if (accepted) {
    inst->SaveConfig();
    
    if ((old_key_bindings == taMisc::KEY_BINDINGS_DEFAULT) && (taMisc::current_key_bindings == taMisc::KEY_BINDINGS_CUSTOM)) {
      taiMisc::LoadCustomKeyBindings();
    }
  }
#endif
}

void taRootBase::MakeWizards() {
  MakeWizards_impl();
}

void taRootBase::MakeWizards_impl() {
  // plugins
  wizards.New(1, &TA_StartupWizard, "StartupWizard");
  wizards.New(1, &TA_PluginWizard, "PluginWizard");
}


/////////////////////////////////////////
//      startup code


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

  taMisc::AddArgName("-nowin", "NoWin");
  taMisc::AddArgName("--nowin", "NoWin");
  taMisc::AddArgNameDesc("NoWin", "\
 -- does not open any windows, but does start the basic GUI infrastructure, as a way of doing offscreen rendering");

  taMisc::AddArgName("-cluster_run", "ClusterRun");
  taMisc::AddArgName("--cluster_run", "ClusterRun");
  taMisc::AddArgNameDesc("ClusterRun", "\
 -- indicates that this job is being run as result of ClusterRun submission -- sets the taMisc::cluster_run flag and populates the ClusterRunJob::cur_job with info about this job, and uses job info to save state, etc");

  taMisc::AddArgName("-attachwait", "AttachWait");
  taMisc::AddArgName("--attachwait", "AttachWait");
  taMisc::AddArgName("attachwait=", "AttachWait");
  taMisc::AddArgNameDesc("AttachWait", "\
 -- after startup, before event loop, enter a wait loop so that you can attach to process with gdb, and then debug it -- very useful for dmem (MPI) debugging -- when passed with a value, value is process number to wait (otherwise all wait) -- after attaching to process, do set var i = 1 to get out of loop");

  taMisc::AddArgName("-a", "AppDir");
  taMisc::AddArgName("--app_dir", "AppDir");
  taMisc::AddArgName("app_dir=", "AppDir");
  taMisc::AddArgNameDesc("AppDir", "\
 -- explicitly specifies location of the app directory (prog libs, etc.)");

  taMisc::AddArgName("-ap", "AppPluginDir");
  taMisc::AddArgName("--app_plugin_dir", "AppPluginDir");
  taMisc::AddArgName("app_plugin_dir=", "AppPluginDir");
  taMisc::AddArgNameDesc("AppPluginDir", "\
 -- explicitly specifies location of the System plugin directory");

  taMisc::AddArgName("-version", "Version");
  taMisc::AddArgName("--version", "Version");
  taMisc::AddArgNameDesc("Version", "\
 -- Prints out version and other information");

  taMisc::AddArgName("-h", "Help");
  taMisc::AddArgName("--help", "Help");
  taMisc::AddArgNameDesc("Help", "\
 -- Prints out help on startup arguments and other usage information");

  taMisc::AddArgName("-gendoc", "GenDoc");
  taMisc::AddArgName("--gendoc", "GenDoc");
  taMisc::AddArgNameDesc("GenDoc", "\
 -- generates documentation source information in XML based on type information scanned by maketa");

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

  taMisc::AddArgName("-i", "Interactive");
  taMisc::AddArgName("--interactive", "Interactive");
  taMisc::AddArgNameDesc("Interactive", "\
 -- Specifies that the console should remain active after running a css script file upon startup (default is otherwise to be non-interactive), quitting after scripts complete");

  taMisc::AddArgName("-ni", "NonInteractive");
  taMisc::AddArgName("--non-interactive", "NonInteractive");
  taMisc::AddArgNameDesc("NonInteractive", "\
 -- Specifies that the console should NOT be activated at all during running and no prompts will be made of the user -- only useful if a STARTUP_RUN program is present that will run and then quit when done -- use this for running in the background)");

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

  taMisc::AddArgName("-up", "UserPluginDir");
  taMisc::AddArgName("--user_plugin_dir", "UserPluginDir");
  taMisc::AddArgName("user_plugin_dir=", "UserPluginDir");
  taMisc::AddArgNameDesc("UserPluginDir", "\
 -- explicitly specifies location of user plugin folder (should normally not need to override)");

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

  taMisc::AddArgName("-gen_prng_params", "GenPrngParams");
  taMisc::AddArgName("--gen_prng_params", "GenPrngParams");
  taMisc::AddArgNameDesc("GenPrngParams", "\
 -- generate new pseudo-random-number-generator parameters for threads -- saves in file: 'new_mtrndpars.txt'");

  ////////////////////////////////////////////////////
  //    All the multi-threading stuff has standard default startup args

  taMisc::AddArgName("--max_cpus", "MaxCpus");
  taMisc::AddArgName("max_cpus=", "MaxCpus");
  taMisc::AddArgNameDesc("MaxCpus", "\
 -- Maximum number of cpus -- this should normally be detected automatically, but in case it isn't -- see n_threads for the actual number to use");

  taMisc::AddArgName("--n_threads", "NThreads");
  taMisc::AddArgName("n_threads=", "NThreads");
  taMisc::AddArgNameDesc("NThreads", "\
 -- Target number of threads to use in multi-threaded code -- should be <= max_cpus and it often is more efficient to use less than what is maximally available");

  ////////////////////////////////////////////////////
  //    Server variables

  taMisc::AddArgName("--server", "Server");
  taMisc::AddArgName("-server", "Server");
  taMisc::AddArgNameDesc("Server", "\
 -- Run the app as a tcp server");

  taMisc::AddArgName("--port", "Port");
  taMisc::AddArgName("-port", "Port");
  taMisc::AddArgName("port=", "Port");
  taMisc::AddArgNameDesc("Port", "\
 -- Specifies the tcp port for server mode (def=5360");


  ////////////////////////////////////////////////////
  //    Plugin making

  taMisc::AddArgName("--no_plugins", "NoPlugins");
  taMisc::AddArgNameDesc("NoPlugins", "\
 -- do not load any plugins -- can be useful if some plugins are misbehaving -- may be better to re-make all the plugins (--make_all_plugins) to fix plugin problems though");

  taMisc::AddArgName("--make_all_plugins", "MakeAllPlugins");
  taMisc::AddArgName("--make_plugins", "MakeAllPlugins");
  taMisc::AddArgNameDesc("MakeAllPlugins", "\
 -- (re)make all the plugins located in the user AND system plugin directories -- these are typically installed with a make install from wherever original source is located, and source is installed to same plugin directory -- make will make from this installed source");

  taMisc::AddArgName("--make_all_user_plugins", "MakeAllUserPlugins");
  taMisc::AddArgNameDesc("MakeAllUserPlugins", "\
 -- (re)make all the plugins located in the user plugin directory -- these are typically installed with a make install from wherever original source is located, and source is installed to user plugin directory -- make will make from this installed source");

  taMisc::AddArgName("--make_all_system_plugins", "MakeAllSystemPlugins");
  taMisc::AddArgNameDesc("MakeAllSystemPlugins", "\
 -- (re)make all the plugins located in the system plugin directory -- these are typically installed with a make install from wherever original source is located, and source is installed to system plugin directory -- make will make from this installed source");

  taMisc::AddArgName("--clean_all_plugins", "CleanAllPlugins");
  taMisc::AddArgName("--clean_plugins", "CleanAllPlugins");
  taMisc::AddArgNameDesc("CleanAllPlugins", "\
 -- clean (remove) all the plugins (just the library files, not the source) located in the user AND system plugin directories");

  taMisc::AddArgName("--clean_all_user_plugins", "CleanAllUserPlugins");
  taMisc::AddArgNameDesc("CleanAllUserPlugins", "\
 -- clean (remove) all the plugins (just the library files, not the source) located in the user plugin directory");

  taMisc::AddArgName("--clean_all_system_plugins", "CleanAllSystemPlugins");
  taMisc::AddArgNameDesc("CleanAllSystemPlugins", "\
 -- clean (remove) all the plugins (just the library files, not the source) located in the system plugin directory");

  taMisc::AddArgName("--make_user_plugin", "MakeUserPlugin");
  taMisc::AddArgName("make_user_plugin=", "MakeUserPlugin");
  taMisc::AddArgNameDesc("MakeUserPlugin", "\
 -- (re)make specified plugin located in the user plugin directory -- these are typically installed with a make install from wherever original source is located, and source is installed to user plugin directory -- make will make from this installed source");

  taMisc::AddArgName("--make_system_plugin", "MakeSystemPlugin");
  taMisc::AddArgName("make_system_plugin=", "MakeSystemPlugin");
  taMisc::AddArgNameDesc("MakeSystemPlugin", "\
 -- (re)make specified plugin located in the system plugin directory -- these are typically installed with a make install from wherever original source is located, and source is installed to system plugin directory -- make will make from this installed source");

  taMisc::AddArgName("--list_plugins", "ListAllPlugins");
  taMisc::AddArgNameDesc("ListAllPlugins", "\
 -- list all of the plugins and their current status.");

  taMisc::AddArgName("--enable_all_plugins", "EnableAllPlugins");
  taMisc::AddArgNameDesc("EnableAllPlugins", "\
 -- mark all the available plugins as enabled -- useful for batch run environments where you cannot enable them via the gui.");

  taMisc::AddArgName("--new_src", "CreateNewSrc");
  taMisc::AddArgName("new_src=", "CreateNewSrc");
  taMisc::AddArgNameDesc("CreateNewSrc", "\
 -- create new source code files for given type name -- does basic formatting and adds to svn and creates new include entries and adds to CMakeLists.txt -- must run in directory where you want to create the new source files!");

  taMisc::AddArgName("--new_spec", "CreateNewSpec");
  taMisc::AddArgName("new_spec=", "CreateNewSpec");
  taMisc::AddArgNameDesc("CreateNewSpec", "\
 -- create new Spec source code files for given type name -- including _core _mbrs _cpp _cuda and plain .h, .cpp -- does basic formatting and adds to svn and creates new include entries and adds to CMakeLists.txt -- must run in directory where you want to create the new source files!");

  taMisc::AddArgName("--new_state", "CreateNewState");
  taMisc::AddArgName("new_state=", "CreateNewState");
  taMisc::AddArgNameDesc("CreateNewState", "\
 -- create new State source code files for given type name -- including _core _cpp _cuda .h, .cpp -- does basic formatting and adds to svn and creates new include entries and adds to CMakeLists.txt -- must run in directory where you want to create the new source files!");

  taMisc::AddArgName("--rename_type", "RenameType");
  taMisc::AddArgName("rename_type=", "RenameType");
  taMisc::AddArgName("--rename_to", "RenameTypeTo");
  taMisc::AddArgName("rename_to=", "RenameTypeTo");
  taMisc::AddArgNameDesc("RenameType", "\
 -- rename type to a new name -- renames file, include file, renames all references in current directory -- must run in directory where you want to do the rename -- must pass rename_to as the target name to rename type to");

  taMisc::AddArgName("--replace_string", "ReplaceString");
  taMisc::AddArgName("replace_string=", "ReplaceString");
  taMisc::AddArgName("--replace_to", "ReplaceStringTo");
  taMisc::AddArgName("replace_to=", "ReplaceStringTo");
  taMisc::AddArgNameDesc("ReplaceString", "\
 -- replace string to new replacement value in all files in current directory -- must run in directory where you want to do the replace -- must pass replace_to as the replacement string");

  taMisc::AddArgName("--remove_type", "RemoveType");
  taMisc::AddArgName("remove_type=", "RemoveType");
  taMisc::AddArgNameDesc("RemoveType", "\
 -- remove existing type -- removes files with name of type, and reports on any files that still refer to that type -- DOES NOT ASK FOR CONFIRMATION -- USE WITH EXTREME CAUTION -- must run in directory where you want to do the rename -- must pass rename_to as the target name to rename type to");

  taMisc::AddArgName("--copyrightify", "CopyrightUpdate");
  taMisc::AddArgName("copyrightify=", "CopyrightUpdate");
  taMisc::AddArgNameDesc("CopyrightUpdate", "\
 -- update the copyright year on specified file or all files in the source code if none specified");

  taMisc::Init_Args(argc, argv);
  return true;
}

bool taRootBase::Startup_ProcessGuiArg(int argc, const char* argv[]) {
#ifdef TA_GUI
  taMisc::use_gui = true;
#else
  taMisc::use_gui = false;
#endif

  taMisc::interactive = true;   // default to true

  // process gui flag right away -- has other implications
  if(taMisc::CheckArgByName("GenDoc") || taMisc::CheckArgByName("Version")
     || taMisc::CheckArgByName("Help")) {
    taMisc::use_plugins = false;                      // no need for plugins for these..
    taMisc::use_gui = false;
    taMisc::interactive = false;
  }

  if(taMisc::CheckArgByName("MakeAllUserPlugins")
     || taMisc::CheckArgByName("MakeAllSystemPlugins")
     || taMisc::CheckArgByName("MakeAllPlugins")
     || taMisc::CheckArgByName("CleanAllPlugins")
     || taMisc::CheckArgByName("CleanAllUserPlugins")
     || taMisc::CheckArgByName("CleanAllSystemPlugins")
     || taMisc::CheckArgByName("CleanAllPlugins")
     || taMisc::CheckArgByName("MakeUserPlugin")
     || taMisc::CheckArgByName("MakeSystemPlugin")
     || taMisc::CheckArgByName("GenPrngParams")
     || taMisc::CheckArgByName("CreateNewSrc")
     || taMisc::CheckArgByName("CreateNewSpec")
     || taMisc::CheckArgByName("RenameType")
     || taMisc::CheckArgByName("ReplaceString")
     || taMisc::CheckArgByName("RemoveType")
     || taMisc::CheckArgByName("CopyrightUpdate")
     ) { // auto nogui by default
    taMisc::use_plugins = false;                      // don't use if making
    taMisc::use_gui = false;
    taMisc::interactive = false;
  }

  if(taMisc::CheckArgByName("CssScript") || taMisc::CheckArgByName("CssCode")) {
    // if code specified, default is then to not run in interactive mode
    taMisc::interactive = false;
  }

  if(taMisc::CheckArgByName("ListAllPlugins")
     || taMisc::CheckArgByName("EnableAllPlugins")) {
    taMisc::use_gui = false;
    taMisc::interactive = false;
  }

  if(taMisc::CheckArgByName("NoPlugins"))
    taMisc::use_plugins = false;                      // don't use if making

  // need to use Init_Args and entire system because sometimes flags get munged together
  if(taMisc::CheckArgByName("NoGui")) {
    taMisc::use_gui = false;
  }
  else if(taMisc::CheckArgByName("Gui")) {
    taMisc::use_gui = true;
  }

  if(taMisc::CheckArgByName("NonInteractive")) {
    taMisc::interactive = false;
  }
  else if(taMisc::CheckArgByName("Interactive")) {
    taMisc::interactive = true;
  }

  if(taMisc::CheckArgByName("NoWin"))
    taMisc::gui_no_win = true;
  else
    taMisc::gui_no_win = false;

  if(taMisc::CheckArgByName("ClusterRun")) {
    taMisc::cluster_run = true;
  }
  
#ifndef TA_GUI
  if(taMisc::use_gui) {
    taMisc::Error("Startup_InitArgs: cannot specify '-gui' switch when compiled without gui support");
    return false;
  }
#endif
  return true;
}

#ifndef TA_QT3D
static CoinImageReaderCB* coin_image_reader_cb_obj = NULL;
#endif // TA_QT3D

#if defined(__APPLE__) 
extern void TemtMacDefaultSettings();
#endif

bool taRootBase::Startup_InitApp(int& argc, const char* argv[]) {

#ifdef TA_GUI
  if(taMisc::use_gui) {
    // When QNetworkAccessManager is instantiated it regularly starts polling 
    // all network interfaces to see if anything changes and if so, what. This 
    // creates a latency spike every 10 seconds on Mac OS 10.12+ and Windows 7 >=
    // when on a wifi connection. 
    // So here we disable it for lack of better measure.
    // This will also cause this message: QObject::startTimer: Timers cannot 
    // have negative intervals
    // For more info see:
    // - https://bugreports.qt.io/browse/QTBUG-40332
    // - https://bugreports.qt.io/browse/QTBUG-46015
    qputenv("QT_BEARER_POLL_TIMEOUT", QByteArray::number(-1));

#if defined(__APPLE__) 
    TemtMacDefaultSettings();
#endif    
    new taApplication(argc, (char**)argv); // accessed as qApp
#ifdef TA_QT3D
    milestone |= SM_QAPP_OBJ;
#else // TA_QT3D
    SIM::Coin3D::Quarter::Quarter::init();
    milestone |= (SM_QAPP_OBJ | SM_SOQT_INIT);
#endif // TA_QT3D

#ifdef QT_OPEN_GL_WIDGET
    // this is synonymous with QT_VERSION >= 0x050600
    QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
    if(taMisc::antialiasing_level > 1) {
      fmt.setSamples(taMisc::antialiasing_level);
    }
#ifdef TA_QT3D
#else
    // set a default format that is quarter compatible
    // fmt.setAlphaBufferSize(8); // this makes network transparent to screen below!
    fmt.setProfile(QSurfaceFormat::CompatibilityProfile); // quarter requires it
#endif // TA_QT3D
    // note: setting this default format here fixes the overly-washed-out extra transparency
    // on mac!  hmm.
    QSurfaceFormat::setDefaultFormat(fmt);
    if(fmt.majorVersion() < 2 || fmt.profile() == QSurfaceFormat::NoProfile) {
      cerr << "This display likely does NOT have a proper level of OpenGL support (version < 2)\n"
           << "OpenGL is required for 3D displays!\n"
           << "Please read the emergent manual for required 3D graphics driver information.\n"
           << "If you open a project with a 3D display, or create one, the program will likely crash!"
           << endl;
    }
    // test for various GL compatibilities now, before we get bitten later!
#else // QT_OPEN_GL_WIDGET
    if(!QGLFormat::hasOpenGL()) {
      cerr << "This display does NOT have OpenGL support, which is required for 3d displays!\n"
           << "Please read the emergent manual for required 3D graphics driver information.\n"
           << "If you open a project with a 3D display, or create one, the program will likely crash!"
           << endl;
    }
#endif
    
#ifndef TA_QT3D
# if COIN_MAJOR_VERSION >= 3
    // this installs the callback to eliminate dependency on simage
    coin_image_reader_cb_obj = new CoinImageReaderCB;
# endif
#endif
  } else
#endif // TA_GUI
  {
    new QCoreApplication(argc, (char**)argv); // accessed as qApp
    QFileInfo fi(argv[0]);
    milestone |= SM_QAPP_OBJ;
  }
  QCoreApplication::instance()->setApplicationName(taMisc::app_name);
#ifdef TA_OS_MAC
  // this is an essential fix for having an OpenGL widget along with other widgets
  // https://bugreports.qt-project.org/browse/QTBUG-28816
  QCoreApplication::instance()->setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
  // does this fix the touchy trackpad problem?? probably!
#if (QT_VERSION >= 0x050000)
  QCoreApplication::instance()->setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
#endif
#endif
  // probably as good a place as any to init ODE
  dInitODE();

  // dealing with bug 1290 - decimal symbol issue
  setlocale(LC_NUMERIC, "C");
  return true;
}

// helper functions for taRootBase::Startup_InitTA_AppFolders()
namespace { // anon
  // Determine if a dir is a plugin dir.
  bool IsPluginDir(const String& path) {
    return QDir(path).exists();
  }

  // Determine if a dir is an app dir.
  bool IsAppDir(const String& path, String* plugin_path = NULL) {
    // First check if the path contains a prog_lib subdirectory.
    QDir dir(path);
    if (dir.exists("prog_lib")) {
      if (plugin_path) {
        // If requested, check if there is a plugin subdirectory; if so,
        // set the out-parameter to it (valid for dev installs).
        String plugin_dir = taMisc::GetSysPluginDir();
        if (dir.exists(plugin_dir)) {
          *plugin_path = path + PATH_SEP + plugin_dir;
        }
        else {
          // Doesn't disqualify the directory as an app dir, so just inform.
          taMisc::DebugInfo("Did not find", plugin_dir, "subdir in", path);
        }
      }

      return true;
    }

    taMisc::DebugInfo("Did not find app_dir as:", path);
    return false;
  }

  // Setup the global variables taMisc::exe_cmd and taMisc::exe_path and taMisc::app_suffix
  void InitExecCmdPath()
  {
    taMisc::exe_cmd = taMisc::GetFileFmPath(taMisc::args_raw.SafeEl(0));
    if(taMisc::exe_cmd.contains('_')) {
      taMisc::app_suffix = taMisc::exe_cmd.from('_');
      // don't use -1 -- some suffixes have _ in them (e.g., version numbers) and
      // the app name hopefully does not have a suffix..
    }
    else if(isdigit(taMisc::exe_cmd.lastchar())) { // emergent7 etc
      taMisc::app_suffix = taMisc::exe_cmd.lastchar();
    }
    else {
      taMisc::app_suffix = "";
    }
#ifdef TA_OS_WIN
    if(taMisc::app_suffix.endsWith(".exe")) {
      taMisc::app_suffix = taMisc::app_suffix.before(".exe");
    }
    if(!taMisc::exe_cmd.endsWith(".exe")) {
      taMisc::exe_cmd += ".exe";
    }
#endif

    taMisc::exe_path = QCoreApplication::applicationDirPath();

    String full_exe = taMisc::exe_path + PATH_SEP + taMisc::exe_cmd;

    QFileInfo fi(full_exe);
    if(!fi.exists()) {
      cerr << "\nWARNING: Not able to find executable at place we think it is:\n"
           << full_exe << "\n"
           << "this means that various things like checking for out-of-date plugins\n"
           << "and detecting if we're running the develoment executable won't work.\n";
    }
    else {
      taMisc::exe_mod_time_int = fi.lastModified().toTime_t();
      taMisc::exe_mod_time = fi.lastModified().toString();

      taMisc::exe_path = fi.absolutePath();

#if defined(TA_OS_MAC)
      /* Note: for Mac, if the bin is in a bundle, then it will be a link
         to the actual file, so in this case, we dereference it
         {app_dir}/{appname.app}/Contents/MacOS (bundle in app root)
         {app_dir}/bin/{appname.app}/Contents/MacOS (bundle in app bin)
         {app_dir}/bin (typically non-gui only, since gui must run from bundle)
      */
      if (taMisc::exe_path.endsWith("/Contents/MacOS")) {
        taMisc::exe_path = fi.canonicalPath();
      }
#endif // Mac
    }
  }

#ifndef TA_OS_WIN
  // Paths to search for an emergent installation.
  const char* DEF_PREFIXES[] = { "/usr/local", "/usr", "/opt/local", "/opt" };
  const int DEF_PREFIX_N = 4;
#endif

  // Determine the application directory to use.  Return false on failure.
  // On success, return true, and set app_dir out parameter.  Out parameter
  // app_plugin_dir may be set for in-place contexts (Windows and dev).
  // Out parameter prefix_dir may be set for non-windows platforms.
  // Side effect: sets taMisc::in_dev_exe and taMisc::use_plugins.
  bool GetAppDir(String &app_dir, String &app_plugin_dir, String &prefix_dir)
  {
    // Search for app path in following order:
    // 1. app_dir command line switch (may require app_plugin_dir switch too)
    // 2. "in-place" development (this is either the same as
    //    or never conflicts with the installed production version)
    // 3. EMERGENTDIR (Windows) or EMERGENT_PREFIX_DIR (Unix) variable
    // 4. a platform-specific heuristic search
    // NOTE: for "in-place" contexts, plugin dir is local, else look independently

    app_dir = taMisc::FindArgByName("AppDir");
    if (app_dir.nonempty() && IsAppDir(app_dir)) {
      return true;
    }

  #ifdef TA_OS_WIN

    // {app_dir}\bin\xxx (MSVS build)
    // {app_dir}\bin (normal release, nmake build)
    String bin_dir = taMisc::exe_path; // tmp to work on..
    bin_dir.gsub("/", "\\");
    if (bin_dir.contains("\\bin")) {
      if (bin_dir.contains("\\build")) {
        app_dir = bin_dir.before("\\build");
        if (IsAppDir(app_dir, &app_plugin_dir)) {
          taMisc::Info("Note: running development executable: not loading plugins.");
          taMisc::in_dev_exe = true;
          taMisc::use_plugins = false;
          return true;
        }
      }

      app_dir = bin_dir.before("\\bin");
      if (IsAppDir(app_dir, &app_plugin_dir)) {
        return true;
      }
    }

    app_dir = getenv("EMERGENTDIR");
    if (app_dir.nonempty() && IsAppDir(app_dir)) {
      return true;
    }

    if (bin_dir.endsWith("\\bin")) {
      app_dir = bin_dir.at(0, bin_dir.length() - 4);
      if (IsAppDir(app_dir)) {
        return true;
      }
    }

  #else // Unix/Mac

    // {app_dir}/build[{SUFF}]/bin (cmake development) TEST FIRST!
    // {app_dir}/bin (legacy development)
    // {prefix_dir}/bin (standard Unix deployment)
    String bin_dir = taMisc::exe_path; // tmp to work on..
    if (bin_dir.endsWith("/bin")) {
      if (bin_dir.contains("/build")) {
        app_dir = bin_dir.before("/build");
        if (IsAppDir(app_dir, &app_plugin_dir)) {
          taMisc::Info("Note: running development executable: not loading plugins.");
          taMisc::in_dev_exe = true;
          taMisc::use_plugins = false;
          return true;
        }
      }

      // always try to find share install guy first
      String tmp_dir = bin_dir.at(0, bin_dir.length() - 4);
      app_dir = tmp_dir + "/share/" + taMisc::default_app_install_folder_name;
      if (IsAppDir(app_dir, &app_plugin_dir)) {
        return true;
      }

      app_dir = tmp_dir;
      if (IsAppDir(app_dir, &app_plugin_dir)) {
        return true;
      }
    }

    // Build a list of directories to search:
    // * environment variable (if present)
    // * the prefix implied by bin_dir (if ends with /bin)
    // * the usual locations (/usr/local etc.)
    std::vector<String> search_prefixes;
    String temp = getenv("EMERGENT_PREFIX_DIR");
    if (!temp.empty()) {
      search_prefixes.push_back(temp);
    }
    if (bin_dir.endsWith("/bin")) {
      search_prefixes.push_back(bin_dir.before("/bin"));
    }
    search_prefixes.insert(
      search_prefixes.end(), DEF_PREFIXES, DEF_PREFIXES + DEF_PREFIX_N);

    // Search the directories.
    for (int i = 0; i < search_prefixes.size(); ++i) {
      prefix_dir = search_prefixes[i];
      app_dir = prefix_dir + "/share/" + taMisc::default_app_install_folder_name;
      if (IsAppDir(app_dir)) {
        // Note: the prefix_dir that successfully produced app_dir will
        // be returned by out-parameter.
        return true;
      }
    }

  #endif // Unix/Mac

    // No valid app_dir found, so clear out-parameters.
    app_dir = _nilString;
    app_plugin_dir = _nilString;
    prefix_dir = _nilString;
    return false;
  }

  bool GetAppPluginDir(String &app_plugin_dir, const String &prefix_dir) {
    // We search for plugin path in following order:
    // 1. app_plugin_dir command line switch
    // 2. previously established "in-place" location (only for Windows
    //    and dev contexts)
    // 3. EMERGENT_PLUGIN_DIR variable
    // 4. Unix/Mac: {EMERGENT_PREFIX_DIR}/lib/Emergent (if env var set)
    // 5. a platform-specific heuristic search
    // NOTE: for "in-place" contexts, plugin dir is local, else look independently

    // Check for command line arg.
    String app_plugin_dir_cmd_line = taMisc::FindArgByName("AppPluginDir");
    if (app_plugin_dir_cmd_line.nonempty()) {
      app_plugin_dir = app_plugin_dir_cmd_line;
      return true;
    }

    // Check "in-place" location.
    if (app_plugin_dir.nonempty()) {
      return true;
    }

    // Check environment variable.
    app_plugin_dir = getenv("EMERGENT_PLUGIN_DIR");
    if (app_plugin_dir.nonempty()) {
      return true;
    }

  #ifdef TA_OS_WIN
    // This directory was created when emergent was installed.
    app_plugin_dir = taMisc::app_dir + "\\" + taMisc::GetSysPluginDir();
    return true;
  #else // Unix/Mac
    // Only got here because no command line arg, no in-place location,
    // and no environment variable.

    // Build a list of directories to search:
    // * default prefix previously discovered
    // * the usual locations (/usr/local etc.)
    std::vector<String> search_prefixes;
    if (!prefix_dir.empty()) {
      search_prefixes.push_back(prefix_dir);
    }
    search_prefixes.insert(
      search_prefixes.end(), DEF_PREFIXES, DEF_PREFIXES + DEF_PREFIX_N);

    // Search the directories.
    for (int i = 0; i < search_prefixes.size(); ++i) {
      app_plugin_dir = search_prefixes[i] + "/lib/" +
        taMisc::default_app_install_folder_name + "/" + taMisc::GetSysPluginDir();
      if (IsPluginDir(app_plugin_dir)) {
        return true;
      }
    }

    // App plugin directory not found.
    app_plugin_dir = _nilString;
    return false;
  #endif // Unix/Mac
  }
}

// hairy, modal, issue-prone -- we put in its own routine
// Sets:
// * taMisc::exe_cmd
// * taMisc::exe_path
// * taMisc::app_dir
// * taMisc::in_dev_exe
// * taMisc::use_plugins
bool taRootBase::Startup_InitTA_AppFolders() {
  // Initialize the key folders.
  String app_dir;
  String app_plugin_dir;
  String prefix_dir; // only applicable for mac/unix.

  // Find the application directory (and if possible, also get the
  // plugin directory and prefix for the application directory).
  bool found = GetAppDir(app_dir, app_plugin_dir, prefix_dir);

  // If found, set taMisc::app_dir.
  if (found) {
    taMisc::app_dir = app_dir;
    #ifdef TA_OS_WIN
      taMisc::app_dir.gsub("/", "\\"); // clean it up, so it never causes issues
    #endif
  }
  else {
    // inability to find the app is fatal in 4.0.19
    taMisc::Error(
      "The application install directory could not be found. Please see:\n"
      "https://grey.colorado.edu/emergent/index.php/User_Guide\n"
      "for instructions on setting command line switches and/or environment\n"
      "variables for non-standard installations of the application.\n");

    // use a Choice so console etc. doesn't disappear immediately, ex. on Windows
    taMisc::Choice("The application will now terminate.");
    return false;
  }

  // Determine which directory to use as the plugin directory.
  found = GetAppPluginDir(app_plugin_dir, prefix_dir);

  // If no plugin directory found, warn the user.
  if (!found || !IsPluginDir(app_plugin_dir)) {
    taMisc::Error("Expected application plugin folder",
      app_plugin_dir, "does not exist! You should check your installation "
      "and/or create this folder, otherwise runtime errors may occur.");
    // Missing plugin directory isn't fatal, so continue on.
  }

  taMisc::app_plugin_dir = app_plugin_dir;
  return true;
}

bool taRootBase::Startup_InitTA_InitUserAppDir() {
  // make sure the folder exists
  // make sure the standard user subfolders exist:
  QDir dir(taMisc::user_app_dir);
  if (!dir.exists()) {
    if (!dir.mkpath(taMisc::user_app_dir)) {
      taMisc::Error("Could not find or make the user dir:", taMisc::user_app_dir,
        "Please make sure this directory exists and is readable, and try again"
        " -- or use the -UserAppDir= command line option.");
      return false;
    }
  }
  //NOTE: we could get excessively anal, and check all of these, but if we
  // can make/read the user folder, then very unlikely will these fail
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + "css_lib");
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + "log");
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + taMisc::GetUserPluginDir());
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + "prefs");
  dir.mkpath(taMisc::user_app_dir + PATH_SEP + "prog_lib");
  return true;
}

bool taRootBase::Startup_InitTA() {
  // first initialize the type data from maketa generated files, registered here:
 
  taMisc::Init_Defaults_PreLoadConfig(); // needs to have this early!

  taMisc::Init_Types();

  taMisc::Init_Hooks(); // client dlls register init hooks -- this calls them!
  milestone |= SM_TYPES_INIT;

  // Set taMisc::exe_cmd and taMisc::exe_path -- need these to determine app_suffix
  // which is key to everything else..
  InitExecCmdPath();

  // user directory, aka Home folder -- we don't necessarily use it as a base here though
  // cmd line override of UserDir takes preference
  taMisc::user_dir = taMisc::FindArgByName("UserDir");
  if (taMisc::user_dir.empty()) {
    taMisc::user_dir = taMisc::GetHomePath();
  }

  // Application folder
  // env var overrides default
  String user_app_dir_env_var = upcase(taMisc::app_prefs_key) + "_USER_APP_DIR";
  String user_app_dir = getenv(user_app_dir_env_var);
  if (user_app_dir.empty()) {
    user_app_dir = taMisc::GetAppDocPath(taMisc::app_prefs_key);
  }
  taMisc::user_app_dir = user_app_dir;
  if (!Startup_InitTA_InitUserAppDir()) return false;

  // Preferences directory
  taMisc::prefs_dir = taMisc::user_app_dir + PATH_SEP + "prefs";
  // then load configuration info: sets lots of user-defined config info
  ((taMisc*)TA_taMisc.GetInstance())->LoadConfig();
  // ugh: reload because legacy options file will load its value
  taMisc::user_app_dir = user_app_dir;

  taMisc::user_plugin_dir = taMisc::user_app_dir + PATH_SEP +
    taMisc::GetUserPluginDir();
  taMisc::user_log_dir = taMisc::user_app_dir + PATH_SEP + "log";

  // System (Share) Folder, System Plugins
  if (!Startup_InitTA_AppFolders()) return false;

  taMisc::Init_Defaults_PostLoadConfig();

  console_options = taMisc::console_options;

  taMisc::default_scope = &TA_taProject; // this is general default

  // load prefs values for us
  taRootBase* inst = instance();
  milestone |= SM_ROOT_CREATE;
  inst->SetFileName(taMisc::prefs_dir + "/root" + taMisc::app_suffix);
  if (QFile::exists(inst->GetFileName())) {
    ++taFiler::no_save_last_fname;
    inst->Load();
    --taFiler::no_save_last_fname;
  }
  else {
    // try without the app suffix
    String good_fnm = inst->GetFileName();
    inst->SetFileName(taMisc::prefs_dir + "/root");
    if (QFile::exists(inst->GetFileName())) {
      ++taFiler::no_save_last_fname;
      inst->Load();
      --taFiler::no_save_last_fname;
    }
    inst->SetFileName(good_fnm); // reinstate for later saving
  }

#ifdef TA_GUI
  if(taMisc::use_gui) {
    // get optional style override
    QString app_ico_nm = ":/images/" + taMisc::app_name + "_32x32.png";
    QPixmap app_ico(app_ico_nm);
    QApplication::setWindowIcon(app_ico);
  }
#endif // TA_GUI

  // make sure the app dir is on the recent paths
  if (instance()->recent_paths.FindEl(taMisc::app_dir) < 0) {
    instance()->AddRecentPath(taMisc::app_dir);
  }

  // and sidebar paths
  instance()->sidebar_paths.AddUnique(taMisc::app_dir);
#if (QT_VERSION >= 0x050000)
  String desktop_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
  String docs_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  String desktop_path = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  String docs_path = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif

  instance()->sidebar_paths.AddUnique(desktop_path);
  instance()->sidebar_paths.AddUnique(docs_path);

  MTRnd::LoadInitParams();

  // start recording stuff at this point -- only with gui active
  if(taMisc::use_gui) {
    taMisc::SetLogFileToDefault();
  }
  return true;
}

bool taRootBase::Startup_EnumeratePlugins() {
  if (!taMisc::use_plugins) return true;
  String plug_log = "plugins" + taMisc::app_suffix + ".log";

  // add plugin folders
  taPlugins::AddPluginFolder(taMisc::app_plugin_dir);
  taPlugins::AddPluginFolder(taMisc::user_plugin_dir);

  taPlugins::InitLog(taMisc::user_log_dir + PATH_SEP + plug_log);
  taPlugins::EnumeratePlugins();

  if(taMisc::CheckArgByName("ListAllPlugins")) {
    tabMisc::root->plugins.ListAllPlugins();
  }
  if(taMisc::CheckArgByName("EnableAllPlugins")) {
    tabMisc::root->plugins.EnableAllPlugins();
    --in_init;                  // allow it to save!
    tabMisc::root->Save();      // save after enabling
    ++in_init;
  }
  return true;
}

bool taRootBase::Startup_LoadPlugins() {
  if (!tabMisc::root) return false; // should be made
  if (!taMisc::use_plugins) return true;
  tabMisc::root->plugins.LoadPlugins();
  return true;
}

bool taRootBase::Startup_InitCss() {
  return cssMisc::Initialize();
}

bool taRootBase::Startup_InitGui() {
#ifdef TA_GUI
  if(taMisc::use_gui) {
    taiM_ = taiMisc::New(taMisc::use_gui);
    taiMC_ = taiM_;
    // the following should be done in the specific app's Main function
//     taiM->icon_bitmap = new QBitmap(emergent_bitmap_width,
//      emergent_bitmap_height, emergent_bitmap_bits);
//    qApp->setWindowIcon(QIcon(*(taiM->icon_bitmap)));

    taMisc::gui_active = true;        // officially active!
    // gui_no_win will turn OFF gui_active after creating the root window!
    Startup_InitViewColors();
    Startup_InitViewBackgrounds();
    Startup_InitKeyBindings();
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
 comment        Firebrick
 string         RosyBrown
 keyword        Purple
 builtin        Orchid (also preprocessor)
 function-name  Blue1
 variable-name  DarkGoldenrod
 type           ForestGreen
 constant       CadetBlue
 warning        Red1
*/

bool taRootBase::Startup_InitViewColors() {
  if(!taMisc::view_colors) {
    taMisc::view_colors = new ViewColor_List;
    taMisc::view_colors->BuildHashTable(100); // speed this one up
  }

  // args are: name, description, foreground, fg_color_name, background, bg_color_name

  taMisc::view_colors->FindMakeViewColor("NotEnabled", "State: !isEnabled",
                                         false, _nilString, true, "grey86");
  taMisc::view_colors->FindMakeViewColor("SpecNotUsed", "State: This  spec is not used",
                                         false, _nilString, true, "grey86");
  taMisc::view_colors->FindMakeViewColor("ChildSpecIsUsed", "State: This spec not used by a child is used",
                                         false, _nilString, true, "grey86");
  taMisc::view_colors->FindMakeViewColor("ChildSpecNotUsed", "State: This spec is used by a child is not used",
                                         false, _nilString, true, "grey86");
  taMisc::view_colors->FindMakeViewColor("NotCalled", "State: function has no callers",
                                         false, _nilString, true, "grey86");
  taMisc::view_colors->FindMakeViewColor("debug_mode", "State: all programs in group are in debug_mode",
                                         false, _nilString, true, "LightSalmon");
  taMisc::view_colors->FindMakeViewColor("ThisInvalid", "State: item not valid according to CheckConfig",
                                         false, _nilString, true, "pink");
  taMisc::view_colors->FindMakeViewColor("ChildInvalid", "State: child not valid according to CheckConfig",
                                         false, _nilString, true, "gold");
  taMisc::view_colors->FindMakeViewColor("SpecialState1", "SpecialState = 1",
                                         false, _nilString, true, "lavender");
  taMisc::view_colors->FindMakeViewColor("SpecialState2", "SpecialState = 2",
                                         false, _nilString, true, "LightYellow");
  taMisc::view_colors->FindMakeViewColor("SpecialState3", "SpecialState = 3",
                                         false, _nilString, true, "PaleGreen");
  taMisc::view_colors->FindMakeViewColor("SpecialState4", "SpecialState = 4",
                                         false, _nilString, true, "MistyRose");
  taMisc::view_colors->FindMakeViewColor("ProgElNonStd", "State: program element is not standard",
                                         false, _nilString, true, "yellow1");
  taMisc::view_colors->FindMakeViewColor("ProgElNewEl", "State: program element is newly added",
                                         false, _nilString, true, "SpringGreen1");
  taMisc::view_colors->FindMakeViewColor("ProgElVerbose", "State: program element is verbose",
                                         false, _nilString, true, "khaki");
  taMisc::view_colors->FindMakeViewColor("ProgElError", "State: program element has Error",
                                         false, _nilString, true, "pink");
  taMisc::view_colors->FindMakeViewColor("ProgElWarning", "State: program element has Warning",
                                         false, _nilString, true, "gold");
  taMisc::view_colors->FindMakeViewColor("SearchMatch", "State: item matches last search",
                                         false, _nilString, true, "yellow1");
  taMisc::view_colors->FindMakeViewColor("ProgElBreakpoint", "State: program element is set for a breakpoint",
                                         false, _nilString, true, "violet");
  taMisc::view_colors->FindMakeViewColor("ProgElBreakpointDisabled", "State: program element is set for a breakpoint",
                                         false, _nilString, true, "violet");
  taMisc::view_colors->FindMakeViewColor("PrintDebugEnabled", "State: when print statement has debug set and the program group also has it set to true",
                                         false, _nilString, true, "LightSalmon");
  taMisc::view_colors->FindMakeViewColor("PrintDebugDisabled", "State: when print statement has debug set and the program group debug boolean is set to false",
                                         false, _nilString, true, "LightSalmon");
  taMisc::view_colors->FindMakeViewColor("Comment", "Program comments",
                                         true, "sea green", true, "medium sea green");
  taMisc::view_colors->FindMakeViewColor("ProgCode", "Program code -- proto element before being parsed into specific one",
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
  taMisc::view_colors->FindMakeViewColor("ControlPanel", "ControlPanel -- editor for selected variables across different objects",
                                         true, "azure4", true, "azure1");
  taMisc::view_colors->FindMakeViewColor("ParamSet", "ParamSet -- collection of selected variables and saved values across different objects",
                                         true, "azure4", true, "azure1");
  taMisc::view_colors->FindMakeViewColor("Doc", "Documentation object",
                                         true, "azure4", true, "azure1");
  return true;
}

bool taRootBase::Startup_InitViewBackgrounds() {
  if(!taMisc::view_backgrounds) {
    taMisc::view_backgrounds = new ViewBackground_List;
    taMisc::view_backgrounds->BuildHashTable(20);
  }

  // args are: name, description, background style (brush pattern)

  taMisc::view_backgrounds->FindMakeViewBackground("ProgElBreakpoint",
      "State: program element is set for an enabled breakpoint", Qt::SolidPattern);
  taMisc::view_backgrounds->FindMakeViewBackground("ProgElBreakpointDisabled",
      "State: program element is set for a disabled breakpoint", Qt::Dense5Pattern);
  taMisc::view_backgrounds->FindMakeViewBackground("PrintDebugEnabled",
      "State: program element is set for an enabled breakpoint", Qt::SolidPattern);
  taMisc::view_backgrounds->FindMakeViewBackground("PrintDebugDisabled",
      "State: program element is set for a disabled breakpoint", Qt::Dense5Pattern);
  taMisc::view_backgrounds->FindMakeViewBackground("ChildSpecNotUsed",
      "State: parent spec is used but not one of child specs", Qt::Dense5Pattern);
  taMisc::view_backgrounds->FindMakeViewBackground("ChildSpecIsUsed",
      "State: parent spec is not used but at least one child spec is used", Qt::Dense5Pattern);

 return true;
}

bool taRootBase::Startup_InitKeyBindings() {
  if(!taMisc::key_binding_lists) {
    taMisc::key_binding_lists = new KeyBindings_List();
  }
  KeyBindings* default_list = new KeyBindings();
  KeyBindings* custom_list = new KeyBindings();
  taMisc::key_binding_lists->Add_(default_list);
  taMisc::key_binding_lists->Add_(custom_list);
  
  taiMisc::LoadDefaultKeyBindings();
  taiMisc::LoadCustomKeyBindings();
  taiMisc::UpdateCustomKeyBindings();
  
  return true;
}

bool taRootBase::Startup_MakeWizards() {
  tabMisc::root->MakeWizards();
  return true;
}

bool taRootBase::Startup_InitPlugins() {
  if (!taMisc::use_plugins) return true;
  if (!tabMisc::root) return false; // should be made
  tabMisc::root->plugins.InitPlugins();
  return true;
}

bool taRootBase::Startup_MakeMainWin() {
  tabMisc::root->version = taMisc::version;
  if(!taMisc::gui_active && !taMisc::gui_no_win) return true;
  // no win creates this!
#ifdef TA_GUI
  // TODO: need to better orchestrate the "OpenWindows" call below with
  // create the default application window
  MainWindowViewer* vwr;
  if(tabMisc::root->viewers.size >= 1) {
    vwr = (MainWindowViewer*)tabMisc::root->viewers[0]; // this shouldn't happen..
  }
  else {
    vwr = MainWindowViewer::NewBrowser(tabMisc::root, NULL, true);
    vwr->SetUserData("view_win_lft", tabMisc::root->rootview_pos.x);
    vwr->SetUserData("view_win_top", tabMisc::root->rootview_pos.y);
    vwr->SetUserData("view_win_wd", tabMisc::root->rootview_size.x);
    vwr->SetUserData("view_win_ht", tabMisc::root->rootview_size.y);
    vwr->SetUserData("view_splitter_state", tabMisc::root->rootview_splits);
    if(console_options & taMisc::CO_GUI_DOCK) {
      ConsoleDockViewer* cdv = new ConsoleDockViewer;
      vwr->docks.Add(cdv);
    }
  }
  vwr->ViewWindow();

  iMainWindowViewer* rootwin = vwr->viewerWindow();
  if (rootwin) { //note: already constrained to max screen size, so we don't have to check
    // main win handle internal app urls
    taiMisc::main_window = rootwin;
    if(taiMisc::net_access_mgr)
      taiMisc::net_access_mgr->setMainWindow(rootwin);
    QDesktopServices::setUrlHandler("ta", rootwin, "taUrlHandler");
    QDesktopServices::setUrlHandler("http", rootwin, "httpUrlHandler");
    QDesktopServices::setUrlHandler("https", rootwin, "httpUrlHandler");
    rootwin->show(); // when we start event loop
  }

  // needs extra time to process window opening
  taMisc::ProcessEvents();
  // tabMisc::root->docs.AutoEdit();

  if(taMisc::gui_no_win) {
    taMisc::gui_active = false;       // now we go dark, after creating just one window..
  }
  else {
    tabMisc::root->wizards.AutoEdit();
    rootwin->setFocus();
  }

  //TODO: following prob not necessary
  //  if (taMisc::gui_active) taiMisc::OpenWindows();
#endif // TA_GUI
  return true;
}

void taRootBase::WindowShowHook() {
  bool static done = false;
  if (!done) {
    done = true; // set now in case ProcessEvents recurses
    // this is very hacky... select the 2nd tab, which will
    // be the first auto guy if there were any
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.SafeEl(0)); // always the default
    if (vwr) {
      vwr->SelectPanelTabNo(1);
      taMisc::ProcessEvents(true);
      vwr->widget()->setFocus();
    }

    qApp->setActiveWindow(vwr->widget());

#ifdef TA_OS_MAC
    // select this window, even if run from command line
    // String cmd = "osascript -e 'tell application \"System Events\" ";
    // cmd << "to keystroke tab using {command down}'"; // go to next item
    // system(cmd);
    // // do it twice -- go to other, come back to us
    // system(cmd);

    // this cannot find the window name -- not sure how else to do it
    // or index 0 or 1 don't work..
    // vwr->SetWinName();                 // get our name now
    // String cmd = "osascript -e 'tell application \"System Events\" to tell process \"";
    // cmd << taMisc::exe_cmd << "\" to perform action \"AXRaise\" of window 0'";
    // taMisc::Info(cmd);
    // system(cmd);
#endif
  }
}

bool taRootBase::Startup_Console() {
  if(taMisc::gui_active) {
    //note: nothing else to do here for gui_dockable
    QcssConsole* con = QcssConsole::getInstance(NULL, cssMisc::TopShell);
    
    QObject::connect(con, SIGNAL(receivedNewStdin(int)), root_adapter,
                     SLOT(ConsoleNewStdin(int)));
    // get notified
    
    if(!(console_options & taMisc::CO_GUI_DOCK)) {
      cssConsoleWindow* cwin = new cssConsoleWindow();
      cwin->lock_to_proj = tabMisc::root->console_locked;
      if(cwin->lock_to_proj) {
        cwin->LockedNewGeom((int)(.025 * taiM->scrn_s.w), (int)(.7 * taiM->scrn_s.h),
                            (int)(.95 * taiM->scrn_s.w), (int)(.25 * taiM->scrn_s.h));
      }
      else {
        cwin->UpdateFmLock();   // calls load geom
      }
      cwin->show();
      // cwin->css_con->clear();     // move cursor to bottom
      taMisc::console_win = cwin; // note: uses a guarded QPointer
      
      if(tabMisc::root->viewers.size >= 1) {
        taMisc::ProcessEvents();
        MainWindowViewer* db = (MainWindowViewer*)tabMisc::root->viewers[0];
        db->ViewWindow();               // make sure root guy is on top
        qApp->setActiveWindow(db->widget());
      }
    }
    
    String filename = taMisc::GetConsoleHistoryFilename();
    QFile history_file(filename);
    if (!history_file.open(QIODevice::ReadOnly)) {
      taMisc::Info("Console history file not found, will be created on first console command");
    }
    else {
      QStringList string_list;
      QTextStream text_stream(&history_file);
      while (true)
      {
        QString line = text_stream.readLine();
        if (line.isNull())
          break;
        else
          string_list.append(line);
      }
      con->InitHistory(string_list);
      history_file.close();
    }
  }
  cssMisc::TopShell->StartupShellInit(cin, cout);

  if(!taMisc::interactive) {
    String hostname = getenv("HOSTNAME");
    if (hostname.nonempty()) {
      taMisc::Info(taMisc::app_name, "running non-interactively on host:", hostname);
    }
  }
  
  return true;
}

void taRootBase::ConsoleNewStdin(int n_lines) {
  if(!taMisc::gui_active) return;
  if(taMisc::console_win) {
    QApplication::alert(taMisc::console_win);
  }
  else {
    // assume dock..
    if(tabMisc::root->viewers.size >= 1) {
      MainWindowViewer* db = (MainWindowViewer*)tabMisc::root->viewers[0];
      QApplication::alert(db->widget());
    }
  }
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
  bool run_startup = true;
  if(taMisc::CheckArgByName("Version")) {
    tabMisc::root->About();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("Help")) {
    String hmsg;
    taMisc::HelpMsg(hmsg);
    cout << hmsg << endl;
    run_startup = false;
  }
  if(taMisc::CheckArgByName("GenDoc")) {
    taMisc::help_detail = taMisc::HD_DEFAULT; // always render default
    taGenDoc::GenDoc(&(taMisc::types));
    run_startup = false;
  }
  if(taMisc::CheckArgByName("MakeAllPlugins")) {
    taPlugins::MakeAllPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("MakeAllUserPlugins")) {
    taPlugins::MakeAllUserPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("MakeUserPlugin")) {
    String plugnm = taMisc::FindArgByName("MakeUserPlugin");
    if(plugnm.nonempty()) {
      taPlugins::MakeUserPlugin(plugnm);
    }
    run_startup = false;
  }
  if(taMisc::CheckArgByName("MakeAllSystemPlugins")) {
    taPlugins::MakeAllSystemPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("MakeSystemPlugin")) {
    String plugnm = taMisc::FindArgByName("MakeSystemPlugin");
    if(plugnm.nonempty()) {
      taPlugins::MakeSystemPlugin(plugnm);
    }
    run_startup = false;
  }
  if(taMisc::CheckArgByName("CleanAllPlugins")) {
    taPlugins::CleanAllPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("CleanAllUserPlugins")) {
    taPlugins::CleanAllUserPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("CleanAllSystemPlugins")) {
    taPlugins::CleanAllSystemPlugins();
    run_startup = false;
  }
  if(taMisc::CheckArgByName("GenPrngParams")) {
    run_startup = false;
    MTRnd::GenInitParams(MTRnd::max_gens, "new_mtrndpars.txt");
  }
  if(taMisc::CheckArgByName("CreateNewSrc")) {
    String srcnm = taMisc::FindArgByName("CreateNewSrc");
    String curpath = QDir::currentPath();
    String top_path = curpath.before("/src/",-1);
    String src_path = curpath.from("/src/",-1);
    src_path = src_path.after("/");
    taMisc::Info("creating new source files for type:", srcnm, "in top path:", top_path,
		 "src_path:", src_path);
    taCodeUtils::CreateNewSrcFiles(srcnm, top_path, src_path);
    run_startup = false;
  }
  if(taMisc::CheckArgByName("CreateNewSpec")) {
    String srcnm = taMisc::FindArgByName("CreateNewSpec");
    String curpath = QDir::currentPath();
    String top_path = curpath.before("/src/",-1);
    String src_path = curpath.from("/src/",-1);
    src_path = src_path.after("/");
    taMisc::Info("creating new spec source files for type:", srcnm, "in top path:", top_path,
		 "src_path:", src_path);
    taCodeUtils::CreateNewSpecFiles(srcnm, top_path, src_path);
    run_startup = false;
  }
  if(taMisc::CheckArgByName("CreateNewState")) {
    String srcnm = taMisc::FindArgByName("CreateNewState");
    String curpath = QDir::currentPath();
    String top_path = curpath.before("/src/",-1);
    String src_path = curpath.from("/src/",-1);
    src_path = src_path.after("/");
    taMisc::Info("creating new state source files for type:", srcnm, "in top path:", top_path,
		 "src_path:", src_path);
    taCodeUtils::CreateNewStateFiles(srcnm, top_path, src_path);
    run_startup = false;
  }
  if(taMisc::CheckArgByName("RenameType")) {
    run_startup = false;
    String oldnm = taMisc::FindArgByName("RenameType");
    String newnm = taMisc::FindArgByName("RenameTypeTo");
    if(newnm.empty()) {
      taMisc::Error("RenameType: no rename_to arg provided for new name");
    }
    else {
      String curpath = QDir::currentPath();
      String top_path = curpath.before("/src/",-1);
      String src_path = curpath.from("/src/",-1);
      src_path = src_path.after("/");
      taMisc::Info("renaming type from old name:", oldnm,"to new name:",newnm,
                   "in top path:", top_path,"src_path:", src_path);
      taCodeUtils::RenameType(oldnm, newnm, top_path, src_path);
      run_startup = false;
    }
  }
  if(taMisc::CheckArgByName("ReplaceString")) {
    run_startup = false;
    String oldnm = taMisc::FindArgByName("ReplaceString");
    String newnm = taMisc::FindArgByName("ReplaceStringTo");
    if(newnm.empty()) {
      taMisc::Error("ReplaceString: no replace_to arg provided for new replace string");
    }
    else {
      String curpath = QDir::currentPath();
      String top_path = curpath.before("/src/",-1);
      String src_path = curpath.from("/src/",-1);
      src_path = src_path.after("/");
      taMisc::Info("replacing string from:", oldnm,"to new replacement:",newnm,
                   "in top path:", top_path,"src_path:", src_path);
      taCodeUtils::ReplaceInDir(oldnm, newnm, top_path, src_path);
      run_startup = false;
    }
  }
  if(taMisc::CheckArgByName("RemoveType")) {
    String srcnm = taMisc::FindArgByName("RemoveType");
    String curpath = QDir::currentPath();
    String top_path = curpath.before("/src/",-1);
    String src_path = curpath.from("/src/",-1);
    src_path = src_path.after("/");
    taMisc::Info("removing type:", srcnm, "in top path:", top_path,
		 "src_path:", src_path);
    taCodeUtils::RemoveType(srcnm, top_path, src_path);
    run_startup = false;
  }
  if(taMisc::CheckArgByName("CopyrightUpdate")) {
    String srcnm = taMisc::FindArgByName("CopyrightUpdate");
    String curpath = QDir::currentPath();
    String top_path = curpath.before("/src/",-1);
    String src_path = curpath.from("/src/",-1);
    src_path = src_path.after("/");
    if(srcnm.nonempty()) {
      taMisc::Info("updating copyright in:", srcnm, "in top path:", top_path,
                   "src_path:", src_path);
      taCodeUtils::CopyrightUpdateFile(top_path + PATH_SEP + src_path + PATH_SEP + srcnm);
    }
    else {
      taMisc::Info("updating copyright in all files in top path:", top_path);
      taCodeUtils::CopyrightUpdateAllFiles(top_path);
    }
    run_startup = false;
  }

  if(run_startup && taPlugins::plugins_out_of_date > 0) {
#ifdef TA_OS_WIN
    int chs = 1;                // don't recompile by default in windows
#else
    int chs = 0;                // default is to recompile on other platforms
#endif
    if(taMisc::interactive) {
      chs = taMisc::Choice("Some plugins are out of date -- rebuild them from the installed source code now?\n\nIf you do 'Rebuild',  emergent will QUIT after rebuilding, and the build process will not be visible in the GUI (run --nogui to see it).  Note - this will only work if your system is configured for building plugins from source. Restart emergent to load the plugins.\n\nIf you 'Ignore', then those plugins will remain unavailable until rebuilt.\n\nIf you 'Clean', then the existing out-of-date plugin files will be removed and you won't be prompted again (you can always build them again later).", "Rebuild", "Ignore", "Clean");
    }
    if(chs == 0) {
      taPlugins::MakeAllOutOfDatePlugins();
      run_startup = false;
    }
    else if(chs == 2) {
      taPlugins::CleanAllOutOfDatePlugins();
      run_startup = true;
    }
  }

  // just load the thing!?
  String proj_ld = taMisc::FindArgByName("Project");
  if(proj_ld.empty())
    proj_ld = taMisc::FindArgValContains(".proj");
  
  if(!proj_ld.empty()) {
    bool file_exists = false;
    QFileInfo checkFile(proj_ld);
    // check if file exists and if yes: Is it really a file and not a directory?
    if (checkFile.exists() && checkFile.isFile()) {
      tabMisc::root->projects.Load(proj_ld);
      taRootBase::ProjectOpened();
    }
    else {
      taMisc::Error("Startup_ProcessArgs: Project file not found");
      if (!taMisc::gui_active) {
          taiMC_->Quit();
      }
    }
  }
  
  if(run_startup) {
    // chain the next step -- this will hopefully happen *after* any post-loading
    // events triggered by the projects.load
    QTimer::singleShot(0, root_adapter, SLOT(Startup_RunStartupScript()));
  }
  else {
    taiMC_->Quit();
  }
  return true;
}

bool taRootBase::Startup_RunStartupScript() {
  bool ran = cssMisc::TopShell->RunStartupScript();
  if(ran && !taMisc::gui_active && !taMisc::interactive)
    taiMC_->Quit(CO_NORMAL_QUIT);
  return true;
}

bool taRootBase::Startup_Main(int& argc, const char* argv[], TypeDef* root_typ) {
  ++in_init;
  root_type = root_typ;
#ifdef GPROF
  moncontrol(0);                // turn off at start
#endif
#ifdef SATURN_PROF
  // this actually seems bad: get a warning
  // initSaturn("");            // store in current wd
#endif

#ifdef TA_OS_MAC
  // this is necessary to get coin to use system fonts wit freetype
  setenv("COIN_FONT_PATH", "/Library/Fonts", 1);
#endif

  // just create the adapter obj, whether needed or not
  root_adapter = new taRootBase_QObj;
  cssMisc::prompt = taMisc::app_name; // the same
  if (taMisc::app_prefs_key.empty())
    taMisc::app_prefs_key = taMisc::app_name;
  if(!Startup_InitDMem(argc, argv)) goto startup_failed;
  if(!Startup_InitArgs(argc, argv)) goto startup_failed;
  if(!Startup_ProcessGuiArg(argc, argv)) goto startup_failed;
  if(!Startup_InitApp(argc, argv)) goto startup_failed;
  if(!Startup_InitTA()) goto startup_failed;
  if(!Startup_EnumeratePlugins()) goto startup_failed;
  if(!Startup_LoadPlugins()) goto startup_failed; // loads those enabled, and does type integration
  if(!Startup_InitCss()) goto startup_failed;
  if(!Startup_InitGui()) goto startup_failed; // note: does the taiType bidding
  Startup_MakeWizards(); // supposedly can't fail...
  if(!Startup_InitPlugins()) goto startup_failed; // state, wizards, etc.
  if(!Startup_MakeMainWin()) goto startup_failed;
  if(!Startup_Console()) goto startup_failed;
  if(!Startup_RegisterSigHandler()) goto startup_failed;
  // note: Startup_ProcessArgs() is called after having entered the event loop
  // note: don't call event loop yet, because we haven't initialized main event loop
  // happens in Startup_Run()
  --in_init;
  if(taMisc::gui_active && (taMisc::dmem_proc == 0))    // only guy and don't have all the other nodes save
    instance()->Save();

  cout << "Running " << taMisc::app_name << " version: " << taMisc::version
       << " svn rev: " << taMisc::svn_rev << endl;
  
  if(taMisc::CheckArgByName("AttachWait")) {
#ifdef DMEM_COMPILE
    String awval = taMisc::FindArgByName("AttachWait");
    if(awval.nonempty()) {
      int procno = (int)awval;
      if(taMisc::dmem_proc != procno) return true; // bail
    }
#endif
    volatile int i = 0;
    printf("PID %d on %s ready for attach\n", taMisc::ProcessId(),
           taMisc::HostName().chars());
    fflush(stdout);
    // NOTE to programmer: in gdb debugger, do: set var i = 1  then continue -- this will break out of
    // following infinite loop and allow code to continue execution
    while (0 == i) {
      taMisc::SleepS(5);
    }
  }

  return true;

  startup_failed:
  Cleanup_Main();
  return false;
}


// don't do the minimize until the tight relationship between the menubar and the root window is fixed
void taRootBase::ProjectOpened() {
  if (taiMisc::main_window && !openProject) {
//    taiMisc::main_window->showMinimized();  // if project is opening on launch minimize root window
    openProject = true;
  }
}

///////////////////////////////////////////////////////////////////////////
//      Run & Cleanup

bool taRootBase::Startup_Run() {
#ifdef DMEM_COMPILE
//TODO: make Run_GuiDMem dispatched after event loop, and fall through
  if((taMisc::dmem_nprocs > 1) && taMisc::gui_active) {
    Run_GuiDMem(); // does its own eventloop dispatch
    return true;
  }
#endif

  if(!taMisc::interactive) {
    taMisc::display_width = 120;
  }
  
  // if in server mode, make it now!
  if (taMisc::args.FindName("Server") >= 0) {
    TemtServer* server = (TemtServer*)instance()->objs.New(1, &TA_TemtServer);
    server->port = (ushort)taMisc::args.GetValDef("Port", 5360).toUInt();
    if (server->InitServer() && server->OpenServer()) {
      cerr << "TemtServer is now running and waiting for connections\n";
    }
    else {
      cerr << "ERROR: could not Initialize or Open TemtServer -- now exiting\n";
      Cleanup_Main();
      return false;
    }
  }
  
  // first thing to do upon entering event loop:
  QTimer::singleShot(0, root_adapter, SLOT(Startup_ProcessArgs()));
  
  if(!taMisc::gui_active && taMisc::interactive) {
    QTimer::singleShot(0, cssMisc::TopShell, SLOT(Shell_NoConsole_Run()));
  }
  
  // Give the root window focus.
  QTimer::singleShot(0, root_adapter, SLOT(FocusRootWinAtStartup()));
  
  // now everyone goes into the event loop
  taiMC_->Exec();
  
  Cleanup_Main();
  return true;
}

extern "C" {
  extern void rl_free_line_state(void);
  extern void rl_cleanup_after_signal(void);
}

// todo: could partition these out into separate guys..
void taRootBase::Cleanup_Main() {
  taMisc::in_shutdown++;
  taiMisc::Cleanup(0);      // cleanup stuff in tai
  taMisc::aka_types.Reset();    // errs happen when this gets reset out of order
  taMisc::reg_funs.Reset();    // errs happen when this gets reset out of order
  // remove sig handler -- very nasty when baddies happen after this point
  if (milestone & SM_REG_SIG) {
    taMisc::Register_Cleanup(SIG_DFL); // replace back to using default
  }
  cssMisc::Shutdown();          // shut down css..
  if (milestone & SM_ROOT_CREATE)
    tabMisc::DeleteRoot();
  if (milestone & SM_TYPES_INIT)
    taMisc::types.RemoveAll();  // get rid of all the types before global dtor!

#ifndef TA_QT3D
  if(taMisc::gui_active && (milestone & SM_SOQT_INIT)) {
#if COIN_MAJOR_VERSION >= 3
    if(coin_image_reader_cb_obj) {
      delete coin_image_reader_cb_obj;
      coin_image_reader_cb_obj= NULL;
    }
#endif
    SIM::Coin3D::Quarter::Quarter::clean();
  }
#endif
  // this may be redundante -- getting errors at end of jobs..
// #ifdef DMEM_COMPILE
//   if (milestone & SM_MPI_INIT)
//     MPI_Finalize();
// #endif
  taThreadMgr::TerminateAllThreads(); // don't leave any active threads lying around

#ifndef TA_OS_WIN
  sleep(1);
#else
  Sleep(1);
#endif

#ifndef TA_OS_WIN
  // only if using readline-based console, reset tty state
  if(!taMisc::gui_active && taMisc::interactive) {
    rl_free_line_state();
    rl_cleanup_after_signal();
  }
#endif

  delete QCoreApplication::instance(); // nuke it!
}

#ifdef DMEM_COMPILE

bool taRootBase::Run_GuiDMem() {
  if (taMisc::dmem_proc == 0) { // master dmem
    DMemShare::InitCmdStream();
    // need to have some initial string in the stream, otherwise it goes EOF and is bad!
    *(DMemShare::cmdstream) << "cerr << \"proc no: \" << taMisc::dmem_proc << endl;" << endl;
    taMisc::StartRecording();
    taiMC_->Exec();  // normal run..
    DMemShare::CloseCmdStream();
    cerr << "proc: 0 quitting!" << endl;
  }
  else { // slave dmems
    taMisc::interactive = false; // don't stay in startup shell
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
//              Recover File Handler

//#ifndef TA_OS_WIN

#include <signal.h>
#include <memory.h>
#include <sstream>

TA_BASEFUNS_CTORS_DEFN(taRootBase);

// for saving a recovery file if program crashes, is killed, etc.
void taRootBase::SaveRecoverFileHandler(int err) {
  static bool has_crashed = false;
  signal(err, SIG_DFL);         // disable catcher

  if(has_crashed) {
    cerr << "Unable to save recover file (multiple errors)...sorry" << endl;
    exit(err);
  }
  has_crashed = true;           // to prevent recursive crashing..

#ifdef TA_OS_WIN
  bool non_term_sig = false;
#else
  bool non_term_sig = ((err == SIGALRM) || (err == SIGUSR1) || (err == SIGUSR2));
#endif

  if(!non_term_sig) {
#ifdef DMEM_COMPILE
    MPI_Finalize();
#endif
    taThreadMgr::TerminateAllThreads(); // don't leave any active threads lying around
#ifdef TA_GUI
    taiMisc::Cleanup(err);      // cleanup stuff in tai
#endif
  }

#ifndef TA_OS_WIN // MS CRT doesn't handle these signals...
  if(non_term_sig) {
    cerr << "Saving project file(s) from signal: ";
  } else
#endif // !TA_OS_WIN
  {
    cerr << "Saving recover file(s) and exiting from signal: ";
  }
  taMisc::Decode_Signal(err);
  cerr << endl;

  taiMiscCore::BgRunKilled(); // saves weights in network, for emergent
  if (tabMisc::root) {
    for (int i = 0; i < tabMisc::root->projects.size; ++i) {
      taProject* prj = tabMisc::root->projects.FastEl(i);
      prj->SaveRecoverFile();
    }
  }

#ifdef TA_OS_WIN // MS CRT doesn't handle these signals...
  exit(err);
#else // TA_OS_WIN // MS CRT doesn't handle these signals...
  if(non_term_sig) {
    taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) SaveRecoverFileHandler);
    has_crashed = false;
  }
  else {
    exit(err);
  }
#endif //
}

void taRootBase::ChooseForDiffCompare(String type_name, taProject* cur_prj, String title_name) {
  bool okc;
  taBase* obj_one = NULL;
  taBase* obj_two = NULL;
  String title;
  taiWidgetTokenChooser* chooser =  NULL;
  TypeDef* targ_type = NULL;
  
  if (title_name.empty()) {
    title_name = type_name;
  }
  
  targ_type = taMisc::FindTypeName(type_name);
  if (!targ_type) {
    taMisc::Warning("Please report this program error, taProject::ChooseForDiffCompare - unknown type_name");
  }
  
  if (targ_type != NULL) {
    chooser = new taiWidgetTokenChooser(targ_type, NULL, NULL, NULL);
    if (cur_prj) {
      chooser->GetImageScoped(NULL, targ_type, cur_prj, &TA_taProject);
    }
    else {
      chooser->GetImage(NULL, targ_type);
    }
    title = "Select first of two " + title_name + "s for comparison";
    chooser->SetTitleText(title);
    okc = chooser->OpenChooser();
    if(okc && chooser->token()) {
      obj_one = chooser->token();
    }
    delete chooser;
    
    if (obj_one != NULL) {
      chooser =  new taiWidgetTokenChooser(targ_type, NULL, NULL, NULL);
      chooser->GetImage(NULL, targ_type);
      title = "Select second " + title_name;
      chooser->SetTitleText(title);
      okc = chooser->OpenChooser();
      if(okc && chooser->token()) {
        obj_two = chooser->token();
      }
      delete chooser;
    }
    
    if (obj_one != NULL && obj_two != NULL) {
      obj_one->DiffCompare(obj_two);
    }
  }
}

bool taRootBase::OpenProjectFromWeb(const String& proj_file_name, const String& wiki_name) {
  String act_wiki_name = wiki_name;
  if(wiki_name.startsWith("http")) { // got the full url -- need to decode
    act_wiki_name = taMediaWiki::GetWikiNameFromURL(wiki_name);
    if(act_wiki_name.empty())
      return false;
  }
  
#if (QT_VERSION >= 0x050000)
  QString defaultLocation =
    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
  QString defaultLocation =
    QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif

  String proj_file = defaultLocation + PATH_SEP + proj_file_name;
  if (!taMediaWiki::FileExists(act_wiki_name, proj_file_name)) {
    taMisc::Error("The project file " + proj_file_name + " on the wiki " + act_wiki_name + " does not exist");
    return false;
  }
  taMisc::Info("Downloading project file " + proj_file_name + " from the wiki " + act_wiki_name);
  taMisc::Busy();
  taMediaWiki::DownloadFile(act_wiki_name, proj_file_name, proj_file);
  taMisc::DoneBusy();
  delayed_proj_open = proj_file;
  tabMisc::DelayedFunCall_gui(this, "DelayedOpenProject");
  taMisc::Info("Opening project from:", proj_file, "now -- may take a few moments..");
  return true;
}

void taRootBase::DelayedOpenProject() {
  if(delayed_proj_open.empty()) return;
  taBase* el = NULL;
  projects.Load(delayed_proj_open, &el);
  delayed_proj_open = "";
}
