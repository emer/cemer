/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

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
