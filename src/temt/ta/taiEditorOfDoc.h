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

#ifndef taiEditorOfDoc_h
#define taiEditorOfDoc_h 1

// parent includes:
#include <taiEditorOfClass>

// member includes:

// declare all other types mentioned but not required to include:
class QTextEdit;
class taDoc;
class QWidget;

class TA_API taiEditorOfDoc: public taiEditorOfClass {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit data host for editing raw HTML/wiki text
INHERITED(taiEditorOfClass)
  Q_OBJECT
public:
  QTextEdit*            tedHtml; // r/w Html tab

  taDoc*                doc() const; // just returns cast of base

  QWidget*     firstTabFocusWidget() CPP11_OVERRIDE;

  taiEditorOfDoc(void* base, TypeDef* typ_, bool read_only_ = false,
                  bool modal_ = false, QObject* parent = 0);
  taiEditorOfDoc() {init();} // just for instance
  ~taiEditorOfDoc() {}
protected:
  void         Constr_Body() CPP11_OVERRIDE;
  void         Constr_Box() CPP11_OVERRIDE; // add the docs box
  void         ClearBody_impl() CPP11_OVERRIDE;
  void         GetValue_Membs() CPP11_OVERRIDE;
  void         GetImage_Membs() CPP11_OVERRIDE;

  bool         eventFilter(QObject* obj, QEvent* event) CPP11_OVERRIDE;

private:
  void init();
};

#endif // taiEditorOfDoc_h
