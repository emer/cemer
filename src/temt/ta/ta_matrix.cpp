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


#include "ta_matrix.h"
#include "ta_thread.h"

#ifdef TA_GUI
# include "ta_matrix_qt.h"
# include <QMimeData>
#endif


//////////////////////////
//  CellRange		//
//////////////////////////

void CellRange::SetExtent(int wd, int ht) {
  col_to = col_fr + wd - 1;
  row_to = row_fr + ht - 1;
}

void CellRange::LimitExtent(int wd, int ht) {
  if (width() > wd) col_to = col_fr + wd - 1;
  if (height() > ht) row_to = row_fr + ht - 1;
}

void CellRange::LimitRange(int row_to_, int col_to_) {
  if (col_to > col_to_) col_to = col_to_;
  if (row_to > row_to_) row_to = row_to_;
}

void CellRange::SetFromModel(const QModelIndexList& indexes) {
//NOTE: this assumes that we only allow contiguous areas -- the
//  input list could have discontiguous guys, but here we will
//  just make an overal sel based on the max
  // start with first guy, then expand it out
  if (indexes.count() > 0) {
    const QModelIndex& mi = indexes.first();
    row_fr = row_to = mi.row();
    col_fr = col_to = mi.column();
    for (int i = 1; i < indexes.count(); ++i) {
      const QModelIndex& mi = indexes.at(i);
      int r = mi.row();
      int c = mi.column();
      if (r < row_fr) row_fr = r;
      else if (r > row_to) row_to = r;
      if (c < col_fr) col_fr = c;
      else if (c > col_to) col_to = c;
    }
  } else {
    Set(0, 0, -1, -1);
  }
}

//////////////////////////
//  MatrixGeom		//
//////////////////////////

MatrixGeom MatrixGeom::td;

MatrixGeom::MatrixGeom(int init_size) {
  Initialize();
  SetSize(init_size);
}
  
MatrixGeom::MatrixGeom(int dims, int d0, int d1, int d2, int d3, int d4, int d5, int d6)
{
  Initialize();
  SetGeom(dims, d0, d1, d2, d3, d4, d5, d6);
}

void MatrixGeom::Initialize() {
  // default minimum geometry is 2d
  size = 0;
  // set all the dim values valid -- other code may use shortcuts to read these
  memset(el, 0, sizeof(el));
}

void MatrixGeom::Destroy() {
#ifdef DEBUG
  // helps detect multi-delete errors
  for (int i = size - 1; i >= 0; el[i--] = 0); 
  size = 0; 
#endif
}

void MatrixGeom::Copy_(const MatrixGeom& cp) {
  SetSize(cp.size);
  for (int i = 0; i < size; ++i) {
    el[i] = cp.el[i];
  }
} 

void MatrixGeom::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // we paranoically set unused vals to 0, in case optimized code elsewhere 
  // is somewhat sleazily directly accessing el values w/o checking size
  for (int i = size; i < TA_MATRIX_DIMS_MAX; ++i) 
    el[i] = 0;
}

void MatrixGeom::Add(int value) {
  if (size >= TA_MATRIX_DIMS_MAX) return;
  el[size++] = value;
}

int MatrixGeom::IndexFmDims(const MatrixGeom& d) const {
  return IndexFmDims_(d.el);
}

int MatrixGeom::IndexFmDims(int d0, int d1, int d2,
    int d3, int d4, int d5, int d6) const
{
  int d[TA_MATRIX_DIMS_MAX];
  d[0] = d0;
  d[1] = d1;
  d[2] = d2;
  d[3] = d3;
  d[4] = d4;
  d[5] = d5;
  d[6] = d6;
  d[7] = 0;
  return IndexFmDims_(d);
}

int MatrixGeom::IndexFmDims_(const int* d) const {
  int rval = -1;
  switch (size) {
  case 0: rval = 0;
    break;
  case 1: rval = d[0];
    break;
  case 2: rval = (d[1] * el[0]) + d[0];
    break;
  case 3: rval = (((d[2] * el[1]) + d[1]) * el[0]) + d[0];
    break;
  case 4: rval = (((((d[3] * el[2]) + d[2]) * el[1]) + d[1]) * el[0]) + d[0];
    break;
  case 5: rval = (((((((d[4] * el[3]) + d[3]) * el[2]) + d[2]) * el[1]) + d[1]) * el[0]) + d[0];
  case 6: rval = (((((((((d[5] * el[4]) + d[4]) * el[3]) + d[3]) * el[2]) + d[2]) * el[1]) + d[1]) * el[0]) + d[0];
  case 7: rval = (((((((((((d[6] * el[5]) + d[5]) * el[4]) + d[4]) * el[3]) + d[3]) * el[2]) + d[2]) * el[1]) + d[1]) * el[0]) + d[0];
    break;
  default: break;
  }
  return rval;
}

void MatrixGeom::DimsFmIndex(int idx, MatrixGeom& d) const {
  d.SetSize(size);
  switch (size) {
  case 0: 
    break;
  case 1:
    d[0] = idx;
    break;
  case 2:
    d[1] = idx / el[0];
    d[0] = idx % el[0];
    break;
  case 3:
    d[2] = idx / (el[1] * el[0]);
    d[1] = (idx % el[1]) / el[0];
    d[0] = idx % (el[1] * el[0]);
    break;
  case 4: 
    d[3] = idx / (el[2] * el[1] * el[0]);
    d[2] = (idx % el[2]) / (el[1] * el[0]);
    d[1] = (idx % (el[2] * el[1])) / el[0];
    d[0] = idx % (el[2] * el[1] * el[0]);
    break;
  case 5: 
    d[4] = idx / (el[3] * el[2] * el[1] * el[0]);
    d[3] = (idx % el[3]) / (el[2] * el[1] * el[0]);
    d[2] = (idx % (el[3] * el[2])) / (el[1] * el[0]);
    d[1] = (idx % (el[3] * el[2] * el[1])) / el[0];
    d[0] = idx % (el[3] * el[2] * el[1] * el[0]);
    break;
  case 6: 
    d[5] = idx / (el[4] * el[3] * el[2] * el[1] * el[0]);
    d[4] = (idx % el[4]) / (el[3] * el[2] * el[1] * el[0]);
    d[3] = (idx % (el[4] * el[3])) / (el[2] * el[1] * el[0]);
    d[2] = (idx % (el[4] * el[3] * el[2])) / (el[1] * el[0]);
    d[1] = (idx % (el[4] * el[3] * el[2] * el[1])) / el[0];
    d[0] = idx % (el[4] * el[3] * el[2] * el[1] * el[0]);
    break;
  case 7: 
    d[6] = idx / (el[5] * el[4] * el[3] * el[2] * el[1] * el[0]);
    d[5] = (idx % el[5]) / (el[3] * el[2] * el[1] * el[0]);
    d[4] = (idx % (el[5] * el[4])) / (el[3] * el[2] * el[1] * el[0]);
    d[3] = (idx % (el[5] * el[4] * el[3])) / (el[2] * el[1] * el[0]);
    d[2] = (idx % (el[5] * el[4] * el[3] * el[2])) / (el[1] * el[0]);
    d[1] = (idx % (el[5] * el[4] * el[3] * el[2] * el[1])) / el[0];
    d[0] = idx % (el[5] * el[4] * el[3] * el[2] * el[1] * el[0]);
    break;
  default: break;
  }
}

void MatrixGeom::Get2DGeom(int& x, int& y) const {
  if (size == 0) {
    x = 0;
    y = 0;
  }
  else if(size == 1) {
    x = FastEl(0);
    y = 1;
  }
  else if(size >= 2) {
    x = FastEl(0);
    y = FastEl(1);
    for (int i = 2; i < size; ++i)
      y *= FastEl(i);
  }
}

void MatrixGeom::Get2DGeomGui(int& x, int& y, bool odd_y, int spc) const {
  if (spc < 0) spc = 0;
  x = 1;
  y = 1;
  if(size == 1) {
    if(odd_y)
      y = FastEl(0);
    else
      x = FastEl(0);
  }
  else if(size == 2) {
    x = FastEl(0);
    y = FastEl(1);
  }
  else if(size == 3) { // series of 2d guys: layout vert or horiz?? vert!?
    if(odd_y) {
      x = FastEl(0);
      y = (FastEl(1) + spc) * FastEl(2); // assume space with 1
    }
    else {
      x = (FastEl(0) + spc) * FastEl(2);
      y = FastEl(1);
    }
  }
  else if(size == 4) { // matrix of 2d guys
    x = (FastEl(0) + spc) * FastEl(2);
    y = (FastEl(1) + spc) * FastEl(3);
  }
  else if(size == 5) { // matrix of 2d guys + time series: vertical
    if(odd_y) {
      x = (FastEl(0) + spc) * FastEl(2);
      y = (FastEl(1) + spc) * FastEl(3) * FastEl(4);
    }
    else {
      x = (FastEl(0) + spc) * FastEl(2) * FastEl(4);
      y = (FastEl(1) + spc) * FastEl(3);
    }
  }
}

int MatrixGeom::Dump_Save_Value(ostream& strm, TAPtr, int) {
  strm << "{ ";
  int i;
  for (i=0; i < size; i++) {
    strm << FastEl(i) << ";";
  }
  return true;
}

int MatrixGeom::Dump_Load_Value(istream& strm, TAPtr) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)    return EOF;
  if(c == ';') // just a path
    return 2;  // signal that just a path was loaded..

  if(c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  c = taMisc::read_till_rb_or_semi(strm);
  int cnt = 0;
  int val;
  while ((c == ';') && (c != EOF)) {
    val = taMisc::LexBuf.toInt();
    if (cnt > size-1)
      Add(val);
    else Set(cnt, val);
    ++cnt;
    c = taMisc::read_till_rb_or_semi(strm);
  }
  if (c==EOF)	return EOF;
  return true;
}

String MatrixGeom::GeomToString(const char* ldelim, const char* rdelim) const {
  String rval(ldelim);
  rval += String(size) + ":";
  int i;
  for (i = 0; i < size-1; ++i) {
    rval += String(el[i]) + ",";
  }
  rval += String(el[i]) + rdelim;
  return rval;
}

void MatrixGeom::GeomFromString(String& str, const char* ldelim, const char* rdelim) {
  str = str.after(ldelim);
  String ds = str.before(':');
  str = str.after(':');
  SetSize((int)ds);
  int i;
  for(i=0;i<size-1;i++) {
    ds = str.before(',');
    str = str.after(',');
    Set(i, (int)ds);
  }
  ds = str.before(rdelim);
  str = str.after(rdelim);
  Set(i, (int)ds);
}

bool MatrixGeom::SetSize(int new_sz) {
  if ((new_sz < 0) || (new_sz > TA_MATRIX_DIMS_MAX)) return false;
  if(size == new_sz) return false;
  // zero out orphaned old elements
  for (int i = size - 1; i >= new_sz; --i)
    el[i] = 0;
  // zero out new elements
  for (int i = size; i < new_sz; ++i)
    el[i] = 0;
  size = new_sz;
  return true;
}

bool MatrixGeom::Equal(const MatrixGeom& other) const {
  if (size != other.size) return false;
  for (int i = 0; i < size; ++i) {
    if (el[i] != other.el[i]) return false;
  }
  return true;
} 

bool MatrixGeom::IsFrameOf(const MatrixGeom& other) const {
  if (size != (other.size - 1)) return false;
  for (int i = 0; i < size; ++i) {
    if (el[i] != other.el[i]) return false;
  }
  return true;
}

int MatrixGeom::Product() const {
  if (size == 0) return 0;
  int rval = el[0];
  for (int i = 1; i < size; ++i)
    rval *= el[i];
  return rval;
}

void MatrixGeom::SetGeom(int dims, int d0, int d1, int d2, int d3, int d4,
  int d5, int d6) 
{
  SetSize(dims);
  el[0] = d0;
  el[1] = d1;
  el[2] = d2;
  el[3] = d3;
  el[4] = d4;
  el[5] = d5;
  el[6] = d6;
}



//////////////////////////
//  taMatrix		//
//////////////////////////

bool taMatrix::GeomIsValid(int dims_, const int geom_[],
  String* err_msg, bool allow_flex)
{
  if ((dims_ <= 0) || (dims_ > TA_MATRIX_DIMS_MAX)) { 
    if (err_msg)
      *err_msg = "dims must be: 0 < dims <= " + String(TA_MATRIX_DIMS_MAX) + 
        " was: " + String(dims_);
    return false;
  }
  
  // we only allow a non-zero value in top dim (flex sizing, if enabled)
  for (int i = 0; i < (dims_ - 1) ; ++i) {
    if (geom_[i] < 0) {
      if (err_msg)
        *err_msg = "geoms must be >= 0";
      return false;
    } else if (geom_[i] == 0) {
      if (!(allow_flex && (i == (dims_ - 1)))) {
        if (err_msg)
          *err_msg = "all but highest-most geom must be >0";
        return false;
      }
    }
  }
  
/*obs,nuke  // note: for loading and initialization, we accept zero dim values,
  // provided all above that dim are also zero
  int i;
  bool found_zero = false;
  for (i = 0; i < (dims_ - 1) ; ++i) {
    if (geom_[i] < 0) {
      *err_msg = "geoms must be >= 0";
      return false;
    } else if (geom_[i] <= 0) {
      found_zero = true;
    } else {
      if (found_zero) {
        *err_msg = "highest-most geoms must all be >0 or 0";
        return false;
      }
    }
  }*/
  
  return true;
}

void taMatrix::SliceDestroying(taMatrix* par_slice, taMatrix* child_slice) {
  par_slice->slices->RemoveEl(child_slice);
  // note: having already sliced, we leave the list now in place
  taBase::UnRef(par_slice);
}

void taMatrix::SliceInitialize(taMatrix* par_slice, taMatrix* child_slice) {
  taBase::Ref(par_slice);
  if (!par_slice->slices) {
    par_slice->slices = new taMatrix_PList;
  }
  par_slice->slices->Add(child_slice);
  child_slice->slice_par = par_slice;
}

void taMatrix::Initialize()
{
  size = 0;
  alloc_size = 0;
  slices = NULL;
  slice_par = NULL;
  fixed_dealloc = NULL;
}
 
void taMatrix::Destroy() {
  CutLinks();
  size = 0;
  alloc_size = 0;
  if (slice_par) {
    SliceDestroying(slice_par, this);
    slice_par = NULL; 
  }
#ifdef DEBUG 
  if (sliceCount() > 0) {
    taMisc::Warning("taMatrix being destroyed with slice_cnt=", String(sliceCount()));
  }
#endif
  if (slices) {
    delete slices;
    slices = NULL;
  }
}

void taMatrix::BatchUpdate(bool begin, bool struc) {
  inherited::BatchUpdate(begin, struc);
  // recursively send blocked updates to slices
  if (slices) for (int i = 0; i < slices->size; ++i) {
    taMatrix* mat = slices->FastEl(i);
    mat->BatchUpdate(begin, struc); // recursive, of course
  }
}

void taMatrix::Add_(const void* it) {
  if(TestError(!canResize(), "Add", "resizing not allowed")) return;
  if(TestError((dims() != 1), "Add", "only allowed when dims=1")) return;
  int idx = frames();
  if(EnforceFrames(idx + 1))
    El_Copy_(FastEl_Flat_(idx), it);
}

bool taMatrix::AddFrames(int n) {
  return EnforceFrames(n + frames());
}

bool taMatrix::Alloc_(int new_alloc) {
  if(TestError((alloc_size < 0), "Alloc_", "cannot alloc a fixed data matrix")) return false;
  // TODO  Check((slice_cnt == 0), "cannot alloc a sliced data matrix");
  // NOTE: this is a low level allocator; alloc is typically managed in frames
  if (alloc_size < new_alloc)	{
    if(fastAlloc() && (alloc_size > 0)) {		// fast alloc = malloc/realloc
      char* old = (char*)data();
      if(!FastRealloc_(new_alloc))
	return false;
      ta_intptr_t delta = (char*)data() - old;
      UpdateSlices_Realloc(delta);
    }
    else {
      char* nw = (char*)MakeArray_(new_alloc);
      if(TestError(!nw, "Alloc_", "could not allocate matrix memory -- matrix is too big!  reverting to old size -- could be fatal!")) {
	return false;
      }
      for (int i = 0; i < size; ++i) {
	El_Copy_(nw + (El_SizeOf_() * i), FastEl_Flat_(i));
      }
      // calculate delta, in bytes, of the new address and update slices
      ta_intptr_t delta = nw - (char*)data();
      UpdateSlices_Realloc(delta);
      // we can now update ourself
      SetArray_(nw);
    }
    alloc_size = new_alloc;
  }
  return true; // this impl assumes this can never fail!
}

bool taMatrix::AllocFrames(int n) {
  if(TestError((alloc_size < 0), "AllocFrames", "cannot alloc a fixed data matrix")) return false;
  if(TestError((n < 0), "AllocFrames", "n (num frames) must be >= 0")) return false;
  int frsz = frameSize();
  if(frsz == 0) return false;		// not dimensioned yet -- don't bother
  int cur_n = alloc_size / frsz;
  if(cur_n >= n) return true;	// already sufficient!

  int act_n = n;		// actual n frames to request
  // start w/ 4, double up to 64, then 1.5x thereafter
  if (cur_n == 0) act_n = MAX(4, act_n);
  else if (cur_n < 64) act_n = MAX((cur_n * 2), act_n);
  else act_n =  MAX(((cur_n * 3) / 2), act_n);
  return Alloc_(act_n * frsz);
}

bool taMatrix::canResize() const {
  return (alloc_size >= 0);
}

void taMatrix::CanCopyCustom_impl(bool to, const taBase* cp, bool quiet,
    bool& allowed, bool& forbidden) const
{
  if (to) return; // no strictures
  if (cp->InheritsFrom(&TA_taMatrix)) {
    allowed = true; // generally allowed
  }
}

void taMatrix::CopyFromCustom_impl(const taBase* cp_fm) {
  if (cp_fm->InheritsFrom(&TA_taMatrix)) {
    Copy_Matrix_impl(static_cast<const taMatrix*>(cp_fm));
  }
  else inherited::CopyFromCustom_impl(cp_fm); // unlikely/prob doesn't happen
}

void taMatrix::Copy_Matrix_impl(const taMatrix* cp) {
  // note: caller has asserted Struct guys
  // assumes copy has been validated
  inherited::Copy_impl(*cp); // do all common generic parent class copying
  SetBaseFlag(COPYING); // note: still have to set/reset here, because not nestable
  SetGeomN(cp->geom);
  for (int i = 0; i < size; ++i) {
    El_SetFmVar_(FastEl_Flat_(i), cp->FastElAsVar_Flat(i));
  }  
  ClearBaseFlag(COPYING);
}

bool taMatrix::CopyFrame(const taMatrix& src, int frame) {
  if (!src.geom.IsFrameOf(geom)) return false;
  if ((frame < 0) || (frame >= frames())) return false;
  int n = frameSize();
  int base = FrameStartIdx(frame);
  // if same data types, we use an optimized copy, else must use variants
  // note that "Inherits" should imply same data type
  DataUpdate(true);
  if (GetTypeDef()->InheritsFrom(src.GetTypeDef())) {
    for (int i = 0; i < n; ++i) {
      El_Copy_(FastEl_Flat_(base + i), src.FastEl_Flat_(i));
    }
  } else {
    for (int i = 0; i < n; ++i) {
      El_SetFmVar_(FastEl_Flat_(base + i), src.El_GetVar_(src.FastEl_Flat_(i)));
    }
  }
  DataUpdate(false);

  return true;
}

int taMatrix::defAlignment() const {
  return Qt::AlignRight; // most mats are numeric, so this is the default
}

int taMatrix::Dump_Load_Item(istream& strm, int idx) {
  int c = taMisc::read_till_semi(strm);
  if (c != EOF) {
    SetFmStr_Flat(taMisc::LexBuf, idx);
  }
  return c;
}

int taMatrix::Dump_Load_Value(istream& strm, TAPtr par) {
//NOTE: this code is compatible with early 4.0 beta release which 
// didn't save the members and only saved the data

  int c = taMisc::skip_white(strm);
  if(c == EOF)	return EOF;
  if(c == ';')	return 2;	// signal that its a path
  if(c == '}') {
    if(strm.peek() == ';') strm.get();
    return 2;
  }

  if (c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  // now, load members (if we have dims, will exit at that point)
  int rval = GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
  // 3 is a hacky code to tell us that it got the [ 
  if ((rval != 3) || (rval == EOF)) return rval;
  
  // we now expect dims, if not completely null
  c = taMisc::skip_white(strm);
  if (c == EOF)    return EOF;
  
  if (c == '[') {
    MatrixGeom ar; // temp, while streaming
    if (strm.peek() == '[')
      strm.get(); // actually gets the [
    do {
      c = taMisc::read_word(strm); // also consumes next char, whether sp or ]
      ar.Add(taMisc::LexBuf.toInt());
    } while ((c != ']') && (c != EOF));
    //note: should always be at least one dim if we had [ but we check anyway
    if (ar.size > 0) {
      SetGeomN(ar);
      //note: we always write the correct number, so early termination is an error!
      int i = 0;
      while ((i < size) && (c != EOF)) {
        c = Dump_Load_Item(strm, i++);
      }
    }
  }
  // because we had data, we have to clean up
  c = taMisc::read_till_rbracket(strm);
  if (c==EOF)	return EOF;
  c = taMisc::read_till_semi(strm);
  if (c==EOF)	return EOF;
  return true;
}

void taMatrix::Dump_Save_Item(ostream& strm, int idx) {
  strm << FastElAsStr_Flat(idx);
}

int taMatrix::Dump_Save_Value(ostream& strm, TAPtr par, int indent) {
  // save the members -- it puts the { out
  //int rval = 
  inherited::Dump_Save_Value(strm, par, indent);
  
  // save data, if not completely null
  int i;
  if (geom.size > 0) {
    taMisc::indent(strm, indent);
    // dims
    strm << "[";
    for (i=0; i< geom.size; ++i) {
      if (i > 0) strm << " ";
      strm << geom.FastEl(i);
    }
    strm << "] ";
    // values
    for (i=0; i < size; ++i) {
      Dump_Save_Item(strm, i);
      strm <<  ';';
    }
  }
 
  return true;
}

/////////////////////////////////////
// 	Binary Dump Format

int taMatrix::BinaryLoad_strm(istream& strm) {
  if(TestError(!BinaryFile_Supported(), "BinaryLoad", "Binary load is not supported for this type of matrix")) return false;
  int c = taMisc::skip_white(strm);
  if(c == EOF)	return EOF;
  if(c == ';')	return 2;	// signal that its a path
  if(c == '}') {
    if(strm.peek() == ';') strm.get();
    return 2;
  }

  if (c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  // now, load members (if we have dims, will exit at that point)
  int rval = GetTypeDef()->members.Dump_Load(strm, (void*)this, NULL);
  // 3 is a hacky code to tell us that it got the [ 
  if ((rval != 3) || (rval == EOF)) return rval;
  
  // we now expect dims, if not completely null
  c = taMisc::skip_white(strm);
  if (c == EOF)    return EOF;
  
  if (c == '[') {
    MatrixGeom ar; // temp, while streaming
    if (strm.peek() == '[')
      strm.get(); // actually gets the [
    do {
      c = taMisc::read_word(strm); // also consumes next char, whether sp or ]
      ar.Add(taMisc::LexBuf.toInt());
    } while ((c != ']') && (c != EOF));
    //note: should always be at least one dim if we had [ but we check anyway
    if (ar.size > 0) {
      SetGeomN(ar);
      //note: we always write the correct number, so early termination is an error!
      int i = 0;
      while (i < size) {
        BinaryLoad_Item(strm, i++);
      }
    }
  }
  // because we had data, we have to clean up
  c = taMisc::read_till_rbracket(strm);
  if (c==EOF)	return EOF;
  c = taMisc::read_till_semi(strm);
  if (c==EOF)	return EOF;
  return true;
}

int taMatrix::BinarySave_strm(ostream& strm) {
  if(TestError(!BinaryFile_Supported(), "BinarySave", "Binary save is not supported for this type of matrix")) return false;
  // save the members -- it puts the { out
  //int rval = 
  inherited::Dump_Save_Value(strm, NULL, 0);
  
  // save data, if not completely null
  int i;
  if (geom.size > 0) {
    taMisc::indent(strm, 0);
    // dims
    strm << "[";
    for (i=0; i< geom.size; ++i) {
      if (i > 0) strm << " ";
      strm << geom.FastEl(i);
    }
    strm << "]";
    // values
    for (i=0; i < size; ++i) {
      BinarySave_Item(strm, i);
    }
  }
  strm << endl << "};" << endl;	// terminate
  return true;
}

void taMatrix::BinarySave(const String& fname) {
  if(TestError(!BinaryFile_Supported(), "BinarySave", "Binary save is not supported for this type of matrix")) return;
  taFiler* flr = GetSaveFiler(fname, ".mat", false, "Binary Matrix");
  if (flr->ostrm) {
    BinarySave_strm(*flr->ostrm);
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

void taMatrix::BinaryLoad(const String& fname) {
  if(TestError(!BinaryFile_Supported(), "BinaryLoad", "Binary load is not supported for this type of matrix")) return;
  taFiler* flr = GetLoadFiler(fname, ".mat", false, "Binary Matrix");
  if(flr->istrm)
    BinaryLoad_strm(*flr->istrm);
  flr->Close();
  taRefN::unRefDone(flr);
}

// This is *the* routine for resizing, so all data change ops/tracking
// can go through this
bool taMatrix::EnforceFrames(int n, bool notify) {
  // note: we enforce the size in terms of underlying cells, for when
  // dimensions are changed (even though that is frowned on...)
  if (!AllocFrames(n)) return false; // does legality test
  int new_size = n * frameSize();
  // avoid spurious notifies -- geom changes do their own notify
  if (new_size == size) {
    geom.Set(geom.size-1, n);	
    return true;
  }
  StructUpdate(true);
  if (new_size > size) {
    const void* blank = El_GetBlank_();
    for (int i = size; i < new_size; ++i) {
      El_Copy_(FastEl_Flat_(i), blank);
    }
    size = new_size;
    geom.Set(geom.size-1, n);
  } else if (new_size < size) {
    ReclaimOrphans_(new_size, size - 1);
    size = new_size;
    geom.Set(geom.size-1, n);	
  }
  StructUpdate(false);
  return true;
}

int taMatrix::FastElIndex(int d0, int d1, int d2, int d3, int d4, int d5, int d6) const {
  int rval = -1;
  switch (geom.size) {
  case 0: 
    if(TestError(true, "FastElIndex", "matrix geometry has not been initialized")) return -1;
    break;
  case 1: rval = d0;
    break;
  case 2: rval = (d1 * geom[0]) + d0;
    break;
  case 3: rval = (((d2 * geom[1]) + d1) * geom[0]) + d0;
    break;
  case 4: rval = (((((d3 * geom[2]) + d2) * geom[1]) + d1) * geom[0]) + d0;
    break;
  case 5: rval = (((((((d4 * geom[3]) + d3) * geom[2]) + d2) * geom[1]) + d1) * geom[0]) + d0;
    break;
  default: break;
  }
  return rval;
}
 
int taMatrix::FastElIndex2D(int d0, int d1) const {
  return (d1 * geom[0]) + d0;
}

int taMatrix::FastElIndexN(const MatrixGeom& indices) const {
  int d0 = indices[0];
  int rval = 0;
  for (int i = indices.size - 1 ; i > 0; --i) {
    int di = indices[i];
    rval += di;
    rval *= geom[i-1];
  }
  rval += d0;
  return rval;
}
 
const String taMatrix::FlatRangeToTSV(int row_fr, int col_fr, int row_to, int col_to) {
  if ((row_fr < 0) || (col_fr < 0) || (row_to < row_fr) || (col_to < col_fr))
    return _nilString;
  // allocate a reasonable best-guess buffer
  STRING_BUF(rval, (col_to - col_fr + 1) * (row_to - row_fr + 1) * 10);
  // to access in 2d, you just ignore the higher dimension
  for (int row = row_fr; row <= row_to; ++row) {
    if (row > row_fr) rval.cat('\n');
    int idx = (row * dim(0)) + col_fr; 
    for (int col = col_fr; col <= col_to; ++col, ++idx) {
      if (col > col_fr) rval.cat('\t');
      rval.cat(SafeElAsStr_Flat(idx));
    }
  }
  return rval;
}

int taMatrix::frames() const {
  if (geom.size == 0) return 0;
  return geom[geom.size-1];
}

int taMatrix::frameSize() const {
  if (geom.size == 0) return 0;
  if (geom.size == 1) return 1;
  int rval = geom[0];
  for (int i = 1; i < (geom.size - 1); ++i) 
    rval *= geom[i];
  return rval;
}

int taMatrix::FrameToRow(int f) const {
  // this needs to work even when dims<=1, so we just return f
  if (dims() <= 1) return f;
  // for dims==2, we just return f, else we multiple by higher dims
  for (int i = dims() - 2; i >= 1; --i)
    f *= dim(i);
  return f;
}

taMatrix* taMatrix::GetFrameSlice_(int frame) {
  int dims_m1 = dims() - 1; //cache
  if(TestError((dims_m1 <= 0),"GetFrameSlice_", "dims must be >1 to GetFrameSlice"))
    return NULL;
  int frames_ = frames(); // cache
  // check frame_base and num_frames in bounds
  if(TestError(((frame < 0) || (frame >= frames_)), "GetFrameSlice_",
	       "frame is out of bounds")) return NULL;
    
  taMatrix* rval = (taMatrix*)MakeToken(); // an empty instance of our type
  if(TestError((!rval), "GetFrameSlice_", "could not make token of matrix")) return NULL;
  
  MatrixGeom slice_geom(dims_m1);
  for (int i = 0; i < dims_m1; ++i)
    slice_geom.Set(i, dim(i));
  int sl_i = FrameStartIdx(frame); //note: must be valid because of prior checks
  rval->SetFixedData_(FastEl_Flat_(sl_i), slice_geom);
  // we do all the funky ref counting etc. in one place
  SliceInitialize(this, rval);
  return rval;
}

taMatrix* taMatrix::GetSlice_(const MatrixGeom& base, 
    int slice_frame_dims, int num_slice_frames)
{
  if(TestError((num_slice_frames <= 0), "GetSlice_", "num_slice_frames must be >= 1"))
    return NULL;
  // (note: we check resulting slice dims in bounds later)
  if (slice_frame_dims == -1)
    slice_frame_dims = dims() - 1;
  // check dim size in bounds
  if(TestError((slice_frame_dims < 0) || (slice_frame_dims >= dims()),
	       "GetSlice_", "slice_frame_dims must be >= 0 and < parent Matrix"))
    return NULL;
  // check start cell in bounds and legal
  int sl_i = SafeElIndexN(base); // -1 if out of bounds
  if(TestError((sl_i < 0), "GetSlice_", "slice base is out of bounds")) return NULL;

  // create geom of slice, and copy our dims
  // note that since we are talking in frames, the dims = frames+1
  MatrixGeom slice_geom(slice_frame_dims + 1); // geometry of the resulting slice
  for (int i = 0; i < slice_frame_dims; ++i)
    slice_geom.Set(i, dim(i));
  slice_geom.Set(slice_frame_dims, num_slice_frames);
  
  // easiest to now check for frames in bounds
  int sl_tot = slice_geom.Product();
  if(TestError(((sl_i + sl_tot) > size), "GetSlice_", "slice end is out of bounds"))
    return NULL;
    
  taMatrix* rval = (taMatrix*)MakeToken(); // an empty instance of our type
  if(TestError((!rval), "GetSlice_", "could not make token of matrix")) return NULL;
  
  rval->SetFixedData_(FastEl_Flat_(sl_i), slice_geom);
  // we do all the funky ref counting etc. in one place
  SliceInitialize(this, rval);
  return rval;
}

taMatrix* taMatrix::GetFrameRangeSlice_(int st_frame, int n_frames) {
  MatrixGeom base = geom;
  for (int i = 0; i < dims()-1; ++i)
    base.Set(i, 0);
  base.Set(dims()-1, st_frame);
  return GetSlice_(base, dims()-1, n_frames);
}

bool taMatrix::InRange(int d0, int d1, int d2, int d3, int d4, int d5, int d6) const {
  switch (geom.size) {
  case 0: return false; // not initialized
  case 1: return ((d0 >= 0) && (d0 < geom[0]));
  case 2: return ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]));
  case 3: return ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]));
  case 4: return ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]))
    && ((d3 >= 0) && (d3 < geom[3]));
  case 5: ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]))
    && ((d3 >= 0) && (d3 < geom[3]))
    && ((d4 >= 0) && (d4 < geom[4]));
  default: return false;
  }
}
 
 
bool taMatrix::InRangeN(const MatrixGeom& indices) const {
  if (indices.size < geom.size) return false;
  for (int i = 0; i < indices.size; ++i) {
    int di = indices[i];
    if ((di < 0) || (di >= geom[i])) return false;
  }
  return true;
}
 
void taMatrix::List(ostream& strm) const {
  strm << "["; 
  for (int d = 0; d < dims(); ++d) {
    if (d > 0) strm << ",";
    strm << dim(d);
  }
  strm << "] {";
  int i;
  for(i=0;i<size;i++) {
    strm << " " << El_GetStr_(FastEl_Flat_(i)) << ",";
    if((i+1) % 8 == 0) {
      strm << endl;
      taMisc::FlushConsole();
    }
  }
  strm << "}";
}

ostream& taMatrix::Output(ostream& strm, int indent) const {
  taMisc::indent(strm, indent);
  List(strm);
  strm << ";\n";
  return strm;
}

bool taMatrix::RemoveFrames(int st_fr, int n_fr) {
  if(TestError(!canResize(), "RemoveFrames", "resizing not allowed")) return false;
  int frames_ = frames(); // cache
  if(st_fr < 0) st_fr = frames_ - 1;
  if(TestError(((st_fr < 0) || (st_fr >= frames_)), "RemoveFrames",
	       "starting frame number out of range:", String(st_fr)))
    return false;
  if(n_fr < 0) n_fr = frames_ - st_fr;
  int end_fr = st_fr + n_fr-1;
  if(TestError(((end_fr < 0) || (end_fr >= frames_)), "RemoveFrames",
	       "ending frame number out of range:", String(end_fr)))
    return false;
  // check if we have to copy data
  int frsz = frameSize();
  if(end_fr != (frames_ - 1)) {
    int fm = (end_fr + 1) * frsz;
    int to = st_fr * frsz;
    while (fm < size) {
      El_Copy_(FastEl_Flat_(to), FastEl_Flat_(fm));
      ++fm; ++to;
    }
  }
  // slice updating
  UpdateSlices_FramesDeleted(FastEl_Flat_(st_fr * frsz), n_fr);
  // notifies
  DataUpdate(true);
  // don't notify, because we are doing it (it can't boggle excisions)
  bool rval = EnforceFrames(frames_ - n_fr, false); // this properly resizes, and reclaims orphans
  DataUpdate(false);
  return rval;
}

bool taMatrix::InsertFrames(int st_fr, int n_fr) {
  if(TestError(!canResize(), "InsertFrames", "resizing not allowed")) return false;
  int sz = frames(); // cache
  if(st_fr < 0) st_fr = sz;
  if(TestError((st_fr>sz), "InsertFrames", "starting frame number out of range",
	       String(st_fr))) return false;
  if(TestError((n_fr <= 0), "InsertFrames", "number of frames <= 0")) return false;
  AddFrames(n_fr);
  if(st_fr==sz) {
    return true;		// done!
  }
  int n_mv = sz - st_fr;	// number that must be moved
  sz += n_fr;			// update to added size
  int frsz = frameSize();
  int trg_o = sz-1;
  int src_o = sz-1-n_fr;
  for(int i=0; i<n_mv; i++) {	// shift everyone over
    int trg_o_st = (trg_o - i) * frsz;
    int src_o_st = (src_o - i) * frsz;
    for(int j=0;j<frsz;j++) {
      El_Copy_(FastEl_Flat_(trg_o_st+j), FastEl_Flat_(src_o_st+j));
    }
  }
  // blank out new guys
  const void* blank = El_GetBlank_();
  int st = st_fr * frsz;
  int ed = (st_fr+n_fr) * frsz; 
  for (int i = st; i < ed; ++i) {
    El_Copy_(FastEl_Flat_(i), blank);
  }
  return true;
}

void taMatrix::Reset() {
  EnforceFrames(0);
  UpdateSlices_Collapse();
}

int taMatrix::rowCount() {
  if (dims() <= 1)
    return 1;
  else if (dims() == 2)
    return dim(1);
  else { // more than 2d, return # flat rows
    int rval = dim(1);
    for (int i = dims() - 1; i > 1; --i)
      rval *= dim(i);
    return rval;
  }
}

int taMatrix::SafeElIndex(int d0, int d1, int d2, int d3, int d4, int d5, int d6) const {
  int rval = -1;
  switch (geom.size) {
  case 0: 
    if(TestError(true, "SafeElIndex", "matrix geometry has not been initialized")) return -1;
    break;
  case 1: //note: no extra dim check needed because size is dim
    rval = d0;
    break;
  case 2: 
    if (((d0 >= 0) && (d0 < geom[0]))
      && ((d1 >= 0) && (d1 < geom[1]))
    ) rval = (d1 * geom[0]) + d0;
    break;
  case 3: 
    if (((d0 >= 0) && (d0 < geom[0]))
      && ((d1 >= 0) && (d1 < geom[1]))
      && ((d2 >= 0) && (d2 < geom[2]))
    ) rval = (((d2 * geom[1]) + d1) * geom[0]) + d0;
    break;
  case 4:
    if (((d0 >= 0) && (d0 < geom[0]))
      && ((d1 >= 0) && (d1 < geom[1]))
      && ((d2 >= 0) && (d2 < geom[2]))
      && ((d3 >= 0) && (d3 < geom[3]))
    ) rval = ( ( (((d3 * geom[2]) + d2) * geom[1]) + d1) * geom[0]) + d0;
    break;
  case 5: 
    if (((d0 >= 0) && (d0 < geom[0]))
      && ((d1 >= 0) && (d1 < geom[1]))
      && ((d2 >= 0) && (d2 < geom[2]))
      && ((d3 >= 0) && (d3 < geom[3]))
      && ((d4 >= 0) && (d4 < geom[4]))
    ) rval = (((((((d4 * geom[3]) + d3) * geom[2]) + d2) * geom[1]) + d1) * geom[0]) + d0;
    break;
  default: break;
  }
  if(TestError((rval < 0) || (rval >= size), "SafeElIndex", "matrix indicies out of range")) {
    rval = -1;
  }
  return rval;
}
 
int taMatrix::SafeElIndexN(const MatrixGeom& indices) const {
  if(TestError((geom.size < 1), "SafeElIndexN", "matrix geometry has not been initialized"))
    return -1;
  if(TestError((indices.size < 1), "SafeElIndexN", "at least 1 index must be specified"))
    return -1;
  if(TestError((indices.size > geom.size), "SafeElIndexN", "too many indices for matrix"))
    return -1;
  int d0 = indices[0];
  if(TestError(((d0 < 0) || (d0 >= geom[0])), "SafeElIndexN", "matrix index 0 out of bounds:"))
    return -1;
  
  int rval = 0;
  for (int i = indices.size - 1 ; i > 0; --i) {
    int di = indices[i];
    if(TestError(((di >= 0) && (di < geom[i])), "SafeElIndexN", "matrix index n out of bounds")) return -1;
    rval += di;
    rval *= geom[i-1];
  }
  rval += d0;
  if(TestError((rval >= size), "SafeElIndexN", "matrix element is out of bounds"))
    return -1;
  return rval;
}
 
void taMatrix::SetFixedData_(void* el_, const MatrixGeom& geom_,
    fixed_dealloc_fun fixed_dealloc_) 
{
  // first, clear out any old data, use NULL first to dealloc if needed
  SetArray_(NULL);
  // now, set new data, and the new deallocer
  fixed_dealloc = fixed_dealloc_;
  SetArray_(el_);
  alloc_size = -1; // flag for fixed data
  geom.Reset();
  SetGeomN(geom_);
  
}

void taMatrix::SetGeom_(int dims_, const int geom_[]) {
  String err_msg;
  bool valid = GeomIsValid(dims_, geom_, &err_msg);
  if(TestError(!valid, "SetGeom_", err_msg)) return;
  // dims=0 is special case, which is just a reset
  if (dims_ == 0) {
    StructUpdate(true);
    Reset(); // deletes data, and collapses slices
    geom.Reset(); // ok, now you know what 0-d really means!!!
    StructUpdate(false);
    return;
  }
  
  // flex not allowed for fixed data
  if (isFixedData()) {
    if(TestError((geom_[dims_-1] == 0), "SetGeom_", 
      "fixed data cannot use flex sizing")) return;
  }
  
  // NOTE: following routine is conservative of existing geom
  // if you are using flex sizing, it will collapse any existing!!!
  // only copy bottom N-1 dims, setting 0 frames -- we size frames in next step
  StructUpdate(true);
  // we collapse slices if #dims change, inner dims change, or frames go down
  bool collapse_slices = geom.SetSize(dims_);
  for (int i = 0; i < (dims_ - 1); ++i) {
    if(geom[i] != geom_[i]) {
      collapse_slices = true;
      geom[i] = geom_[i];
    }
  }
  if (geom[dims_ -1] > geom_[dims_ - 1])
    collapse_slices = true;
  
  // assign storage if not fixed
  if (isFixedData()) {
    geom[dims_ -1] = geom_[dims_ - 1];
    size = geom.Product();
  } else {
    // next step actually sets last geom
    EnforceFrames(geom_[dims_-1]); 
  }
  if (collapse_slices) {
    UpdateSlices_Collapse();
  }
  StructUpdate(false);
}

void taMatrix::Slice_Collapse() {
  // called when our referent has become invalid, for whatever reason
  SetArray_(NULL);
  geom.Reset();
  size = 0;
  // if we have collapsed, so have any of our slices...
  UpdateSlices_Collapse();
  DataChanged(DCR_ITEM_UPDATED);
}

void taMatrix::Slice_Realloc(ta_intptr_t base_delta) {
  // called when the base address of our slice has been moved, for whatever reason
  void* mat_el = data(); // cache
  if (mat_el) { // note already collapsed
    SetArray_((void*) (((char*)mat_el) + base_delta));
    // do it recursively
    UpdateSlices_Realloc(base_delta);
  }
  // note: we recursively updated other slices before doing our own notify
  DataChanged(DCR_ITEM_UPDATED);
}

int taMatrix::sliceCount() const {
  if (slices) return slices->size; // note: allowed to be zero
  else        return 0; // hasn't been needed yet
}

void taMatrix::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //NOTE: you are NOT allowed to change geom this way -- must use the SetGeom api call
  if (taMisc::is_loading) {
    UpdateGeom();
  }
}

void taMatrix::UpdateGeom() {
  // NOTE: this routine is ONLY intended for the case of updating after loading
  // from a stream -- no slice checking because there can't really be any yet!
  // handle legacy/graceful case wherein size is non-zero, but no dims -- 
  // set dims to 1, and dim[0] to the size
  if ((size != 0) && (geom.size == 0)) {
    geom.SetSize(1);
    geom.FastEl(0) = size;
  }
  
  // get overall framesize and frames
  int dims_ = geom.size; // cache
  // make sure dims are valid (outer dim can be 0, others must be >0)
  int i;
  for (i = 0; i < (dims_ - 1) ; ++i) {
    if (geom[i] <= 0) {
      taMisc::Error(this->GetPath(), "geoms[0..N-2] must be > 0; object is now invalid");
      return;
    }
  }
  // assign storage (if not fixed) and set size
  if (isFixedData()) {
    size = frames() * frameSize();
  } else {
    EnforceFrames(geom[dims_-1]); // does nothing if outer dim==0
  }
}

void taMatrix::UpdateSlices_Collapse() {
  if (!slices) return;
  for (int i = 0; i < slices->size; ++i) {
    taMatrix* mat = slices->FastEl(i);
    mat->Slice_Collapse();
  }
}

void taMatrix::UpdateSlices_FramesDeleted(void* deletion_base, int num) {
  if (!slices) return;
  // get address of first (old) address beyond the deleted frames
  void* post_deletion = (char*)deletion_base + (num * frameSize() * El_SizeOf_());
  for (int i = 0; i < slices->size; ++i) {
    taMatrix* mat = slices->FastEl(i);
    void* mat_el = mat->data(); // cache
    if (!mat_el) continue; // collapsed
    if (mat_el < deletion_base)
      continue; // earlier than deleted frame, nothing to do
    else if ((mat_el >= deletion_base) && (mat_el < post_deletion))
      mat->Slice_Collapse(); // dude, you so don't exist anymore!
    else { // after the delete point, so fix up
      ta_intptr_t base_delta = (char*)deletion_base - (char*)post_deletion;
      mat->Slice_Realloc(base_delta);
    }
  }
}

void taMatrix::UpdateSlices_Realloc(ta_intptr_t base_delta) {
  if (!slices) return;
  for (int i = 0; i < slices->size; ++i) {
    taMatrix* mat = slices->FastEl(i);
    mat->Slice_Realloc(base_delta); // note, recursively calls realloc
  }
}
 

//////////////////////////
//   String_Matrix	//
//////////////////////////

int String_Matrix::defAlignment() const {
  return Qt::AlignLeft;
}

int String_Matrix::Dump_Load_Item(istream& strm, int idx) {
  int c = taMisc::skip_till_start_quote_or_semi(strm);
  if (c == '\"') {
    c = taMisc::read_till_end_quote_semi(strm);
  }
  if (c != EOF) {
    FastEl_Flat(idx) = taMisc::LexBuf;
  }
  return c;
}

void String_Matrix::Dump_Save_Item(ostream& strm, int idx) {
// note: we don't write "" for empty
  taMisc::write_quoted_string(strm, FastEl_Flat(idx));
}

void String_Matrix::ReclaimOrphans_(int from, int to) {
  for (int i = from; i <= to; ++i) {
    el[i] = _nilString;
  }
}

//////////////////////////
//   float_Matrix	//
//////////////////////////

void float_Matrix::Dump_Save_Item(ostream& strm, int idx) {
  strm << String(FastEl_Flat(idx), "%.8g");
}

bool float_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
#ifdef TA_USE_QT
  str.toFloat(&rval); //discard result
#endif
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid floating point number";
  return rval;
}


//////////////////////////
//   double_Matrix	//
//////////////////////////

void double_Matrix::Dump_Save_Item(ostream& strm, int idx) {
  strm << String(FastEl_Flat(idx), "%.16g");
}

bool double_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
#ifdef TA_USE_QT
  str.toDouble(&rval); //discard result
#endif
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid floating point number";
  return rval;
}


//////////////////////////
//   int_Matrix	//
//////////////////////////

bool int_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
#ifdef TA_USE_QT
  str.toInt(&rval, 0); //auto-base sensing; discard result
#endif
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid integer number";
  return rval;
}


//////////////////////////
//   byte_Matrix	//
//////////////////////////

bool byte_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
  ushort val = 0;
#ifdef TA_USE_QT
  val = str.toUShort(&rval, 0); //auto-base sensing
#endif
  // check for overflow
  if (rval && (val > 255)) rval = false;
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid byte value (0-255)";
  return rval;
}

//////////////////////////
//   Variant_Matrix	//
//////////////////////////

int Variant_Matrix::Dump_Load_Item(istream& strm, int idx) {
  int c;
  Variant& val = FastEl_Flat(idx);
  if (!val.Dump_Load_Type(strm, c)) goto end;
  if (c == EOF) return c;
  
  switch (val.type()) {
  case Variant::T_Invalid: // no content
    c = taMisc::skip_white(strm); // should read ;
    break; 
  case Variant::T_Bool:
  case Variant::T_Int:
  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
  case Variant::T_Double:
    c = taMisc::read_till_semi(strm);
    val.updateFromString(taMisc::LexBuf);
    break;
  case Variant::T_Char: {// reads chars as ints
    c = taMisc::read_till_semi(strm);
    int sc = taMisc::LexBuf.toInt();
    val.setChar((char)sc, val.isNull());
    }
    break;
  
  case Variant::T_String: 
    //note: an empty string was not written
    c = taMisc::skip_till_start_quote_or_semi(strm);
    if (c == '\"') {
      c = taMisc::read_till_end_quote_semi(strm);
      val.updateFromString(taMisc::LexBuf);
    }
    break;
//  case Variant::T_Ptr:  //not streamable
//TODO: maybe we should issue a warning???
//    break;
  
//  case Variant::T_Base: 
//  case Variant::T_Matrix:
//TODO: what is the meaning of streaming a taBase????
// should it be the path??? what if unowned???
//    break;
  
  default: 
    c = taMisc::read_till_semi(strm);
    break;
  }
  
end:
  return c;
}

void Variant_Matrix::Dump_Save_Item(ostream& strm, int idx) {
  const Variant& val = FastEl_Flat(idx);
  val.Dump_Save_Type(strm);
  switch (val.type()) {
  case Variant::T_Invalid: break; // no content
  case Variant::T_Bool:
  case Variant::T_Int:
  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
    strm << ' ' << val.toString();
    break;
  case Variant::T_Double:
    strm << ' ' << String(val.toDouble(), "%.16lg");
    break;
  case Variant::T_Char: // write chars as ints
    strm << ' ' << val.toInt();
    break;
  
  case Variant::T_String: 
    strm << ' ';
    //note: doesn't write empty strings
    taMisc::write_quoted_string(strm, val.toString());
    break;
  case Variant::T_Ptr:  //not streamable
//TODO: maybe we should issue a warning???
    break;
  
  case Variant::T_Base: 
  case Variant::T_Matrix:
//TODO: what is the meaning of streaming a taBase????
// should it be the path??? what if unowned???
    break;
  
  default: 
    break;
  }
}

void Variant_Matrix::ReclaimOrphans_(int from, int to) {
  for (int i = from; i <= to; ++i) {
    el[i] = _nilVariant;
  }
}


//////////////////////////
//   rgb_Matrix	//
//////////////////////////

bool rgb_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
  ushort val = 0;
#ifdef TA_USE_QT
  val = str.toUShort(&rval, 0); //auto-base sensing
#endif
  // check for overflow
  if (rval && (val > 255)) rval = false;
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid byte value (0-255)";
  return rval;
}

const Variant Variant_Matrix::blank;
const String String_Matrix::blank;
const double double_Matrix::blank = 0.0;
const float float_Matrix::blank = 0.0f;
const int int_Matrix::blank = 0;
const unsigned char byte_Matrix::blank = '\0';
const rgb_t rgb_Matrix::blank;

