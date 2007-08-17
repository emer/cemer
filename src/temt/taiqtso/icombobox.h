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


// icombobox.h -- taqt checkbox

#ifndef ICOMBOBOX_H
#define ICOMBOBOX_H

#include "taiqtso_def.h"

#include <qobject.h>
#include <qcombobox.h>

class TAIQTSO_API iComboBox: public QComboBox {
  Q_OBJECT
public:
  iComboBox(QWidget* parent = NULL);

  bool		hilight() {return mhilight;}

public slots:
  virtual void	setHilight(bool value); // set(true) ignored if ro
protected:
  bool 		mhilight;
  QPalette	defPalette;

  void		init();
//  void		paintEvent(QPaintEvent* pe); // override
};

#endif // ISPINBOX_H
