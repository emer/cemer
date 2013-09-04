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

#include "iApplicationToolBar.h"
#include <iMainWindowViewer>


#include <taiMisc>

#include <QToolButton>

void iApplicationToolBar::Constr_post() {
  iMainWindowViewer* win = viewerWindow(); //cache
  iToolBar* tb = this;

  int icon_sz = taiM_->label_height(taiMisc::sizMedium);
  tb->setIconSize(QSize(icon_sz, icon_sz));

  // Actions have already been constructed for the viewer window's menus.
  // Now add these actions to the toolbar for convenient access.
  tb->addAction(win->historyBackAction);
  tb->addAction(win->historyForwardAction);

  //TEMP
  if (QToolButton* but = qobject_cast<QToolButton*>(tb->widgetForAction(win->historyBackAction))) {
    but->setArrowType(Qt::LeftArrow);
    but->setText("");
  }
  if (QToolButton* but = qobject_cast<QToolButton*>(tb->widgetForAction(win->historyForwardAction))) {
    but->setArrowType(Qt::RightArrow);
    but->setText("");
  }

  tb->addAction(win->editFindAction);

  tb->addSeparator();
  tb->addAction(win->fileNewAction);
  tb->addAction(win->fileOpenAction);
  tb->addAction(win->fileSaveAction);
  tb->addAction(win->fileSaveAsAction);
  // tb->addAction(win->fileSaveNotesAction);
  tb->addAction(win->fileUpdateChangeLogAction);
  tb->addAction(win->fileCloseAction);
  // tb->addAction(win->filePrintAction);

  tb->addSeparator();
  tb->addAction(win->editUndoAction);
  tb->addAction(win->editRedoAction);

  tb->addSeparator();
  tb->addAction(win->editCutAction);
  tb->addAction(win->editCopyAction);
  tb->addAction(win->editPasteAction);
  tb->addAction(win->editPasteIntoAction);
  tb->addAction(win->editPasteAssignAction);
  tb->addAction(win->editPasteAppendAction);

  tb->addSeparator();
  tb->addAction(win->ctrlStopAction);
  tb->addAction(win->ctrlContAction);

  tb->addSeparator();
  tb->addAction(win->helpHelpAction);
  tb->addAction(win->helpFileBugAction);
}

