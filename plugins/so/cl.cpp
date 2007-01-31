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

#include "cl.h"

void ClLayerSpec::Initialize() {
  netin_type = MAX_NETIN_WINS;
}

void ClLayerSpec::Compute_Act(SoLayer* lay) {
  if(lay->ext_flag & Unit::EXT) { // input layer
    SoLayerSpec::Compute_Act(lay);
    return;
  }
  if(lay->units.leaves == 0)
    return;
  
  SoUnit* win_u = FindWinner(lay);
  if(win_u == NULL) return;

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.spec;

  float lvcnt = (float)lay->units.leaves;
  lay->avg_act = (uspec->act_range.max / lvcnt) +
    (((lvcnt - 1.0f) * uspec->act_range.min) / lvcnt);

  win_u->act = uspec->act_range.max;
  win_u->act_i = lay->avg_act;	// this is the rescaled value..
}

void SoftClUnitSpec::Initialize() {
  var = 1.0f;
  norm_const = 1.0f / sqrtf(2.0f * 3.14159265358979323846 * var);
  denom_const = 0.5f / var;
}

void SoftClUnitSpec::UpdateAfterEdit() {
  SoUnitSpec::UpdateAfterEdit();
  norm_const = 1.0f / sqrtf(2.0f * 3.14159265358979323846 * var);
  denom_const = 0.5f / var;
}

void SoftClUnitSpec::Compute_Net(Unit* u) {
  // do distance instead of net input
  u->net = 0.0f;
  Con_Group* recv_gp;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, u->recv., g)
    u->net += recv_gp->Compute_Dist(u);
}

void SoftClUnitSpec::Compute_Act(Unit* u) {
  SoUnit* su = (SoUnit*)u;
  if(u->ext_flag & Unit::EXT)
    su->act = su->act_i = u->ext;
  else
    su->act = su->act_i = norm_const * expf(-denom_const * su->net);
}

void SoftClLayerSpec::Initialize() {
  netin_type = MIN_NETIN_WINS;	// not that this is actually used..
}

void SoftClLayerSpec::Compute_Act(SoLayer* lay) {
  if(lay->ext_flag & Unit::EXT) { // input layer
    SoLayerSpec::Compute_Act(lay);
    return;
  }

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.spec;

  float sum = 0.0f;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i) {
    u->Compute_Act();
    sum += u->act;
  }

  FOR_ITR_EL(Unit, u, lay->units., i) {
    u->act = uspec->act_range.Project(u->act / sum);
    // normalize by sum, rescale to act range range
  }

  Compute_AvgAct(lay);
}
