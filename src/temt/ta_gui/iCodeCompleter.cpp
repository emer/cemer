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

#include "iCodeCompleter.h"

#include <QStringList>
#include <QStringListModel>
#include <QCoreApplication>
#include <QKeyEvent>

#include <MemberDef>
#include <MethodDef>
#include <EnumDef>
#include <DataTable>
#include <Program>

#include <taMisc>
#include <taiMisc>
#include <taBase>
#include <String_Array>
#include <Completions>

taTypeDef_Of(Function);

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

  list_model = new iCodeCompleterModel(string_list, NULL, this);
  setModel(list_model);
  host_type = INLINE_HOST;
  field_type = EXPRESSION;
}

void iCodeCompleter::SetModelList(String_Array *list) {
  if (list) {
    string_list.clear();
    list->ToQStringList(string_list);
    list_model->setStringList(string_list);
   }
}

void iCodeCompleter::SetCompletions(Completions* completions) {
  if (completions) {
    string_list.clear();
    QStringList str_list;
    completions->string_completions.ToQStringList(str_list);
    for (int i=0; i<completions->object_completions.size; i++) {
      taBase* base = completions->object_completions.FastEl(i);
      if (base && (base->InheritsFrom(&TA_Function) || base->InheritsFrom(&TA_Program))) {
        String name_plus = base->GetName()  + "()";
        string_list.append(name_plus);
      }
      else {
        string_list.append(base->GetName());
      }
    }
    
    // Members
    for (int i=0; i<completions->member_completions.size; i++) {
      MemberDef* member_def = completions->member_completions.FastEl(i);
      string_list.append(member_def->name);
    }
    
    //  Methods
    for (int i=0; i<completions->method_completions.size; i++) {
      MethodDef* method_def = completions->method_completions.FastEl(i);
      String name_plus = method_def->name  + "()";
      string_list.append(name_plus);
    }
    
    string_list.append(str_list);

    //  Enums
    for (int i=0; i<completions->enum_completions.size; i++) {
      EnumDef* enum_def = completions->enum_completions.FastEl(i);
      string_list.append(enum_def->name);
    }

    //  Statics
    for (int i=0; i<completions->static_completions.size; i++) {
      TypeDef* static_def = completions->static_completions.FastEl(i);
      string_list.append(static_def->name + "::");
    }

    //  Miscellaneous classes - 
    for (int i=0; i<completions->misc_completions.size; i++) {
      TypeDef* scope_def = completions->misc_completions.FastEl(i);
      string_list.append(scope_def->name + "::");
    }

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
  
  if (host_type == TEXT_EDIT_HOST) {
    if (key_event->key() == Qt::Key_Tab) {
      if (GetList()->size() == 1) {
        QCoreApplication* app = QCoreApplication::instance();
        app->postEvent(popup(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier)); // only one item in list
        app->postEvent(popup(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier));  // select
        return true;
      }
      return inherited::eventFilter(obj, event);
    }
    if (key_event->key() == Qt::Key_Return) {
      return inherited::eventFilter(obj, event);
    }
    last_epression_text = GetText();
    return inherited::eventFilter(obj, event);
  }
  else {
    if (key_event->key() == Qt::Key_Tab) {
//      if (field_type == EXPRESSION && GetText() == last_epression_text) {
//        return inherited::eventFilter(obj, event);
//      }
//      last_epression_text = GetText();
      QCoreApplication* app = QCoreApplication::instance();
      if (popup()->isVisible()) {
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
      }
      else {  // no popup in sight
        return inherited::eventFilter(obj, event);
      }
      return true;
    }
    else {
      return inherited::eventFilter(obj, event);
    }
  }
}

void iCodeCompleter::FilterList(String seed) {
  for (int i = string_list.size() - 1; i >= 0; i--) {
    String item = string_list.at(i);
    item = GetPretext() + item;  //
    if (!item.startsWithCI(seed)) {
      string_list.removeAt(i);
    }
  }
}

void iCodeCompleter::ExtendSeed(String& seed) {
  if (string_list.size() <= 1) return;
  
  String pretext = GetPretext(); // strip off the pretext (restore at end)
  seed = seed.after(pretext);
  
  bool keep_trying = true;
  String first_item = (String)string_list.at(0);
  String extended_seed;
  // get next char of first item and see if all the other items have the same next char
  while (keep_trying) {
    if (first_item.length() <= seed.length()) { // ran out of chars
      keep_trying = false;
      break;
    }
    extended_seed = first_item.through(seed.length());
    for (int i = string_list.size() - 1; i >= 0; i--) {
      String item = string_list.at(i);
      if (!item.startsWithCI(extended_seed)) {
        keep_trying = false;
        break;
      }
    }
    if (keep_trying) {
      seed = extended_seed;
    }
  }
  seed = pretext + seed;
}

String iCodeCompleter::GetPretext() {
  String pretext;
  if (field_type == EXPRESSION) {
    pretext = ProgExprBase::completion_text_before;
    pretext = pretext.before(pretext.length() - ProgExprBase::completion_lookup_seed.length());
  }
  return pretext;
}

String iCodeCompleter::GetText() {
  return ProgExprBase::completion_text_before;
}

bool iCodeCompleter::ExpressionTakesArgs(String expression) {
  return ProgExprBase::ExpressionTakesArgs(expression);
}

bool iCodeCompleter::HasSelected() {
  return (popup()->currentIndex().row() != -1);
}

//// class iCompleterPopupView
//
//bool iCompleterPopupView::eventFilter(QObject* obj, QEvent* event) {
//  if (event->type() == QEvent::ShortcutOverride) {
//    if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_N || static_cast<QKeyEvent*>(event)->key() == Qt::Key_P) {
//      event->accept();
//      return true;
//    }
//  }
//  
//  if (event->type() != QEvent::KeyPress) {
//    return inherited::eventFilter(obj, event);
//  }
//  QKeyEvent* key_event = static_cast<QKeyEvent *>(event);
//  if (key_event->key() == Qt::Key_Tab) {
//    return true;
//  }
//
//  else {
//    return inherited::eventFilter(obj, event);
//  }
//}

// class iCodeCompleterModel

iCodeCompleterModel::iCodeCompleterModel(QObject* parent, iCodeCompleter* cc) :
 inherited::QStringListModel(parent)
{
  completer = cc;
}

iCodeCompleterModel::iCodeCompleterModel(const QStringList &strings, QObject *parent, iCodeCompleter* cc) :
  inherited::QStringListModel(strings, parent)
{
  completer = cc;
}

QVariant iCodeCompleterModel::data(const QModelIndex& index, int role) const {
  switch (role) {
    case Qt::DisplayRole:
      return inherited::data(index, role);
      
    case Qt::EditRole: { // this is the default role used for completion matching
      QVariant temp = inherited::data(index, role);
      String pretext;
      String append_text;
      if (completer->field_type == iCodeCompleter::EXPRESSION) {
        pretext = ProgExprBase::completion_text_before;
        pretext = pretext.before(pretext.length() - ProgExprBase::completion_lookup_seed.length());
        append_text = ProgExprBase::completion_append_text;
      }
      String complete = pretext + temp.toString() + append_text;
      return complete;
    }
      
    default:
      break;
  }
  return QVariant();
}


