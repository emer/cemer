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

#ifndef iCodeCompleter_h
#define iCodeCompleter_h 1

#include "ta_def.h"

// parent includes:
#include <QCompleter>
#include <QStringListModel>

// member includes:
#include <taBase_List>

// declare all other types mentioned but not required to include:

class TA_API iCodeCompleter : public QCompleter {
  // Creates a list of code completions that make semantic sense - uses lookup logic to generate the list of options
  Q_OBJECT
INHERITED(QCompleter)
public:
  iCodeCompleter(QObject* parent = Q_NULLPTR);
  iCodeCompleter(QAbstractItemModel *model, QObject *parent = Q_NULLPTR);

  taBase_List*            base_list;
  QStringListModel*       list_model;

  void                    SetModelList(taBase_List* list);
  QStringList*            GetList() { return &string_list; }
  taBase*                 GetToken();  // return the base_list item that is the current completion

protected:
  QStringList             string_list;

private:
  void                     init();
  
public slots:

};

#endif // iCodeCompleter_h
