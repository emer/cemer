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

// cl.h: competitive learning built out of so

#ifndef cl_h
#define cl_h

#include <so/so.h>

class ClConSpec;
class SoftClConSpec;
class ClLayerSpec;
class SoftClUnitSpec;
class SoftClConSpec;

class ClConSpec : public SoConSpec {
  // competitive learning connection spec (uses normalized input activation)
public:
  inline void	C_Compute_dWt(SoCon* cn, SoCon_Group* cg, 
				      Unit* ru, Unit* su);
  inline void 	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute weight change according to Cl function (normalized input acts)

  void 	Initialize()		{ };
  void	Destroy()		{ };
  SIMPLE_COPY(ClConSpec);
  COPY_FUNS(ClConSpec, SoConSpec);
  TA_BASEFUNS(ClConSpec);
};

class SoftClConSpec : public SoConSpec {
  // soft competitive learning connection spec
public:
  inline void		C_Compute_dWt(SoCon* cn, SoCon_Group* cg, 
				      Unit* ru, Unit* su);
  inline virtual void 	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute weight change according to soft Cl function

  void 	Initialize()		{ };
  void	Destroy()		{ };
  SIMPLE_COPY(SoftClConSpec);
  COPY_FUNS(SoftClConSpec, SoConSpec);
  TA_BASEFUNS(SoftClConSpec);
};


class ClLayerSpec : public SoLayerSpec {
  // competitive learning layer spec
public:
  void		Compute_Act(SoLayer* lay);
  // set activation to be 1.0 for unit with most input, 0 else

  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(ClLayerSpec);
};

class SoftClUnitSpec : public SoUnitSpec {
  // soft competitive learning unit spec
public:
  float		var;		// variance of the Gaussian activation function
  float         norm_const;     // #HIDDEN normalization const for Gaussian
  float         denom_const;    // #HIDDEN denominator const for Gaussian

  void 		Compute_Net(Unit* u); // redefine to call compute_dist
  void 		Compute_Act(Unit* u);
  // activation is a gaussian function of the net input

  void  UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(SoftClUnitSpec);
  COPY_FUNS(SoftClUnitSpec, SoUnitSpec);
  TA_BASEFUNS(SoftClUnitSpec);
};

class SoftClLayerSpec : public SoLayerSpec {
  // soft competitive learning layer spec: does a softmax on the units
public:

  void		Compute_Act(SoLayer* lay);
  // set activation to be softmax of unit activations

  void	Initialize();
  void	Destroy()	{ };
  TA_BASEFUNS(SoftClLayerSpec);
};

//////////////////////////////////
//	Inline Functions	//
//////////////////////////////////

inline void ClConSpec::
C_Compute_dWt(SoCon* cn, SoCon_Group* cg, Unit* ru, Unit* su)
{
  cn->dwt += ru->act * ((su->act / cg->sum_in_act) - cn->wt);
}

inline void ClConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  Compute_AvgInAct((SoCon_Group*)cg, ru);
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoCon_Group*)cg, ru, cg->Un(i)));
}

inline void SoftClConSpec::
C_Compute_dWt(SoCon* cn, SoCon_Group*, Unit* ru, Unit* su)
{
  cn->dwt += ru->act * (su->act - cn->wt);
}

inline void SoftClConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoCon_Group*)cg, ru, cg->Un(i)));
}

#endif // cl_h
