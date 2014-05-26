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

#include "iTreeWidget.h"

#include <iTreeWidgetItem>

#include <taMisc>
#include <taiMisc>
// ^^ note: this creates dependence on ta stuff, but needed for keyboard prefs

#include <QMap>
#include <QModelIndex>
#include <QScrollBar>
#include <QPalette>
#include <QContextMenuEvent>
#include <iLineEdit>
#include <QTextEdit>
#include <QCoreApplication>

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
  scrollTimerId = 0;
  init();
}

iTreeWidget::~iTreeWidget()
{
  if (m_highlightColors) {
    delete (ColorMap*)(m_highlightColors);
  }
}

void  iTreeWidget::init() {
  move_after_edit = 0;
  edit_start_pos = 0;
  edit_start_kill = false;
  ext_select_on = false;
  m_sibling_sel = true;
  m_highlightRows = false;
  m_highlightColors = NULL; // created if highlighting enabled
  // we never use this class for Qt-internal dnd semantics
  setAutoScroll(true);
  setDragDropMode(DragDrop);

  iTreeWidgetDefaultDelegate* del = new iTreeWidgetDefaultDelegate(this);
  setItemDelegate(del);

  connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
    this,  SLOT(this_itemExpanded(QTreeWidgetItem*)) );
  connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
    this,  SLOT(this_itemCollapsed(QTreeWidgetItem*)) );
  // default is to do auto-open
//buggy!!!  setAutoExpandDelay(500);

  // make active same as inactive
  QPalette p = palette();
  p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Active, QPalette::Highlight));
  p.setColor(QPalette::Inactive, QPalette::HighlightedText, p.color(QPalette::Active, QPalette::HighlightedText));
  setPalette(p);
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
  if(!scrollTimerId)
    scrollTimerId = startTimer(100);
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
  // don't do keyboard search if doing edit key pressed
  // for some reason this is always activating even when this is not a trigger
  // and thus preventing it from working
  // if(editTriggers() & QAbstractItemView::EditKeyPressed)
  //   return;

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


QModelIndex iTreeWidget::indexFromItem(QTreeWidgetItem* itm, int column) const {
  return inherited::indexFromItem(itm, column);
}

bool iTreeWidget::selectItem(QTreeWidgetItem* itm, int column) {
  QModelIndex idx = indexFromItem(itm, column);
  if(!idx.isValid()) return false;
  selectionModel()->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);
  return true;
}

QTreeWidgetItem* iTreeWidget::getPrevItem(QTreeWidgetItem* itm, int n_up) const {
  QModelIndex mi = indexFromItem(itm);
  QModelIndex pi = mi.sibling(mi.row()-n_up, mi.column());
  if(!pi.isValid())
    return NULL;
  return itemFromIndex(pi);
}

QTreeWidgetItem* iTreeWidget::getNextItem(QTreeWidgetItem* itm, int n_dn) const {
  QModelIndex mi = indexFromItem(itm);
  QModelIndex pi = mi.sibling(mi.row()+n_dn, mi.column());
  if(!pi.isValid())
    return NULL;
  return itemFromIndex(pi);
}

void iTreeWidget::keyPressEvent(QKeyEvent* e) {
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  
  QTreeWidgetItem* cur_item = currentItem();
  
  if(cur_item) {
    EditTriggers etrig = editTriggers();
    if(cur_item->text(0).isEmpty()) {
      if(!(etrig & QAbstractItemView::AnyKeyPressed)) {
        etrig |= QAbstractItemView::AnyKeyPressed;
        setEditTriggers(etrig);
      }
    }
    else {
      if(etrig & QAbstractItemView::AnyKeyPressed) {
        etrig &= ~QAbstractItemView::AnyKeyPressed;
        setEditTriggers(etrig);
      }
    }
  }
  
  if (e->modifiers() & Qt::ShiftModifier) {
    QTreeWidgetItem* prev_or_next_item = NULL;
    if (e->key() == Qt::Key_Up) {
      prev_or_next_item = getPrevItem(currentItem(), 1);
      
    }
    else if (e->key() == Qt::Key_Down) {
      prev_or_next_item = getNextItem(currentItem(), 1);
    }
    if (prev_or_next_item) {
      if (prev_or_next_item->isSelected() == false) {
        prev_or_next_item->setSelected(true);
        setCurrentItem(prev_or_next_item);
      }
      else {
        setCurrentItem(prev_or_next_item);            }
    }
    
    e->accept();
    return;
  }
  
  if(ctrl_pressed) {
    QPersistentModelIndex newCurrent = currentIndex();
    switch (e->key()) {
      case Qt::Key_S:		// s works too
      case Qt::Key_Space:
        clearSelection();
        // select this guy
        selectionModel()->setCurrentIndex(currentIndex(),
                                          QItemSelectionModel::ClearAndSelect);
        // if(cur_item && cur_item->flags() & Qt::ItemIsEditable) {
        // editItem(cur_item);     // todo: get column
        // }
        // else {
        ext_select_on = true;
        // }
        e->accept();
        return;			// don't continue
      case Qt::Key_G:
        clearExtSelection();
        e->accept();
        break;
      case Qt::Key_N:
        newCurrent = moveCursor(MoveDown, e->modifiers());
        e->accept();
        break;
      case Qt::Key_P:
        newCurrent = moveCursor(MoveUp, e->modifiers());
        e->accept();
        break;
      case Qt::Key_U:
      case Qt::Key_Up:
        newCurrent = moveCursor(MovePageUp, e->modifiers());
        e->accept();
        break;
      case Qt::Key_Down:
        newCurrent = moveCursor(MovePageDown, e->modifiers());
        e->accept();
        break;
      case Qt::Key_V:
        if(taMisc::emacs_mode) {
          newCurrent = moveCursor(MovePageDown, e->modifiers());
          e->accept();
        }
        else {
          e->ignore();		// save for paste elsewhere
        }
        break;
      case Qt::Key_F:
        // if(cur_item && cur_item->flags() & Qt::ItemIsEditable) {
        //   editItem(cur_item);     // todo: get column
        // }
        // else {
        newCurrent = moveCursor(MoveRight, e->modifiers());
        // }
        e->accept();
        break;
      case Qt::Key_B:
        newCurrent = moveCursor(MoveLeft, e->modifiers());
        e->accept();
        break;
      case Qt::Key_Enter:
      case Qt::Key_Return:
        e->ignore();		// pass this on to anyone higher (e.g., a dialog!)
        return;
      case Qt::Key_A:
        if(cur_item && cur_item->flags() & Qt::ItemIsEditable) {
          edit_start_pos = 0;
          edit_start_kill = false;
          editItem(cur_item);     // todo: get column
        }
        e->accept();
        break;
      case Qt::Key_E:
        if(cur_item && cur_item->flags() & Qt::ItemIsEditable) {
          edit_start_pos = -1;
          edit_start_kill = false;
          editItem(cur_item);     // todo: get column
        }
        e->accept();
        break;
      case Qt::Key_K:
        if(cur_item && cur_item->flags() & Qt::ItemIsEditable) {
          edit_start_pos = 0;
          edit_start_kill = true;
          editItem(cur_item);     // todo: get column
        }
        e->accept();
        break;
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
  // else {
  //   switch (e->key()) {
  //   case Qt::Key_Right:
  //     if(cur_item && cur_item->flags() & Qt::ItemIsEditable) {
  //       editItem(cur_item);     // todo: get column
  //       e->accept();
  //       return;
  //     }
  //     break;
  //   }
  // }
  
  // esc interferes with dialog cancel and other such things
  //   if(event->key() == Qt::Key_Escape) {
  //     clearExtSelection();
  //     return;
  //   }
  inherited::keyPressEvent( e );
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

void iTreeWidget::scrollTo(const QModelIndex &index, ScrollHint hint) {
  inherited::scrollTo(index, hint);
}

void iTreeWidget::scrollTo(QTreeWidgetItem* item, ScrollHint hint) {
  if (!item) return;
  ext_select_on = false;
  scrollTo(indexFromItem(item), hint);
}


void iTreeWidget::timerEvent(QTimerEvent* e) {
#if (QT_VERSION >= 0x050000)
  // this fix is only needed for qt5
  if(e->timerId() == scrollTimerId)
    dragScroll();
#endif
  inherited::timerEvent(e);
}

void iTreeWidget::dragScroll() {
  QModelIndex index = indexAt(mapFromGlobal(QCursor::pos()));
  QModelIndex indexB = indexBelow(index);
  QRect vRect = visualRect(indexB);
  if( height() <= vRect.y() + (vRect.height()))
    scrollTo(indexB);
  else if( y() >= visualRect(indexAbove(index)).y())
    scrollTo(indexAbove(index));
  else {
    killTimer(scrollTimerId);
    scrollTimerId=0;
  }
}

void iTreeWidget::itemWasEdited(const QModelIndex& index) const {
  ((iTreeWidget*)this)->edit_start_pos = 0;
  ((iTreeWidget*)this)->edit_start_kill = false;
  emit itemEdited(index, move_after_edit);
}

void iTreeWidget::lookupKeyPressed(iLineEdit* le) const {
}


////////////////////////////////////////////////
//      iTreeWidgetDefaultDelegate


iTreeWidgetDefaultDelegate::iTreeWidgetDefaultDelegate(iTreeWidget* own_tw) :
  inherited(own_tw)
{
  own_tree_widg = own_tw;
}

QWidget* iTreeWidgetDefaultDelegate::createEditor(QWidget *parent,
                                                  const QStyleOptionViewItem &option,
                                                  const QModelIndex &index) const {
  QWidget* widg = inherited::createEditor(parent, option, index);
  QLineEdit* le = dynamic_cast<QLineEdit*>(widg);
  if(le) {
    iLineEdit* il = new iLineEdit(parent);
    if(own_tree_widg) {
      QObject::connect(il, SIGNAL(lookupKeyPressed(iLineEdit*)),
                       own_tree_widg, SLOT(lookupKeyPressed(iLineEdit*)) );
      il->init_start_pos = own_tree_widg->edit_start_pos;
      il->init_start_kill = own_tree_widg->edit_start_kill;
    }
    return il;
  }
  return widg;
}

void iTreeWidgetDefaultDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                              const QModelIndex& index) const {
  inherited::setModelData(editor, model, index);
  if(own_tree_widg) 
    own_tree_widg->itemWasEdited(index);
}

bool iTreeWidgetDefaultDelegate::eventFilter(QObject *object, QEvent *event) {
  QWidget *editor = qobject_cast<QWidget*>(object);
  if (!editor)
    return false;
  iLineEdit* le = dynamic_cast<iLineEdit*>(editor);
  if(!le) {
    return inherited::eventFilter(object, event);
  }
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* ke = static_cast<QKeyEvent *>(event);
    bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(ke);
    switch (ke->key()) {
    case Qt::Key_Tab:
      if(own_tree_widg) {
        own_tree_widg->move_after_edit = 1;
        // own_tree_widg->edit_start_pos = 0;
      }
      emit commitData(editor);
      emit closeEditor(editor, QAbstractItemDelegate::EditNextItem);
      return true;
    case Qt::Key_Backtab:
      if(own_tree_widg) {
        own_tree_widg->move_after_edit = -1;
        // own_tree_widg->edit_start_pos = 0;
      }
      emit commitData(editor);
      emit closeEditor(editor, QAbstractItemDelegate::EditPreviousItem);
      return true;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      if(!le->hasAcceptableInput())
        return false;
      if(own_tree_widg) {
        own_tree_widg->move_after_edit = 0;
        own_tree_widg->edit_start_pos = 0;
      }
      emit commitData(editor);
      // if(ctrl_pressed) {
      //   emit closeEditor(editor, QAbstractItemDelegate::EditNextItem);
      //   return true;
      // }
      // else {
        emit closeEditor(editor, QAbstractItemDelegate::SubmitModelCache);
        return true;             // filter the return
      // }
    case Qt::Key_Escape:
      // don't commit data
      if(own_tree_widg) {
        own_tree_widg->edit_start_pos = 0;
      }
      emit closeEditor(editor, QAbstractItemDelegate::RevertModelCache);
      break;
    default:
      return false;
    }
    if (editor->parentWidget())
      editor->parentWidget()->setFocus();
    return true;
  }
  else if (event->type() == QEvent::FocusOut) {
    return false;               // never close for focus out
  }
  else if (event->type() == QEvent::ShortcutOverride) {
    if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
      event->accept();
      return true;
    }
  }
  return false;
}

