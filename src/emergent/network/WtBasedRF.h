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

#ifndef WtBasedRF_h
#define WtBasedRF_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:
#include <DataTable>
#include <float_Matrix>
#include <Network>
#include <RetinaProc>

// declare all other types mentioned but not required to include:

eTypeDef_Of(WtBasedRF);

class E_API WtBasedRF : public taNBase {
  // #STEM_BASE ##CAT_Network
INHERITED(taNBase)
public:
  NetworkRef        network;
  // the network to operate on -- all layers (except lesioned or iconified) are computed, with each layer getting a column of the data table
  LayerRef          trg_layer;
  // #PROJ_SCOPE the target layer to compute receptive fields for: each unit in the layer gets a row of the data table. There is a single matrix column the same dimensions as the original image and shows the reconstructed image values.
  LayerRef          snd_layer;
  // #PROJ_SCOPE the layer projecting to the target layer.
  RetinaProcRef     v1_retinaProc;
  // #READ_ONLY #HIDDEN #NO_SAVE the retina proc for the v1 layer - needed to get filter values
  
  DataTable         v1Gabor_GridFilters;
  // #READ_ONLY #HIDDEN #NO_SAVE table of the filter values applied to the image
  DataTableRef      trg_layer_wts;
  // #NO_SAVE auxiliary data table in same format as rf_data for holding the sum of target unit activation-weighted activations: rf_data is sum_data / wt_array followed by normalization
  DataTableRef      rf_data;
  // data table containing the results of the receptive field computation (is completely configured by this object!) -- one row per unit of the trg_layer

  String            GetDisplayName() const override;
  virtual void      InitAll(DataTable* dt, Network* net, Layer* tlay, Layer* slay, V1RetinaProc* rproc, DataTable* wts);

  virtual bool      ComputeRF();
  // #BUTTON #CAT_WtBasedRF compute the rf_data based on V2 wts and V1 filters
  virtual bool      ComputeV2RF();
  // #CAT_WtBasedRF compute the rf_data based on V2 wts and V1 filters
  virtual bool      ComputeHigherLayerRF();
  // #CAT_WtBasedRF compute the rf_data for layers beyond V2. These calculations use the RF computations from all layers below (e.g. the V3 representational analysis uses the values computed for V2)
  
  TA_SIMPLE_BASEFUNS(WtBasedRF);
protected:
  void	UpdateAfterEdit_impl();
  void	CheckThisConfig_impl(bool quiet, bool& rval) override;

private:
  void Initialize();
  void Destroy()     { };
};

#endif // WtBasedRF_h
