// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef WtInitPrompt_h
#define WtInitPrompt_h 1

// parent includes:
#include "network_def.h"
#include <IfGuiPrompt>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(WtInitPrompt);

class E_API WtInitPrompt: public IfGuiPrompt { 
  // special program element for prompting whether to initialize network weights -- only prompts if network has been trained (epoch > 0) -- requires a variable named: network -- will complain if not found!
INHERITED(IfGuiPrompt)
public:
  String	GetToolbarName() const CPP11_OVERRIDE { return "wt init prmt"; }
  TA_BASEFUNS_NOCOPY(WtInitPrompt);
protected:
  void		GenCssPre_impl(Program* prog) CPP11_OVERRIDE; 
  void		GenCssPost_impl(Program* prog) CPP11_OVERRIDE; 

private:
  void	Initialize();
  void	Destroy()	{ } //
};

#endif // WtInitPrompt_h
