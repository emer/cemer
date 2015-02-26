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

eTypeDef_Of(ScriptPrjnSpec);

class E_API ScriptPrjnSpec : public ProjectionSpec, public ScriptBase {
  // Script-controlled connectivity: use prjn and make_cons variables in script code to refer to current projection and make_cons state that script is operating on; recv layer is prjn->layer, send layer is prjn->from; if(!make_cons), must do ru->RecvConsPreAlloc and su->SendConsPreAlloc calls pprior to making connections -- else use ru->ConnectFrom(su, prjn) or ru->ConnectFromCk to make connections
INHERITED(ProjectionSpec)
public:
  Projection*	prjn;		// #READ_ONLY #NO_SAVE this holds the argument to the prjn
  bool          make_cons;      // #READ_ONLY #NO_SAVE this holds the argument to the make_cons
  bool          do_init_wts;    // #READ_ONLY #NO_SAVE this is true when Init_Weights_Prjn is called
  SArg_Array	s_args;		// string-valued arguments to pass to script

  void	Connect_impl(Projection* prj, bool make_cns) override;
  void	Init_Weights_Prjn(Projection* prjn, ConGroup* cg, Network* net,
                          int thr_no) override;
  // NOTE: if you allow init_wts you must set wts in your script


  TypeDef*	GetThisTypeDef() const	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }

  virtual void	Compile();
  // #BUTTON compile script from script file into internal runnable format

  void	InitLinks();
  void	Copy_(const ScriptPrjnSpec& cp);
  TA_BASEFUNS(ScriptPrjnSpec);
protected:
  void  UpdateAfterEdit_impl() override;
  void  CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void	Initialize();
  void 	Destroy();
};

#endif // ScriptPrjnSpec_h
