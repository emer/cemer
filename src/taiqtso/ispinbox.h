// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


// ispinbox.h -- taqt spinbox

#ifndef ISPINBOX_H
#define ISPINBOX_H

#include "taiqtso_def.h"

#include <QSpinBox>

class TAIQTSO_API iSpinBox: public QSpinBox {
  Q_OBJECT
#ifndef __MAKETA__
typedef QSpinBox inherited;
#endif
public:
  iSpinBox(QWidget* parent = 0);

  bool		hilight() {return mhilight;}
  bool 		isReadOnly() {return mreadOnly;}

  void		setPaletteBackgroundColor3(const QColor &); // override, pushes through to lineEdit
  void		stepUp(); // override
  void		stepDown(); // override
  void		updateDisplay(); // override -- hack to disable up/down

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
  virtual void	setHilight(bool value);
  virtual void	setReadOnly(bool value);
protected:
  bool mhilight;
  bool mreadOnly;
  void 		focusInEvent(QFocusEvent* ev); // override
  void 		focusOutEvent(QFocusEvent* ev); // override
  void		init();
private:
  int updating;
};

#endif // ISPINBOX_H
