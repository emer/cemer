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
  NumberBar( QWidget *parent, bool enable_icons = false );
  ~NumberBar();

  void setCurrentLine( int lineno );
  void setStopLine( int lineno );
  void setBugLine( int lineno );

  void setTextEdit( iTextEdit *edit );
  void paintEvent( QPaintEvent *ev );

protected:
  bool event( QEvent *ev );

private:
  bool	icons_enabled;
  iTextEdit *edit;
  QPixmap stopMarker;
  QPixmap currentMarker;
  QPixmap bugMarker;
  int stopLine;
  int currentLine;
  int bugLine;
  QRect stopRect;
  QRect currentRect;
  QRect bugRect;
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
  iTextEdit *textEdit() const { return view; }

  /**
   * Sets the line that should have the current line indicator.
   * A value of -1 indicates no line should show the indicator.
   */
  void setCurrentLine( int lineno );

  /**
   * Sets the line that should have the stop line indicator.
   * A value of -1 indicates no line should show the indicator.
   */
  void setStopLine( int lineno );

  /**
   * Sets the line that should have the bug line indicator.
   * A value of -1 indicates no line should show the indicator.
   */
  void setBugLine( int lineno );

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
  int currentLine;
  QTextCursor highlight;
};


#endif // NUMBERED_TEXT_VIEW_H


