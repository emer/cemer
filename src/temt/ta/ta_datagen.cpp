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

#include "ta_datagen.h"
#include "css_machine.h"
#include "ta_geometry.h"
#include "ta_project.h"		// for debugging

#include <QDir>

void SubMatrixOpEl::Initialize() {
}

String SubMatrixOpEl::GetDisplayName() const {
  return col_name;
}

void SubMatrixOpEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(col_lookup) {
    CheckError(!col_lookup->is_matrix, quiet, rval, "column must be a matrix column");
  }
}

void SubMatrixOpSpec::Initialize() {
  ops.SetBaseType(&TA_SubMatrixOpEl);
}

void SubMatrixOpSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if((bool)sub_matrix_table) {
    SetDataTable(sub_matrix_table);
  }
}

///////////////////////////////////////////
//		taDataGen

bool taDataGen::CheckDims(float_Matrix* mat, int dims) {
  if(!mat) return false;
  if(mat->dims() != dims) {
    taMisc::Warning("taDataGen: matrix:", mat->name, "path:", mat->GetPath(),
		    "is not of dimension:", String(dims), "it is:", String(mat->dims()));
    return false;
  }
  return true;
}

DataCol* taDataGen::GetFloatMatrixDataCol(const DataTable* src_data, const String& data_col_nm) {
  if(!src_data) return NULL;
  DataCol* da = src_data->FindColName(data_col_nm, true); // err msg
  if(!da)
    return NULL;
  if(!da->is_matrix) {
    taMisc::Error("taDataGen: column named:", data_col_nm,
		  "is not a matrix in data table:", src_data->name);
    return NULL;
  }
  if(da->valType() != VT_FLOAT) {
    taMisc::Error("taDataGen: column named:", data_col_nm,
		  "is not of type float in data table:", src_data->name);
    return NULL;
  }
  return da;
}

DataCol* taDataGen::GetFloatDataCol(const DataTable* src_data, const String& data_col_nm) {
  if(!src_data) return NULL;
  DataCol* da = src_data->FindColName(data_col_nm, true); // err msg
  if(!da)
    return NULL;
  if(da->is_matrix) {
    taMisc::Error("taDataGen: column named:", data_col_nm,
		  "is a matrix in data table:", src_data->name, "must be a scalar!");
    return NULL;
  }
  if(da->valType() != VT_FLOAT) {
    taMisc::Error("taDataGen: column named:", data_col_nm,
		  "is not of type float in data table:", src_data->name);
    return NULL;
  }
  return da;
}

bool taDataGen::GetDest(DataTable*& dest, const DataTable* src, const String& suffix) {
  if(dest) return false;
  taProject* proj = GET_OWNER(src, taProject);
  dest = proj->GetNewInputDataTable(src->name + "_" + suffix, true);
  return true;
}


///////////////////////////////////////////////////////////////////
// basic operations

bool taDataGen::Clear(DataTable* data, const String& col_nm, float val) {
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataCol* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!Clear(data, da->name, val))
	  rval = false;
      }
    }
    return rval;
  }
  DataCol* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    mat->InitVals(val);
    taBase::unRefDone(mat);
  }
  data->DataUpdate(false);
  return true;
}

bool taDataGen::SimpleMath(DataTable* data, const String& col_nm, const SimpleMathSpec& math) {
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataCol* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!SimpleMath(data, da->name, math))
	  rval = false;
      }
    }
    return rval;
  }
  DataCol* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    taMath_float::vec_simple_math(mat, math);
    taBase::unRefDone(mat);
  }
  data->DataUpdate(false);
  return true;
}

///////////////////////////////////////////////////////////////////
// list opterations

bool taDataGen::CrossLists(DataTable* dest, const DataTable* data_list_1,
			   const DataTable* data_list_2, const DataTable* data_list_3,
			   const DataTable* data_list_4, const DataTable* data_list_5) {
  if(!data_list_1 || data_list_1->rows == 0) {
    taMisc::Error("taDataGen:CrossLists data_list_1 is NULL or has no rows");
    return false;
  }
  if(!data_list_2 || data_list_2->rows == 0) {
    taMisc::Error("taDataGen:CrossLists data_list_2 is NULL or has no rows");
    return false;
  }
  
  const DataTable* dats[5];
  dats[0] = data_list_1;
  dats[1] = data_list_2;

  int n_lists = 2;
  if(data_list_3 && data_list_3->rows > 0) dats[n_lists++] = data_list_3;
  if(data_list_4 && data_list_3->rows > 0) dats[n_lists++] = data_list_4;
  if(data_list_5 && data_list_3->rows > 0) dats[n_lists++] = data_list_5;

  String dst_nm = String("x_") + data_list_2->name;
  for(int l=2;l<n_lists;l++)
    dst_nm += String("_x_") + dats[l]->name;

  GetDest(dest, data_list_1, dst_nm);
  dest->StructUpdate(true);
  dest->Reset();

  MatrixGeom mg;
  for(int l=0; l<n_lists; l++) {
    mg.AddDim(dats[l]->rows);	// number of rows are like geometries of each dim
    for(int i=0; i < dats[l]->data.size; i++) {
      DataCol* sda = dats[l]->data.FastEl(i);
      DataCol* nda = (DataCol*)sda->MakeToken();
      dest->data.Add(nda);
      nda->Copy_NoData(*sda);
    }
  }

  dest->UniqueColNames();	// make them unique!

  int max_n = mg.Product();

  MatrixGeom rows;
  for(int ri=0; ri<max_n; ri++) {
    mg.DimsFmIndex(ri, rows);	// get dim vals from overall index
    dest->AddBlankRow();
    int didx = 0;
    for(int l=0; l<n_lists; l++) {
      for(int i=0; i < dats[l]->data.size; i++, didx++) {
	DataCol* sda = dats[l]->data.FastEl(i);
	DataCol* nda = dest->data.FastEl(didx);
	nda->CopyFromRow(-1, *sda, rows[l]);
      }
    }
  }
  dest->StructUpdate(false);
  return true;
}

bool taDataGen::CombineFrequencies(DataTable* dest, const DataTable* data_in,
				   const String& freq_col_nm, CombineOp opr,
				   bool renorm_freqs) {
  if(!data_in || data_in->rows == 0) {
    taMisc::Error("taDataGen:CombineFrequencies data_list_in is NULL or has no rows");
    return false;
  }
  
  GetDest(dest, data_in, "CombFreq");
  dest->StructUpdate(true);
  dest->Reset();

  int_Array freq_cols;		// cols having frequency info
  int_Array data_cols;		// all other data, incl 1st freq (1-to-1 with dest cols)
  int dest_freq_col = -1;
  
  for(int i=0; i < data_in->data.size; i++) {
    DataCol* sda = data_in->data.FastEl(i);
    bool add = false;
    if(sda->name.contains(freq_col_nm) && !sda->is_matrix) {
      freq_cols.Add(i);
      if(freq_cols.size == 1) {
	add = true;	// add the first one
	dest_freq_col = data_cols.size;
      }
    }
    else {
      add = true;
    }
    if(add) {
      data_cols.Add(i);		// incl the one freq, so has 1-to-1 with dest cols
      DataCol* nda = (DataCol*)sda->MakeToken();
      dest->data.Add(nda);
      nda->Copy_NoData(*sda);
    }
  }

  if(freq_cols.size < 2) {
    taMisc::Error("taDataGen:CombineFrequencies could not find two or more scalar columns containing name:",
		  freq_col_nm);
    return false;
  }

  DataCol* freq_da = dest->data.FastEl(dest_freq_col);

  for(int ri=0;ri<data_in->rows; ri++) {
    dest->AddBlankRow();
    float cmb_val = data_in->data.FastEl(freq_cols[0])->GetValAsFloat(ri);
    for(int i=1;i<freq_cols.size; i++) {
      float col_val = data_in->data.FastEl(freq_cols[i])->GetValAsFloat(ri);
      if(opr == MULTIPLY)
	cmb_val *= col_val;
      else
	cmb_val += col_val;
    }
    freq_da->SetValAsFloat(cmb_val, ri);
    for(int i=0;i<data_cols.size; i++) {
      if(data_cols[i] != freq_cols[0]) {
	DataCol* sda = data_in->data.FastEl(data_cols[i]);
	DataCol* nda = dest->data.FastEl(i);
	nda->CopyFromRow(ri, *sda, ri);
      }
    }
  }

  if(renorm_freqs) {
    taMath_float::vec_norm_sum((float_Matrix*)freq_da->AR(), 1.0f, 0.0f);
  }

  dest->StructUpdate(false);
  return true;
}

bool taDataGen::ReplicateByFrequency(DataTable* dest, const DataTable* data_in,
				     int total_number, const String& freq_col_nm,
				     bool renorm_freqs) {
  if(!data_in || data_in->rows == 0) {
    taMisc::Error("taDataGen:ReplicateByFrequency data_list_in is NULL or has no rows");
    return false;
  }

  DataCol* freq_col = GetFloatDataCol(data_in, freq_col_nm);
  if(!freq_col) {
    taMisc::Error("taDataGen:ReplicateByFrequency column of name:", freq_col_nm,
		  "not found in data table:", data_in->name);
    return false;
  }
  
  GetDest(dest, data_in, "ReplFreq");
  dest->StructUpdate(true);
  dest->Reset();

  dest->Copy_NoData(*data_in);		// give it same structure

  float_Matrix norm_freq(false);
  float_Matrix* freqs = (float_Matrix*)freq_col->AR();
  if(renorm_freqs) {
    norm_freq = *freqs;		// copy the data!
    taMath_float::vec_norm_sum(&norm_freq, 1.0f, 0.0f);
    freqs = &norm_freq;
  }

  for(int i=0;i<data_in->rows;i++) {
    int n_repl = (int)(.5f + (float)total_number * freqs->FastEl_Flat(i));
    for(int k=0;k<n_repl;k++) {
      dest->AddBlankRow();
      dest->CopyFromRow(-1, *data_in, i);
    }
  }

  dest->StructUpdate(false);
  return true;
}

bool taDataGen::SampleByFrequency(DataTable* dest, const DataTable* data_in,
				  int n_samples, const String& freq_col_nm,
				  bool renorm_freqs) {
  if(!data_in || data_in->rows == 0) {
    taMisc::Error("taDataGen:SampleByFrequency data_list_in is NULL or has no rows");
    return false;
  }

  DataCol* freq_col = GetFloatDataCol(data_in, freq_col_nm);
  if(!freq_col) {
    taMisc::Error("taDataGen:SampleByFrequency column of name:", freq_col_nm,
		  "not found in data table:", data_in->name);
    return false;
  }
  
  GetDest(dest, data_in, "SampleFreq");
  dest->StructUpdate(true);
  dest->Reset();

  dest->Copy_NoData(*data_in);		// give it same structure

  float_Matrix norm_freq(false);
  float_Matrix* freqs = (float_Matrix*)freq_col->AR();
  if(renorm_freqs) {
    norm_freq = *freqs;		// copy the data!
    taMath_float::vec_norm_sum(&norm_freq, 1.0f, 0.0f);
    freqs = &norm_freq;
  }

  for(int i=0;i<data_in->rows;i++) {
    for(int j=0;j<n_samples;j++) {
      if(Random::BoolProb(freqs->FastEl_Flat(i))) {
	dest->AddBlankRow();
	dest->CopyFromRow(-1, *data_in, i);
      }
    }
  }

  dest->StructUpdate(false);
  return true;
}

bool taDataGen::NsByFrequency(DataTable* dest, const DataTable* data_in,
				     int total_number, const String& freq_col_nm,
				     bool renorm_freqs) {
  if(!data_in || data_in->rows == 0) {
    taMisc::Error("taDataGen:NsByFrequency data_list_in is NULL or has no rows");
    return false;
  }

  DataCol* freq_col = GetFloatDataCol(data_in, freq_col_nm);
  if(!freq_col) {
    taMisc::Error("taDataGen:NsByFrequency column of name:", freq_col_nm,
		  "not found in data table:", data_in->name);
    return false;
  }
  
  GetDest(dest, data_in, "ReplFreqNs");
  dest->StructUpdate(true);
  dest->Reset();

  dest->Copy_NoData(*data_in);		// give it same structure
  DataCol* ns = dest->NewCol(DataCol::VT_INT, "n_repls");

  float_Matrix norm_freq(false);
  float_Matrix* freqs = (float_Matrix*)freq_col->AR();
  if(renorm_freqs) {
    norm_freq = *freqs;		// copy the data!
    taMath_float::vec_norm_sum(&norm_freq, 1.0f, 0.0f);
    freqs = &norm_freq;
  }

  for(int i=0;i<data_in->rows;i++) {
    int n_repl = (int)(.5f + (float)total_number * freqs->FastEl_Flat(i));
    dest->AddBlankRow();
    dest->CopyFromRow(-1, *data_in, i);
    ns->SetValAsInt(n_repl, -1);
  }

  dest->StructUpdate(false);
  return true;
}

int taDataGen::ProbSelectRow(DataTable* data_table, const String& key_col,
			     const String& key_val, const String& p_col) {
  double psum = 0.0;
  double rndval = Random::ZeroOne();
  int row_no = data_table->FindValColName(key_val, key_col, 0);
  while (row_no >= 0 && row_no < data_table->rows) {
    double pval = data_table->GetValColName(p_col, row_no).toDouble();
    psum += pval;
    if(rndval < psum) {		// we're it
      return row_no;
    }
    row_no = data_table->FindValColName(key_val, key_col, row_no+1);
  }
  // failed
  return -1;
}

int taDataGen::ProbSelectColNo(DataTable* data_table, int row_no,
			       const String& colval1, const String& colp1,
			       const String& colval2, const String& colp2,
			       const String& colval3, const String& colp3,
			       const String& colval4, const String& colp4,
			       const String& colval5, const String& colp5,
			       const String& colval6, const String& colp6,
			       const String& colval7, const String& colp7,
			       const String& colval8, const String& colp8) {
  double psum = 0.0;
  double rndval = Random::ZeroOne();
  int col_no = -1;
  if(colp1.nonempty()) {
    psum += data_table->GetValColName(colp1, row_no).toDouble();
    if(rndval < psum) {
      col_no = data_table->FindColNameIdx(colval1, true);
      return col_no;
    }
  }
  if(colp2.nonempty()) {
    psum += data_table->GetValColName(colp2, row_no).toDouble();
    if(rndval < psum) {
      col_no = data_table->FindColNameIdx(colval2, true);
      return col_no;
    }
  }
  if(colp3.nonempty()) {
    psum += data_table->GetValColName(colp3, row_no).toDouble();
    if(rndval < psum) {
      col_no = data_table->FindColNameIdx(colval3, true);
      return col_no;
    }
  }
  if(colp4.nonempty()) {
    psum += data_table->GetValColName(colp4, row_no).toDouble();
    if(rndval < psum) {
      col_no = data_table->FindColNameIdx(colval4, true);
      return col_no;
    }
  }
  if(colp5.nonempty()) {
    psum += data_table->GetValColName(colp5, row_no).toDouble();
    if(rndval < psum) {
      col_no = data_table->FindColNameIdx(colval5, true);
      return col_no;
    }
  }
  if(colp6.nonempty()) {
    psum += data_table->GetValColName(colp6, row_no).toDouble();
    if(rndval < psum) {
      col_no = data_table->FindColNameIdx(colval6, true);
      return col_no;
    }
  }
  if(colp7.nonempty()) {
    psum += data_table->GetValColName(colp7, row_no).toDouble();
    if(rndval < psum) {
      col_no = data_table->FindColNameIdx(colval7, true);
      return col_no;
    }
  }
  if(colp8.nonempty()) {
    psum += data_table->GetValColName(colp8, row_no).toDouble();
    if(rndval < psum) {
      col_no = data_table->FindColNameIdx(colval8, true);
      return col_no;
    }
  }
  // nothing selected
  return -1;
}

Variant taDataGen::ProbSelectColVal(DataTable* data_table, int row_no,
				    const String& colval1, const String& colp1,
				    const String& colval2, const String& colp2,
				    const String& colval3, const String& colp3,
				    const String& colval4, const String& colp4,
				    const String& colval5, const String& colp5,
				    const String& colval6, const String& colp6,
				    const String& colval7, const String& colp7,
				    const String& colval8, const String& colp8) {
  int col_no = ProbSelectColNo(data_table, row_no, colval1, colp1, colval2, colp2,
			       colval3, colp3, colval4, colp4, colval5, colp5,
			       colval6, colp6, colval7, colp7, colval8, colp8);
  if(col_no >= 0) {
    return data_table->GetVal(col_no, row_no);
  }
  return _nilVariant;
}

///////////////////////////////////////////////////////////////////
// gen sorted permutations

bool taDataGen::SortedPermutations(DataTable* dest,int n) {
  if(!dest) return false;
  int i;
  int *v = new int[n];
  dest->StructUpdate(true);
  dest->Reset();
  dest->NewColMatrix(taBase::VT_INT,"p",2,n,1);
  for (i=0;i<n;i++)
    v[i]=i+1;
  GSP_permute(dest,v,0,n);
  delete [] v;
  dest->StructUpdate(false);
  return true;
}

void taDataGen::GSP_permute(DataTable* p,int *v,int start,int n) {
  GSP_write(p,v,n);
  if(start<n){
    int i,j;
    for (i=n-2;i>=start;i--) {
      for (j=i+1;j<n;j++) {
        GSP_swap(v,i,j);
        GSP_permute(p,v,i+1,n);
      }
      GSP_rotateLeft(v,i,n);
    }
  }
}

void taDataGen::GSP_write(DataTable* p,int* v,int size) {
  int i, rows;
  p->AddRows();
  if (v!=0){
    for (i=0;i<size;i++)
      p->SetMatrixVal(v[i],0,p->rows-1,i,0);
  }
}

void taDataGen::GSP_swap(int* v,int i,int j) {
  int t;
  t=v[i];
  v[i]=v[j];
  v[j]=t;
}

void taDataGen::GSP_rotateLeft(int* v,int start,int n) {
  int tmp = v[start];
  for (int i=start;i<n-1;i++)
    v[i]=v[i+1];
  v[n-1]=tmp;
}

///////////////////////////////////////////////////////////////////
// drawing routines

bool taDataGen::WritePoint(float_Matrix* mat, int x, int y, float color, bool wrap) {
  if(!CheckDims(mat, 2)) return false;
  bool clipped = (TwoDCoord::WrapClipOne(wrap, x, mat->dim(0)) ||
		  TwoDCoord::WrapClipOne(wrap, y, mat->dim(1)));
  if(clipped)
    return false;
  mat->FastEl(x,y) = color;
  return true;
}

bool taDataGen::RenderLine(float_Matrix* mat, int xs, int ys, int xe, int ye,
			   float color, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int xd = xe - xs;
  int yd = ye - ys;
  int x,y;
  if((xd == 0) && (yd == 0)) {
    WritePoint(mat, xs, ys, color, wrap);
    return true;
  }
  if(xd == 0) {
    if(yd > 0) { for(y=ys;y<=ye;y++) WritePoint(mat, xs, y, color, wrap); }
    else       { for(y=ys;y>=ye;y--) WritePoint(mat, xs, y, color, wrap); }
    return true;
  }
  if(yd == 0) {
    if(xd > 0)	{ for(x=xs;x<=xe;x++) WritePoint(mat, x, ys, color, wrap); }
    else	{ for(x=xs;x>=xe;x--) WritePoint(mat, x, ys, color, wrap); }
    return true;
  }
  if(abs(xd) > abs(yd)) {
    if(yd > 0) yd++; else yd--;
    if(xd > 0) {
      for(x=xs; x<=xe; x++) {
	y = ys + (yd * (x - xs)) / abs(xd);
	if(yd > 0) y = MIN(ye, y);
	else y = MAX(ye, y);
	WritePoint(mat, x, y, color, wrap);
      }
    }
    else {
      for(x=xs; x>=xe; x--) {
	y = ys + (yd * (xs - x)) / abs(xd);
	if(yd > 0) y = MIN(ye, y);
	else y = MAX(ye, y);
	WritePoint(mat, x, y, color, wrap);
      }
    }
  }
  else {
    if(xd > 0) xd++; else xd--;
    if(yd > 0) {
      for(y=ys; y<=ye; y++) {
	x = xs + (xd * (y - ys)) / abs(yd);
	if(xd > 0) x = MIN(xe, x);
	else x = MAX(xe, x);
	WritePoint(mat, x, y, color, wrap);
      }
    }
    else {
      for(y=ys; y>=ye; y--) {
	x = xs + (xd * (ys - y)) / abs(yd);
	if(xd > 0) x = MIN(xe, x);
	else x = MAX(xe, x);
	WritePoint(mat, x, y, color, wrap);
      }
    }
  }
  return true;
}

bool taDataGen::WriteXPoints(float_Matrix* mat, int x, int y,
			     const float_Matrix& color, int wdth, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int del = (wdth - 1) / 2;
  int i;
  for(i=0; i<wdth; i++)
    WritePoint(mat, x, y+i-del, color.FastEl(i), wrap);
  return true;
}

bool taDataGen::WriteYPoints(float_Matrix* mat, int x, int y,
			     const float_Matrix& color, int wdth, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int del = (wdth - 1) / 2;
  int i;
  for(i=0; i<wdth; i++)
    WritePoint(mat, x+i-del, y, color.FastEl(i), wrap);
  return true;
}

bool taDataGen::RenderWideLine(float_Matrix* mat, int xs, int ys, int xe, int ye,
			       const float_Matrix& color, int wdth, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int xd = xe - xs;
  int yd = ye - ys;
  int x,y;
  if((xd == 0) && (yd == 0)) {
    WriteXPoints(mat, xs, ys, color, wdth, wrap);
    WriteYPoints(mat, xs, ys, color, wdth, wrap);
    return true;
  }
  if(xd == 0) {
    if(yd > 0) { for(y=ys;y<=ye;y++) WriteXPoints(mat, xs, y, color, wdth, wrap); }
    else       { for(y=ys;y>=ye;y--) WriteXPoints(mat, xs, y, color, wdth, wrap); }
    return true;
  }
  if(yd == 0) {
    if(xd > 0)	{ for(x=xs;x<=xe;x++) WriteYPoints(mat, x, ys, color, wdth, wrap); }
    else	{ for(x=xs;x>=xe;x--) WriteYPoints(mat, x, ys, color, wdth, wrap); }
    return true;
  }
  if(abs(xd) > abs(yd)) {
    if(xd > 0) {
      for(x=xs; x<=xe; x++) {
	y = ys + (yd * (x - xs)) / abs(xd);
	WriteXPoints(mat, x, y, color, wdth, wrap);
      }
    }
    else {
      for(x=xs; x>=xe; x--) {
	y = ys + (yd * (xs - x)) / abs(xd);
	WriteXPoints(mat, x, y, color, wdth, wrap);
      }
    }
  }
  else {
    if(yd > 0) {
      for(y=ys; y<=ye; y++) {
	x = xs + (xd * (y - ys)) / abs(yd);
	WriteYPoints(mat, x, y, color, wdth, wrap);
      }
    }
    else {
      for(y=ys; y>=ye; y--) {
	x = xs + (xd * (ys - y)) / abs(yd);
	WriteYPoints(mat, x, y, color, wdth, wrap);
      }
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// random pattern generation

bool taDataGen::AddNoiseMat(float_Matrix* mat, const Random& rnd_spec) {
  for(int i=0; i<mat->size; i++)
    mat->FastEl_Flat(i) += rnd_spec.Gen();
  return true;
}

bool taDataGen::AddNoise(DataTable* data, const String& col_nm, const Random& rnd_spec) {
  if(!data) return false;
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataCol* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!AddNoise(data, da->name, rnd_spec))
	  rval = false;
      }
    }
    return rval;
  }
  DataCol* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    AddNoiseMat(mat, rnd_spec);
    taBase::unRefDone(mat);
  }
  data->DataUpdate(false);
  return true;
}

bool taDataGen::PermutedBinaryMat(float_Matrix* mat, int n_on, float on_val, float off_val) {
  float_Array ary(false);
  ary.SetSize(mat->size);
  int n_max = MIN(mat->size, n_on);
  int i;
  for(i=0; i<n_max; i++)
    ary.Set(i, on_val);
  for(;i<mat->size;i++)
    ary.Set(i, off_val);
  ary.Permute();
  for(i=0;i<mat->size;i++) {
    mat->FastEl_Flat(i) = ary.FastEl(i);
  }
  return true;
}

bool taDataGen::PermutedBinary(DataTable* data, const String& col_nm, int n_on,
			       float on_val, float off_val) {
  if(!data) return false;
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataCol* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!PermutedBinary(data, da->name, n_on, on_val, off_val))
	  rval = false;
      }
    }
    return rval;
  }
  DataCol* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    PermutedBinaryMat(mat, n_on, on_val, off_val);
    taBase::unRefDone(mat);
  }
  data->DataUpdate(false);
  return true;
}

bool taDataGen::PermutedBinary_MinDist(DataTable* data, const String& col_nm, int n_on,
				       float dist, taMath::DistMetric metric,
				       bool norm, float tol)
{
  if(!data) return false;
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataCol* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!PermutedBinary_MinDist(data, da->name, n_on, dist, metric, norm, tol))
	  rval = false;
      }
    }
    return rval;
  }
  DataCol* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  bool larger_further = taMath::dist_larger_further(metric);
  int bogus_count = 0;
  data->DataUpdate(true);
  for(int i =0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    int cnt = 100 + (10 * (i + 1));   // 100 plus 10 more for every new stim
    bool ok = false;
    float min_d;
    do {
      PermutedBinaryMat(mat, n_on);
      min_d = LastMinDist(da, i, metric, norm, tol);
      cnt--;
      if(larger_further)
	ok = (min_d >= dist);
      else
	ok = (min_d <= dist);
    } while(!ok && (cnt > 0));
    taBase::unRefDone(mat);
    
    if(cnt == 0) {
      taMisc::Warning("*** PermutedBinary_MinDist row:", String(i), "dist of:", (String)min_d,
		     "under dist limit:", (String)dist);
      bogus_count++;
    }
    if(bogus_count > 5) {
      taMisc::Warning("PermutedBinary_MinDist Giving up after 5 stimuli under the limit, set limits lower");
      data->DataUpdate(false);
      return false;
    }
  }
  data->DataUpdate(false);
  return true;
}

bool taDataGen::FlipBitsMat(float_Matrix* mat, int n_off, int n_on) {
  int_Array on_ary, off_ary;
  int i;
  for(i=0; i<mat->size; i++) {
    if(mat->FastEl_Flat(i) == 1.0)
      on_ary.Add(i);
    else
      off_ary.Add(i);
  }
  on_ary.Permute();
  off_ary.Permute();
  int n_max = MIN(on_ary.size, n_off);
  for(i=0; i<n_max; i++)
    mat->FastEl_Flat(on_ary.FastEl(i)) = 0.0f;
  n_max = MIN(off_ary.size, n_on);
  for(i=0; i<n_max; i++)
    mat->FastEl_Flat(off_ary.FastEl(i)) = 1.0f;
  return true;
}

bool taDataGen::FlipBits(DataTable* data, const String& col_nm, int n_off, int n_on) {
  if(!data) return false;
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataCol* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!FlipBits(data, da->name, n_off, n_on))
	  rval = false;
      }
    }
    return rval;
  }
  DataCol* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    FlipBitsMat(mat, n_off, n_on);
    taBase::unRefDone(mat);
  }
  data->DataUpdate(false);
  return true;
}

bool taDataGen::FlipBits_MinMax(DataTable* data, const String& col_nm, int n_off, int n_on,
				float min_dist, float max_dist,
				taMath::DistMetric metric, bool norm, float tol)
{
  if(!data) return false;
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataCol* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!FlipBits_MinMax(data, da->name, n_off, n_on, min_dist, max_dist,
			    metric, norm, tol))
	  rval = false;
      }
    }
    return rval;
  }
  DataCol* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  float_Matrix orig_pat(false);
  int bogus_count = 0;
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    int cnt = 100 + (10 * (i + 1));   // 100 plus 10 more for every new stim
    bool ok = false;
    float min_d, max_d;
    orig_pat.Reset();
    orig_pat = *mat;
    do {
      FlipBitsMat(mat, n_off, n_on);
      min_d = LastMinMaxDist(da, i, max_d, metric, norm, tol);
      cnt--;
      ok = ((min_d >= min_dist) && (max_d <= max_dist));
      if(!ok)			// restore original pattern if not ok..
	*mat = orig_pat;
    } while(!ok && (cnt > 0));
    taBase::unRefDone(mat);
    
    if(cnt == 0) {
      taMisc::Warning("*** FlipBits_MinMax, row:", String(i), "min/max dist of:",
		      String(min_d), String(max_d), "not within dist limits:",
		      String(min_dist), String(max_dist));
      bogus_count++;
    }
    if(bogus_count > 5) {
      taMisc::Warning("FlipBits_MinMax: Giving up after 5 stimuli under the limit, set limits lower");
      data->DataUpdate(false);
      return false;
    }
  }
  data->DataUpdate(false);
  return true;
}

float taDataGen::LastMinDist(DataCol* da, int row, taMath::DistMetric metric,
			     bool norm, float tol)
{
  bool larger_further = taMath::dist_larger_further(metric);
  float rval;
  if(larger_further)
    rval = taMath::flt_max;
  else
    rval  = -taMath::flt_max;
  if(row == 0) return rval;

  row = MIN(da->rows()-1, row);
  float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(row);
  taBase::Ref(mat);
  for(int i=0;i<row;i++) {
    float_Matrix* o_mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(o_mat);
    float dst = taMath_float::vec_dist(mat, o_mat, metric, norm, tol);
    if(larger_further)
      rval = MIN(dst, rval);
    else
      rval = MAX(dst, rval);
    taBase::unRefDone(o_mat);
  }
  taBase::unRefDone(mat);
  return rval;
}

float taDataGen::LastMinMaxDist(DataCol* da, int row, float& max_dist,
				taMath::DistMetric metric, bool norm, float tol)
{
  float rval = taMath::flt_max;
  max_dist = 0;
  if(row == 0) return rval;

  row = MIN(da->rows()-1, row);
  float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(row);
  taBase::Ref(mat);
  for(int i=0;i<row;i++) {
    float_Matrix* o_mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(o_mat);
    float dst = taMath_float::vec_dist(mat, o_mat, metric, norm, tol);
    rval = MIN(dst, rval);
    max_dist = MAX(dst, max_dist);
    taBase::unRefDone(o_mat);
  }
  taBase::unRefDone(mat);
  return rval;
}

// bool taDataGen::FlipBits_GpMinMax(int pat_no, int n_off, int n_on,
// 				    float within_min_dist, float within_max_dist,
// 				    float between_dist, taMath::DistMetric metric,
// 				    bool norm, float tol, int st_gp, int ed_gp)
// {
//   if(pat_no < 0) {
//     EventSpec* es = (EventSpec*)event_specs.DefaultEl();
//     if(es == NULL) return;
//     for(int pn = 0; pn < es->patterns.size; pn++) {
//       FlipBits_GpMinMax(pn, n_off, n_on, within_min_dist, within_max_dist, between_dist,
// 			metric, norm, tol, st_gp, ed_gp);
//     }
//     return;
//   }
//   bool larger_further = taMath::dist_larger_further(metric);
//   taMath orig_pat;
//   int bogus_count = 0;
//   int g;
//   int mx_gp = events.gp.size;
//   if(ed_gp >= 0)
//     mx_gp = MIN(ed_gp, mx_gp);
//   for(g = st_gp; g < mx_gp; g++) {
//     Event_MGroup* gp = (Event_MGroup*)events.gp.FastEl(g);

//     int ev_ctr = 0;
//     taLeafItr i;
//     Event* ev;
//     FOR_ITR_EL(Event, ev, gp->, i) {
//       Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
//       if(pat == NULL) {
// 	taMisc::Error("*** Pattern number:", String(pat_no), "not found");
// 	return;
//       }
//       int cnt = 100 + (10 * (ev_ctr + 1));   // 100 plus 10 more for every new stim
//       bool ok = false;
//       float min_d, max_d, min_w, max_w;
//       min_w = FLT_MAX;
//       orig_pat.Reset();
//       orig_pat = pat->value;
//       do {
// 	FlipBitsPat(pat, n_off, n_on);
// 	min_d = GpWithinMinMaxDist(gp, ev_ctr, pat_no, max_d, metric, norm, tol);
// 	cnt--;
// 	ok = ((min_d >= within_min_dist) && (max_d <= within_max_dist));
// 	if(!ok)			// restore original pattern if not ok..
// 	  pat->value = orig_pat;
// 	else {
// 	  min_w = GpLastMinMaxDist(g, pat, pat_no, max_w, metric, norm, tol);
// 	  if(larger_further)
// 	    ok = (min_w >= between_dist);
// 	  else
// 	    ok = (max_w <= between_dist); // todo: check this!
// 	  if(!ok)
// 	    pat->value = orig_pat;
// 	}
//       } while(!ok && (cnt > 0));
    
//       if(cnt == 0) {
// 	taMisc::Error("*** Event:", ev->name, "within min/max dist of:", String(min_d), 
// 		      String(max_d), "not within limits:", String(within_min_dist),
// 		      String(within_max_dist), "or between:", String(min_w),
// 		      "over:",String(between_dist));
// 	bogus_count++;
//       }
//       if(bogus_count > 5) {
// 	taMisc::Error("Giving up after 5 stimuli under the limit, set limits lower");
// 	return;
//       }
//       ev_ctr++;
//     }
//   }
//   UpdateAllViews();
// }

// float taDataGen::GpWithinMinMaxDist(Event_MGroup* gp, int n, int pat_no, float& max_dist,
// 				      taMath::DistMetric metric, bool norm, float tol)
// {
//   float rval = FLT_MAX;
//   max_dist = 0;
//   if(n == 0) return rval;

//   n = MIN(gp->leaves-1, n);
//   Event* trg_ev = (Event*)gp->Leaf(n);
//   Pattern* trg_pat = (Pattern*)trg_ev->patterns.Leaf(pat_no);
//   if(trg_pat == NULL) {
//     taMisc::Error("*** Pattern number:", String(pat_no), "not found");
//     return rval;
//   }

//   taLeafItr i;
//   Event* ev;
//   FOR_ITR_EL(Event, ev, gp->, i) {
//     if(ev == trg_ev) break;
//     Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
//     if(pat == NULL) {
//       taMisc::Error("*** Pattern number:", String(pat_no), "not found");
//       return rval;
//     }
//     float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
//     rval = MIN(dst, rval);
//     max_dist = MAX(dst, max_dist);
//   }
//   return rval;
// }

// float taDataGen::GpMinMaxDist(Event_MGroup* gp, Pattern* trg_pat, int pat_no, float& max_dist,
// 				taMath::DistMetric metric, bool norm, float tol)
// {
//   float rval = FLT_MAX;
//   max_dist = 0;
//   taLeafItr i;
//   Event* ev;
//   FOR_ITR_EL(Event, ev, gp->, i) {
//     Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
//     if(pat == NULL) {
//       taMisc::Error("*** Pattern number:", String(pat_no), "not found");
//       return rval;
//     }
//     float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
//     rval = MIN(dst, rval);
//     max_dist = MAX(dst, max_dist);
//   }
//   return rval;
// }

// float taDataGen::GpLastMinMaxDist(int gp_no, Pattern* trg_pat, int pat_no, float& max_dist,
// 				    taMath::DistMetric metric, bool norm, float tol, int st_gp)
// {
//   float rval = FLT_MAX;
//   max_dist = 0;
//   int mx_gp = MIN(gp_no, events.gp.size);
//   int g;
//   for(g=st_gp; g<mx_gp; g++) {
//     Event_MGroup* gp = (Event_MGroup*)events.gp.FastEl(g);
//     float mx_dst;
//     float dst = GpMinMaxDist(gp, trg_pat, pat_no, mx_dst, metric, norm, tol);
//     rval = MIN(dst, rval);
//     max_dist = MAX(mx_dst, max_dist);
//   }
//   return rval;
// }


///////////////////////////////////////////////////////////////////
// sub-matrix operations -- read/write from sub matricies to/from larger matrix

bool taDataGen::WriteFmSubMatricies(DataTable* dest, const String& dest_col_nm, 
				    DataTable* sub_mtx_src, SubMatrixOpSpec* spec, 
				    taMatrix::RenderOp render_op,
				    bool reset_first) {
  if(!dest) { taMisc::Error("taDataGen::WriteFmSubMatricies: dest is NULL"); return false; }
  if(!sub_mtx_src) { taMisc::Error("taDataGen::WriteFmSubMatricies: sub_mtx_src is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataGen::WriteFmSubMatricies: spec is NULL"); return false; }

  DataCol* dcol = dest->FindColName(dest_col_nm, true); // warn
  if(!dcol) return false;

  dest->StructUpdate(true);
  spec->GetColumns(sub_mtx_src);		// cache column pointers & indicies from names

  int dms,d0,d1,d2,d3,d4,d5,d6;

  for(int i=0;i<spec->ops.size; i++) {
    SubMatrixOpEl* ds = (SubMatrixOpEl*)spec->ops.FastEl(i);
    if(ds->col_idx < 0) continue; // couldn't find this col
    ds->offset.GetGeom(dms,d0,d1,d2,d3,d4,d5,d6);
    DataCol* scol = sub_mtx_src->data.FastEl(ds->col_idx);
    dcol->AR()->WriteFmSubMatrixFrames(scol->AR(), render_op, d0,d1,d2,d3,d4,d5,d6);
  }
  spec->ClearColumns();
  dest->StructUpdate(false);
  return true;
}

bool taDataGen::ReadToSubMatricies(DataTable* src, const String& src_col_nm, 
				   DataTable* sub_mtx_dest, SubMatrixOpSpec* spec,
				   taMatrix::RenderOp render_op,
				   bool reset_first) {
  if(!src) { taMisc::Error("taDataGen::ReadToSubMatricies: src is NULL"); return false; }
  if(!sub_mtx_dest) { taMisc::Error("taDataGen::ReadToSubMatricies: sub_mtx_dest is NULL"); return false; }
  if(!spec) { taMisc::Error("taDataGen::ReadToSubMatricies: spec is NULL"); return false; }

  DataCol* scol = src->FindColName(src_col_nm, true); // warn
  if(!scol) return false;

  sub_mtx_dest->StructUpdate(true);
  spec->GetColumns(sub_mtx_dest);		// cache column pointers & indicies from names

  int dms,d0,d1,d2,d3,d4,d5,d6;

  for(int i=0;i<spec->ops.size; i++) {
    SubMatrixOpEl* ds = (SubMatrixOpEl*)spec->ops.FastEl(i);
    if(ds->col_idx < 0) continue; // couldn't find this col
    ds->offset.GetGeom(dms,d0,d1,d2,d3,d4,d5,d6);
    DataCol* dcol = sub_mtx_dest->data.FastEl(ds->col_idx);
    scol->AR()->ReadToSubMatrixFrames(dcol->AR(), render_op, d0,d1,d2,d3,d4,d5,d6);
  }
  spec->ClearColumns();
  sub_mtx_dest->StructUpdate(false);
  return true;
}


///////////////////////////////////////////////////////////////////
// 	FeatPats -- Generating larger patterns from Feature patterns


taMatrix* taDataGen::GetFeatPatNo(DataTable* feat_vocab, const String& feat_col_nm, int row_no) {
  return feat_vocab->GetValAsMatrixColName(feat_col_nm, row_no);
}

taMatrix* taDataGen::GetFeatPatName(DataTable* feat_vocab, const String& feat_col_nm, 
				    const String& feat_row_name, const String& name_col_nm) {
  return feat_vocab->GetValAsMatrixColRowName(feat_col_nm, name_col_nm, feat_row_name);
}

taMatrix* taDataGen::GetRndFeatPat(DataTable* feat_vocab, const String& feat_col_nm) {
  int rnd_idx = Random::IntZeroN(feat_vocab->rows);
  return GetFeatPatNo(feat_vocab, feat_col_nm, rnd_idx);
}


bool taDataGen::GenRndFeatPats(DataTable* dest, const String& dest_col, int n_pats,
			       DataTable* feat_vocab, const String& feat_col_nm) {
  if(!dest) { taMisc::Error("taDataGen::GenRndFeatPats: dest is NULL"); return false; }
  if(!feat_vocab) { taMisc::Error("taDataGen::GenRndFeatPats: feat_vocab is NULL"); return false; }

  DataCol* dcol = dest->FindColName(dest_col, true);
  if(!dcol) return false;
  if(dcol->cell_dims() < 2) {
    taMisc::Error("taDataGen::GenRndFeatPats: destination column must be matrix with dimensionality >= 2");
    return false;
  }

  taMatrixPtr ref_pat;  ref_pat = GetFeatPatNo(feat_vocab, feat_col_nm, 0); // get 1st guy as ref
  if(ref_pat->dims() != 2) {
    taMisc::Error("taDataGen::GenRndFeatPats: feature vocab must have 2d matrix cells");
    return false;
  }
  int feat_x = ref_pat->dim(0);     int feat_y = ref_pat->dim(1);
  int pat_x = dcol->GetCellGeom(0);  int pat_y = dcol->GetCellGeom(1);
  int feat_sz = feat_x * feat_y;

  if(pat_x != feat_x || pat_y % feat_y != 0) {
    taMisc::Error("taDataGen::GenRndFeatPats: features do not fit evenly into dest cell geometry -- innermost (x) dimension must be equal, and next (y) dimension must evenly divisible");
    return false;
  }

  dest->StructUpdate(true);
  dest->EnforceRows(n_pats);

  int n_feats = dcol->cell_size() / feat_sz; // how many features per pattern

  MatrixGeom dpt;
  int dms,d0,d1,d2,d3,d4,d5,d6;

  for(int pat=0;pat<n_pats;pat++) {
    for(int i=0;i<n_feats;i++) {
      int dest_idx = feat_sz * i;
      dcol->cell_geom.DimsFmIndex(dest_idx, dpt);
      dpt.GetGeom(dms,d0,d1,d2,d3,d4,d5,d6);
      taMatrixPtr rnd_pat; rnd_pat = GetRndFeatPat(feat_vocab, feat_col_nm);
      taMatrixPtr dst_pat; dst_pat = dcol->GetValAsMatrix(pat);
      dst_pat->WriteFmSubMatrix(rnd_pat, taMatrix::COPY, d0,d1,d2,d3,d4,d5,d6);
    }
  }
  dest->StructUpdate(false);
  return true;
}

bool taDataGen::GenItemsFmProtos(DataTable* items, const String& dest_col, 
				 DataTable* protos, int n_items, float flip_pct,
				 DataTable* feat_vocab, const String& feat_col_nm) {
  if(!items) { taMisc::Error("taDataGen::GenItemsFmProtos: items is NULL"); return false; }
  if(!protos) { taMisc::Error("taDataGen::GenItemsFmProtos: protos is NULL"); return false; }
  if(!feat_vocab) { taMisc::Error("taDataGen::GenItemsFmProtos: feat_vocab is NULL"); return false; }

  DataCol* dcol = protos->FindColName(dest_col, true);
  if(!dcol) return false;
  if(dcol->cell_dims() < 2) {
    taMisc::Error("taDataGen::GenItemsFmProtos: destination column must be matrix with dimensionality >= 2");
    return false;
  }

  taMatrixPtr ref_pat;  ref_pat = GetFeatPatNo(feat_vocab, feat_col_nm, 0); // get 1st guy as ref
  if(ref_pat->dims() != 2) {
    taMisc::Error("taDataGen::GenItemsFmProtos: feature vocab must have 2d matrix cells");
    return false;
  }
  int feat_x = ref_pat->dim(0);     int feat_y = ref_pat->dim(1);
  int pat_x = dcol->GetCellGeom(0);  int pat_y = dcol->GetCellGeom(1);
  int feat_sz = feat_x * feat_y;

  if(pat_x != feat_x || pat_y % feat_y != 0) {
    taMisc::Error("taDataGen::GenItemsFmProtos: features do not fit evenly into dest cell geometry -- innermost (x) dimension must be equal, and next (y) dimension must evenly divisible");
    return false;
  }

  items->StructUpdate(true);

  int n_feats = dcol->cell_size() / feat_sz; // how many features per pattern
  int flip_n = (int)(flip_pct * (float)n_feats);
  flip_n = MIN(n_feats, flip_n);
  flip_n = MAX(1, flip_n);	// flip at least 1!

  items->ResetData();
  items->Copy_NoData(*protos);
  taDataProc::ReplicateRows(items, protos, n_items);

  // rename items if name col is there
  DataCol* nm_col = items->FindColName("Name", false); // no warning
  if(nm_col) {
    int rw = 0;
    for(int prot=0;prot<protos->rows;prot++) {
      for(int itm=0;itm<n_items;itm++, rw++) {
	String cur_nm = nm_col->GetValAsString(rw);
	nm_col->SetValAsString(cur_nm + "_" + String(itm), rw);
      }
    }
  }
  dcol = items->FindColName(dest_col, true); // get it for items now

  MatrixGeom dpt;
  int dms,d0,d1,d2,d3,d4,d5,d6;

  int_Array flip_list;
  flip_list.SetSize(n_feats);
  flip_list.FillSeq();

  for(int pat=0;pat<items->rows;pat++) {
    flip_list.Permute();	// flip for each item
    for (int i=0; i<flip_n; i++) {
      int dest_idx = feat_sz * flip_list[i]; // permute indirection
      dcol->cell_geom.DimsFmIndex(dest_idx, dpt);
      dpt.GetGeom(dms,d0,d1,d2,d3,d4,d5,d6);
      taMatrixPtr rnd_pat; rnd_pat = GetRndFeatPat(feat_vocab, feat_col_nm);
      taMatrixPtr dst_pat; dst_pat = dcol->GetValAsMatrix(pat);
      dst_pat->WriteFmSubMatrix(rnd_pat, taMatrix::COPY, d0,d1,d2,d3,d4,d5,d6);
    }
  }
  items->StructUpdate(false);
  return true;
}

bool taDataGen::GenNamedFeatPats(DataTable* dest, const String& dest_col,
				 DataTable* feat_vocab, const String& feat_col_nm,
				 DataTable* names, const String& name_col_nm,
				 const String& feat_name_col_nm) {
  if(!dest) { taMisc::Error("taDataGen::GenNamedFeatPats: dest is NULL"); return false; }
  if(!names) { taMisc::Error("taDataGen::GenNamedFeatPats: names is NULL"); return false; }
  if(!feat_vocab) { taMisc::Error("taDataGen::GenNamedFeatPats: feat_vocab is NULL"); return false; }

  DataCol* dcol = dest->FindColName(dest_col, true);
  if(!dcol) return false;
  if(dcol->cell_dims() < 2) {
    taMisc::Error("taDataGen::GenNamedFeatPats: destination column must be matrix with dimensionality >= 2");
    return false;
  }

  DataCol* nmcol = names->FindColName(name_col_nm, true);
  if(!nmcol) return false;

  taMatrixPtr ref_pat;  ref_pat = GetFeatPatNo(feat_vocab, feat_col_nm, 0); // get 1st guy as ref
  if(ref_pat->dims() != 2) {
    taMisc::Error("taDataGen::GenNamedFeatPats: feature vocab must have 2d matrix cells");
    return false;
  }
  int feat_x = ref_pat->dim(0);     int feat_y = ref_pat->dim(1);
  int pat_x = dcol->GetCellGeom(0);  int pat_y = dcol->GetCellGeom(1);
  int feat_sz = feat_x * feat_y;

  if(pat_x != feat_x || pat_y % feat_y != 0) {
    taMisc::Error("taDataGen::GenNamedFeatPats: features do not fit evenly into dest cell geometry -- innermost (x) dimension must be equal, and next (y) dimension must evenly divisible");
    return false;
  }

  dest->StructUpdate(true);
  dest->EnforceRows(names->rows); // same as names

  int n_feats = dcol->cell_size() / feat_sz; // how many features per pattern
  int n_names = nmcol->cell_size();

  int ft_max = MIN(n_feats, n_names); // only go as high as have names

  MatrixGeom dpt;
  int dms,d0,d1,d2,d3,d4,d5,d6;

  for(int pat=0;pat<dest->rows;pat++) {
    for(int i=0;i<ft_max;i++) {
      int dest_idx = feat_sz * i;
      dcol->cell_geom.DimsFmIndex(dest_idx, dpt);
      dpt.GetGeom(dms,d0,d1,d2,d3,d4,d5,d6);
      String cur_nm = nmcol->GetValAsStringM(pat, i);
      taMatrixPtr nm_pat; nm_pat = GetFeatPatName(feat_vocab, feat_col_nm, cur_nm,
						  feat_name_col_nm);
      if(!(bool)nm_pat) continue; // didn't find it.
      taMatrixPtr dst_pat; dst_pat = dcol->GetValAsMatrix(pat);
      dst_pat->WriteFmSubMatrix(nm_pat, taMatrix::COPY, d0,d1,d2,d3,d4,d5,d6);
    }
  }
  dest->StructUpdate(false);
  return true;
}

///////////////////////////////////////////////////////////////////
// misc data functions

static bool taDataGen_GetDirFiles_impl(DataTable* dest, const String& dir_path, 
				       const String& filter, bool recursive,
				       int fname_idx, int path_idx, int depth) {

  if(depth > 20) {
    taMisc::Warning("taDataGen::GetDirFiles -- recursive directory search with depth > 20: probably symbolic link loop");
    return false;
  }
  
  bool found_some = false;
  QDir dir(dir_path);
  if(recursive)
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);
  else
    dir.setFilter(QDir::Files);
  QFileInfoList files = dir.entryInfoList();
  if(files.size() == 0) return false;
  for(int i=0;i<files.size();i++) {
    QFileInfo fli = files[i];
    String fl = fli.fileName();
    if(fli.isFile()) {
      if(filter.empty() || fl.contains(filter)) {
	dest->AddBlankRow();
	found_some = true;
	if(fname_idx >= 0) {
	  dest->SetValAsString(fl, fname_idx, -1);
	}
	if(path_idx >= 0) {
	  dest->SetValAsString(fli.filePath(), path_idx, -1);
	}
      }
    }
    if(recursive && fli.isDir()) {
      String nwpth = dir_path + "/" + fl;
      bool gs = taDataGen_GetDirFiles_impl(dest, nwpth, filter, recursive, fname_idx,
					   path_idx, depth+1);
      found_some = found_some || gs; 
    }
  }
  return found_some;
}


bool taDataGen::GetDirFiles(DataTable* dest, const String& dir_path, 
			    const String& filter, bool recursive,
			    const String& fname_col_nm,
			    const String& path_col_nm) {
  if(!dest) return false;
  dest->StructUpdate(true);

  int fname_idx = -1;
  if(!fname_col_nm.empty())
    dest->FindMakeColName(fname_col_nm, fname_idx, DataTable::VT_STRING, 0);

  int path_idx = -1;
  if(!path_col_nm.empty())
    dest->FindMakeColName(path_col_nm, path_idx, DataTable::VT_STRING, 0);

  return taDataGen_GetDirFiles_impl(dest, dir_path, filter, recursive, fname_idx, path_idx, 0);
}

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

void DataGenCall::Initialize() {
  min_type = &TA_taDataGen;
  object_type = &TA_taDataGen;
}
