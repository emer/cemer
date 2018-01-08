// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "taiWidgetProgStepButton.h"
#include <taProject>
#include <MethodDef>
#include <taiWidgetActions>
#include <Program>
#include <iContextMenuButton>

#include <taMisc>
#include <taiMisc>

#include <QRadioButton>
#include <QStackedWidget>
#include <QToolBar>
#include <QGridLayout>
#include <QToolButton>


static const int std_steps_data[] = {1,2,3,4,5,6,7,8,9,10,15,20,25,30,40,50,60,75,100};
const int* taiWidgetProgStepButton::std_steps = std_steps_data;
const int taiWidgetProgStepButton::n_std_steps = 19;

taiWidgetProgStepButton::taiWidgetProgStepButton(void* bs, MethodDef* md, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_)
: taiWidgetMethod(bs, md, typ_, host_, par, gui_parent_, flags_)
{
  is_menu_item = false;
  tool_bar = NULL;
  n_step_progs = 0;
  step_buts = NULL;
}

void taiWidgetProgStepButton::CallFunList(void* itm) {
  Program* prg = (Program*)base; // definitively this
  if(!prg) return;

  Program* trg = (Program*)itm;

  QPointer<taiWidgetProgStepButton> ths = this; // to detect us being deleted
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

  prg->Step_Gui(trg);   // that was simple!
}

QWidget* taiWidgetProgStepButton::GetButtonRep() {
  Program* prg = (Program*)base; // definitively this
  if(!prg) return buttonRep;

  // we are all up-to-date and this is expensive so just bail!!
  if(buttonRep && !prg->sub_progs_updtd && n_step_progs == prg->sub_progs_step.size) {
    for(int i=0;i<prg->sub_progs_step.size; i++) {
      Program* sp = (Program*)prg->sub_progs_step[i];
      QToolButton* tbut = step_buts[i];
      String stpnm = sp->short_nm + ":" + String(sp->step_n);
      tbut->setText(stpnm);
    }
    return buttonRep;
  }

  if(!buttonRep) {
    buttonRep = new QStackedWidget(gui_parent);
    SetRep(buttonRep);
  }

  if(step_buts)
    delete[] step_buts;
  step_buts = new QToolButton*[prg->sub_progs_step.size];
  
  QToolBar* newbar = new QToolBar();
  newbar->setFont(taiM->menuFont(defSize()));
  QLabel* lbl = new QLabel("Step:");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("Run N Step(s) of a program -- select N with context menu or hold down button (this Step label is not a button and does not do anything)"));
  lbl->setFont(taiM->menuFont(defSize()));
  newbar->addWidget(lbl);

  int but_marg_minus = 12;

  // QWidget* stpwidg = new QWidget();
  // QHBoxLayout* hbl = new QHBoxLayout(stpwidg);
  // hbl->setMargin(0); hbl->setSpacing(0);
  for(int i=0;i<prg->sub_progs_step.size; i++) {
    Program* sp = (Program*)prg->sub_progs_step[i];
    QToolButton* tbut = new iContextMenuButton();
    step_buts[i] = tbut;
    String stpnm = sp->short_nm + ":" + String(sp->step_n);
    iAction* act = new iAction(taiWidgetActions::normal, stpnm);
    act->usr_data = (void*)sp;
    act->connect(iAction::ptr_act, this, SLOT(CallFunList(void*)));
    act->setToolTip(taiMisc::ToolTipPreProcess(sp->name + " : Current step size (hold button to change step size)"));
    act->setFont(taiM->menuFont(defSize()));
    tbut->setDefaultAction(act);
    if(i == prg->sub_progs_step.size-1) {
      tbut->setShortcut(QKeySequence("F10"));
    }
    QSize sz = tbut->minimumSizeHint();
    
#ifdef TA_OS_WIN
   tbut->setMinimumWidth(sz.width() + 20);
#endif
    tbut->setMaximumWidth(sz.width() - but_marg_minus);
    newbar->addSeparator();
    newbar->addWidget(tbut);

    QMenu* step_menu = new QMenu(tbut);
    step_menu->setFont(taiM->menuFont(taiMisc::fonMedium));

    for(int st=0; st<n_std_steps; st++) {
      int multiplex_val = 100 * i + st;
      iAction* step_act = new iAction(multiplex_val, String(std_steps[st]),
                                      QKeySequence());
      step_act->connect(iAction::int_act, this,  SLOT(setStepN(int)));
      step_act->setParent(step_menu);
      step_menu->addAction(step_act);
    }
    tbut->setMenu(step_menu);
    tbut->setPopupMode(QToolButton::DelayedPopup);
  }
  // newbar->addWidget(stpwidg);
  n_step_progs = prg->sub_progs_step.size;

  if(tool_bar) {
    ((QStackedWidget*)buttonRep.data())->removeWidget(tool_bar);
  }
  tool_bar = newbar;
  ((QStackedWidget*)buttonRep.data())->addWidget(tool_bar);
  return buttonRep;
}

void taiWidgetProgStepButton::setStepN(int multiplex_val) {
  Program* prg = (Program*)base; // definitively this
  int prog_idx = multiplex_val / 100;
  int step_idx = multiplex_val % 100;
  Program* sp = (Program*)prg->sub_progs_step[prog_idx];
  int step_n = std_steps[step_idx];
  sp->step_n = step_n;
  GetButtonRep();
}

bool taiWidgetProgStepButton::UpdateEnabled() {
  return UpdateButtonRep(true);        // always update!
}

bool taiWidgetProgStepButton::UpdateButtonRep(bool enab) {
  if(!base || !buttonRep) return false;
  GetButtonRep();
  taiWidgetMethod::UpdateButtonRep(enab);
  return true;
}
