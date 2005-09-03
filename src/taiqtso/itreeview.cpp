/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

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

