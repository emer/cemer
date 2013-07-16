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

#ifndef iDialogItemChooser_h
#define iDialogItemChooser_h 1

// parent includes:
#include "ta_def.h"

#ifndef __MAKETA__
#include <iDialog>
#endif

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class taiWidgetItemChooser; //
class QVBoxLayout; // 
class QComboBox; //
class iTreeWidget; //
class QTreeWidgetItem; //
class QLineEdit; //


class TA_API iDialogItemChooser : public iDialog {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS select items from a list, much like a file chooser; can be tokens from typedef or items on a list
  Q_OBJECT
  INHERITED(iDialog)
public:
#ifndef __MAKETA__
  enum Roles { // extra roles, for additional data, etc.
    ObjDataRole = Qt::UserRole + 1,
    ObjCatRole,  // for object category string, whether shown or not
    NewFunRole,  // new function call on object
  };
#endif
  static const String   cat_none; // "none" category
  static int            filt_delay; // delay, in msec, to invoke filter after typing

  static iDialogItemChooser* New(const String& caption, taiWidgetItemChooser* client = NULL,
    int ft = 0, QWidget* par_window_ = NULL);

  String                caption;        // current caption at top of chooser
  bool                  multi_cats;     // each item may have multiple categories, separated by commas
  String                init_filter;    // initial filter string -- prefix with ^ for starts-with

  inline taiWidgetItemChooser* client() const {return m_client;} // only valid in Constr and between Choose...accept/reject
  void*                 selObj() const {return m_selObj;} // current selected object
  void                  setSelObj(void* value, bool force = false);     //

  int                   catFilter() const {return m_cat_filter;}
  void                  setCatFilter(int value, bool force = false);
  int                   view() const {return m_view;}
  void                  setView(int value, bool force = false);

  QVBoxLayout*          layOuter;
  QComboBox*              cmbView;
  QComboBox*              cmbCat; //note: item 0 is "all" (i.e., no filtering)
  iTreeWidget*            items;        // list of items
  QPushButton*              btnOk;
  QPushButton*              btnCancel;
  QLineEdit*              filter;

  virtual bool          Choose(taiWidgetItemChooser* client);
  // main user interface: this actually puts up the dialog, returns true if Ok, false if cancel

  virtual void          Clear();        // reset data
  void                  SetFilter(const QString& filt); // apply a filter
  void                  ClearFilter(); // remove filtering

  virtual QTreeWidgetItem* AddItem(const QString& itm_txt, QTreeWidgetItem* parent,
    const void* data_ = NULL); // add one item to dialog, optionally with data
  virtual QTreeWidgetItem* AddItem(const QString& itm_cat, const QString& itm_txt,
    QTreeWidgetItem* parent, const void* data_ = NULL);
    // add one categorized item to dialog, optionally with data

protected:
  int                   m_changing;
  bool                  m_fully_up;
  void*                 m_selObj;       // current selected object
  QTreeWidgetItem*      m_selItem; // cached for showEvent
  int                   m_view;
  int                   m_cat_filter;
  taiWidgetItemChooser*       m_client; // NOTE: only valid in Constr and between Choose...accept/reject
  QString               last_filter; // for checking if anything changed
  QTimer*               timFilter; // timer for filter changes

  void                  showEvent(QShowEvent* event); //override

  int                   ApplyFiltering();
  virtual void          Refresh();      // rebuild current view
  bool                  SetCurrentItemByData(void* value);
  bool                  SetInitView(void* sel_val, const String& filt_str);
  bool                  ShowItem(const QTreeWidgetItem* item) const;
  virtual void          Constr(taiWidgetItemChooser* client_);
   // does constr, called in static, so can extend
  virtual void          SelectItem(QTreeWidgetItem* itm, bool is_first = false);
  // select the item in items list, by one form of force or another

  iDialogItemChooser(const String& caption, QWidget* par_window_);
protected slots:
  void                  accept(); // override
  void                  reject(); // override
  void                  items_itemDoubleClicked(QTreeWidgetItem* itm, int col);
  void                  cmbView_currentIndexChanged(int index);
  void                  cmbCat_currentIndexChanged(int index);
  void                  filter_textChanged(const QString& text);
  void                  timFilter_timeout();
  void                  show_timeout(); // for scrolling to item
private:
  void          init(const String& captn); // called by constructors
  iDialogItemChooser(const iDialogItemChooser&); //no
  iDialogItemChooser& operator=(const iDialogItemChooser&); //no
};

#endif // iDialogItemChooser_h
