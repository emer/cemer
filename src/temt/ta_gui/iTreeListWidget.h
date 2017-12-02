// Copyright 2017, Regents of the University of Colorado,
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

#ifndef iTreeListWidget_h
#define iTreeListWidget_h 1

#include "ta_def.h"

#include <QEvent>
#include <QTreeWidget>

class iTreeListWidgetItem; // This class makes the tree look like a list because it forces drops on top level items to be top level items

class TA_API iTreeListWidget: public QTreeWidget {
  INHERITED(QTreeWidget)
  Q_OBJECT
public:
  
  iTreeListWidget(QWidget* parent = 0);
  ~iTreeListWidget();
  
protected:
  void                  dropEvent(QDropEvent* e) override;

private:
  void                  init();
};

#endif // iTreeListWidget_h

