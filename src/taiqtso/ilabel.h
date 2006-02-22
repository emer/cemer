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


// ilabel.h -- taqt label

#ifndef ILABEL_H
#define ILABEL_H

#include "taiqtso_def.h"

#include <QLabel>

class TAIQTSO_API iLabel: public QLabel {
//class __declspec(dllexport) iLabel: public QLabel {
  Q_OBJECT
INHERITED(QLabel)
public:
  iLabel(QWidget* parent = 0);
  iLabel(const char* text, QWidget* parent); //note: can't have defaults, ambiguity

  bool		highlight() {return mhighlight;}

public slots:
  virtual void	setHighlight(bool value); // 
protected:
  bool mhighlight;
  void		init();
//  void    	paintEvent(QPaintEvent* pe); // override
};

#endif // ISPINBOX_H
