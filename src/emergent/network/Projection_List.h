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

#ifndef Projection_List_h
#define Projection_List_h 1

// parent includes:
#include <taList>
#ifdef __TA_COMPILE__
#include "network_def.h"
class Projection;
eTypeDef_Of(Projection);
#else
#include <Projection>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class Layer; //

eTypeDef_Of(Projection_List);

class E_API Projection_List: public taList<Projection> {
  // ##CAT_Network #AKA_Projection_Group group of projections
INHERITED(taList<Projection>)
public:
  bool  send_prjns;     // #NO_SAVE #HIDDEN if true, this is a sending projection group which is just links

  virtual Projection*   ConnectFrom(Layer* lay);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure make a new projection from given layer (can also drag and drop a layer into projections group to call this function)

  virtual Projection*   FindPrjnFrom(Layer* lay);
  // #CAT_Structure find a projection from the given layer (from pointer = lay) -- only makes sense in layers.projections group
  virtual Projection*   FindPrjnTo(Layer* lay);
  // #CAT_Structure find a projection to given recv layer = lay (layer pointer = lay) -- only makes sense in layers.send_prjns group

  String       GetTypeDecoKey() const override { return "Projection"; }

  TA_SIMPLE_BASEFUNS(Projection_List);
private:
  void  Initialize();
  void  Destroy()               { };
};

// needs the actual layer include to compile TA file
#ifdef __TA_COMPILE__
#include <Projection>
#include <Layer>
#endif

#endif // Projection_List_h