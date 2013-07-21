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

#include "taProject.h"
#include <taiEditorOfClass>
#include <MainWindowViewer>
#include <iMainWindowViewer>
#include <taiMiscCore>
#include <iTreeView>
#include <taFiler>
#include <iDialogPublishDocs>
#include <taiEditorOfString>

#include <ctime>

taTypeDef_Of(taDataProc);
taTypeDef_Of(taDataAnal);
taTypeDef_Of(taDataGen);
taTypeDef_Of(taImageProc);
taTypeDef_Of(ClusterRun);

#include <SigLinkSignal>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>

#include <QDir>
#include <QFileInfo>

#include <css_machine.h>
#include <css_qtconsole.h>


using namespace std;


#ifdef TA_GUI
class SimLogEditDialog: public taiEditorOfClass {
public:
  override bool ShowMember(MemberDef* md) const {
    // just show a small subset of the members
    bool rval = (md->ShowMember(show()) && (md->im != NULL));
    if (!rval) return rval;
// note: we also include a couple of members we know are in taProject
    if (!(md->name.contains("desc") || (md->name == "version") || (md->name == "save_rmv_units")
         || (md->name == "file_name"))) return false;
    return true;
  }
  override void Constr_Methods_impl() { }       // suppress methods

  SimLogEditDialog(void* base, TypeDef* tp, bool read_only_,
        bool modal_) : taiEditorOfClass(base, tp, read_only_, modal_) { };
};
#endif


void taProject::Initialize() {
  auto_name = true;
  m_dirty = false;
  m_no_save = false;
  viewers.SetBaseType(&TA_TopLevelViewer);
  viewers_tmp.SetBaseType(&TA_TopLevelViewer);
  save_view = true;
  save_as_only = false;
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
  taBase::Own(wiki_url, this);
  taBase::Own(templates, this);
  taBase::Own(docs, this);
  taBase::Own(wizards, this);
  taBase::Own(edits, this);
  taBase::Own(data, this);
  taBase::Own(data_proc, this);
  taBase::Own(programs, this);
  taBase::Own(viewers, this);
  taBase::Own(viewers_tmp, this);
  taBase::Own(undo_mgr, this);

  // note: any derived programs should install additional guys..
  // put in NO_CLIP to suppress clip ops, since we don't want any for these guys
  FindMakeNewDataProc(&TA_taDataProc, "data_base")->SetUserData("NO_CLIP", true);
  FindMakeNewDataProc(&TA_taDataAnal, "data_anal")->SetUserData("NO_CLIP", true);
  FindMakeNewDataProc(&TA_taDataGen, "data_gen")->SetUserData("NO_CLIP", true);
  FindMakeNewDataProc(&TA_taImageProc, "image_proc")->SetUserData("NO_CLIP", true);
  // not actually useful to have these guys visible..  no user-accessible matrix objs
  // if in datatable, it should be accessible in above
//   FindMakeNewDataProc(&TA_taMath_float, "math_float")->SetUserData("NO_CLIP", true);
//   FindMakeNewDataProc(&TA_taMath_double, "math_double")->SetUserData("NO_CLIP", true);

}

void taProject::InitLinks_post() {
  if (!taMisc::is_loading) {
    // ensure we have a cluster run guy
    FindMakeSelectEdit("ClusterRun", &TA_ClusterRun);
    DoView();
  }
}

void taProject::CutLinks() {
  CutLinks_impl();
  inherited::CutLinks();
}

void taProject::CutLinks_impl() {
  viewers_tmp.CutLinks();
  viewers.CutLinks();
  programs.CutLinks();
  data_proc.CutLinks();
  data.CutLinks();
  edits.CutLinks();
  wizards.CutLinks();
  docs.CutLinks();
  templates.CutLinks();
}

void taProject::Copy_(const taProject& cp) {
  // delete things first, to avoid dangling references
  programs.Reset();
  viewers_tmp.Reset();
  viewers.Reset();
  data.Reset();

  edits.Reset();

  tags = cp.tags;
  templates = cp.templates;
  docs = cp.docs;
  wizards = cp.wizards;
  edits = cp.edits;
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
    FindMakeSelectEdit("ClusterRun", &TA_ClusterRun);
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

SelectEdit* taProject::FindMakeSelectEdit(const String& nm, TypeDef* type) {
  SelectEdit* rval = edits.FindName(nm);
  if(rval) return rval;
  rval = (SelectEdit*)edits.NewEl(1, type);
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

MainWindowViewer* taProject::GetDefaultProjectBrowser() {
  MainWindowViewer* vwr = NULL;
  // iterate to find 1st Browser -- will actually be 2nd item in 2x2
  for (int i = 0; i < viewers.size; ++i) {
    vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (vwr && vwr->isProjBrowser()) return vwr;
  }
  return NULL;
}

MainWindowViewer* taProject::GetDefaultProjectViewer() {
  // get the default T3 guy
  MainWindowViewer* vwr = NULL;
  // iterate to find 1st Viewer -- will actually be 1st item in 2x2
  for (int i = 0; i < viewers.size; ++i) {
    vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (vwr && vwr->isProjViewer()) return vwr;
  }
  return NULL;
}

void taProject::Dump_Load_post() {
  inherited::Dump_Load_post();
  if(taMisc::is_undo_loading) return; // none of this.
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
  MainWindowViewer* vwr = AssertDefaultProjectBrowser(true);
  if(!vwr) return;
  // allow to process new window before asserting default items
  // debug mode still doesn't work here tho.. 
  taMisc::ProcessEvents();
  // note: we want a doc to be the default item, if possible
  if(vwr->widget()) {
    vwr->widget()->GetMainTreeView()->ExpandDefault();
  }
  docs.AutoEdit();
  wizards.AutoEdit();
  edits.AutoEdit();
  // this is very hacky... select the 2nd tab, which will
  // be the first auto guy if there were any
  taiMiscCore::ProcessEvents();
  vwr->SelectPanelTabNo(1);
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
  OpenViewers(); // opens both 2x2 if we made those
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

void taProject::ProjDirToCurrent() {
  if(proj_dir.empty()) {
    proj_dir = QDir::currentPath(); // default to current now..
  }
  QDir::setCurrent(proj_dir);
}

bool taProject::SetFileName(const String& val) {
  if (GetFileName() == val) return true;
  inherited::SetFileName(val);
  QFileInfo fi(val);
  proj_dir = fi.absolutePath();
  MainWindowViewer* vwr = GetDefaultProjectBrowser();
  if(vwr) {
    vwr->SetWinName();
  }
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
  taMisc::save_use_name_paths = true; // project is one guy that DOES use name paths!
  int rval = GetTypeDef()->Dump_Save(strm, (void*)this, par, indent);
  setDirty(false);
  taMisc::save_use_name_paths = false; // default is off, so restore to default for everything else
  return rval;
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

int taProject::SaveAs(const String& fname) {
  if (save_view == true) { // save current view with project
    for (int i = 0; i < viewers.size; ++i) {
      MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
      if (!(vwr && vwr->isProjBrowser())) continue;
      iMainWindowViewer* imwv = vwr->widget();
      if(!imwv) continue;
      imwv->viewSaveView();
    }
  }

  bool tmp_save_as_only = save_as_only;
  if (save_as_only)
    save_as_only = false;   // ok to save the new project on top of itself so set to false before the save

  int rval = false;
  taFiler* flr = GetSaveFiler(fname, _nilString, -1, _nilString, true, tmp_save_as_only);

  if (flr->ostrm) {
    QFileInfo fi(flr->FileName()); // set to current working dir
    proj_dir = fi.absolutePath();
    QDir::setCurrent(fi.absolutePath());
    if(auto_name) {
      String nwnm = taMisc::GetFileFmPath(flr->FileName());
      if(nwnm.contains(".proj")) nwnm = nwnm.before(".proj",-1);
      SetName(nwnm);
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

void taProject::PublishDocsOnWeb(const String &repositoryName)
{
  iDialogPublishDocs dialog(repositoryName);
  if (dialog.exec()) {
    // User clicked OK.
    QString name = dialog.getName();
    QString desc = dialog.getDesc();
    QStringList tags = dialog.getTags();

    // TODO: upload the docs.  Initial plan was to turn the pub docs dialog
    // into an "uploading progress" dialog, but after trying a few things
    // it seems to make more sense to create a new progress dialog that
    // could also be used for the PublishProject stage.
  }
}

String taProject::GetProjTemplatePath(ProjLibs library) {
  if(library == SEARCH_LIBS) {
    taMisc::Error("Cannot do SEARCH_LIBS for saving -- program saved in local directory!");
    return "";
  }
  String path = "./";
  if(library == USER_LIB)
    path = taMisc::proj_template_paths.GetVal("UserLib").toString();
  else if(library == SYSTEM_LIB) {
    if(taMisc::in_dev_exe) {
      String top_lev_path = taMisc::GetDirFmPath(taMisc::exe_path, 1); // go up 1
      path = top_lev_path + "/proj_templates";
    }
    else {
      path = taMisc::proj_template_paths.GetVal("SystemLib").toString();
    }
  }
  else if(library == WEB_LIB)
    path = taMisc::proj_template_paths.GetVal("WebLib").toString();
  if(library != WEB_LIB) {
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
                          const String& tags, ProjLibs library) {
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
  strm << "tags=" + tags << endl;
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
  if(file_name.empty()) return;
  String log_fn = file_name;
  if(log_fn.contains(".proj"))
    log_fn = log_fn.before(".proj",-1);
  log_fn += ".plog";
  taMisc::SetLogFile(log_fn);
}

bool taProject::CleanFiles() {
  bool got_one = false;
  String fnm = file_name.before(".proj");
  String autosave = fnm + "_autosave.proj";
  got_one |= QFile::remove(autosave.chars());
  tabMisc::root->recent_files.RemoveEl(autosave);
  for(int i=0;i<100;i++) {
    String recover = fnm + "_recover" + String(i) + ".proj";
    got_one |= QFile::remove(recover.chars());
    tabMisc::root->recent_files.RemoveEl(recover);

    String console = fnm + "_console" + String(i) + ".txt";
    QFile::remove(console.chars());

    recover = fnm + "_autosave_recover" + String(i) + ".proj";
    got_one |= QFile::remove(recover.chars());
    tabMisc::root->recent_files.RemoveEl(recover);

    console = fnm + "_autosave_console" + String(i) + ".txt";
    QFile::remove(console.chars());
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


void taProject::UpdateChangeLog() {
#ifdef TA_GUI
  version.step++;               // increment the step always
  TypeDef* td = GetTypeDef();
  MemberDef* md = td->members.FindName("last_change_desc");
  taiEditorOfString* dlg = new taiEditorOfString(md, this, td, false); // false = not read only
  dlg->Constr("Please enter a detailed description of the changes made to the project since it was last saved -- this will be recorded in a docs object called ChangeLog.  You can use self-contained HTML formatting tags.  <b>NOTE: Cancel</b> here is <i>only</i> for the change log entry -- not for the project save!");
  if(dlg->Edit(true)) {
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
    if(!last_change_desc.empty()) nw_txt += last_change_desc + "\n";

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
  delete dlg;
#endif
}

void taProject::UndoStats(bool show_list, bool show_diffs) {
  undo_mgr.ReportStats(show_list, show_diffs);
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
  taMisc::save_use_name_paths = false; // no name paths for recover files
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
   if(cssMisc::TopShell->console_type == taMisc::CT_GUI) {
      String cfnm = fnm;
      cfnm.gsub("_recover", "_console");
      cfnm.gsub((const char*)ftype_ext, ".txt");
      QcssConsole* qcons = QcssConsole::getInstance();
      if(qcons)
        qcons->saveContents(cfnm);
    }
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
    taMisc::save_use_name_paths = false; // don't use name paths for autosave!
    int rval = GetTypeDef()->Dump_Save(*flr->ostrm, (void*)this);
    // note: not using Save_strm to preserve the dirty bit!
    saved = true;
  }
  flr->Close();
  taRefN::unRefDone(flr);

  auto_save_timer.StartTimer(true); // start it up for next time around..

  taMisc::DebugInfo("Saved auto save file:", fnm);

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
