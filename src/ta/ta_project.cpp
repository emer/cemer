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

// 

#include "ta_project.h"

#include "ta_dump.h"
#include "ta_css.h"

#include <QCoreApplication>

#ifdef TA_GUI
# include "css_qt.h"
# include "ta_qt.h"
# include "ta_qtdata.h" // for taiObjChooser
# include "ta_qtdialog.h"
# include "ta_qttype_def.h"

# include <QWidgetList>
#endif

#include <time.h>

#ifdef DMEM_COMPILE
# include <mpi.h>
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
// note: we also include a couple of members we know are in pdp
    if (!(md->name.contains("desc") || (md->name == "use_sim_log") || (md->name == "save_rmv_units")
	 || (md->name == "prev_file_nm"))) return false;
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
  taBase::Own(programs, this);
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
  programs.CutLinks();
  edits.CutLinks();
  wizards.CutLinks();
  templates.CutLinks();
}

void taProject::Copy_(const taProject& cp) {
  // delete things first, to avoid dangling references
  viewers.Reset();
  edits.Reset();
  programs.Reset();
  
  templates = cp.templates;
  wizards = cp.wizards;
  edits = cp.edits;
  viewers = cp.viewers;
  programs = cp.programs;
  // NOTE: once a derived project has all the relevant stuff copied, it needs to call this:
  // UpdatePointers_NewPar(&cp, this); // update pointers within entire project..
  setDirty(true);
}

void taProject::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
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

int taProject::Load(istream& strm, TAPtr par, void** el) {
  int rval = inherited::Load(strm, par, el); // load-em-up
  if (rval) {	 // don't do this as a dump_load_value cuz we need an updateafteredit..
    if (taMisc::gui_active) {
      tabMisc::post_load_opr.Link(&wizards);
//       pdpMisc::post_load_opr.Link(&programs);
#ifdef TA_GUI
      tabMisc::post_load_opr.Link(&edits);
#endif
    } else {
      wizards.AutoEdit();
//       programs.AutoRun();
#ifdef TA_GUI
      edits.AutoEdit();
#endif
    }
  }
  return rval;
}

void taProject::AssertDefaultProjectBrowser(bool auto_open) {
  MainWindowViewer* vwr = NULL;
  // get the default one, if there is one unopened
  vwr = GetDefaultProjectBrowser();
  if (!vwr) {
    vwr = MakeProjectBrowser_impl();
    vwr->SetName("DefaultProjectBrowser");
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

int taProject::SaveAs(ostream& strm, TAPtr par, int indent) {
#ifdef TA_GUI
  if (use_sim_log) {
    UpdateSimLog();
  }
#endif
  return inherited::SaveAs(strm, par, indent);
}

void taProject::setDirty(bool value) {
  // note: inherited only forwards 'dirty' up the chain, not '!dirty'
  inherited::setDirty(value);
  m_dirty = value;
}


bool taProject::SetFileName(const String& val) {
  prev_file_nm = GetFileName();
  return inherited::SetFileName(val);
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
    fh << file_name << " <- " << prev_file_nm << "\t" << tstamp << "\t" << user << endl;
    if(!desc.empty()) fh << "\t" << desc << endl;
    fh.close(); fh.clear();
  }
#endif
}

//////////////////////////
//   Project_Group	//
//////////////////////////

int Project_Group::Load(istream& strm, TAPtr par) {
/*obs  if ((ta_file != NULL) && !ta_file->dir.empty() && (ta_file->dir != ".")) {
    // change directories to where the project was loaded!
    ta_file->GetDir();
    chdir(ta_file->dir);
    String fnm = ta_file->fname.after('/',-1);
    ta_file->dir = "";
    ta_file->fname = fnm;
  } */
  int rval = inherited::Load(strm, par);
  if (rval) {
    taProject* p;
    taLeafItr i;
    FOR_ITR_EL(taProject, p, this->, i) {
      if (taMisc::gui_active) {
	tabMisc::post_load_opr.Add(&(p->wizards));
// 	tabMisc::post_load_opr.Add(&(p->programs));
	tabMisc::post_load_opr.Add(&(p->edits));
      } else {
	p->wizards.AutoEdit();
// 	p->programs.AutoRun();
	p->edits.AutoEdit();
      }
    }
  }
  return rval;
}


//////////////////////////
//   taRoot		//
//////////////////////////

void taRootBase::Initialize() {
  version_no = taMisc::version_no;
  SetName("root");
  projects.SetName("projects");
}

void taRootBase::Destroy() {
  CutLinks(); // note: will prob already have been done
  bool we_are_root = (tabMisc::root == this); // 'true' if we are the one and only root app object
  if (we_are_root) {
    tabMisc::root = NULL; //TODO: maybe for cleanness we should do a SetPointer thingy, since we set it that way...
#ifdef DMEM_COMPILE
    if(taMisc::dmem_nprocs > 1) {
      taMisc::RecordScript(".Quit();\n");
    } else
#endif
  }
}

void taRootBase::InitLinks() {
  inherited::InitLinks();
  taBase::Own(templates, this);
  taBase::Own(projects, this);
  taBase::Own(viewers, this);
  AddTemplates(); // note: ok that this will be called here, before subclass has finished its own
}

void taRootBase::CutLinks() {
  viewers.CutLinks();
  projects.CutLinks();
  templates.CutLinks();
  inherited::CutLinks();
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
