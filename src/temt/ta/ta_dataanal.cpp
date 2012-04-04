// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "ta_dataanal.h"
#include "ta_dataproc.h"
#include "css_machine.h"
#include "ta_project.h"		// for debugging
#include "ta_datatable_qtso.h"

//////////////////////////
// 	ClustNode	//
//////////////////////////

void ClustLink::Initialize() {
  dist = 0.0f;
  node = NULL;
}

void ClustLink::Copy_(const ClustLink& cp) {
  dist = cp.dist;
  taBase::SetPointer((taBase**)&node, cp.node);
}

void ClustLink::CutLinks() {
  taBase::DelPointer((taBase**)&node);
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

////////////////////////////////////////////////////////////////////////////////////
//			taDataAnal

bool taDataAnal::GetDest(DataTable*& dest, const DataTable* src, const String& suffix) {
  if(dest) {
    dest->ResetData();		// always clear out for new data
    return false;
  }
  taProject* proj = GET_OWNER(src, taProject);
  dest = proj->GetNewAnalysisDataTable(src->name + "_" + suffix, true);
  return true;
}

DataCol* taDataAnal::GetMatrixDataCol(DataTable* src_data, const String& data_col_nm) {
  DataCol* da = src_data->FindColName(data_col_nm, true); // err msg
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

DataCol* taDataAnal::GetStringDataCol(DataTable* src_data, const String& name_col_nm) {
  if(name_col_nm.empty()) return NULL;
  DataCol* nmda = src_data->FindColName(name_col_nm, true); // err msg
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

DataCol* taDataAnal::GetNumDataCol(DataTable* src_data, const String& name_col_nm) {
  if(name_col_nm.empty()) return NULL;
  DataCol* nmda = src_data->FindColName(name_col_nm, true); // err msg
  if(!nmda)
    return NULL;
  if(nmda->is_matrix) {
    taMisc::Error("taDataAnal: column named:", name_col_nm,
		  "is a matrix where a scalar is required, in data table:", src_data->name);
    return NULL;
  }
  if(!nmda->isNumeric()) {
    taMisc::Error("taDataAnal: column named:", name_col_nm,
		  "is not a numeric type -- must be, in data table:", src_data->name);
    return NULL;
  }
  return nmda;
}

////////////////////////////////////////////////////////////////////////////////////
//	stats 

String taDataAnal::RegressLinear(DataTable* src_data, const String& x_data_col_nm,
				const String& y_data_col_nm, bool render_line) {
  
  String rval = "err";
  DataCol* xda = GetNumDataCol(src_data, x_data_col_nm);
  if(!xda) return rval;
  DataCol* yda = GetNumDataCol(src_data, y_data_col_nm);
  if(!yda) return rval;

  double r, b, m;

  if(xda->valType() == VT_DOUBLE && yda->valType() == VT_DOUBLE) {
    double cov00, cov01, cov11, sum_sq;
    taMath_double::vec_regress_lin((double_Matrix*)xda->AR(), (double_Matrix*)yda->AR(),
					  b, m, cov00, cov01, cov11, sum_sq);
    r = taMath_double::vec_correl((double_Matrix*)xda->AR(), (double_Matrix*)yda->AR());
  }
  else if(xda->valType() == VT_FLOAT && yda->valType() == VT_FLOAT) {
    float fb, fm, fcov00, fcov01, fcov11, fsum_sq;
    taMath_float::vec_regress_lin((float_Matrix*)xda->AR(), (float_Matrix*)yda->AR(),
				  fb, fm, fcov00, fcov01, fcov11, fsum_sq);
    b=fb; m=fm;
    r = (double)taMath_float::vec_correl((float_Matrix*)xda->AR(), (float_Matrix*)yda->AR());
  }
  else {
    double_Matrix xm(false);
    double_Matrix ym(false);
    double_Matrix* xmp = &xm;
    double_Matrix* ymp = &ym;
    if(xda->valType() == VT_DOUBLE) {
      xmp = (double_Matrix*)xda->AR();
    }
    else if(xda->valType() == VT_FLOAT) {
      taMath_double::mat_cvt_float_to_double(&xm, (float_Matrix*)xda->AR());
    }
    else if(xda->valType() == VT_INT) {
      taMath_double::vec_fm_ints(&xm, (int_Matrix*)xda->AR());
    }

    if(yda->valType() == VT_DOUBLE) {
      ymp = (double_Matrix*)yda->AR();
    }
    else if(yda->valType() == VT_FLOAT) {
      taMath_double::mat_cvt_float_to_double(&ym, (float_Matrix*)yda->AR());
    }
    else if(yda->valType() == VT_INT) {
      taMath_double::vec_fm_ints(&ym, (int_Matrix*)yda->AR());
    }
    double cov00, cov01, cov11, sum_sq;
    taMath_double::vec_regress_lin(xmp, ymp, b, m, cov00, cov01, cov11, sum_sq);
    r = taMath_double::vec_correl(xmp, ymp);
  }

  rval = yda->name + " = " + String(m) + " * " + xda->name + " + "
    + String(b) + "; r = " + String(r) + "; r^2 = "
    + String(r*r);

  taMisc::Info(rval);

  if(render_line) {
    src_data->StructUpdate(true);
    int idx;
    DataCol* rl = src_data->FindMakeColName("regress_line", idx, VT_FLOAT);
    rl->desc = rval;
    for(int i=0; i<src_data->rows;i++) {
      float xv = xda->GetValAsFloat(i);
      float yv = (float)b + (float)m * xv;
      rl->SetValAsFloat(yv, i);
    }
    src_data->StructUpdate(false);
  }
  return rval;
}

bool taDataAnal::MultiClassClassificationViaLinearRegression(DataTable* src_data,
							     DataTable* dest_data,
							     const String& data_col_nm,
							     const String& name_col_nm,
							     const String& class_col_nm,
							     const String& mode_col_nm) {
  
  String fun_name = "MultiClassClassificationViaLinearRegression";

  if (!src_data) {
    taMisc::Error(fun_name + " - src_data cannot be NULL");return false;}

  int src_rows = src_data->rows;

  if (src_rows < 2) {
    taMisc::Error(fun_name + " - src_data must have at least 2 rows");return false;}

  // dest_data - one row only
  GetDest(dest_data, src_data, fun_name);

  // work - workspace with one row only. users can request to save this data in dest_data
  DataTable* work = new DataTable; taBase::Ref(work);

  // *_col - check for existence
  DataCol* data_col = src_data->FindColName(data_col_nm);
  DataCol* name_col = src_data->FindColName(name_col_nm);
  DataCol* class_col = src_data->FindColName(class_col_nm);
  DataCol* mode_col = src_data->FindColName(mode_col_nm);

  if (!data_col || !name_col || !class_col || !mode_col) { 
    taMisc::Error(fun_name + " - data, name class and mode_col_nm's must point to existing columns in the table."); return false;}

  if (data_col->valType() != VT_FLOAT &&
      data_col->valType() != VT_DOUBLE) {
    taMisc::Error(fun_name + " - data_col must contain either float or double data."); return false;}        

  if (name_col->valType() != VT_STRING ||
      class_col->valType() != VT_STRING ||
      mode_col->valType() != VT_STRING) {
    taMisc::Error(fun_name + " - data, name class and mode_col_nm's must all be of type string."); return false;}    

  // sorted_src_data - sort src_data by mode (train/test), class and name
  DataTable* sorted_src_data = new DataTable; taBase::Ref(sorted_src_data);
  DataSortSpec* sort_spec = new DataSortSpec; taBase::Ref(sort_spec);
  sort_spec->AddColumn(mode_col_nm, src_data);
  sort_spec->AddColumn(class_col_nm, src_data);
  sort_spec->AddColumn(name_col_nm, src_data);
  taDataProc::Sort(sorted_src_data, src_data, sort_spec);

  // *_col - point at sorted_src_data instead of src_data and rename
  // X - vectors of data; N - names; C - target class; M - training mode
  data_col =  sorted_src_data->FindColName(data_col_nm);   data_col->name = "X";
  name_col =  sorted_src_data->FindColName(name_col_nm);   name_col->name = "N"; 
  class_col = sorted_src_data->FindColName(class_col_nm);  class_col->name = "C";
  mode_col =  sorted_src_data->FindColName(mode_col_nm);   mode_col->name = "M";

  // Check the matrix geometry of data. If the dims are 2 then
  // make the 2nd dim 1 and flatten all along the first dimension
  // TODO: Could support arbitrary dimensions
  int data_dims = data_col->GetValAsMatrix(0)->dims();
  int data_d0, data_d1;
  if (data_dims == 2) {
    data_d0 = data_col->GetValAsMatrix(0)->dim(0);
    data_d1 = data_col->GetValAsMatrix(0)->dim(1);
    sorted_src_data->ChangeColTypeGeom("X", data_col->valType(), 2, data_d0 * data_d1, 1);
  }

  // classes - a vector of unique class names, giving them numerical order
  taDataProc::AllDataToOne2DCell(work, sorted_src_data, class_col->valType(), "C");
  work->FindColName("One2dCell")->name = "C";
  String_Matrix* classes_full = (String_Matrix*)work->GetValAsMatrix("C",0);
  String_Matrix* classes = new String_Matrix; taBase::Ref(classes);
  taMath_double::vec_uniq(classes_full, classes, true);

  // split data by conjunctions of mode/class (the reason we sorted)
  // and send each col of each split to a matrix col in the workspace
  DataSelectSpec* select_spec = new DataSelectSpec; taBase::Ref(select_spec);
  DataSelectEl* M = (DataSelectEl*)select_spec->AddColumn("M", sorted_src_data);
  String mode, category, cmp_category;
  int TRAIN_C_d1, TEST_C_d1, TRAIN_X_d0;
  double chisq;

  // One vs. All classification - For each class all other classes are the negative class

  for (int i=0; i < classes->size; i++) {

    DataTable* select_tmp = new DataTable; taBase::Ref(select_tmp);
    DataTable* one_cell_tmp = new DataTable; taBase::Ref(one_cell_tmp);

    category = classes->FastEl(i);

    M->cmp = "TRAIN";
    taDataProc::SelectRows(select_tmp, sorted_src_data, select_spec);
    taDataProc::AllDataToOne2DCell(work, select_tmp, data_col->valType(), "X", "TRAIN_X_" + category);
    taDataProc::AllDataToOne2DCell(work, select_tmp, class_col->valType(), "C", "TRAIN_C_" + category);
    taDataProc::AllDataToOne2DCell(work, select_tmp, name_col->valType(), "N", "TRAIN_N_" + category);

    M->cmp = "TEST";
    taDataProc::SelectRows(select_tmp, sorted_src_data, select_spec);
    taDataProc::AllDataToOne2DCell(work, select_tmp, data_col->valType(), "X", "TEST_X_" + category);
    taDataProc::AllDataToOne2DCell(work, select_tmp, class_col->valType(), "C", "TEST_C_" + category);
    taDataProc::AllDataToOne2DCell(work, select_tmp, name_col->valType(), "N", "TEST_N_" + category);

    TRAIN_C_d1 = work->FindColName("TRAIN_C_" + category)->GetCellGeom(1);
    TEST_C_d1 = work->FindColName("TEST_C_" + category)->GetCellGeom(1);
    TRAIN_X_d0 = work->FindColName("TRAIN_X_" + category)->GetCellGeom(0);

    // T - Target value (integer 0/1)
    work->FindMakeColMatrix("TRAIN_T_" + category, VT_DOUBLE, 1, TRAIN_C_d1);
    work->FindMakeColMatrix("TEST_T_" + category, VT_DOUBLE, 1, TEST_C_d1);

    // CO - Regression coefficients
    work->FindMakeColMatrix("TRAIN_CO_" + category, VT_DOUBLE, 1, TRAIN_X_d0); 
    work->FindMakeColMatrix("TRAIN_COV_" + category, VT_DOUBLE, 2, TRAIN_X_d0, TRAIN_X_d0); // COV - Covariance matrix
    work->FindMakeColMatrix("TRAIN_YPRIME_" + category, VT_DOUBLE, 1, TRAIN_C_d1); // YPRIME - test set values
    work->FindMakeColMatrix("TEST_YPRIME_" + category, VT_DOUBLE, 1, TEST_C_d1); // YPRIME - test set values
    work->FindMakeCol("TRAIN_CHISQ_" + category, VT_INT); // CHISQ

    // C - Class names - sets all other classes to 0, this class to 1
    for (int j=0; j < TRAIN_C_d1; j++) {
      cmp_category = work->GetMatrixVal("TRAIN_C_" + category, 0, j);
      if (cmp_category == category)
	work->SetMatrixVal(1, "TRAIN_T_" + category, 0, j);
      else
	work->SetMatrixVal(0, "TRAIN_T_" + category, 0, j);
    }

    for (int j=0; j < TEST_C_d1; j++) {
      cmp_category = work->GetMatrixVal("TEST_C_" + category, 0, j);
      if (cmp_category == category)
	work->SetMatrixVal(1, "TEST_T_" + category, 0, j);
      else
	work->SetMatrixVal(0, "TEST_T_" + category, 0, j);
    }

    double_Matrix* TRAIN_X = (double_Matrix*)work->FindColName("TRAIN_X_" + category)->GetValAsMatrix(0);
    double_Matrix* TEST_X = (double_Matrix*)work->FindColName("TEST_X_" + category)->GetValAsMatrix(0);

    double_Matrix* TRAIN_X_dbl = new double_Matrix;  taBase::Ref(TRAIN_X_dbl);
    double_Matrix* TEST_X_dbl = new double_Matrix; taBase::Ref(TEST_X_dbl);

    // Convert float data to double

    ValType X_type = TRAIN_X->GetDataValType();
    if (X_type == VT_FLOAT) {
      taMath::mat_cvt_float_to_double(TRAIN_X_dbl, (float_Matrix*)TRAIN_X);
      taMath::mat_cvt_float_to_double(TEST_X_dbl, (float_Matrix*)TEST_X);
    }
    else if (X_type == VT_DOUBLE) {
      TRAIN_X_dbl = TRAIN_X;
      TEST_X_dbl = TEST_X;
    }
    else {
      taMisc::Error("X must be either a float_Matrix or double_Matrix");
      return false;
    }

    double_Matrix* T = (double_Matrix*)work->FindColName("TRAIN_T_" + category)->GetValAsMatrix(0);
    double_Matrix* CO = (double_Matrix*)work->FindColName("TRAIN_CO_" + category)->GetValAsMatrix(0);
    double_Matrix* COV = (double_Matrix*)work->FindColName("TRAIN_COV_" + category)->GetValAsMatrix(0);
    double_Matrix* TRAIN_YPRIME = (double_Matrix*)work->FindColName("TRAIN_YPRIME_" + category)->GetValAsMatrix(0);
    double_Matrix* TEST_YPRIME = (double_Matrix*)work->FindColName("TEST_YPRIME_" + category)->GetValAsMatrix(0);

    taMath_double::vec_regress_multi_lin(TRAIN_X_dbl, T, CO, COV, chisq);
    taMath_double::mat_vec_product(TRAIN_X_dbl, CO, TRAIN_YPRIME);
    taMath_double::mat_vec_product(TEST_X_dbl, CO, TEST_YPRIME);

    work->SetVal(chisq, "TRAIN_CHISQ_" + category, 0);

    taBase::unRefDone(TRAIN_X_dbl);
    taBase::unRefDone(TEST_X_dbl);
    taBase::unRefDone(select_tmp);    
    taBase::unRefDone(one_cell_tmp);
  }

  // Create class-level confusion matrix
  DataTable* confusion = new DataTable; taBase::Ref(confusion);
  String col_nm = "TEST_YPRIME_" + classes->FastEl(0);
  String cm = "ConfusionMatrix";
  DataCol* TEST_YPRIME_0 = work->FindColName(col_nm);
  int d0 = TEST_YPRIME_0->GetCellGeom(0);
  ValType val_type = TEST_YPRIME_0->valType();
  taDataProc::AllDataToOne2DCell(confusion, work, val_type, "TEST_YPRIME_", cm);
  confusion->ChangeColTypeGeom(cm, val_type, 2, d0, classes->size);

  // Create target confusion matrix
  DataTable* confusion_target = new DataTable; taBase::Ref(confusion_target);
  col_nm = "TEST_T_" + classes->FastEl(0);
  cm = "ConfusionMatrixTarget";
  DataCol* TEST_T_0 = work->FindColName(col_nm);
  d0 = TEST_T_0->GetCellGeom(0);
  val_type = TEST_T_0->valType();
  taDataProc::AllDataToOne2DCell(confusion_target, work, val_type, "TEST_T_", cm);
  confusion_target->ChangeColTypeGeom(cm, val_type, 2, d0, classes->size);

  // class lengths - how many stimuli are in each test class?
  DataTable* class_lengths = new DataTable; taBase::Ref(class_lengths);
  Relation* rel = new Relation; taBase::Ref(rel);
  double_Matrix* class_mat;
  double length;
  rel->rel = (Relation::Relations)0;
  rel->val = 1;
  class_lengths->NewColMatrix(VT_INT, "CL", 1, classes->size);
  class_lengths->AddRows();
  int_Matrix* class_lengths_mat = (int_Matrix*)class_lengths->FindColName("CL")->GetValAsMatrix(0);

  for (int i=0; i < classes->size; i++) {
    class_mat = (double_Matrix*)work->FindColName("TEST_T_" + (String)classes->FastEl(i))->GetValAsMatrix(0);
    length = taMath_double::vec_count(class_mat, *rel);
    class_lengths->SetMatrixFlatVal(length, "CL", 0, i);
  }

  // Compute Confusion Matrix, ROC Accuracy, Precision, Recall, F-Measure

  String class_nm;

  double_Matrix* ConfusionMatrix = (double_Matrix*)confusion->FindColName("ConfusionMatrix")->GetValAsMatrix(0);

  double tp = 0.0f, fp = 0.0f, tn = 0.0f, fn = 0.0f;     // True/False Positive/Negative counts
  double ctp = 0.0f, cfp = 0.0f, ctn = 0.0f, cfn = 0.0f;
  double fpr = 0.0f, tpr = 0.0f;                         // True/False Positive Rate
  double ctpr = 0.0f, cfpr = 0.0f;
  double precision = 0.0f, recall = 0.0f;                // Precision and Recall
  double cprecision = 0.0f, crecall = 0.0f;
  double fm = 0.0f;                                      // F-measure
  double cfm = 0.0f;
  double acc = 0.0f;                                     // Accuracy 
  double cacc = 0.0f;
  double roc_acc = 0.0f;                                 // ROC Accuracy
  double croc_acc = 0.0f;

  // c_mat - Final class-level confusion matrix
  // ConfusionMatrix (above and below) is the raw item-level classifier outputs
  double_Matrix* c_mat = new double_Matrix; taBase::Ref(c_mat);
  c_mat->SetGeom(2, classes->size, classes->size);

  // slice - for tmp usage below
  double_Matrix* slice = new double_Matrix; taBase::Ref(slice);

  int max_index = 0; // Index of the classifier with the largest response
  double max = 0.0f; // Actual max value. Not Used.
  int ctr = 0; // Iterator used to keep track of the stimulus
  int class_length; // Number of stimuli in this class
  int n_classes = classes->size;

  // Iterate over all the stimuli once and select the 
  // maximal classifier response
  for (int j=0; j < n_classes; j++) {

    class_length = class_lengths_mat->FastEl(j);

    for (int k=0; k < class_length; k++) {

      taMath_double::mat_slice(slice, ConfusionMatrix, ctr, ctr);
      max = taMath_double::vec_max(slice, max_index);

      // If max_index is this class then score one true positive and
      // one true negative for every other class
      if (max_index == j) {
	c_mat->Set(c_mat->FastElAsDouble(j, j) + 1, j, j);
	tp++;
	tn += n_classes - 1;
      }
      // If max_index is not this class then score one false positive
      // for the max class, one false negative for the target class,
      // and n-2 true negatives for the other classes
      else {
	c_mat->Set(c_mat->FastElAsDouble(j, max_index) + 1.0f, j, max_index);
	fp++;
	tn += n_classes - 2;
	fn++;
      }
      ctr++;
    }
  }

  tpr = tp / (tp + fn);
  fpr = fp / (tn + fp);
  precision = tp / (tp + fp);
  recall = tp / (tp + fn);
  fm = 2.0f / ((1.0f / precision) + (1.0f / recall));
  acc = (tp + tn) / (tp + tn + fp + fn);
  roc_acc = 1.0f - sqrt(fpr*fpr + 1.0f - 2.0f*tpr + tpr*tpr);


//   cout << fun_name << " results: \n";

//   cout << "----------------------\n";
//   cout << "         True    False\n";
//   cout << "True   " << tp << "   " << fp << "\n";
//   cout << "False  " << fn << "   " << tn << "\n";
//   cout << "----------------------\n";
//   cout << "Note: Since false positives and false negatives mirror eachother in this\n";
//   cout << "          multiclass classifier the Sensitivity, Precision and Recall measures are equal\n";
//   cout << "Note: The ROC Accuracy measure is the distance to a perfect classifier on the\n";
//   cout << "          Receiver Operator Characteristic curve and it is recommended that you use\n";
//   cout << "          this metric to evaluate the classifier.\n";
//   cout << "Recall / Sensitivity / True Positive Rate (TPR) = TP/(TP+FN) = " << tpr << "\n";
//   cout << "Precision / Positive Prediction Value = TP/(TP+FP) = " << precision << "\n";
//   cout << "Specificity / True Negative Rate (TNR) = TN/(TN+FP) = " << tn / (tn+fp) << "\n";
//   cout << "False Positive Rate (FPR) = FP/(FP+TN) = " << fpr << "\n";

  taMisc::Info("ROC Accuracy = 1-SQRT[FPR^2 + (TPR-1)^2] = ", String(roc_acc));

  dest_data->Reset();
//   dest_data->NewColFmMatrix(c_mat, "ConfusionMatrix");

//   taDataProc::ConcatCols(dest_data, dest_data, work);

  // TODO: Do this for the rest of these stats..
  dest_data->NewColDouble("ROC_Accuracy");
  dest_data->AddRows();
  dest_data->SetVal(roc_acc, "ROC_Accuracy", 0);


  taBase::unRefDone(work);
  taBase::unRefDone(sorted_src_data);
  taBase::unRefDone(sort_spec);
  taBase::unRefDone(select_spec);
  taBase::unRefDone(confusion);
  taBase::unRefDone(confusion_target);
  taBase::unRefDone(class_lengths);
  taBase::unRefDone(rel);
  taBase::unRef(ConfusionMatrix);

  return true;

};

bool taDataAnal::ReceiverOperatingCharacteristic(DataTable* src_data,
						 bool view,
						 DataTable* dest_data,
						 const String& signal_data_col_nm,
						 const String& noise_data_col_nm,
						 float PRE_thr) {

  String fun_name = "ReceiverOperatingCharacteristic";
  
  if (!src_data) {
    taMisc::Error(fun_name + " - src_data cannot be NULL");return false;}

  GetDest(dest_data, src_data, fun_name);

  double_Matrix* vec_signal = (double_Matrix*)src_data->GetColMatrix(src_data->FindColNameIdx(signal_data_col_nm));
  double_Matrix* vec_noise = (double_Matrix*)src_data->GetColMatrix(src_data->FindColNameIdx(noise_data_col_nm));

  if (!(vec_signal->size == vec_noise->size)) {
    taMisc::Error("signal and noise vectors must have the same length");
    return false;
  }
  
  if (!(vec_signal->size > 0 && vec_noise > 0)) {
    taMisc::Error("signal and noise vectors must each have at least one element");
    return false;
  }

  double_Matrix* criterion_data = new double_Matrix;
  criterion_data->SetGeom(1, vec_signal->size + vec_noise->size);

  for (int i = 0; i < vec_signal->size; i++) {
    criterion_data->Set(vec_signal->FastEl(i), i);
    criterion_data->Set(vec_noise->FastEl(i), vec_noise->size + i);
  }

  taMath_double::vec_sort(criterion_data);

  dest_data->Reset();
  dest_data->NewColDouble("TPR");
  dest_data->NewColDouble("FPR");
  dest_data->NewColDouble("TPR_fit");
  dest_data->NewColDouble("Criterion");
  dest_data->NewColDouble("TP");  
  dest_data->NewColDouble("FP");
  dest_data->NewColDouble("TN");
  dest_data->NewColDouble("FN");
  dest_data->NewColDouble("Precision");
  dest_data->NewColDouble("Recall");
  dest_data->NewColDouble("Fmeasure");
  dest_data->NewColDouble("CHISQ");
  dest_data->NewColString("fun_string");
  dest_data->NewColDouble("FPR_smooth");
  dest_data->NewColDouble("TPR_smooth");
  dest_data->NewColDouble("AUC");
  dest_data->NewColDouble("MIN");
  dest_data->NewColDouble("MAX");
  dest_data->NewColDouble("PRE");
  dest_data->NewColDouble("SSEC");
  dest_data->NewColDouble("SSEA");
  dest_data->NewColDouble("degree");

  dest_data->AddRows(criterion_data->size + 1000);

  for (int i = 0; i < criterion_data->size; i++) {
    double criterion = criterion_data->FastEl(i) - .000000001;
    double tp = 0;
    double fp = 0;
    double fn = 0;
    double tn = 0;

    for (int j = 0; j < vec_signal->size; j++) {
      if (vec_signal->FastEl(j) > criterion)
	tp += 1;
      else
	fn += 1;
    }

    for (int j = 0; j < vec_noise->size; j++) {
      if (vec_noise->FastEl(j) > criterion)
	fp += 1;
      else
	tn += 1;
    }
    
    double recall = tp / (tp + fn);
    double precision = tp / (tp + fp);
    double fmeasure = 2*precision*recall/(precision+recall);
    double tpr = recall;
    double fpr = fp / (fp + tn);
 
    dest_data->SetVal(tpr, "TPR", i);
    dest_data->SetVal(fpr, "FPR", i);
    dest_data->SetVal(criterion, "Criterion", i);
    dest_data->SetVal(tp, "TP", i);
    dest_data->SetVal(fp, "FP", i);
    dest_data->SetVal(tn, "TN", i);
    dest_data->SetVal(fn, "FN", i);
    dest_data->SetVal(precision, "Precision", i);
    dest_data->SetVal(recall, "Recall", i);
    dest_data->SetVal(recall, "Fmeasure", i);
  }

  int obs = vec_signal->size;
 
  // Add 1,0 0,1 to tpr/fpr to help constrain the fit
  double_Matrix* vec_tpr = new double_Matrix; taBase::Ref(vec_tpr);
  double_Matrix* vec_fpr = new double_Matrix; taBase::Ref(vec_fpr);

  vec_tpr->CopyFrom(dest_data->data[0]->AR());
  vec_fpr->CopyFrom(dest_data->data[1]->AR());
  vec_tpr->InsertFrames(vec_tpr->size-1, 1);
  vec_fpr->InsertFrames(vec_tpr->size-1, 1);
  vec_tpr->InsertFrames(0,1);
  vec_fpr->InsertFrames(0,1);
  vec_tpr->Set(1, vec_tpr->size);
  vec_fpr->Set(1, vec_tpr->size);

  double SSEA = 0.0; // sum of squared errors of the augmented model
  double SSEC = 0.0;// sum of squared errors of the compact model
  double PRE = 0.0; // the computed pre to be compared to the user's provided threshold pre
  double chisq;
  double x;
  double y;
  String fun_string;

  double_Matrix* cov = new double_Matrix; taBase::Ref(cov);
  double_Matrix* coef = new double_Matrix; taBase::Ref(coef);
  double_Matrix* coef_tmp = new double_Matrix; taBase::Ref(coef_tmp);
  double_Matrix* coef_tmp_prv = new double_Matrix; taBase::Ref(coef_tmp_prv);
  double_Matrix* y_fit = new double_Matrix; taBase::Ref(y_fit);

  y_fit->SetGeom(1, vec_fpr->size); 

  for (int degree = 1; degree < vec_fpr->size * 2; degree++) {

    coef_tmp_prv->CopyFrom(coef_tmp);

    taMath_double::vec_regress_multi_lin_polynomial(vec_fpr, vec_tpr, coef_tmp, cov, degree, chisq);

    for (int i = 0; i < vec_fpr->size; i++) {
      x = vec_fpr->FastEl(i);
      y = coef_tmp->FastEl(0);

      for (int j = 1; j < degree; j++)
	y += coef_tmp->FastEl(j) * pow(x, j);
	
      y_fit->Set_Flat(y, i);
    }

    fun_string = "Y = ";
    for (int i = 0; i < degree; i++) fun_string += (String)coef_tmp->FastEl(i) + "*x^" + (String)i + " + ";
    fun_string += "e";

    SSEA = taMath_double::vec_ss_dist(vec_tpr, y_fit);

    PRE = (SSEC - SSEA)/SSEC;

    dest_data->SetVal(PRE, "PRE", degree - 1);
    dest_data->SetVal(SSEC, "SSEC", degree - 1);
    dest_data->SetVal(SSEA, "SSEA", degree - 1);
    dest_data->SetVal(degree, "degree", degree - 1);
    dest_data->SetVal(chisq, "CHISQ", degree - 1);
    dest_data->SetVal(fun_string, "fun_string", degree - 1);

    if (PRE < PRE_thr && degree != 0) break;

    SSEC = SSEA;
  }

  taBase::unRefDone(cov);
  taBase::unRefDone(coef);
  taBase::unRefDone(coef_tmp);
  taBase::unRefDone(coef_tmp_prv);
  taBase::unRefDone(vec_tpr);
  taBase::unRefDone(vec_fpr);

  double area = taMath_double::integrate_polynomial(coef, 0, 1);

  dest_data->SetVal(area, "AUC", 0);

  double_Matrix* tpr_smooth = new double_Matrix; taBase::Ref(tpr_smooth);
  tpr_smooth = (double_Matrix*)dest_data->GetColMatrix(dest_data->FindColNameIdx("TPR_smooth"));
  int roc_min_idx;
  int roc_max_idx;
  taMath_double::vec_min(tpr_smooth, roc_min_idx);
  taMath_double::vec_max(tpr_smooth, roc_max_idx);
  dest_data->SetVal(tpr_smooth->FastEl(roc_min_idx), "MIN", 0);
  dest_data->SetVal(tpr_smooth->FastEl(roc_max_idx), "MAX", 0);
  taBase::unRefDone(tpr_smooth);

  return true;
}

////////////////////////////////////////////////////////////////////////////////////
//	distance matricies

bool taDataAnal::DistMatrix(float_Matrix* dist_mat, DataTable* src_data,
			    const String& data_col_nm,
			    taMath::DistMetric metric, bool norm, float tol,
			    bool incl_scalars) {
  if(!src_data) {
    taMisc::Error("taDataAnal::DistMatrix -- src_data is NULL -- must pass in this data");
    return false;
  }
  if(!dist_mat) {
    taMisc::Error("taDataAnal::DistMatrix -- dist_mat is NULL -- must pass in this matrix");
    return false;
  }

  if(data_col_nm.empty()) {
    int n_rows = src_data->rows;
    dist_mat->SetGeom(2, n_rows, n_rows);
    for(int ar=0; ar<n_rows; ar++) {
      for(int br=0; br<n_rows; br++) {
	double cell_dist = 0.0;
	for(int cl=0; cl<src_data->cols(); cl++) {
	  DataCol* da = src_data->GetColData(cl);
	  if(!da) return false;
	  if(da->valType() != VT_FLOAT && da->valType() != VT_DOUBLE) continue;
	  if(!da->isMatrix()) {
	    if(!incl_scalars) continue;
	    float fa = src_data->GetValAsFloat(cl, ar);
	    float fb = src_data->GetValAsFloat(cl, br);
	    cell_dist += taMath_float::scalar_dist(fa, fb, metric, tol);
	  }
	  else {
	    if(da->valType() == VT_FLOAT) {
	      float_Matrix* ta = (float_Matrix*)src_data->GetValAsMatrix(cl, ar);
	      taBase::Ref(ta);
	      float_Matrix* tb = (float_Matrix*)src_data->GetValAsMatrix(cl, br);
	      taBase::Ref(tb);
	      float dist = taMath_float::vec_dist(ta, tb, metric, norm, tol);
	      cell_dist += dist;
	      taBase::unRefDone(ta);
	      taBase::unRefDone(tb);
	    }
	    else {		// VT_DOUBLE
	      double_Matrix* ta = (double_Matrix*)src_data->GetValAsMatrix(cl, ar);
	      taBase::Ref(ta);
	      double_Matrix* tb = (double_Matrix*)src_data->GetValAsMatrix(cl, br);
	      taBase::Ref(tb);
	      double dist = taMath_double::vec_dist(ta, tb, metric, norm, tol);
	      cell_dist += dist;
	      taBase::unRefDone(ta);
	      taBase::unRefDone(tb);
	    }
	  }
	}
	dist_mat->FastEl(br,ar) = cell_dist;
      }
    }
    return true;
  }
  else {
    DataCol* da = GetMatrixDataCol(src_data, data_col_nm);
    if(!da)
      return false;
    bool rval = true;
    if(da->valType() == VT_FLOAT) {
      rval = taMath_float::mat_dist(dist_mat, (float_Matrix*)da->AR(), metric, norm, tol);
    }
    else if(da->valType() == VT_DOUBLE) {
      double_Matrix ddmat(false);
      rval = taMath_double::mat_dist(&ddmat, (double_Matrix*)da->AR(), metric, norm, tol);
      taMath::mat_cvt_double_to_float(dist_mat, &ddmat);
    }
    return rval;
  }
}

bool taDataAnal::DistMatrixTable(DataTable* dist_mat, bool view, DataTable* src_data,
				 const String& data_col_nm, const String& name_col_nm,
				 taMath::DistMetric metric, bool norm, float tol,
				 bool incl_scalars) {
  if(!src_data) {
    taMisc::Error("taDataAnal::DistMatrixTable -- src_data is NULL -- must pass in this data");
    return false;
  }
  float_Matrix dmat(false);
  bool rval = DistMatrix(&dmat, src_data, data_col_nm, metric, norm, tol, incl_scalars);
  if(!rval) return false;
  GetDest(dist_mat, src_data, "DistMatrix");
  dist_mat->StructUpdate(true);
  dist_mat->Reset();	// get rid of any existing cols
  if(!name_col_nm.empty()) {
    DataCol* nmda = src_data->FindColName(name_col_nm, true); // errmsg
    if(nmda) {
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
      dist_mat->StructUpdate(false);
      if(view) dist_mat->FindMakeGridView();
      return true;
    }
  }
  // no labels -- just make a col
  String cl_nm = src_data->name + "_DistMatrix";
  int idx;
  DataCol* dmda = dist_mat->FindMakeColName(cl_nm, idx, VT_FLOAT, 2, dmat.dim(0),
						   dmat.dim(1));
  dmda->SetUserData("TOP_ZERO", true);
  dist_mat->SetUserData("N_ROWS", 1);
  dist_mat->SetUserData("AUTO_SCALE", true);
  dist_mat->AddBlankRow();
  dmda->SetValAsMatrix(&dmat, -1);
  dist_mat->StructUpdate(false);
  if(view) dist_mat->FindMakeGridView();
  return true;
}

bool taDataAnal::CrossDistMatrix(float_Matrix* dist_mat,
				 DataTable* src_data_a, const String& data_col_nm_a,
				 DataTable* src_data_b, const String& data_col_nm_b,
				 taMath::DistMetric metric, bool norm, float tol,
				 bool incl_scalars) {
  if(!src_data_a || !src_data_b) {
    taMisc::Error("taDataAnal::CrossDistMatrix -- src_data is NULL -- must pass in this data");
    return false;
  }
  if(!dist_mat) {
    taMisc::Error("taDataAnal::CrossDistMatrix -- dist_mat is NULL -- must pass in this matrix");
    return false;
  }
  if(data_col_nm_a.empty() && data_col_nm_b.empty()) {
    int n_rows = src_data_a->rows;
    int n_cols = src_data_b->rows;
    dist_mat->SetGeom(2, n_cols, n_rows);
    for(int ar=0; ar<n_rows; ar++) {
      for(int br=0; br<n_cols; br++) {
	double cell_dist = 0.0;
	for(int cl=0; cl<src_data_a->cols(); cl++) {
	  DataCol* da_a = src_data_a->GetColData(cl);
	  DataCol* da_b = src_data_b->GetColData(cl);
	  if(!da_a || !da_b) return false;
	  if(da_a->valType() != da_b->valType()) continue;
	  if(da_a->valType() != VT_FLOAT && da_a->valType() != VT_DOUBLE) continue;
	  if(!da_a->isMatrix() || !da_b->isMatrix()) {
	    if(!incl_scalars || (da_a->isMatrix() != da_b->isMatrix())) continue;
	    float fa = src_data_a->GetValAsFloat(cl, ar);
	    float fb = src_data_b->GetValAsFloat(cl, br);
	    cell_dist += taMath_float::scalar_dist(fa, fb, metric, tol);
	  }
	  else {
	    if(da_a->valType() == VT_FLOAT) {
	      float_Matrix* ta = (float_Matrix*)src_data_a->GetValAsMatrix(cl, ar);
	      taBase::Ref(ta);
	      float_Matrix* tb = (float_Matrix*)src_data_b->GetValAsMatrix(cl, br);
	      taBase::Ref(tb);
	      float dist = taMath_float::vec_dist(ta, tb, metric, norm, tol);
	      cell_dist += dist;
	      taBase::unRefDone(ta);
	      taBase::unRefDone(tb);
	    }
	    else {		// VT_DOUBLE
	      double_Matrix* ta = (double_Matrix*)src_data_a->GetValAsMatrix(cl, ar);
	      taBase::Ref(ta);
	      double_Matrix* tb = (double_Matrix*)src_data_b->GetValAsMatrix(cl, br);
	      taBase::Ref(tb);
	      double dist = taMath_double::vec_dist(ta, tb, metric, norm, tol);
	      cell_dist += dist;
	      taBase::unRefDone(ta);
	      taBase::unRefDone(tb);
	    }
	  }
	}
	dist_mat->FastEl(br,ar) = cell_dist;
      }
    }
    return true;
  }
  else {
    DataCol* da_a = GetMatrixDataCol(src_data_a, data_col_nm_a);
    if(!da_a)
      return false;
    DataCol* da_b = GetMatrixDataCol(src_data_b, data_col_nm_b);
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
      double_Matrix ddmat(false);
      rval = taMath_double::mat_cross_dist(&ddmat, (double_Matrix*)da_a->AR(), (double_Matrix*)da_b->AR(), metric, norm, tol);
      taMath::mat_cvt_double_to_float(dist_mat, &ddmat);
    }
    return rval;
  }
}

bool taDataAnal::CrossDistMatrixTable(DataTable* dist_mat, bool view,
				      DataTable* src_data_a, const String& data_col_nm_a,
				      const String& name_col_nm_a,
				      DataTable* src_data_b, const String& data_col_nm_b,
				      const String& name_col_nm_b,
				      taMath::DistMetric metric, bool norm, float tol,
				      bool incl_scalars) {
  if(!src_data_a || !src_data_b) {
    taMisc::Error("taDataAnal::CrossDistMatrixTable -- src_data is NULL -- must pass in this data");
    return false;
  }
  float_Matrix dmat(false);
  bool rval = CrossDistMatrix(&dmat, src_data_a, data_col_nm_a, src_data_b, data_col_nm_b, 
			      metric, norm, tol, incl_scalars);
  if(!rval) return false;
  GetDest(dist_mat, src_data_a, src_data_b->name + "_DistMatrix");
  dist_mat->StructUpdate(true);
  dist_mat->Reset();	// nuke everything
  if(!name_col_nm_a.empty() && !name_col_nm_b.empty()) {
    DataCol* nmda_a = src_data_a->FindColName(name_col_nm_a, true); // errmsg
    DataCol* nmda_b = src_data_b->FindColName(name_col_nm_b, true); // errmsg
    if(nmda_a && nmda_b) {
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
      dist_mat->StructUpdate(false);
      if(view) dist_mat->FindMakeGridView();
      return true;
    }
  }
  // no labels -- just make a col
  String cl_nm = src_data_a->name + "_" + src_data_b->name + "_DistMatrix";
  int idx;
  DataCol* dmda = dist_mat->FindMakeColName(cl_nm, idx, VT_FLOAT, 2, dmat.dim(0),
						   dmat.dim(1));
  dmda->SetUserData("TOP_ZERO", true);
  dist_mat->SetUserData("N_ROWS", 1);
  dist_mat->SetUserData("AUTO_SCALE", true);
  dist_mat->AddBlankRow();
  dmda->SetValAsMatrix(&dmat, -1);
  dist_mat->StructUpdate(false);
  if(view) dist_mat->FindMakeGridView();
  return true;
}

///////////////////////////////////////////////////////////////////
// correlation matricies

bool taDataAnal::CorrelMatrix(float_Matrix* correl_mat, DataTable* src_data,
			      const String& data_col_nm) {
  if(!src_data) {
    taMisc::Error("taDataAnal::CorrelMatrix -- src_data is NULL -- must pass in this data");
    return false;
  }
  if(!correl_mat) {
    taMisc::Error("taDataAnal::CorrelMatrix -- correl_mat is NULL -- must pass in this matrix");
    return false;
  }
  DataCol* da = GetMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return false;
  bool rval = true;
  if(da->valType() == VT_FLOAT) {
    rval = taMath_float::mat_correl(correl_mat, (float_Matrix*)da->AR());
  }
  else if(da->valType() == VT_DOUBLE) {
    double_Matrix ddmat(false);
    rval = taMath_double::mat_correl(&ddmat, (double_Matrix*)da->AR());
    taMath::mat_cvt_double_to_float(correl_mat, &ddmat);
  }
  return rval;
}

bool taDataAnal::CorrelMatrixTable(DataTable* correl_mat, bool view, DataTable* src_data,
				   const String& data_col_nm) {
  if(!src_data) {
    taMisc::Error("taDataAnal::CorrelMatrixTable -- src_data is NULL -- must pass in this data");
    return false;
  }
  float_Matrix dmat(false);
  bool rval = CorrelMatrix(&dmat, src_data, data_col_nm);
  if(!rval) return false;
  GetDest(correl_mat, src_data, "CorrelMatrix");
  // no labels -- just make a col
  String cl_nm = src_data->name + "_CorrelMatrix";
  int idx;
  DataCol* dmda = correl_mat->FindMakeColName(cl_nm, idx, VT_FLOAT, 2, dmat.dim(0),
						     dmat.dim(1));
  dmda->SetUserData("TOP_ZERO", true);
  correl_mat->SetUserData("N_ROWS", 1);
  correl_mat->SetUserData("AUTO_SCALE", true);
  correl_mat->AddBlankRow();
  dmda->SetValAsMatrix(&dmat, -1);
  if(view) correl_mat->FindMakeGridView();
  return true;
}

///////////////////////////////////////////////////////////////////
// standard multidimensional data analysis methods

bool taDataAnal::Cluster(DataTable* clust_data, bool view, DataTable* src_data,
			 const String& data_col_nm, const String& name_col_nm,
			 taMath::DistMetric metric, bool norm, float tol) {
  if(!src_data) {
    taMisc::Error("taDataAnal::Cluster -- src_data is NULL -- must pass in this data");
    return false;
  }
  DataCol* da = GetMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return false;
  DataCol* nmda = src_data->FindColName(name_col_nm, true); // errmsg
  if(!nmda)
    return false;
  
  GetDest(clust_data, src_data, "col_" + data_col_nm + "_Cluster");

  clust_data->StructUpdate(true);

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

  clust_data->StructUpdate(false);

  if(view) clust_data->FindMakeGraphView();
  return true;
}

bool taDataAnal::PCAEigens(float_Matrix* eigen_vals, float_Matrix* eigen_vecs,
			   DataTable* src_data, const String& data_col_nm) {
  if(!src_data) {
    taMisc::Error("taDataAnal::PCAEigens -- src_data is NULL -- must pass in this data");
    return false;
  }
  if(!eigen_vals) {
    taMisc::Error("taDataAnal::PCAEigens -- eigen_vals is NULL -- must pass in this matrix");
    return false;
  }
  if(!eigen_vecs) {
    taMisc::Error("taDataAnal::PCAEigens -- eigen_vecs is NULL -- must pass in this matrix");
    return false;
  }
  float_Matrix correl_mat(false);
  if(!CorrelMatrix(&correl_mat, src_data, data_col_nm))
    return false;
  return taMath_float::mat_eigen_owrite(&correl_mat, eigen_vals, eigen_vecs);
}

bool taDataAnal::PCAEigenTable(DataTable* pca_data, bool view, DataTable* src_data,
			       const String& data_col_nm) {
  if(!src_data) {
    taMisc::Error("taDataAnal::PCAEigenTable -- src_data is NULL -- must pass in this data");
    return false;
  }
  float_Matrix eigen_vals(false);
  float_Matrix eigen_vecs(false);
  if(!PCAEigens(&eigen_vals, &eigen_vecs, src_data, data_col_nm)) return false;

  GetDest(pca_data, src_data, "col_" + data_col_nm + "_PCAEigens");

  DataCol* da = GetMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return false;

  String cl_nm = data_col_nm + "_PCAEigens";
  int idx;
  DataCol* dmda = pca_data->FindMakeColName(cl_nm, idx, VT_FLOAT,
						   da->cell_dims(), da->GetCellGeom(0),
						   da->GetCellGeom(1), da->GetCellGeom(2),
						   da->GetCellGeom(3));

  pca_data->StructUpdate(true);
  for(int i=0;i<eigen_vecs.dim(0);i++) {
    pca_data->AddBlankRow();
    for(int j=0;j<eigen_vecs.dim(1);j++) {
      dmda->SetValAsFloatM(eigen_vecs.FastEl(i,j), -1, j);
    }
  }
  pca_data->StructUpdate(false);
  if(view) pca_data->FindMakeGridView();
  return true;
}

bool taDataAnal::PCA2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			   const String& data_col_nm, const String& name_col_nm, 
			   int x_axis_c, int y_axis_c) {
  if(!src_data) {
    taMisc::Error("taDataAnal::PCA2dPrjn -- src_data is NULL -- must pass in this data");
    return false;
  }
  DataCol* da = GetMatrixDataCol(src_data, data_col_nm);
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

  float_Matrix eigen_vals(false);
  float_Matrix eigen_vecs(false);
  if(!PCAEigens(&eigen_vals, &eigen_vecs, src_data, data_col_nm)) return false;

  float_Matrix xevec(false);		// x eigen vector
  taMath_float::mat_col(&xevec, &eigen_vecs, x_axis_c); // eigen vector = column 
  {
    String msg;
    msg << "Component no: " << x_axis_c << " has eigenvalue: "
	<< eigen_vals.FastEl(x_axis_c);
    taMisc::Info(msg);
  }

  float_Matrix yevec(false);		// x eigen vector
  taMath_float::mat_col(&yevec, &eigen_vecs, y_axis_c); // eigen vector = column 
  {
    String msg;
    msg << "Component no: " << y_axis_c << " has eigenvalue: "
	<< eigen_vals.FastEl(y_axis_c);
    taMisc::Info(msg);
  }

  float_Matrix xprjn(false);
  taMath_float::mat_prjn(&xprjn, (float_Matrix*)da->AR(), &xevec);
  float_Matrix yprjn(false);
  taMath_float::mat_prjn(&yprjn, (float_Matrix*)da->AR(), &yevec);

  DataCol* nmda = src_data->FindColName(name_col_nm, true); // errmsg

  GetDest(prjn_data, src_data, "col_" + data_col_nm + "_PCA2dPrjn");

  int idx;
  DataCol* nm = NULL;
  if(nmda)
    nm = prjn_data->FindMakeColName(name_col_nm, idx, VT_STRING);
  DataCol* xda = prjn_data->FindMakeColName("x_prjn", idx, VT_FLOAT);
  DataCol* yda = prjn_data->FindMakeColName("y_prjn", idx, VT_FLOAT);

  xda->SetUserData("X_AXIS", true);
  yda->SetUserData("PLOT_1", true);
  if(nm)
    nm->SetUserData("PLOT_2", true);
  prjn_data->SetUserData("PLOT_STYLE", "POINTS");

  for(int i=0;i<src_data->rows;i++) {
    prjn_data->AddBlankRow();
    if(nm)
      nm->SetValAsString(nmda->GetValAsString(i),-1);
    xda->SetValAsFloat(xprjn.FastEl(i), -1);
    yda->SetValAsFloat(yprjn.FastEl(i), -1);
  }

  if(view) prjn_data->FindMakeGraphView();
  return true;
}  

bool taDataAnal::MDS2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			   const String& data_col_nm, const String& name_col_nm, 
			   int x_axis_c, int y_axis_c,
			   taMath::DistMetric metric, bool norm, float tol)
{
  if(!src_data) {
    taMisc::Error("taDataAnal::MDS2dPrjn -- src_data is NULL -- must pass in this data");
    return false;
  }

  int dim = src_data->rows;
  if((x_axis_c < 0) || (x_axis_c >= dim)) {
    taMisc::Error("MDS2dPrjn: x_axis component must be between 0 and",String(dim-1));
    return false;
  }
  if((y_axis_c < 0) || (y_axis_c >= dim)) {
    taMisc::Error("MDS2dPrjn: y_axis component must be between 0 and",String(dim-1));
    return false;
  }

  float_Matrix dist_mat(false);
  DistMatrix(&dist_mat, src_data, data_col_nm, metric, norm, tol);

  float_Matrix xy_coords(false);
  taMath_float::mat_mds_owrite(&dist_mat, &xy_coords, x_axis_c, y_axis_c);

  DataCol* nmda = src_data->FindColName(name_col_nm, true); // errmsg

  GetDest(prjn_data, src_data, "col_" + data_col_nm + "_MDS2dPrjn");

  int idx;
  DataCol* nm = NULL;
  if(nmda)
    nm = prjn_data->FindMakeColName(name_col_nm, idx, VT_STRING);
  DataCol* xda = prjn_data->FindMakeColName("x_prjn", idx, VT_FLOAT);
  DataCol* yda = prjn_data->FindMakeColName("y_prjn", idx, VT_FLOAT);

  xda->SetUserData("X_AXIS", true);
  yda->SetUserData("PLOT_1", true);
  if(nm)
    nm->SetUserData("PLOT_2", true);
  prjn_data->SetUserData("PLOT_STYLE", "POINTS");

  for(int i=0;i<src_data->rows;i++) {
    prjn_data->AddBlankRow();
    if(nm)
      nm->SetValAsString(nmda->GetValAsString(i),-1);
    xda->SetValAsFloat(xy_coords.FastEl(0, i), -1);
    yda->SetValAsFloat(xy_coords.FastEl(1, i), -1);
  }

  if(view) prjn_data->FindMakeGraphView();
  return true;
}

bool taDataAnal::RowPat2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			     const String& data_col_nm, const String& name_col_nm,
			     int x_row, int y_row,
			     taMath::DistMetric metric, bool norm, float tol)
{
  if(!src_data) {
    taMisc::Error("taDataAnal::RowPat2dPrjn -- src_data is NULL -- must pass in this data");
    return false;
  }
  DataCol* da = GetMatrixDataCol(src_data, data_col_nm);
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

  float_Matrix xprjn(false);
  taMath_float::mat_prjn(&xprjn, (float_Matrix*)da->AR(), xrow, metric, norm, tol);
  float_Matrix yprjn(false);
  taMath_float::mat_prjn(&yprjn, (float_Matrix*)da->AR(), yrow, metric, norm, tol);

  taBase::UnRef(xrow);
  taBase::UnRef(yrow);

  DataCol* nmda = src_data->FindColName(name_col_nm, true); // errmsg

  GetDest(prjn_data, src_data, "col_" + data_col_nm + "_RowPat2dPrjn");

  int idx;
  DataCol* nm = NULL;
  if(nmda)
    nm = prjn_data->FindMakeColName(name_col_nm, idx, VT_STRING);
  DataCol* xda = prjn_data->FindMakeColName("x_prjn", idx, VT_FLOAT);
  DataCol* yda = prjn_data->FindMakeColName("y_prjn", idx, VT_FLOAT);

  xda->SetUserData("X_AXIS", true);
  yda->SetUserData("PLOT_1", true);
  if(nm)
    nm->SetUserData("PLOT_2", true);
  prjn_data->SetUserData("PLOT_STYLE", "POINTS");

  for(int i=0;i<src_data->rows;i++) {
    prjn_data->AddBlankRow();
    if(nm)
      nm->SetValAsString(nmda->GetValAsString(i),-1);
    xda->SetValAsFloat(xprjn.FastEl(i), -1);
    yda->SetValAsFloat(yprjn.FastEl(i), -1);
  }

  if(view) prjn_data->FindMakeGraphView();
  return true;
}

bool taDataAnal::TimeAvg(DataTable* avg_data, bool view, DataTable* src_data,
			 float avg_dt, bool float_only)
{
  if(!src_data) {
    taMisc::Error("taDataAnal::TimeAvg -- src_data is NULL -- must pass in this data");
    return false;
  }

  float_Matrix float_tmp(false);
  GetDest(avg_data, src_data, "TimeAvg");

  avg_data->Reset();
  *avg_data = *src_data;	// do complete copy, then operate in place
  for(int i=0;i<avg_data->data.size;i++) {
    DataCol* da = avg_data->data[i];
    if(!da->isNumeric()) continue;
    da->ClearColFlag(DataCol::CALC); // don't recompute!
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
      taMath_float::vec_fm_ints(&float_tmp, mat);
      taMath_float::mat_time_avg(&float_tmp, avg_dt);
      taMath_float::vec_to_ints(mat, &float_tmp);
    }
  }

  if(view) avg_data->FindMakeGraphView();
  return true;
}

bool taDataAnal::SmoothImpl(DataTable* smooth_data, bool view, DataTable* src_data,
			    float_Matrix* flt_kern, double_Matrix* dbl_kern,
			    int kern_half_wd, bool keep_edges, bool float_only)
{
  bool has_matrix = false;
  for(int i=0;i<src_data->data.size;i++) {
    DataCol* sda = src_data->data[i];
    if(sda->isMatrix()) {
      has_matrix = true; 
      break;
    }
  }
  if(has_matrix && !keep_edges) {
    taMisc::Warning("taDataAnal::SmoothImpl: data table",src_data->GetDisplayName(),
		    "has matrix cells, must use keep_edges = true! this is how it is being run");
    keep_edges = true;
  }

  float_Matrix float_tmp(false);
  float_Matrix float_tmp2(false);
  smooth_data->StructUpdate(true);
  smooth_data->Reset();
  String ad_nm = smooth_data->name;
  *smooth_data = *src_data;	// todo: deal with !keep_edges
  smooth_data->name = ad_nm;

  if(!keep_edges) {
    smooth_data->RemoveRows(0,kern_half_wd); // get rid of first n rows
    smooth_data->RemoveRows(-1,kern_half_wd); // get rid of last n rows
  }

  for(int i=0;i<smooth_data->data.size;i++) {
    DataCol* da = smooth_data->data[i];
    DataCol* sda = src_data->data[i];
    if(!da->isNumeric()) continue;
    da->ClearColFlag(DataCol::CALC); // don't recompute!
    if(da->valType() == VT_BYTE) continue;
    if(float_only && (da->valType() == VT_INT)) continue;
    
    if(da->valType() == VT_FLOAT) {
      if(da->isMatrix())
	taMath_float::mat_frame_convolve((float_Matrix*)da->AR(), (float_Matrix*)sda->AR(), 
					 flt_kern);
      else
	taMath_float::vec_convolve((float_Matrix*)da->AR(), (float_Matrix*)sda->AR(), 
				   flt_kern, keep_edges);
    }
    else if(da->valType() == VT_DOUBLE) {
      if(da->isMatrix())
	taMath_double::mat_frame_convolve((double_Matrix*)da->AR(), (double_Matrix*)sda->AR(), 
					 dbl_kern);
      else
	taMath_double::vec_convolve((double_Matrix*)da->AR(), (double_Matrix*)sda->AR(), 
				 dbl_kern, keep_edges);
    }
    else if(da->valType() == VT_INT) { // expensive double-convert..
      int_Matrix* mat = (int_Matrix*)sda->AR();
      taMath_float::vec_fm_ints(&float_tmp, mat);
      float_tmp2.SetGeomN(mat->geom);
      if(da->isMatrix())
	taMath_float::mat_frame_convolve(&float_tmp2, &float_tmp, flt_kern);
      else
	taMath_float::vec_convolve(&float_tmp2, &float_tmp, flt_kern, keep_edges);
      int_Matrix* smat = (int_Matrix*)da->AR();
      taMath_float::vec_to_ints(smat, &float_tmp2);
    }
  }
  smooth_data->StructUpdate(false);

  if(view) smooth_data->FindMakeGraphView();
  return true;
}

bool taDataAnal::SmoothUniform(DataTable* smooth_data, bool view, DataTable* src_data,
			       int kern_half_wd, bool neg_tail, bool pos_tail,
			       bool keep_edges, bool float_only)
{
  if(!src_data) {
    taMisc::Error("taDataAnal::SmoothUniform -- src_data is NULL -- must pass in this data");
    return false;
  }

  GetDest(smooth_data, src_data, "SmoothUniform");

  float_Matrix flt_kern(false);
  taMath_float::vec_kern_uniform(&flt_kern, kern_half_wd, neg_tail, pos_tail);
  double_Matrix dbl_kern(false);
  taMath_double::vec_kern_uniform(&dbl_kern, kern_half_wd, neg_tail, pos_tail);

  return SmoothImpl(smooth_data, view, src_data, &flt_kern, &dbl_kern,
		    kern_half_wd, keep_edges, float_only);
}

bool taDataAnal::SmoothGauss(DataTable* smooth_data, bool view, DataTable* src_data,
			     int kern_half_wd, float kern_sigma, bool neg_tail, bool pos_tail,
			     bool keep_edges, bool float_only)
{
  if(!src_data) {
    taMisc::Error("taDataAnal::SmoothGauss -- src_data is NULL -- must pass in this data");
    return false;
  }

  GetDest(smooth_data, src_data, "SmoothGauss");

  float_Matrix flt_kern(false);
  taMath_float::vec_kern_gauss(&flt_kern, kern_half_wd, kern_sigma, neg_tail, pos_tail);
  double_Matrix dbl_kern(false);
  taMath_double::vec_kern_gauss(&dbl_kern, kern_half_wd, kern_sigma, neg_tail, pos_tail);

  return SmoothImpl(smooth_data, view, src_data, &flt_kern, &dbl_kern,
		    kern_half_wd, keep_edges, float_only);
}

bool taDataAnal::SmoothExp(DataTable* smooth_data, bool view, DataTable* src_data,
			   int kern_half_wd, float kern_exp, bool neg_tail,
			   bool pos_tail, bool keep_edges, bool float_only)
{
  if(!src_data) {
    taMisc::Error("taDataAnal::SmoothExp -- src_data is NULL -- must pass in this data");
    return false;
  }

  GetDest(smooth_data, src_data, "SmoothExp");

  float_Matrix flt_kern(false);
  taMath_float::vec_kern_exp(&flt_kern, kern_half_wd, kern_exp, neg_tail, pos_tail);
  double_Matrix dbl_kern(false);
  taMath_double::vec_kern_exp(&dbl_kern, kern_half_wd, kern_exp, neg_tail, pos_tail);

  return SmoothImpl(smooth_data, view, src_data, &flt_kern, &dbl_kern,
		    kern_half_wd, keep_edges, float_only);
}

bool taDataAnal::SmoothPow(DataTable* smooth_data, bool view, DataTable* src_data,
			   int kern_half_wd, float kern_exp, bool neg_tail,
			   bool pos_tail, bool keep_edges, bool float_only)
{
  if(!src_data) {
    taMisc::Error("taDataAnal::SmoothPow -- src_data is NULL -- must pass in this data");
    return false;
  }

  GetDest(smooth_data, src_data, "SmoothPow");

  if(kern_exp > 0.0f) {
    taMisc::Warning("Note: typically the power exponent is negative to produce an inverse relationship such that points further away are weighted lower");
  }

  float_Matrix flt_kern(false);
  taMath_float::vec_kern_pow(&flt_kern, kern_half_wd, kern_exp, neg_tail, pos_tail);
  double_Matrix dbl_kern(false);
  taMath_double::vec_kern_pow(&dbl_kern, kern_half_wd, kern_exp, neg_tail, pos_tail);

  return SmoothImpl(smooth_data, view, src_data, &flt_kern, &dbl_kern,
		    kern_half_wd, keep_edges, float_only);
}

bool taDataAnal::Matrix3DGraph(DataTable* data, const String& x_axis_col, const String& z_axis_col) {
  if(!data) return false;

  DataCol* xax = GetNumDataCol(data, x_axis_col);
  if(!xax) return false;
  DataCol* zax = GetNumDataCol(data, z_axis_col);
  if(!zax) return false;

  data->SortCol(xax, true, zax, true);

  DataTable dupl(false);
  dupl.CopyFrom(data);
  dupl.SortColName(z_axis_col, true, x_axis_col, true);
  taDataProc::AppendRows(data, &dupl);
  dupl.Reset();
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

bool DataAnalCall::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!code.contains("::")) return false;
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String mthobj = lhs;
  if(lhs.contains('='))
    mthobj = trim(lhs.after('='));
  String objnm = mthobj.before("::");
  TypeDef* td = taMisc::types.FindName(objnm);
  if(!td) return false;
  if(objnm == "taDataAnal") return true;
  return false;
}

