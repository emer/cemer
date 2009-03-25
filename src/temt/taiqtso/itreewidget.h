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


#ifndef ITREEWIDGET_H
#define ITREEWIDGET_H

#include "taiqtso_def.h"

#include <QEvent>
#include <QPoint>
#include <QTableView>
#include <QTreeWidget>

class iTreeWidgetItem; //
//class iTWDropEvent;

class TAIQTSO_API iTreeWidget: public QTreeWidget { 
INHERITED(QTreeWidget)
  Q_OBJECT
public:
  bool			allColumnsShowFocus() const; // qt3 compat
  
  bool			hasHighlightColor(int idx) const;
  void			setHighlightColor(int idx, const QColor& base);
    // synthesizes the darker highlight color
  void			setHighlightColor(int idx, const QColor& base,
    const QColor& hilight);
    // sets the color for idx >= 1 (0 undefined); base should be fairly light, hilight darker, if NULL, then it is computed
    
  bool			highlightRows() const {return m_highlightRows;}
    // whether we use the highlight information from items
  void			setHighlightRows(bool value);
  void 			resizeColumnsToContents(); // convenience: resizes all but last col
  
  iTreeWidget(QWidget* parent = 0);
  ~iTreeWidget();
  
signals:
  void 			contextMenuRequested(QTreeWidgetItem* item, const QPoint& pos, int col);
  
protected:
  DropIndicatorPosition drop_ind; // we capture this from the dragMove event
  QPoint		drop_pos; // we capture this from the drop event
  int			key_mods; // we captur this from the drop event
  bool			m_highlightRows;
  mutable void*		m_highlightColors; // a QMap
  
  void*			highlightColors() const; // insures map exists
  void 			drawRow(QPainter* painter,
    const QStyleOptionViewItem& option, const QModelIndex& index) const; //override
  void 			dragMoveEvent(QDragMoveEvent* ev);  //override
  void			dropEvent(QDropEvent* e); // override
  bool 			dropMimeData(QTreeWidgetItem* parent, int index, 
    const QMimeData* data, Qt::DropAction action); // override -- we always delegate to the item, and always return false (we handle item manipulation manually)
  void 			contextMenuEvent(QContextMenuEvent* e); // override
  void			doItemExpanded(QTreeWidgetItem* item, bool expanded);
_(Qt::DropActions	supportedDropActions() const;)

  void 		keyPressEvent(QKeyEvent* e);	// override
  
protected slots:
  void			this_itemExpanded(QTreeWidgetItem* item);
  void			this_itemCollapsed(QTreeWidgetItem* item);
private:
  void			init();
};


class TAIQTSO_API iTreeWidgetItem: public QTreeWidgetItem { 
INHERITED(QTreeWidgetItem)
friend class iTreeWidget;
public:
  enum WhereIndicator {
    WI_ON,	// this is the value passed when item was dropped right on another
    WI_BEFORE,	// indicates one of the "between" situations (except end); item invoked is the item before which the new item should go
    WI_AT_END	// indicates the "at end" situation; item invoked is the *parent* item
  };
  
  bool			isExpandedLeaf() const; // only true if all parents are also expanded
  inline bool		lazyChildren() const {return lazy_children;}
    // true while lc enabled, but not yet expanded
  void			enableLazyChildren(); // call on create or anytime when empty
  virtual int		highlightIndex() const {return 0;}
    // subclass must override to return >0 for highlighting
  void			setBackgroundColor(const QColor& base, int col = -1);
    // set bg color for indicated column, or all if col=-1 
  void			resetBackgroundColor(int col = -1);
    // return text color for indicated column to default, or for all cols if col=-1
  void			setTextColor(const QColor& color, int col = -1);
    // set text color for indicated column, or all if col=-1; note: statically hides Qt member
  void			resetTextColor(int col);
    // return text color for indicated column to default, or for all cols if col=-1
  bool			willHaveChildren() const;
    // only called when resolving lazy children status -- true if (likely) will have children
  
  virtual bool		canAcceptDrop(const QMimeData* mime) const {return false;}
    // returns whether we can accept the given data in a drop upon us
  
  virtual void		CreateChildren(); // creates the children, called automatically on expand if lazy_children; normally override _impl
  void 			UpdateLazyChildren(); // only call when you think child status may have changed, to add or remove the placeholder +
  

  iTreeWidgetItem(iTreeWidget* parent);
  iTreeWidgetItem(iTreeWidget* parent, iTreeWidgetItem* preceding);
  iTreeWidgetItem(iTreeWidgetItem* parent);
  iTreeWidgetItem(iTreeWidgetItem* parent, iTreeWidgetItem* preceding);
  
protected:
  uint			lazy_children : 1;
  uint			children_created : 1;
  
  virtual void		dropped(const QMimeData* mime, const QPoint& pos, 
    int key_mods, WhereIndicator where) {}
    // what to do when data dropped, usually we put up a drop context menu
  virtual void		itemExpanded(bool expanded); // called when exanded or closed
  virtual void		willHaveChildren_impl(bool& will) const {}
    // set true if will, and only need to call inherited if still false
  virtual void		CreateChildren_impl() {} // override this to create the true children
private:
  void			init();
};

#endif
