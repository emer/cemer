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

// Copyright (C) 1995-2007 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// Hack file for maketa because it chokes on Qt header files

#ifndef TA_MAKETA_DEFS_H
#define TA_MAKETA_DEFS_H

// a couple of these are the actual guys when we have NO_GUI

// dummy classes for the view system, when no TA_GUI
#if defined(__MAKETA__) || !defined(TA_GUI)
class QObject {
public:
  QObject() {} // #IGNORE
};

class QPaintDevice {
public:
  QPaintDevice() {} // #IGNORE
};

class QWidget : public QObject, public QPaintDevice {
public:
  QWidget() {} // #IGNORE
};
#endif

#endif // QTMAKETA_H
