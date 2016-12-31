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
  QKeyEvent* key_event = static_cast<QKeyEvent *>(event);
  if (key_event->key() == Qt::Key_Tab) {
    QCoreApplication* app = QCoreApplication::instance();
    if (event->type() == QEvent::KeyPress && currentRow() == 0) { // select first popup item
      app->postEvent(popup(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
    }
    app->postEvent(popup(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier));
    return true;
  }
  else {
    return inherited::eventFilter(obj, event);
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
      
    case Qt::EditRole: {
      QVariant temp = inherited::data(index, role);
      String pretext;
      if (ProgExprBase::completion_path_pre_text.empty()) {
        pretext = ProgExprBase::completion_pre_text;
      }
      else {
        pretext = ProgExprBase::completion_path_pre_text;
        if (ProgExprBase::completion_pre_text.nonempty()) { // this fixes bug 2938
          pretext = pretext.before(pretext.length() - ProgExprBase::completion_lookup_seed.length());
        }
      }
      return pretext + temp.toString() + ProgExprBase::completion_append_text;
    }
      
    default:
      break;
  }
  return QVariant();
}


