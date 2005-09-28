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

// itreeview.h -- defines a TreeView class, which is a ListView customized
//   specifically for hierarchical tree views
// NOTE: there are many functions on TreeView and TreeViewItem that take
//   these item types as parameters -- as long as you don't try to put
//   any QListViewItem items in the iTreeView, it should be fine
//   to cast (iTreeViewItem*)aQTreeViewItem anywhere

#ifndef ITREEVIEW_H
#define ITREEVIEW_H

#include "taiqtso_def.h"

#include <qlistview.h>
#include <qstring.h>

class iTreeView;

class TAIQTSO_API iTreeViewItem: public QListViewItem {
public:
  iTreeViewItem (iTreeView* parent, const char* label_ = 0, const void* data_ = NULL); // a root item
  iTreeViewItem (iTreeViewItem* parent, const char* label_ = 0, const void* data_ = NULL); // a subitem
//if needed, etc.  iTreeViewItem (iTreeView* parent, iTreeViewItem* after);
//  iTreeViewItem (iTreeViewItem* parent, iTreeViewItem* after);

  const void* data() const {return mdata;}
  void setData(const void* value);

  virtual void setText(const char* value);
  virtual QString text() const;

  iTreeViewItem * firstChild () const {return (iTreeViewItem*)QListViewItem::firstChild();}
  iTreeViewItem * nextSibling () const {return (iTreeViewItem*)QListViewItem::nextSibling();}
  iTreeViewItem * parent () const {return (iTreeViewItem*)QListViewItem::parent();}
  iTreeViewItem * itemAbove () {return (iTreeViewItem*)QListViewItem::itemAbove();}
  iTreeViewItem * itemBelow () {return (iTreeViewItem*)QListViewItem::itemBelow();}

protected:
  void init(const char* label_, const void* data_);
  void* mdata;
};

class TAIQTSO_API iTreeView: public QListView {
  Q_OBJECT
public:
  iTreeView (QWidget* parent = 0, const char* name = 0, WFlags f = 0);

  iTreeViewItem* addItem(iTreeViewItem* parent_ = NULL, const char* label_ = NULL, const void* data_ = NULL); // adds to root if parent = NULL
  void setSelectedData(const void* data_, bool selected = true); // select by TreeView.data value
  void* selectedData () const;

  iTreeViewItem* findItem(const void* data_); // find the item by the item.data

  iTreeViewItem* selectedItem () const {return (iTreeViewItem*)QListView::selectedItem();}
  iTreeViewItem* currentItem () const {return (iTreeViewItem*)QListView::currentItem();}
  iTreeViewItem* firstChild () const {return (iTreeViewItem*)QListView::firstChild();}
  iTreeViewItem* lastItem () const {return (iTreeViewItem*)QListView::lastItem();}
#ifndef __MAKETA__
signals:
  void selectionChanged ( iTreeViewItem * item );
  void currentChanged ( iTreeViewItem * item );
  void clicked ( iTreeViewItem * item );
  void clicked ( iTreeViewItem * item, const QPoint & pnt, int c );
  void pressed ( iTreeViewItem * item );
  void pressed ( iTreeViewItem * item, const QPoint & pnt, int c );
  void doubleClicked ( iTreeViewItem * item, const QPoint &, int );
  void returnPressed ( iTreeViewItem * item );
  void spacePressed ( iTreeViewItem * item );
  void rightButtonClicked ( iTreeViewItem * item, const QPoint &, int );
  void rightButtonPressed ( iTreeViewItem * item, const QPoint &, int );
  void mouseButtonPressed ( int button, iTreeViewItem * item, const QPoint & pos, int c );
  void mouseButtonClicked ( int button, iTreeViewItem * item, const QPoint & pos, int c );
  void contextMenuRequested ( iTreeViewItem * item, const QPoint & pos, int col );
  void onItem ( iTreeViewItem * item);
  void expanded ( iTreeViewItem * item );
  void collapsed ( iTreeViewItem * item );
  void itemRenamed ( iTreeViewItem * item, int col, const QString & text );
  void itemRenamed ( iTreeViewItem * item, int col );
#endif
protected slots: // compatibility slots
  void selectionChanged ( QListViewItem * item ) {emit selectionChanged( (iTreeViewItem*)item);}
  void currentChanged ( QListViewItem * item ) {emit currentChanged( (iTreeViewItem*)item);}
  void clicked ( QListViewItem * item ) {emit clicked( (iTreeViewItem*)item);}
  void clicked ( QListViewItem * item, const QPoint & pnt, int c ) {emit clicked( (iTreeViewItem*)item, pnt, c);}
  void pressed ( QListViewItem * item ) {emit pressed( (iTreeViewItem*)item);}
  void pressed ( QListViewItem * item, const QPoint & pnt, int c ) {emit pressed( (iTreeViewItem*)item, pnt, c);}
  void doubleClicked ( QListViewItem * item, const QPoint & pnt, int c) {emit doubleClicked( (iTreeViewItem*)item, pnt, c);}
  void returnPressed ( QListViewItem * item ) {emit returnPressed( (iTreeViewItem*)item);}
  void spacePressed ( QListViewItem * item ) {emit spacePressed( (iTreeViewItem*)item);}
  void rightButtonClicked ( QListViewItem * item, const QPoint & pnt, int c) {emit rightButtonClicked( (iTreeViewItem*)item, pnt, c);}
  void rightButtonPressed ( QListViewItem * item, const QPoint & pnt, int c) {emit rightButtonPressed( (iTreeViewItem*)item, pnt, c);}
  void mouseButtonPressed ( int button, QListViewItem * item, const QPoint & pos, int c ) {emit mouseButtonPressed(button,  (iTreeViewItem*)item, pos, c);}
  void mouseButtonClicked ( int button, QListViewItem * item, const QPoint & pos, int c ) {emit mouseButtonClicked(button,  (iTreeViewItem*)item, pos, c);}
  void contextMenuRequested ( QListViewItem * item, const QPoint & pos, int col ) {emit contextMenuRequested( (iTreeViewItem*)item, pos, col);}
  void onItem ( QListViewItem * item) {emit onItem( (iTreeViewItem*)item);}
  void expanded ( QListViewItem * item ) {emit onItem( (iTreeViewItem*)item);}
  void collapsed ( QListViewItem * item ) {emit collapsed( (iTreeViewItem*)item);}
  void itemRenamed ( QListViewItem * item, int col, const QString & text ) {emit itemRenamed( (iTreeViewItem*)item, col, text);}
  void itemRenamed ( QListViewItem * item, int col ) {emit itemRenamed( (iTreeViewItem*)item, col);}

};

class TAIQTSO_API iTreeViewItemIterator: public QListViewItemIterator {
public:
  iTreeViewItemIterator(iTreeViewItem* item) : QListViewItemIterator(item) {}
  iTreeViewItemIterator(iTreeViewItem* item, int iteratorFlags)
      : QListViewItemIterator(item, iteratorFlags) {}
  iTreeViewItemIterator(iTreeView* tv) : QListViewItemIterator(tv) {}
  iTreeViewItemIterator(iTreeView* tv, int iteratorFlags)
      : QListViewItemIterator(tv, iteratorFlags) {}

  iTreeViewItemIterator& operator++ () {return (iTreeViewItemIterator&)QListViewItemIterator::operator++();} //prefix ++
  iTreeViewItemIterator& operator+= (int j) {return (iTreeViewItemIterator&)QListViewItemIterator::operator+=(j);}
  iTreeViewItemIterator& operator-- () {return (iTreeViewItemIterator&)QListViewItemIterator::operator--();} // prefix --
  iTreeViewItemIterator& operator-= (int j) {return (iTreeViewItemIterator&)QListViewItemIterator::operator-=(j);}
  iTreeViewItem * operator* () {return (iTreeViewItem*)QListViewItemIterator::operator*();}
  iTreeViewItem* current () const {return (iTreeViewItem*)QListViewItemIterator::current();}

};

#endif // ITREEVIEW_H
