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

#ifndef iDataTableSearch_h
#define iDataTableSearch_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QWidget>
#endif

// member includes:
#ifndef __MAKETA__
#include <QList>
#endif

#include <taVector2i_List>

// declare all other types mentioned but not required to include:
class iLineEdit; //
class iDataTableView; //
class iDataTableModel; //
class QToolBar; //
class QLabel; //
class iActionMenuButton; //
class iMenuButton; //
class QMenu; //


class TA_API iDataTableSearch : public QWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS search widget for a DataTable view -- controls searching in this view

  Q_OBJECT
  friend class iDataTableView;

public:
  enum SearchMode {
    CONTAINS = 0,       // if string is a substring of value
    MATCHES,            // string same as value but case insensitive
  };
  
  iDataTableView*       table_view; // DataTable view for the table we search
  iDataTableModel*      table_model; // table model holds list of found items

  QToolBar*             srch_bar;
  iLineEdit*            srch_text;
  QLabel*               srch_nfound;
  QAction*              srch_clear;
  QAction*              srch_prev;
  QAction*              srch_next;
  QMenu*                srch_mode_menu;
  iActionMenuButton*    srch_mode_button;

  QAction*              contains_action;
  QAction*              matches_action;
  enum SearchMode       search_mode;

  void                  Search(iDataTableSearch::SearchMode mode);
  // all searches, partial or exact, are case insensitive
  
  iDataTableSearch(QWidget* parent = NULL);
  iDataTableSearch(iDataTableView* table_view_, QWidget* parent = NULL);
  ~iDataTableSearch();

public slots:
  void                  TextEntered();
  void                  SelectNext();
  void                  SelectPrevious();
  void                  SearchClear();
  void                  ContainsSelected();
  void                  MatchesSelected();

protected:
  taVector2i_List*      found_list;  // pass this to data table so it doesn't need to include any Qt code
  int                   cur_item; // currrent item
  
private:
  void Constr();                // construct widget
};

#endif // iDataTableSearch_h
