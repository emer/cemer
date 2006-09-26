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

#include "itreewidget.h"

#include <iostream>
using namespace std;

iTreeWidget::iTreeWidget(QWidget* parent)
:inherited(parent)
{
  connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
    this,  SLOT(this_itemExpanded(QTreeWidgetItem*)) );
}

bool iTreeWidget::allColumnsShowFocus() const {
//NOTE: assuming the following test will return this
  return (selectionBehavior() & QAbstractItemView::SelectRows);
}


//NOTE: this routine copied and modified from Qt3 source
void iTreeWidget::contextMenuEvent(QContextMenuEvent* e)
{
  if (!receivers( SIGNAL(contextMenuRequested(QTreeWidgetItem*,const QPoint&,int)) ) ) {
      e->ignore();
      return;
  }
  if (e->reason() == QContextMenuEvent::Keyboard ) {
    QTreeWidgetItem* item = currentItem();
    if (item) {
      QRect r = visualItemRect(item);
      QPoint p = r.topLeft();
      if (allColumnsShowFocus() )
        p += QPoint( width() / 2, ( r.height() / 2 ) );
      else
        p += QPoint( columnWidth( 0 ) / 2, ( r.height() / 2 ) );
      p.rx() = QMAX( 0, p.x() );
      p.rx() = QMIN( visibleRegion().boundingRect().width(), p.x() ); //TEST
      emit contextMenuRequested(item, viewport()->mapToGlobal( p ), -1 );
    }
  } else {
    QPoint vp = e->pos() ; 
    QTreeWidgetItem* i = itemAt( vp );
//    int c = i ? d->h->mapToLogical( d->h->cellAt( vp.x() ) ) : -1;
    int c = 0; // TODO
    emit contextMenuRequested( i, viewport()->mapToGlobal( vp ), c );
  }
}

void iTreeWidget::doItemExpanded(QTreeWidgetItem* item_, bool expanded) {
  // safe cast, null if not of type
  iTreeWidgetItem* item = dynamic_cast<iTreeWidgetItem*>(item_);
  if (item)
    item->itemExpanded(expanded);
}

void iTreeWidget::dropEvent(QDropEvent* e) {
  drop_pos = e->pos();
  inherited::dropEvent(e);
}

bool iTreeWidget::dropMimeData(QTreeWidgetItem* parent, int index, 
  const QMimeData* data, Qt::DropAction action) 
{
  iTreeWidgetItem* item = dynamic_cast<iTreeWidgetItem*>(parent);
//NOTE: index doesn't seem to be needed -- parent always seems to indicate
// the target of the drop action
  if (!item) return false;
  item->dropped(data, drop_pos);
  return false; // never let Qt manipulate the items
}

void iTreeWidget::resizeColumnsToContents() {
  for (int i = 0; i < (columnCount() - 1); ++i) {
    resizeColumnToContents(i);
  }
}

void iTreeWidget::this_itemCollapsed(QTreeWidgetItem* item) {
  doItemExpanded(item, false);
}

void iTreeWidget::this_itemExpanded(QTreeWidgetItem* item) {
  doItemExpanded(item, true);
}


//////////////////////////
//  iTreeWidgetItem	//
//////////////////////////

iTreeWidgetItem::iTreeWidgetItem(iTreeWidget* parent)
:inherited(parent)
{
  init();
}

iTreeWidgetItem::iTreeWidgetItem(iTreeWidget* parent, iTreeWidgetItem* preceding)
:inherited(parent, preceding)
{
  init();
}

iTreeWidgetItem::iTreeWidgetItem(iTreeWidgetItem* parent)
:inherited(parent)
{
  init();
}

iTreeWidgetItem::iTreeWidgetItem(iTreeWidgetItem* parent, iTreeWidgetItem* preceding)
:inherited(parent, preceding)
{
  init();
}

void iTreeWidgetItem::init() {
  lazy_children = false;
  children_created = false;
}

void iTreeWidgetItem::enableLazyChildren() {
  if (lazy_children || (childCount() > 0) ) return;
  lazy_children = true; // do first, in case any signals happen during following...
  new QTreeWidgetItem(this); // just a placeholder, gets deleted
}

void iTreeWidgetItem::CreateChildren() {
  if (lazy_children) {
    // should only be one child -- prob something went wrong if not
    int cc = childCount();
    if (cc > 1) {
      qWarning("iTreeWidgetItem::createLazyChildren: 1 dummy item expected; %i items encountered.\n", cc);
    } else if (cc == 1) { // expected case
      delete takeChild(0); // the dummy
    } // 0 case shouldn't happen
    lazy_children = false;
  }
  CreateChildren_impl();
  children_created = true;
}

bool iTreeWidgetItem::isExpanded() const {
  QTreeWidget* tw = treeWidget();
  return (tw && tw->isItemExpanded(this));
}

void iTreeWidgetItem::itemExpanded(bool expanded) {
  if (!expanded || !lazy_children) return;
  CreateChildren();
}
