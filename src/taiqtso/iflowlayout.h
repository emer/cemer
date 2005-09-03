/****************************************************************************
** $Id: qt/examples/customlayout/flow.h   2.3.2   edited 2001-01-26 $
**
** Definition of simple flow layout for custom layout example
**
** Created : 979899
**
** Copyright (C) 1997 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef IFLOWLAYOUT_H
#define IFLOWLAYOUT_H

#include "taiqtso_def.h"

#include <qlayout.h>
#include <qptrlist.h>

#define defAlign ((Qt::AlignmentFlags)(Qt::AlignTop | Qt::AlignCenter))

// Alignment handling:
// Left, Right, and Center put the "spacing" value between members
// Justify pushes items to both edges, and distributes the space evenly

typedef QPtrList<QLayoutItem> QLayoutItemList; //note: not exported

class TAIQTSO_API iFlowLayout : public QLayout
{
public:
  iFlowLayout(QWidget *parent, int border=0, int space=-1,
     int aligment = defAlign);
  iFlowLayout(QLayout* parent, int space=-1,
     int aligment = defAlign);
  iFlowLayout(int space=-1, int aligment = defAlign);

  ~iFlowLayout();

  void addItem( QLayoutItem *item);
  void addWidget(QWidget* item) {add(item);} // for source code consistency
  int alignment(){return malignment;}
  void setAlignment(int value); //default is left top
  bool hasHeightForWidth() const;
  int heightForWidth( int ) const;
  QSize sizeHint() const;
  QSize minimumSize() const;
  QLayoutIterator iterator();
  QSizePolicy::ExpandData expanding() const;

protected:
  int malignment;
  void setGeometry( const QRect& );

private:
  void init();
  int doLayout( const QRect&, bool testonly = FALSE );
  void layoutLine(const QRect& r, QLayoutItemList& line_it);
  QLayoutItemList* list;
  int cached_width;
  int cached_hfw;

};

#endif
