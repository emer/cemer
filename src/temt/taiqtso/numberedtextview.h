/* This file is part of the KDE libraries
    Copyright (C) 2005, 2006 Ian Reinhart Geiser <geiseri@kde.org>
    Copyright (C) 2005, 2006 Matt Broadstone <mbroadst@gmail.com>
    Copyright (C) 2005, 2006 Richard J. Moore <rich@kde.org>
    Copyright (C) 2005, 2006 Erik L. Bunce <kde@bunce.us>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
// -*- c++ -*-
#ifndef NUMBERED_TEXT_VIEW_H
#define NUMBERED_TEXT_VIEW_H

#include <QtGui/QFrame>
#include <QtGui/QPixmap>
#include <QtGui/QTextCursor>

#include "itextedit.h"

class QHBoxLayout;

/**
 * @internal Used to display the numbers.
 */
class NumberBar : public QWidget {
  Q_OBJECT
public:
  enum LineFlags {		// flags stored in line_flags for each line
    LF_NONE 	= 0,		// no flag here
    LF_BREAK	= 0x0001,	// breakpoint at this line
    LF_ERROR	= 0x0002,	// there is an error at this line
    LF_WARNING 	= 0x0004,	// there is a warning at this line
  };

  NumberBar( QWidget *parent, bool enable_icons = false );
  ~NumberBar();

  void setTextEdit( iTextEdit *edit );
  override void paintEvent( QPaintEvent *ev );

  virtual void	setLineFlags(int lineno, int flags);
  // set line flags for given line
  virtual int	lineFlags(int lineno) const;
  // get line flags for given line

  virtual void	clearAllLineFlags();
  // reset all line flags
  virtual void 	setCurrentLine( int lineno );
  // set line that is current executing

  virtual int	lineNumberFmPos(float y_pos);
  // get line number from position relative to y position within this widget
  virtual QRectF rectFromLineNumber(int lineno);
  // get visible rectangle of the entire numberbar entry for given line number, including number and icons

signals:
  void	lineFlagsUpdated(int lineno, int flags);
  // a gui action resulted in the flags for given line being updated
  void  viewSource(int lineno);
  // signal to view source for given line number (e.g., what generated this line of code)

public slots:
  void	setBreakpointSlot();
  // set breakpoint at cur_lineno
  void	clearBreakpointSlot();
  // clear breakpoint at cur_lineno
  void	viewSourceSlot();
  // view source at cur_lineno

protected:
  override bool event( QEvent *ev );
//   override void mousePressEvent (QMouseEvent * event);
  override void contextMenuEvent ( QContextMenuEvent * event );

  QVector<int>	line_flags;	// flags for each line of code
  int	cur_lineno;		// current line number -- for gui interactions
  int	text_width;		// width of the text display
  int	text_height;		// height of the text display
  int	icon_width;		// width of the extra icon
  int	total_width;		// total width of bar
  bool	icons_enabled;
  iTextEdit *edit;
  QPixmap stopMarker;
  QPixmap currentMarker;
  QPixmap bugMarker;
  int 	currentLine;		// current line -- where the code execution is now
};

/**
 * Displays a iTextEdit with line numbers.
 */
class NumberedTextView : public QFrame {
  Q_OBJECT
public:
  NumberedTextView( QWidget *parent = 0, bool enable_icons = false);
  ~NumberedTextView();

  /** Returns the iTextEdit of the main view. */
  iTextEdit* 	textEdit() const { return view; }
  /** the NumberBar has a lot of the functionality -- access it directly */
  NumberBar* 	numberBar() const { return numbers; }

  /** set the lines to highlight in the view -- -1 for none */
  void	setHighlightLines(int start_ln, int n_lines=1);

  /** @internal Used to get tooltip events from the view for the hover signal. */
  bool eventFilter( QObject *obj, QEvent *event );

Q_SIGNALS:
  /**
   * Emitted when the mouse is hovered over the text edit component.
   * @param word The word under the mouse pointer
   */
  void mouseHover( const QString &word );

  /**
   * Emitted when the mouse is hovered over the text edit component.
   * @param pos The position of the mouse pointer.
   * @param word The word under the mouse pointer
   */
  void mouseHover( const QPoint &pos, const QString &word );

protected Q_SLOTS:
  /** @internal Used to update the highlight on the current line. */
  void textChanged( int pos, int added, int removed );

private:
  iTextEdit *view;
  NumberBar *numbers;
  QHBoxLayout *box;
  int 	hl_line;
  int 	hl_n;
  bool	in_text_changed;
};


#endif // NUMBERED_TEXT_VIEW_H


