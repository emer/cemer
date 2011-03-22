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



// netstru_extra.cc


#include "netstru_extra.h"
#include "css_machine.h"

#ifdef TA_GUI
//probnotneeded #include "netstru_qtso.h"
#endif

/////////////////////////////
//	    Full	   //
/////////////////////////////

void FullPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;

  int recv_no = prjn->from->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    recv_no--;

  int send_no = prjn->layer->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    send_no--;

  // pre-allocate connections!
  prjn->layer->RecvConsPreAlloc(recv_no, prjn);
  prjn->from->SendConsPreAlloc(send_no, prjn);

  Unit* ru, *su;
  taLeafItr ru_itr, su_itr;
  FOR_ITR_EL(Unit, ru, prjn->layer->units., ru_itr) {
    FOR_ITR_EL(Unit, su, prjn->from->units., su_itr) {
      if(self_con || (ru != su))
	ru->ConnectFrom(su, prjn);
    }
  }
}

int FullPrjnSpec::ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt) {
  if(!(bool)prjn->from)	return 0;

  int rval = 0;

  int no = prjn->from->units.leaves;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    no--;

  int n_new_cons = (int)(p_add_con * (float)no);
  if(n_new_cons <= 0) return 0;
  int_Array new_idxs;
  new_idxs.SetSize(no);
  new_idxs.FillSeq();
  Unit* ru;
  taLeafItr ru_itr;
  FOR_ITR_EL(Unit, ru, prjn->layer->units., ru_itr) {
    new_idxs.Permute();
    for(int i=0;i<n_new_cons;i++) {
      Unit* su = (Unit*)prjn->from->units.Leaf(new_idxs[i]);
      Connection* cn = ru->ConnectFromCk(su, prjn); // check means that it won't add any new connections if already there!
      if(cn) {
	cn->wt = init_wt;
	rval++;
      }
    }
  }
  return rval;
}

/////////////////////////////
//	  OneToOne	   //
/////////////////////////////

void OneToOnePrjnSpec::Initialize() {
  n_conns = -1;
  recv_start = 0;
  send_start = 0;
  SetUnique("self_con", true);
  self_con = true;		// doesn't make sense to not do self con!
}

void OneToOnePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;

  int i;
  int max_n = n_conns;
  if(n_conns < 0)
    max_n = prjn->layer->units.leaves - recv_start;
  max_n = MIN(prjn->layer->units.leaves - recv_start, max_n);
  max_n = MIN(prjn->from->units.leaves - send_start, max_n);
  for(i=0; i<max_n; i++) {
    Unit* ru = (Unit*)prjn->layer->units.Leaf(recv_start + i);
    Unit* su = (Unit*)prjn->from->units.Leaf(send_start + i);
    if(self_con || (ru != su)) {
      ru->RecvConsPreAlloc(1, prjn);
      su->SendConsPreAlloc(1, prjn);
      ru->ConnectFrom(su, prjn);
    }
  }
}

/////////////////////////////
//	  Tessel	   //
/////////////////////////////

void TessEl::Initialize() {
  wt_val = 1.0f;
}

void TesselPrjnSpec::Initialize() {
  recv_n = -1;
  recv_skip = 1;
  recv_group = 1;
  wrap = true;
  send_scale = 1.0f;
  send_offs.SetBaseType(&TA_TessEl);
}

void TesselPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  recv_skip.SetGtEq(1);
  recv_group.SetGtEq(1);
}

void TesselPrjnSpec::MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y) {
  last_make_cmd = "MakeEllipse( half_width=" + String(half_width)
    + ", half_height=" + String(half_height)
    + ", ctr_x=" + String(ctr_x) + ", ctr_y=" + String(ctr_y) + ")";
  DataChanged(DCR_ITEM_UPDATED);
  send_offs.Reset();
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
	TessEl* te = (TessEl*)send_offs.New(1, &TA_TessEl);
	te->send_off.x = x;
	te->send_off.y = y;
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
	TessEl* te = (TessEl*)send_offs.New(1, &TA_TessEl);
	te->send_off.x = x;
	te->send_off.y = y;
      }
    }
  }
}

void TesselPrjnSpec::MakeRectangle(int width, int height, int left, int bottom) {
  last_make_cmd = "MakeRectangle(width=" + String(width) + ", height=" + String(height)
    + ", left=" + String(left) + ", bottom=" + String(bottom) + ")";
  DataChanged(DCR_ITEM_UPDATED);
  send_offs.Reset();
  int y;
  for(y = bottom; y < bottom + height; y++) {
    int x;
    for(x = left; x < left + width; x++) {
      TessEl* te = (TessEl*)send_offs.New(1, &TA_TessEl);
      te->send_off.x = x;
      te->send_off.y = y;
    }
  }
}
/*TODO
void TesselPrjnSpec::MakeFromNetView(NetView* view) {
  if((view == NULL) || (view->editor == NULL) || (view->editor->netg == NULL))
    return;
  if(view->editor->netg->selectgroup.size <= 0) {
    taMisc::Error("Must select some units to get connection pattern from");
    return;
  }
  send_offs.Reset();
  int i;
  taBase* itm;
  Unit* center = NULL;
  for(i=0; i< view->editor->netg->selectgroup.size; i++) {
    itm = view->editor->netg->selectgroup.FastEl(i);
    if(!itm->InheritsFrom(TA_Unit))      continue;
    Unit* un = (Unit*) itm;
    if(center == NULL) {
      center = un;
      continue;
    }
    TessEl* te = (TessEl*)send_offs.New(1, &TA_TessEl);
    te->send_off = un->pos - center->pos;
  }
}*/

void TesselPrjnSpec::WeightsFromDist(float scale) {
  last_weights_cmd = "WeightsFromDist(scale=" + String(scale) + ")";
  DataChanged(DCR_ITEM_UPDATED);
  TwoDCoord zero;
  int i;
  TessEl* te;
  for(i = 0; i< send_offs.size; i++) {
    te = (TessEl*)send_offs.FastEl(i);
    float dist = te->send_off.Dist(zero);
    te->wt_val = scale * (1.0f / dist);
  }
}

void TesselPrjnSpec::WeightsFromGausDist(float scale, float sigma) {
  last_weights_cmd = "WeightsFromGausDist(scale=" + String(scale) 
    + ", sigma=" + String(sigma) + ")";
  DataChanged(DCR_ITEM_UPDATED);
  TwoDCoord zero;
  int i;
  TessEl* te;
  for(i = 0; i< send_offs.size; i++) {
    te = (TessEl*)send_offs.FastEl(i);
    float dist = te->send_off.Dist(zero);
    te->wt_val = scale * exp(-0.5 * dist / (sigma * sigma));
  }
}

// todo: this assumes that things are in order.. (can't really check otherwise)
// which breaks for clipped patterns
void TesselPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  int mxi = MIN(cg->size, send_offs.size);
  int i;
  for(i=0; i<mxi; i++) {
    TessEl* te = (TessEl*)send_offs.FastEl(i);
    cg->Cn(i)->wt = te->wt_val;
  }
}

void TesselPrjnSpec::GetCtrFmRecv(TwoDCoord& sctr, TwoDCoord ruc) {
  ruc -= recv_off;
  ruc /= recv_group;	ruc *= recv_group;	// this takes int part of
  ruc += recv_off;	// then re-add offset
  FloatTwoDCoord scruc = ruc;
  scruc *= send_scale;
  scruc += send_off;
  sctr = scruc;		// take int part at the end
}

void TesselPrjnSpec::Connect_RecvUnit(Unit* ru_u, const TwoDCoord& ruc, Projection* prjn,
				      bool send_alloc) {
  // allocate cons
  if(!send_alloc)
    ru_u->RecvConsPreAlloc(send_offs.size, prjn);

  PosTwoDCoord su_geo = prjn->from->flat_geom;
  // positions of center of recv in sending layer
  TwoDCoord sctr;
  GetCtrFmRecv(sctr, ruc);
  int i;
  TessEl* te;
  for(i = 0; i< send_offs.size; i++) {
    te = (TessEl*)send_offs.FastEl(i);
    TwoDCoord suc = te->send_off + sctr;
    if(suc.WrapClip(wrap, su_geo) && !wrap)
      continue;
    Unit* su_u = prjn->from->UnitAtCoord(suc);
    if((su_u == NULL) || (!self_con && (su_u == ru_u)))
      continue;
    if(send_alloc)
      ru_u->ConnectFrom(su_u, prjn, send_alloc);
    else
      ru_u->ConnectFromCk(su_u, prjn); // check on 2nd pass
  }
}

void TesselPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;

  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;

//   TestWarning(!wrap && init_wts, "Connect_impl",
// 	      "non-wrapped tessel prjn spec with init_wts does not usually work!");
  PosTwoDCoord ru_geo = prjn->layer->flat_geom;

  TwoDCoord use_recv_n = recv_n;

  if(recv_n.x == -1)
    use_recv_n.x = ru_geo.x;
  if(recv_n.y == -1)
    use_recv_n.y = ru_geo.y;

  TwoDCoord ruc, nuc;
  for(int alloc_loop=1; alloc_loop >= 0; alloc_loop--) {
    for(ruc.y = recv_off.y, nuc.y = 0; (ruc.y < ru_geo.y) && (nuc.y < use_recv_n.y);
	ruc.y += recv_skip.y, nuc.y++)
      {
	for(ruc.x = recv_off.x, nuc.x = 0; (ruc.x < ru_geo.x) && (nuc.x < use_recv_n.x);
	    ruc.x += recv_skip.x, nuc.x++)
	  {
	    Unit* ru_u = prjn->layer->UnitAtCoord(ruc);
	    if(ru_u == NULL)
	      continue;
	    Connect_RecvUnit(ru_u, ruc, prjn, alloc_loop);
	  }
      }

    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}


/////////////////////////////
//	  ReallyUniformRnd	   //
/////////////////////////////

void RandomPrjnSpec::Initialize() {
  p_con = .25;
}

void RandomPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(p_con > 1.0f) p_con = 1.0f;
  if(p_con < 0.0f) p_con = 0.0f;
}

void RandomPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(same_seed)
    rndm_seed.OldSeed();  

  int n_send_units = prjn->from->units.leaves;
  int n_recv_units = prjn->layer->units.leaves;

  int_Matrix* send_alloc = new int_Matrix;
  int_Matrix* recv_alloc = new int_Matrix;
  int_Matrix* cons = new int_Matrix;
  
  send_alloc->SetGeom(1, n_send_units);
  recv_alloc->SetGeom(1, n_recv_units);
  cons->SetGeom(2, n_recv_units, n_send_units);

  for (int i = 0; i < n_recv_units; i++) {
    for (int j= 0; j < n_send_units; j++) {
      if (Random::BoolProb(p_con)) {
	cons->Set(1, i, j);
	send_alloc->Set(send_alloc->FastEl(j) + 1, j);
	recv_alloc->Set(recv_alloc->FastEl(i) + 1, i);
      }
    }
  }

  for (int i = 0; i < n_recv_units; i++)
    prjn->layer->units.FastEl(i)->RecvConsPreAlloc(recv_alloc->FastEl(i), prjn);

  for (int j = 0; j < n_send_units; j++)
    prjn->from->units.FastEl(j)->SendConsPreAlloc(send_alloc->FastEl(j), prjn);

  for (int i = 0; i < n_recv_units; i++) {
    for (int j = 0; j < n_send_units; j++) {
      if (cons->FastEl(i, j))
	prjn->layer->units.FastEl(i)->ConnectFrom(prjn->from->units.FastEl(j), prjn);
    }
  }
}

/////////////////////////////
//	  UniformRnd	   //
/////////////////////////////

void UniformRndPrjnSpec::Initialize() {
  p_con = .25;
}

void UniformRndPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(p_con > 1.0f) p_con = 1.0f;
  if(p_con < 0.0f) p_con = 0.0f;
}

void UniformRndPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(same_seed)
    rndm_seed.OldSeed();

  int recv_no;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    recv_no = (int) ((p_con * (float)(prjn->from->units.leaves-1)) + .5f);
  else
    recv_no = (int) ((p_con * ((float)prjn->from->units.leaves)) + .5f);
  if(recv_no <= 0) recv_no = 1;

  // sending number is even distribution across senders plus some imbalance factor
  float send_no_flt = (float)(prjn->layer->units.leaves * recv_no) / (float)prjn->from->units.leaves;
  // add SEM as corrective factor
  float send_sem = send_no_flt / sqrtf(send_no_flt);
  int send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f);
  if(send_no > prjn->layer->units.leaves) send_no = prjn->layer->units.leaves;

  // pre-allocate connections!
  prjn->layer->RecvConsPreAlloc(recv_no, prjn);
  prjn->from->SendConsPreAlloc(send_no, prjn);

  Unit* ru, *su;
  taLeafItr ru_itr, su_itr;
  if((prjn->from.ptr() == prjn->layer) && sym_self) {
    Layer* lay = prjn->layer;
    // trick is to divide cons in half, choose recv, send at random
    // for 1/2 cons, then go through all units and make the symmetric cons..
    TestWarning(p_con > .95f, "Connect_impl", "there is usually less than complete connectivity for high values of p_con (>.95) in symmetric, self-connected layers using permute!");
    // pre-allocate connections!
    int first;
    if(!self_con)
      first = (int) (.5f * p_con * (float)(prjn->from->units.leaves-1));
    else
      first = (int) (.5f * p_con * (float)prjn->from->units.leaves);
    if(first <= 0) first = 1;

    UnitPtrList ru_list;		// receiver permution list
    UnitPtrList perm_list;	// sender permution list

    FOR_ITR_EL(Unit, ru, lay->units., ru_itr)	// need to permute recvs because of exclusion
      ru_list.Link(ru);			// on making a symmetric connection in first pass
    ru_list.Permute();

    for(int i=0;i<ru_list.size; i++) {
      ru = ru_list.FastEl(i);
      perm_list.Reset();
      FOR_ITR_EL(Unit, su, lay->units., su_itr) {
	if(!self_con && (ru == su)) continue;
	// don't connect to anyone who already recvs from me cuz that will make
	// a symmetric connection which isn't good: symmetry will be enforced later
	RecvCons* scg = su->recv.FindPrjn(prjn);
	if(scg->FindConFromIdx(ru) >= 0) continue;
	perm_list.Link(su);
      }
      perm_list.Permute();
      int j;
      for(j=0; j<first && j<perm_list.size; j++)	// only connect 1/2 of the units
	ru->ConnectFrom((Unit*)perm_list[j], prjn, false, true);
      // true = ignore errs -- to be expected
    }
    // now go thru and make the symmetric connections
    FOR_ITR_EL(Unit, ru, lay->units., ru_itr) {
      SendCons* scg = ru->send.FindPrjn(prjn);
      if(scg == NULL) continue;
      int i;
      for(i=0;i<scg->size;i++) {
	Unit* su = scg->Un(i);
	ru->ConnectFromCk(su, prjn, true);
	// true = ignore errs -- to be expected
      }
    }
  }
  else {			// not a symmetric self projection
    UnitPtrList perm_list;	// permution list
    FOR_ITR_EL(Unit, ru, prjn->layer->units., ru_itr) {
      perm_list.Reset();
      FOR_ITR_EL(Unit, su, prjn->from->units., su_itr) {
	if(!self_con && (ru == su)) continue;
	perm_list.Link(su);
      }
      perm_list.Permute();
      for(int i=0; i<recv_no && i<perm_list.size; i++)
	ru->ConnectFrom((Unit*)perm_list[i], prjn, false, true);
      // true = ignore errs -- to be expected
    }
  }
}

/////////////////////////////
//	  PolarRnd	   //
/////////////////////////////

void PolarRndPrjnSpec::Initialize() {
  p_con = .25;

  rnd_dist.type = Random::GAUSSIAN;
  rnd_dist.mean = 0.0f;
  rnd_dist.var = .25f;

  rnd_angle.type = Random::UNIFORM;
  rnd_angle.mean = 0.5f;
  rnd_angle.var = 0.5f;

  dist_type = XY_DIST_CENTER_NORM;
  wrap = false;
  max_retries = 1000;

  same_seed = false;
  rndm_seed.GetCurrent();
}

void PolarRndPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(p_con > 1.0f) p_con = 1.0f;
  if(p_con < 0.0f) p_con = 0.0f;
}

float PolarRndPrjnSpec::UnitDist(UnitDistType typ, Projection* prjn,
			       const TwoDCoord& ru, const TwoDCoord& su)
{
  FloatTwoDCoord half(.5f);
  PosTwoDCoord ru_geom = prjn->layer->flat_geom;
  PosTwoDCoord su_geom = prjn->from->flat_geom;
  switch(typ) {
  case XY_DIST:
    return ru.Dist(su);
  case XY_DIST_CENTER: {
    FloatTwoDCoord rctr = ru_geom;   rctr *= half;
    FloatTwoDCoord sctr = su_geom;    sctr *= half;
    TwoDCoord ruc = ru - (TwoDCoord)rctr;
    TwoDCoord suc = su - (TwoDCoord)sctr;
    return ruc.Dist(suc);
  }
  case XY_DIST_NORM: {
    FloatTwoDCoord ruc = ru;	ruc /= (FloatTwoDCoord)ru_geom;
    FloatTwoDCoord suc = su;	suc /= (FloatTwoDCoord)su_geom;
    return ruc.Dist(suc);
  }
  case XY_DIST_CENTER_NORM: {
    FloatTwoDCoord rctr = ru_geom;   rctr *= half;
    FloatTwoDCoord sctr = su_geom;    sctr *= half;
    FloatTwoDCoord ruc = ((FloatTwoDCoord)ru - rctr) / rctr;
    FloatTwoDCoord suc = ((FloatTwoDCoord)su - sctr) / sctr;
    return ruc.Dist(suc);
  }
  }
  return 0.0f;
}

Unit* PolarRndPrjnSpec::GetUnitFmOff(UnitDistType typ, bool wrap, Projection* prjn,
				   const TwoDCoord& ru, const FloatTwoDCoord& su_off)
{
  FloatTwoDCoord half(.5f);
  PosTwoDCoord ru_geom = prjn->layer->flat_geom;
  PosTwoDCoord su_geom = prjn->from->flat_geom;
  TwoDCoord suc;		// actual su coordinates
  switch(typ) {
  case XY_DIST: {
    suc = su_off;
    suc += ru;
    break;
  }
  case XY_DIST_CENTER: {	// do everything relative to center
    FloatTwoDCoord rctr = ru_geom;   rctr *= half;
    FloatTwoDCoord sctr = su_geom;    sctr *= half;
    TwoDCoord ruc = ru - (TwoDCoord)rctr;
    suc = su_off;
    suc += ruc;			// add the centerized coordinates
    suc += (TwoDCoord)sctr;	// then add the sending center back into it..
    break;
  }
  case XY_DIST_NORM: {
    FloatTwoDCoord ruc = ru;	ruc /= (FloatTwoDCoord)ru_geom;
    FloatTwoDCoord suf = su_off + ruc; // su_off is in normalized coords, so normalize ru
    suf *= (FloatTwoDCoord)su_geom;
    suc = suf;
    break;
  }
  case XY_DIST_CENTER_NORM: {
    FloatTwoDCoord rctr = ru_geom;   rctr *= half;
    FloatTwoDCoord sctr = su_geom;    sctr *= half;
    FloatTwoDCoord ruc = ((FloatTwoDCoord)ru - rctr) / rctr;
    FloatTwoDCoord suf = su_off + ruc;
    suf *= sctr;    suf += sctr;
    suc = suf;
    break;
  }
  }
  if(suc.WrapClip(wrap, su_geom) && !wrap)
    return NULL;

  Unit* su_u = (Unit*)prjn->from->UnitAtCoord(suc);
  return su_u;
}


float PolarRndPrjnSpec::GetDistProb(Projection* prjn, Unit* ru, Unit* su) {
  if(rnd_dist.type == Random::UNIFORM)
    return p_con;
  float prob = p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, su->pos));
  if(wrap) {
    PosTwoDCoord su_geom = prjn->from->flat_geom;
    TwoDCoord suc = su->pos;
    suc.x += su_geom.x; // wrap around in x
    prob += p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
    suc.y += su_geom.y; // wrap around in x & y
    prob += p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
    suc.x = su->pos.x;		// just y
    prob += p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
    suc = su->pos;
    suc.x -= su_geom.x; // wrap around in x
    prob -= p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
    suc.y -= su_geom.y; // wrap around in y
    prob += p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
    suc.x = su->pos.x;		// just y
    prob += p_con * rnd_dist.Density(UnitDist(dist_type, prjn, ru->pos, suc));
  }
  return prob;
}

// todo: could put in some sending limits, and do recvs in random order

void PolarRndPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(same_seed)
    rndm_seed.OldSeed();

  int recv_no;
  if(!self_con && (prjn->from.ptr() == prjn->layer))
    recv_no = (int) ((p_con * (float)(prjn->from->units.leaves-1)) + .5f);
  else
    recv_no = (int) ((p_con * (float)prjn->from->units.leaves) + .5f);
  if(recv_no <= 0) recv_no = 1;

  // sending number is even distribution across senders plus some imbalance factor
  float send_no_flt = (float)(prjn->layer->units.leaves * recv_no) / (float)prjn->from->units.leaves;
  // add SEM as corrective factor
  float send_sem = send_no_flt / sqrtf(send_no_flt);
  int send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f); // polar needs some extra insurance
  if(send_no > prjn->layer->units.leaves) send_no = prjn->layer->units.leaves;

  // pre-allocate connections!
  prjn->layer->RecvConsPreAlloc(recv_no, prjn);
  prjn->from->SendConsPreAlloc(send_no, prjn);

  Unit* ru, *su;
  taLeafItr ru_itr;
  PosTwoDCoord ru_geom = prjn->layer->flat_geom;
  TwoDCoord ru_pos;		// do this according to act_geom..
  int cnt = 0;
  for(ru = (Unit*)prjn->layer->units.FirstEl(ru_itr); ru;
      ru = (Unit*)prjn->layer->units.NextEl(ru_itr), cnt++) {
    ru_pos.y = cnt / ru_geom.x;
    ru_pos.x = cnt % ru_geom.x;
    RecvCons* recv_gp = NULL;
    FloatTwoDCoord suc;
    int n_con = 0;
    int n_retry = 0;
    while((n_con < recv_no) && (n_retry < max_retries)) { // limit number of retries
      float dist = rnd_dist.Gen();		// just get random deviate from distribution
      float angle = 2.0 * 3.14159265 * rnd_angle.Gen(); // same for angle
      suc.x = dist * cos(angle);
      suc.y = dist * sin(angle);
      su = GetUnitFmOff(dist_type, wrap, prjn, ru_pos, suc);
      if((su == NULL) || (!self_con && (ru == su))) {
	n_retry++;
	continue;
      }
      if(ru->ConnectFromCk(su, prjn, recv_gp))
	n_con++;
      else {
	n_retry++;		// already connected, retry
	continue;
      }
    }
    TestWarning(n_con < recv_no, "Connect_impl",
		"target number of connections:",String(recv_no),
		"not made, only made:",String(n_con));
  }
}

void PolarRndPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  for(int i=0; i<cg->size; i++) {
    cg->Cn(i)->wt = GetDistProb(prjn, ru, cg->Un(i));
  }
}

/////////////////////////////
//	  Symmetric	   //
/////////////////////////////

void SymmetricPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;

  Unit* ru, *su;
  taLeafItr ru_itr, su_itr;
  FOR_ITR_EL(Unit, ru, prjn->layer->units., ru_itr) {
    int n_cons = 0;
    FOR_ITR_EL(Unit, su, prjn->from->units., su_itr) {
      if(RecvCons::FindRecipRecvCon(su, ru, prjn->layer))
	n_cons++;
    }
    ru->RecvConsPreAlloc(n_cons, prjn); 
  }
 
  // todo: not 100% sure this is correct!!!
  FOR_ITR_EL(Unit, su, prjn->from->units., su_itr) {
    int n_cons = 0;
    FOR_ITR_EL(Unit, ru, prjn->layer->units., ru_itr) {
      if(RecvCons::FindRecipSendCon(ru, su, prjn->from))
	n_cons++;
    }
    su->SendConsPreAlloc(n_cons, prjn);
  }

  int cnt = 0;
  FOR_ITR_EL(Unit, ru, prjn->layer->units., ru_itr) {
    FOR_ITR_EL(Unit, su, prjn->from->units., su_itr) {
      if(RecvCons::FindRecipRecvCon(su, ru, prjn->layer))
	if(ru->ConnectFrom(su, prjn))
	  cnt++;
    }
  }
  TestWarning(cnt == 0, "Connect_impl", "did not make any connections.",
	      "Note that this layer must be *earlier* in list of layers than the one you are trying to symmetrize from.");
}

/////////////////////////////
//	  Script	   //
/////////////////////////////

void ScriptPrjnSpec::Initialize() {
  prjn = NULL;
}

void ScriptPrjnSpec::Destroy() {
  prjn = NULL;
}

void ScriptPrjnSpec::InitLinks() {
  ProjectionSpec::InitLinks();
  taBase::Own(s_args, this);
  if(script_file->fname().empty())	// initialize only on startup up, not transfer
    SetScript("");
}

void ScriptPrjnSpec::Copy_(const ScriptPrjnSpec& cp) {
  s_args = cp.s_args;
  *script_file = *(cp.script_file);
  script_string = cp.script_string;
  script_filename = cp.script_filename;
}

void ScriptPrjnSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  
  //make sure most recent compile succeeded
  CheckError(!script_compiled, quiet, rval,
    "The script did not compile -- please recompile and check console for errors");
}

void ScriptPrjnSpec::Connect_impl(Projection* prj) {
  prjn = prj;			// set the arg for the script
  RunScript();
  prjn = NULL;
}

void ScriptPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateScriptFile();		// deals with all the script file management issues
  CompileScript(true); // force
/*v3 nuke  if(!script_file->fname().empty()) {
    //note: fname() is name only
    name = script_file->fname().before(".css");
    int i;
    for(i=0;i<s_args.size;i++)
      name += String("_") + s_args[i];
  }*/
}

void ScriptPrjnSpec::Compile() {
  UpdateScriptFile();		// deals with all the script file management issues
  CompileScript(true);		// force recompile!
}

/////////////////////////////
//	  Program	   //
/////////////////////////////

void ProgramPrjnSpec::Initialize() {
}

void ProgramPrjnSpec::Destroy() {
}

void ProgramPrjnSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!CheckError(!(bool)prog, quiet, rval,
		 "The program prog is not set -- no projections will be made!"))
    return;
  CheckError(!prog->HasVar("prjn"), quiet, rval,
	     "The program does not have the required 'prjn' arg variable -- no projections will be made!");
}

void ProgramPrjnSpec::Connect_impl(Projection* prj) {
  if(!prog) return;
  bool did_it = prog->SetVar("prjn", prj);
  if(!did_it) return;
  prog->Run();
}

void ProgramPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

/////////////////////////////
//	  Custom	   //
/////////////////////////////

void CustomPrjnSpec::Connect(Projection* prjn) {
  // make sure i have the correct indicies for my con_groups..
  prjn->recv_idx = -1;
  prjn->send_idx = -1;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, prjn->layer->units., i) {
    int idx = u->recv.FindPrjnIdx(prjn);
    if(idx >= 0) {
      prjn->recv_idx = idx;
      break;
    }
  }
  FOR_ITR_EL(Unit, u, prjn->from->units., i) {
    int idx = u->send.FindPrjnIdx(prjn);
    if(idx >= 0) {
      prjn->send_idx = idx;
      break;
    }
  }

  prjn->projected = true;	// don't do anything else..
}

//////////////////////////////////////////
//	UnitGroup-based PrjnSpecs	//
//////////////////////////////////////////

//////////////////////////////////////////
// 	GpOneToOnePrjnSpec		//
//////////////////////////////////////////

void GpOneToOnePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord ru_geo = recv_lay->gp_geom;
  TwoDCoord su_geo = send_lay->gp_geom;

  // revert to main group if no sub groups
  Layer::AccessMode racc_md = Layer::ACC_GP;
  if(!recv_lay->unit_groups) racc_md = Layer::ACC_LAY;
  Layer::AccessMode sacc_md = Layer::ACC_GP;
  if(!send_lay->unit_groups) sacc_md = Layer::ACC_LAY;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md);
  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md);

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = recv_lay->gp_geom.n - recv_start;
  max_n = MIN(recv_lay->gp_geom.n - recv_start, max_n);
  max_n = MIN(send_lay->gp_geom.n - send_start, max_n);
  max_n = MAX(1, max_n);	// lower limit of 1
  for(int i=0; i<max_n; i++) {

    int rgpidx = i + recv_start;
    int sgpidx = i + send_start;

    // pre-allocate connections
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
      ru->RecvConsPreAlloc(su_nunits, prjn);
    }
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
      su->SendConsPreAlloc(ru_nunits, prjn);
    }
      
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
      for(int sui=0; sui < su_nunits; sui++) {
	Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
	if(self_con || (ru != su))
	  ru->ConnectFrom(su, prjn);
      }
    }
  }
}


//////////////////////////////////////////
// 	RndGpOneToOnePrjnSpec		//
//////////////////////////////////////////

void RndGpOneToOnePrjnSpec::Initialize() {
  p_con = .25;
  same_seed = false;
  rndm_seed.GetCurrent();
}

void RndGpOneToOnePrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(p_con > 1.0f) p_con = 1.0f;
  if(p_con < 0.0f) p_con = 0.0f;
}

void RndGpOneToOnePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(same_seed)
    rndm_seed.OldSeed();

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord ru_geo = recv_lay->gp_geom;
  TwoDCoord su_geo = send_lay->gp_geom;

  // revert to main group if no sub groups
  Layer::AccessMode racc_md = Layer::ACC_GP;
  if(!recv_lay->unit_groups) racc_md = Layer::ACC_LAY;
  Layer::AccessMode sacc_md = Layer::ACC_GP;
  if(!send_lay->unit_groups) sacc_md = Layer::ACC_LAY;

  int ru_nunits = recv_lay->UnitAccess_NUnits(racc_md);
  int su_nunits = send_lay->UnitAccess_NUnits(sacc_md);

  int max_n = n_conns;
  if(n_conns < 0)
    max_n = recv_lay->gp_geom.n - recv_start;
  max_n = MIN(recv_lay->gp_geom.n - recv_start, max_n);
  max_n = MIN(send_lay->gp_geom.n - send_start, max_n);
  max_n = MAX(1, max_n);	// lower limit of 1
  for(int i=0; i<max_n; i++) {
    int rgpidx = i + recv_start;
    int sgpidx = i + send_start;

    int recv_no = (int) ((p_con * (float)su_nunits) + .5f);
    if(!self_con && (send_lay == recv_lay && rgpidx == sgpidx))
      recv_no--;
    if(recv_no <= 0)
      recv_no = 1;

    // sending number is even distribution across senders plus some imbalance factor
    float send_no_flt = (float)(ru_nunits * recv_no) / (float)su_nunits;
    // add SEM as corrective factor
    float send_sem = send_no_flt / sqrtf(send_no_flt);
    int send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f);
    if(send_no > ru_nunits) send_no = ru_nunits;

    // pre-allocate connections
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
      ru->RecvConsPreAlloc(recv_no, prjn);
    }
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
      su->SendConsPreAlloc(send_no, prjn);
    }

    UnitPtrList perm_list;	// permution list
    for(int rui=0; rui < ru_nunits; rui++) {
      perm_list.Reset();
      Unit* ru = recv_lay->UnitAccess(racc_md, rui, rgpidx);
      for(int sui=0; sui < su_nunits; sui++) {
	Unit* su = send_lay->UnitAccess(sacc_md, sui, sgpidx);
	perm_list.Link(su);
      }
      perm_list.Permute();
      for(int j=0; j<recv_no; j++)
	ru->ConnectFrom((Unit*)perm_list[j], prjn);
    }
  }
}


//////////////////////////////////////////
// 	GpOneToManyPrjnSpec		//
//////////////////////////////////////////

void GpOneToManyPrjnSpec::Initialize() {
  n_con_groups = SEND_ONLY;
}

void GpOneToManyPrjnSpec::GetNGroups(Projection* prjn, int& r_n_ugp, int& s_n_ugp) {
  Unit_Group* recv_ugp = &(prjn->layer->units);
  Unit_Group* send_ugp = &(prjn->from->units);

  s_n_ugp = n_conns;
  if(n_conns < 0)
    s_n_ugp = send_ugp->gp.size - send_start;
  s_n_ugp = MIN(send_ugp->gp.size - send_start, s_n_ugp);
  s_n_ugp = MAX(1, s_n_ugp);	// lower limit of 1

  if(recv_ugp->gp.size > 0)
    r_n_ugp = recv_ugp->gp.size;
  else
    r_n_ugp = 1;

  if(n_con_groups == RECV_SEND_PAIR) {
    prjn->recv_n = s_n_ugp;
    prjn->send_n = r_n_ugp;
  }
  else if(n_con_groups == SEND_ONLY) {
    prjn->recv_n = s_n_ugp;
    prjn->send_n = 1;
  }
  else {
    prjn->recv_n = 1;
    prjn->send_n = 1;
  }
}

// preconnect assumes full interconnectivty in order to maintain
// homogeneity of all unit con_groups in a layer.  this wastes some, but what can you do..
void GpOneToManyPrjnSpec::PreConnect(Projection* prjn) {
  prjn->SetFrom();		// justin case
  if(!(bool)prjn->from)	return;

  int r_n_ugp, s_n_ugp;
  int old_send_start = send_start; // temporarilly resort to full connectivity
  int old_n_conns = n_conns;
  send_start = 0;
  n_conns = -1;
  GetNGroups(prjn, r_n_ugp, s_n_ugp);
  send_start = old_send_start;
  n_conns = old_n_conns;

  Unit_Group* recv_ugp = &(prjn->layer->units);
  Unit_Group* send_ugp = &(prjn->from->units);

  // make first set of congroups to get indicies
  Unit* first_ru = (Unit*)recv_ugp->Leaf(0);
  Unit* first_su = (Unit*)send_ugp->Leaf(0);
  if((first_ru == NULL) || (first_su == NULL))
    return;
  RecvCons* recv_gp = first_ru->recv.NewPrjn(prjn);
  prjn->recv_idx = first_ru->recv.size - 1;
  SendCons* send_gp = first_su->send.NewPrjn(prjn);
  prjn->send_idx = first_su->send.size - 1;
  // set reciprocal indicies
  recv_gp->other_idx = prjn->send_idx;
  send_gp->other_idx = prjn->recv_idx;

  // use basic connectivity routine to set indicies..
  int r, s;
  for(r=0; r<r_n_ugp; r++) {
    Unit_Group* rgp;
    if(recv_ugp->gp.size > 0)
      rgp = (Unit_Group*)recv_ugp->gp.FastEl(r);
    else
      rgp = recv_ugp;
    for(s=0; s<s_n_ugp; s++) {
      Unit_Group* sgp;
      if(send_ugp->gp.size > 0)
	sgp = (Unit_Group*)send_ugp->gp.FastEl(s);
      else
	sgp = send_ugp;

      int recv_idx = prjn->recv_idx;
      int send_idx = prjn->send_idx;
      if(n_con_groups != ONE_GROUP)
	recv_idx += s;
      if(n_con_groups == RECV_SEND_PAIR)
	send_idx += r;

      // then its full connectivity..
      Unit* u;
      taLeafItr u_itr;
      FOR_ITR_EL(Unit, u, rgp->, u_itr) {
	if((u == first_ru) && (s == 0))	continue; // skip this one
	recv_gp = u->recv.NewPrjn(prjn);
	recv_gp->other_idx = send_idx;
      }
      FOR_ITR_EL(Unit, u, sgp->, u_itr) {
	if((u == first_su) && (r == 0))	continue; // skip this one
	send_gp = u->send.NewPrjn(prjn);
	send_gp->other_idx = recv_idx;
      }
    }
  }
}

void GpOneToManyPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;

  int orig_recv_idx = prjn->recv_idx;
  int orig_send_idx = prjn->send_idx;

  int r_n_ugp, s_n_ugp;
  GetNGroups(prjn, r_n_ugp, s_n_ugp);

  Unit_Group* recv_ugp = &(prjn->layer->units);
  Unit_Group* send_ugp = &(prjn->from->units);
  int r, s;
  for(r=0; r<r_n_ugp; r++) {
    Unit_Group* rgp;
    if(recv_ugp->gp.size > 0)
      rgp = (Unit_Group*)recv_ugp->gp.FastEl(r);
    else
      rgp = recv_ugp;
    for(s=0; s<s_n_ugp; s++) {
      Unit_Group* sgp;
      if(send_ugp->gp.size > 0)
	sgp = (Unit_Group*)send_ugp->gp.FastEl(send_start + s);
      else
	sgp = send_ugp;

      if(n_con_groups != ONE_GROUP)
	prjn->recv_idx = orig_recv_idx + s;
      if(n_con_groups == RECV_SEND_PAIR)
	prjn->send_idx = orig_send_idx + r;

      // then its full connectivity..
      Unit* ru, *su;
      taLeafItr ru_itr, su_itr;
      FOR_ITR_EL(Unit, ru, rgp->, ru_itr) {
	ru->RecvConsPreAlloc(sgp->leaves, prjn);
      }
      FOR_ITR_EL(Unit, su, sgp->, su_itr) {
	su->SendConsPreAlloc(rgp->leaves, prjn);
      }

      FOR_ITR_EL(Unit, ru, rgp->, ru_itr) {
	FOR_ITR_EL(Unit, su, sgp->, su_itr) {
	  if(self_con || (ru != su))
	    ru->ConnectFrom(su, prjn);
	}
      }
    }
  }

  prjn->recv_idx = orig_recv_idx;
  prjn->send_idx = orig_send_idx;
}


/////////////////////////////
//	GpRndTessel	   //
/////////////////////////////

void GpTessEl::Initialize() {
  p_con = 1.0f;
}

void GpRndTesselPrjnSpec::Initialize() {
  recv_gp_n = -1;
  recv_gp_skip = 1;
  recv_gp_group = 1;
  send_gp_scale = 1.0f;
  send_gp_offs.SetBaseType(&TA_GpTessEl);

  wrap = true;
  def_p_con = .25f;
  sym_self = true;
  same_seed = false;
  rndm_seed.GetCurrent();
}

void GpRndTesselPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  recv_gp_skip.SetGtEq(1);
  recv_gp_group.SetGtEq(1);
}

void GpRndTesselPrjnSpec::MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y) {
  send_gp_offs.Reset();
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
	GpTessEl* te = (GpTessEl*)send_gp_offs.New(1, &TA_GpTessEl);
	te->send_gp_off.x = x;
	te->send_gp_off.y = y;
	te->p_con = def_p_con;
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
	GpTessEl* te = (GpTessEl*)send_gp_offs.New(1, &TA_GpTessEl);
	te->send_gp_off.x = x;
	te->send_gp_off.y = y;
	te->p_con = def_p_con;
      }
    }
  }
}

void GpRndTesselPrjnSpec::MakeRectangle(int width, int height, int left, int bottom) {
  send_gp_offs.Reset();
  int y;
  for(y = bottom; y < bottom + height; y++) {
    int x;
    for(x = left; x < left + width; x++) {
      GpTessEl* te = (GpTessEl*)send_gp_offs.New(1, &TA_GpTessEl);
      te->send_gp_off.x = x;
      te->send_gp_off.y = y;
      te->p_con = def_p_con;
    }
  }
}

void GpRndTesselPrjnSpec::SetPCon(float p_con, int start, int end) {
  if(end == -1)	end = send_gp_offs.size;  else end = MIN(send_gp_offs.size, end);
  int i;
  for(i=start;i<end;i++) {
    GpTessEl* te = (GpTessEl*)send_gp_offs.FastEl(i);
    te->p_con = p_con;
  }
}


void GpRndTesselPrjnSpec::GetCtrFmRecv(TwoDCoord& sctr, TwoDCoord ruc) {
  ruc -= recv_gp_off;
  ruc /= recv_gp_group;	ruc *= recv_gp_group;	// this takes int part of
  ruc += recv_gp_off;	// then re-add offset
  FloatTwoDCoord scruc = ruc;
  scruc *= send_gp_scale;
  sctr = scruc;		// center of sending units
  sctr += send_gp_border;
}

void GpRndTesselPrjnSpec::Connect_Gps(int rgpidx, int sgpidx, float p_con,
				      Projection* prjn, bool send_alloc) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  bool same_gp = (recv_lay == send_lay && rgpidx == sgpidx);

  if(send_alloc) {
    int send_no = 1;
    int recv_no = 1;
    if(p_con < 1.0f) {
      if(p_con < 0) p_con = 1.0f;
      if(!self_con && same_gp)
	recv_no = (int) ((p_con * (float)(su_nunits-1)) + .5f);
      else
	recv_no = (int) ((p_con * (float)su_nunits) + .5f);
      if(recv_no <= 0)  recv_no = 1;
      recv_no = MAX(su_nunits, recv_no);

      // sending number is even distribution across senders plus some imbalance factor
      float send_no_flt = (float)(ru_nunits * recv_no) / (float)su_nunits;
      // add SEM as corrective factor
      float send_sem = send_no_flt / sqrtf(send_no_flt);
      send_no = (int)(send_no_flt + 2.0f * send_sem + 5.0f);
      send_no = MIN(ru_nunits, send_no);

      if(same_gp)
	recv_no += 2;		// bit of extra room here too
    }
    else {
      send_no = ru_nunits;
      recv_no = su_nunits;
    }
    for(int sui=0;sui<su_nunits;sui++) {
      Unit* su = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      su->SendConsAllocInc(send_no, prjn);
    }

    // also do incremental alloc of the recv guy too!
    for(int rui=0;rui<ru_nunits;rui++) {
      Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      ru->RecvConsAllocInc(recv_no, prjn);
    }
    return;
  }

  if(p_con < 0) {		// this means: make symmetric connections!
    Connect_Gps_Sym(rgpidx, sgpidx, p_con, prjn);
  }
  else if(p_con == 1.0f) {
    Connect_Gps_Full(rgpidx, sgpidx, prjn);
  }
  else if(same_gp && sym_self) {
    Connect_Gps_SymSameGp(rgpidx, sgpidx, p_con, prjn);
  }
  else {
    if(recv_lay == send_lay && sym_self) {
      Connect_Gps_SymSameLay(rgpidx, sgpidx, p_con, prjn);
    }
    else {
      Connect_Gps_Std(rgpidx, sgpidx, p_con, prjn);
    }
  }
}

void GpRndTesselPrjnSpec::Connect_Gps_Sym(int rgpidx, int sgpidx,
					  float p_con, Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  if((prjn->from.ptr() != prjn->layer) || !sym_self)
    return;			// not applicable otherwise!

  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
    for(int g=0;g<ru->send.size;g++) {
      SendCons* scg = ru->send.FastEl(g);
      if((scg->prjn->layer != send_lay) || (scg->prjn->layer != recv_lay))
	continue;		// only deal with self projections to this same layer
      for(int i=0;i<scg->size;i++) {
	Unit* su = scg->Un(i);
	// only connect if this sender is in actual group I'm trying to connect
	int osgpidx = su->UnitGpIdx();
	if(osgpidx == sgpidx) {
	  ru->ConnectFromCk(su, prjn);
	}
      }
    }
  }
}

void GpRndTesselPrjnSpec::Connect_Gps_SymSameGp(int rgpidx, int sgpidx,
				float p_con, Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  // trick is to divide cons in half, choose recv, send at random
  // for 1/2 cons, then go through all units and make the symmetric cons..
  // pre-allocate connections!
  TestWarning(p_con > .95f, "Connect_Gps_SymSameGp",
	      "usually produces less than complete connectivity for high values of p_con in symmetric, self-connected layers using permute!");
  int n_cons;
  if(!self_con)
    n_cons = (int) (p_con * (float)(su_nunits-1) + .5f);
  else
    n_cons = (int) (p_con * (float)su_nunits + .5f);
  int first = (int)(.5f * (float)n_cons);
  if(first <= 0) first = 1;


  UnitPtrList ru_list;		// receiver permution list
  for(int rui=0; rui<ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
    ru_list.Link(ru);			// on making a symmetric connection in first pass
  }
  ru_list.Permute();

  UnitPtrList perm_list;
  for(int i=0;i<ru_list.size; i++) {
    Unit* ru = ru_list.FastEl(i);
    perm_list.Reset();
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      if(!self_con && (ru == su)) continue;
      // don't connect to anyone who already recvs from me cuz that will make
      // a symmetric connection which isn't good: symmetry will be enforced later
      RecvCons* scg = su->recv.FindPrjn(prjn);
      if(scg->FindConFromIdx(ru) >= 0) continue;
      perm_list.Link(su);
    }
    perm_list.Permute();

    for(int j=0; j<first && j<perm_list.size; j++) {	// only connect 1/2 of the units
      ru->ConnectFromCk((Unit*)perm_list[j], prjn);
    }
  }
  // now go thru and make the symmetric connections
  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
    SendCons* scg = ru->send.FindPrjn(prjn);
    if(scg == NULL) continue;
    for(int i=0;i<scg->size;i++) {
      Unit* su = scg->Un(i);
      ru->ConnectFromCk(su, prjn);
    }
  }
}

void GpRndTesselPrjnSpec::Connect_Gps_SymSameLay(int rgpidx, int sgpidx,
						 float p_con, Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  // within the same layer, i want to make connections symmetric: either i'm the
  // first to connect to other group, or other group has already connected to me
  // so I should just make symmetric versions of its connections
  // take first send unit and find if it recvs from anyone in this prjn yet
  Unit* su = send_lay->UnitAtUnGpIdx(0, sgpidx);
  RecvCons* scg = su->recv.FindPrjn(prjn);
  if((scg != NULL) && (scg->size > 0)) {	// sender has been connected already: try to connect me!
    int n_con = 0;		// number of actual connections made

    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      SendCons* scg = ru->send.FindPrjn(prjn);
      if(scg == NULL) continue;
      for(int i=0;i<scg->size;i++) {
	Unit* su = scg->Un(i);
	// only connect if this sender is in actual group I'm trying to connect
	int osgpidx = su->UnitGpIdx();
	if(osgpidx == sgpidx) {
	  if(ru->ConnectFromCk(su, prjn))
	    n_con++;
	}
      }
    }
    if(n_con > 0)		// made some connections, bail
      return;
    // otherwise, go ahead and make new connections!
  }
  Connect_Gps_Std(rgpidx, sgpidx, p_con, prjn);
}

void GpRndTesselPrjnSpec::Connect_Gps_Std(int rgpidx, int sgpidx,
					  float p_con, Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  bool same_gp = (recv_lay == send_lay && rgpidx == sgpidx);

  int recv_no;
  if(!self_con && same_gp)
    recv_no = (int) ((p_con * (float)(su_nunits-1)) + .5f);
  else
    recv_no = (int) ((p_con * (float)su_nunits) + .5f);
  if(recv_no <= 0)  recv_no = 1;

  UnitPtrList perm_list;	// permution list
  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
    perm_list.Reset();
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      if(!self_con && (ru == su)) continue;
      perm_list.Link(su);
    }
    for(int i=0; i<recv_no; i++)
      ru->ConnectFrom((Unit*)perm_list[i], prjn);
  }
}

void GpRndTesselPrjnSpec::Connect_Gps_Full(int rgpidx, int sgpidx,
					   Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  for(int rui=0; rui < ru_nunits; rui++) {
    Unit* ru = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      if(self_con || (ru != su))
	ru->ConnectFrom(su, prjn);
    }
  }
}

void GpRndTesselPrjnSpec::Connect_RecvGp(int rgpidx, const TwoDCoord& ruc,
					 Projection* prjn, bool send_alloc) {

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  TwoDCoord& su_geo = send_lay->gp_geom;
  TwoDCoord sctr;
  GetCtrFmRecv(sctr, ruc);  // positions of center of recv in sending layer
  for(int i = 0; i< send_gp_offs.size; i++) {
    GpTessEl* te = (GpTessEl*)send_gp_offs.FastEl(i);
    TwoDCoord suc = te->send_gp_off + sctr;
    if(suc.WrapClip(wrap, su_geo) && !wrap)
      continue;
    int sgpidx = send_lay->UnitGpIdxFmPos(suc);
    if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;
    Connect_Gps(rgpidx, sgpidx, te->p_con, prjn, send_alloc);
  }
}

void GpRndTesselPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(same_seed)
    rndm_seed.OldSeed();

  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;

  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
		 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups,  "Connect_impl",
		 "requires send layer to have unit groups!")) {
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  TwoDCoord& ru_geo = prjn->layer->gp_geom;
  TwoDCoord use_recv_gp_n = recv_gp_n;

  if(recv_gp_n.x == -1)
    use_recv_gp_n.x = ru_geo.x;
  if(recv_gp_n.y == -1)
    use_recv_gp_n.y = ru_geo.y;

  TwoDCoord ruc, nuc;
  for(int alloc_loop=1; alloc_loop >= 0; alloc_loop--) {
    for(ruc.y = recv_gp_off.y, nuc.y = 0; (ruc.y < ru_geo.y) && (nuc.y < use_recv_gp_n.y);
	ruc.y += recv_gp_skip.y, nuc.y++)
      {
	for(ruc.x = recv_gp_off.x, nuc.x = 0; (ruc.x < ru_geo.x) && (nuc.x < use_recv_gp_n.x);
	    ruc.x += recv_gp_skip.x, nuc.x++)
	  {
	    int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
	    if(!recv_lay->UnitGpIdxIsValid(rgpidx)) continue;
	    Connect_RecvGp(rgpidx, ruc, prjn, alloc_loop);
	  }
      }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      recv_lay->RecvConsPostAlloc(prjn);
      send_lay->SendConsPostAlloc(prjn);
    }
  }
}

///////////////////////////////////////////////////////
//		TiledRFPrjnSpec
///////////////////////////////////////////////////////

void TiledRFPrjnSpec::Initialize() {
  recv_gp_border = 0;
  recv_gp_ex_st = -1;
  recv_gp_ex_n = 0;
  send_border = 0;
  send_adj_rfsz = 0;
  send_adj_sndloc = 0;
  rf_width_mult = 1.0f;
}

bool TiledRFPrjnSpec::InitRFSizes(Projection* prjn) {
  if(!(bool)prjn->from)	return false;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return false;
  if(TestWarning(!prjn->layer->unit_groups, "InitRFSizes",
		 "requires recv layer to have unit groups!")) {
    return false;
  }

  ru_geo = prjn->layer->gp_geom;
  recv_gp_ed = ru_geo - recv_gp_border;
  recv_gp_ex_ed = recv_gp_ex_st + recv_gp_ex_n;

  su_act_geom = prjn->from->flat_geom;

// 0 1 2 3 4 5 6 7 8 9 a b c d e = 14+1 = 15
// 0 0 0 0 0 0
//       1 1 1 1 1 1
//             2 2 2 2 2 2
//                   3 3 3 3 3 3
//
// 4 gps, ovlp = 3, width = 6 
// ovlp = tot / (nr + 1) = 15 / 5 = 3

// send scale:
// ovlp / recv_gp size 

// send off:
// - recv off * scale

  n_recv_gps = ru_geo - (2 * recv_gp_border);	// total number of recv groups covered
  n_send_units = TwoDCoord(su_act_geom) - (2 * send_border);

  rf_ovlp.x = (int)floor(((float)(n_send_units.x + send_adj_rfsz.x) / (float)(n_recv_gps.x + 1)) + .5f);
  rf_ovlp.y = (int)floor(((float)(n_send_units.y + send_adj_rfsz.y) / (float)(n_recv_gps.y + 1)) + .5f);

  // how to move the receptive fields over the sending layer (floating point)
  rf_move = FloatTwoDCoord(n_send_units + send_adj_sndloc) / FloatTwoDCoord(n_recv_gps + 1);

  FloatTwoDCoord rfw = (FloatTwoDCoord)rf_ovlp * 2.0f * rf_width_mult;
  rf_width = rfw;
//   cerr << "prjn: " << name << " layer: " << prjn->layer->name << " from: " << prjn->from->name
//        << " rf size: " << rf_ovlp.x << ", " << rf_ovlp.y
//        << " act send size: " << rf_ovlp.x * (n_recv_gps.x + 1)
//        << ", " << rf_ovlp.y * (n_recv_gps.y + 1)
//        << " trg send size: " << n_send_units.x << ", " << n_send_units.y
//        << endl;
  return true;
}

void TiledRFPrjnSpec::Connect_impl(Projection* prjn) {
  if(!InitRFSizes(prjn)) return;
  int n_cons = rf_width.Product();
  TwoDCoord ruc;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(ruc.y = recv_gp_border.y; ruc.y < recv_gp_ed.y; ruc.y++) {
      for(ruc.x = recv_gp_border.x; ruc.x < recv_gp_ed.x; ruc.x++) {

	if((ruc.y >= recv_gp_ex_st.y) && (ruc.y < recv_gp_ex_ed.y) &&
	   (ruc.x >= recv_gp_ex_st.x) && (ruc.x < recv_gp_ex_ed.x)) continue;

	int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
	if(!recv_lay->UnitGpIdxIsValid(rgpidx)) continue;

	TwoDCoord su_st;
	su_st.x = send_border.x + (int)floor((float)(ruc.x - recv_gp_border.x) * rf_move.x);
	su_st.y = send_border.y + (int)floor((float)(ruc.y - recv_gp_border.y) * rf_move.y);

	TwoDCoord su_ed = su_st + rf_width;

	for(int rui=0;rui < ru_nunits; rui++) {
	  Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
	  if(!alloc_loop)
	    ru_u->RecvConsPreAlloc(n_cons, prjn);

	  TwoDCoord suc;
	  for(suc.y = su_st.y; suc.y < su_ed.y; suc.y++) {
	    for(suc.x = su_st.x; suc.x < su_ed.x; suc.x++) {
	      Unit* su_u = prjn->from->UnitAtCoord(suc);
	      if(su_u == NULL) continue;
	      if(!self_con && (su_u == ru_u)) continue;
	      ru_u->ConnectFrom(su_u, prjn, alloc_loop);
	    }
	  }
	}
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

int TiledRFPrjnSpec::ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt) {
  if(!InitRFSizes(prjn)) return 0;
  int rval = 0;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int n_cons = rf_width.x * rf_width.y;
  int n_new_cons = (int)(p_add_con * (float)n_cons);
  if(n_new_cons <= 0) return 0;
  int_Array new_idxs;
  new_idxs.SetSize(n_cons);
  new_idxs.FillSeq();

  TwoDCoord ruc;
  for(ruc.y = recv_gp_border.y; ruc.y < recv_gp_ed.y; ruc.y++) {
    for(ruc.x = recv_gp_border.x; ruc.x < recv_gp_ed.x; ruc.x++) {
      if((ruc.y >= recv_gp_ex_st.y) && (ruc.y < recv_gp_ex_ed.y) &&
	 (ruc.x >= recv_gp_ex_st.x) && (ruc.x < recv_gp_ex_ed.x)) continue;

      int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
      if(!recv_lay->UnitGpIdxIsValid(rgpidx)) continue;

      TwoDCoord su_st;
      su_st.x = send_border.x + (int)floor((float)(ruc.x - recv_gp_border.x) * rf_move.x);
      su_st.y = send_border.y + (int)floor((float)(ruc.y - recv_gp_border.y) * rf_move.y);

      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);

	new_idxs.Permute();

	for(int i=0;i<n_new_cons;i++) {
	  int su_idx = new_idxs[i];
	  TwoDCoord suc;
	  suc.y = su_idx / rf_width.x;
	  suc.x = su_idx % rf_width.x;
	  suc += su_st;
	  Unit* su_u = prjn->from->UnitAtCoord(suc);
	  if(su_u == NULL) continue;
	  if(!self_con && (su_u == ru_u)) continue;
	  Connection* cn = ru_u->ConnectFromCk(su_u, prjn); // gotta check!
	  if(cn) {
	    cn->wt = init_wt;
	    rval++;
	  }
	}
      }
    }
  }
  return rval;
}

void TiledRFPrjnSpec::SelectRF(Projection* prjn) {
  if(!InitRFSizes(prjn)) return;
  // todo: fixme

  /*  Network* net = prjn->layer->own_net;
  if(net == NULL) return;
  NetView* nv = (NetView*)net->views.DefaultEl();
  if(nv == NULL) return;

  taBase_List* selgp = nv->GetSelectGroup();
  selgp->Reset();

  TwoDCoord ruc;
  for(ruc.y = recv_gp_border.y; ruc.y < recv_gp_ed.y; ruc.y++) {
    for(ruc.x = recv_gp_border.x; ruc.x < recv_gp_ed.x; ruc.x++) {
      if((ruc.y >= recv_gp_ex_st.y) && (ruc.y < recv_gp_ex_ed.y) &&
	 (ruc.x >= recv_gp_ex_st.x) && (ruc.x < recv_gp_ex_ed.x)) continue;
      Unit_Group* ru_gp = prjn->layer->UnitGpAtCoord(ruc);
      if(ru_gp == NULL) continue;

      selgp->LinkUnique(ru_gp);

      TwoDCoord su_st;
      su_st.x = send_border.x + (int)floor((float)(ruc.x - recv_gp_border.x) * rf_move.x);
      su_st.y = send_border.y + (int)floor((float)(ruc.y - recv_gp_border.y) * rf_move.y);

      TwoDCoord suc;
      for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
	for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
	  Unit* su_u = prjn->from->UnitAtCoord(suc);
	  if(su_u == NULL) continue;

	  selgp->LinkUnique(su_u);
	}
      }
    }
  }
  nv->UpdateSelect();
  */
}

//////////////////////////////////////////////////////////
//  	TiledGpRFPrjnSpec

void TiledGpRFPrjnSpec::Initialize() {
  send_gp_size = 4;
  send_gp_skip = 2;
  wrap = false;
  reciprocal = false;
}

void TiledGpRFPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
		 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
		 "requires send layer to have unit groups!")) {
    return;
  }

  if(reciprocal) {
    Connect_Reciprocal(prjn);
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord ru_geo = recv_lay->gp_geom;
  TwoDCoord su_geo = send_lay->gp_geom;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int sg_sz_tot = send_gp_size.Product();
  int alloc_no = sg_sz_tot * su_nunits;

  TwoDCoord ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
	TwoDCoord su_st;
	if(wrap)	su_st = (ruc-1) * send_gp_skip;
	else		su_st = ruc * send_gp_skip;

	for(int rui=0; rui < ru_nunits; rui++) {
	  Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
	  if(!alloc_loop)
	    ru_u->RecvConsPreAlloc(alloc_no, prjn);
	}

	TwoDCoord suc;
	TwoDCoord suc_wrp;
	for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
	  for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
	    suc_wrp = suc;
	    if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
	      continue;
	    int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
	    if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

	    Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, sgpidx, alloc_loop);
	  }
	}
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void TiledGpRFPrjnSpec::Connect_Reciprocal(Projection* prjn) {
  Layer* recv_lay = prjn->from;	// from perspective of non-recip!
  Layer* send_lay = prjn->layer;
  TwoDCoord ru_geo = recv_lay->gp_geom;
  TwoDCoord su_geo = send_lay->gp_geom;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int_Array alloc_sz;
  alloc_sz.SetSize(su_geo.Product()); // alloc sizes per each su unit group
  alloc_sz.InitVals(0);
    
  TwoDCoord ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      TwoDCoord su_st;
      if(wrap)	su_st = (ruc-1) * send_gp_skip;
      else	su_st = ruc * send_gp_skip;

      TwoDCoord suc;
      TwoDCoord suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
	for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
	  suc_wrp = suc;
	  if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
	    continue;
	  int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
	  if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;
	  alloc_sz[sgpidx] += ru_nunits;
	}
      }
    }
  }

  // now actually allocate
  for(int sug=0; sug < send_lay->gp_geom.n; sug++) {
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sug);
      su_u->RecvConsPreAlloc(alloc_sz[sug], prjn);
    }
  }

  // then connect
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
	TwoDCoord su_st;
	if(wrap)	su_st = (ruc-1) * send_gp_skip;
	else		su_st = ruc * send_gp_skip;

	TwoDCoord suc;
	TwoDCoord suc_wrp;
	for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
	  for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
	    suc_wrp = suc;
	    if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
	      continue;
	    int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
	    if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;
	    
	    Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, sgpidx, alloc_loop);
	  }
	}
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void TiledGpRFPrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay, Layer* send_lay,
					  int rgpidx, int sgpidx, int alloc_loop) {
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  if(reciprocal) {		// reciprocal is backwards!
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      for(int rui=0; rui < ru_nunits; rui++) {
	Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
	if(!self_con && (su_u == ru_u)) continue;
	su_u->ConnectFrom(ru_u, prjn, alloc_loop); // recip!
      }
    }
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      for(int sui=0; sui < su_nunits; sui++) {
	Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
	if(!self_con && (su_u == ru_u)) continue;
	ru_u->ConnectFrom(su_u, prjn, alloc_loop); // recip!
      }
    }
  }
}

int TiledGpRFPrjnSpec::ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt) {
  if(!(bool)prjn->from)	return 0;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return 0;
  if(TestWarning(!prjn->layer->unit_groups, "ProbAddCons_impl",
		 "requires recv layer to have unit groups!")) {
    return 0;
  }
  if(TestWarning(!prjn->from->unit_groups, "ProbAddCons_impl",
		 "requires send layer to have unit groups!")) {
    return 0;
  }

  int rval = 0;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {
    recv_lay = prjn->from;
    send_lay = prjn->layer;
  }

  TwoDCoord ru_geo = recv_lay->gp_geom;
  TwoDCoord su_geo = send_lay->gp_geom;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  TwoDCoord ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      TwoDCoord su_st;
      if(wrap)	su_st = (ruc-1) * send_gp_skip;
      else	su_st = ruc * send_gp_skip;

      TwoDCoord suc;
      TwoDCoord suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
	for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
	  suc_wrp = suc;
	  if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
	    continue;
	  int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
	  if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

	  for(int rui=0; rui < ru_nunits; rui++) {
	    for(int sui=0; sui < su_nunits; sui++) {
	      Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
              Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
	      if(!self_con && (su_u == ru_u)) continue;

	      // just do a basic probabilistic version: too hard to permute..
	      if(Random::ZeroOne() > p_add_con) continue; // no-go

	      Connection* con;
	      if(!reciprocal)
		con = ru_u->ConnectFromCk(su_u, prjn); // gotta check!
	      else
		con = su_u->ConnectFromCk(ru_u, prjn);
	      if(con) {
		con->wt = init_wt;
		rval++;
	      }
	    }
	  }
	}
      }
    }
  }
  return rval;
}

bool TiledGpRFPrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
  trg_send_geom.x = send_x;
  trg_send_geom.y = send_y;

  if(wrap)
    trg_recv_geom = (trg_send_geom / send_gp_skip);
  else
    trg_recv_geom = (trg_send_geom / send_gp_skip) - 1;

  // now fix it the other way
  if(wrap)
    trg_send_geom = (trg_recv_geom * send_gp_skip);
  else
    trg_send_geom = ((trg_recv_geom +1) * send_gp_skip);

  DataChanged(DCR_ITEM_UPDATED);
  return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
}

bool TiledGpRFPrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
  trg_recv_geom.x = recv_x;
  trg_recv_geom.y = recv_y;

  if(wrap)
    trg_send_geom = (trg_recv_geom * send_gp_skip);
  else
    trg_send_geom = ((trg_recv_geom+1) * send_gp_skip);

  // now fix it the other way
  if(wrap)
    trg_recv_geom = (trg_send_geom / send_gp_skip);
  else
    trg_recv_geom = (trg_send_geom / send_gp_skip) - 1;

  DataChanged(DCR_ITEM_UPDATED);
  return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
}


///////////////////////////////////////////////////////
//		TiledNovlpPrjnSpec
///////////////////////////////////////////////////////

void TiledNovlpPrjnSpec::Initialize() {
  reciprocal = false;
}

bool TiledNovlpPrjnSpec::InitRFSizes(Projection* prjn) {
  if(!(bool)prjn->from)	return false;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return false;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {
    recv_lay = prjn->from;
    send_lay = prjn->layer;
  }

  if(TestWarning(!recv_lay->unit_groups, "InitRFSizes",
		 "requires recv layer to have unit groups!")) {
    return false;
  }

  ru_geo = recv_lay->gp_geom;

  su_act_geom = send_lay->flat_geom;

  rf_width.x = (float)su_act_geom.x / (float)ru_geo.x;
  rf_width.y = (float)su_act_geom.y / (float)ru_geo.y;

  return true;
}

void TiledNovlpPrjnSpec::Connect_impl(Projection* prjn) {
  if(!InitRFSizes(prjn)) return;

  if(reciprocal) {
    Connect_Reciprocal(prjn);
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  TwoDCoord ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
	TwoDCoord su_st;
	su_st.x = (int)((float)ruc.x * rf_width.x);
	su_st.y = (int)((float)ruc.y * rf_width.y);

	for(int rui=0; rui < ru_nunits; rui++) {
	  Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
	  int alloc_sz = ((int)(rf_width.x) + 1) * ((int)(rf_width.y) + 1);
	  if(!alloc_loop)
	    ru_u->RecvConsPreAlloc(alloc_sz, prjn);	

	  TwoDCoord suc;
	  for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
	    for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
	      Unit* su_u = send_lay->UnitAtCoord(suc);
	      if(su_u == NULL) continue;

	      if(!self_con && (su_u == ru_u)) continue;
	      ru_u->ConnectFrom(su_u, prjn, alloc_loop);
	    }
	  }
	}
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void TiledNovlpPrjnSpec::Connect_Reciprocal(Projection* prjn) {
  Layer* recv_lay = prjn->from;
  Layer* send_lay = prjn->layer;
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int_Array alloc_sz;
  alloc_sz.SetSize(send_lay->flat_geom.Product());
  alloc_sz.InitVals(0);

  // find alloc sizes
  TwoDCoord ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      TwoDCoord su_st;
      su_st.x = (int)((float)ruc.x * rf_width.x);
      su_st.y = (int)((float)ruc.y * rf_width.y);

      TwoDCoord suc;
      for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
	for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
	  Unit* su_u = send_lay->UnitAtCoord(suc);
	  if(su_u == NULL) continue;
	  int sugp_idx = suc.y * send_lay->flat_geom.x + suc.x;
	  alloc_sz[sugp_idx] += ru_nunits;
	}
      }
    }
  }

  // do the alloc
  TwoDCoord suc;
  for(suc.y = 0; suc.y < send_lay->flat_geom.y; suc.y++) {
    for(suc.x = 0; suc.x < send_lay->flat_geom.x; suc.x++) {
      Unit* su_u = send_lay->UnitAtCoord(suc);
      if(su_u == NULL) continue;
      int sugp_idx = suc.y * send_lay->flat_geom.x + suc.x;
      su_u->RecvConsPreAlloc(alloc_sz[sugp_idx], prjn);
    }
  }

  // then make the connections!
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
	TwoDCoord su_st;
	su_st.x = (int)((float)ruc.x * rf_width.x);
	su_st.y = (int)((float)ruc.y * rf_width.y);

	TwoDCoord suc;
	for(suc.y = su_st.y; suc.y < su_st.y + rf_width.y; suc.y++) {
	  for(suc.x = su_st.x; suc.x < su_st.x + rf_width.x; suc.x++) {
	    Unit* su_u = send_lay->UnitAtCoord(suc);
	    if(su_u == NULL) continue;

	    for(int rui=0; rui < ru_nunits; rui++) {
	      Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
	      if(!self_con && (su_u == ru_u)) continue;
	      su_u->ConnectFrom(ru_u, prjn, alloc_loop);
	    }
	  }
	}
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

///////////////////////////////////////////////////////////////////
// 	GaussRFPrjnSpec

void GaussRFPrjnSpec::Initialize() {
  init_wts = true;
  rf_width = 6;
  rf_move = 3.0f;
  wrap = false;
  gauss_sigma = 1.0f;
}

void GaussRFPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;

  int n_cons = rf_width.Product();
  TwoDCoord rf_half_wd = rf_width / 2;
  TwoDCoord rug_geo = prjn->layer->flat_geom; // uses flat geom -- not ug based
  TwoDCoord su_geo = prjn->from->flat_geom;

  TwoDCoord ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(ruc.y = 0; ruc.y < rug_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < rug_geo.x; ruc.x++) {
	Unit* ru_u = prjn->layer->UnitAtCoord(ruc);
	if(!ru_u) continue;

	if(!alloc_loop)
	  ru_u->RecvConsPreAlloc(n_cons, prjn);

	TwoDCoord su_st;
	if(wrap) {
	  su_st.x = (int)floor((float)ruc.x * rf_move.x) - rf_half_wd.x;
	  su_st.y = (int)floor((float)ruc.y * rf_move.y) - rf_half_wd.y;
	}
	else {
	  su_st.x = (int)floor((float)ruc.x * rf_move.x);
	  su_st.y = (int)floor((float)ruc.y * rf_move.y);
	}

	su_st.WrapClip(wrap, su_geo);
	TwoDCoord su_ed = su_st + rf_width;
	if(wrap) {
	  su_ed.WrapClip(wrap, su_geo); // just wrap ends too
	}
	else {
	  if(su_ed.x > su_geo.x) {
	    su_ed.x = su_geo.x; su_st.x = su_ed.x - rf_width.x;
	  }
	  if(su_ed.y > su_geo.y) {
	    su_ed.y = su_geo.y; su_st.y = su_ed.y - rf_width.y;
	  }
	}


	TwoDCoord suc;
	TwoDCoord suc_wrp;
	for(suc.y = 0; suc.y < rf_width.y; suc.y++) {
	  for(suc.x = 0; suc.x < rf_width.x; suc.x++) {
	    suc_wrp = su_st + suc;
	    if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
	      continue;
	    Unit* su_u = prjn->from->UnitAtCoord(suc_wrp);
	    if(su_u == NULL) continue;
	    if(!self_con && (su_u == ru_u)) continue;

	    ru_u->ConnectFrom(su_u, prjn, alloc_loop); // don't check: saves lots of time!
	  }
	}
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void GaussRFPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init

  TwoDCoord rf_half_wd = rf_width / 2;
  FloatTwoDCoord rf_ctr = rf_half_wd;
  if(rf_half_wd * 2 == rf_width) // even
    rf_ctr -= .5f;

  float sig_sq = gauss_sigma * gauss_sigma;

  for(int i=0; i<cg->size; i++) {
    int su_x = i % rf_width.x;
    int su_y = i / rf_width.x;

    float dst = taMath_float::euc_dist_sq(su_x, su_y, rf_ctr.x, rf_ctr.y);
    float wt = expf(-0.5 * dst / sig_sq);

    cg->Cn(i)->wt = wt;
  }
}

bool GaussRFPrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
  trg_send_geom.x = send_x;
  trg_send_geom.y = send_y;

  if(wrap)
    trg_recv_geom = (trg_send_geom / rf_move);
  else
    trg_recv_geom = (trg_send_geom / rf_move) - 1;

  // now fix it the other way
  if(wrap)
    trg_send_geom = (trg_recv_geom * rf_move);
  else
    trg_send_geom = ((trg_recv_geom +1) * rf_move);

  DataChanged(DCR_ITEM_UPDATED);
  return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
}

bool GaussRFPrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
  trg_recv_geom.x = recv_x;
  trg_recv_geom.y = recv_y;

  if(wrap)
    trg_send_geom = (trg_recv_geom * rf_move);
  else
    trg_send_geom = ((trg_recv_geom+1) * rf_move);

  // now fix it the other way
  if(wrap)
    trg_recv_geom = (trg_send_geom / rf_move);
  else
    trg_recv_geom = (trg_send_geom / rf_move) - 1;

  DataChanged(DCR_ITEM_UPDATED);
  return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
}


/////////////////////////////////////
//	  GradientWtsPrjnSpec	   //
/////////////////////////////////////

void GradientWtsPrjnSpec::Initialize() {
  wt_range.min = 0.0f;
  wt_range.max = 0.5f;
  wt_range.UpdateAfterEdit_NoGui();
  invert = false;
  grad_x = true;
  grad_y = true;
  wrap = true;
  grad_type = LINEAR;
  use_gps = true;
  gauss_sig = 0.3f;
  Defaults_init();
}

void GradientWtsPrjnSpec::Defaults_init() {
  init_wts = true;
  add_rnd_wts = true;
}


void GradientWtsPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  if(use_gps && prjn->layer->unit_groups)
    InitWeights_RecvGps(prjn, cg, ru);
  else
    InitWeights_RecvFlat(prjn, cg, ru);
}


void GradientWtsPrjnSpec::SetWtFmDist(Projection* prjn, RecvCons* cg, Unit* ru, float dist, 
				      int cg_idx) {
  float wt_val;
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

///////////////////////////////////////////////
//	0	1	2	3    recv
//      0	.33     .66	1    rgp_x
//	  			
//	0	1	2	3    send
//      0	.33	.66	1    sgp_x
//      4	5	6	7    wrp_x > .5  int
//	1.33	1.66	2	2.33 wrp_x > .5  flt
//      -4	-3	-2	-1   wrp_x < .5  int
//      -1.33   -1	-.66	-.33 wrp_x < .5	 flt

void GradientWtsPrjnSpec::InitWeights_RecvGps(Projection* prjn, RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  Unit* lru = (Unit*)ru;
  int rgpidx = lru->UnitGpIdx();
  TwoDCoord rgp_pos = recv_lay->UnitGpPosFmIdx(rgpidx); // position relative to overall gp geom
  float rgp_x = (float)rgp_pos.x / (float)MAX(recv_lay->gp_geom.x-1, 1);
  float rgp_y = (float)rgp_pos.y / (float)MAX(recv_lay->gp_geom.y-1, 1);

  float max_dist = 1.0f;
  if(grad_x && grad_y)
    max_dist = sqrtf(2.0f);

  float mxs_x = (float)MAX(send_lay->flat_geom.x-1, 1);
  float mxs_y = (float)MAX(send_lay->flat_geom.y-1, 1);

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    TwoDCoord su_pos;
    su->GetLayerAbsPos(su_pos);
    float su_x = (float)su_pos.x / mxs_x;
    float su_y = (float)su_pos.y / mxs_y;

    float wrp_x, wrp_y;
    if(wrap) {
      if(rgp_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
      else	      	wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
      if(rgp_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
      else	      	wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
    }

    float dist;
    if(grad_x && grad_y) {
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
    else if(grad_x) {
      dist = fabsf(su_x - rgp_x);
      if(wrap) {
	float wrp_dist = fabsf(wrp_x - rgp_x);
	if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    else if(grad_y) {
      dist = fabsf(su_y - rgp_y);
      if(wrap) {
	float wrp_dist = fabsf(wrp_y - rgp_y);
	if(wrp_dist < dist) dist = wrp_dist;
      }
    }

    dist /= max_dist;		// keep it normalized

    SetWtFmDist(prjn, cg, ru, dist, i);
  }
}

void GradientWtsPrjnSpec::InitWeights_RecvFlat(Projection* prjn, RecvCons* cg, Unit* ru) {
  Layer* recv_lay = (Layer*)prjn->layer;
  Layer* send_lay = (Layer*)prjn->from.ptr();
  TwoDCoord ru_pos;
  ru->GetLayerAbsPos(ru_pos);
  float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom.x-1, 1);
  float ru_y = (float)ru_pos.y / (float)MAX(recv_lay->flat_geom.y-1, 1);

  float max_dist = 1.0f;
  if(grad_x && grad_y)
    max_dist = sqrtf(2.0f);

  float mxs_x = (float)MAX(send_lay->flat_geom.x-1, 1);
  float mxs_y = (float)MAX(send_lay->flat_geom.y-1, 1);

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    TwoDCoord su_pos;
    su->GetLayerAbsPos(su_pos);
    float su_x = (float)su_pos.x / mxs_x;
    float su_y = (float)su_pos.y / mxs_y;

    float wrp_x, wrp_y;
    if(wrap) {
      if(ru_x > .5f)   	wrp_x = (float)(su_pos.x + send_lay->flat_geom.x) / mxs_x;
      else	      	wrp_x = (float)(su_pos.x - send_lay->flat_geom.x) / mxs_x;
      if(ru_y > .5f)   	wrp_y = (float)(su_pos.y + send_lay->flat_geom.y) / mxs_y;
      else	      	wrp_y = (float)(su_pos.y - send_lay->flat_geom.y) / mxs_y;
    }

    float dist;
    if(grad_x && grad_y) {
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
    else if(grad_x) {
      dist = fabsf(su_x - ru_x);
      if(wrap) {
	float wrp_dist = fabsf(wrp_x - ru_x);
	if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    else if(grad_y) {
      dist = fabsf(su_y - ru_y);
      if(wrap) {
	float wrp_dist = fabsf(wrp_y - ru_y);
	if(wrp_dist < dist) dist = wrp_dist;
      }
    }

    dist /= max_dist;		// keep it normalized

    SetWtFmDist(prjn, cg, ru, dist, i);
  }
}

