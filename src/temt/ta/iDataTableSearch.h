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

// declare all other types mentioned but not required to include:
class iLineEdit; //
class iDataTableView; //
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
  iDataTableView*            table_view; // pointer to the DataTable view that we operate on

  QLabel*                srch_label;
  QToolBar*             srch_bar;
  iLineEdit*            srch_text;
  QLabel*               srch_nfound;
  QAction*              srch_clear;
  QAction*              srch_prev;
  QAction*              srch_next;
  QMenu*                srch_mode_menu;

  void                  Search();
  
  void                  UnHighlightFound();
  // un-highlight all items
  void                  HighlightFound();
  // highlight all found items

  iDataTableSearch(QWidget* parent = NULL);
  iDataTableSearch(iDataTableView* table_view_, QWidget* parent = NULL);
  ~iDataTableSearch();

protected:
  QList<QModelIndex>    found_items;  // full list of items found
  int                   cur_item; // currrent item
  
  void                  SelectCurrent();
  // select the data table cell that is the current item in the found_items list
  void                  SelectNext();
  void                  SelectPrevious();
  
private:
  void Constr();                // construct widget
};

#endif // iDataTableSearch_h
