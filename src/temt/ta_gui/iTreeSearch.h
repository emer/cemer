// Copyright 2013-2017, Regents of the University of Colorado,
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
#include "ta_def.h"
#ifndef __MAKETA__
#include <QWidget>
#endif

// member includes:
#ifndef __MAKETA__
#include <QList>
#endif
#include <taBase_RefList>

// declare all other types mentioned but not required to include:
class iLineEdit; //
class iTreeView; //
class iTreeViewItem; //
class iMenuButton; //
class iActionMenuButton; //

class QToolBar; //
class QLabel; //
class QMenu; //

class TA_API iTreeSearch: public QWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS search widget for a tree view -- controls searching in this view
  Q_OBJECT
  friend class iTreeView;
  friend class iTreeViewItem;
public:
  enum SearchMode {
    TEXT = 0,         // display text only, ignore case
    DEEP              // object name, val, description, members, etc.
  };
  
  iTreeView*            tree_view; // pointer to the tree view that we operate on
  
  QToolBar*             srch_bar;
  iLineEdit*            srch_text;
  iLineEdit*            repl_text;  // this text replaces srch_text in replace mode
  QLabel*               srch_nfound;
  iActionMenuButton*    srch_mode_button;
  QMenu*                srch_mode_menu;
  QAction*              srch_clear;
  QAction*              srch_prev;
  QAction*              srch_next;
  QAction*              repl_next;  // replace the current selection and move to next selection
  QAction*              repl_text_action; // this is the iLineEdit for entering the replace text
  QAction*              srch_nfound_action; // we need the action so we can do an insert just before this label
  QAction*              find_action;
  QAction*              find_deep_action;
  QAction*              replace_action;
  
  enum SearchMode       search_mode;
  
  taBase_RefList        found_items; // full list of items found
  int                   cur_item; // currrent item
  
  void                  Search(iTreeSearch::SearchMode mode);
  // search using either the simple text mode for searching the visible strings in the tree or the deep search that checks object name, val, description, etc. and can handle conjunction of search terms
  void                  unHighlightFound();
  // un-highlight all items
  void                  highlightFound();
  // highlight all found items
  void                  selectCurrent(bool replace = false);
  // select current search item
  bool                  IsMatch(iTreeViewItem* item);
  // is this item in the list of found items?
  
  iTreeSearch(QWidget* parent = NULL);
  iTreeSearch(iTreeView* tree_view_, QWidget* parent = NULL);
  ~iTreeSearch();
  
  public slots:
  void                  srch_text_entered();
  void                  srch_clear_clicked();
  void                  treeview_to_updt();
  void                  srch_next_clicked();
  void                  repl_next_clicked();
  void                  srch_prev_clicked();
  void                  TextFindSelected();
  void                  TextReplaceSelected();
  void                  DeepFindSelected();
  
private:
  void Constr();                // construct widget
};

#endif // iTreeSearch_h
