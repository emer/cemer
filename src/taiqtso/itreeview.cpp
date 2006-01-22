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

#include <Q3Header>
using namespace Qt;

iTreeViewItem::iTreeViewItem (iTreeView* parent, const char* label_, const void* data_)
:Q3ListViewItem(parent)
{
  init(label_, data_);
}

iTreeViewItem::iTreeViewItem (iTreeViewItem* parent, const char* label_, const void* data_)
:Q3ListViewItem(parent)
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
  return Q3ListViewItem::text(0);
}

void iTreeViewItem::setText(const char* value) {
  Q3ListViewItem::setText(0, QString(value));
}


// iTreeView

iTreeView::iTreeView (QWidget* parent, WFlags f)
: Q3ListView(parent, NULL, f)
{
  addColumn("Items");
  setRootIsDecorated(true);
//  header()->setClickEnabled(false);
  header()->hide();

  // signal forwarding
  connect(this, SIGNAL(selectionChanged ( Q3ListViewItem*  )), this, SLOT(selectionChanged( Q3ListViewItem*)));
  connect(this, SIGNAL(currentChanged ( Q3ListViewItem*  )), this, SLOT(currentChanged( Q3ListViewItem*)));
  connect(this, SIGNAL(clicked ( Q3ListViewItem*  )), this, SLOT(clicked( Q3ListViewItem*)));
  connect(this, SIGNAL(clicked ( Q3ListViewItem* , const QPoint&, int  )), this, SLOT(clicked( Q3ListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(pressed ( Q3ListViewItem*  )), this, SLOT(pressed( Q3ListViewItem*)));
  connect(this, SIGNAL(pressed ( Q3ListViewItem* , const QPoint & , int )), this, SLOT(pressed( Q3ListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(doubleClicked ( Q3ListViewItem* , const QPoint & , int )), this, SLOT(doubleClicked( Q3ListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(returnPressed ( Q3ListViewItem*  )), this, SLOT(returnPressed( Q3ListViewItem*)));
  connect(this, SIGNAL(spacePressed ( Q3ListViewItem*  )), this, SLOT(spacePressed( Q3ListViewItem*)));
  connect(this, SIGNAL(rightButtonClicked ( Q3ListViewItem* , const QPoint &, int)), this, SLOT(rightButtonClicked( Q3ListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(rightButtonPressed ( Q3ListViewItem* , const QPoint &, int)), this, SLOT(rightButtonPressed( Q3ListViewItem*, const QPoint&, int)));
  connect(this, SIGNAL(mouseButtonPressed ( int , Q3ListViewItem* , const QPoint&, int)), this, SLOT(mouseButtonPressed(int,  Q3ListViewItem*, const QPoint& , int)));
  connect(this, SIGNAL(mouseButtonClicked ( int , Q3ListViewItem* , const QPoint&, int)), this, SLOT(mouseButtonClicked(int, Q3ListViewItem*, const QPoint& , int)));
  connect(this, SIGNAL(contextMenuRequested ( Q3ListViewItem* , const QPoint&, int)), this, SLOT(contextMenuRequested( Q3ListViewItem*, const QPoint& , int)));
  connect(this, SIGNAL(onItem ( Q3ListViewItem* )), this, SLOT(onItem( Q3ListViewItem*)));
  connect(this, SIGNAL(expanded ( Q3ListViewItem*  )), this, SLOT(onItem( Q3ListViewItem*)));
  connect(this, SIGNAL(collapsed ( Q3ListViewItem*  )), this, SLOT(collapsed( Q3ListViewItem*)));
  connect(this, SIGNAL(itemRenamed ( Q3ListViewItem* , int , const QString&  )), this, SLOT(itemRenamed( Q3ListViewItem*, int, const QString&)));
  connect(this, SIGNAL(itemRenamed ( Q3ListViewItem* , int  )), this, SLOT(itemRenamed( Q3ListViewItem*, int)));
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

