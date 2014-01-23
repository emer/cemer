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

#ifndef TesselPrjnSpec_h
#define TesselPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:
#include <taVector2i>
#include <taVector2f>

// declare all other types mentioned but not required to include:

eTypeDef_Of(TessEl);

class E_API TessEl : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec one element of a tesselation specification
INHERITED(taOBase)
public:
  taVector2i	send_off;	// offset from current receiving unit
  float		wt_val;		// value to assign to weight

  TA_SIMPLE_BASEFUNS(TessEl);
private:
  void	Initialize();
  void	Destroy()	{ };
};

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
  taVector2i	recv_off;	// offset in layer for start of recv units to begin connecting -- can leave some unconnected units around the edges if desired
  taVector2i	recv_n;		// number of receiving units to connect in each dimension (-1 for all)
  taVector2i	recv_skip;	// increment for recv units in each dimension -- 1 = connect all units; 2 = skip every other unit, etc
  taVector2i	recv_group;	// group together this many units under the same starting coord, resulting in a tile pattern -- each member of the group (which needn't correspond to an actual unit group in the recv layer) gets the same effective location as the first member of the group
  bool		wrap;		// whether to wrap coordinates around in the sending layer (e.g., if it goes past the right side, then it continues back on the left).  otherwise it will clip off connections at the edges.  Any clipping will affect the ability to initialize weight patterns properly, so it is best to avoid that.
  taVector2f send_scale;	// scale to apply to transform receiving unit coords into sending unit coords -- can use this to compensate for differences in the sizes between the two layers
  taVector2f send_off;	// #AKA_send_border constant offset to add to sending offsets relative to the position of the receiving unit -- this just adds a constant to the specific send_offs that specify the detailed pattern of connectivity
  TessEl_List	send_offs;	// offsets of the sending units -- these are added to the location of the recv unit to determine which sending units to receive from -- can create any arbitrary patterns here, or use the MakeEllipse or MakeRectangle buttons to create those std patterns

  String	last_make_cmd; // #READ_ONLY #SHOW shows the last Make.. command that was run (if blank, none or it was done prior to the addition of this feature in version 4.1.0) -- useful for modifying later
  String	last_weights_cmd; // #READ_ONLY #SHOW shows the last Weights.. command that was run (if blank, none or it was done prior to the addition of this feature in version 4.1.0) -- useful for modifying later

  void	Connect_impl(Projection* prjn) override;
  void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) override;
  // uses weight values as specified in the tesselel's

  virtual void	GetCtrFmRecv(taVector2i& sctr, taVector2i ruc);
  // get center of sender coords from receiving coords
  virtual void	Connect_RecvUnit(Unit* ru_u, const taVector2i& ruc, Projection* prjn,
				 bool send_alloc);
  // connects one recv unit to all senders

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

  TA_SIMPLE_BASEFUNS(TesselPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // TesselPrjnSpec_h
