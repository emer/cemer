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

#ifndef iDialogRegexp_h
#define iDialogRegexp_h 1

// parent includes:
#include "ta_def.h"
#include <iDialog>

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class taiRegexpField; //
class iDialogRegexpPopulator; //
class QStandardItem; //
class QDialogButtonBox; //
class QVBoxLayout; // 
class QHBoxLayout; // 
class QComboBox; //
class QListWidget; //
class QListWidgetItem; //
class QStandardItemModel; //
class QTableView; //
class QSortFilterProxyModel; //
class QLineEdit; //

class TA_API iDialogRegexp : public iDialog {
  Q_OBJECT
  INHERITED(iDialog)
public:
    iDialogRegexp(taiRegexpField* regexp_field, const String& field_name, iDialogRegexpPopulator *re_populator, const void *fieldOwner, bool read_only,
                  bool editor_mode = false);

  bool          isReadOnly()    { return m_read_only; }
  bool          applyClicked()  { return m_apply_clicked; }

  String        field_value;    // when used as a standalone dialog, this is the field return value

public slots:
  override void accept();

protected slots:
  virtual void          btnAdd_clicked();
  virtual void          btnDel_clicked();
  virtual void          btnApply_clicked();
  virtual void          btnReset_clicked();

  virtual void          RegexpPartChosen(int index);
  virtual void          RegexpPartEdited();
  virtual void          RegexpLineEdited();
  virtual void          RegexpSelectionChanged();

  virtual void          TableItemChanged(QStandardItem* item);

protected:
  enum ExtraColumns {
    INDEX_COL,          // One extra column for the label indices.
    LABEL_COL,          // One extra column for the full labels (hidden).
    NUM_EXTRA_COLS      // Number of extra columns.
  };

  // Helpers functions called by the ctor.
  virtual void          CreateTableModel();
  virtual void          LayoutInstructions(QVBoxLayout *vbox, QString field_name);
  virtual void          LayoutRegexpList(QVBoxLayout *vbox);
  virtual QHBoxLayout * LayoutEditBoxes(QVBoxLayout *vbox);
  virtual void          LayoutTableView(QVBoxLayout *vbox, QHBoxLayout *hbox_combos);
  virtual void          LayoutButtons(QVBoxLayout *vbox);

  // Keeps combo-box line-edit in sync with the selected entry.
  virtual void          SelectCombo(QComboBox *combo, int index);

  // Helpers for RegexpSelectionChanged().
  virtual void          BuildCombos(QString regexp);
  virtual QStringList   GetComboChoices(QString regexp, int part);
  virtual void          EnableEditBoxes(QString regexp);
#ifndef __MAKETA__
  virtual void          ApplyFilters(QList<QListWidgetItem *> selected_items);
#endif

  // Split up and join regexp alternatives.
  virtual QStringList   SplitRegexpAlternatives(QString regexp);
#ifndef __MAKETA__
  virtual QString       JoinRegexpAlternatives(QList<QListWidgetItem *> items);
#endif

  // Split up and join regexp parts.
  virtual QStringList   SplitRegexp(const QString &regexp);
  virtual QString       JoinRegexp(const QStringList &regexp_parts);
  virtual QString       GetEscapedSeparator();

  // Enable/disable the apply/reset buttons.
  virtual void          setApplyEnabled(bool enabled);

  override void         keyPressEvent(QKeyEvent *e);

// Data members
protected:
  static const QString DOT_STAR;

  taiRegexpField*       m_field;
  iDialogRegexpPopulator*      m_populator;
  const void *          m_fieldOwner;
  bool                  m_read_only;
  bool                  m_editor_mode;
  bool                  m_apply_clicked;
  QStandardItemModel*   m_table_model;
  QTableView*           m_table_view;
  int                   m_num_parts;
  int                   m_extra_cols;
  QSortFilterProxyModel* m_proxy_model;
  QListWidget*          m_regexp_list;
  QLineEdit*            m_regexp_line_edit;
#ifndef __MAKETA__
  QList<QComboBox *>    m_regexp_combos;
#endif

  QPushButton*          btnAdd;
  QPushButton*          btnDel;
  QPushButton*          btnApply;
  QPushButton*          btnReset;
  QDialogButtonBox*     m_button_box;
};

#endif // iDialogRegexp_h
