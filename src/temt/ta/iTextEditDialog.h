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

#ifndef iTextEditDialog_h
#define iTextEditDialog_h 1

#include "ta_def.h"

#include <iDialog>

// externals
class QTextEdit;
class QPushButton;

class TA_API iTextEditDialog : public iDialog {
  Q_OBJECT
INHERITED(iDialog)
public:
  QTextEdit*	txtText;
  QPushButton*	btnPrint;
  QPushButton*	btnCopy;
  QPushButton*	btnOk; // read/write only
  QPushButton* 	btnCancel; // or close, if read only
  
  bool		isReadOnly() {return m_readOnly;}
  virtual void	setText(const QString& value);
  
  iTextEditDialog(bool readOnly = false, QWidget* parent = 0);
  ~iTextEditDialog();
protected:
  bool		m_readOnly;
  
protected slots:
  void		btnPrint_clicked();
  void		copyToClipboard(); // copy text of msg to clipboard
  
private:
  void 		init(bool readOnly);
};


#endif // iTextEditDialog_h
