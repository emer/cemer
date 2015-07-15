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

#ifndef iDiffTextView_h
#define iDiffTextView_h 1

// parent includes:
#include "ta_def.h"
#include <QFrame>

// member includes:

// declare all other types mentioned but not required to include:

class iTextEdit;
class iNumberBar;
class iSplitter;

/**
 * Displays side-by-side iTextEdit's with diff info and line numbers.
 */
class iDiffTextView : public QFrame {
  Q_OBJECT
public:
  iDiffTextView( QWidget *parent = 0, bool enable_icons = false);
  ~iDiffTextView();

  void  setDiffString(const QString& str);
  // set the string containing diffs to view

  void  fixLineSpacing(iTextEdit* vw);
  // fix the line spacing to be normal and not so wide like it defaults to..

  /** Returns the iTextEdit of the main view. */
  iTextEdit* 	textEdit0() const { return view0; }
  /** the iNumberBar has a lot of the functionality -- access it directly */
  iNumberBar* 	numberBar0() const { return numbers0; }

  /** Returns the iTextEdit of the main view. */
  iTextEdit* 	textEdit1() const { return view1; }
  /** the iNumberBar has a lot of the functionality -- access it directly */
  iNumberBar* 	numberBar1() const { return numbers1; }

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
  // void textChanged( int pos, int added, int removed );

  void  view0Scrolled(int value);
  void  view1Scrolled(int value);

private:
  iTextEdit *view0;
  iTextEdit *view1;
  iNumberBar *numbers0;
  iNumberBar *numbers1;
  iSplitter   *split;
  int 	hl_line;
  int 	hl_n;
  bool	in_text_changed;
};

#endif // iDiffTextView_h
