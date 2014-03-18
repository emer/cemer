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

#include "taiEditor.h"
#include <iDialogEditor>
#include <iHiLightButton>

#include <taMisc>
#include <taiMisc>

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>


#define LAYBODY_MARGIN  1
#define LAYBODY_SPACING 0

void taiEditor::MakeDarkBgColor(const iColor& bg, iColor& dk_bg) {
  dk_bg.set(taiMisc::ivBrightness_to_Qt_lightdark(bg, taiM->edit_darkbg_brightness));
}


taiEditor::taiEditor(TypeDef* typ_, bool read_only_,
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


taiEditor::~taiEditor() {
  if (dialog != NULL) DoDestr_Dialog(dialog);
}

void taiEditor::InitGuiFields(bool) {
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

void taiEditor::StartEndLayout(bool start) {
  if(!widget()) return;
  if (start) {
    widget()->setUpdatesEnabled(false);
  } else { // end
    widget()->setUpdatesEnabled(true);
  }
}

void taiEditor::Apply() {
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
  taBase* obj = Base_();
  if(!obj || obj->isDestroying()) {     // do not refresh if we just killed ourselves!
    --updating;
    Cancel();
    return;
  }
  if (reshow_on_apply)
    defer_reshow_req = true; // forces rebuild so CONDSHOW guys work
  Refresh(); // GetImage/Unchanged, unless a defer_reshow pending
  --updating;
}

taBase* taiEditor::Base_() const {
  if (typ && typ->IsActualTaBase())
    return (taBase*)root;
  else return NULL;
}

void taiEditor::Revert() {
  GetImage();
  Unchanged();
}

void taiEditor::DoDestr_Dialog(iDialogEditor*& dlg) { // common sub-code for destructing a dialog instance
  if (dlg != NULL) {
    dlg->owner = NULL; // prevent reverse deletion
    if(!taMisc::in_shutdown) {
      dlg->deleteLater();
      dlg->close(); // destructive close
    }
    dlg = NULL;
  }
}

void taiEditor::Cancel() { //note: taiEditorOfClass takes care of cancelling panels
  state = CANCELED;
  Cancel_impl();
}

void taiEditor::Cancel_impl() { //note: taiEditorOfClass takes care of cancelling panels
  if (dialog) {
    dialog->dismiss(false);
  }
  if (prompt) {
    prompt->setText("");
  }
}

void taiEditor::Changed() {
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
void taiEditor::Constr(const char* aprompt, const char* win_title,
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



void taiEditor::ConstrDeferred() {
  if (state != DEFERRED1) {
    taMisc::Error("taiEditorWidgetsMain::ConstrDeferred2: expected host to be in state DEFERRED1");
    return;
  }
  Constr_impl();
  state = CONSTRUCTED;
  GetImage();
}

void taiEditor::Constr_impl() {
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
  QPalette pal = widButtons->palette();
  pal.setColor(QPalette::Background, bg_color);
  widButtons->setPalette(pal); 
  layButtons = new QHBoxLayout(widButtons);
  layButtons->setMargin(0);
//def  layButtons->setMargin(2); // facilitates container
  Constr_Buttons();
  Constr_Final();
  StartEndLayout(false);
//NOTE: do NOT do a processevents -- it causes improperly nested event calls
// in some cases, such as constructing the browser
}

void taiEditor::Constr_Strings() {
  // default behavior just sets actuals to defs,
  // but usually the defs are either replaced, or elaborated
  prompt_str = def_prompt();
  win_str = def_title();
}

void taiEditor::Constr_Widget() {
  if (mwidget != NULL) return;
  mwidget = new QWidget();
  QPalette pal = widget()->palette();
  pal.setColor(QPalette::Background, bg_color);
  widget()->setPalette(pal); 
  widget()->setFont(taiM->dialogFont(ctrl_size));
  vblDialog = new QVBoxLayout(widget()); //marg=2
  vblDialog->setSpacing(0); // need to manage ourself to get nicest look
  vblDialog->setMargin(2);
}

void taiEditor::Constr_Prompt() {
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

void taiEditor::Constr_Buttons() {
  QWidget* par = widButtons;

  if(!isDialog()) {
    help_but = new iHiLightButton("&Help", par);
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
        okbut = new iHiLightButton("&Close", par); //note: ok to reuse C as accelerator, because effect is same as Cancel
      } else {
        okbut = new iHiLightButton("&Ok", par);
      }
      layButtons->addWidget(okbut, 0, (Qt::AlignVCenter));
      connect(okbut, SIGNAL(clicked()),
          this, SLOT(Ok()) );
    }
    if (read_only) {
      canbut = NULL;
    }
    else {
      canbut = new iHiLightButton("&Cancel", par);
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
      apply_but = new iHiLightButton("&Apply", par);
      layButtons->addWidget(apply_but, 0, (Qt::AlignVCenter));
      connect(apply_but, SIGNAL(clicked()),
          this, SLOT(Apply()) );
      revert_but = new iHiLightButton("&Revert", par);
      layButtons->addWidget(revert_but, 0, (Qt::AlignVCenter));
      connect(revert_but, SIGNAL(clicked()),
          this, SLOT(Revert()) );
    }
    Unchanged();
  }
//nuke  layButtons->addSpacing(10); // don't flush hard right
}


void taiEditor::SigLinkDestroying(taSigLink* dl) {
// TENT, TODO: confirm this is right...
//NO!  if (!isConstructed()) return;
  Cancel();
  root = NULL;
}

void taiEditor::SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) {
//inherited class completely implements
}

void taiEditor::DoConstr_Dialog(iDialogEditor*& dlg) {
  // common subcode for creating a dialog -- used by the taiDialogEditor and taiEditDialog cousin classes
  if (dlg) return; // already constructed
  if (modal) // s/b parented to current win
    dlg = new iDialogEditor(this, QApplication::activeWindow());
  else
    dlg = new iDialogEditor(this, NULL, Qt::WindowMinimizeButtonHint);
  // note: X can't seem to handle more than 12-14 windows, so making these top-level is an issue
  // BUT it is also highly unusable to make them owned, since then they obscure parent window
  dlg->setWindowTitle(win_str);
//  dlg->setMinimumWidth(400); //TODO: maybe parameterize; note: would need to set layout FreeResize as well
}

void taiEditor::DoRaise_Dialog() {
  if (!dialog) return;
  if (!modal) {
    dialog->raise();
    dialog->setFocus();
  }
}

int taiEditor::Edit(bool modal_, int min_width, int min_height) {
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
  if (!modal && (GetTypeDef()->InheritsFrom(&TA_taiEditorWidgetsMain))) {
    taiMisc::active_dialogs.AddUnique((taiEditorWidgetsMain*)this); // add to the list of active dialogs
  }
  state = ACTIVE;
  int rval = dialog->post(modal);
  if(modal)
    Cancel();
  return rval;
}

void taiEditor::GetImage_PromptTitle() {
  Constr_Strings(); // in case changed, ex. desc change or name change
  if (prompt) // always made???
    prompt->setText(prompt_str);
  // dialogs only
  if (dialog)
    dialog->setWindowTitle(win_str);
}

void taiEditor::setBgColor(const iColor& new_bg) {
  bg_color = new_bg;
  MakeDarkBgColor(bg_color, bg_color_dark);
}

void taiEditor::Help() {
  taBase* obj = Base_();
  if(!obj) return;
  obj->Help();
}

void taiEditor::Ok() { //note: only used for Dialogs
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

void taiEditor::Ok_impl() {
}

void taiEditor::Refresh() {
  Refresh_impl(defer_reshow_req);
}

void taiEditor::Unchanged() {
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

void taiEditor::WidgetDeleting() {
  InitGuiFields(); // called virtually
  state = ZOMBIE;
}

taiEditor_List taiEditor::async_apply_list;
taiEditor_List taiEditor::async_reshow_list;
taiEditor_List taiEditor::async_reconstr_list;
taiEditor_List taiEditor::async_getimage_list;

bool taiEditor::AsyncWaitProc() {
  static bool in_waitproc = false;

  if(async_apply_list.size == 0 && async_reshow_list.size == 0 &&
     async_getimage_list.size == 0 && async_reconstr_list.size == 0) return false;

  if(in_waitproc) return false;
  in_waitproc = true;

  // order is important here: don't want to have one thing trigger another right away..
  bool did_some = false;
  for(int i=0;i<async_getimage_list.size;i++) {
    taiEditor* dhb = async_getimage_list.SafeEl(i);
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
    taiEditor* dhb = async_reconstr_list.SafeEl(i);
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
    taiEditor* dhb = async_reshow_list.SafeEl(i);
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
    taiEditor* dhb = async_apply_list.SafeEl(i);
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

void taiEditor::Apply_Async() {
//    Apply();       // no reason to actually async this..
  if (apply_req) return; // already waiting
  if (state != ACTIVE) return;
  apply_req = true;
  taMisc::do_wait_proc = true;
  async_apply_list.Link(this);
}

void taiEditor::ReShow_Async(bool forced) {
  if (reshow_req) return; // already waiting
  if ((state & STATE_MASK) >= CANCELED) return;
  reshow_req = true;
  reshow_req_forced = forced;
  taMisc::do_wait_proc = true;
  async_reshow_list.Link(this);
}

void taiEditor::ReConstr_Async() {
  if(reconstr_req) return;
  reconstr_req = true;
  taMisc::do_wait_proc = true;
  async_reconstr_list.Link(this);
}

void taiEditor::GetImage_Async() {
  getimage_req = true;
  taMisc::do_wait_proc = true;
  async_getimage_list.Link(this);
}

void taiEditor::DebugDestroy(QObject* obj) {
}
