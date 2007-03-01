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

// ta_qtdialog.cpp


#include "ta_qt.h"
#include "ta_qtdialog.h"
#include "ta_qttype.h"
#include "ta_qtdata.h"
#include "ta_base.h"
#include "css_qt.h"
#include "css_basic_types.h"
#include "css_ta.h"
#include "ta_seledit.h"
#include "ta_TA_type.h"

#include <qapplication.h>
#include <QCursor>
#include <qdesktopwidget.h>
#include <qdialog.h>
#include <qevent.h>
//#include <qhbox.h>
#include <QButtonGroup>
#include <qlabel.h>
#include <qlayout.h>
#include <QMenuBar>
#include <QMenu>
#include <qmessagebox.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpalette.h>
#include <QList>
#include <qpushbutton.h>
#include <QScrollArea>
#include <qsizepolicy.h>
#include <QSplitter>
#include <qstring.h>
#include <Q3StyleSheet>
//#include <qtable.h>
#include <QTextEdit>
#include <qtimer.h>
#include <qtooltip.h>
#include <QVBoxLayout>
#include <qwidget.h>

#include "ibutton.h"
#include "icolor.h"
#include "ieditgrid.h"
#include "iflowlayout.h"
#include "iscrollarea.h"

// TODO: why is String=osString in this file, unless I do this:
#include "ta_string.h"


/*obs class QHackMouseEvent: public QMouseEvent { // hack to enable us to change the mouse button
public:
  QHackMouseEvent(const QHackMouseEvent& dummy): QMouseEvent(dummy) {}
  void		setButton(ushort val) {b = val;}
}; */

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
/*obs
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
} */


//////////////////////////////////////////////////////////
// 		taiChoiceDialog				//
//////////////////////////////////////////////////////////

int taiChoiceDialog::ChoiceDialog(QWidget* win, const char* prompt,
	const char* win_title, bool no_cancel)
{
  if (win == NULL) win = QApplication::activeWindow();
  taiChoiceDialog* dlg = new taiChoiceDialog(win, prompt, win_title, no_cancel);
  // show the dialog
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  int rval = dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
  return rval;
}

void taiChoiceDialog::ErrorDialog(QWidget* parent_, const char* msg, const char* win_title)
{
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  QMessageBox::warning(parent_, win_title, msg);
  QApplication::restoreOverrideCursor();
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
  bgChoiceButtons = new QButtonGroup(this); // note: not a widget, invisible
  bgChoiceButtons->setExclusive(false); // not really applicable
  
  connect(bgChoiceButtons, SIGNAL(buttonClicked(int)),
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
  bgChoiceButtons->addButton(but, curId);
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
    QPushButton* but = (QPushButton*)bgChoiceButtons->button(but_index);
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
//   iDialog		//
//////////////////////////

iDialog::iDialog(taiDataHost* owner_, QWidget* parent, int wflags)
:QDialog(parent, (Qt::WFlags)wflags) 
{
  owner = owner_;
  mcentralWidget = NULL;
  scr = new iScrollArea(this);
  scr->setWidgetResizable(true);
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(0);
  layOuter->setSpacing(0); // none needed
  layOuter->addWidget(scr, 1);
  
  iSize ss = taiM->scrn_s;
  setMaximumSize(ss.width(), ss.height());
  setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
}

iDialog::~iDialog() {
  if (owner != NULL) {
    owner->WidgetDeleting(); // removes our ref
    owner = NULL;
  }
}

void iDialog::closeEvent(QCloseEvent* ev) {
  //note: a higher level routine may already have resolved changes
  // if so, the ResolveChanges call is superfluous
  ev->accept(); // default, unless we override;
  if (owner == NULL) return;
  bool discarded = false;
  CancelOp cancel_op = CO_PROCEED;
  owner->ResolveChanges(cancel_op, &discarded);
  if (cancel_op == CO_CANCEL) {
    ev->ignore();
    return;
  } else if (!discarded) {
    owner->state = taiDataHost::ACCEPTED;
    return; // not rejected
  }
  // discarded, or didn't have any changes
  owner->state = taiDataHost::CANCELED;
  setResult(Rejected);
}

bool iDialog::post(bool modal) {
  if (modal) {
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
    bool rval = (exec() == Accepted);
    QApplication::restoreOverrideCursor();
    return rval;
  } else {
    show();
    return true;
  }
}

void iDialog::dismiss(bool accept_) {
  if (accept_)
    accept();
  else
    reject();
}

void iDialog::iconify() {
  // Iv compatibility routine
  if (isModal() || isMinimized()) return;
  showMinimized();
}

void iDialog::deiconify() {
   // Iv compatibility routine
  if (isModal() || !isMinimized()) return;
  showNormal();
}

void iDialog::setButtonsWidget(QWidget* widg) {
  widg->setParent(this);
  layOuter->addWidget(widg);
}

void iDialog::setCentralWidget(QWidget* widg) {
  mcentralWidget = widg;
  scr->setWidget(widg);
  widg->show(); 
}

//////////////////////////////////
// 	EditDataPanel		//
//////////////////////////////////

EditDataPanel::EditDataPanel(taiEditDataHost* owner_, taiDataLink* dl_)
:inherited(dl_)
{
  owner = owner_;
  bgcol = NULL;
}

EditDataPanel::~EditDataPanel() {
  if (owner) {
    owner->WidgetDeleting(); // removes our ref
    owner = NULL;
  }
}

void EditDataPanel::Closing(CancelOp& cancel_op) {
  //note: a higher level routine may already have resolved changes
  // if so, the ResolveChanges call is superfluous
  if (!owner) return;
  bool discarded = false;
  owner->ResolveChanges(cancel_op, &discarded);
  if (cancel_op == CO_CANCEL) {
    return;
  } else if (!discarded) {
    owner->state = taiDataHost::ACCEPTED;
    return; 
  }
  // discarded, or didn't have any changes
  owner->state = taiDataHost::CANCELED;
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

void EditDataPanel::Render_impl() {
  inherited::Render_impl();
  taiEditDataHost* edh = editDataHost();
  if (edh->state >= taiDataHost::CONSTRUCTED) return;
  
  edh->ConstrDeferred();
  setCentralWidget(edh->widget());
  setButtonsWidget(edh->widButtons);
  taiMisc::active_edits.Add(edh); // add to the list of active edit dialogs
  edh->state = taiDataHost::ACTIVE;
  //prob need to do a refresh!!!
}

void EditDataPanel::ResolveChanges_impl(CancelOp& cancel_op) {
  if (!owner) return;
  owner->ResolveChanges(cancel_op);
}


//////////////////////////////////
// iMethodButtonFrame		//
//////////////////////////////////

iMethodButtonFrame::iMethodButtonFrame(taBase* base_, QWidget* parent)
:inherited(parent)
{
  base = NULL;
  setBase(base_);
  Init();
}

iMethodButtonFrame::~iMethodButtonFrame()
{
  setBase(NULL);
}

void iMethodButtonFrame::Init() {
  lay = new iFlowLayout(this, 3, taiM->hspc_c, (Qt::AlignCenter));
}

void iMethodButtonFrame::DataLinkDestroying(taDataLink* dl) {
  base = NULL;
  //TODO: delete the buttons etc.
}
 
void iMethodButtonFrame::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
  if (dcr > DCR_ITEM_UPDATED_ND) return;
  GetImage();
}

void iMethodButtonFrame::setBase(taBase* ta) {
  if (base == ta) return;
  if (base) {
    base->RemoveDataClient(this);
    base = NULL;
  }
  base = ta;
  if (base) {
    base->AddDataClient(this);
  }
}

const iColor* iMethodButtonFrame::colorOfCurRow() const {
  return NULL;
} 
  
bool iMethodButtonFrame::HasChanged() {
  return false; // not needed for methods
}	

bool iMethodButtonFrame::isConstructed() {
  return true;
}

bool iMethodButtonFrame::isModal() {
  return false;
}

bool iMethodButtonFrame::isReadOnly() {
  return false;
} 

iMainWindowViewer* iMethodButtonFrame::viewerWindow() const {
  return NULL; // prob not needed
}

void* iMethodButtonFrame::Base() {
  return base;
}

TypeDef* iMethodButtonFrame::GetBaseTypeDef() {
  if (base) return base->GetTypeDef();
  else return &TA_taBase;
}

void iMethodButtonFrame::GetImage() {
//TODO
}


//////////////////////////////////
// 	taiDataHost		//
//////////////////////////////////

class ReShowEvent: public QEvent {
INHERITED(QEvent)
public:
  bool forced;
  ReShowEvent(bool forced_):inherited((QEvent::Type)taiDataHost::CET_RESHOW) {
    forced = forced;
  }
};

#define LAYBODY_MARGIN	1
#define LAYBODY_SPACING	0

void taiDataHost::DeleteChildrenLater(QObject* obj) {
  if (obj == NULL) return;
  QObject* chobj;
  const QObjectList& ol = obj->children(); 
  int i = 0;
  while (ol.count() > 0) {
    i = ol.size() - 1;
    chobj = ol.at(i);
    delete chobj;
/*    if (chobj->isWidgetType()) ((QWidget*)chobj)->setParent((QWidget*)NULL);
    else  chobj->setParent((QObject*)NULL);
    chobj->deleteLater(); // deleted in event loop */
  }
}

void taiDataHost::MakeDarkBgColor(const iColor& bg, iColor& dk_bg) {
  dk_bg.set(taiMisc::ivBrightness_to_Qt_lightdark(bg, taiM->edit_darkbg_brightness));
}


taiDataHost::taiDataHost(TypeDef* typ_, bool read_only_, bool modal_, QObject* parent)
:QObject(parent)
{
  read_only = read_only_;
  modified = false;
  typ = typ_;
  cur_base = NULL;

  InitGuiFields();

  bg_color = new iColor(); //value set later
  bg_color_dark = new iColor(); //value set later
  if (taiM == NULL) ctrl_size = taiMisc::sizMedium;
  else              ctrl_size = taiM->ctrl_size; // for early type system instance when no taiM yet
  row_height = 1; // actual value set in Constr
  mouse_button = 0;
  cur_row = 0;
  modal = modal_;
  no_ok_but = false;
  dialog = NULL;
  state = EXISTS;
  sel_item_md = NULL;
  rebuild_body = false;
  warn_clobber = false;
  host_type = HT_DIALOG; // default, set later
  reshow_req = false;
  get_image_req = false;
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

// note: called non-virtually in our ctor, and virtually in WidgetDeleting
void taiDataHost::InitGuiFields(bool) { 
  mwidget = NULL;
  vblDialog = NULL;
  prompt = NULL;
  splBody = NULL;
  scrBody = NULL;
  layBody = NULL;
  body = NULL;
  frmMethButtons = NULL;
  layMethButtons = NULL;
  show_meth_buttons = false;
  widButtons = NULL;
  layButtons = NULL;
  okbut = NULL;
  canbut = NULL;
  apply_but = NULL;
  revert_but = NULL;
}

void taiDataHost::setBgColor(const iColor* new_bg) {
  if (new_bg == NULL) {
    // get from default pallette
    bg_color->set(QApplication::palette().color(QPalette::Active, QColorGroup::Background));
  } else {
    bg_color->set(new_bg);
  }
  MakeDarkBgColor(*bg_color, *bg_color_dark);
}

const iColor* taiDataHost::colorOfRow(int row) const {
  if ((row % 2) == 0) {
    return bg_color;
  } else {
    return bg_color_dark;
  }
}

int taiDataHost::AddSectionLabel(int row, QWidget* wid, const String& desc) {
  QFont f(taiM->nameFont(ctrl_size));
  f.setBold(true);
  wid->setFont(f);
  wid->setFixedHeight(taiM->label_height(ctrl_size));
  wid->setPaletteBackgroundColor(*colorOfRow(row));
  if (!desc.empty()) {
    QToolTip::add(wid, desc);
  }
  if (row < 0)
    row = layBody->numRows();
  if (layBody->numRows() < (row + 1)) {
    layBody->expand(row + 1, 2);
  }
  layBody->setRowSpacing(row, row_height + (2 * LAYBODY_MARGIN)); //note: margins not automatically baked in to max height
  QHBoxLayout* layH = new QHBoxLayout();
  
  
  layH->addSpacing(2);
  // we add group-box-like frame lines to separate sections
  QFrame* ln = new QFrame(body);
  ln->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  ln->setMinimumWidth(16);
  layH->addWidget(ln, 0, (Qt::AlignLeft | Qt::AlignVCenter));
  ln->show();
  layH->addSpacing(4); // leave a bit more room before ctrl
  layH->addWidget(wid, 0, (Qt::AlignLeft | Qt::AlignVCenter));
  layH->addSpacing(2); // don't need as much room to look balanced
  ln = new QFrame(body);
  ln->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  ln->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  layH->addWidget(ln, 1, (Qt::AlignVCenter));
  ln->show();
  layH->addSpacing(2);
  // add the item to span both cols
  layBody->addLayout(layH, row, 0, 1, 2, (Qt::AlignLeft | Qt::AlignVCenter)); 
  wid->show(); // needed for rebuilds, to make the widget show
  return row;
}

int taiDataHost::AddName(int row, const String& name, const String& desc,
   taiData* buddy, MemberDef* md)
{
  iLabel* label = new iLabel(row, name, body);
  label->setFont(taiM->nameFont(ctrl_size));
  label->setFixedHeight(taiM->label_height(ctrl_size));
  label->setPaletteBackgroundColor(*colorOfRow(row));
  if (md) label->setUserData((ta_intptr_t)md);
  connect(label, SIGNAL(contextMenuInvoked(iLabel*, QContextMenuEvent*)),
      this, SLOT(label_contextMenuInvoked(iLabel*, QContextMenuEvent*)) );
// if it is an iLabel connecting a taiData, then connect the highlighting for non-default values
  QWidget* buddy_widg = NULL;
  if (buddy) {
    buddy_widg = buddy->GetRep();
    connect(buddy, SIGNAL(settingHighlight(bool)),
        label, SLOT(setHighlight(bool)) );
  }
  

  if (!desc.empty()) {
    QToolTip::add(label, desc);
    if (buddy_widg != NULL) {
      QToolTip::add(buddy_widg, desc);
    }
  }
  // add a label item in first column
  if (row < 0)
    row = layBody->numRows();
  if (layBody->numRows() < (row + 1)) {
    layBody->expand(row + 1, 2);
  }
  layBody->setRowSpacing(row, row_height + (2 * LAYBODY_MARGIN)); //note: margins not automatically baked in to max height
  QHBoxLayout* layH = new QHBoxLayout();
  layH->addWidget(label, 0, (Qt::AlignLeft | Qt::AlignVCenter));
/*  layH->addItem(new QSpacerItem(2, row_height, QSizePolicy::Fixed), row, 0,
    1, 1, (Qt::AlignRight | Qt::AlignVCenter)); */
  layH->addSpacing(2);
  layBody->addLayout(layH, row, 0, (Qt::AlignLeft | Qt::AlignVCenter));
  label->show(); // needed for rebuilds, to make the widget show
  return row;
}
int taiDataHost::AddData(int row, QWidget* data, bool fill_hor) {
   // add a data item in second column
    if (row < 0)
      row = layBody->numRows();
    if (layBody->numRows() < (row + 1)) {
      layBody->expand(row + 1, 2);
    }
    layBody->setRowSpacing(row, row_height + (2 * LAYBODY_MARGIN)); //note: margins not automatically baked in to max height
    QHBoxLayout* hbl = new QHBoxLayout();
    layBody->addLayout(hbl, row, 1);
    hbl->addWidget(data, 0);
    if (!fill_hor) hbl->addStretch();
    data->show(); // needed for rebuilds, to make the widget show
    return row;
}

void taiDataHost::AddMultiRowName(iEditGrid* multi_body, int row, const String& name, const String& desc) {
  SetMultiSize(row + 1, 0); //0 gets set to multi_col
  QLabel* label = new QLabel(name, (QWidget*)NULL);
  label->setFont(taiM->nameFont(ctrl_size));
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
  label->setFont(taiM->nameFont(ctrl_size));
  label->setFixedHeight(taiM->label_height(ctrl_size));
  if (!desc.empty()) {
    QToolTip::add(label, desc);
  }
  multi_body->setColNameWidget(col, label);
  label->show(); //required to show when rebuilding
}

void taiDataHost::AddMultiData(iEditGrid* multi_body, int row, int col, QWidget* data) {
//  SetMultiSize(row - 1, col - 1);
  SetMultiSize(row + 1, col + 1);
  QHBoxLayout* hbl = new QHBoxLayout();
  hbl->setMargin(0);
  hbl->addWidget(data, 0,  (Qt::AlignLeft | Qt::AlignVCenter));
  hbl->addStretch();
  multi_body->setDataLayout(row, col, hbl);
  data->show(); //required to show when rebuilding
}

void taiDataHost::Apply() {
  if (warn_clobber) {
    int chs = taiChoiceDialog::ChoiceDialog
      (NULL, "Warning: this object has changed since you started editing -- if you apply now, you will overwrite those changes -- what do you want to do?!Apply!Revert!Cancel!");
    if(chs == 1) {
      Revert();
      return;
    }
    if(chs == 2)
      return;
  }
  ++updating;
  GetValue();
  GetImage();
  Unchanged();
  --updating;
}

void taiDataHost::BodyCleared() { // called when last widget cleared from body
  if (!(state & SHOW_CHANGED)) return; // probably just destroying
  rebuild_body = true;
  ReConstr_Body();
  state &= ~SHOW_CHANGED;
}

void taiDataHost::Cancel() { //note: taiEditDataHost takes care of cancelling panels
  state = CANCELED;
  Cancel_impl();
}

void taiDataHost::Cancel_impl() { //note: taiEditDataHost takes care of cancelling panels
  if (dialog) {
    dialog->dismiss(false);
  }
  if (prompt) {
    prompt->setText("");
  }
  // delete any methods
  if (frmMethButtons) {
    QWidget* t = frmMethButtons; // avoid any possible callback issues
    frmMethButtons = NULL;
    delete t;
  }

  warn_clobber = false; // just in case
}

void taiDataHost::Changed() {
  if (modified) return; // handled already
  modified = true;
  if (updating) return;
  if (apply_but != NULL) {
      apply_but->setEnabled(true);
      apply_but->setHiLight(true);

  }
  if (revert_but != NULL) {
      revert_but->setEnabled(true);
  }
}

void taiDataHost::ClearBody() {
  widget()->setUpdatesEnabled(false);
  ClearBody_impl();
  taiMiscCore::RunPending(); // not a bad idea to update gui before proceeding
  BodyCleared(); //rebuilds if ShowChanged
  widget()->setUpdatesEnabled(true);
}

void taiDataHost::ClearBody_impl() {
  DeleteChildrenLater(body);
}

/* NOTE: Constr_Xxx methods presented in execution (not lexical) order */
void taiDataHost::Constr(const char* aprompt, const char* win_title,
  const iColor* bgclr, HostType host_type_, bool deferred) 
{
  host_type = host_type_;
  setBgColor(bgclr);
  row_height = taiM->max_control_height(ctrl_size); // 3 if using line between; 2 if using even/odd shading
  Constr_Strings(aprompt, win_title);
  Constr_WinName();
  Constr_Widget();
  if (host_type != HT_CONTROL) 
    Constr_Methods();
  Constr_RegNotifies(); // taiEditHost registers notifies
  state = DEFERRED1;
  if (deferred) return;
  ConstrDeferred(); // else finish it now!
}



void taiDataHost::ConstrDeferred() {
  if (state != DEFERRED1) {
    taMisc::Error("taiDataHost::ConstrDeferred2: expected host to be in state DEFERRED1");
    return;
  }
  Constr_impl();
  state = CONSTRUCTED;
  GetImage();
}

void taiDataHost::Constr_Strings(const char* prompt, const char* win_title) {
  prompt_str = prompt;
  win_str = win_title;
}

void taiDataHost::Constr_impl() {
  widget()->setUpdatesEnabled(false);
  Constr_Prompt();
  Constr_Box();
  Constr_Body();
  if (host_type != HT_CONTROL) 
    Insert_Methods(); // if created, AND unowned
  // create container for ok/cancel/apply etc. buttons
  widButtons = new QWidget(); // parented when we do setButtonsWidget
  widButtons->setAutoFillBackground(true);
  if (bg_color) {
    widButtons->setPaletteBackgroundColor(*bg_color);
  }
  layButtons = new QHBoxLayout(widButtons);
//def  layButtons->setMargin(2); // facilitates container
  Constr_Buttons();
  Constr_Final();
  widget()->setUpdatesEnabled(true);
//NOTE: do NOT do a processevents -- it causes improperly nested event calls
// in some cases, such as constructing the browser
}

void taiDataHost::Constr_Widget() {
  if (mwidget != NULL) return;
  mwidget = new QWidget();
  if (bg_color != NULL) {
    widget()->setPaletteBackgroundColor(*bg_color);
  }
  widget()->setFont(taiM->dialogFont(ctrl_size));
  vblDialog = new QVBoxLayout(widget()); //marg=2
  vblDialog->setSpacing(0); // need to manage ourself to get nicest look
}

void taiDataHost::Constr_WinName() {
//nothing
}

void taiDataHost::Constr_Prompt() {
  if (prompt != NULL) return; // already constructed
//NOTE: don't use RichText format because it doesn't word wrap!
  prompt = new QLabel(widget()); 
  prompt->setWordWrap(true); // so it doesn't dominate hor sizing
  QFont f = taiM->nameFont(ctrl_size);
  f.setBold(true); 
  prompt->setFont(f);
  prompt->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  vblDialog->addWidget(prompt);
  prompt->setText(prompt_str);
  vblDialog->addSpacing(2);
/*  // add a separator line
  QFrame* line = new QFrame(widget());
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vblDialog->add(line);
  vblDialog->addSpacing(2); // add now, so next guy doesn't have to no*/
}

void taiDataHost::Constr_Box() {
  //note: see also gpiMultiEditDialog::Constr_Box, if changes made to this implementation
  //note: see ClearBody for guards against deleting the structural widgets when clearing
  QWidget* scr_par = (splBody == NULL) ? widget() : splBody;
  scrBody = new iScrollArea(scr_par);
  scrBody->viewport()->setPaletteBackgroundColor(*bg_color_dark);
//Qt3  scrBody->setResizePolicy(Q3ScrollView::AutoOneFit);
  scrBody->setWidgetResizable(true); // TODO: confirm
  body = new iStripeWidget();
  scrBody->setWidget(body);
  if (bg_color != NULL) {
    body->setPaletteBackgroundColor(*bg_color);
    if (bg_color_dark != NULL)
      ((iStripeWidget*)body)->setHiLightColor(*bg_color_dark);
    ((iStripeWidget*)body)->setStripeHeight(row_height + (2 * LAYBODY_MARGIN));
    //TODO: if adding spacing, need to include LAYBODY_SPACING;

  }
  if (splBody == NULL) {
    vblDialog->addWidget(scrBody, 1); // gets all the space
  }
  //note: the layout is added in Constr_Body, because it gets deleted when we change the 'show'
}

void taiDataHost::Constr_Body() {
  QVBoxLayout* vbl = new QVBoxLayout(body);
  vbl->setMargin(0);
  layBody = new QGridLayout();
  layBody->setSpacing(LAYBODY_SPACING);
  layBody->setMargin(LAYBODY_MARGIN);
  layBody->setColumnStretch(1,1);
  vbl->addLayout(layBody);
  vbl->addStretch(1);
}

void taiDataHost::Constr_Methods() {
  Constr_Methods_impl();
  Constr_ShowMenu();
}


void taiDataHost::Constr_Methods_impl() { //note: conditional constructions used by SelectEditHost to rebuild methods
  QFrame* tmp = frmMethButtons;
  if (!frmMethButtons) {
    show_meth_buttons = false; // set true if any created
    tmp = new QFrame(); // tmp = new QFrame(widget());
    tmp->setVisible(false); // prevents it showing as global win in some situations
    tmp->setAutoFillBackground(true); // for when disconnected from us
    if (bg_color) {
      tmp->setPaletteBackgroundColor(*bg_color);
    }
    tmp->setFrameStyle( QFrame::GroupBoxPanel | QFrame::Sunken );
    tmp->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  }
  if (!layMethButtons) {
    layMethButtons = new iFlowLayout(tmp, 3, taiM->hspc_c, (Qt::AlignCenter)); // margin, space, align
  }
  if (!frmMethButtons) {
    frmMethButtons = tmp;
    //TODO: if this is a SelectEdit, and this is a rebuild for first time,
    // we will need to do the Insert_Methods call
  }
}

void taiDataHost::Insert_Methods() {
  //NOTE: for taiEditDataHost, menus are always put in widget() even in deferred
  if (frmMethButtons && !frmMethButtons->parentWidget()) {
    // meth buttons always at bottom of inner layout
    vblDialog->addSpacing(2);
    vblDialog->addWidget(frmMethButtons);
    frmMethButtons->setVisible(show_meth_buttons); // needed for deferred insert
  }
}

void taiDataHost::Constr_Buttons() {
  QWidget* par = widButtons;
  layButtons->addStretch();
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
      layButtons->addWidget(okbut, 0, (Qt::AlignVCenter));
      connect(okbut, SIGNAL(clicked()),
          this, SLOT(Ok()) );
    }
    if (read_only) {
      canbut = NULL;
    }
    else {
      canbut = new HiLightButton("&Cancel", par);
      layButtons->addWidget(canbut, 0, (Qt::AlignVCenter));
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
      layButtons->addSpacing(20); // TODO: parameterize, and give it some stretchiness
      apply_but = new HiLightButton("&Apply", par);
      layButtons->addWidget(apply_but, 0, (Qt::AlignVCenter));
      connect(apply_but, SIGNAL(clicked()),
          this, SLOT(Apply()) );
      revert_but = new HiLightButton("&Revert", par);
      layButtons->addWidget(revert_but, 0, (Qt::AlignVCenter));
      connect(revert_but, SIGNAL(clicked()),
          this, SLOT(Revert()) );
    }
    Unchanged();
  }
  layButtons->addSpacing(10); // don't flush hard right
}

void taiDataHost::Constr_Final() {
  // we put all the stretch factor setting here, so it is easy to make code changes if necessary
  if (splBody) vblDialog->setStretchFactor(splBody, 1);
  else         vblDialog->setStretchFactor(scrBody, 1);
}

void taiDataHost::customEvent(QEvent* ev_) {
  // we return early if we don't accept, otherwise fall through to accept
  switch ((int)ev_->type()) {
  case CET_RESHOW: {
    ReShowEvent* ev = static_cast<ReShowEvent*>(ev_);
    if (reshow_req) {
      if (state == ACTIVE) {
        ReShow(ev->forced);
        GetImage();
      }
      reshow_req = false;
    }
  } break;
  case CET_GET_IMAGE: {
    if (get_image_req) {
      if (state == ACTIVE) {
        GetImage();
      }
      get_image_req = false;
    }
  } break;
  default: inherited(ev_); 
    return; // don't accept
  }
  ev_->accept();
}

void taiDataHost::ReShow_Async(bool forced) {
  if (reshow_req) return; // already waiting
  if (state != ACTIVE) return;
  ReShowEvent* ev = new ReShowEvent(forced);
  reshow_req = true;
  QCoreApplication::postEvent(this, ev);
}

void taiDataHost::GetImage_Async() {
  // reshow does a getimage, so ignore if a reshow pending
  if (get_image_req || reshow_req) return; // already waiting
  if (state != ACTIVE) return;
  QEvent* ev = new QEvent((QEvent::Type)CET_GET_IMAGE);
  get_image_req = true;
  QCoreApplication::postEvent(this, ev);
}


void taiDataHost::DataLinkDestroying(taDataLink* dl) {
// TENT, TODO: confirm this is right...
  cur_base = NULL;
//NO!  if (!isConstructed()) return;
  Cancel();
}
 
void taiDataHost::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
  // note: because of deferred construction, we may still need to update buttons/menus
  if (state == DEFERRED1) {
    GetImage();
    return;
  }
  // note: we should have unlinked if cancelled, but if not, ignore if cancelled
  if (!isConstructed()) return;
  dch.UpdateFromDataChanged(dcr);
  // we only care about the rebuilding ones, for others, we just call notify
  //NOTE: list/group subclasses typically detect changes in their GetImage routine
  //  so we don't really subclass this routine or explicitly detect the list/group notifies
  if (dch.doStructUpdate()) //note: clears su and du states
    ReShow();
  else if (dch.doDataUpdate()) //note: clears du state
    NotifyChanged();
  
}

void taiDataHost::label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e) {
  QMenu* menu = new QMenu(widget());
  //note: don't use body for menu parent, because some context menu choices cause ReShow, which deletes body items!
  Q_CHECK_PTR(menu);
  int last_id = -1;
  FillLabelContextMenu(sender, menu, last_id);
  if (menu->count() > 0)
    menu->exec(sender->mapToGlobal(e->pos()));
  delete menu;
}

void taiDataHost::DoConstr_Dialog(iDialog*& dlg) {
  // common subcode for creating a dialog -- used by the taiDialog and taiEditDialog cousin classes
  if (dlg) return; // already constructed
  if (modal) // s/b parented to current win
    dlg = new iDialog(this, QApplication::activeWindow());
  else 
    dlg = new iDialog(this, NULL, Qt::WindowMinimizeButtonHint);
  // note: X can't seem to handle more than 12-14 windows, so making these top-level is an issue
  // BUT it is also highly unusable to make them owned, since then they obscure parent window
  dlg->setCaption(win_str);
  dlg->setMinimumWidth(400); //TODO: maybe parameterize; note: would need to set layout FreeResize as well
}

void taiDataHost::DoDestr_Dialog(iDialog*& dlg) { // common sub-code for destructing a dialog instance
  if (dlg != NULL) {
    dlg->owner = NULL; // prevent reverse deletion
    dlg->close(true); // destructive close
    dlg = NULL;
  }
}

void taiDataHost::DoRaise_Dialog() {
  if (!dialog) return;
  if (!modal) {
    dialog->raise();
    dialog->setFocus();
  }
}

int taiDataHost::Edit(bool modal_) { // only called if isDialog() true
  if (state != CONSTRUCTED)
    return false;
  modal = modal_;
  if (dialog == NULL) DoConstr_Dialog(dialog);
//dialog->resize(dialog->minimumWidth(), 1);
  dialog->setCentralWidget(widget());
  dialog->setButtonsWidget(widButtons);
  if (!modal) {
    taiMisc::active_dialogs.AddUnique(this); // add to the list of active dialogs
  }
  state = ACTIVE;
  return dialog->post(modal);
}

void taiDataHost::FillLabelContextMenu(iLabel* sender, QMenu* menu, int& last_id) {
  sel_item_md = (MemberDef*)qvariant_cast<ta_intptr_t>(sender->userData());
}

void taiDataHost::Iconify(bool value) {
  if (!dialog) return;
  if (value) dialog->iconify();
  else       dialog->deiconify();
}

void taiDataHost::Ok() { //note: only used for Dialogs
  //note: IV version used to scold user for pressing Ok in a readonly dialog --
  // we just interpret Ok as Cancel
  if (read_only) {
    Cancel();
    return;
  }
  // NOTE: we herein might be bypassing the clobber warn, but shouldn't really
  //be possible to modify ourself externally inside a dialog
  if (HasChanged()) {
    GetValue();
    Unchanged();
  }
  state = ACCEPTED;
  mouse_button = okbut->mouse_button;
  if (dialog) {
    dialog->dismiss(true);
  }
}

void taiDataHost::NotifyChanged() {
  if (updating) return; // it is us that caused this
  // if no changes have been made in this instance, then just refresh,
  // otherwise, user will have to decide what to do, i.e., revert
  if (HasChanged()) {
    warn_clobber = true;
  } else {
    GetImage_Async();
  }
}

void taiDataHost::ReConstr_Body() {
  if (!isConstructed()) return;
  Constr_Body();
  GetImage();
}

bool taiDataHost::ReShow(bool force) {
//note: only called with force from ReShowEdits, typ only from a SelEdit dialog
  if (!updating) {
    // was not use that caused this...
    if (force) {
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
    } else { // not forced, normal situation for datachanged notifies
      if (HasChanged()) {
        warn_clobber = true; //TODO: prob should have a state variable, so we will rebuild
        return false;
      }
    }
  }
  state |= SHOW_CHANGED; 
  ClearBody(); // rebuilds body after clearing -- but SHOW_CHANGED prevents GetImage...
  GetImage(); // ... so we do it here
  return true;
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
  warn_clobber = false;
}

void taiDataHost::WidgetDeleting() {
  InitGuiFields(); // called virtually
  state = ZOMBIE;
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
  if (updating || (taMisc::is_loading)) return;
  if (!revert_but) return;
  revert_but->setHiLight(true);
  revert_but->setEnabled(true);
}

void taiDataHost::UnSetRevert(){
  if (!revert_but) return;
  revert_but->setHiLight(false);
  revert_but->setEnabled(false);
}


//////////////////////////////////
// 	MembSet_List		//
//////////////////////////////////

void MembSet_List::ResetItems(bool data_only) {
  for (int i = size - 1; i >= 0; --i) {
    MembSet* ms = FastEl(i);
    ms->data_el.Reset();
    if (!data_only) ms->memb_el.Reset();
  }
}

void MembSet_List::SetMinSize(int n) {
  if (n < 0) return;
  Alloc(n); // noop if already sized larger
  while (n > size) {
    Add(new MembSet);
  }
}


//////////////////////////////////
// 	taiEditDataHost		//
//////////////////////////////////


taiEditDataHost::taiEditDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
:taiDataHost(typ_, read_only_, modal_, parent)
{
  cur_base = base;
  // note: subclass might add more membs, and might set def_size to use them
  membs.SetMinSize(MS_CNT); 
  membs.def_size = MS_CNT;
  show_set(MS_NORM) = true;
  inline_mode = false;
  bgrp = new QButtonGroup(this);
  bgrp->setExclusive(false);
  connect(bgrp, SIGNAL(buttonClicked(int)),
    this, SLOT(bgrp_buttonClicked(int)) );
  InitGuiFields(false);
  //note: don't register for notification until constr starts
}

taiEditDataHost::~taiEditDataHost() {
  membs.Reset(); membs.def_size = 0;
  meth_el.Reset();
  taiMisc::active_edits.RemoveEl(this);
  taiMisc::css_active_edits.RemoveEl(this);
  // remove data client -- harmless if already done in Cancel
  if  (cur_base && (typ && typ->InheritsFrom(&TA_taBase))) {
    ((taBase*)cur_base)->RemoveDataClient(this);
    cur_base = NULL;
  }
  bgrp = NULL;
}

// note: called non-virtually in our ctor, and virtually in WidgetDeleting
void taiEditDataHost::InitGuiFields(bool virt) { 
  if (virt) inherited::InitGuiFields();
  cur_menu = NULL;
  cur_menu_but = NULL;
  menu = NULL;
  panel = NULL;
}


void taiEditDataHost::AddMethButton(taiMethodData* mth_rep, const char* label) {
  QAbstractButton* but = mth_rep->GetButtonRep();
  DoAddMethButton(but);
  if (label != NULL) {
    but->setText(label);
  }
}

void taiEditDataHost::bgrp_buttonClicked(int id) {
  // id is an index of the membs
  iCheckBox* chk = qobject_cast<iCheckBox*>(bgrp->button(id));
  if (!chk) return; // shouldn't happen
  if ((id < 0) || (id >= membs.size)) return; // ditto
  show_set(id) = chk->isChecked();
  ReShow_Async();
}

void taiEditDataHost::Cancel_impl() {
//NOTE: must be ok to call this if was still deferred
  // delete all methods and menu
  if (menu) {
    delete menu;
    menu = NULL;
  }
  if  (cur_base && (typ && typ->InheritsFrom(&TA_taBase))) {
    ((taBase*)cur_base)->RemoveDataClient(this);
  }
  if (isPanel()) {
    if (panel != NULL)
      panel->ClosePanel();
  } else if (isControl()) {
    //TODO: need to verify what to do!
    ClearBody();
  } 
  inherited::Cancel_impl();
}

void taiEditDataHost::ClearBody_impl() {
  // delete ALL the data items -- Qt will automatically disconnect the signals/slots
  for (int i = 0; i < membs.size; ++i)
    data_el(i).Reset();
  inherited::ClearBody_impl(); // deletes the body widgets, except structural ones
}

void taiEditDataHost::Constr_impl() {
  inline_mode = (typ && typ->it->requiresInline());
  if (!inline_mode) {
    Enum_Members();
  }
  inherited::Constr_impl();
}

void taiEditDataHost::Enum_Members() {
  if (membs.def_size <= 0) return; // not handling anything
  MemberSpace& ms = typ->members;
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.FastEl(i);
//TEMP
if (md->name == "n_units"){
int i = 0; ++i;
}
    if (md->im == NULL) continue; // this puppy won't show nohow!set_grp
    if (md->ShowMember(~taMisc::IS_NORMAL, TypeItem::SC_EDIT, taMisc::IS_NORMAL)) {
      memb_el(MS_NORM).Add(md);
      continue;
    } 
    if (membs.def_size <= MS_EXPT) continue;
    // set the show_set guys at this point to default to app values
    if (!(show() & taMisc::NO_EXPERT)) 
      show_set(MS_EXPT) = true;
    if (md->ShowMember(0, TypeItem::SC_EDIT, taMisc::IS_EXPERT)) {
      memb_el(MS_EXPT).Add(md);
      continue;
    } 
    if (membs.def_size <= MS_HIDD) continue; 
    if (!(show() & taMisc::NO_HIDDEN)) 
      show_set(MS_HIDD) = true;
    if (md->ShowMember(~taMisc::IS_HIDDEN & ~taMisc::IS_NORMAL, 
      TypeItem::SC_EDIT, taMisc::IS_HIDDEN)) {
      memb_el(MS_HIDD).Add(md);
      continue;
    }
  }
}


void taiEditDataHost::Constr_Body() {
  inherited::Constr_Body();
  if (inline_mode) {
    Constr_Inline();
  } else {
    Constr_Data_Labels();
  }
}

void taiEditDataHost::Constr_Data_Labels() {
  int idx = 0;
  // Normal members
  if (MS_NORM >= membs.def_size) return; // don't do those
  if (show_set(MS_NORM) && (memb_el(MS_NORM).size > 0)) {
//    Constr_Data_impl(idx, &memb_el(MS_NORM), &data_el(MS_NORM));
    Constr_Data_Labels_impl(idx, &memb_el(MS_NORM), &data_el(MS_NORM));
  }
  for (int j = MS_EXPT; j <= MS_HIDD; ++j) {
    if (j >= membs.def_size) return; // don't do those
    if (memb_el(j).size == 0) continue;
    String text; String desc;
    switch (j) {
    case MS_EXPT:
      text = "Expert Items";
      desc = "show member items that are usually only needed for advanced purposes";
      break;
    case MS_HIDD:
      text = "Hidden Items";
      desc = "show member items that are rarely needed by most users";
      break;
    default: continue; // shouldn't happen!
    }
    iCheckBox* chk = new iCheckBox(text.chars(), body);
    AddSectionLabel(idx, chk, desc);
    ++idx;
    bgrp->addButton(chk, j);
    // if we are to show this section, then check the box, and build, else nothing else
    if (show_set(j)) {
      chk->setChecked(true);
//      Constr_Data_impl(idx, &memb_el(j), &data_el(j));
      Constr_Data_Labels_impl(idx, &memb_el(j), &data_el(j));
    }
  }
}

void taiEditDataHost::Constr_Inline() {
  data_el(0).Reset(); // should already be clear
  // specify inline flag, just to be sure
  taiData* mb_dat = typ->it->GetDataRep(this, NULL, body, NULL, taiData::flgInline);
  data_el(0).Add(mb_dat);
  QWidget* rep = mb_dat->GetRep();
  bool fill_hor = mb_dat->fillHor();
  AddData(0, rep, fill_hor);
}

void taiEditDataHost::Constr_Data_Labels_impl(int& idx, Member_List* ms,
  taiDataList* dl) 
{
  String name;
  String desc;
  for (int i = 0; i < ms->size; ++i) {
    MemberDef* md = ms->FastEl(i);
    
    // Create data widget
    taiData* mb_dat = md->im->GetDataRep(this, NULL, body);
    dl->Add(mb_dat);
    QWidget* rep = mb_dat->GetRep();
    bool fill_hor = mb_dat->fillHor();
    AddData(idx, rep, fill_hor);
    
    // create label
    name = "";
    desc = "";
    GetName(md, name, desc);
    AddName(idx, name, desc, mb_dat, md);
    ++idx;
  }
}

void taiEditDataHost::Constr_Strings(const char* aprompt, const char* win_title) {
  win_str = String(win_title);
  if (typ != NULL) {
    prompt_str = typ->name;
    if (typ->InheritsFrom(TA_taBase)) {
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
  }
  String sapr;
  if (aprompt != NULL) sapr = aprompt;
  if (!sapr.empty())
    prompt_str += ": " + sapr;
  else
    if (typ != NULL) prompt_str +=  ": " + typ->desc;
}

void taiEditDataHost::Constr_Methods_impl() {
  inherited::Constr_Methods_impl();
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

void taiEditDataHost::Constr_RegNotifies() {
  if ((typ && typ->InheritsFrom(&TA_taBase) && cur_base)) {
    ((taBase*)cur_base)->AddDataClient(this);
  }
}
//void taiEditDataHost::Constr_ShowMenu() {
// moved to be by the setShow etc. calls, for clarity

void taiEditDataHost::Constr_Final() {
  inherited::Constr_Final();
}

void taiEditDataHost::DoAddMethButton(QAbstractButton* but) {
  show_meth_buttons = true;
  // we use "medium" size for buttons
  but->setFont(taiM->buttonFont(taiMisc::fonMedium));
  but->setFixedHeight(taiM->button_height(taiMisc::sizMedium));
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
  MemberDef* md = sel_item_md;
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
  return inherited::Edit(modal_);
}

EditDataPanel* taiEditDataHost::EditPanel(taiDataLink* link) {
  if (state != CONSTRUCTED)
    return NULL;
  if (panel == NULL)
    panel = new EditDataPanel(this, link); //TODO: make sure this conversion is always valid!!!
  panel->setCentralWidget(widget());
  panel->setButtonsWidget(widButtons);
  taiMisc::active_edits.Add(this); // add to the list of active edit dialogs
  state = ACTIVE;
  return panel;
}

EditDataPanel* taiEditDataHost::EditPanelDeferred(taiDataLink* link) {
  panel = new EditDataPanel(this, link); //TODO: make sure this conversion is always valid!!!
  
  return panel;
}

void taiEditDataHost::ConstrEditControl(const iColor* bgcol) {
  Constr("", "", bgcol, HT_CONTROL);
  taiMisc::active_edits.Add(this); // add to the list of active edit dialogs
  state = ACTIVE;
}

void taiEditDataHost::FillLabelContextMenu(iLabel* sender, QMenu* menu, int& last_id) {
  inherited::FillLabelContextMenu(sender, menu, last_id);
  FillLabelContextMenu_SelEdit(sender, menu, last_id);
}

void taiEditDataHost::FillLabelContextMenu_SelEdit(iLabel* sender, 
  QMenu* menu, int& last_id)
{
  // have to be a taBase to use SelEdit
  if ((cur_base == NULL) || (typ == NULL) || (!typ->InheritsFrom(&TA_taBase))) return; 
  MemberDef* md = sel_item_md; // from inherited routine
  if (md == NULL) return;
  // get list of select edits
  TypeDef* td = SelectEdit::StatTypeDef(0);
  if (td->tokens.size == 0) return;
  // if any edits, populate menu for adding, for all seledits not already on
  QMenu* sub = new QMenu(body);
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
  sub = new QMenu(body);
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
  if (typ == NULL)  return;
  
  for (int i = 0; i < meth_el.size; ++i) {
    taiMethodData* mth_rep = (taiMethodData*)meth_el.SafeEl(i);
    if ( !(mth_rep->hasButtonRep())) //note: construction forced creation of all buttons
      continue;
      
    bool ghost_on = false; // defaults here make it editable in test chain below
    bool val_is_eq = false;
    if (!taiType::CheckProcessCondMembMeth("GHOST", mth_rep->meth, cur_base, ghost_on, val_is_eq))
      continue;
    QAbstractButton* but = mth_rep->GetButtonRep(); //note: always exists because hasButtonRep was true
    if (ghost_on) {
      but->setEnabled(!val_is_eq);
    } else {
      but->setEnabled(val_is_eq);
    }
  }
}

void taiEditDataHost::GetImage() {
  if ((typ == NULL) || (cur_base == NULL)) return;
  if (state >= ACCEPTED ) return;
  if (state > DEFERRED1) {
    GetImage_Membs();
  }
  if (host_type != HT_CONTROL) 
    GetButtonImage();
  Unchanged();
}

void taiEditDataHost::GetImage_Membs() {
  if (inline_mode) {
    GetImageInline_impl(cur_base);
  } else {
    GetImage_Membs_def();
  }
}

void taiEditDataHost::GetImageInline_impl(const void* base) {
  taiData* mb_dat = data_el(0).SafeEl(0);
  if (mb_dat) 
    typ->it->GetImage(mb_dat, base);
}

void taiEditDataHost::GetImage_Membs_def() {
  for (int i = 0; i < membs.def_size; ++i) {
    if (show_set(i) && (data_el(i).size > 0))
      GetImage_impl(&memb_el(i), data_el(i), cur_base);
  }
}

void taiEditDataHost::GetImage_impl(const Member_List* ms, const taiDataList& dl,
  void* base)
{
  for (int i = 0; i < dl.size; ++i) {
    MemberDef* md = ms->SafeEl(i);
    taiData* mb_dat = dl.SafeEl(i);
    if ((md == NULL) || (mb_dat == NULL))
      taMisc::Error("taiEditDataHost::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i), "\n");
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
      if (!smd->ShowMember(show(), TypeItem::SC_EDIT) || smd->HasOption("HIDDEN_INLINE"))
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
  if ((typ == NULL) || (cur_base == NULL)) return;
  if (state >= ACCEPTED ) return;
  if (state > DEFERRED1) {
    GetValue_Membs();
  }
  GetButtonImage();
  Unchanged();
}

void taiEditDataHost::GetValue_Membs() {
  if (inline_mode) {
    GetValueInline_impl(cur_base);
  } else {
    GetValue_Membs_def();
  }
  if (typ->InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)cur_base;
    rbase->UpdateAfterEdit();	// hook to update the contents after an edit..
    taiMisc::Update(rbase);
  }
}

void taiEditDataHost::GetValue_Membs_def() {
  for (int i = 0; i < membs.def_size; ++i) {
    if (show_set(i) && (data_el(i).size > 0))
      GetValue_impl(&memb_el(i), data_el(i), cur_base);
  }
}

void taiEditDataHost::GetValue_impl(const Member_List* ms, const taiDataList& dl,
  void* base) const
{
  bool first_diff = true;
  for (int i = 0; i < ms->size; ++i) {
    MemberDef* md = ms->FastEl(i);
    taiData* mb_dat = dl.SafeEl(i);
    if (mb_dat == NULL)
      taMisc::Error("taiEditDataHost::GetValue_impl(): unexpected dl=NULL at i ", String(i), "\n");
    else
      md->im->GetMbrValue(mb_dat, base, first_diff);
  }
  if (!first_diff)
    taiMember::EndScript(base);
}

void taiEditDataHost::GetValueInline_impl(void* base) const {
  taiData* mb_dat = data_el(0).SafeEl(0);
  if (mb_dat) 
    typ->it->GetValue(mb_dat, base);
}

void taiEditDataHost::ResolveChanges(CancelOp& cancel_op, bool* discarded) {
  // called by root on closing, dialog on closing, etc. etc.
  if (HasChanged()) {
    GetValue();
  }
  
/* NOTE: we have changed this to always silently save
  bool forced = (cancel_op == CO_NOT_CANCELLABLE);
  if (HasChanged()) {
    int chs;
    if (forced)
      chs = taMisc::Choice("Changes have not been applied", "&Apply", "&Discard Changes");
    else
      chs = taMisc::Choice("Changes have not been applied", "&Apply", "&Discard Changes", "&Cancel");
    switch (chs) {
    case 0: // Apply
      GetValue();
      break;
    case 1: // Discard
      // only reshow if cancellable, otherwise we will be shutdown anyway,
      // so don't add unnecessary gui operations
      if (forced) Unchanged();
      else        GetImage(); // has Unchanged baked in
      if (discarded) *discarded = true;
      break;
    case  2: // Cancel (non-forced only)
      cancel_op = CO_CANCEL;
      break;
    }
  } */
}

taMisc::ShowMembs taiEditDataHost::show() const {
  return taMisc::show_gui;
}

bool taiEditDataHost::ShowMember(MemberDef* md) const {
  return (md->ShowMember(show(), TypeItem::SC_EDIT) && (md->im != NULL));
}

void taiEditDataHost::SetCurMenu(MethodDef* md) {
  if (!menu) {
    // we can't use QMainMenu on Mac, and QMenu doesn't work for some
    // reason (doesn't become visible, no matter what); but a toolbar works
//TODO: it looks slightly funny, but maybe we should do it the same on
// all platforms, to give the same look (ex. for screenshots)???
//#ifdef TA_OS_MAC
    menu = new taiToolBar(widget(), taiMisc::fonSmall,NULL); 
    vblDialog->insertWidget(0, menu->GetRep()); //note: no spacing needed after
    vblDialog->insertSpacing(1, 2);
/*#else
    menu = new taiMenuBar(taiMisc::fonSmall,
      NULL, this, NULL, widget());
    vblDialog->setMenuBar(menu->GetRep());
#endif*/
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
  cur_menu_but = taiActions::New(taiMenu::buttonmenu, taiMenu::normal, taiMisc::fonSmall,
	    NULL, this, NULL, widget());
  cur_menu_but->setLabel(men_nm);
  DoAddMethButton((QAbstractButton*)cur_menu_but->GetRep()); // rep is the button for buttonmenu
  ta_menu_buttons.Add(cur_menu_but);
}

iMainWindowViewer* taiEditDataHost::viewerWindow() const {
  iMainWindowViewer* dv = NULL; 
  if (panel) dv = panel->viewerWindow();
  return dv;
}
