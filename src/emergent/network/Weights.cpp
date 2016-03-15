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

#include "Weights.h"

#include <MemberDef>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Weights);

void Weights::Initialize() {
  batch = 0;
  epoch = 0;
  save_with_proj = false;
  auto_load = false;
  quiet_load = false;
}

void Weights::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::is_loading && !save_with_proj && auto_load && load_file.nonempty()) {
    taMisc::Info("Auto loading weights file:", load_file);
    LoadWeights(load_file);
    taMisc::Info("Done", load_file);
  }
}

taBase::DumpQueryResult Weights::Dump_QuerySaveMember(MemberDef* md) {
  if (md->name != "wt_file")
    return inherited::Dump_QuerySaveMember(md);
  return (save_with_proj) ? DQR_SAVE : DQR_NO_SAVE;
}


// WeightsFmNet and WeightsToNet are in Network

bool Weights::HasWeights(bool err) {
  if(err) {
    if(TestError(wt_file.empty(), "HasWeights", "weight file contents is empty")) {
      return false;
    }
  }
  else {
    return wt_file.nonempty();
  }
  return true;
}

bool Weights::SaveWeights(const String& fname) {
  if(!HasWeights(true)) return false;
  return wt_file.SaveToFile(fname);
}

bool Weights::LoadWeights(const String& fname) {
  String filename(fname);
  filename.trim();
  return wt_file.LoadFromFile(filename);
}
