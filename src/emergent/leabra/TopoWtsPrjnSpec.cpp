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

#include "TopoWtsPrjnSpec.h"
#include <Network>
#include <taMath_float>

#include <taMisc>

void TopoWtsPrjnSpec::Initialize() {
  wt_range.min = 0.1f;
  wt_range.max = 0.5f;
  wt_range.UpdateAfterEdit_NoGui();
  invert = false;
  grad_x = true;
  grad_x_grad_y = false;
  grad_y = true;
  grad_y_grad_x = false;
  wrap = true;
  grad_type = LINEAR;
  use_send_gps = false;
  use_recv_gps = false;
  custom_send_range = false;
  custom_recv_range = false;
  send_range_start = 0;
  send_range_end = -1;
  recv_range_start = 0;
  recv_range_end = -1;
  gauss_sig = 0.3f;

  already_warned = 0;

  Defaults_init();
}

void TopoWtsPrjnSpec::Defaults_init() {
  init_wts = true;
  add_rnd_wts = true;
  add_rnd_wts_scale = 1.0f;
}

bool TopoWtsPrjnSpec::TestWarning(bool test, const char* fun_name,
				  const char* a, const char* b, const char* c,
				  const char* d, const char* e, const char* f,
				  const char* g, const char* h) const {
  if(!test) return false;
  const_cast<TopoWtsPrjnSpec*>(this)->already_warned++;
  if(already_warned > 10)
    return true;
  return taMisc::TestWarning(this, test, fun_name, a, b, c, d, e, f, g, h);
}

void TopoWtsPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  already_warned = 0;		// reset

  int recv_no = prjn->from->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    recv_no--;

  int send_no = prjn->layer->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    send_no--;

  // pre-allocate connections!
  prjn->layer->RecvConsPreAlloc(recv_no, prjn);
  prjn->from->SendConsPreAlloc(send_no, prjn);

  if(!use_recv_gps) { // recv NO -- not using recv_gps
    for(int y = 0; y < prjn->layer->flat_geom.y; y++) {
      for(int x = 0; x < prjn->layer->flat_geom.x; x++) {
	Unit* ru = prjn->layer->UnitAtCoord(x,y);
	//ru->ConnectFrom(su, prjn);
	if(!use_send_gps) { // send NO -- not using send_gps
	  for(int y = 0; y < prjn->from->flat_geom.y; y++) {
	    for(int x = 0; x < prjn->from->flat_geom.x; x++) {
	      Unit* su = prjn->from->UnitAtCoord(x,y);
	      ru->ConnectFrom(su, prjn);
	    }
	  }
	}
	else { // send YES -- using send_gps
	  FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
	    if(self_con || (ru != su))
	      ru->ConnectFrom(su, prjn);
	  }
	}
      }
    }
  }
  else { // recv YES -- using recv_gps
    FOREACH_ELEM_IN_GROUP(Unit, ru, prjn->layer->units) {
      //bool ignore_unit_gps = false;
      //prjn->from.ptr()->unit_groups
      //if(!use_send_gps && prjn->from.ptr()->unit_groups)
      //ignore_unit_gps = true;
      //if(ignore_unit_gps) {
      if(!use_send_gps) { // send NO -- not using send_gps
	for(int y = 0; y < prjn->from->flat_geom.y; y++) {
	  for(int x = 0; x < prjn->from->flat_geom.x; x++) {
	    Unit* su = prjn->from->UnitAtCoord(x,y);
	    ru->ConnectFrom(su, prjn);
	  }
	}
      }
      else { // send YES -- using send gps
	FOREACH_ELEM_IN_GROUP(Unit, su, prjn->from->units) {
	  if(self_con || (ru != su))
	    ru->ConnectFrom(su, prjn);
	}
      }
    }
  }
}

void TopoWtsPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  if(TestError((grad_x_grad_y && grad_y && !grad_y_grad_x), "TopoWtsPrjnSpec::C_InitWeights ", " both send_x and send_y are mapping to recv_y; you must set either: grad_y = false or grad_y_grad_x = true -- aborting!"))
    return;
  if(TestError((grad_y_grad_x && grad_x && !grad_x_grad_y), "TopoWtsPrjnSpec::C_InitWeights ", " both send_y and send_x are mapping to recv_x; you must set either: grad_x = false or grad_x_grad_y = true -- aborting!"))
    return;
  if(TestError(((send_range_end.x != -1 && send_range_end.x < send_range_start.x) || (send_range_end.y != -1 && send_range_end.y < send_range_start.y)), "TopoWtsPrjnSpec::C_InitWeights ", " either send_range_end.x or send_range_end.y is less than start_idx. Please correct -- aborting!"))
    return;
  if(TestError(((recv_range_end.x != -1 && recv_range_end.x < recv_range_start.x) || (recv_range_end.y != -1 && recv_range_end.y < recv_range_start.y)), "TopoWtsPrjnSpec::C_InitWeights ", " either recv_range_end.x or recv_range_end.y is less than start_idx. Please correct -- aborting!"))
    return;

  if(TestError((use_send_gps && !(prjn->from.ptr()->unit_groups)), "TopoWtsPrjnSpec::C_InitWeights ", " use_send_gps TRUE, but send_lay does not have unit groups. Please fix -- aborting!"))
    return;
  if(TestError((use_recv_gps && !(prjn->layer->unit_groups)), "TopoWtsPrjnSpec::C_InitWeights ", " use_recv_gps TRUE, but recv_lay does not have unit groups. Please fix -- aborting!"))
    return;

  if(use_send_gps && prjn->from.ptr()->unit_groups) {
    if (use_recv_gps && prjn->layer->unit_groups)
      InitWeights_SendGpsRecvGps(prjn, cg, ru);
    else
      InitWeights_SendGpsRecvFlat(prjn, cg, ru);
  }
  else
    if (use_recv_gps && prjn->layer->unit_groups)
      InitWeights_SendFlatRecvGps(prjn, cg, ru);
    else
      InitWeights_SendFlatRecvFlat(prjn, cg, ru);
}

void TopoWtsPrjnSpec::SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist,
				  int cg_idx) {
  float wt_val = wt_range.min;
  if(grad_type == LINEAR) {
    if(invert)
      wt_val = wt_range.min + dist * wt_range.Range();
    else
      wt_val = wt_range.max - dist * wt_range.Range();
  }
  else if(grad_type == GAUSSIAN) {
    float gaus = taMath_float::gauss_den_nonorm(dist, gauss_sig);
    if(invert)
      wt_val = wt_range.max - gaus * wt_range.Range();
    else
      wt_val = wt_range.min + gaus * wt_range.Range();
  }
  cg->Cn(cg_idx)->wt = wt_val;
}

// wrap calculations ///////////////////////////////
////////////////////////////////////////////////////
//      0       1       2       3    recv
//      0       .33     .66     1    rgp_x
//
//      0       1       2       3    send
//      0       .33     .66     1    sgp_x
//      4       5       6       7    wrp_x > .5  int
//      1.33    1.66    2       2.33 wrp_x > .5  flt
//      -4      -3      -2      -1   wrp_x < .5  int
//      -1.33   -1      -.66    -.33 wrp_x < .5  flt

void TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps(Projection* prjn, RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();

  taVector2i srs = send_range_start; // actual send range start
  taVector2i sre = send_range_end;   // actual send range end
  taVector2i rrs = recv_range_start; // actual recv range start
  taVector2i rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) sre.x = send_lay->gp_geom.x-1;
  if(sre.y == -1) sre.y = send_lay->gp_geom.y-1;
  if(rre.x == -1) rre.x = recv_lay->gp_geom.x-1;
  if(rre.y == -1) rre.y = recv_lay->gp_geom.y-1;

  if(TestWarning((sre.x > send_lay->gp_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
	       "send_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
    sre.x = send_lay->gp_geom.x-1;
  }
  if(TestWarning((sre.y > send_lay->gp_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
	       "send_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
    sre.y = send_lay->gp_geom.y-1;
  }
  if(TestWarning((rre.x > recv_lay->gp_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
	       "recv_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
    rre.x = recv_lay->gp_geom.x-1;
  }
  if(TestWarning((rre.y > recv_lay->gp_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvGps ",
	       "recv_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
    rre.y = recv_lay->gp_geom.y-1;
  }
  // else should be the values set by user

  int rgpidx = ru->UnitGpIdx();
  taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position relative to overall gp geom

  float max_dist = 1.0f;
  if(grad_x && grad_y) // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
    max_dist = sqrtf(2.0f);
  float dist = 0.0f;

  // is recv un_gp (and ru) in range?  -- if not, init wts and get next cg/ru
  if(rgp_pos.x < rrs.x || rgp_pos.x > rre.x || rgp_pos.y < rrs.y || rgp_pos.y > rre.y) {
    for(int i=0; i<cg->size; i++) {
      SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
    }
    return; // done -- get a new cg/ru
  }

  // if in-range, normalize recv range (for topological congruence to send range)
  rgp_pos.x -= rrs.x;
  rgp_pos.y -= rrs.y;

  //float rgp_x = (float)rgp_pos.x / (float)MAX(recv_lay->gp_geom.x-1, 1);
  //float rgp_y = (float)rgp_pos.y / (float)MAX(recv_lay->gp_geom.y-1, 1);
  float rgp_x = (float)rgp_pos.x / (float)MAX((rre.x - rrs.x), 1);
  float rgp_y = (float)rgp_pos.y / (float)MAX((rre.y - rrs.y), 1);

  //  float max_dist = 1.0f;
  //  if(grad_x && grad_y) // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
  //    max_dist = sqrtf(2.0f);

  // now adjust the send groups..
  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    int sgpidx = su->UnitGpIdx();
    taVector2i sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // position relative to overall gp geom
    // send un_gp in range?  -- get next un_gp if not
    if(sgp_pos.x < srs.x || sgp_pos.x > sre.x || sgp_pos.y < srs.y || sgp_pos.y > sre.y) {
      SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
      continue;
    }
    // if in-range, normalize send range (for topological congruence to recv range)
    sgp_pos.x -= srs.x;
    sgp_pos.y -= srs.y;

    float sgp_x = (float)sgp_pos.x / mxs_x;
    float sgp_y = (float)sgp_pos.y / mxs_y;

    float wrp_x, wrp_y; // I guess this wrap stuff is still good with recv gps and send gps?
    if(wrap) {
      if(rgp_x > .5f)   wrp_x = (float)(sgp_pos.x + send_lay->gp_geom.x) / mxs_x;
      else              wrp_x = (float)(sgp_pos.x - send_lay->gp_geom.x) / mxs_x;
      if(rgp_y > .5f)   wrp_y = (float)(sgp_pos.y + send_lay->gp_geom.y) / mxs_y;
      else              wrp_y = (float)(sgp_pos.y - send_lay->gp_geom.y) / mxs_y;
    }

    float dist = 0.0f;
    if(grad_x && grad_y) {
      if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	dist = taMath_float::euc_dist(sgp_x, sgp_y, rgp_x, rgp_y);
	if(wrap) {
	  float wrp_dist = taMath_float::euc_dist(wrp_x, sgp_y, rgp_x, rgp_y);
	  if(wrp_dist < dist) {
	    dist = wrp_dist;
	    float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_x, rgp_y);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	  else {
	    float wrp_dist = taMath_float::euc_dist(sgp_x, wrp_y, rgp_x, rgp_y);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	}
      }
      else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	dist = taMath_float::euc_dist(sgp_x, sgp_y, rgp_y, rgp_x);
	if(wrap) {
	  float wrp_dist = taMath_float::euc_dist(wrp_x, sgp_y, rgp_y, rgp_x);
	  if(wrp_dist < dist) {
	    dist = wrp_dist;
	    float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_y, rgp_x);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	  else {
	    float wrp_dist = taMath_float::euc_dist(sgp_x, wrp_y, rgp_y, rgp_x);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	}
      }
    }
    else if(grad_x) { // two cases: x to x and send_x to recv_y
      if(!grad_x_grad_y) { // i.e., the usual x to x case
	dist = fabsf(sgp_x - rgp_x);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_x - rgp_x);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
      else { // the send_x to recv_y case
	dist = fabsf(sgp_x - rgp_y);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_x - rgp_y);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
    }
    else if(grad_y) { // again two cases: y to y and send_y to recv_x
      if(!grad_y_grad_x) { // i.e. the usual y to y case
	dist = fabsf(sgp_y - rgp_y);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_y - rgp_y);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
      else { // the send_y to recv_x case
	dist = fabsf(sgp_y - rgp_x);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_y - rgp_x);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
    }

    dist /= max_dist;           // keep it normalized

    SetWtFmDist(prjn, cg, ru, dist, i); // i corresponds to simple su (not sgp) -- one of entries in the cg (congroup) list
  }
}

void TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat(Projection* prjn,
						  RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();

  taVector2i srs = send_range_start; // actual send range start
  taVector2i sre = send_range_end;   // actual send range end
  taVector2i rrs = recv_range_start; // actual recv range start
  taVector2i rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) sre.x = send_lay->gp_geom.x-1;
  if(sre.y == -1) sre.y = send_lay->gp_geom.y-1;
  if(rre.x == -1) rre.x = recv_lay->flat_geom.x-1;
  if(rre.y == -1) rre.y = recv_lay->flat_geom.y-1;

  if(TestWarning((sre.x > send_lay->gp_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
	       "send_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
    sre.x = send_lay->gp_geom.x-1;
  }
  if(TestWarning((sre.y > send_lay->gp_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
	       "send_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
    sre.y = send_lay->gp_geom.y-1;
  }
  if(TestWarning((rre.x > recv_lay->flat_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
	       "recv_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    rre.x = recv_lay->flat_geom.x-1;
  }
  if(TestWarning((rre.y > recv_lay->flat_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendGpsRecvFlat ",
	       "recv_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    rre.y = recv_lay->flat_geom.y-1;
  }
  // else should be the values set by user

  float ru_x = 0.0f;
  float ru_y = 0.0f;
  float max_dist = 1.0f;
  float dist = 0.0f;
  if (!recv_lay->unit_groups) { // no unit groups to worry about!
    taVector2i ru_pos;
    recv_lay->UnitLogPos(ru, ru_pos);
    if(grad_x && grad_y) {// applies whether grad_x_grad_y && grad_y_grad_x also true, or not
      max_dist = sqrtf(2.0f);
    }
    float dist = 0.0f;
    // is recv unit in range?  -- if not, init wts and get next cg/ru
    if(ru_pos.x < rrs.x || ru_pos.x > rre.x || ru_pos.y < rrs.y || ru_pos.y > rre.y) {
      for(int i=0; i<cg->size; i++) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
      }
      return; // done -- get a new cg/ru
    }
    // if in-range, normalize recv range (for topological congruence to send range)
    //ru_pos.x = ru_pos.x - rrs.x;
    ru_pos.x -= rrs.x; 	// start at start-indexed units!
    //ru_pos.y = ru_pos.y - rrs.y;
    ru_pos.y -= rrs.y;

    //float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1); // original guys..
    //float ru_y = (float)ru_pos.y / (float)MAX(recv_lay->flat_geom.y-1, 1);
    ru_x = (float)ru_pos.x / (float)MAX((rre.x - rrs.x), 1);
    ru_y = (float)ru_pos.y / (float)MAX((rre.y - rrs.y), 1);
  }
  else { // recv_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
    taVector2i ru_pos; // hold planar Cartesian coordinates within unit group
    int runidx = 0;
    int rgpidx = 0;
    recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
    ru_pos.x = runidx % recv_lay->un_geom.x;
    ru_pos.y = runidx / recv_lay->un_geom.x;
    taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
    // convert to planar x, y across whole layer
    //ru_pos.x = ru_pos.x + (recv_lay->un_geom.x * rgp_pos.x);
    ru_pos.x += recv_lay->un_geom.x * rgp_pos.x;
    ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;

    if(grad_x && grad_y) { // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
      max_dist = sqrtf(2.0f);
    }
    // is recv unit out-of-range?  -- if so, init wts and get next cg/ru
    if(ru_pos.x < rrs.x || ru_pos.x > rre.x || ru_pos.y < rrs.y || ru_pos.y > rre.y) {
      for(int i=0; i<cg->size; i++) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
      }
      return; // done -- get a new cg/ru
    }
    // if in-range, normalize recv range (for topological congruence to send range)
    //ru_pos.x = ru_pos.x - rrs.x;
    ru_pos.x -= rrs.x;		  // start at start-indexed units!
    ru_pos.y -= rrs.y;

    //float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1); // original guys..
    ru_x = (float)ru_pos.x / (float)MAX((rre.x - rrs.x), 1);
    ru_y = (float)ru_pos.y / (float)MAX((rre.y - rrs.y), 1);
  }

  // now normalize send groups and correct for any offsets!
  //float mxs_x = (float)MAX(send_lay->flat_geom.x-1, 1); // original guys..
  //float mxs_y = (float)MAX(send_lay->flat_geom.y-1, 1);
  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    int sgpidx = su->UnitGpIdx();
    taVector2i sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // position relative to overall gp geom
    // send un_gp in range?  -- get next un_gp if not
    if(sgp_pos.x < srs.x || sgp_pos.x > sre.x) {
      SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
      continue;
    }
    if(sgp_pos.y < srs.y || sgp_pos.y > sre.y) {
      SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
      continue;
    }
    // if in-range, normalize send range (for topological congruence to recv range)
    sgp_pos.x -= srs.x; // start at start indexed units!
    sgp_pos.y -= srs.y;

    float sgp_x = (float)sgp_pos.x / mxs_x;
    float sgp_y = (float)sgp_pos.y / mxs_y;

    float wrp_x, wrp_y; // TODO: confirm sometime that this wrap stuff is still good with recv gps and/or send gps?
    if(wrap) {
      if(ru_x > .5f)    wrp_x = (float)(sgp_pos.x + send_lay->gp_geom.x) / mxs_x;
      else              wrp_x = (float)(sgp_pos.x - send_lay->gp_geom.x) / mxs_x;
      if(ru_y > .5f)    wrp_y = (float)(sgp_pos.y + send_lay->gp_geom.y) / mxs_y;
      else              wrp_y = (float)(sgp_pos.y - send_lay->gp_geom.y) / mxs_y;
    }
    float dist = 0.0f;
    if(grad_x && grad_y) {
      if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	dist = taMath_float::euc_dist(sgp_x, sgp_y, ru_x, ru_y);
	if(wrap) {
	  float wrp_dist = taMath_float::euc_dist(wrp_x, sgp_y, ru_x, ru_y);
	  if(wrp_dist < dist) {
	    dist = wrp_dist;
	    float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_x, ru_y);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	  else {
	    float wrp_dist = taMath_float::euc_dist(sgp_x, wrp_y, ru_x, ru_y);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	}
      }
      else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	dist = taMath_float::euc_dist(sgp_x, sgp_y, ru_y, ru_x);
	if(wrap) {
	  float wrp_dist = taMath_float::euc_dist(wrp_x, sgp_y, ru_y, ru_x);
	  if(wrp_dist < dist) {
	    dist = wrp_dist;
	    float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_y, ru_x);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	  else {
	    float wrp_dist = taMath_float::euc_dist(sgp_x, wrp_y, ru_y, ru_x);
	    if(wrp_dist < dist)
	      dist = wrp_dist;
	  }
	}
      }
    }
    else if(grad_x) { // two cases: x to x and send_x to recv_y
      if(!grad_x_grad_y) { // i.e., the usual x to x case
	dist = fabsf(sgp_x - ru_x);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_x - ru_x);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
      else { // the send_x to recv_y case
	dist = fabsf(sgp_x - ru_y);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_x - ru_y);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
    }
    else if(grad_y) { // again two cases: y to y and send_y to recv_x
      if(!grad_y_grad_x) { // i.e. the usual y to y case
	dist = fabsf(sgp_y - ru_y);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_y - ru_y);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
      else { // the send_y to recv_x case
	dist = fabsf(sgp_y - ru_x);
	if(wrap) {
	  float wrp_dist = fabsf(wrp_y - ru_x);
	  if(wrp_dist < dist) dist = wrp_dist;
	}
      }
    }

    dist /= max_dist;           // keep it normalized

    SetWtFmDist(prjn, cg, ru, dist, i); // i is still the simple su -- one of entries in the cg (congroup) list
  }
}

void TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps(Projection* prjn, RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();

  taVector2i srs = send_range_start; // actual send range start
  taVector2i sre = send_range_end;   // actual send range end
  taVector2i rrs = recv_range_start; // actual recv range start
  taVector2i rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) sre.x = send_lay->flat_geom.x-1;
  if(sre.y == -1) sre.y = send_lay->flat_geom.y-1;
  if(rre.x == -1) rre.x = recv_lay->gp_geom.x-1;
  if(rre.y == -1) rre.y = recv_lay->gp_geom.y-1;

  if(TestWarning((sre.x > send_lay->flat_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
	       "send_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    sre.x = send_lay->flat_geom.x-1;
  }
  if(TestWarning((sre.y > send_lay->flat_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
	       "send_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    sre.y = send_lay->flat_geom.y-1;
  }
  if(TestWarning((rre.x > recv_lay->gp_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
	       "recv_range_end.x is > gp_geom.x -- using gp_geom.x-1")) {
    rre.x = recv_lay->gp_geom.x-1;
  }
  if(TestWarning((rre.y > recv_lay->gp_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvGps ",
	       "recv_range_end.y is > gp_geom.y -- using gp_geom.y-1")) {
    rre.y = recv_lay->gp_geom.y-1;
  }

  // TODO: should have tests for start > end at this point, but not very likely

  int rgpidx = ru->UnitGpIdx();
  taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position relative to overall gp geom

  //  // recv un_gp in range?  -- get next un_gp if not
  //  if(rgp_pos.x < rrs.x || rgp_pos.x > rre.x)
  //    return;
  //  if(rgp_pos.y < rrs.y || rgp_pos.y > rre.y)
  //    return;

  float max_dist = 1.0f;
  if(grad_x && grad_y) // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
    max_dist = sqrtf(2.0f);
  float dist = 0.0f;

  // is recv un_gp in range?  -- if not, init wts and get next cg/ru
  if(rgp_pos.x < rrs.x || rgp_pos.x > rre.x || rgp_pos.y < rrs.y || rgp_pos.y > rre.y) {
    for(int i=0; i<cg->size; i++) {
      SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!) -- 1.0 = max dist
    }
    return; // done -- get a new cg/ru
  }

  // if in-range, normalize recv range (for topological congruence to send range)
  rgp_pos.x -= rrs.x;
  rgp_pos.y -= rrs.y;

  //float rgp_x = (float)rgp_pos.x / (float)MAX(recv_lay->gp_geom.x-1, 1);
  //float rgp_y = (float)rgp_pos.y / (float)MAX(recv_lay->gp_geom.y-1, 1);
  float rgp_x = (float)rgp_pos.x / (float)MAX((rre.x - rrs.x), 1);
  float rgp_y = (float)rgp_pos.y / (float)MAX((rre.y - rrs.y), 1);

  //  float max_dist = 1.0f;
  //  if(grad_x && grad_y) // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
  //    max_dist = sqrtf(2.0f);

  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  if(!send_lay->unit_groups) { // sending layer does NOT have unit groups anyway -- nothing to worry about!
    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i);
      taVector2i su_pos;
      send_lay->UnitLogPos(su, su_pos);
      // sending unit in range?  -- get next unit if not
      if(su_pos.x < srs.x || su_pos.x > sre.x) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      if(su_pos.y < srs.y || su_pos.y > sre.y) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      // SPECIAL CASE: only one send unit in range so everyone gets max wt!
      if(sre.x - srs.x == 0 && sre.y - srs.y == 0) { // only one sending unit!
    SetWtFmDist(prjn, cg, ru, 0.0f, i); // 0.0 = min dist -> max wts
    continue;
      }
      // if in-range, normalize the send range (for topological congruence to recv range)
      su_pos.x -= srs.x;
      su_pos.y -= srs.y;
      float su_x = (float)su_pos.x / mxs_x;
      float su_y = (float)su_pos.y / mxs_y;

      float wrp_x, wrp_y; // I guess this wrap stuff is still good with recv gps but send flat?
      if(wrap) {
	if(rgp_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
	else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
	if(rgp_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
	else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
      }

      float dist = 0.0f;
      if(grad_x && grad_y) {
	if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	  dist = taMath_float::euc_dist(su_x, su_y, rgp_x, rgp_y);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, rgp_x, rgp_y);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_x, rgp_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, rgp_x, rgp_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
	else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	  dist = taMath_float::euc_dist(su_x, su_y, rgp_y, rgp_x);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, rgp_y, rgp_x);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_y, rgp_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, rgp_y, rgp_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
      }
      else if(grad_x) { // two cases: x to x and send_x to recv_y
	if(!grad_x_grad_y) { // i.e., the usual x to x case
	  dist = fabsf(su_x - rgp_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - rgp_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_x to recv_y case
	  dist = fabsf(su_x - rgp_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - rgp_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }
      else if(grad_y) { // again two cases: y to y and send_y to recv_x
	if(!grad_y_grad_x) { // i.e. the usual y to y case
	  dist = fabsf(su_y - rgp_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - rgp_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_y to recv_x case
	  dist = fabsf(su_y - rgp_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - rgp_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }

      dist /= max_dist;           // keep it normalized

      SetWtFmDist(prjn, cg, ru, dist, i);
    }
  }
  else { // send_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i);
      int sunidx = 0;
      int sgpidx = 0;
      send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp) su_pos.x = sunidx % send_lay->un_geom.x;

      taVector2i su_pos;
      su_pos.x = sunidx % send_lay->un_geom.x;
      su_pos.y = sunidx / send_lay->un_geom.x;
      taVector2i sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom

      // convert to planar x, y across whole layer
      //su_pos.x = su_pos.x + (send_lay->un_geom.x * sgp_pos.x);
      su_pos.x += send_lay->un_geom.x * sgp_pos.x;
      su_pos.y += send_lay->un_geom.y * sgp_pos.y;

      // sending unit in range?  -- get next unit if not
      if(su_pos.x < srs.x || su_pos.x > sre.x) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      if(su_pos.y < srs.y || su_pos.y > sre.y) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      // SPECIAL CASE: only one send unit in range so everyone gets max wt!
      if(sre.x - srs.x == 0 && sre.y - srs.y == 0) { // only one sending unit!
    SetWtFmDist(prjn, cg, ru, 0.0f, i); // 0.0 = min dist -> max wts
    continue;
      }
      // if in-range, normalize the send range (for topological congruence to recv range)
      su_pos.x -= srs.x;
      su_pos.y -= srs.y;
      float su_x = (float)su_pos.x / mxs_x;
      float su_y = (float)su_pos.y / mxs_y;

      float wrp_x, wrp_y; // I guess this wrap stuff is still good with recv gps but send flat?
      if(wrap) {
	if(rgp_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
	else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
	if(rgp_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
	else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
      }

      float dist = 0.0f;
      if(grad_x && grad_y) {
	if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	  dist = taMath_float::euc_dist(su_x, su_y, rgp_x, rgp_y);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, rgp_x, rgp_y);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_x, rgp_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, rgp_x, rgp_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
	else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	  dist = taMath_float::euc_dist(su_x, su_y, rgp_y, rgp_x);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, rgp_y, rgp_x);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, rgp_y, rgp_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, rgp_y, rgp_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
      }
      else if(grad_x) { // two cases: x to x and send_x to recv_y
	if(!grad_x_grad_y) { // i.e., the usual x to x case
	  dist = fabsf(su_x - rgp_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - rgp_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_x to recv_y case
	  dist = fabsf(su_x - rgp_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - rgp_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }
      else if(grad_y) { // again two cases: y to y and send_y to recv_x
	if(!grad_y_grad_x) { // i.e. the usual y to y case
	  dist = fabsf(su_y - rgp_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - rgp_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_y to recv_x case
	  dist = fabsf(su_y - rgp_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - rgp_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }

      dist /= max_dist;           // keep it normalized

      SetWtFmDist(prjn, cg, ru, dist, i);
    }
  }
}

void TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat(Projection* prjn, RecvCons* cg, Unit* ru) {

  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();

  taVector2i srs = send_range_start; // actual send range start
  taVector2i sre = send_range_end;   // actual send range end
  taVector2i rrs = recv_range_start; // actual recv range start
  taVector2i rre = recv_range_end;   // actual recv range end

  if(!custom_send_range) {
    srs = 0;
    sre = -1;		// sets both in one call
  }
  if(!custom_recv_range) {
    rrs = 0;
    rre = -1;
  }
  //  end_idx = -1 means set to last unit
  if(sre.x == -1) sre.x = send_lay->flat_geom.x-1;
  if(sre.y == -1) sre.y = send_lay->flat_geom.y-1;
  if(rre.x == -1) rre.x = recv_lay->flat_geom.x-1;
  if(rre.y == -1) rre.y = recv_lay->flat_geom.y-1;

  if(TestWarning((sre.x > send_lay->flat_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
	       "send_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    sre.x = send_lay->flat_geom.x-1;
  }
  if(TestWarning((sre.y > send_lay->flat_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
	       "send_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    sre.y = send_lay->flat_geom.y-1;
  }
  if(TestWarning((rre.x > recv_lay->flat_geom.x -1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
	       "recv_range_end.x is > flat_geom.x -- using flat_geom.x-1")) {
    rre.x = recv_lay->flat_geom.x-1;
  }
  if(TestWarning((rre.y > recv_lay->flat_geom.y-1),
	       "TopoWtsPrjnSpec::InitWeights_SendFlatRecvFlat ",
	       "recv_range_end.y is > flat_geom.y -- using flat_geom.y-1")) {
    rre.y = recv_lay->flat_geom.y-1;
  }
  // else should be the values set by user

  float ru_x = 0.0f;
  float ru_y = 0.0f;
  float max_dist = 1.0f;
  float dist = 0.0f;
  if (!recv_lay->unit_groups) { // no unit groups to worry about!
    taVector2i ru_pos;
    recv_lay->UnitLogPos(ru, ru_pos);
    if(grad_x && grad_y) {// applies whether grad_x_grad_y && grad_y_grad_x also true, or not
      max_dist = sqrtf(2.0f);
    }
    float dist = 0.0f;
    // is recv unit in range?  -- if not, init wts and get next cg/ru
    if(ru_pos.x < rrs.x || ru_pos.x > rre.x ||
       ru_pos.y < rrs.y || ru_pos.y > rre.y) {
      for(int i=0; i<cg->size; i++) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
      }
      return; // done -- get a new cg/ru
    }
    // if in-range, normalize recv range (for topological congruence to send range)
    //ru_pos.x = ru_pos.x - rrs.x;
    ru_pos.x -= rrs.x; 	// start at start-indexed units!
    //ru_pos.y = ru_pos.y - rrs.y;
    ru_pos.y -= rrs.y;

    //float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1); // original guys..
    //float ru_y = (float)ru_pos.y / (float)MAX(recv_lay->flat_geom.y-1, 1);
    ru_x = (float)ru_pos.x / (float)MAX((rre.x - rrs.x), 1);
    ru_y = (float)ru_pos.y / (float)MAX((rre.y - rrs.y), 1);
  }
  else { // recv_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
    taVector2i ru_pos; // hold planar Cartesian coordinates within unit group
    int runidx = 0;
    int rgpidx = 0;
    recv_lay->UnGpIdxFmUnitIdx(ru->idx, runidx, rgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp)
    ru_pos.x = runidx % recv_lay->un_geom.x;
    ru_pos.y = runidx / recv_lay->un_geom.x;
    taVector2i rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // group position relative to gp geom
    // convert to planar x, y across whole layer
    //ru_pos.x = ru_pos.x + (recv_lay->un_geom.x * rgp_pos.x);
    ru_pos.x += recv_lay->un_geom.x * rgp_pos.x;
    ru_pos.y += recv_lay->un_geom.y * rgp_pos.y;

    if(grad_x && grad_y) { // applies whether grad_x_grad_y && grad_y_grad_x also true, or not
      max_dist = sqrtf(2.0f);
    }
    // is recv unit out-of-range?  -- if so, init wts and get next cg/ru
    if(ru_pos.x < rrs.x || ru_pos.x > rre.x ||
       ru_pos.y < rrs.y || ru_pos.y > rre.y) {
      for(int i=0; i<cg->size; i++) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // i = one of entries in the cg (congroup) list -- corresponds to a su (not a sgp!)
      }
      return; // done -- get a new cg/ru
    }
    // if in-range, normalize recv range (for topological congruence to send range)
    //ru_pos.x = ru_pos.x - rrs.x;
    ru_pos.x -= rrs.x;		  // start at start-indexed units!
    ru_pos.y -= rrs.y;

    //float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1); // original guys..
    ru_x = (float)ru_pos.x / (float)MAX((rre.x - rrs.x), 1);
    ru_y = (float)ru_pos.y / (float)MAX((rre.y - rrs.y), 1);
  }

  float mxs_x = (float)MAX((sre.x - srs.x), 1);
  float mxs_y = (float)MAX((sre.y - srs.y), 1);

  if(!send_lay->unit_groups) { // send layer does NOT have unit groups -- nothing to worry about!
    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i);
      taVector2i su_pos;
      send_lay->UnitLogPos(su, su_pos);
      // sending unit in range?  -- get next unit if not
      if(su_pos.x < srs.x || su_pos.x > sre.x) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      if(su_pos.y < srs.y || su_pos.y > sre.y) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      // SPECIAL CASE: only one send unit in range so everyone gets max wt!
      if(sre.x - srs.x == 0 && sre.y - srs.y == 0) { // only one sending unit!
    SetWtFmDist(prjn, cg, ru, 0.0f, i); // 0.0 = min dist -> max wts
    continue;
      }
      // if in-range, normalize the send range (for topological congruence to recv range)
      su_pos.x -= srs.x;
      su_pos.y -= srs.y;
      float su_x = (float)su_pos.x / mxs_x;
      float su_y = (float)su_pos.y / mxs_y;

      float wrp_x, wrp_y; // I guess this wrap stuff is still good with recv gps but send flat?
      if(wrap) {
	if(ru_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
	else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
	if(ru_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
	else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
      }

      float dist = 0.0f;
      if(grad_x && grad_y) {
	if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	  dist = taMath_float::euc_dist(su_x, su_y, ru_x, ru_y);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, ru_x, ru_y);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_x, ru_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, ru_x, ru_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
	else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	  dist = taMath_float::euc_dist(su_x, su_y, ru_y, ru_x);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, ru_y, ru_x);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_y, ru_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, ru_y, ru_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
      }
      else if(grad_x) { // two cases: x to x and send_x to recv_y
	if(!grad_x_grad_y) { // i.e., the usual x to x case
	  dist = fabsf(su_x - ru_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - ru_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_x to recv_y case
	  dist = fabsf(su_x - ru_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - ru_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }
      else if(grad_y) { // again two cases: y to y and send_y to recv_x
	if(!grad_y_grad_x) { // i.e. the usual y to y case
	  dist = fabsf(su_y - ru_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - ru_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_y to recv_x case
	  dist = fabsf(su_y - ru_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - ru_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }

      dist /= max_dist;           // keep it normalized

      SetWtFmDist(prjn, cg, ru, dist, i);
    }
  }
  else { // send_lay *DOES* have unit groups, but ignore for mapping -- need to compute flat x,y coords
    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i);
      int sunidx = 0;
      int sgpidx = 0;
      send_lay->UnGpIdxFmUnitIdx(su->idx, sunidx, sgpidx); // idx is 1-D index for unit within containing unit group, which for virt_groups is just the one Unit_Group* units object for that layer; i.e., just the flat idx within the whole layer; returns unidx (index of unit within un_gp), gpidx (index of gp) su_pos.x = sunidx % send_lay->un_geom.x;

      taVector2i su_pos;
      su_pos.x = sunidx % send_lay->un_geom.x;
      su_pos.y = sunidx / send_lay->un_geom.x;
      taVector2i sgp_pos = send_lay->UnitGpPosFmIdx(sgpidx); // group position relative to gp geom

      // convert to planar x, y across whole layer
      //su_pos.x = su_pos.x + (send_lay->un_geom.x * sgp_pos.x);
      su_pos.x += send_lay->un_geom.x * sgp_pos.x;
      su_pos.y += send_lay->un_geom.y * sgp_pos.y;

      // sending unit in range?  -- get next unit if not
      if(su_pos.x < srs.x || su_pos.x > sre.x) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      if(su_pos.y < srs.y || su_pos.y > sre.y) {
	SetWtFmDist(prjn, cg, ru, 1.0f, i); // 1.0 = max dist
	continue;
      }
      // if in-range, normalize the send range (for topological congruence to recv range)
      su_pos.x -= srs.x;
      su_pos.y -= srs.y;
      float su_x = (float)su_pos.x / mxs_x;
      float su_y = (float)su_pos.y / mxs_y;

      float wrp_x, wrp_y; // I guess this wrap stuff is still good with recv gps but send flat?
      if(wrap) {
	if(ru_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
	else              wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
	if(ru_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
	else              wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
      }

      float dist = 0.0f;
      if(grad_x && grad_y) {
	if(!grad_x_grad_y && !grad_y_grad_x) { // i.e., x maps to x and y maps to y
	  dist = taMath_float::euc_dist(su_x, su_y, ru_x, ru_y);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, ru_x, ru_y);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_x, ru_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, ru_x, ru_y);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
	else { // i.e., send_x maps to recv_y and send_y maps to recv_x
	  dist = taMath_float::euc_dist(su_x, su_y, ru_y, ru_x);
	  if(wrap) {
	    float wrp_dist = taMath_float::euc_dist(wrp_x, su_y, ru_y, ru_x);
	    if(wrp_dist < dist) {
	      dist = wrp_dist;
	      float wrp_dist = taMath_float::euc_dist(wrp_x, wrp_y, ru_y, ru_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	    else {
	      float wrp_dist = taMath_float::euc_dist(su_x, wrp_y, ru_y, ru_x);
	      if(wrp_dist < dist)
		dist = wrp_dist;
	    }
	  }
	}
      }
      else if(grad_x) { // two cases: x to x and send_x to recv_y
	if(!grad_x_grad_y) { // i.e., the usual x to x case
	  dist = fabsf(su_x - ru_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - ru_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_x to recv_y case
	  dist = fabsf(su_x - ru_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_x - ru_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }
      else if(grad_y) { // again two cases: y to y and send_y to recv_x
	if(!grad_y_grad_x) { // i.e. the usual y to y case
	  dist = fabsf(su_y - ru_y);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - ru_y);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
	else { // the send_y to recv_x case
	  dist = fabsf(su_y - ru_x);
	  if(wrap) {
	    float wrp_dist = fabsf(wrp_y - ru_x);
	    if(wrp_dist < dist) dist = wrp_dist;
	  }
	}
      }

      dist /= max_dist;           // keep it normalized

      SetWtFmDist(prjn, cg, ru, dist, i);
    }
  }
}

