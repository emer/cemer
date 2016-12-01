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

#include <MemberDef>
#include <DataTable>
#include <Program>

#include <taMisc>
#include <taBase>

iCodeCompleter::iCodeCompleter(QObject* parent) {
  init();
}

iCodeCompleter::iCodeCompleter(QAbstractItemModel *model, QObject *parent) {
  init();
}

void iCodeCompleter::init() {
  base_list = NULL;
  setMaxVisibleItems(10);
  list_model = new QStringListModel(string_list, NULL);
  this->setModel(list_model);
}

void iCodeCompleter::SetModelList(taBase_List* list) {
  base_list = list;
  string_list.clear();
  for (int i=0; i<list->size; i++) {
    taBase* base = list->FastEl(i);
    string_list.append(base->GetName());
  }
  list_model->setStringList(string_list);
}

taBase* iCodeCompleter::GetToken() {
  int index = -1;
  for (int i=0; i<base_list->size; i++) {
    taBase* btmp = base_list->SafeEl(i);
    if (btmp->GetName() == currentCompletion()) {
      return base_list->SafeEl(i);
    }
  }
  return NULL;
}