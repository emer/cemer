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

#include <QMap>
#include <QModelIndex>

#include <iostream>
using namespace std;

class ColorEntry {
public:
  QColor	base; // for normal background (light color)
  QColor	highlight; // for when row is highlighted (darker)
  
  void		getValues(QColor& b, QColor& h) const {
    b = base; h = highlight;}
  
  ColorEntry() {}
  ColorEntry(const QColor& b, const QColor& h)
  : base(b), highlight(h) {}
  ColorEntry(const ColorEntry& cp)
  : base(cp.base), highlight(cp.highlight) {}
  
  ColorEntry& operator=(const ColorEntry& cp) {
    base = cp.base; highlight = cp.highlight; return *this;}
};

typedef QMap<int, ColorEntry> ColorMap;

iTreeWidget::iTreeWidget(QWidget* parent)
:inherited(parent)
{
  init();
}

iTreeWidget::~iTreeWidget()
{
  if (m_highlightColors) {
    delete (ColorMap*)(m_highlightColors);
  }
}

void  iTreeWidget::init() {
  m_highlightRows = false;
  m_highlightColors = NULL; // created if highlighting enabled
  // we never use this class for Qt-internal dnd semantics
  setDragDropMode(DragDrop);
  connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
    this,  SLOT(this_itemExpanded(QTreeWidgetItem*)) );
  connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
    this,  SLOT(this_itemCollapsed(QTreeWidgetItem*)) );
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

void iTreeWidget::dragMoveEvent(QDragMoveEvent* ev) {
  // force all ops to be "Move" regardless (since we determine op in menu anyway)
  ev->setDropAction(Qt::MoveAction);
  //note: we accept autoscroll decision regardless
  inherited::dragMoveEvent(ev);
  switch (dropIndicatorPosition()) {
  //TEMP: forbid above/below
  case AboveItem:
  case BelowItem:
    ev->ignore();
    break;
  case OnItem: {
    QModelIndex index = indexAt(ev->pos());
    iTreeWidgetItem* item = dynamic_cast<iTreeWidgetItem*>(itemFromIndex(index));
    if (item) {
      if (!item->acceptDrop(ev->mimeData())) {
        ev->ignore();
      }
    } else ev->ignore();
    } break;
  case OnViewport:
     ev->ignore();
    break;
  }

}

void iTreeWidget::drawRow(QPainter* painter,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
/*  if (highlightRows()) {
    iTreeWidgetItem* item = dynamic_cast<iTreeWidgetItem*>(itemFromIndex(index));
    if (!item) goto no_hi;
    int idx = item->highlightIndex();
    if (!hasHighlightColor(idx)) goto no_hi;
    ColorMap* cm = (ColorMap*)highlightColors();
    ColorEntry& ce = (*cm)[idx]; //NOTE: insure idx exists, otherwise creates dummy ce
    QStyleOptionViewItem opt = option; // gotta dup to modify
    opt.palette.setColor(QPalette::Active, QPalette::Base, ce.base); 
    opt.palette.setColor(QPalette::Active, QPalette::Highlight, ce.highlight); 
    opt.palette.setColor(QPalette::Inactive, QPalette::Highlight, ce.highlight); 
    inherited::drawRow(painter, opt, index);
    return;
  }
no_hi: */
  inherited::drawRow(painter, option, index);
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

/*bool iTreeWidget::getHighlightColor(int idx, QColor& base,
    QColor& hilight) const
{
  if ((idx <= 0) || ! m_highlightColors) return false;
  ColorMap* cm = (ColorMap*)highlightColors();
  if (cm->contains(idx)) {
    ColorEntry& ce = (*cm)[idx];
    ce.getValues(base, highlight);
    return true;
  }
  return false;
}*/

bool iTreeWidget::hasHighlightColor(int idx) const {
  if ((idx <= 0) || ! m_highlightColors) return false;
  ColorMap* cm = (ColorMap*)highlightColors();
  return cm->contains(idx);
}

void* iTreeWidget::highlightColors() const {
  if (!m_highlightColors) 
    m_highlightColors = new ColorMap;
  return m_highlightColors;
}

void iTreeWidget::resizeColumnsToContents() {
//FIXME: BA 2006-12-22
// need to develop better approach, perhaps we can
// at least shrink a col
// the key properties:
/* key code from Qt treewidget:
    int contents = sizeHintForColumn(column);
    int header = d->header->isHidden() ? 0 : d->header->sectionSizeHint(column);
    d->header->resizeSection(column, qMax(contents, header));
*/
  if (columnCount() > 0)
    resizeColumnToContents(0);
/*was:  for (int i = 0; i < (columnCount() - 1); ++i) {
    resizeColumnToContents(i);
  }
*/
}

void iTreeWidget::setHighlightColor(int idx, const QColor& base)
{
  QColor h = base.dark(150); // returns color 100/150 (2/3) as bright
  setHighlightColor(idx, base, h);
}

void iTreeWidget::setHighlightColor(int idx, const QColor& base,
    const QColor& hilight)
{
  if (idx <= 0) return; // bad caller!!!
  ColorMap* cm = (ColorMap*)highlightColors();
  cm->insert(idx, ColorEntry(base, hilight));
}

void iTreeWidget::setHighlightRows(bool value) {
  if (m_highlightRows == value) return;
  m_highlightRows = value;
  update();
}

Qt::DropActions iTreeWidget::supportedDropActions() const {
  return Qt::MoveAction;
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

void iTreeWidgetItem::setBackgroundColor(const QColor& color, int col) 
{
  if (col < 0) {
    for (int i = 0; i < columnCount(); ++i) {
      setData(i, Qt::BackgroundColorRole, color);
    }
  } else if (col < columnCount()) {
    setData(col, Qt::BackgroundColorRole, color);
  }
}

void iTreeWidgetItem::setTextColor(const QColor& color, int col) {
  if (col < 0) {
    for (int i = 0; i < columnCount(); ++i) {
      setData(i, Qt::TextColorRole, color);
    }
  } else if (col < columnCount()) {
    setData(col, Qt::TextColorRole, color);
  }
}

void iTreeWidgetItem::resetBackgroundColor(int col) {
  QVariant var; // check for existing color, don't set if not necessary
  if (col < 0) {
    for (int i = 0; i < columnCount(); ++i) {
      var = data(i, Qt::BackgroundColorRole);
      if (var.isValid())
        setData(i, Qt::BackgroundColorRole, QVariant());
    }
  } else if (col < columnCount()) {
    var = data(col, Qt::BackgroundColorRole);
    if (var.isValid())
      setData(col, Qt::BackgroundColorRole, QVariant());
  }
}

void iTreeWidgetItem::resetTextColor(int col) {
  QVariant var; // check for existing color, don't set if not necessary
  if (col < 0) {
    for (int i = 0; i < columnCount(); ++i) {
      var = data(i, Qt::TextColorRole);
      if (var.isValid())
        setData(i, Qt::TextColorRole, QVariant());
    }
  } else if (col < columnCount()) {
    var = data(col, Qt::TextColorRole);
    if (var.isValid())
      setData(col, Qt::TextColorRole, QVariant());
  }
}


