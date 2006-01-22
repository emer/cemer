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

#ifndef ILINEEDIT_H
#define ILINEEDIT_H

#include "taiqtso_def.h"

#include <QLineEdit>

class TAIQTSO_API iLineEdit: public QLineEdit {
  Q_OBJECT
#ifndef __MAKETA__
typedef QLineEdit inherited;
#endif
public:
  iLineEdit(QWidget* parent = 0);
  iLineEdit(const char* text, QWidget* parent); //note: can't have defaults, ambiguity

  bool		hilight() {return mhilight;}

#ifndef __MAKETA__
signals:
  void		focusChanged(bool got_focus);
#endif

public slots:
  virtual void	setHilight(bool value); //note: won't accept set(true) if readonly
  void		setReadOnly(bool value);

protected:
  bool mhilight;
  void 		focusInEvent(QFocusEvent* ev); // override
  void 		focusOutEvent(QFocusEvent* ev); // override
  void		init();
};

#endif // ILINEEDIT_H
