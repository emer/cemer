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


#ifndef ITREEWIDGET_H
#define ITREEWIDGET_H

#include "taiqtso_def.h"

#include <QEvent>
#include <QPoint>
#include <QTreeWidget>

class iTreeWidgetItem; //
//class iTWDropEvent;

class TAIQTSO_API iTreeWidget: public QTreeWidget { 
INHERITED(QTreeWidget)
  Q_OBJECT
public:
/*  enum CustomEventType {
    TWDrop		= QEvent::User + 1
  };*/
  
  bool			allColumnsShowFocus() const; // qt3 compat
  
  void 			resizeColumnsToContents(); // convenience: resizes all but last col
  
  iTreeWidget(QWidget* parent = 0);
  
signals:
  void 			contextMenuRequested(QTreeWidgetItem* item, const QPoint& pos, int col);
  
protected:
  QPoint		drop_pos; // we capture this from the drop event
  
//  void			customEvent(QEvent* e); // override
  
  void			dropEvent(QDropEvent* e); // override
  bool 			dropMimeData(QTreeWidgetItem* parent, int index, 
    const QMimeData* data, Qt::DropAction action); // override -- we always delegate to the item, and always return false (we handle item manipulation manually)
  void 			contextMenuEvent(QContextMenuEvent* e); // override
  QMimeData* 		mimeData(const QList<QTreeWidgetItem *> items) const; // override -- we supply mimeData in our own internal formats, not those of tree items
  void			doItemExpanded(QTreeWidgetItem* item, bool expanded);
  
protected: // new overridables
  virtual QMimeData* 	mimeDataSingle(iTreeWidgetItem* item) const; // default delegates to item
  virtual QMimeData* 	mimeDataMulti(const QList<iTreeWidgetItem*> items) const
    {return NULL;} // note: there will normally be 2 or more items in the list
  
protected slots:
  void			this_itemExpanded(QTreeWidgetItem* item);
  void			this_itemCollapsed(QTreeWidgetItem* item);
};

/*class TAIQTSO_API iTWDropEvent: public QEvent {
INHERITED(QTreeWidget)
public:
  QEvent::Type		type() {return (QEvent::Type)iTreeWidget::iTWDrop;}
  QPoint		pos;
  iTreeWidgetItem*	item;
  
  iTWDropEvent(const QPoint& pos_, iTreeWidgetItem* item_): pos(pos_), item(item_) {}
};*/


class TAIQTSO_API iTreeWidgetItem: public QTreeWidgetItem { 
INHERITED(QTreeWidgetItem)
friend class iTreeWidget;
public:
  
  bool			isExpanded() const;
  inline bool		lazyChildren() const {return lazy_children;}
    // true while lc enabled, but not yet expanded
  void			enableLazyChildren(); // call on create or anytime when empty
  
  virtual bool		acceptDrop(const QMimeData* mime) const {return false;}
    // returns whether we can accept the given data in a drop upon us
  
  virtual void		CreateChildren(); // creates the children, called automatically on expand if lazy_children; normally override _impl
  

  iTreeWidgetItem(iTreeWidget* parent);
  iTreeWidgetItem(iTreeWidget* parent, iTreeWidgetItem* preceding);
  iTreeWidgetItem(iTreeWidgetItem* parent);
  iTreeWidgetItem(iTreeWidgetItem* parent, iTreeWidgetItem* preceding);
  
protected:
  uint			lazy_children : 1;
  uint			children_created : 1;
  
  virtual void		dropped(const QMimeData* mime, const QPoint& pos) {}
    // what to do when data dropped, usually we put up a drop context menu
  virtual void		itemExpanded(bool expanded); // called when exanded or closed
  virtual QMimeData*	mimeData() const {return NULL;} // called for single item src request
  virtual void		CreateChildren_impl() {} // override this to create the true children
private:
  void			init();
};


#endif
