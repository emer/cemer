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

#include "WtBasedRF.h"
#include <taMath_float>

#include <TiledGpRFPrjnSpec>
#include <V1RegionSpec>
#include <LeabraV1LayerSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(WtBasedRF);

void WtBasedRF::Initialize() {
}

void WtBasedRF::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void WtBasedRF::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!trg_layer, quiet, rval,"trg_layer is NULL");
  CheckError(!snd_layer, quiet, rval,"snd_layer is NULL");
  String projection_name = "Fm_" + snd_layer->name;
  Projection* projection = trg_layer->projections.FindLeafName(projection_name);
  CheckError(!projection, quiet, rval,"projection " + projection_name + " not found. The projection name should be in the form Fm_NameOfLayer");
}

String WtBasedRF::GetDisplayName() const {
  String rval = name;
  if(network) rval += " fm net: " + network->name;
  if(rf_data) rval += " to data: " + rf_data->name;
  if(trg_layer) rval += " trg lay: " + trg_layer->name;
  return rval;
}

void WtBasedRF::InitAll(DataTable* dt, Network* net, Layer* tlay, Layer* slay, V1RetinaProc* rproc, DataTable* wts) {
  rf_data = dt;
  network = net;
  trg_layer = tlay;
  snd_layer = slay;
  v1_retinaProc = rproc;
  trg_layer_wts = wts;
  
  if(!network || !rf_data || !trg_layer || !snd_layer || !v1_retinaProc || !trg_layer_wts)
    return;
  rf_data->ResetData();
}

bool WtBasedRF::ComputeRF() {
  if (!trg_layer || !snd_layer)
    return false;
  
  LayerSpec* spec = snd_layer->GetLayerSpec();
  if (spec->InheritsFrom(&TA_LeabraV1LayerSpec)) {
    return ComputeV2RF();
  }
  else {
    return ComputeHigherLayerRF();
  }
}

bool WtBasedRF::ComputeV2RF() {
  if (!trg_layer || !snd_layer)
    return false;
  // build weights table
  trg_layer_wts->StructUpdate(true);
  trg_layer_wts->Reset();
  trg_layer->WeightsToTable(trg_layer_wts, snd_layer);
  taVector2i snd_layer_unit_grp_geom = snd_layer->un_geom;
  String prjn_name = "Fm_" + snd_layer->name;
  Projection* prjn = trg_layer->projections.FindLeafName(prjn_name);
  TiledGpRFPrjnSpec* tiled_prjn_spec = dynamic_cast<TiledGpRFPrjnSpec*>(prjn->GetPrjnSpec());
  DataCol* dc = trg_layer_wts->data.SafeEl(0);
  MatrixGeom trg_layer_mg;
  trg_layer_mg.SetGeom(4, snd_layer_unit_grp_geom.x, snd_layer_unit_grp_geom.y, tiled_prjn_spec->send_gp_size.x, tiled_prjn_spec->send_gp_size.y);
  dc->ChangeColCellGeom(trg_layer_mg);
  trg_layer_wts->StructUpdate(false);

  // config the rf_data table
  rf_data->StructUpdate(true);
  rf_data->Reset();
  int rows = trg_layer_wts->rows;
  taVector2i snd_layer_grp_geom = snd_layer->gp_geom;
  DataCol* rf_data_values_col;
  DataCol* rf_data_count_col;
  // column of calculated image values
  rf_data_values_col = rf_data->FindMakeColMatrix("values", VT_FLOAT, 2, snd_layer_grp_geom.x, snd_layer_grp_geom.y);
  // column to keep track of the number of values summed so we can correctly average
  rf_data_count_col = rf_data->FindMakeColMatrix("count", VT_FLOAT, 2, snd_layer_grp_geom.x, snd_layer_grp_geom.y);
  rf_data->EnforceRows(rows);
  rf_data->StructUpdate(false);
  
  
  V1RegionSpec* region_spec = dynamic_cast<V1RegionSpec*>(v1_retinaProc->regions.SafeEl(0));
  region_spec->GridGaborFilters(&v1Gabor_GridFilters);
  DataCol* filter_col = v1Gabor_GridFilters.GetColData("Filter");
  if(TestError(!filter_col, "ComputeV2RF", "filter column not found in v1Gabor_GridFilters table")) {
    return false;
  }

  float_Matrix* filter = NULL;
  // the V1 unit group geometry is the same as the filter
  filter = new float_Matrix[snd_layer_unit_grp_geom.x * snd_layer_unit_grp_geom.y];
  for (int row=0; row<v1Gabor_GridFilters.rows; row++) {
    filter[row] = v1Gabor_GridFilters.GetValAsMatrixColName("Filter", row);
  }

  int filter_size_x;
  int filter_size_y;
  filter_col->Get2DCellGeom(filter_size_x, filter_size_y);
  
  float_Matrix* sum_matrix = new float_Matrix(); // sums of filter * weights
  float_Matrix* tmp_matrix = new float_Matrix(); // filter * weights (gets added to running sum)
  int_Matrix* marker_matrix = new int_Matrix(); // all 1's - used to mark the large count matrix for averaging of summed values
  int_Matrix* count_matrix = new int_Matrix(); // tracks the number of values that are summed for each unit of the original image
  sum_matrix->SetGeom(2, filter_size_x, filter_size_y);
  tmp_matrix->SetGeom(2, filter_size_x, filter_size_y);
  marker_matrix->SetGeom(2, filter_size_x, filter_size_y);
  *marker_matrix = 1;
  count_matrix->SetGeom(2, snd_layer_grp_geom.x, snd_layer_grp_geom.y); // same size as rf_data
  
  bool all_good = true;
  for (int wts_row=0; wts_row<trg_layer_wts->rows; wts_row++) {
    *count_matrix = 0;
    Unit* trg_layer_unit = trg_layer->UnitAccess(Layer::ACC_LAY, wts_row, 0);
    if(TestError(!trg_layer_unit, "ComputeV2RF", "trg_layer_unit is null")) {
      all_good = false;
    }
    int unit_no = 0;  //
    ConGroup* recv_cons = trg_layer_unit->FindRecvConGroupFrom(snd_layer);
    if(TestError(!recv_cons, "ComputeV2RF", "recv_cons is null")) {
      all_good = false;
    }
    if (all_good) {
      int dim_0;  // filter angle
      int dim_1;  // filter on_center/off_center
      for (int dim_3=0; dim_3<trg_layer_mg.dim(3); dim_3++) {
        for (int dim_2=0; dim_2<trg_layer_mg.dim(2); dim_2++) {
          *sum_matrix = 0;  //reset
          for (dim_1=0; dim_1<trg_layer_mg.dim(1); dim_1++) {
            for (dim_0=0; dim_0<trg_layer_mg.dim(0); dim_0++) {
              int filter_no = dim_0;
              DataCol* wts_col = trg_layer_wts->GetColData(0);  // only one column
              float weight = wts_col->GetValAsFloatMDims(wts_row, dim_0, dim_1, dim_2, dim_3);
              if (dim_1 == 0) {
                *tmp_matrix = filter[filter_no] * weight;
              }
              if (dim_1 == 1) {
                *tmp_matrix = filter[filter_no] * -weight;
              }
              *sum_matrix = * sum_matrix + *tmp_matrix; //
            }
          }
          Unit* snd_layer_unit = recv_cons->SafeUn(unit_no);
          if(TestError(!snd_layer_unit, "ComputeV2RF", "snd_layer_unit is null")) {
            delete[] filter;
            delete sum_matrix;
            delete tmp_matrix;
            delete marker_matrix;
            delete count_matrix;
            return false;
          }
          taVector2i snd_layer_log_pos;
          snd_layer_unit->LayerLogPos(snd_layer_log_pos);
          unit_no = unit_no + (trg_layer_mg.dim(0) * trg_layer_mg.dim(1));  // first unit of each group is what we need - jump to next group
          snd_layer_log_pos.x = snd_layer_log_pos.x/dim_0;
          snd_layer_log_pos.y = snd_layer_log_pos.y/dim_1;
          // write the sum values to the rf_data table
          rf_data_values_col->WriteFmSubMatrix2DWrap(wts_row, sum_matrix, taMatrix::ADD, snd_layer_log_pos.x, snd_layer_log_pos.y);
          // up the count for those cells of rf_data we just added to
          count_matrix->WriteFmSubMatrix2DWrap(marker_matrix, taMatrix::ADD, snd_layer_log_pos.x, snd_layer_log_pos.y);
        }
      }
      rf_data_count_col->SetValAsMatrix(count_matrix, wts_row);
      // can't divide by zero
      for (int i=0; i<count_matrix->size; i++) {
        if (count_matrix->taMatrix::SafeElAsFloat_Flat(i) == 0.0) {
          count_matrix->Set_Flat(1.0, i);
        }
      }
      rf_data_values_col->WriteFmSubMatrix2DWrap(wts_row, count_matrix, taMatrix::DIV, 0, 0);
    }
    else {
      delete[] filter;
      delete sum_matrix;
      delete tmp_matrix;
      delete marker_matrix;
      delete count_matrix;
      return false;
    }
  }
  delete[] filter;
  delete sum_matrix;
  delete tmp_matrix;
  delete marker_matrix;
  delete count_matrix;

  return true;
}

bool	WtBasedRF::ComputeHigherLayerRF() {
  return true;
}