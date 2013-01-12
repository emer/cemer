// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "ClustNode.h"
#include <float_Matrix>
#include <DataTable>
#include <DataCol>
#include <taMath_float>
#include <taMath_double>
#include <double_Matrix>
#include <MinMax>
#include <float_Data>
#include <String_Data>

#include <taMisc>


void ClustNode::Initialize() {
  pat = NULL;
  leaf_idx = -1;
  leaf_dists = NULL;
  par_dist = 0.0f;
  nn_dist = 0.0f;
  tmp_dist = 0.0f;
  y = 0.0f;
}

void ClustNode::InitLinks() {
  inherited::InitLinks();
  taBase::Own(children, this);
  taBase::Own(nns, this);
}

void ClustNode::CutLinks() {
  taBase::DelPointer((taBase**)&leaf_dists);
  taBase::DelPointer((taBase**)&pat);
  children.Reset();
  nns.Reset();
  inherited::CutLinks();
}

void ClustNode::SetPat(taMatrix* pt) {
  taBase::SetPointer((taBase**)&pat, pt);
}

void ClustNode::AddChild(ClustNode* nd, float dst) {
  ClustLink* lk = new ClustLink;
  taBase::SetPointer((taBase**)&(lk->node), nd);
  lk->dist = dst;
  children.Add(lk);
}

void ClustNode::LinkNN(ClustNode* nd, float dst) {
  ClustLink* lk = new ClustLink;
  taBase::SetPointer((taBase**)&(lk->node), nd);
  lk->dist = dst;
  nns.Add(lk);
}

bool ClustNode::RemoveChild(ClustNode* nd) {
  bool rval = false;
  for(int i=children.size-1;i>=0;i--) {
    if(GetChild(i) == nd) {
      children.RemoveIdx(i);
      rval = true;
    }
  }
  return rval;
}

int ClustNode::FindChild(ClustNode* nd) {
  for(int i=0;i<children.size;i++) {
    if(GetChild(i) == nd)
      return i;
  }
  return -1;
}

bool ClustNode::Cluster(taMath::DistMetric metric, bool norm, float tol) {
  if(!taMath::dist_larger_further(metric)) {
    taMisc::Error("Cluster requires distance metric where larger = further apart");
    return false;
  }
  if(children.size <= 1) {
    taMisc::Error("Cluster requires at least 2 items to operate on!");
    return false;
  }

  ClustNode* nd0 = GetChild(0);
  if((nd0->pat->GetDataValType() != VT_FLOAT) && (nd0->pat->GetDataValType() != VT_DOUBLE)) {
    taMisc::Error("Cluster: Pattern data must be either float or double");
    return false;
  }

  taBase::SetPointer((taBase**)&leaf_dists, new float_Matrix);
  leaf_dists->SetGeom(2, children.size, children.size);
  for(int i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    nd->leaf_idx = i;
    taBase::SetPointer((taBase**)&(nd->leaf_dists), leaf_dists);
    for(int j=0;j<children.size;j++) {
      ClustNode* ond = GetChild(j);
      float dst;
      if(nd->pat->GetDataValType() == VT_FLOAT)
	dst = taMath_float::vec_dist((float_Matrix*)nd->pat, (float_Matrix*)ond->pat, metric, norm, tol);
      else
	dst = (float)taMath_double::vec_dist((double_Matrix*)nd->pat, (double_Matrix*)ond->pat, metric, norm, tol);
      leaf_dists->FastEl(j,i) = dst;
    }
  }

  do {
    // set nearest neighbor pointers
    NNeighbors(metric, norm, tol);
    // find closest and make a new node
  } while (ClustOnClosest());

  SetParDists(0.0f);

  taBase::DelPointer((taBase**)&leaf_dists);
  return true;
}

void ClustNode::GraphData(DataTable* dt) {
  dt->StructUpdate(true);
  SetYs(0.5f);
  nn_dist = 0.0f;
  dt->Reset();
  DataCol* da_x = dt->NewColFloat("X");
  da_x->SetUserData("X_AXIS", true);
  DataCol* da_y = dt->NewColFloat("Y");
  da_y->SetUserData("PLOT_1", true);
  DataCol* da_l = dt->NewColString("Label");
  da_l->SetUserData("PLOT_2", true);
  GraphData_impl(dt);

  MinMax xmm;
  da_x->GetMinMaxScale(xmm);
  da_x->SetUserData("MAX", xmm.max * 1.15f);

  MinMax ymm;
  da_y->GetMinMaxScale(ymm);
  da_y->SetUserData("MAX", ymm.max + .3f);
  da_y->SetUserData("MIN", 0.2f);
  dt->StructUpdate(false);
}

void ClustNode::GraphData_impl(DataTable* dt) {
  if(pat) {			// leaf
    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist - nn_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);

    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);
    dt->SetValAsString(name, 2, -1);

    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist - nn_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);

    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist - nn_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);
  }
  else {
    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist - nn_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);

    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);

    int i;
    for(i=0; i<children.size; i++) {
      ClustLink* nd = (ClustLink*)children[i];
      nd->node->GraphData_impl(dt);
      dt->AddBlankRow();
      dt->SetValAsFloat(par_dist, 0, -1);
      dt->SetValAsFloat(y, 1, -1);
    }

    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist - nn_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);
  }
}

static const float clust_dist_tol = 1.0e-6f;

bool ClustNode::ClustOnClosest() {
  if(children.size < 2)
    return false;		// cannot have any more clustering to do!

  float min_d = taMath::flt_max;
  int min_idx=-1;
  int i;
  for(i=0; i<children.size; i++) { // find node with closest neighbors
    ClustNode* nd = GetChild(i);
    if(nd->nn_dist < min_d) {
      min_d = nd->nn_dist;
      min_idx = i;
    }
  }
  if(min_idx < 0) return false;
  // make a new cluster around this node
  ClustNode* nd = GetChild(min_idx);
  ClustNode* new_clust = new ClustNode;
  AddChild(new_clust, min_d);
  // add the min node and its nearest neighbors to the new cluster
  new_clust->AddChild(nd);
  children.RemoveIdx(min_idx);
  for(i=0;i<nd->nns.size; i++) {
    ClustLink* nlk = (ClustLink*)nd->nns[i];
    if(fabsf(nlk->dist - min_d) > clust_dist_tol)
      continue;
    new_clust->AddChild(nlk->node);
    RemoveChild(nlk->node); // and the nns
  }

  // then finally check if any other nns at min_d have other nns not already obtained
  for(i=0;i<nd->nns.size; i++) {
    ClustLink* nlk = (ClustLink*)nd->nns[i];
    if(fabsf(nlk->dist - min_d) > clust_dist_tol)
      continue;
    ClustNode* nn = nlk->node;
    int j;
    for(j=0; j<nn->nns.size; j++) {
      ClustLink* nn_nlk = (ClustLink*)nn->nns[j];
      if(fabsf(nn_nlk->dist - min_d) > clust_dist_tol)
	continue;
      if(new_clust->FindChild(nn_nlk->node) < 0) { // not in new clust yet
	new_clust->AddChild(nn_nlk->node);
	RemoveChild(nn_nlk->node); // and remove from main list
      }
    }
  }

  if(children.size < 2)
    return false;		// cannot have any more clustering to do!
  return true;
}

void ClustNode::NNeighbors(taMath::DistMetric metric, bool norm, float tol) {
  int i;
  for(i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    nd->nns.Reset();
    nd->nn_dist = taMath::flt_max;
  }
  for(i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    float min_d = taMath::flt_max;
    int j;
    for(j=i+1; j<children.size; j++) {
      ClustNode* ond = GetChild(j);
      ond->tmp_dist = nd->Dist(*ond, metric, norm, tol);
      min_d = MIN(ond->tmp_dist, min_d);
    }
    for(j=i+1; j<children.size; j++) {
      ClustNode* ond = GetChild(j);
      if(fabsf(ond->tmp_dist - min_d) < clust_dist_tol) {
	nd->LinkNN(ond, min_d);	// link together with distance
	ond->LinkNN(nd, min_d);
      }
    }
  }
  // now make a 2nd pass and get smallest distance for each node and its neighbors
  for(i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    nd->nn_dist = taMath::flt_max;
    int j;
    for(j=0; j<nd->nns.size; j++) {
      ClustLink* nlk = (ClustLink*)nd->nns[j];
      nd->nn_dist = MIN(nd->nn_dist, nlk->dist);
    }
  }
}

float ClustNode::Dist(const ClustNode& oth, taMath::DistMetric metric,
		      bool norm, float tol) const
{
  float rval = 0.0f;
  if(pat) {
    if(oth.pat) {
      if(leaf_dists) {
	rval = leaf_dists->FastEl(leaf_idx, oth.leaf_idx);
      }
      else {
	if(pat->GetDataValType() == VT_FLOAT)
	  rval = taMath_float::vec_dist((float_Matrix*)pat, (float_Matrix*)oth.pat, metric, norm, tol);
	else
	  rval= (float)taMath_double::vec_dist((double_Matrix*)pat, (double_Matrix*)oth.pat, metric, norm, tol);
      }
    }
    else {
      rval = oth.Dist(*this, metric, norm, tol);
    }
  }
  else {
    int i;
    for(i=0; i<children.size; i++) {
      ClustNode* nd = GetChild(i);
      if(nd->pat) {
	if(oth.pat) {
	  if(leaf_dists) {
	    rval += leaf_dists->FastEl(nd->leaf_idx, oth.leaf_idx);
	  }
	  else {
	    if(nd->pat->GetDataValType() == VT_FLOAT)
	      rval += taMath_float::vec_dist((float_Matrix*)nd->pat, (float_Matrix*)oth.pat, metric, norm, tol);
	    else
	      rval += (float)taMath_double::vec_dist((double_Matrix*)nd->pat, (double_Matrix*)oth.pat, metric, norm, tol);
	  }
	}
	else {
	  rval += oth.Dist(*nd, metric, norm, tol);
	}
      }
      else {
	rval += nd->Dist(oth, metric, norm, tol);
      }
    }
    if(children.size > 1)
      rval /= (float)children.size;
  }
  return rval;
}

void ClustNode::SetYs(float y_init) {
  static float global_y;
  if(y_init != -1.0f)
    global_y = y_init;
  if(pat == NULL) {
    float y_avg = 0.0f;
    int i;
    for(i=0; i<children.size; i++) {
      ClustNode* nd = GetChild(i);
      nd->SetYs();
      y_avg += nd->y;
    }
    if(children.size > 1)
      y_avg /= (float)children.size; // average of all y's of kids
    y = y_avg;
  }
  else {
    y = global_y;
    global_y += 1.0f;
  }
}

float ClustNode::SetParDists(float par_d) {
  par_dist = par_d + nn_dist;
  float max_d = -taMath::flt_max;
  int i;
  for(i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    float dst = nd->SetParDists(par_dist);
    max_d = MAX(dst, max_d);
  }
  return max_d;
}
