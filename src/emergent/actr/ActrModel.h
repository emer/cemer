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

#ifndef ActrModel_h
#define ActrModel_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ActrChunkType_List>
#include <ActrModule_List>
#include <ActrBuffer_List>
#include <ActrProduction_Group>


// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrModel);

class E_API ActrModel : public taNBase {
  // ##INSTANCE ##CAT_ActR a complete ACT-R model, including productions, buffers, chunks, etc
INHERITED(taNBase)
public:
  String                desc;  // #EDIT_DIALOG #HIDDEN_INLINE description of this model
  float                 util_lrate; // #DEF_0.2 production utility learning rate (alpha)
  float                 util_noise; // noise value (sigma) for the production selection process
  float                 prod_time;  // #DEF_0.050 how many seconds does one production take to fire -- default is 50ms
  float                 cur_time;   // #READ_ONLY #SHOW current time in the model

  ActrChunkType_List    chunk_types;  // all chunk types used within the model must be defined here
  ActrModule_List       modules;      // modules -- always contains declarative as the first one, and optional other ones
  ActrBuffer_List       buffers;      // buffers for containing active chunks -- always contains at least retrieval and goal buffers, and others according to modules
  ActrProduction_Group  productions;  // all the productions defined for the model

  ActrBufferRef         goal_buf;     // #READ_ONLY our goal buffer
  ActrModuleRef         decl_mod;     // #READ_ONLY our declarative module

  virtual void          Init() { }; // #BUTTON initialize model at the start
  virtual void          Step() { }; // #BUTTON run one step of processing

  virtual ActrProduction* SelectNextProduction() { return NULL; }
  // #CAT_ActR find next production to fire..

  virtual void          DefaultConfig();
  // ensure that we have at least the basic default config

  override String       GetDesc() const {return desc;}

  TA_SIMPLE_BASEFUNS(ActrModel);
protected:
  void UpdateAfterEdit_impl(); 

private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrModel_h
