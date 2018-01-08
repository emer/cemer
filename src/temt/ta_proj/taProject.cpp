// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "taProject.h"
#include <taDoc>
#include <taiEditorOfClass>
#include <MainWindowViewer>
#include <iMainWindowViewer>
#include <taiMiscCore>
#include <iTreeView>
#include <taFiler>
#include <iDialogPublishDocs>
#include <taiEditorOfString>
#include <ClusterRun>
#include <ClusterRunJob>
#include <Program>
#include <iSubversionBrowser>
#include <iSvnFileListModel>
#include <SubversionClient>
#include <iDialogChoice>
#include <ctime>
#include <ObjDiff>
#include <ParamSet>
#include <Patch>

taTypeDef_Of(taDataProc);
SMARTREF_OF_CPP(taProject);

taTypeDef_Of(taDataAnal);
taTypeDef_Of(taDataGen);
// taTypeDef_Of(taImageProc);

#include <SigLinkSignal>
#include <taMisc>
#include <tabMisc>
#include <taiMisc>
#include <taRootBase>
#include <taMediaWiki>
#include <taGuiDialog>

#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <QFileDialog>
#if (QT_VERSION >= 0x050000)
#include <QStandardPaths>
#endif
#include <QDesktopServices>
#include <QDateTime>
#include <QBoxLayout>
#include <iTextEdit>
#include <cssConsoleWindow>

#include <css_machine.h>
#include <css_qtconsole.h>

TA_BASEFUNS_CTORS_DEFN(taProject);


using namespace std;


#ifdef TA_GUI
class SimLogEditDialog: public taiEditorOfClass {
public:
  bool ShowMember(MemberDef* md) const override {
    // just show a small subset of the members
    bool rval = (!md->IsEditorHidden() && (md->im != NULL));
    if (!rval) return rval;
// note: we also include a couple of members we know are in taProject
    if (!(md->name.contains("desc") || (md->name == "version") ||
          (md->name == "save_rmv_units")
          || (md->name == "file_name"))) return false;
    return true;
  }
  void Constr_Methods_impl() override { }       // suppress methods

  SimLogEditDialog(void* base, TypeDef* tp, bool read_only_,
        bool modal_) : taiEditorOfClass(base, tp, read_only_, modal_) { };
};
#endif


taProject* taProject::cur_proj = NULL;

void taProject::Initialize() {
  auto_name = true;
  m_dirty = false;
  m_no_save = false;
  viewers.SetBaseType(&TA_TopLevelViewer);
  viewers_tmp.SetBaseType(&TA_TopLevelViewer);
  save_view = true;
  save_as_only = false;
  no_dialogs = false;
}

void taProject::Destroy() {
  CutLinks();
}

void taProject::InitLinks() {
  //note: this routine is private, so subclasses must use _impl or _post
  inherited::InitLinks();
  InitLinks_impl();
  InitLinks_post();
}

void taProject::InitLinks_impl() {
  taBase::Own(version, this);
  taBase::Own(license, this);
  taBase::Own(wiki, this);
  taBase::Own(patches, this);
  taBase::Own(docs, this);
  taBase::Own(wizards, this);
  taBase::Own(ctrl_panels, this);
  taBase::Own(active_params, this);
  taBase::Own(archived_params, this);
  taBase::Own(data, this);
  taBase::Own(data_proc, this);
  taBase::Own(programs, this);
  taBase::Own(viewers, this);
  taBase::Own(viewers_tmp, this);
  taBase::Own(undo_mgr, this);

  ctrl_panels.el_typ = &TA_ControlPanel; // set this as default type

  // note: any derived programs should install additional guys..
  // put in NO_CLIP to suppress clip ops, since we don't want any for these guys
  FindMakeNewDataProc(&TA_taDataProc, "data_base")->SetUserData("NO_CLIP", true);
  FindMakeNewDataProc(&TA_taDataAnal, "data_anal")->SetUserData("NO_CLIP", true);
  FindMakeNewDataProc(&TA_taDataGen, "data_gen")->SetUserData("NO_CLIP", true);
  // FindMakeNewDataProc(&TA_taImageProc, "image_proc")->SetUserData("NO_CLIP", true);
  // not actually useful to have these guys visible..  no user-accessible matrix objs
  // if in datatable, it should be accessible in above
//   FindMakeNewDataProc(&TA_taMath_float, "math_float")->SetUserData("NO_CLIP", true);
//   FindMakeNewDataProc(&TA_taMath_double, "math_double")->SetUserData("NO_CLIP", true);
}

void taProject::InitLinks_post() {
  if (!taMisc::is_loading) {
    // ensure we have a cluster run guy
    FindMakeControlPanel("ClusterRun", &TA_ClusterRun);
    DoView();
  }
  ctrl_panels.el_typ = &TA_ControlPanel; // set this as default type
  SetProjAsCurrent();
}

void taProject::CutLinks() {
  taMisc::SetLogFileToDefault(); // don't log to us anymore
  taProject::cur_proj = NULL;
  if(tabMisc::root && !tabMisc::root->isDestroying()) {
    for(int i=0; i<tabMisc::root->projects.size; i++) {
      taProject* proj = tabMisc::root->projects[i];
      if(proj != this && !proj->isDestroying()) {
        proj->SetProjAsCurrent();
        break;
      }
    }
  }
  CutLinks_impl();
  inherited::CutLinks();
}

void taProject::CutLinks_impl() {
  viewers_tmp.CutLinks();
  viewers.CutLinks();
  programs.CutLinks();
  data_proc.CutLinks();
  data.CutLinks();
  ctrl_panels.CutLinks();
  active_params.CutLinks();
  archived_params.CutLinks();
  wizards.CutLinks();
  docs.CutLinks();
}

void taProject::Copy_(const taProject& cp) {
  // delete things first, to avoid dangling references
  programs.Reset();
  viewers_tmp.Reset();
  viewers.Reset();
  data.Reset();

  ctrl_panels.Reset();

  tags = cp.tags;
  docs = cp.docs;
  wizards = cp.wizards;
  ctrl_panels = cp.ctrl_panels;
  data = cp.data;
  data_proc = cp.data_proc;
  viewers = cp.viewers;         // todo: open windows here etc
  programs = cp.programs;
  // NOTE: once a derived project has all the relevant stuff copied, it needs to call this:
  // UpdatePointers_NewPar(&cp, this); // update pointers within entire project..
  setDirty(true);
}

void taProject::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  if(taMisc::is_loading) {	// make sure we have one of these for old projects
    FindMakeControlPanel("ClusterRun", &TA_ClusterRun);
    ctrl_panels.el_typ = &TA_ControlPanel; // set this as default type
  }
  UpdateUi();
}

taBase* taProject::FindMakeNewDataProc(TypeDef* typ, const String& nm) {
  taBase* rval = data_proc.FindType(typ);
  if(rval) return rval;
  rval = data_proc.NewEl(1, typ);
  rval->SetName(nm);
  rval->SigEmitUpdated();
  return rval;
}

ControlPanel* taProject::FindMakeControlPanel(const String& nm, TypeDef* type) {
  ControlPanel* rval = ctrl_panels.FindName(nm);
  if(rval) return rval;
  rval = (ControlPanel*)ctrl_panels.NewEl(1, type);
  rval->SetName(nm);
  rval->SigEmitUpdated();
  return rval;
}

taDoc* taProject::FindMakeDoc(const String& nm, const String& wiki_nm, const String& web_url) {
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

String taProject::GetClusterRunPath() {
  if(taMisc::cluster_run) { // we are on the cluster
    return taMisc::GetDirFmPath(proj_dir); // up one from this path
  }
  // otherwise we are on workstation -- get cr svn repo
  ClusterRun* cr = (ClusterRun*)FindMakeControlPanel("ClusterRun", &TA_ClusterRun);
  if(cr) {
    return cr->GetSvnPath();
  }
  return proj_dir;              // just current path otherwise
}

bool taProject::GetClusterRunJob(int updt_interval_mins) {
  static int last_updt_min = -1;
  if(!taMisc::cluster_run) return false;
  bool get_new_data = false;
  if(ClusterRunJob::cur_job) { // we already have current job info
    taDateTime now; now.currentDateTime();
    int now_min = now.minute();
    if(now_min != last_updt_min && (now_min % updt_interval_mins == 0)) {
      last_updt_min = now_min;
      get_new_data = true;
      // taMisc::Info("Cluster Run: get new data every:",String(updt_interval_mins),"mins");
    }
  }
  else {
    get_new_data = true;        // don't have any -- get some!
  }
  if(!get_new_data) {
    return false;
  }
  ClusterRun* cr = (ClusterRun*)FindMakeControlPanel("ClusterRun", &TA_ClusterRun);
  if(!cr) return false;         // should not happen
  bool got_new = cr->LoadMyRunningTable();
  if(cr->jobs_running.rows == 0) {
    taMisc::Info("Cluster Run: no local jobs_running.dat data");
    return false; // no data..
  }
  String tag = taMisc::FindArgByName("tag");
  if(tag.startsWith('_'))       // typically true
    tag = tag.after('_');
  int tag_row = cr->jobs_running.FindVal(tag, "tag", 0, false);
  if(tag_row < 0) {
    taMisc::Info("Cluster Run: could not find tag:", tag, "in jobs_running.dat data");
    return false; // our job not in it
  }
  bool got = cr->GetCurJobData(tag);       // get it
  if(got_new && got) {
    ClusterRunJob* cj = ClusterRunJob::cur_job;
    taMisc::Info
      ("Cluster run job info loaded:", String(got_new), "tag:", cj->tag,
       "start: " + cj->start_time.toString(ClusterRun::timestamp_fmt),
       "end: " + cj->run_time_end.toString(ClusterRun::timestamp_fmt),
       "label: " +  cj->label, "notes: " + cj->notes);
  }
  return true;
}

String taProject::CheckClusterRunCmd() {
  if(!taMisc::cluster_run) return "";
  ClusterRun* cr = (ClusterRun*)FindMakeControlPanel("ClusterRun", &TA_ClusterRun);
  if(!cr) return false;         // should not happen
  bool got_new = cr->LoadMyRunningCmdTable();
  if(!got_new) return "";
  String tag = taMisc::FindArgByName("tag");
  if(tag.startsWith('_'))       // typically true
    tag = tag.after('_');
  int tag_row = cr->jobs_running_cmd.FindVal(tag, "tag", 0, false);
  if(tag_row < 0) {
    return false; // our job not in it
  }
  String cmd = cr->jobs_running_cmd.GetValAsString("status", tag_row);
  String sub_time = cr->jobs_running_cmd.GetValAsString("submit_time", tag_row);
  // could get other data too, if needed..
  cr->jobs_running_cmd.RemoveRows(tag_row, 1);
  cr->SaveMyRunningCmdTable();
  taMisc::Info("Cluster run job command:", cmd, "received for tag:", tag,
               "submit time: " + sub_time);
  return cmd;
}

MainWindowViewer* taProject::GetDefaultProjectBrowser() {
  MainWindowViewer* vwr = NULL;
  // iterate to find 1st Browser -- will actually be 2nd item in 2x2 (2x2 no longer supported)
  for (int i = 0; i < viewers.size; ++i) {
    vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (vwr && vwr->isProjBrowser()) return vwr;
  }
  return NULL;
}

MainWindowViewer* taProject::GetDefaultProjectViewer() {
  // get the default T3 guy
  MainWindowViewer* vwr = NULL;
  // iterate to find 1st Viewer -- will actually be 1st item in 2x2 (2x2 no longer supported)
  for (int i = 0; i < viewers.size; ++i) {
    vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (vwr && vwr->isProjViewer()) return vwr;
  }
  return NULL;
}

void taProject::Dump_Load_post() {
  inherited::Dump_Load_post();
  if(taMisc::is_undo_loading)
    return; // none of this.
  OpenProjectLog();
  DoView();
  
  setDirty(false);              // nobody should start off dirty!
  if(!taMisc::interactive) {
    bool startup_run = programs.RunStartupProgs();      // run startups now..
    if(!taMisc::gui_active && startup_run) taiMC_->Quit();
  }
}

void taProject::DoView() {
  if (!taMisc::gui_active || taMisc::is_undo_loading) return;
  MainWindowViewer* main_window_viewer = AssertDefaultProjectBrowser(true);
  if(!main_window_viewer) return;
  
  // allow to process new window before asserting default items
  // debug mode still doesn't work here tho.. 
  taMisc::ProcessEvents();
  
  // restore tree state if saved - otherwise use defaults
  if(main_window_viewer->widget()) {
    iTreeView* nav_tree_view = main_window_viewer->widget()->GetMainTreeView();
    if (nav_tree_view) {
      nav_tree_view->SetTreeStateClean();  // ignore the expand/collapse that occurs opening the views
      if (tree_state.size != 0 && file_name.nonempty()) {
        nav_tree_view->RestoreTreeState(tree_state);
      }
      else {
        nav_tree_view->ExpandDefault();
      }
    }
  }

  docs.RestorePanels();
  wizards.RestorePanels();
  ctrl_panels.RestorePanels();
  data.RestorePanels();
  programs.RestorePanels();
  
  // this is very hacky... select the 2nd tab, which will
  // be the first auto guy if there were any
  taMisc::ProcessEvents();
  main_window_viewer->SelectPanelTabNo(1);
}

MainWindowViewer* taProject::AssertDefaultProjectBrowser(bool auto_open) {
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
    OpenViewers();
  }
  return vwr;
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
  if (viewer_name != "(default name)") {
    vwr->SetName(viewer_name);
    vwr->SigEmitUpdated();
  }
  OpenViewers(); // opens both 2x2 if we made those (2x2 no longer supported)
}

void taProject::OpenNewProjectViewer(String viewer_name) {
  MainWindowViewer* vwr =  MainWindowViewer::NewProjectViewer(this); // added to viewers
  if (viewer_name != "(default name)") {
    vwr->SetName(viewer_name);
    vwr->SigEmitUpdated();
  }
  vwr->ViewWindow();
}

void taProject::OpenViewers() {
  for (int i = 0; i < viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (!vwr) continue;
    //TODO: add a auto_open flag to viewers, but allow prol UserData soln
    if (vwr->GetUserDataDef("auto_open", true).toBool())
      vwr->ViewWindow(); // noop if already open
  }
}

void taProject::RefreshAllViews() {
  if(!taMisc::gui_active) return;
  for (int i = 0; i < viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
    imwv->viewRefresh();
  }
}

void taProject::UpdateUi() {
  if(!taMisc::gui_active) return;
  for (int i = 0; i < viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
    imwv->UpdateUi();
  }
}

void taProject::DelayedUpdateUi() {
  if(!taMisc::gui_active) return;
  for (int i = 0; i < viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
    imwv->DelayedUpdateUi();
  }
}

void taProject::SelectT3ViewTabNo(int tab_idx) {
  MainWindowViewer* proj_view = GetDefaultProjectViewer();
  if(!proj_view || !proj_view->SelectT3ViewTabNo(tab_idx)) {
    taMisc::Warning("could not activate 3D View Tab number:", String(tab_idx));
  }
}

void taProject::SelectT3ViewTabName(const String& tab_nm) {
  MainWindowViewer* proj_view = GetDefaultProjectViewer();
  if(!proj_view || !proj_view->SelectT3ViewTabName(tab_nm)) {
    taMisc::Warning("could not activate 3D View Tab named:", tab_nm);
  }
}

DataTable* taProject::GetNewInputDataTable(const String& nw_nm, bool msg) {
  DataTable_Group* dgp = (DataTable_Group*)data.FindMakeGpName("InputData");
  DataTable* rval = NULL;
  if(!nw_nm.empty()) {
    rval = dgp->FindName(nw_nm);
    if(rval) return rval;
  }
  rval = dgp->NewEl(1, &TA_DataTable);
  if(!nw_nm.empty()) {
    rval->name = nw_nm;
    rval->SigEmitUpdated();
  }
  if(msg)
    taMisc::Info("Note: created new data table named:", rval->name, "in .data.InputData");
  return rval;
}

DataTable* taProject::GetNewOutputDataTable(const String& nw_nm, bool msg) {
  DataTable_Group* dgp = (DataTable_Group*)data.FindMakeGpName("OutputData");
  DataTable* rval = NULL;
  if(!nw_nm.empty()) {
    rval = dgp->FindName(nw_nm);
    if(rval) return rval;
  }
  rval = dgp->NewEl(1, &TA_DataTable);
  if(!nw_nm.empty()) {
    rval->name = nw_nm;
    rval->SigEmitUpdated();
  }
  if(msg)
    taMisc::Info("Note: created new data table named:", rval->name, "in .data.OutputData");
  return rval;
}

DataTable* taProject::GetNewAnalysisDataTable(const String& nw_nm, bool msg) {
  DataTable_Group* dgp = (DataTable_Group*)data.FindMakeGpName("AnalysisData");
  DataTable* rval = NULL;
  if(!nw_nm.empty()) {
    rval = dgp->FindName(nw_nm);
    if(rval) return rval;
  }
  rval = dgp->NewEl(1, &TA_DataTable);
  if(!nw_nm.empty()) {
    rval->name = nw_nm;
    rval->SigEmitUpdated();
  }
  if(msg)
    taMisc::Info("Note: created new data table named:", rval->name, "in .data.AnalysisData");
  return rval;
}

void taProject::SetProjAsCurrent() {
  cur_proj = this;
  if(proj_dir.empty()) {
    proj_dir = QDir::currentPath(); // default to current now..
  }
  QDir::setCurrent(proj_dir);
}

bool taProject::SetFileName(const String& val) {
  if (GetFileName() == val)
    return true;
  inherited::SetFileName(val);
  if(taMisc::interactive && auto_name && !file_name.contains("_autosave")) {
    AutoNameProj(file_name);
  }
  MainWindowViewer* vwr = GetDefaultProjectBrowser();
  if(vwr) {
    vwr->SetWinName();
  }
  if(val.empty()) {
    proj_dir = taMisc::GetHomePath();
    return true;
  }
  QFileInfo fi(val);
  proj_dir = fi.absolutePath();
  // note: too dangerous to save root, since we are still saving project...
  // BUT changes should get saved when we close the filer anyway
  if(taMisc::gui_active) {
    if(!file_name.contains("proj_templates")) { // exclude new from template guys
      tabMisc::root->AddRecentFile(file_name, true); // use file_name instead of val b/c it is cannonicalized!
    }
  }
  return true;
}

int taProject::Save_strm(ostream& strm, taBase* par, int indent) {
  if(!file_name.contains("proj_templates")) { // exclude new from template guys
    SaveSetAuthor();
  }
      
  int rval = GetTypeDef()->Dump_Save(strm, (void*)this, par, indent);
  setDirty(false);
  return rval;
}

void taProject::AddMeAsAuthor(bool sole_author, bool updt_license) {
  if(sole_author) {
    author = "";
    email = "";
    updt_license = true;
  }
  if(author.empty())
    author = taMisc::project_author;
  else
    author += ", " + taMisc::project_author;
  if(email.empty())
    email = taMisc::author_email;
  else
    email += ", " + taMisc::author_email;
  if(updt_license) {
    license.license = taMisc::license_def;
    license.owner_name = taMisc::license_owner;
    license.org = taMisc::license_org;
    license.year = QDateTime::currentDateTime().toString("yyyy");
  }
  SigEmitUpdated();
}

void taProject::SaveSetAuthor() {
  if(author.empty() && taMisc::project_author.nonempty()) {
    author = taMisc::project_author;
  }
  if(email.empty() && taMisc::author_email.nonempty()) {
    email = taMisc::author_email;
  }
  if(license.license == taLicense::NO_LIC && taMisc::license_def != taLicense::NO_LIC) {
    license.license = taMisc::license_def;
    license.owner_name = taMisc::license_owner;
    license.org = taMisc::license_org;
    license.year = QDateTime::currentDateTime().toString("yyyy");
  }
}

int taProject::Save() {
  String fname = GetFileName(); // empty if 1st or not supported
  if(fname.contains("_recover")) {
    int chs = taMisc::Choice("This appears to be a recover file that was saved during a previous crash -- you may not want to save to this file name", "Save to this _recover file", "Let me choose a new name", "Save to non-_recover version of this file");
    if(chs == 1) fname = "";    // this will prompt for name
    else if(chs == 2) {
      fname = fname.before("_recover") + fname.from(".",-1);
    }
  }
  if(fname.contains("_autosave")) {
    int chs = taMisc::Choice("This appears to be an auto-save file -- you may not want to save to this file name", "Save to this _autosave file", "Let me choose a new name", "Save to non-_autosave version of this file");
    if(chs == 1) fname = "";    // this will prompt for name
    else if(chs == 2) {
      fname = fname.before("_autosave") + fname.from(".",-1);
    }
  }
  return SaveAs(fname);
}


void taProject::AutoNameProj(const String& fname) {
  String nwnm = taMisc::GetFileFmPath(fname);
  if(nwnm.contains(".proj")) nwnm = nwnm.before(".proj",-1);
  SetName(nwnm);
}

void taProject::SaveViewState() {
  if(!taMisc::gui_active) return;
  for (int i = 0; i < viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
    imwv->viewSaveView();
  }
  if(taMisc::console_win) {
    taMisc::console_win->SaveGeom();
  }
}

int taProject::SaveAs(const String& fname) {
  // note: Save() calls SaveAs -- common path here
  bool tmp_save_as_only = save_as_only;
  if (save_as_only)
    save_as_only = false;   // ok to save the new project on top of itself so set to false before the save

  int rval = false;
  taFiler* flr = GetSaveFiler(fname, _nilString, -1, _nilString, true, tmp_save_as_only);
  
  if (flr->ostrm) {
    if (save_view) { // save current view with project
      SaveViewState();
    }
    
    QFileInfo fi(flr->FileName()); // set to current working dir
    proj_dir = fi.absolutePath();
    QDir::setCurrent(fi.absolutePath());
    if(auto_name) {
      AutoNameProj(flr->FileName());
    }
    Save_strm(*(flr->ostrm));
    flr->Close();
    rval = true;
    // remove autosave and recover files after successful save of non-recover version
    String fnm = flr->FileName();
    if(!fnm.contains("_recover") && !fnm.contains("_autosave")) {
      CleanFiles();
    }
    OpenProjectLog();
  }
  else { // if the save wasn't successful reset the save_as_only flag
    save_as_only = tmp_save_as_only;
  }
  
  taRefN::unRefDone(flr);
  SigEmit(SLS_ITEM_UPDATED_ND);
  return rval;
}

int taProject::SaveCopy(const String& fname) {
  int rval = false;
  taFiler* flr = GetSaveFiler(fname, _nilString, -1, _nilString, false, true);
  // false = getset_file_name, true = make_copy
  
  if (flr->ostrm) {
    if (save_view) { // save current view with project
      SaveViewState();
    }
    
    QFileInfo fi(flr->FileName()); // set to current working dir
    Save_strm(*(flr->ostrm));
    flr->Close();
    rval = true;
  }
  
  taRefN::unRefDone(flr);
  SigEmit(SLS_ITEM_UPDATED_ND);
  return rval;
}

bool taProject::PublishProjectOnWeb(const String &wiki_name)
{
  if (GetFileName().empty()) {
    int choice = taMisc::Choice("The project must be saved locally before it can be published", "Save", "Cancel");
    if (choice == 0) {
      Save();
    }
    else {
      return false;
    }
  }
  
  if (GetFileName().empty()) { // was the project really saved?
    return false;
  }
  wiki.wiki = wiki_name;
  if(wiki.page_name.empty())    // use name by default
    wiki.page_name = name;
  return taMediaWiki::PublishItemOnWeb(wiki_name, "Project", this->name, GetFileName(), wiki.page_name, tags, desc, version, author, email, pub_cite, this);
}

bool taProject::UpdateProjectOnWeb(const String& wiki_name) {
  // todo: we could do more here to synchronize updates from wiki etc
  // to a update change log prompt here!
  String msg = "Project updated on wiki: " + wiki_name + " changes: ";
  String com_itm_str = "Record this web update in project ChangeLog doc:";
  bool updt_change_log = true;
  bool do_it = SvnCommitDialog(msg, updt_change_log, com_itm_str);
  if(do_it) {
    if(updt_change_log) {
      RecordChangeLog(msg);
      Save();
    }
    return taMediaWiki::UpdateItemOnWeb(wiki_name, "Project", this->name, GetFileName(),
                                        version, this);
  }
  return false;
}


bool taProject::UploadFilesForProjectOnWeb(const String &wiki_name) {
  bool rval = false;
  MainWindowViewer* browser = GetDefaultProjectBrowser();
  if (browser) {
    if (QWidget *widget = browser->widget()) {
      QFileDialog file_dlg(widget);
      file_dlg.setFileMode(QFileDialog::ExistingFiles);
      QStringList file_name_list;
      if (file_dlg.exec()) {
        file_name_list = file_dlg.selectedFiles();
      }
      if (!file_name_list.empty()) {
        rval = true;
        for (int i=0; i<file_name_list.size(); i++) {
          String file_name = file_name_list.at(i);
          String file_name_only = taMisc::GetFileFmPath(file_name);
          if (taMediaWiki::FileExists(wiki_name, file_name_only)) {
            String info = "File " + file_name + " already on wiki. Upload revision?";
            int choice = taMisc::Choice(info, "Upload Revision", "Cancel");
            if (choice == 0) {
              rval = taMediaWiki::UploadOtherFile(wiki_name, "Project", file_name, this->name, true); // true - update new revision
            }
          }
          else { // 1st upload
            rval = taMediaWiki::UploadOtherFile(wiki_name, "Project", file_name, this->name, false);
          }
        }
      }
      taMisc::Info("Upload Completed!");
    }
  }
  return rval;
}

bool taProject::OpenProjectFromWeb(const String& proj_file_name, const String& wiki_name) {
  return tabMisc::root->OpenProjectFromWeb(proj_file_name, wiki_name);
}

String taProject::GetProjTemplatePath(ProjLibs library) {
  String path = "./";
  path = taMisc::proj_template_paths[library].value.toString();
  if(library == SYSTEM_LIB && taMisc::in_dev_exe) {
    String top_lev_path = taMisc::GetDirFmPath(taMisc::exe_path, 1); // go up 1
    path = top_lev_path + "/proj_templates";
  }
  if(library < WEB_APP_LIB) {
    QFileInfo qfi(path);
    if(!qfi.isDir()) {
      QDir qd;
      qd.mkpath(path);          // attempt to make it..
      taMisc::Warning("Note: did mkdir for project template library directory:", path);
    }
  }
  return path;
}

void taProject::SaveAsTemplate(const String& template_name, const String& desc,
                               const String& tagsa, ProjLibs library) {
  String path = GetProjTemplatePath(library);
  String fname = path + "/" + template_name + ".proj";
  QFileInfo qfi(fname);
  if(qfi.isFile()) {
    int chs = taMisc::Choice("Project template file: " + fname + " already exists: Overwrite?",
                             "Ok", "Cancel");
    if(chs == 1) return;
  }
  SaveAs(fname);
  
  String infofnm = fname.before(".proj",-1) + ".tmplt"; // template info
  fstream strm;
  strm.open(infofnm, ios::out);
  if(strm.bad() || strm.eof()) {
    taMisc::Error("Project::SaveAsTemplate: could not open template info file name for saving:",
                  infofnm);
    strm.close();
    return;
  }
  strm << "name=" + template_name << endl;
  strm << "tags=" + tagsa << endl;
  strm << "desc=" + desc << endl;
  strm.close();

  Project_Group::proj_templates.FindProjects();
}

int taProject::Load(const String& fname, taBase** loaded_obj_ptr) {
  TestError(true, "Load", "Cannot load a new project file on top of an existing project -- must load an entirely new project");
  return 0;
}

void taProject::OpenProjectLog() {
  if(!taMisc::gui_active) return; //  only for gui..
  if (taMisc::project_log) {  // only if the user wants a log file!!
    if(file_name.empty()) return;
    String log_fn = file_name;
    if(log_fn.contains(".proj"))
      log_fn = log_fn.before(".proj",-1);
    log_fn += ".plog";
    taMisc::SetLogFile(log_fn);
  }
}

bool taProject::CleanFiles() {
  bool got_one = false;
  String fnm = file_name.before(".proj");
  String autosave = fnm + "_autosave.proj";
  got_one |= QFile::remove(autosave.toQString());
  tabMisc::root->recent_files.RemoveEl(autosave);
  for(int i=0;i<100;i++) {
    String recover = fnm + "_recover" + String(i) + ".proj";
    got_one |= QFile::remove(recover.toQString());
    tabMisc::root->recent_files.RemoveEl(recover);

    String console = fnm + "_console" + String(i) + ".txt";
    QFile::remove(console.toQString());

    recover = fnm + "_autosave_recover" + String(i) + ".proj";
    got_one |= QFile::remove(recover.toQString());
    tabMisc::root->recent_files.RemoveEl(recover);

    recover = fnm + "_recover" + String(i) + "_autosave.proj";
    got_one |= QFile::remove(recover.toQString());
    tabMisc::root->recent_files.RemoveEl(recover);

    console = fnm + "_autosave_console" + String(i) + ".txt";
    QFile::remove(console.toQString());
  }
  if(got_one && taMisc::dmem_proc == 0) {
    tabMisc::root->Save();      // save with updated files list
  }
  return got_one;
}

void taProject::ViewLicense() {
  license.ViewLicense();
}

void taProject::ViewProjLog_Editor() {
  String fnm = file_name.before(".proj");
  fnm += ".plog";
  taMisc::EditFile(fnm);
}

void taProject::ViewProjLog() {
  String fnm = file_name.before(".proj");
  fnm += ".plog";
  fstream fh;
  fh.open(fnm, ios::in);
  if(fh.good()) {
    view_plog.Load_str(fh);
    TypeDef* td = GetTypeDef();
    MemberDef* md = td->members.FindName("view_plog");
    taiEditorOfString* host_ = new taiEditorOfString(md, this, td, true, false, NULL, true);
    // args are: read_only, modal, parent, line_nos
    host_->Constr("Project Log for Project: " + name);
    host_->Edit(false);
  }
  else {
    taMisc::Warning("could not open project log file:", fnm);
  }
}

int taProject::SaveNoteChanges() {
  UpdateChangeLog();
  return Save();
}

int taProject::SaveAsNoteChanges(const String& fname) {
  UpdateChangeLog();
  return SaveAs(fname);
}

void taProject::setDirty(bool value) {
  // Set the window state to modified/unmodified.  Only attempt if the dirty
  // flag has actually changed, since this may be expensive and called often.
  if (value != m_dirty) {
    if (MainWindowViewer *browser = GetDefaultProjectBrowser()) {
      if (QWidget *widget = browser->widget()) {
        widget->setWindowModified(value);
      }
    }
  }

  // note: inherited only forwards 'dirty' up the chain, not '!dirty'
  inherited::setDirty(value);
  m_dirty = value;
  if (!value) m_no_save = false;
}

String taProject::GetDir() {
  return taMisc::GetDirFmPath(GetFileName());
}

void taProject::SvnBrowser() {
  String path = taMisc::GetDirFmPath(GetFileName());
  if(path.nonempty()) {
    iSubversionBrowser* svb = iSubversionBrowser::OpenBrowser("", path);
  }
}

bool taProject::SvnAdd() {
  Save();
  bool in_repo = true;
  String path = GetFileName();
  int svn_rev = 0;
  if(path.nonempty()) {
    in_repo = iSvnFileListModel::FileInRepo(path, svn_rev);
    if (in_repo) {
      String msg = "The project " + path + " is already in the repository";
      taMisc::Error(msg);
      return false;
    }
    if (iSvnFileListModel::AddFile(taMisc::GetDirFmPath(path),
                                   taMisc::GetFileFmPath(path))) {
      String msg;
      String com_itm_str = path + " A";
      bool updt_change_log = true;
      bool do_it = SvnCommitDialog(msg, updt_change_log, com_itm_str);
      if(do_it) {
        if(updt_change_log) {
          RecordChangeLog(msg);
          Save();
        }
        bool project_file_only = true;
        bool first_commit = true;
        return iSvnFileListModel::CommitFile(path, project_file_only, msg, first_commit);
      }
    }
  }
  return false;
}

void taProject::SvnCommit(bool project_file_only) {
  Save();
  bool in_repo = true;
  String path = GetFileName();
  int svn_rev = 0;
  if (project_file_only) {
    in_repo = iSvnFileListModel::FileInRepo(path, svn_rev);
    if (!in_repo) {
      iDialogChoice::ConfirmDialog(NULL, "Project file not found in repository - to add choose SVN Add from the file menu", "", false);
    }
  }
  
  if(path.nonempty() && in_repo) {
    String msg;
    String com_itm_str = path + " M";
    bool updt_change_log = true;
    bool do_it = SvnCommitDialog(msg, updt_change_log, com_itm_str);
    if(do_it) {
      if(updt_change_log) {
        String ulmsg;
        if(svn_rev > 0)
          ulmsg = "svn rev: " + String(svn_rev+1) + " " + msg;
        else
          ulmsg = msg;
        RecordChangeLog(ulmsg);
        Save();
      }
      iSvnFileListModel::CommitFile(path, project_file_only, msg, false); // false = first commit
    }
  }
}

bool taProject::SvnCommitDialog(String& commit_msg, bool& updt_change_log,
                                const String& com_itm_str) {
  taGuiDialog dlg;
  taBase::Ref(dlg);   // no need to UnRef - will be deleted at end of method
  
  dlg.win_title = "Svn Commit Message";
  dlg.width = taiMisc::resizeByMainFont(400);
  dlg.height = taiMisc::resizeByMainFont(200);
  
  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);
  
  String row("");
  int space = 5;
  
  row = "prompt";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("prmpt_lbl", widget, row, "label=Provide Commit Message:;");
  
  row = "itms";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("itms_lbl", widget, row, "label=" + com_itm_str +";");
  
  row = "msg";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  // use a text field not a single line edit box
  iTextEdit* text_edit = new iTextEdit();
  // Get the hbox for this row so we can add our combobox to it.
  text_edit->setText(commit_msg);
  taGuiLayout *hboxEmer = dlg.FindLayout(row);
  if (!hboxEmer)
    return false;
  QBoxLayout *hbox = hboxEmer->layout;
  if (!hbox)
    return false;
  hbox->addWidget(text_edit);

  row = "chlog";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String tt = "tooltip=This will add a new entry in the docs/ChangeLog doc with the svn commit message and time-stamp etc -- increments version step by 1 -- provides a project-local record of changes -- recommended!;";
  dlg.AddLabel("chlog_lbl", widget, row, "label=Update Change Log: ;" + tt);
  dlg.AddBoolCheckbox(&updt_change_log, "usechlog", widget, row, tt);
  dlg.AddStretch(row);
  
  int drval = dlg.PostDialog(true);
  if(drval) {
    commit_msg = String(text_edit->toPlainText());
    return true;
  }
  return false;
}

void taProject::SvnDiff() {
  SubversionClient svn_client;
  try {
    svn_client.SetWorkingCopyPath(proj_dir);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in setting working copy\n", ex.what());
    return;
  }
  String fnm = taMisc::GetFileFmPath(file_name);
  if(fnm.empty()) {
    taMisc::Error("file name for project is empty!");
    return;
  }
  int rev = 0;
  int kind = 0;
  String root_url;
  int last_change_rev = 0;
  int last_changed_date = 0;
  String last_changed_author;
  int64_t sz = 0;
  try {
    svn_client.GetInfo(fnm, rev, kind, root_url, last_change_rev, last_changed_date,
                        last_changed_author, sz);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in GetInfo\n", ex.what());
    return;
  }

  String change_str = taDateTime::fmTimeToString(last_changed_date,
                                                 "yyyy_MM_dd_hh_mm_ss");
  
  taMisc::Info("Doing DiffCompare on file:", fnm, "svn revision last changed:",
               String(last_change_rev), "on:", change_str, "by:", last_changed_author);
  String rev_fnm = fnm.before(".proj") + "_" + String(last_change_rev) + ".proj";

  if(taMisc::FileExists(rev_fnm)) {
    taMisc::RemoveFile(rev_fnm);
  }
  
  String url;
  try {
    svn_client.GetUrlFromPath(url, fnm);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in GetUrl\n", ex.what());
    return;
  }
  
  try {
    svn_client.SaveFile(url, rev_fnm, last_change_rev);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in SaveFile\n", ex.what());
    return;
  }

  int pidx = tabMisc::root->projects.size;
  tabMisc::root->projects.Load(rev_fnm);
  taProject* proj = tabMisc::root->projects.SafeEl(pidx);
  taMisc::ProcessEvents();
  if(proj) {
    ObjDiff* diff = new ObjDiff;
    diff->a_only = true;        // don't allow changes to B
    diff->Diff(proj, this);      // compute the diffs
    diff->DisplayDialog(false);  // non-modal -- dialog disposes of diff
  }
}

void taProject::SvnPrevDiff() {
  SubversionClient svn_client;
  try {
    svn_client.SetWorkingCopyPath(proj_dir);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in setting working copy\n", ex.what());
    return;
  }
  String fnm = taMisc::GetFileFmPath(file_name);
  if(fnm.empty()) {
    taMisc::Error("file name for project is empty!");
    return;
  }
  int rev = 0;
  int kind = 0;
  String root_url;
  int last_change_rev = 0;
  int last_changed_date = 0;
  String last_changed_author;
  int64_t sz = 0;
  try {
    svn_client.GetInfo(fnm, rev, kind, root_url, last_change_rev, last_changed_date,
                        last_changed_author, sz);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in GetInfo\n", ex.what());
    return;
  }

  String change_str = taDateTime::fmTimeToString(last_changed_date,
                                                 "yyyy_MM_dd_hh_mm_ss");
  
  taMisc::Info("Last SVN revision info for file:", fnm, "rev:",
               String(last_change_rev), "on:", change_str, "by:", last_changed_author);
  String rev_fnm = fnm.before(".proj") + "_" + String(last_change_rev) + ".proj";

  if(taMisc::FileExists(rev_fnm)) {
    taMisc::RemoveFile(rev_fnm);
  }
  
  String url;
  try {
    svn_client.GetUrlFromPath(url, fnm);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in GetUrl\n", ex.what());
    return;
  }
  
  try {
    svn_client.SaveFile(url, rev_fnm, last_change_rev);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in SaveFile\n", ex.what());
    return;
  }

  // now try to find prior revision!
  int_PArray    revs;            // one per rev -- revision number
  String_PArray commit_msgs;     // one per rev -- the commit message
  String_PArray authors;         // one per rev -- author of rev
  int_PArray    times;           // one per rev -- time as secs since 1970
  int_PArray    files_start_idx; // one per rev -- starting index in files/actions
  int_PArray    files_n;    // one per rev -- number of files in files/actions
  String_PArray files;      // raw list of all files for all logs
  String_PArray actions; // one-to-one with files, mod action for each file

  int prev_rev = last_change_rev-1;
  int search_chunk = 100;
  while(true) {
    try {
      svn_client.GetLogs(revs, commit_msgs, authors, times, files_start_idx, files_n,
                          files, actions, url, prev_rev, search_chunk);
    }
    catch (const SubversionClient::Exception &ex) {
      taMisc::Error("Error doing GetLog in SubversionClient.\n", ex.what());
      return;
    }
    if(revs.size > 0) break;
    prev_rev -= search_chunk;
    if(prev_rev < search_chunk)
      break;
  }

  if(revs.size == 0) {
    taMisc::Error("No previous revision found!");
    return;
  }

  // find highest revision
  int highest_rev = -1;
  int rev_idx = -1;
  for(int i=0; i<revs.size; i++) {
    int revt = revs[i];
    if(revt > highest_rev) {
      highest_rev = revt;
      rev_idx = i;
    }
  }
  
  change_str = taDateTime::fmTimeToString(times[rev_idx], "yyyy_MM_dd_hh_mm_ss");
  prev_rev = revs[rev_idx]; 
  taMisc::Info("Prior SVN revision info for file: " + fnm + " rev:",
               String(prev_rev), "on:", change_str, "by:", authors[rev_idx], "message:\n",
               commit_msgs[rev_idx]);
  String prev_fnm = fnm.before(".proj") + "_" + String(prev_rev) + ".proj";

  if(taMisc::FileExists(prev_fnm)) {
    taMisc::RemoveFile(prev_fnm);
  }
  try {
    svn_client.SaveFile(url, prev_fnm, prev_rev);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in SaveFile\n", ex.what());
    return;
  }

  int pidx = tabMisc::root->projects.size;
  tabMisc::root->projects.Load(rev_fnm);
  taProject* last_proj = tabMisc::root->projects.SafeEl(pidx);
  taMisc::ProcessEvents();
  tabMisc::root->projects.Load(prev_fnm);
  taProject* prev_proj = tabMisc::root->projects.SafeEl(pidx+1);
  taMisc::ProcessEvents();
  if(last_proj && prev_proj) {
    ObjDiff* diff = new ObjDiff;
    diff->Diff(prev_proj, last_proj);      // compute the diffs
    diff->DisplayDialog(false);  // non-modal -- dialog disposes of diff
  }
}

void taProject::DiffMerge(taProject* vers1, taProject* vers2) {
  ObjDiff* diff1 = new ObjDiff;
  diff1->a_only = true;        // don't allow changes to B
  diff1->Diff(this, vers1); // compute the diffs
  diff1->DisplayDialog(false);  // non-modal -- dialog disposes of diff
  
  ObjDiff* diff2 = new ObjDiff;
  diff2->a_only = true;        // don't allow changes to B
  diff2->Diff(this, vers2); // compute the diffs
  diff2->DisplayDialog(false);  // non-modal -- dialog disposes of diff
}

void taProject::DiffMergeFiles(const String& vers1_fname, const String& vers2_fname) {
  int pidx = tabMisc::root->projects.size;
  tabMisc::root->projects.Load(vers1_fname);
  taProject* v1_proj = tabMisc::root->projects.SafeEl(pidx);
  if(!v1_proj) return;
  pidx++;
  tabMisc::root->projects.Load(vers2_fname);
  taProject* v2_proj = tabMisc::root->projects.SafeEl(pidx);
  if(!v2_proj) return;

  taMisc::ProcessEvents();
  DiffMerge(v1_proj, v2_proj);
}

void taProject::CleanSvnProjs() {
  QDir dir(proj_dir);
  QStringList files = dir.entryList();
  for(int i=0;i<files.size();i++) {
    String fl = files[i];
    if(!fl.startsWith(name) || !fl.endsWith(".proj"))
      continue;
    String aft = fl.after(name);
    if(!aft.startsWith('_')) continue;
    aft = aft.after('_');
    aft = aft.before(".proj", -1);
    if(!aft.isInt()) continue;
    taMisc::Info("Removing svn project file:", fl);
    taMisc::RemoveFile(fl);
  }
}

bool taProject::DiffCompare(taBase* cmp_obj) {
  return inherited::DiffCompare(cmp_obj);
}

void taProject::UpdateChangeLog() {
  TypeDef* td = GetTypeDef();
  MemberDef* md = td->members.FindName("last_change_desc");
  taiEditorOfString* dlg = new taiEditorOfString(md, this, td, false); // false = not read only
  dlg->Constr("Please enter a detailed description of the changes made to the project since it was last saved -- this will be recorded in a docs object called ChangeLog.  You can use self-contained HTML formatting tags.  <b>NOTE: Cancel</b> here is <i>only</i> for the change log entry -- not for the project save!");
  if(dlg->Edit(true)) {
    RecordChangeLog(last_change_desc);
  }
  delete dlg;
}

void taProject::RecordChangeLog(const String& msg) {
  version.step++;               // increment the step always
  last_change_desc = msg;       // record last change msg
  
  time_t tmp = time(NULL);
  String tstamp = ctime(&tmp);
  tstamp = tstamp.before('\n');

  String vers = version.GetString();

  String user = taMisc::UserName();
  String host = taMisc::HostName();
  if (host.nonempty()) user += String("@") + host;

  String cur_fname = taMisc::GetFileFmPath(file_name);
  String prv_fname = taMisc::GetFileFmPath(GetFileName());

  if(prv_fname == cur_fname) prv_fname = "";
  else prv_fname = "(was: <code>" + prv_fname + "</code>)";

  String nw_txt = "\n<li>" + tstamp
    + " version: " + vers + " user: " + user + " file_name: <code>" + cur_fname
    + "</code> " + prv_fname + "<br>\n";
  if(!msg.empty()) nw_txt += msg + "\n";

  taDoc* doc = docs.FindName("ChangeLog");
  if(!doc) {
    doc = docs.NewEl(1);
    doc->name = "ChangeLog";
    doc->text = "<html>\n<head>ChangeLog</head>\n<body>\n<h1>ChangeLog</h1>\n<ul>\n";
    doc->text += nw_txt;
    doc->text += "</ul>\n</body>\n</html>\n";
  }
  else {
    String hdr = trim(doc->text.through("<ul>"));
    String trl = trim(doc->text.after("<ul>"));
    doc->text = hdr + "\n\n" + nw_txt + trl;
  }
  doc->UpdateText();
  doc->SigEmitUpdated();
}

void taProject::UndoStats(bool show_list, bool show_diffs) {
  undo_mgr.ReportStats(show_list, show_diffs);
}

void taProject::UndoSaveCurRec() {
  undo_mgr.SaveCurSrcRec();
}

void taProject::ReplaceString(const String& srch, const String& repl) {
  undo_mgr.SaveUndo(this, "ReplaceString", NULL, false, this); // global save

  String proj_str;
  tabMisc::cur_undo_save_top = this; // let others know who we're saving for..
  tabMisc::cur_undo_mod_obj = this; // let others know who we're saving for..
  tabMisc::cur_undo_save_owner = this;
  ++taMisc::is_undo_saving;
  Save_String(proj_str);
  --taMisc::is_undo_saving;
  tabMisc::cur_undo_save_top = NULL;
  tabMisc::cur_undo_mod_obj = NULL;
  tabMisc::cur_undo_save_owner = NULL;

  proj_str.gsub(srch, repl);

  ++taMisc::is_undo_loading;
  Load_String(proj_str);
  taMisc::ProcessEvents();      // get any post-load things *before* turning off undo flag..
  --taMisc::is_undo_loading;

  tabMisc::DelayedFunCall_gui(this,"RefreshAllViews");
}

String taProject::GetAutoFileName(const String& suffix, const String& ftype_ext) {
  String rval;
  if(file_name.empty()) {
    rval = taMisc::user_dir + PATH_SEP;
    if(name.empty()) {
      rval += GetTypeDef()->name;
    }
    else {
      rval += name;
    }
  }
  else {
    if(file_name.contains(ftype_ext)) {
      rval = file_name.before(ftype_ext, -1);
    }
    else {
      rval = file_name;         // whatever
    }
  }
  if(rval.contains(suffix))
    rval = rval.through(suffix, -1);
  else
    rval += suffix;
  return rval;
}

void taProject::SaveRecoverFile_strm(ostream& strm) {
  int rval = GetTypeDef()->Dump_Save(strm, (void*)this);
  //  setDirty(false);  // definitely not
}

void taProject::SaveRecoverFile() {
  String ftype_ext = ".proj";
  String newfm = GetAutoFileName("_recover", ftype_ext);
  int cnt = taMisc::GetUniqueFileNumber(0, newfm, ftype_ext);
  String fnm = newfm + String(cnt) + ftype_ext; // note: this is a full path!
  QFileInfo qfi(fnm);
  QFileInfo qfd(qfi.path());
  bool usr_fl = false;
  if(!qfd.isWritable()) {
    usr_fl = true;
    fnm = taMisc::user_dir + PATH_SEP + taMisc::GetFileFmPath(fnm);
  }
  taFiler* flr = GetSaveFiler(fnm, _nilString, -1, _nilString);
  bool saved = false;
  if(flr->ostrm) {
    SaveRecoverFile_strm(*flr->ostrm);
    saved = true;
  }
  if(usr_fl) {
    taMisc::DebugInfo("Error saving recover file in original location -- now saved in user directory: ", fnm);
  }
  flr->Close();
  taRefN::unRefDone(flr);
  // log filename -- get abs path
  if (saved) {
    // save root, which wasn't saved
    tabMisc::root->Save();
  }

  // now try to save console
  if(saved) {
    String cfnm = fnm;
    cfnm.gsub("_recover", "_console");
    cfnm.gsub((const char*)ftype_ext, ".txt");
    QcssConsole* qcons = QcssConsole::getInstance();
    if(qcons)
      qcons->saveContents(cfnm);
  }
}

bool taProject::AutoSave(bool force) {
  if(!force) {
    if(!isDirty()) {                    // don't save until dirty
      auto_save_timer.StartTimer(true); // reset
      return false;
    }
    if(auto_save_timer.start.tot == 0) { // first timer..
      auto_save_timer.StartTimer(true); // reset
      return false;
    }
    auto_save_timer.EndTimer();
    if(auto_save_timer.s_used < (double)taMisc::auto_save_interval) {
      auto_save_timer.StartTimer(false); // don't reset!
      return false;             // not yet
    }
    // ok, times up!
  }

  String orig_fnm = file_name;

  String ftype_ext = ".proj";
  String newfnm = GetAutoFileName("_autosave", ftype_ext) + ftype_ext;
  String fnm = newfnm; // note: this is a full path!
  QFileInfo qfi(fnm);
  QFileInfo qfd(qfi.path());
  if(!qfd.isWritable()) {       // use path!
    fnm = taMisc::user_dir + PATH_SEP + taMisc::GetFileFmPath(fnm);
    taMisc::DebugInfo("Error saving auto save file in original location:", newfnm,
                      " -- now saved in user directory:", fnm);
  }
  taFiler* flr = GetSaveFiler(fnm, _nilString, -1, _nilString);
  bool saved = false;
  if(flr->ostrm) {
    if(taMisc::undo.debug)
      taMisc::Info("Autosave start...");
    ++taMisc::is_auto_saving;
    int rval = GetTypeDef()->Dump_Save(*flr->ostrm, (void*)this);
    // note: not using Save_strm to preserve the dirty bit!
    --taMisc::is_auto_saving;
    saved = true;
    if(taMisc::undo.debug)
      taMisc::Info("Autosave end.");
  }
  flr->Close();
  taRefN::unRefDone(flr);

  auto_save_timer.StartTimer(true); // start it up for next time around..

  // restore original:
  file_name = orig_fnm;
  MainWindowViewer* vwr = GetDefaultProjectBrowser();
  if(vwr) {
    vwr->SetWinName();
  }
  return true;
}

void taProject::CloseLater() {
  undo_mgr.diff_threads.RemoveAll();
  // get rid of any undo saving that might just be happening
  inherited::CloseLater();
}

void taProject::SetSaveView(bool value) {
  save_view = value;
}

String taProject::ProgGlobalStatus() {
  String rval;
  if(Program::global_run_state == Program::RUN) {
    rval = "Run: ";
    if(last_run_prog) {
      rval += " " + last_run_prog->name; // could use short..
    }
  }
  else if(Program::global_run_state == Program::STOP) {
    rval = Program::DecodeStopReason(Program::stop_reason);
    if(last_stop_prog) {
      rval += last_stop_prog->name;
    }
  }
  else if(Program::global_run_state == Program::INIT) {
    rval = "Init: ";
    if(last_run_prog) {
      rval += " " + last_run_prog->name; // could use short..
    }
  }
  else if(Program::global_run_state == Program::DONE) {
    rval = "Done: ";
    if(last_run_prog) {
      rval += " " + last_run_prog->name; // could use short..
    }
  }
  else if(Program::global_run_state == Program::NOT_INIT) {
    rval = "No Program Initialized";
  }
  return rval;
}

void taProject::ParamSetComparePeers(ParamSet* key_set, ParamSet* peer_set) {
  if (!key_set || !peer_set) return;
  
  DataTable_Group* dgp = (DataTable_Group*)data.FindMakeGpName("ParamData");
  String table_name = key_set->name + "_param_peer_table";
  DataTable* param_table = (DataTable*)dgp->FindLeafName_(table_name);
  
  if (!param_table) {
    param_table = dgp->NewEl(1, NULL);   // add a new data table to the group
    param_table->SetName(table_name);
    param_table->ClearDataFlag(DataTable::SAVE_ROWS); // don't save these
    param_table->StructUpdate(true);
  
    DataCol* dc_member = (DataCol*)param_table->FindMakeCol("Member", taBase::VT_STRING);
    dc_member->SetColFlag(DataCol::READ_ONLY);
    
    DataCol* dc_spec = (DataCol*)param_table->FindMakeCol(key_set->name, taBase::VT_STRING);
    dc_spec->SetColFlag(DataCol::READ_ONLY);
    param_table->StructUpdate(false);
    param_table->RefreshViews();
    
    WriteParamMbrNamesToTable(param_table, key_set);
    WriteParamSavedValsToTable(param_table, key_set);
  }
  else {
    WriteParamMbrNamesToTable(param_table, peer_set);
    WriteParamSavedValsToTable(param_table, peer_set);
  }

  param_table->StructUpdate(true);
  AddPeerToParamCompareTable(param_table, peer_set);
  param_table->StructUpdate(false);
  tabMisc::DelayedFunCall_gui(param_table, "BrowserSelectMe");
}

void taProject::WriteParamMbrNamesToTable(DataTable* param_table, ParamSet* param_set) {
  if (!param_table || !param_set) return;

  DataCol* mbr_name_column = param_table->data.FindName("Member");
  if (!mbr_name_column) return;
  
  int index = param_table->rows; // we are appending
  for (int m=0; m<param_set->mbrs.size; m++) {
    ControlPanelMember* item = param_set->mbrs.FastEl(m);
    String name = item->GetName();
    if (!mbr_name_column) break;
    if (mbr_name_column->FindVal(name) == -1) {
      param_table->AddBlankRow();
      param_table->SetValAsVar(name, 0, index);
      index++;
    }
  }
}

void taProject::WriteParamSavedValsToTable(DataTable* param_table, ParamSet* param_set) {
  if (!param_table || !param_set) return;
  
  DataCol* mbr_name_column = param_table->data.FindName("Member");
  if (!mbr_name_column) return;

  DataCol* param_column = param_table->data.FindName(param_set->name);
  if (!param_column) return;

  for (int m=0; m<param_set->mbrs.size; m++) {
    ControlPanelMember* item = param_set->mbrs.FastEl(m);
    int row = mbr_name_column->FindVal(item->GetName());
    String value = item->data.saved_value;
    param_table->SetValAsVar(value, param_column->GetName(), row);
  }
}

void taProject::AddPeerToParamCompareTable(DataTable* param_table, ParamSet* peer_set) {
  if (!param_table->FindColName(peer_set->name)) {
    param_table->StructUpdate(true);
    DataCol* dc = (DataCol*)param_table->FindMakeCol(peer_set->name, taBase::VT_STRING);
    if (dc) {
      dc->SetColFlag(DataCol::READ_ONLY);
    }
    param_table->StructUpdate(false);
  }
  // regardless update values
  WriteParamMbrNamesToTable(param_table, peer_set);  // and any members not in first peer
  WriteParamSavedValsToTable(param_table, peer_set); // not child, is peer
}

