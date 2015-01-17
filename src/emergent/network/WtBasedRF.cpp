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
  String prjn_name = "Fm_" + snd_layer->name;
  Projection* prjn = trg_layer->projections.FindLeafName(prjn_name);
  CheckError(!prjn, quiet, rval, "projection " + prjn_name + " not found. The projection name should be in the form Fm_NameOfLayer");
}

String WtBasedRF::GetDisplayName() const {
  String rval = name;
  if(network) rval += " fm net: " + network->name;
  if(dt_trg_rf) rval += " to data: " + dt_trg_rf->name;
  if(trg_layer) rval += " trg lay: " + trg_layer->name;
  return rval;
}

bool WtBasedRF::ComputeV2RF(Network* net, DataTable* dt_trg, DataTable* wts, Layer* tlay, Layer* slay, V1RetinaProc* rproc) {
  network = net;
  dt_trg_rf = dt_trg;
  trg_layer_wts = wts;
  trg_layer = tlay;
  snd_layer = slay;
  v1_retinaProc = rproc;
  
  if(!network || !dt_trg_rf || !trg_layer_wts || !trg_layer || !snd_layer || !v1_retinaProc)
    return false;
  
  dt_trg_rf->ResetData();
  
  LayerSpec* spec = snd_layer->GetLayerSpec();
  if (!spec->InheritsFrom(&TA_LeabraV1LayerSpec)) {
    taMisc::Error("If the analysis_mode is SEND_LAYER_V1 the sending layer must have a spec that is or derives from LeabraV1LayerSpec");
    return false;
  }
  
  // build weights table
  trg_layer_wts->StructUpdate(true);
  trg_layer_wts->Reset();
  trg_layer->WeightsToTable(trg_layer_wts, snd_layer);
  taVector2i snd_layer_unit_grp_geom = snd_layer->un_geom;
  String prjn_name = "Fm_" + snd_layer->name;
  Projection* prjn = trg_layer->projections.FindLeafName(prjn_name);
  if(TestError(!prjn, "ComputeV2RF", "projection " + prjn_name + " not found. The projection name should be in the form Fm_NameOfLayer")) {
    return false;
  }
  TiledGpRFPrjnSpec* tiled_prjn_spec = dynamic_cast<TiledGpRFPrjnSpec*>(prjn->GetPrjnSpec());
  DataCol* dc = trg_layer_wts->data.SafeEl(0);
  MatrixGeom trg_layer_mg;
  trg_layer_mg.SetGeom(4, snd_layer_unit_grp_geom.x, snd_layer_unit_grp_geom.y, tiled_prjn_spec->send_gp_size.x, tiled_prjn_spec->send_gp_size.y);
  dc->ChangeColCellGeom(trg_layer_mg);
  trg_layer_wts->StructUpdate(false);

  // config the dt_trg_rf table
  dt_trg_rf->StructUpdate(true);
  dt_trg_rf->Reset();
  int wt_rows = trg_layer_wts->rows;
  taVector2i snd_layer_grp_geom = snd_layer->gp_geom;
  DataCol* dt_trg_rf_values_col;
  DataCol* dt_trg_rf_count_col;
  // column of calculated image values - the snd_layer_grp_geom is the size of the image
  dt_trg_rf_values_col = dt_trg_rf->FindMakeColMatrix("values", VT_FLOAT, 2, snd_layer_grp_geom.x, snd_layer_grp_geom.y);
  // column to keep track of the number of values summed so we can correctly average
  dt_trg_rf_count_col = dt_trg_rf->FindMakeColMatrix("count", VT_INT, 2, snd_layer_grp_geom.x, snd_layer_grp_geom.y);
  dt_trg_rf->EnforceRows(wt_rows);
  dt_trg_rf->StructUpdate(false);
  
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

  taVector2i filter_size;
  filter_col->Get2DCellGeom(filter_size.x, filter_size.y);
  
  float_Matrix* sum_matrix = new float_Matrix(); // sums of filter * weights
  float_Matrix* tmp_matrix = new float_Matrix(); // filter * weights (gets added to running sum)
  int_Matrix* marker_matrix = new int_Matrix(); // all 1's - used to mark the large count matrix for averaging of summed values
  int_Matrix* count_matrix = new int_Matrix(); // tracks the number of values that are summed for each unit of the original image
  sum_matrix->SetGeom(2, filter_size.x, filter_size.y);
  tmp_matrix->SetGeom(2, filter_size.x, filter_size.y);
  marker_matrix->SetGeom(2, filter_size.x, filter_size.y);
  *marker_matrix = 1;
  count_matrix->SetGeom(2, snd_layer_grp_geom.x, snd_layer_grp_geom.y); // same size as dt_trg_rf
  
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
              *sum_matrix += *tmp_matrix;
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
          // write the sum values to the dt_trg_rf table
          dt_trg_rf_values_col->WriteFmSubMatrix2DWrap(wts_row, sum_matrix, taMatrix::ADD, snd_layer_log_pos.x, snd_layer_log_pos.y);
          // up the count for those cells of dt_trg_rf we just added to
          count_matrix->WriteFmSubMatrix2DWrap(marker_matrix, taMatrix::ADD, snd_layer_log_pos.x, snd_layer_log_pos.y);
        }
      }
      dt_trg_rf_count_col->SetValAsMatrix(count_matrix, wts_row);
      // can't divide by zero
      for (int i=0; i<count_matrix->size; i++) {
        if (count_matrix->taMatrix::SafeElAsFloat_Flat(i) == 0) {
          count_matrix->Set_Flat(1, i);
        }
      }
      dt_trg_rf_values_col->WriteFmSubMatrix2DWrap(wts_row, count_matrix, taMatrix::DIV, 0, 0);
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

bool WtBasedRF::ComputeHigherLayerRF(Network* net, DataTable* dt_trg, DataTable* dt_snd, DataTable* wts,  Layer* tlay, Layer* slay) {
  
  network = net;
  dt_trg_rf = dt_trg;
  dt_snd_rf = dt_snd;
  trg_layer_wts = wts;
  trg_layer = tlay;
  snd_layer = slay;
  
  if(!network || !dt_trg_rf || !dt_snd_rf || !trg_layer_wts || !trg_layer || !snd_layer)
    return false;

  dt_trg_rf->ResetData();
  
  // build weights table
  trg_layer_wts->StructUpdate(true);
  trg_layer_wts->Reset();
  trg_layer->WeightsToTable(trg_layer_wts, snd_layer);
  taVector2i snd_layer_unit_grp_geom = snd_layer->un_geom;
  String prjn_name = "Fm_" + snd_layer->name;
  Projection* prjn = trg_layer->projections.FindLeafName(prjn_name);
  if(TestError(!prjn, "ComputeHigherLayerRF", "projection " + prjn_name + " not found. The projection name should be in the form Fm_NameOfLayer")) {
    return false;
  }
  TiledGpRFPrjnSpec* tiled_prjn_spec = dynamic_cast<TiledGpRFPrjnSpec*>(prjn->GetPrjnSpec());
  DataCol* dc = trg_layer_wts->data.SafeEl(0);
  MatrixGeom trg_layer_mg;
  trg_layer_mg.SetGeom(4, snd_layer_unit_grp_geom.x, snd_layer_unit_grp_geom.y, tiled_prjn_spec->send_gp_size.x, tiled_prjn_spec->send_gp_size.y);
  dc->ChangeColCellGeom(trg_layer_mg);
  trg_layer_wts->StructUpdate(false);
  
  // config the dt_trg_rf table
  dt_trg_rf->StructUpdate(true);
  dt_trg_rf->Reset();
  int wt_rows = trg_layer_wts->rows;
  taVector2i snd_layer_grp_geom = snd_layer->gp_geom;
  DataCol* dt_trg_rf_values_col;
  DataCol* dt_trg_rf_count_col;
  // column of calculated image values - get the size of the image from the previously generated rf data table
  DataCol* snd_values_col = dt_snd_rf->GetColData("values");
  taVector2i image_size;
  snd_values_col->Get2DCellGeom(image_size.x, image_size.y);
  dt_trg_rf_values_col = dt_trg_rf->FindMakeColMatrix("values", VT_FLOAT, 2, image_size.x, image_size.y);
  // column to keep track of the number of values summed so we can correctly average
  dt_trg_rf_count_col = dt_trg_rf->FindMakeColMatrix("count", VT_INT, 2, image_size.x, image_size.y);
  dt_trg_rf->EnforceRows(wt_rows);
  dt_trg_rf->StructUpdate(false);
  
  float_Matrix* sum_matrix = new float_Matrix(); // sums of sending layer values multiplied by target layer weights
  float_Matrix* tmp_matrix = new float_Matrix(); // multiplication of sending layer values and target layer weights for one connection
  int_Matrix* marker_matrix = new int_Matrix(); // all 1's - used to mark the large count matrix for averaging of summed values
  int_Matrix* count_matrix = new int_Matrix(); // tracks the number of values that are summed for each unit of the original image - use for averaging
  sum_matrix->SetGeom(2, image_size.x, image_size.y);
  tmp_matrix->SetGeom(2, image_size.x, image_size.y);
  marker_matrix->SetGeom(2, image_size.x, image_size.y);
  *marker_matrix = 0;
  count_matrix->SetGeom(2, image_size.x, image_size.y); // same size as dt_trg_rf
  
  bool all_good = true;
//  for (int wts_row=0; wts_row<1; wts_row++) {
  for (int wts_row=0; wts_row<trg_layer_wts->rows; wts_row++) {
    *count_matrix = 0;
    *sum_matrix = 0;
    // get the target layer unit so we can find the sending units it recvs from
    Unit* trg_layer_unit = trg_layer->UnitAccess(Layer::ACC_LAY, wts_row, 0);
    if(TestError(!trg_layer_unit, "ComputeHigherLayerRF", "trg_layer_unit is null")) {
      all_good = false;
    }
    ConGroup* recv_cons = trg_layer_unit->FindRecvConGroupFrom(snd_layer);
    if(TestError(!recv_cons, "ComputeHigherLayerRF", "recv_cons is null")) {
      all_good = false;
    }
    if (all_good) {
      for (int u=0; u<recv_cons->size; u++) {
        Unit* snd_unit = recv_cons->SafeUn(u);
        taVector2i snd_layer_log_pos;
        snd_unit->LayerLogPos(snd_layer_log_pos);
        //              taMisc::DebugInfo((String)snd_layer_log_pos.x, (String)snd_layer_log_pos.y );
        int snd_total_cols = snd_layer_grp_geom.x * snd_layer_unit_grp_geom.x;
        int snd_flat_pos = (snd_layer_log_pos.x * snd_total_cols) + snd_layer_log_pos.y;
        //              taMisc::DebugInfo((String)flat_pos);
        DataCol* wts_col = trg_layer_wts->GetColData(0);  // only one column
        
        float weight = wts_col->GetValAsVarM(wts_row, u).toFloat();
        
        DataCol* dt_snd_rf_values_col = dt_snd_rf->GetColData("values");
        float_Matrix* snd_values_matrix = (float_Matrix*)dt_snd_rf_values_col->GetValAsMatrix(snd_flat_pos);
        taBase::Ref(snd_values_matrix);
        *tmp_matrix = *snd_values_matrix * weight;
        taBase::unRefDone(snd_values_matrix);
        *sum_matrix += *tmp_matrix;
        
        DataCol* dt_snd_rf_count_col = dt_snd_rf->GetColData("count");
        marker_matrix = (int_Matrix*)dt_snd_rf_count_col->GetValAsMatrix(snd_flat_pos);
        taBase::Ref(marker_matrix);
        for (int i=0; i<marker_matrix->size; i++) {
          if (marker_matrix->taMatrix::SafeElAsVar_Flat(i) > 0) {
            marker_matrix->Set_Flat(1, i);
          }
        }
        *count_matrix += marker_matrix;
        taBase::unRefDone(marker_matrix);
      }
      // can't divide by zero
      for (int i=0; i<count_matrix->size; i++) {
        if (count_matrix->taMatrix::SafeElAsFloat_Flat(i) == 0) {
          count_matrix->Set_Flat(1, i);
        }
      }
      // the number of values summed is greater for some cells
      *sum_matrix /= *count_matrix;
      dt_trg_rf_values_col->SetValAsMatrix(sum_matrix, wts_row);
      dt_trg_rf_count_col->SetValAsMatrix(count_matrix, wts_row);
    }
    else {
      delete sum_matrix;
      delete tmp_matrix;
      delete count_matrix;
      return false;
    }
  }
  delete sum_matrix;
  delete tmp_matrix;
  delete count_matrix;
  
  return true;
}
