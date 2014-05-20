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

#ifndef LayerWriterEl_h
#define LayerWriterEl_h 1

// parent includes:
#include <LayerDataEl>

// member includes:
#include <RandomSpec>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LayerWriterEl);

class E_API LayerWriterEl : public LayerDataEl {
  // #STEM_BASE controls the writing of input data from a data source to a network layer
INHERITED(LayerDataEl)
public: 
  bool		use_layer_type; // #DEF_true use layer_type information on the layer to determine flags to set (if false, turn on EXPERT showing to view flags)
  bool		quiet;		// turn off warning messages (e.g., about layer_type = HIDDEN)
  bool		na_by_range;	// #EXPERT use act_range on the unitspec for the units to determine inputs that are not appplicable (n/a) and thus do not get relevant flags or values set: those that have input values outside the range are n/a
  Unit::ExtType	ext_flags;	// #EXPERT #CONDSHOW_OFF_use_layer_type:true how to flag the unit/layer's external input status
  RandomSpec	noise;		// #EXPERT noise optionally added to values when applied

  virtual bool	ApplyInputData(DataTable* db, Network* net);
  // #CAT_LayerWriter apply data to the layer on network, using the network's current context settings (TEST,TRAIN,etc) -- returns success

  TA_SIMPLE_BASEFUNS(LayerWriterEl);
protected:
  void	CheckThisConfig_impl(bool quiet, bool& rval) override;

private:
  void	Initialize();
  void 	Destroy();
};

#endif // LayerWriterEl_h
