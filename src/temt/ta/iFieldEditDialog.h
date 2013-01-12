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

#ifndef iFieldEditDialog_h
#define iFieldEditDialog_h 1

// parent includes:
#include "ta_def.h"
#include <iDialog>

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class iTextEdit; // 
class QPushButton; // 
class taiField;


class TA_API iFieldEditDialog : public iDialog {
  Q_OBJECT
  INHERITED(iDialog)
public:
  iTextEdit*    txtText;
  QPushButton*  btnOk; // read/write only
  QPushButton*  btnCancel; // or close, if read only
  QPushButton*  btnApply; // writes it back to field
  QPushButton*  btnRevert; // gets back from field

  bool          isReadOnly() { return m_read_only; }
  virtual void  setText(const QString& value);

  iFieldEditDialog(bool modal_, bool read_only, const String& desc, taiField* parent);
  ~iFieldEditDialog();

public slots:
  override void accept();
  override void reject();

protected:
  taiField*     field;
  bool          m_read_only;

  void          setApplyEnabled(bool enabled); // set apply/revert enabled or not

protected slots:
  void          btnApply_clicked();
  void          btnRevert_clicked();
  void          repChanged();

private:
  void          init(bool readOnly, const String& desc);
};

#endif // iFieldEditDialog_h
