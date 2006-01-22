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


// itreeview.h -- defines a TreeView class, which is a ListView customized
//   specifically for hierarchical tree views
// NOTE: there are many functions on TreeView and TreeViewItem that take
//   these item types as parameters -- as long as you don't try to put
//   any Q3ListViewItem items in the iTreeView, it should be fine
//   to cast (iTreeViewItem*)aQTreeViewItem anywhere

#ifndef ITREEVIEW_H
#define ITREEVIEW_H

#include "taiqtso_def.h"

#include <Q3ListView>
#include <QString>

class iTreeView;

class TAIQTSO_API iTreeViewItem: public Q3ListViewItem {
public:
  iTreeViewItem (iTreeView* parent, const char* label_ = 0, const void* data_ = NULL); // a root item
  iTreeViewItem (iTreeViewItem* parent, const char* label_ = 0, const void* data_ = NULL); // a subitem
//if needed, etc.  iTreeViewItem (iTreeView* parent, iTreeViewItem* after);
//  iTreeViewItem (iTreeViewItem* parent, iTreeViewItem* after);

  const void* data() const {return mdata;}
  void setData(const void* value);

  virtual void setText(const char* value);
  virtual QString text() const;

  iTreeViewItem * firstChild () const {return (iTreeViewItem*)Q3ListViewItem::firstChild();}
  iTreeViewItem * nextSibling () const {return (iTreeViewItem*)Q3ListViewItem::nextSibling();}
  iTreeViewItem * parent () const {return (iTreeViewItem*)Q3ListViewItem::parent();}
  iTreeViewItem * itemAbove () {return (iTreeViewItem*)Q3ListViewItem::itemAbove();}
  iTreeViewItem * itemBelow () {return (iTreeViewItem*)Q3ListViewItem::itemBelow();}

protected:
  void init(const char* label_, const void* data_);
  void* mdata;
};

class TAIQTSO_API iTreeView: public Q3ListView {
  Q_OBJECT
public:
  iTreeView (QWidget* parent = 0, Qt::WFlags f = 0);

  iTreeViewItem* addItem(iTreeViewItem* parent_ = NULL, const char* label_ = NULL, const void* data_ = NULL); // adds to root if parent = NULL
  void setSelectedData(const void* data_, bool selected = true); // select by TreeView.data value
  void* selectedData () const;

  iTreeViewItem* findItem(const void* data_); // find the item by the item.data

  iTreeViewItem* selectedItem () const {return (iTreeViewItem*)Q3ListView::selectedItem();}
  iTreeViewItem* currentItem () const {return (iTreeViewItem*)Q3ListView::currentItem();}
  iTreeViewItem* firstChild () const {return (iTreeViewItem*)Q3ListView::firstChild();}
  iTreeViewItem* lastItem () const {return (iTreeViewItem*)Q3ListView::lastItem();}
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
  void selectionChanged ( Q3ListViewItem * item ) {emit selectionChanged( (iTreeViewItem*)item);}
  void currentChanged ( Q3ListViewItem * item ) {emit currentChanged( (iTreeViewItem*)item);}
  void clicked ( Q3ListViewItem * item ) {emit clicked( (iTreeViewItem*)item);}
  void clicked ( Q3ListViewItem * item, const QPoint & pnt, int c ) {emit clicked( (iTreeViewItem*)item, pnt, c);}
  void pressed ( Q3ListViewItem * item ) {emit pressed( (iTreeViewItem*)item);}
  void pressed ( Q3ListViewItem * item, const QPoint & pnt, int c ) {emit pressed( (iTreeViewItem*)item, pnt, c);}
  void doubleClicked ( Q3ListViewItem * item, const QPoint & pnt, int c) {emit doubleClicked( (iTreeViewItem*)item, pnt, c);}
  void returnPressed ( Q3ListViewItem * item ) {emit returnPressed( (iTreeViewItem*)item);}
  void spacePressed ( Q3ListViewItem * item ) {emit spacePressed( (iTreeViewItem*)item);}
  void rightButtonClicked ( Q3ListViewItem * item, const QPoint & pnt, int c) {emit rightButtonClicked( (iTreeViewItem*)item, pnt, c);}
  void rightButtonPressed ( Q3ListViewItem * item, const QPoint & pnt, int c) {emit rightButtonPressed( (iTreeViewItem*)item, pnt, c);}
  void mouseButtonPressed ( int button, Q3ListViewItem * item, const QPoint & pos, int c ) {emit mouseButtonPressed(button,  (iTreeViewItem*)item, pos, c);}
  void mouseButtonClicked ( int button, Q3ListViewItem * item, const QPoint & pos, int c ) {emit mouseButtonClicked(button,  (iTreeViewItem*)item, pos, c);}
  void contextMenuRequested ( Q3ListViewItem * item, const QPoint & pos, int col ) {emit contextMenuRequested( (iTreeViewItem*)item, pos, col);}
  void onItem ( Q3ListViewItem * item) {emit onItem( (iTreeViewItem*)item);}
  void expanded ( Q3ListViewItem * item ) {emit onItem( (iTreeViewItem*)item);}
  void collapsed ( Q3ListViewItem * item ) {emit collapsed( (iTreeViewItem*)item);}
  void itemRenamed ( Q3ListViewItem * item, int col, const QString & text ) {emit itemRenamed( (iTreeViewItem*)item, col, text);}
  void itemRenamed ( Q3ListViewItem * item, int col ) {emit itemRenamed( (iTreeViewItem*)item, col);}

};

class TAIQTSO_API iTreeViewItemIterator: public Q3ListViewItemIterator {
public:
  iTreeViewItemIterator(iTreeViewItem* item) : Q3ListViewItemIterator(item) {}
  iTreeViewItemIterator(iTreeViewItem* item, int iteratorFlags)
      : Q3ListViewItemIterator(item, iteratorFlags) {}
  iTreeViewItemIterator(iTreeView* tv) : Q3ListViewItemIterator(tv) {}
  iTreeViewItemIterator(iTreeView* tv, int iteratorFlags)
      : Q3ListViewItemIterator(tv, iteratorFlags) {}

  iTreeViewItemIterator& operator++ () {return (iTreeViewItemIterator&)Q3ListViewItemIterator::operator++();} //prefix ++
  iTreeViewItemIterator& operator+= (int j) {return (iTreeViewItemIterator&)Q3ListViewItemIterator::operator+=(j);}
  iTreeViewItemIterator& operator-- () {return (iTreeViewItemIterator&)Q3ListViewItemIterator::operator--();} // prefix --
  iTreeViewItemIterator& operator-= (int j) {return (iTreeViewItemIterator&)Q3ListViewItemIterator::operator-=(j);}
  iTreeViewItem * operator* () {return (iTreeViewItem*)Q3ListViewItemIterator::operator*();}
  iTreeViewItem* current () const {return (iTreeViewItem*)Q3ListViewItemIterator::current();}

};

#endif // ITREEVIEW_H
