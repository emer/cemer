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

#ifndef ActBasedRF_h
#define ActBasedRF_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <DataTable>
#include <float_Matrix>
#include <NetworkRef>
#include <Layer>

// declare all other types mentioned but not required to include:

TypeDef_Of(ActBasedRF);

class EMERGENT_API ActBasedRF : public taNBase {
  // #STEM_BASE ##CAT_Network computes an activation-based receptive field of network unit activations weighted by the activation of target units within selected target layer across a range of testing patterns: effectively shows what patterns of network activation are associated with the activation of target units, regardless of direct connectivity: columns of data table are for each network layer, and rows are the units within the target layer
INHERITED(taNBase)
public:
  enum NormMode {		// how to normalize the final rf data after computing the weighted averages
    NO_NORM,			// no normalization at all
    NORM_TRG_UNIT,		// normalize across each target unit separately: each row of the rf_data table has a max value of 1
    NORM_TRG_LAYER,		// normalize across entire target layer of units: entire rf_data table has a max value of 1
    NORM_RF_LAY,		// normalize across each receptive field layer (i.e., all the layers in the network) separately: each column of the rf_data table has a max value of 1
    NORM_TRG_UNIT_RF_LAY,	// normalize across each target unit for each receptive field layer: each cell of the rf_data table has a max value of 1
  };

  DataTableRef	rf_data;
  // the data table containing the results of the receptive field computation (is completely configured by this object!) -- columns are layers of the network, and rows are units of the trg_layer -- this is computed from sum_data / wt_array followed by normalization
  DataTable	sum_data;
  // #READ_ONLY #HIDDEN #NO_SAVE auxiliary data table in same format as rf_data for holding the sum of target unit activation-weighted activations: rf_data is sum_data / wt_array followed by normalization
  float_Matrix	wt_array;
  // #READ_ONLY #HIDDEN #NO_SAVE array of length = number of units in trg_layer -- contains accumulated weights for each unit
  NetworkRef	network;
  // the network to operate on -- all layers (except lesioned or iconified) are computed, with each layer getting a column of the data table
  LayerRef	trg_layer;
  // #PROJ_SCOPE the target layer to compute receptive fields for: each unit in the layer gets a row of the data table, and the columns in that row show the activation based receptive field for that unit for all the other layers in the network
  NormMode	norm_mode;
  // how to normalize the resulting values
  float		threshold;
  // threshold on absolute value of target unit activation for including in overall average -- can produce sharper results by increasing the threshold to only include cases where the unit is strongly active

  virtual void	ConfigDataTable(DataTable* dt, Network* net);
  // #CAT_ActBasedRF configure data table based on current network (called internally for rf_data, sum_data, and wt_data)

  virtual void	InitData();
  // #BUTTON #CAT_ActBasedRF initialize the data based on current network settings (calls ConfigDataTable on rf_data, sum_data, and wt_data, and intializes all values to 0)

  virtual void 	InitAll(DataTable* dt, Network* net, Layer* tlay);
  // #CAT_ActBasedRF set the data table, network, trg_lay pointers and initialize everything -- convenience function for init_code of programs -- resets all data first and then calls InitData after setting

  virtual bool	IncrementSums();
  // #BUTTON #CAT_ActBasedRF update the receptive field data based on current network activations (computes sum_data and wt_data, does NOT compute rf_data based on those: see ComputeRF)

  virtual bool	ComputeRF();
  // #BUTTON #CAT_ActBasedRF compute the rf_data based on currently accumulated sum_data and wt_data from multiple previous calls to IncrementSums function

  virtual bool	CopyRFtoNetWtPrjn(int trg_unit_no);
  // #BUTTON #CAT_ActBasedRF copy given target unit number's rf_data to the network's wt_prjn field (weight projection), for easy visualization in the network context

  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(ActBasedRF);
protected:
  void	UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
    
private:
  void	Initialize();
  void 	Destroy() {}
};

#endif // ActBasedRF_h
