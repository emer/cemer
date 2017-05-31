// Copyright 2017, Regents of the University of Colorado,
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

#ifndef iDialogObjDiffBrowser_h
#define iDialogObjDiffBrowser_h 1

// parent includes:
#include "ta_def.h"
#include <iDialog>

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class ObjDiff; //
class ObjDiffRec; //
class iDialogObjDiffFilter; //
class QVBoxLayout; //
class QTreeWidget; //
class QTreeWidgetItem; //
class QPushButton; //
class QBrush; //
class QCheckBox; //

class TA_API iDialogObjDiffBrowser : public iDialog {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS select items from a list, much like a file chooser; can be tokens from typedef or items on a list
INHERITED(iDialog)
  Q_OBJECT
public:
  enum ODBCols {
    COL_NEST,
    COL_NDIFF,
    COL_A_FLG,
    COL_A_NM,
    COL_A_VAL,
    COL_A_VIEW,
    COL_SEP,
    COL_B_FLG,
    COL_B_NM,
    COL_B_VAL,
    COL_B_VIEW,
    COL_N,
  };

  static iDialogObjDiffBrowser* New(ObjDiff* diffs,
                                int font_type, QWidget* par_window_ = NULL);

  String                caption;        // current caption at top of chooser
  ObjDiff*              odl;            // diffs list -- we own this list and will delete it when done
  iDialogObjDiffFilter* filter_dlg;

  QVBoxLayout*          layOuter;
  QTreeWidget*            items;        // list of items
  QPushButton*              btnAllA;
  QPushButton*              btnFiltA;
  QCheckBox*                chkApplyA;
  QPushButton*              btnAllB;
  QPushButton*              btnFiltB;
  QCheckBox*                chkApplyB;
  QPushButton*              btnOk;
  QPushButton*              btnCancel;
  QBrush*                   add_color;
  QBrush*                   del_color;
  QBrush*                   chg_color;
  QBrush*                   add_color_lt;
  QBrush*                   del_color_lt;
  QBrush*                   chg_color_lt;
  QBrush*                   off_color;
  QBrush*                   ctxt_color;
  QBrush*                   ctxt_color_lt;

  virtual bool  Browse(bool modal);
  // main user interface: this actually puts up the dialog -- if non-modal, will return immediately with true, otherwise returns after user selects ok or not (returns true if OK)

  virtual void  Constr();
  virtual void  AddItems();
  virtual void  ToggleAll(int a_or_b);
  // toggle checked state of all items -- a_or_b = 0 for A, 1 for B
  virtual void  SetFiltered(int a_or_b, bool on, bool add, bool del, bool chg,
                            bool nm_not, String nm_contains, bool val_not,
                            String val_contains);
  // toggle checked state of all items -- a_or_b = 0 for A, 1 for B filtered by given filter settings
  virtual void  UpdateItemDisp(QTreeWidgetItem* witm, ObjDiffRec* rec, int a_or_b);
  // update display of item

  virtual void  ViewItem(ObjDiffRec* odr, int a_or_b);

  iDialogObjDiffBrowser(const String& captn, QWidget* par_window_);
  ~iDialogObjDiffBrowser();
protected slots:
  void         accept() override;
  void         reject() override;
  virtual  void         toggleAllA();
  virtual  void         setFilteredA();
  virtual  void         toggleAllB();
  virtual  void         setFilteredB();
  virtual  void         applyAclick();
  virtual  void         applyBclick();
  void                  itemClicked(QTreeWidgetItem* itm, int column);
};

#endif // iDialogObjDiffBrowser_h
