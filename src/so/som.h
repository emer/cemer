// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// som.h: self-organizing maps (kohonen nets) built of so

#ifndef som_h
#define som_h

#include <so/so.h>

class SomUnitSpec : public SoUnitSpec {
  // self-organizing feature maps: net input is distance, not raw netin
public:
  void 		Compute_Net(Unit* u); // redefine to call compute_dist

  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_BASEFUNS(SomUnitSpec);
};


class NeighborEl : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER one element of a neighborhood function
public:
  TwoDCoord	off;		// offset from "winning" unit
  float		act_val;	// activation value for this unit

  void	Initialize();
  void	Destroy()	{ };
  void	InitLinks();
  void	Copy_(const NeighborEl& cp);
  COPY_FUNS(NeighborEl, taOBase);
  TA_BASEFUNS(NeighborEl);
};

class NeighborEl_List : public taList<NeighborEl> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of NeighborEl objects
public:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(NeighborEl_List);
};

class SomLayerSpec : public SoLayerSpec {
  // self-organizing map activates a neighborhood of elements
public:
  NeighborEl_List	neighborhood;
  // neighborhood kernel function (determines activations around max unit)
  bool			wrap;
  // whether to wrap the neighborhood function around the output layer or not

  virtual void	KernelEllipse(int half_width, int half_height, int ctr_x, int ctr_y);
  // #MENU_BUTTON #MENU_ON_Kernel make a kernel in the form of an elipse
  virtual void	KernelRectangle(int width, int height, int ctr_x, int ctr_y);
  // #MENU_BUTTON make a kernel in the form of a rectangle
  virtual void	KernelFromNetView(NetView* view);
  // #MENU_BUTTON make kernel from selected units in netview, first unit is center, then other positions

  virtual void  StepKernelActs(float val=1.0);
  // #MENU_BUTTON #MENU_ON_Acts kernel activations are 1.0, others are 0
  virtual void	LinearKernelActs(float scale=1.0);
  // #MENU_BUTTON assign acts as a linear function of distance from center
  virtual void	GaussianKernelActs(float scale=1.0, float sigma=1.0);
  // #MENU_BUTTON assign kernel acts as a Gaussian function of distance from center

  int		WrapClip(int coord, int max_coord);
  // does coordinate wrapping

  void		Compute_Act(SoLayer* lay);
  // set activation as function of kernel

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(SomLayerSpec);
};


#endif // som_h
