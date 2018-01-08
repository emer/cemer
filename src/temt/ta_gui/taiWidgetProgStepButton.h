// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef taiWidgetProgStepButton_h
#define taiWidgetProgStepButton_h 1

// parent includes:
#include <taiWidgetMethod>

// member includes:

// declare all other types mentioned but not required to include:
class Program; //
class QToolBar; //
class QToolButton; //

class TA_API taiWidgetProgStepButton : public taiWidgetMethod {
  // one method with a list of buttons for args
  Q_OBJECT
public:
  static const int* std_steps;  // standard step increments
  static const int  n_std_steps;      // number of standard step increments
  
  QWidget* GetButtonRep() override;
  bool  UpdateEnabled() override;
  bool UpdateButtonRep(bool enab) override;

  taiWidgetProgStepButton(void* bs, MethodDef* md, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0);

public slots:
  virtual void  CallFunList(void* prg); // call step on given program (void* needed for callback)
  virtual void  setStepN(int multiplex_val); // set step callback -- step prog * 100 + step_idx

protected:
  int           n_step_progs;   // number of step progs we have rendered currently
  QToolBar*     tool_bar;
  QToolButton** step_buts;
};

#endif // taiWidgetProgStepButton_h
