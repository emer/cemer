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

#ifndef ActrVisionModule_h
#define ActrVisionModule_h 1

// parent includes:
#include <ActrModule>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrVisParams);

class E_API ActrVisParams : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR vision parameters
INHERITED(taOBase)
public:
  int           n_finst;       // #DEF_4 :visual-num-finsts in ACT-R -- number of 'fingers of instantiation' for visual system
  float         finst_span;    // #DEF_3 :visual-finst-span in ACT-R -- how long a 'fingers of instantiation' persists for visual system
  float         onset_span;    // #DEF_0.5 :visual-onset-span in ACT-R -- how long an item recently added to the visicon will be marked as new, and how long a scene change notice will be available
  float         move_tol;      // #DEF_0.5 :visual-movement-tolerance in ACT-R -- how far an object can move and still be considered the same object, in degrees of visual angle

  override String       GetTypeDecoKey() const { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrVisParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ActrAttnParams);

class E_API ActrAttnParams : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR attention parameters
INHERITED(taOBase)
public:
  bool          auto_attend;    // :auto-attend in ACT-R -- if true, visual-location requests result in an automatic move-attention action to the found location, at a latency of 50ms -- saves one additional production
  float         latency;        // #DEF_0.085 :visual-attention-latency in ACT-R -- how long a visual attention shift will take, in seconds
  float         scene_chg_thresh; // #DEF_0.25 :scene-change-threshold in ACT-R -- the smallest proportion of change in the visicon that results in a signal that the scene has changed -- must be in range 0..1

  override String       GetTypeDecoKey() const { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrAttnParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ActrVisionModule);

class E_API ActrVisionModule : public ActrModule {
  // the ACT-R vision module, supporting the visual and visual-location buffers
INHERITED(ActrModule)
public:
  ActrVisParams         params;        // visual system parameters
  ActrAttnParams        attn;           // attentional parameters
  ActrBufferRef         location_buffer; // the visual-location buffer for this module
  ActrChunk_List        visicon;        // current contents of the visual iconic memory -- what is available in the perceptual input display
  ActrChunk_List        finsts;         // #HIDDEN #NO_SAVE list of current declarative fingers of instantiation chunks
  ActrChunk_List        eligible;       // #HIDDEN #NO_SAVE list of all matching chunks eligible
  ActrChunkRef          found;          // #HIDDEN #NO_SAVE final chunk found on last find-location request
  String                last_cmd;       // #HIDDEN #NO_SAVE #SHOW last command (chunk type) executed by module

  virtual void  VisionRequest(ActrEvent& event);
  // #CAT_ActR process a vision request
  virtual void  VisualLocationRequest(ActrEvent& event);
  // #CAT_ActR process a visual_location request
  virtual void  MoveAttentionRequest(ActrEvent& event);
  // #CAT_ActR process a move_attention request
  virtual void  EncodingComplete(ActrEvent& event);
  // #CAT_ActR done encoding new location -- activate chunk

  virtual bool  FindMatchingLocation(ActrChunk* ck);
  // #CAT_ActR find a location in the visicon that matches parameters of given chunk -- put matches in eligible -- returns true
  virtual void  ChooseFromEligibleLocations();
  // #CAT_ActR when multiple items in eligible, choose one based on add time -- ties selected at random

  virtual void  RemoveOldFinsts();
  // #CAT_ActR remove any old finsts (older than finst_span) -- done prior to matching
  virtual void  UpdateFinsts(ActrChunk* attend);
  // #CAT_ActR update finsts to reflect given attended item

  virtual void   AddToVisIcon(ActrChunk* ck);
  // #CAT_ActR add item to the visicon -- sets the t_new to be the current time at adding

  override void  InitModule();
  override void  ProcessEvent(ActrEvent& event);
  override bool  ProcessQuery(ActrBuffer* buf, const String& query,
                             bool why_not = false);
  override bool  SetParam(const String& param_nm, Variant par1, Variant par2);
  override void  Init();

  TA_SIMPLE_BASEFUNS(ActrVisionModule);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrVisionModule_h
