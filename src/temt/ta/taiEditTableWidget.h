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

#ifndef taiEditTableWidget_h
#define taiEditTableWidget_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiEditTableWidget: public QTableWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBER a table widget for an edit dialog
INHERITED(QTableWidget)
  Q_OBJECT
public:

  taiEditTableWidget(QWidget* parent = NULL);


protected:
  override void         keyPressEvent(QKeyEvent* e);
};

#endif // taiEditTableWidget_h
