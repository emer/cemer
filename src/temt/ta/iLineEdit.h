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

#ifndef iLineEdit_h
#define iLineEdit_h 1

#include "ta_def.h"

#include <QLineEdit>

class TA_API iLineEdit: public QLineEdit {
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
  void		lookupKeyPressed(iLineEdit* le);
  // use this as hook for looking up information based on current text etc (completion) -- bound to Ctrl-L
#endif

public slots:
  virtual void	setReadOnly(bool value);
  virtual void	editInEditor(); // edit contents in modal dialog
  virtual void  doLookup();     // what we do when the lookup key is pressed

protected:
  int		mmin_char_width; // note: we limit to 128
  int		mchar_width; // note: we limit to 128
  bool		ext_select_on;	   // toggled by Ctrl+space -- extends selection with keyboard movement
  
  void 		focusInEvent(QFocusEvent* ev) override;
  void 		focusOutEvent(QFocusEvent* ev) override;
  void 		keyPressEvent(QKeyEvent* e) override;
  void 		wheelEvent(QWheelEvent * event);
  bool 		event(QEvent * event);
private:  
  void		init();
};


#endif // iLineEdit_h
