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

#ifndef iDialogKeyBindings_h
#define iDialogKeyBindings_h 1

// parent includes:
#include "ta_def.h"
#include <iDialog>

// member includes:
class iMainWindowViewer; //
class QVBoxLayout; //
class QPushButton; //
class QFormLayout; //
class KeyBindings; //

// declare all other types mentioned but not required to include:

taTypeDef_Of(iDialogKeyBindings);

class TA_API iDialogKeyBindings : public iDialog {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS A dialog to collect user defined key bindings for predefined actions
  INHERITED(iDialog)
  Q_OBJECT
public:
  static iDialogKeyBindings* New(iMainWindowViewer* par_window_ = NULL);
  
  iDialogKeyBindings(QWidget* par_window_);
  iDialogKeyBindings();
  ~iDialogKeyBindings();
  
  QVBoxLayout*          layOuter;

  
protected:
  virtual void          Constr();
  
  QPushButton*          button_ok;
  QPushButton*          button_revert;
  
  KeyBindings*          current_bindings;

  QFormLayout*          bindings_layout[10];

  protected slots:
    void                accept() override;
    void                reject() override;
  
private:
};

#endif // iDialogKeyBindings_h
