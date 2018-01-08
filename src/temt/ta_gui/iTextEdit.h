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

#ifndef iTextEdit_h
#define iTextEdit_h 1

#include "ta_def.h"

#include <iCodeCompleter>
#include <QTextEdit>

// provides emacs keys for text editing!

class TA_API iTextEdit: public QTextEdit { 
INHERITED(QTextEdit)
  Q_OBJECT
public:
  iTextEdit(QWidget* parent = 0, bool add_completer = false);
  iTextEdit(const QString &text, QWidget *parent = 0, bool add_completer = false);
  ~iTextEdit();

  bool              completion_enabled;
  
  virtual void      clearExtSelection();	   // clear extended selection mode and also clear any existing selection
  iCodeCompleter*   GetCompleter() { return completer; }

public slots:
  virtual bool      findPrompt();               // find function with prompt for search string
  void              DoCompletion(bool extend);  // what we do when the code completion key combo is pressed
  void              CompletionDone();           // On return (selection of completion)
  
#ifndef __MAKETA__
signals:
  void              lookupKeyPressed();	// use this as hook for looking up information based on current text etc (completion) -- bound to Ctrl-L
  void              completed(QModelIndex index); // emit this signal when the use makes a completion selection
  void              characterEntered(); // use this as hook for looking up information based on current text etc (completion) -- when using the Completer feature rather than lookup feature
#endif

protected:
  bool              ext_select_on;	 // toggled by Ctrl+space -- extends selection with keyboard movement
  QString           prev_find_str;	 // previous find string
  iCodeCompleter*   completer; //
  int               cursor_position_from_end; // cursor position for start of completion - only set during completion
  int               cursor_offset;   // compensate for cursor movement by user while completion selections are visible
  void              keyPressEvent(QKeyEvent* e) override;
  void              contextMenuEvent(QContextMenuEvent* e) override;
  void              focusInEvent(QFocusEvent* e) override;
  bool              eventFilter(QObject* obj, QEvent * e) override;

  protected slots:
  void              InsertCompletion(const QString& the_completion);
  void              selectionChanged();

private:
  void              init(bool add_completer = false);
  
};

#endif // iTextEdit_h
