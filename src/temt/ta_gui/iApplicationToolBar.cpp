// Copyright 2013-2018, Regents of the University of Colorado,
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
#include <iBrowseHistory>

#include <QAction>

void iApplicationToolBar::Constr_post() {
  iMainWindowViewer* win = viewerWindow(); //cache
  iToolBar* tb = this;
  
  tb->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(taMisc::app_toolbar_style));
  
  int icon_sz = taiM_->label_height(taiMisc::sizMedium);
  tb->setIconSize(QSize(icon_sz, icon_sz));

  history_back_button = new QToolButton();
  history_back_button->setPopupMode(QToolButton::DelayedPopup);
  history_back_button->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(taMisc::app_toolbar_style));
  tb->addWidget(history_back_button);
  history_back_button->setDefaultAction(win->history_back_action);
  history_back_menu = new QMenu(this);
  connect(history_back_menu, SIGNAL(aboutToShow()), win, SLOT(BackMenuAboutToShow()));
  history_back_button->setMenu(history_back_menu);
  connect(history_back_menu, SIGNAL(triggered(QAction*)), win, SLOT(HistoryGoTo(QAction*)));
  win->history_back_action->setIcon(QIcon(QPixmap(":/images/previous_icon.png")));
  
  history_forward_button = new QToolButton();
  history_forward_button->setPopupMode(QToolButton::DelayedPopup);
  history_forward_button->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(taMisc::app_toolbar_style));
  tb->addWidget(history_forward_button);
  history_forward_button->setDefaultAction(win->history_forward_action);
  history_forward_menu = new QMenu(this);
  connect(history_forward_menu, SIGNAL(aboutToShow()), win, SLOT(ForwardMenuAboutToShow()));
  history_forward_button->setMenu(history_forward_menu);
  connect(history_forward_menu, SIGNAL(triggered(QAction*)), win, SLOT(HistoryGoTo(QAction*)));
  win->history_forward_action->setIcon(QIcon(QPixmap(":/images/next_icon.png")));

  edit_undo_button = new QToolButton();
  edit_undo_button->setPopupMode(QToolButton::DelayedPopup);
  edit_undo_button->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(taMisc::app_toolbar_style));
  tb->addWidget(edit_undo_button);
  edit_undo_button->setDefaultAction(win->edit_undo_action);
  win->edit_undo_action->setShortcut(QKeySequence(QKeySequence::Undo));
  edit_undo_menu = new QMenu(this);
  connect(edit_undo_menu, SIGNAL(aboutToShow()), win, SLOT(UndoMenuAboutToShow()));
  edit_undo_button->setMenu(edit_undo_menu);
  connect(edit_undo_menu, SIGNAL(triggered(QAction*)), win, SLOT(UndoJump(QAction*)));
  win->edit_undo_action->setIcon(QIcon(QPixmap(":/images/editundo.png")));

  edit_redo_button = new QToolButton();
  edit_redo_button->setPopupMode(QToolButton::DelayedPopup);
  edit_redo_button->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(taMisc::app_toolbar_style));
  tb->addWidget(edit_redo_button);
  edit_redo_button->setDefaultAction(win->edit_redo_action);
  win->edit_redo_action->setShortcut(QKeySequence(QKeySequence::Redo));
  edit_redo_menu = new QMenu(this);
  connect(edit_redo_menu, SIGNAL(aboutToShow()), win, SLOT(RedoMenuAboutToShow()));
  edit_redo_button->setMenu(edit_redo_menu);
  connect(edit_redo_menu, SIGNAL(triggered(QAction*)), win, SLOT(RedoJump(QAction*)));
  win->edit_redo_action->setIcon(QIcon(QPixmap(":/images/editredo.png")));

  // Actions have already been constructed for the viewer window's menus.
  // Now add these actions to the toolbar for convenient access.
  tb->addSeparator();
  tb->addAction(win->editFindAction);

  tb->addSeparator();
  tb->addAction(win->fileNewAction);
  tb->addAction(win->fileOpenAction);

  if(!win->isRoot()) {
    tb->addAction(win->fileCloseAction);
    tb->addAction(win->fileSaveAction);
    tb->addAction(win->fileSaveAsAction);

    tb->addSeparator();
    tb->addAction(win->fileOpenSvnBrowserAction);
    tb->addAction(win->fileSvnCommitAction);
    
    tb->addSeparator();
    tb->addAction(win->editCutAction);
    tb->addAction(win->editCopyAction);
    tb->addAction(win->editPasteAction);
    tb->addAction(win->editPasteIntoAction);
    tb->addAction(win->editPasteAssignAction);
    tb->addAction(win->editPasteAppendAction);

    tb->addSeparator();
    tb->addAction(win->ctrlInitAction);
    tb->addAction(win->ctrlRunAction);
    tb->addAction(win->ctrlStopAction);
    tb->addAction(win->ctrlStepAction);
    tb->addAction(win->progTraceAction);
    tb->addAction(win->progStatusAction);
  }
  else {
    tb->addSeparator();
  }    
  tb->addAction(win->viewConsoleFrontAction);

  tb->addSeparator();
  tb->addAction(win->helpWebDocsAction);
  tb->addAction(win->fileOptionsAction);
  tb->addAction(win->helpFileBugAction);
}
