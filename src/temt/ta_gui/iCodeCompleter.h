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

class Completions;
class iCodeCompleterModel;

// declare all other types mentioned but not required to include:

class TA_API iCodeCompleter : public QCompleter {
  // Creates a list of code completions that make semantic sense - uses lookup logic to generate the list of options
  Q_OBJECT
INHERITED(QCompleter)
  friend class iCodeCompleterModel;
public:
  
  enum FieldType {
    EXPRESSION,         // all code
    SIMPLE,             // fields for single items, e.g. column name
  };
  
  enum HostType {
    DIALOG_HOST,        // the field is a single line field in a dialog using an iLineEdit
    INLINE_HOST,        // the field is an inline edit tree item using an iLineEdit
    TEXT_EDIT_HOST,     // the field is a dialog with an iTextEdit
  };
  
  iCodeCompleter(QObject* parent = 0);
  iCodeCompleter(QAbstractItemModel *model, QObject *parent = 0);

  iCodeCompleterModel*    list_model;
  String                  last_epression_text;  // hold onto for comparison on next tab
  FieldType               field_type;
  HostType                host_type;
  String                  chooser_selection;  // if chooser used instead of popup store the selection in here

  void                    SetModelList(String_Array* list);
  void                    SetCompletions(Completions* completions);
  QStringList*            GetList() { return &string_list; }
  String                  GetCurrent();             // return the current selection
  void                    ClearList();              // clear the model list
  void                    FilterList(String seed);  // pair down based on seed
  void                    ExtendSeed(String& seed); // extend the seed if possible -- called prefix in QCompleter terminology
  void                    SetIsDialogField(bool dialog_field) { is_dialog_field = dialog_field; }
  bool                    ExpressionTakesArgs(String expression); // call ProgExprBase to know if method takes arguments
  void                    SetHostType(HostType type) { host_type = type; }
  void                    SetFieldType(FieldType type) { field_type = type; }
  bool                    HasSelected();             // is one of the completion choices highlighted
  int                     GetListCount() { return GetList()->size(); }
  bool                    PopUpIsVisible() { return popup()->isVisible(); }
  void                    DialogClosing() { popup()->hide(); }  // dialogs should call this when deleting
  String                  GetPreText();  // get the pretext from the expression parser
  String                  GetAppendText() { return append_text; }  // get the append text from the expression parser
  String                  GetText();     // get the text before cursor from the expression parser

protected:
  QStringList             string_list;
  bool                    eventFilter(QObject* obj, QEvent* event) override;
  bool                    is_dialog_field; // is the widget using this completer in a dialog - if so special tab handling
  String                  pre_text;      // text before completion
  String                  append_text;   // text following completion
  
private:
  void                    Init();
  
public slots:

};


//class TA_API iCompleterPopupView: public QListView {
//  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBER table editor; model flattens >2d into 2d by frames
//  INHERITED(QListView)
//  Q_OBJECT
//public:
//  
//protected:
//  bool                    eventFilter(QObject* obj, QEvent* event) override;
//};

class TA_API iCodeCompleterModel: public QStringListModel {
  // #NO_INSTANCE #NO_CSS #NO_MEMBERS class that implements the Qt Model interface for CodeCompleter so we can display and match with separate roles
  INHERITED(QStringListModel)
  Q_OBJECT
public:
  iCodeCompleterModel(QObject *parent = 0, iCodeCompleter* cc = NULL);
  iCodeCompleterModel(const QStringList &strings, QObject *parent = 0, iCodeCompleter* cc = NULL);
  
  QVariant data(const QModelIndex &index, int role) const override;
  
protected:
  iCodeCompleter* completer;
};


#endif // iCodeCompleter_h
