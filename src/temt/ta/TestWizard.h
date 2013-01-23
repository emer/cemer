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

#ifndef TestWizard_h
#define TestWizard_h 1

// parent includes:
#include <taWizard>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(TestWizard);

class TA_API TestWizard:  public taWizard {
  // test wizard -- during devel, all Wizards must inherit from this
INHERITED(taWizard)
public:
  int           inty; // #PAGE_0_Start
  float         floaty;
  String        outputy; // #PAGE_1_Output

  void          LaunchWizard(); // #BUTTON launch the wizard

  TA_BASEFUNS(TestWizard);
private:
  SIMPLE_COPY(TestWizard);
  void  Initialize() {inty=0; floaty=1;}
  void  Destroy()       { CutLinks(); }
};

#endif // TestWizard_h
