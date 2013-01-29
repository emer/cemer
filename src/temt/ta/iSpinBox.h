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

#ifndef iSpinBox_h
#define iSpinBox_h 1

#include "ta_def.h"

#include <QSpinBox>

class TA_API iSpinBox: public QSpinBox {
  Q_OBJECT
INHERITED(QSpinBox)
public:
  iSpinBox(QWidget* parent = 0);

  // from hidden lineedit
  bool		hasSelectedText();
  void		cut();
  void		copy();
  void		paste();
  void		del();

#ifndef __MAKETA__
signals:
  void		focusChanged(bool got_focus);
  void		selectionChanged(); // note: passed on from hidden lineedit control
#endif

public slots:
  virtual void	setReadOnly(bool value); //note: hides inherited member
protected:
  void 		focusInEvent(QFocusEvent* ev); // override
  void 		focusOutEvent(QFocusEvent* ev); // override
  void 		wheelEvent(QWheelEvent * event);
  void		init();
private:
  int updating;
};

#endif // iSpinBox_h
