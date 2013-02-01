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

#include "iNumberedTextView.h"

#include <iNumberBar>
#include <iTextEdit>

#include <QTextDocument>
#include <QTextBlock>
#include <QTextEdit>
#include <QBoxLayout>
#include <QScrollBar>
#include <QPainter>
#include <QTextObjectInterface>
#include <QToolTip>
#include <QDebug>
#include <QMenu>

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

iNumberedTextView::iNumberedTextView( QWidget *parent, bool enable_icons )
  : QFrame( parent )
{
  setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  setLineWidth( 2 );

  hl_line = -1;
  hl_n = 1;
  in_text_changed = false;

  // Setup the main view
  view = new iTextEdit( this );
  view->setFontFamily( "Courier" );
  view->setLineWrapMode( iTextEdit::NoWrap );
  view->setFrameStyle( QFrame::NoFrame );
  view->installEventFilter( this );

  connect( view->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(textChanged(int,int,int)) );

  // Setup the line number pane
  numbers = new iNumberBar( this, enable_icons );
  numbers->setTextEdit( view );

  box = new QHBoxLayout( this );
  box->setSpacing( 3 );	// some space btwn
  box->setMargin( 0 );
  box->addWidget( numbers );
  box->addWidget( view );
}

iNumberedTextView::~iNumberedTextView() {
}

void iNumberedTextView::setHighlightLines(int start_ln, int n_lines) {
  hl_line = start_ln;
  hl_n = n_lines;
  textChanged( 0, 0, 1 );
}

void iNumberedTextView::textChanged( int pos, int removed, int added ) {
  Q_UNUSED( pos );

  if ( removed == 0 && added == 0 )
    return;

  if(in_text_changed) return;	// already in here -- this causes self-changes...
  in_text_changed = true;

  int lineCount = 1;
  for ( QTextBlock block = view->document()->begin();
	block.isValid(); block = block.next(), ++lineCount ) {

    // set format to remove extra space in html mode paragraphs for viewing listings
    // as this is intended for
    QTextCursor tc = QTextCursor(block);
    QTextBlockFormat fmt = block.blockFormat();
    if(fmt.topMargin() != 1 || fmt.bottomMargin() != 1) {
      fmt.setTopMargin(1);
      fmt.setBottomMargin(1);
      tc.setBlockFormat( fmt );
    }
//     if(lineCount >= hl_line && lineCount < hl_line + hl_n) {
//       QColor bg = view->palette().highlight().color().light( 175 );
//       fmt.setBackground( bg );
//     }
//     else {
//       QColor bg = view->palette().base().color();
//       fmt.setBackground( bg );
//     }
  }

  if(hl_line > 1) {
    QTextCursor tc = view->textCursor();
    tc.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
    tc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, hl_line-1);
    tc.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, hl_n);
    view->setTextCursor(tc);
  }

  in_text_changed = false;
}

bool iNumberedTextView::eventFilter( QObject *obj, QEvent *event )
{
  if ( obj != view )
    return QFrame::eventFilter(obj, event);

  if ( event->type() == QEvent::ToolTip ) {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

    QTextCursor cursor = view->cursorForPosition( helpEvent->pos() );
    cursor.movePosition( QTextCursor::StartOfWord, QTextCursor::MoveAnchor );
    cursor.movePosition( QTextCursor::EndOfWord, QTextCursor::KeepAnchor );

    QString word = cursor.selectedText();
    int lineno = numbers->lineNumberFmPos(helpEvent->pos().y());
    emit mouseHover(helpEvent->globalPos(), lineno, word);

    // QToolTip::showText( helpEvent->globalPos(), word ); // For testing
  }

  return false;
}
