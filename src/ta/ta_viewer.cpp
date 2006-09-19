// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "ta_viewer.h"

#include "ta_project.h"

#ifdef TA_GUI
# include "ta_qt.h"
# include "ta_qtviewer.h"
# include "ta_qtbrowse.h"
# include "ta_classbrowse.h"
#endif

//TODO: this file will need to have many routines modalized for TA_GUI
// typically, just stub entire thing out -- maybe we can just stub out
// all these funs in the header, and then collect them all here in a #ifdef


//////////////////////////
//   DataViewer		//
//////////////////////////

void DataViewer::GetFileProps(TypeDef* td, String& fltr, bool& cmprs) {
  int opt;
  if ((opt = td->opts.FindContains("EXT_")) >= 0) {
    fltr = td->opts.FastEl(opt).after("EXT_");
    fltr = "*." + fltr + "*";
  } else
    fltr = "*";
  cmprs = false;
  if (td->HasOption("COMPRESS"))
    cmprs = true;
}

void DataViewer::Initialize() {
  m_dvwidget = NULL;
  display_toggle = true;
}

void DataViewer::Destroy() {
  CutLinks();
  if (m_dvwidget) {
    m_dvwidget->Close(); // typically is destructive close, and calls us back + resets instance
    m_dvwidget = NULL; // 
  }
}

void DataViewer::InitLinks() {
  inherited::InitLinks();
}

void DataViewer::CutLinks() {
  CloseWindow();
  inherited::CutLinks();
}

void DataViewer::Copy_(const DataViewer& cp) {
  display_toggle = cp.display_toggle;
//TODO: should probably call Clear() too
}

void DataViewer::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
}

void DataViewer::CloseWindow() {
  if (isMapped()) {
    CloseWindow_impl(); 
    m_dvwidget = NULL;
  }
}	

void DataViewer::CloseWindow_impl() { // only called if mapped
  m_dvwidget->Close(); // typically is destructive close, and calls us back + resets instance
}

void DataViewer::Constr(QWidget* gui_parent) {
  if (!taMisc::gui_active || dvwidget()) return;
  Constr_impl(gui_parent);
  Constr_post();
}

void DataViewer::Constr_impl(QWidget* gui_parent) {
  m_dvwidget = ConstrWidget_impl(gui_parent);
  m_dvwidget->Constr(); // virtual guy
}

/* TBD
void DataViewer::GetPrintFileDlg(PrintFmt fmt) {
  static PrintFmt last_fmt = POSTSCRIPT;
  if(!taMisc::gui_active) return;
  if((print_file != NULL) && (last_fmt == fmt))  return;
  if(print_file != NULL) {
    taRefN::unRefDone(print_file);
    print_file = NULL;
  }

  String ext = GetPrintFileExt(fmt);

  print_file = taFiler::New("Print", ext);
  taRefN::Ref(print_file);
  last_fmt = fmt;
}

String DataViewer::GetPrintFileExt(PrintFmt fmt) {
  String ext;
  if(fmt == POSTSCRIPT)
    ext = ".ps";
  else if(fmt == JPEG)
    ext = ".jpg";
  else if(fmt == TIFF)
    ext = ".tiff";
  else if(fmt == PDF)
    ext = ".pdf";
  return ext;
} */

bool DataViewer::isMapped() const {
  return (taMisc::gui_active && m_dvwidget);
}

void DataViewer::Lower() {
  if (!isMapped()) return;
  // note: only makes sense in some contexts
  widget()->lower();
}

void DataViewer::Raise() {
  if (!isMapped()) return;
  // note: only makes sense in some contexts
  widget()->raise();
}

/*obs void DataViewer::ReSize(float width, float height) {
  if (!taMisc::gui_active || (dvwidget() == NULL)) return;
  if ((width > 0.0f) && (height > 0.0f)) {
    win_state.wd = width;
    win_state.ht = height;
    win_state.SetWinState();
  } else {
    GetWinState();
  }
} */

void DataViewer::WidgetDeleting() {
  m_dvwidget = NULL;
}

QWidget* DataViewer::widget() {
  return (m_dvwidget) ? m_dvwidget->widget() : NULL; 
}


//////////////////////////////////
//   DataViewer_List	 	//
//////////////////////////////////

void DataViewer_List::Constr_impl(QWidget* gui_parent) {
  for (int i = 0; i < size; ++i) {
    DataViewer* dv = FastEl(i);
    dv->Constr_impl(gui_parent);
  }
}

void DataViewer_List::Constr_post() {
  for (int i = 0; i < size; ++i) {
    DataViewer* dv = FastEl(i);
    dv->Constr_post();
  }
}

//////////////////////////////////
//   FrameViewer	 	//
//////////////////////////////////

void FrameViewer::Initialize() {
  m_main_window_viewer = NULL;
}

MainWindowViewer* FrameViewer::mainWindowViewer() {
  if (!m_main_window_viewer)
    m_main_window_viewer = GET_MY_OWNER(MainWindowViewer); 
  return m_main_window_viewer;
}

iMainWindowViewer* FrameViewer::window() {
  if (mainWindowViewer()) return m_main_window_viewer->window(); 
  else return NULL;
}


//////////////////////////////////
// 	BrowseViewer	 	//
//////////////////////////////////

void BrowseViewer::Initialize() {
  root_typ = NULL;
  root_md = NULL;
}

void BrowseViewer::Copy_(const BrowseViewer& cp) {
  root_typ = cp.root_typ;
  root_md = cp.root_md;
}

void BrowseViewer::Clear_impl() {
  if (!isMapped()) return;
  widget()->Reset();
}

void BrowseViewer::Render_impl() {
  if (!isMapped()) return;
  widget()->ApplyRoot();
}

/* nn void BrowseViewer::TreeNodeDestroying(taiTreeDataNode* item) {
  if (!m_window) return;
  browser_win()->TreeNodeDestroying(item);
} */



//////////////////////////////////
//  tabBrowseViewer	 	//
//////////////////////////////////

tabBrowseViewer* tabBrowseViewer::New(TAPtr root, MemberDef* root_md) {
  if (!root) return NULL; // nice try, buster
  tabBrowseViewer* rval = new tabBrowseViewer;
  rval->m_root = root;
  rval->root_typ = root->GetTypeDef();
  rval->root_md = root_md;
  return rval;
}

void tabBrowseViewer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(m_root, this);
}

void tabBrowseViewer::CutLinks() {
  inherited::CutLinks();
}

void tabBrowseViewer::Copy_(const tabBrowseViewer& cp) {
  m_root = cp.m_root;
}

IDataViewWidget* tabBrowseViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iBrowseViewer(this, gui_parent);
}

void tabBrowseViewer::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  // if root has vanished, window must die
  //TODO: should prob propagate this up to the enclosing iMainWindowViewer???
  if (!m_root && dvwidget()) {
    CloseWindow();
  }
}

//////////////////////////////////
//   ClassBrowseViewer	 	//
//////////////////////////////////

ClassBrowseViewer* ClassBrowseViewer::New(void* root, TypeDef* root_typ, MemberDef* root_md) {
  ClassBrowseViewer* rval = new ClassBrowseViewer();
  rval->setRoot(root, root_typ, root_md);
  return rval;
}

void ClassBrowseViewer::Initialize() {
  m_root = NULL;
}

void ClassBrowseViewer::Copy_(const ClassBrowseViewer& cp) {
  m_root = cp.m_root;
  RootToStr();
}

IDataViewWidget* ClassBrowseViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iClassBrowseViewer(this, gui_parent);
}

void ClassBrowseViewer::UpdateAfterEdit() {
  if (taMisc::is_loading) {
    // reget the actual guy, based on the saved string descr
    StrToRoot();
  }
  inherited::UpdateAfterEdit();
}

void ClassBrowseViewer::RootToStr() {
  if (!m_root || !root_typ) {
    root_str = _nilString;
    return;
  }
  taMisc::TypeInfoKind tik = taMisc::TypeToTypeInfoKind(root_typ);
  //TODO: full description of type, requires support from ta_type for searching/converting
  // for now, we only support the root typespace
  if (m_root == &taMisc::types)
    root_str = ".types";
  else 
    root_str = _nilString; // TEMP
}

void ClassBrowseViewer::setRoot(void* root_, TypeDef* root_typ_, MemberDef* root_md_) {
  m_root = root_;
  root_typ = root_typ_;
  root_md = root_md_;
  RootToStr();
}

void ClassBrowseViewer::StrToRoot() {
  //TODO: full description of type, requires support from ta_type for searching/converting
  // for now, we only support the root typespace
  taMisc::TypeInfoKind tik = taMisc::TypeToTypeInfoKind(root_typ);
  if ((tik == taMisc::TIK_TYPESPACE) && (root_str == ".types")) {
    m_root = &taMisc::types;
    root_md = TA_taMisc.members.FindName("types");
  } else {
    m_root = NULL;
  }
}


//////////////////////////////////
// 	TabViewer	 	//
//////////////////////////////////

void TabViewer::Initialize() {
}

IDataViewWidget* TabViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iTabViewer(this, gui_parent);
}

/*void TabViewer::Clear_impl() {
  if (!m_window) return;
  browser_win()->Reset();
}*/


//////////////////////////
//	WindowState	//
//////////////////////////

float WindowState::Offs(float cur, float by) {
  float rval = cur + by;
  if (rval > 1.0f) rval -= 1.0f;
  if (rval < 0.0f) rval = 0.0f;
  return rval;
}

void WindowState::Initialize() {
  if (taiM) { // guard for instance creation before sys init
    lft = (taiM->scrn_s.w > 0) ? (float)(taiM->scrn_geom.left()) / (float)(taiM->scrn_s.w) : 0.0f;
    top =  (taiM->scrn_s.h > 0) ? (float)(taiM->scrn_geom.top()) / (float)(taiM->scrn_s.h) : 0.0f;
  } else {
    lft = 0.0f;
    top = 0.0f;
  }
  wd = 0.75f;
  ht = 0.8f; // default window size
  iconified = false;
}

void WindowState::Copy_(const WindowState& cp) {
  lft = cp.lft;
  top = cp.top;
  wd = cp.wd;
  ht = cp.ht;
  iconified = cp.iconified;
}

void WindowState::UpdateAfterEdit() {
  // make sure to limit on mac, due to wacky menubar and dock
  //TODO: maybe this should be used for all? ex. windows or kde taskbar etc.
#ifdef QT_OS_MACX
  if (taiM) { // guard for instance creation before sys init
    lft = MAX(lft, (taiM->scrn_s.w > 0) ? 
      (float)(taiM->scrn_geom.left()) / (float)(taiM->scrn_s.w) : 0.0f);
    top = MAX(top, (taiM->scrn_s.h > 0) ? 
      (float)(taiM->scrn_geom.top()) / (float)(taiM->scrn_s.h) : 0.0f);
  }
//TODO: prob should limit wd and ht too, because of dock, and inability to size if grip is offscreen
#endif
  inherited::UpdateAfterEdit();
  if (!taMisc::is_loading)
    SetWinState();
}

void WindowState::GetWinState() {
  TopLevelViewer* tlv = topLevelViewer();
  if (!tlv || !tlv->isMapped()) return;
  QWidget* widget = tlv->widget(); // cache

  iRect r = widget->frameGeometry();
  // convert from screen coords to relative (note, allowed to be >1.0)
  lft = (float)r.left() / (float)(taiM->scrn_s.w); // all of these convert from screen coords
  top = (float)r.top() / (float)(taiM->scrn_s.h);
  wd = (float)r.width() / (float)(taiM->scrn_s.w);
  ht = (float)r.height() / (float)(taiM->scrn_s.h);
  iconified = widget->isMinimized();
}

void WindowState::SetWinState() {
  TopLevelViewer* tlv = topLevelViewer();
  if (!tlv || !tlv->isMapped()) return;
  QWidget* widget = tlv->widget(); // cache

  // convert to pixels
  iRect tr = iRect(
    (int)(lft * taiM->scrn_s.w),
    (int)(top * taiM->scrn_s.h),
    (int)(wd * taiM->scrn_s.w),
    (int)(ht * taiM->scrn_s.h)
  );

  //note: resize/move combo recommended by Qt
  widget->resize(tr.w, tr.h);
  widget->move(tr.x, tr.y);
  if (iconified)
    widget->showMinimized();
  else
    widget->showNormal();

}

TopLevelViewer* WindowState::topLevelViewer() {
  TopLevelViewer* rval = GET_MY_OWNER(TopLevelViewer); // note: ok for when UserData as well 
  return rval;
}

void WindowState::ScriptWinState(ostream& strm) {
  TopLevelViewer* tlv = topLevelViewer();
  if (!tlv || !tlv->isMapped()) return;

  GetWinState();
  String temp = tlv->GetPath();
  if (iconified) {
    temp += ".Iconify();\n";
  }
  else {
    temp += String(".SetWinState(") + String(lft) + ", " +
      String(top) + ", " + String(wd) + ", " + String(ht) + ");\n";
  }
  if (taMisc::record_script != NULL)  taMisc::RecordScript(temp);
  else   strm << temp;
}


//////////////////////////
//   TopLevelViewer	//
//////////////////////////

void TopLevelViewer::Initialize() {
}

void TopLevelViewer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(win_state, this); //TEMP: until is UserData
  if (taMisc::is_loading && isTopLevel() && openOnLoad())
      taiMisc::unopened_windows.LinkUnique(this);
}

void TopLevelViewer::CutLinks() {
  win_state.CutLinks(); // TEMP: until is UserData
  inherited::CutLinks();
}


void TopLevelViewer::Copy_(const TopLevelViewer& cp) {
  win_state = cp.win_state; //TEMP until is UserData, then nuke
  if (isTopLevel()) {
    WindowState& win_state = winState(); // cache
    // offset the copy
    win_state.lft = WindowState::Offs(win_state.lft, 0.05f);
    win_state.top = WindowState::Offs(win_state.top, 0.05f);
  }
  if (!GetName().empty())
    SetName(GetName() + "_copy");
}

void TopLevelViewer::DeIconify() {
  if (!isMapped() || !isTopLevel()) return;
  if (widget()->isMinimized())
    widget()->showNormal();
  winState().iconified = false; //TEMP
}

void TopLevelViewer::Iconify() {
  if (!isMapped() || !isTopLevel()) return;
  if (!widget()->isMinimized())
    widget()->showMinimized();
  winState().iconified = true; // TEMP
}

void TopLevelViewer::Constr_impl(QWidget* gui_parent) {
  inherited::Constr_impl(gui_parent);
  if (!dvwidget()) return; // shouldn't happen
}

void TopLevelViewer::Constr_post() {
  inherited::Constr_post();
  winState().SetWinState();
  SetWinName();
}

void TopLevelViewer::Dump_Save_pre() {
  inherited::Dump_Save_pre();
//TODO: we can eliminate this with UserData system
  winState().GetWinState(); // get window position *before* saving!
}

void TopLevelViewer::SetWinName() {
  if (!isMapped()) return;
  MakeWinName_impl();
  widget()->setCaption(win_name);
}

void TopLevelViewer::ViewWindow() {
  if (!taMisc::gui_active) return;
  if (dvwidget()) {
    DeIconify();
    Raise();
  } else {
// TENT
    // if not owned yet, put us in the global guy
    if (!GetOwner() && tabMisc::root)
      tabMisc::root->viewers.Add(this); // does InitLinks
// /TENT
    Constr(); // no parent;
    Render();
    
//    if(((left != -1.0f) && (top != -1.0f)) || ((width != -1.0f) && (height != -1.0f)))
//      SetWinState(left, top, width, height);
  }
}

void TopLevelViewer::WindowClosing(CancelOp& cancel_op) {
  if (taMisc::quitting)
    cancel_op = CO_NOT_CANCELLABLE; // set it, for all others
  bool forced = (cancel_op == CO_NOT_CANCELLABLE);

  if (hasChanges()) {
    int chs;
    if (forced)
      chs= taMisc::Choice("You have unsaved changes -- do you want to save before closing?",
        "&Save", "&Discard Changes");
    else 
      chs= taMisc::Choice("You have unsaved changes -- do you want to save before closing?",
        "&Save", "&Discard Changes", "&Cancel");

    switch (chs) {
    case 0:
      SaveData(); 
      break;
    case 1:
      break;
    case 2: //only possible if not forced
      cancel_op = CO_CANCEL;
      return;
    }
  }

  //following done only for a root win
  if (!forced && isRoot()) {
    int chs = taMisc::Choice("Closing this window will end the application.", "&Quit", "&Save All and Quit", "&Cancel");
    switch (chs) {
    case 1:
      if (tabMisc::root) tabMisc::root->SaveAll();
      //fall through
    case 0:
      taiM->MainWindowClosing(cancel_op);
      if (cancel_op != CO_CANCEL) {
       taMisc::quitting = true;
       return;
      }
      forced = taMisc::quitting;
      break;
    case 2:
      cancel_op = CO_CANCEL;
      return;
    }
  }

  //note: should have exited if cancel, but we check just in case
//no, this deletes the viewer, which is not what we want, just because gui win is closing
//  if (!cancel)  tabMisc::Close_Obj(this);

/*was TODO  cancel = true; // falling through to end of routine is "proceed"
  if (InheritsFrom(TA_WinView) && (((WinView*)this)->mgr != NULL)) {
    WinMgr* mgr = ((WinView*)this)->mgr;
    int chs = taMisc::Choice("Close (PERMANENTLY Destroy) this VIEW of the object, or destroy the ENTIRE object including all views, losing all unsaved changes?", "Close View", "Close Obj", "Save Then Close Obj", "Cancel");
    if (chs == 3)
      return; //cancel
    else if(chs == 2) {
      taFiler* taf = mgr->GetFileDlg();
      taRefN::Ref(taf);
      ostream* strm = taf->Save();
      if((strm != NULL) && strm->good()) {
	taiMisc::RecordScript(mgr->GetPath() + ".Save(" + taf->fname + ");\n");
	mgr->SetFileName(taf->fname);
	DMEM_GUI_RUN_IF {
	  mgr->Save(*strm);
	}
      }
      taRefN::unRef(taf); //no Done, in case supplier isn't using ref counting
    } else if(chs == 0) {
      taiMisc::RecordScript(GetPath() + ".Close();\n");
      DMEM_GUI_RUN_IF {
	tabMisc::Close_Obj(this);
      }
      cancel = false;
      return;
    }
    taiMisc::RecordScript(mgr->GetPath() + ".Close();\n");
    DMEM_GUI_RUN_IF {
      tabMisc::Close_Obj(mgr);
    }
  } else {
    int chs = taMisc::Choice("Ok to Close (PERMANENTLY Destroy) this object, losing all unsaved changes?", "Close", "Save Then Close", "Cancel");
    if(chs == 2)
      return; //cancel
    else if (chs == 1) {
      taFiler* taf = GetFileDlg();
      taRefN::Ref(taf);
      ostream* strm = taf->Save();
      if((strm != NULL) && strm->good()) {
	taiMisc::RecordScript(GetPath() + ".Save(" + taf->fname + ");\n");
	SetFileName(taf->fname);
	DMEM_GUI_RUN_IF {
	  Save(*strm);
	}
      }
      taRefN::unRefDone(taf);
    }
    taiMisc::RecordScript(GetPath() + ".Close();\n");
    DMEM_GUI_RUN_IF {
      tabMisc::Close_Obj(this);
    }
  }
  cancel = false; */
}

WindowState& TopLevelViewer::winState() {
  return win_state; // TEMP: until is UserData, then we'll get from there and return addr
}


//////////////////////////
//   DockViewer		//
//////////////////////////

IDataViewWidget* DockViewer::ConstrWidget_impl(QWidget* gui_parent) {
//TODO: maybe we don't even need a generic one, but it does enable us to
// make a purely taBase guy that doesn't need its own special gui guy
  return new iDockViewer(this, gui_parent);
}


//////////////////////////
//   ToolBar		//
//////////////////////////

void ToolBar::Initialize() {
  index = -1; // assigned when added to list
  lft = 0.0f;
  top = 0.0f;
  o = Horizontal;
  mapped = false;
  m_window = NULL;
  m_viewer = NULL;
}

void ToolBar::Destroy() {
  CutLinks();
}

void ToolBar::Copy_(const ToolBar& cp) {
  lft = cp.lft;
  top = cp.top;
  o = cp.o;
  mapped = cp.mapped;
}

void ToolBar::CutLinks() {
//obs  taiMisc::CloseEdits((void*)this, GetTypeDef());
  CloseWindow();
//obs  win_owner = NULL;
  inherited::CutLinks();
}

void ToolBar::CloseWindow() {
  if (!m_window) return;
  m_window->m_toolBar = NULL;
  m_window->deleteLater();
  m_window = NULL;
}

void ToolBar::Constr_Window_impl() {
  if (m_window) return; // shouldn't happen
  iMainWindowViewer* vw = viewer_win();
  if (!vw) return;

  m_window = vw->Constr_ToolBar(this, this->GetName());
  viewer_win()->AddToolBar(m_window);
}

void ToolBar::Dump_Save_pre() {
  inherited::Dump_Save_pre();
  GetWinState();
}

void ToolBar::GetWinState() {
  if (!m_window) return;
  iRect r = m_window->frameGeometry();
  // convert from screen coords to relative (note, allowed to be >1.0)
  lft = (float)r.left() / (float)(taiM->scrn_s.w); // all of these convert from screen coords
  top = (float)r.top() / (float)(taiM->scrn_s.h);
  DataChanged(DCR_ITEM_UPDATED);
}

void ToolBar::OpenNewWindow_impl() {
  iMainWindowViewer* vw = viewer_win();
  if (!vw) return;
  Constr_Window_impl();
  if (!m_window) return; // shouldn't happen
  vw->InitToolBar(GetName(), m_window);
  SetWinState();
  m_window->show();
  mapped = true;
}

void ToolBar::SetWinState() {
  if (!m_window) return;

  //TODO: docked
  m_window->setOrientation((Qt::Orientation)o);
  iRect r = m_window->frameGeometry();
  r.x = (int)(lft * taiM->scrn_s.w);
  r.y = (int)(top * taiM->scrn_s.h);
  m_window->move(r.topLeft());
  m_window->resize(r.size());
}

void ToolBar::Show() {
  if (m_window) m_window->show();
  else OpenNewWindow_impl();
}

void ToolBar::Hide() {
  if (!m_window) return;
  m_window->hide();
}

MainWindowViewer* ToolBar::viewer() {
  if (!m_viewer) {
    m_viewer = GET_MY_OWNER(MainWindowViewer);
  }
  return m_viewer;
}

iMainWindowViewer* ToolBar::viewer_win() {
  return (viewer()) ? m_viewer->window() : NULL;
}

void ToolBar::WindowClosing() {
  GetWinState(); //huh? TODO: why now???
  m_window = NULL;
}


//////////////////////////
//   MainWindowViewer	//
//////////////////////////

// TEMP

void MainWindowViewer:: FileOptionsAction(){} // 
void MainWindowViewer:: FileCloseAction(){} // #ACT #MM_&File|&Close #MENUGP_LAST #MENU_GP_FileClose Quit Action(root) or Close Window Action(non-root)
void MainWindowViewer::	EditUndoAction(){} // #ACT 
void MainWindowViewer::	EditRedoAction(){} // #ACT 
void MainWindowViewer::	EditCutAction(){} // #ACT 
void MainWindowViewer::	EditCopyAction(){} // #ACT 
void MainWindowViewer::	EditPasteAction(){} // #ACT 
void MainWindowViewer::	EditFindAction(){} // #ACT 
void MainWindowViewer::	HelpIndexAction(){} // #ACT 
void MainWindowViewer::	HelpContentsAction(){} // #ACT 

// /TEMP

TypeDef* MainWindowViewer::def_browser_type = &TA_MainWindowViewer; 

MainWindowViewer* MainWindowViewer::NewBrowser(TAPtr root,
  MemberDef* root_md, bool is_root)
{
  if (!def_browser_type || !(def_browser_type->InheritsFrom(&TA_MainWindowViewer))) 
    def_browser_type = &TA_MainWindowViewer; // just in case
  
  MainWindowViewer* rval = (MainWindowViewer*)taBase::MakeToken(def_browser_type);
  rval->m_is_root = is_root;
  tabBrowseViewer* cb = tabBrowseViewer::New(root, root_md);
  rval->frames.Add(cb);
  TabViewer* pv = new TabViewer;
  rval->frames.Add(pv);
  return rval;
}


MainWindowViewer* MainWindowViewer::NewClassBrowser(void* root, TypeDef* root_typ,
  MemberDef* root_md)
{
  MainWindowViewer* rval = new MainWindowViewer;
  ClassBrowseViewer* cb = ClassBrowseViewer::New(root, root_typ, root_md);
  rval->frames.Add(cb);
  TabViewer* pv = new TabViewer;
  rval->frames.Add(pv);
  return rval;
}

void MainWindowViewer::Initialize() {
  m_is_root = false;
  menu = NULL;
  cur_menu = NULL;
  ta_menus = new taiMenu_List;
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

  // add default toolbars to new instance
  if (!taMisc::is_loading) {
    ToolBar* tb = new ToolBar();
    tb->SetName("Application");
    toolbars.Add(tb);
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
}

void MainWindowViewer::AddFrame(FrameViewer* fv, int at_index) 
{
  if (!fv) return;
  //note: if we are mapped, then the chg detector for .frames will map the guy
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

void MainWindowViewer::Clear_impl(taDataView* par) {
  docks.Clear_impl(this);
  frames.Clear_impl(this);
  inherited::Clear_impl(par);
}

void MainWindowViewer::CloseWindow_impl() {
  inherited::CloseWindow_impl();
  ta_menus->Reset();
  cur_menu = NULL;
  menu = NULL; // window deletes
}

void MainWindowViewer::Constr_impl(QWidget* gui_parent) {
  inherited::Constr_impl(gui_parent); // prob just creates the widget
  if (!dvwidget()) return; // shouldn't happen

  ConstrMainMenu_impl();
  ConstrToolBars_impl();
  ConstrFrames_impl();
  ConstrDocks_impl();

  iMainWindowViewer* win = window(); //cache
  win->is_root = m_is_root;
//TODO, replace:  win->SelectionChanged(true); // initializes selection system
}

void MainWindowViewer::Constr_post() {
  inherited::Constr_post();
  frames.Constr_post();
  docks.Constr_post();
}

void MainWindowViewer::ConstrDocks_impl() {
  //TODO: add the dock guys 
  // note: this is only ever for docked guys -- floating guys are standalone
}

void MainWindowViewer::ConstrFrames_impl() {
  iMainWindowViewer* win = window(); //cache
  for (int i = 0; i < frames.size; ++i) {
    FrameViewer* fv = frames.FastEl(i);
    if (!fv) continue; // shouldn't happen
    // note: don't parent the frame, since we use the api to add it
    fv->Constr_impl(NULL);
    win->AddFrameViewer(fv->widget());
  }
}

void MainWindowViewer::ConstrMainMenu_impl() {
 // TODO: new main menu constr 
}

/*TODO: somehow need to merge this concept with existing more hardwired menus on iMainWindowViewer
void MainWindowViewer::Constr_Menu_impl() {
  TypeDef* typ = GetTypeDef();

  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((!md->HasOption("VMENU")) || (md->im == NULL))
      continue;

    taiMethodData* mth_rep = md->im->GetMethodRep(NULL, NULL, NULL, NULL); // no buttons;
    if (mth_rep == NULL) continue;

//    this_meths.Add(mth_rep);

    // find or make current menu
    String men_nm = md->OptionAfter("MENU_ON_");
    if (men_nm != "") {
      cur_menu = ta_menus.FindName(men_nm);
      if (cur_menu != NULL)  goto cont;
    }
    if (cur_menu != NULL) goto cont;

    if (men_nm == "")
      men_nm = "File";
    cur_menu = menu->AddSubMenu(men_nm);
    ta_menus.Add(cur_menu);

cont:
    mth_rep->AddToMenu(cur_menu);
  }
} */

void MainWindowViewer::ConstrToolBars_impl() {
//TODO: Toolbar handling is tentative

  // show any visible toolbars
  for (int i = 0; i < toolbars.size; ++i) {
    ToolBar* tb = toolbars.FastEl(i);
    if (!tb->mapped) continue;
    tb->Show();
  }
  // populate the toolbar view menu 
  iMainWindowViewer* win = window(); //cache
  // populate the toolbar list
  for (int i = 0; i < toolbars.size; ++i) {
    ToolBar* tb = toolbars.FastEl(i);
    win->AddToolBarMenu(tb->GetName(), i);
  }
}

IDataViewWidget* MainWindowViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iMainWindowViewer(this, gui_parent);
}

void MainWindowViewer::DataChanged_Child(TAPtr child, int dcr, void* op1, void* op2) {
  if (child == &frames) {
    // if reorder, then do a gui reorder
    //TODO:
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

void MainWindowViewer::MakeWinName_impl() {
  String prog_nm = taiM->classname();
  String name;
  if (data()) name = data()->GetName();
  String nw_name = prog_nm + ": " + GetPath() + "(" + name + ")";
  win_name = nw_name;
}

void MainWindowViewer::Render_pre(taDataView* par) {
  inherited::Render_pre(par);
  frames.Render_pre(this);
  docks.Render_pre(this);
}

void MainWindowViewer::Render_impl() {
  inherited::Render_impl();
  frames.Render_impl();
  docks.Render_impl();
}

void MainWindowViewer::Render_post() {
  inherited::Render_post();
  frames.Render_post();
  docks.Render_post();
}

void MainWindowViewer::Reset_impl() {
  docks.Reset_impl();
  frames.Reset_impl();
  //TODO: prob should reset toolbars too
  inherited::Reset_impl();
}



/*obs bool MainWindowViewer::ThisMenuFilter(MethodDef* md) {
  if((md->name == "GetAllWinPos") || (md->name == "ScriptAllWinPos") ||
     (md->name == "SetWinState") || (md->name == "SelectForEdit") ||
     (md->name == "SelectFunForEdit")) return false;
  return true;
} */

void MainWindowViewer::WidgetDeleting() {
  menu = NULL;
  inherited::WidgetDeleting();
}
