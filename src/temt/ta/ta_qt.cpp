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


// ta_qt.cc

#include "ta_script.h"
#include "ta_qt.h"
#include "ta_viewer.h"
#include "ta_qtdialog.h"
#include "ta_qtviewer.h"
#include "ta_qttype_def.h"
#include "css_qt.h"
#include "css_ta.h"
#include "ta_qtcursors.h" //note: only place this s/b included
#include "ta_platform.h"
#include "ta_TA_type.h"

#include "icolor.h"
#include "ilineedit.h"
#include "inetworkaccessmanager.h"
#include "icookiejar.h"
# include <QWebSettings>

#include <qapplication.h>
#include <qbitmap.h>
#include <qcolor.h> // needed for qbitmap
#include <qcursor.h>
#include <qdesktopwidget.h>
#include <qfont.h>
#include <QFontMetrics>
#include <qrect.h>

#include <qlabel.h>// metrics
#include <qmenubar.h>// metrics
#include <QMessageBox>
#include <qprogressdialog.h>
#include <qpushbutton.h> // metrics
#include <QSessionManager>
#include <QKeyEvent>
#include <QScrollBar>

// #ifdef TA_USE_INVENTOR
// #endif


//#include <ostream.h>

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

using namespace std;

// Hook function that ReadLine calls 10 times/s in order to pump the Qt event loop

int tai_rl_hook_proc() {
  // processes events for 3 sec. or until no more events, whichever comes first
  qApp->processEvents();
  return 0; // what is this for?
}


//////////////////////////////////////////////////////////
// 	taiMisc: miscellaneous useful stuff 		//
//////////////////////////////////////////////////////////

iMainWindowViewer* iTopLevelWindow_List::FindMainWindowById(int id) {
  for (int i = 0; i < size; ++i) {
    iMainWindowViewer* rval = SafeElAsMainWindow(i);
    if (rval && (rval->uniqueId() == id)) return rval;
  }
  return NULL;
} 

iMainWindowViewer* iTopLevelWindow_List::SafeElAsMainWindow(int i) {
  return dynamic_cast<iMainWindowViewer*>(PosSafeEl(i)->widget());
}

iDockViewer* iTopLevelWindow_List::SafeElAsDockWindow(int i) {
  return dynamic_cast<iDockViewer*>(PosSafeEl(i)->widget());
}

void iTopLevelWindow_List::GotFocus_MainWindow(iMainWindowViewer* imw) {
  int idx = FindEl(imw);
  if (idx > 0) MoveIdx(idx, 0);
}

void iTopLevelWindow_List::GotFocus_DockWindow(iDockViewer* idv) {
  int idx = FindEl(idv);
  if (idx > 0) MoveIdx(idx, 0);
}

iMainWindowViewer* iTopLevelWindow_List::Peek_MainWindow() {
  for (int i = 0; i < size; ++i) {
    iMainWindowViewer* rval = SafeElAsMainWindow(i);
    if (rval) return rval;
  }
  return NULL;
}

iDockViewer* iTopLevelWindow_List::Peek_DockWindow() {
  for (int i = 0; i < size; ++i) {
    iDockViewer* rval = SafeElAsDockWindow(i);
    if (rval) return rval;
  }
  return NULL;
}


TA_API taiMisc* taiM_ = NULL;

const int taiMisc::FONT_MED = 1;
const int taiMisc::FONT_SM = 2;

taiHostDialog_List 	taiMisc::active_dialogs;
taiEditDataHost_List	taiMisc::active_edits;
taiEditDataHost_List 	taiMisc::css_active_edits;
iTopLevelWindow_List	taiMisc::active_wins;
TypeSpace		taiMisc::arg_types;
QPointer<iMainWindowViewer> taiMisc::main_window;
taBase_PtrList		taiMisc::unopened_windows;
void (*taiMisc::Update_Hook)(taBase*) = NULL;
iNetworkAccessManager*	taiMisc::net_access_mgr = NULL;

int taiMisc::busy_count = 0;


taiMisc* taiMisc::New(bool gui, QObject* parent) {
  taiMisc* rval = new taiMisc(parent);
  rval->Init(gui);
  return rval;
}

taiMisc::taiMisc(QObject* parent)
:inherited(parent) 
{
}

void taiMisc::Init(bool gui) {
  inherited::Init(gui);

  taMisc::WaitProc = &WaitProc; // typically gets replaced in pdpbase.cpp

  taMisc::ScriptRecordingGui_Hook = &ScriptRecordingGui_; // note: ok to do more than once
  load_dlg = NULL;
  InitMetrics();

  if (taMisc::not_constr || taMisc::in_init) 
    return;

  QBitmap waiter = QBitmap::fromData(QSize(wait_cursor_width, wait_cursor_height), 
    wait_cursor_bits, QImage::Format_MonoLSB);
  QBitmap waiter_m = QBitmap::fromData(QSize(wait_cursor_width, wait_cursor_height), 
    wait_mask_bits, QImage::Format_MonoLSB);
  wait_cursor = new QCursor(waiter, waiter_m, wait_cursor_x_hot, wait_cursor_y_hot);

  // Record cursor
  QBitmap recorder = QBitmap::fromData(QSize(record_cursor_width, record_cursor_height),
    record_cursor_bits, QImage::Format_MonoLSB);
  QBitmap recorder_m = QBitmap::fromData(QSize(record_cursor_width, record_cursor_height), 
    record_mask_bits, QImage::Format_MonoLSB);
  record_cursor = new QCursor(recorder, recorder_m, record_cursor_x_hot, record_cursor_y_hot);

  icon_bitmap = NULL;
  QDesktopWidget* dw = QApplication::desktop();
  connect(dw, SIGNAL(resized(int)), 
    this, SLOT(desktopWidget_resized(int)));
  connect(dw, SIGNAL(workAreaResized(int)), 
    this, SLOT(desktopWidget_workAreaResized(int)));

  QWebSettings *defaultSettings = QWebSettings::globalSettings();
  defaultSettings->setAttribute(QWebSettings::PluginsEnabled, true);

  net_access_mgr = new iNetworkAccessManager;
  net_access_mgr->setCookieJar(new iCookieJar(net_access_mgr));
}

int taiMisc::Exec_impl() {
  return qApp->exec();
}


taiMisc::~taiMisc() {
  delete wait_cursor;
  wait_cursor = NULL;
  delete record_cursor;
  record_cursor = NULL;
  delete net_access_mgr;
  net_access_mgr = NULL;
  if (taiM_ == this)
    taiM_ = NULL;
}

void taiMisc::Busy_(bool busy) {
  if (!taMisc::gui_active)    return;
  if (busy) {
    ++busy_count;	// keep track of number of times called
  //  if (cssiSession::block_in_event == true) // already busy
  //    return;
  //  cssiSession::block_in_event = true;
    if (busy_count == 1) SetWinCursors();
  } else {
    if(--busy_count == 0) {
      RestoreWinCursors(); //added 4.0
    }
    if (busy_count < 0) {
      cerr << "Warning: taiMisc::DoneBusy() called more times than Busy()\n";
      busy_count = 0;
    }
  }
}

void taiMisc::CheckConfigResult_(bool ok) {
//note: only called if !quiet, and if !ok only if confirm_success
  if (!taMisc::gui_active) {
    inherited::CheckConfigResult_(ok);
    return;
  }
  
  if (ok) {
    taiChoiceDialog::ConfirmDialog(QApplication::activeWindow(), 
      "No configuration errors were found.",
      "Check Succeeded", false);
  } else {
    iTextEditDialog* td = new iTextEditDialog(true);
    td->setWindowTitle("Check Failed");
    td->setText(taMisc::last_check_msg);
    td->exec();
    td->deleteLater();
  }
}

void taiMisc::GetWindowList(Widget_List& rval) {
//note: try the app's thoughts on this...
  foreach(QWidget* widget, QApplication::topLevelWidgets()) {
    if (!widget->isHidden())
      rval.Add(widget);
  }
}

void taiMisc::desktopWidget_resized(int screen) {
  HandleScreenGeomChange();
}

void taiMisc::desktopWidget_workAreaResized(int screen) {
  HandleScreenGeomChange();
}

void taiMisc::HandleScreenGeomChange() {
  QRect old_scrn_geom(scrn_geom);
  InitMetrics(true);
  foreach(QWidget* widget, QApplication::topLevelWidgets()) {
    if (!widget->isHidden())
      HandleScreenGeomChange_Window(old_scrn_geom, widget);
  }
/*  for (int i = 0; i < active_wins.size; ++i) {
    QWidget* win = active_wins.FastEl(i);
    Window_HandleScreenGeomChange(win);
  }*/
}

void taiMisc::HandleScreenGeomChange_Window(const QRect& old_scrn_geom, QWidget* win) {
  // if size bigger than avail size, then shrink it
  // easiest policy is just to resize top-level wins by change in size
  QRect r(win->frameGeometry());
  QSize new_sz((int)(r.width() * ((float)scrn_s.width() / old_scrn_geom.width())),
    (int)(r.height() * ((float)scrn_s.height() / old_scrn_geom.height())));
  win->resize(new_sz);
  
  //TODO
  // move it to same size-relative point, in new coords
  // this handles off-screen cases, as well as things like task bar redocks, etc.
  
  // moves only if top left no longer accessible
  const int marg = 50; // so user is able to grab title bar etc.
  bool redo = false;
  QPoint pos(r.topLeft());
  if (r.x() > (scrn_geom.right() - marg)) {
    pos.setX(scrn_geom.right() - marg);
    redo = true;
  } else if (r.x() < scrn_geom.left()) { // task bar left case
    pos.setX(scrn_geom.left());
    redo = true;
  }
  if (r.y() > (scrn_geom.bottom() - marg)) {
    pos.setY(scrn_geom.bottom() - marg);
    redo = true;
  } else if (r.y() < scrn_geom.top()) { // task bar top case
    pos.setY(scrn_geom.top());
    redo = true;
  }
  if (redo) {
    win->move(pos);
  }
}

void taiMisc::InitMetrics(bool reinit) {
  // everything that requires Qt to be initialized and could depend on Settings being loaded
  
  if (!reinit) {
    // set up the initial font from (already loaded) Settings
    QFont font(taMisc::font_name, taMisc::font_size);
    //TODO: we should probably put practical lower/upper limits on font sizes
    qApp->setFont(font);
    
    edit_darkbg_brightness = -0.15f;
    edit_lightbg_brightness = 0.50f;
  }
  
  ctrl_size = sizMedium;
  vsep_c = 3;
  hsep_c = 3;
  vspc_c = 6;
  hspc_c = 6;
  dlgm_c = 8;
// default dialog sizes
  QDesktopWidget *d = QApplication::desktop();
  int primaryScreen = d->primaryScreen();
  scrn_geom = d->availableGeometry(primaryScreen);
  scrn_s = scrn_geom.size();
  if (scrn_s.height() <= 768)
    base_height = 21;
  else if (scrn_s.height() <= 1024)
    base_height = 25;
  else
    base_height = 29;
    
  // control sizes -- depend on size of default font
  if (taMisc::font_size <= 10) {
    // Small
//TODO:  not extensively tested as of 9/24/06
    mbutton_ht[0] = 21; mbutton_ht[1] = 23; mbutton_ht[2] = 24;
    mlabel_ht[0] = 17; mlabel_ht[1] = 19; mlabel_ht[2] = 20;
    mtext_ht[0] = 19; mtext_ht[1] = 21; mtext_ht[2] = 22;
  } else if (taMisc::font_size > 12) {
    // Big
//TODO:  not extensively tested as of 9/24/06
    mbutton_ht[0] = 27; mbutton_ht[1] = 30; mbutton_ht[2] = 33;
    mlabel_ht[0] = 24; mlabel_ht[1] = 26; mlabel_ht[2] = 29;
    mtext_ht[0] = 26; mtext_ht[1] = 28; mtext_ht[2] = 31;
  }  else {
    // Med
    mbutton_ht[0] = 24;  mbutton_ht[1] = 25;  mbutton_ht[2] = 27;
    mlabel_ht[0] = 20;  mlabel_ht[1] = 21;  mlabel_ht[2] = 24;
    mtext_ht[0] = 22;  mtext_ht[1] = 23;  mtext_ht[2] = 26;
  } 
  
  // fonts -- note, no way to get Qt's metrics without instances!
  QWidget* w = NULL;
  w = new QPushButton("the rain in spain", (QWidget*)NULL);
  mbig_button_font = QApplication::font(w);
  QFontMetrics fm(mbig_button_font);
  max_button_width = fm.size(Qt::TextSingleLine,"12345678901234567890").width();
  delete w;

  mbig_dialog_font = QApplication::font(); // use the default font

  w = new QMenuBar();
    mbig_menu_font = QApplication::font(w);
  delete w;

  w = new QLabel("the rain in spain", (QWidget*)NULL);
    mbig_name_font = QApplication::font(w);
  delete w;
  
}

/* Qt Metrics note

On Linux/X11, using Sans font (default)

Default Widget sizes for various default font sizes --
() indicates a usable size

			Sans-9		Sans-10		Sans-11
QPushButton		31 (24)		32 (25)		35 (27)
QLabel			20		20		20
QLineEdit		22		23		26
  QSpinBox		22		23
QComboBox		27 (24)		27 (25)		27
QRadioButton		20		21		24
  QCheckBox		20


Actual Widget sizes for SizeSpec sizes --
 indented names means ctrl assumes size of outdented control above it

			sizSmall	sizMed		sizBig
QPushButton		24		25		27
  QComboBox
QLabel			20		21		24
  QCheckBox
QLineEdit		22		23		26
  QSpinBox



*/
int taiMisc::button_height(int sizeSpec) {
  switch (sizeSpec & siz_mask) {
  case sizSmall: return mbutton_ht[0]; break;
  case sizBig: return mbutton_ht[2]; break;
  default: return mbutton_ht[1]; break;
  }
}

int taiMisc::combo_height(int sizeSpec) {
  return button_height(sizeSpec);
}

int taiMisc::max_control_height(int sizeSpec) {
  return button_height(sizeSpec);
}

int taiMisc::label_height(int sizeSpec) {
  switch (sizeSpec & siz_mask) {
  case sizSmall: return mlabel_ht[0]; break;
  case sizBig: return mlabel_ht[2]; break;
  default: return mlabel_ht[1]; break;
  }
}

int taiMisc::text_height(int sizeSpec) {
  switch (sizeSpec & siz_mask) {
  case sizSmall: return mtext_ht[0]; break;
  case sizBig: return mtext_ht[2]; break;
  default: return mtext_ht[1]; break;
  }
}

void taiMisc::AdjustFont(int fontSpec, iFont& font) {
  if (fontSpec & fonItalic) font.setItalic(true);
  if (fontSpec & fonBold) font.setBold(true);
  if (fontSpec & fonUnderline) font.setUnderline(true);
  if ((fontSpec & fonStretch_mask) == fonSkinny) font.setStretch(QFont::Condensed);
  if ((fontSpec & fonStretch_mask) == fonWide) font.setStretch(QFont::Expanded);
  int fsz = (fontSpec & fonSize_mask);
  switch (fsz) {
  case fonSmall:
    font.setPointSize(font.pointSize - FONT_SM);
    break;
     //NOTE: assumes not using pixel-size, otherwise       
  case fonMedium:
     font.setPointSize(font.pointSize - FONT_MED); 
  case fonBig: // BIG is the default, no adjust needed
  default: //defFontSize
    break;
  }
}

iSize taiMisc::dialogSize(int dialogSpec) {
  iSize rval;
  bool ver = (dialogSpec & dlgVer);
  dialogSpec &= dlgSize_mask;
  if (dialogSpec == 0) dialogSpec = dlgMedium;
  switch (dialogSpec) {
  case dlgMini:
    if (ver) rval = iSize((scrn_s.w * 2) / 10, (scrn_s.h * 3) / 10);// .2w .3h small vertically oriented dialog (ex. popup list selectors)
    else rval = iSize((scrn_s.w * 3) / 10, (scrn_s.h * 2) / 10); // .3w .2h small horizontally oriented dialog
    break;
  case dlgSmall:
    if (ver) rval = iSize((scrn_s.w * 3) / 10, (scrn_s.h * 2) / 5);// .3w .4h small vertically oriented dialog (ex. popup list selectors)
    else rval = iSize((scrn_s.w * 2) / 5, (scrn_s.h * 3) / 10); // .4w .3h small horizontally oriented dialog
    break;
  case dlgBig:
    if (ver) rval = iSize((scrn_s.w * 2) / 5, (scrn_s.h * 4) / 5);// .4w .8h  big vertically oriented dialog
    else rval = iSize((scrn_s.w * 4) / 5, (scrn_s.h * 3) / 5);// .8w .6h big horizontally oriented dialog
    break;
  case dlgMedium:
  default: // medium, the default
    if (ver) rval = iSize((scrn_s.w * 2) / 5, (scrn_s.h * 3) / 5);// .4w .6h  medium vertically oriented dialog
    else rval = iSize((scrn_s.w * 3) / 5, (scrn_s.h * 2) / 5);// .6w .4h  medium horizontally oriented dialog
    break;
  }
  return rval;
}

iFont taiMisc::buttonFont(int fontSpec) {
  iFont rval = mbig_button_font;
  AdjustFont(fontSpec, rval);
  return rval;
}

iFont taiMisc::dialogFont(int fontSpec) {
  iFont rval = mbig_dialog_font;
  AdjustFont(fontSpec, rval);
  return rval;
}

iFont taiMisc::menuFont(int fontSpec) {
  iFont rval = mbig_menu_font;
  AdjustFont(fontSpec, rval);
  return rval;
}

iFont taiMisc::nameFont(int fontSpec) {
  iFont rval = mbig_name_font;
  AdjustFont(fontSpec, rval);
  return rval;
}

void taiMisc::FormatButton(QAbstractButton* but, const String& text,
  int fontSpec) 
{
  but->setFont(taiM->menuFont(fontSpec)); //note: we use menu font -- TODO: might need to use a button font
  but->setMaximumWidth(maxButtonWidth(fontSpec));
  but->setText(text);
//TODO: may conflict w/ desc text
//  but->setToolTip(text);
}

void taiMisc::LoadDialogDestroyed() {
  load_dlg = NULL;
}

int taiMisc::maxButtonWidth(int sizeSpec) const {
//TODO: should probably parameterize
  return max_button_width;
}

QLabel* taiMisc::NewLabel(const String& text, QWidget* parent, int fontSpec) {
  if (fontSpec == 0) fontSpec = ctrl_size;
  QLabel* rval = new QLabel(text, parent);
  rval->setFixedHeight(label_height(fontSpec));
  rval->setFont(nameFont(fontSpec));
  return rval;
}

void taiMisc::OnQuitting_impl(CancelOp& cancel_op) {
// called when quitting -- does all saves
  ResolveEditChanges(cancel_op);
  if (cancel_op == CO_CANCEL) return;
  ResolveViewerChanges(cancel_op);
}

void taiMisc::Quit_impl(CancelOp cancel_op) {
  if(taMisc::gui_no_win) {
    QCoreApplication::instance()->quit();
  }
  else {
    qApp->closeAllWindows();
  }
}

void taiMisc::ResolveEditChanges(CancelOp& cancel_op) {
  for (int i = 0; i < taiMisc::active_edits.size; ++i) {
    taiDataHostBase* edh = taiMisc::active_edits.FastEl(i);
    if (!edh || (edh->state != taiEditDataHost::ACTIVE)) continue;
    edh->ResolveChanges(cancel_op); // don't need 'discard'
    if (cancel_op == CO_CANCEL) return;
  }
}

void taiMisc::ResolveViewerChanges(CancelOp& cancel_op) {
  for (int i = 0; i < taiMisc::active_wins.size; ++i) {
    IDataViewWidget* dvw = taiMisc::active_wins.FastEl(i);
    dvw->ResolveChanges(cancel_op);
    if (cancel_op == CO_CANCEL) return;
  }
}

void taiMisc::Update(taBase* obj) {
  if (Update_Hook != NULL)
    (*Update_Hook)(obj);
}

void taiMisc::PurgeDialogs() {
  bool did_purge = false;
  for (int i = active_dialogs.size - 1; i >= 0; --i) {
    taiDataHost* dlg = active_dialogs.FastEl(i);
    if ((dlg->state & taiDataHost::STATE_MASK) >= taiDataHost::ACCEPTED) {
      active_dialogs.RemoveIdx(i);
      did_purge = true;
    }
  }
}

void taiMisc::RestoreWinCursors() {
  QApplication::restoreOverrideCursor();
}

void taiMisc::ScriptRecordingGui_(bool start){
  if (!taMisc::gui_active)    return;
  if (start) SetWinCursors();
  else       RestoreWinCursors();
}

void taiMisc::SetWinCursors() {
  QPointer<QWidget> m_prev_active = QApplication::activeWindow();

  bool is_busy = false;
  bool is_rec = false;
  if((taiMisc::busy_count > 0) /*|| cssiSession::block_in_event*/)
    is_busy = true;
  if (taMisc::record_script !=NULL)
    is_rec = true;
  // busy trumps recording...
  if (is_busy) {
    QApplication::setOverrideCursor(*taiM->wait_cursor);
    goto bail;
  }

  if (is_rec) {
    QApplication::setOverrideCursor(*taiM->record_cursor);
    goto bail;
  }

  taMisc::Warning("*** Unexpected call to SetWinCursors -- not busy or recording.");

 bail:
  if((bool)m_prev_active) {
//     QApplication::setActiveWindow(m_prev_active);
    // note: above does NOT work! -- likely source of bug in cocoa 4.6.0
    m_prev_active->activateWindow();
  }
}

/*bool taiMisc::RevertEdits(void* obj, TypeDef*) {
  if (!taMisc::gui_active)    return false;
  bool got_one = false;
  for (int i = active_edits.size-1; i >= 0; --i) {
    taiEditDataHost* dlg = active_edits.FastEl(i);
    if((dlg->root == obj) && (dlg->state == taiDataHost::ACTIVE)) {
      dlg->Revert_force();
      got_one = true;
    }
  }
  return got_one;
}*/

/*obs bool taiMisc::ReShowEdits(void* obj, TypeDef*, bool force) {
  if (!taMisc::gui_active)    return false;
  bool got_one = false;
  for (int i = active_edits.size-1; i >= 0; --i) {
    taiEditDataHost* edh = active_edits.FastEl(i);
    if((edh->root == obj) && (edh->state == taiDataHost::ACTIVE)) {
      edh->ReShow_Async(force);
      got_one = true; 
    }
  }
  return got_one;
}*/

taiEditDataHost* taiMisc::FindEdit(void* obj, iMainWindowViewer* not_in_win) {
  //NOTE: not_in_win works as follows:
  // NULL: ok to return any edit (typically used to get show value)
  // !NULL: must get other win that not; used to raise that edit panel to top, so
  //  shouldn't hide the edit panel that invoked the operation
  if (!taMisc::gui_active) return NULL;
  for (int i = active_edits.size - 1; i >= 0; --i) {
    taiEditDataHost* host = active_edits.FastEl(i);
    if ((host->root != obj) || (host->state != taiDataHost::ACTIVE))
      continue;
    if (host->isDialog() && !host->modal)
      return host;
    // is a EditPanel
    if (host->isPanel() && (host->dataPanel()->window() != not_in_win)) // if niw NULL, then will always be true
      return host;
  }
  return NULL;
}

taiEditDataHost* taiMisc::FindEditDialog(void* obj, bool read_only_state) {
  if (!taMisc::gui_active) return NULL;
  for (int i = active_edits.size - 1; i >= 0; --i) {
    taiEditDataHost* host = active_edits.FastEl(i);
    if ((host->root != obj) || (host->state != taiDataHost::ACTIVE))
      continue;
    if (host->isDialog() && !host->modal && (host->read_only == read_only_state))
      return host;
  }
  return NULL;
}

taiEditDataHost* taiMisc::FindEditPanel(void* obj, bool read_only,
  iMainWindowViewer* not_in_win) 
{
  //NOTE: not_in_win works as follows:
  // NULL: ok to return any edit (typically used to get show value)
  // !NULL: must get other win that not; used to raise that edit panel to top, so
  //  shouldn't hide the edit panel that invoked the operation
  if (!taMisc::gui_active) return NULL;
  for (int i = active_edits.size - 1; i >= 0; --i) {
    taiEditDataHost* host = active_edits.FastEl(i);
    if ((host->root != obj) || (host->state != taiDataHost::ACTIVE))
      continue;
    if (host->isPanel() && (host->read_only == read_only) &&
      (host->dataPanel()->window() != not_in_win)) // if niw NULL, then will always be true
      return host;
  }
  return NULL;
}

void taiMisc::OpenWindows(){
  if(unopened_windows.size == 0)	return;
  if (taMisc::is_loading) return; // don't recurse

  //TODO: WHY are we setting the loading context????
  ++taMisc::is_loading;
  int i;
  for(i=0;i < unopened_windows.size;i++){
    TopLevelViewer* win = dynamic_cast<TopLevelViewer*>(unopened_windows.FastEl(i));
    if (!win) continue;
    win->ViewWindow();
    win->UpdateAfterEdit();
  }
  taiMisc::RunPending();
  unopened_windows.RemoveAll();
  --taMisc::is_loading;
  cssiSession::RaiseObjEdits(); // make sure css objects are in front!

  taiMisc::RunPending();
}

void taiMisc::WaitProc() {
  if(taMisc::gui_active) {
    taiMisc::OpenWindows();
  }
  taiMiscCore::WaitProc();
  if (!taMisc::gui_active) return;
  AbstractScriptBase::Wait_RecompileScripts();
}

void taiMisc::ScriptIconify(void*, int) {
// do nothing, use script win pos to record final iconify status
//  DataViewer* wb = (DataViewer *) obj;
//   if(onoff)
//     taiMisc::RecordScript(wb->GetPathNames() + ".Iconify();\n");
//   else
//     taiMisc::RecordScript(wb->GetPathNames() + ".DeIconify();\n");
}


/*nn void taiMisc::CreateLoadDialog(const char* caption){
  if (!taMisc::gui_active) return;
  String cap = caption;
  if (caption)
    cap = caption;
  else
    cap = "Loading...............";
  if (load_dlg == NULL) {
    load_dlg = new QProgressDialog(qApp->mainWidget(), NULL, false, Qt::WDestructiveClose); // no name, not modal
    connect(load_dlg, SIGNAL(destroyed()), this, SLOT(LoadDialogDestroyed()) );
  }
  load_dlg->setCaption(cap);
}

void taiMisc::SetLoadDialog(char* tpname, int totalSteps){
  if (!taMisc::taMisc::gui_active) return;
  if (load_dlg == NULL) return;
  String loadstring = String("Loading: ") + String(tpname);
  load_dlg->setLabelText(loadstring);
  load_dlg->setTotalSteps(totalSteps);
  if (totalSteps == 0)
    load_dlg->setMinimumDuration(0); // no show delay
  RunPending();
}

void taiMisc::StepLoadDialog(int stepNum) {
  if (!taMisc::taMisc::gui_active) return;
  if (load_dlg == NULL) return;
  load_dlg->setProgress(stepNum);
  RunPending();
}
void taiMisc::RemoveLoadDialog(){
  if (!taMisc::taMisc::gui_active) return;
  if (load_dlg == NULL) return;
  load_dlg->close();
  RunPending();
  //note: deletes on close, then calls back and clears load_dlg member
} */


/*TEMP void taiMisc::DeleteWindows() {
 int i;
  for(i=delete_wins.size-1; i>=0; i--) {
    delete delete_wins.FastEl(i);
  }
  delete_wins.RemoveAll();
}*/

void taiMisc::Cleanup(int) {
#ifndef __GNUG__
  String cmd = "/bin/rm ";
  cmd += taPlatform::getTempPath() + "/tai_gf." + String((int)taPlatform::processId()) + ".* >/dev/null 2>&1";
  system(cmd);			// get rid of any remaining temp files
#endif
}


String taiMisc::color_to_string(const iColor& color) {
  String result = "#" + String(color.red(), "%02x")
      + String(color.green(), "%02x") + String(color.blue(), "%02x");
  return result;
}

iColor taiMisc::ivBrightness_to_Qt_lightdark(const QColor& qtColor, float ivBrightness) {
// applies a legacy IV brightness factor to a Qt Color
  //TODO: maybe entire thing can be simplified to just using .light  and ivBrightness
  int qFactor;
  if (ivBrightness >= 0) {
    qFactor = 100 + (int)(100.0 * ivBrightness);
    return iColor(qtColor.light(qFactor));
  } else { // ivBrightness < 0
    qFactor = (int)(100.0 / (1 + ivBrightness));
    return iColor(qtColor.dark(qFactor));
  }
}


bool taiMisc::KeyEventCtrlPressed(QKeyEvent* e) {
  bool ctrl_pressed = false;
  if(e->modifiers() & Qt::ControlModifier)
    ctrl_pressed = true;
#ifdef TA_OS_MAC
  // Command + V should NOT be registered as ctrl_pressed on a mac -- that is paste..
  if(ctrl_pressed & e->key() == Qt::Key_V) ctrl_pressed = false;
  // actual ctrl = meta on apple -- enable this
  if(e->modifiers() & Qt::MetaModifier)
    ctrl_pressed = true;
#endif
  return ctrl_pressed;
}

bool taiMisc::KeyEventFilterEmacs_Nav(QObject* obj, QKeyEvent* e) {
  bool ctrl_pressed = KeyEventCtrlPressed(e);
  if(!ctrl_pressed) return false;
  QCoreApplication* app = QCoreApplication::instance();
  switch(e->key()) {
  case Qt::Key_P:
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));
    return true;		// we absorb this event
  case Qt::Key_N:
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
    return true;		// we absorb this event
  case Qt::Key_A:
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::ControlModifier));
    return true;		// we absorb this event
  case Qt::Key_E:
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ControlModifier));
    return true;		// we absorb this event
  case Qt::Key_F:
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier));
    return true;		// we absorb this event
  case Qt::Key_B:
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
    return true;		// we absorb this event
  case Qt::Key_U:
  case Qt::Key_Up:		// translate ctrl+up to page up
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier));
    return true;		// we absorb this event
  case Qt::Key_Down:		// translate ctrl+down to page down
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
    return true;
  case Qt::Key_V:
    if(taMisc::emacs_mode) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
      return true;		// we absorb this event
    }
    else {
      return false;		// pass it on..
    }
  }
  return false;
}

bool taiMisc::KeyEventFilterEmacs_Edit(QObject* obj, QKeyEvent* e) {
  if(KeyEventFilterEmacs_Nav(obj, e))
    return true;
  bool ctrl_pressed = KeyEventCtrlPressed(e);
  QCoreApplication* app = QCoreApplication::instance();
  if(ctrl_pressed) {
    switch(e->key()) {
    case Qt::Key_D:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier));
      return true;		// we absorb this event
    case Qt::Key_H:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier));
      return true;		// we absorb this event
    case Qt::Key_K:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Clear, Qt::NoModifier));
      return true;		// we absorb this event
    case Qt::Key_Y:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
      return true;		// we absorb this event
    case Qt::Key_W:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier));
      return true;		// we absorb this event
    case Qt::Key_Slash:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
      return true;		// we absorb this event
    case Qt::Key_Minus:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
      return true;		// we absorb this event
    }
  }
  if(e->modifiers() & Qt::AltModifier && e->key() == Qt::Key_W) { // copy
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier));
    return true;		// we absorb this event
  }
  return false;
}

bool taiMisc::KeyEventFilterEmacs_Clip(QObject* obj, QKeyEvent* e) {
  bool ctrl_pressed = KeyEventCtrlPressed(e);
  QCoreApplication* app = QCoreApplication::instance();
  if(ctrl_pressed) {
    switch(e->key()) {
    case Qt::Key_Y:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
      return true;		// we absorb this event
    case Qt::Key_W:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier));
      return true;		// we absorb this event
    case Qt::Key_Slash:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
      return true;		// we absorb this event
    case Qt::Key_Minus:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
      return true;		// we absorb this event
    }
  }
  if(e->modifiers() & Qt::AltModifier && e->key() == Qt::Key_W) { // copy
    app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier));
    return true;		// we absorb this event
  }
  return false;
}

void taiMisc::ScrollTo_SA(QAbstractScrollArea* sa, int scr_pos) {
  sa->verticalScrollBar()->setValue(scr_pos);
}

void taiMisc::CenterOn_SA(QAbstractScrollArea* sa, QWidget* sa_main_widg, QWidget* widg) {
  int w_ht = widg->height();
  int top_in_vc = MapToAreaV_SA(sa, sa_main_widg, widg, 0);
  int ctr_pos = top_in_vc + w_ht / 2;
  int vpt_ht = sa->viewport()->height();
  ScrollTo_SA(sa, ctr_pos);
}

void taiMisc::KeepInView_SA(QAbstractScrollArea* sa, QWidget* sa_main_widg, QWidget* widg) {
  int w_ht = widg->height();
  int top_in_vc = MapToAreaV_SA(sa, sa_main_widg, widg, 0);
  int ctr_pos = top_in_vc + w_ht / 2;
  int bot_pos = top_in_vc + w_ht;
  
  if(PosInView_SA(sa, top_in_vc) && PosInView_SA(sa, bot_pos))
    return;			// already in view

  int vpt_ht = sa->viewport()->height();
  int scpos = sa->verticalScrollBar()->value();
  int scbot = scpos + sa->viewport()->height();
  if(ctr_pos < scpos) {		// closer to top
    int nwtop = MAX(top_in_vc-12,0);
    ScrollTo_SA(sa, nwtop); // scroll up to top, plus a bit of margin
    taMisc::Info("Keep top:", String(nwtop));
  }
  else {
    int nwbot = MIN(bot_pos+12,vpt_ht);
    int nwtop = MAX(nwbot - vpt_ht, 0);
    ScrollTo_SA(sa, nwtop); // scroll down to bottom plus some margin
    taMisc::Info("Keep bot:", String(nwbot));
  }
}

bool taiMisc::PosInView_SA(QAbstractScrollArea* sa, int scr_pos) {
  int scpos = sa->verticalScrollBar()->value();
  int scbot = scpos + sa->viewport()->height();
  if(scr_pos <= scpos && scr_pos >= scbot) return true;
  return false;
}

QPoint taiMisc::MapToArea_SA(QAbstractScrollArea* sa, QWidget* sa_main_widg, QWidget* widg, const QPoint& pt) {
  return widg->mapTo(sa_main_widg, pt);
}

int taiMisc::MapToAreaV_SA(QAbstractScrollArea* sa, QWidget* sa_main_widg, QWidget* widg, int pt_y) {
  QPoint pt(0, pt_y);
  QPoint rv = MapToArea_SA(sa, sa_main_widg, widg, pt);
  return rv.y();
}

