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

#include "taApplication.h"
#include <taRootBase>

taApplication::taApplication(int & argc, char ** argv) : QApplication(argc, argv) {
}

bool taApplication::event(QEvent *event) {
  if(event->type() == QEvent::FileOpen) {
    String fname = static_cast<QFileOpenEvent*>(event)->file();
    taRootBase::instance()->projects.Load(fname);
    return true;
  }
  return QApplication::event(event);
}

