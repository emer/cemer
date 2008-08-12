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

#ifndef IBUTTON_H
#define IBUTTON_H

#include "taiqtso_def.h"

#include <QColor>
#include <QPushButton>
#include <QToolButton>

class TAIQTSO_API iMenuButton: public QToolButton {
  // ##NO_CSS button to use for menu buttons (standalone or in toolbars)
INHERITED(QToolButton)
  Q_OBJECT
public:
  explicit iMenuButton(QWidget* parent = 0);
  iMenuButton(const QString& text, QWidget* parent);

private:
  void		init();
};

class TAIQTSO_API iColorButton: public QToolButton {
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


#endif

