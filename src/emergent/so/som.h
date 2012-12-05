// Copyright, 1995-2007, Regents of the University of Colorado,
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



// som.h: self-organizing maps (kohonen nets) built of so

#ifndef som_h
#define som_h

#include "so.h"

class SomUnitSpec : public SoUnitSpec {
  // self-organizing feature maps: net input is distance, not raw netin
INHERITED(SoUnitSpec)
public:
  override void Compute_Netin(Unit* u, Network* net, int thread_no=-1);
  // redefine to call compute_dist

  TA_BASEFUNS_NOCOPY(SomUnitSpec);
private:
  void	Initialize()	{ };
  void	Destroy()	{ };
};


class NeighborEl : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER one element of a neighborhood function
INHERITED(taOBase)
public:
  taVector2i	off;		// offset from "winning" unit
  float		act_val;	// activation value for this unit

  TA_SIMPLE_BASEFUNS(NeighborEl);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class NeighborEl_List : public taList<NeighborEl> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of NeighborEl objects
INHERITED(taList<NeighborEl>)
public:
  NOCOPY(NeighborEl_List);
  TA_BASEFUNS(NeighborEl_List);
private:
  void	Initialize() 		{ SetBaseType(&TA_NeighborEl); }
  void 	Destroy()		{ };
};

class SomLayerSpec : public SoLayerSpec {
  // self-organizing map activates a neighborhood of elements
INHERITED(SoLayerSpec)
public:
  NeighborEl_List	neighborhood;
  // neighborhood kernel function (determines activations around max unit)
  bool			wrap;
  // whether to wrap the neighborhood function around the output layer or not

  virtual void	KernelEllipse(int half_width, int half_height, int ctr_x, int ctr_y);
  // #MENU_BUTTON #MENU_ON_Kernel make a kernel in the form of an elipse
  virtual void	KernelRectangle(int width, int height, int ctr_x, int ctr_y);
  // #MENU_BUTTON make a kernel in the form of a rectangle
//   virtual void	KernelFromNetView(NetView* view);
  // #MENU_BUTTON make kernel from selected units in netview, first unit is center, then other positions

  virtual void  StepKernelActs(float val=1.0);
  // #MENU_BUTTON #MENU_ON_Acts kernel activations are 1.0, others are 0
  virtual void	LinearKernelActs(float scale=1.0);
  // #MENU_BUTTON assign acts as a linear function of distance from center
  virtual void	GaussianKernelActs(float scale=1.0, float sigma=1.0);
  // #MENU_BUTTON assign kernel acts as a Gaussian function of distance from center

  int		WrapClip(int coord, int max_coord);
  // does coordinate wrapping

  override void	Compute_Act_post(SoLayer* lay, SoNetwork* net);
  // set activation as function of kernel

  TA_SIMPLE_BASEFUNS(SomLayerSpec);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};


#endif // som_h
