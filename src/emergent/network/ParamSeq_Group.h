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

#ifndef ParamSeq_Group_h
#define ParamSeq_Group_h 1

// parent includes:
#include "network_def.h"
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class ParamSeq; // 

eTypeDef_Of(ParamSeq_Group);

class E_API ParamSeq_Group : public taGroup<ParamSeq> {
  // #EXPAND_DEF_2 group of parameter sequences, where each parameter sequence defines parameters that are set at a particular epoch 
INHERITED(taGroup<ParamSeq>)
public:

  String          GetTypeDecoKey() const override { return "ControlPanel"; }

  virtual bool    SetParamsAtEpoch(int epoch);
  // #BUTTON set all parameter sequence values at given epoch -- returns true if any changed
 
  TA_BASEFUNS_NOCOPY(ParamSeq_Group);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ParamSeq_Group_h
