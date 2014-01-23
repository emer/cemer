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

#ifndef Projection_Group_h
#define Projection_Group_h 1

// parent includes:
#include <taGroup>
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

eTypeDef_Of(Projection_Group);

class E_API Projection_Group: public taGroup<Projection> {
  // ##CAT_Network group of projections
INHERITED(taGroup<Projection>)
public:
  bool  send_prjns;     // #NO_SAVE #HIDDEN if true, this is a sending projection group which is just links

  virtual Projection*   ConnectFrom(Layer* lay);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure make a new projection from given layer (can also drag and drop a layer into projections group to call this function)

  String       GetTypeDecoKey() const override { return "Projection"; }

  void  SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);
  TA_SIMPLE_BASEFUNS(Projection_Group);
private:
  void  Initialize()            { SetBaseType(&TA_Projection); send_prjns = false; }
  void  Destroy()               { };
};

// needs the actual layer include to compile TA file
#ifdef __TA_COMPILE__
#include <Projection>
#include <Layer>
#endif

#endif // Projection_Group_h
