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

#include "taiProgStepButton.h"
#include <taProject>
#include <MethodDef>
#include <taiActions>

#include <taMisc>
#include <taiMisc>

#include <QRadioButton>
#include <QStackedWidget>
#include <QToolBar>
#include <QGridLayout>
#include <QToolButton>


taiProgStepButton::taiProgStepButton(void* bs, MethodDef* md, TypeDef* typ_, IWidgetHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiMethodData(bs, md, typ_, host_, par, gui_parent_, flags_)
{
  is_menu_item = false;
  tool_bar = NULL;
  new_step_n = -1;
  last_step_n = 1;
  step10_val = 10;
  n_step_progs = 0;
  last_step = NULL;             // reset when switching
}

void taiProgStepButton::CallFunList(void* itm) {
  Program* prg = (Program*)base; // definitively this
  if(!prg) return;

  Program* trg = (Program*)itm;

  QPointer<taiProgStepButton> ths = this; // to detect us being deleted
  ApplyBefore();
  // note: this is not a great situation, whereby applying deletes us, but
  // we warn user (and should probably do something, ie a directive that you
  // have to save before)
  if (!ths) {
    taMisc::Error("This menu item or button action apparently cannot be invoked when you have not applied changes. Please try the operation again. (The developers would appreciate hearing about this situation.");
    return;
  }

  taProject* proj = (taProject*)prg->GetOwner(&TA_taProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(prg, "Call Method: " + meth->name, NULL, true);
    // true = force project-level save
  }

  if(trg && trg->owner) {
    if(new_step_n > 0)
      trg->step_n = new_step_n;
    last_step_n = trg->step_n;
  }

  prg->Step_Gui(trg);   // that was simple!

  last_step = trg;
  new_step_n = -1;
}

void taiProgStepButton::Step1(bool on) {
  if(!on) {
    stp1->setChecked(true);     // can't click off!
    return;
  }
  Program* prg = (Program*)base; // definitively this
  if(!prg) return;
  new_step_n = 1;
  stp5->setChecked(false);
  stp10->setChecked(false);
}

void taiProgStepButton::Step5(bool on) {
  if(!on) {
    stp5->setChecked(true);     // can't click off!
    return;
  }
  Program* prg = (Program*)base; // definitively this
  if(!prg) return;
  new_step_n = 5;
  stp1->setChecked(false);
  stp10->setChecked(false);
}


void taiProgStepButton::Step10(bool on) {
  if(!on) {
    stp10->setChecked(true);    // can't click off!
    return;
  }
  Program* prg = (Program*)base; // definitively this
  if(!prg) return;
  new_step_n = step10_val;
  stp1->setChecked(false);
  stp5->setChecked(false);
}


QWidget* taiProgStepButton::GetButtonRep() {
  Program* prg = (Program*)base; // definitively this
  if(!prg) return buttonRep;

  // we are all up-to-date and this is expensive so just bail!!
  if(buttonRep && !prg->sub_progs_updtd && n_step_progs == prg->sub_progs_step.size)
    return buttonRep;

  if(!buttonRep) {
    buttonRep = new QStackedWidget(gui_parent);
    SetRep(buttonRep);
    if(prg->step_prog)
      last_step_n = prg->step_prog->step_n;
  }

  QToolBar* newbar = new QToolBar();
  newbar->setFont(taiM->menuFont(defSize()));
  QLabel* lbl = new QLabel("Step:");
  lbl->setToolTip("Run N Step(s) of a program -- select N (1,5,10) in adjacent buttons, and then click on the name of the program shown after N's to Step that given program (this Step label is not a button and does not do anything)");
  lbl->setFont(taiM->menuFont(defSize()));
  newbar->addWidget(lbl);

  QWidget* intstak = new QWidget();
  QGridLayout* glay = new QGridLayout(intstak);
  glay->setMargin(0); glay->setSpacing(0);

  stp1 = new QRadioButton(intstak);
  stp1->setToolTip("step by single (1) steps");
//   QSize sz = stp1->minimumSizeHint();
  // this size is useless!
//   taMisc::Info("radio sz:", String(sz.width()), String(sz.height()));
  int radio_width = 16;
  int radio_height = 16;
  stp1->setMaximumSize(radio_width, radio_height);
  connect(stp1, SIGNAL(clicked(bool)), this, SLOT(Step1(bool)) );
  glay->addWidget(stp1, 0, 0, Qt::AlignHCenter);

  stp5 = new QRadioButton(intstak);
  stp5->setToolTip("step by 5 steps per step click");
  stp5->setMaximumSize(radio_width, radio_height);
  connect(stp5, SIGNAL(clicked(bool)), this, SLOT(Step5(bool)) );
  glay->addWidget(stp5, 0, 1, Qt::AlignHCenter);

  stp10 = new QRadioButton(intstak);
  stp10->setToolTip("step by 10 steps per step click, or amount shown if different from 10 (if program step_n != {1,5,10}");
  stp10->setMaximumSize(radio_width, radio_height);
  connect(stp10, SIGNAL(clicked(bool)), this, SLOT(Step10(bool)) );
  glay->addWidget(stp10, 0, 2, Qt::AlignHCenter);

  step10_val = 10;
  int stp_n = last_step_n;
  if(new_step_n > 0)
    stp_n = new_step_n;

  if(stp_n == 1)
    stp1->setChecked(true);
  else if(stp_n == 5)
    stp5->setChecked(true);
  else if(stp_n == 10)
    stp10->setChecked(true);
  else {
    step10_val = stp_n;
    stp10->setChecked(true);
  }

  int steps[3] = {1,5,step10_val};

  for(int i=0;i<3;i++) {
    lbl = new QLabel(String(steps[i]));
    lbl->setFont(taiM->menuFont(taiMisc::sizSmall));
    glay->addWidget(lbl, 1, i, Qt::AlignHCenter);
  }

  newbar->addWidget(intstak);

  int but_marg_minus = 10;

  QWidget* stpwidg = new QWidget();
  QHBoxLayout* hbl = new QHBoxLayout(stpwidg);
  hbl->setMargin(0); hbl->setSpacing(0);
  for(int i=0;i<prg->sub_progs_step.size; i++) {
    Program* sp = (Program*)prg->sub_progs_step[i];
    QToolButton* tbut = new QToolButton(stpwidg);
    taiAction* act = new taiAction(taiActions::normal, sp->short_nm);
    act->usr_data = (void*)sp;
    act->connect(taiAction::ptr_act, this, SLOT(CallFunList(void*)));
    act->setToolTip(sp->name);
    act->setFont(taiM->menuFont(defSize()));
    tbut->setDefaultAction(act);
    if(i == prg->sub_progs_step.size-1) {
      tbut->setShortcut(QKeySequence("F10"));
    }
    QSize sz = tbut->minimumSizeHint();
    tbut->setMaximumWidth(sz.width() - but_marg_minus);
    hbl->addWidget(tbut);
  }
  newbar->addWidget(stpwidg);
 n_step_progs = prg->sub_progs_step.size;

  if(tool_bar) {
    ((QStackedWidget*)buttonRep.data())->removeWidget(tool_bar);
  }
  tool_bar = newbar;
  ((QStackedWidget*)buttonRep.data())->addWidget(tool_bar);
//   newbar->show();
  return buttonRep;
}

bool taiProgStepButton::UpdateButtonRep() {
  if(!base || !buttonRep) return false;
  GetButtonRep();
  taiMethodData::UpdateButtonRep();
  return true;
}
