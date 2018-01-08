// Copyright 2014-2017, Regents of the University of Colorado,
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

#include "iPanelOfStartupWizard.h"
#include <SigLinkSignal>
#include <iSplitter>
#include <taiWidgetProjTemplateElChooser>
#include <taiWidgetNameVarArrayChooser>
#include <taiWidgetStringArrayChooser>
#include <tabMisc>
#include <taRootBase>
#include <iDialogItemChooser>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <iAction>
#include <taFiler>
#include <MainWindowViewer>
#include <iMainWindowViewer>
#include <BuiltinTypeDefs>
#include <iTreeWidget>
#include <iHelpBrowser>

#include <taMisc>
#include <taiMisc>

const String iPanelOfStartupWizard::clear_menu_txt = " -> Clear Menu <- ";
const String iPanelOfStartupWizard::remove_files_txt = " -> Remove All Recover / AutoSave Files <- ";


iPanelOfStartupWizard::iPanelOfStartupWizard(taiSigLink* dl_)
:inherited(dl_) // usual case: we dynamically set the link, via setDoc
{
  sw_split = new iSplitter();
  setCentralWidget(sw_split);
  
  QLabel* lbl = NULL;
  QHBoxLayout* hb = NULL;

  ///////////////////////////////////////////
  //  new project template chooser

  ls_split = new iSplitter(Qt::Vertical);
  sw_split->addWidget(ls_split);

  // new project templates
  QFrame* npfrm = new QFrame(this);
  npfrm->setFrameStyle(QFrame::Panel); //  | QFrame::Sunken);

  QVBoxLayout* lay_np = new QVBoxLayout(npfrm);
  lay_np->setMargin(0); lay_np->setSpacing(2);

  hb = new QHBoxLayout;
  hb->setMargin(0);
  lbl = new QLabel("<b>Create New Project -- Choose Starting Template</b>");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("Select from one of the following templates as a starting point for creating a new project -- you can also save your own custom templates"));
  hb->addStretch();
  hb->addWidget(lbl);
  hb->addStretch();
  lay_np->addLayout(hb);

  new_proj_tmplt = NULL;
  new_proj_chs = new taiWidgetProjTemplateElChooser(&TA_ProjTemplateEl,
                                                    NULL, NULL, NULL, 0); // last is flags
  new_proj_chs->GetImage(&(tabMisc::root->projects.proj_templates), 
                         new_proj_tmplt);

  new_proj_chs->BuildCategories(); // for subtypes that use categories
  String chs_title = new_proj_chs->titleText();
  new_proj_chs_dlg = iDialogItemChooser::New(chs_title, new_proj_chs, 0, ls_split, iDialogItemChooser::flgNoCancel);

  lay_np->addWidget(new_proj_chs_dlg->body);
  new_proj_chs_dlg->Activate(new_proj_chs);
  connect(new_proj_chs_dlg, SIGNAL(accepted()), this, SLOT(NewProjSelected()) );
  ls_split->addWidget(npfrm);

  // new from web
  QFrame* nwfrm = new QFrame(this);
  nwfrm->setFrameStyle(QFrame::Panel); //  | QFrame::Sunken);

  QVBoxLayout* lay_nw = new QVBoxLayout(nwfrm);
  lay_nw->setMargin(0); lay_nw->setSpacing(2);

  hb = new QHBoxLayout;
  hb->setMargin(0);
  lbl = new QLabel("<b>Open Project From Web -- Choose Wiki To Browse</b>");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("Select from one of the following wiki locations to browse for downloading and opening a new project"));
  hb->addStretch();
  hb->addWidget(lbl);
  hb->addStretch();
  lay_nw->addLayout(hb);

  new_web_nv = NULL;
  new_web_chs = new taiWidgetNameVarArrayChooser(&TA_NameVar,
                                          NULL, NULL, NULL, 0); // last is flags
  new_web_chs->GetImage(&(taMisc::wikis), new_web_nv);

  new_web_chs->BuildCategories(); // for subtypes that use categories
  chs_title = new_web_chs->titleText();
  new_web_chs_dlg = iDialogItemChooser::New(chs_title, new_web_chs, 0, ls_split, iDialogItemChooser::flgNoCancel);

  lay_nw->addWidget(new_web_chs_dlg->body);
  new_web_chs_dlg->Activate(new_web_chs);
  connect(new_web_chs_dlg, SIGNAL(accepted()), this, SLOT(NewWebSelected()) );
  ls_split->addWidget(nwfrm);
  
  ///////////////////////////////////////////
  //  recent project items chooser

  QFrame* rpfrm = new QFrame(this);
  rpfrm->setFrameStyle(QFrame::Panel); //  | QFrame::Sunken);

  QVBoxLayout* lay_rp = new QVBoxLayout(rpfrm);
  lay_rp->setMargin(0); lay_rp->setSpacing(2);

  hb = new QHBoxLayout;
  hb->setMargin(0);
  lbl = new QLabel("<b>Select Recently Opened Project</b>");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("Select a project from among the following list of recently opened projects, presented in order of recency"));
  hb->addStretch();
  hb->addWidget(lbl);
  hb->addStretch();
  lay_rp->addLayout(hb);

  int nrf = tabMisc::root->recent_files.size;
  recent_files.SetSize(nrf + 2);
  int i;
  for(i=0; i<nrf; i++) {
    recent_files[i] = taMisc::CompressFilePath(tabMisc::root->recent_files[i]);
  }
  recent_files[i++] = clear_menu_txt;
  recent_files[i++] = remove_files_txt;

  rec_proj_nm = NULL;
  rec_proj_chs = new taiWidgetStringArrayChooser(&TA_taString,
                                                 NULL, NULL, NULL, 0); // last is flags
  rec_proj_chs->GetImage(&(recent_files), rec_proj_nm);

  rec_proj_chs->BuildCategories(); // for subtypes that use categories
  chs_title = rec_proj_chs->titleText();
  rec_proj_chs_dlg = iDialogItemChooser::New(chs_title, rec_proj_chs, 0, sw_split, iDialogItemChooser::flgNoCancel);

  lay_rp->addWidget(rec_proj_chs_dlg->body);
  rec_proj_chs_dlg->Activate(rec_proj_chs);
  // rec_proj_chs_dlg->items->setSortingEnabled(true);
  // rec_proj_chs_dlg->items->sortByColumn(0, Qt::AscendingOrder);
  connect(rec_proj_chs_dlg, SIGNAL(accepted()), this, SLOT(RecProjSelected()) );
  sw_split->addWidget(rpfrm);

  if(tabMisc::root->startupwiz_splits.nonempty()) {
    QByteArray ba =
      QByteArray::fromBase64(QByteArray(tabMisc::root->startupwiz_splits.chars()));
    sw_split->restoreState(ba);
  }
  if(tabMisc::root->startupwiz_ls_splits.nonempty()) {
    QByteArray ba =
      QByteArray::fromBase64(QByteArray(tabMisc::root->startupwiz_ls_splits.chars()));
    ls_split->restoreState(ba);
  }
}

iPanelOfStartupWizard::~iPanelOfStartupWizard() {
  delete new_proj_chs_dlg;
  delete new_proj_chs;
  delete new_web_chs_dlg;
  delete new_web_chs;
  delete rec_proj_chs_dlg;
  delete rec_proj_chs;
}

void iPanelOfStartupWizard::UpdateRecents() {
  int nrf = tabMisc::root->recent_files.size;
  recent_files.SetSize(nrf+2);
  int i;
  for(i=0; i<nrf; i++) {
    recent_files[i] = taMisc::CompressFilePath(tabMisc::root->recent_files[i]);
  }
  recent_files[i++] = clear_menu_txt;
  recent_files[i++] = remove_files_txt;
  // todo: i can't seem to force this thing to redraw if number of items doesn't change!
  // rec_proj_chs_dlg->items->clear();
  // taMisc::RunPending();
  rec_proj_chs_dlg->Activate(rec_proj_chs);
  // rec_proj_chs_dlg->items->repaint();
  new_proj_chs_dlg->Activate(new_proj_chs);
}

void iPanelOfStartupWizard::SaveSplitterSettings() {
  if(!tabMisc::root) return;
  tabMisc::root->startupwiz_splits = String(sw_split->saveState().toBase64().constData());
  tabMisc::root->startupwiz_ls_splits =
    String(ls_split->saveState().toBase64().constData());
}

void iPanelOfStartupWizard::NewProjSelected() {
  SaveSplitterSettings();
  new_proj_tmplt = (ProjTemplateEl*)new_proj_chs_dlg->selObj();
  if(new_proj_tmplt) {
    tabMisc::root->projects.NewFromTemplate(new_proj_tmplt);
  }
}

void iPanelOfStartupWizard::NewWebSelected() {
  SaveSplitterSettings();
  new_web_nv = (NameVar*)new_web_chs_dlg->selObj();
  if(new_web_nv) {
    String wiki_url = taMisc::GetWikiURL(new_web_nv->name);
    String url = wiki_url + taMisc::pub_proj_page;
    iHelpBrowser::StatLoadUrl(url);
  }
}

void iPanelOfStartupWizard::RecProjSelected() {
  SaveSplitterSettings();
  rec_proj_nm = (String*)rec_proj_chs_dlg->selObj();
  if(rec_proj_nm) {
    for (int i = 0; i < tabMisc::root->viewers.size; ++i) {
      MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(tabMisc::root->viewers.FastEl(i));
      if (!(vwr && vwr->isRoot())) continue;
      iMainWindowViewer* imwv = vwr->widget();
      if(!imwv) continue;
      if(*rec_proj_nm == clear_menu_txt) {
        imwv->fileClearRecentsMenu();
      }
      else if(*rec_proj_nm == remove_files_txt) {
        imwv->fileCleanRecentsMenu();
      }
      else {
        String rfn = taMisc::ExpandFilePath(*rec_proj_nm);
        imwv->fileOpenFile(rfn);
      }
      break;
    }
  }
}
void iPanelOfStartupWizard::OpenProject() {
  SaveSplitterSettings();
  for (int i = 0; i < tabMisc::root->viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(tabMisc::root->viewers.FastEl(i));
    if (!(vwr && vwr->isRoot())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
    imwv->fileOpen();
    break;
  }
}

bool iPanelOfStartupWizard::ignoreSigEmit() const {
  return false;
  //  return !isVisible(); -- this doesn't seem to be giving accurate results!!!
}

void iPanelOfStartupWizard::SigLinkDestroying(taSigLink* dl) {
  
}

void iPanelOfStartupWizard::UpdatePanel_impl() {
  SaveSplitterSettings();
  UpdateRecents();
  inherited::UpdatePanel_impl();
}

void iPanelOfStartupWizard::SigEmit_impl(int sls, void* op1_, void* op2_) {
  inherited::SigEmit_impl(sls, op1_, op2_);
  if (sls <= SLS_ITEM_UPDATED_ND) {
    this->m_update_req = true; // so we update next time we show
    UpdatePanel();
  }
}

