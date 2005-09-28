// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
/****************************************************************************
** $Id: qt/examples/customlayout/flow.cpp   2.3.2   edited 2001-01-26 $
**
** Implementing your own layout: flow example
**
** Copyright (C) 1996 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "iflowlayout.h"

template class  QPtrList<QLayoutItem>;

class iFlowLayoutIterator :public QGLayoutIterator
{
public:
    iFlowLayoutIterator( QList<QLayoutItem> *l ) :idx(0), list(l)  {}
    uint count() const;
    QLayoutItem *current();
    QLayoutItem *next();
    QLayoutItem *takeCurrent();

private:
    int idx;
    QList<QLayoutItem> *list;

};

uint iFlowLayoutIterator::count() const
{
    return list->count();
}

QLayoutItem *iFlowLayoutIterator::current()
{
    return idx < int(count()) ? list->at(idx) : 0;
}

QLayoutItem *iFlowLayoutIterator::next()
{
    idx++; return current();
}

QLayoutItem *iFlowLayoutIterator::takeCurrent()
{
    return idx < int(count()) ? list->take( idx ) : 0;
}


/////////////////////////////////////////////////////
//  iFlowLayout                                    //
/////////////////////////////////////////////////////

iFlowLayout::iFlowLayout(QWidget *parent, int border, int space,
    int aligment)
:  QLayout( parent, border, space),
  malignment(aligment), cached_width(0)
{
  init();
}

iFlowLayout::iFlowLayout(QLayout* parent, int space, int aligment)
:   QLayout( parent, space),
    malignment(aligment), cached_width(0)
{
  init();
}

iFlowLayout::iFlowLayout(int space, int aligment)
      : QLayout( space),
      malignment(aligment), cached_width(0)
{
  init();
}

iFlowLayout::~iFlowLayout()
{
  deleteAllItems();
  delete list;
  list = NULL;
}

void iFlowLayout::init() {
  list = new QLayoutItemList();
}

int iFlowLayout::heightForWidth( int w ) const
{
    if ( cached_width != w ) {
        //Not all C++ compilers support "mutable" yet:
        iFlowLayout * mthis = (iFlowLayout*)this;
        int h = mthis->doLayout( QRect(0,0,w,0), TRUE );
        mthis->cached_hfw = h;
        return h;
    }
    return cached_hfw;
}

void iFlowLayout::addItem( QLayoutItem *item)
{
    list->append( item );
}

void iFlowLayout::setAlignment(int value) {
  if (malignment == value) return;
  malignment = value;
  doLayout(geometry());
}

bool iFlowLayout::hasHeightForWidth() const
{
    return TRUE;
}

QSize iFlowLayout::sizeHint() const
{
    return minimumSize();
}

QSizePolicy::ExpandData iFlowLayout::expanding() const
{
    return QSizePolicy::NoDirection;
}

QLayoutIterator iFlowLayout::iterator()
{
    return QLayoutIterator( new iFlowLayoutIterator( list ) );
}

void iFlowLayout::setGeometry( const QRect &r )
{
    QLayout::setGeometry( r );
    doLayout( r );
}

int spaceUsed(QLayoutItemList& line_it) {
  int rval = 0;
  for (uint i = 0; i < line_it.count(); ++i) {
    QLayoutItem* o = line_it.at(i);
    rval += o->sizeHint().width();
  }
  return rval;
}

void iFlowLayout::layoutLine(const QRect& r, QLayoutItemList& line_it) {
  int num = line_it.count();
  if (num == 0) return;
  // set defaults for the geometry items (Left values) -- alignment may override
  int y = r.y();
  int x = r.x(); //left
  int spc = spacing(); // space between items
  int spc_r = 0; // remainder -- add until used up

//  int r_spc; // right space;
  if (malignment & Qt::AlignJustify) {
    int used = spaceUsed(line_it);
    if (num == 1) {
      x += ((r.width() - used) / 2); // center single item
    } else {
      spc = (r.width() - used) / (num - 1);
      spc_r = (r.width() - used) % (num - 1);
    }
  } else if (malignment & Qt::AlignRight) {
    int used = spaceUsed(line_it);
    x += r.width() - (used + ((num - 1) * spc));
  } else if (malignment & Qt::AlignHCenter) {
    int used = spaceUsed(line_it);
    x += (r.width() - (used + ((num - 1) * spc))) / 2;
  } // else assume Left

  for (uint i = 0; i < line_it.count(); ++i) {
    QLayoutItem* o = line_it.at(i);
    o->setGeometry( QRect( QPoint(x, y ), o->sizeHint() ) );
    x = x + o->sizeHint().width() + spc;
    if (spc_r > 0) { // allocate the few spaces left over -- prevents space at right
      ++x;
      --spc_r;
    }
  }
}

int iFlowLayout::doLayout( const QRect &r, bool testonly ) {
  int x = r.x() + margin(); // x,y are the top left of where we are at
  int y = r.y() + margin();
  int h = 0;          //height of this line so far.
  int lines = 0;
  QLayoutItemList line_it; // items for current line
  QListIterator<QLayoutItem> it(*list);
  QLayoutItem *o;
  while ( (o = it.current()) != 0 ) {
      ++it;
      int nextX = x + o->sizeHint().width() + spacing();
      if ( (nextX - spacing() > (r.right() - margin())) && (h > 0) ) {
        if (lines > 0)  y += spacing();
        // set geometry of items for this line
        if ( !testonly ) {
          layoutLine(QRect(r.x(), y, r.width(), h), line_it); //l t w h
        }
        line_it.clear();

        // advance to next line
        x = r.x();
        y = y + h;
        nextX = x + o->sizeHint().width() + spacing();
        h = 0;
        ++lines;
      }
      line_it.append(o);
      x = nextX;
      h = QMAX( h,  o->sizeHint().height() );
  }
  if (line_it.count() > 0) {
    if (lines > 0)  y += spacing();
    if ( !testonly ) {
      layoutLine(QRect(r.x() + margin(), y, r.width() - margin(), h), line_it); //l t w h
    }
    ++lines;
  }
  return (y + h + margin()) - r.y();
}

/*int iFlowLayout::doLayout( const QRect &r, bool testonly ) {
  int x = r.x() + margin(); // x,y are the top left of where we are at
  int y = r.y() + margin();
  int h = 0;          //height of this line so far.
  int lines = 0;
  QLayoutItemList line_it; // items for current line
  QListIterator<QLayoutItem> it(list);
  QLayoutItem *o;
  while ( (o=it.current()) != 0 ) {
      if (lines > 0)  y += spacing();
      ++it;
      int nextX = x + o->sizeHint().width() + spacing();
      if ( (nextX - spacing() > (r.right() - margin())) && (h > 0) ) {
        // set geometry of items for this line
        if ( !testonly ) {
          layoutLine(QRect(r.x(), y, r.width(), h), line_it); //l t w h
        }
        line_it.clear();

        // advance to next line
        x = r.x();
        y = y + h;
        nextX = x + o->sizeHint().width() + spacing();
        h = 0;
        ++lines;
      }
      line_it.append(o);
      x = nextX;
      h = QMAX( h,  o->sizeHint().height() );
  }
  if (line_it.count() > 0) {
    if (lines > 0)  y += spacing();
    layoutLine(QRect(r.x() + margin(), y, r.width() - margin(), h), line_it); //l t w h
    ++lines;
  }
  return (y + h + margin()) - r.y();
} */

/*
int iFlowLayout::doLayout( const QRect &r, bool testonly ) {
    int x = r.x();
    int y = r.y();
    int h = 0;          //height of this line so far.
    QListIterator<QLayoutItem> it(list);
    QLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
        ++it;
        int nextX = x + o->sizeHint().width() + spacing();
        if ( nextX - spacing() > r.right() && h > 0 ) {
            x = r.x();
            y = y + h + spacing();
            nextX = x + o->sizeHint().width() + spacing();
            h = 0;
        }
        if ( !testonly )
            o->setGeometry( QRect( QPoint( x, y ), o->sizeHint() ) );
        x = nextX;
        h = QMAX( h,  o->sizeHint().height() );
    }
    return y + h - r.y();
}

*/
QSize iFlowLayout::minimumSize() const
{
    QSize s(0,0);
    QListIterator<QLayoutItem> it(*list);
    QLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
        ++it;
        s = s.expandedTo( o->minimumSize() );
    }
    return s;
}

