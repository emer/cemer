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


#include "itreeview.h"

#include <qheader.h>

iTreeViewItem::iTreeViewItem (iTreeView* parent, const char* label_, const void* data_)
:QListViewItem(parent)
{
  init(label_, data_);
}

iTreeViewItem::iTreeViewItem (iTreeViewItem* parent, const char* label_, const void* data_)
:QListViewItem(parent)
{
  init(label_, data_);
}

void iTreeViewItem::init(const char* label_, const void* data_) {
  setData(data_);
  setText(label_);
}

void iTreeViewItem::setData(const void* value)  {
  mdata =  (void*)value; // cast away constness
}

QString iTreeViewItem::text() const {
  return QListViewItem::text(0);
}

void iTreeViewItem::setText(const char* value) {
  QListViewItem::setText(0, QString(value));
}


// iTreeView

iTreeView::iTreeView (QWidget* parent, const char* name, WFlags f)
: QListView(parent, name, f)
{
  addColumn("Items");
  setRootIsDecorated(true);
//  header()->setClickEnabled(false);
  header()->hide();

  // signal forwarding
  connect(this, SIGNAL(selectionChanged ( QListViewItem*  )), this, SLOT(selectionChanged( QListViewItem*)));
  connect(this, SIGNAL(currentChanged ( QListViewItem*  )), this, SLOT(currentChanged( QListViewItem*)));
  connect(this, SIGNAL(clicked ( QListViewItem*  )), this, SLOT(clicked( QListViewItem*)));
  connect(this, SIGNAL(clicked ( QListViewItem* , const QPoint&, int  )), this, SLOT(clicked( QListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(pressed ( QListViewItem*  )), this, SLOT(pressed( QListViewItem*)));
  connect(this, SIGNAL(pressed ( QListViewItem* , const QPoint & , int )), this, SLOT(pressed( QListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(doubleClicked ( QListViewItem* , const QPoint & , int )), this, SLOT(doubleClicked( QListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(returnPressed ( QListViewItem*  )), this, SLOT(returnPressed( QListViewItem*)));
  connect(this, SIGNAL(spacePressed ( QListViewItem*  )), this, SLOT(spacePressed( QListViewItem*)));
  connect(this, SIGNAL(rightButtonClicked ( QListViewItem* , const QPoint &, int)), this, SLOT(rightButtonClicked( QListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(rightButtonPressed ( QListViewItem* , const QPoint &, int)), this, SLOT(rightButtonPressed( QListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(mouseButtonPressed ( int , QListViewItem* , const QPoint&, int)), this, SLOT(mouseButtonPressed(int,  QListViewItem*, const QPoint& , int)));
  connect(this, SIGNAL(mouseButtonClicked ( int , QListViewItem* , const QPoint&, int)), this, SLOT(mouseButtonClicked(int, QListViewItem*, const QPoint& , int)));
  connect(this, SIGNAL(contextMenuRequested ( QListViewItem* , const QPoint&, int)), this, SLOT(contextMenuRequested( QListViewItem*, const QPoint& , int)));
  connect(this, SIGNAL(onItem ( QListViewItem* )), this, SLOT(onItem( QListViewItem*)));
  connect(this, SIGNAL(expanded ( QListViewItem*  )), this, SLOT(onItem( QListViewItem*)));
  connect(this, SIGNAL(collapsed ( QListViewItem*  )), this, SLOT(collapsed( QListViewItem*)));
  connect(this, SIGNAL(itemRenamed ( QListViewItem* , int , const QString&  )), this, SLOT(itemRenamed( QListViewItem*, int, const QString&)));
  connect(this, SIGNAL(itemRenamed ( QListViewItem* , int  )), this, SLOT(itemRenamed( QListViewItem*, int)));
}

iTreeViewItem* iTreeView::addItem(iTreeViewItem* parent_, const char* label_, const void* data_) {
  if (parent_ == NULL)
    return new iTreeViewItem(this, label_, data_);
  else
    return new iTreeViewItem(parent_, label_, data_);
}

iTreeViewItem* iTreeView::findItem(const void* data_) {
  iTreeViewItemIterator it(this);
  while (it.current() ) {
    iTreeViewItem* itm = it.current();
    if (itm->data() == data_) return itm;
    ++it;
  }
  return NULL;
}


void iTreeView::setSelectedData(const void* data_, bool selected) {
  iTreeViewItem* itm = findItem(data_);
  if (itm != NULL) {
    setSelected(itm, selected);
    ensureItemVisible(itm);
  }

}

void* iTreeView::selectedData () const {
  void* rval = NULL;
  iTreeViewItem* sel = selectedItem();
  if (sel != NULL)
    rval = (void*)sel->data();
  return rval;
}

