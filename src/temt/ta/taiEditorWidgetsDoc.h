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

#ifndef taiEditorWidgetsDoc_h
#define taiEditorWidgetsDoc_h 1

// parent includes:
#include <taiEditorWidgetsOfClass>

// member includes:

// declare all other types mentioned but not required to include:
class QTextEdit;
class taDoc;
class QWidget;

class TA_API taiEditorWidgetsDoc: public taiEditorWidgetsOfClass {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit data host for editing raw HTML/wiki text
INHERITED(taiEditorWidgetsOfClass)
  Q_OBJECT
public:
  QTextEdit*            tedHtml; // r/w Html tab

  taDoc*                doc() const; // just returns cast of base

  override QWidget*     firstTabFocusWidget();

  taiEditorWidgetsDoc(void* base, TypeDef* typ_, bool read_only_ = false,
                  bool modal_ = false, QObject* parent = 0);
  taiEditorWidgetsDoc() {init();} // just for instance
  ~taiEditorWidgetsDoc() {}
protected:
  override void         Constr_Body();
  override void         Constr_Box(); // add the docs box
  override void         ClearBody_impl();
  override void         GetValue_Membs();
  override void         GetImage_Membs();

  override bool         eventFilter(QObject* obj, QEvent* event);

private:
  void init();
};

#endif // taiEditorWidgetsDoc_h
