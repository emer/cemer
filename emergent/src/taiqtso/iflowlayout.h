// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

#define defAlign (Qt::AlignTop | Qt::AlignCenter)

// Alignment handling:
// Left, Right, and Center put the "spacing" value between members
// Justify pushes items to both edges, and distributes the space evenly

typedef QList<QLayoutItem*> QLayoutItemList; //note: not exported

class TAIQTSO_API iFlowLayout : public QLayout
{
public:
  iFlowLayout(QWidget *parent, int margin = 0, int spacing = -1,
     Qt::Alignment aligment = defAlign);
//  iFlowLayout(QLayout* parent, int spacing = -1,
//     Qt::Alignment aligment = defAlign);
  iFlowLayout(int spacing = -1, Qt::Alignment aligment = defAlign);

  ~iFlowLayout();

  void addItem( QLayoutItem *item);
  Qt::Alignment alignment(){return malignment;}
  void setAlignment(Qt::Alignment value); //default is left top
  bool hasHeightForWidth() const;
  int heightForWidth( int ) const;
//  QLayoutIterator iterator();
  Qt::Orientations expandingDirections() const; //

public: // required or desired overrides
  int 			count() const;
  QLayoutItem* 		itemAt(int index) const;
  QSize 		sizeHint() const;
  QSize minimumSize() const; //
//    void setGeometry(const QRect &rect);
  QLayoutItem*		takeAt(int index);

protected:
  Qt::Alignment malignment;
  void setGeometry( const QRect& );

private:
  void init();
  int doLayout( const QRect&, bool testonly = FALSE );
  void layoutLine(const QRect& r, QLayoutItemList& line_it);
  QLayoutItemList itemList;
  int cached_width;
  int cached_hfw;

};

#endif
