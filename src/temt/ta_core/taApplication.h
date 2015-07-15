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

#ifndef taApplication_h
#define taApplication_h 1

// parent includes:
#include "ta_def.h"
#include <QApplication>

// member includes:

// declare all other types mentioned but not required to include:

// this is currently only needed for Mac applications to get open events from finder
// but could potentially be extended in the future..

class taApplication : public QApplication {
  Q_OBJECT
public:
  taApplication ( int & argc, char ** argv );
protected:
  bool event(QEvent *);
};

#endif // taApplication_h
