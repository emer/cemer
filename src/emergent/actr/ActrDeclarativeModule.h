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

#ifndef ActrDeclarativeModule_h
#define ActrDeclarativeModule_h 1

// parent includes:
#include <ActrModule>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(DeclarativeParams);

class E_API DeclarativeParams : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR declarative memory parameters
INHERITED(taOBase)
public:
  // todo: params from actr6
  //  blc ans pas lf le mp ms md rt bll mas 

  override String       GetTypeDecoKey() const { return "Program"; }

  TA_SIMPLE_BASEFUNS(DeclarativeParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};


eTypeDef_Of(ActrDeclarativeModule);

class E_API ActrDeclarativeModule : public ActrModule {
  // declarative memory module
INHERITED(ActrModule)
public:
  ActrChunk_List        init_chunks; // initial chunks to start pre-loaded into declarative memory
  ActrChunk_List        active; // #NO_EXPAND_ALL the current chunks active in declarative memory
  ActrChunk_List        eligible; // #HIDDEN #NO_SAVE list of all matching chunks eligible for retrieval
  ActrChunkRef          retrieved;  // #HIDDEN #NO_SAVE final chunk retrieved on last retrieval

  virtual bool  AddChunk(ActrChunk* ck, bool merge = true);
  // #CAT_ActR add a new chunk to declarative memory active buffer, if merge is true, chunk is merged with any existing ones, and base activation is increased
  virtual bool FindMatching(ActrChunk* ck);
  // #CAT_ActR find all chunks that match given chunk, put in eligible list
  virtual float  UpdateBaseAct(ActrChunk* ck);
  // #CAT_ActR update the base level activation given that it was used..

  virtual void  RetrievalRequest(ActrEvent& event);
  // #CAT_ActR process a retrieval request from given chunk probe
  virtual void  RetrievedChunk(ActrEvent& event);
  // #CAT_ActR after proper amount of time has passed, we register the retrieval
  virtual void  RetrievalFailure(ActrEvent& event);
  // #CAT_ActR after proper amount of time has passed, we register the retrieval failure

  override void  InitModule();
  override void  ProcessEvent(ActrEvent& event);
  override void  Init();

  TA_SIMPLE_BASEFUNS(ActrDeclarativeModule);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrDeclarativeModule_h
