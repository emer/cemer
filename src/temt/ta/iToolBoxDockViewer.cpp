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

#include "iToolBoxDockViewer.h"
#include <ToolBoxDockViewer>
#include <ToolBoxRegistrar_PtrList>
#include <iClipWidgetAction>

#include <taMisc>
#include <taiMisc>

#include <QToolBox>
#include <QToolBar>


IDataViewWidget* ToolBoxDockViewer::ConstrWidget_impl(QWidget* gui_parent) {
    return new iToolBoxDockViewer(this, gui_parent); // usually parented later
}


iToolBoxDockViewer::iToolBoxDockViewer(ToolBoxDockViewer* viewer_, QWidget* parent)
  : inherited(viewer_, parent)
{
  Init();
}

iToolBoxDockViewer::~iToolBoxDockViewer()
{
}

void iToolBoxDockViewer::Init() {
  tbx = new QToolBox();
  //note: if we don't set font, tabs seem to have too big a font
  tbx->setFont(taiM->buttonFont(taiMisc::defFontSize));
  setWidget(tbx);
}

int iToolBoxDockViewer::AssertSection(const String& sec_name) {
  int sec = -1;
  for (int i = 0; i < tbx->count(); ++i) {
    if (tbx->itemText(i) == sec_name) {
      sec = i;
      break;
    }
  }
  if (sec < 0) {
    QToolBar* tb = new QToolBar;
    tb->setOrientation(Qt::Vertical);
    sec = tbx->addItem(tb, sec_name);
  }
  return sec;
}

QWidget* iToolBoxDockViewer::AddClipToolWidget(int sec, iClipWidgetAction* cwa) {
  QToolBar* w = sectionWidget(sec);
  if (!w) return NULL; // user didn't assert
  // don't leak!
  if (cwa->parent() == NULL) {
    cwa->setParent(this);
  }
  w->addAction(cwa);
  connect(cwa, SIGNAL(triggered()), cwa, SLOT(copyToClipboard()) ); // ie to self
  return w->widgetForAction(cwa);
}

void iToolBoxDockViewer::AddSeparator(int sec) {
  QToolBar* w = sectionWidget(sec);
  if (!w) return; // user didn't assert
  w->addSeparator();
}

void iToolBoxDockViewer::Constr_post() {
  ToolBoxRegistrar_PtrList* list = ToolBoxRegistrar::instances();
  for (int i = 0; i < list->size; ++i) {
    ToolBoxProc proc = list->FastEl(i)->proc;
    proc(this);
  }
}

QToolBar* iToolBoxDockViewer::sectionWidget(int sec) {
  QToolBar* rval = qobject_cast<QToolBar*>(tbx->widget(sec));
  return rval;
}

