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
  // the network to operate on
  LayerRef          trg_layer;
  // #PROJ_SCOPE the target layer to compute receptive fields for: each unit in the layer gets a row of the data table. There is a "values" matrix column the same dimensions as the original image that shows the reconstructed image values. A second column for internal use keeps track of the number of values summed so that we can average the values for each pixel of the image.
  DataTableRef      trg_layer_wts;
  // #NO_SAVE auxiliary data table in same format as rf_data for holding the sum of target unit activation-weighted activations: rf_data is sum_data / wt_array followed by normalization
  LayerRef          snd_layer;
  // #PROJ_SCOPE the layer projecting to the target layer.
  RetinaProcRef     v1_retinaProc;
  // #READ_ONLY #HIDDEN #NO_SAVE the retina proc for the v1 layer - needed to get filter values
  DataTable         v1Gabor_GridFilters;
  // #READ_ONLY #HIDDEN #NO_SAVE table of the filter values applied to the image
  DataTableRef      dt_trg_rf;
  // data table that will contain the results of the receptive field computation (is completely configured by this object!) -- one row per unit of the trg_layer.
  DataTableRef      dt_snd_rf;
  // data table containing the results of a previous computation of a wt based receptive field (e.g. This might be V2 and the target now is V3 or V4)
  float             trg_wt_threshold;
  // if the trg_layer_wt is below this value consider the weight 0
  String            GetDisplayName() const override;
  //  virtual void      InitV2RF(Network* net, DataTable* dt_rf, DataTable* wts,  Layer* tlay, Layer* slay, V1RetinaProc* rproc);
  virtual bool      ComputeV2RF(Network* net, DataTable* dt_trg, DataTable* wts, Layer* tlay, Layer* slay, V1RetinaProc* rproc, float wt_threshold = 0);
  // #CAT_WtBasedRF compute the rf_data based on V2 wts and V1 filters
  virtual bool      ComputeHigherLayerRF(Network* net, DataTable* dt_trg, DataTable* dt_snd, DataTable* wts,  Layer* tlay, Layer* slay);
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
