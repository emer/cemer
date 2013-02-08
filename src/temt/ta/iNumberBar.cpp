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

#include "iNumberBar.h"

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

iNumberBar::iNumberBar( QWidget *parent, bool enable_icons)
  : QWidget( parent ), edit(0), currentLine(-1)
{
  // Make room for 4 digits and the breakpoint icon
  cur_lineno = 0;
  icons_enabled = enable_icons;
  text_width = fontMetrics().width( QString("0000"));
  text_height = fontMetrics().height();
  icon_width = fontMetrics().width( QString("BRK"));
  total_width = text_width;
  if(enable_icons) {
    total_width += icon_width + fontMetrics().width( QString("=>"));
//     // note: can't seem to find these icons!  need to get them somehow..
//     stopMarker = QPixmap( "images/no.png" );
//     currentMarker = QPixmap( "images/next.png" );
//     bugMarker = QPixmap( "images/bug.png" );
  }
  setFixedWidth(total_width);
}

iNumberBar::~iNumberBar() {
}

void iNumberBar::setCurrentLine(int lineno, bool recenter) {
  if(currentLine == lineno) return;
  currentLine = lineno;
  if(edit && recenter) {
    centerOnLine(currentLine);
  }
}

void iNumberBar::centerOnLine(int lineno) {
  QRectF rect = rectFromLineNumber(lineno);
  qreal hlf_ht = 0.5 * (qreal)edit->viewport()->height();
  qreal curval = (qreal)edit->verticalScrollBar()->value();
  qreal nwval = (rect.top() + curval) - hlf_ht; // rect is relative to current val!
  if(nwval < 0.0) nwval = 0.0;
  edit->verticalScrollBar()->setValue((int)nwval);
}

void iNumberBar::setLineFlags(int lineno, int flags) {
  if(lineno < 1 || lineno >= line_flags.size()) return;
  line_flags.replace(lineno, flags);
}

int iNumberBar::lineFlags( int lineno ) const {
  if(lineno < 1 || lineno >= line_flags.size()) return LF_NONE;
  return line_flags.at(lineno);
}

void iNumberBar::clearAllLineFlags() {
  for(int i=0; i<line_flags.size(); i++) {
    line_flags[i] = LF_NONE;
  }
}

void iNumberBar::setTextEdit( iTextEdit *edit ) {
  this->edit = edit;
  connect( edit->document()->documentLayout(), SIGNAL( update(const QRectF &) ),
	   this, SLOT( update() ) );
  connect( edit->verticalScrollBar(), SIGNAL(valueChanged(int) ),
	   this, SLOT( update() ) );
}

int iNumberBar::lineNumberFmPos(float y_pos) {
  QAbstractTextDocumentLayout *layout = edit->document()->documentLayout();
  int contentsY = edit->verticalScrollBar()->value();
  y_pos += contentsY;		// scroll us up..
  int lineCount = 1;
  for ( QTextBlock block = edit->document()->begin();
	block.isValid(); block = block.next(), ++lineCount ) {

    const QRectF boundingRect = layout->blockBoundingRect( block );
    if(y_pos <= boundingRect.bottom() && y_pos >= boundingRect.top())
      return lineCount;
  }
  return -1;
}

QRectF iNumberBar::rectFromLineNumber(int lineno) {
  QAbstractTextDocumentLayout *layout = edit->document()->documentLayout();
  int contentsY = edit->verticalScrollBar()->value();
  QRectF rval;
  if(lineno < 1 || lineno >= edit->document()->blockCount()) return rval;
  int lineCount = 1;
  QTextBlock block = edit->document()->findBlockByNumber(lineno);
  if(!block.isValid()) return rval;

  const QRectF boundingRect = layout->blockBoundingRect( block );
  QPointF position = boundingRect.topLeft();
  rval = QRectF(0, qRound( position.y() ) - contentsY, total_width, text_height);
  return rval;
}

void iNumberBar::paintEvent( QPaintEvent * ) {
  QAbstractTextDocumentLayout *layout = edit->document()->documentLayout();
  int contentsY = edit->verticalScrollBar()->value();
  qreal pageBottom = contentsY + edit->viewport()->height();
  const QFontMetrics fm = fontMetrics();
  const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1
  int lineCount = 1;

  line_flags.resize(edit->document()->blockCount()+1); // make sure we hold everything -- use 1+ indexing

  QPainter p(this);

  for ( QTextBlock block = edit->document()->begin();
	block.isValid(); block = block.next(), ++lineCount ) {

    const QRectF boundingRect = layout->blockBoundingRect( block );

    QPointF position = boundingRect.topLeft();
    if ( position.y() + boundingRect.height() < contentsY )
      continue;
    if ( position.y() > pageBottom )
      break;

    const QString txt = QString::number( lineCount );
    p.drawText( width() - fm.width(txt), qRound( position.y() ) - contentsY + ascent, txt );

    if(icons_enabled) {
      int lflag = lineFlags(lineCount);
      if ( lflag & LF_ERROR || lflag & LF_WARNING) {
	p.drawText(1, qRound( position.y() ) - contentsY + ascent, "ERR");
// 	p.drawPixmap( 1, qRound( position.y() ) - contentsY, bugMarker );
      }

      // Stop marker
      if ( lflag & LF_BREAK ) {
	p.drawText(1, qRound( position.y() ) - contentsY + ascent, "BRK");
//  	p.drawPixmap(1, qRound( position.y() ) - contentsY, stopMarker );
      }

      // Current line marker
      if ( currentLine == lineCount ) {
	p.drawText(icon_width, qRound( position.y() ) - contentsY + ascent, "=>");
// 	p.drawPixmap(icon_width, qRound( position.y() ) - contentsY, currentMarker );
      }
    }
  }
}

void iNumberBar::contextMenuEvent(QContextMenuEvent * e) {
  cur_lineno = lineNumberFmPos((float)e->pos().y());
  if(cur_lineno < 1) return;	// invalid..
  int lfg = lineFlags(cur_lineno);
  QMenu* menu = new QMenu(this);
  if(lfg & LF_BREAK)
    menu->addAction("Clear Breakpoint", this, SLOT(clearBreakpointSlot())); 
  else
    menu->addAction("Set Breakpoint", this, SLOT(setBreakpointSlot())); 
  menu->addAction("View Source", this, SLOT(viewSourceSlot())); 
  menu->exec(QCursor::pos());
  delete menu;
}

void iNumberBar::setBreakpointSlot() {
  int lfg = lineFlags(cur_lineno);
  lfg |= LF_BREAK;
  setLineFlags(cur_lineno, lfg);
  emit lineFlagsUpdated(cur_lineno, lfg);
}

void iNumberBar::clearBreakpointSlot() {
  int lfg = lineFlags(cur_lineno);
  lfg &= ~LF_BREAK;
  setLineFlags(cur_lineno, lfg);
  emit lineFlagsUpdated(cur_lineno, lfg);
}

void iNumberBar::viewSourceSlot() {
  emit viewSource(cur_lineno);
}

bool iNumberBar::event( QEvent *event ) {
  if ( event->type() == QEvent::ToolTip ) {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

    int lineno = lineNumberFmPos(helpEvent->pos().y());
    int lfg = lineFlags(cur_lineno);
    if(lfg & LF_BREAK) {
      QToolTip::showText( helpEvent->globalPos(), "Stop Here" );
    }
    else if(lfg & LF_ERROR) {
      QToolTip::showText( helpEvent->globalPos(), "Error Here" );
    }
    else if(lfg & LF_WARNING) {
      QToolTip::showText( helpEvent->globalPos(), "Warning Here" );
    }
    else if ( lineno == currentLine ) {
      QToolTip::showText( helpEvent->globalPos(), "Current Line" );
    }
  }

  return QWidget::event(event);
}

