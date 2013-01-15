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

#include "LayerDataEl_List.h"

void LayerDataEl_List::SetDataNetwork(DataBlock* db, Network* net) {
  for(int i = 0; i < size; ++i) {
    LayerDataEl* it = FastEl(i);
    it->SetDataNetwork(db, net);
  }
}

LayerDataEl* LayerDataEl_List::FindChanName(const String& cnm) {
  for(int i = 0; i < size; ++i) {
    LayerDataEl* it = FastEl(i);
    if(it->chan_name == cnm) return it;
  }
  return NULL;
}

LayerDataEl* LayerDataEl_List::FindMakeChanName(const String& cnm, bool& made_new) {
  made_new = false;
  LayerDataEl* ld = FindChanName(cnm);
  if(ld) return ld;
  ld = (LayerDataEl*)New(1);
  ld->chan_name = cnm;
  made_new = true;
  return ld;
}

LayerDataEl* LayerDataEl_List::FindLayerName(const String& lnm) {
  for(int i = 0; i < size; ++i) {
    LayerDataEl* it = FastEl(i);
    if(it->layer_name == lnm) return it;
  }
  return NULL;
}

LayerDataEl* LayerDataEl_List::FindMakeLayerName(const String& lnm, bool& made_new) {
  made_new = false;
  LayerDataEl* ld = FindLayerName(lnm);
  if(ld) return ld;
  ld = (LayerDataEl*)New(1);
  ld->layer_name = lnm;
  made_new = true;
  return ld;
}

LayerDataEl* LayerDataEl_List::FindLayerData(const String& cnm, const String& lnm) {
  for(int i = 0; i < size; ++i) {
    LayerDataEl* it = FastEl(i);
    if((it->layer_name == lnm) && (it->chan_name == cnm)) return it;
  }
  return NULL;
}

LayerDataEl* LayerDataEl_List::FindMakeLayerData(const String& cnm, const String& lnm,
                                                 bool& made_new) {
  made_new = false;
  LayerDataEl* ld = FindLayerData(cnm, lnm);
  if(ld) return ld;
  ld = (LayerDataEl*)New(1);
  ld->chan_name = cnm;
  ld->layer_name = lnm;
  made_new = true;
  return ld;
}

