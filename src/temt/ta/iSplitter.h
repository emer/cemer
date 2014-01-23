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

#ifndef iSplitter_h
#define iSplitter_h 1

#include "ta_def.h"

#include <QSplitter>

class iSplitter : public QSplitter {
  // ##NO_CSS splitter that supports double-click on splitter handles to expand/collapse
INHERITED(QSplitter)
  Q_OBJECT
public:
  iSplitter(QWidget *parent = 0);
  iSplitter(Qt::Orientation orientation, QWidget *parent = 0);
  ~iSplitter();
		
public slots:
  void 	collapseToggle(int index);
  void	saveSizes();
		
protected:
  QList<int> 	saved_sizes;
  bool		in_collapsing;	// in collapse mode

  QSplitterHandle* createHandle() CPP11_OVERRIDE;
  void showEvent(QShowEvent* ev) CPP11_OVERRIDE;
protected slots:
  void		showDelayed();
};

#endif // iSplitter_h
