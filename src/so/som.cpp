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

#include "som.h"
#include <pdp/net_iv.h>		// for the "from netview" thing

void SomUnitSpec::Compute_Net(Unit* u) {
  // Modified by Danke, Feb. 9, 2003
  if (u->ext_flag & Unit::EXT)
    u->net = u->ext;
  else {
    // do distance instead of net input
    u->net = 0.0f;
    Con_Group* recv_gp;
    int g;
    FOR_ITR_GP(Con_Group, recv_gp, u->recv., g) {
      if(!recv_gp->prjn->from->lesion)
	u->net += recv_gp->Compute_Dist(u);
    }
  }
}

void NeighborEl::Initialize() {
  act_val = 1.0;
}

void NeighborEl::InitLinks() {
  taOBase::InitLinks();
  taBase::Own(off, this);
}

void NeighborEl::Copy_(const NeighborEl& cp) {
  off = cp.off;
  act_val = cp.act_val;
}

void SomLayerSpec::Initialize() {
  wrap = false;
  netin_type = MIN_NETIN_WINS;
}

void SomLayerSpec::InitLinks() {
  SoLayerSpec::InitLinks();
  taBase::Own(neighborhood, this);
}

void SomLayerSpec::CutLinks() {
  neighborhood.CutLinks();
  SoLayerSpec::CutLinks();
}

void SomLayerSpec::KernelEllipse(int half_width, int half_height, int ctr_x, int ctr_y) {
  neighborhood.Reset();
  int strt_x = ctr_x - half_width;
  int end_x = ctr_x + half_width;
  int strt_y = ctr_y - half_height;
  int end_y = ctr_y + half_height;
  if(half_width == half_height) { // circle
    int y;
    for(y = strt_y; y <= end_y; y++) {
      int x;
      for(x = strt_x; x <= end_x; x++) {
	int dist = ((x - ctr_x) * (x - ctr_x)) + ((y - ctr_y) * (y - ctr_y));
	if(dist > (half_width * half_width))
	  continue;		// outside the circle
	NeighborEl* te = (NeighborEl*)neighborhood.New(1, &TA_NeighborEl);
	te->off.x = x;
	te->off.y = y;
      }
    }
  }
  else {			// ellipse
    float f1_x, f1_y;		// foci
    float f2_x, f2_y;
    float two_a;			// two times largest axis
    
    if(half_width > half_height) {
      two_a = (float)half_width * 2;
      float c = sqrtf((float)(half_width * half_width) - (float)(half_height * half_height));
      f1_x = (float)ctr_x - c;
      f1_y = (float)ctr_y;
      f2_x = (float)ctr_x + c;
      f2_y = (float)ctr_y;
    }
    else {
      two_a = (float)half_height * 2;
      float c = sqrtf((float)(half_height * half_height) - (float)(half_width * half_width));
      f1_x = (float)ctr_x;
      f1_y = (float)ctr_y - c;
      f2_x = (float)ctr_x;
      f2_y = (float)ctr_y + c;
    }

    int y;
    for(y = strt_y; y <= end_y; y++) {
      int x;
      for(x = strt_x; x <= end_x; x++) {
	float dist = sqrtf((((float)x - f1_x) * ((float)x - f1_x)) + (((float)y - f1_y) * ((float)y - f1_y))) +
	  sqrtf((((float)x - f2_x) * ((float)x - f2_x)) + (((float)y - f2_y) * ((float)y - f2_y)));
	if(dist > two_a)
	  continue;
	NeighborEl* te = (NeighborEl*)neighborhood.New(1, &TA_NeighborEl);
	te->off.x = x;
	te->off.y = y;
      }
    }
  }
}

void SomLayerSpec::KernelRectangle(int width, int height, int ctr_x, int ctr_y) {
  neighborhood.Reset();
  int strt_x = ctr_x - (width / 2);
  int strt_y = ctr_y - (height / 2);
  int y;
  for(y = 0; y < height; y++) {
    int x;
    for(x = 0; x < width; x++) {
      NeighborEl* te = (NeighborEl*)neighborhood.New(1, &TA_NeighborEl);
      te->off.x = strt_x + x;
      te->off.y = strt_y + y;
    }
  }
}

void SomLayerSpec::KernelFromNetView(NetView* view) {
  if((view == NULL) || (view->editor == NULL) || (view->editor->netg == NULL))
    return;
  if(view->editor->netg->selectgroup.size <= 0) {
    taMisc::Error("Must select some units to get connection pattern from");
    return;
  }
  neighborhood.Reset();
  int i;
  taBase* itm;
  Unit* center = NULL;
  for(i=0; i< view->editor->netg->selectgroup.size; i++) {
    itm = view->editor->netg->selectgroup.FastEl(i);
    if(!itm->InheritsFrom(TA_Unit))      continue;
    Unit* un = (Unit*) itm;
    if(center == NULL) {
      center = un;
    }
    NeighborEl* te = (NeighborEl*)neighborhood.New(1, &TA_NeighborEl);
    te->off.x = un->pos.x - center->pos.x;
    te->off.y = un->pos.y - center->pos.y;
  }
}

void SomLayerSpec::StepKernelActs(float val) {
  int i;
  NeighborEl* te;
  for(i = 0; i< neighborhood.size; i++) {
    te = (NeighborEl*)neighborhood.FastEl(i);
    te->act_val = val;
  }
}

void SomLayerSpec::LinearKernelActs(float scale) {
  int i;
  NeighborEl* te;
  for(i = 0; i< neighborhood.size; i++) {
    te = (NeighborEl*)neighborhood.FastEl(i);
    float dist = sqrtf((float)(te->off.x * te->off.x + 
			     te->off.y * te->off.y));
    te->act_val = scale * (1.0f / (dist + 1.0f));
  }
}

void SomLayerSpec::GaussianKernelActs(float scale, float sigma) {
  int i;
  NeighborEl* te;
  for(i = 0; i< neighborhood.size; i++) {
    te = (NeighborEl*)neighborhood.FastEl(i);
    float dist = (float)(te->off.x * te->off.x + 
			 te->off.y * te->off.y);
    te->act_val = scale * exp(-0.5 * dist / sigma);
  }
}

int SomLayerSpec::WrapClip(int coord, int max_coord) {
  if(coord >= max_coord) {
    if(wrap)
      coord = coord % max_coord;
    else
      coord = -1;
  }
  else if(coord < 0) {
    if(wrap)
      coord = max_coord + (coord % max_coord);
    else
      coord = -1;
  }
  return coord;
}


void SomLayerSpec::Compute_Act(SoLayer* lay) {
  if(lay->ext_flag & Unit::EXT) {  // input layer
    SoLayerSpec::Compute_Act(lay);
    return;
  }

  // Added by Danke, Feb. 9, 2003
  if(lay->units.leaves > 0 &&      // sync layer
     lay->units.FastEl(0)->ext_flag & Unit::EXT) {
    SoLayerSpec::Compute_Act(lay);
    return;
  }

  Unit* win_u = FindWinner(lay);
  lay->winner = win_u;
  if(win_u == NULL)	return;

  int i;
  NeighborEl* te;
  for(i = 0; i< neighborhood.size; i++) {
    te = (NeighborEl*)neighborhood.FastEl(i);
    int su_x = WrapClip(win_u->pos.x + te->off.x, lay->geom.x);
    int su_y = WrapClip(win_u->pos.y + te->off.y, lay->geom.y);

    if((su_x < 0) || (su_y < 0))
      continue;

    int su_idx = su_y * lay->geom.x + su_x;
    if(su_idx >= lay->units.leaves)
      continue;

    Unit* su_u = (Unit*)lay->units.Leaf(su_idx);
    su_u->act = te->act_val;
  }
}
