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

#include "ActBasedRF.h"
#include <Network>


void ActBasedRF::Initialize() {
  norm_mode = NORM_TRG_UNIT_RF_LAY;
  threshold = .5f;
}

void ActBasedRF::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void ActBasedRF::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!rf_data, quiet, rval, "rf_data is NULL");
  CheckError(!network, quiet, rval,"network is NULL");
  CheckError(!trg_layer, quiet, rval,"trg_layer is NULL");
}

String ActBasedRF::GetDisplayName() const {
  String rval = name;
  if(network) rval += " fm net: " + network->name;
  if(rf_data) rval += " to data: " + rf_data->name;
  if(trg_layer) rval += " trg lay: " + trg_layer->name;
  return rval;
}

void ActBasedRF::ConfigDataTable(DataTable* dt, Network* net) {
  dt->StructUpdate(true);
  dt->Reset();                  // nuke cols -- ensure matching
  int rows = trg_layer->units.leaves;
  int idx;
  FOREACH_ELEM_IN_GROUP(Layer, lay, net->layers) {
    if(lay->lesioned()) continue;
    DataCol* da;
    if(lay->unit_groups)
      da = dt->FindMakeColName(lay->name, idx, VT_FLOAT, 4, lay->un_geom.x,
                          lay->un_geom.y, lay->gp_geom.x, lay->gp_geom.y);
    else
      da = dt->FindMakeColName(lay->name, idx, VT_FLOAT, 2, lay->un_geom.x,
                          lay->un_geom.y);
  }
  dt->EnforceRows(rows);
  dt->StructUpdate(false);
}

void ActBasedRF::InitData() {
  if(!network || !rf_data || !trg_layer) return;
  ConfigDataTable(rf_data, network);
  ConfigDataTable(&sum_data, network);
  wt_array.SetGeom(1, trg_layer->units.leaves);

  for(int i=0;i<rf_data->data.size; i++) {
    DataCol* da = rf_data->data.FastEl(i);
    ((float_Matrix*)da->AR())->InitVals();
  }
  for(int i=0;i<sum_data.data.size; i++) {
    DataCol* da = sum_data.data.FastEl(i);
    ((float_Matrix*)da->AR())->InitVals();
  }
  wt_array.InitVals();
  rf_data->UpdateAllViews();
}

void ActBasedRF::InitAll(DataTable* dt, Network* net, Layer* tlay) {
  rf_data = dt;
  network = net;
  trg_layer = tlay;

  if(!network || !rf_data || !trg_layer) return;
  rf_data->ResetData();
  sum_data.ResetData();
  InitData();
}

bool ActBasedRF::IncrementSums() {
  if(!network || !rf_data || !trg_layer) return false;

  int idx;
  FOREACH_ELEM_IN_GROUP(Layer, lay, network->layers) {
    if(lay->lesioned() || lay->Iconified()) continue;
    DataCol* sum_da = NULL;
    if(lay->unit_groups) {
      sum_da = sum_data.FindMakeColName(lay->name, idx, VT_FLOAT, 4, lay->un_geom.x,
                                        lay->un_geom.y, lay->gp_geom.x, lay->gp_geom.y);
    }
    else {
      sum_da = sum_data.FindMakeColName(lay->name, idx, VT_FLOAT, 2, lay->un_geom.x,
                                        lay->un_geom.y);
    }

    int tidx = 0;
    Unit* tu;
    taLeafItr tui;
    for(tu = trg_layer->units.FirstEl(tui); tu; tu = trg_layer->units.NextEl(tui), tidx++) {
      float tact = fabsf(tu->act);
      if(tact < threshold) continue; // not this time!

      wt_array.FastEl(tidx) += tact;

      float_Matrix* sum_mat = (float_Matrix*)sum_da->GetValAsMatrix(tidx);
      taBase::Ref(sum_mat);
      int sidx = 0;
      Unit* su;
      taLeafItr sui;
      for(su = lay->units.FirstEl(sui); su; su = lay->units.NextEl(sui), sidx++) {
        sum_mat->FastEl(sidx) += tu->act * su->act;
      }
      taBase::unRefDone(sum_mat);
    }
  }
  return true;
}

bool ActBasedRF::ComputeRF() {
  if(!rf_data) return false;
  if(TestError(rf_data->data.size != sum_data.data.size ||
               sum_data.rows != wt_array.size, "ComputeRF", "data tables not the same size, do InitData and re-run!"))
    return false;

  // first, do the normalization
  for(int i=0;i<rf_data->data.size; i++) {
    DataCol* rf_da = rf_data->data.FastEl(i);
    DataCol* sum_da = sum_data.data.FastEl(i);

    for(int r=0;r<rf_data->rows; r++) {
      float wt = wt_array.FastEl(r);
      float sc = 1.0f;
      if(wt > 0.0f)
        sc = 1.0f / wt;
      float_Matrix* rf_mat = (float_Matrix*)rf_da->GetValAsMatrix(r);
      float_Matrix* sum_mat = (float_Matrix*)sum_da->GetValAsMatrix(r);
      taBase::Ref(rf_mat);
      taBase::Ref(sum_mat);

      for(int j=0;j<rf_mat->size; j++) {
        rf_mat->FastEl(j) = sum_mat->FastEl(j) * sc;
      }

      taBase::unRefDone(rf_mat);
      taBase::unRefDone(sum_mat);
    }
  }

  int idx;

  switch(norm_mode) {
  case NO_NORM:
    break;
  case NORM_TRG_UNIT: {
    // per row, across cols
    for(int r=0;r<rf_data->rows; r++) {
      float max_val = 0.0f;
      for(int i=0;i<rf_data->data.size; i++) {
        DataCol* rf_da = rf_data->data.FastEl(i);
        float_Matrix* mat = (float_Matrix*)rf_da->GetValAsMatrix(r);
        taBase::Ref(mat);
        float mx = taMath_float::vec_abs_max(mat, idx);
        max_val = MAX(mx, max_val);
        taBase::unRefDone(mat);
      }
      float sc = 1.0f;
      if(max_val > 0.0f)
        sc = 1.0f / max_val;
      for(int i=0;i<rf_data->data.size; i++) {
        DataCol* rf_da = rf_data->data.FastEl(i);
        float_Matrix* mat = (float_Matrix*)rf_da->GetValAsMatrix(r);
        taBase::Ref(mat);
        taMath_float::vec_mult_scalar(mat, sc);
        taBase::unRefDone(mat);
      }
    }
    break;
  }
  case NORM_TRG_LAYER: {
    // over entire set of data
    float max_val = 0.0f;
    for(int i=0;i<rf_data->data.size; i++) {
      DataCol* rf_da = rf_data->data.FastEl(i);
      float mx = taMath_float::vec_abs_max((float_Matrix*)rf_da->AR(), idx);
      max_val = MAX(mx, max_val);
    }
    float sc = 1.0f;
    if(max_val > 0.0f)
      sc = 1.0f / max_val;
    for(int i=0;i<rf_data->data.size; i++) {
      DataCol* rf_da = rf_data->data.FastEl(i);
      taMath_float::vec_mult_scalar((float_Matrix*)rf_da->AR(), sc);
    }
    break;
  }
  case NORM_RF_LAY: {
    // per column
    for(int i=0;i<rf_data->data.size; i++) {
      DataCol* rf_da = rf_data->data.FastEl(i);
      taMath_float::vec_norm_abs_max((float_Matrix*)rf_da->AR());
    }
    break;
  }
  case NORM_TRG_UNIT_RF_LAY: {
    // per cell
    for(int r=0;r<rf_data->rows; r++) {
      for(int i=0;i<rf_data->data.size; i++) {
        DataCol* rf_da = rf_data->data.FastEl(i);
        float_Matrix* mat = (float_Matrix*)rf_da->GetValAsMatrix(r);
        taBase::Ref(mat);
        taMath_float::vec_norm_abs_max(mat);
        taBase::unRefDone(mat);
      }
    }
    break;
  }
  }

  rf_data->UpdateAllViews();
  return true;
}

bool ActBasedRF::CopyRFtoNetWtPrjn(int trg_unit_no) {
  if(!network || !rf_data || !trg_layer) return false;

  if(TestError(trg_unit_no >= rf_data->rows, "CopyRFtoNetWtPrjn", "trg_unit_no is greater than number of target units"))
    return false;

  int idx;
  FOREACH_ELEM_IN_GROUP(Layer, lay, network->layers) {
    if(lay->lesioned() || lay->Iconified()) continue;
    DataCol* rf_da = NULL;
    if(lay->unit_groups) {
      rf_da = rf_data->FindMakeColName(lay->name, idx, VT_FLOAT, 4, lay->un_geom.x,
                                        lay->un_geom.y, lay->gp_geom.x, lay->gp_geom.y);
    }
    else {
      rf_da = rf_data->FindMakeColName(lay->name, idx, VT_FLOAT, 2, lay->un_geom.x,
                                        lay->un_geom.y);
    }

    int mx = MAX(lay->units.leaves, rf_da->cell_size());

    for(int i=0;i<mx; i++) {
      Unit* u = lay->units.Leaf(i);
      float rfv = rf_da->GetValAsFloatM(trg_unit_no, i);
      u->wt_prjn = rfv;
    }
  }
  network->UpdateAllViews();
  return true;
}
