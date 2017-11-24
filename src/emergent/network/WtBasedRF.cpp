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

#include "WtBasedRF.h"
#include <taMath_float>
#include <Network>
#include <Layer>
#include <V1RegionSpec>

#include <taMisc>

#include <QElapsedTimer>
#include <QDebug>

TA_BASEFUNS_CTORS_DEFN(WtBasedRF);

void WtBasedRF::Initialize() {
  trg_wt_threshold = 0; // don't exclude any values
}

void WtBasedRF::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void WtBasedRF::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!trg_layer, quiet, rval,"trg_layer is NULL");
  CheckError(!snd_layer, quiet, rval,"snd_layer is NULL");
  if (trg_layer && snd_layer) {
    String prjn_name = "Fm_" + snd_layer->name;
    Projection* prjn = trg_layer->projections.FindName(prjn_name);
    CheckError(!prjn, quiet, rval, "projection " + prjn_name + " not found. The projection name should be in the form Fm_NameOfLayer");
  }
}

String WtBasedRF::GetDisplayName() const {
  String rval = name;
  if(network) rval += " fm net: " + network->name;
  if(dt_trg_rf) rval += " to data: " + dt_trg_rf->name;
  if(trg_layer) rval += " trg lay: " + trg_layer->name;
  return rval;
}

bool WtBasedRF::ComputeV2RF(Network* net, DataTable* dt_trg, DataTable* wts, Layer* tlay, Layer* slay, V1RetinaProc* rproc, float wt_threshold) {
  network = net;
  dt_trg_rf = dt_trg;
  trg_layer_wts = wts;
  trg_layer = tlay;
  snd_layer = slay;
  v1_retinaProc = rproc;
  trg_wt_threshold = wt_threshold;
  
  if(!network || !dt_trg_rf || !trg_layer_wts || !trg_layer || !snd_layer || !v1_retinaProc)
    return false;
  
  if(!net->IsBuiltIntact())
    return false;
  dt_trg_rf->ResetData();
  
  // build weights table
  trg_layer_wts->StructUpdate(true);
  trg_layer_wts->Reset();
  trg_layer->WeightsToTable(trg_layer_wts, snd_layer);
  taVector2i snd_layer_unit_grp_geom = snd_layer->un_geom;
  trg_layer_wts->StructUpdate(false);
  
  // config the dt_trg_rf table
  dt_trg_rf->StructUpdate(true);
  dt_trg_rf->Reset();
  int wt_rows = trg_layer_wts->rows;
  taVector2i snd_layer_grp_geom = snd_layer->gp_geom;
  DataCol* dt_trg_rf_values_col;
  DataCol* dt_trg_rf_count_col;
  V1RegionSpec* region_spec = dynamic_cast<V1RegionSpec*>(v1_retinaProc->regions.SafeEl(0));
  int space = region_spec->v1s_specs.spacing;  // spacing of filters
  
  // column of calculated image values - the matrix is the size of the original image which will "normally"
  // be the same as the size of V1 if the filters are spaced every pixel (spacing = 1)
  // but if spacing is 2, V1 will be half the size of the image
  // we want the image size so mulitply by the spacing factor
  dt_trg_rf_values_col = dt_trg_rf->FindMakeColMatrix("values", VT_FLOAT, 2, snd_layer_grp_geom.x * space, snd_layer_grp_geom.y * space);
  // column to keep track of the number of values summed so we can correctly average
  dt_trg_rf_count_col = dt_trg_rf->FindMakeColMatrix("count", VT_INT, 2, snd_layer_grp_geom.x * space, snd_layer_grp_geom.y * space);
  dt_trg_rf->EnforceRows(wt_rows);
  dt_trg_rf->StructUpdate(false);
  
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
  
  taVector2i filter_size;
  filter_col->Get2DCellGeom(filter_size.x, filter_size.y);
  int filter_angles =region_spec->v1s_specs.n_angles;
  
  float_Matrix* tmp_matrix = new float_Matrix(); // filter * weight
  int_Matrix* marker_matrix = new int_Matrix(); // all 1's - used to mark the large count matrix for averaging of summed values
  int_Matrix* count_matrix = new int_Matrix(); // tracks the number of values that are summed for each unit of the original image
  tmp_matrix->SetGeom(2, filter_size.x, filter_size.y);
  marker_matrix->SetGeom(2, filter_size.x, filter_size.y);
  *marker_matrix = 1;
  count_matrix->SetGeom(2, dt_trg_rf_count_col->GetCellGeom(0), dt_trg_rf_count_col->GetCellGeom(1));
  
  int last_row = trg_layer_wts->rows;

//  last_row = 1; // for debug
  
  for (int wts_row=0; wts_row<last_row; wts_row++) {
    *count_matrix = 0;
    taVector2i snd_layer_units;
    snd_layer_units.x = snd_layer_unit_grp_geom.x * snd_layer_grp_geom.x;
    snd_layer_units.y = snd_layer_unit_grp_geom.y * snd_layer_grp_geom.y;
    
    for (int row_grp=0; row_grp<snd_layer_grp_geom.y; row_grp++) {
      for (int col_grp=0; col_grp<snd_layer_grp_geom.x; col_grp++) {
        for (int row_unit=0; row_unit<snd_layer_unit_grp_geom.y; row_unit++) {
          for (int col_unit=0; col_unit<snd_layer_unit_grp_geom.x; col_unit++) {
            // we only concern ourselves with the V1s (simple) filters for now
            // if this is all that is enabled the snd_layer_unit_grp_geom will be 4 x 2 (2 rows of simple filter)
            // but if the V1c (complex) filters are enable we could have as many as 5 rows per V1 unit group.
            // The V1s filters are in the last 2 rows of each unit group so rows 3 & 4 (zero based) when there are 5 rows
            DataCol* wts_col = trg_layer_wts->GetColData(0);  // only one column
            if ((row_unit == snd_layer_unit_grp_geom.y - 1) || (row_unit == snd_layer_unit_grp_geom.y - 2)) {
              int row = (row_grp * snd_layer_unit_grp_geom.y) + row_unit;
              int col = (col_grp * snd_layer_unit_grp_geom.x) + col_unit;
              UnitState_cpp* snd_unit = snd_layer->GetUnitStateFlatXY(network->net_state, col, row);
              float snd_act = snd_unit->act;
              float weight = wts_col->GetValAsFloatMDims(wts_row, col, row);
              if (weight > wt_threshold) {
                int filter_no = col % filter_angles;
                if (row % 2 == 0) { // 2 for on-center/off-center
                  *tmp_matrix = filter[filter_no] * (weight * snd_act);
                }
                else {
                  *tmp_matrix = filter[filter_no] * (-weight * snd_act);
                }
                taVector2i snd_layer_log_pos; // log means logical
                snd_layer_log_pos.x = (col/filter_angles) * space;
                snd_layer_log_pos.y = (row/snd_layer_unit_grp_geom.y) * space;
                
                // write the sum values to the dt_trg_rf table
                dt_trg_rf_values_col->WriteFmSubMatrix2DWrap(wts_row, tmp_matrix, taMatrix::ADD, snd_layer_log_pos.x, snd_layer_log_pos.y);
                // up the count for those cells of dt_trg_rf we just added to
                count_matrix->WriteFmSubMatrix2DWrap(marker_matrix, taMatrix::ADD, snd_layer_log_pos.x, snd_layer_log_pos.y);
              }
            }
            else {
              continue;
            }
          }
        }
      }
    }
    dt_trg_rf_count_col->SetValAsMatrix(count_matrix, wts_row);
    // can't divide by zero
    for (int i=0; i<count_matrix->size; i++) {
      if (count_matrix->taMatrix::SafeElAsFloat_Flat(i) == 0) {
        count_matrix->Set_Flat(1, i);
      }
    }
    dt_trg_rf_values_col->WriteFmSubMatrix(wts_row, count_matrix, taMatrix::DIV, 0, 0);  // matrices same size
  }
  delete[] filter;
  delete tmp_matrix;
  delete marker_matrix;
  delete count_matrix;
  
  return true;
}

bool WtBasedRF::ComputeHigherLayerRF(Network* net, DataTable* dt_trg, DataTable* dt_snd, DataTable* wts,  Layer* tlay, Layer* slay, float wt_threshold) {
  
  network = net;
  dt_trg_rf = dt_trg;
  dt_snd_rf = dt_snd;
  trg_layer_wts = wts;
  trg_layer = tlay;
  snd_layer = slay;
  
  if(!network || !dt_trg_rf || !dt_snd_rf || !trg_layer_wts || !trg_layer || !snd_layer)
    return false;

  LAYER_STATE* snd_lst = snd_layer->GetLayerState(network->net_state);
  
  dt_trg_rf->ResetData();
  
  // build weights table
  trg_layer_wts->StructUpdate(true);
  trg_layer_wts->Reset();
  trg_layer->WeightsToTable(trg_layer_wts, snd_layer);
  taVector2i snd_layer_unit_grp_geom = snd_layer->un_geom;
  trg_layer_wts->StructUpdate(false);
  
  // config the dt_trg_rf table
  dt_trg_rf->StructUpdate(true);
  dt_trg_rf->Reset();
  int wt_rows = trg_layer_wts->rows;
  taVector2i snd_layer_grp_geom = snd_layer->gp_geom;
  DataCol* dt_trg_rf_values_col;
  
  // column of calculated image values - get the size of the image from the previously generated rf data table
  // ** use the values that are multiplied by the actual activations that were calculated for the specific image
  DataCol* dt_snd_rf_values_col = dt_snd_rf->GetColData("values_x_act");
  taVector2i image_size;
  dt_snd_rf_values_col->Get2DCellGeom(image_size.x, image_size.y);
  dt_trg_rf_values_col = dt_trg_rf->FindMakeColMatrix("values", VT_FLOAT, 2, image_size.x, image_size.y);
  dt_trg_rf->EnforceRows(wt_rows);
  dt_trg_rf->StructUpdate(false);

  // NOTE: using a static matrix here would be much more efficient if this is an inner loop
  float_Matrix* sum_matrix = new float_Matrix(); // sums of sending layer values multiplied by target layer weights
  sum_matrix->SetGeom(2, image_size.x, image_size.y);
  
  bool all_good = true;
  
//  QElapsedTimer timer;
//  timer.start();
  
  int units_per_group = trg_layer->un_geom.x * trg_layer->un_geom.y;

  // for every unit in the target layer
  // (each row in table is a matrix of weights for the connections to a unit in target layer)
  int first_row = 0;  // first row for which we want to generate the representation
  int last_row = trg_layer_wts->rows;
//  last_row = 1;  // for debug
  
  for (int wts_row = first_row; wts_row < last_row; wts_row++) {
    *sum_matrix = 0;
    // get the target layer unit so we can find the sending units it recvs from
    UnitState_cpp* trg_layer_unit = trg_layer->GetUnitStateFlatXY(network->net_state, wts_row, 0);
    if(TestError(!trg_layer_unit, "ComputeHigherLayerRF", "trg_layer_unit is null")) {
      all_good = false;
    }
    ConState_cpp* recv_cons = trg_layer_unit->FindRecvConStateFrom(network->net_state, snd_lst);
    if(TestError(!recv_cons, "ComputeHigherLayerRF", "recv_cons is null")) {
      all_good = false;
    }
    UnGpState_cpp* trg_ugp = trg_layer_unit->GetOwnUnGp(network->net_state);
    if (all_good) {
      for (int u=0; u<recv_cons->size; u++) {
        UnitState_cpp* snd_unit = recv_cons->UnState(u, network->net_state);
        taVector2i snd_layer_unit_grp_logical_position;
        snd_layer_unit_grp_logical_position.SetXY(trg_ugp->pos_x, trg_ugp->pos_y);
        
        DataCol* wts_col = trg_layer_wts->GetColData(0);  // only one column
        float weight = wts_col->GetValAsFloatM(wts_row, snd_unit->lay_un_idx);
        if (taMath_float::fabs(weight) < wt_threshold) continue;
        
        float_Matrix* snd_values_matrix = (float_Matrix*)dt_snd_rf_values_col->GetValAsMatrix(snd_unit->lay_un_idx);
        taBase::Ref(snd_values_matrix);
        for (int s=0; s<snd_values_matrix->size; s++) {
          sum_matrix->FastEl_Flat(s) += snd_values_matrix->FastEl_Flat(s) * weight;
        }
        taBase::UnRef(snd_values_matrix);
      }
      dt_trg_rf_values_col->SetValAsMatrix(sum_matrix, wts_row);
    }
    else {
      delete sum_matrix;
      return false;
    }
  }
  
//  qDebug() << "ComputeHigherLayerRF took " << timer.elapsed() << "milliseconds";
  delete sum_matrix;
  
  return true;
}
