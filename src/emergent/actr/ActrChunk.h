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

#ifndef ActrChunk_h
#define ActrChunk_h 1

// parent includes:
#include <taINBase>

// member includes:
#include <ActrChunkType>
#include <SArg_Array>

#ifndef __MAKETA__
# include <cmath>
#endif

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrChunk);

class E_API ActrChunk : public taINBase {
  // ##INSTANCE ##EDIT_INLINE ##CAT_ActR ##SCOPE_ActrModel a single chunk of memory in ActR
INHERITED(taINBase)
public:
  ActrChunkTypeRef      chunk_type; // the type of this chunk
  SArg_Array            vals;       // the values -- same number as slots in chunk_type
  float                 n_act;      // #READ_ONLY #SHOW number of times chunk has been activated
  float                 t_new;      // #READ_ONLY #SHOW time when chunk was created
  float                 base_act;   // #READ_ONLY #SHOW base level activation = ln(n_act / (1-d)) - d * ln(time - t_new) where time = current time and d = decay parameter (optimized calculation)

  inline float Compute_BaseAct(float time, float decay) {
    if(n_act == 0) base_act = 0.0f;
    else {
      base_act = std::log(n_act / (1.0f - decay)) - decay * std::log(time - t_new);
    }
    return base_act;
  }
  // #CAT_ActR compute the base-level activation as a function of current time and decay parameter

  override String GetTypeDecoKey() const { return "ProgVar"; }

  TA_SIMPLE_BASEFUNS(ActrChunk);
protected:
  void  UpdateAfterEdit_impl();

private:
  void Initialize();
  void Destroy();
};

SmartRef_Of(ActrChunk); // ActrChunkRef

#endif // ActrChunk_h
