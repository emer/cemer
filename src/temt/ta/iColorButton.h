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

#ifndef iColorButton_h
#define iColorButton_h 1

#include "ta_def.h"
#include <QColor>
#include <QToolButton>

class TA_API iColorButton: public QToolButton {
INHERITED(QToolButton)
  Q_OBJECT
public:
  bool		useAlpha() const {return m_use_alpha;}
  void		setUseAlpha(bool val) {m_use_alpha = val;}

  QColor	color() const {return m_col;}
  void		setColor(const QColor& val);
  
//  QSize		sizeHint() const;
  
  explicit iColorButton(QWidget* parent = 0);
  iColorButton(int r, int g, int b, int a = 255, QWidget* parent = 0);
  
#ifndef __MAKETA__
signals:
  void		colorChanged(); // sent when user changes color (but not programmatically)
#endif

protected:
  QColor	m_col;
  bool		m_use_alpha;
  void 		paintEvent(QPaintEvent* pe);
  
protected slots:
  void		this_clicked();
  
private:
  void		init(int r=0, int b=0, int g=0, int a=0);
};

#endif // iColorButton_h
