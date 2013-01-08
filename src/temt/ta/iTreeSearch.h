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

#ifndef iTreeSearch_h
#define iTreeSearch_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:


class TA_API iTreeSearch: public QWidget {
// ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS search widget for a tree view -- controls searching in this view
  Q_OBJECT
friend class iTreeView;
friend class iTreeViewItem;
public:
  iTreeView*            tree_view; // pointer to the tree view that we operate on

  QToolBar*             srch_bar;
  QLabel*               srch_label;
  iLineEdit*            srch_text;
  QLabel*               srch_nfound;
  QAction*              srch_clear;
  QAction*              srch_prev;
  QAction*              srch_next;
#ifndef __MAKETA__
  QList<iTreeViewItem*> srch_found; // list of items found
#endif
  int                   cur_item; // currrent item

  void                  search();
  // perform search based on current search text
  void                  unHighlightFound();
  // un-highlight all items
  void                  highlightFound();
  // highlight all found items
  void                  selectCurrent();
  // select current search item

  iTreeSearch(QWidget* parent = NULL);
  iTreeSearch(iTreeView* tree_view_, QWidget* parent = NULL);
  ~iTreeSearch();

public slots:
  void                  srch_text_entered();
  void                  srch_clear_clicked();
  void                  treeview_to_updt();
  void                  srch_next_clicked();
  void                  srch_prev_clicked();

private:
  void Constr();                // construct widget
};

#endif // iTreeSearch_h
