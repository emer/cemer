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

#include "LayerWriterEl.h"
#include <DataTable>
#include <LayerWriter>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LayerWriterEl);


void LayerWriterEl::Initialize() {
  use_layer_type = true;
  quiet = false;
  na_by_range = false;
  ext_flags = UnitVars::NO_EXTERNAL;
  noise.type = Random::NONE;
  noise.mean = 0.0f;
  noise.var = 0.5f;
}

void LayerWriterEl::Destroy() {
  CutLinks();
}

void LayerWriterEl::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_LayerWriterEl);
  if(owner && !taMisc::is_loading) {
    LayerWriter* own = GET_MY_OWNER(LayerWriter);
    if(own) {
      SetDataNetwork(own->data, own->network);
    }
  }
}

void LayerWriterEl::CutLinks() {
  CutLinks_taAuto(&TA_LayerWriterEl);
  inherited::CutLinks();
}

void LayerWriterEl::CheckThisConfig_impl(bool qt, bool& rval) {
  inherited::CheckThisConfig_impl(qt, rval);
  if(!network) return;
  if(net_target == LAYER) {
    Layer* lay = (Layer*)network->layers.FindLeafName(layer_name);
    if(!lay) return;            // already checked in parent
    TestWarning(!quiet && use_layer_type && lay->layer_type == Layer::HIDDEN, "CheckConfig",
      "layer_type is HIDDEN -- not appropriate for writing to (by default). Turn use_layer_type off and set appropriate ext_flags if this is intentional.");
  }
}

// note: we always do the lookup by name every time -- it just doesn't cost
// that much and it makes everything so much simpler!
bool LayerWriterEl::ApplyInputData(DataTable* db, Network* net) {
  if(!db || !net) return false;
  int chan_idx = GetColIdx(db);
  if(chan_idx < 0) return false;
  if(net_target == TRIAL_NAME) {
    net->trial_name = db->GetData(chan_idx).toString();
    return true;
  }
  else if(net_target == GROUP_NAME) {
    net->group_name = db->GetData(chan_idx).toString();
    return true;
  }
  // LAYER
  Layer* lay = (Layer*)net->layers.FindLeafName(layer_name);
  if(!lay) return false;
  taMatrixPtr mat(db->GetMatrixData(chan_idx)); //note: refs mat
  if(TestWarning(!mat, "ApplyInputData",
                 "could not get matrix data from column:",
                 col_name, "in data:",db->name)) {
    return false;
  }
  if(use_layer_type) {
    if(lay->layer_type == Layer::INPUT)
      ext_flags = UnitVars::EXT;
    else if(lay->layer_type == Layer::TARGET)
      ext_flags = UnitVars::TARG;
    else
      ext_flags = UnitVars::COMP;
  }

  // note: always provide all data, it is up to the network to decide how to use it
  // based on train_mode
  // get the data as a slice -- therefore, frame is always 0
  lay->ApplyInputData(mat, ext_flags, &noise, &offset, na_by_range);
  // mat unrefs at this point, or on exit from routine
  return true;
}
