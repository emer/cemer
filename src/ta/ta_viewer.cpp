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

#include "css_console.h"

#ifdef TA_GUI
# include "ta_qt.h"
# include "ta_qtviewer.h"
# include "ta_qtclassbrowse.h"
# ifdef HAVE_QT_CONSOLE
#   include "css_qtconsole.h"
# endif
# include <QApplication>
# include <QScrollArea>
#include <QPrintDialog>
#include <QPrinter>
#include <QPixmap>
#include <QPainter>
#endif

//TODO: this file will need to have many routines modalized for TA_GUI
// typically, just stub entire thing out -- maybe we can just stub out
// all these funs in the header, and then collect them all here in a #ifdef


//////////////////////////
//   DataViewer		//
//////////////////////////

String_Array DataViewer::image_exts;

bool DataViewer::InitImageExts() {
  if(image_exts.size == N_IMG_FMTS) return false;
  image_exts.Reset();
  image_exts.Add("eps");
  image_exts.Add("jpg");
  image_exts.Add("png");
  image_exts.Add("ppm");
  return true;
}

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
  visible = true; // default for most types
}

void DataViewer::Destroy() {
  CutLinks();
}

void DataViewer::InitLinks() {
  inherited::InitLinks();
  InitImageExts();
}

void DataViewer::CutLinks() {
  if (m_dvwidget) {
    m_dvwidget->Close(); // destructive close
    m_dvwidget = NULL; // 
  }
  inherited::CutLinks();
}

void DataViewer::Copy_(const DataViewer& cp) {
  visible = cp.visible; //note: not the same as mapped
//TODO: should probably call Clear() too
}

void DataViewer::CloseWindow_impl() { // only called if mapped
  m_dvwidget->Close(); // typically is destructive close, and calls us back + resets instance
  m_dvwidget = NULL; // for safety
  //note: don't call inherited, because we don't want to call children
  // gui destroy at this level should always destroy all nested gui items
}

void DataViewer::Constr(QWidget* gui_parent) {
  Constr_impl(gui_parent);
  Constr_post();
}

void DataViewer::Constr_impl(QWidget* gui_parent) {
  m_dvwidget = ConstrWidget_impl(gui_parent);
  m_dvwidget->Constr(); // virtual guy
}

void DataViewer::Constr_post() {
  m_dvwidget->Constr_post();
  // call inherited last, for a top-down type of call sequence
  inherited::Constr_post(); // does children
  // now, finally restore the state
  SetWinState();
}

void DataViewer::Dump_Save_pre() {
  inherited::Dump_Save_pre();
  GetWinState(); // prior to saving
}

void DataViewer::GetWinState() {
  if (!isMapped()) return;
  GetWinState_impl();
}

void DataViewer::Hide() {
  if (!isMapped()) return;
  Hide_impl();
  //TODO: set the "visible" property false
}

void DataViewer::Hide_impl() {
  widget()->hide();
}

bool DataViewer::isVisible() const {
  return visible;
}

void DataViewer::ResolveChanges(CancelOp& cancel_op) {
  if (!isMapped()) return;
  ResolveChanges_impl(cancel_op);
}

void DataViewer::ResolveChanges_impl(CancelOp& cancel_op) {
  dvwidget()->ResolveChanges(cancel_op);
}

void DataViewer::Show() {
  if (!isMapped()) return;
  Show_impl();
  //TODO: set the "visible" property true
}

void DataViewer::Show_impl() {
  widget()->show();
}

void DataViewer::setVisible(bool value, bool update_view) {
  if (visible == value) return;
  visible = value;
  if (update_view)
    if (visible) Show(); else Hide();
}

void DataViewer::SetWinState() {
  if (!isMapped()) return;
  SetWinState_impl();
}


QPixmap DataViewer::GrabImage(bool& got_image) {
  if(!widget()) {
    got_image = false;
    return QPixmap();
  }
  got_image = true;
  return QPixmap::grabWidget(widget());
}

bool DataViewer::SaveImageAs(const String& fname, ImageFormat img_fmt) {
  if(TestError(img_fmt == EPS, "SaveImageAs",
	       "EPS (encapsulated postscript) not supported for this type of view"))
    return false;
  bool rval = false;
  String ext = image_exts.SafeEl(img_fmt);
  taFiler* flr = GetSaveFiler(fname, ext);
  if(flr->ostrm) {
    QPixmap pix = GrabImage(rval);
    if(rval) {
      flr->Close();
      pix.save(flr->fileName(), ext, taMisc::jpeg_quality);
    }
  }
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

bool DataViewer::PrintImage() {
  bool rval = false;
  QPixmap pix = GrabImage(rval);
  if(rval) {
    QPrinter pr;
    QPrintDialog pd(&pr, widget());
    if(pd.exec() == QDialog::Accepted) {
      QPainter p(&pr);
      p.drawPixmap(0, 0, pix);
    }
  }  
  return rval;
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
//NOTE: do NOT put gui_active into this!!!
  return (m_dvwidget);
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

iMainWindowViewer* DataViewer::viewerWindow() const {
  if (m_dvwidget) return m_dvwidget->viewerWindow(); 
  else return NULL;
}


void DataViewer::WidgetDeleting() {
  WidgetDeleting_impl();
  if (deleteOnWinClose()) {
    // do a deferred delete
    // NOTE: these are kind of dangerous and can cause issues...
    taBase::Ref(this); // to prevent us deleting when removed from list
    CloseLater();
  }
}

void DataViewer::WidgetDeleting_impl() {
  m_dvwidget = NULL;
}

QWidget* DataViewer::widget() {
  return (m_dvwidget) ? m_dvwidget->widget() : NULL; 
}


//////////////////////////////////
//   DataViewer_List	 	//
//////////////////////////////////


//////////////////////////////////
//   FrameViewer	 	//
//////////////////////////////////

void FrameViewer::Initialize() {
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

void BrowseViewer::Render_pre() {
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
//nn  taMisc::TypeInfoKind tik = taMisc::TypeToTypeInfoKind(root_typ);
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
// 	PanelViewer	 	//
//////////////////////////////////

void PanelViewer::Initialize() {
}

IDataViewWidget* PanelViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iTabViewer(this, gui_parent);
}

/*void PanelViewer::Clear_impl() {
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
  wd = 0.95f;
  ht = 0.7f; // default window size
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

bool TopLevelViewer::deleteOnWinClose() const {
//NOTE: this behavior can be changed, but is the current default:
  // if we are in a project, then persist, else transient
  taProject* proj = GET_MY_OWNER(taProject);
  return (!proj);
}

void TopLevelViewer::Iconify() {
  if (!isMapped() || !isTopLevel()) return;
  if (!widget()->isMinimized())
    widget()->showMinimized();
  winState().iconified = true; // TEMP
}

void TopLevelViewer::GetWinState_impl() {
  winState().GetWinState();
}

void TopLevelViewer::SetWinName() {
//   if (!isMapped()) return;
  MakeWinName_impl();
  if(widget())
    widget()->setWindowTitle(win_name);
//     widget()->setCaption(win_name);
}

void TopLevelViewer::ViewWindow() {
  if (!taMisc::gui_active) return;
  if (dvwidget()) {
    DeIconify();
  } else {
    // if not owned yet, put us in the global guy
    if (!GetOwner() && tabMisc::root)
      tabMisc::root->viewers.Add(this); // does InitLinks
    Constr(); // no root
    Render();
    
//    if(((left != -1.0f) && (top != -1.0f)) || ((width != -1.0f) && (height != -1.0f)))
//      SetWinState(left, top, width, height);
  }
  Show();
}

void TopLevelViewer::WindowClosing(CancelOp& cancel_op) {
  ResolveChanges(cancel_op); // note, may have been done earlier
  if (cancel_op == CO_CANCEL) return;
  
  //root win is special, since closing it forces shutdown
  // if only implicitly closing it, ask user, and then maybe force shutdown
  if (isRoot()) {
    switch (taMisc::quitting) {
    case taMisc::QF_RUNNING: {
      int chs = taMisc::Choice("Closing this window will end the application.", "&Quit", "&Save All and Quit", "&Cancel");
      switch (chs) {
      case 1:
        taMisc::quitting = taMisc::QF_USER_QUIT; // tentative
        taiMiscCore::OnQuitting(cancel_op); // saves all edits etc.; restores running state if cancelled
        if (cancel_op == CO_CANCEL) return;
        if (tabMisc::root) tabMisc::root->SaveAll();
        //fall through
      case 0: {
        // to avoid bug we send a msg to main obj, then cancel here
        QEvent* ev = new QEvent((QEvent::Type)taiMiscCore::CE_QUIT);
        QCoreApplication::postEvent(taiM, ev);
       // taiMiscCore::Quit(CO_NOT_CANCELLABLE); // no going back now
        cancel_op = CO_CANCEL;
        } return;
        //WARNING: undefined after this point -- do not add any more code after calling Quit
      case 2:
        cancel_op = CO_CANCEL;
        taMisc::quitting = taMisc::QF_RUNNING; // in case anyone set
        return;
      } 
      } break;
    case taMisc::QF_USER_QUIT: 
      // ok, upgrade to non-cancellable
      taMisc::quitting = taMisc::QF_FORCE_QUIT;
    default: break; // force-quit
    }
  }
}

void TopLevelViewer::SetWinState_impl() {
  winState().SetWinState();
  SetWinName();
}

WindowState& TopLevelViewer::winState() {
  return win_state; // TEMP: until is UserData, then we'll get from there and return addr
}


//////////////////////////
//   DockViewer		//
//////////////////////////

void DockViewer::Initialize() {
  dock_flags = DV_NONE;
  dock_area = Qt::BottomDockWidgetArea;
}

IDataViewWidget* DockViewer::ConstrWidget_impl(QWidget* gui_parent) {
//TODO: maybe we don't even need a generic one, but it does enable us to
// make a purely taBase guy that doesn't need its own special gui guy
  return new iDockViewer(this, gui_parent);
}

//////////////////////////
//   ConsoleDockViewer
//////////////////////////

void ConsoleDockViewer::Initialize() {
  dock_flags = (DockViewerFlags)(DV_MOVABLE | DV_FLOATABLE);
}

IDataViewWidget* ConsoleDockViewer::ConstrWidget_impl(QWidget* gui_parent) {
  iDockViewer* dv = new iDockViewer(this, gui_parent); // par usually NULL

  QScrollArea* sa = new QScrollArea;
  sa->setWidgetResizable(true);
  dv->setWidget(sa);
//TODO: enable  this for the generic Q&D console, and modalize for QcssConsole

#if (defined(HAVE_QT_CONSOLE))
  QcssConsole* con = QcssConsole::getInstance(NULL, cssMisc::TopShell);
  sa->setWidget((QWidget*)con);
#endif
  return dv;
}

void ConsoleDockViewer::MakeWinName_impl() {
  win_name = "css Console";
}

//////////////////////////
//   ToolBoxDockViewer	//
//////////////////////////

ToolBoxDockViewer* ToolBoxDockViewer::New() {
  ToolBoxDockViewer* tb = new ToolBoxDockViewer;
  tb->SetName("Tools");
  tb->dock_flags = DockViewer::DockViewerFlags(DV_MOVABLE | DV_FLOATABLE);
  tb->dock_area = Qt::LeftDockWidgetArea;
  return tb;
}

void ToolBoxDockViewer::Initialize() {
}

void ToolBoxDockViewer::MakeWinName_impl() {
  //NOTE: descriptions for win and tabs need to be very short (not much space)
  win_name = "Tools";
}


//////////////////////////
//   ToolBoxRegistrar	//
//////////////////////////

ToolBoxRegistrar_PtrList* ToolBoxRegistrar::m_instances;

ToolBoxRegistrar_PtrList* ToolBoxRegistrar::instances() {
  if (!m_instances)
    m_instances = new ToolBoxRegistrar_PtrList;
  return m_instances;
}

ToolBoxRegistrar::ToolBoxRegistrar(ToolBoxProc proc_)
: proc(proc_)
{
  instances()->Add(this);
}


//////////////////////////
//   ToolBar		//
//////////////////////////

void ToolBar::Initialize() {
  lft = 0.0f;
  top = 0.0f;
  o = Horizontal;
  visible = false; // overrides base
}

void ToolBar::Copy_(const ToolBar& cp) {
  lft = cp.lft;
  top = cp.top;
  o = cp.o;
}

void ToolBar::Constr_impl(QWidget* gui_parent) {
  inherited::Constr_impl(gui_parent);
  widget()->setName(name);
}

void ToolBar::GetWinState_impl() {
  iRect r = widget()->frameGeometry();
  // convert from screen coords to relative (note, allowed to be >1.0)
  lft = (float)r.left() / (float)(taiM->scrn_s.w); // all of these convert from screen coords
  top = (float)r.top() / (float)(taiM->scrn_s.h);
  DataChanged(DCR_ITEM_UPDATED);
}

void ToolBar::SetWinState_impl() {
  //TODO: docked, etc.
  iToolBar* itb = widget(); //cache
  itb->setOrientation((Qt::Orientation)o);
  iRect r = itb->frameGeometry();
  r.x = (int)(lft * taiM->scrn_s.w);
  r.y = (int)(top * taiM->scrn_s.h);
  itb->move(r.topLeft());
  itb->resize(r.size());
}

void ToolBar::WidgetDeleting_impl() {
  inherited::WidgetDeleting_impl();
  visible = false;
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
TypeDef* MainWindowViewer::def_viewer_type = &TA_MainWindowViewer; 

MainWindowViewer* MainWindowViewer::GetDefaultProjectBrowser(taProject* proj) {
// look in list of viewer wins for most current
  for (int i = taiMisc::active_wins. size - 1; i >= 0; --i) {
    iMainWindowViewer* ivw = taiMisc::active_wins.FastElAsMainWindow(i); 
    // check if it is a proj viewer
    if (!ivw || !ivw->isProjViewer()) continue;
    // if proj specified, check if same
    if (proj && (ivw->curProject() != proj)) continue;
    return ivw->viewer();
  } 
  return NULL;
}

MainWindowViewer* MainWindowViewer::NewBrowser(TAPtr root,
  MemberDef* root_md, bool is_root)
{
  if (!def_browser_type || !(def_browser_type->InheritsFrom(&TA_MainWindowViewer))) 
    def_browser_type = &TA_MainWindowViewer; // just in case
  
  MainWindowViewer* rval = (MainWindowViewer*)taBase::MakeToken(def_browser_type);
  rval->SetData(root);
  rval->m_is_root = is_root;
  tabBrowseViewer* cb = tabBrowseViewer::New(root, root_md);
  rval->frames.Add(cb);
  PanelViewer* pv = new PanelViewer;
  rval->frames.Add(pv);
  // browsers are added to the global viewers, and not persistent
  if (tabMisc::root)
    tabMisc::root->viewers.Add(rval); // does InitLinks
  return rval;
}

MainWindowViewer* MainWindowViewer::NewClassBrowser(void* root, TypeDef* root_typ,
  MemberDef* root_md)
{
  MainWindowViewer* rval = new MainWindowViewer;
  ClassBrowseViewer* cb = ClassBrowseViewer::New(root, root_typ, root_md);
  rval->frames.Add(cb);
  PanelViewer* pv = new PanelViewer;
  rval->frames.Add(pv);
  // browsers are added to the global viewers, and not persistent
  if (tabMisc::root)
    tabMisc::root->viewers.Add(rval); // does InitLinks
  return rval;
}

MainWindowViewer* MainWindowViewer::NewProjectBrowser(taProject* proj) {
  if (!proj) return NULL;
  if (!def_viewer_type || !(def_viewer_type->InheritsFrom(&TA_MainWindowViewer))) 
    def_viewer_type = &TA_MainWindowViewer; // just in case
  
  MainWindowViewer* rval = (MainWindowViewer*)taBase::MakeToken(def_viewer_type);
  rval->SetData(proj);
  rval->m_is_proj_viewer = true;
  tabBrowseViewer* cb = tabBrowseViewer::New(proj);
  rval->frames.Add(cb);
  FrameViewer* fv = rval->AddFrameByType(&TA_PanelViewer);
  fv = rval->AddFrameByType(taMisc::types.FindName("T3DataViewer")); // sleazy, but effective
  // always added to the viewer collection
  proj->viewers.Add(rval);
  // global toolbar
  ToolBoxDockViewer* tb = ToolBoxDockViewer::New(); // initializes
  rval->AddDock(tb);
  return rval;
}

void MainWindowViewer::Initialize() {
  m_is_root = false;
  m_is_proj_viewer = false;
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

  // add default toolbars to new instance or if missing from loaded
  //TODO: cleaner if we put this in AssertDefaultToolBars
  AddToolBarByType(&TA_ToolBar, "Application");
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

  ConstrMainMenu_impl();
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
    ((DataViewer*)dv)->Constr_impl(NULL);
    win->AddDockViewer(dv->widget(), (Qt::DockWidgetArea)dv->dock_area);
  }
}

void MainWindowViewer::ConstrFrames_impl() {
  iMainWindowViewer* win = viewerWindow(); //cache
  for (int i = 0; i < frames.size; ++i) {
    FrameViewer* fv = frames.FastEl(i);
    if (!fv) continue; // shouldn't happen
    // note: don't parent the frame, since we use the api to add it
    ((DataViewer*)fv)->Constr_impl(NULL);
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
  for (int i = 0; i < toolbars.size; ++i) {
    ToolBar* tb = toolbars.FastEl(i);
    OnToolBarAdded(tb, false);
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

void MainWindowViewer::Hide_impl() {
  widget()->lower();
}

void MainWindowViewer::MakeWinName_impl() {
  String prog_nm = taiM->classname(); // note: this is not working
  String name;
  String fname;
  if (data()) {
    name = data()->GetName();
    fname = taMisc::GetFileFmPath(data()->GetFileName());
  }
  String nw_name = GetPath() + "(" + name + ") - " + fname; // prog_nm ;
  win_name = nw_name;
}

void MainWindowViewer::OnToolBarAdded(ToolBar* tb, bool post_constr) {
//TODO: just nuke the post_constr variable -- 
  iMainWindowViewer* win = viewerWindow(); //cache
  //note: always costructed, even if not visible
  ((DataViewer*)tb)->Constr_impl(NULL);
  
  // add to the toolbar view menu 
  win->AddToolBar(tb->widget());
  if (post_constr) {
    ((DataViewer*)tb)->Constr_post(); // gotta do this manually post-Constr
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

/*obs
void MainWindowViewer::ResolveChanges_impl(CancelOp& cancel_op) {
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
} */

void MainWindowViewer::Show_impl() {
  SetWinName();
  QWidget* wid = widget();
  wid->raise();
  qApp->setActiveWindow(wid);
}

/*obs bool MainWindowViewer::ThisMenuFilter(MethodDef* md) {
  if((md->name == "GetAllWinPos") || (md->name == "ScriptAllWinPos") ||
     (md->name == "SetWinState") || (md->name == "SelectForEdit") ||
     (md->name == "SelectFunForEdit")) return false;
  return true;
} */

void MainWindowViewer::WidgetDeleting_impl() {
  menu = NULL;
  inherited::WidgetDeleting_impl();
}


//////////////////////////
//   TreeDecorationSpec	//
//////////////////////////


void TreeDecorationSpec::Initialize() {
}

void TreeDecorationSpec::Destroy() {
}

void TreeDecorationSpec::InitLinks() {
  inherited::InitLinks();
  taBase::Own(&text_color, this);
}

void TreeDecorationSpec::CutLinks() {
  text_color.CutLinks();
  inherited::CutLinks();
}

void TreeDecorationSpec::Copy_(const TreeDecorationSpec& cp) {
  text_color = cp.text_color;
}
