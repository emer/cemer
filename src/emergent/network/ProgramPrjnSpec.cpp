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

#include "ProgramPrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(ProgramPrjnSpec);

void ProgramPrjnSpec::Initialize() {
}

void ProgramPrjnSpec::Destroy() {
}

void ProgramPrjnSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!CheckError(!(bool)prog, quiet, rval,
                 "The program prog is not set -- no projections will be made!"))
    return;
  CheckError(!prog->HasVar("prjn"), quiet, rval,
             "The program does not have the required 'prjn' arg variable -- no projections will be made!");
}

void ProgramPrjnSpec::Connect_impl(Projection* prj) {
  if(!prog) return;
  bool did_it = prog->SetVar("prjn", prj);
  if(!did_it) return;
  prog->Run();
}

void ProgramPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

