// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// for debugging the qconsole, uncomment this define and it will revert to qandd console
// #define QANDD_CONSOLE 1

#include "ta_project.h"
#include "ta_platform.h"
#include "ta_dump.h"
#include "ta_plugin.h"
#include "ta_gendoc.h"
#include "ta_server.h"

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
# include <QDesktopServices>
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

//////////////////////////
//   taDoc		//
//////////////////////////

const String taDoc::init_text(
"<html>\n"
"<head></head>\n"
"<body>\n"
"== Enter Title Here ==\n"
"</body>\n"
"</html>\n");

void taDoc::Initialize() {
  auto_open = false;
  if (!taMisc::is_loading && !taMisc::is_duplicating)
    text = init_text;
}

void taDoc::UpdateText() {
  html_text = WikiParse(text);
}

void taDoc::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateText();
}

static String wiki_parse_str_between(const String& cl, const String& sts, const String& eds) {
  if(cl.freq(sts) == 1 && cl.freq(eds) == 1) {
    String btwn = cl.after(sts);
    btwn = btwn.before(eds);
    return btwn;
  }
  return _nilString;
}

String taDoc::WikiParse(const String& in_str) {
  String rval;
  String rest = in_str;
  bool bullet1 = false;
  bool bullet2 = false;
  bool num1 = false;
  bool num2 = false;
  while(rest.contains("\n")) {
    String cl = rest.before("\n");
    rest = rest.after("\n");

    // need bullet first because it sets context with <ul> </ul>
    if(cl.startsWith("* ")) {
      if(bullet2) { cl = "</ul> <li> " + cl.after("* "); bullet2 = false; }
      else if(!bullet1) cl = "<ul><li> " + cl.after("* ");
      else cl = "<li> " + cl.after("* ");
      bullet1 = true;
    }
    else if(cl.startsWith("** ")) {
      if(!bullet2) cl = "<ul><li> " + cl.after("** ");
      else cl = "<li> " + cl.after("** ");
      bullet2 = true;
    }
    else if(cl.startsWith("# ")) {
      if(num2) { cl = "</ol> <li> " + cl.after("# "); num2 = false; }
      else if(!num1) cl = "<ol><li> " + cl.after("# ");
      else cl = "<li> " + cl.after("# ");
      num1 = true;
    }
    else if(cl.startsWith("## ")) {
      if(!num2) cl = "<ol><li> " + cl.after("## ");
      else cl = "<li> " + cl.after("## ");
      num2 = true;
    }
    else {
      if(bullet2) { cl += "</ul>"; bullet2 = false; }
      if(bullet1) { cl += "</ul>"; bullet1 = false; }

      if(num2) { cl += "</ol>"; num2 = false; }
      if(num1) { cl += "</ol>"; num1 = false; }
    }

    if(cl.empty()) {		// make a <P> for blank lines..
      rval += "<P>\n";
      continue;
    }

    // headers
    if(wiki_parse_str_between(cl, "==== ", " ====").nonempty()) {
      cl.gsub("==== ", "<h4> ");
      cl.gsub(" ====", " </h4>");
    }
    else if(wiki_parse_str_between(cl, "=== ", " ===").nonempty()) {
      cl.gsub("=== ", "<h3> ");
      cl.gsub(" ===", " </h3>");
    }
    else if(wiki_parse_str_between(cl, "== ", " ==").nonempty()) {
      cl.gsub("== ", "<h2> ");
      cl.gsub(" ==", " </h2>");
    }
    else if(wiki_parse_str_between(cl, "= ", " =").nonempty()) {
      cl.gsub("= ", "<h1> ");
      cl.gsub(" =", " </h1>");
    }

    // links
    String href = wiki_parse_str_between(cl, "[[", "]]");
    if(!href.empty()) {
      bool ta_tag = false;
      if(href.startsWith('.')) {
	ta_tag = true;
	href = "ta:" + href;
      }
      String tag = href;
      if(tag.contains('|')) {
	href = href.before('|');
	tag = tag.after('|');
      }
      else if(ta_tag) {
	if(tag.contains("()")) {
	  String fnm = tag.after('.',-1);
	  tag = tag.before('.',-1);
	  if(tag.contains('.')) { // should!
	    tag = tag.after('.',-1);
	  }
	  tag += "." + fnm;
	}
	else {
	  tag = tag.after('.',-1);
	}
      }
      cl = cl.before("[[") + "<a href=\"" + href + "\">" + tag + "</a>" + cl.after("]]");
    }

    // bold
    String bld = wiki_parse_str_between(cl, " '''", "''' ");
    if(!bld.empty()) {
      cl = cl.before(" '''") + " <b>" + bld + "</b> " + cl.after("''' ");
    }
    else {
      bld = wiki_parse_str_between(cl, " ''", "'' ");
      if(!bld.empty()) {
	cl = cl.before(" ''") + " <i>" + bld + "</i> " + cl.after("'' ");
      }
    }

    rval += cl + "\n";
  }
  return rval;
}


//////////////////////////////////
//  Doc_Group		//
//////////////////////////////////

void Doc_Group::AutoEdit() {
  taDoc* doc;
  taLeafItr i;
  FOR_ITR_EL(taDoc, doc, this->, i) {
    if (doc->auto_open)
      doc->EditPanel(true, true); // true,true = new tab, pinned in place
  }
}


//////////////////////////
//   taWizard		//
//////////////////////////

void taWizard::Initialize() {
  auto_open = true;
  SetUserData("NO_CLIP", true);
}

//////////////////////////////////
// 	Wizard_Group		//
//////////////////////////////////

void Wizard_Group::AutoEdit() {
  taWizard* wz;
  taLeafItr i;
  FOR_ITR_EL(taWizard, wz, this->, i) {
    if (wz->auto_open)
      wz->EditPanel(true, true);// true,true = new tab, pinned in place
  }
}

//////////////////////////////////
//	SelectEdit_Group	//
//////////////////////////////////

void SelectEdit_Group::AutoEdit() {
  taLeafItr i;
  SelectEdit* se;
  FOR_ITR_EL(SelectEdit, se, this->, i) {
    if(se->config.auto_edit)
      se->EditPanel(true, true);	// true,true = new tab, pinned in place
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
    if (!(md->name.contains("desc") || (md->name == "use_change_log") || (md->name == "save_rmv_units")
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
  use_change_log = true;
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
  taBase::Own(docs, this);
  taBase::Own(wizards, this);
  taBase::Own(edits, this);
  taBase::Own(data, this);
  taBase::Own(data_proc, this);
  taBase::Own(programs, this);
  taBase::Own(viewers, this);

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
    AssertDefaultProjectBrowser(true);
    AssertDefaultWiz(true);	// make default and do edit it
  } else {
    AssertDefaultProjectBrowser(false);
    AssertDefaultWiz(false);	// make default and don't edit it
  }
  // then select second tab -- see PostLoadAutos for more info
  if (taMisc::gui_active) {
    taiMiscCore::ProcessEvents();
    taiMiscCore::ProcessEvents();
    MainWindowViewer* vwr = GetDefaultProjectBrowser();
    if(vwr) {
      vwr->SelectPanelTabNo(1);
    }
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
  docs.CutLinks();
  templates.CutLinks();
}

void taProject::Copy_(const taProject& cp) {
  // delete things first, to avoid dangling references
  programs.Reset();
  viewers.Reset();
  data.Reset();

  edits.Reset();
  
  templates = cp.templates;
  docs = cp.docs;
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
  taBase* rval = data_proc.FindType(typ);
  if(rval) return rval;
  rval = data_proc.NewEl(1, typ);
  rval->SetName(nm);
  rval->DataChanged(DCR_ITEM_UPDATED);
  return rval;
}

SelectEdit* taProject::FindMakeSelectEdit(const String& nm) {
  SelectEdit* rval = edits.FindName(nm);
  if(rval) return rval;
  rval = (SelectEdit*)edits.New(1);
  rval->SetName(nm);
  rval->DataChanged(DCR_ITEM_UPDATED);
  return rval;
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
  if (taMisc::gui_active) {
    MainWindowViewer* vwr = AssertDefaultProjectBrowser(true);
    // note: we want a doc to be the default item, if possible
    docs.AutoEdit();
    wizards.AutoEdit();
    edits.AutoEdit();
    // this is very hacky... select the 2nd tab, which will 
    // be the first auto guy if there were any
    // make double sure..
    taiMiscCore::ProcessEvents();
    taiMiscCore::ProcessEvents();
    vwr->SelectPanelTabNo(1);
  }
  // this is done separately in startup code
  //  programs.RunStartupProgs();	// only at last step!
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
    vwr->DataChanged(DCR_ITEM_UPDATED);
  }
  OpenViewers(); // opens both 2x2 if we made those
}

void taProject::OpenNewProjectViewer(String viewer_name) {
  MainWindowViewer* vwr =  MainWindowViewer::NewProjectViewer(this); // added to viewers
  if (viewer_name != "(default name)") {
    vwr->SetName(viewer_name);
    vwr->DataChanged(DCR_ITEM_UPDATED);
  }
  vwr->ViewWindow();
}

void taProject::OpenViewers() {
  for (int i = 0; i < viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(viewers.FastEl(i));
    if (!vwr) continue;
    vwr->ViewWindow(); // noop if already open
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
    rval->DataChanged(DCR_ITEM_UPDATED);
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
    rval->DataChanged(DCR_ITEM_UPDATED);
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
    rval->DataChanged(DCR_ITEM_UPDATED);
  }
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
  return true;
}

int taProject::Save_strm(ostream& strm, TAPtr par, int indent) {
#ifdef TA_GUI
  if (use_change_log) {
    UpdateChangeLog();
  }
#endif
  int rval = inherited::Save_strm(strm, par, indent);
  return rval;
}

void taProject::setDirty(bool value) {
  // note: inherited only forwards 'dirty' up the chain, not '!dirty'
  inherited::setDirty(value);
  m_dirty = value;
  if (!value) m_no_save = false;
}


void taProject::UpdateChangeLog() {
#ifdef TA_GUI
  TypeDef* td = GetTypeDef();
  MemberDef* md = td->members.FindName("last_change_desc");
  taiStringDataHost* dlg = new taiStringDataHost(md, this, td, false); // false = not read only
  dlg->Constr("Please enter a detailed description of the changes made to the project since it was last saved -- this will be recorded in a docs object called ChangeLog.  You can use self-contained HTML formatting tags.  To turn off this feature, toggle use_change_log on the Project.  <b>NOTE: Cancel</b> here is <i>only</i> for the change log entry -- not for the project save!");
  if(dlg->Edit(true)) {
    time_t tmp = time(NULL);
    String tstamp = ctime(&tmp);
    tstamp = tstamp.before('\n');

    String user = taPlatform::userName();
    String host = taPlatform::hostName();
    if (host.nonempty()) user += String("@") + host;

    String cur_fname = taMisc::GetFileFmPath(file_name);
    String prv_fname = taMisc::GetFileFmPath(GetFileName());

    if(prv_fname == cur_fname) prv_fname = "";
    else prv_fname = "(was: <code>" + prv_fname + "</code>)";

    String nw_txt = "\n<li>" + tstamp + " " + user + " <code>" + cur_fname
      + "</code>" + prv_fname + "<br>\n";
    if(!last_change_desc.empty()) nw_txt += "  " + last_change_desc + "\n";

    taDoc* doc = docs.FindName("ChangeLog");
    if(!doc) {
      doc = docs.NewEl(1);
      doc->name = "ChangeLog";
      doc->text = "<html>\n<head>ChangeLog</head>\n<body>\n<h1>ChangeLog</h1>\n<ul>\n";
      doc->text += nw_txt;
      doc->text += "</ul>\n</body>\n</html>\n";
    }
    else {
      String hdr = doc->text.through("<ul>\n");
      String trl = doc->text.after("<ul>\n");
      doc->text = hdr + nw_txt + trl;
    }
    doc->UpdateText();
    doc->DataChanged(DCR_ITEM_UPDATED);
  }
#endif
}

void taProject::SaveRecoverFile() {
  bool tmp_change_log = use_change_log;
  use_change_log = false;		// don't pop up any dialogs..

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
      use_change_log = false;
      SaveRecoverFile_strm(*flr->ostrm);
      saved = true;
    }
  }
  flr->Close();
  taRefN::unRefDone(flr);

  use_change_log = tmp_change_log;

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

int Project_Group::Load(const String& fname, taBase** loaded_obj_ptr) { 
  // chg working dir to that of project -- simplifies lots of stuff immensely
  QFileInfo fi(fname);
  QDir::setCurrent(fi.absolutePath());
  int rval = inherited::Load(fname, loaded_obj_ptr);
  return rval;
}

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
  version = taMisc::version;
  taBase::Own(templates, this);
  taBase::Own(projects, this);
  taBase::Own(viewers, this);
  taBase::Own(plugins, this);
  taBase::Own(plugin_deps, this);
  taBase::Own(mime_factories, this);
  taBase::Own(colorspecs, this);
  // create colorspecs even if nogui, since they are referenced in projects
  colorspecs.SetDefaultColor();	
  taBase::Own(objs, this);
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
  objs.CutLinks();
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
  info += "The Emergent Toolbox (TEMT) Info\n";
  info += "This is the TEMT software package, version: ";
  info += taMisc::version;
  info += "\n\n";
  info += "WWW Page: http://grey.colorado.edu/temt\n";
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
  int accepted = ie->EditDialog(inst, false, true); // r/w, modal
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

  taMisc::AddArgName("-i", "CssInteractive");
  taMisc::AddArgName("--interactive", "CssInteractive");
  taMisc::AddArgNameDesc("CssInteractive", "\
 -- Specifies that the css console should remain active after running a css script file upon startup");

  taMisc::AddArgName("-ni", "CssNonInteractive");
  taMisc::AddArgName("--non-interactive", "CssNonInteractive");
  taMisc::AddArgNameDesc("CssNonInteractive", "\
 -- Specifies that the css console should NOT be activated at all during running (e.g., if a STARTUP_RUN program is present that will run and then quit out)");

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
  
  // Server variables
  taMisc::AddArgName("--server", "Server");
  taMisc::AddArgNameDesc("Server", "\
 -- Run the app as a tcp server");

  taMisc::AddArgName("--port", "Port");
  taMisc::AddArgName("port=", "Port");
  taMisc::AddArgNameDesc("Port", "\
 -- Specifies the tcp port for server mode (def=5360");

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
//   // we will just take the last one found on cmd line
//   for (int i = taMisc::argc - 1; i > 0; --i) {
//     String arg = argv[i];
//      if (arg.endsWith("-nogui")) {
//       taMisc::use_gui = false; 
//       break;
//     } else if (arg.endsWith("-gui")) {
//       taMisc::use_gui = true; 
//       break;
//     }
//   }

  if(taMisc::CheckArgByName("GenDoc")) { // auto nogui by default
    taMisc::use_gui = false;
    cssMisc::init_interactive = false;
  }

  // need to use Init_Args and entire system because sometimes flags get munged together
  if(taMisc::CheckArgByName("NoGui"))
    taMisc::use_gui = false;
  else if(taMisc::CheckArgByName("Gui"))
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
    // get optional style override
    String gstyle;
    if(taMisc::gui_style != taMisc::GS_DEFAULT) {
      gstyle = TA_taMisc.GetEnumString("GuiStyle", taMisc::gui_style).after("GS_").downcase();
    }
// quasi-temp hack because Mac style on Mac breaks layouts in 4.3.1
// # if defined(TA_OS_MAC) && (QT_VERSION >= 0x040300) // && (QT_VERSION < 0x040400)
//     if (gstyle.empty()) gstyle = "windows"; // this looks nice and works
// # endif
    if(gstyle.nonempty()) {
      QApplication::setStyle(gstyle);
    }
# ifdef TA_USE_INVENTOR
    new QApplication(argc, (char**)argv); // accessed as qApp
    SoQt::init(argc, (char**)argv, cssMisc::prompt.chars()); // creates a special Coin QApplication instance
    milestone |= (SM_QAPP_OBJ | SM_SOQT_INIT);
# else
    new QApplication(argc, (char**)argv); // accessed as qApp
    milestone |= SM_QAPP_OBJ;
# endif
    QString app_ico_nm = ":/images/" + taMisc::app_name + "_32x32.png";
    QPixmap app_ico(app_ico_nm);
    QApplication::setWindowIcon(app_ico);
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
  
/* NOTE: we give priority to the EMERGENTDIR variable first, for
   development purposes. Otherwise, we use the heuristic search
   based on the platform, and as a last resort, prompt user.
*/
  app_dir = getenv("EMERGENTDIR");
  if (app_dir.nonempty() && isAppDir(app_dir))
    goto have_app_dir;

  //  app_dir = QCoreApplication::applicationDirPath();
  app_dir = "";			// TODO: above is just causing error right now, so 
  // have commented it out for time being until replaced by native version (bug #154)
  // note that this actually doesn't seem to be causing any problems, and a quick
  // review of code below suggests that windows is the only platform where this is 
  // actually being used!
#ifdef TA_OS_WIN
/*
  {app_dir}\bin
*/
  // note: Qt docs say it returns the '/' version...
  if (app_dir.endsWith("/bin") || app_dir.endsWith("\bin")) {
    app_dir = app_dir.at(0, app_dir.length() - 4);
    if (isAppDir(app_dir)) goto have_app_dir;
  }
#else // Mac and Unix -- defaults
  // this is the normal install default, and possible alternate
  app_dir = "/usr/local/share/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/opt/local/share/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;

# if defined(TA_OS_MAC)
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
    
# else // non-Mac Unix
/*
  {app_dir}/bin
*/
  if (app_dir.endsWith("/bin")) {
    app_dir = app_dir.at(0, app_dir.length() - 4);
    if (isAppDir(app_dir)) goto have_app_dir;
  }
# endif // non-Mac Unix-specific

// continue with Mac/Unix tests
  // on Unix platforms, check the other plausible prefix folders
  app_dir = "/usr/local/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/usr/share/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/usr/local/src/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/usr/share/src/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  // these are more usual on Mac or BSD-ish ones:
  app_dir = "/opt/local/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/opt/share/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/opt/local/src/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;
  app_dir = "/opt/share/src/" + taMisc::default_app_install_folder_name;
  if (isAppDir(app_dir)) goto have_app_dir;

#endif // all modality

  // common code for failure to grok the app path
  //  app_dir = QCoreApplication::applicationDirPath();
  // TODO: see note above.
  // first, maybe it is actually the exe's folder itself?
  if (isAppDir(app_dir)) goto have_app_dir;
  
  // is it the current directory? -- probe with known subfolder
  app_dir = QDir::currentPath();
  if (isAppDir(app_dir)) goto have_app_dir;
  
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

bool FileWithContentExists(const String& in1, const String& fname) {
  bool rval = false;
  fstream in2;

  int i1 = 0;
  in2.open(fname, ios::in | ios::binary);
  if (!in2.is_open()) goto exit1;
  char c1;  char c2;
  bool g1;  bool g2;
  while (true) {
    g1 = (i1 < in1.length()); // we have at least one more char
    g2 = in2.get(c2);
    if (!g1 && !g2) break; // same size, done 
    if (!(g1 && g2)) goto exit; // different sizes
    c1 = in1.elem(i1++);
    if (c1 != c2) goto exit;  // different content
  }
  rval = true; 

exit:
  in2.close();
exit1:
  return rval;
}

bool MakeUserPluginConfigProxy(const String& uplugin_dir,
  const String& fname) 
{
//note: isEmpty takes a raw var, not its content
  String f(
"tmpEMERGENT_DIR = $(EMERGENTDIR)\n"
"isEmpty( tmpEMERGENT_DIR ) {\n"
"  tmpEMERGENT_DIR = " + taMisc::app_dir + "\n"
"}\n"
"!include($${tmpEMERGENT_DIR}/plugins/" + fname + ")  {\n"
"  error(\"could not find <emergentdir>/plugins/" + fname + "\")\n"
"}\n");
  String tfname = uplugin_dir + "/" + fname;
  if (FileWithContentExists(f, tfname)) return true;
  ofstream ofs(tfname, ios_base::out | ios_base::trunc);
  if (!ofs.is_open()) return false;
  ofs << f;
  ofs.close();
  return true;
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
  // plugin making stuff should only run when interactive
  if (taMisc::use_gui && (taMisc::dmem_proc == 0)) {
    // make the user plugin folder, and assert the proxy files
    // we redo those every time, in case app has been upgraded
    // but don't fail if this can't be done -- just warn user
    String uplugin_dir = taMisc::user_app_dir + PATH_SEP + "plugins";
    bool err = false;
    String msg;
    if (!dir.exists(uplugin_dir) && !dir.mkdir(uplugin_dir)) {
      err = true;
      msg += "Startup_InitTA_initUserAppDir: can't make " + uplugin_dir + "\n";
    }
    if (!MakeUserPluginConfigProxy(uplugin_dir, "config.pri")) {
      err = true;
      msg += "Startup_InitTA_initUserAppDir: can't make config.pri\n";
    }
    if (!MakeUserPluginConfigProxy(uplugin_dir, "shared_pre.pri")) {
      err = true;
      msg += "Startup_InitTA_initUserAppDir: can't make shared_pre.pri\n";
    }
    if (!MakeUserPluginConfigProxy(uplugin_dir, "shared.pri")) {
      err = true;
      msg += "Startup_InitTA_initUserAppDir: can't make shared.pri\n";
    }
    // copy the Makefile
    QFile::remove(uplugin_dir + "/Makefile");
    if (!QFile::copy(taMisc::app_dir + "/plugins/Makefile.plugin",
        uplugin_dir + "/Makefile")) {
      err = true;
      msg += "Startup_InitTA_initUserAppDir: can't copy plugins/Makefile.plugin\n";
    }
    // todo: this err message is pointless until the plugins/Makefile can be
    // installed, and it can't be installed because it would conflict with the Makefile.am
    // that would allow it to be installed according to the standard configure
    // auto everything whatever.  need a workaround, and then can remove false below..
    if (false && err) {
      taMisc::Warning(msg);
      taMisc::Warning("Your user folder could not be set up properly to build plugins -- this will not affect running the basic application but will prevent you from building or compiling your own plugins. Please contact your system administrator.");
    }
  } // gui mode
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
  if (taMisc::proj_view_pref == -1) {
    taMisc::proj_view_pref = taMisc::PVP_3PANE;
  }

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
  // make sure the app dir is on the recent paths
  if (instance()->recent_paths.FindEl(taMisc::app_dir) < 0) {
    instance()->AddRecentPath(taMisc::app_dir);
  }
  return true;
}
  	
bool taRootBase::Startup_EnumeratePlugins() {
#ifdef TA_OS_WIN
  String plug_dir = "\\lib"; 
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
  // add plugin folders
  taPlugins::AddPluginFolder(taMisc::app_dir + plug_dir + PATH_SEP + "plugins" + plug_sub);
  taPlugins::AddPluginFolder(taMisc::user_app_dir + plug_dir + PATH_SEP + "plugins" + plug_sub);
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
    // the following should be done in the specific app's Main function
//     taiM->icon_bitmap = new QBitmap(emergent_bitmap_width,
//     	emergent_bitmap_height, emergent_bitmap_bits);
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
  taMisc::view_colors->FindMakeViewColor("Doc", "Documentation object",
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
  tabMisc::root->version = taMisc::version;
  if(!taMisc::gui_active) return true;
#ifdef TA_GUI
  // TODO: need to better orchestrate the "OpenWindows" call below with
  // create the default application window
  MainWindowViewer* db = MainWindowViewer::NewBrowser(tabMisc::root, NULL, true);
  // try to size fairly large to avoid scrollbars
  db->SetUserData("view_win_wd", 0.6f);
  float ht = 0.5f; // no console
//  iSize s(1024, 480); // no console  (note: values obtained empirically)
  if ((console_type == taMisc::CT_GUI) && (!(console_options & taMisc::CO_GUI_TRACKING))) {
    ht = 0.8f; // console
    ConsoleDockViewer* cdv = new ConsoleDockViewer;
    db->docks.Add(cdv);
  }
  db->SetUserData("view_win_ht", ht); 
  db->ViewWindow();
  iMainWindowViewer* bw = db->viewerWindow();
  if (bw) { //note: already constrained to max screen size, so we don't have to check
    // main win handle internal app urls
    taiMisc::main_window = bw;
    QDesktopServices::setUrlHandler("ta", bw, "globalUrlHandler");
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
  if(taMisc::CheckArgByName("GenDoc")) {
    fstream gen_doc_xml;
    gen_doc_xml.open(taMisc::app_name + "_TA_doc.xml", ios::out);
    taGenDoc::GenDoc(&(taMisc::types), gen_doc_xml);
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

  bool startup_run = false;
  if(tabMisc::root->projects.size == 1) {
    taProject* prj = tabMisc::root->projects[0];
    startup_run = prj->programs.RunStartupProgs();
  }
  if(!cssMisc::init_interactive || (!taMisc::gui_active && startup_run)) taiMC_->Quit();

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
  if(!Startup_InitArgs(argc, argv)) goto startup_failed;
  if(!Startup_ProcessGuiArg(argc, argv)) goto startup_failed;
  if(!Startup_InitTA(ta_init_fun)) goto startup_failed;
  if(!Startup_InitApp(argc, argv)) goto startup_failed;
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

  // if in server mode, make it now!
  if (taMisc::args.FindName("Server") >= 0) {
    TemtServer* server = (TemtServer*)instance()->objs.New(1, &TA_TemtServer);
    server->port = (ushort)taMisc::args.GetValDef("Port", 5360).toUInt();
    if (server->InitServer() && server->OpenServer()) {
      cerr << "TemtServer is now running and waiting for connections\n";
    } else {
      cerr << "ERROR: could not Initialize or Open TemtServer\n";
    }
  }
  
  
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
