// Copyright 2017, Regents of the University of Colorado,
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

#ifndef ProjectionSpec_h
#define ProjectionSpec_h 1

#include <State_main>

// parent includes:
#include <BaseSpec>
#include <SpecPtr>

// member includes:

// declare all other types mentioned but not required to include:
class Projection; //

#include <NetworkState_cpp>
#include <UnitState_cpp>

#include <State_main>

eTypeDef_Of(RenormInitWtsSpec);

#include <ProjectionSpec_mbrs>

eTypeDef_Of(ProjectionSpec);

class E_API ProjectionSpec : public BaseSpec {
  // #STEM_BASE #VIRT_BASE ##CAT_Projection Specifies the connectivity between layers (ie. full vs. partial)
INHERITED(BaseSpec)
public:

#include <ProjectionSpec_core>
  
  bool CheckConnect(Projection* prjn, bool quiet);


  String        GetTypeDecoKey() const override { return "ProjectionSpec"; }
  String        GetToolbarName() const override { return "prjn spec"; }

  void  UpdateStateSpecs() override;
  
  void  InitLinks() override;
  SIMPLE_COPY(ProjectionSpec);
  TA_BASEFUNS(ProjectionSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
  
private:
  void  Initialize();
  void  Destroy()               { CutLinks(); }
  void  Defaults_init()         { };
};

SPECPTR_OF(ProjectionSpec);

#endif // ProjectionSpec_h
