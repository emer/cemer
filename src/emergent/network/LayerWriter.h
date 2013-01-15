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

#ifndef LayerWriter_h
#define LayerWriter_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <LayerDataEl_List>
#include <LayerWriterEl>
#include <NetworkRef>

// declare all other types mentioned but not required to include:

TypeDef_Of(LayerWriter);

class EMERGENT_API LayerWriter : public taNBase {
  // #STEM_BASE ##CAT_Network ##DEF_CHILD_layer_data #DEF_CHILDNAME_LayerDataEls controls the writing of input data from a data source to network layers
INHERITED(taNBase)
public:
  DataBlockRef		data;
  // the data object with input data to present to the network
  NetworkRef		network;
  // the network to present the input data to
  LayerDataEl_List	layer_data;
  // the layers/input data channel mappings to present to the network

  virtual LayerDataEl*	AddLayerData()	{ return (LayerDataEl*)layer_data.New(1); }
  // #BUTTON add a new layer data item

  virtual void 	SetDataNetwork(DataBlock* db, Network* net);
  // #CAT_LayerWriter set the data table and network pointers -- convenience function for programs 

  virtual void	AutoConfig(bool remove_unused = true);
  // #BUTTON #CAT_LayerWriter do a 'best guess' configuration of items by matching up like-named data Channels and network Layers -- if remove_unused is true, then layer writer elements that existed previously but were not found in input data and network are removed

  virtual bool	ApplyInputData();
  // #CAT_LayerWriter apply data to the layers, using the network's current context settings (TEST,TRAIN,etc) -- returns success

  override taList_impl*	children_() {return &layer_data;}
  override String	GetDisplayName() const;

  TA_SIMPLE_BASEFUNS(LayerWriter);
protected:
  void	UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void CheckChildConfig_impl(bool quiet, bool& rval);
    
private:
  void	Initialize();
  void 	Destroy() {}
};

#endif // LayerWriter_h
