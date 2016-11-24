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

iCodeCompleter::iCodeCompleter(QObject* parent) {
  init();
}

iCodeCompleter::iCodeCompleter(QAbstractItemModel *model, QObject *parent) {
  init();
}

void iCodeCompleter::init() {
  base_list.Reset();
  GetTokens(&TA_DataTable);

  for (int i=0; i<base_list.size; i++) {
    taBase* base = base_list.FastEl(i);
    string_list.append(base->GetName());
  }
  list_model = new QStringListModel(string_list, NULL);

  this->setModel(list_model);
}

void iCodeCompleter::GetTokens(TypeDef* td) {
  for(int i=0; i<td->tokens.size; i++) {
    taBase* btmp = (taBase*)td->tokens.FastEl(i);
    if(!btmp)
      continue;
    taBase* parent = btmp->GetParent();
    // keeps templates out of the list of actual instances
    if (btmp->GetPath().startsWith(".templates")) {
      continue;
    }
    // keeps templates out of the list of actual instances
    if (!parent)
      continue;

    // added to keep cluster run data tables from showing in chooser but perhaps otherwise useful
    taBase* owner = btmp->GetOwner();
    if (owner) {
      MemberDef* md = owner->FindMemberName(btmp->GetName());
      if (md && md->HasOption("HIDDEN_CHOOSER"))
        continue;
    }
    //    if ((bool)scope_ref && !btmp->SameScope(scope_ref, scope_typ))
    //      continue;
    //    if (!ShowToken(btmp)) continue;

    base_list.Link(btmp);
  }
}

void iCodeCompleter::setCompletionPrefix(const QString &prefix) {
  taMisc::DebugInfo(prefix);
  
  base_list.RemoveAll();
  string_list.clear();

  // just a quick and dirty way to test out changing model contents
  // as the user types
  // Real task is to hook this to the lookup code and build the lists
  // from that existing logic.
  if (prefix.length() > 2) {
    GetTokens(&TA_Program);
  }
  else {
    GetTokens(&TA_DataTable);
  }
  
  for (int i=0; i<base_list.size; i++) {
    taBase* base = base_list.FastEl(i);
    string_list.append(base->GetName());
  }
  list_model->setStringList(string_list);
}


