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

#ifndef taiStringDataHost_h
#define taiStringDataHost_h 1

// parent includes:
#include <taiDataHostBase>

// member includes:

// declare all other types mentioned but not required to include:
class QTextEdit; //
class QPushButton; //


class TA_API taiStringDataHost: public taiDataHostBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit dialog for displaying a single string value in an editor format
INHERITED(taiDataHostBase)
  Q_OBJECT
public:
  QTextEdit*            edit;
  MemberDef*            mbr; // the member being edited (doesn't have to be String)
  bool                  line_nos; // display line numbers in the editor view
  bool                  rich_text; // string has rich text, not plain

  void                  Constr(const char* prompt = "", const char* win_title = "");
  USING(inherited::GetImage)
  override void         GetImage();
  override void         GetValue();
  override void         Constr_Buttons();

  virtual void          SelectLines(int st_line, int end_line);
  // select a range of lines in the editor

  override bool         eventFilter(QObject *obj, QEvent *event);
  // event filter to trigger apply button on Ctrl+Return

  taiStringDataHost(MemberDef* mbr, void* base, TypeDef* typ_ = NULL,
                    bool read_only_ = false, bool modal_ = false, QObject* parent = 0,
                    bool line_nos_ = false, bool rich_text_ = false);
  ~taiStringDataHost();


protected slots:
  void          btnPrint_clicked();

protected:
  QPushButton*          btnPrint;

  void                  DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);
  override void         Constr_Strings();
  override void         Constr_Box();
  override void         Constr_RegNotifies();
  override void         DoConstr_Dialog(iHostDialog*& dlg);
  override void         ResolveChanges(CancelOp& cancel_op, bool* discarded = NULL);
  override void         Ok_impl();
};

#endif // taiStringDataHost_h
