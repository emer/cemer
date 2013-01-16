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

#ifndef LayerDataEl_List_h
#define LayerDataEl_List_h 1

// parent includes:
#include "network_def.h"
#include <taList>
#include <LayerDataEl>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(LayerDataEl_List);

class EMERGENT_API LayerDataEl_List: public taList<LayerDataEl> {
  // ##CAT_Network list of individual LayerDataEl objects
INHERITED(taList<LayerDataEl>)
public:

  virtual void 	SetDataNetwork(DataBlock* db, Network* net);
  // #CAT_LayerData set the data table and network pointers enable looking up columns/layer names

  virtual LayerDataEl* FindChanName(const String& chn_name);
  // #CAT_LayerData find (first) layer data that applies to given data channel name
  virtual LayerDataEl* FindMakeChanName(const String& chn_name, bool& made_new);
  // #CAT_LayerData find (first) layer data that applies to given data channel name -- make it if it doesn't exist

  virtual LayerDataEl* FindLayerName(const String& lay_name);
  // #CAT_LayerData find (first) layer data that applies to given layer name
  virtual LayerDataEl* FindMakeLayerName(const String& lay_name, bool& made_new);
  // #CAT_LayerData find (first) layer data that applies to given layer name -- make it if it doesn't exist 

  virtual LayerDataEl* FindLayerData(const String& chn_name, const String& lay_name);
  // #CAT_LayerData find (first) layer data that applies to given data channel name and layer name
  virtual LayerDataEl* FindMakeLayerData(const String& chn_name, const String& lay_name,
					 bool& made_new);
  // #CAT_LayerData find (first) layer data that applies to given data channel name and layer name -- make it if it doesn't exist

  TA_BASEFUNS_NOCOPY(LayerDataEl_List);
private:
  void	Initialize() { SetBaseType(&TA_LayerDataEl); }
  void 	Destroy() {}
};

#endif // LayerDataEl_List_h
