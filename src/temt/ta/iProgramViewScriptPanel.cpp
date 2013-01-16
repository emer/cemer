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

#include "iProgramViewScriptPanel.h"
#include <Program>
#include <NumberedTextView>
#include <NumberBar>
#include <iTextEdit>
#include <iDataPanelSet>

#include <DataChangedReason>
#include <tabMisc>

#include <css_machine.h>

#include <QScrollBar>
#include <QToolTip>

iProgramViewScriptPanel::iProgramViewScriptPanel(taiDataLink* dl_)
:inherited(dl_)
{
  Program* prg_ = prog();
  vs = NULL;
  if (prg_) {
    prg_->ViewScriptUpdate();
    vs = new NumberedTextView(NULL, true); // enable icons
    vs->textEdit()->setReadOnly(true);
    vs->textEdit()->setHtml(prg_->view_script);
    setCentralWidget(vs);

    NumberBar* nb = vs->numberBar();
    connect(nb, SIGNAL(lineFlagsUpdated(int, int)), this,
            SLOT(lineFlagsUpdated(int, int)));
    connect(nb, SIGNAL(viewSource(int)), this,
            SLOT(viewSource(int)));
    connect(vs, SIGNAL(mouseHover(const QPoint&, int, const QString&)), this,
            SLOT(mouseHover(const QPoint&, int, const QString&)));
  }
}

iProgramViewScriptPanel::~iProgramViewScriptPanel() {
  // qt should take care of it
}

QWidget* iProgramViewScriptPanel::firstTabFocusWidget() {
  return vs;
}

bool iProgramViewScriptPanel::ignoreDataChanged() const {
  return false; // don't ignore -- we do SmartButComplicatedIgnore(TM)
//  return !isVisible();
}

void iProgramViewScriptPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  if(vs && vs->isVisible())
    UpdatePanel_impl();
//   if (dcr <= DCR_ITEM_UPDATED_ND) {
//     this->m_update_req = true; // so we update next time we show, if hidden
//   }
//   inherited::DataChanged_impl(dcr, op1_, op2_);
}

bool iProgramViewScriptPanel::HasChanged() {
  return false;
}

void iProgramViewScriptPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
}

void iProgramViewScriptPanel::UpdatePanel_impl() {
  if(!vs) return;
  inherited::UpdatePanel_impl(); // clears reg flag and updates tab
  Program* prg_ = prog();
  if(!prg_) return;

  NumberBar* nb = vs->numberBar();
  nb->clearAllLineFlags();
  for(int i=1;i<prg_->script_list.size;i++) {
    ProgLine* pl = prg_->script_list.FastEl(i);
    int pflg = NumberBar::LF_NONE;
    if(pl->HasPLineFlag(ProgLine::BREAKPOINT))
      pflg |= NumberBar::LF_BREAK;
    if(pl->HasPLineFlag(ProgLine::PROG_ERROR))
      pflg |= NumberBar::LF_ERROR;
    if(pl->HasPLineFlag(ProgLine::WARNING))
      pflg |= NumberBar::LF_WARNING;
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

void iProgramViewScriptPanel::lineFlagsUpdated(int lineno, int flags) {
  Program* prg_ = prog();
  if(!prg_) return;
  ProgLine* pl = prg_->script_list.SafeEl(lineno);
  if(!pl) return;

  // this is the only flag that is user-settable
  if(flags & NumberBar::LF_BREAK)
    pl->SetBreakpoint();
  else
    pl->ClearBreakpoint();

  // basic update
  prg_->ViewScriptUpdate();
  int contentsY = vs->textEdit()->verticalScrollBar()->value();
  vs->textEdit()->setHtml(prg_->view_script);
  vs->textEdit()->verticalScrollBar()->setValue(contentsY);
}

void iProgramViewScriptPanel::viewSource(int lineno) {
  Program* prg_ = prog();
  if(!prg_) return;
  ProgLine* pl = prg_->script_list.SafeEl(lineno);
  if(!pl || !pl->prog_el) return;
  tabMisc::DelayedFunCall_gui(pl->prog_el, "BrowserSelectMe");
}

void iProgramViewScriptPanel::mouseHover(const QPoint &pos, int lineno, const QString& word) {
  Program* prg_ = prog();
  if(!prg_) return;
  String rval = prg_->GetProgCodeInfo(lineno, word);
  if(rval.nonempty())
    QToolTip::showText(pos, rval);
}

void iProgramViewScriptPanel::ResolveChanges_impl(CancelOp& cancel_op) {
}

void iProgramViewScriptPanel::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  m_dps->UpdateMethodButtons();
}
