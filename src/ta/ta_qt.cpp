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


// ta_qt.cc

#include "ta_script.h"
#include "ta_qt.h"
#include "ta_viewer.h"
#include "ta_qtdialog.h"
#include "ta_qtviewer.h"
#include "ta_qttype_def.h"
#include "css_qt.h"
#include "ta_css.h"
#include "ta_qtcursors.h" //note: only place this s/b included
#include "ta_platform.h"
#include "ta_TA_type.h"

#include "icolor.h"

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

#ifdef TA_USE_INVENTOR
#include <Inventor/Qt/SoQt.h>
#endif


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

iMainWindowViewer* iTopLevelWindow_List::FastElAsMainWindow(int i) {
  return dynamic_cast<iMainWindowViewer*>(FastEl(i)->widget());
}

iDockViewer* iTopLevelWindow_List::FastElAsDockWindow(int i) {
  return dynamic_cast<iDockViewer*>(FastEl(i)->widget());
}

iMainWindowViewer* iTopLevelWindow_List::Peek_MainWindow() {
  for (int i = 0; i < size; ++i) {
    iMainWindowViewer* rval = FastElAsMainWindow(i);
    if (rval) return rval;
  }
  return NULL;
}

iDockViewer* iTopLevelWindow_List::Peek_DockWindow() {
  for (int i = 0; i < size; ++i) {
    iDockViewer* rval = FastElAsDockWindow(i);
    if (rval) return rval;
  }
  return NULL;
}


TA_API taiMisc* taiM_ = NULL;

const int taiMisc::FONT_MED = 1;
const int taiMisc::FONT_SM = 2;

taiDialog_List 		taiMisc::active_dialogs;
taiEditDataHost_List	taiMisc::active_edits;
taiEditDataHost_List 	taiMisc::css_active_edits;
iTopLevelWindow_List	taiMisc::active_wins;
TypeSpace		taiMisc::arg_types;
QWidget*		taiMisc::main_window = NULL;
taBase_PtrList		taiMisc::unopened_windows;
void (*taiMisc::Update_Hook)(TAPtr) = NULL;

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

  // initialize the type system
  taiType::InitializeTypes();

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
}

taiMisc::~taiMisc() {
  delete wait_cursor;
  wait_cursor = NULL;
  delete record_cursor;
  record_cursor = NULL;
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
    QMessageBox::information(QApplication::activeWindow(),
      "Check Succeeded", 
      "No configuration errors were found.");
  } else {
    iTextEditDialog* td = new iTextEditDialog(true);
    td->setWindowTitle("Check Failed");
    td->setText(taMisc::last_check_msg);
    td->exec();
    td->deleteLater();
  }
}



void taiMisc::InitMetrics() {
  // everything that requires Qt to be initialized and could depend on Settings being loaded
  
  // set up the initial font from (already loaded) Settings
  QFont font(taMisc::font_name, taMisc::font_size);
//TODO: we should probably put practical lower/upper limits on font sizes
  qApp->setFont(font);
  
  edit_darkbg_brightness = -0.15f;
  edit_lightbg_brightness = 0.50f;

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

  mbig_dialog_font = QApplication::font(NULL); // use the default font

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
  switch (dialogSpec & dlgSize_mask) {
  case dlgSmall:
    if (ver) rval = iSize((scrn_s.w * 3) / 10, (scrn_s.h * 2) / 5);// .3w .4h small vertically oriented dialog (ex. popup list selectors)
    else rval = iSize((scrn_s.w * 2) / 5, (scrn_s.h * 3) / 10); // .4w .3h small horizontally oriented dialog
    break;
  case dlgBig:
    if (ver) rval = iSize((scrn_s.w * 3) / 5, (scrn_s.h * 4) / 5);// .6w .8h  big vertically oriented dialog
    else rval = iSize((scrn_s.w * 4) / 5, (scrn_s.h * 3) / 5);// .8h .6w big horizontally oriented dialog
    break;
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
  qApp->closeAllWindows();
}

void taiMisc::ResolveEditChanges(CancelOp& cancel_op) {
  for (int i = 0; i < taiMisc::active_edits.size; ++i) {
    taiEditDataHost* edh = taiMisc::active_edits.FastEl(i);
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

void taiMisc::Update(TAPtr obj) {
  if (Update_Hook != NULL)
    (*Update_Hook)(obj);
}

void taiMisc::PurgeDialogs() {
  bool did_purge = false;
  for (int i = active_dialogs.size - 1; i >= 0; --i) {
    taiDataHost* dlg = active_dialogs.FastEl(i);
    if ((dlg->state == taiDataHost::ACCEPTED) || (dlg->state == taiDataHost::CANCELED)) {
      active_dialogs.Remove(i);
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
  bool is_busy = false;
  bool is_rec = false;
  if((taiMisc::busy_count > 0) /*|| cssiSession::block_in_event*/)
    is_busy = true;
  if (taMisc::record_script !=NULL)
    is_rec = true;
  // busy trumps recording...
  if (is_busy) {
    QApplication::setOverrideCursor(*taiM->wait_cursor);
    return;
  }

  if (is_rec) {
    QApplication::setOverrideCursor(*taiM->record_cursor);
    return;
  }

  taMisc::Error("*** Unexpected call to SetWinCursors -- not busy or recording.");
}


/*obs bool taiMisc::CloseEdits(void* obj, TypeDef* td) {
  if(!taMisc::gui_active)    return false;
  PurgeDialogs();
  bool got_one = false;
  //note: for panels, Cancel() causes panel to close/delete
  if(!td->InheritsFrom(TA_taBase)) {
    // look for edits whose object is 'obj'
    for (int i=active_edits.size-1; i>=0; i--) {
      taiEditDataHost* edh = active_edits.FastEl(i);
      if((edh->cur_base == obj) && (edh->state == taiDataHost::ACTIVE)) {
	edh->Cancel();
	got_one = true;
      }
    }
    return got_one;
  }

  TAPtr ta_obj = (TAPtr)obj;
  int i;
  for(i=active_edits.size-1; i>=0; i--) {
    taiEditDataHost* edh = active_edits.FastEl(i);
    if((edh->typ == NULL) || !edh->typ->InheritsFrom(TA_taBase) ||
       (edh->state != taiDataHost::ACTIVE))
      continue;
    TAPtr dbase = (TAPtr)edh->cur_base;
    if(dbase == ta_obj) {
      edh->Cancel();
      got_one = true;
      continue;
    }
    TAPtr dbpar = dbase->GetOwner(td); // if its a parent of that type..
    if(dbpar == ta_obj) {
      edh->Cancel();
      got_one = true;
      continue;
    }
    // also check for groups that might contain the object
    if(!edh->typ->InheritsFrom(TA_taList_impl))
      continue;
    taList_impl* lst = (taList_impl*)edh->cur_base;
    if(lst->Find(ta_obj) >= 0)
       edh->SetRevert();	// it's been changed..
  }
  return got_one;
} */

/*
bool taiMisc::NotifyEdits(void* obj, TypeDef*) {
  if(!taMisc::gui_active)    return false;
  bool got_one = false;
  for (int i = active_edits.size - 1; i >= 0; --i) {
    taiEditDataHost* dlg = active_edits.FastEl(i);
    if((dlg->state != taiDataHost::ACTIVE) || (dlg->cur_base == NULL) ||
       (dlg->typ == NULL))
      continue;
    // this object contains the given one
    if((dlg->cur_base <= obj) && ((char*)obj <= ((char*)dlg->cur_base + dlg->typ->size))) {
      // if not modified, then update it, otherwise we should notify the user
       dlg->NotifyChanged();	// it's been changed..
      got_one = true;
      continue;
    }
    // also check for groups that might contain the object
    if(!dlg->typ->InheritsFrom(TA_taList_impl))
      continue;
    taList_impl* lst = (taList_impl*)dlg->cur_base;
    if(lst->Find((TAPtr)obj) >= 0) {
       dlg->NotifyChanged();	// it's been changed..
       got_one = true;
    }
  }
  return got_one;
}
*/
bool taiMisc::RevertEdits(void* obj, TypeDef*) {
  if (!taMisc::gui_active)    return false;
  bool got_one = false;
  for (int i = active_edits.size-1; i >= 0; --i) {
    taiEditDataHost* dlg = active_edits.FastEl(i);
    if((dlg->cur_base == obj) && (dlg->state == taiDataHost::ACTIVE)) {
      dlg->Revert_force();
      got_one = true;
    }
  }
  return got_one;
}

bool taiMisc::ReShowEdits(void* obj, TypeDef*, bool force) {
  if (!taMisc::gui_active)    return false;
  bool got_one = false;
  for (int i = active_edits.size-1; i >= 0; --i) {
    taiEditDataHost* edh = active_edits.FastEl(i);
    if((edh->cur_base == obj) && (edh->state == taiDataHost::ACTIVE)) {
      got_one = got_one || edh->ReShow(force);
    }
  }
  return got_one;
}

taiEditDataHost* taiMisc::FindEdit(void* obj, iMainWindowViewer* not_in_win) {
  //NOTE: not_in_win works as follows:
  // NULL: ok to return any edit (typically used to get show value)
  // !NULL: must get other win that not; used to raise that edit panel to top, so
  //  shouldn't hide the edit panel that invoked the operation
  if (!taMisc::gui_active) return NULL;
  for (int i = active_edits.size - 1; i >= 0; --i) {
    taiEditDataHost* host = active_edits.FastEl(i);
    if ((host->cur_base != obj) || (host->state != taiDataHost::ACTIVE))
      continue;
    if (host->isDialog() && !host->modal)
      return host;
    // is a EditPanel
    if (host->dataPanel()->window() != not_in_win) // if niw NULL, then will always be true
      return host;
  }
  return NULL;
}

taiEditDataHost* taiMisc::FindEditDialog(void* obj, bool read_only_state) {
  if (!taMisc::gui_active) return NULL;
  for (int i = active_edits.size - 1; i >= 0; --i) {
    taiEditDataHost* host = active_edits.FastEl(i);
    if ((host->cur_base != obj) || (host->state != taiDataHost::ACTIVE))
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
    if ((host->cur_base != obj) || (host->state != taiDataHost::ACTIVE))
      continue;
    if (host->isPanel() && (host->read_only == read_only) &&
      (host->dataPanel()->window() != not_in_win)) // if niw NULL, then will always be true
      return host;
  }
  return NULL;
}


// old winb funcs

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
  for(i=0;i < unopened_windows.size;i++){
    TopLevelViewer* win = dynamic_cast<TopLevelViewer*>(unopened_windows.FastEl(i));
    if (win->isIconified()) win->Iconify();
  }
  unopened_windows.RemoveAll();
  --taMisc::is_loading;
  cssiSession::RaiseObjEdits(); // make sure css objects are in front!

  taiMisc::RunPending();
  for(i=taiMisc::active_edits.size-1; i>=0; i--) {
    taiEditDataHost* dlg = taiMisc::active_edits.FastEl(i);
    if((dlg->typ != NULL) && dlg->typ->InheritsFrom(TA_SelectEdit) && (dlg->state == taiDataHost::ACTIVE))
      dlg->Raise();
  }
  taiMisc::RunPending();
}

void taiMisc::WaitProc() {
  taiMiscCore::WaitProc();
  if (!taMisc::gui_active) return;
  AbstractScriptBase::Wait_RecompileScripts();
}

void taiMisc::ScriptIconify(void*, int) {
// do nothing, use script win pos to record final iconify status
//  DataViewer* wb = (DataViewer *) obj;
//   if(onoff)
//     taiMisc::RecordScript(wb->GetPath() + ".Iconify();\n");
//   else
//     taiMisc::RecordScript(wb->GetPath() + ".DeIconify();\n");
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
  cmd += taMisc::tmp_dir + "/tai_gf." + String((int)taPlatform::processId()) + ".* >/dev/null 2>&1";
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


// Script Record

/* TODO: translate to Qt
////////////////////////////////////////////////////////////////////////////
// Dump JPEG, TIFF
////////////////////////////////////////////////////////////////////////////

#ifndef CYGWIN

extern "C" {
#include <jpeglib.h>
// using the IV version of lib tiff!
#include <TIFF/tiffio.h>
}

#define lowbit(x) ((x) & (~(x) + 1))

void taiMisc::DumpJpeg(XDisplay* dpy, XWindow win, const char* fnm, int quality, int xstart, int ystart, int width, int height) {
  XWindowAttributes win_info;
  if(!XGetWindowAttributes(dpy, win, &win_info)) {
    taMisc::Error("DumpJPEG: XGetWindowAttributes failed!");
    return;
  }

  if(!((win_info.visual->c_class == TrueColor) || (win_info.visual->c_class == DirectColor))) {
    taMisc::Error("DumpJPEG: only works for TrueColor or DirectColor displays!");
    return;
  }

  if(width < 0) width = win_info.width - xstart;
  if(height < 0) height = win_info.height - ystart;

  XImage* image = XGetImage(dpy, win, xstart, ystart, width, height, AllPlanes, ZPixmap);
  if(image == NULL) {
    taMisc::Error("*** DumpJPEG: XGetImage failed!");
    return;
  }

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  FILE* outfile;
  if ((outfile = fopen(fnm, "wb")) == NULL) {
    taMisc::Error("DumpJPEG: Can't open file:", fnm);
    return;
  }
  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width = image->width; 	// image width and height, in pixels
  cinfo.image_height = image->height;
  cinfo.input_components = 3;	// # of color components per pixel
  cinfo.in_color_space = JCS_RGB; // colorspace of input image

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, false);

  jpeg_start_compress(&cinfo, TRUE);

  JSAMPROW row_pointer[1];	// pointer to a single row
  int row_stride = image->width * 3;	// JSAMPLEs per row in image_buffer

  JSAMPLE* scanline = new JSAMPLE[row_stride];
  row_pointer[0] = scanline;

  ulong maxval = 1 << image->depth;

  ulong red1 = lowbit(image->red_mask);
  ulong green1 = lowbit(image->green_mask);
  ulong blue1 = lowbit(image->blue_mask);

  // r = high, g = med, blue = low bits; compute # of bytes as follows:
  ulong bluesz = green1;
  ulong greensz = red1 / green1;
  ulong redsz = maxval / red1;

  ulong bluemult = 256 / bluesz; // 256 is scale expected by JPEG
  ulong greenmult = 256 / greensz;
  ulong redmult = 256 / redsz;

  while (cinfo.next_scanline < cinfo.image_height) {
    int scpos = 0;
    for(int x=0; x<image->width;x++) {
      ulong pixel = XGetPixel(image, x, cinfo.next_scanline);
      ulong rm = pixel & image->red_mask;
      ulong gm = pixel & image->green_mask;
      ulong bm = pixel & image->blue_mask;

      rm /= red1; gm /= green1; bm /= blue1; // get rid of mask offsets
      rm *= redmult; gm *= greenmult; bm *= bluemult;// upscale from 5-6-5 bits to 8 bits

      scanline[scpos] = (JSAMPLE)rm;
      scanline[scpos+1] = (JSAMPLE)gm;
      scanline[scpos+2] = (JSAMPLE)bm;
      scpos+=3;
    }
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  delete [] scanline;

  jpeg_finish_compress(&cinfo);
  fclose(outfile);
  jpeg_destroy_compress(&cinfo);

  XDestroyImage(image);
}

void taiMisc::DumpJpegIv(iWindow* win, const char* fnm, int quality, int xstart, int ystart, int width, int height) {
  ivDisplay* dsp = ivSession::instance()->default_display();
  ivDisplayRep* d = dsp->rep();
  ivWindowRep* rep = win->rep();
  DumpJpeg(d->display_, rep->xwindow_, fnm, quality, xstart, ystart, width, height);
}

void taiMisc::DumpTiff(XDisplay* dpy, XWindow win, const char* fnm, int xstart, int ystart, int width, int height) {
  XWindowAttributes win_info;
  if(!XGetWindowAttributes(dpy, win, &win_info)) {
    taMisc::Error("DumpTIFF: XGetWindowAttributes failed!");
    return;
  }

  if(!((win_info.visual->c_class == TrueColor) || (win_info.visual->c_class == DirectColor))) {
    taMisc::Error("DumpTIFF: only works for TrueColor or DirectColor displays!");
    return;
  }

  if(width < 0) width = win_info.width - xstart;
  if(height < 0) height = win_info.height - ystart;

  XImage* image = XGetImage(dpy, win, xstart, ystart, width, height, AllPlanes, ZPixmap);
  if(image == NULL) {
    taMisc::Error("*** DumpTIFF: XGetImage failed!");
    return;
  }

  TIFF* tiff;
  if((tiff = TIFFOpen(fnm, "w")) == NULL) {
    taMisc::Error("DumpTIFF: Can't open file:", fnm);
    return;
  }

  TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, image->width);
  TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, image->height);
  TIFFSetField(tiff, TIFFTAG_XRESOLUTION, 100.0);
  TIFFSetField(tiff, TIFFTAG_YRESOLUTION, 100.0);
  TIFFSetField(tiff, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
  TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, image->height);
  TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8,8,8);
  TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
  TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 3);
  TIFFSetField(tiff, TIFFTAG_PLANARCONFIG,  PLANARCONFIG_CONTIG);
  TIFFSetField(tiff, TIFFTAG_ARTIST, "TypeAccess/PDP++ iWindow Dump");
//   TIFFSetField(tiff, TIFFTAG_ ,  );
//   TIFFSetField(tiff, TIFFTAG_ ,  );
//   TIFFSetField(tiff, TIFFTAG_ ,  );

  int row_stride = image->width * 3;	// JSAMPLEs per row in image_buffer
  JSAMPLE* scanline = new JSAMPLE[row_stride];

  ulong maxval = 1 << image->depth;
  ulong red1 = lowbit(image->red_mask);
  ulong green1 = lowbit(image->green_mask);
  ulong blue1 = lowbit(image->blue_mask);

  // r = high, g = med, blue = low bits; compute # of bytes as follows:
  ulong bluesz = green1;
  ulong greensz = red1 / green1;
  ulong redsz = maxval / red1;

  ulong bluemult = 256 / bluesz; // 256 is scale expected by JPEG
  ulong greenmult = 256 / greensz;
  ulong redmult = 256 / redsz;

  for(int y=0;y<image->height;y++) {
    int scpos = 0;
    for(int x=0; x<image->width;x++) {
      ulong pixel = XGetPixel(image, x, y);
      ulong rm = pixel & image->red_mask;
      ulong gm = pixel & image->green_mask;
      ulong bm = pixel & image->blue_mask;

      rm /= red1; gm /= green1; bm /= blue1; // get rid of mask offsets
      rm *= redmult; gm *= greenmult; bm *= bluemult;// upscale from 5-6-5 bits to 8 bits

      scanline[scpos] = (JSAMPLE)rm;
      scanline[scpos+1] = (JSAMPLE)gm;
      scanline[scpos+2] = (JSAMPLE)bm;
      scpos+=3;
    }

    TIFFWriteScanline(tiff, scanline, y, 0);
  }

  delete [] scanline;

  TIFFClose(tiff);

  XDestroyImage(image);
}

void taiMisc::DumpTiffIv(iWindow* win, const char* fnm, int xstart, int ystart, int width, int height) {
  ivDisplay* dsp = ivSession::instance()->default_display();
  ivDisplayRep* d = dsp->rep();
  ivWindowRep* rep = win->rep();
  DumpTiff(d->display_, rep->xwindow_, fnm, xstart, ystart, width, height);
}

#endif // CYGWIN
*/  //TODO


//////////////////////////
//  iApplication 	//
//////////////////////////

/* from Qt docs:
The default implementation requests interaction and sends a close event to all visible top-level widgets. If any event was rejected, the shutdown is canceled.
*/
void iApplication::commitData(QSessionManager& manager) {
  taMisc::quitting = taMisc::QF_FORCE_QUIT;
  CancelOp cancel_op = CO_NOT_CANCELLABLE;
  taiMiscCore::OnQuitting(cancel_op); // save changes, etc.
  inherited::commitData(manager);
}
