// Copyright, 1995-2005, Regents of the University of Colorado,
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


// css_qt.h: Qt-css interface

#ifndef CSS_QT_H
#define CSS_QT_H

#include "ta_stdef.h"
#include "css_def.h"

// externals
class cssProgSpace;
class cssClassType;
class cssClassInst;

class CSS_API cssiSession  { // this handles events for css/qt
public:
  static void	CancelProgEdits(cssProgSpace* prsp);
  // cancel any active edit dialogs for given prog space
  static void	CancelClassEdits(cssClassType* cltyp);
  // cancel any active edit dialogs for given class type
  static void	CancelObjEdits(cssClassInst* clobj);
  // cancel any active edit dialogs for given class object
  static void	RaiseObjEdits();
  // bring all object edit dialogs to the front (e.g., after loading)

private:
  cssiSession();
  ~cssiSession();
};

#endif // CSS_QT_H
