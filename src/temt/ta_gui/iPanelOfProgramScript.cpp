// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "iPanelOfProgramScript.h"
#include <Program>
#include <iNumberedTextView>
#include <iNumberBar>
#include <iTextEdit>
#include <iPanelSet>

#include <SigLinkSignal>
#include <tabMisc>

#include <css_machine.h>

#include <QScrollBar>
#include <QToolTip>

iPanelOfProgramScript::iPanelOfProgramScript(taiSigLink* dl_)
:inherited(dl_)
{
  Program* prg_ = prog();
  vs = NULL;
  if (prg_) {
    prg_->ViewScriptUpdate();
    vs = new iNumberedTextView(NULL, true); // enable icons
    vs->textEdit()->setReadOnly(true);
    vs->textEdit()->setHtml(prg_->view_script);
    setCentralWidget(vs);

    iNumberBar* nb = vs->numberBar();
    connect(nb, SIGNAL(lineFlagsUpdated(int, int)), this,
            SLOT(lineFlagsUpdated(int, int)));
    connect(nb, SIGNAL(viewSource(int)), this,
            SLOT(viewSource(int)));
    connect(vs, SIGNAL(mouseHover(const QPoint&, int, const QString&)), this,
            SLOT(mouseHover(const QPoint&, int, const QString&)));
  }
}

iPanelOfProgramScript::~iPanelOfProgramScript() {
  // qt should take care of it
}

QWidget* iPanelOfProgramScript::firstTabFocusWidget() {
  return vs;
}

bool iPanelOfProgramScript::ignoreSigEmit() const {
  return false; // don't ignore -- we do SmartButComplicatedIgnore(TM)
//  return !isVisible();
}

void iPanelOfProgramScript::SigEmit_impl(int sls, void* op1_, void* op2_) {
  if(vs && vs->isVisible())
    UpdatePanel_impl();
//   if (sls <= SLS_ITEM_UPDATED_ND) {
//     this->m_update_req = true; // so we update next time we show, if hidden
//   }
//   inherited::SigEmit_impl(sls, op1_, op2_);
}

bool iPanelOfProgramScript::HasChanged() {
  return false;
}

void iPanelOfProgramScript::OnWindowBind_impl(iPanelViewer* itv) {
  inherited::OnWindowBind_impl(itv);
}

void iPanelOfProgramScript::UpdatePanel_impl() {
  if(!vs) return;
  inherited::UpdatePanel_impl(); // clears reg flag and updates tab
  Program* prg_ = prog();
  if(!prg_) return;

  iNumberBar* nb = vs->numberBar();
  nb->clearAllLineFlags();
  for(int i=1;i<prg_->script_list.size;i++) {
    ProgLine* pl = prg_->script_list.FastEl(i);
    int pflg = iNumberBar::LF_NONE;
    if(pl->HasPLineFlag(ProgLine::BREAKPOINT))
      pflg |= iNumberBar::LF_BREAK;
    if(pl->HasPLineFlag(ProgLine::PROG_ERROR))
      pflg |= iNumberBar::LF_ERROR;
    if(pl->HasPLineFlag(ProgLine::WARNING))
      pflg |= iNumberBar::LF_WARNING;
    nb->setLineFlags(i, pflg);
  }

  if(prg_->script) {
    nb->setCurrentLine(prg_->script->CurRunSrcLn());
  }

  prg_->ViewScriptUpdate();
  int contentsY = vs->textEdit()->verticalScrollBar()->value();
  vs->textEdit()->setHtml(prg_->view_script);
  vs->textEdit()->verticalScrollBar()->setValue(contentsY);
}

void iPanelOfProgramScript::lineFlagsUpdated(int lineno, int flags) {
  Program* prg_ = prog();
  if(!prg_) return;
  ProgLine* pl = prg_->script_list.SafeEl(lineno);
  if(!pl) return;

  // this is the only flag that is user-settable
  if(flags & iNumberBar::LF_BREAK)
    pl->SetBreakpoint();
  else
    pl->ClearBreakpoint();

  // basic update
  prg_->ViewScriptUpdate();
  int contentsY = vs->textEdit()->verticalScrollBar()->value();
  vs->textEdit()->setHtml(prg_->view_script);
  vs->textEdit()->verticalScrollBar()->setValue(contentsY);
}

void iPanelOfProgramScript::viewSource(int lineno) {
  Program* prg_ = prog();
  if(!prg_) return;
  ProgLine* pl = prg_->script_list.SafeEl(lineno);
  if(!pl || !pl->prog_el) return;
  tabMisc::DelayedFunCall_gui(pl->prog_el, "BrowserSelectMe");
}

void iPanelOfProgramScript::mouseHover(const QPoint &pos, int lineno, const QString& word) {
  Program* prg_ = prog();
  if(!prg_) return;
  String rval = prg_->GetProgCodeInfo(lineno, word);
  if(rval.nonempty())
    QToolTip::showText(pos, rval);
}

void iPanelOfProgramScript::ResolveChanges_impl(CancelOp& cancel_op) {
}

void iPanelOfProgramScript::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  m_dps->UpdateMethodsEnabled();
}
