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

#ifndef ProgramCallFun_h
#define ProgramCallFun_h 1

// parent includes:
#include <ProgramCall>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(ProgramCallFun);

class TA_API ProgramCallFun : public ProgramCall {
  // call (run) a specific function within another program (function must not take any args!), setting any arguments before hand
INHERITED(ProgramCall)
public:
  String        function;       // function name to call -- must not take any args -- the calling args are those for the overall Program, not for the specific function within the program

  bool         CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool         CvtFmCode(const String& code) override;

  String       GetDisplayName() const override;
  String       GetToolbarName() const override { return "prog fun()"; }
 
  PROGEL_SIMPLE_BASEFUNS(ProgramCallFun);
 protected:
  void         GenCssBody_impl(Program* prog) override;
 private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // ProgramCallFun_h
