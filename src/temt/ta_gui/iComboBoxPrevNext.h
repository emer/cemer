// Copyright 2017-2017, Regents of the University of Colorado,
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

#ifndef iComboBoxPrevNext_h
#define iComboBoxPrevNext_h 1

// parent includes:
#include <iComboBox>
#include <QWidget>

// member includes:

// declare all other types mentioned but not required to include:
class QToolButton;
class QHBoxLayout;
class QVBoxLayout;

class TA_API iComboBoxPrevNext : public QWidget {
  // #IGNORE a ComboBox with previous / next arrows to quickly scroll through options without having to do the menu selection process
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
  Q_OBJECT
public:
  QHBoxLayout*		lay;
  iComboBox*            combo_box;
  QVBoxLayout*		vlay;
  QToolButton*          prev_button;
  QToolButton*          next_button;

  iComboBoxPrevNext(QWidget* parent = NULL);
  ~iComboBoxPrevNext();                                         
                                   
public slots:
  virtual void  PrevItem();     // move combo box selection to previous item
  virtual void  NextItem();     // move combo box selection to next item


public:
  // pass-through the ComboBox API, to make it drop-in replacement
  bool isEditable() const { return combo_box->isEditable(); }
  void setEditable(bool eb) { combo_box->setEditable(eb); }
  int count()   { return combo_box->count(); }
  QString currentText() const { return combo_box->currentText(); }
  int currentIndex() const { return combo_box->currentIndex(); }
  int maxVisibleItems() const { return combo_box->maxVisibleItems(); }
  void setMaxVisibleItems(int mx) { combo_box->setMaxVisibleItems(mx); }
  int maxCount() const { return combo_box->maxCount(); }
  void setMaxCount(int mx) { combo_box->setMaxCount(mx); }
  QComboBox::InsertPolicy insertPolicy() const { return combo_box->insertPolicy(); }
  void setInsertPolicy(QComboBox::InsertPolicy ip) { combo_box->setInsertPolicy(ip); }
  QComboBox::SizeAdjustPolicy sizeAdjustPolicy() const { return combo_box->sizeAdjustPolicy(); }
  void setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy sp) { combo_box->setSizeAdjustPolicy(sp); }
  int minimumContentsLength() const { return combo_box->minimumContentsLength(); }
  void setMinimumContentsLength(int mc) { combo_box->setMinimumContentsLength(mc); }
  QSize iconSize() const { return combo_box->iconSize(); }
  void setIconSize(QSize sz) { combo_box->setIconSize(sz); }

  bool duplicatesEnabled() const { return combo_box->duplicatesEnabled(); }
  void setDuplicatesEnabled(bool de) { combo_box->setDuplicatesEnabled(de); }
  bool hasFrame() const { return combo_box->hasFrame(); }
  void setFrame(bool hf) { combo_box->setFrame(hf); }
  int modelColumn() const { return combo_box->modelColumn(); }
  void setModelColumn(int mc) { combo_box->setModelColumn(mc); }

  void setLineEdit(QLineEdit *edit) { combo_box->setLineEdit(edit); }
  QLineEdit *lineEdit() const { return combo_box->lineEdit(); }

#ifndef QT_NO_VALIDATOR
  void setValidator(const QValidator *v) { combo_box->setValidator(v); }
  const QValidator *validator() const { return combo_box->validator(); }
#endif

#ifndef QT_NO_COMPLETER
  void setCompleter(QCompleter *c) { combo_box->setCompleter(c); }
  QCompleter *completer() const { return combo_box->completer(); }
#endif

  QAbstractItemDelegate *itemDelegate() const { return combo_box->itemDelegate(); }
  void setItemDelegate(QAbstractItemDelegate *delegate) { combo_box->setItemDelegate(delegate); }

  QAbstractItemModel *model() const { return combo_box->model(); }
  void setModel(QAbstractItemModel *model) { combo_box->setModel(model); };

  QModelIndex rootModelIndex() const { return combo_box->rootModelIndex(); };
  void setRootModelIndex(const QModelIndex &index) { combo_box->setRootModelIndex(index); };

  QString itemText(int index) const { return combo_box->itemText(index); }
  QIcon itemIcon(int index) const { return combo_box->itemIcon(index); }
  QVariant itemData(int index, int role = Qt::UserRole) const { return combo_box->itemData(index, role); }

  void addItem(const QString &text, const QVariant &userData = QVariant())
  { combo_box->addItem(text, userData); }
  void addItem(const QIcon &icon, const QString &text, const QVariant &userData = QVariant()) { combo_box->addItem(icon, text, userData); }
  void addItems(const QStringList &texts) { combo_box->addItems(texts); }

  void addColor(const QString& color) { combo_box->addColor(color); }
  
  void insertItem(int index, const QString &text, const QVariant &userData = QVariant())
  { combo_box->insertItem(index, text, userData); }
  void insertItem(int index, const QIcon &icon, const QString &text,
    const QVariant &userData = QVariant())
  { combo_box->insertItem(index, icon, text, userData); }
  void insertItems(int index, const QStringList &texts) { combo_box->insertItems(index, texts); }
  void insertSeparator(int index) { combo_box->insertSeparator(index); }

  void removeItem(int index) { combo_box->removeItem(index); }

  void setItemText(int index, const QString &text) { combo_box->setItemText(index, text); }
  void setItemIcon(int index, const QIcon &icon) { combo_box->setItemIcon(index, icon); }
  void setItemData(int index, const QVariant &value, int role = Qt::UserRole)
  { combo_box->setItemData(index, value, role); }

  QAbstractItemView *view() const  { return combo_box->view(); }
  void setView(QAbstractItemView *itemView) { combo_box->setView(itemView); }

  int findText(const QString &text, Qt::MatchFlags flags =
               static_cast<Qt::MatchFlags>(Qt::MatchExactly|Qt::MatchCaseSensitive)) const
  { return combo_box->findText(text, flags); }
  int findData(const QVariant &data, int role = Qt::UserRole,
               Qt::MatchFlags flags =
               static_cast<Qt::MatchFlags>(Qt::MatchExactly|Qt::MatchCaseSensitive)) const
  { return combo_box->findData(data, role, flags); }

  void showPopup() { combo_box->showPopup(); }
  void hidePopup() { combo_box->hidePopup(); }

 public Q_SLOTS:
  void clear() { combo_box->clear(); }
  void clearEditText() { combo_box->clearEditText(); }
  void setEditText(const QString &text) { combo_box->setEditText(text); }
  void setCurrentIndex(int index) { combo_box->setCurrentIndex(index); }
  void updateSize() { combo_box->updateSize(); }

 Q_SIGNALS:
  void editTextChanged(const QString &);
  void activated(int index);
  void activated(const QString &);
  void highlighted(int index);
  void highlighted(const QString &);
  void currentIndexChanged(int index);
  void currentIndexChanged(const QString &);
  void currentTextChanged(const QString &);
};

#endif // iComboBoxPrevNext_h
