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

#ifndef ScriptPrjnSpec_h
#define ScriptPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>
#include <ScriptBase>

// member includes:
#include <SArg_Array>

// declare all other types mentioned but not required to include:

TypeDef_Of(ScriptPrjnSpec);

class EMERGENT_API ScriptPrjnSpec : public ProjectionSpec, public ScriptBase {
  // Script-controlled connectivity: use prjn variable in script code to refer to current projection that script is operating on; recv layer is prjn->layer, send layer is prjn->from; must do ru->RecvConsPreAlloc and su->SendConsPreAlloc calls pprior to making connections -- use ru->ConnectFrom(su, prjn) or ru->ConnectFromCk to make connections
INHERITED(ProjectionSpec)
public:
  Projection*	prjn;		// #READ_ONLY #NO_SAVE this holds the argument to the prjn
  SArg_Array	s_args;		// string-valued arguments to pass to script

  override void	Connect_impl(Projection* prj);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {}
    // NOTE: if you allow init_wts you must set wts in your script

  TypeDef*	GetThisTypeDef() const	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }

  virtual void	Compile();
  // #BUTTON compile script from script file into internal runnable format

  void	InitLinks();
  void	Copy_(const ScriptPrjnSpec& cp);
  TA_BASEFUNS(ScriptPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
  override void CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void 	Destroy();
};

#endif // ScriptPrjnSpec_h
