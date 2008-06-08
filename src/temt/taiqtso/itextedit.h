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


#ifndef ITEXTEDIT_H
#define ITEXTEDIT_H

#include "taiqtso_def.h"

#include <QTextEdit>

// provides emacs keys for text editing!

class TAIQTSO_API iTextEdit: public QTextEdit { 
INHERITED(QTextEdit)
  Q_OBJECT
public:
  
  iTextEdit(QWidget* parent = 0);
  iTextEdit(const QString &text, QWidget *parent = 0);
  ~iTextEdit();
  
#ifndef __MAKETA__
signals:
  void		lookupKeyPressed();	// use this as hook for looking up information based on current text etc (completion) -- bound to Ctrl-L
#endif

protected:
  void keyPressEvent(QKeyEvent* e);
};

#endif
