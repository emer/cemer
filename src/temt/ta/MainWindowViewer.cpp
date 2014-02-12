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

#include "MainWindowViewer.h"
#include <iMainWindowViewer>
#include <taProject>
#include <BrowseViewerTaBase>
#include <PanelViewer>
#include <TypeInfoBrowser>
#include <ToolBoxDockViewer>
#include <ToolBar>
#include <iPanelViewer>
#include <T3PanelViewer>
#include <iTabView>
#include <iT3PanelViewer>

#include <taMisc>
#include <taiMisc>
#include <tabMisc>
#include <taRootBase>

#include <QSplitter>
#include <QApplication>

TA_BASEFUNS_CTORS_DEFN(MainWindowViewer);

void MainWindowViewer:: FileOptionsAction(){} //
void MainWindowViewer:: FileCloseAction(){} // #ACT #MM_&File|&Close #MENUGP_LAST #MENU_GP_FileClose Quit Action(root) or Close Window Action(non-root)
void MainWindowViewer:: EditUndoAction(){} // #ACT
void MainWindowViewer:: EditRedoAction(){} // #ACT
void MainWindowViewer:: EditCutAction(){} // #ACT
void MainWindowViewer:: EditCopyAction(){} // #ACT
void MainWindowViewer:: EditPasteAction(){} // #ACT
void MainWindowViewer:: EditFindAction(){} // #ACT
void MainWindowViewer:: HelpIndexAction(){} // #ACT
void MainWindowViewer:: HelpContentsAction(){} // #ACT

// /TEMP

TypeDef* MainWindowViewer::def_browser_type = &TA_MainWindowViewer;
TypeDef* MainWindowViewer::def_viewer_type = &TA_MainWindowViewer;

MainWindowViewer* MainWindowViewer::GetDefaultProjectBrowser(taProject* proj) {
// look in list of viewer wins for most current
  for (int i = taiMisc::active_wins. size - 1; i >= 0; --i) {
    iMainWindowViewer* ivw = taiMisc::active_wins.SafeElAsMainWindow(i);
    // check if it is a proj viewer
    if (!ivw || !ivw->isProjBrowser()) continue;
    // if proj specified, check if same
    if (proj && (ivw->curProject() != proj)) continue;
    return ivw->viewer();
  }
  return NULL;
}

MainWindowViewer* MainWindowViewer::GetDefaultProjectViewer(taProject* proj) {
// look in list of viewer wins for most current
  for (int i = taiMisc::active_wins. size - 1; i >= 0; --i) {
    iMainWindowViewer* ivw = taiMisc::active_wins.SafeElAsMainWindow(i);
    // check if it is a proj viewer
    if (!ivw || !ivw->isProjViewer()) continue;
    // if proj specified, check if same
    if (proj && (ivw->curProject() != proj)) continue;
    return ivw->viewer();
  }
  return NULL;
}

MainWindowViewer* MainWindowViewer::NewBrowser(taBase* root,
  MemberDef* root_md, bool is_root)
{
  if (!def_browser_type || !(def_browser_type->InheritsFrom(&TA_MainWindowViewer)))
    def_browser_type = &TA_MainWindowViewer; // just in case

  MainWindowViewer* rval = (MainWindowViewer*)taBase::MakeToken(def_browser_type);
  rval->SetData(root);
  rval->m_is_root = is_root;
  BrowseViewerTaBase* cb = BrowseViewerTaBase::New(root, root_md);
  rval->frames.Add(cb);
  PanelViewer* pv = new PanelViewer;
  rval->frames.Add(pv);
  // browsers are added to the global viewers, and not persistent
  taProject* proj = GET_OWNER(root, taProject);
  if(proj) {
    proj->viewers_tmp.Add(rval);
  }
  else {
    if(tabMisc::root) {
      if(is_root)
        tabMisc::root->viewers.Add(rval); // this is our guy!
      else
        tabMisc::root->viewers_tmp.Add(rval); // does InitLinks
    }
  }
  return rval;
}

MainWindowViewer* MainWindowViewer::NewTypeInfoBrowser(void* root, TypeDef* root_typ,
  MemberDef* root_md)
{
  MainWindowViewer* rval = new MainWindowViewer;
  TypeInfoBrowser* cb = TypeInfoBrowser::New(root, root_typ, root_md);
  rval->frames.Add(cb);
  PanelViewer* pv = new PanelViewer;
  rval->frames.Add(pv);
  // browsers are added to the global viewers, and not persistent
  if (tabMisc::root)
    tabMisc::root->viewers_tmp.Add(rval); // does InitLinks
  return rval;
}

MainWindowViewer* MainWindowViewer::NewEditDialog(taBase* root) {
  if (!def_browser_type || !(def_browser_type->InheritsFrom(&TA_MainWindowViewer)))
    def_browser_type = &TA_MainWindowViewer; // just in case

  MainWindowViewer* rval = (MainWindowViewer*)taBase::MakeToken(def_browser_type);
  rval->SetData(root);
  rval->m_is_root = false;
  rval->m_is_dialog = true;
  PanelViewer* pv = new PanelViewer;
  rval->frames.Add(pv);
  // nuke or modify some stuff usually added:
  // browsers are added to the global viewers, and not persistent
  taProject* proj = GET_OWNER(root, taProject);
  if(proj) {
    proj->viewers_tmp.Add(rval);
  }
  else {
    if (tabMisc::root)
      tabMisc::root->viewers.Add(rval); // does InitLinks
  }
  return rval;
}

MainWindowViewer* MainWindowViewer::FindEditDialog(taBase* root) {
  taViewer_List* vwrs = NULL;
  taProject* proj = GET_OWNER(root, taProject);
  if(proj) {
    vwrs = &(proj->viewers_tmp);
  }
  else {
    if(tabMisc::root)
      vwrs = &(tabMisc::root->viewers_tmp);
  }
  if(!vwrs) return NULL;
  for(int i=0; i< vwrs->size; i++) {
    taDataView* dv = vwrs->FastEl(i);
    taBase* dt = dv->data();
    if((dt == root) && dv->InheritsFrom(&TA_MainWindowViewer))
      return (MainWindowViewer*)dv;
  }
  return NULL;
}

MainWindowViewer* MainWindowViewer::NewProjectBrowser(taProject* proj) {
  if (!proj) return NULL;
  if (!def_viewer_type || !(def_viewer_type->InheritsFrom(&TA_MainWindowViewer)))
    def_viewer_type = &TA_MainWindowViewer; // just in case

  FrameViewer* fv = NULL;
  MainWindowViewer* rval = (MainWindowViewer*)taBase::MakeToken(def_viewer_type);
  rval->SetData(proj);
  // tree guy
  BrowseViewerTaBase* cb = BrowseViewerTaBase::New(proj);
  rval->frames.Add(cb);
  // panel guy
  fv = rval->AddFrameByType(&TA_PanelViewer);
  MainWindowViewer* viewer = NULL; // only for 2x2 mode
  switch (taMisc::proj_view_pref) {
  case taMisc::PVP_2x2: {
    rval->setBrowserViewer(true, false);
    viewer = (MainWindowViewer*)taBase::MakeToken(def_viewer_type);
    fv = viewer->AddFrameByType(&TA_PanelViewer);
    fv = viewer->AddFrameByType(&TA_T3PanelViewer);
    viewer->SetData(proj);
    viewer->setBrowserViewer(false, true);
    // twiddle sizes a bit, to get overlap
    rval->SetUserData("view_win_wd", 0.6667f);
    viewer->SetUserData("view_win_lft", 0.3333f);
    viewer->SetUserData("view_win_wd", 0.6667f);
  } break;
  case taMisc::PVP_3PANE: {
    rval->setBrowserViewer(true, true);
    fv = rval->AddFrameByType(&TA_T3PanelViewer);
  } break;
  // no default, must handle all cases
  }
  // always added to the viewer collection
  // 2x2 only -- better to add this first, because it ends up underneath
  if (viewer) {
    proj->viewers.Add(viewer); // will get auto-opened later
  }
  proj->viewers.Add(rval);
  // global toolbar
  ToolBoxDockViewer* tb = ToolBoxDockViewer::New(); // initializes
  rval->AddDock(tb);
  return rval;
}

MainWindowViewer* MainWindowViewer::NewProjectViewer(taProject* proj) {
  if (!proj) return NULL;
  if (!def_viewer_type || !(def_viewer_type->InheritsFrom(&TA_MainWindowViewer)))
    def_viewer_type = &TA_MainWindowViewer; // just in case

  MainWindowViewer* viewer  = (MainWindowViewer*)taBase::MakeToken(def_viewer_type);
  FrameViewer* fv = viewer->AddFrameByType(&TA_PanelViewer);
  fv = viewer->AddFrameByType(&TA_T3PanelViewer);
  viewer->SetData(proj);
  viewer->setBrowserViewer(false, true);
  // twiddle sizes a bit, to get overlap
  viewer->SetUserData("view_win_lft", 0.3333f);
  viewer->SetUserData("view_win_wd", 0.6667f);
  proj->viewers.Add(viewer); // will get auto-opened later
  return viewer;
}

void MainWindowViewer::Initialize() {
  m_is_root = false;
  m_is_viewer_xor_browser = false; // note: weird, for compatability w <=4.0.6
  m_is_proj_viewer = false;
  m_is_dialog = false;
  menu = NULL;
  cur_menu = NULL;
  ta_menus = new taiWidgetActions_List;
}

void MainWindowViewer::Destroy() {
  CutLinks();
  if (ta_menus) {
    delete ta_menus;
    ta_menus = NULL;
  }
}

void MainWindowViewer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(toolbars, this);
  taBase::Own(frames, this);
  taBase::Own(docks, this);

  // add default toolbars to new (non-dialog) instance or if missing from loaded
  if (!isDialog()) {
    AddToolBarByType(&TA_ToolBar, "Application");
  }
}

void MainWindowViewer::Copy_(const MainWindowViewer& cp) {
  toolbars = cp.toolbars;
  frames = cp.frames;
  docks = cp.docks;
}

void MainWindowViewer::CutLinks() {
  docks.CutLinks();
  frames.CutLinks();
  toolbars.CutLinks();
  inherited::CutLinks();
}

void MainWindowViewer::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  SetWinName();
}

bool MainWindowViewer::GetWinState() {
  if (!inherited::GetWinState()) return false;
  toolbars.GetWinState();
  frames.GetWinState();
  docks.GetWinState();
  // relative sizes of panels
  QSplitter* spl = widget()->body;
  SetUserData("view_splitter_state",
    String(spl->saveState().toBase64().constData()));
  return true;
}

bool MainWindowViewer::SetWinState() {
  if (!inherited::SetWinState()) return false;
  toolbars.SetWinState();
  frames.SetWinState();
  docks.SetWinState();
  String str = GetUserDataAsString("view_splitter_state");
  if (str.nonempty()) {
    QByteArray ba = QByteArray::fromBase64(QByteArray(str.chars()));
    QSplitter* spl = widget()->body;
    spl->restoreState(ba);
  }
  return true;
}

void MainWindowViewer::AddDock(DockViewer* dv) {
  docks.Add(dv);
}

void MainWindowViewer::AddFrame(FrameViewer* fv, int at_index)
{
  if (!fv) return;
  //TODO: note: if we are mapped, then the chg detector for .frames will map the guy
  if (at_index < 0)
    frames.Add(fv);
  else
    frames.Insert(fv, at_index);
}

FrameViewer* MainWindowViewer::AddFrameByType(TypeDef* typ, int at_index)
{
  if (!typ || !typ->InheritsFrom(&TA_FrameViewer)) return NULL;
  FrameViewer* rval = (FrameViewer*)taBase::MakeToken(typ);
  AddFrame(rval, at_index);
  return rval;
}

bool MainWindowViewer::AddToolBar(ToolBar* tb) {
  if (!tb) return false;
  if (!toolbars.AddUnique(tb)) return false; // already added!
  if (isMapped()) OnToolBarAdded(tb, true);
  return true;
}

ToolBar* MainWindowViewer::AddToolBarByType(TypeDef* typ,
  const String& tb_name)
 {
   if (toolbars.FindName(tb_name)) return NULL;
   ToolBar* tb = (ToolBar*)taBase::MakeToken(typ);
   if (tb) {
    tb->SetName(tb_name);
    AddToolBar(tb);
   }
   return tb;
 }

void MainWindowViewer::CloseWindow_impl() {
  ta_menus->Reset();
  cur_menu = NULL;
  menu = NULL; // window deletes
  inherited::CloseWindow_impl();
}

void MainWindowViewer::Constr_impl(QWidget* gui_parent) {
  inherited::Constr_impl(gui_parent); // prob just creates the widget
  if (!dvwidget()) return; // shouldn't happen

  ConstrToolBars_impl();
  ConstrFrames_impl();
  ConstrDocks_impl();

//  iMainWindowViewer* win = window(); //cache
  //note: it pulls isRoot and isProjViewer from us
//TODO, replace:  win->SelectionChanged(true); // initializes selection system
}

void MainWindowViewer::ConstrDocks_impl() {
  iMainWindowViewer* win = viewerWindow(); //cache
  for (int i = 0; i < docks.size; ++i) {
    DockViewer* dv = docks.FastEl(i);
    if (!dv) continue; // shouldn't happen
    // note: don't parent the frame, since we use the api to add it
    ((taViewer*)dv)->Constr_impl(NULL);
    win->AddDockViewer(dv->widget(), (Qt::DockWidgetArea)dv->dock_area);
  }
}

void MainWindowViewer::ConstrFrames_impl() {
  iMainWindowViewer* win = viewerWindow(); //cache
  for (int i = 0; i < frames.size; ++i) {
    FrameViewer* fv = frames.FastEl(i);
    if (!fv) continue; // shouldn't happen
    // note: don't parent the frame, since we use the api to add it
    ((taViewer*)fv)->Constr_impl(NULL);
    win->AddFrameViewer(fv->widget());
    // no effect:
//     if(frames.size >= 2 && i == frames.size -1 && fv->InheritsFrom(&TA_T3PanelViewer)) {
//       // do not include the T3 guy in the tab order -- it is rather deadly
//       fv->widget()->setFocusPolicy(Qt::NoFocus); // not tab focus! -- note: not having any effect
//     }
  }
  // do not include the T3 guy in the tab order -- it is rather deadly
  // this doesn't seem to have any effect at all!
//   if(frames.size >= 2 && frames.SafeEl(-1)->InheritsFrom(&TA_T3PanelViewer)) {
//     for (int i = 0; i < frames.size-2; ++i) {
//       FrameViewer* fv = frames.FastEl(i);
//       FrameViewer* fvn = frames.FastEl(i+1);
//       if (!fv || !fvn) continue; // shouldn't happen
//       QWidget::setTabOrder(fv->widget(), fvn->widget());
//     }
//     QWidget::setTabOrder(frames.SafeEl(-2)->widget(), frames.FastEl(0)->widget());
//     // wrap back around
//   }
}

void MainWindowViewer::ConstrToolBars_impl() {
  for (int i = 0; i < toolbars.size; ++i) {
    ToolBar* tb = toolbars.FastEl(i);
    OnToolBarAdded(tb, false);
  }
}

IViewerWidget* MainWindowViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iMainWindowViewer(this, gui_parent);
}

void MainWindowViewer::SigEmit_Child(taBase* child, int sls, void* op1, void* op2) {
  if (child == &frames) {
    // if reorder, then do a gui reorder
    //TODO:
  }
}

void MainWindowViewer::DoActionChildren_impl(DataViewAction act) {
// note: only ever called with one action
  if (act & CONSTR_MASK) {
    inherited::DoActionChildren_impl(act);
    toolbars.DoAction(act);
    frames.DoAction(act);
    docks.DoAction(act);
  } else { // DESTR_MASK
    docks.DoAction(act);
    frames.DoAction(act);
    toolbars.DoAction(act);
    inherited::DoActionChildren_impl(act);
  }
}

FrameViewer* MainWindowViewer::FindFrameByType(TypeDef* typ, int& at_index, int from_index) {
  FrameViewer* rval = NULL;
  for (int i = from_index; i < frames.size; ++i) {
    rval = frames.FastEl(i);
    if (rval->InheritsFrom(typ)) {
      at_index = i;
      return rval;
    }
  }
  at_index = -1;
  return NULL;
}

ToolBar* MainWindowViewer::FindToolBarByType(TypeDef* typ,
  const String& tb_name)
{
  for (int i = 0; i < toolbars.size; ++i) {
    ToolBar* tb = toolbars.FastEl(i);
    if ((tb->name == tb_name) &&
      tb->GetTypeDef()->InheritsFrom(typ))
      return tb;
  }
  return NULL;
}

void MainWindowViewer::FrameSizeToSize(iSize& sz) {
  //TODO: maybe estimate if the taiM guy not set yet
  sz.h -= taiM->frame_s.h;
  sz.w -= taiM->frame_s.w;
}

void MainWindowViewer::Hide_impl() {
  widget()->lower();
}

bool MainWindowViewer::isProjBrowser() const {
  return m_is_proj_viewer ? !m_is_viewer_xor_browser : m_is_viewer_xor_browser;
}

bool MainWindowViewer::isProjShower() const {
  return !(m_is_proj_viewer || m_is_viewer_xor_browser);
}

void MainWindowViewer::setBrowserViewer(bool is_browser, bool is_viewer) {
  m_is_proj_viewer = is_viewer;
  m_is_viewer_xor_browser = is_viewer ? !is_browser : is_browser;
}

void MainWindowViewer::MakeWinName_impl() {
  String prog_nm = taiM->classname(); // note: this is not working
  String name;
  String fname;
  String pathnm;
  if (data()) {
    name = data()->GetName();
    fname = taMisc::GetFileFmPath(data()->GetFileName());
    pathnm = data()->GetPathNames();
  }
  else {
    pathnm = GetPathNames();
  }
  // The [*] indicates where Qt should put the asterisk indicating the
  // file has not been saved.  On the Mac, Qt puts a dot in the close
  // button instead.
  if (fname.empty()) fname = name.empty() ? "(Untitled)" : name;
  String nw_name = pathnm + " - " + fname + " [*]"; // prog_nm ;
  win_name = nw_name;
  if(m_is_root) {
    win_name = taMisc::app_name + " v" + taMisc::version + " (svn: " + taMisc::svn_rev + ")";
  }
}

void MainWindowViewer::OnToolBarAdded(ToolBar* tb, bool post_constr) {
//TODO: just nuke the post_constr variable --
  iMainWindowViewer* win = viewerWindow(); //cache
  //note: always costructed, even if not visible
  ((taViewer*)tb)->Constr_impl(NULL);

  // add to the toolbar view menu
  win->AddToolBar(tb->widget());
  if (post_constr) {
    ((taViewer*)tb)->Constr_post(); // gotta do this manually post-Constr
  }
}

void MainWindowViewer::ResolveChanges(CancelOp& cancel_op) {
  // need to do depth first; note: toolbars can't have changes
  for (int i = 0; i < frames.size; ++i) {
    FrameViewer* fv = frames.FastEl(i);
    fv->ResolveChanges(cancel_op);
    if (cancel_op == CO_CANCEL) return;
  }
  inherited::ResolveChanges(cancel_op);
}

void MainWindowViewer::Show_impl() {
  SetWinName();
  QWidget* wid = widget();
  wid->raise();
  qApp->setActiveWindow(wid);
}

void MainWindowViewer::WidgetDeleting_impl() {
  menu = NULL;
  inherited::WidgetDeleting_impl();
}

bool MainWindowViewer::SelectPanelTabNo(int tab_no) {
  int idx;
  PanelViewer* pv = (PanelViewer*)FindFrameByType(&TA_PanelViewer, idx);
  if (pv && pv->widget()) {
    iPanelViewer* itv = pv->widget();
    return itv->tabView()->SetCurrentTab(tab_no);
  }
  return false;
}

bool MainWindowViewer::SelectPanelTabName(const String& tab_nm) {
  int idx;
  PanelViewer* pv = (PanelViewer*)FindFrameByType(&TA_PanelViewer, idx);
  if (pv && pv->widget()) {
    iPanelViewer* itv = pv->widget();
    return itv->tabView()->SetCurrentTabName(tab_nm);
  }
  return false;
}

bool MainWindowViewer::SelectT3ViewTabNo(int tab_no) {
  int idx;
  T3PanelViewer* pv = (T3PanelViewer*)FindFrameByType(&TA_T3PanelViewer, idx);
  if (pv && pv->widget()) {
    iT3PanelViewer* itv = pv->widget();
    return itv->SetCurrentTab(tab_no);
  }
  return false;
}

bool MainWindowViewer::SelectT3ViewTabName(const String& tab_nm) {
  int idx;
  T3PanelViewer* pv = (T3PanelViewer*)FindFrameByType(&TA_T3PanelViewer, idx);
  if (pv && pv->widget()) {
    iT3PanelViewer* itv = pv->widget();
    return itv->SetCurrentTabName(tab_nm);
  }
  return false;
}

BrowseViewer* MainWindowViewer::GetLeftBrowser() {
  int idx;
  BrowseViewer* bv = (BrowseViewer*)FindFrameByType(&TA_BrowseViewer, idx);
  return bv;
}

PanelViewer* MainWindowViewer::GetMiddlePanel() {
  int idx;
  PanelViewer* pv = (PanelViewer*)FindFrameByType(&TA_PanelViewer, idx);
  return pv;
}

T3PanelViewer* MainWindowViewer::GetRightViewer() {
  int idx;
  T3PanelViewer* pv = (T3PanelViewer*)FindFrameByType(&TA_T3PanelViewer, idx);
  return pv;
}

DockViewer* MainWindowViewer::FindDockViewerByName(const String& dv_name) {
  DockViewer* dv = NULL;
  for (int i = 0; i < docks.size; ++i) {
    DockViewer* dv = docks.FastEl(i);
    taMisc::DebugInfo(dv->name);
    if (dv->name == dv_name)
      return dv;
  }
  return dv;
}
