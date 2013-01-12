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

#include "iTreeWidgetItem.h"

#include <iTreeWidget>

#include <QMap>
#include <QModelIndex>
#include <QScrollBar>
#include <QPalette>


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
  if(!children_created) {
    CreateChildren();
    if(childCount() > 0) {
      QTreeWidgetItem* lst_chld = child(childCount()-1);
      ((iTreeWidget*)treeWidget())->scrollTo(lst_chld); // make sure all items are now visible
    }
  }
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



