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

#include "taiMisc.h"
#include <iNetworkAccessManager>
#include <iCookieJar>
#include <iDialogTextEdit>
#include <iDialogChoice>
#include <iWidget_List>
#include <iPanelOfEditor>
#include <TopLevelViewer>
#include <iMainWindowViewer>
#include <AbstractScriptBase>
#include <KeyBindings>
#include <KeyBindings_List>
#include <KeyActionPair_PArray>

#include <taMisc>

#include <css_qt.h>
#include <css_qtconsole.h>

#include <QBitmap>
#include <QCursor>
#include <QDesktopWidget>
#include <QApplication>
#include <QWebSettings>
#include <QPushButton>
#include <QMenuBar>
#include <QScrollBar>
#include <QDebug>


#define record_cursor_width 16
#define record_cursor_height 16
#define record_cursor_x_hot 1
#define record_cursor_y_hot 1
static uchar record_cursor_bits[] = {
   0x03, 0x00, 0x1f, 0x00, 0x7e, 0x00, 0xfe, 0x01, 0xfe, 0x00, 0x7c, 0x00,
   0xfc, 0x00, 0xd8, 0x01, 0x88, 0x03, 0x00, 0x00, 0xb8, 0x3b, 0xa8, 0x08,
   0xb8, 0x09, 0xa8, 0x08, 0xa8, 0x3b, 0x00, 0x00};

static uchar record_mask_bits[] = {
   0x03, 0x00, 0x1f, 0x00, 0x7e, 0x00, 0xfe, 0x01, 0xfe, 0x00, 0x7c, 0x00,
   0xfc, 0x00, 0xd8, 0x01, 0x88, 0x03, 0x00, 0x00, 0xb8, 0x3b, 0xa8, 0x08,
   0xb8, 0x09, 0xa8, 0x08, 0xa8, 0x3b, 0x00, 0x00};


// Wait Cursor, 16x16

#define wait_cursor_width 16
#define wait_cursor_height 16
#define wait_cursor_x_hot 7
#define wait_cursor_y_hot 8
static uchar wait_cursor_bits[] = {
   0xfc, 0x3f, 0x00, 0x00, 0xfc, 0x3f, 0x08, 0x10, 0xd0, 0x0b, 0xe0, 0x07,
   0xc0, 0x03, 0x80, 0x01, 0x80, 0x01, 0x40, 0x02, 0x20, 0x05, 0xd0, 0x0b,
   0xf8, 0x1f, 0xfc, 0x3f, 0x00, 0x00, 0xfc, 0x3f};

static uchar wait_mask_bits[] = {
   0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfc, 0x3f, 0xfc, 0x3f, 0xf8, 0x1f,
   0xf8, 0x1f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf8, 0x1f, 0xf8, 0x1f, 0xfc, 0x3f,
   0xfc, 0x3f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f};


TA_API taiMisc* taiM_ = NULL;

const int taiMisc::FONT_MED = 1;
const int taiMisc::FONT_SM = 2;

const String taiMisc::DEFAULT_PROJ_SPLITTERS = "AAAA/wAAAAEAAAADAAABAAAAAfwAAAKMAQAAAAcBAAAAAQE=";

taiDialogEditor_List      taiMisc::active_dialogs;
taiEditorOfClass_List    taiMisc::active_edits;
taiEditorOfClass_List    taiMisc::css_active_edits;
iTopLevelWindow_List    taiMisc::active_wins;
TypeSpace               taiMisc::arg_types;
QPointer<iMainWindowViewer> taiMisc::main_window;
taBase_PtrList          taiMisc::unopened_windows;
void (*taiMisc::Update_Hook)(taBase*) = NULL;
iNetworkAccessManager*  taiMisc::net_access_mgr = NULL;

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

  // QBitmap waiter = QBitmap::fromData(QSize(wait_cursor_width, wait_cursor_height),
  //   wait_cursor_bits, QImage::Format_MonoLSB);
  // QBitmap waiter_m = QBitmap::fromData(QSize(wait_cursor_width, wait_cursor_height),
  //   wait_mask_bits, QImage::Format_MonoLSB);
  // wait_cursor = new QCursor(waiter, waiter_m, wait_cursor_x_hot, wait_cursor_y_hot);
  wait_cursor = new QCursor(Qt::BusyCursor);

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
    ++busy_count;       // keep track of number of times called
    if (busy_count == 1)
      SetWinCursors();
  }
  else {
    if(--busy_count == 0) {
      RestoreWinCursors(); //added 4.0
    }
    if (busy_count < 0) {
      taMisc::Warning("taiMisc::DoneBusy() called more times than Busy()");
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
    iDialogChoice::ConfirmDialog(QApplication::activeWindow(),
      "No configuration errors were found.",
      "Check Succeeded", false);
  } else {
    iDialogTextEdit* td = new iDialogTextEdit(true);
    td->setWindowTitle("Check Failed");
    td->setText(taMisc::last_check_msg);
    td->exec();
    td->deleteLater();
  }
}

void taiMisc::GetWindowList(iWidget_List& rval) {
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
  if (scrn_s.height() <= 768)   // jr 07/11/13 - base_height only used in T3 UnitNode
    base_height = 21;
  else if (scrn_s.height() <= 1024)
    base_height = 25;
  else
    base_height = 29;

  // control sizes -- depend on size of default font
  if (taMisc::font_size <= 10) {
    // Small
//    mbutton_ht[0] = 21; mbutton_ht[1] = 23; mbutton_ht[2] = 24;
    mbutton_ht[0] = 20; mbutton_ht[1] = 22; mbutton_ht[2] = 23;
    mlabel_ht[0] = 17; mlabel_ht[1] = 19; mlabel_ht[2] = 20;
    mtext_ht[0] = 19; mtext_ht[1] = 21; mtext_ht[2] = 22;
    currentSizeSpec = sizSmall;
  } else if (taMisc::font_size > 13) {
    // Big
//    mbutton_ht[0] = 27; mbutton_ht[1] = 30; mbutton_ht[2] = 33;
    mbutton_ht[0] = 24; mbutton_ht[1] = 27; mbutton_ht[2] = 30;
    mlabel_ht[0] = 24; mlabel_ht[1] = 26; mlabel_ht[2] = 29;
    mtext_ht[0] = 26; mtext_ht[1] = 28; mtext_ht[2] = 31;
    currentSizeSpec = sizBig;
  }  else {
    // Med
//    mbutton_ht[0] = 24;  mbutton_ht[1] = 25;  mbutton_ht[2] = 27;
    mbutton_ht[0] = 22;  mbutton_ht[1] = 23;  mbutton_ht[2] = 25;
    mlabel_ht[0] = 20;  mlabel_ht[1] = 21;  mlabel_ht[2] = 24;
    mtext_ht[0] = 22;  mtext_ht[1] = 23;  mtext_ht[2] = 26;
    currentSizeSpec = sizMedium;
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

  QFontMetrics fmn(mbig_name_font);
  int avgw = fmn.averageCharWidth();
  int avgh = fmn.height();
  scrn_chars.w = scrn_s.w / avgw;
  scrn_chars.h = scrn_s.h / avgh;
}

/* Qt Metrics note

On Linux/X11, using Sans font (default)

Default Widget sizes for various default font sizes --
() indicates a usable size

                        Sans-9          Sans-10         Sans-11
QPushButton             31 (24)         32 (25)         35 (27)
QLabel                  20              20              20
QLineEdit               22              23              26
  QSpinBox              22              23
QComboBox               27 (24)         27 (25)         27
QRadioButton            20              21              24
  QCheckBox             20


Actual Widget sizes for SizeSpec sizes --
 indented names means ctrl assumes size of outdented control above it

                        sizSmall        sizMed          sizBig
QPushButton             24              25              27
  QComboBox
QLabel                  20              21              24
  QCheckBox
QLineEdit               22              23              26
  QSpinBox

*/

taiMisc::SizeSpec taiMisc::GetCurrentSizeSpec() {
  return currentSizeSpec;
}

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
//  but->setToolTip(taiMisc::ToolTipPreProcess(text));
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
  // This line is commented out because it causes problems on the
  // iViewPanelOfNetwork dialog (two-line text labels get clipped).
  //rval->setFixedHeight(label_height(fontSpec));
  rval->setFont(nameFont(fontSpec));
  return rval;
}

QString taiMisc::ToolTipPreProcess(const String &tip_str) {
  // make the string rich text so Qt will wrap it
  String rich_str = String("<span>" + tip_str + "</span>");
  return rich_str.toQString();
}

void taiMisc::OnQuitting_impl(CancelOp& cancel_op) {
// called when quitting -- does all saves
  if(!taMisc::interactive) return;
  ResolveEditChanges(cancel_op);
  if (cancel_op == CO_CANCEL) return;
  ResolveViewerChanges(cancel_op);
}

void taiMisc::Quit_impl(CancelOp cancel_op) {
  if(taMisc::gui_no_win) {
    QCoreApplication::instance()->quit();
  }
  else {
    // Notify the CSS console that the application is quitting,
    // otherwise it may remain blocked waiting for user input.
    if (QcssConsole *qcons = QcssConsole::getInstance()) {
      qcons->onQuit();
    }
    
    qApp->closeAllWindows();
    qApp->quit();               // the cssConsole does not close -- just quit!
  }
}

void taiMisc::ResolveEditChanges(CancelOp& cancel_op) {
  for (int i = 0; i < taiMisc::active_edits.size; ++i) {
    taiEditor* edh = taiMisc::active_edits.FastEl(i);
    if (!edh || (edh->state != taiEditorOfClass::ACTIVE)) continue;
    edh->ResolveChanges(cancel_op); // don't need 'discard'
    if (cancel_op == CO_CANCEL) return;
  }
}

void taiMisc::ResolveViewerChanges(CancelOp& cancel_op) {
  for (int i = 0; i < taiMisc::active_wins.size; ++i) {
    IViewerWidget* dvw = taiMisc::active_wins.FastEl(i);
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
    taiEditorWidgetsMain* dlg = active_dialogs.FastEl(i);
    if ((dlg->state & taiEditorWidgetsMain::STATE_MASK) >= taiEditorWidgetsMain::ACCEPTED) {
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
#ifdef RESTORE_PREV_ACTIVE_WINDOW
  QPointer<QWidget> m_prev_active = QApplication::activeWindow();
#endif

  bool is_busy = false;
  bool is_rec = false;
  if((taiMisc::busy_count > 0) /*|| cssiSession::block_in_event*/)
    is_busy = true;
  if (taMisc::record_on)
    is_rec = true;
  // busy trumps recording...
  if (is_busy) {
    QApplication::setOverrideCursor(*taiM->wait_cursor);
#ifdef RESTORE_PREV_ACTIVE_WINDOW
    goto bail;
#else
    return;
#endif
  }

  if (is_rec) {
    QApplication::setOverrideCursor(*taiM->record_cursor);
#ifdef RESTORE_PREV_ACTIVE_WINDOW
    goto bail;
#else
    return;
#endif
  }

  taMisc::Warning("*** Unexpected call to SetWinCursors -- not busy or recording.");

#ifdef RESTORE_PREV_ACTIVE_WINDOW
 bail:
  if((bool)m_prev_active) {
//     QApplication::setActiveWindow(m_prev_active);
    // note: above does NOT work! -- likely source of bug in cocoa 4.6.0
    m_prev_active->activateWindow();
  }
#endif
}

/*bool taiMisc::RevertEdits(void* obj, TypeDef*) {
  if (!taMisc::gui_active)    return false;
  bool got_one = false;
  for (int i = active_edits.size-1; i >= 0; --i) {
    taiEditorOfClass* dlg = active_edits.FastEl(i);
    if((dlg->root == obj) && (dlg->state == taiEditorWidgetsMain::ACTIVE)) {
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
    taiEditorOfClass* edh = active_edits.FastEl(i);
    if((edh->root == obj) && (edh->state == taiEditorWidgetsMain::ACTIVE)) {
      edh->ReShow_Async(force);
      got_one = true;
    }
  }
  return got_one;
}*/

taiEditorOfClass* taiMisc::FindEdit(void* obj, iMainWindowViewer* not_in_win) {
  //NOTE: not_in_win works as follows:
  // NULL: ok to return any edit (typically used to get show value)
  // !NULL: must get other win that not; used to raise that edit panel to top, so
  //  shouldn't hide the edit panel that invoked the operation
  if (!taMisc::gui_active) return NULL;
  for (int i = active_edits.size - 1; i >= 0; --i) {
    taiEditorOfClass* host = active_edits.FastEl(i);
    if ((host->root != obj) || (host->state != taiEditorWidgetsMain::ACTIVE))
      continue;
    if (host->isDialog() && !host->modal)
      return host;
    // is a EditPanel
    if (host->isPanel() && (host->dataPanel()->window() != not_in_win)) // if niw NULL, then will always be true
      return host;
  }
  return NULL;
}

taiEditorOfClass* taiMisc::FindEditDialog(void* obj, bool read_only_state) {
  if (!taMisc::gui_active) return NULL;
  for (int i = active_edits.size - 1; i >= 0; --i) {
    taiEditorOfClass* host = active_edits.FastEl(i);
    if ((host->root != obj) || (host->state != taiEditorWidgetsMain::ACTIVE))
      continue;
    if (host->isDialog() && !host->modal && (host->read_only == read_only_state))
      return host;
  }
  return NULL;
}

taiEditorOfClass* taiMisc::FindEditPanel(void* obj, bool read_only,
  iMainWindowViewer* not_in_win)
{
  //NOTE: not_in_win works as follows:
  // NULL: ok to return any edit (typically used to get show value)
  // !NULL: must get other win that not; used to raise that edit panel to top, so
  //  shouldn't hide the edit panel that invoked the operation
  if (!taMisc::gui_active) return NULL;
  for (int i = active_edits.size - 1; i >= 0; --i) {
    taiEditorOfClass* host = active_edits.FastEl(i);
    if ((host->root != obj) || (host->state != taiEditorWidgetsMain::ACTIVE))
      continue;
    if (host->isPanel() && (host->read_only == read_only) &&
      (host->dataPanel()->window() != not_in_win)) // if niw NULL, then will always be true
      return host;
  }
  return NULL;
}

void taiMisc::OpenWindows(){
  if(unopened_windows.size == 0)        return;
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
//  taViewer* wb = (taViewer *) obj;
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
  cmd += taMisc::GetTemporaryPath() + "/tai_gf." + String((int)taMisc::ProcessId()) + ".* >/dev/null 2>&1";
  system(cmd);                  // get rid of any remaining temp files
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

iMainWindowViewer* taiMisc::FindMainWinParent(QObject* obj) {
  QObject* tobj = obj;
  while(tobj) {
    if(tobj->inherits("iMainWindowViewer")) {
      return qobject_cast<iMainWindowViewer*>(tobj);
    }
    tobj = tobj->parent();
  }
  return NULL;
}


bool taiMisc::UpdateUiOnCtrlPressed(QObject* obj, QKeyEvent* e) {
  iMainWindowViewer* imwv = FindMainWinParent(obj);
  if(!imwv) {
    // taMisc::DebugInfo("no imwv!");
    return false;
  }

  bool ctrl_pressed = false;
#ifndef TA_OS_MAC
  // trying all possible ways to look for ctrl pressed, b/c sometimes they don't work!
  if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
    ctrl_pressed = true;
  }
  else if (e->modifiers() & Qt::ControlModifier) {
    ctrl_pressed = true;
  }
#if (QT_VERSION >= 0x040800)
  else if (QApplication::queryKeyboardModifiers() & Qt::ControlModifier) {
    ctrl_pressed = true;
  }
#endif
#endif
  
#ifdef TA_OS_MAC
  // actual ctrl = meta on apple -- enable this
  if (QApplication::keyboardModifiers() & Qt::MetaModifier) {
    ctrl_pressed = true;
  }
  else if (e->modifiers() & Qt::MetaModifier) {
    ctrl_pressed = true;
  }
#if (QT_VERSION >= 0x040800)
  else if (QApplication::queryKeyboardModifiers() & Qt::MetaModifier) {
    ctrl_pressed = true;
  }
#endif
#endif

  if(ctrl_pressed) {
    imwv->UpdateUi();
    // taMisc::DebugInfo("ui");
  }
  return ctrl_pressed;
}

bool taiMisc::KeyEventCtrlPressed(QKeyEvent* e) {
  bool ctrl_pressed = false;

#ifdef TA_OS_MAC
  // On mac the control key sends Meta
  if (e->modifiers() & Qt::MetaModifier) {
    ctrl_pressed = true;
  }
 #else
  if (e->modifiers() & Qt::ControlModifier) { // regular modifiers() is safer for generated events..
    ctrl_pressed = true;
  }
#endif

  return ctrl_pressed;
}

taiMisc::BoundAction taiMisc::GetActionFromKeyEvent(taiMisc::BindingContext context, QKeyEvent* key_event) {
  int key_int = key_event->key();
  
  Qt::KeyboardModifiers modifiers = key_event->modifiers();
  if(modifiers & Qt::ShiftModifier)
    key_int += Qt::SHIFT;
  if(modifiers & Qt::ControlModifier)
    key_int += Qt::CTRL;
  if(modifiers & Qt::AltModifier)
    key_int += Qt::ALT;
  if(modifiers & Qt::MetaModifier)
    key_int += Qt::META;
  
  KeyBindings* bindings = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::current_key_bindings));
  return bindings->Action(context, QKeySequence(key_int));
}

QKeySequence taiMisc::GetSequenceFromAction(taiMisc::BindingContext context, taiMisc::BoundAction action) {
  KeyBindings* bindings = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::current_key_bindings));
  return bindings->KeySequence(context, action);
}

String taiMisc::GetSequenceFromActionFriendly(taiMisc::BindingContext context, taiMisc::BoundAction action) {
  String friendly_str;
  KeyBindings* bindings = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::current_key_bindings));
  friendly_str = bindings->KeySequence(context, action).toString();
  
#ifdef TA_OS_MAC
  if (friendly_str.contains("Ctrl")) {
    friendly_str = friendly_str.repl("Ctrl", "command");
  }
  else if (friendly_str.contains("Meta")) {
    friendly_str = friendly_str.repl("Meta", "control");
  }
  else if (friendly_str.contains("Alt")) {
    if (context == taiMisc::TEXTEDIT_CONTEXT) {
      if (action == taiMisc::TEXTEDIT_COPY_CLEAR) {
        friendly_str = friendly_str.repl("?", "W");
      }
      if (action == taiMisc::TEXTEDIT_WORD_FORWARD) {
        friendly_str = friendly_str.repl("?", "F");
      }
      if (action == taiMisc::TEXTEDIT_WORD_BACKWARD) {
        friendly_str = friendly_str.repl("?", "B");
      }
    }
    if (context == taiMisc::PROJECTWINDOW_CONTEXT) {
      if (action == taiMisc::PROJECTWINDOW_FRAME_LEFT_II) {
        friendly_str = friendly_str.repl("?", "J");
      }
      if (action == taiMisc::PROJECTWINDOW_FRAME_RIGHT_II) {
        friendly_str = friendly_str.repl("\xac", "L");
      }
    }
    if (context == taiMisc::TREE_CONTEXT) {
      if (action == taiMisc::TREE_FIND) {
        friendly_str = friendly_str.repl("?", "F");
      }
      if (action == taiMisc::taiMisc::TREE_FIND_REPLACE) {
        friendly_str = friendly_str.repl("?", "R");
      }
    }
  }
#endif
  return friendly_str;
}

void taiMisc::ScrollTo_SA(QAbstractScrollArea* sa, int scr_pos) {
  sa->verticalScrollBar()->setValue(scr_pos);
}

void taiMisc::CenterOn_SA(QAbstractScrollArea* sa, QWidget* sa_main_widg, QWidget* widg) {
  int w_ht = widg->height();
  int top_in_vc = MapToAreaV_SA(sa, sa_main_widg, widg, 0);
  int ctr_pos = top_in_vc + w_ht / 2;
//  int vpt_ht = sa->viewport()->height();
  ScrollTo_SA(sa, ctr_pos);
}

void taiMisc::KeepInView_SA(QAbstractScrollArea* sa, QWidget* sa_main_widg, QWidget* widg) {
  int w_ht = widg->height();
  int top_in_vc = MapToAreaV_SA(sa, sa_main_widg, widg, 0);
  int ctr_pos = top_in_vc + w_ht / 2;
  int bot_pos = top_in_vc + w_ht;

  if(PosInView_SA(sa, top_in_vc) && PosInView_SA(sa, bot_pos))
    return;                     // already in view

  int vpt_ht = sa->viewport()->height();
  int scpos = sa->verticalScrollBar()->value();
//  int scbot = scpos + sa->viewport()->height();
  if(ctr_pos < scpos) {         // closer to top
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

void taiMisc::DeleteWidgetsLater(QObject* obj) {
  if (obj == NULL) return;
  QObject* chobj;
  const QObjectList& ol = obj->children();
  for(int i = ol.count()-1; i >= 0; i--) {
    chobj = ol.at(i);
    if(chobj->inherits("QWidget")) {
      ((QWidget*)chobj)->hide();
    }
    chobj->deleteLater();
  }
}

void taiMisc::DeleteChildrenLater(QObject* obj) {
  if (obj == NULL) return;
  QObject* chobj;
  const QObjectList& ol = obj->children();
  for(int i = ol.count()-1; i >= 0; i--) {
    chobj = ol.at(i);
    chobj->deleteLater();
  }
}

void taiMisc::DeleteChildrenNow(QObject* obj) {
  if (obj == NULL) return;
  QObject* chobj;
  const QObjectList& ol = obj->children();
  int i = 0;
  while (ol.count() > 0) {
    i = ol.size() - 1;
    chobj = ol.at(i);
    delete chobj;
  }
}

void taiMisc::LoadDefaultKeyBindings() {
#ifdef TA_OS_MAC
  int control_key = static_cast<int>(Qt::MetaModifier);
  int meta_key = static_cast<int>(Qt::ControlModifier);  // "command" key
#else
  int control_key = static_cast<int>(Qt::ControlModifier);
  int meta_key = static_cast<int>(Qt::MetaModifier);
#endif
  
  KeyBindings* default_list = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::KEY_BINDINGS_DEFAULT));
  
  default_list->Reset();
// menu items for mac only (for now anyway)
#ifdef TA_OS_MAC
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_NEW, QKeySequence(meta_key + Qt::Key_N));
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_OPEN, QKeySequence(meta_key + Qt::Key_O));
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_CLOSE, QKeySequence(meta_key + Qt::Key_W));
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_FIND, QKeySequence(meta_key + Qt::Key_F));
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_FIND_NEXT, QKeySequence(meta_key + Qt::Key_G));
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_MINIMIZE, QKeySequence(meta_key + Qt::Key_M));
#endif
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_DELETE, QKeySequence(control_key + Qt::Key_D));
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_DUPLICATE, QKeySequence(control_key + Qt::Key_M));
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_FIND_NEXT, QKeySequence("F3"));
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_STOP, QKeySequence("F8"));
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_CONTINUE, QKeySequence("F9"));
  default_list->Add(taiMisc::MENU_CONTEXT, taiMisc::MENU_STEP, QKeySequence("F10"));

#ifdef TA_OS_MAC
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_NAVIGATOR_ONLY, QKeySequence(meta_key + Qt::Key_1));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_EDITOR_ONLY, QKeySequence(meta_key + Qt::Key_2));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_NAVIGATOR_AND_EDITOR, QKeySequence(meta_key + Qt::Key_3));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_VISUALIZER_ONLY, QKeySequence(meta_key + Qt::Key_4));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_NAVIGATOR_AND_VISUALIZER, QKeySequence(meta_key + Qt::Key_5));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_EDITOR_AND_VISUALIZER, QKeySequence(meta_key + Qt::Key_6));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_ALL_FRAMES, QKeySequence(meta_key + Qt::Key_7));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_RESET_FRAMES, QKeySequence(meta_key + Qt::Key_8), "reset frames to default widths");
#else
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_NAVIGATOR_ONLY, QKeySequence(control_key + Qt::Key_1));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_EDITOR_ONLY, QKeySequence(control_key + Qt::Key_2));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_NAVIGATOR_AND_EDITOR, QKeySequence(control_key + Qt::Key_3));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_VISUALIZER_ONLY, QKeySequence(control_key + Qt::Key_4));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_NAVIGATOR_AND_VISUALIZER, QKeySequence(control_key + Qt::Key_5));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_EDITOR_AND_VISUALIZER, QKeySequence(control_key + Qt::Key_6));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_ALL_FRAMES, QKeySequence(control_key + Qt::Key_7));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_RESET_FRAMES, QKeySequence(control_key + Qt::Key_8), "reset frames to default widths");
#endif
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_FRAME_LEFT, QKeySequence(control_key + Qt::Key_J));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_FRAME_RIGHT, QKeySequence(control_key + Qt::Key_L));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_SHIFT_TAB_LEFT, QKeySequence(control_key + Qt::Key_B));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_SHIFT_TAB_LEFT_II, QKeySequence());
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_SHIFT_TAB_RIGHT, QKeySequence(control_key + Qt::Key_F));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_SHIFT_TAB_RIGHT_II, QKeySequence());
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_PANEL_VIEW_LEFT, QKeySequence(Qt::ShiftModifier + control_key + Qt::Key_B));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_PANEL_VIEW_LEFT_II, QKeySequence());
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_PANEL_VIEW_RIGHT, QKeySequence(Qt::ShiftModifier + control_key + Qt::Key_F));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_PANEL_VIEW_RIGHT_II, QKeySequence());
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_DELETE, QKeySequence(control_key + Qt::Key_D));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_DELETE_II, QKeySequence());
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_TOGGLE_PANEL_PIN, QKeySequence(control_key + Qt::Key_P));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_TOGGLE_PANEL_PIN_II, QKeySequence());
  
#ifdef TA_OS_MAC  // why both control and alt for shifting focus
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_FRAME_LEFT_II, QKeySequence(Qt::AltModifier + 0x2206));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_FRAME_RIGHT_II, QKeySequence(Qt::AltModifier + 0xAC));
#else
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_FRAME_LEFT_II, QKeySequence(Qt::AltModifier + Qt::Key_J));
  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_FRAME_RIGHT_II, QKeySequence(Qt::AltModifier + Qt::Key_L));
#endif  
  
//  default_list->Add(taiMisc::PROJECTWINDOW_CONTEXT, taiMisc::PROJECTWINDOW_XYZ, QKeySequence(Qt::Key_Question));

  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_STOP, QKeySequence(meta_key + Qt::Key_C));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_STOP_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_UNDO, QKeySequence(control_key + Qt::Key_C));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_UNDO_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CLEAR, QKeySequence(control_key + Qt::Key_Period));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CLEAR_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_BACKSPACE, QKeySequence(Qt::Key_Backspace));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_BACKSPACE_II, QKeySequence(control_key + Qt::Key_H));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_QUIT_PAGING, QKeySequence(Qt::Key_Q));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_QUIT_PAGING_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CONTINUE_PAGING, QKeySequence(Qt::Key_C));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CONTINUE_PAGING_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_HISTORY_BACKWARD, QKeySequence(Qt::Key_Up));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_HISTORY_BACKWARD_II, QKeySequence(control_key + Qt::Key_P));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_HISTORY_FORWARD, QKeySequence(Qt::Key_Down));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_HISTORY_FORWARD_II, QKeySequence(control_key + Qt::Key_N));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_HOME, QKeySequence(control_key + Qt::Key_A));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_HOME_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_END, QKeySequence(control_key + Qt::Key_E));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_END_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CURSOR_FORWARD, QKeySequence(control_key + Qt::Key_F));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CURSOR_FORWARD_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CURSOR_BACKWARD, QKeySequence(control_key + Qt::Key_B));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CURSOR_BACKWARD_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_DESELECT, QKeySequence(control_key + Qt::Key_Space));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_DESELECT_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CLEAR_SELECTION, QKeySequence(control_key + Qt::Key_G));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CLEAR_SELECTION_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_DELETE, QKeySequence(control_key + Qt::Key_D));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_DELETE_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_KILL, QKeySequence(control_key + Qt::Key_K));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_KILL_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_PASTE, QKeySequence(control_key + Qt::Key_Y));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_PASTE_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CUT, QKeySequence(control_key + Qt::Key_W));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_CUT_II, QKeySequence());
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_AUTO_COMPLETE, QKeySequence(Qt::Key_Tab));
  default_list->Add(taiMisc::CONSOLE_CONTEXT, taiMisc::CONSOLE_AUTO_COMPLETE_II, QKeySequence());
  
  
  
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_TOGGLE_FOCUS, QKeySequence(control_key + Qt::Key_T));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_TOGGLE_FOCUS_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_EDIT_HOME, QKeySequence(control_key + Qt::Key_A));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_EDIT_HOME_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_EDIT_END, QKeySequence(control_key + Qt::Key_E));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_EDIT_END_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_DELETE, QKeySequence(control_key + Qt::Key_D));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_DELETE_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_SELECT, QKeySequence(control_key + Qt::Key_S));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_SELECT_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_CLEAR_SELECTION, QKeySequence(control_key + Qt::Key_G));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_CLEAR_SELECTION_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_MOVE_FOCUS_UP, QKeySequence(control_key + Qt::Key_P));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_MOVE_FOCUS_UP_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_MOVE_FOCUS_DOWN, QKeySequence(control_key + Qt::Key_N));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_MOVE_FOCUS_DOWN_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_MOVE_FOCUS_RIGHT, QKeySequence(control_key + Qt::Key_F));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_MOVE_FOCUS_RIGHT_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_MOVE_FOCUS_LEFT, QKeySequence(control_key + Qt::Key_B));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_MOVE_FOCUS_LEFT_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_INSERT, QKeySequence(control_key + Qt::Key_I));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_INSERT_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_INSERT_AFTER, QKeySequence(control_key + Qt::Key_O));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_INSERT_AFTER_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_DUPLICATE, QKeySequence(control_key + Qt::Key_M));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_DUPLICATE_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_DELETE_TO_END, QKeySequence(control_key + Qt::Key_K));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_DELETE_TO_END_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_PAGE_UP, QKeySequence(control_key + Qt::Key_U));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_PAGE_UP_II, QKeySequence());
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_PAGE_DOWN, QKeySequence(control_key + Qt::Key_V));
  default_list->Add(taiMisc::DATATABLE_CONTEXT, taiMisc::DATATABLE_PAGE_DOWN_II, QKeySequence());
  
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_NEW_DEFAULT_ELEMENT, QKeySequence(Qt::Key_Enter));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_NEW_DEFAULT_ELEMENT_II, QKeySequence(Qt::Key_Return));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_NEW_DEFAULT_ELEMENT_ABOVE, QKeySequence(Qt::ShiftModifier + Qt::Key_Enter));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_NEW_DEFAULT_ELEMENT_ABOVE_II, QKeySequence(Qt::ShiftModifier + Qt::Key_Return));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_NEW_ELEMENT_ABOVE, QKeySequence(control_key + Qt::Key_I));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_NEW_ELEMENT_ABOVE_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_NEW_ELEMENT_BELOW, QKeySequence(control_key + Qt::Key_O));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_NEW_ELEMENT_BELOW_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_CUT, QKeySequence(control_key + Qt::Key_W));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_CUT_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_PASTE, QKeySequence(control_key + Qt::Key_Y));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_PASTE_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_DUPLICATE, QKeySequence(control_key + Qt::Key_M));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_DUPLICATE_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_DELETE, QKeySequence(control_key + Qt::Key_D));
#ifdef TA_OS_MAC
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_DELETE_II, QKeySequence(meta_key + Qt::Key_Backspace));
#else
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_DELETE_II, QKeySequence(control_key + Qt::Key_Backspace));
#endif
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_START_EXTENDED_SELECTION, QKeySequence(control_key + Qt::Key_Space));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_START_EXTENDED_SELECTION_II, QKeySequence(control_key + Qt::Key_S));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_CLEAR_SELECTION, QKeySequence(control_key + Qt::Key_G));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_CLEAR_SELECTION_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_FORWARD, QKeySequence(control_key + Qt::Key_F));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_FORWARD_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_BACKWARD, QKeySequence(control_key + Qt::Key_B));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_BACKWARD_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_MOVE_SELECTION_UP, QKeySequence(control_key + Qt::Key_P));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_MOVE_SELECTION_UP_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_MOVE_SELECTION_DOWN, QKeySequence(control_key + Qt::Key_N));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_MOVE_SELECTION_DOWN_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_PAGE_UP, QKeySequence(control_key + Qt::Key_Up));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_PAGE_UP_II, QKeySequence(control_key + Qt::Key_U));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_PAGE_DOWN, QKeySequence(control_key + Qt::Key_Down));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_PAGE_DOWN_II, QKeySequence(control_key + Qt::Key_V));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_EDIT_HOME, QKeySequence(control_key + Qt::Key_A));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_EDIT_HOME_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_EDIT_END, QKeySequence(control_key + Qt::Key_E));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_EDIT_END_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_EDIT_DELETE_TO_END, QKeySequence(control_key + Qt::Key_K));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_EDIT_DELETE_TO_END_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_HISTORY_FORWARD, QKeySequence(control_key + Qt::Key_Right));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_HISTORY_FORWARD_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_HISTORY_BACKWARD, QKeySequence(control_key + Qt::Key_Left));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_HISTORY_BACKWARD_II, QKeySequence());
#ifdef TA_OS_MAC
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_FIND, QKeySequence(Qt::AltModifier + 0x191));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_FIND_REPLACE, QKeySequence(Qt::AltModifier + 0xAE));
#else
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_FIND, QKeySequence(Qt::AltModifier + Qt::Key_F));
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_FIND_REPLACE, QKeySequence(Qt::AltModifier + Qt::Key_R));
#endif
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_FIND_II, QKeySequence());
  default_list->Add(taiMisc::TREE_CONTEXT, taiMisc::TREE_FIND_REPLACE_II, QKeySequence());


  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_INTERACTION_MODE_OFF, QKeySequence(Qt::Key_V));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_INTERACTION_MODE_OFF_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_INTERACTION_MODE_ON, QKeySequence(Qt::Key_I));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_INTERACTION_MODE_ON_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_INTERACTION_MODE_TOGGLE, QKeySequence(Qt::Key_Escape));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_INTERACTION_MODE_TOGGLE_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_RESET_VIEW, QKeySequence(Qt::Key_Home));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_RESET_VIEW_II, QKeySequence(Qt::Key_H));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_VIEW_ALL, QKeySequence(Qt::Key_A));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_VIEW_ALL_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_SEEK, QKeySequence(Qt::Key_S));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_SEEK_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_PAN_LEFT, QKeySequence(Qt::ShiftModifier + Qt::Key_Left));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_PAN_LEFT_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_PAN_RIGHT, QKeySequence(Qt::ShiftModifier + Qt::Key_Right));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_PAN_RIGHT_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_PAN_UP, QKeySequence(Qt::ShiftModifier + Qt::Key_Up));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_PAN_UP_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_PAN_DOWN, QKeySequence(Qt::ShiftModifier + Qt::Key_Down));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_PAN_DOWN_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ROTATE_LEFT, QKeySequence(Qt::Key_Left));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ROTATE_LEFT_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ROTATE_RIGHT, QKeySequence(Qt::Key_Right));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ROTATE_RIGHT_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ROTATE_UP, QKeySequence(Qt::Key_Up));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ROTATE_UP_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ROTATE_DOWN, QKeySequence(Qt::Key_Down));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ROTATE_DOWN_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ZOOM_IN, QKeySequence(Qt::ShiftModifier + Qt::Key_Plus));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ZOOM_IN_II, QKeySequence(Qt::Key_Equal));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ZOOM_OUT, QKeySequence(Qt::Key_Minus));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_ZOOM_OUT_II, QKeySequence(Qt::ShiftModifier + Qt::Key_Underscore));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_VIEW_LEFT, QKeySequence(Qt::ShiftModifier + control_key + Qt::Key_B));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_VIEW_LEFT_II, QKeySequence());
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_VIEW_RIGHT, QKeySequence(Qt::ShiftModifier + control_key + Qt::Key_F));
  default_list->Add(taiMisc::GRAPHICS_CONTEXT, taiMisc::GRAPHICS_VIEW_RIGHT_II, QKeySequence());
  
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_LOOKUP, QKeySequence(control_key + Qt::Key_L), "displays a list of members/methods to complete the expression");
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_LOOKUP_II, QKeySequence(), "displays a list of members/methods to complete the expression");
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CURSOR_UP, QKeySequence(control_key + Qt::Key_P));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CURSOR_UP_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CURSOR_DOWN, QKeySequence(control_key + Qt::Key_N));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CURSOR_DOWN_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_START_SELECT, QKeySequence(control_key + Qt::Key_Space));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_START_SELECT_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CLEAR_SELECTION, QKeySequence(control_key + Qt::Key_G));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CLEAR_SELECTION_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_HOME, QKeySequence(control_key + Qt::Key_A));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_HOME_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_END, QKeySequence(control_key + Qt::Key_E));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_END_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CURSOR_FORWARD, QKeySequence(control_key + Qt::Key_F));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CURSOR_FORWARD_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CURSOR_BACKWARD, QKeySequence(control_key + Qt::Key_B));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CURSOR_BACKWARD_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_DELETE, QKeySequence(control_key + Qt::Key_D));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_DELETE_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_BACKSPACE, QKeySequence(control_key + Qt::Key_H));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_BACKSPACE_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_DELETE_TO_END, QKeySequence(control_key + Qt::Key_K));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_DELETE_TO_END_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_SELECT_ALL, QKeySequence(meta_key + Qt::Key_A));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_SELECT_ALL_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_PASTE, QKeySequence(control_key + Qt::Key_Y));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_PASTE_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CUT, QKeySequence(control_key + Qt::Key_W));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_CUT_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_UNDO, QKeySequence(control_key + Qt::Key_Minus));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_UNDO_II, QKeySequence(control_key + Qt::Key_Slash));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_FIND_IN_TEXT, QKeySequence(control_key + Qt::Key_S));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_FIND_IN_TEXT_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_PAGE_UP, QKeySequence(control_key + Qt::Key_U));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_PAGE_UP_II, QKeySequence());
#ifdef TA_OS_MAC
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_PAGE_DOWN, QKeySequence(control_key + Qt::Key_V));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_COPY_CLEAR, QKeySequence(Qt::AltModifier + 0x2211)); // W
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_WORD_FORWARD, QKeySequence(Qt::AltModifier + 0x191)); // F
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_WORD_BACKWARD, QKeySequence(Qt::AltModifier + 0x222B)); // B
#else
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_PAGE_DOWN, QKeySequence());  // leave empty - ctrl + v is paste on non-mac
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_COPY_CLEAR, QKeySequence(Qt::AltModifier + Qt::Key_W));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_WORD_FORWARD, QKeySequence(Qt::AltModifier + Qt::Key_F));
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_WORD_BACKWARD, QKeySequence(Qt::AltModifier + Qt::Key_B));
#endif
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_PAGE_DOWN_II, QKeySequence());  // leave empty - ctrl + v is paste on non-mac
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_COPY_CLEAR_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_WORD_FORWARD_II, QKeySequence());
  default_list->Add(taiMisc::TEXTEDIT_CONTEXT, taiMisc::TEXTEDIT_WORD_BACKWARD_II, QKeySequence());
}

void taiMisc::LoadCustomKeyBindings() {
  String filename = taMisc::GetCustomKeyFilename();
  QFile in_file(filename);
  if (!in_file.exists()) {
    DefaultCustomKeyBindings();  // create the default custom key bindings file (file in prefs dir)
  }
  if (!in_file.open(QIODevice::ReadOnly)) {
    taMisc::Info("Unable to load custom key bindings");
    return;
  }
  
  KeyBindings* bindings = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::KEY_BINDINGS_CUSTOM));
  bindings->Reset();
  
  QDataStream in(&in_file);
  QString context;
  QString action;
  QKeySequence ks;
  String enum_tp_nm;
  
  for (int i=0; i<taiMisc::ACTION_COUNT; i++) {
    in >> context >> action >> ks;
    int context_val = TA_taiMisc.GetEnumVal((String)context, enum_tp_nm);
    int action_val = TA_taiMisc.GetEnumVal((String)action, enum_tp_nm);
    bindings->Add(static_cast<taiMisc::BindingContext>(context_val), static_cast<taiMisc::BoundAction>(action_val), ks);
  }
}

void taiMisc::DefaultCustomKeyBindings() {
  String filename = taMisc::GetCustomKeyFilename();
  QFile file(filename);
  if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
    return;
  }
  QDataStream out(&file);
  
  // set the binding set to the default to get the default bindings - that is the starting point for custom bindings
  taMisc::KeyBindingSet current_set = taMisc::current_key_bindings;
  taMisc::SetKeyBindingSet(taMisc::KEY_BINDINGS_DEFAULT);
  
  String action;
  String context;
  String context_pre;
  int context_count = static_cast<int>(taiMisc::CONTEXT_COUNT);
  for (int ctxt=0; ctxt<context_count; ctxt++) {
    taiMisc::BindingContext current_context = taiMisc::BindingContext(ctxt);
    context = TA_taiMisc.GetEnumString("BindingContext", ctxt);
    context_pre = context.before("_CONTEXT");  // strip off "_CONTEXT"
    int action_count = static_cast<int>(taiMisc::ACTION_COUNT);
    for (int i=0; i<action_count; i++) {
      action = TA_taiMisc.GetEnumString("BoundAction", i);
      if (action.startsWith(context_pre)) {
        taiMisc::BoundAction current_action = taiMisc::BoundAction(i);
        QKeySequence key_seq = taiMisc::GetSequenceFromAction(current_context, current_action);
        out << (QString)context.chars() << (QString)action.chars() << key_seq;
      }
    }
  }
  file.close();
  taMisc::SetKeyBindingSet(current_set);  // reset key binding set - might not have been default on enter
}

void taiMisc::UpdateCustomKeyBindings() {
  if (true) { // check version
    // compare the default bindings to see if there are actions that aren't in the custom bindings
    // or if there are actions in custom that are no longer in default
    KeyBindings* default_bindings = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::KEY_BINDINGS_DEFAULT));
    KeyBindings* custom_bindings = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::KEY_BINDINGS_CUSTOM));
    
    // set the context, retrieve that sub list for each set of bindings, do the comparison
    int context_count = static_cast<int>(taiMisc::CONTEXT_COUNT);
    // Add new bindings not previously supported
    for (int ctxt=0; ctxt<context_count; ctxt++) {
      KeyActionPair_PArray* default_pairs = default_bindings->CurrentBindings(static_cast<taiMisc::BindingContext>(ctxt));
      KeyActionPair_PArray* custom_pairs = custom_bindings->CurrentBindings(static_cast<taiMisc::BindingContext>(ctxt));
      for (int i=0; i<default_pairs->size; i++) {
        KeyActionPair* pair = &default_pairs->SafeEl(i);
        if (custom_pairs->FindAction(pair->action) == -1) { // if pairing not found
          custom_bindings->Add(static_cast<taiMisc::BindingContext>(ctxt), pair->action, QKeySequence(pair->key_sequence));
//          String action_str = TA_taiMisc.GetEnumString("BoundAction", static_cast<int>(pair->action));  // for debug
        }
      }
    }
    
    // Remove custom bindings no longer supported
    for (int ctxt=0; ctxt<context_count; ctxt++) {
      KeyActionPair_PArray* default_pairs = default_bindings->CurrentBindings(static_cast<taiMisc::BindingContext>(ctxt));
      KeyActionPair_PArray* custom_pairs = custom_bindings->CurrentBindings(static_cast<taiMisc::BindingContext>(ctxt));
      for (int i=custom_pairs->size-1; i>=0; i--) {  // start at end of list
        KeyActionPair* pair = &custom_pairs->SafeEl(i);
        if (default_pairs->FindAction(pair->action) == -1) { // if pairing not found
//          String action_str = TA_taiMisc.GetEnumString("BoundAction", static_cast<int>(pair->action));  // for debug
          custom_bindings->Remove(static_cast<taiMisc::BindingContext>(ctxt), pair->action, QKeySequence());  // just check action
        }
      }
    }
    SaveCustomKeyBindings();
  }
}

void taiMisc::SaveCustomKeyBindings() {
  String filename = taMisc::prefs_dir + PATH_SEP + "custom_keys";
  QFile file(filename);
  if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
    return;
  }
  QDataStream out(&file);
  
  KeyBindings* custom_bindings = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::KEY_BINDINGS_CUSTOM));
  
  // add any bindings that are in the default list that aren't in the custom list
  int context_count = static_cast<int>(taiMisc::CONTEXT_COUNT);
  for (int ctxt=0; ctxt<context_count; ctxt++) {
    KeyActionPair_PArray* pairs = custom_bindings->CurrentBindings(static_cast<taiMisc::BindingContext>(ctxt));
    for (int i=0; i<pairs->size; i++) {
      KeyActionPair* pair = &pairs->SafeEl(i);
      taiMisc::BoundAction action = pair->action;
      QKeySequence ks = pair->key_sequence;
      String context_str = TA_taiMisc.GetEnumString("BindingContext", ctxt);
      String action_str = TA_taiMisc.GetEnumString("BoundAction", static_cast<int>(action));
      out << (QString)context_str.chars() << (QString)action_str.chars() << ks;
    }
  }
  // do the same for bindings which no longer exist
  file.close();
}
