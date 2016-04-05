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


#include <taMisc>
#include <taiMisc>

#include <QToolButton>

void iApplicationToolBar::Constr_post() {
  iMainWindowViewer* win = viewerWindow(); //cache
  iToolBar* tb = this;
  
  tb->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(taMisc::app_toolbar_style));
  
  int icon_sz = taiM_->label_height(taiMisc::sizMedium);
  tb->setIconSize(QSize(icon_sz, icon_sz));

  // Actions have already been constructed for the viewer window's menus.
  // Now add these actions to the toolbar for convenient access.
  tb->addAction(win->historyBackAction);
  win->historyBackAction->setIcon(QIcon(QPixmap(":/images/previous_icon.png")));

  tb->addAction(win->historyForwardAction);
  win->historyForwardAction->setIcon(QIcon(QPixmap(":/images/next_icon.png")));

  tb->addSeparator();
  tb->addAction(win->editFindAction);

  tb->addSeparator();
  tb->addAction(win->fileNewAction);
  tb->addAction(win->fileOpenAction);
  tb->addAction(win->fileCloseAction);
  tb->addAction(win->fileSaveAction);
  tb->addAction(win->fileSaveAsAction);

  tb->addSeparator();
  tb->addAction(win->fileOpenSvnBrowserAction);
  tb->addAction(win->fileSvnCommitAction);
  
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
  tb->addAction(win->ctrlInitAction);
  tb->addAction(win->ctrlStopAction);
  tb->addAction(win->ctrlStepAction);
  tb->addAction(win->ctrlRunAction);
  tb->addAction(win->progStatusAction);
  tb->addAction(win->viewConsoleFrontAction);

  tb->addSeparator();
  tb->addAction(win->helpHelpAction);
  tb->addAction(win->fileOptionsAction);
  tb->addAction(win->helpFileBugAction);
}

