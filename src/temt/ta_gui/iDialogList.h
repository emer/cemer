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

#ifndef iDialogList_h
#define iDialogList_h 1

// parent includes:
#include <QDialog>

// member includes:
#include <taString>
#include <String_Array>
#include <taBase_PtrList>

// declare all other types mentioned but not required to include:
class QVBoxLayout; //

class iTextBrowser; //
class iMainWindowViewer; //

class TA_API iDialogList : public QDialog {
  // ##NO_TOKENS ##NO_CSS a class to display a non-modal dialog of search results
INHERITED(QDialog)
  Q_OBJECT

public:
  static iDialogList*   New(int ft = 0, iMainWindowViewer* par_window_ = NULL);
  void                  SetList(taBase_PtrList& base_list, String title,
                                const String_Array* strings, const String& info_title);
  // a list of items to display in a non-modal dialog - shows item and path for each object in table - optional extra info string

protected:
  iDialogList(iMainWindowViewer* par_window_);
  ~iDialogList() {};

  taBase_PtrList        item_list;
  // #IGNORE
  
  QVBoxLayout*          layOuter;
  iTextBrowser*         results;      // list of result items
  String                src;

  virtual void          Constr();
  
protected slots:
  void                  results_setSourceRequest(iTextBrowser* src, const QUrl& url, bool& cancel);

private:
  void                  init(); // called by constructors
};

#endif // iDialogList_h
