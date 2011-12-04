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
#include "ta_project.h"
#include "ta_TA_type.h"

#include <QApplication>
#include <QCursor>
#include <QClipboard>
#include <qdesktopwidget.h>
#include <qevent.h>
//#include <qhbox.h>
#include <QButtonGroup>
#include <qlabel.h>
#include <qlayout.h>
#if ((QT_VERSION >= 0x040400) && defined(TA_USE_QFORMLAYOUT))
# include "iformlayout.h"
#endif
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
#include <qstring.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <QVBoxLayout>
#include <qwidget.h>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextCursor>

#include "ibutton.h"
#include "icolor.h"
#include "ilineedit.h"
#include "ieditgrid.h"
#include "iflowlayout.h"
#include "iscrollarea.h"
#include "itextedit.h"
#include "numberedtextview.h"

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
  mhiLight_color.setRgb(0x66, 0xFF, 0x66); // medium-light green
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
     setPaletteBackgroundColor(QApplication::palette().color(QPalette::Active, QPalette::Button));
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

const String taiChoiceDialog::delimiter("!|");

int taiChoiceDialog::ChoiceDialog(QWidget* parent_, const String& msg,
  const String& but_list, const char* title)
{
  taiChoiceDialog* dlg = new taiChoiceDialog(Question, QString(title),
    msg.toQString(), but_list,  parent_, false);
  // show the dialog
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  dlg->setIcon(QMessageBox::Question);
  dlg->setWindowModality(Qt::ApplicationModal);
  dlg->resize( taiM->dialogSize(taiMisc::dlgMedium | taiMisc::dlgVer) );
  int rval = dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
  return rval;
}


bool taiChoiceDialog::ErrorDialog(QWidget* parent_, const char* msg,
  const char* title, bool copy_but_, bool cancel_errs_but_)
{
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  String buts;
  if(cancel_errs_but_)
    buts = "Cancel Remaining Error Dialogs" + delimiter + "OK";
  taiChoiceDialog* dlg = new taiChoiceDialog(Warning, QString(title), 
    QString(msg), buts, parent_, copy_but_);
  dlg->setIcon(QMessageBox::Critical);
  dlg->setWindowModality(Qt::ApplicationModal);
  int chs = dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
  return chs == 0;		// true if cancel
}

void taiChoiceDialog::ConfirmDialog(QWidget* parent_, const char* msg,
  const char* title, bool copy_but_)
{
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  taiChoiceDialog* dlg = new taiChoiceDialog(Information, QString(title), 
    QString(msg), "", parent_, copy_but_);
  dlg->setIcon(QMessageBox::Question);
  dlg->setWindowModality(Qt::ApplicationModal);
  dlg->resize( taiM->dialogSize(taiMisc::dlgMedium | taiMisc::dlgVer) );
  dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
}

taiChoiceDialog::taiChoiceDialog(Icon icon, const QString& title,
  const QString& text, String but_list, QWidget* parent,
  bool copy_but_)
:inherited(icon, title, text, NoButton, parent)
{
  if (title.isEmpty()) {
    setWindowTitle(QCoreApplication::instance()->applicationName());
  }
  bgChoiceButtons = new QButtonGroup(this); // note: not a widget, invisible
  bgChoiceButtons->setExclusive(false); // not really applicable

  // create buttons
  String_PArray sa;
  if (but_list.contains(delimiter)) { // has options encoded within prompt string
    // strip leading/trailing delims, and parse all at once, to get count now
    if (but_list.startsWith(delimiter))
      but_list = but_list.after(delimiter);
    if (but_list.endsWith(delimiter))
      but_list = but_list.left(but_list.length() - delimiter.length());
    sa.SetFromString(but_list, delimiter);
  }
  // we always have at least an Ok button
  if (sa.size == 0) {
    sa.Add("&Ok");
  }
  num_chs = sa.size;

  for (int i = 0; i < sa.size; ++i) {
    ButtonRole role = AcceptRole;
    if ((i > 0) && (i == (sa.size - 1)))
      role = RejectRole;
    Constr_OneBut(sa[i], i, role);
  }

  if (copy_but_) {
    // damn copy button
    QAbstractButton* but = Constr_OneBut("Copy to clipboard", -2, ActionRole);
    connect(but, SIGNAL(clicked()), this, SLOT(copyToClipboard()));
  }
}

void taiChoiceDialog::done(int r) {
  QAbstractButton* but = clickedButton();
  int id = -1;
  if (but) 
    id = bgChoiceButtons->id(but);
  if (id < -1) return; // ignore
  // if user goes Esc, it returns -1 -- we want our last button instead
  if ((id == -1) || (r < 0))
    r = num_chs - 1;
  inherited::done(r);  
}

int taiChoiceDialog::exec() {
  // we're losing focus here so need to restore it!!
// #ifdef TA_OS_MAC
  QPointer<QWidget> m_prev_active = QApplication::activeWindow();
// #endif
  int rval = inherited::exec();
  //#ifdef TA_OS_MAC
  if((bool)m_prev_active) {
//     QApplication::setActiveWindow(m_prev_active);
    // note: above does NOT work! -- likely source of bug in cocoa 4.6.0
    m_prev_active->activateWindow();
  }
// #endif
  return rval;
}

void taiChoiceDialog::copyToClipboard() {
  QApplication::clipboard()->setText(text());
}

QAbstractButton* taiChoiceDialog::Constr_OneBut(String lbl, int id, ButtonRole role) {
  if (lbl.startsWith(" ")) // allow for one space..
    lbl = lbl.after(' ');
  if (lbl.empty())
    return NULL; // not really supposed to happen...
  
  QPushButton* but = new QPushButton(lbl, this);
  addButton(but, role);
  bgChoiceButtons->addButton(but, id);
  return but;
}

void taiChoiceDialog::keyPressEvent(QKeyEvent* ev) {
  // we allow 0-n numeric keys to be accelerators for the buttons
  int key_code = ev->key();
  if ((key_code >= Qt::Key_0) && (key_code <= Qt::Key_9)) {
    int but_index = key_code - Qt::Key_0;
    QPushButton* but = (QPushButton*)bgChoiceButtons->button(but_index);
    if (but != NULL) {
      // simulate effect of pressing the button
      but->click();
      ev->accept(); // swallow key
    }
  } else {
    QDialog::keyPressEvent(ev);
  }
}


//////////////////////////
//   iHostDialog		//
//////////////////////////

iHostDialog::iHostDialog(taiDataHostBase* owner_, QWidget* parent, int wflags)
:iDialog(parent, (Qt::WFlags)wflags) 
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

iHostDialog::~iHostDialog() {
  if (owner != NULL) {
    owner->WidgetDeleting(); // removes our ref
    owner = NULL;
  }
}

void iHostDialog::closeEvent(QCloseEvent* ev) {
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

bool iHostDialog::post(bool modal) {
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

void iHostDialog::dismiss(bool accept_) {
  if (accept_)
    accept();
  else
    reject();
}

void iHostDialog::iconify() {
  // Iv compatibility routine
  if (isModal() || isMinimized()) return;
  showMinimized();
}

void iHostDialog::deiconify() {
   // Iv compatibility routine
  if (isModal() || !isMinimized()) return;
  showNormal();
}

void iHostDialog::setButtonsWidget(QWidget* widg) {
  widg->setParent(this);
  layOuter->addWidget(widg);
}

void iHostDialog::setCentralWidget(QWidget* widg) {
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
    owner->deleteLater(); // no one else does this!
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

void EditDataPanel::UpdatePanel_impl() {
  if (owner) owner->Refresh();
}

const iColor EditDataPanel::GetTabColor(bool selected, bool& ok) const {
  if (owner) {
    ok = true;
    return (selected) ? owner->bg_color : owner->bg_color_dark;
  } else       
    return inherited::GetTabColor(selected, ok);
}

bool EditDataPanel::HasChanged_impl() {
  if (owner) return owner->HasChanged();
  else       return false;
}

String EditDataPanel::panel_type() const {
  static String str("Properties");
  return str;
}

void EditDataPanel::UpdatePanel() {
  //   iDataPanel::UpdatePanel_impl(); // for tab stuff -- skip our direct inherit!
  // actually, our refresh has built-in vis testing, so don't do visibility check which
  // the parent guy does!  this allows edit guy to update buttons even if it is not
  // itself visible..
  UpdatePanel_impl();
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

QWidget* EditDataPanel::firstTabFocusWidget() {
  return editDataHost()->firstTabFocusWidget();
}


//////////////////////////////////
// iMethodButtonMgr		//
//////////////////////////////////

iMethodButtonMgr::iMethodButtonMgr(QObject* parent)
:inherited(parent)
{
  widg = NULL;
  m_lay = NULL;
  Init();
}

iMethodButtonMgr::iMethodButtonMgr(QWidget* widg_, QLayout* lay_, QObject* parent)
:inherited(parent)
{
  widg = widg_;
  m_lay = lay_;
  Init();
}

iMethodButtonMgr::~iMethodButtonMgr()
{
  Reset();
}

void iMethodButtonMgr::Init() {
  host = NULL;
  show_meth_buttons = false;
  base = NULL;
  typ = NULL;
  cur_menu_but = NULL;
//  m_lay = new iFlowLayout(this, 3, taiM->hspc_c, (Qt::AlignCenter));
}

void iMethodButtonMgr::AddMethButton(taiMethodData* mth_rep, const String& label) {
  QWidget* but = mth_rep->GetButtonRep();
  DoAddMethButton(but);
  if (label.nonempty() && but->inherits("QAbstractButton")) {
    ((QAbstractButton*)but)->setText(label);
  }
}

void iMethodButtonMgr::Reset() {
  cur_menu_but = NULL;
  while (meth_el.size > 0) {
    taiData* dat = meth_el.Pop();
    dat->Delete();
  }
  while (ta_menu_buttons.size > 0) {
    taiData* dat = ta_menu_buttons.Pop();
    dat->Delete();
  }
  setBase(NULL);
  show_meth_buttons = false;
}

void iMethodButtonMgr::Constr(QWidget* widg_, QLayout* lay_, 
  taBase* base_, IDataHost* host_)
{
  widg = widg_;
  m_lay = lay_;  
  Constr_impl(base_, host_);
}

void iMethodButtonMgr::Constr(taBase* base_, IDataHost* host_) 
{
  Constr_impl(base_, host_);
}

void iMethodButtonMgr::Constr_impl(taBase* base_, IDataHost* host_) 
{
  Reset();
  host = host_; // prob not needed
  setBase(base_);
  show_meth_buttons = false;
  Constr_Methods_impl();
}

void iMethodButtonMgr::Constr_Methods_impl() {
  if (typ == NULL) return;

  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((md->im == NULL) || (md->name == "Edit")) // don't put edit on edit dialogs..
      continue;
    if (!md->ShowMethod()) continue;
    taiMethod* im = md->im;
    if (im == NULL)
      continue;

    taiMethodData* mth_rep = NULL;
    if (md->HasOption("MENU_BUTTON")) {
      SetCurMenuButton(md);
      mth_rep = im->GetMenuMethodRep(base, host, NULL, NULL);
      mth_rep->AddToMenu(cur_menu_but);
      meth_el.Add(mth_rep);
    }
    /*
    if (md->HasOption("MENU")) {
      SetCurMenu(md);
      mth_rep = im->GetMenuMethodRep(base, host, NULL, NULL);
      mth_rep->AddToMenu(cur_menu);
      meth_el.Add(mth_rep);
    }*/
    if (md->HasOption("BUTTON")) {
      mth_rep = im->GetButtonMethodRep(base, host, NULL, widg);
      AddMethButton(mth_rep);
      meth_el.Add(mth_rep);
    }
  }
}

void iMethodButtonMgr::DataLinkDestroying(taDataLink* dl) {
  base = NULL;
  //TODO: delete the buttons etc.
}
 
void iMethodButtonMgr::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
  if (dcr > DCR_ITEM_UPDATED_ND) return;
  GetImage();
}

void iMethodButtonMgr::DoAddMethButton(QWidget* but) {
  show_meth_buttons = true;
  // we use "medium" size for buttons
  but->setFont(taiM->buttonFont(taiMisc::fonMedium));
  but->setFixedHeight(taiM->button_height(taiMisc::sizMedium));
  if (but->parent() != widg) {
    widg->setParent(but);
  }
  m_lay->addWidget(but);
  but->show(); // needed when rebuilding
}

void iMethodButtonMgr::GetImage() {
  if(!widg || !widg->isVisible()) return;

  for (int i = 0; i < meth_el.size; ++i) {
    taiMethodData* mth_rep = (taiMethodData*)meth_el.SafeEl(i);
    if ( !(mth_rep->hasButtonRep())) //note: construction forced creation of all buttons
      continue;

    mth_rep->UpdateButtonRep();
  }
}

void iMethodButtonMgr::setBase(taBase* ta) {
  if (base == ta) return;
  if (base) {
    base->RemoveDataClient(this);
    base = NULL;
    typ = NULL;
  }
  base = ta;
  if (base) {
    base->AddDataClient(this);
    typ = base->GetTypeDef();
  }
}

void iMethodButtonMgr::SetCurMenuButton(MethodDef* md) {
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
	    NULL, host, NULL, widg);
  cur_menu_but->setLabel(men_nm);
  DoAddMethButton(cur_menu_but->GetRep()); // rep is the button for buttonmenu
  ta_menu_buttons.Add(cur_menu_but);
}



//////////////////////////////////
// 	taiDataHostBase		//
//////////////////////////////////

#define LAYBODY_MARGIN	1
#define LAYBODY_SPACING	0

void taiDataHostBase::MakeDarkBgColor(const iColor& bg, iColor& dk_bg) {
  dk_bg.set(taiMisc::ivBrightness_to_Qt_lightdark(bg, taiM->edit_darkbg_brightness));
}


taiDataHostBase::taiDataHostBase(TypeDef* typ_, bool read_only_,
  bool modal_, QObject* parent)
:QObject(parent)
{
  read_only = read_only_;
  modified = false;
  typ = typ_;
  root = NULL;
  modal = modal_;
  state = EXISTS;

  // default background colors
  setBgColor(QApplication::palette().color(QPalette::Active, QPalette::Background));
  
  InitGuiFields(false);

  if (taiM == NULL) ctrl_size = taiMisc::sizMedium;
  else              ctrl_size = taiM->ctrl_size; // for early type system instance when no taiM yet
  row_height = 1; // actual value set in Constr
  mouse_button = 0;
  no_ok_but = false;
  dialog = NULL;
  host_type = HT_DIALOG; // default, set later
  reshow_req = false;
  reshow_req_forced = false;
  reconstr_req = false;
  defer_reshow_req = false;
  getimage_req = false;
  apply_req = false;
  reshow_on_apply = true;
  warn_clobber = false;
}


taiDataHostBase::~taiDataHostBase() {
  if (dialog != NULL) DoDestr_Dialog(dialog);
}

void taiDataHostBase::InitGuiFields(bool) {
  mwidget = NULL;
  vblDialog = NULL;
  prompt = NULL;
  body = NULL;
  scrBody = NULL;
  widButtons = NULL;
  layButtons = NULL;
  okbut = NULL;
  canbut = NULL;
  apply_but = NULL;
  revert_but = NULL;
  help_but = NULL;
}

void taiDataHostBase::StartEndLayout(bool start) {
  if (start) {
    widget()->setUpdatesEnabled(false);
  } else { // end
    widget()->setUpdatesEnabled(true);
  }
}

void taiDataHostBase::Apply() {
  if (warn_clobber) {
    int chs = taMisc::Choice("Warning: this object has changed since you started editing -- if you apply now, you will overwrite those changes -- what do you want to do?",
			     "&Apply", "&Revert", "&Cancel");
    if(chs == 1) {
      Revert();
      return;
    }
    if(chs == 2)
      return;
  }
  ++updating;
  GetValue();
  if (reshow_on_apply)
    defer_reshow_req = true; // forces rebuild so CONDSHOW guys work
  Refresh(); // GetImage/Unchanged, unless a defer_reshow pending
  --updating;
}

taBase*	taiDataHostBase::Base_() const {
  if (typ && typ->InheritsFrom(&TA_taBase))
    return (taBase*)root;
  else return NULL;
}

void taiDataHostBase::Revert() {
  GetImage();
  Unchanged();
}

void taiDataHostBase::DoDestr_Dialog(iHostDialog*& dlg) { // common sub-code for destructing a dialog instance
  if (dlg != NULL) {
    dlg->owner = NULL; // prevent reverse deletion
    if(!taMisc::in_shutdown) {
      dlg->deleteLater(); 
      dlg->close(); // destructive close
    }
    dlg = NULL;
  }
}

void taiDataHostBase::Cancel() { //note: taiEditDataHost takes care of cancelling panels
  state = CANCELED;
  Cancel_impl();
}

void taiDataHostBase::Cancel_impl() { //note: taiEditDataHost takes care of cancelling panels
  if (dialog) {
    dialog->dismiss(false);
  }
  if (prompt) {
    prompt->setText("");
  }
}

void taiDataHostBase::Changed() {
  if (updating) return; // spurious
  if (modified) return; // handled already
  modified = true;
  if (apply_but != NULL) {
      apply_but->setEnabled(true);
      apply_but->setHiLight(true);

  }
  if (revert_but != NULL) {
      revert_but->setEnabled(true);
  }
}

/* NOTE: Constr_Xxx methods presented in execution (not lexical) order */
void taiDataHostBase::Constr(const char* aprompt, const char* win_title,
  HostType host_type_, bool deferred) 
{
  if (aprompt) m_def_prompt = String(aprompt);
  if (win_title) m_def_title = String(win_title);
  host_type = host_type_;
  Constr_Strings();
  Constr_Widget();
  if (host_type != HT_CONTROL) 
    Constr_Methods();
  Constr_RegNotifies(); // taiEditHost registers notifies
  state = DEFERRED1;
  if (deferred) {GetImage(true); return;}
  ConstrDeferred(); // else finish it now!
}



void taiDataHostBase::ConstrDeferred() {
  if (state != DEFERRED1) {
    taMisc::Error("taiDataHost::ConstrDeferred2: expected host to be in state DEFERRED1");
    return;
  }
  Constr_impl();
  state = CONSTRUCTED;
  GetImage();
}

void taiDataHostBase::Constr_impl() {
  row_height = taiM->max_control_height(ctrl_size); // 3 if using line between; 2 if using even/odd shading
  StartEndLayout(true);
  Constr_Prompt();
  Constr_Box();
  Constr_Body();
  if (host_type != HT_CONTROL) 
    Insert_Methods(); // if created, AND unowned
  // create container for ok/cancel/apply etc. buttons
  widButtons = new QWidget(); // parented when we do setButtonsWidget
  widButtons->setAutoFillBackground(true);
  SET_PALETTE_BACKGROUND_COLOR(widButtons, bg_color);
  layButtons = new QHBoxLayout(widButtons);
  layButtons->setMargin(0);
//def  layButtons->setMargin(2); // facilitates container
  Constr_Buttons();
  Constr_Final();
  StartEndLayout(false);
//NOTE: do NOT do a processevents -- it causes improperly nested event calls
// in some cases, such as constructing the browser
}

void taiDataHostBase::Constr_Strings() {
  // default behavior just sets actuals to defs,
  // but usually the defs are either replaced, or elaborated
  prompt_str = def_prompt();
  win_str = def_title();
}

void taiDataHostBase::Constr_Widget() {
  if (mwidget != NULL) return;
  mwidget = new QWidget();
  SET_PALETTE_BACKGROUND_COLOR(widget(), bg_color);
  widget()->setFont(taiM->dialogFont(ctrl_size));
  vblDialog = new QVBoxLayout(widget()); //marg=2
  vblDialog->setSpacing(0); // need to manage ourself to get nicest look
  vblDialog->setMargin(2);
}

void taiDataHostBase::Constr_Prompt() {
  if (prompt != NULL) return; // already constructed
//NOTE: don't use RichText format because it doesn't word wrap!
  prompt = new QLabel(widget()); 
  prompt->setWordWrap(true); // so it doesn't dominate hor sizing
  QFont f = taiM->nameFont(taiMisc::fonBig);
  f.setBold(true); 
  prompt->setFont(f);
  prompt->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  vblDialog->addWidget(prompt);
  prompt->setText(prompt_str);
  vblDialog->addSpacing(2);
}

void taiDataHostBase::Constr_Buttons() {
  QWidget* par = widButtons;

  if(!isDialog()) {
    help_but = new HiLightButton("&Help", par);
    layButtons->addWidget(help_but, 0, (Qt::AlignVCenter));
    connect(help_but, SIGNAL(clicked()), this, SLOT(Help()) );
  }

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
      layButtons->addSpacing(20); 
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
//nuke  layButtons->addSpacing(10); // don't flush hard right
}


void taiDataHostBase::DataLinkDestroying(taDataLink* dl) {
// TENT, TODO: confirm this is right...
//NO!  if (!isConstructed()) return;
  Cancel();
  root = NULL;
}
 
void taiDataHostBase::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
//inherited class completely implements
}

void taiDataHostBase::DoConstr_Dialog(iHostDialog*& dlg) {
  // common subcode for creating a dialog -- used by the taiHostDialog and taiEditDialog cousin classes
  if (dlg) return; // already constructed
  if (modal) // s/b parented to current win
    dlg = new iHostDialog(this, QApplication::activeWindow());
  else 
    dlg = new iHostDialog(this, NULL, Qt::WindowMinimizeButtonHint);
  // note: X can't seem to handle more than 12-14 windows, so making these top-level is an issue
  // BUT it is also highly unusable to make them owned, since then they obscure parent window
  dlg->setWindowTitle(win_str);
//  dlg->setMinimumWidth(400); //TODO: maybe parameterize; note: would need to set layout FreeResize as well
}

void taiDataHostBase::DoRaise_Dialog() {
  if (!dialog) return;
  if (!modal) {
    dialog->raise();
    dialog->setFocus();
  }
}

int taiDataHostBase::Edit(bool modal_, int min_width, int min_height) {
  // only called if isDialog() true
  if (state != CONSTRUCTED)
    return false;
  modal = modal_;
  if (dialog == NULL) DoConstr_Dialog(dialog);
//dialog->resize(dialog->minimumWidth(), 1);
  dialog->setCentralWidget(widget());
  dialog->setButtonsWidget(widButtons);
  if(min_width > 0)
    dialog->setMinimumWidth(min_width);
  if(min_height > 0)
    dialog->setMinimumHeight(min_height);
  //note: following is hack from rebasing
  if (!modal && (GetTypeDef()->InheritsFrom(&TA_taiDataHost))) {
    taiMisc::active_dialogs.AddUnique((taiDataHost*)this); // add to the list of active dialogs
  }
  state = ACTIVE;
  int rval = dialog->post(modal);
  if(modal)
    Cancel();
  return rval;
}

void taiDataHostBase::GetImage_PromptTitle() {
  Constr_Strings(); // in case changed, ex. desc change or name change
  if (prompt) // always made???
    prompt->setText(prompt_str);
  // dialogs only
  if (dialog)
    dialog->setWindowTitle(win_str);
}

void taiDataHostBase::setBgColor(const iColor& new_bg) {
  bg_color = new_bg;
  MakeDarkBgColor(bg_color, bg_color_dark);
}

void taiDataHostBase::Help() {
  taBase* obj = Base_();
  if(!obj) return;
  obj->Help();
}

void taiDataHostBase::Ok() { //note: only used for Dialogs
  //note: IV version used to scold user for pressing Ok in a readonly dialog --
  // we just interpret Ok as Cancel
  if (read_only) {
    Cancel();
    return;
  }
  Ok_impl();
  state = ACCEPTED;
  mouse_button = okbut->mouse_button;
  if (dialog) {
    dialog->dismiss(true);
  }
}

void taiDataHostBase::Ok_impl() { 
}

void taiDataHostBase::Refresh() {
  Refresh_impl(defer_reshow_req);
}

void taiDataHostBase::Unchanged() {
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

void taiDataHostBase::WidgetDeleting() {
  InitGuiFields(); // called virtually
  state = ZOMBIE;
}

taiDataHostBase_List taiDataHostBase::async_apply_list;
taiDataHostBase_List taiDataHostBase::async_reshow_list;
taiDataHostBase_List taiDataHostBase::async_reconstr_list;
taiDataHostBase_List taiDataHostBase::async_getimage_list;

bool taiDataHostBase::AsyncWaitProc() {
  static bool in_waitproc = false;

  if(async_apply_list.size == 0 && async_reshow_list.size == 0 &&
     async_getimage_list.size == 0 && async_reconstr_list.size == 0) return false;

  if(in_waitproc) return false;
  in_waitproc = true;

  // order is important here: don't want to have one thing trigger another right away..
  bool did_some = false;
  for(int i=0;i<async_getimage_list.size;i++) {
    taiDataHostBase* dhb = async_getimage_list.SafeEl(i);
    if(!dhb) continue;
    dhb->getimage_req = false;
    if ((dhb->state & STATE_MASK) < CANCELED) {
      dhb->GetImage(false);
      did_some = true;
    }
  }
  async_getimage_list.Reset();
  if(did_some) {
    goto leave;
  }

  for(int i=0;i<async_reconstr_list.size;i++) {
    taiDataHostBase* dhb = async_reconstr_list.SafeEl(i);
    if(!dhb) continue;
    if(dhb->reconstr_req) {
      dhb->reconstr_req = false;
      dhb->ReConstr_Body();
      dhb->state &= ~SHOW_CHANGED;
      did_some = true;
    }
  }
  async_reconstr_list.Reset();
  if(did_some) {
    goto leave;
  }

  for(int i=0;i<async_reshow_list.size;i++) {
    taiDataHostBase* dhb = async_reshow_list.SafeEl(i);
    if(!dhb) continue;
    if(dhb->reshow_req) {
      dhb->reshow_req = false;
      if(dhb->state == ACTIVE) {
        dhb->ReShow(dhb->reshow_req_forced);
	did_some = true;
      }
    }
  }
  async_reshow_list.Reset();
  if(did_some) {
    goto leave;
  }

  for(int i=0;i<async_apply_list.size;i++) {
    taiDataHostBase* dhb = async_apply_list.SafeEl(i);
    if(!dhb) continue;
    if(dhb->apply_req) {
      dhb->apply_req = false;
      if(dhb->state == ACTIVE) {
        dhb->Apply();
	did_some = true;
      }
    }
  }
  async_apply_list.Reset();

 leave:
  if(did_some) {
    taMisc::RunPending();
    taMisc::RunPending();
    taMisc::RunPending();

    if(async_apply_list.size != 0 || async_reshow_list.size != 0 ||
       async_getimage_list.size != 0 || async_reconstr_list.size != 0)
      taMisc::do_wait_proc = true; // go around again
  }

  in_waitproc = false;
  return true;
}

void taiDataHostBase::Apply_Async() {
//    Apply();	     // no reason to actually async this..
  if (apply_req) return; // already waiting
  if (state != ACTIVE) return;
  apply_req = true;
  taMisc::do_wait_proc = true;
  async_apply_list.Link(this);
}

void taiDataHostBase::ReShow_Async(bool forced) {
  if (reshow_req) return; // already waiting
  if ((state & STATE_MASK) >= CANCELED) return;
  reshow_req = true;
  reshow_req_forced = forced;
  taMisc::do_wait_proc = true;
  async_reshow_list.Link(this);
}

void taiDataHostBase::ReConstr_Async() {
  if(reconstr_req) return;
  reconstr_req = true;
  taMisc::do_wait_proc = true;
  async_reconstr_list.Link(this);
}

void taiDataHostBase::GetImage_Async() {
  // reshow does a getimage, so ignore if a reshow pending
//   cerr << "req getimage async on: " << typ->name << endl;
//   taMisc::FlushConsole();
//   if (getimage_req) return; // already waiting
  // IMPORTANT: if above getimage_req is honored, then it can end up with some blank 
  // program control panels -- needs the redundant getimage guys..  doesn't happen very often
  // we can get these for DEFERRED as well, for buttons, ex/esp Program panels
//   if ((state & STATE_MASK) >= CANCELED) {
//      cerr << "getimage async cancelled on: " << typ->name << " state: " << (state & STATE_MASK) << endl;
//     taMisc::FlushConsole();
//     return;
//   }
  getimage_req = true;
  taMisc::do_wait_proc = true;
  async_getimage_list.Link(this);
}

void taiDataHostBase::DebugDestroy(QObject* obj) {
  cerr << "debug destroying: " << endl;
}


//////////////////////////////////
//  taiDataHost_impl		//
//////////////////////////////////

void taiDataHost_impl::DoFillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id, taBase* rbase, MemberDef* md, QWidget* menu_par,
  QObject* slot_obj, const char* slot)
{
  // have to be a taBase to use SelEdit
  if (!rbase || !md) return; 
//obs  if (!(membs.GetFlatDataItem(sel_item_idx, &md) && md))
//    return; 
  // get list of select edits
  taProject* proj = dynamic_cast<taProject*>(rbase->GetThisOrOwner(&TA_taProject));
  if (!proj || proj->edits.leaves == 0) return;

  // if any edits, populate menu for adding, for all seledits not already on
  QMenu* sub = menu->addMenu("Add to SelectEdit");
  connect(sub, SIGNAL(triggered(QAction*)), slot_obj, slot);
  sub->setFont(menu->font());
  QAction* act = NULL; // we need to track last one
  for (int i = 0; i < proj->edits.leaves; ++i) {
    SelectEdit* se = proj->edits.Leaf(i);
    act = sub->addAction(se->GetName()/*, slot_obj, slot*/); //
    act->setData(i); // sets data, which is what is used in signal, to i
    // determine if already on that seledit, and disable if it is (we do this to maintain constant positionality in menu)
    if (se->FindMbrBase(rbase, md) >= 0)
      act->setEnabled(false);
  }
  if (sub->actions().count() == 0)
    sub->setEnabled(false); // show item for usability, but disable
    
  // TODO: if any edits, populate menu for removing, for all seledits already on
  sub = menu->addMenu("Remove from SelectEdit");
  connect(sub, SIGNAL(triggered(QAction*)), slot_obj, slot);
  sub->setFont(menu->font());
  for (int i = 0; i < proj->edits.leaves; ++i) {
    SelectEdit* se = proj->edits.Leaf(i);
    act = sub->addAction(se->GetName()/*, slot_obj, slot*/);
    act->setData(i); // sets data, which is what is used in signal, to i
    // determine if already on that seledit, and disable if it isn't
    if (se->FindMbrBase(rbase, md) < 0)
      act->setEnabled(false);
  }
  if (sub->actions().count() == 0)
    sub->setEnabled(false); // show item for usability, but disable
}

void taiDataHost_impl::GetName(MemberDef* md, String& name, String& help_text) {
  name = md->GetLabel();
  help_text = ""; // just in case
  MemberDef::GetMembDesc(md, help_text, "");
}

taiDataHost_impl::taiDataHost_impl(TypeDef* typ_, bool read_only_, bool modal_, QObject* parent)
:inherited(typ_, read_only_, modal_, parent)
{
  InitGuiFields(false);

  sel_item_dat = NULL;
  sel_item_mbr = NULL;
  sel_item_base = NULL;
  rebuild_body = false;
  sel_edit_mbrs = true; // inherited guys can turn off
}

taiDataHost_impl::~taiDataHost_impl() {
}

// note: called non-virtually in our ctor, and virtually in WidgetDeleting
void taiDataHost_impl::InitGuiFields(bool virt) { 
  if (virt)  inherited::InitGuiFields(virt);
  frmMethButtons = NULL;
  layMethButtons = NULL;
  show_meth_buttons = false;
}

const iColor taiDataHost_impl::colorOfRow(int row) const {
  if ((row % 2) == 0) {
    return bg_color;
  } else {
    return bg_color_dark;
  }
}

void taiDataHost_impl::Cancel_impl() { //note: taiEditDataHost takes care of cancelling panels
  inherited::Cancel_impl();
  // delete any methods
  if (frmMethButtons) {
    QWidget* t = frmMethButtons; // avoid any possible callback issues
    frmMethButtons = NULL;
    delete t;
  }

  warn_clobber = false; // just in case
}

void taiDataHost_impl::Constr_Methods() {
  Constr_Methods_impl();
}

void taiDataHost_impl::Constr_Methods_impl() { //note: conditional constructions used by SelectEditHost to rebuild methods
  QFrame* tmp = frmMethButtons;
  if (!frmMethButtons) {
    show_meth_buttons = false; // set true if any created
    tmp = new QFrame(); // tmp = new QFrame(widget());
    tmp->setVisible(false); // prevents it showing as global win in some situations
    tmp->setAutoFillBackground(true); // for when disconnected from us
    SET_PALETTE_BACKGROUND_COLOR(tmp, bg_color);
    tmp->setFrameStyle( QFrame::Panel | QFrame::Sunken );
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

taBase* taiDataHost_impl::GetMembBase_Flat(int) {
  return (taBase*)root; // pray!
}

taBase* taiDataHost_impl::GetMethBase_Flat(int) {
  return (taBase*)root; // pray!
}

void taiDataHost_impl::Insert_Methods() {
  //NOTE: for taiEditDataHost, menus are always put in widget() even in deferred
  if (frmMethButtons && !frmMethButtons->parentWidget()) {
    // meth buttons always at bottom of inner layout
    vblDialog->addSpacing(2);
    vblDialog->addWidget(frmMethButtons);
    frmMethButtons->setVisible(show_meth_buttons); // needed for deferred insert
  }
}

void taiDataHost_impl::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
//note: nothing in base, by design
  //NOTE: list/group subclasses typically detect changes in their GetImage routine
  //  so we don't really subclass this routine or explicitly detect the list/group notifies
  // note: because of deferred construction, we may still need to update buttons/menus

  if (state == DEFERRED1) {
    Refresh_impl(false);
    return;
  }
  // note: we should have unlinked if cancelled, but if not, ignore if cancelled
  if (!isConstructed()) return;
  
  if (updating) return; // it is us that caused this
  if (dcr == DCR_STRUCT_UPDATE_END) {
    Refresh_impl(true);
  } 
  // RESOLVE_NOW is typically invoked by a button method
  else if (dcr == DCR_RESOLVE_NOW)
  {
    CancelOp cancel_op = CO_PROCEED; //note: may make more sense to be CO_NOT_CANCELLABLE
    ResolveChanges(cancel_op);
  }
  // we really just want to ignore the BEGIN-type guys, otherwise accept all others
  else if (!((dcr == DCR_STRUCT_UPDATE_BEGIN) ||
    (dcr == DCR_DATA_UPDATE_BEGIN)))
  {
    Refresh_impl(false);
  }
}

void taiDataHost_impl::Refresh_impl(bool reshow) {
  // if no changes have been made in this instance, then just refresh,
  // otherwise, user will have to decide what to do, i.e., revert
//   cerr << "Refresh_impl on: " << typ->name << " reshow: " << reshow << endl;
//   taMisc::FlushConsole();

  if (HasChanged()) {
    warn_clobber = true;
    if (reshow) defer_reshow_req = true; // if not already set
  } else {
    if (reshow) {
      ReShow();			// this must NOT be _Async -- otherwise doesn't work with carbon qt on mac
    }
    else {
      GetImage_Async();
    }
  }
}

void taiDataHost_impl::label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e) {
  QMenu* menu = new QMenu(widget());
  //note: don't use body for menu parent, because some context menu choices cause ReShow, which deletes body items!
  Q_CHECK_PTR(menu);
  int last_id = -1;
  sel_item_dat = (taiData*)qvariant_cast<ta_intptr_t>(sender->userData()); // pray!!!
  if (sel_item_dat) {
    sel_item_mbr = sel_item_dat->mbr;
    sel_item_base = sel_item_dat->Base();
  } else {
    sel_item_mbr = NULL;
    sel_item_base = NULL;
  }
  FillLabelContextMenu(menu, last_id);
  if (menu->actions().count() > 0)
    menu->exec(sender->mapToGlobal(e->pos()));
  delete menu;
}

void taiDataHost_impl::FillLabelContextMenu(QMenu* menu, int& last_id) {
  // only add member help if exists
  if (sel_item_mbr) {
    menu->addAction("&Help", this, SLOT(helpMenu_triggered()));
  }
}

void taiDataHost_impl::helpMenu_triggered() {
  iHelpBrowser::StatLoadMember(sel_item_mbr);
}

void taiDataHost_impl::Iconify(bool value) {
  if (!dialog) return;
  if (value) dialog->iconify();
  else       dialog->deiconify();
}

void taiDataHost_impl::Ok_impl() { //note: only used for Dialogs
  // NOTE: we herein might be bypassing the clobber warn, but shouldn't really
  //be possible to modify ourself externally inside a dialog
  inherited::Ok_impl();
  if (HasChanged()) {
    GetValue();
    Unchanged();
  }
}

void taiDataHost_impl::ClearBody(bool waitproc) {
//   cerr << "ClearBody on: " << typ->name << " waitproc: " << waitproc << endl;
//   taMisc::FlushConsole();
  StartEndLayout(true);
  ClearBody_impl();
  if (!(state & SHOW_CHANGED)) return; // probably just destroying
//   if(waitproc) { // these guys cause redraw to flash a lot and lose scroll position!
//     taMisc::RunPending();
//     taMisc::RunPending();
//     taMisc::RunPending();
//   }
  //  ReConstr_Async(); 
  // can actually just do this live here
  ReConstr_Body();
  state &= ~SHOW_CHANGED;
  reconstr_req = false;
  StartEndLayout(false);
}

void taiDataHost_impl::ClearBody_impl() {
  taiMisc::DeleteChildrenLater(body);
}

void taiDataHost_impl::ReConstr_Body() {
  if (!isConstructed()) return;
//   cerr << "ReConstr_Body on: " << typ->name << endl;
//   taMisc::FlushConsole();
  rebuild_body = true;
  ++updating;			// prevents spurious changed flags from coming in
  Constr_Body();
  {		// this is key for selectedit rebuilding on bool toggles, for example
    taMisc::RunPending();
    taMisc::RunPending();
    taMisc::RunPending();
  }
  GetImage_Async();		// async all the way -- otherwise doesn't work
//   GetImage(false);
  rebuild_body = false;		// in case..
  --updating;
}

bool taiDataHost_impl::ReShow(bool force) {
//   cerr << "ReShow on: " << typ->name << endl;
//   taMisc::FlushConsole();
// if not visible, we may refresh the buttons if visible, otherwise nothing else
  if (!mwidget) return false;//. huh?
  //note: extremely unlikely to be updating if invisible, so we do this test here
  if (!mwidget->isVisible()) {
    defer_reshow_req = true;
    GetImage(false); // no-force; invisible-friendly
    return false;    // that's it -- otherwise clears when not visible! -- absence of this was source of major crazy bug fixed 11/18/09..
  }
//note: only called with force from ReShowEdits, typ only from a SelEdit dialog
  if (!updating) {
    // was not us that caused this...
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
        warn_clobber = true; 
        defer_reshow_req = true;
        return false;
      }
    }
  }
  state |= SHOW_CHANGED; 
  ClearBody(); // rebuilds body after clearing -- but SHOW_CHANGED prevents GetImage...
  defer_reshow_req = false; // if it was true
  return true;
}

void taiDataHost_impl::Revert_force() {
  if (modified && (taMisc::auto_revert == taMisc::CONFIRM_REVERT)) {
    int chs = taMisc::Choice
      ("Revert: You have edited the data -- apply, or revert and lose changes?",
      "Apply", "Revert", "Cancel");
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

taMisc::ShowMembs taiDataHost_impl::show() const {
  return taMisc::show_gui;
}

void taiDataHost_impl::SetRevert(){
  if (updating || (taMisc::is_loading)) return;
  if (!revert_but) return;
  revert_but->setHiLight(true);
  revert_but->setEnabled(true);
}

void taiDataHost_impl::UnSetRevert(){
  if (!revert_but) return;
  revert_but->setHiLight(false);
  revert_but->setEnabled(false);
}


//////////////////////////////////
// 	taiDataHost		//
//////////////////////////////////

iLabel* taiDataHost::MakeInitEditLabel(const String& name, QWidget* par,
  int ctrl_size, const String& desc, taiData* buddy,
  QObject* ctx_obj, const char* ctx_slot, int row)
{
  iLabel* label = new iLabel(row, name, par);
  label->setFont(taiM->nameFont(ctrl_size));
  label->setFixedHeight(taiM->label_height(ctrl_size));
  if (buddy) label->setUserData((ta_intptr_t)buddy);
  if (ctx_obj) QObject::connect(
    label, SIGNAL(contextMenuInvoked(iLabel*, QContextMenuEvent*)),
      ctx_obj, ctx_slot );
// if it is an iLabel connecting a taiData, then connect the highlighting for non-default values
  QWidget* buddy_widg = NULL;
  if (buddy) {
    buddy->setLabel(label);
    buddy_widg = buddy->GetRep();
    QObject::connect(buddy, SIGNAL(settingHighlight(bool)),
        label, SLOT(setHighlight(bool)) );
  }
  

  if (!desc.empty()) {
    label->setToolTip(desc);
    label->setStatusTip(desc);
    if (buddy_widg != NULL) {
      buddy_widg->setToolTip(desc);
      buddy_widg->setStatusTip(desc);
    }
  }
  return label;
}


taiDataHost::taiDataHost(TypeDef* typ_, bool read_only_, bool modal_, QObject* parent)
:inherited(typ_, read_only_, modal_, parent)
{
  InitGuiFields(false);

  cur_row = 0;
  dat_cnt = 0;
  first_tab_foc = NULL;
}

taiDataHost::~taiDataHost() {
}

// note: called non-virtually in our ctor, and virtually in WidgetDeleting
void taiDataHost::InitGuiFields(bool virt) { 
  if (virt)  inherited::InitGuiFields(virt);
  splBody = NULL;
  scrBody = NULL;
  layBody = NULL;
  first_tab_foc = NULL;
}

int taiDataHost::AddSectionLabel(int row, QWidget* wid, const String& desc) {
  if (row < 0)
    row = layBody->rowCount();
  QFont f(taiM->nameFont(ctrl_size));
  f.setBold(true);
  wid->setFont(f);
  wid->setFixedHeight(row_height);
  SET_PALETTE_BACKGROUND_COLOR(wid, colorOfRow(row));
  if (!desc.empty()) {
    wid->setToolTip(desc);
  }
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
#if ((QT_VERSION >= 0x040400) && defined(TA_USE_QFORMLAYOUT))
  layBody->addRow(layH);
#else
  layBody->setRowMinimumHeight(row, row_height + (2 * LAYBODY_MARGIN)); //note: margins not automatically baked in to max height
  layBody->addLayout(layH, row, 0, 1, 2, (Qt::AlignLeft | Qt::AlignVCenter)); 
#endif  
  wid->show(); // needed for rebuilds, to make the widget show
  return row;
}

int taiDataHost::AddNameData(int row, const String& name, const String& desc,
   QWidget* data, taiData* buddy, MemberDef* md, bool fill_hor)
{
  if (row < 0)
    row = layBody->rowCount();
//LABEL
  iLabel* label = MakeInitEditLabel(name, body, ctrl_size, desc, buddy,
    this, SLOT(label_contextMenuInvoked(iLabel*, QContextMenuEvent*)), row); 
  
//DATA
  // note1: margins not automatically baked in to max height
  // note2: if guy goes invisible, we'll set its row height to 0 in GetImage
  QHBoxLayout* lay_dat = new QHBoxLayout();
  lay_dat->setMargin(0);
  lay_dat->addWidget(data, 0, Qt::AlignVCenter/*, (Qt::AlignLeft | Qt::AlignVCenter)*/);
  if (!fill_hor) lay_dat->addStretch();
  
// add label/body and show
#if ((QT_VERSION >= 0x040400) && defined(TA_USE_QFORMLAYOUT))
  label->setMinimumHeight(row_height);
  label->setMaximumHeight(row_height);
  lay_dat->addStrut(row_height); // make it full height, so controls center
  layBody->addRow(label, lay_dat);
#else

  
  QHBoxLayout* lay_lbl = new QHBoxLayout();
  lay_lbl->setMargin(0);
  lay_lbl->addWidget(label, 0, (Qt::AlignLeft | Qt::AlignVCenter));
  lay_lbl->addSpacing(2);
  layBody->setRowMinimumHeight(row, row_height + (2 * LAYBODY_MARGIN)); 
  layBody->addLayout(lay_lbl, row, 0, (Qt::AlignLeft | Qt::AlignVCenter));
  layBody->addLayout(lay_dat, row, 1);
#endif  
  
//   if(!first_tab_foc) {
//     if(data->focusPolicy() & Qt::TabFocus) {
// //       cerr << "setting first tab focus to row: " << row << " of type: " << data->metaObject()->className() << endl;
//       first_tab_foc = data;
//     }
//   }

  label->show(); // needed for rebuilds, to make the widget show  
  data->show(); // needed for rebuilds, to make the widget show
  
  return row;
}

int taiDataHost::AddData(int row, QWidget* data, bool fill_hor)
{
  if (row < 0)
    row = layBody->rowCount();
  
  
//DATA
  // note1: margins not automatically baked in to max height
  // note2: if guy goes invisible, we'll set its row height to 0 in GetImage
  QHBoxLayout* hbl = new QHBoxLayout();
  hbl->setMargin(0);
  hbl->addWidget(data, 0, Qt::AlignVCenter);
  if (!fill_hor) hbl->addStretch();

// add label/body and show
#if ((QT_VERSION >= 0x040400) && defined(TA_USE_QFORMLAYOUT))
  layBody->addRow(hbl);
#else
  layBody->setRowMinimumHeight(row, row_height + (2 * LAYBODY_MARGIN)); 
  layBody->addLayout(hbl, row, 0, 1, 2); // col 0, span 1 row, span 2 cols
#endif  

//   if(!first_tab_foc) {
//     if(data->focusPolicy() & Qt::TabFocus) {
//       first_tab_foc = data;
//     }
//   }
  
  data->show(); // needed for rebuilds, to make the widget show
  
  return row;
}
void taiDataHost::AddMultiRowName(iEditGrid* multi_body, int row, const String& name, const String& desc) {
  SetMultiSize(row + 1, 0); //0 gets set to multi_col
  QLabel* label = new QLabel(name, (QWidget*)NULL);
  label->setFont(taiM->nameFont(ctrl_size));
  label->setFixedHeight(taiM->label_height(ctrl_size));
  SET_PALETTE_BACKGROUND_COLOR(label, colorOfRow(row));
  if (!desc.empty()) {
    label->setToolTip(desc);
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
    label->setToolTip(desc);
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

void taiDataHost::Constr_Box() {
  //note: see also gpiMultiEditDialog::Constr_Box, if changes made to this implementation
  //note: see ClearBody for guards against deleting the structural widgets when clearing
  QWidget* scr_par = (splBody == NULL) ? widget() : splBody;
  scrBody = new iScrollArea(scr_par);
  SET_PALETTE_BACKGROUND_COLOR(scrBody->viewport(), bg_color_dark);
  scrBody->setWidgetResizable(true); 
  body = new iStripeWidget();
  body_vlay = new QVBoxLayout(body);
  body_vlay->setMargin(0);
  
  scrBody->setWidget(body);
  SET_PALETTE_BACKGROUND_COLOR(body, bg_color);
  ((iStripeWidget*)body)->setHiLightColor(bg_color_dark);
  ((iStripeWidget*)body)->setStripeHeight(row_height + (2 * LAYBODY_MARGIN));
  //TODO: if adding spacing, need to include LAYBODY_SPACING;
  if (splBody == NULL) {
    vblDialog->addWidget(scrBody, 1); // gets all the space
  }
  //note: the layout is added in Constr_Body, because it gets deleted when we change the 'show'
}

void taiDataHost::Constr_Body_impl() {
  first_tab_foc = NULL;		// reset
#if ((QT_VERSION >= 0x040400) && defined(TA_USE_QFORMLAYOUT))
  layBody = new iFormLayout();
  layBody->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
  layBody->setLabelAlignment(Qt::AlignLeft);
  layBody->setRowWrapPolicy(iFormLayout::DontWrapRows);
  layBody->setHorizontalSpacing(2 * LAYBODY_MARGIN);
  layBody->setVerticalSpacing(2 * LAYBODY_MARGIN);
  layBody->setContentsMargins(LAYBODY_MARGIN, 0, LAYBODY_MARGIN, 0);
  layBody->setFieldGrowthPolicy(iFormLayout::AllNonFixedFieldsGrow); // TBD
#else
  layBody = new QGridLayout();
#if QT_VERSION >= 0x040300
  layBody->setHorizontalSpacing(LAYBODY_SPACING);
  layBody->setVerticalSpacing(0);
  layBody->setContentsMargins(LAYBODY_MARGIN, 0, LAYBODY_MARGIN, 0);
#else
  layBody->setSpacing(LAYBODY_SPACING);
  layBody->setMargin(LAYBODY_MARGIN);
#endif
  layBody->setColumnStretch(1,1);
#endif // 4.4 vs. <4.4
  body_vlay->addLayout(layBody);
  body_vlay->addStretch(1);
}

void taiDataHost::ClearBody_impl() {
  if(body) {
    delete body->layout();	// nuke our vboxlayout guy
    taiMisc::DeleteWidgetsLater(body);
    body_vlay = new QVBoxLayout(body);
    body_vlay->setMargin(0);
  }
}

void taiDataHost::Constr_Final() {
  inherited::Constr_Final();
  // we put all the stretch factor setting here, so it is easy to make code changes if necessary
  if (splBody) vblDialog->setStretchFactor(splBody, 1);
  else         vblDialog->setStretchFactor(scrBody, 1);
}


//////////////////////////////////
// 	MembSet_List		//
//////////////////////////////////

MembSet_List::~MembSet_List() {
  Reset();
}

bool MembSet_List::GetFlatDataItem(int idx, MemberDef** mbr, taiData** dat) {
  for (int i = 0; i < size; ++i) {
    MembSet* ms = FastEl(i);
    int msd_size = ms->data_el.size; 
    if (idx >= msd_size) {
      idx -= msd_size;
      continue;
    }
    if (idx < msd_size) {
      if (mbr) *mbr = ms->memb_el.SafeEl(idx);
      if (dat) *dat = ms->data_el.SafeEl(idx); // supposed to be 1:1, but safer
      return true;
    }
    break; // out of range
  }
  return false;
}

int MembSet_List::GetFlatDataIndex(taiData* dat) {
  if (!dat) return -1;
  int rval = 0;
  for (int i = 0; i < size; ++i) {
    MembSet* ms = FastEl(i);
    int ti_set = ms->data_el.FindEl(dat);
    if (ti_set >= 0) {
      return (rval + ti_set);
    } else {
      rval += ms->data_el.size;
    }
  }
  return -1;
}

int MembSet_List::GetFlatDataIndex(MemberDef* mbr, taBase* base) {
  if (!mbr || !base) return -1;
  int rval = 0;
  for (int i = 0; i < size; ++i) {
    MembSet* ms = FastEl(i);
    for (int j = 0; j < ms->data_el.size; ++j, ++rval) {
      if (mbr != ms->memb_el.PosSafeEl(j)) continue;
      if (ms->data_el.FastEl(j)->Base() == base) return rval;
    }
  }
  return -1;
}

int MembSet_List::GetDataSize() const {
  int rval = 0;
  for (int i = 0; i < size; ++i) {
    MembSet* ms = FastEl(i);
    rval += ms->data_el.size;
  }
  return rval;
}

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
  root = base;
  // note: subclass might add more membs, and might set def_size to use them
  membs.SetMinSize(MS_CNT); 
  membs.def_size = MS_CNT;
  show_set(MS_NORM) = true;
  
  for (int j = MS_EXPT; j <= MS_HIDD; ++j) {
    MembSet* ms = membs.SafeEl(j);
    if (!ms) break; // shouldn't happen
    ms->modal = true;
    switch (j) {
    case MS_EXPT:
      ms->text = "Expert Items";
      ms->desc = "show member items that are usually only needed for advanced purposes";
      break;
    case MS_HIDD:
      ms->text = "Hidden Items";
      ms->desc = "show member items that are rarely needed by most users";
      break;
    default: continue; // shouldn't happen!
    }
  }
  
  
  inline_mode = false;
  no_meth_menu = false;
  bgrp = new QButtonGroup(this);
  bgrp->setExclusive(false);
  connect(bgrp, SIGNAL(buttonClicked(int)),
    this, SLOT(bgrp_buttonClicked(int)) );
  menu = NULL;
  InitGuiFields(false);
  ta_menu_buttons.own_items = true; // so they get destroyed
  //note: don't register for notification until constr starts
}

taiEditDataHost::~taiEditDataHost() {
  membs.Reset(); membs.def_size = 0;
  meth_el.Reset();
  taiMisc::active_edits.RemoveEl(this);
  taiMisc::css_active_edits.RemoveEl(this);
  // remove data client -- harmless if already done in Cancel
  taBase* rbase = Base();
  if  (rbase) {
    rbase->RemoveDataClient(this);
    root = NULL;
  }
  bgrp = NULL;
  if (menu) {
    delete menu;
    menu = NULL;
  }
}

// note: called non-virtually in our ctor, and virtually in WidgetDeleting
void taiEditDataHost::InitGuiFields(bool virt) { 
  if (virt) inherited::InitGuiFields();
  cur_menu = NULL;
  cur_menu_but = NULL;
  panel = NULL;
}


void taiEditDataHost::AddMethButton(taiMethodData* mth_rep, const String& label) {
  QWidget* but = mth_rep->GetButtonRep();
  DoAddMethButton(but);
  if(label.nonempty() && but->inherits("QAbstractButton")) {
    ((QAbstractButton*)but)->setText(label);
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
  taBase* rbase = Base();
  if  (rbase) {
    rbase->RemoveDataClient(this);
  }
  if (isPanel()) {
    if (panel != NULL)
      panel->ClosePanel();
  } else if (isControl()) {
    //TODO: need to verify what to do!
    ClearBody(false); // no event loop
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
  if (!typ) return; // class browser or such
  MemberSpace& ms = typ->members;
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.FastEl(i);
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
  Constr_Body_impl();
  if (inline_mode) {
    dat_cnt = 0;
    Constr_Inline();
  } else {
    Constr_Data_Labels();
  }
}

void taiEditDataHost::Constr_Data_Labels() {
  int idx = 0; // basically a row counter
  dat_cnt = 0; // NOT advanced for the section rows
  // Normal members
  if (MS_NORM >= membs.def_size) return; // don't do those
  if (show_set(MS_NORM) && (memb_el(MS_NORM).size > 0)) {
//    Constr_Data_impl(idx, &memb_el(MS_NORM), &data_el(MS_NORM));
    Constr_Data_Labels_impl(idx, &memb_el(MS_NORM), &data_el(MS_NORM));
  }
  for (int j = MS_EXPT; j <= MS_HIDD; ++j) {
    if (j >= membs.def_size) return; // don't do those
    MembSet* ms = membs.SafeEl(j);
    if (!ms || ms->memb_el.size == 0) continue;
    String text = ms->text; 
    String desc = ms->desc;
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
    //AddData(idx, rep, fill_hor);

    // create label
    name = "";
    desc = "";
    GetName(md, name, desc);
    AddNameData(idx, name, desc, rep, mb_dat, md, fill_hor);
    ++idx;
    ++dat_cnt;
  }
}

void taiEditDataHost::Constr_Strings() {
//NOTE: this is INSANE!
  win_str = String(def_title());
  String desc; 
  if (typ != NULL) {
    prompt_str = typ->name;
    taBase* rbase = Base();
    if (rbase) {
      desc = rbase->GetDesc(); // often empty -- use td if so
      if(rbase->GetOwner() != NULL)
        win_str += String(" ") + rbase->GetPathNames();
      if(rbase->GetName() != "") {
        win_str += String(" (") + rbase->GetName() + ")";
        prompt_str = rbase->GetName() + " (" + typ->name + ")";
      }
      else
        win_str += String(" (") + typ->name + ")";
    }
    if (desc.empty()) desc = typ->desc;
  }
  String sapr = def_prompt();
  if (!sapr.empty())
    prompt_str += ": " + sapr;
  else
    if (desc.nonempty()) prompt_str +=  ": " + desc;
}

void taiEditDataHost::Constr_Methods_impl() {
  inherited::Constr_Methods_impl();
  if ((typ == NULL) || no_meth_menu) return; 

  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((md->im == NULL) || (md->name == "Edit")) // don't put edit on edit dialogs..
      continue;
    if (!md->ShowMethod()) continue;
    taiMethod* im = md->im;
//    taiMethodData* mth_rep = md->im->GetMethodRep(root, this, NULL, frmMethButtons); //buttons are in the frame
    if (im == NULL)
      continue;

    taiMethodData* mth_rep = NULL;
    if (md->HasOption("MENU_BUTTON")) {
      SetCurMenuButton(md);
      mth_rep = im->GetMenuMethodRep(root, this, NULL, NULL/*frmMethButtons*/);
      mth_rep->AddToMenu(cur_menu_but);
      meth_el.Add(mth_rep);
    }
    if (md->HasOption("MENU")) {
      SetCurMenu(md);
      mth_rep = im->GetMenuMethodRep(root, this, NULL, NULL/*frmMethButtons*/);
      mth_rep->AddToMenu(cur_menu);
      meth_el.Add(mth_rep);
    }
    if (md->HasOption("BUTTON")) {
      mth_rep = im->GetButtonMethodRep(root, this, NULL, frmMethButtons);
      AddMethButton(mth_rep);
      meth_el.Add(mth_rep);
    }
/*obs    // add to menu if a menu item
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
    }*/
  }
}

void taiEditDataHost::Constr_RegNotifies() {
  taBase* rbase = Base();
  if (rbase) {
    rbase->AddDataClient(this);
  }
}
//void taiEditDataHost::Constr_ShowMenu() {
// moved to be by the setShow etc. calls, for clarity

void taiEditDataHost::Constr_Final() {
  inherited::Constr_Final();
  if(body)
    body->installEventFilter(this); // hopefully everyone below body will get it too!
}

void taiEditDataHost::DoAddMethButton(QWidget* but) {
  show_meth_buttons = true;
  // we use "medium" size for buttons
  but->setFont(taiM->buttonFont(taiMisc::fonMedium));
  but->setFixedHeight(taiM->button_height(taiMisc::sizMedium));
  if (but->parent() != frmMethButtons) {
     but->setParent(frmMethButtons);
  }
  layMethButtons->addWidget(but);
  but->show(); // needed when rebuilding
}

void taiEditDataHost::DoRaise_Panel() {
  //TODO
}

void taiEditDataHost::DoSelectForEdit(QAction* act){
//note: this routine is duplicated in the ProgEditor
  taProject* proj = dynamic_cast<taProject*>(((taBase*)root)->GetThisOrOwner(&TA_taProject));
  if (!proj) return;
  
  int param = act->data().toInt();
  SelectEdit* se = proj->edits.Leaf(param);
 
  if (!sel_item_base) return; // shouldn't happen!
  taBase* rbase = sel_item_base;
  MemberDef* md = sel_item_mbr;
  if (!md || !se || !rbase) return; //shouldn't happen...
  
  //NOTE: this handler adds if not on, or removes if already on
  int idx;
  if ((idx = se->FindMbrBase(rbase, md)) >= 0)
    se->RemoveField(idx);
  else {
    se->SelectMember(rbase, md);
  }
}

MemberDef* taiEditDataHost::GetMemberPropsForSelect(int sel_idx, taBase** base,
    String& lbl, String& desc)
{
  MemberDef* md = NULL;
  if (!(membs.GetFlatDataItem(sel_idx, &md) && md))
    return NULL;
  taBase* rbase = Base();
  if (rbase) {
    if (base) *base = rbase;
    String tlbl = rbase->GetName().elidedTo(16);
    lbl = tlbl;
  }
  return md;
}

void taiEditDataHost::DoConstr_Dialog(iHostDialog*& dlg) {
  inherited::DoConstr_Dialog(dlg);
  if(!modal) {
#ifdef TA_OS_MAC
    // unfortunately, staysontop prevents any other dialog from opening..
    dlg->setWindowFlags(Qt::WindowMinimizeButtonHint);
#else
    dlg->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint);
#endif
  }
}

int taiEditDataHost::Edit(bool modal_, int min_width, int min_height) {
  if (!modal_)
    taiMisc::active_edits.Add(this); // add to the list of active edit dialogs
  return inherited::Edit(modal_, min_width, min_height);
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

void taiDataHostBase::ConstrEditControl() {
  Constr("", "", HT_CONTROL);
//TEMP
//TODO: need to deal with the now wrongly based taiEDH stuff in taiMisc
  taiEditDataHost* edh = dynamic_cast<taiEditDataHost*>(this);
  if (edh) taiMisc::active_edits.Add(edh); // add to the list of active edit dialogs
  state = ACTIVE;
}

void taiEditDataHost::FillLabelContextMenu(QMenu* menu, int& last_id) {
  inherited::FillLabelContextMenu(menu, last_id);
  if (sel_edit_mbrs) { 
    FillLabelContextMenu_SelEdit(menu, last_id);
  }
}

void taiEditDataHost::FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id)
{
  DoFillLabelContextMenu_SelEdit(menu, last_id, sel_item_base, sel_item_mbr, body,
  this, SLOT(DoSelectForEdit(QAction*)));
}

void taiEditDataHost::GetButtonImage(bool force) {
  if(!typ || !mwidget || !frmMethButtons)  return;
  if(!force && !frmMethButtons->isVisible()) {
//     cerr << "buttons not visible on: " << typ->name << endl;
    return;
  }

//   cerr << "updating buttons on: " << typ->name << endl;

  for (int i = 0; i < meth_el.size; ++i) {
    taiMethodData* mth_rep = (taiMethodData*)meth_el.SafeEl(i);
    if ( !(mth_rep->hasButtonRep())) //note: construction forced creation of all buttons
      continue;

    mth_rep->UpdateButtonRep();
  }
}

void taiEditDataHost::GetImage(bool force) {
//   cerr << "GetImage start on: " << typ->name << endl;
  if ((host_type != HT_CONTROL) || (frmMethButtons != NULL))
    GetButtonImage(force); // does its own visible check
  if (!mwidget) return; // huh?
  //note: we could be invisible, so we only do what is visible
  if (!force && !mwidget->isVisible()) return;
  if ((typ == NULL) || (root == NULL)) return;
  if (state >= ACCEPTED ) return;
//   cerr << "GetImage do on: " << typ->name << endl;
//   taMisc::FlushConsole();
  ++updating;
  StartEndLayout(true);
  GetImage_PromptTitle();
  if (state > DEFERRED1) {
    GetImage_Membs();
  }
  Unchanged();
  StartEndLayout(false);
  --updating;
}

void taiEditDataHost::GetImage_Membs() {
  cur_row = 0;
  if (inline_mode) {
    GetImageInline_impl(root);
  } else {
    GetImage_Membs_def();
  }

  // search through children to find first tab focus widget
  // skip over flags
  first_tab_foc = NULL;
  QList<QWidget*> list = qFindChildren<QWidget*>(body);
  for (int i=0; i<list.size(); ++i) {
    QWidget* rep = list.at(i);
//     cerr << i << "\t" << rep->metaObject()->className() << endl;
    if(// rep->isVisible() &&
       rep->isEnabled() &&
       (rep->focusPolicy() & Qt::TabFocus) &&
       !rep->inherits("QCheckBox")) {
      if(rep->inherits("QLineEdit")) {
	QLineEdit* qle = (QLineEdit*)rep;
	if(qle->isReadOnly()) continue;
      }
      first_tab_foc = rep;
//       cerr << "focused on: " << i << "\t" << rep->metaObject()->className() << endl;
      break;
    }
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
      GetImage_impl(&memb_el(i), data_el(i), root);
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
    else {
      if (typ && typ->InheritsFrom(&TA_taBase))
        mb_dat->SetBase((taBase*)base); // used for things like Seledit context menu
      md->im->GetImage(mb_dat, base); // need to do this first, to affect visible
#if ((QT_VERSION >= 0x040400) && defined(TA_USE_QFORMLAYOUT))
      // note: visibles are cached, so nothing happens if it hasn't changed
      layBody->setVisible(cur_row, mb_dat->visible());
#else      
      if (mb_dat->visible()) {
        layBody->setRowMinimumHeight(cur_row, row_height + (2 * LAYBODY_MARGIN)); 
      } else {
        layBody->setRowMinimumHeight(cur_row, 0); 
      }
#endif
      ++cur_row;
    }
  }
}

void taiEditDataHost::GetValue() {
  if ((typ == NULL) || (root == NULL)) return;
  if (state >= ACCEPTED ) return;
  if(!mwidget->isVisible()) {
    taMisc::DebugInfo("taiEditDataHost::GetValue attempt to GetValue with invisible widget!");
  }
  if (state > DEFERRED1) {
    GetValue_Membs();
  }
  GetButtonImage();
  Unchanged();
}

void taiEditDataHost::GetValue_Membs() {
  taBase* rbase = Base();
  if(rbase && root) {
    taProject* proj = (taProject*)((taBase*)root)->GetOwner(&TA_taProject);
    if(proj) {
      proj->undo_mgr.SaveUndo(rbase, "Edit", rbase);
    }
  }

  if (inline_mode) {
    GetValueInline_impl(root);
  } else {
    GetValue_Membs_def();
  }
  if (rbase) {
    rbase->UpdateAfterEdit();	// hook to update the contents after an edit..
    // for gui updates, always make sure that the names are unique within the list
    rbase->MakeNameUnique();
    taiMisc::Update(rbase);
  }
}

void taiEditDataHost::GetValue_Membs_def() {
  for (int i = 0; i < membs.def_size; ++i) {
    if (show_set(i) && (data_el(i).size > 0))
      GetValue_impl(&memb_el(i), data_el(i), root);
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
  // called by root on closing, dialog on closing, DCR_RESOLVE_NOW op, etc. etc.
  if (HasChanged()) {
    if(!mwidget->isVisible()) {
      taMisc::DebugInfo("taiEditDataHost::ResolveChanges attempt to get value with invisible widget!");
    }
    GetValue();
  }
}
 
bool taiEditDataHost::ShowMember(MemberDef* md) const {
  return (md->ShowMember(show(), TypeItem::SC_EDIT) && (md->im != NULL));
}

void taiEditDataHost::SetCurMenu(MethodDef* md) {
  // note: men_nm will be blank if implicit (i.e. last one)
  // if no explicit name, and no menu yet, we use "Actions"
  String men_nm = md->OptionAfter("MENU_ON_");
  SetCurMenu_Name(men_nm);
}

void taiEditDataHost::SetCurMenu_Name(String men_nm) {
  if (!menu) {
    // we can't use QMainMenu on Mac, and QMenu doesn't work for some
    // reason (doesn't become visible, no matter what); but a toolbar works
    // we don't use these on all platforms for uniformity because they SUCK!
#ifdef TA_OS_MAC
    menu = new taiToolBar(widget(), taiMisc::fonSmall,NULL); 
    vblDialog->insertWidget(0, menu->GetRep()); //note: no spacing needed after
    vblDialog->insertSpacing(1, 2);
#else
    menu = new taiMenuBar(taiMisc::fonSmall,
      NULL, this, NULL, widget());
    vblDialog->setMenuBar(menu->GetRep());
#endif
  }
  if (men_nm.nonempty()) {
    cur_menu = ta_menus.FindName(men_nm);
    if (cur_menu != NULL)  return;
  }
  if (cur_menu != NULL) return;

  if (men_nm.empty())
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
  DoAddMethButton(cur_menu_but->GetRep()); // rep is the button for buttonmenu
  ta_menu_buttons.Add(cur_menu_but);
}

iMainWindowViewer* taiEditDataHost::viewerWindow() const {
  iMainWindowViewer* dv = NULL; 
  if (panel) dv = panel->viewerWindow();
  return dv;
}

#include "ta_program_qt.h"

bool taiEditDataHost::eventFilter(QObject* obj, QEvent* event) {
  if(event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }

  QKeyEvent* e = static_cast<QKeyEvent *>(event);

  QCoreApplication* app = QCoreApplication::instance();
  iMainWindowViewer* mvw = viewerWindow();
  if(mvw) {
    mvw->FocusIsMiddlePanel();
    if(mvw->KeyEventFilterWindowNav(obj, e))
      return true;
  }

  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed && ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))) {
    if(modal)
      Ok();
    else
      Apply();
    if(mvw)
      mvw->FocusCurTreeView(); // return focus back to current browser
    return true;
  }
  // for edit dialogs -- arrows = tabs..
  if(ctrl_pressed) {
    switch(e->key()) {
    case Qt::Key_N:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
      return true;		// we absorb this event
    case Qt::Key_P:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier));
      return true;		// we absorb this event
    case Qt::Key_V:
      if(taMisc::emacs_mode) {
	for(int i=0;i<5;i++)	// page up = 5
	  app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
	return true;		// we absorb this event
      }
      return false;
    case Qt::Key_Down:
      for(int i=0;i<5;i++)
	app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
      return true;		// we absorb this event
    case Qt::Key_U:
    case Qt::Key_Up:
      for(int i=0;i<5;i++)
	app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier));
      return true;		// we absorb this event
    }
  }
  else {
    switch (e->key()) {
    case Qt::Key_Down:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
      return true;		// we absorb this event
    case Qt::Key_Up:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier));
      return true;		// we absorb this event
    case Qt::Key_PageDown:
      for(int i=0;i<5;i++)
	app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
      return true;		// we absorb this event
    case Qt::Key_PageUp:
      for(int i=0;i<5;i++)
	app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier));
      return true;		// we absorb this event
    }
  }
  if(e->key() == Qt::Key_Escape) {
    if(modal)
      Cancel();
    else
      Revert();			// do it!
    if(mvw)
      mvw->FocusCurTreeView(); // return focus back to current browser
    return true;
  }
  return QObject::eventFilter(obj, event);
}

//////////////////////////////////////////////////
//		StringDataHost
//////////////////////////////////////////////////

taiStringDataHost::taiStringDataHost(MemberDef* mbr_, void* base_, TypeDef* typ_,
     bool read_only_, bool modal_, QObject* parent, bool line_nos_,
				     bool rich_text_)
:inherited(typ_ ,read_only_, modal_, parent)
{
  root = base_;
  mbr = mbr_;
  edit = NULL;
  btnPrint = NULL;
  line_nos = line_nos_;
  rich_text = rich_text_;
}

taiStringDataHost::~taiStringDataHost() {
}

void taiStringDataHost::Constr(const char* prompt, const char* win_title) {
  inherited::Constr(prompt, win_title);
}

void taiStringDataHost::Constr_Box() {
  if(line_nos) {
    NumberedTextView* ntv = new NumberedTextView(widget());
    edit = ntv->textEdit();
    vblDialog->addWidget(ntv, 1);
  }
  else {
    edit = new iTextEdit(widget());
    vblDialog->addWidget(edit, 1);
  }
  edit->installEventFilter(this); // hopefully everyone below body will get it too!
}

void taiStringDataHost::Constr_RegNotifies() {
  taBase* rbase = Base_(); // cache
  if (rbase) {
    rbase->AddDataClient(this);
  }
}

void taiStringDataHost::Constr_Strings() {
//NO  inherited::Constr_Strings(prompt_str_, win_str_); // for if non-empty
  taBase* rbase = Base_(); // cache
  if (rbase && mbr) {
    win_str = "Editing " + rbase->GetPathNames() + ":" + mbr->GetLabel();
  }
  if (mbr) {
    prompt_str = mbr->GetLabel() + ": " + mbr->desc;
  }
  
}

void taiStringDataHost::Constr_Buttons() {
  inherited::Constr_Buttons();
  // printing is now declared silly..
//   btnPrint = new QPushButton("&Print", widget());
//   layButtons->addSpacing(16);
//   layButtons->addWidget(btnPrint, 0, (Qt::AlignVCenter));
//   connect(btnPrint, SIGNAL(clicked()), this, SLOT(btnPrint_clicked()) );
}


void taiStringDataHost::btnPrint_clicked() {
  QPrinter pr;
  QPrintDialog pd(&pr, widget());
  if (pd.exec() != iDialog::Accepted) return;
  // print ...
  edit->document()->print(&pr);
}

void taiStringDataHost::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
}

void taiStringDataHost::DoConstr_Dialog(iHostDialog*& dlg) {
  inherited::DoConstr_Dialog(dlg);
  dlg->resize( taiM->dialogSize(taiMisc::dlgBig | taiMisc::dlgVer) );
}


void taiStringDataHost::GetImage() {
  const String val = mbr->type->GetValStr(mbr->GetOff(root), root, mbr);
  if(rich_text)
    edit->setHtml(val);
  else
    edit->setPlainText(val);
}

void taiStringDataHost::GetValue() {
  String val = edit->toPlainText();
  mbr->type->SetValStr(val, mbr->GetOff(root), root, mbr);
}

void taiStringDataHost::ResolveChanges(CancelOp& cancel_op, bool* discarded) {
  // called by root on closing, dialog on closing, etc. etc.
  if (modified) {
    GetValue();
  }
}

void taiStringDataHost::Ok_impl() { //note: only used for Dialogs
  inherited::Ok_impl();
  //  if (modified) {
    GetValue();
    Unchanged();
    //  }
}

void taiStringDataHost::SelectLines(int st_line, int end_line) {
  if(!edit) return;
  QTextCursor tc = edit->textCursor();
  tc.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
  if(st_line > 1) {
    tc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, st_line-1);
  }
  if(end_line >= st_line) {
    tc.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, (end_line-st_line)+1);
  }
  edit->setTextCursor(tc);
}

bool taiStringDataHost::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* e = static_cast<QKeyEvent *>(event);
    bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
    if(ctrl_pressed && ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))) {
      Ok();			// do it!
      return true;
    }
    if(e->key() == Qt::Key_Escape) {
      Cancel();			// do it!
      return true;
    }
  }
  return QObject::eventFilter(obj, event);
}


//////////////////////////////////////////////////
//  taiWizardDataHost
//////////////////////////////////////////////////

taiWizardDataHost::taiWizardDataHost(taWizard* base_, TypeDef* typ_,
	     bool read_only_, bool modal_, QObject* parent)
:inherited(typ_ ,read_only_, modal_, parent)
{
  root = base_;
  tabs = NULL;
}

taiWizardDataHost::~taiWizardDataHost() {
}

/*void taiWizardDataHost::Constr(const char* prompt, const char* win_title) {
  inherited::Constr(prompt, win_title);
}*/

void taiWizardDataHost::Constr_Box() {
  tabs = new QTabWidget(widget());
  vblDialog->addWidget(tabs, 1);
}

void taiWizardDataHost::Constr_RegNotifies() {
/*TODO  taBase* rbase = Base_(); // cache
  if (rbase) {
    rbase->AddDataClient(this);
  }*/
}


void taiWizardDataHost::Constr_Buttons() {
  inherited::Constr_Buttons();
/*TODO  btnPrint = new QPushButton("&Print", widget());
  layButtons->addSpacing(16);
  layButtons->addWidget(btnPrint, 0, (Qt::AlignVCenter));
  connect(btnPrint, SIGNAL(clicked()), this, SLOT(btnPrint_clicked()) );*/
}

void taiWizardDataHost::Constr_Data_Labels()
{
  // assert all the tabs -- note that the number can never change
  if (tabs->count() == 0) {
    for (int i = 0; i < membs.size; ++i) {
      iStripeWidget* tab = new iStripeWidget();
      tabs->addTab(tab, page_names.SafeEl(i));
      SET_PALETTE_BACKGROUND_COLOR(tab, bg_color);
      tab->setHiLightColor(bg_color_dark);
      tab->setStripeHeight(row_height + (2 * LAYBODY_MARGIN));
    }
  }
}

void taiWizardDataHost::Constr_Data_Labels_impl(int& idx, Member_List* ms,
     taiDataList* dl)
{
}

void taiWizardDataHost::Constr_Strings() {
/*TODO*/
}

void taiWizardDataHost::Enum_Members() {
  if (!typ) return; // class browser or such
  page_names.Reset();
  membs.Reset();
  
  MemberSpace& ms = typ->members;
  int page_no = 0; // always contains the most recent explicitly marked page
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.FastEl(i);
    if (md->im == NULL) continue; // this puppy won't show nohow!set_grp
    // we skip the normal taNBase members
    if ((md->name == "name") || (md->name == "auto_open")) 
      continue;
    
    // we only show Normal guys in Wizards, sorry charlie!
    if (!md->ShowMember(~taMisc::IS_NORMAL, TypeItem::SC_EDIT, taMisc::IS_NORMAL)) {
      continue;
    } 
    
    // parse page directive -- if none, values are 0, ""
    String pd = md->OptionAfter("PAGE_");
    String page_name; // we only set if included
    if (pd.nonempty()) {
      if (pd.contains("_")) {
        page_name = pd.after("_"); // "" if empty
        pd = pd.before("_");
      }
      page_no = pd.toInt();
    }
    // add to lists if needed -- both have same counts
    if (page_no >= page_names.size) {
      page_names.SetSize(page_no + 1);
      membs.SetMinSize(page_no + 1);
    }
    if (page_name.nonempty()) {
      page_names[page_no] = page_name;
    }
    memb_el(page_no).Add(md);
  }
}

/*void taiWizardDataHost::btnPrint_clicked() {
  QPrinter pr;
  QPrintDialog pd(&pr, widget());
  if (pd.exec() != iDialog::Accepted) return;
  // print ...
  edit->document()->print(&pr);
}*/

/*void taiWizardDataHost::GetImage() {
}

void taiWizardDataHost::GetValue() {
}*/

void taiWizardDataHost::ResolveChanges(CancelOp& cancel_op, bool* discarded) {
  // called by root on closing, dialog on closing, etc. etc.
  if (modified) {
    GetValue();
  }
}

void taiWizardDataHost::Ok_impl() { //note: only used for Dialogs
  inherited::Ok_impl();
  //  if (modified) {
    GetValue();
    Unchanged();
    //  }
}

//////////////////////////////////////////////////
//  	iPluginEditor
//////////////////////////////////////////////////

iPluginEditor* iPluginEditor::New(const String& dir, const String& file_bse) {
  iPluginEditor* pe = new iPluginEditor();
  pe->dir_path = dir;
  pe->file_base = file_bse;

  pe->LoadFiles();

  iSize sz = taiM->dialogSize(taiMisc::dlgBig);
  pe->resize(sz.width(), (int)(1.2f * (float)sz.height())); // a bit bigger than .6h
  pe->show();

  return pe;
}

// note: we parent to main_win so something will delete it
iPluginEditor::iPluginEditor() 
:inherited(taiMisc::main_window)
{
  init();
}

iPluginEditor::~iPluginEditor() {
}

void iPluginEditor::init() {
  this->setAttribute(Qt::WA_DeleteOnClose, true); // kill on close
  this->setWindowTitle("Plugin Editor");

  main_widg = new QWidget();
  main_vbox = new QVBoxLayout(main_widg);
  main_vbox->setMargin(0);

  tool_box = new QHBoxLayout();
  main_vbox->addLayout(tool_box);

  tool_bar = new QToolBar(main_widg);
  tool_box->addWidget(tool_bar);

  actSave = tool_bar->addAction("Save");
  actSave->setToolTip("Save source code to files");

  actCompile = tool_bar->addAction("Compile");
  actCompile->setShortcut(QKeySequence("Ctrl+M"));
  actCompile->setToolTip("Compile (Make) plugin from current source code -- assumes the build directory exists and has been made previously -- if unsure, use ReBuild first -- does a Save first before compiling");

  actReBuild = tool_bar->addAction("ReBuild");
  actReBuild->setToolTip("ReBuild (Make) plugin from current source code, starting everything from scratch at the beginning, including a make clean -- avoids any out-of-date build stuff messing up the compile -- does a Save first before compiling");

  split = new iSplitter(main_widg);
  main_vbox->addWidget(split);

  hfile_view = new NumberedTextView;
  cfile_view = new NumberedTextView;

  hfile_view->installEventFilter(this);
  cfile_view->installEventFilter(this);

  hfile_view->textEdit()->setLineWrapMode(QTextEdit::WidgetWidth);
  cfile_view->textEdit()->setLineWrapMode(QTextEdit::WidgetWidth);
  hfile_view->textEdit()->setTabStopWidth(40);
  cfile_view->textEdit()->setTabStopWidth(40);

  split->addWidget(hfile_view);
  split->addWidget(cfile_view);

  setCentralWidget(main_widg);

  connect(actSave, SIGNAL(triggered()), this, SLOT(save_clicked()) );
  connect(actCompile, SIGNAL(triggered()), this, SLOT(compile_clicked()) );
  connect(actReBuild, SIGNAL(triggered()), this, SLOT(rebuild_clicked()) );
}

void iPluginEditor::save_clicked() {
  SaveFiles();
}

void iPluginEditor::compile_clicked() {
  SaveFiles();
  Compile();
}

void iPluginEditor::rebuild_clicked() {
  SaveFiles();
  ReBuild();
}

void iPluginEditor::LoadFiles() {
  fstream fsrch;
  String hfnm = dir_path + "/" + file_base + ".h";
  fsrch.open(hfnm, ios::in);
  if(!fsrch.good()) {
    taMisc::Error("Could not open file name for saving", hfnm);
    return;
  }
  String hstr;
  hstr.Load_str(fsrch);
  fsrch.close();
  hfile_view->textEdit()->setPlainText(hstr);

  fstream fsrcc;
  String cfnm = dir_path + "/" + file_base + ".cpp";
  fsrcc.open(cfnm, ios::in);
  if(!fsrcc.good()) {
    taMisc::Error("Could not open file name for saving", cfnm);
    return;
  }
  String cstr;
  cstr.Load_str(fsrcc);
  fsrcc.close();
  cfile_view->textEdit()->setPlainText(cstr);
}

void iPluginEditor::SaveFiles() {
  fstream fsrch;
  String hfnm = dir_path + "/" + file_base + ".h";
  fsrch.open(hfnm, ios::out);
  if(!fsrch.good()) {
    taMisc::Error("Could not open file name for saving", hfnm);
    return;
  }
  String hstr = hfile_view->textEdit()->toPlainText();
  hstr.Save_str(fsrch);
  fsrch.close();

  fstream fsrcc;
  String cfnm = dir_path + "/" + file_base + ".cpp";
  fsrcc.open(cfnm, ios::out);
  if(!fsrcc.good()) {
    taMisc::Error("Could not open file name for saving", cfnm);
    return;
  }
  String cstr = cfile_view->textEdit()->toPlainText();
  cstr.Save_str(fsrcc);
  fsrcc.close();
}

void iPluginEditor::Compile() {
  taPlugins::MakePlugin(dir_path, file_base, false, false); // false = assume user, false = no full rebuild
}

void iPluginEditor::ReBuild() {
  taPlugins::MakePlugin(dir_path, file_base, false, true); // false = assume user, true = full rebuild
}

void iPluginEditor::closeEvent(QCloseEvent* ev) {
  int chs = taMisc::Choice("Closing editor -- Save Files first?", "Save Files", "Discard Changes");
  if(chs == 0)
    SaveFiles();
  inherited::closeEvent(ev);
}

bool iPluginEditor::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }
  QKeyEvent* e = static_cast<QKeyEvent *>(event);
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed) {
    switch(e->key()) {
    case Qt::Key_O:
      if(hfile_view->hasFocus() || hfile_view->textEdit()->hasFocus())
	cfile_view->textEdit()->setFocus();
      else
	hfile_view->textEdit()->setFocus();
      return true;		// we absorb this event
    case Qt::Key_M:
      Compile();
      return true;
    }
  }
  return inherited::eventFilter(obj, event);
}

