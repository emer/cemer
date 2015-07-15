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

#ifndef StartupWizard_h
#define StartupWizard_h 1

// parent includes:
#include <taWizard>

// member includes:
#include <String_Array>

// declare all other types mentioned but not required to include:

taTypeDef_Of(StartupWizard);

class TA_API StartupWizard : public taWizard {
  // object that triggers the startup wizard in root -- really just a trigger for it
INHERITED(taWizard)
public:
  String_ArrayRef    rec_files_ref;   // #IGNORE reference to recent files array

  void          SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
                                 int sls, void* op1_, void* op2_) override;

  void  InitLinks();
  TA_BASEFUNS_NOCOPY(StartupWizard);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // StartupWizard_h
