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

#ifndef ActrImaginalModule_h
#define ActrImaginalModule_h 1

// parent includes:
#include <ActrModule>

// member includes:
#include <Random>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrImaginalModule);

class E_API ActrImaginalModule : public ActrModule {
  // imaginal module -- responsible for maintaining task relevant context information -- chunk creation and modification both take time
INHERITED(ActrModule)
public:
  Random         imaginal_delay; // time in seconds it takes for a request or modification to the imaginal buffer to complete -- default is non-random, .2 secons
  ActrBufferRef  action_buffer; // the imaginal-action buffer for this module

  virtual void   ImaginalRequest(ActrEvent& event);
  // #CAT_ActR process imaginal buffer request
  virtual void   CreateNewChunk(ActrEvent& event);
  // #CAT_ActR create a new chunk -- delayed
  virtual void   SetBufferChunk(ActrEvent& event);
  // #CAT_ActR set buffer to new chunk
  virtual void   ImaginalBufferMod(ActrEvent& event);
  // #CAT_ActR process imaginal buffer modification
  virtual void   ModImaginalChunk(ActrEvent& event);
  // #CAT_ActR actually do the modifcation -- delayed

  override void  InitModule();
  override void  ProcessEvent(ActrEvent& event);
  override bool  SetParam(const String& param_nm, Variant par1, Variant par2);
  override void  Init();

  TA_SIMPLE_BASEFUNS(ActrImaginalModule);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrImaginalModule_h
