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

#ifndef ProgramPrjnSpec_h
#define ProgramPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>
#include <Program>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ProgramPrjnSpec);

class E_API ProgramPrjnSpec : public ProjectionSpec {
  // Program-controlled connectivity: points to a program that will generate appropriate connections -- MUST have 'prjn', 'make_cons', and 'do_init_wts' arg variables program, which will be set prior to calling to relevant projection -- recv layer is prjn->layer, send layer is prjn->from; must do ru->RecvConsPreAlloc and su->SendConsPreAlloc calls for !make_cons -- for make_cons, use ru->ConnectFrom(su, prjn) or ru->ConnectFromCk to make connections
INHERITED(ProjectionSpec)
public:
  ProgramRef	prog;		// program to call to connect layers -- prjn arg value (must exist) is set to current projection and then it is called

  void	Connect_impl(Projection* prj, bool make_cons) override;
  void	Init_Weights_Prjn(Projection* prjn, RecvCons* cg, Unit* ru, Network* net) override;

  TA_SIMPLE_BASEFUNS(ProgramPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
  void CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void	Initialize();
  void 	Destroy();
};

#endif // ProgramPrjnSpec_h
