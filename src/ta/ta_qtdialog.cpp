// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// /* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// ta_qtdialog.cc


#include "ta_qt.h"
#include "ta_qtdialog.h"
#include "ta_qttype.h"
#include "ta_qtdata.h"
#include "ta_base.h"
#include "css_qt.h"
#include "css_basic_types.h"
#include "ta_css.h"
#include "ta_seledit.h"
#include "ta_TA_type.h"

#include "icolor.h"
#include "ieditgrid.h"
#include "iflowlayout.h"

#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qdialog.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qhbox.h>
#include <qhbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qmenudata.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qptrlist.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qsizepolicy.h>
#include <qstring.h>
#include <qstylesheet.h>
#include <qtable.h>
#include <qtimer.h>
#include <qtooltip.h>
//#include <qvbox.h>
#include <qwidget.h>


// TODO: why is String=osString in this file, unless I do this:
#include "ta_string.h"

class QHackMouseEvent: public QMouseEvent { // hack to enable us to change the mouse button
public:
  QHackMouseEvent(const QHackMouseEvent& dummy): QMouseEvent(dummy) {}
  void		setButton(ushort val) {b = val;}
};

//////////////////////////////////////////////////
// 		HiLightButton			//
//////////////////////////////////////////////////
/*
ScriptButton::ScriptButton(char* name, ivAction* a, char* srp)
: ivButton(NULL,NULL,new ivTelltaleState,a) {
  script = srp;
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivButton* temp = wkit->push_button(name,NULL);
  ivResource::ref(temp);
  body(temp->body());
  state(temp->state());
  ivResource::unref(temp);
}

void ScriptButton::release(const ivEvent& e){
  taiMisc::RecordScript(script);
  ivButton::release(e);
}
*/

HiLightButton::HiLightButton(QWidget* parent, const char* script_)
:QPushButton(parent)
{
  init(script_);
}

HiLightButton::HiLightButton(const String& text, QWidget* parent, const char* script_)
:QPushButton(text, parent)
{
  init(script_);
}

HiLightButton::~HiLightButton(){
}


void HiLightButton::init(const char* script_) {
  mhiLight = false;
  mouse_button = Qt::NoButton;
  if (script_ != NULL)
    mscript = script_;
  mhiLight_color = new iColor(0x66, 0xFF, 0x66); // medium-light green
}

void HiLightButton::released() {
  if (!mscript.empty()) {
    taMisc::RecordScript(mscript);
  }
  QPushButton::released();
}

void HiLightButton::setHiLight(bool value) {
  if (mhiLight == value) return;
  //TODO: hilighting!
  if (value) {
     setPaletteBackgroundColor(mhiLight_color);
  } else {
     setPaletteBackgroundColor(QApplication::palette().color(QPalette::Active, QColorGroup::Button));
  }
  mhiLight = value;
}

void HiLightButton::mousePressEvent(QMouseEvent* mev) {
  mouse_button = mev->button();
  if (mouse_button == Qt::RightButton) {
    ((QHackMouseEvent*)mev)->setButton(Qt::LeftButton);
  }
  QPushButton::mousePressEvent(mev);
}

void HiLightButton::mouseReleaseEvent(QMouseEvent* mev) {
  mouse_button = mev->button();
  if (mouse_button == Qt::RightButton) {
    ((QHackMouseEvent*)mev)->setButton(Qt::LeftButton);
  }
  QPushButton::mouseReleaseEvent(mev);
}

void HiLightButton::mouseMoveEvent(QMouseEvent*  mev) {
  mouse_button = mev->button();
  if (mouse_button == Qt::RightButton) {
    ((QHackMouseEvent*)mev)->setButton(Qt::LeftButton);
  }
  QPushButton::mouseMoveEvent(mev);
}


//////////////////////////////////
// 	iContextLabel		//
//////////////////////////////////

iContextLabel::iContextLabel(QWidget* parent, const char* name)
:QLabel(parent, name)
{
  mindex = -1;
}

iContextLabel::iContextLabel(int index_, const String& text, QWidget* parent, const char* name)
:QLabel(text, parent, name)
{
  mindex = index_;
}

void iContextLabel::contextMenuEvent (QContextMenuEvent* e) {
  emit contextMenuInvoked(this, e);
};


//////////////////////////////////////////////////////////
// 		taiChoiceDialog				//
//////////////////////////////////////////////////////////

int taiChoiceDialog::ChoiceDialog(QWidget* win, const char* prompt,
	const char* win_title, bool no_cancel)
{
  if (win == NULL) win = qApp->mainWidget();
  taiChoiceDialog* dlg = new taiChoiceDialog(win, prompt, win_title, no_cancel);
  // show the dialog
  int rval = dlg->exec();
  delete dlg;
  return rval;
}

void taiChoiceDialog::ErrorDialog(QWidget* parent_, const char* msg, const char* win_title)
{
     QMessageBox::warning(parent_, win_title, msg);
}

taiChoiceDialog::taiChoiceDialog(QWidget* par, const char* prompt,
	const char* win_title, bool no_cancel_)
  : QDialog(par, 0, true)
{
  iSize ss = taiM->scrn_s;
  setMaximumSize(ss.width(), ss.height());
  setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

  no_cancel = no_cancel_;
  // Main layout
  vblMain = new QVBoxLayout(this);
  // Text area
  txtMessage = new QLabel(this);
  txtMessage->setMinimumSize(400, 130); //TODO: could maybe be set based on display size
  txtMessage->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  vblMain->add(txtMessage);
  //txtMessage->show();

  // Button layout, and (hidden)button group
  hblButtons = new QHBoxLayout();
  hblButtons->setSpacing(5); // TODO: button spacing should be central constant
  vblMain->addLayout(hblButtons);
  bgChoiceButtons = new QButtonGroup(this);
  bgChoiceButtons->hide(); // we just using it for organizing and signalling

  connect(bgChoiceButtons, SIGNAL(clicked(int)),
      this, SLOT(done(int)) );

  setCaption(win_title);
  String prompt_str(prompt);
  String blabs = prompt_str.after('!');
  prompt_str = prompt_str.before('!');
  txtMessage->setText(prompt_str);

  // create buttons

  int curId = 0;
  hblButtons->addStretch();
  while (Constr_OneBut(blabs, curId)) {
    ++curId;
  }
  hblButtons->addStretch();
}

bool taiChoiceDialog::Constr_OneBut(String& lbl, int curId) {
  if(lbl.length() == 0)
    return false;
  String blab = lbl.before('!');
  if(blab == "") {
    blab = lbl;
    lbl = "";			// done next time, anyway
  }
  else
    lbl = lbl.after('!');
  if(blab == "")
    return false;

  if(blab[0] == ' ')		// allow for one space..
    blab = blab.after(' ');

  QPushButton* but = new QPushButton(blab, this);
  hblButtons->add(but);
  bgChoiceButtons->insert(but);
//  but->show();

  // set first button to be default for dialog
  if (curId == 0) {
    but->setDefault(true);
    but->setAutoDefault(true);
  }

  // set 2nd button to be cancel for dialog
  if ((!no_cancel) && (curId == 1)) {
    connect(but, SIGNAL(clicked()),
      this, SLOT(reject()));
  }
  return true;
}

void taiChoiceDialog::accept() {
   // override -- it should return 0 for our purpose (not 1)
  done(0);
}

void taiChoiceDialog::keyPressEvent(QKeyEvent* ev) {
  // we allow 0-n numeric keys to be accelerators for the buttons
  int key_ascii = ev->ascii();
  if ((key_ascii >= '0') && (key_ascii <= '9')) {
    int but_index = key_ascii - '0';
    QPushButton* but = (QPushButton*)bgChoiceButtons->find(but_index);
    if (but != NULL) {
      // simulate effect of pressing the button
      done(but_index);
      ev->accept(); // swallow key
    }
  } else {
    QDialog::keyPressEvent(ev);
  }
}

void taiChoiceDialog::reject() {
  // this is the easiest way to override the Esc behavior of choosing button 1
  if (!no_cancel)
    done(1);
}


//////////////////////////
// 	Dialog	//
//////////////////////////

Dialog::Dialog(taiDataHost* owner_, QWidget* parent, bool modal, WFlags f)
:QDialog(parent, NULL, modal, f) //TODO: NULL is for name -- remove for Qt4
{
  // set maximum size -- we will manually size the central widget (which should be a scrollbox)
  iSize ss = taiM->scrn_s;
  setMaximumSize(ss.width(), ss.height());
  setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
  resize(1,1);

  owner = owner_;
  mcentralWidget = NULL;
  was_accepted = false;
}

Dialog::~Dialog() {
  if (owner != NULL) {
    owner->dialog = NULL;
    owner = NULL;
  }
  taiMisc::active_wins.Remove(this); // to really be certain...
}

void Dialog::closeEvent(QCloseEvent* ev) {
  ev->accept(); // default, unless we override;
  if (owner == NULL) return;
  if (owner->HasChanged()) {
    int chs = taMisc::Choice("Changes have not been applied", "&Apply", "A&bandon Changes", "&Cancel");
    switch (chs) {
    case  2:
      ev->ignore();
      return;
      break;
    case  0:
      owner->GetValue();
      owner->state = taiDataHost::ACCEPTED;
      break;
    default:
      owner->state = taiDataHost::CANCELED;
      break;
    }
  } else { //!owner->modified
    owner->state = taiDataHost::CANCELED;
  }
  if (ev->isAccepted()) {
     taiMisc::active_wins.Remove(this);
  }
}

bool Dialog::post(bool modal) {
  taiMisc::active_wins.Add(this);
  if (modal) {
    return (exec() == 0);
  } else {
    show();
    return true;
  }
}

void Dialog::accept() {
   // override -- it should return 0 for our purpose (not 1)
  was_accepted = true;
  done(0);
}

void Dialog::reject() {
  // this is the easiest way to override the Esc behavior of choosing button 1
//  if (!no_cancel)
    done(1);
}
void Dialog::dismiss(bool accept_) {
  if (accept_)
    accept();
  else
    reject();
}

void Dialog::iconify() {
     // Iv compatibility routine
  if (isMinimized()) return;
  showMinimized();
}

void Dialog::deiconify() {
   // Iv compatibility routine
  if (!isMinimized()) return;
  showNormal();
}

void Dialog::resizeEvent(QResizeEvent* ev) {
  QDialog::resizeEvent(ev);
  // note: we already have our new geometry at this point
  if (mcentralWidget)
    mcentralWidget->resize(size());
}

void Dialog::setCentralWidget(QScrollView* widg) {
  mcentralWidget = widg;
  widg->reparent(this, QPoint(0,0), false);
  widg->show(); // layout should occur here
  qApp->processEvents();
  // size ourself to exactly fit the widget contents, except constrain widget to be our max size
  // note: the 4's are tweak factors for scrollbox
  QSize sz = QSize(widg->contentsWidth() + 4, widg->contentsHeight() + 4).boundedTo(maximumSize());
  setMinimumSize(sz);
  resize(sz);
}

//////////////////////////////////
// 	EditDataPanel		//
//////////////////////////////////

EditDataPanel::EditDataPanel(taiEditDataHost* owner_, taiDataLink* dl_)
:inherited(dl_)
{
  owner = owner_;
  mcentralWidget = NULL;
}

EditDataPanel::~EditDataPanel() {
  if (owner != NULL) {
    owner->panel = NULL;
    owner = NULL;
  }
}

void EditDataPanel::Closing(bool forced, bool& cancel) {
  if (owner == NULL) return;
  if (owner->HasChanged()) {
    int chs;
    if (forced)
      chs = taMisc::Choice("This Edit Panel is closing but changes have not been applied", "&Apply", "A&bandon Changes");
    else
      chs = taMisc::Choice("Changes have not been applied", "&Apply", "A&bandon Changes", "&Cancel");
    switch (chs) {
    case 0: // Apply
      owner->GetValue();
      owner->state = taiDataHost::ACCEPTED;
      break;
    case 1: // Abandon
      owner->state = taiDataHost::CANCELED;
      break;
    case  2: // Cancel (non-forced only)
      cancel = true;
      return;
      break;
    }
  } else { //!owner->modified
    owner->state = taiDataHost::CANCELED;
  }
}

void EditDataPanel::GetImage_impl() {
  if (owner) owner->GetImage();
}

const iColor*  EditDataPanel::GetTabColor(bool selected) const {
  if (owner) return (selected) ? owner->bg_color : owner->bg_color_dark;
  else       return inherited::GetTabColor(selected);
}

bool EditDataPanel::HasChanged() {
  if (owner) return owner->HasChanged();
  else       return false;
}

String EditDataPanel::panel_type() const {
  static String str("Properties");
  return str;
}

void EditDataPanel::resizeEvent(QResizeEvent* ev) {
  inherited::resizeEvent(ev);
  // note: we already have our new geometry at this point
  if (mcentralWidget)
    mcentralWidget->resize(size());
}

void EditDataPanel::setCentralWidget(QScrollView* widg) {
  mcentralWidget = widg;
  widg->resize(size());
  widg->reparent(this, QPoint(0,0), false);
  widg->show(); // layout should occur here
//nn  qApp->processEvents();
}


//////////////////////////////////
// 	taiDataHost		//
//////////////////////////////////

#define LAYBODY_MARGIN	1
#define LAYBODY_SPACING	0

void taiDataHost::DeleteChildrenLater(QObject* obj) {
  if (obj == NULL) return;
  QObjectList* ol = (QObjectList*)(obj->children()); //unconstify it
  if (ol == NULL) return;
  for (int i = ol->count() - 1; i >= 0; --i) {
    QObject* chobj = ol->at(i);
    chobj->deleteLater(); // deleted in event loop
  }
}

taiDataHost::taiDataHost(TypeDef* typ_, bool read_only_, bool modal_, QObject* parent)
:QObject(parent)
{
  read_only = read_only_;
  use_show = false;
  modified = false;
  typ = typ_;
  cur_base = NULL;

  prompt = NULL;
  scrDialog = NULL;
  vblDialog = NULL;
  frmMethButtons = NULL;
  layMethButtons = NULL;
  showMethButtons = false;
  hblButtons = NULL;
  okbut = NULL;
  canbut = NULL;
  apply_but = NULL;
  revert_but = NULL;
  body = NULL;
  splBody = NULL;
  scrBody = NULL;
  layBody = NULL;

  bg_color = new iColor(); //value set later
  bg_color_dark = new iColor(); //value set later
  if (taiM == NULL) ctrl_size = taiMisc::sizMedium;
  else              ctrl_size = taiM->ctrl_size; // for early type system instance when no taiM yet
  row_height = 1; // actual value set in Constr
  mouse_button = 0;
  cur_base = NULL;
  cur_row = 0;
  no_revert_hilight = false;
  modal = modal_;
  no_ok_but = false;
  dialog = NULL;
  mwidget = NULL;
  state = EXISTS;
  sel_item_index = -1;
  rebuild_body = false;
}

taiDataHost::~taiDataHost() {
  if (dialog != NULL) DoDestr_Dialog(dialog);
  if (bg_color != NULL) {
    delete bg_color;
    bg_color = NULL;
  }
  if (bg_color_dark != NULL) {
    delete bg_color_dark;
    bg_color_dark = NULL;
  }
}

void taiDataHost::setBgColor(const iColor* new_bg) {
  if (new_bg == NULL) {
    // get from default pallette
    bg_color->set(QApplication::palette().color(QPalette::Active, QColorGroup::Background));
  } else {
    bg_color->set(new_bg);
  }

  bg_color_dark->set(taiMisc::ivBrightness_to_Qt_lightdark(*bg_color, taiM->edit_darkbg_brightness));
}

const iColor* taiDataHost::colorOfRow(int row) const {
  if ((row % 2) == 0) {
    return bg_color;
  } else {
    return bg_color_dark;
  }
}

int taiDataHost::AddName(int row, const String& name, const String& desc, QWidget* buddy) {
  iContextLabel* label = new iContextLabel(row, name, body);
  label->setFixedHeight(taiM->label_height(ctrl_size));
  label->setPaletteBackgroundColor(*colorOfRow(row));
  connect(label, SIGNAL(contextMenuInvoked(iContextLabel*, QContextMenuEvent*)),
      this, SLOT(label_contextMenuInvoked(iContextLabel*, QContextMenuEvent*)) );

  if (!desc.empty()) {
    QToolTip::add(label, desc);
    if (buddy != NULL) {
      QToolTip::add(buddy, desc);
    }
  }
  // add a label item in first column
  if (row < 0)
    row = layBody->numRows();
  if (layBody->numRows() < (row + 1)) {
    layBody->expand(row + 1, 2);
  }
  layBody->setRowSpacing(row, row_height + (2 * LAYBODY_MARGIN)); //note: margins not automatically baked in to max height
  layBody->addWidget(label, row, 0, (Qt::AlignLeft | Qt::AlignVCenter));
  layBody->addItem(new QSpacerItem(2, row_height), row, 0);
  label->show(); // needed for rebuilds, to make the widget show
  return row;
}
int taiDataHost::AddData(int row, QWidget* data) {
   // add a data item in second column
    if (row < 0)
      row = layBody->numRows();
    if (layBody->numRows() < (row + 1)) {
      layBody->expand(row + 1, 2);
    }
    layBody->setRowSpacing(row, row_height + (2 * LAYBODY_MARGIN)); //note: margins not automatically baked in to max height
    QHBoxLayout* hbl = new QHBoxLayout();
    layBody->addLayout(hbl, row, 1);
    hbl->addWidget(data, 0,  (Qt::AlignLeft | Qt::AlignVCenter));
    hbl->addStretch();
    data->show(); // needed for rebuilds, to make the widget show
    return row;
}

void taiDataHost::AddMultiRowName(iEditGrid* multi_body, int row, const String& name, const String& desc) {
  SetMultiSize(row + 1, 0); //0 gets set to multi_col
  QLabel* label = new QLabel(name, (QWidget*)NULL);
  label->setFixedHeight(taiM->label_height(ctrl_size));
  label->setPaletteBackgroundColor(*colorOfRow(row));
  if (!desc.empty()) {
    QToolTip::add(label, desc);
  }
  multi_body->setRowNameWidget(row, label);
  label->show(); //required to show when rebuilding
}

void taiDataHost::AddMultiColName(iEditGrid* multi_body, int col, const String& name, const String& desc) {
  SetMultiSize(0, col + 1); // 0 gets set to multi_rows
  QLabel* label = new QLabel(name, (QWidget*)NULL);
  if (!desc.empty()) {
    QToolTip::add(label, desc);
  }
  multi_body->setColNameWidget(col, label);
  label->show(); //required to show when rebuilding
}

void taiDataHost::AddMultiData(iEditGrid* multi_body, int row, int col, QWidget* data) {
  SetMultiSize(row - 1, col - 1);
  QHBoxLayout* hbl = new QHBoxLayout();
  hbl->addWidget(data, 0,  (Qt::AlignLeft | Qt::AlignVCenter));
  hbl->addStretch();
  multi_body->setDataLayout(row, col, hbl);
  data->show(); //required to show when rebuilding
}

void taiDataHost::Apply() {
  no_revert_hilight = true;
  GetValue();
  GetImage();
  Unchanged();
  no_revert_hilight = false;
}

void taiDataHost::BodyCleared() { // called when event loop clears last widget from body
  if (!(state & SHOW_CHANGED)) return; // probably just destroying
  ReConstr_Body();
  state &= ~SHOW_CHANGED;
}

void taiDataHost::Cancel() { //note: taiEditDataHost takes care of cancelling panels
  state = CANCELED;
  if (isDialog()) {
    dialog->dismiss(0);
  }
}

void taiDataHost::Changed() {
  if (modified) return; // handled already
  modified = true;
  if (no_revert_hilight) return;
  if (apply_but != NULL) {
      apply_but->setEnabled(true);
      apply_but->setHiLight(true);

  }
  if (revert_but != NULL) {
      revert_but->setEnabled(true);
  }
}

void taiDataHost::ClearBody() {
  rebuild_body = true;
  ClearBody_impl();
}

void taiDataHost::ClearBody_impl() {
  // now request for delete of the constructed body widgets
  // we must do deferred delete, because Qt specifies that it is dangerous to delete widgets for
  // whom there may be events outstanding -- deleteLater puts the object in the event loop queue
  // for deletion
  DeleteChildrenLater(body);
}

/* NOTE: Constr_Xxx methods presented in execution (not lexical) order */
void taiDataHost::Constr(const char* aprompt, const char* win_title, const iColor* bgclr, bool as_panel) {
  is_panel = as_panel;
  setBgColor(bgclr);
  row_height = taiM->max_control_height(ctrl_size); // 3 if using line between; 2 if using even/odd shading
  Constr_Strings(aprompt, win_title);
  Constr_impl();
  state = CONSTRUCTED;
}

void taiDataHost::Constr_Strings(const char* prompt, const char* win_title) {
  prompt_str = prompt;
  win_str = win_title;
}

void taiDataHost::Constr_impl() {
  Constr_WinName();
  Constr_Widget();
  widget()->setUpdatesEnabled(false);
  Constr_Prompt();
  Constr_Box();
  Constr_Body();
  Constr_Methods();
  // create container for ok/cancel/apply etc. buttons
  hblButtons = new QHBoxLayout();
//  vblDialog->addStretch(100); // provides a degree of freedom for small body heights -- all other strechfactors=0
  vblDialog->addStretch();
  vblDialog->addSpacing(taiM->vsep_c);
  vblDialog->addLayout(hblButtons);
  vblDialog->addSpacing(taiM->vsep_c);
  Constr_Buttons();
  Constr_Final();
  widget()->setUpdatesEnabled(true);
//NOTE: do NOT do a processevents -- it causes improperly nested event calls
// in some cases, such as constructing the browser  qApp->processEvents();
}

void taiDataHost::Constr_Widget() {
  if (mwidget != NULL) return;
//TESTING:
  scrDialog = new QScrollView();
  scrDialog->setResizePolicy(QScrollView::AutoOneFit);
  mwidget = new QWidget(scrDialog->viewport());
  scrDialog->addChild(mwidget);
  if (bg_color != NULL) {
    widget()->setPaletteBackgroundColor(*bg_color);
  }
  widget()->setFont(taiM->dialogFont(ctrl_size));
  vblDialog = new QVBoxLayout(widget());
}

void taiDataHost::Constr_WinName() {
  if (win_str.length() > 50)
      win_str = win_str.from((int)(win_str.length() - 50));
}

void taiDataHost::Constr_Prompt() {
  if (prompt != NULL) return; // already constructed
  // convert to html-ish format, for display
  QString s = QStyleSheet::convertFromPlainText(prompt_str);
  prompt = new QLabel(s, widget());
  prompt->font().setBold(true);
  prompt->setTextFormat(Qt::RichText);
//  prompt->setFixedHeight(taiM->label_height(ctrl_size));
  prompt->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  vblDialog->addWidget(prompt);
  QFrame* line = new QFrame(widget());
  // add a separator line
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vblDialog->add(line);
}

void taiDataHost::Constr_Box() {
  //note: see also gpiMultiEditDialog::Constr_Box, if changes made to this implementation
  //note: see ClearBody for guards against deleting the structural widgets when clearing
  QWidget* scr_par = (splBody == NULL) ? widget() : splBody;
  scrBody = new QScrollView(scr_par);
  scrBody->viewport()->setPaletteBackgroundColor(*bg_color_dark);
  scrBody->setResizePolicy(QScrollView::AutoOneFit);
  body = new iStripeWidget(scrBody->viewport());
  scrBody->addChild(body);
  if (bg_color != NULL) {
    body->setPaletteBackgroundColor(*bg_color);
    if (bg_color_dark != NULL)
      ((iStripeWidget*)body)->setHiLightColor(*bg_color_dark);
    ((iStripeWidget*)body)->setStripeHeight(row_height + (2 * LAYBODY_MARGIN));
    //TODO: if adding spacing, need to include LAYBODY_SPACING;

  }
  if (splBody == NULL) vblDialog->addWidget(scrBody);
  //note: the layout is added in Constr_Body, because it gets deleted when we change the 'show'
}


void taiDataHost::Constr_Body() {
  QVBoxLayout* vbl = new QVBoxLayout(body);
  layBody = new QGridLayout((int)1, 2, LAYBODY_SPACING); //margin, space between
  layBody->setMargin(LAYBODY_MARGIN);
  vbl->addLayout(layBody);
  vbl->addStretch(1);
  // since vbl is first object in body, we use its deletion after showchange to indicate we can rebuild
  connect(vbl, SIGNAL(destroyed()), this, SLOT(BodyCleared()) );
}
void taiDataHost::Constr_Methods() { //note: conditional constructions used by SelectEditHost to rebuild methods
  QFrame* tmp = frmMethButtons;
  if (frmMethButtons == NULL) {
    showMethButtons = false; // set true if any created
    tmp = new QFrame(widget());
    tmp->setFrameStyle( QFrame::GroupBoxPanel | QFrame::Sunken );
    tmp->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  }
  if (layMethButtons == NULL) {
    layMethButtons = new iFlowLayout(tmp, 5, taiM->hspc_c, (Qt::AlignCenter | Qt::AlignTop)); // margin, space, align
  }
  if (frmMethButtons == NULL) {
    frmMethButtons = tmp;
    vblDialog->addSpacing(taiM->vspc_c);
    vblDialog->addWidget(frmMethButtons);
  }
}

void taiDataHost::Constr_Buttons() {
  QWidget* par = widget();
  hblButtons->addStretch();
  if (isDialog()) { // add dialog-like buttons
    if(!modal && no_ok_but) {
      okbut = NULL;
    }
    else {
      if (read_only) {
        okbut = new HiLightButton("&Close", par); //note: ok to reuse C as accelerator, because effect is same as Cancel
      } else {
        okbut = new HiLightButton("&Ok", par);
      }
      hblButtons->addWidget(okbut, 0, (Qt::AlignVCenter));
      connect(okbut, SIGNAL(clicked()),
          this, SLOT(Ok()) );
    }
    if (read_only) {
      canbut = NULL;
    }
    else {
      canbut = new HiLightButton("&Cancel", par);
      hblButtons->addWidget(canbut, 0, (Qt::AlignVCenter));
      connect(canbut, SIGNAL(clicked()),
          this, SLOT(Cancel()) );
    }
  }

  if (modal) {
    apply_but = NULL;
    revert_but = NULL;
  } else {
     // dont' put apply/revert buttons on a readonly dialog!
    if (!read_only) {
      hblButtons->addSpacing(20); // TODO: parameterize, and give it some stretchiness
      apply_but = new HiLightButton("&Apply", par);
      hblButtons->addWidget(apply_but, 0, (Qt::AlignVCenter));
      connect(apply_but, SIGNAL(clicked()),
          this, SLOT(Apply()) );
      revert_but = new HiLightButton("&Revert", par);
      hblButtons->addWidget(revert_but, 0, (Qt::AlignVCenter));
      connect(revert_but, SIGNAL(clicked()),
          this, SLOT(Revert()) );
    }
    Unchanged();
  }
  hblButtons->addSpacing(10); // don't flush hard right
}

void taiDataHost::Constr_Final() {
  // we put all the stretch factor setting here, so it is easy to make code changes if necessary
/*stretch not needed
  vblDialog->setStretchFactor(prompt, 1);
  vblDialog->setStretchFactor(scrBody, 0); //note: won't be in layout for list/group edits, so this call will just be noop
  vblDialog->setStretchFactor(frmMethButtons, 1);
  vblDialog->setStretchFactor(hblButtons, 1); //note: layout, not widget
*/
  if (splBody) vblDialog->setStretchFactor(splBody, 1);
  else         vblDialog->setStretchFactor(scrBody, 1);

  frmMethButtons->setHidden(!showMethButtons);
}

void taiDataHost::label_contextMenuInvoked(iContextLabel* sender, QContextMenuEvent* e) {
  QPopupMenu* menu = new QPopupMenu(widget());
  //note: don't use body for menu parent, because some context menu choices cause ReShow, which deletes body items!
  Q_CHECK_PTR(menu);
  int last_id = -1;
  FillLabelContextMenu(sender, menu, last_id);
  if (menu->count() > 0)
    menu->exec(sender->mapToGlobal(e->pos()));
  delete menu;
}

void taiDataHost::DoConstr_Dialog(Dialog*& dlg) {
  // common subcode for creating a dialog -- used by the taiDialog and taiEditDialog cousin classes
  if (dlg != NULL) return; // already constructed
  dlg = new Dialog(this, qApp->activeWindow(), modal); //NOTE: dialogs should be parented to main app or
  // another top-level window, otherwise X can't seem to handle more than 12-14 windows
  dlg->setCaption(win_str);
  //TODO: max_w seems to span both monitors of a dual head system!!!
  dlg->setMinimumWidth(400); //TODO: maybe parameterize; note: would need to set layout FreeResize as well
}

void taiDataHost::DoDestr_Dialog(Dialog*& dlg) { // common sub-code for destructing a dialog instance
  if (dlg != NULL) {
    dlg->owner = NULL; // prevent reverse deletion
    dlg->close(true); // destructive close
    dlg = NULL;
  }
}

void taiDataHost::DoRaise_Dialog() {
  if (!isDialog()) return;
  if (!modal) {
    Dialog* dlg = (Dialog*)widget();
    dlg->raise();
    dlg->setFocus();
  }
}

int taiDataHost::Edit(bool modal_) { // only called if isDialog() true
  if (state != CONSTRUCTED)
    return false;
  modal = modal_;
  if (dialog == NULL) DoConstr_Dialog(dialog);
//dialog->resize(dialog->minimumWidth(), 1);
  dialog->setCentralWidget(scrDialog);
  if (!modal) {
    taiMisc::active_dialogs.AddUnique(this); // add to the list of active dialogs
  }
  state = ACTIVE;
  return dialog->post(modal);
}

void taiDataHost::FillLabelContextMenu(iContextLabel* sender, QPopupMenu* menu, int& last_id) {
  sel_item_index = sender->index();
}

void taiDataHost::Iconify(bool value) {
  if (!isDialog()) return;
  Dialog* dlg = (Dialog*)widget();
  if (!dlg) return;
  if (value) dlg->iconify();
  else       dlg->deiconify();
}

void taiDataHost::Ok() { //note: only used for Dialogs
  //note: IV version used to scold user for pressing Ok in a readonly dialog --
  // we just interpret Ok as Cancel
  if (read_only) {
    Cancel();
    return;
  }
  if (HasChanged())
    GetValue();
  state = ACCEPTED;
  mouse_button = okbut->mouse_button;
  if (isDialog()) {
    dialog->dismiss(1);
  }
}

void taiDataHost::ReConstr_Body() {
  Constr_Body();
  GetImage();
}

void taiDataHost::Revert() {
  GetImage();
  Unchanged();
}

void taiDataHost::Revert_force() {
  if (modified && (taMisc::auto_revert == taMisc::CONFIRM_REVERT)) {
    int chs = taiChoiceDialog::ChoiceDialog
      (NULL, "Revert: You have edited the data -- apply, or revert and lose changes?!Apply!Revert!Cancel!");
    if(chs == 2)
      return;
    if(chs == 0) {
      Apply();
      return;
    }
  }
  Unchanged();
  Revert();			// use real revert to be sure..
}
void taiDataHost::Unchanged() {
  modified = false;
  //TODO: set gui to unchanged state
  if (apply_but != NULL) {
      apply_but->setEnabled(false);
      apply_but->setHiLight(false);
  }
  if (revert_but != NULL) {
      revert_but->setEnabled(false);
      revert_but->setHiLight(false);
  }
}


//////////////////////////////////
// 	taiDialog		//
//////////////////////////////////

/* OBS:
class taiDButton : public ivButton {
public:
  taiDialog* diag;
  virtual void release(const ivEvent&);
  static taiDButton* GetButton(taiDialog* d, const char* s, ivAction* a);
  taiDButton(taiDialog* d, ivGlyph* g, ivStyle* s, ivTelltaleState* t, ivAction* a);
};

taiDButton::taiDButton(taiDialog* d, ivGlyph* g, ivStyle* s, ivTelltaleState* t, ivAction* a)
: ivButton(g,s,t,a) {
  diag = d;
}

taiDButton* taiDButton::GetButton(taiDialog *d, const char* s, ivAction* a){
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivTelltaleState* t = new ivTelltaleState(ivTelltaleState::is_enabled);
  return new taiDButton(d,wkit->default_button_look(wkit->label(s),t),
		      wkit->style(),t,a);
}

void taiDButton::release(const ivEvent& e) {
  diag->mouse_button = taiM->GetButton(e);
  ivButton::release(e);
}
*/
// **TODO maybe each dialog should have its own style so that
// it could update its name ect...
//ivStyle*     taiDialog::style = NULL;

/*taiDialog::taiDialog(TypeDef* typ_, bool modal_, bool read_only_, QObject* parent, const char* name)
: taiDataHost(typ_, read_only_, parent, name)
{
  dialog = NULL;
  modal = modal_;
  no_ok_but = false;
  def_size = 0;
}

taiDialog::~taiDialog() {
  DoDestr_Dialog(dialog);
} */

/* table approach not very successful -- cells are fixed size, and contained widgets are resized
 ugh, tables!
void  taiDialog::Constr_Box() {
  body = new QTable(1, 1, dialog);
  body->horizontalHeader()->hide();
  QHeader* vh = body->verticalHeader();
  vh->setResizeEnabled(false);
  vh->setMovingEnabled(false);
  vh->setClickEnabled(false);
  body->resize(1, 1); // let it expand
  body->setPaletteBackgroundColor(*bg_color_dark); //TODO: replace/nuke when we use Palettes
  vblDialog->addWidget(body, 2);
}


void taiDialog::AddName(int index, String& name, String& desc, QWidget* buddy) {
   // add a data item in data column
    if (body->numRows() < (index + 1))
      body->setNumRows(index + 1);
    body->verticalHeader()->setLabel(index, name);
    // set a tooltip on buddy, if supplied
    if ((buddy != NULL) && (!desc.empty())) {
      QToolTip::add(buddy, desc);
    }

}

void taiDialog::AddData(int index, QWidget* data) {
   // add a data item in data column
    if (body->numRows() < (index + 1))
      body->setNumRows(index + 1);
    body->setCellWidget(index, 0, data);
    int h;
//    h = MAX(data->height(), body->rowHeight(index));
    h = 37; // temp -- button height, s/b highest
    body->setRowHeight(index, h);
} */


void taiDataHost::SetRevert(){
  if((no_revert_hilight == true) || (taMisc::is_loading)) return;
  if(revert_but == NULL) return;
  revert_but->setHiLight(true);
  revert_but->setEnabled(true);
}

void taiDataHost::UnSetRevert(){
  if(revert_but == NULL) return;
  revert_but->setHiLight(false);
  revert_but->setEnabled(false);
}



//////////////////////////////////
// 	taiEditDataHost		//
//////////////////////////////////


taiEditDataHost::taiEditDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
:taiDataHost(typ_, read_only_, modal_, parent)
{
  cur_base = base;
  use_show = true; // some descendant classes override, as well as construction methods
  show = taMisc::show_gui;
  cur_menu = NULL;
  cur_menu_but = NULL;
  show_menu = NULL;
  menu = NULL;
  panel = NULL;
}

taiEditDataHost::~taiEditDataHost() {
  data_el.Reset();
  meth_el.Reset();
  taiMisc::active_edits.Remove(this);
  taiMisc::css_active_edits.Remove(this);
}

void taiEditDataHost::AddMethButton(taiMethodData* mth_rep, const char* label) {
  QPushButton* but = mth_rep->GetButtonRep();
  DoAddMethButton(but);
  if (label != NULL) {
    but->setText(label);
  }
}

void taiEditDataHost::Cancel() {
  state = CANCELED;
  if (isPanel()) {
    if (panel != NULL)
      panel->ClosePanel();
  } else taiDataHost::Cancel();
}

void taiEditDataHost::ClearBody_impl() {
  // delete the data items -- Qt will automatically disconnect the signals/slots
  memb_el.Reset();
  data_el.Reset();
  taiDataHost::ClearBody_impl(); // deletes the body widgets, except structural ones
}

/* IV version:
void taiEditDataHost::GetMembDescRep(MemberDef* md, ivMenu* dscm, String indent) {
  String desc = md->desc;
  String defval = md->OptionAfter("DEF_");
  if(!defval.empty())
    desc = String("[Default: ") + defval + "] " + desc;
  else
    desc = desc;
  if(!indent.empty())
    desc = indent + md->GetLabel() + String(": ") + desc;
  dscm->append_item
    (taiM->wkit->menu_item
     (new ivLabel((char*)desc, taiM->name_font, taiM->font_foreground)));
  if(md->type->InheritsFormal(TA_class) &&
     (md->type->HasOption("INLINE") || md->type->HasOption("EDIT_INLINE"))) {
    indent += "  ";
    int i;
    for(i=0; i<md->type->members.size; i++) {
      MemberDef* smd = md->type->members.FastEl(i);
      if(!smd->ShowMember(taMisc::show_iv) || smd->HasOption("HIDDEN_INLINE"))
	continue;
      GetMembDescRep(smd, dscm, indent);
    }
  }
  else if(md->type->InheritsFormal(TA_enum)) {
    int i;
    for(i=0; i<md->type->enum_vals.size; i++) {
      EnumDef* ed = md->type->enum_vals.FastEl(i);
      if(ed->desc.empty() || (ed->desc == " ") || (ed->desc == "  ")) continue;
      desc = indent + "  " + ed->GetLabel() + String(": ") + ed->desc;
      dscm->append_item
	(taiM->wkit->menu_item
	 (new ivLabel((char*)desc, taiM->name_font, taiM->font_foreground)));
    }
  }
}
*/

/*obsQWidget* taiEditDataHost::GetNameRep(MemberDef* md, QWidget* dataWidget) {
// TODO: get compound description, and if too long, then create a hyperlink-like label
// that pops up a little tooltip-style window
  String nm;
  String desc;
  GetName(md, nm, desc);
  QLabel* rval = new QLabel(nm, body);
  rval->setFixedHeight(taiM->label_height(ctrl_size));
  if (dataWidget != NULL) {
    rval->setBuddy(dataWidget);
    // set tool tip in buddy widget
    if (!desc.empty()) {
      QToolTip::add(dataWidget, desc);
    }
  }
  return rval;
} */

void taiEditDataHost::Constr_Body() {
  taiDataHost::Constr_Body();
  Constr_Data();
  Constr_Labels();
}

void taiEditDataHost::Constr_Data() {
  Constr_Data_impl(typ->members, &data_el);
}

void taiEditDataHost::Constr_Labels() {
  Constr_Labels_impl(typ->members, &data_el);
}

void taiEditDataHost::Constr_Labels_impl(const MemberSpace& ms, taiDataList* dl) {
  QWidget* rep;
  String name;
  String desc;
  int cnt = 0;
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.SafeEl(i);
    if (!ShowMember(md))
      continue;

    // Get data widget, if dl provided
    rep = NULL;
    if (dl != NULL) {
      taiData* mb_dat = dl->SafeEl(cnt);
      if (mb_dat != NULL)
        rep = mb_dat->GetRep();
    }

    // create label
    name = "";
    desc = "";
    GetName(md, name, desc);

    // add to layout
    AddName(cnt, name, desc, rep);
    ++cnt;
  }
}

void taiEditDataHost::Constr_Data_impl(const MemberSpace& ms, taiDataList* dl) {
  QWidget* rep;
  int cnt = 0;
  cur_row = 0;
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.FastEl(i);
    if (!ShowMember(md))
      continue;

    // Create data widget
    memb_el.Add(md);
    taiData* mb_dat = md->im->GetDataRep(this, NULL, body);
    dl->Add(mb_dat);
    rep = mb_dat->GetRep();

    AddData(cnt, rep);
    ++cnt;
    ++cur_row;
  }
}

void taiEditDataHost::Constr_Strings(const char* aprompt, const char* win_title) {
  prompt_str = typ->name;
  win_str = String(taiM->classname()) + ": " + win_title;
  if(typ->InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)cur_base;
    if(rbase->GetOwner() != NULL)
      win_str += String(" ") + rbase->GetPath();
    if(rbase->GetName() != "") {
      win_str += String(" (") + rbase->GetName() + ")";
      prompt_str = rbase->GetName() + " (" + typ->name + ")";
    }
    else
      win_str += String(" (") + typ->name + ")";
  }
  String sapr;
  if(aprompt != NULL) sapr = aprompt;
  if(!sapr.empty())
    prompt_str += ": " + sapr;
  else
    prompt_str +=  ": " + typ->desc;
}

void taiEditDataHost::Constr_Methods() {
  taiDataHost::Constr_Methods();
  Constr_Methods_impl();
}

void taiEditDataHost::Constr_Methods_impl() {
  if (typ == NULL) return;

  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((md->im == NULL) || (md->name == "Edit")) // don't put edit on edit dialogs..
      continue;
    taiMethodData* mth_rep = md->im->GetMethodRep(cur_base, this, NULL, frmMethButtons); //buttons are in the frame
    if (mth_rep == NULL)
      continue;

    meth_el.Add(mth_rep);
    // add to menu if a menu item
    if (mth_rep->is_menu_item) {
      if(md->HasOption("MENU_BUTTON")) {
      	SetCurMenuButton(md);
        mth_rep->AddToMenu(cur_menu_but);
      } else {
      	SetCurMenu(md);
        mth_rep->AddToMenu(cur_menu);
      }
    } else {
      AddMethButton(mth_rep);
    }
  }
}

void taiEditDataHost::Constr_ShowMenu() {
  if (!use_show) return;
  if (menu == NULL) return;	// if don't even have a menu, bail
//note: ok in Qt version, TODO: delete this line  if (modal) return; // can't change what to show on modal dialog

  show_menu = menu->AddSubMenu("&Show");

  // first two items are commands that set the other toggle flags
  show_menu->AddItem("Normal &only", NULL, taiMenu::normal, taiMenuEl::men_act,
      this, SLOT(ShowChange(taiMenuEl*)) );
  show_menu->AddItem("&All", NULL, taiMenu::normal, taiMenuEl::men_act,
      this, SLOT(ShowChange(taiMenuEl*)) );
  show_menu->AddSep();
  show_menu->AddItem("&Normal", NULL, taiMenu::toggle, taiMenuEl::men_act,
      this, SLOT(ShowChange(taiMenuEl*)) );
  show_menu->AddItem("&Hidden", NULL, taiMenu::toggle, taiMenuEl::men_act,
      this, SLOT(ShowChange(taiMenuEl*)) );
  show_menu->AddItem("&Read Only", NULL, taiMenu::toggle, taiMenuEl::men_act,
      this, SLOT(ShowChange(taiMenuEl*)) );
  show_menu->AddItem("&Detail", NULL, taiMenu::toggle, taiMenuEl::men_act,
      this, SLOT(ShowChange(taiMenuEl*)) );
  setShowValues(show); // sets toggles
}

void taiEditDataHost::Constr_Final() {
  taiDataHost::Constr_Final();
  Constr_ShowMenu();
  GetImage();
}

void taiEditDataHost::DoAddMethButton(QPushButton* but) {
  showMethButtons = true;
  but->setFixedHeight(taiM->button_height(ctrl_size));
  if (but->parent() != frmMethButtons) {
    but->reparent(frmMethButtons, QPoint(0, 0));
  }
  layMethButtons->addWidget(but);
  but->show(); // needed when rebuilding
}

void taiEditDataHost::DoRaise_Panel() {
  //TODO
}

void taiEditDataHost::DoSelectForEdit(int param){
  //NOTE: this handler adds if not on, or removes if already on
  MemberDef* md = memb_el.SafeEl(sel_item_index);
  TypeDef* td = SelectEdit::StatTypeDef(0);
  SelectEdit* se = (SelectEdit*)td->tokens.SafeEl_(param);
  if ((md == NULL) || (se == NULL)) return; //shouldn't happen...
  int idx;
  if ((idx = se->FindMbrBase((taBase*)cur_base, md)) >= 0)
    se->RemoveField(idx);
  else
    se->SelectMember((taBase*)cur_base, md, "");
}

int taiEditDataHost::Edit(bool modal_) {
  if (!modal_)
    taiMisc::active_edits.Add(this); // add to the list of active edit dialogs
  return taiDataHost::Edit(modal_);
}

EditDataPanel* taiEditDataHost::EditPanel(taiDataLink* link) {
  if (state != CONSTRUCTED)
    return NULL;
  if (panel == NULL)
    panel = new EditDataPanel(this, link); //TODO: make sure this conversion is always valid!!!
  panel->setCentralWidget(scrDialog);
  taiMisc::active_edits.Add(this); // add to the list of active edit dialogs
  state = ACTIVE;
  return panel;
}

void taiEditDataHost::FillLabelContextMenu(iContextLabel* sender, QPopupMenu* menu, int& last_id) {
  taiDataHost::FillLabelContextMenu(sender, menu, last_id);
  FillLabelContextMenu_SelEdit(sender, menu, last_id);
}

void taiEditDataHost::FillLabelContextMenu_SelEdit(iContextLabel* sender, QPopupMenu* menu, int& last_id) {
  if ((cur_base == NULL) || (!typ->InheritsFrom(&TA_taBase))) return; // have to be a taBase to use SelEdit
  MemberDef* md = memb_el.SafeEl(sel_item_index);
  if (md == NULL) return;
  // save the index of the data item, for the handler routines
  sel_item_index = sender->index();
  // get list of select edits
  TypeDef* td = SelectEdit::StatTypeDef(0);
  if (td->tokens.size == 0) return;
  // if any edits, populate menu for adding, for all seledits not already on
  QPopupMenu* sub = new QPopupMenu(body);
  sub->setFont(menu->font());
  for (int i = 0; i < td->tokens.size; ++i) {
    SelectEdit* se = (SelectEdit*)td->tokens[i];
    sub->insertItem(se->GetName(), this, SLOT(DoSelectForEdit(int)), 0, i); // set id to i
    sub->setItemParameter(i, i); // sets param, which is what is passed in signal, to i
    // determine if already on that seledit, and disable if it is (we do this to maintain constant positionality in menu)
    if (se->FindMbrBase((taBase*)cur_base, md) >= 0)
      sub->setItemEnabled(i, false);
  }
  menu->insertItem("Add to SelectEdit", sub, ++last_id);
  if (sub->count() == 0)
    menu->setItemEnabled(last_id, false); // show item for usability, but disable
  // TODO: if any edits, populate menu for removing, for all seledits already on
  sub = new QPopupMenu(body);
  sub->setFont(menu->font());
  for (int i = 0; i < td->tokens.size; ++i) {
    SelectEdit* se = (SelectEdit*)td->tokens[i];
    sub->insertItem(se->GetName(), this, SLOT(DoSelectForEdit(int)), 0, i); // set id to i
    sub->setItemParameter(i, i); // sets param, which is what is passed in signal, to i
    // determine if already on that seledit, and disable if it isn't
    if (se->FindMbrBase((taBase*)cur_base, md) < 0)
      sub->setItemEnabled(i, false);
  }
  menu->insertItem("Remove from SelectEdit", sub, ++last_id);
  if (sub->count() == 0)
    menu->setItemEnabled(last_id, false); // show item for usability, but disable
}

void taiEditDataHost::GetButtonImage() {
  if(typ == NULL)
    return;
  for (int i = 0; i < meth_el.size; ++i) {
    taiMethMenu* mth_rep = (taiMethMenu*)meth_el.SafeEl(i);
    if ( !(mth_rep->hasButtonRep()) //note: construction forced creation of all buttons
       || (mth_rep->meth->OptionAfter("GHOST_") == "") )
      continue;
    String mbnm = mth_rep->meth->OptionAfter("GHOST_");
    String on_off = mbnm.before("_");
    mbnm = mbnm.after("_");
    MemberDef* mbrd = typ->members.FindName(mbnm);
    if((mbrd == NULL) || !(mbrd->type->InheritsFrom(TA_bool)
	|| mbrd->type->InheritsFrom(TA_int)))
      continue;

    QPushButton* but = mth_rep->GetButtonRep(); //note: always exists because hasButtonRep was true
    bool but_state;
    if (mbrd->type->InheritsFrom(TA_bool))
      but_state = *((bool*)mbrd->GetOff(cur_base));
    else
      but_state = *((int*)mbrd->GetOff(cur_base));
    if (on_off == "ON")
      but->setEnabled(but_state);
    else
      but->setEnabled(!but_state);
  }
}

void taiEditDataHost::GetImage() {
  GetImage_impl(typ->members, data_el, cur_base);
  GetButtonImage();
  Unchanged();
}

void taiEditDataHost::GetImage_impl(const MemberSpace& ms, const taiDataList& dl,
	void* base)
{
  int cnt = 0;
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.FastEl(i);
    if (!ShowMember(md))
      continue;
    taiData* mb_dat = dl.SafeEl(cnt++);
    if (mb_dat == NULL)
      taMisc::Error("taiEditDataHost::GetImage_impl(): unexpected dl=NULL at cnt ", String(cnt - 1), "\n");
    else
      md->im->GetImage(mb_dat, base);
  }
}

void taiEditDataHost::GetMembDesc(MemberDef* md, String& dsc_str, String indent) {
  String desc = md->desc;
  String defval = md->OptionAfter("DEF_");
  if(!defval.empty())
    desc = String("[Default: ") + defval + "] " + desc;
  else
    desc = desc;
  if(!indent.empty())
    desc = indent + md->GetLabel() + String(": ") + desc;
  if (!dsc_str.empty())
    dsc_str += "<br>";
  dsc_str += desc;
  if(md->type->InheritsFormal(TA_class) &&
     (md->type->HasOption("INLINE") || md->type->HasOption("EDIT_INLINE"))) {
    indent += "  ";
    for (int i=0; i < md->type->members.size; ++i) {
      MemberDef* smd = md->type->members.FastEl(i);
      if (!smd->ShowMember(show) || smd->HasOption("HIDDEN_INLINE"))
	continue;
      GetMembDesc(smd, dsc_str, indent);
    }
  } else if (md->type->InheritsFormal(TA_enum)) {
    for (int i = 0; i < md->type->enum_vals.size; ++i) {
      EnumDef* ed = md->type->enum_vals.FastEl(i);
      if (ed->desc.empty() || (ed->desc == " ") || (ed->desc == "  ")) continue;
      desc = indent + "  " + ed->GetLabel() + String(": ") + ed->desc;
      if (!dsc_str.empty())
        dsc_str += "<br>";
      dsc_str += desc;
    }
  }
}

void taiEditDataHost::GetName(MemberDef* md, String& name, String& desc) {
  name = md->GetLabel();
  desc = ""; // just in case
  GetMembDesc(md, desc, "");
}

void taiEditDataHost::GetValue() {
  GetValue_impl(typ->members, data_el, cur_base);
  if (typ->InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)cur_base;
    rbase->UpdateAfterEdit();	// hook to update the contents after an edit..
    taiMisc::Update(rbase);
  }
  GetButtonImage();
  Unchanged();
}

void taiEditDataHost::GetValue_impl(const MemberSpace& ms, const taiDataList& dl,
	void* base)
{
  bool first_diff = true;
  int cnt = 0;
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.FastEl(i);
    if (!ShowMember(md))
      continue;
    taiData* mb_dat = dl.SafeEl(cnt++);
    if (mb_dat == NULL)
      taMisc::Error("taiEditDataHost::GetValue_impl(): unexpected dl=NULL at cnt ", String(cnt - 1), "\n");
    else
      md->im->GetMbrValue(mb_dat, base, first_diff);
  }
  if (!first_diff)
    taiMember::EndScript(base);
}

bool taiEditDataHost::ReShow(bool force) {
  if (!force) {
      // get confirmation if changed, and possibly exit
    if (HasChanged()) {
      int chs = taMisc::Choice("Changes must be applied before rebuilding", "&Apply", "&Cancel");
      switch (chs) {
      case  1: // just ignore and exit
        return false;
        break;
      case  0:
      default:
        Apply();
        break;
      }
    }
  }
  // clear body -- much of the deleting is deferred, so event loop must pick up when it changes
  ClearBody();
  state |= SHOW_CHANGED; // will get changed in event handler for body items deleted
  return true;
}

void taiEditDataHost::SetItemAsHandler(taiData* item, bool set_it) {
  iDataViewer* dv; // cached
  if (!panel || !(dv = panel->viewer_win())) return; // only used by panels
  if (set_it) {
    dv->SetClipboardHandler(item,
      SLOT(this_GetEditActionsEnabled(int&)),
      SLOT(this_EditAction(int)),
      SLOT(this_SetActionsEnabled()),
      SIGNAL(UpdateUi()) );
  } else {
    dv->SetClipboardHandler(NULL);
  }
}

void taiEditDataHost::setShowValues(taMisc::ShowMembs value) {
  if (show_menu == NULL) return;
  //note: nothing to do for the command items
  (*show_menu)[2]->setChecked(!(value & taMisc::NO_NORMAL));
  (*show_menu)[3]->setChecked(!(value & taMisc::NO_HIDDEN));
  (*show_menu)[4]->setChecked(!(value & taMisc::NO_READ_ONLY));
  (*show_menu)[5]->setChecked(!(value & taMisc::NO_DETAIL));
  show = value;
}

void taiEditDataHost::setShow(taMisc::ShowMembs value) {
  if (show_menu == NULL) return;
  taMisc::ShowMembs old_show = show;
  if (show != value) {
      // get confirmation if changed, and possibly exit
    if (HasChanged()) {
      int chs = taMisc::Choice("Changes must be applied before changing view", "&Apply", "&Cancel");
      switch (chs) {
      case  1: // just ignore and exit
  	setShowValues(old_show); // need to correct menu before cancelling
        return;
        break;
      case  0:
      default:
        Apply();
        break;
      }
    }
  }
  setShowValues(value);

  // only reconfigure if view actually changed
  if (old_show == value) return;
  // clear body -- much of the deleting is deferred, so event loop must pick up when it changes
  ClearBody();
  state |= SHOW_CHANGED; // will get changed in event handler for body items deleted
}

void taiEditDataHost::ShowChange(taiMenuEl* sender) {
  //note: we allow modal show change in Qt version, because we don't destroy the dialog
  int new_show;
  if (sender->index() == 0)
    new_show = taMisc::NORM_MEMBS;
  else if (sender->index() == 1)
    new_show = taMisc::ALL_MEMBS;
  else {
    int mask;
    switch (sender->index()) {
      case 2: mask = taMisc::NO_NORMAL; break;
      case 3: mask = taMisc::NO_HIDDEN; break;
      case 4: mask = taMisc::NO_READ_ONLY; break;
      case 5: mask = taMisc::NO_DETAIL; break;
      default: mask = 0; break; // should never happen
    }
    new_show = sender->isChecked() ? show & ~mask : show | mask;
  }
  setShow((taMisc::ShowMembs)new_show);
}

bool taiEditDataHost::ShowMember(MemberDef* md) {
  return (md->ShowMember(show) && (md->im != NULL));
}

void taiEditDataHost::SetCurMenu(MethodDef* md) {
  if (menu == NULL) {
    menu = new taiMenu(taiMenu::menubar, taiMenu::normal, taiMisc::fonSmall,
      NULL, this, NULL, widget());
    vblDialog->setMenuBar(menu->rep_bar());
  }
  String men_nm = md->OptionAfter("MENU_ON_");
  if (men_nm != "") {
    cur_menu = ta_menus.FindName(men_nm);
    if (cur_menu != NULL)  return;
  }
  if (cur_menu != NULL) return;

  if (men_nm == "")
    men_nm = "Actions";
  cur_menu = menu->AddSubMenu(men_nm);
  ta_menus.Add(cur_menu);
}


void taiEditDataHost::SetCurMenuButton(MethodDef* md) {
  String men_nm = md->OptionAfter("MENU_ON_");
  if (men_nm != "") {
    cur_menu_but = ta_menu_buttons.FindName(men_nm);
    if (cur_menu_but != NULL)  return;
  }
  if (cur_menu_but != NULL)  return;

  if (men_nm == "")
    men_nm = "Misc"; //note: this description not great, but should be different from "Actions", esp. for
       // context menus in the browser (otherwise, there are 2 "Actions" menus); see also tabDataLink::FillContextMenu_impl
      // also, must work when it appears before the other label (ex "Misc", then "Actions" )
  cur_menu_but = new taiMenu(taiMenu::buttonmenu, taiMenu::normal, taiMisc::fonSmall,
	    NULL, this, NULL, widget());
  cur_menu_but->setLabel(men_nm);
  DoAddMethButton((QPushButton*)cur_menu_but->GetRep()); // rep is the button for buttonmenu
  ta_menu_buttons.Add(cur_menu_but);
}

/*

//////////////////////////////////////////////////
// 		taiTokenDialog			//
//////////////////////////////////////////////////

taiTokenDialog::taiTokenDialog(TypeDef* td, TAPtr scp_ref, TAPtr in_itm) {
  ths = td;
  scope_ref = scp_ref;
  itm = in_itm;
}
taiTokenDialog::~taiTokenDialog() {
  if(itm_rep != NULL)
    delete itm_rep;
  itm_rep = NULL;
}
void taiTokenDialog::Constr_Box() {
  itm_rep = new taiToken(taiMenu::menubar, taiMenu::small, ths, this);
  itm_rep->scope_ref = scope_ref;
  itm_rep->GetMenu();

  box = layout->hbox
    (layout->hglue(), taiM->top_spc(layout->hbox(wkit->label("Token:"),
				  taiM->vspc, itm_rep->GetLook())), layout->hglue());
  itm_rep->GetImage(itm, scope_ref, par_win);
}

void taiTokenDialog::GetValue() {
  itm = (TAPtr)itm_rep->GetValue();
}

TAPtr taiTokenDialog::GetToken(TypeDef* td, const char* prompt,
				TAPtr scp_ref, TAPtr init_itm, QDialog* win)
{
  taiTokenDialog* tok = new taiTokenDialog(td, scp_ref, init_itm);
  tok->Constr(win, true, prompt);
  TAPtr rval;
  if(tok->Edit())
    rval = tok->itm;
  else
    rval = NULL;

  delete tok;
  return rval;
}

//////////////////////////////////////////////////
// 		taiTypeDialog			//
//////////////////////////////////////////////////

taiTypeDialog::taiTypeDialog(TypeDef* td, TypeDef* init_tp) {
  base_typ = td;
  sel_typ = init_tp;
}

taiTypeDialog::~taiTypeDialog() {
  if(typ_rep != NULL) delete typ_rep;
  typ_rep = NULL;
}

void taiTypeDialog::Constr_Box() {
  typ_rep = new taiTypeHier(taiMenu::menubar, taiMenu::small, base_typ, this);
  typ_rep->GetMenu();

  box = layout->hbox
    (layout->hglue(), taiM->top_spc(layout->hbox(wkit->label("Type:"),
				 taiM->vspc, typ_rep->GetLook())), layout->hglue());
  if(sel_typ != NULL)
    typ_rep->GetImage(sel_typ);
  else
    typ_rep->GetImage(base_typ);
}

void taiTypeDialog::GetValue() {
  sel_typ = typ_rep->GetValue();
}

TypeDef* taiTypeDialog::GetType(TypeDef* td, const char* prompt, TypeDef* init_tp,
				 QDialog* win)
{
  taiTypeDialog* tok = new taiTypeDialog(td, init_tp);
  tok->Constr(win, true, prompt);
  TypeDef* rval;
  if(tok->Edit())
    rval = tok->sel_typ;
  else
    rval = NULL;

  delete tok;
  return rval;
}

//////////////////////////////////////////////////
// 		taiEnumDialog			//
//////////////////////////////////////////////////

taiEnumDialog::taiEnumDialog(TypeDef* td, int init_vl) {
  enum_typ = td;
  sel_val = init_vl;
}

taiEnumDialog::~taiEnumDialog() {
  if(enm_rep != NULL) delete enm_rep;
  enm_rep = NULL;
}

void taiEnumDialog::Constr_Box() {
  enm_rep = new taiMenu
    (taiMenu::menubar, taiMenu::radio_update, taiMenu::small, this);
  int i;
  for(i=0; i<enum_typ->enum_vals.size; i++) {
    enm_rep->AddItem(enum_typ->enum_vals.FastEl(i)->GetLabel());
  }

  box = layout->hbox
    (layout->hglue(), taiM->top_spc(layout->hbox(wkit->label("Value:"),
				 taiM->vspc, enm_rep->GetLook())), layout->hglue());
  EnumDef* td = enum_typ->enum_vals.FindNo(sel_val);
  if(td != NULL)
    enm_rep->GetImage(td->idx);
}

void taiEnumDialog::GetValue() {
  taiMenuEl* cur = enm_rep->GetValue();
  EnumDef* td = NULL;
  if(cur && (cur->itm_no < enum_typ->enum_vals.size))
    td = enum_typ->enum_vals.FastEl(cur->itm_no);
  if(td)
    sel_val = td->enum_no;
}

int taiEnumDialog::GetEnum(TypeDef* td, const char* prompt, int init_vl,
			    QDialog* win)
{
  taiEnumDialog* tok = new taiEnumDialog(td, init_vl);
  tok->Constr(win, true, prompt);
  int rval;
  if(tok->Edit())
    rval = tok->sel_val;
  else
    rval = -1;

  delete tok;
  return rval;
}
*/

//////////////////////////////////
// 	taFiler_impl		//
//////////////////////////////////

class taFiler_impl : public taFiler {
public:
  override bool	GetFileName(String& fname, FilerOperation filerOperation);

  taFiler_impl(const String& dir_, const String& filter_, bool compress_);
  ~taFiler_impl() {}
};

taFiler_impl::taFiler_impl(const String& dir_, const String& filter_, bool compress_) :
  taFiler(dir_, filter_, compress_)
{
  //does nothing
}

bool taFiler_impl::GetFileName(String& fname, FilerOperation filerOperation) {
  bool result = false;
  QFileDialog* fd = new QFileDialog(fname, filter, NULL, 0, true); // no parent, no name, modal
  fd->setDir(dir);

  String caption;
  switch (filerOperation) {
  case foOpen:
    fd->setMode(QFileDialog::ExistingFile);
//OBS:    fd->style()->attribute("caption", "Select File to Open for Reading");
    caption = String("Open: ") + filter;
    break;
  case foSave:
    // TODO: will this ever be called???
    return false;
    break;
  case foSaveAs:
    fd->setMode(QFileDialog::AnyFile);
//OBS:    fd->style()->attribute("caption", "Select File to Save for Writing");
    caption = String("Save: ") + filter;
    break;
  case foAppend:
    fd->setMode(QFileDialog::AnyFile);
//OBS:    fd->style()->attribute("caption", "Select File to Append for Writing");
    caption = String("Append: ") + filter;
    break;
  }

  fd->setCaption(caption);

  if (fd->exec() == QDialog::Accepted) {
        fname = fd->selectedFile();
        result = true;
  }

  if (fd != NULL) {
    delete fd;
    fd = NULL;
  }
  return result;
}

taFiler* taFiler_CreateInstance(const String& dir, const String& filter, bool compress) {
  return new taFiler_impl(dir, filter, compress);
}

