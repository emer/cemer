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

#include "LayerWriter.h"
#include <Network>

void LayerWriter::Initialize() {
  layer_data.SetBaseType(&TA_LayerWriterEl);
}

void LayerWriter::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  layer_data.SetDataNetwork(data, network);
}

void LayerWriter::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!data, quiet, rval, "data is NULL");
  CheckError(!network, quiet, rval,"network is NULL");
}

void LayerWriter::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  layer_data.SetDataNetwork(data, network); // make sure it has ptrs first!
  layer_data.CheckConfig(quiet, rval);
}

String LayerWriter::GetDisplayName() const {
  String rval = name;
  if(data) rval += " fm data: " + data->name;
  if(network) rval += " to net: " + network->name;
  return rval;
}

void LayerWriter::SetDataNetwork(DataBlock* db, Network* net) {
  data = db;
  network = net;
  layer_data.SetDataNetwork(data, network);
}

void LayerWriter::AutoConfig(bool remove_unused) {
  if(TestError(!data || !network, "AutoConfig",
               "Either the data or the network variables are not set -- please set them and try again!")) {
    return;
  }

  StructUpdate(true);

  for(int i=0;i<layer_data.size;i++) {
    layer_data.FastEl(i)->ClearBaseFlag(BF_MISC1); // use this for marking usage
  }

  bool made_new;
  FOREACH_ELEM_IN_GROUP(Layer, lay, network->layers) {
    if(lay->layer_type == Layer::HIDDEN) continue;
    int chan_idx = data->GetSourceChannelByName(lay->name, false);
    if(TestWarning(chan_idx < 0, "AutoConfig",
                   "did not find data column for layer named:", lay->name, "of type:", TA_Layer.GetEnumString("LayerType", lay->layer_type))) {
      continue; // not found
    }
    LayerWriterEl* lrw = (LayerWriterEl*)layer_data.FindMakeLayerData(lay->name, lay->name, made_new);
    lrw->SetDataNetwork(data, network);
    lrw->SigEmitUpdated();
    lrw->SetBaseFlag(BF_MISC1); // mark as used
  }
  int nm_idx = data->GetSourceChannelByName("Name", false);
  if(nm_idx >= 0) {
    LayerWriterEl* lrw = (LayerWriterEl*)layer_data.FindMakeChanName("Name", made_new);
    lrw->net_target = LayerDataEl::TRIAL_NAME;
    lrw->SigEmitUpdated();
    lrw->SetBaseFlag(BF_MISC1); // mark as used
  }
  int gp_idx = data->GetSourceChannelByName("Group", false);
  if(gp_idx >= 0) {
    LayerWriterEl* lrw = (LayerWriterEl*)layer_data.FindMakeChanName("Group", made_new);
    lrw->net_target = LayerDataEl::GROUP_NAME;
    lrw->SigEmitUpdated();
    lrw->SetBaseFlag(BF_MISC1); // mark as used
  }

  if(remove_unused) {
    for(int i=layer_data.size-1; i>=0; i--) {
      if(!layer_data.FastEl(i)->HasBaseFlag(BF_MISC1))
        layer_data.RemoveIdx(i);
    }
  }

  StructUpdate(false);
}

bool LayerWriter::ApplyInputData() {
  if(!data || !network) return false;
  if(TestError(!data->ReadAvailable(), "ApplyInputData",
               "data not available for reading from datatable:", data->GetName(),
               "you must perform a ReadItem or ReadFirst/Next on data to select row to read input patterns from (std Epoch program does this in NetDataLoop)")) {
    return false;
  }
  bool rval = true;
  for (int i = 0; i < layer_data.size; ++i) {
    LayerWriterEl* lw = (LayerWriterEl*)layer_data.FastEl(i);
    bool nval = lw->ApplyInputData(data, network);
    if(!nval) rval = false;
  }
  return rval;
}


