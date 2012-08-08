// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ilineedit.h -- text editing field, and modal text editor

#ifndef ILINEEDIT_H
#define ILINEEDIT_H

#include "taiqtso_def.h"

#include <QLineEdit>
#include "idialog.h"

// externals
class QTextEdit;
class QPushButton;

class TAIQTSO_API iLineEdit: public QLineEdit {
  Q_OBJECT
INHERITED(QLineEdit)
public:
  iLineEdit(QWidget* parent = 0);
  iLineEdit(const char* text, QWidget* parent); //note: can't have defaults, ambiguity

  inline int	charWidth() {return mchar_width;} 
  void		setCharWidth(int num); // sets width to accommodate num chars of
  inline int	minCharWidth() {return mmin_char_width;} 
  void		setMinCharWidth(int num); // sets aprox min width to accommodate num chars of average text in current font; 0=no restriction; limited to 128
  virtual void	clearExtSelection();	   // clear extended selection mode and also clear any existing selection
  virtual void	emitReturnPressed();	   // emit this signal
  
#ifndef __MAKETA__
signals:
  void		focusChanged(bool got_focus);
  void		lookupKeyPressed();	// use this as hook for looking up information based on current text etc (completion) -- bound to Ctrl-L
#endif

public slots:
  void		setReadOnly(bool value);
  void		editInEditor(); // edit contents in modal dialog

protected:
  int		mmin_char_width; // note: we limit to 128
  int		mchar_width; // note: we limit to 128
  bool		ext_select_on;	   // toggled by Ctrl+space -- extends selection with keyboard movement
  
  void 		focusInEvent(QFocusEvent* ev); // override
  void 		focusOutEvent(QFocusEvent* ev); // override
  void 		keyPressEvent(QKeyEvent* e);	// override
  void 		wheelEvent(QWheelEvent * event);
  bool 		event(QEvent * event);
private:  
  void		init();
};


class TAIQTSO_API iTextEditDialog : public iDialog {
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


#endif // ILINEEDIT_H
