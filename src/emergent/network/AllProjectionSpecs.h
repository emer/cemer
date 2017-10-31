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

#ifndef AllProjectionSpecs_h
#define AllProjectionSpecs_h 1

#include <ProjectionSpec>

#include <RndSeed>
#include <taVector2i>
#include <taVector2f>

#include <State_main>

// need to define all the ones for _mbrs here
eTypeDef_Of(TessEl);

#include <AllProjectionSpecs_mbrs>

#include <State_main>

eTypeDef_Of(FullPrjnSpec);

class E_API FullPrjnSpec : public ProjectionSpec {
  // Full connectivity between layers
INHERITED(ProjectionSpec)
public:

#include <FullPrjnSpec>
  
  TA_BASEFUNS_NOCOPY(FullPrjnSpec);
private:
  void	Initialize() 		{ Initialize_core(); }
  void 	Destroy()		{ };
};

eTypeDef_Of(OneToOnePrjnSpec);

class E_API OneToOnePrjnSpec : public ProjectionSpec {
  // one-to-one connectivity (1st unit to 1st unit, etc)
INHERITED(ProjectionSpec)
public:

#include <OneToOnePrjnSpec>
  
  TA_SIMPLE_BASEFUNS(OneToOnePrjnSpec);
private:
  void	Initialize()            { Initialize_core(); SetUnique("self_con", true); self_con = true; }
  void 	Destroy()		{ };
};

eTypeDef_Of(RandomPrjnSpec);

class E_API RandomPrjnSpec : public ProjectionSpec {
  // Connects all units with probability p_con -- note it ALWAYS uses the same seed, because of the two-pass nature of the connection process -- you can update rndm_seed prior to connecting to get different patterns of connectivity
INHERITED(ProjectionSpec)
public:

#include <RandomPrjnSpec>
  
  TA_SIMPLE_BASEFUNS(RandomPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override {
    inherited::UpdateAfterEdit_impl();
    if(p_con > 1.0f) p_con = 1.0f;
    if(p_con < 0.0f) p_con = 0.0f;
  }
   
private:
  void	Initialize()    { Initialize_core(); }
  void 	Destroy()	{ };
};

eTypeDef_Of(UniformRndPrjnSpec);

class E_API UniformRndPrjnSpec : public ProjectionSpec {
  // Uniform random connectivity between layers -- only 'permute' stye randomness is supported, creates same number of connections per unit
INHERITED(ProjectionSpec)
public:

#include <UniformRndPrjnSpec>
  
  TA_SIMPLE_BASEFUNS(UniformRndPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override {
    inherited::UpdateAfterEdit_impl();
    if(p_con > 1.0f) p_con = 1.0f;
    if(p_con < 0.0f) p_con = 0.0f;
  }
   
private:
  void	Initialize()    { Initialize_core(); }
  void 	Destroy()	{ };
};


////////////////////
//      Tessel 

eTypeDef_Of(TessEl_List);

class E_API TessEl_List : public taList<TessEl> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec list of TessEl objects
INHERITED(taList<TessEl>)
public:
  TA_BASEFUNS_LITE_NOCOPY(TessEl_List);
private:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
};

eTypeDef_Of(TesselPrjnSpec);

class E_API TesselPrjnSpec : public ProjectionSpec {
  // arbitrary tesselations (repeating patterns) of connectivity -- sweeps over receiving units and connects with sending units based on projection of recv unit position into sending layer, plus sending offsets that specify the connectivity pattern
INHERITED(ProjectionSpec)
public:

#include <TesselPrjnSpec>

  String	last_make_cmd; // #READ_ONLY #SHOW shows the last Make.. command that was run (if blank, none or it was done prior to the addition of this feature in version 4.1.0) -- useful for modifying later
  String	last_weights_cmd; // #READ_ONLY #SHOW shows the last Weights.. command that was run (if blank, none or it was done prior to the addition of this feature in version 4.1.0) -- useful for modifying later
  TessEl_List	send_offs;	// offsets of the sending units -- these are added to the location of the recv unit to determine which sending units to receive from -- can create any arbitrary patterns here, or use the MakeEllipse or MakeRectangle buttons to create those std patterns

  virtual void	MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y);
  // #BUTTON #MENU_SEP_BEFORE make a connection pattern in the form of an elipse: center is located at ctr_x,y and extends half_width and half_height therefrom
  virtual void	MakeRectangle(int width, int height, int left, int bottom);
  // #BUTTON make a connection pattern in the form of a rectangle starting at left, bottom coordinate and going right and up by width, height
//TODO  virtual void	MakeFromNetView(NetView* view);
  /* #MENU make connection pattern from selected units in netview,
     first unit is center, then sender positions */
  virtual void	WeightsFromDist(float scale);
  // #BUTTON assign weights as a linear function of sender distance
  virtual void	WeightsFromGausDist(float scale, float sigma);
  // #BUTTON assign weights as a Gaussian function of sender distance

  virtual void  SyncSendOffs();
  // #IGNORE sync our dynamic send_offs array with the compiled version used by actual State projection code
  virtual void  CopyToState_SendOffs(void* state_spec, const char* state_suffix);
  // #IGNORE copy our compiled send offs data to other state spec

  void    CopyToState(void* state_spec, const char* state_suffix) override;
  void    UpdateStateSpecs() override;
  
  TA_SIMPLE_BASEFUNS(TesselPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
private:
  void	Initialize() { Initialize_core(); send_offs.SetBaseType(&TA_TessEl); }
  void	Destroy()    { CutLinks(); FreeSendOffs(); }
};

#endif // AllProjectionSpecs_h
