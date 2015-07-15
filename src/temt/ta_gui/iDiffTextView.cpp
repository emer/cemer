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

#include "iDiffTextView.h"

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
#include <iSplitter>
#include <QLabel>
#include <QAbstractScrollArea>

#include <taMisc>

iDiffTextView::iDiffTextView( QWidget *parent, bool enable_icons )
  : QFrame( parent )
{
  setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  setLineWidth( 2 );

  hl_line = -1;
  hl_n = 1;
  in_text_changed = false;

  QHBoxLayout* mhb = new QHBoxLayout(this);
  mhb->setMargin( 0 );

  split = new iSplitter(this);
  mhb->addWidget(split);

  QFrame* fr0 = new QFrame(this);
  fr0->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  QVBoxLayout* vb0 = new QVBoxLayout(fr0);
  vb0->setSpacing( 2 );
  vb0->setMargin( 0 );
  QLabel* hdr0 = new QLabel("<b>Previous / Original</b>");
  QHBoxLayout* hdb0 = new QHBoxLayout;
  hdb0->setMargin(0);
  hdb0->addStretch();
  hdb0->addWidget(hdr0);
  hdb0->addStretch();
  vb0->addLayout(hdb0);

  QHBoxLayout* hb0 = new QHBoxLayout;
  hb0->setSpacing( 3 );
  hb0->setMargin( 0 );
  vb0->addLayout(hb0);

  view0 = new iTextEdit( this );
  view0->setFontFamily( "Courier" );
  view0->setLineWrapMode( iTextEdit::NoWrap );
  view0->setFrameStyle( QFrame::NoFrame );
  view0->installEventFilter( this );
  view0->setFontPointSize(taMisc::font_size);
  view0->setReadOnly(true);

  // Setup the line number pane
  numbers0 = new iNumberBar( this, enable_icons );
  numbers0->setTextEdit( view0 );

  hb0->addWidget(numbers0);
  hb0->addWidget(view0);

  split->addWidget(fr0);

  QFrame* fr1 = new QFrame(this);
  fr1->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  QVBoxLayout* vb1 = new QVBoxLayout(fr1);
  vb1->setSpacing( 2 );
  vb1->setMargin( 0 );
  QLabel* hdr1 = new QLabel("<b>Current / Modified</b>");
  QHBoxLayout* hdb1 = new QHBoxLayout;
  hdb1->setMargin(0);
  hdb1->addStretch();
  hdb1->addWidget(hdr1);
  hdb1->addStretch();
  vb1->addLayout(hdb1);

  QHBoxLayout* hb1 = new QHBoxLayout;
  hb1->setSpacing( 3 );
  hb1->setMargin( 0 );
  vb1->addLayout(hb1);

  view1 = new iTextEdit( this );
  view1->setFontFamily( "Courier" );
  view1->setLineWrapMode( iTextEdit::NoWrap );
  view1->setFrameStyle( QFrame::NoFrame );
  view1->installEventFilter( this );
  view1->setFontPointSize(taMisc::font_size);
  view1->setReadOnly(true);

  // Setup the line number pane
  numbers1 = new iNumberBar( this, enable_icons );
  numbers1->setTextEdit( view1 );

  hb1->addWidget(numbers1);
  hb1->addWidget(view1);

  split->addWidget(fr1);

  // connect( view->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(textChanged(int,int,int)) );

  connect( view0->verticalScrollBar(), SIGNAL(valueChanged(int)), 
           this, SLOT(view0Scrolled(int)) );
  connect( view1->verticalScrollBar(), SIGNAL(valueChanged(int)), 
           this, SLOT(view1Scrolled(int)) );
}

iDiffTextView::~iDiffTextView() {
}

void iDiffTextView::view0Scrolled(int value) {
  view1->verticalScrollBar()->setSliderPosition(value);
}

void iDiffTextView::view1Scrolled(int value) {
  view0->verticalScrollBar()->setSliderPosition(value);
}

void iDiffTextView::setDiffString(const QString& str) {
  QStringList lines = str.split("\n");
  QString txt0;
  QString txt1;
  int mx = lines.count();
  for(int i=0; i<mx; i++) {
// #if (QT_VERSION >= 0x050000)
//     QString ln = lines[i].toHtmlEscaped();
// #else
//     QString ln = Qt::escape(lines[i]);
// #endif
    QString ln = lines[i];
    QString lnp = "<pre>" + ln + "</pre>";
    QString ln0;
    QString ln1;
    bool ln0sty = false;
    bool ln1sty = false;
    if(ln.startsWith("-")) {
      // ln0 = QString(" ") + ln.right(1);
      ln0 += "<p style=\"background-color:#ffd0d0\">";
      ln0 +=lnp;
      ln0sty = true;
      ln1 += "<p style=\"background-color:#e0e0e0\">&nbsp; ";
      ln1sty = true;
    }
    else if(ln.startsWith("+")) {
      // ln1 = QString(" ") + ln.right(1);
      ln1 += "<p style=\"background-color:#d0ffd0\">";
      ln1 += lnp;
      ln1sty = true;
      ln0 += "<p style=\"background-color:#e0e0e0\">&nbsp; ";
      ln0sty = true;
    }
    else {
      ln0 = lnp;
      ln1 = lnp;
    }
    txt0 += ln0;
    if(ln0sty)
      txt0 += "</p>\n";
    else
      txt0 += "<p>\n";
    txt1 += ln1;
    if(ln1sty)
      txt1 += "</p>\n";
    else
      txt1 += "<p>\n";
  }
  view0->setHtml(txt0);
  view1->setHtml(txt1);
  fixLineSpacing(view0);
  fixLineSpacing(view1);
}

void iDiffTextView::fixLineSpacing(iTextEdit* vw) {
  int lineCount = 1;
  for ( QTextBlock block = vw->document()->begin();
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
  }
}

void iDiffTextView::setHighlightLines(int start_ln, int n_lines) {
  hl_line = start_ln;
  hl_n = n_lines;
  //  textChanged( 0, 0, 1 );
}

// void iDiffTextView::textChanged( int pos, int removed, int added ) {
//   Q_UNUSED( pos );

//   if ( removed == 0 && added == 0 )
//     return;

//   if(in_text_changed) return;	// already in here -- this causes self-changes...
//   in_text_changed = true;

//   int lineCount = 1;
//   for ( QTextBlock block = view->document()->begin();
// 	block.isValid(); block = block.next(), ++lineCount ) {

//     // set format to remove extra space in html mode paragraphs for viewing listings
//     // as this is intended for
//     QTextCursor tc = QTextCursor(block);
//     QTextBlockFormat fmt = block.blockFormat();
//     if(fmt.topMargin() != 1 || fmt.bottomMargin() != 1) {
//       fmt.setTopMargin(1);
//       fmt.setBottomMargin(1);
//       tc.setBlockFormat( fmt );
//     }
// //     if(lineCount >= hl_line && lineCount < hl_line + hl_n) {
// //       QColor bg = view->palette().highlight().color().light( 175 );
// //       fmt.setBackground( bg );
// //     }
// //     else {
// //       QColor bg = view->palette().base().color();
// //       fmt.setBackground( bg );
// //     }
//   }

//   if(hl_line > 1) {
//     QTextCursor tc = view->textCursor();
//     tc.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
//     tc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, hl_line-1);
//     tc.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, hl_n);
//     view->setTextCursor(tc);
//   }

//   in_text_changed = false;
// }

bool iDiffTextView::eventFilter( QObject *obj, QEvent *event )
{
  // if ( obj != view )
  return QFrame::eventFilter(obj, event);

  // if ( event->type() == QEvent::ToolTip ) {
  //   QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

  //   QTextCursor cursor = view->cursorForPosition( helpEvent->pos() );
  //   cursor.movePosition( QTextCursor::StartOfWord, QTextCursor::MoveAnchor );
  //   cursor.movePosition( QTextCursor::EndOfWord, QTextCursor::KeepAnchor );

  //   QString word = cursor.selectedText();
  //   int lineno = numbers->lineNumberFmPos(helpEvent->pos().y());
  //   emit mouseHover(helpEvent->globalPos(), lineno, word);

  //   // QToolTip::showText( helpEvent->globalPos(), word ); // For testing
  // }

  // return false;
}
