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


// icheckbox.h -- taqt checkbox

#ifndef ICHECKBOX_H
#define ICHECKBOX_H

#include "taiqtso_def.h"

#include <QCheckBox>

class TAIQTSO_API iCheckBox: public QCheckBox {
//class __declspec(dllexport) iCheckBox: public QCheckBox {
  Q_OBJECT
INHERITED(QCheckBox)
public:
  iCheckBox(QWidget* parent = 0);
  iCheckBox(const char* text, QWidget* parent); //note: can't have defaults, ambiguity
  iCheckBox(bool value, QWidget* parent); //note: can't have defaults, ambiguity

  bool 		isReadOnly() const {return mread_only;}

public slots:
  virtual void	setReadOnly(bool value);
  
protected:
  bool 		mread_only;
  void		init();
};

#endif // ISPINBOX_H
