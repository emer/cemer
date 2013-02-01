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

#ifndef taiProgStepButton_h
#define taiProgStepButton_h 1

// parent includes:
#include <taiMethodData>

// member includes:

// declare all other types mentioned but not required to include:
class Program; //
class QToolBar; //
class QRadioButton; //


class TA_API taiProgStepButton : public taiMethodData {
  // one method with a list of buttons for args
  Q_OBJECT
public:
  override QWidget* GetButtonRep();
  override bool UpdateButtonRep();

  taiProgStepButton(void* bs, MethodDef* md, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0);

 public slots:
  virtual void  CallFunList(void* prg); // call step on given program (void* needed for callback)
  virtual void  Step1(bool on);         // step level callbacks
  virtual void  Step5(bool on);
  virtual void  Step10(bool on);

 protected:
  Program*      last_step;      // last program stepped -- used to reset step size when switching
  int           new_step_n;     // if > 0, then this is the new value to apply to next program stepped
  int           last_step_n;    // last step n value used
  int           step10_val;
  int           n_step_progs;   // number of step progs we have rendered currently
  QToolBar*     tool_bar;
  QRadioButton* stp1;
  QRadioButton* stp5;
  QRadioButton* stp10;
};

#endif // taiProgStepButton_h
