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
#include <taVector2f>

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

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Program"; }

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

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Program"; }

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
  enum Attended {
    NO_ATTENDED,                // no constraint on attended or not
    ATTENDED,                   // yes attended (in finst list)
    NOT_ATTENDED,               // not attended (not in finst list)
    NEW_NOT_ATTENDED,           // newly added (within onset span) and not currently attended
  };
  enum Nearest {
    NO_NEAREST,                 // no constraint on nearest
    NEAR_XY,                    // nearest to given x-y coords
    NEAR_X,                     // nearest just to x coord
    NEAR_Y,                     // nearest just to y coord
    NEAR_CLOCKWISE,             // clockwise around center
    NEAR_COUNTERCLOCKWISE,      // counterclockwise around center
  };

  ActrVisParams         params;        // visual system parameters
  ActrAttnParams        attn;           // attentional parameters
  ActrBufferRef         location_buffer; // the visual-location buffer for this module
  ActrChunk_List        visicon;        // current contents of the visual iconic memory -- what is available in the perceptual input display
  ActrChunk_List        objects;        // place where object chunks can be created and stored -- not used directly in requests or other run-time processing, except if referred to by visual location chunks installed into the visicon.  see AddObject and related functions for adding to here and to the visicon.
  ActrChunk_List        finsts;         // #HIDDEN #NO_SAVE list of current declarative fingers of instantiation chunks
  ActrChunk_List        eligible;       // #HIDDEN #NO_SAVE list of all matching chunks eligible
  ActrChunkRef          found;          // #HIDDEN #NO_SAVE final chunk found on last find-location request
  ActrChunkRef          attended;       // #HIDDEN #NO_SAVE currently attended location 
  ActrChunkRef          tracking;       // #HIDDEN #NO_SAVE currently tracked object (vis_obj)
  taVector2f            center;         // center point for clockwise or counterclockwise location specifications -- can be set directly or via params

  virtual void  VisionRequest(ActrEvent& event);
  // #CAT_ActR process a vision request
  virtual void  VisualLocationRequest(ActrEvent& event);
  // #CAT_ActR process a visual_location request
  virtual void  MoveAttentionRequest(ActrEvent& event);
  // #CAT_ActR process a move_attention request
  virtual void  StartTrackingRequest(ActrEvent& event);
  // #CAT_ActR process a start_tracking request
  virtual void  ClearRequest(ActrEvent& event);
  // #CAT_ActR process a clear request
  virtual void  ClearSceneChangeRequest(ActrEvent& event);
  // #CAT_ActR process a clear_scene_change request
  virtual void  AssignFinstRequest(ActrEvent& event);
  // #CAT_ActR process a assign_finst request
  virtual void  EncodingComplete(ActrEvent& event);
  // #CAT_ActR done encoding new location -- activate chunk

  virtual bool  FindMatchingLocation(ActrChunk* ck, Attended attd = NO_ATTENDED, 
                                     Nearest nearest = NO_NEAREST,
                                     float nx = 0, float ny = 0,
                                     float ctrx = 0, float ctry = 0);
  // #CAT_ActR find a location in the visicon that matches parameters of given chunk, plus additional parameters -- put matches in eligible -- returns true if at least one match found
  virtual void  ChooseFromEligibleLocations();
  // #CAT_ActR when multiple items in eligible, choose one based on add time -- ties selected at random

  virtual void  RemoveOldFinsts();
  // #CAT_ActR remove any old finsts (older than finst_span) -- done prior to matching
  virtual void  UpdateFinsts(ActrChunk* attend);
  // #CAT_ActR update finsts to reflect given attended item

  virtual ActrChunk* AddObject(const String& nm, const String& value = "", const String& status = "",
                               const String& color = "", const String& height = "",
                               const String& width = "", const String& typ_nm = "visual_object");
  // #CAT_ActR add object to the objects list with given properties -- updates features if object of that name already exists
  virtual ActrChunk* AddObjToVisIcon(const String& nm, float screen_x, float screen_y, 
                                     float distance, const String& kind = "", 
                                     const String& value = "", const String& status = "",
                                     const String& color = "", const String& height = "",
                                    const String& width = "", const String& typ_nm = "visual_location");
  // #CAT_ActR add object to the objects list with given properties -- updates features if object of that name already exists, and then add it to the visicon with a new visual_location chunk -- kind is text, color, ??
  virtual void   AddToVisIcon(ActrChunk* ck);
  // #CAT_ActR add chunk (must be of type visual_location) to the visicon -- creates a new copy of the input chunk, and sets the t_new to be the current time at adding

  void  InitModule() CPP11_OVERRIDE;
  void  ProcessEvent(ActrEvent& event) CPP11_OVERRIDE;
  bool  ProcessQuery(ActrBuffer* buf, const String& query,
                             bool why_not = false) CPP11_OVERRIDE;
  bool  SetParam(const String& param_nm, Variant par1, Variant par2) CPP11_OVERRIDE;
  void  Init() CPP11_OVERRIDE;

  TA_SIMPLE_BASEFUNS(ActrVisionModule);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrVisionModule_h
