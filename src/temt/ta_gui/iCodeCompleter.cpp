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

#include "iCodeCompleter.h"

#include <QStringList>
#include <QStringListModel>
#include <QCoreApplication>
#include <QKeyEvent>

#include <MemberDef>
#include <DataTable>
#include <Program>

#include <taMisc>
#include <taiMisc>
#include <taBase>
#include <String_Array>

iCodeCompleter::iCodeCompleter(QObject* parent) {
  Init();
}

iCodeCompleter::iCodeCompleter(QAbstractItemModel *model, QObject *parent) {
  Init();
}

void iCodeCompleter::Init() {
#if (QT_VERSION >= 0x040600)
  setMaxVisibleItems(taMisc::code_completion.max_choices);
#endif
  //  iCompleterPopupView* completer_popup = new iCompleterPopupView();
  //  setPopup(completer_popup);

  list_model = new iCodeCompleterModel(string_list, NULL);
  setModel(list_model);
  
  //  setModelSorting(QCompleter::CaseInsensitivelySortedModel);
}

void iCodeCompleter::SetModelList(String_Array *list) {
  if (list) {
    string_list.clear();
    list->ToQStringList(string_list);
    list_model->setStringList(string_list);
   }
}

String iCodeCompleter::GetCurrent() {
  return currentCompletion();
}

void iCodeCompleter::ClearList() {
  string_list.clear();
  list_model->setStringList(string_list);
}


bool iCodeCompleter::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }
  QKeyEvent* key_event = static_cast<QKeyEvent *>(event);
  if (key_event->key() == Qt::Key_Tab) {
    QCoreApplication* app = QCoreApplication::instance();
    if (event->type() == QEvent::KeyPress && popup()->currentIndex().row() != -1) {  // some item is highlighted
      app->postEvent(popup(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier));  // select
    }
    else if (GetList()->size() == 1) {
      app->postEvent(popup(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier)); // only one item in list
      app->postEvent(popup(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier));  // select
    }
    else {
      app->postEvent(popup(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));  // try extending
    }
    return true;
  }
  else {
    return inherited::eventFilter(obj, event);
  }
}

void iCodeCompleter::FilterList(String seed) {
  for (int i = string_list.size() - 1; i >= 0; i--) {
    String item = string_list.at(i);
    String item_copy = item;
    item_copy.downcase();
    if (!item_copy.startsWith(seed)) {
      string_list.removeAt(i);
    }
  }
}

void iCodeCompleter::ExtendSeed(String& seed) {
  if (string_list.size() <= 1) return;
  
  bool keep_trying = true;
  String first_item = (String)string_list.at(0);
  String extended_seed;
  // get next char of first item and see if all the other items have the same next char
  while (keep_trying) {
    extended_seed = first_item.through(seed.length());
    String extended_seed_copy = extended_seed;
    extended_seed_copy.downcase();
    for (int i = string_list.size() - 1; i >= 0; i--) {
      String item = string_list.at(i);
      String item_copy = item;
      item_copy.downcase();
      if (!item_copy.startsWith(extended_seed_copy)) {
        keep_trying = false;
        break;
      }
    }
    if (keep_trying) {
      seed = extended_seed;
    }
  }
}

bool iCompleterPopupView::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::ShortcutOverride) {
    if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_N || static_cast<QKeyEvent*>(event)->key() == Qt::Key_P) {
      event->accept();
      return true;
    }
  }
  
  if (event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }
  QKeyEvent* key_event = static_cast<QKeyEvent *>(event);
  if (key_event->key() == Qt::Key_Tab) {
    return true;
  }

  else {
    return inherited::eventFilter(obj, event);
  }
}

iCodeCompleterModel::iCodeCompleterModel(QObject* parent) :
 inherited::QStringListModel(parent)
{
}

iCodeCompleterModel::iCodeCompleterModel(const QStringList &strings, QObject *parent) :
  inherited::QStringListModel(strings, parent)
{
}

QVariant iCodeCompleterModel::data(const QModelIndex& index, int role) const {
  switch (role) {
    case Qt::DisplayRole:
      return inherited::data(index, role);
      
    case Qt::EditRole: { // this is the default role used for completion matching
      QVariant temp = inherited::data(index, role);
      String pretext;
      pretext = ProgExprBase::completion_text_before;
      pretext = pretext.before(pretext.length() - ProgExprBase::completion_lookup_seed.length());
      String complete = pretext + temp.toString() + ProgExprBase::completion_append_text;
      return complete;
    }
      
    default:
      break;
  }
  return QVariant();
}


