// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "itreewidget.h"

#include <QMap>
#include <QModelIndex>
#include <QScrollBar>

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
  ext_select_on = false;
  m_sibling_sel = true;
  m_highlightRows = false;
  m_highlightColors = NULL; // created if highlighting enabled
  // we never use this class for Qt-internal dnd semantics
  setDragDropMode(DragDrop);
  connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
    this,  SLOT(this_itemExpanded(QTreeWidgetItem*)) );
  connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
    this,  SLOT(this_itemCollapsed(QTreeWidgetItem*)) );
  // default is to do auto-open
//buggy!!!  setAutoExpandDelay(500);
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
      p.rx() = qMax( 0, p.x() );
      p.rx() = qMin( visibleRegion().boundingRect().width(), p.x() ); //TEST
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
  ext_select_on = false;
  //note: we accept autoscroll decision regardless
  inherited::dragMoveEvent(ev);
  if (ev->isAccepted()) {
    drop_ind = dropIndicatorPosition();
    switch (drop_ind) {
    //TEMP: forbid above/below
    case AboveItem:
    case BelowItem:
//      ev->setDropAction(Qt::IgnoreAction);
//      ev->ignore(); // puts up the stop sign icon
      break;
    case OnItem: {
      QModelIndex index = indexAt(ev->pos());
      iTreeWidgetItem* item = dynamic_cast<iTreeWidgetItem*>(itemFromIndex(index));
      if (!item || !item->canAcceptDrop(ev->mimeData())) {
        ev->setDropAction(Qt::IgnoreAction);
      }
      } break;
    case OnViewport:
      ev->setDropAction(Qt::IgnoreAction);
      ev->ignore(); // puts up the stop sign icon
      break;
    }
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
  ext_select_on = false;
  drop_pos = e->pos();
  key_mods = e->keyboardModifiers();
  // we have to skip QTreeWidget because it does the evil gui move, esp. on Mac
//  QTreeView::dropEvent(e);
//TEMP MAC
  QDropEvent et(e->pos(), Qt::CopyAction, e->mimeData(), e->mouseButtons(),
    e->keyboardModifiers(), e->type());
  QTreeView::dropEvent(&et);
}

bool iTreeWidget::dropMimeData(QTreeWidgetItem* parent, int index, 
  const QMimeData* data, Qt::DropAction action) 
{
  iTreeWidgetItem* item = dynamic_cast<iTreeWidgetItem*>(parent);
  
  iTreeWidgetItem::WhereIndicator where = iTreeWidgetItem::WI_ON; // default
  switch (drop_ind) {
  case AboveItem:
  case BelowItem:
    // both these cases are essentially identical -- the index indicates
    // the new target position -- but we need to differentiate the
    // "before" case in our semantics, from the "at end" case
    // NOTE: if parent==NULL then this is the root list, ie us
    if (parent) {
      if (index == parent->childCount()) {
        // "at end" case -- item is correct, but we set the flag
        where = iTreeWidgetItem::WI_AT_END;
      } else { // "before" case
        // so we'll change to that item..
        where = iTreeWidgetItem::WI_BEFORE;
        item = dynamic_cast<iTreeWidgetItem*>(parent->child(index));
      }
    } else { // use us
      if (index == topLevelItemCount()) {
        // "at end" case -- item is correct, but we set the flag
        where = iTreeWidgetItem::WI_AT_END;
        //NOTE: this case cannot be handled, since there is no parent item
        // which could handle the drop, therefore, without redesign, we
        // can't process an "after last" on the root, ex. in a listview
      } else { // "before" case
        // so we'll change to that item..
        where = iTreeWidgetItem::WI_BEFORE;
        item = dynamic_cast<iTreeWidgetItem*>(topLevelItem(index));
      }
    }
    break;
  case OnItem: // default case
    break;
  case OnViewport:
    return false; // probably shouldn't happen
  }
  if (!item) return false;
  
  item->dropped(data, drop_pos, key_mods, where);
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

void iTreeWidget::clearExtSelection() {
  ext_select_on = false;
  inherited::clearSelection();
}

void iTreeWidget::keyboardSearch(const QString& search) {
  // this makes it go directly to single selection mode for keyboard search --
  // perhaps not as powerful as extended select under keyboard ctrl but a LOT
  // more intuitive, esp on mac where you don't even get the ghosty outline of
  // the potentially-selected item..
  QAbstractItemView::SelectionMode prv_md = selectionMode();
  setSelectionMode(QAbstractItemView::SingleSelection);
  inherited::keyboardSearch(search);
  setSelectionMode(prv_md);
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

void iTreeWidget::setSiblingSel(bool value) {
  if (m_sibling_sel == value) return;
  m_sibling_sel = value;
  selectionModel()->clearSelection();
}

Qt::DropActions iTreeWidget::supportedDropActions() const {
  // we have to support both, because Move is def on Mac, Copy for others
  // we have to support all, to allow the keyboard shortcuts on drop
  return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}


void iTreeWidget::this_itemCollapsed(QTreeWidgetItem* item) {
  doItemExpanded(item, false);
}

void iTreeWidget::this_itemExpanded(QTreeWidgetItem* item) {
  doItemExpanded(item, true);
}


void iTreeWidget::keyPressEvent(QKeyEvent* event) {
  bool ctrl_pressed = false;
  if(event->modifiers() & Qt::ControlModifier)
    ctrl_pressed = true;
#ifdef TA_OS_MAC
  // ctrl = meta on apple
  if(event->modifiers() & Qt::MetaModifier)
    ctrl_pressed = true;
#endif
  if(ctrl_pressed) {
    QPersistentModelIndex newCurrent = currentIndex();
    switch (event->key()) {
    case Qt::Key_Space:
      clearSelection();
      ext_select_on = true;
      break;
    case Qt::Key_G:
      clearExtSelection();
      break;
    case Qt::Key_N:
      newCurrent = moveCursor(MoveDown, event->modifiers());
      break;
    case Qt::Key_P:
      newCurrent = moveCursor(MoveUp, event->modifiers());
      break;
    case Qt::Key_U:
      newCurrent = moveCursor(MovePageUp, event->modifiers());
      break;
#ifdef TA_OS_MAC
      // this is a conflict with paste -- only works on mac where cmd and ctrl are diff!
    case Qt::Key_V:
      newCurrent = moveCursor(MovePageDown, event->modifiers());
      break;
#endif
    case Qt::Key_F:
      newCurrent = moveCursor(MoveRight, event->modifiers());
      break;
    case Qt::Key_B:
      newCurrent = moveCursor(MoveLeft, event->modifiers());
      break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      event->ignore();		// pass this on to anyone higher (e.g., a dialog!)
      return;
    }

    // from qabstractitemview.cpp
    QPersistentModelIndex oldCurrent = currentIndex();
    if(newCurrent.isValid() && newCurrent != oldCurrent) {
      QItemSelectionModel::SelectionFlags command;
      if(ext_select_on) {
	// the following logic prevents selecting items at different levels!
	QModelIndexList sels = selectionModel()->selectedIndexes();
	if(m_sibling_sel && sels.count() > 0) {
	  QModelIndex firstpar = sels[0].parent();
	  if(newCurrent.parent() == firstpar) // only select at same level!
	    command = QItemSelectionModel::Select;
	  else
	    command = QItemSelectionModel::Current;
	}
	else {
	  command = QItemSelectionModel::Select;
	}
      }
      else {
	command = QItemSelectionModel::ClearAndSelect;
      }
      selectionModel()->setCurrentIndex(newCurrent, command);
      return;
    }
  }
  if(event->key() == Qt::Key_Escape) {
    clearExtSelection();
    return;
  }
  inherited::keyPressEvent( event );
}

void iTreeWidget::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) {
  if(!m_sibling_sel) {
    inherited::setSelection(rect, command); // do it
    return;
  }

  // get selection state before new selections added..
  QModelIndex firstpar;
  {
    QModelIndexList sels = selectionModel()->selectedIndexes();
    if(sels.count() > 0) {
      firstpar = sels[0].parent();
    }
  }
  // only allow selections at same level (i.e., having a common parent)
  inherited::setSelection(rect, command); // do it
  if(!firstpar.isValid()) return;	  // no prior sel -- just bail
  // now fix it up
  QModelIndexList sels = selectionModel()->selectedIndexes();
  if(sels.count() > 2) {
    int idx = sels.count()-1;
    while(idx >= 0 && sels.count() > idx) {
      QModelIndex itm = sels[idx--];
      if(itm.parent() != firstpar) {
	selectionModel()->select(itm, QItemSelectionModel::Toggle); // turn off
      }
    }
  }
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
  if (children_created || lazy_children) return; // enable is not update!!!!
  lazy_children = true; // do first, in case any signals happen during following...
  UpdateLazyChildren();
}

void iTreeWidgetItem::UpdateLazyChildren() {
  if (!lazy_children) return;
  bool will = willHaveChildren(); // note: this is not 100% guaranteed accurate
  int cc = childCount();
#ifdef DEBUG
  if (cc > 1) {
    qWarning("iTreeWidgetItem::createLazyChildren: 1 dummy item expected; %i items encountered.\n", cc);
  }
#endif
  if (will) {
    if (cc > 0) return; // correct already
    new QTreeWidgetItem(this); // just a placeholder, gets deleted
  } else { // wont'
    if (cc > 0) // we assume only 1!!! 
      delete takeChild(0); // the dummy
    // else none, as should be
  }
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

bool iTreeWidgetItem::isExpandedLeaf() const {
  const QTreeWidgetItem* twi = this;
  while (twi) {
    if (!twi->isExpanded()) return false;
    twi = twi->parent();
  }
  return true;
}

void iTreeWidgetItem::itemExpanded(bool expanded) {
  if (!expanded) return; //  || !lazy_children) return;
  if(!children_created)
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

bool iTreeWidgetItem::willHaveChildren() const {
  bool will = false;
  willHaveChildren_impl(will);
  return will;
}



