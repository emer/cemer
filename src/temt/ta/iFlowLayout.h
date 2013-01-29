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

#ifndef iFlowLayout_h
#define iFlowLayout_h 1

#include "ta_def.h"

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

#define defAlign (Qt::AlignTop | Qt::AlignCenter)

// Alignment handling:
// Left, Right, and Center put the "spacing" value between members
// Justify pushes items to both edges, and distributes the space evenly

typedef QList<QLayoutItem*> QLayoutItemList; //note: not exported

class TA_API iFlowLayout : public QLayout
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
  int doLayout( const QRect&, bool testonly = false );
  void layoutLine(const QRect& r, QLayoutItemList& line_it);
  QLayoutItemList itemList;
  int cached_width;
  int cached_hfw;

};

#endif // iFlowLayout_h
