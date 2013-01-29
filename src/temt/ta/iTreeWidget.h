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

#ifndef iTreeWidget_h
#define iTreeWidget_h 1

#include "ta_def.h"

#include <QEvent>
#include <QPoint>
#include <QTableView>
#include <QTreeWidget>

class iTreeWidgetItem; //
//class iTWDropEvent;

class TA_API iTreeWidget: public QTreeWidget { 
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

  bool			siblingSel() const {return m_sibling_sel;}
  // whether we enforce sibling-only selection
  void			setSiblingSel(bool value);

  void 			resizeColumnsToContents(); // convenience: resizes all but last col
  virtual void		clearExtSelection();	   // clear extended selection mode and also clear any existing selection

#ifndef __MAKETA__
  void 			scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
  void			scrollTo(QTreeWidgetItem* item, ScrollHint hint = EnsureVisible);
#endif

  override void 	keyboardSearch(const QString &search);
  
  iTreeWidget(QWidget* parent = 0);
  ~iTreeWidget();
  
signals:
  void 			contextMenuRequested(QTreeWidgetItem* item, const QPoint& pos, int col);
  
protected:
  DropIndicatorPosition drop_ind; // we capture this from the dragMove event
  QPoint		drop_pos; // we capture this from the drop event
  int			key_mods; // we captur this from the drop event
  bool			m_highlightRows;
  bool			m_sibling_sel; // if true, only siblings (items at same level) can be selected -- items at different levels of the tree will not be selected -- true by default
  bool			ext_select_on;	   // toggled by Ctrl+space -- extends selection with keyboard movement
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
  Qt::DropActions	supportedDropActions() const;
  void 			setSelection(const QRect &rect,
				     QItemSelectionModel::SelectionFlags command);

  override void 	keyPressEvent(QKeyEvent* e);	// override
  
protected slots:
  void			this_itemExpanded(QTreeWidgetItem* item);
  void			this_itemCollapsed(QTreeWidgetItem* item);
private:
  void			init();
};


#endif // iTreeWidget_h
