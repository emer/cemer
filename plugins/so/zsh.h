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

// zsh.h: zero-sum-hebbian built out of so

#ifndef zsh_h
#define zsh_h

#include "so.h"

class SO_API ZshConSpec : public SoConSpec {
  // zero-sum-hebbian (subtractive normalization) learning
public:
  bool		soft_wt_bound;
  // soft weight bounding *(1-wt) for inc, *wt for dec

  inline void	C_Compute_dWt(SoCon* cn, SoCon_Group* cg, 
				      Unit* ru, Unit* su);
  inline void 	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute weight change according to Zsh function

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(ZshConSpec);
  COPY_FUNS(ZshConSpec, SoConSpec);
  TA_BASEFUNS(ZshConSpec);
};

// MaxIn is an algorithm developed in O'Reilly, 1994, which is based
// on units that have a signal-to-noise ratio activation function
// and maximize this ratio over learning.  It basically amounts to 
// Zsh plus SoftCl, though the actual derivative for MaxIn dynamically
// weights the Zsh-like term, which is missing in this version

class SO_API MaxInConSpec : public ZshConSpec {
  // approximation to MaxIn (Zsh + SoftCl)
public:
  float		k_scl;
  // strength of the soft-competitive learning component

  inline void	C_Compute_dWt(SoCon* cn, SoCon_Group* cg, 
				      Unit* ru, Unit* su);
  inline void 	Compute_dWt(Con_Group* cg, Unit* ru);
  // compute weight change according to approximate MaxIn function

  void 	Initialize();
  void	Destroy()		{ };
  SIMPLE_COPY(MaxInConSpec);
  COPY_FUNS(MaxInConSpec, ZshConSpec);
  TA_BASEFUNS(MaxInConSpec);
};



//////////////////////////////////
//	Inline Functions	//
//////////////////////////////////

inline void ZshConSpec::
C_Compute_dWt(SoCon* cn, SoCon_Group* cg, Unit* ru, Unit* su) {
  float tmp = ru->act * (su->act - cg->avg_in_act);
  if(soft_wt_bound) {
    if(tmp > 0)			
      tmp *= wt_limits.max - cn->wt;
    else
      tmp *= cn->wt - wt_limits.min; 
  }
  cn->dwt += tmp;
}

inline void ZshConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  Compute_AvgInAct((SoCon_Group*)cg, ru);
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoCon_Group*)cg, ru, cg->Un(i)));
}

inline void MaxInConSpec::
C_Compute_dWt(SoCon* cn, SoCon_Group* cg, Unit* ru, Unit* su) {
  float tmp = ru->act * (su->act - cg->avg_in_act) +
    k_scl * ru->act * (su->act - cn->wt);
  if(soft_wt_bound) {
    if(tmp > 0)			
      tmp *= wt_limits.max - cn->wt;
    else
      tmp *= cn->wt - wt_limits.min;
  }
  cn->dwt += tmp;
}

inline void MaxInConSpec::Compute_dWt(Con_Group* cg, Unit* ru) {
  Compute_AvgInAct((SoCon_Group*)cg, ru);
  CON_GROUP_LOOP(cg, C_Compute_dWt((SoCon*)cg->Cn(i), 
				   (SoCon_Group*)cg, ru, cg->Un(i)));
}

#endif // zsh_h
