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
#include <QStringList>
#include <QStringListModel>

#include <QListView>

// member includes:
#include <taBase_List>

// declare all other types mentioned but not required to include:

class TA_API iCodeCompleter : public QCompleter {
  // Creates a list of code completions that make semantic sense - uses lookup logic to generate the list of options
  Q_OBJECT
INHERITED(QCompleter)
public:
  iCodeCompleter(QObject* parent = 0);
  iCodeCompleter(QAbstractItemModel *model, QObject *parent = 0);

  QStringListModel*       list_model;

  void                    SetModelList(String_Array* list);
  QStringList*            GetList() { return &string_list; }
  String                  GetCurrent();  // return the current selection
  void                    ClearList();   // clear the model list
  
protected:
  QStringList             string_list;
  bool                    eventFilter(QObject* obj, QEvent* event) override;

private:
  void                    Init();
  
public slots:

};


class TA_API iCompleterPopupView: public QListView {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBER table editor; model flattens >2d into 2d by frames
  INHERITED(QListView)
  Q_OBJECT
public:
  
protected:
  bool                    eventFilter(QObject* obj, QEvent* event) override;
};


#endif // iCodeCompleter_h
