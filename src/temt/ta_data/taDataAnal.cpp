// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "taDataAnal.h"
#include <taProject>
#include <DataTable>
#include <float_Data>
#include <float_Matrix>
#include <double_Matrix>
#include <taMath_double>
#include <taMath_float>
#include <DataSortSpec>
#include <DataSelectSpec>
#include <DataSelectEl>
#include <DataGroupSpec>
#include <DataGroupEl>
#include <taDataProc>
#include <Relation>
#include <int_Matrix>
#include <ClustNode>
#include <GridTableView>
#include <T3Annotation>
#include <Completions>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(taDataAnal);


bool taDataAnal::GetDest(DataTable*& dest, const DataTable* src, const String& suffix, bool reset) {
  if(dest) {
    if(reset)
      dest->ResetData();		// always clear out for new data
    return false;
  }
  taProject* proj = src->GetMyProj();
  dest = proj->GetNewAnalysisDataTable(src->name + "_" + suffix, true);
  return true;
}

DataCol* taDataAnal::GetDataCol(DataTable* src_data, const String& data_col_nm) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal: src_data is NULL or has no rows");
    return NULL;
  }
  DataCol* da = src_data->FindColName(data_col_nm, true); // err msg
  return da;
}

DataCol* taDataAnal::GetMatrixDataCol(DataTable* src_data, const String& data_col_nm) {
  DataCol* da = GetDataCol(src_data, data_col_nm);
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

DataCol* taDataAnal::GetNonMatrixDataCol(DataTable* src_data, const String& data_col_nm) {
  DataCol* da = GetDataCol(src_data, data_col_nm);
  if(!da)
    return NULL;
  if(da->is_matrix) {
    taMisc::Error("taDataAnal: column named:", data_col_nm,
		  "is a matrix where a scalar is required, in data table:", src_data->name);
    return NULL;
  }
  return da;
}

DataCol* taDataAnal::GetStringDataCol(DataTable* src_data, const String& data_col_nm) {
  DataCol* da = GetNonMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return NULL;
  if(da->valType() != VT_STRING) {
    taMisc::Error("taDataAnal: column named:", data_col_nm,
		  "is not of type String in data table:", src_data->name);
    return NULL;
  }
  return da;
}

DataCol* taDataAnal::GetNumDataCol(DataTable* src_data, const String& data_col_nm) {
  DataCol* da = GetNonMatrixDataCol(src_data, data_col_nm);
  if(!da)
    return NULL;
  if(!da->isNumeric()) {
    taMisc::Error("taDataAnal: column named:", data_col_nm,
		  "is not a numeric type -- must be, in data table:", src_data->name);
    return NULL;
  }
  return da;
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

float taDataAnal::AnovaOneWay(DataTable* result_data, DataTable* src_data,
                              const String& cond_col_nm, const String& data_col_nm) {
  float rval = 0.0f;
  DataCol* cda = GetNonMatrixDataCol(src_data, cond_col_nm);
  if(!cda) return rval;
  DataCol* yda = GetNumDataCol(src_data, data_col_nm);
  if(!yda) return rval;

  GetDest(result_data, src_data, "AnovaOneWay", true);

  DataGroupSpec gp_spec;
  gp_spec.append_agg_name = true;
  gp_spec.SetDataTable(src_data);
  DataGroupEl* cond_gp = (DataGroupEl*)gp_spec.AddColumn(cond_col_nm, src_data);
  cond_gp->agg.op = Aggregate::GROUP;
  DataGroupEl* mean_gp = (DataGroupEl*)gp_spec.AddColumn(data_col_nm, src_data);
  mean_gp->agg.op = Aggregate::MEAN;
  DataGroupEl* ss_gp = (DataGroupEl*)gp_spec.AddColumn(data_col_nm, src_data);
  ss_gp->agg.op = Aggregate::SS;
  DataGroupEl* n_gp = (DataGroupEl*)gp_spec.AddColumn(data_col_nm, src_data);
  n_gp->agg.op = Aggregate::N;

  taDataProc::Group(result_data, src_data, &gp_spec);

  int n_gps = result_data->rows;
  int n_result_cols = 4;
  
  if(n_gps <= 1) {
    taMisc::Error("ANOVA requires 2 or more groups / levels within condition variable -- source data only has 1 or less");
    return rval;
  }

  DataCol* conds_col = result_data->GetColData(0);
  DataCol* means_col = result_data->GetColData(1);
  DataCol* ss_col = result_data->GetColData(2);
  DataCol* n_col = result_data->GetColData(3);
  float grand_mean = 0.0f;
  float between_ss = 0.0f;
  float between_df = n_gps - 1;
  float within_ss = 0.0f;
  float within_df = src_data->rows - n_gps; // todo: check
  if(means_col->valType() == taBase::VT_DOUBLE) {
    double_Matrix* gp_means = (double_Matrix*)means_col->AR();
    double_Matrix* gp_ss = (double_Matrix*)ss_col->AR();
    grand_mean = (float)taMath_double::vec_mean(gp_means);
    within_ss = (float)taMath_double::vec_sum(gp_ss);
  }
  else {
    float_Matrix* gp_means = (float_Matrix*)means_col->AR();
    float_Matrix* gp_ss = (float_Matrix*)ss_col->AR();
    grand_mean = taMath_float::vec_mean(gp_means);
    within_ss = taMath_float::vec_sum(gp_ss);
  }

  for(int i=0;i<n_gps;i++) {
    float mean = means_col->GetValAsFloat(i);
    float n = n_col->GetValAsFloat(i);
    float md = (mean - grand_mean);
    between_ss += n * md * md;
  }

  float between_ms = between_ss / between_df;
  float within_ms = within_ss / within_df;
  float f_ratio = between_ms / within_ms;
  float prob = taMath_float::Ftest_q(f_ratio, between_df, within_df);

  result_data->AddRows(3);
  result_data->SetVal("Between_mean_MS_df", 0, n_gps);
  result_data->SetVal(grand_mean, 1, n_gps);
  result_data->SetVal(between_ms, 2, n_gps);
  result_data->SetVal(between_df, 3, n_gps);
  result_data->SetVal("Within_mean_MS_df", 0, n_gps+1);
  result_data->SetVal(grand_mean, 1, n_gps+1);
  result_data->SetVal(within_ms, 2, n_gps+1);
  result_data->SetVal(within_df, 3, n_gps+1);
  result_data->SetVal("Ftest_p_F", 0, n_gps+2);
  result_data->SetVal(prob, 1, n_gps+2);
  result_data->SetVal(f_ratio, 2, n_gps+2);

  // now do all the pairwise t-tests
  for(int i=1;i<n_gps;i++) {
    String cn = conds_col->GetVal(i).toString();
    float_Data* dt = result_data->NewColFloat(cn + "_t");
    dt->desc = "Welch t-test statistic for pairwise comparison between these conditions";
    float_Data* dp = result_data->NewColFloat(cn + "_p");
    dp->desc = "probability for Welch t-test of pairwise comparison between these conditions";
  }
  for(int i=0;i<n_gps;i++) {
    float mn1 = means_col->GetValAsFloat(i);
    float ss1 = ss_col->GetValAsFloat(i);
    float n1 = n_col->GetValAsFloat(i);
    float var1 = ss1 / (n1 -1.0f);
    float varon1 = var1 / n1;
    for(int j=i+1;j<n_gps;j++) {
      float mn2 = means_col->GetValAsFloat(j);
      float ss2 = ss_col->GetValAsFloat(j);
      float n2 = n_col->GetValAsFloat(j);
      float var2 = ss2 / (n2 -1.0f);
      float varon2 = var2 / n2;

      // using Welch's t-test for unequal var, n etc -- more robust
      float md = fabsf(mn1 - mn2);     // mean diff
      float sum_varon = varon1 + varon2;
      float sd_est = sqrtf(sum_varon);
      float t_stat = md / sd_est;
      
      float welch_df = sum_varon * sum_varon / 
        ( ((varon1*varon1) / (n1 - 1.0f)) + ((varon2 * varon2) / (n2 - 1.0f)) );

      float welch_df_int = taMath_float::floor(welch_df);
      float welch_p = 1.0f - taMath_float::students_cum(t_stat, welch_df_int);

      result_data->SetVal(t_stat, n_result_cols + 2 * (j-1), i);
      result_data->SetVal(welch_p, n_result_cols + 2 * (j-1) + 1, i);
    }
  }
  
  return prob;
}

bool taDataAnal::AnovaNWay(DataTable* result_data, DataTable* src_data,
                           const String& cond_col_nms, const String& data_col_nm) {
  bool rval = false;
  String_Array cond_cols;
  cond_cols.Split(cond_col_nms, " ");
  if(cond_cols.size == 0) {
    taMisc::Error("AnovaNWay: no condition column names specified:", cond_col_nms);
    return rval;
  }
  if(cond_cols.size == 1) {
    return AnovaOneWay(result_data, src_data, cond_col_nms, data_col_nm);
  }

  // this is just for verifying src_data really
  DataCol* cda = GetNonMatrixDataCol(src_data, cond_cols[0]);
  if(!cda) return rval;
  DataCol* yda = GetNumDataCol(src_data, data_col_nm);
  if(!yda) return rval;

  GetDest(result_data, src_data, "AnovaNWay", true);

  int cidx;
  DataCol* r_src_col = result_data->FindMakeColName("source", cidx, VT_STRING);
  r_src_col->desc = "Source of variability";

  DataCol* r_df_col = result_data->FindMakeColName("df", cidx, VT_FLOAT);
  r_df_col->desc = "Degrees of freedom for source -- n-1";
  DataCol* r_ss_col = result_data->FindMakeColName("SS", cidx, VT_FLOAT);
  r_ss_col->desc = "Sum of squares due to source ";
  DataCol* r_ms_col = result_data->FindMakeColName("MS", cidx, VT_FLOAT);
  r_ms_col->desc = "Mean squares due to source: SS / df";

  DataCol* r_f_col = result_data->FindMakeColName("F_ratio", cidx, VT_FLOAT);
  r_f_col->desc = "F-statistic for source: MS_b / MS_w = ratio of MS for source (between factor) over overall MS within groups (error)";
  DataCol* r_p_col = result_data->FindMakeColName("p", cidx, VT_FLOAT);
  r_p_col->desc = "probability associated with given F statistic for this source -- how likely that differences are due strictly to chance";
  
  int n_conds = cond_cols.size;

  DataTable all_conds(false);
  all_conds.OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!

  DataGroupSpec all_gp_spec;
  all_gp_spec.append_agg_name = true;
  all_gp_spec.SetDataTable(src_data);
  for(int ci=0; ci < n_conds; ci++) {
    String cond = cond_cols[ci];
    DataGroupEl* cond_gp = (DataGroupEl*)all_gp_spec.AddColumn(cond_cols[ci], src_data);
    cond_gp->agg.op = Aggregate::GROUP;
  }
  DataGroupEl* mean_gp = (DataGroupEl*)all_gp_spec.AddColumn(data_col_nm, src_data);
  mean_gp->agg.op = Aggregate::MEAN;
  DataGroupEl* ss_gp = (DataGroupEl*)all_gp_spec.AddColumn(data_col_nm, src_data);
  ss_gp->agg.op = Aggregate::SS;
  DataGroupEl* n_gp = (DataGroupEl*)all_gp_spec.AddColumn(data_col_nm, src_data);
  n_gp->agg.op = Aggregate::N;

  taDataProc::Group(&all_conds, src_data, &all_gp_spec);

  float all_gps = all_conds.rows;
  float n_per_cell = (float)src_data->rows / all_gps;
  float within_df = all_gps * (n_per_cell - 1.0f);
  
  float grand_mean = 0.0f;
  float within_ss = 0.0f;
  DataCol* all_means_col = all_conds.GetColData(n_conds);
  DataCol* all_ss_col = all_conds.GetColData(n_conds+1);

  if(all_means_col->valType() == taBase::VT_DOUBLE) {
    double_Matrix* gp_means = (double_Matrix*)all_means_col->AR();
    double_Matrix* gp_ss = (double_Matrix*)all_ss_col->AR();
    grand_mean = (float)taMath_double::vec_mean(gp_means);
    within_ss = (float)taMath_double::vec_sum(gp_ss);
  }
  else {
    float_Matrix* gp_means = (float_Matrix*)all_means_col->AR();
    float_Matrix* gp_ss = (float_Matrix*)all_ss_col->AR();
    grand_mean = taMath_float::vec_mean(gp_means);
    within_ss = taMath_float::vec_sum(gp_ss);
  }

  float within_ms = within_ss / within_df;

  DataTable_Group cond_tabs;
  cond_tabs.SetSize(n_conds);

  // main effects
  DataGroupSpec gp_spec;
  gp_spec.append_agg_name = true;
  gp_spec.SetDataTable(src_data);
  DataGroupEl* cond_gp = (DataGroupEl*)gp_spec.AddColumn(cond_cols[0], src_data);
  cond_gp->agg.op = Aggregate::GROUP;
  mean_gp = (DataGroupEl*)gp_spec.AddColumn(data_col_nm, src_data);
  mean_gp->agg.op = Aggregate::MEAN;
  n_gp = (DataGroupEl*)gp_spec.AddColumn(data_col_nm, src_data);
  n_gp->agg.op = Aggregate::N;

  for(int ci=0; ci < n_conds; ci++) {
    String cond = cond_cols[ci];
    DataTable* res_tab = cond_tabs[ci];
    DataGroupEl* ncond_gp = (DataGroupEl*)gp_spec.ops[0];
    ncond_gp->SetColName(cond);
    
    taDataProc::Group(res_tab, src_data, &gp_spec);
    int n_gps = res_tab->rows;
    if(n_gps <= 1) {
      taMisc::Error("ANOVA requires 2 or more groups / levels per condition -- source data only has 1 or less for condition:", cond);
      return rval;
    }

    DataCol* means_col = res_tab->GetColData(1);
    DataCol* n_col = res_tab->GetColData(2);
    float between_ss = 0.0f;
    float between_df = n_gps - 1;

    for(int i=0;i<n_gps;i++) {
      float mean = means_col->GetValAsFloat(i);
      float n = n_col->GetValAsFloat(i);
      float md = (mean - grand_mean);
      between_ss += n * md * md;
    }

    float between_ms = between_ss / between_df;

    float f_ratio = between_ms / within_ms;
    float prob = taMath_float::Ftest_q(f_ratio, between_df, within_df);

    result_data->AddRows(1);
    r_src_col->SetVal(cond, -1);
    r_df_col->SetVal(between_df, -1);
    r_ss_col->SetVal(between_ss, -1);
    r_ms_col->SetVal(between_ms, -1);

    r_f_col->SetVal(f_ratio, -1);
    r_p_col->SetVal(prob, -1);
  }

  // two-way interactions
  DataGroupSpec gp_spec_2w;
  gp_spec_2w.append_agg_name = true;
  gp_spec_2w.SetDataTable(src_data);
  cond_gp = (DataGroupEl*)gp_spec_2w.AddColumn(cond_cols[0], src_data);
  cond_gp->agg.op = Aggregate::GROUP;
  cond_gp = (DataGroupEl*)gp_spec_2w.AddColumn(cond_cols[1], src_data);
  cond_gp->agg.op = Aggregate::GROUP;
  mean_gp = (DataGroupEl*)gp_spec_2w.AddColumn(data_col_nm, src_data);
  mean_gp->agg.op = Aggregate::MEAN;
  n_gp = (DataGroupEl*)gp_spec_2w.AddColumn(data_col_nm, src_data);
  n_gp->agg.op = Aggregate::N;

  int_Matrix tidx2w;
  tidx2w.SetGeom(2, n_conds, n_conds); // store table index for pairwise comparisons
  
  for(int c1=0; c1 < n_conds; c1++) {
    String cond1 = cond_cols[c1];
    int n1 = cond_tabs[c1]->rows;
    cond_gp = (DataGroupEl*)gp_spec_2w.ops[0];
    cond_gp->SetColName(cond1);
    for(int c2=c1+1; c2 < n_conds; c2++) {
      String cond2 = cond_cols[c2];
      int n2 = cond_tabs[c2]->rows;
      cond_gp = (DataGroupEl*)gp_spec_2w.ops[1];
      cond_gp->SetColName(cond2);

      tidx2w.FastEl2d(c1, c2) = cond_tabs.size;
      DataTable* res_tab = (DataTable*)cond_tabs.NewEl(1);
      taDataProc::Group(res_tab, src_data, &gp_spec_2w);

      DataCol* means_col = res_tab->GetColData(2);
      DataCol* n_col = res_tab->GetColData(3);
      float inter_ss = 0.0f;
      float inter_df = (n1-1) * (n2-1);

      int cell = 0;
      for(int j=0; j<n1; j++) { // j = level of factor1
        float mean1 = cond_tabs[c1]->GetValAsFloat(1, j);
        for(int i=0; i<n2; i++, cell++) { // i = level of factor2
          float mean2 = cond_tabs[c2]->GetValAsFloat(1, i);
          float mean = means_col->GetValAsFloat(cell);
          float n = n_col->GetValAsFloat(cell);
          
          float md = (mean - mean1 - mean2 + grand_mean);
          inter_ss += n * md * md;
        }
      }

      float inter_ms = inter_ss / inter_df;

      float f_ratio = inter_ms / within_ms;
      float prob = taMath_float::Ftest_q(f_ratio, inter_df, within_df);

      result_data->AddRows(1);
      r_src_col->SetVal(cond1 + "*" + cond2, -1);
      r_df_col->SetVal(inter_df, -1);
      r_ss_col->SetVal(inter_ss, -1);
      r_ms_col->SetVal(inter_ms, -1);

      r_f_col->SetVal(f_ratio, -1);
      r_p_col->SetVal(prob, -1);
    }
  }
  
  // three-way interactions
  if(n_conds >= 3) {
    DataGroupSpec gp_spec_3w;
    gp_spec_3w.append_agg_name = true;
    gp_spec_3w.SetDataTable(src_data);
    cond_gp = (DataGroupEl*)gp_spec_3w.AddColumn(cond_cols[0], src_data);
    cond_gp->agg.op = Aggregate::GROUP;
    cond_gp = (DataGroupEl*)gp_spec_3w.AddColumn(cond_cols[1], src_data);
    cond_gp->agg.op = Aggregate::GROUP;
    cond_gp = (DataGroupEl*)gp_spec_3w.AddColumn(cond_cols[2], src_data);
    cond_gp->agg.op = Aggregate::GROUP;
    mean_gp = (DataGroupEl*)gp_spec_3w.AddColumn(data_col_nm, src_data);
    mean_gp->agg.op = Aggregate::MEAN;
    n_gp = (DataGroupEl*)gp_spec_3w.AddColumn(data_col_nm, src_data);
    n_gp->agg.op = Aggregate::N;

    for(int c1=0; c1 < n_conds; c1++) {
      String cond1 = cond_cols[c1];
      int n1 = cond_tabs[c1]->rows;
      cond_gp = (DataGroupEl*)gp_spec_3w.ops[0];
      cond_gp->SetColName(cond1);
      for(int c2=c1+1; c2 < n_conds; c2++) {
        String cond2 = cond_cols[c2];
        int n2 = cond_tabs[c2]->rows;
        cond_gp = (DataGroupEl*)gp_spec_3w.ops[1];
        cond_gp->SetColName(cond2);
        for(int c3=c2+1; c3 < n_conds; c3++) {
          String cond3 = cond_cols[c3];
          int n3 = cond_tabs[c3]->rows;
          cond_gp = (DataGroupEl*)gp_spec_3w.ops[2];
          cond_gp->SetColName(cond3);

          DataTable* res_tab = (DataTable*)cond_tabs.NewEl(1);
          taDataProc::Group(res_tab, src_data, &gp_spec_3w);

          DataCol* means_col = res_tab->GetColData(3);
          DataCol* n_col = res_tab->GetColData(4);
          float inter_ss = 0.0f;
          float inter_df = (n1-1) * (n2-1) * (n3-1);

          int tidx12 = tidx2w.FastEl2d(c1, c2);
          int tidx13 = tidx2w.FastEl2d(c1, c3);
          int tidx23 = tidx2w.FastEl2d(c2, c3);
          
          int cell = 0;
          for(int j=0; j<n1; j++) { // j = level of factor1
            float mean1 = cond_tabs[c1]->GetValAsFloat(1, j);
            for(int i=0; i<n2; i++) { // i = level of factor2
              float mean2 = cond_tabs[c2]->GetValAsFloat(1, i);
              int cidx12 = j * n2 + i; // cell index
              float mean12 = cond_tabs[tidx12]->GetValAsFloat(2, cidx12);
              for(int k=0; k<n3; k++, cell++) { // k = level of factor3
                float mean3 = cond_tabs[c3]->GetValAsFloat(1, k);

                int cidx13 = j * n3 + k; // cell index
                float mean13 = cond_tabs[tidx13]->GetValAsFloat(2, cidx13);

                int cidx23 = i * n3 + k; // cell index
                float mean23 = cond_tabs[tidx23]->GetValAsFloat(2, cidx23);

                
                float mean = means_col->GetValAsFloat(cell);
                float n = n_col->GetValAsFloat(cell);

                // from http://www.skidmore.edu/~hfoley/Handouts/_baks/K.Ch19.notes.pdf.0002.81ae.bak
                // [ABC] - [AB] - [AC] - [BC] + [A] + [B] + [C] - [T]
                float md = (mean - mean12 - mean13 - mean23 + mean1 + mean2 + mean3 -
                            grand_mean);
                inter_ss += n * md * md;
              }
            }
          }

          float inter_ms = inter_ss / inter_df;

          float f_ratio = inter_ms / within_ms;
          float prob = taMath_float::Ftest_q(f_ratio, inter_df, within_df);

          result_data->AddRows(1);
          r_src_col->SetVal(cond1 + "*" + cond2 + "*" + cond3, -1);
          r_df_col->SetVal(inter_df, -1);
          r_ss_col->SetVal(inter_ss, -1);
          r_ms_col->SetVal(inter_ms, -1);

          r_f_col->SetVal(f_ratio, -1);
          r_p_col->SetVal(prob, -1);
        }
      }
    }
  }
  
  result_data->AddRows(1);
  r_src_col->SetVal("within_error", -1);
  r_df_col->SetVal(within_df, -1);
  r_ss_col->SetVal(within_ss, -1);
  r_ms_col->SetVal(within_ms, -1);

  return true;
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
  DataTable* work = new DataTable(false);
  work->OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!

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
  DataTable* sorted_src_data = new DataTable(false);
  sorted_src_data->OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!
  DataSortSpec* sort_spec = new DataSortSpec;
  sort_spec->OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!
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

    DataTable* select_tmp = new DataTable(false);
    select_tmp->OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!
    DataTable* one_cell_tmp = new DataTable(false);
    one_cell_tmp->OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!

    category = classes->FastEl1d(i);

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
      cmp_category = work->GetMatrixVal("TRAIN_C_" + category, 0, j).toString();
      if (cmp_category == category)
	work->SetMatrixVal(1, "TRAIN_T_" + category, 0, j);
      else
	work->SetMatrixVal(0, "TRAIN_T_" + category, 0, j);
    }

    for (int j=0; j < TEST_C_d1; j++) {
      cmp_category = work->GetMatrixVal("TEST_C_" + category, 0, j).toString();
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
  DataTable* confusion = new DataTable(false);
  confusion->OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!
  String col_nm = "TEST_YPRIME_" + classes->FastEl1d(0);
  String cm = "ConfusionMatrix";
  DataCol* TEST_YPRIME_0 = work->FindColName(col_nm);
  int d0 = TEST_YPRIME_0->GetCellGeom(0);
  ValType val_type = TEST_YPRIME_0->valType();
  taDataProc::AllDataToOne2DCell(confusion, work, val_type, "TEST_YPRIME_", cm);
  confusion->ChangeColTypeGeom(cm, val_type, 2, d0, classes->size);

  // Create target confusion matrix
  DataTable* confusion_target = new DataTable(false);
  confusion_target->OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!
  col_nm = "TEST_T_" + classes->FastEl1d(0);
  cm = "ConfusionMatrixTarget";
  DataCol* TEST_T_0 = work->FindColName(col_nm);
  d0 = TEST_T_0->GetCellGeom(0);
  val_type = TEST_T_0->valType();
  taDataProc::AllDataToOne2DCell(confusion_target, work, val_type, "TEST_T_", cm);
  confusion_target->ChangeColTypeGeom(cm, val_type, 2, d0, classes->size);

  // class lengths - how many stimuli are in each test class?
  DataTable* class_lengths = new DataTable(false);
  class_lengths->OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!
  Relation* rel = new Relation; taBase::Ref(rel);
  double_Matrix* class_mat;
  double length;
  rel->rel = (Relation::Relations)0;
  rel->val = 1;
  class_lengths->NewColMatrix(VT_INT, "CL", 1, classes->size);
  class_lengths->AddRows();
  int_Matrix* class_lengths_mat = (int_Matrix*)class_lengths->FindColName("CL")->GetValAsMatrix(0);

  for (int i=0; i < classes->size; i++) {
    class_mat = (double_Matrix*)work->FindColName("TEST_T_" + (String)classes->FastEl1d(i))->GetValAsMatrix(0);
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

    class_length = class_lengths_mat->FastEl1d(j);

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
  
  if (!(vec_signal->size > 0 && vec_noise->size > 0)) {
    taMisc::Error("signal and noise vectors must each have at least one element");
    return false;
  }

  double_Matrix* criterion_data = new double_Matrix;
  criterion_data->SetGeom(1, vec_signal->size + vec_noise->size);

  for (int i = 0; i < vec_signal->size; i++) {
    criterion_data->Set(vec_signal->FastEl1d(i), i);
    criterion_data->Set(vec_noise->FastEl1d(i), vec_noise->size + i);
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
    double criterion = criterion_data->FastEl1d(i) - .000000001;
    double tp = 0;
    double fp = 0;
    double fn = 0;
    double tn = 0;

    for (int j = 0; j < vec_signal->size; j++) {
      if (vec_signal->FastEl1d(j) > criterion)
	tp += 1;
      else
	fn += 1;
    }

    for (int j = 0; j < vec_noise->size; j++) {
      if (vec_noise->FastEl1d(j) > criterion)
	fp += 1;
      else
	tn += 1;
    }
    
    double recall = tp / (tp + fn);
    double precision = tp / (tp + fp);
    double fmeasure = 2.0*precision*recall/(precision+recall);
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
    dest_data->SetVal(fmeasure, "Fmeasure", i);
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
      x = vec_fpr->FastEl1d(i);
      y = coef_tmp->FastEl1d(0);

      for (int j = 1; j < degree; j++)
	y += coef_tmp->FastEl1d(j) * pow(x, j);
	
      y_fit->Set_Flat(y, i);
    }

    fun_string = "Y = ";
    for (int i = 0; i < degree; i++) fun_string += (String)coef_tmp->FastEl1d(i) + "*x^" + (String)i + " + ";
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
  dest_data->SetVal(tpr_smooth->FastEl1d(roc_min_idx), "MIN", 0);
  dest_data->SetVal(tpr_smooth->FastEl1d(roc_max_idx), "MAX", 0);
  taBase::unRefDone(tpr_smooth);

  return true;
}

////////////////////////////////////////////////////////////////////////////////////
//	distance matricies

bool taDataAnal::DistMatrix(float_Matrix* dist_mat, DataTable* src_data,
			    const String& data_col_nm,
			    taMath::DistMetric metric, bool norm, float tol,
			    bool incl_scalars) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::DistMatrix -- src_data is NULL or has no data -- must pass in this data");
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
	dist_mat->FastEl2d(br,ar) = cell_dist;
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
				 bool incl_scalars, bool name_labels, bool gp_names) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::DistMatrixTable -- src_data is NULL or has no data -- must pass in this data");
    return false;
  }
  if(src_data->rows == 0) {
    taMisc::Error("taDataAnal::DistMatrixTable -- src_data has no rows -- cannot compute distance matrix");
    return false;
  }
  float_Matrix dmat(false);
  bool rval = DistMatrix(&dmat, src_data, data_col_nm, metric, norm, tol, incl_scalars);
  if(!rval) return false;
  GetDest(dist_mat, src_data, "DistMatrix");
  dist_mat->StructUpdate(true);
  dist_mat->Reset();	// get rid of any existing cols
  if(name_col_nm.nonempty() && !name_labels) {
    DataCol* nmda = src_data->FindColName(name_col_nm, true); // errmsg
    if(nmda) {
      dist_mat->NewColString("Name");
      int n = dmat.dim(0);
      for(int i=0;i<n;i++) {
	String nm = nmda->GetValAsString(i);
        if(dist_mat->data.FindNameIdx(nm) >= 0) { // prevent repeats
          nm += String("_") + String(dist_mat->data.size);
        }
	dist_mat->NewColFloat(nm);
      }
      for(int i=0;i<n;i++) {
	String nm = nmda->GetValAsString(i);
	dist_mat->AddBlankRow();
	dist_mat->SetValAsString(nm, 0, -1);
	for(int j=0;j<n;j++) {
	  dist_mat->SetValAsFloat(dmat.FastEl2d(j,i), j+1, -1);
	}
      }
      if(view && taMisc::gui_active) {
        dist_mat->FindMakeGridView(NULL, false); // don't select view
      }
      dist_mat->StructUpdate(false);
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
  dist_mat->SetUserData("HEADER_OFF", true);
  dist_mat->AddBlankRow();
  dmda->SetValAsMatrix(&dmat, -1);
  if(view && taMisc::gui_active) {
    GridTableView* gtv = dist_mat->FindMakeGridView(NULL, false); // don't select view
    if(name_labels && !name_col_nm.empty()) {
      gtv->AnnoteClearAll();
      gtv->AddHorizLabels(*src_data, name_col_nm, gp_names, true, 0.0f, 1.0f);
      gtv->AddVertLabels(*src_data, name_col_nm, gp_names, true, false, 0.0f, 1.0f);
      // false = NOT bot zero
    }
  }
  dist_mat->StructUpdate(false);
  return true;
}

bool taDataAnal::CrossDistMatrix(float_Matrix* dist_mat,
				 DataTable* src_data_a, const String& data_col_nm_a,
				 DataTable* src_data_b, const String& data_col_nm_b,
				 taMath::DistMetric metric, bool norm, float tol,
				 bool incl_scalars) {
  if(!src_data_a || !src_data_b || src_data_a->rows == 0 || src_data_b->rows == 0) {
    taMisc::Error("taDataAnal::CrossDistMatrix -- src_data is NULL or has no data -- must pass in this data");
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
	dist_mat->FastEl2d(br,ar) = cell_dist;
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
  if(!src_data_a || !src_data_b || src_data_a->rows == 0 || src_data_b->rows == 0) {
    taMisc::Error("taDataAnal::CrossDistMatrixTable -- src_data is NULL or has no data -- must pass in this data");
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
	  dist_mat->SetValAsFloat(dmat.FastEl2d(j,i), j+1, -1);
	}
      }
      if(view && taMisc::gui_active) {
        dist_mat->FindMakeGridView(NULL, false); // don't select view
      }
      dist_mat->StructUpdate(false);
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
  if(view && taMisc::gui_active) {
    dist_mat->FindMakeGridView(NULL, false); // don't select view
  }
  dist_mat->StructUpdate(false);
  return true;
}

///////////////////////////////////////////////////////////////////
// correlation matricies

bool taDataAnal::CorrelMatrix(float_Matrix* correl_mat, DataTable* src_data,
			      const String& data_col_nm) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::CorrelMatrix -- src_data is NULL or has no data -- must pass in this data");
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
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::CorrelMatrixTable -- src_data is NULL or has no data -- must pass in this data");
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
  if(view && taMisc::gui_active) {
    correl_mat->FindMakeGridView(NULL, false); // don't select view
  }
  return true;
}

void taDataAnal::DistMatrixGroupSimilarity(float& avg_sim, float& max_sim, float& max_avg_sim, 
        float& max_max_sim, DataTable* src_data,
				 const String& data_col_nm, const String& name_col_nm,
        const String& avg_sim_col_nm, const String& max_sim_col_nm) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::DistMatrixGroupSimilarity -- src_data is NULL or has no data -- must pass in this data");
    return;
  }

  DataCol* nmda = src_data->FindColName(name_col_nm, true); // errmsg
  if(nmda == NULL) {
    return;
  }
    
  DataCol* asimda = src_data->FindColName(avg_sim_col_nm, false); // no errmsg
  DataCol* msimda = src_data->FindColName(max_sim_col_nm, false); // no errmsg
    
  float_Matrix dmat(false);
  bool rval = DistMatrix(&dmat, src_data, data_col_nm, taMath::INNER_PROD, true, 0.0f, false);
  if(!rval) return;

  avg_sim = 0.0f;
  max_sim = 0.0f;
  max_avg_sim = 0.0f;
  max_max_sim = 0.0f;
  String cur_cat;
  float cat_max_avg_sim = 0.0f;
  float cat_max_max_sim = 0.0f;
  int cat_n = 0;
  
  int n = dmat.dim(0);
  for(int i=0; i<n; i++) {
    String nm1 = nmda->GetValAsString(i);
    if(cur_cat != nm1) {
      if(cur_cat != "") {
        max_avg_sim += cat_max_avg_sim;
        max_max_sim += cat_max_max_sim;
        cat_max_avg_sim = 0.0f;
        cat_max_max_sim = 0.0f;
        cat_n++;
      }
      cur_cat = nm1;
    }

    float same_sum = 0.0f;
    int same_n = 0;
    float diff_sum = 0.0f;
    int diff_n = 0;
    float same_max = 0.0f;
    float diff_max = 0.0f;
    for(int j=0; j<n; j++) {
		   if(j==i) continue;
      String nm2 = nmda->GetValAsString(j);
      float val = dmat.FastEl2d(i,j);
      if(nm1 == nm2) {
        same_sum += val;
        same_max = fmaxf(same_max, val);
        same_n++;
      }
      else {
        diff_sum += val;
        diff_max = fmaxf(diff_max, val);
        diff_n++;
      }
    }
    
    if(same_n > 0) {
      same_sum /= (float)same_n;
    }
    if(diff_n > 0) {
      diff_sum /= (float)diff_n;
    }
    if(diff_sum < 1.0f) {
      float sv = 1.0f - ((1.0f - same_sum) / (1.0f - diff_sum));
      if(sv < -1.0f) { // avoid issues related to small number division artifacts
        sv = -1.0f;
      }
      if(asimda != NULL) {
        asimda->SetVal(sv, i);
      }
      avg_sim += sv;
      cat_max_avg_sim = fmaxf(cat_max_avg_sim, sv);
    }
    if(diff_max < 1.0f) {
      float sv = 1.0f - ((1.0f - same_max) / (1.0f - diff_max));
      if(sv < -1.0f) { // avoid issues related to small number division artifacts
        sv = -1.0f;
      }
      if(msimda != NULL) {
        msimda->SetVal(sv, i);
      }
      max_sim += sv;
      cat_max_max_sim = fmaxf(cat_max_max_sim, sv);
    }
  }
  max_avg_sim += cat_max_avg_sim;
  max_max_sim += cat_max_max_sim;
  cat_n++;
  
  max_avg_sim /= float(cat_n);
  max_max_sim /= float(cat_n);

  if(n > 0) {
    max_sim /= float(n);
    avg_sim /= float(n);
  }
}

void taDataAnal::DistMatrixGroupSimStats(DataTable* group_stats, DataTable* src_data,
                                          const String& data_col_nm, const String& name_col_nm,
                                          taMath::DistMetric metric, bool norm, float tol,
                                          bool incl_scalars) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::DistMatrixGroupSimilarity -- src_data is NULL or has no data -- must pass in this data");
    return;
  }
  GetDest(group_stats, src_data, "_GroupSimStats");

  DataCol* nmda = src_data->FindColName(name_col_nm, true); // errmsg
  if(nmda == NULL) {
    return;
  }
    
  src_data->Sort(name_col_nm, true); // must be
    
  group_stats->ResetData();
  int idx;
  DataCol* gpst_gp = group_stats->FindMakeColName("group", idx, VT_STRING, 0);
  DataCol* gpst_avg_in = group_stats->FindMakeColName("avg_within", idx, VT_FLOAT, 0);
  DataCol* gpst_avg_out = group_stats->FindMakeColName("avg_between", idx, VT_FLOAT, 0);
  DataCol* gpst_max_in = group_stats->FindMakeColName("max_within", idx, VT_FLOAT, 0);
  DataCol* gpst_max_out = group_stats->FindMakeColName("max_between", idx, VT_FLOAT, 0);

  float_Matrix dmat(false);
  bool rval = DistMatrix(&dmat, src_data, data_col_nm, metric, norm, tol, incl_scalars);
  if(!rval) return;

  float avg_in_sum = 0.0f;
  float avg_out_sum = 0.0f;
  float max_in_sum = 0.0f;
  float max_out_sum = 0.0f;
  int gp_n = 0;
  String prvnm;
  int n = dmat.dim(0);
  for(int i=0; i<=n; i++) {
    String nm1;
    if(i < n) {
      nm1 = nmda->GetValAsString(i);
    }
    if(nm1 != prvnm || i == n) {
      if(!prvnm.empty() && gp_n > 0) {
        group_stats->AddBlankRow();
        gpst_gp->SetVal(prvnm, -1);
        gpst_avg_in->SetVal(avg_in_sum / float(gp_n), -1);
        gpst_avg_out->SetVal(avg_out_sum / float(gp_n), -1);
        gpst_max_in->SetVal(max_in_sum / float(gp_n), -1);
        gpst_max_out->SetVal(max_out_sum / float(gp_n), -1);
        group_stats->WriteClose();
      }
      if(i == n) {
        break;
      }
      avg_in_sum = 0.0f;
      avg_out_sum = 0.0f;
      max_in_sum = 0.0f;
      max_out_sum = 0.0f;
      gp_n = 0;
    }
    
    float same_sum = 0.0f;
    float same_max = 0.0f;
    int same_n = 0;
    float diff_sum = 0.0f;
    float diff_max = 0.0f;
    int diff_n = 0;
    for(int j=0; j<n; j++) {
      if(j==i) continue;
      String nm2 = nmda->GetValAsString(j);
      float dv = dmat.FastEl2d(i,j);
      if(nm1 == nm2) {
        same_sum += dv;
        same_max = fmaxf(same_max, dv);
        same_n++;
      }
      else {
        diff_sum += dv;
        diff_max = fmaxf(diff_max, dv);
        diff_n++;
      }
    }
    if(same_n > 0) {
      avg_in_sum += same_sum / float(same_n);
      max_in_sum += same_max;
    }
    if(diff_n > 0) {
      avg_out_sum += diff_sum / float(diff_n);
      max_out_sum += diff_max;
    }
    gp_n++;
    prvnm = nm1;
  }
}


///////////////////////////////////////////////////////////////////
// standard multidimensional data analysis methods

bool taDataAnal::Cluster(DataTable* clust_data, bool view, DataTable* src_data,
			 const String& data_col_nm, const String& name_col_nm,
			 taMath::DistMetric metric, bool norm, float tol) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::Cluster -- src_data is NULL or has no data -- must pass in this data");
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

  if(view && taMisc::gui_active) {
    clust_data->FindMakeGraphView(NULL, false); // don't select view
  }
  clust_data->StructUpdate(false);
  return true;
}

bool taDataAnal::PCAEigens(float_Matrix* eigen_vals, float_Matrix* eigen_vecs,
			   DataTable* src_data, const String& data_col_nm) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::PCAEigens -- src_data is NULL or has no data -- must pass in this data");
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
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::PCAEigenTable -- src_data is NULL or has no data -- must pass in this data");
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
      dmda->SetValAsFloatM(eigen_vecs.FastEl2d(i,j), -1, j);
    }
  }
  if(view && taMisc::gui_active) {
    pca_data->FindMakeGridView(NULL, false); // don't select view
  }
  pca_data->StructUpdate(false);
  return true;
}

bool taDataAnal::PCA2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			   const String& data_col_nm, const String& name_col_nm, 
			   int x_axis_c, int y_axis_c) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::PCA2dPrjn -- src_data is NULL or has no data -- must pass in this data");
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
	<< eigen_vals.FastEl1d(x_axis_c);
    taMisc::Info(msg);
  }

  float_Matrix yevec(false);		// x eigen vector
  taMath_float::mat_col(&yevec, &eigen_vecs, y_axis_c); // eigen vector = column 
  {
    String msg;
    msg << "Component no: " << y_axis_c << " has eigenvalue: "
	<< eigen_vals.FastEl1d(y_axis_c);
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
    xda->SetValAsFloat(xprjn.FastEl1d(i), -1);
    yda->SetValAsFloat(yprjn.FastEl1d(i), -1);
  }

  if(view && taMisc::gui_active) {
    prjn_data->FindMakeGraphView(NULL, false); // don't select view
  }
  return true;
}  

bool taDataAnal::MDS2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			   const String& data_col_nm, const String& name_col_nm, 
			   int x_axis_c, int y_axis_c,
			   taMath::DistMetric metric, bool norm, float tol)
{
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::MDS2dPrjn -- src_data is NULL or has no data -- must pass in this data");
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
    xda->SetValAsFloat(xy_coords.FastEl2d(0, i), -1);
    yda->SetValAsFloat(xy_coords.FastEl2d(1, i), -1);
  }

  if(view && taMisc::gui_active) {
    prjn_data->FindMakeGraphView(NULL, false); // don't select view
  }
  return true;
}

bool taDataAnal::RowPat2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			     const String& data_col_nm, const String& name_col_nm,
			     int x_row, int y_row,
			     taMath::DistMetric metric, bool norm, float tol)
{
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::RowPat2dPrjn -- src_data is NULL or has no data -- must pass in this data");
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
    xda->SetValAsFloat(xprjn.FastEl1d(i), -1);
    yda->SetValAsFloat(yprjn.FastEl1d(i), -1);
  }

  if(view && taMisc::gui_active) {
    prjn_data->FindMakeGraphView(NULL, false); // don't select view
  }
  return true;
}

bool taDataAnal::TimeAvg(DataTable* avg_data, bool view, DataTable* src_data,
			 float avg_dt, bool float_only)
{
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::TimeAvg -- src_data is NULL or has no data -- must pass in this data");
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

  if(view && taMisc::gui_active) {
    avg_data->FindMakeGraphView(NULL, false); // don't select view
  }
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
  if(view && taMisc::gui_active) {
    smooth_data->FindMakeGraphView(NULL, false); // don't select view
  }
  smooth_data->StructUpdate(false);
  return true;
}

bool taDataAnal::SmoothUniform(DataTable* smooth_data, bool view, DataTable* src_data,
			       int kern_half_wd, bool neg_tail, bool pos_tail,
			       bool keep_edges, bool float_only)
{
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::SmoothUniform -- src_data is NULL or has no data -- must pass in this data");
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
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::SmoothGauss -- src_data is NULL or has no data -- must pass in this data");
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
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::SmoothExp -- src_data is NULL or has no data -- must pass in this data");
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
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::SmoothPow -- src_data is NULL or has no data -- must pass in this data");
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
  dupl.OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!
  dupl.CopyFrom(data);
  dupl.SortColName(z_axis_col, true, x_axis_col, true);
  taDataProc::AppendRows(data, &dupl);
  dupl.Reset();
  return true;
}

bool taDataAnal::Histogram(DataTable* hist_data, DataTable* src_data,
                           const String& src_col, float bin_size,
                           float min_val, float max_val, bool view) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::Histogram -- src_data is NULL or has no data -- must pass in this data");
    return false;
  }
  
  GetDest(hist_data, src_data, "Histogram");

  DataCol* da = GetNumDataCol(src_data, src_col);
  if(!da) return false;

  hist_data->StructUpdate(true);
  int idx;

  if(da->valType() == VT_FLOAT) {
    DataCol* binda = hist_data->FindMakeColName(da->name, idx, VT_FLOAT);
    DataCol* hda = hist_data->FindMakeColName("count", idx, VT_FLOAT);
    float_Matrix bin_tmp(false);
    taMath_float::vec_histogram_bins(&bin_tmp, (float_Matrix*)da->AR(),  
                                bin_size, min_val, max_val);
    hist_data->EnforceRows(bin_tmp.size);
    binda->AR()->CopyFrom(&bin_tmp);
    taMath_float::vec_histogram((float_Matrix*)hda->AR(), (float_Matrix*)da->AR(),  
                                bin_size, min_val, max_val);
  }
  else if(da->valType() == VT_DOUBLE) {
    DataCol* binda = hist_data->FindMakeColName(da->name, idx, VT_DOUBLE);
    DataCol* hda = hist_data->FindMakeColName("count", idx, VT_DOUBLE);
    double_Matrix bin_tmp(false);
    taMath_double::vec_histogram_bins(&bin_tmp, (double_Matrix*)da->AR(),  
                                bin_size, min_val, max_val);
    hist_data->EnforceRows(bin_tmp.size);
    binda->AR()->CopyFrom(&bin_tmp);
    taMath_double::vec_histogram((double_Matrix*)hda->AR(), (double_Matrix*)da->AR(),  
                                 bin_size, min_val, max_val);
  }
  else if(da->valType() == VT_INT) { // convert
    float_Matrix float_tmp(false);
    int_Matrix* mat = (int_Matrix*)da->AR();
    taMath_float::vec_fm_ints(&float_tmp, mat);
    DataCol* binda = hist_data->FindMakeColName(da->name, idx, VT_FLOAT);
    DataCol* hda = hist_data->FindMakeColName("count", idx, VT_FLOAT);
    float_Matrix bin_tmp(false);
    taMath_float::vec_histogram_bins(&bin_tmp, &float_tmp,  
                                bin_size, min_val, max_val);
    hist_data->EnforceRows(bin_tmp.size);
    binda->AR()->CopyFrom(&bin_tmp);
    taMath_float::vec_histogram((float_Matrix*)hda->AR(), &float_tmp,  
                                bin_size, min_val, max_val);
  }

  hist_data->SetUserData("GRAPH_TYPE", "BAR");
  
  if(view && taMisc::gui_active) {
    hist_data->FindMakeGraphView(NULL, false); // don't select view
  }
  hist_data->StructUpdate(false);
  
  return true;
}

bool taDataAnal::MatrixCellFreq
(DataTable* freq_data, DataTable* src_data, const String& src_col,
 bool thresh, float thr_val, bool mean, bool view) {
  if(!src_data || src_data->rows == 0) {
    taMisc::Error("taDataAnal::MatrixCellFreq -- src_data is NULL or has no data -- must pass in this data");
    return false;
  }
  
  GetDest(freq_data, src_data, "MatrixCellFreq", false); // false = don't reset

  DataCol* da = GetMatrixDataCol(src_data, src_col);
  if(!da) return false;

  freq_data->StructUpdate(true);
  int idx;

  DataCol* freqda = freq_data->FindMakeColMatrixN
    (da->name, VT_FLOAT, da->cell_geom, idx);

  freq_data->EnforceRows(1);
  freqda->InitVals(0.0f);
  float_MatrixPtr freq_mtx;
  freq_mtx = (float_Matrix*)freqda->GetValAsMatrix(0);

  float_Matrix tmp;
  
  for(int ri=0; ri < src_data->rows; ri++) {
    if(da->valType() == VT_FLOAT) {
      float_MatrixPtr mtx;
      mtx = (float_Matrix*)da->GetValAsMatrix(ri);
      if(thresh) {
        tmp.CopyFrom(mtx);
        taMath_float::vec_threshold(&tmp, thr_val);
        *(freq_mtx.ptr()) += tmp;
      }
      else {
        *(freq_mtx.ptr()) += *(mtx.ptr());
      }
    }
    else if(da->valType() == VT_DOUBLE) {
      double_MatrixPtr mtx;
      mtx = (double_Matrix*)da->GetValAsMatrix(ri);
      tmp.CopyFrom(mtx);
      if(thresh) {
        taMath_float::vec_threshold(&tmp, thr_val);
      }
      *(freq_mtx.ptr()) += tmp;
    }
    else if(da->valType() == VT_INT) { // convert
      int_MatrixPtr mtx;
      mtx = (int_Matrix*)da->GetValAsMatrix(ri);
      tmp.CopyFrom(mtx);
      if(thresh) {
        taMath_float::vec_threshold(&tmp, thr_val);
      }
      *(freq_mtx.ptr()) += tmp;
    }
  }

  if(mean) {
    *(freq_mtx.ptr()) /= (float)src_data->rows;
  }
  
  if(view) {
    freq_data->SetUserData("N_ROWS", 1);
    // freq_data->SetUserData("AUTO_SCALE", true);
  }
  if(view && taMisc::gui_active) {
    freq_data->FindMakeGridView(NULL, false); // don't select view
  }
  freq_data->StructUpdate(false);
  
  return true;
}

void taDataAnal::GetArgCompletionList(const String& method, const String& arg, const String_Array& arg_values, taBase* arg_obj, const String& cur_txt, Completions& completions) {
  if (arg_obj) {
    if (arg_obj->InheritsFrom(&TA_DataTable)) {
      DataTable* table = (DataTable*)arg_obj;
      FOREACH_ELEM_IN_LIST(DataCol, col, table->data) {
        completions.object_completions.Link(col);
      }
    }
  }
}

String taDataAnal::GetArgForCompletion(const String& method, const String& arg) {
  if (method == "Histogram" || method == "MatrixCellFreq") {
    if (arg == "src_col") {
      return "src_data";
    }
  }
  else if (method == "Matrix3DGraph") {
    if (arg == "x_axis_col" || arg == "z_axis_col") {
      return "data";
    }
  }
  else if (method == "PCAEigens" || method == "PCAEigenTable") {
    if (arg == "data_col_nm") {
      return "src_data";
    }
  }
  else if (method == "Cluster" || method == "PCA2dPrjn" || method == "MDS2dPrjn" || method == "RowPat2dPrjn") {
    if (arg == "name_col_nm" || arg == "data_col_nm") {
      return "src_data";
    }
  }
  else if (method == "RegressLinear") {
    if (arg == "x_data_col_nm" || arg == "y_data_col_nm") {
      return "src_data";
    }
  }
  else if (method == "AnovaOneWay") {
    if (arg == "cond_col_nm" || arg == "data_col_nm") {
      return "src_data";
    }
  }
  else if (method == "AnovaNWay") {
    if (arg == "cond_col_nms" || arg == "data_col_nm") {
      return "src_data";
    }
  }
  else if (method == "DistMatrixTable") {
    if (arg == "data_col_nm" || arg == "name_col_nm") {
      return "src_data";
    }
  }
  else if (method == "CrossDistMatrixTable") {
    if (arg == "data_col_nm_a" || arg == "name_col_nm_a") {
      return "src_data_a";
    }
    if (arg == "data_col_nm_b" || arg == "name_col_nm_b") {
      return "src_data_b";
    }
  }
  else if (method == "DistMatrixTable") {
    if (arg == "data_col_nm" || arg == "name_col_nm") {
      return "src_data";
    }
  }
  else if (method == "CorrelMatrixTable") {
    if (arg == "data_col_nm") {
      return "src_data";
    }
  }


  return _nilString;
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
