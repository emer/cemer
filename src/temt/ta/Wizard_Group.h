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

#ifndef Wizard_Group_h
#define Wizard_Group_h 1

// parent includes:
#include <taWizard>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API Wizard_Group : public taGroup<taWizard> {
  // ##CAT_Wizard ##EXPAND_DEF_1 group of wizard objects
INHERITED(taGroup<taWizard>)
public:
  virtual void          AutoEdit();

  override String       GetTypeDecoKey() const { return "Wizard"; }

  TA_BASEFUNS(Wizard_Group);
private:
  NOCOPY(Wizard_Group)
  void  Initialize()            { SetBaseType(&TA_taWizard); }
  void  Destroy()               { };
};

#endif // Wizard_Group_h
