// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "ta_viewer.h"

#include "ta_project.h"

#include "css_console.h"

#ifdef TA_GUI
# include "ta_qt.h"
# include "ta_qtviewer.h"
# include "t3viewer.h"
# include "ta_qtclassbrowse.h"
# ifdef HAVE_QT_CONSOLE
#   include "css_qtconsole.h"
# endif
# include <QApplication>
# include <QPrintDialog>
# include <QPrinter>
# include <QPixmap>
# include <QPainter>
# include <QScrollArea>
# include <QSplitter>
#endif


//////////////////////////
//   DataViewer		//
//////////////////////////

String_Array DataViewer::image_exts;

bool DataViewer::InitImageExts() {
  if(image_exts.size == IV+1) return false;
  image_exts.Reset();
  image_exts.Add("eps");
  image_exts.Add("jpg");
  image_exts.Add("png");
  image_exts.Add("ppm");
  image_exts.Add("iv");
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
//no: we only save explicitly in v4
//  GetWinState(); // prior to saving
}

bool DataViewer::GetWinState() {
  if (!isMapped()) return false;
  GetWinState_impl();
  return true;
}

void DataViewer::Hide() {
  if (!isMapped()) return;
  Hide_impl();
  visible = false;
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
  visible = true;
}

void DataViewer::Show_impl() {
  widget()->show();
  //  widget()->raise();
}

void DataViewer::setVisible(bool value, bool update_view) {
  if (visible == value) return;
  visible = value;
  if (update_view) {
    if (visible) Show(); else Hide();
  }
}

bool DataViewer::SetWinState() {
  if (!isMapped()) return false;
  SetWinState_impl();
  return true;
}


QPixmap DataViewer::GrabImage(bool& got_image) {
  if(!widget()) {
    got_image = false;
    return QPixmap();
  }
  got_image = true;
  return QPixmap::grabWidget(widget());
  //  return QPixmap::grabWindow(widget()->winId());
}

bool DataViewer::SaveImageAs(const String& fname, ImageFormat img_fmt) {
  if(TestError(img_fmt == EPS || img_fmt == IV, "SaveImageAs",
	       "EPS (encapsulated postscript) or IV (Open Inventor) not supported for this type of view"))
    return false;
  bool rval = false;
  String ext = image_exts.SafeEl(img_fmt);
  String fext = String(".") + ext; // filer needs .
  taFiler* flr = GetSaveFiler(fname, fext);
  if(flr->ostrm) {
    QPixmap pix = GrabImage(rval);
    if(rval) {
      flr->Close();
      int quality = taMisc::jpeg_quality;
      // for png, quality should be 0, which uses maximum deflate compression
      if (img_fmt == PNG) {
        quality = 0;
      }
      pix.save(flr->FileName(), ext, quality);
      cerr << "Saving image of size: " << pix.width() << " x " << pix.height() << " depth: " << pix.depth() << " to: " << flr->FileName() << endl;
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

MainWindowViewer* DataViewer::parent() const {
  if (!m_parent) {
    m_parent = (taDataView*)GetOwner(parentType()); // NULL if no owner, or no compatible type
  }
  return dynamic_cast<MainWindowViewer*>(m_parent); // dyn for safety
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

void DataViewer_List::GetWinState() {
  for (int i = 0; i < size; ++i) {
    FastEl(i)->GetWinState();
  }
}

void DataViewer_List::SetWinState() {
  for (int i = 0; i < size; ++i) {
    FastEl(i)->SetWinState();
  }
}


//////////////////////////////////
//   FrameViewer	 	//
//////////////////////////////////

void FrameViewer::Initialize() {
}

void FrameViewer::GetWinState_impl() {
  iFrameViewer* wid = widget();
  if (!wid) return; // not supposed to happen
  wid->GetWinState();
}

void FrameViewer::SetWinState_impl() {
  iFrameViewer* wid = widget();
  if (!wid) return; // not supposed to happen
  wid->SetWinState();
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

tabBrowseViewer* tabBrowseViewer::New(taBase* root, MemberDef* root_md) {
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
//   TopLevelViewer	//
//////////////////////////

void TopLevelViewer::Initialize() {
}

void TopLevelViewer::InitLinks() {
  inherited::InitLinks();
  if (taMisc::is_loading && isTopLevel() && openOnLoad())
      taiMisc::unopened_windows.LinkUnique(this);
}

void TopLevelViewer::CutLinks() {
  inherited::CutLinks();
}


void TopLevelViewer::Copy_(const TopLevelViewer& cp) {
  if (!GetName().empty())
    SetName(GetName() + "_copy");
}

void TopLevelViewer::GetWinState_impl() {
  QWidget* widget = this->widget(); // cache

  iRect r = widget->frameGeometry(); //note: same as size() for widgets
  // convert from screen coords to relative (note, allowed to be >1.0)
  // adjust for scrn geom, esp for evil mac
  float lft = (float)(r.left() - taiM->scrn_geom.left()) / 
    (float)(taiM->scrn_s.w); // all of these convert from screen coords
  float top = (float)(r.top() - taiM->scrn_geom.top()) / (float)(taiM->scrn_s.h);
  float wd = (float)r.width() / (float)(taiM->scrn_s.w);
  float ht = (float)r.height() / (float)(taiM->scrn_s.h);
  bool iconified = widget->isMinimized();
  // save in UserData
  SetUserData("view_win_lft", lft);
  SetUserData("view_win_top", top);
  SetUserData("view_win_wd", wd);
  SetUserData("view_win_ht", ht);
  SetUserData("view_win_iconified", iconified);
}

void TopLevelViewer::SetWinState_impl() {
  float lft = GetUserDataDef("view_win_lft", 0.0f).toFloat();
  float top = GetUserDataDef("view_win_top", 0.0f).toFloat();
  float wd = GetUserDataDef("view_win_wd", 1.0f).toFloat();
  float ht = GetUserDataDef("view_win_ht", 1.0f).toFloat();
  bool iconified = GetUserDataDef("view_win_iconified", false).toBool();
  // adjust ht in case we are using the console win
  if (taMisc::console_win) {
    if (ht > 0.8f) ht = 0.8f;
  }

/*
  // make sure to limit on mac, due to wacky menubar and dock
  //TODO: maybe this should be used for all? ex. windows or kde taskbar etc.
#ifdef TA_OS_MAC
  if (taiM) { // guard for instance creation before sys init
    lft = MAX(lft, (taiM->scrn_s.w > 0) ? 
      (float)(taiM->scrn_geom.left()) / (float)(taiM->scrn_s.w) : 0.0f);
    top = MAX(top, (taiM->scrn_s.h > 0) ? 
      (float)(taiM->scrn_geom.top()) / (float)(taiM->scrn_s.h) : 0.0f);
  }
//TODO: prob should limit wd and ht too, because of dock, and inability to size if grip is offscreen
#endif */
  
  QWidget* widget = this->widget(); // cache

  // convert to pixels
  // we use the screen geom offsets, mostly for evil mac
  iSize s((int)(wd * taiM->scrn_s.w), (int)(ht * taiM->scrn_s.h));
  FrameSizeToSize(s); // only does anything for wins(int)(ht * taiM->scrn_s.h)
  iRect tr = iRect(
    (int)((lft * taiM->scrn_s.w)) + taiM->scrn_geom.left(),
    (int)((top * taiM->scrn_s.h)) + taiM->scrn_geom.top(),
    s.w,
    s.h
  );

  //note: resize/move combo recommended by Qt
  widget->resize(tr.w, tr.h);
  widget->move(tr.x, tr.y);
  if (iconified)
    widget->showMinimized();
  else
    widget->show();
  
  SetWinName();
}

void TopLevelViewer::DeIconify() {
  if (!isMapped() || !isTopLevel()) return;
  if (widget()->isMinimized())
    widget()->showNormal();
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

void DockViewer::GetWinState_impl() {
  inherited::GetWinState_impl();
  QWidget* wid = widget(); //cache
  bool vis = wid->isVisible();
  SetUserData("view_visible", vis);
}

void DockViewer::SetWinState_impl() {
  inherited::SetWinState_impl();
  QWidget* wid = widget(); //cache
  bool vis = GetUserDataDef("view_visible", true).toBool();
  wid->setVisible(vis);
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
  widget()->setObjectName(name);
}

void ToolBar::GetWinState_impl() {
  inherited::GetWinState_impl();
  iRect r = widget()->frameGeometry();
  // convert from screen coords to relative (note, allowed to be >1.0)
  lft = (float)r.left() / (float)(taiM->scrn_s.w); // all of these convert from screen coords
  top = (float)r.top() / (float)(taiM->scrn_s.h);
  SetUserData("view_win_visible", widget()->isVisible());
  DataChanged(DCR_ITEM_UPDATED);
}

void ToolBar::SetWinState_impl() {
  inherited::SetWinState_impl();
  //TODO: docked, etc.
  iToolBar* itb = widget(); //cache
  bool vis = GetUserDataDef("view_win_visible", true).toBool();
  itb->setVisible(vis);
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
  if (tabMisc::root)
    tabMisc::root->viewers.Add(rval); // does InitLinks
  return rval;
} 

MainWindowViewer* MainWindowViewer::FindEditDialog(taBase* root) {
  if (!tabMisc::root) return NULL;
  for(int i=0; i<tabMisc::root->viewers.size; i++) {
    taDataView* dv = tabMisc::root->viewers[i];
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
  tabBrowseViewer* cb = tabBrowseViewer::New(proj);
  rval->frames.Add(cb);
  // panel guy
  fv = rval->AddFrameByType(&TA_PanelViewer);
  MainWindowViewer* viewer = NULL; // only for 2x2 mode
  switch (taMisc::proj_view_pref) {
  case taMisc::PVP_2x2: {
    rval->setBrowserViewer(true, false);
    viewer = (MainWindowViewer*)taBase::MakeToken(def_viewer_type);
    fv = viewer->AddFrameByType(&TA_PanelViewer);
    fv = viewer->AddFrameByType(&TA_T3DataViewer);
    viewer->SetData(proj);
    viewer->setBrowserViewer(false, true);
    // twiddle sizes a bit, to get overlap
    rval->SetUserData("view_win_wd", 0.6667f);
    viewer->SetUserData("view_win_lft", 0.3333f);
    viewer->SetUserData("view_win_wd", 0.6667f);
  } break;
  case taMisc::PVP_3PANE: {
    rval->setBrowserViewer(true, true);
    fv = rval->AddFrameByType(&TA_T3DataViewer);
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
  fv = viewer->AddFrameByType(&TA_T3DataViewer);
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
    // no effect:
//     if(frames.size >= 2 && i == frames.size -1 && fv->InheritsFrom(&TA_T3DataViewer)) {
//       // do not include the T3 guy in the tab order -- it is rather deadly
//       fv->widget()->setFocusPolicy(Qt::NoFocus); // not tab focus! -- note: not having any effect
//     }
  }
  // do not include the T3 guy in the tab order -- it is rather deadly
  // this doesn't seem to have any effect at all!
//   if(frames.size >= 2 && frames.SafeEl(-1)->InheritsFrom(&TA_T3DataViewer)) {
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

IDataViewWidget* MainWindowViewer::ConstrWidget_impl(QWidget* gui_parent) {
  return new iMainWindowViewer(this, gui_parent);
}

void MainWindowViewer::DataChanged_Child(taBase* child, int dcr, void* op1, void* op2) {
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
    iTabViewer* itv = pv->widget();
    return itv->tabView()->SetCurrentTab(tab_no);
  }
  return false;
}

bool MainWindowViewer::SelectPanelTabName(const String& tab_nm) {
  int idx;
  PanelViewer* pv = (PanelViewer*)FindFrameByType(&TA_PanelViewer, idx);
  if (pv && pv->widget()) {
    iTabViewer* itv = pv->widget();
    return itv->tabView()->SetCurrentTabName(tab_nm);
  }
  return false;
}

bool MainWindowViewer::SelectT3ViewTabNo(int tab_no) {
  int idx;
  T3DataViewer* pv = (T3DataViewer*)FindFrameByType(&TA_T3DataViewer, idx);
  if (pv && pv->widget()) {
    iT3DataViewer* itv = pv->widget();
    return itv->SetCurrentTab(tab_no);
  }
  return false;
}

bool MainWindowViewer::SelectT3ViewTabName(const String& tab_nm) {
  int idx;
  T3DataViewer* pv = (T3DataViewer*)FindFrameByType(&TA_T3DataViewer, idx);
  if (pv && pv->widget()) {
    iT3DataViewer* itv = pv->widget();
    return itv->SetCurrentTabName(tab_nm);
  }
  return false;
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
