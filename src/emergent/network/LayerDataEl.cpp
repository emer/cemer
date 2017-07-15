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

#include "LayerDataEl.h"
#include <LayerWriter>
#include <DataTable>
#include <MemberDef>

TA_BASEFUNS_CTORS_DEFN(LayerDataEl);

void LayerDataEl::Initialize() {
  net_target = LAYER;
}

void LayerDataEl::Destroy() {
  CutLinks();
}

void LayerDataEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!data) {
    LayerWriter* lw = GET_MY_OWNER(LayerWriter);
    if(lw && lw->data) {
      data = lw->data;
    }
  }
  if(!col_name.empty() && layer_name.empty()) {
    layer_name = col_name;
  }
  if(col_name.empty() && !layer_name.empty()) {
    col_name = layer_name;
  }
}

String LayerDataEl::GetDisplayName() const {
  String rval = "data chan: " + col_name;
  rval += " net: " + GetTypeDef()->GetEnumString("NetTarget", net_target);
  if(net_target == LAYER) {
    rval += " " + layer_name;
  }
  return rval;
}

void LayerDataEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  // these data/network things are set by parent prior to this being called (hopefully)
  if(!data || ! network) return;
  CheckError(col_name.empty(), quiet, rval,
             "col_name is empty");
  CheckError(GetColIdx(data) < 0, quiet, rval,
             "column named",col_name, "not found in data:", data->name);
  if(net_target == LAYER) {
    Layer* lay = (Layer*)network->layers.FindLeafName(layer_name);
    if(CheckError(!lay, quiet, rval,
                  "cannot find layer named:",
                  layer_name, "in network:", network->name)) {
      return; // fatal
    }
  }
}

void LayerDataEl::SetDataNetwork(DataTable* db, Network* net) {
  data = db;
  network = net;
}

int LayerDataEl::GetColIdx(DataTable* db) {
  return db->FindColNameIdx(col_name, true);
}

void LayerDataEl::GetMemberCompletionList(const MemberDef* md, String_Array& list) {
  if (md && md->name == "col_name") {
    if (data) {
      FOREACH_ELEM_IN_LIST(DataCol, col, data->data) {
        list.Add(col->name);
      }
    }
  }
  else if (md && md->name == "layer_name") {
    if (net_target == LAYER) {
      FOREACH_ELEM_IN_GROUP(Layer, lay, network->layers) {
        list.Add(lay->name);
      }
    }
  }
}

