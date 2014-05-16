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
#include <QStyledItemDelegate>

class iTreeWidgetItem; //
class iLineEdit; //

class TA_API iTreeWidget: public QTreeWidget { 
INHERITED(QTreeWidget)
  Q_OBJECT
public:
  int           move_after_edit; // direction to move after edit
  int           edit_start_pos;  // position to start when editing

  bool		hasHighlightColor(int idx) const;
  void		setHighlightColor(int idx, const QColor& base);
    // synthesizes the darker highlight color
  void		setHighlightColor(int idx, const QColor& base,  const QColor& hilight);
    // sets the color for idx >= 1 (0 undefined); base should be fairly light, hilight darker, if NULL, then it is computed
    
  bool		highlightRows() const {return m_highlightRows;}
    // whether we use the highlight information from items
  void		setHighlightRows(bool value);

  bool		siblingSel() const {return m_sibling_sel;}
  // whether we enforce sibling-only selection
  void		setSiblingSel(bool value);
  QModelIndex   indexFromItem(QTreeWidgetItem* itm, int column = 0) const;
  // get the model index of this item -- promoting this function to public
  bool          selectItem(QTreeWidgetItem* itm, int column = 0);
  // select given item
  QTreeWidgetItem* getPrevItem(QTreeWidgetItem* itm, int n_up = 1) const;
  // get the previous item in the tree before given itm, if avail
  QTreeWidgetItem* getNextItem(QTreeWidgetItem* itm, int n_dn = 1) const;
  // get the next item in the tree after given itm, if avail

  void 		resizeColumnsToContents(); // convenience: resizes all but last col
  virtual void	clearExtSelection();	   // clear extended selection mode and also clear any existing selection

#ifndef __MAKETA__
  void 		scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
  void		scrollTo(QTreeWidgetItem* item, ScrollHint hint = EnsureVisible);
#endif

  void 	keyboardSearch(const QString &search) override;
  
  iTreeWidget(QWidget* parent = 0);
  ~iTreeWidget();
  
signals:
  void 		contextMenuRequested(QTreeWidgetItem* item, const QPoint& pos, int col);
  void          itemEdited(const QModelIndex& index, int move_after = 0) const;
  // signal when given item was edited -- emitted by itemWasEdited slot -- move_after is direction to move down (+1) or up (-1) or nowhere 0

public slots:
  virtual void  itemWasEdited(const QModelIndex& index) const;
  // called by delegate when item data at given index was edited
  virtual void  lookupKeyPressed(iLineEdit* le) const;
  // called by editor for lookup key

protected:
  DropIndicatorPosition drop_ind; // we capture this from the dragMove event
  QPoint		drop_pos; // we capture this from the drop event
  int			key_mods; // we captur this from the drop event
  bool			m_highlightRows;
  bool			m_sibling_sel; // if true, only siblings (items at same level) can be selected -- items at different levels of the tree will not be selected -- true by default
  bool			ext_select_on;	   // toggled by Ctrl+space -- extends selection with keyboard movement
  mutable void*		m_highlightColors; // a QMap
  int                   scrollTimerId;
  
  void*		highlightColors() const; // insures map exists
  void 	        drawRow(QPainter* painter, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override;
  void 	        dragMoveEvent(QDragMoveEvent* ev) override;
  void		dropEvent(QDropEvent* e) override;
  bool 	        dropMimeData(QTreeWidgetItem* parent, int index, 
                             const QMimeData* data, Qt::DropAction action) override;
  // we always delegate to the item, and always return false (we handle item manipulation manually)
  void 	        contextMenuEvent(QContextMenuEvent* e) override;
  void		doItemExpanded(QTreeWidgetItem* item, bool expanded);
  void          timerEvent(QTimerEvent* e) override;
  Qt::DropActions supportedDropActions() const;
  void 		setSelection(const QRect &rect,
			     QItemSelectionModel::SelectionFlags command);
  // this is workaround for drag scrolling bug in qt5.2
  virtual  void dragScroll();

  void 	        keyPressEvent(QKeyEvent* e) override;
  // bool          event(QEvent* ev) override;

protected slots:
  void		this_itemExpanded(QTreeWidgetItem* item);
  void		this_itemCollapsed(QTreeWidgetItem* item);
private:
  void		init();
};

class TA_API iTreeWidgetDefaultDelegate: public QStyledItemDelegate {
  // this delegate is used to provide editing feedback info to the standard tree widget
INHERITED(QStyledItemDelegate)
Q_OBJECT
public:
  iTreeWidget*   own_tree_widg;
 
  iTreeWidgetDefaultDelegate(iTreeWidget* own_tw);

  QWidget*     createEditor(QWidget *parent,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;

  void         setModelData(QWidget* editor, QAbstractItemModel* model,
                            const QModelIndex& index) const override;

  bool          eventFilter(QObject *object, QEvent *event);
};

#endif // iTreeWidget_h
