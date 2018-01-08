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

#ifndef iNumberedTextView_h
#define iNumberedTextView_h 1

#include "ta_def.h"

#include <QFrame>

class iTextEdit;
class iNumberBar;
class QHBoxLayout;

/**
 * Displays a iTextEdit with line numbers.
 */
class iNumberedTextView : public QFrame {
  Q_OBJECT
public:
  iNumberedTextView( QWidget *parent = 0, bool enable_icons = false);
  ~iNumberedTextView();

  /** Returns the iTextEdit of the main view. */
  iTextEdit* 	textEdit() const { return view; }
  /** the iNumberBar has a lot of the functionality -- access it directly */
  iNumberBar* 	numberBar() const { return numbers; }

  /** set the lines to highlight in the view -- -1 for none */
  void	setHighlightLines(int start_ln, int n_lines=1);

  /** @internal Used to get tooltip events from the view for the hover signal. */
  bool eventFilter( QObject *obj, QEvent *event );

Q_SIGNALS:
  /**
   * Emitted when the mouse is hovered over the text edit component.
   * @param lineno The line number that the mouse is on
   * @param word The word under the mouse pointer
   */
  void mouseHover(const QPoint &pos, int lineno, const QString& word);

protected Q_SLOTS:
  /** @internal Used to update the highlight on the current line. */
  void textChanged( int pos, int added, int removed );

private:
  iTextEdit *view;
  iNumberBar *numbers;
  QHBoxLayout *box;
  int 	hl_line;
  int 	hl_n;
  bool	in_text_changed;
};


#endif // iNumberedTextView_h
