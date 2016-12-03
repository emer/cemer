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
#include <String_Array>

iCodeCompleter::iCodeCompleter(QObject* parent) {
  Init();
}

iCodeCompleter::iCodeCompleter(QAbstractItemModel *model, QObject *parent) {
  Init();
}

void iCodeCompleter::Init() {
  setMaxVisibleItems(10);
  list_model = new QStringListModel(string_list, NULL);
  this->setModel(list_model);
}

void iCodeCompleter::SetModelList(String_Array *list) {
  string_list.clear();
  list->ToQStringList(string_list);
  list_model->setStringList(string_list);
}

String iCodeCompleter::GetCurrent() {
  return currentCompletion();
}