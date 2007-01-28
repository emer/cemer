// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "ta_dataanal.h"
#include "css_machine.h"
#include "ta_project.h"		// for debugging


//////////////////////////
// 	ClustNode	//
//////////////////////////

void ClustLink::Initialize() {
  dist = 0.0f;
  node = NULL;
}

void ClustLink::Copy_(const ClustLink& cp) {
  dist = cp.dist;
  taBase::SetPointer((TAPtr*)&node, cp.node);
}

void ClustLink::CutLinks() {
  taBase::DelPointer((TAPtr*)&node);
  taBase::CutLinks();
}

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
  taBase::DelPointer((TAPtr*)&leaf_dists);
  taBase::DelPointer((TAPtr*)&pat);
  children.Reset();
  nns.Reset();
  inherited::CutLinks();
}

void ClustNode::SetPat(taMatrix* pt) {
  taBase::SetPointer((TAPtr*)&pat, pt);
}

void ClustNode::AddChild(ClustNode* nd, float dst) {
  ClustLink* lk = new ClustLink;
  taBase::SetPointer((TAPtr*)&(lk->node), nd);
  lk->dist = dst;
  children.Add(lk);
}

void ClustNode::LinkNN(ClustNode* nd, float dst) {
  ClustLink* lk = new ClustLink;
  taBase::SetPointer((TAPtr*)&(lk->node), nd);
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

  taBase::SetPointer((TAPtr*)&leaf_dists, new float_Matrix);
  leaf_dists->SetGeom(2, children.size, children.size);
  for(int i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    nd->leaf_idx = i;
    taBase::SetPointer((TAPtr*)&(nd->leaf_dists), leaf_dists);
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

  taBase::DelPointer((TAPtr*)&leaf_dists);
  return true;
}

void ClustNode::GraphData(DataTable* dt) {
  SetYs(0.5f);
  nn_dist = 0.0f;
  dt->Reset();
  dt->NewColFloat("X");
  dt->NewColFloat("Y");
  DataArray_impl* da = dt->NewColString("Label");
  da->SetUserData("DISP_STRING", true);
  da->SetUserData("AXIS", 1); // labels use same axis as y values
  da->SetUserData("STRING_COORDS", 1); // use y values
  GraphData_impl(dt);
  
  // todo: fix this later!
//   float_Matrix* xar = (float_Matrix*)dt->GetColMatrix(0);
//   dt->AddColDispOpt(String("MAX=") + String(taMath_float::vec_max(xar) * 1.15f), 0); // adds extra room for labels

//   float_Matrix* yar = (float_Matrix*)dt->GetColMatrix(1);
//   dt->AddColDispOpt(String("MAX=") + String(taMath_float::vec_max(yar) + .3f), 1); // adds extra room for labels
//   dt->AddColDispOpt("MIN=0.2", 1);
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

////////////////////////////////////////////////////////////////////////////////////
//			taDataAnal

bool taDataAnal::GetDest(DataTable*& dest, DataTable* src, const String& suffix) {
  if(dest) {
    dest->ResetData();		// always clear out for new data
    return false;
  }
  taProject* proj = GET_OWNER(src, taProject);
  DataTable_Group* dgp = (DataTable_Group*)proj->data.FindMakeGpName("AnalysisData");
  dest = dgp->NewEl(1, &TA_DataTable);
  String nm = src->name + "_" + suffix;
  dest->name = nm;
  taMisc::Warning("Note: taDataAnal created new data table named:", nm, "in .data.AnalysisData");
  return true;
}

DataArray_impl* taDataAnal::GetMatrixDataCol(DataTable* src_data, const String& data_col_nm) {
  int idx;
  DataArray_impl* da = src_data->FindColName(data_col_nm, idx, true); // err msg
  if(!da)
    return NULL;
  if(!da->is_matrix) {
    taMisc::Error("taDataAnal: column named:", data_col_nm,
		  "is not a matrix in data table:", src_data->name);
    return NULL;
  }
  if((da->valType() != VT_FLOAT) && (da->valType() != VT_DOUBLE)) {
    taMisc::Error("taDataAnal: column named:", data_col_nm,
		  "is not of type float or double in data table:", src_data->name);
    return NULL;
  }
  return da;
}

DataArray_impl* taDataAnal::GetStringDataCol(DataTable* src_data, const String& name_col_nm) {
  if(name_col_nm.empty()) return NULL;
  int idx;
  DataArray_impl* nmda = src_data->FindColName(name_col_nm, idx, true); // err msg
  if(!nmda)
    return NULL;
  if(nmda->is_matrix) {
    taMisc::Error("taDataAnal: column named:", name_col_nm,
		  "is a matrix where a scalar is required, in data table:", src_data->name);
    return NULL;
  }
  if(nmda->valType() != VT_STRING) {
    taMisc::Error("taDataAnal: column named:", name_col_nm,
		  "is not of type String in data table:", src_data->name);
    return NULL;
  }
  return nmda;
}

////////////////////////////////////////////////////////////////////////////////////
//	distance matricies

bool taDataAnal::DistMatrix(float_Matrix* dist_mat, DataTable* src_data,
			    const String& data_col_nm,
			    taMath::DistMetric metric, bool norm, float tol) {
  DataArray_impl* da = GetMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return false;
  bool rval = true;
  if(da->valType() == VT_FLOAT) {
    rval = taMath_float::mat_dist(dist_mat, (float_Matrix*)da->AR(), metric, norm, tol);
  }
  else if(da->valType() == VT_DOUBLE) {
    double_Matrix ddmat;
    rval = taMath_double::mat_dist(&ddmat, (double_Matrix*)da->AR(), metric, norm, tol);
    taMath::mat_cvt_double_to_float(dist_mat, &ddmat);
  }
  return rval;
}

bool taDataAnal::DistMatrixTable(DataTable* dist_mat, bool view, DataTable* src_data,
				 const String& data_col_nm, const String& name_col_nm,
				 taMath::DistMetric metric, bool norm, float tol) {
  if(!src_data) return false;
  float_Matrix dmat;
  bool rval = DistMatrix(&dmat, src_data, data_col_nm, metric, norm, tol);
  if(!rval) return false;
  GetDest(dist_mat, src_data, "DistMatrix");
  if(!name_col_nm.empty()) {
    DataArray_impl* nmda = GetStringDataCol(src_data, name_col_nm);
    if(nmda) {
      dist_mat->Reset();	// nuke everything
      dist_mat->NewColString("Name");
      int n = dmat.dim(0);
      for(int i=0;i<n;i++) {
	String nm = nmda->GetValAsString(i);
	dist_mat->NewColFloat(nm);
      }
      for(int i=0;i<n;i++) {
	String nm = nmda->GetValAsString(i);
	dist_mat->AddBlankRow();
	dist_mat->SetValAsString(nm, 0, -1);
	for(int j=0;j<n;j++) {
	  dist_mat->SetValAsFloat(dmat.FastEl(j,i), j+1, -1);
	}
      }
      if(view) dist_mat->NewGridView();
      return true;
    }
  }
  // no labels -- just make a col
  String cl_nm = src_data->name + "_DistMatrix";
  int idx;
  DataArray_impl* dmda = dist_mat->FindMakeColName(cl_nm, idx, VT_FLOAT, 2, dmat.dim(0),
						   dmat.dim(1));
  dist_mat->AddBlankRow();
  dmda->SetValAsMatrix(&dmat, -1);
  if(view) dist_mat->NewGridView();
  return true;
}

bool taDataAnal::CrossDistMatrix(float_Matrix* dist_mat,
				 DataTable* src_data_a, const String& data_col_nm_a,
				 DataTable* src_data_b, const String& data_col_nm_b,
				 taMath::DistMetric metric, bool norm, float tol) {
  DataArray_impl* da_a = GetMatrixDataCol(src_data_a, data_col_nm_a);
  if(!da_a)
    return false;
  DataArray_impl* da_b = GetMatrixDataCol(src_data_b, data_col_nm_b);
  if(!da_b)
    return false;
  if(da_a->valType() != da_b->valType()) {
    taMisc::Error("CrossDistMatrix: matrix data types do not match!");
    return false;
  }
  bool rval = true;
  if(da_a->valType() == VT_FLOAT) {
    rval = taMath_float::mat_cross_dist(dist_mat, (float_Matrix*)da_a->AR(), (float_Matrix*)da_b->AR(), metric, norm, tol);
  }
  else if(da_a->valType() == VT_DOUBLE) {
    double_Matrix ddmat;
    rval = taMath_double::mat_cross_dist(&ddmat, (double_Matrix*)da_a->AR(), (double_Matrix*)da_b->AR(), metric, norm, tol);
    taMath::mat_cvt_double_to_float(dist_mat, &ddmat);
  }
  return rval;
}

bool taDataAnal::CrossDistMatrixTable(DataTable* dist_mat, bool view,
				      DataTable* src_data_a, const String& data_col_nm_a,
				      const String& name_col_nm_a,
				      DataTable* src_data_b, const String& data_col_nm_b,
				      const String& name_col_nm_b,
				      taMath::DistMetric metric, bool norm, float tol) {
  if(!src_data_a || !src_data_b) return false;
  float_Matrix dmat;
  bool rval = CrossDistMatrix(&dmat, src_data_a, data_col_nm_a, src_data_b, data_col_nm_b, 
			      metric, norm, tol);
  if(!rval) return false;
  GetDest(dist_mat, src_data_a, src_data_b->name + "_DistMatrix");
  if(!name_col_nm_a.empty() && !name_col_nm_b.empty()) {
    DataArray_impl* nmda_a = GetStringDataCol(src_data_a, name_col_nm_a);
    DataArray_impl* nmda_b = GetStringDataCol(src_data_b, name_col_nm_b);
    if(nmda_a && nmda_b) {
      dist_mat->Reset();	// nuke everything
      dist_mat->NewColString("Name");
      int col_n = dmat.dim(0);
      int row_n = dmat.dim(1);
      for(int i=0;i<col_n;i++) {
	String nm = nmda_b->GetValAsString(i);
	dist_mat->NewColFloat(nm);
      }
      for(int i=0;i<row_n;i++) {
	String nm = nmda_a->GetValAsString(i);
	dist_mat->AddBlankRow();
	dist_mat->SetValAsString(nm, 0, -1);
	for(int j=0;j<col_n;j++) {
	  dist_mat->SetValAsFloat(dmat.FastEl(j,i), j+1, -1);
	}
      }
      if(view) dist_mat->NewGridView();
      return true;
    }
  }
  // no labels -- just make a col
  String cl_nm = src_data_a->name + "_" + src_data_b->name + "_DistMatrix";
  int idx;
  DataArray_impl* dmda = dist_mat->FindMakeColName(cl_nm, idx, VT_FLOAT, 2, dmat.dim(0),
						   dmat.dim(1));
  dist_mat->AddBlankRow();
  dmda->SetValAsMatrix(&dmat, -1);
  if(view) dist_mat->NewGridView();
  return true;
}

///////////////////////////////////////////////////////////////////
// correlation matricies

bool taDataAnal::CorrelMatrix(float_Matrix* correl_mat, DataTable* src_data,
			      const String& data_col_nm) {
  DataArray_impl* da = GetMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return false;
  bool rval = true;
  if(da->valType() == VT_FLOAT) {
    rval = taMath_float::mat_correl(correl_mat, (float_Matrix*)da->AR());
  }
  else if(da->valType() == VT_DOUBLE) {
    double_Matrix ddmat;
    rval = taMath_double::mat_correl(&ddmat, (double_Matrix*)da->AR());
    taMath::mat_cvt_double_to_float(correl_mat, &ddmat);
  }
  return rval;
}

bool taDataAnal::CorrelMatrixTable(DataTable* correl_mat, bool view, DataTable* src_data,
				   const String& data_col_nm) {
  if(!src_data) return false;
  float_Matrix dmat;
  bool rval = CorrelMatrix(&dmat, src_data, data_col_nm);
  if(!rval) return false;
  GetDest(correl_mat, src_data, "CorrelMatrix");
  // no labels -- just make a col
  String cl_nm = src_data->name + "_CorrelMatrix";
  int idx;
  DataArray_impl* dmda = correl_mat->FindMakeColName(cl_nm, idx, VT_FLOAT, 2, dmat.dim(0),
						     dmat.dim(1));
  correl_mat->AddBlankRow();
  dmda->SetValAsMatrix(&dmat, -1);
  if(view) correl_mat->NewGridView();
  return true;
}

///////////////////////////////////////////////////////////////////
// standard multidimensional data analysis methods

bool taDataAnal::Cluster(DataTable* clust_data, bool view, DataTable* src_data,
			 const String& data_col_nm, const String& name_col_nm,
			 taMath::DistMetric metric, bool norm, float tol) {
  if(!src_data) return false;
  DataArray_impl* da = GetMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return false;
  DataArray_impl* nmda = GetStringDataCol(src_data, name_col_nm);
  if(!nmda)
    return false;
  
  GetDest(clust_data, src_data, "col_" + data_col_nm + "_Cluster");

  ClustNode root;
  taBase::Ref(root);		// just in case
  for(int i=0;i<src_data->rows;i++) {
    ClustNode* nd = new ClustNode;
    nd->name = nmda->GetValAsString(i);
    taMatrix* mat = da->GetValAsMatrix(i);
    nd->SetPat(mat);
    root.AddChild(nd);
  }

  root.Cluster(metric, norm, tol);
  root.GraphData(clust_data);
  if(view) clust_data->NewGraphView();
  return true;
}

bool taDataAnal::PCAEigens(float_Matrix* eigen_vals, float_Matrix* eigen_vecs,
			   DataTable* src_data, const String& data_col_nm) {
  if(!src_data) return false;
  float_Matrix correl_mat;
  if(!CorrelMatrix(&correl_mat, src_data, data_col_nm))
    return false;
  return taMath_float::mat_eigen_owrite(&correl_mat, eigen_vals, eigen_vecs);
}

bool taDataAnal::PCAEigenTable(DataTable* pca_data, bool view, DataTable* src_data,
			       const String& data_col_nm) {
  if(!src_data) return false;
  float_Matrix eigen_vals;
  float_Matrix eigen_vecs;
  if(!PCAEigens(&eigen_vals, &eigen_vecs, src_data, data_col_nm)) return false;

  GetDest(pca_data, src_data, "col_" + data_col_nm + "_PCAEigens");

  DataArray_impl* da = GetMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return false;

  String cl_nm = data_col_nm + "_PCAEigens";
  int idx;
  DataArray_impl* dmda = pca_data->FindMakeColName(cl_nm, idx, VT_FLOAT,
						   da->cell_dims(), da->GetCellGeom(0),
						   da->GetCellGeom(1), da->GetCellGeom(2),
						   da->GetCellGeom(3));
  for(int i=0;i<eigen_vecs.dim(0);i++) {
    pca_data->AddBlankRow();
    for(int j=0;j<eigen_vecs.dim(1);j++) {
      dmda->SetValAsFloatM(eigen_vecs.FastEl(i,j), -1, j);
    }
  }

  if(view) pca_data->NewGridView();
  return true;
}

bool taDataAnal::PCA2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			   const String& data_col_nm, const String& name_col_nm, 
			   int x_axis_c, int y_axis_c) {
  if(!src_data) return false;
  DataArray_impl* da = GetMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return false;
  int dim = da->cell_size();

  if((x_axis_c < 0) || (x_axis_c >= dim)) {
    taMisc::Error("*** PCA: x_axis component must be between 0 and",String(dim-1));
    return false;
  }
  if((y_axis_c < 0) || (y_axis_c >= dim)) {
    taMisc::Error("*** PCA: y_axis component must be between 0 and",String(dim-1));
    return false;
  }

  float_Matrix eigen_vals;
  float_Matrix eigen_vecs;
  if(!PCAEigens(&eigen_vals, &eigen_vecs, src_data, data_col_nm)) return false;

  float_Matrix xevec;		// x eigen vector
  taMath_float::mat_col(&xevec, &eigen_vecs, x_axis_c); // eigen vector = column 
  cerr << "Component no: " << x_axis_c << " has eigenvalue: "
       << eigen_vals.FastEl(x_axis_c) << endl;

  float_Matrix yevec;		// x eigen vector
  taMath_float::mat_col(&yevec, &eigen_vecs, y_axis_c); // eigen vector = column 
  cerr << "Component no: " << y_axis_c << " has eigenvalue: "
       << eigen_vals.FastEl(y_axis_c) << endl;

  float_Matrix xprjn;
  taMath_float::mat_prjn(&xprjn, (float_Matrix*)da->AR(), &xevec);
  float_Matrix yprjn;
  taMath_float::mat_prjn(&yprjn, (float_Matrix*)da->AR(), &yevec);

  DataArray_impl* nmda = GetStringDataCol(src_data, name_col_nm);

  GetDest(prjn_data, src_data, "col_" + data_col_nm + "_PCA2dPrjn");

  int idx;
  DataArray_impl* nm = NULL;
  if(nmda)
    nm = prjn_data->FindMakeColName(name_col_nm, idx, VT_STRING);
  DataArray_impl* xda = prjn_data->FindMakeColName("x_prjn", idx, VT_FLOAT);
  DataArray_impl* yda = prjn_data->FindMakeColName("y_prjn", idx, VT_FLOAT);

  for(int i=0;i<src_data->rows;i++) {
    prjn_data->AddBlankRow();
    if(nm)
      nm->SetValAsString(nmda->GetValAsString(i),-1);
    xda->SetValAsFloat(xprjn.FastEl(i), -1);
    yda->SetValAsFloat(yprjn.FastEl(i), -1);
  }

  if(view) prjn_data->NewGraphView();
  return true;
}  

bool taDataAnal::MDS2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			   const String& data_col_nm, const String& name_col_nm, 
			   int x_axis_c, int y_axis_c,
			   taMath::DistMetric metric, bool norm, float tol)
{
  if(!src_data) return false;
  int dim = src_data->rows;
  if((x_axis_c < 0) || (x_axis_c >= dim)) {
    taMisc::Error("MDS2dPrjn: x_axis component must be between 0 and",String(dim-1));
    return false;
  }
  if((y_axis_c < 0) || (y_axis_c >= dim)) {
    taMisc::Error("MDS2dPrjn: y_axis component must be between 0 and",String(dim-1));
    return false;
  }

  float_Matrix dist_mat;
  DistMatrix(&dist_mat, src_data, data_col_nm, metric, norm, tol);

  float_Matrix xy_coords;
  taMath_float::mat_mds_owrite(&dist_mat, &xy_coords, x_axis_c, y_axis_c);

  DataArray_impl* nmda = GetStringDataCol(src_data, name_col_nm);

  GetDest(prjn_data, src_data, "col_" + data_col_nm + "_MDS2dPrjn");

  int idx;
  DataArray_impl* nm = NULL;
  if(nmda)
    nm = prjn_data->FindMakeColName(name_col_nm, idx, VT_STRING);
  DataArray_impl* xda = prjn_data->FindMakeColName("x_prjn", idx, VT_FLOAT);
  DataArray_impl* yda = prjn_data->FindMakeColName("y_prjn", idx, VT_FLOAT);

  for(int i=0;i<src_data->rows;i++) {
    prjn_data->AddBlankRow();
    if(nm)
      nm->SetValAsString(nmda->GetValAsString(i),-1);
    xda->SetValAsFloat(xy_coords.FastEl(0, i), -1);
    yda->SetValAsFloat(xy_coords.FastEl(1, i), -1);
  }

  if(view) prjn_data->NewGraphView();
  return true;
}

bool taDataAnal::RowPat2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			     const String& data_col_nm, const String& name_col_nm,
			     int x_row, int y_row,
			     taMath::DistMetric metric, bool norm, float tol)
{
  if(!src_data) return false;
  DataArray_impl* da = GetMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return false;
  int n_rows = src_data->rows;
  if((x_row < 0) || (x_row >= n_rows)) {
    taMisc::Error("RowPat2dPrjn: x_axis row must be between 0 and",String(n_rows-1));
    return false;
  }
  if((y_row < 0) || (y_row >= n_rows)) {
    taMisc::Error("RowPat2dPrjn: y_axis row must be between 0 and",String(n_rows-1));
    return false;
  }

  float_Matrix* xrow = (float_Matrix*)da->GetValAsMatrix(x_row);
  taBase::Ref(xrow);
  float_Matrix* yrow = (float_Matrix*)da->GetValAsMatrix(y_row);
  taBase::Ref(yrow);

  float_Matrix xprjn;
  taMath_float::mat_prjn(&xprjn, (float_Matrix*)da->AR(), xrow, metric, norm, tol);
  float_Matrix yprjn;
  taMath_float::mat_prjn(&yprjn, (float_Matrix*)da->AR(), yrow, metric, norm, tol);

  DataArray_impl* nmda = GetStringDataCol(src_data, name_col_nm);

  GetDest(prjn_data, src_data, "col_" + data_col_nm + "_RowPat2dPrjn");

  int idx;
  DataArray_impl* nm = NULL;
  if(nmda)
    nm = prjn_data->FindMakeColName(name_col_nm, idx, VT_STRING);
  DataArray_impl* xda = prjn_data->FindMakeColName("x_prjn", idx, VT_FLOAT);
  DataArray_impl* yda = prjn_data->FindMakeColName("y_prjn", idx, VT_FLOAT);

  for(int i=0;i<src_data->rows;i++) {
    prjn_data->AddBlankRow();
    if(nm)
      nm->SetValAsString(nmda->GetValAsString(i),-1);
    xda->SetValAsFloat(xprjn.FastEl(i), -1);
    yda->SetValAsFloat(yprjn.FastEl(i), -1);
  }

  if(view) prjn_data->NewGraphView();
  return true;
}

bool taDataAnal::TimeAvg(DataTable* time_avg_data, bool view, DataTable* src_data,
			 float avg_dt, bool float_only)
{
  if(!src_data) return false;

  float_Matrix float_tmp;
  GetDest(time_avg_data, src_data, "TimeAvg");

  time_avg_data->Reset();
  *time_avg_data = *src_data;	// do complete copy, then operate in place
  for(int i=0;i<time_avg_data->data.size;i++) {
    DataArray_impl* da = time_avg_data->data[i];
    if(!da->isNumeric()) continue;
    if(da->valType() == VT_BYTE) continue;
    if(float_only && (da->valType() == VT_INT)) continue;
    if(da->valType() == VT_FLOAT) {
      taMath_float::mat_time_avg((float_Matrix*)da->AR(), avg_dt);
    }
    else if(da->valType() == VT_DOUBLE) {
      taMath_double::mat_time_avg((double_Matrix*)da->AR(), avg_dt);
    }
    else if(da->valType() == VT_INT) { // expensive double-convert..
      int_Matrix* mat = (int_Matrix*)da->AR();
      float_tmp.SetGeomN(mat->geom);
      for(int i=0;i<mat->size;i++) float_tmp.FastEl_Flat(i) = (float)mat->FastEl_Flat(i);
      taMath_float::mat_time_avg(&float_tmp, avg_dt);
      for(int i=0;i<mat->size;i++) mat->FastEl_Flat(i) = (int)float_tmp.FastEl_Flat(i);
    }
  }

  if(view) time_avg_data->NewGraphView();
  return true;
}

/*
void Environment::PatFreqGrid(GridLog* disp_log, float act_thresh, bool prop) {
  if(events.leaves == 0)
    return;

  if(event_specs.size == 0) return;

  if(disp_log == NULL) {
    disp_log = (GridLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
    if(disp_log == NULL) return;
  }
  else {
    LogView* lv = (LogView*)disp_log->views.SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return;
  }

  disp_log->name = "PatFreqGrid: " + name;
  DataTable* dt = &(disp_log->data);
  dt->Reset();

  EventSpec* es = (EventSpec*)event_specs.DefaultEl();
  if(es == NULL) return;
  float_RArray freqs;
  int pat_no;
  for(pat_no = 0; pat_no < es->patterns.size; pat_no++) {
    PatternSpec* ps = (PatternSpec*)es->patterns[pat_no];
    PatFreqArray(freqs, pat_no, act_thresh, prop);

    DataTable* dtgp = dt->NewGroupFloat(String("pattern_") + String(pat_no), freqs.size);

    bool first = true;
    for(int i=0;i<freqs.size;i++) {
      String nm;
      if(!ps->value_names[i].empty())
	nm = ps->value_names[i];
      else
	nm = String("v") + String(i);
      if(first) {
	nm = String("<") + String(ps->geom.x) + ">" + nm;
	first = false;
	String xgeom = "GEOM_X=" + String(ps->geom.x);
	dtgp->AddColDispOpt(xgeom, i);
	String ygeom = "GEOM_Y=" + String(ps->geom.y);
	dtgp->AddColDispOpt(ygeom, i);
	dtgp->AddColDispOpt("USE_EL_NAMES", i); // each column has separate element names in gp
      }
      dtgp->SetColName(nm, i);
    }

    for(int i=0;i<freqs.size;i++) {
      dtgp->AddFloatVal(freqs[i], i); // col cnt, subgp 0
    }
  }

  disp_log->ViewAllData();
  GridLogView* glv = (GridLogView*)disp_log->views.SafeEl(0);
  if(glv == NULL) return;
  glv->auto_scale = true;
  glv->AllBlockTextOn();
  glv->SetBlockSizes(20,1);
  glv->UpdateGridLayout();
}

void Environment::PatAggGrid(GridLog* disp_log, Aggregate& agg) {
  if(events.leaves == 0)
    return;

  if(event_specs.size == 0) return;

  if(disp_log == NULL) {
    disp_log = (GridLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
    if(disp_log == NULL) return;
  }
  else {
    LogView* lv = (LogView*)disp_log->views.SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return;
  }

  disp_log->name = "PatAggGrid: " + name;
  DataTable* dt = &(disp_log->data);
  dt->Reset();

  EventSpec* es = (EventSpec*)event_specs.DefaultEl();
  if(es == NULL) return;
  float_RArray freqs;
  int pat_no;
  for(pat_no = 0; pat_no < es->patterns.size; pat_no++) {
    PatternSpec* ps = (PatternSpec*)es->patterns[pat_no];
    PatAggArray(freqs, pat_no, agg);

    DataTable* dtgp = dt->NewGroupFloat(String("pattern_") + String(pat_no), freqs.size);

    bool first = true;
    for(int i=0;i<freqs.size;i++) {
      String nm;
      if(!ps->value_names[i].empty())
	nm = ps->value_names[i];
      else
	nm = String("v") + String(i);
      if(first) {
	nm = String("<") + String(ps->geom.x) + ">" + nm;
	first = false;
	String xgeom = "GEOM_X=" + String(ps->geom.x);
	dtgp->AddColDispOpt(xgeom, i);
	String ygeom = "GEOM_Y=" + String(ps->geom.y);
	dtgp->AddColDispOpt(ygeom, i);
	dtgp->AddColDispOpt("USE_EL_NAMES", i); // each column has separate element names in gp
      }
      dtgp->SetColName(nm, i);
    }

    for(int i=0;i<freqs.size;i++) {
      dtgp->AddFloatVal(freqs[i], i); // col cnt, subgp 0
    }
  }

  disp_log->ViewAllData();
  GridLogView* glv = (GridLogView*)disp_log->views.SafeEl(0);
  if(glv == NULL) return;
  glv->auto_scale = true;
  glv->AllBlockTextOn();
  glv->SetBlockSizes(20,1);
  glv->UpdateGridLayout();
}

*/


/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

void DataAnalCall::Initialize() {
  min_type = &TA_taDataAnal;
  object_type = &TA_taDataAnal;
}
