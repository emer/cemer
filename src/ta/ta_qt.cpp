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
#include <qrect.h>

#include <qlabel.h>// metrics
//#include <qlayout.h>
//#include <qlineedit.h>
#include <qmenubar.h>// metrics
#include <qprogressdialog.h>
#include <qpushbutton.h> // metrics
//#include <qspinbox.h>
//#include <qstring.h>

#ifdef TA_USE_INVENTOR
#include <Inventor/Qt/SoQt.h>
/*nn  #ifndef SOQT_INTERNAL
    #define HACK_SOQT_INTERNAL
    #define SOQT_INTERNAL
  #endif
  #include <Inventor/Qt/SoQtP.h> //WARNING: not installed by default -- must be copied from source directory
  #ifdef HACK_SOQT_INTERNAL
    #undef HACK_SOQT_INTERNAL
    #undef SOQT_INTERNAL
  #endif */
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

TA_API taiMisc* taiM = NULL;

bool 			taiMisc::gui_active = false;
taiDialog_List 		taiMisc::active_dialogs;
taiEditDataHost_List	taiMisc::active_edits;
taiEditDataHost_List 	taiMisc::css_active_edits;
iDataViewer_PtrList 	taiMisc::viewer_wins;
TypeSpace		taiMisc::arg_types;
QWidget*		taiMisc::main_window = NULL;
taBase_PtrList		taiMisc::unopened_windows;
void (*taiMisc::Update_Hook)(TAPtr) = NULL;

int taiMisc::busy_count = 0;


void taiMisc::Initialize(bool gui, const char* classname_) {
  cssiSession::Init();		// setup the interaction between qt & css
  // sets the readline idle handler to cssiSession::Run

  // allocate gui structures
  taiM = new taiMisc(classname_);

  // initialize the type system
  taiType::InitializeTypes();

#ifdef CYGWIN
  extern void iv_display_scale(float scale);
  iv_display_scale(taMisc::mswin_scale);
#endif

  if (gui & !cssiSession::WaitProc) {
    cssiSession::WaitProc = tabMisc::WaitProc;
  }

  //NOTE: we need to preset gui here, to enable things like the root to actually make its window..
  // however, strictly speaking, we really shouldn't set these until the main window is set via
  // SetMainWindow
  taiMisc::gui_active = gui;
  taMisc::gui_active = gui;
}

void taiMisc::SetMainWindow(QWidget* win) {
  main_window = win;
  QObject::connect(win, SIGNAL(destroyed()), taiM, SLOT(MainWindowDestroyed()) );
//Qt3  qApp->setMainWidget(win);
#ifdef TA_USE_INVENTOR
//NOTE: we called this already, which created the special SoQApplication object
// This second call just sets the main widget
//  SoQtP::mainwidget = win;
//  SoQt::init(win);
#endif

  win->show(); //note: doesn't actually show until event loop called, from rl callback
  // indicate gui_active -- cleared when main window destroyed
  taiMisc::gui_active = true;		// now it is activated
  taMisc::gui_active = true;		// now it is activated
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
  case sizSmall: return 24; break;
  case sizBig: return 27; break;
  default: return 25; break;
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
  case sizSmall: return 20; break;
  case sizBig: return 24; break;
  default: return 21; break;
  }
}

int taiMisc::text_height(int sizeSpec) {
  switch (sizeSpec & siz_mask) {
  case sizSmall: return 22; break;
  case sizBig: return 26; break;
  default: return 23; break;
  }
}


taiMisc::taiMisc(const char* classname_) {
  mclassname = classname_;
  init();
}
taiMisc::taiMisc() {
  init();
}
void taiMisc::init() {
  taMisc::Busy_Hook = &Busy_;
  taMisc::ScriptRecordingGui_Hook = &ScriptRecordingGui_; // note: ok to do more than once
  taMisc::DelayedMenuUpdate_Hook = &DelayedMenuUpdate_;
  load_dlg = NULL;
  InitMetrics();

  edit_darkbg_brightness = -0.15f;
  edit_lightbg_brightness = 0.50f;

  if(taMisc::not_constr || taMisc::in_init) {
/*    vsep = NULL;
    hsep = NULL;
    vfsep = NULL;
    hfsep = NULL;
    vspc = NULL;
    hspc = NULL;
    vfspc = NULL;
    hfspc = NULL;
    name_font = NULL;
    small_menu_font = NULL;
    small_submenu_font = NULL;
    big_menu_font = NULL;
    big_submenu_font = NULL;
    big_menubar_font = NULL;
    big_italic_menubar_font = NULL;
    small_button_width=46.0;
    medium_button_width=72.0;
    big_button_width=115.0;
    title_style = NULL;
    apply_button_style = NULL;
    name_style = NULL;
    wait_cursor = NULL;
    record_cursor = NULL;*/
    return;
  }

//OBS  font_foreground = new iColor(QApplication::palette().color(QPalette::Active, QColorGroup::Text));

//OBS  edit_darkbg = wkit->background()->brightness(edit_darkbg_brightness);

//OBS  edit_lightbg = new QColor(1.0f, 1.0f, 0.0f, 1.0f); // wkit->background()->brightness(edit_lightbg_brightness);
/* OBS:
  // instead of pushing and popping styles, just get the damn fonts!
  ivStyle* sty;

  // name_style is special because it also specifies a color for the buttons
  name_style = new ivStyle(style);  name_style->alias("name");
  ivResource::ref(name_style);
  wkit->push_style(name_style);
  name_font = (QFont*)wkit->font();  ivResource::ref(name_font);
  wkit->pop_style();

  sty = new ivStyle(style);  sty->alias("small_menu");
  wkit->push_style(sty);
  small_menu_font = (QFont*)wkit->font();  ivResource::ref(small_menu_font);
  wkit->pop_style();

  sty = new ivStyle(style);  sty->alias("small_submenu");
  wkit->push_style(sty);
  small_submenu_font = (QFont*)wkit->font();  ivResource::ref(small_submenu_font);
  wkit->pop_style();

  sty = new ivStyle(style);  sty->alias("big_menu");
  wkit->push_style(sty);
  big_menu_font = (QFont*)wkit->font();  ivResource::ref(big_menu_font);
  wkit->pop_style();

  sty = new ivStyle(style);  sty->alias("big_submenu");
  wkit->push_style(sty);
  big_submenu_font = (QFont*)wkit->font();  ivResource::ref(big_submenu_font);
  wkit->pop_style();

  sty = new ivStyle(style);  sty->alias("big_menubar");
  wkit->push_style(sty);
  big_menubar_font = (QFont*)wkit->font();  ivResource::ref(big_menubar_font);
  wkit->pop_style();

  sty = new ivStyle(style);  sty->alias("big_italic_menubar");
  wkit->push_style(sty);
  big_italic_menubar_font = (QFont*)wkit->font();  ivResource::ref(big_italic_menubar_font);
  wkit->pop_style();

  sty = new ivStyle(style);  sty->alias("TaIVButton");
  wkit->push_style(sty);
*/ // OBS
//  small_button_width = 46;
//OBS:  wkit->style()->find_attribute("SmallWidth", small_button_width);
//  medium_button_width=72;
//  wkit->style()->find_attribute("MediumWidth", medium_button_width);
//  big_button_width= 100;
//OBS:   wkit->style()->find_attribute("BigWidth", big_button_width);
//OBS:   wkit->pop_style();

/* OBS:
  title_style = new ivStyle(style);
  ivResource::ref(title_style);
  title_style->alias("title");

  apply_button_style = new ivStyle(style);
  ivResource::ref(apply_button_style);
  apply_button_style->alias("apply_button");
*/
  
  // Wait cursor -- TODO: shouldn't we just use standard system wait cursor???
/*qt3  QBitmap waiter = QBitmap(wait_cursor_width, wait_cursor_height, wait_cursor_bits, TRUE);
  QBitmap waiter_m = QBitmap(wait_cursor_width, wait_cursor_height, wait_mask_bits, TRUE); */
  QBitmap waiter = QBitmap::fromData(QSize(wait_cursor_width, wait_cursor_height), 
    wait_cursor_bits, QImage::Format_MonoLSB);
  QBitmap waiter_m = QBitmap::fromData(QSize(wait_cursor_width, wait_cursor_height), 
    wait_mask_bits, QImage::Format_MonoLSB);
  wait_cursor = new QCursor(waiter, waiter_m, wait_cursor_x_hot, wait_cursor_y_hot);

  // Record cursor
/*qt3  QBitmap recorder = QBitmap(record_cursor_width, record_cursor_height, record_cursor_bits, TRUE);
  QBitmap recorder_m = QBitmap(record_cursor_width, record_cursor_height, record_mask_bits, TRUE); */
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
}

void taiMisc::InitMetrics() {
  ctrl_size = sizMedium;
  vsep_c = 3;
  hsep_c = 3;
  vspc_c = 6;
  hspc_c = 6;
  dlgm_c = 8;
// default dialog sizes
  QDesktopWidget *d = QApplication::desktop();
  int primaryScreen = d->primaryScreen();
  QRect scrn_geom = d->availableGeometry(primaryScreen);
  scrn_s = scrn_geom.size();
//TEMP: TODO: comment out this line
  scrn_s = iSize(1280, 1024); // temp
  if (scrn_s.height() <= 768)
    base_height = 21;
  else if (scrn_s.height() <= 1024)
    base_height = 25;
  else
    base_height = 29;
  // fonts -- note, no way to get Qt's metrics without instances!
  QWidget* w = NULL;
  w = new QPushButton("the rain in spain", (QWidget*)NULL);
    mbig_button_font = QApplication::font(w);
  delete w;

  mbig_dialog_font = QApplication::font(NULL); // use the default font

  w = new QMenuBar();
    mbig_menu_font = QApplication::font(w);
  delete w;

  w = new QLabel("the rain in spain", (QWidget*)NULL);
    mbig_name_font = QApplication::font(w);
  delete w;

}
void taiMisc::AdjustFont(int fontSpec, iFont& font) {
  if (fontSpec & fonItalic) font.setItalic(true);
  if (fontSpec & fonBold) font.setBold(true);
  if (fontSpec & fonUnderline) font.setUnderline(true);
  if ((fontSpec & fonStretch_mask) == fonSkinny) font.setStretch(QFont::Condensed);
  if ((fontSpec & fonStretch_mask) == fonWide) font.setStretch(QFont::Expanded);
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

#define FONT_MED  1
#define FONT_SM   2
iFont taiMisc::buttonFont(int fontSpec) {
  iFont rval = mbig_button_font;
  if ((fontSpec & fonSize_mask) == fonMedium)
    rval.setPointSize(rval.pointSize - FONT_MED); //NOTE: assumes not using pixel-size, otherwise pointsize=-1
  else if  ((fontSpec & fonSize_mask) == fonSmall)
    rval.setPointSize(rval.pointSize - FONT_SM); //NOTE: assumes not using pixel-size, otherwise pointsize=-1
  AdjustFont(fontSpec, rval);
  return rval;
}

iFont taiMisc::dialogFont(int fontSpec) {
  iFont rval = mbig_dialog_font;
  if ((fontSpec & fonSize_mask) == fonMedium)
    rval.setPointSize(rval.pointSize - FONT_MED); //NOTE: assumes not using pixel-size, otherwise pointsize=-1
  else if  ((fontSpec & fonSize_mask) == fonSmall)
    rval.setPointSize(rval.pointSize - FONT_SM); //NOTE: assumes not using pixel-size, otherwise pointsize=-1
  AdjustFont(fontSpec, rval);
  return rval;
}

iFont taiMisc::menuFont(int fontSpec) {
  iFont rval = mbig_menu_font;
  if ((fontSpec & fonSize_mask) == fonMedium)
    rval.setPointSize(rval.pointSize - FONT_MED); //NOTE: assumes not using pixel-size, otherwise pointsize=-1
  else if  ((fontSpec & fonSize_mask) == fonSmall)
    rval.setPointSize(rval.pointSize - FONT_SM); //NOTE: assumes not using pixel-size, otherwise pointsize=-1
  AdjustFont(fontSpec, rval);
  return rval;
}

iFont taiMisc::nameFont(int fontSpec) {
  iFont rval = mbig_name_font;
  if ((fontSpec & fonSize_mask) == fonMedium)
    rval.setPointSize(rval.pointSize - FONT_MED); //NOTE: assumes not using pixel-size, otherwise pointsize=-1
  else if  ((fontSpec & fonSize_mask) == fonSmall)
    rval.setPointSize(rval.pointSize - FONT_SM); //NOTE: assumes not using pixel-size, otherwise pointsize=-1
  AdjustFont(fontSpec, rval);
  return rval;
}

void taiMisc::MainWindowDestroyed() {
  main_window = NULL;
  taiMisc::gui_active = false;
  taMisc::gui_active = false;
}

void taiMisc::LoadDialogDestroyed() {
  load_dlg = NULL;
}

void taiMisc::MainWindowClosing(bool& cancel) {
   // called by main_window in close event -- we can cancel it
  //TODO: go through active wins, and close -- prompt user if any active ones found
  //TODO: we will need to figure out how to do this, because we need to pump the event loop!
}

void taiMisc::Update(TAPtr obj) {
  if (Update_Hook != NULL)
    (*Update_Hook)(obj);
}

void taiMisc::Busy_(bool busy) {
  if (!gui_active)    return;
  if (busy) {
    ++busy_count;	// keep track of number of times called
  //  if (cssiSession::block_in_event == true) // already busy
  //    return;
  //  cssiSession::block_in_event = true;
    if (busy_count == 1) SetWinCursors();
  } else {
    if(--busy_count == 0) {
      cssiSession::done_busy = true;
      RestoreWinCursors(); //added 4.0
    }
    if (busy_count < 0) {
      cerr << "Warning: taiMisc::DoneBusy() called more times than Busy()\n";
      busy_count = 0;
    }
  }
}

void taiMisc::DoneBusy_impl() {
  if (!gui_active)    return;
//  cssiSession::block_in_event = false;
//3.2a  RestoreWinCursors();
}

int taiMisc::RunPending() {
  if (!gui_active)    return false;
  return cssiSession::RunPending();
}

void taiMisc::ScriptRecordingGui_(bool start){
  if (!gui_active)    return;
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

void taiMisc::RestoreWinCursors() {
  QApplication::restoreOverrideCursor();
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


/*obs bool taiMisc::CloseEdits(void* obj, TypeDef* td) {
  if(!gui_active)    return false;
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
  if(!gui_active)    return false;
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
  if (!gui_active)    return false;
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
  if (!gui_active)    return false;
  bool got_one = false;
  for (int i = active_edits.size-1; i >= 0; --i) {
    taiEditDataHost* edh = active_edits.FastEl(i);
    if((edh->cur_base == obj) && (edh->state == taiDataHost::ACTIVE)) {
      got_one = got_one || edh->ReShow(force);
    }
  }
  return got_one;
}

taiEditDataHost* taiMisc::FindEdit(void* obj, TypeDef*, iDataViewer* not_in_win) {
  //NOTE: not_in_win works as follows:
  // NULL: ok to return any edit (typically used to get show value)
  // !NULL: must get other win that not; used to raise that edit panel to top, so
  //  shouldn't hide the edit panel that invoked the operation
  if (!gui_active) return NULL;
  for (int i = active_edits.size - 1; i >= 0; --i) {
    taiEditDataHost* host = active_edits.FastEl(i);
    if ((host->cur_base != obj) || (host->state != taiDataHost::ACTIVE))
      continue;
    if (host->isDialog() && !host->modal)
      return host;
    // is a EditPanel
    if (host->dataPanel()->viewer_win() != not_in_win) // if niw NULL, then will always be true
      return host;
  }
  return NULL;
}

// old winb funcs

void taiMisc::OpenWindows(){
  if(unopened_windows.size == 0)	return;

  taMisc::is_loading = true;
  int i;
  for(i=0;i < unopened_windows.size;i++){
    DataViewer* win = (DataViewer*)unopened_windows.FastEl(i);
    win->OpenNewWindow();
    win->UpdateAfterEdit();
  }
  taiMisc::RunPending();
  for(i=0;i < unopened_windows.size;i++){
    DataViewer* win =   ((DataViewer *) unopened_windows.FastEl(i));
    if (win->iconified) win->Iconify();
  }
  unopened_windows.RemoveAll();
  taMisc::is_loading = false;
  cssiSession::RaiseObjEdits(); // make sure css objects are in front!

  taiMisc::RunPending();
  for(i=taiMisc::active_edits.size-1; i>=0; i--) {
    taiEditDataHost* dlg = taiMisc::active_edits.FastEl(i);
    if((dlg->typ != NULL) && dlg->typ->InheritsFrom(TA_SelectEdit) && (dlg->state == taiDataHost::ACTIVE))
      dlg->Raise();
  }
  taiMisc::RunPending();
}

int taiMisc::WaitProc() {
  if(tabMisc::WaitProc())
    return true;

  if(!taMisc::gui_active)    return false;

/*obs  if(update_winpos.size > 0) {
    Wait_UpdateWinPos();
    return true;
  }*/
/*  if(update_menus.size > 0) {
    Wait_UpdateMenus();
    return true;
  } */
  return Script::Wait_RecompileScripts();
}

/*obs void winbMisc::Wait_UpdateMenus() {
  if(!taMisc::gui_active)    return;
  taMisc::Busy();
  int i;
  for(i=0; i<update_menus.size; i++) {
    TAPtr it = update_menus.FastEl(i);
    if(taBase::GetRefn(it) == 0) {
      taMisc::Error("*** Object is not owned:", it->GetName(), "of type:",
		     it->GetTypeDef()->name);
      taBase::Ref(it);
    }
    MenuUpdate(it);
  }
  update_menus.RemoveAll();
  taMisc::DoneBusy();
}*/

/*obs void winbMisc::Wait_UpdateWinPos() {
  if(!taMisc::gui_active)    return;
  taMisc::Busy();
  int i;
  for(i=0; i<update_winpos.size; i++) {
    TAPtr it = update_winpos.FastEl(i);
    if(taBase::GetRefn(it) == 0) {
      taMisc::Error("*** Object is not owned:", it->GetName(), "of type:",
		     it->GetTypeDef()->name);
      taBase::Ref(it);
    }
    if(it->InheritsFrom(&TA_DataViewer)) {
      ((DataViewer*)it)->SetWinPos();	// extra safe..
    }
  }
  update_winpos.RemoveAll();
  taMisc::DoneBusy();
} */

void taiMisc::DelayedMenuUpdate_(TAPtr obj) {
  if(!taMisc::gui_active)    return;
/*obs   if(taBase::GetRefn(obj) == 0) {
    taMisc::Error("*** Object is not owned:", obj->GetName(), "of type:",
		  obj->GetTypeDef()->name);
    taBase::Ref(obj);
  }
  update_menus.LinkUnique(obj); */
}

/*obs void winbMisc::MenuUpdate(TAPtr obj) {
  if(!taMisc::gui_active)	return;

  taMisc::Busy();

  // try to update the menu group (less impact)
  MenuGroup_impl* mg;
  if(obj->InheritsFrom(TA_MenuGroup_impl))
    mg = (MenuGroup_impl*)obj;
  else
    mg = GET_OWNER(obj,MenuGroup_impl);

  TAPtr ownr = mg;
  // get the highest menugroup
  if((mg != NULL) && (mg->owner != NULL) &&
     mg->owner->InheritsFrom(TA_taList_impl))
  {
    ownr = mg->owner;
    while((ownr != NULL) && (ownr->GetOwner() != NULL) &&
	  ownr->GetOwner()->InheritsFrom(TA_taList_impl)) {
      if(ownr->InheritsFrom(TA_MenuGroup_impl))
	mg = (MenuGroup_impl*)ownr;
      ownr = ownr->GetOwner();
      if(ownr->InheritsFrom(TA_MenuGroup_impl))	// always try to get a mgroup
	mg = (MenuGroup_impl*)ownr;
    }
  }

  if((mg != NULL) && (mg->HasMenu())) {
    mg->UpdateMenu();
  }
  else {
    if(ownr != NULL)		// get the owner of the highest group
      ownr = ownr->GetOwner();

    // update it if it is a winbase..
    if((ownr != NULL) && ownr->InheritsFrom(&TA_DataViewer)) {
      ((DataViewer*)ownr)->UpdateMenus();
      taMisc::DoneBusy();
      return;
    }
    else if((ownr != NULL) && ownr->HasOption("MEMB_IN_GPMENU")) {
      // if it has a IN_GPMENU, it might be that we are in a subgroup of it
      // so go ahead and update that object instead of us
      taMisc::DoneBusy();
      MenuUpdate(ownr);
      return;
    }

    // if all else fails, then update based on the first winbase
    DataViewer* we = GET_OWNER(obj,DataViewer);
    if (we != NULL)
      we->UpdateMenus();
    else if(obj->InheritsFrom(TA_DataViewer)) // if nothing else, do object itself
      ((DataViewer*)obj)->UpdateMenus();
  }

  taMisc::DoneBusy();
} */

int taiMisc::SetIconify(void* obj, int onoff){
  DataViewer* wb = (DataViewer *) obj;
  if(onoff != -1) {
    if(!onoff && !wb->IsMapped())
      return wb->iconified;	// attempt to update iconified (non-mapped) window!
    if(!onoff && wb->iconified)	// switching from iconfied to not, update menus
      DelayedMenuUpdate_(wb);	// update menus when de-iconifying
    wb->iconified = onoff;
  }
  return (int) wb->iconified;
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
  if (!gui_active) return;
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
  if (!taMisc::gui_active) return;
  if (load_dlg == NULL) return;
  String loadstring = String("Loading: ") + String(tpname);
  load_dlg->setLabelText(loadstring);
  load_dlg->setTotalSteps(totalSteps);
  if (totalSteps == 0)
    load_dlg->setMinimumDuration(0); // no show delay
  RunPending();
}

void taiMisc::StepLoadDialog(int stepNum) {
  if (!taMisc::gui_active) return;
  if (load_dlg == NULL) return;
  load_dlg->setProgress(stepNum);
  RunPending();
}
void taiMisc::RemoveLoadDialog(){
  if (!taMisc::gui_active) return;
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


//////////////////////////////////////////////////////////
// 	taiMisc: helper functions for iv              //
//////////////////////////////////////////////////////////

/* TODO:
QWidget* taiMisc::small_button(QWidget* b){
  return(layout->hfixed(b, small_button_width));
}

QWidget* taiMisc::medium_button(QWidget* b){
  return(layout->hfixed(b, medium_button_width));
}

QWidget* taiMisc::big_button(QWidget* b){
  return(layout->hfixed(b, big_button_width));
}


QWidget* taiMisc::small_flex_button(QWidget* b){
  return(layout->hflexible(layout->hnatural(b,small_button_width)));
}

QWidget* taiMisc::medium_flex_button(QWidget* b){
  return(layout->hflexible(layout->hnatural(b,medium_button_width)));
}

QWidget* taiMisc::big_flex_button(QWidget* b){
  return(layout->hflexible(layout->hnatural(b,big_button_width)));
}
*/
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

