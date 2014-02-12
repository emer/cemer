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

#include "ActrNBase.h"

#include <ActrModel>
#include <ActrProceduralModule>
#include <ActrDeclarativeModule>
#include <ActrGoalModule>

TA_BASEFUNS_CTORS_DEFN(ActrNBase);

void ActrNBase::Initialize() {
  own_model = NULL;
}

void ActrNBase::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ActrNBase);
  own_model = GET_MY_OWNER(ActrModel);
}

void ActrNBase::CutLinks() {
  own_model = NULL;
  CutLinks_taAuto(&TA_ActrNBase);
  inherited::CutLinks();
}

ActrProceduralModule*   ActrNBase::ProceduralModule() {
  if(!own_model) return NULL;
  // todo: could use fixed indexes if we really want to optimize
  return (ActrProceduralModule*)own_model->modules.FindName("procedural");
}

ActrDeclarativeModule*  ActrNBase::DeclarativeModule() {
  if(!own_model) return NULL;
  // todo: could use fixed indexes if we really want to optimize
  return (ActrDeclarativeModule*)own_model->modules.FindName("declarative");
}

ActrGoalModule*         ActrNBase::GoalModule() {
  if(!own_model) return NULL;
  // todo: could use fixed indexes if we really want to optimize
  return (ActrGoalModule*)own_model->modules.FindName("goal");
}

ActrModule*             ActrNBase::ModuleNamed(const String& nm) {
  if(!own_model) return NULL;
  return (ActrModule*)own_model->modules.FindName(nm);
}

ActrBuffer*             ActrNBase::BufferNamed(const String& nm) {
  if(!own_model) return NULL;
  return (ActrBuffer*)own_model->buffers.FindName(nm);
}
