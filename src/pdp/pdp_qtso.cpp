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




#include "pdp_qtso.h"
#include "pdpshell.h"
#include "ta_qt.h"
#include "ta_qtdialog.h"
#include "netstru_qtso.h"
#include "program.h"

#include "icombobox.h"
#include "ilineedit.h"
#include "ispinbox.h"

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QStackedWidget>

#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoTransform.h>


//////////////////////////////////
//	taiSpecMember		//
//////////////////////////////////

int taiSpecMember::BidForMember(MemberDef* md, TypeDef* td) {
  if((td->InheritsFrom(TA_BaseSpec) || td->InheritsFrom(TA_BaseSubSpec))
     && !(md->HasOption("NO_INHERIT")))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

// put some check here for if the spec base has no spec parent
//  (((TA_BaseSpec *) dlg->cur_base)->owner->GetOwner(dlg->typ) == NULL))

//   if((dlg != NULL) && (dlg->cur_base != NULL) && (dlg->typ != NULL) &&
//      dlg->typ->InheritsFrom(TA_BaseSpec) &&
//      (((BaseSpec*)dlg->cur_base)->GetOwner(&TA_BaseSpec) == NULL))
//   {

bool taiSpecMember::NoCheckBox(IDataHost* host_) const {
  void* base = host_->Base();
  TypeDef* typ = host_->GetBaseTypeDef();
  if((host_ == NULL) || (base == NULL) || (typ == NULL))
    return true;

  if(typ->InheritsFrom(TA_BaseSpec_Group))
    return false;		// always use a check box for these..

  if(typ->InheritsFrom(TA_BaseSpec)) {
    BaseSpec* bs = (BaseSpec*)base;
    if(bs->FindParent() != NULL)
      return false;		// owner has a parent spec, needs a box
    return true;		// no owner, no box..
  } else if(typ->InheritsFrom(TA_BaseSubSpec)) {
    BaseSubSpec* bs = (BaseSubSpec*)base;
    if(bs->FindParent() != NULL)
      return false;		// owner has a parent spec, needs a box
    return true;		// no owner, no box..
  } else {
    TAPtr tap = (TAPtr)base;
    BaseSpec* bs = (BaseSpec*)tap->GetOwner(&TA_BaseSpec); // find an owner..
    if(bs == NULL)
      return true;		// no owner, no box..
    if(bs->FindParent() != NULL)
      return false;		// owner is owned by a spec, needs a box..
  }
  return true;			// default is to not have box...
}


taiData* taiSpecMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent, int flags_) {
  no_check_box = NoCheckBox(host_);
  if (no_check_box) {
    taiData* rdat;
    if (m_sub_types != NULL)
      rdat = sub_types()->GetDataRep(host_, par, gui_parent);
    else
      rdat = taiMember::GetDataRep_impl(host_, par, gui_parent, flags_);
    return rdat;
  } else {
    taiPlusToggle* rval = new taiPlusToggle(NULL, host_, par, gui_parent);
    rval->InitLayout();
    taiData* rdat;
    if (m_sub_types)
      rdat = sub_types()->GetDataRep(host_, rval, rval->GetRep(), NULL, flags_);
    else
      rdat = taiMember::GetDataRep_impl(host_, rval, rval->GetRep(), flags_);
    rval->data = rdat;
    rval->AddChildWidget(rdat->GetRep());
    rval->EndLayout();
    return rval;
  }
}

void taiSpecMember::GetImage_impl(taiData* dat, const void* base) {
  IDataHost* host_ = dat->host;
  if (no_check_box) {
    if (m_sub_types != NULL)
      sub_types()->GetImage(dat,base);
    else
      taiMember::GetImage_impl(dat, base);
  } else {
    taiPlusToggle* rval = (taiPlusToggle*)dat;
    if (m_sub_types != NULL)
      sub_types()->GetImage(rval->data,base);
    else
      taiMember::GetImage_impl(rval->data, base);
    bool uniq = false;
    if(typ->InheritsFrom(TA_BaseSpec)) {
      BaseSpec* es = (BaseSpec*)base;
      uniq = es->GetUnique(mbr->idx);
    }
    else {
      BaseSubSpec* es = (BaseSubSpec*)base;
      uniq = es->GetUnique(mbr->idx);
    }
    rval->GetImage(uniq);
    if(uniq)
      rval->orig_val = "true";
    else
      rval->orig_val = "false";
  }
}

void taiSpecMember::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  IDataHost* host_ = dat->host;
  if (no_check_box) {
    if (m_sub_types != NULL)
      sub_types()->GetMbrValue(dat, base, first_diff);
    else
      taiMember::GetMbrValue(dat, base, first_diff);
    return;
  }

  taiPlusToggle* rval = (taiPlusToggle*)dat;
  if(typ->InheritsFrom(TA_BaseSpec)) {
    BaseSpec* es = (BaseSpec*)base;
    es->SetUnique(mbr->idx,rval->GetValue());
//    es->UpdateMbr(mbr->idx);
  }
  else {
    BaseSubSpec* es = (BaseSubSpec*)base;
    es->SetUnique(mbr->idx,rval->GetValue());
//    es->UpdateMbr(mbr->idx);
  }

  CmpOrigVal(dat, base, first_diff);
  if (m_sub_types != NULL)
    sub_types()->GetMbrValue(rval->data, base, first_diff);
  else
    taiMember::GetMbrValue(rval->data, base, first_diff);
}

void taiSpecMember::CmpOrigVal(taiData* dat, const void* base, bool& first_diff) {
  if((taMisc::record_script == NULL) || !typ->InheritsFrom(TA_taBase))
    return;
  String new_val;
  bool uniq = false;
  if(typ->InheritsFrom(TA_BaseSpec)) {
    BaseSpec* es = (BaseSpec*)base;
    uniq = es->GetUnique(mbr->idx);
  }
  else {
    BaseSubSpec* es = (BaseSubSpec*)base;
    uniq = es->GetUnique(mbr->idx);
  }
  if(uniq)
    new_val = "true";
  else
    new_val = "false";
  if(dat->orig_val == new_val)
    return;
  if(first_diff)
    StartScript(base);
  first_diff = false;

  *taMisc::record_script << "  ths->SetUnique(\"" << mbr->name << "\", "
			   << new_val << ");" << endl;
}



//////////////////////////
//   iPdpDataBrowser	//
//////////////////////////

iPdpDataBrowser::iPdpDataBrowser(taBase* root_, MemberDef* md_, TypeDef* typ_, 
  PdpDataBrowser* browser_,  QWidget* parent)
:iDataBrowser(root_, md_, typ_, browser_,  parent)
{  
}

iPdpDataBrowser::~iPdpDataBrowser() {
}

void iPdpDataBrowser::Constr_Menu_impl() {
  inherited::Constr_Menu_impl();
  // customize the menu text
//TODO: add custom hints to the file items
}

ProjectBase* iPdpDataBrowser::curProject() {
  DataBrowser* db = browser();
  if (!db) return NULL;
  if (!db->root) return NULL; //shouldn't happen
  if (!db->root->GetTypeDef()->InheritsFrom(&TA_ProjectBase)) return NULL; // we aren't a project
  
  return (ProjectBase*)db->root.ptr();
/* obs
  ProjectBase* rval = NULL;
  ISelectable* ci = curItem();
  if (ci) {
    taBase* ta = ci->taData(); // null if not tabase, but probably must be
    if (ta) {
      if (ta->InheritsFrom(&TA_ProjectBase))
        rval = (ProjectBase*)ta;
      else
        rval = (ProjectBase*)ta->GetOwner(&TA_ProjectBase);
    }
    // last resort is first project, if any
    if (!rval && pdpMisc::root)
      rval = pdpMisc::root->projects.SafeEl(0);
  }
  return rval; */
}

void iPdpDataBrowser::NewBrowser(ProjectBase* proj) {
  if (!proj) return;
  PdpDataBrowser* db = PdpDataBrowser::New(proj,NULL, false, true);
  db->InitLinks(); // no one else to do it!
  db->ViewWindow();
}


void iPdpDataBrowser::fileNew() {
  if (!pdpMisc::root) return;
  ProjectBase* proj = (ProjectBase*)pdpMisc::root->projects.New(); // let user choose type
  NewBrowser(proj);
}

void iPdpDataBrowser::fileOpen() {
  if (!pdpMisc::root) return;
  void* el = NULL;
  pdpMisc::root->projects.Load_File(&TA_ProjectBase, &el);
  ProjectBase* proj = (ProjectBase*)el;
  NewBrowser(proj);
}

void iPdpDataBrowser::fileSave() {
  ProjectBase* proj = curProject();
  if (!proj) return;
  proj->Save_File();
}

void iPdpDataBrowser::fileSaveAs() {
  ProjectBase* proj = curProject();
  if (!proj) return;
  proj->SaveAs_File();
}

void iPdpDataBrowser::fileSaveAll() {
  if (!pdpMisc::root) return;
  pdpMisc::root->SaveAll();
}

void iPdpDataBrowser::fileClose() {
  ProjectBase* proj = curProject();
  if (!proj) return;
//TODO: shouldn't we save, or confirm???
  proj->Close();
}


//////////////////////////
//   PdpDataBrowser	//
//////////////////////////

PdpDataBrowser* PdpDataBrowser::New(taBase* root, MemberDef* md, bool is_root
  ,bool del_on_close) 
{
  PdpDataBrowser* rval = new PdpDataBrowser();
  rval->Constr(root, md, is_root);
  rval->del_root_on_close = del_on_close;
  return rval;
}

void PdpDataBrowser::Constr_Window_impl() {
  if (!root) return;
  m_window = new iPdpDataBrowser(root, md, root->GetTypeDef(), this);
}


//////////////////////////
//   ipdpDataViewer	//
//////////////////////////

ipdpDataViewer::ipdpDataViewer(void* root_, TypeDef* typ_, pdpDataViewer* viewer_,
  QWidget* parent)
:iT3DataViewer(root_, typ_, (T3DataViewer*)viewer_, parent)
{
}

/*void iNetworkViewer::SetActionsEnabled_impl() {
  //todo
} */


//////////////////////////
//	pdpDataViewer	//
//////////////////////////

pdpDataViewer* pdpDataViewer::New(ProjectBase* proj_) {
  pdpDataViewer* rval = new pdpDataViewer();
  proj_->AddDataView(rval);
  return rval;
}

void pdpDataViewer::Initialize() {
  data_base = &TA_ProjectBase;
}

void pdpDataViewer::Destroy() {
}

void pdpDataViewer::Constr_Window_impl() {
  m_window = new ipdpDataViewer(proj(), proj()->GetTypeDef(), this);
//note:multi is default  viewer_win()->sel_mode = iT3DataViewer::SM_MULTI;
}

