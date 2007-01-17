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


#include "ta_matrix.h"
#include "ta_thread.h"

#ifdef TA_GUI
# include "ta_matrix_qt.h"
# include <QMimeData>
#endif


//////////////////////////
//  CellRange		//
//////////////////////////

void CellRange::SetFromModel(const QModelIndexList& indexes) {
  if (indexes.count() > 0) {
    const QModelIndex& mi = indexes.first();
    row_fr = mi.row();
    col_fr = mi.column();
    if (indexes.count() > 1) {
      const QModelIndex& mi = indexes.last();
      row_to = mi.row();
      col_to = mi.column();
    } else { // note: this prob doesn't happen
      row_to = row_fr;
      col_to = col_fr;
    }
  } else {
    Set(0, 0, -1, -1);
  }
}

//////////////////////////
//  MatrixGeom		//
//////////////////////////

MatrixGeom::MatrixGeom(int init_size) {
  Initialize();
  SetSize(init_size);
}
  
MatrixGeom::MatrixGeom(int dims, int d0, int d1, int d2, int d3, int d4) {
  Initialize();
  SetGeom(dims, d0, d1, d2, d3, d4);
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

int MatrixGeom::IndexFmDims(int d0, int d1, int d2, int d3, int d4) const {
  int rval = -1;
  switch (size) {
  case 0: rval = 0;
    break;
  case 1: rval = d0;
    break;
  case 2: rval = (d1 * el[0]) + d0;
    break;
  case 3: rval = (((d2 * el[1]) + d1) * el[0]) + d0;
    break;
  case 4: rval = (((((d3 * el[2]) + d2) * el[1]) + d1) * el[0]) + d0;
    break;
  case 5: rval = (((((((d4 * el[3]) + d3) * el[2]) + d2) * el[1]) + d1) * el[0]) + d0;
    break;
  default: break;
  }
  return rval;
}

void MatrixGeom::DimsFmIndex(int idx, int& d0, int& d1, int& d2, int& d3, int& d4) const {
  switch (size) {
  case 0: 
    break;
  case 1:
    d0 = idx;
    break;
  case 2:
    d1 = idx / el[0];
    d0 = idx % el[0];
    break;
  case 3:
    d2 = idx / (el[1] * el[0]);
    d1 = (idx % el[1]) / el[0];
    d0 = idx % (el[1] * el[0]);
    break;
  case 4: 
    d3 = idx / (el[2] * el[1] * el[0]);
    d2 = (idx % el[2]) / (el[1] * el[0]);
    d1 = (idx % (el[2] * el[1])) / el[0];
    d0 = idx % (el[2] * el[1] * el[0]);
    break;
  case 5: 
    d4 = idx / (el[3] * el[2] * el[1] * el[0]);
    d3 = (idx % el[3]) / (el[2] * el[1] * el[0]);
    d2 = (idx % (el[3] * el[2])) / (el[1] * el[0]);
    d1 = (idx % (el[3] * el[2] * el[1])) / el[0];
    d0 = idx % (el[3] * el[2] * el[1] * el[0]);
    break;
  default: break;
  }
}

void MatrixGeom::Get2DGeom(int& x, int& y, bool odd_y) const {
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
      y = (FastEl(1) + 1) * FastEl(2); // assume space with 1
    }
    else {
      x = (FastEl(0) + 1) * FastEl(2);
      y = FastEl(1);
    }
  }
  else if(size == 4) { // matrix of 2d guys
    x = (FastEl(0) + 1) * FastEl(2);
    y = (FastEl(1) + 1) * FastEl(3);
  }
  else if(size == 5) { // matrix of 2d guys + time series: vertical
    if(odd_y) {
      x = (FastEl(0) + 1) * FastEl(2);
      y = (FastEl(1) + 1) * FastEl(3) * FastEl(4);
    }
    else {
      x = (FastEl(0) + 1) * FastEl(2) * FastEl(4);
      y = (FastEl(1) + 1) * FastEl(3);
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

void MatrixGeom::SetGeom(int dims, int d0, int d1, int d2, int d3, int d4) {
  SetSize(dims);
  el[0] = d0;
  el[1] = d1;
  el[2] = d2;
  el[3] = d3;
  el[4] = d4;
}



//////////////////////////
//  taMatrix		//
//////////////////////////

bool taMatrix::GeomIsValid(int dims_, const int geom_[], String* err_msg) {
  if ((dims_ < 0) || (dims_ > TA_MATRIX_DIMS_MAX)) { 
    if (err_msg != NULL)
      *err_msg = "dims must be: 0 <= dims <= " + String(TA_MATRIX_DIMS_MAX) + 
        " was: " + String(dims_);
    return false;
  }
  
  // note: for loading and initialization, we accept zero dim values,
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
  }
  
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
#ifdef TA_GUI
  m_dm = NULL;
#endif
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
    taMisc::Error("taMatrix being destroyed with slice_cnt=", String(sliceCount()));
  }
#endif
  if (slices) {
    delete slices;
    slices = NULL;
  }
  if (m_dm) {
    RemoveDataClient(m_dm);
//TODO: needed still?    delete m_dm;
    m_dm = NULL;
  }
}

void taMatrix::Add_(const void* it) {
  Check(canResize(), "resizing not allowed");
  Check((dims() == 1), "Add() only allowed when dims=1");
  int idx = frames();
  EnforceFrames(idx + 1);
  El_Copy_(FastEl_Flat_(idx), it);
}

void taMatrix::AddFrames(int n) {
  Check(canResize(), "resizing not allowed");
  
  EnforceFrames(n + frames());
}

void taMatrix::Alloc_(int new_alloc) {
  Check((alloc_size >= 0), "cannot alloc a fixed data matrix");
//TODO  Check((slice_cnt == 0), "cannot alloc a sliced data matrix");
  
//NOTE: this is a low level allocator; alloc is typically managed in frames
  if (alloc_size < new_alloc)	{
    char* nw = (char*)MakeArray_(new_alloc);
    for (int i = 0; i < size; ++i) {
      El_Copy_(nw + (El_SizeOf_() * i), FastEl_Flat_(i));
    }
    // calculate delta, in bytes, of the new address and update slices
    ta_intptr_t delta = nw - (char*)data();
    UpdateSlices_Realloc(delta);
    // we can now update ourself
    SetArray_(nw);
    alloc_size = new_alloc;
  }
}

void taMatrix::AllocFrames(int n) {
  Alloc_(n * frameSize());
}

bool taMatrix::canResize() const {
  return (alloc_size >= 0);
}


void taMatrix::Copy_(const taMatrix& cp) {
  // note: we must inherit from the copy
  Check(GetTypeDef()->InheritsFrom(cp.GetTypeDef()), "cannot copy " +
    String(GetTypeDef()->name) + " from " + String(cp.GetTypeDef()->name));
    
  // first, zero out current, if any
  SetArray_(NULL);
  if (cp.isFixedData()) {
    SetFixedData_(cp.data(), cp.geom); //sets size
  } else {
    alloc_size = 0;
    size = 0;
    geom.Reset();
    SetGeomN(cp.geom);
    for (int i = 0; i < size; ++i) {
      El_Copy_(FastEl_Flat_(i), cp.FastEl_Flat_(i));
    }
  }
  // notifies
  if (m_dm) {
    m_dm->emit_dataChanged(0, 0, rowCount() - 1, dim(0) - 1);
  }
}

bool taMatrix::CopyFrame(const taMatrix& src, int frame) {
  if (!src.geom.IsFrameOf(geom)) return false;
  if ((frame < 0) || (frame >= frames())) return false;
  int n = frameSize();
  int base = FrameStartIdx(frame);
  // if same data types, we use an optimized copy, else must use variants
  // note that "Inherits" should imply same data type
  if (GetTypeDef()->InheritsFrom(src.GetTypeDef())) {
    for (int i = 0; i < n; ++i) {
      El_Copy_(FastEl_Flat_(base + i), src.FastEl_Flat_(i));
    }
  } else {
    for (int i = 0; i < n; ++i) {
      El_SetFmVar_(FastEl_Flat_(base + i), src.El_GetVar_(src.FastEl_Flat_(i)));
    }
  }
  // notifies
  if (m_dm) {
    m_dm->emit_dataChanged(FrameToRow(frame), 0, FrameToRow(frame + 1) - 1, dim(0) - 1);
  }
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
  int c = taMisc::skip_white(strm);
  if (c == EOF)    return EOF;
  if (c == ';') // just a path
    return 2;  // signal that just a path was loaded..

  
  if (c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  // we expect, but don't require, the [..] dims 
  MatrixGeom ar; // temp, while streaming
  c = taMisc::skip_white(strm, true); // use peek mode, until we're sure
  if (c == '[') {
    strm.get(); // actually gets the [
    do {
      c = taMisc::read_word(strm); // also consumes next char, whether sp or ]
      ar.Add(taMisc::LexBuf.toInt());
    } while ((c != ']') && (c != EOF));
  }
  //note: should always be at least one dim if we had [ but we check anyway
  if (ar.size > 0) {
    SetGeomN(ar);
    //note: we always write the correct number, so early termination is an error!
    int i = 0;
    while ((i < size) && (c != EOF)) {
      c = Dump_Load_Item(strm, i++);
    }
  }
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
  strm << "{ ";
  int i;
  if (geom.size > 0) {
    strm << "[";
    for (i=0; i< geom.size; ++i) {
      if (i > 0) strm << " ";
      strm << geom.FastEl(i);
    }
    strm << "] ";
  }
  for (i=0; i < size; ++i) {
    Dump_Save_Item(strm, i);
    strm <<  ';';
  }
  return true;
}

// This is *the* routine for resizing, so all data change ops/tracking
// can go through this
void taMatrix::EnforceFrames(int n, bool notify) {
  Check(canResize(), "resizing not allowed");
  
  // note: we enforce the size in terms of underlying cells, for when
  // dimensions are changed (even though that is frowned on...)
  int new_size = n * frameSize();
  Alloc_(new_size); // makes sure we have enough raw space
  // blank new or reclaim old
  bool is_1d = (dims() <= 1); // 1d makes the row ops into col ops for notify
  if (new_size > size) {
    // Qt data model notification begin
    bool was_zero = (size == 0);
    bool in_row = false;
    bool in_col = false;
    if (m_dm && notify) {
      if (is_1d) {
        if (was_zero) { 
          m_dm->beginInsertRows(QModelIndex(), 0, 1); 
          in_row = true;
        }
        m_dm->beginInsertColumns(QModelIndex(), size, new_size - 1);
        in_col = true;
      } else { //Nd
        m_dm->beginInsertRows(QModelIndex(), rowCount(), FrameToRow(n) - 1); 
        in_row = true;
        if (was_zero) { 
          m_dm->beginInsertColumns(QModelIndex(), 0, dim(0) - 1);
          in_col = true;
        }
      }
    }
    
    // data change
    const void* blank = El_GetBlank_();
    for (int i = size; i < new_size; ++i) {
      El_Copy_(FastEl_Flat_(i), blank);
    }
    size = new_size;
    geom.Set(geom.size-1, n);
    	
    // notify end
    if (m_dm && notify) {
      if (in_col) m_dm->endInsertColumns();
      if (in_row) m_dm->endInsertRows();
    }
    
  } else if (new_size < size) {
    bool to_zero = (new_size == 0);
    bool rm_row = false;
    bool rm_col = false;
    if (m_dm && notify) {
      if (is_1d) {
        if (to_zero) {
          m_dm->beginRemoveRows(QModelIndex(), 0, 1); 
          rm_row = true;
        }
        m_dm->beginRemoveColumns(QModelIndex(), 0, dim(0) - 1);
        rm_col = true;
      } else { // Nd
        m_dm->beginRemoveRows(QModelIndex(), FrameToRow(n), rowCount() - 1); 
        rm_row = true;
        if (to_zero) {
          m_dm->beginRemoveColumns(QModelIndex(), 0, dim(0) - 1);
          rm_col = true;
        }
      }
    }
    
    ReclaimOrphans_(new_size, size - 1);
    size = new_size;
    geom.Set(geom.size-1, n);	
    
    if (m_dm && notify) {
      if (rm_col) m_dm->endRemoveColumns();
      if (rm_row) m_dm->endRemoveRows();
    }
  }
  else {
    geom.Set(geom.size-1, n);	
  }
  DataChanged(DCR_ITEM_UPDATED);
}

int taMatrix::FastElIndex(int d0, int d1, int d2, int d3, int d4) const {
//TODO: rewrite for extra dims
  int rval = -1;
  switch (geom.size) {
  case 0: Assert(false, "matrix geometry has not been initialized");
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
    
  
//  Assert((rval < size), "matrix element is out of bounds");
//  if (rval < 0) rval = 0; // 0 is probably the "safest" unsafe value
  return rval;
}
 
int taMatrix::FastElIndex2D(int d0, int d1) const {
  return (d1 * geom[0]) + d0;
}

int taMatrix::FastElIndexN(const MatrixGeom& indices) const {
/*  Assert((geom.size >= 1), "matrix geometry has not been initialized");
  Assert((indices.size >= 1), "at least 1 index must be specified");
  Assert((indices.size <= geom.size), "too many indices for matrix");*/
  int d0 = indices[0];
//  Assert(((d0 >= 0) && (d0 < geom[0])), "matrix index out of bounds");
  
  int rval = 0;
  for (int i = indices.size - 1 ; i > 0; --i) {
    int di = indices[i];
//    Assert(((di >= 0) && (di < geom[i])), "matrix index out of bounds");
    rval += di;
    rval *= geom[i-1];
  }
  rval += d0;
//  Assert((rval < size), "matrix element is out of bounds");
  return rval;
}
 
const String taMatrix::FlatRangeToTSV(int row_fr, int col_fr, int row_to, int col_to) {
  if ((row_fr < 0) || (col_fr < 0) || (row_to < row_fr) || (col_to < col_fr))
    return _nilString;
  // allocate a reasonable best-guess buffer
  STRING_BUF(rval, (col_fr - col_to + 1) * (row_fr - row_to + 1) * 10);
  // to access in 2d, you just ignore the higher dimension
  for (int row = row_fr; row <= row_to; ++row) {
    if (row > 0) rval.cat('\n');
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

MatrixTableModel* taMatrix::GetDataModel() {
  if (!m_dm) {
    //shared by all views; persists now till we die; no affect on refcnt
    m_dm = new MatrixTableModel(this);
  }
  return m_dm;
}

taMatrix* taMatrix::GetFrameSlice_(int frame) {
  int dims_m1 = dims() - 1; //cache
  Check((dims_m1 > 0),
    "dims must be >1 to GetFrameSlice");
  int frames_ = frames(); // cache
  // check frame_base and num_frames in bounds
  Check(((frame >= 0) && (frame < frames_)),
    "frame is out of bounds");
    
  taMatrix* rval = (taMatrix*)MakeToken(); // an empty instance of our type
  Check((rval), "could not make token of matrix");
  
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
  Check((num_slice_frames > 0),
    "num_slice_frames must be >= 1");
  // (note: we check resulting slice dims in bounds later)
  if (slice_frame_dims == -1)
    slice_frame_dims = dims() - 1;
  // check dim size in bounds
  Check((slice_frame_dims >= 0) && (slice_frame_dims < dims()),
    "slice_frame_dims must be >= 0 and < parent Matrix");
  // check start cell in bounds and legal
  int sl_i = SafeElIndexN(base); // -1 if out of bounds
  Check((sl_i >= 0),
    "slice base is out of bounds");

  // create geom of slice, and copy our dims
  // note that since we are talking in frames, the dims = frames+1
  MatrixGeom slice_geom(slice_frame_dims + 1); // geometry of the resulting slice
  for (int i = 0; i < slice_frame_dims; ++i)
    slice_geom.Set(i, dim(i));
  slice_geom.Set(slice_frame_dims, num_slice_frames);
  
  // easiest to now check for frames in bounds
  int sl_tot = slice_geom.Product();
  Check(((sl_i + sl_tot) <= size),
    "slice end is out of bounds");
    
  taMatrix* rval = (taMatrix*)MakeToken(); // an empty instance of our type
  Check((rval), "could not make token of matrix");
  
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

bool taMatrix::InRange(int d0, int d1, int d2, int d3, int d4) const {
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

void taMatrix::RemoveFrame(int n) {
  Check(canResize(), "matrix is not resizable");
  int frames_ = frames(); // cache
  Check(((n >= 0) && (n < frames_)), "frame number out of bounds");
  // check if we have to copy data
  if (n != (frames_ - 1)) {
    int fm = (n + 1) * frameSize();
    int to = fm - frameSize();
    while (fm < size) {
      El_Copy_(FastEl_Flat_(to), FastEl_Flat_(fm));
      ++fm;
      ++to;
    }
  }
  // slice updating
  UpdateSlices_FramesDeleted(FastEl_Flat_(n * frameSize()), 1);
  // notifies
  bool is_1d = (dims() <= 1);
  if (m_dm) {
    if (is_1d)
      m_dm->beginRemoveColumns(QModelIndex(), n, n); 
    else
      m_dm->beginRemoveRows(QModelIndex(), FrameToRow(n), FrameToRow(n + 1) - 1); 
  }
  
  // don't notify, because we are doing it (it can't boggle excisions)
  EnforceFrames(frames_ - 1, false); // this properly resizes, and reclaims orphans
  
  if (m_dm) {
    if (is_1d)
      m_dm->endRemoveColumns(); 
    else
      m_dm->endRemoveRows();
  }
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

int taMatrix::SafeElIndex(int d0, int d1, int d2, int d3, int d4) const {
  int rval = -1;
  switch (geom.size) {
  //case 0: Assert(false, "matrix geometry has not been initialized");
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
  if ((rval < 0) || (rval >= size)) {
    rval = -1;
  }
  return rval;
}
 
int taMatrix::SafeElIndexN(const MatrixGeom& indices) const {
  Check((geom.size >= 1), "matrix geometry has not been initialized");
  Check((indices.size >= 1), "at least 1 index must be specified");
  Check((indices.size <= geom.size), "too many indices for matrix");
  int d0 = indices[0];
  Check(((d0 >= 0) && (d0 < geom[0])), "matrix index out of bounds");
  
  int rval = 0;
  for (int i = indices.size - 1 ; i > 0; --i) {
    int di = indices[i];
    Assert(((di >= 0) && (di < geom[i])), "matrix index out of bounds");
    rval += di;
    rval *= geom[i-1];
  }
  rval += d0;
  Check((rval < size), "matrix element is out of bounds");
  return rval;
}
 
void taMatrix::SetFixedData_(void* el_, const MatrixGeom& geom_) {
  // first, clear out any old data
  SetArray_(el_);
  alloc_size = -1; // flag for fixed data
  geom.Reset();
  SetGeomN(geom_);
}

void taMatrix::SetGeom_(int dims_, const int geom_[]) {
  String err_msg;
  bool valid = GeomIsValid(dims_, geom_, &err_msg);
  Check(valid, err_msg);
  
  // NOTE: following routine is conservative of existing geom, and will ignore flex sizing if already sized
  // only copy bottom N-1 dims, setting 0 frames -- we size frames in next step
  bool changed = geom.SetSize(dims_);
  for (int i = 0; i < (dims_ - 1) ; ++i) {
    if(geom[i] != geom_[i]) {
      changed = true;
      geom[i] = geom_[i];
    }
  }

  // assign storage if not fixed
  if (isFixedData()) {
    if(geom[dims_-1] != geom_[dims_-1]) {
      changed = true;
      geom[dims_-1] = geom_[dims_-1];
    }
    if(changed) {
      UpdateSlices_Collapse();
      size = geom.Product();
    }
  } else {
    // if flex case, we skip this step -- it will stay zero on new, or retain value if data exists
    if (geom_[dims_-1] != 0) {
      // exact value case
      geom[dims_-1] = 0; // next step actually sets
      // todo: should use "changed" flag to reset??
      EnforceFrames(geom_[dims_-1]); // does nothing if outer dim==0
    }
  }
  DataChanged(DCR_ITEM_UPDATED);
}

void taMatrix::Slice_Collapse() {
  // called when our referent has become invalid, for whatever reason
  SetArray_(NULL);
  geom.Reset();
  size = 0;
  // if we have collapsed, so have any of our slices...
  UpdateSlices_Collapse();
  DataChanged(DCR_ITEM_UPDATED);
  if (m_dm) 
    m_dm->emit_layoutChanged();
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
  if (m_dm) 
    m_dm->emit_layoutChanged();
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

//////////////////////////////////////////////////////////////////////////////
// 		 class MatrixTableModel
//  The MatrixTableModel provides a 2-d table model for TA Matrix objects.
//  For Gui editing, we map indexes in 2d flat table.

MatrixGeom MatrixTableModel::tgeom; 

MatrixTableModel::MatrixTableModel(taMatrix* mat_) 
:inherited(NULL)
{
  m_mat = mat_;
  if (m_mat)
    m_mat->AddDataClient(this);

}

MatrixTableModel::~MatrixTableModel() {
  if (m_mat) {
    m_mat->m_dm = NULL;
  }
  m_mat = NULL;
}

int MatrixTableModel::columnCount(const QModelIndex& parent) const {
  if (!m_mat) return 0;
  return m_mat->geom.FastEl(0);
}

QVariant MatrixTableModel::data(const QModelIndex& index, int role) const {
  if (!m_mat) return QVariant();
  switch (role) {
  case Qt::DisplayRole: 
  case Qt::EditRole:
    return m_mat->SafeElAsStr_Flat(matIndex(index));
//Qt::DecorationRole
//Qt::ToolTipRole
//Qt::StatusTipRole
//Qt::WhatsThisRole
//Qt::SizeHintRole -- QSize
//Qt::FontRole--  QFont: font for the text
  case Qt::TextAlignmentRole:
    return m_mat->defAlignment();
/*Qt::BackgroundColorRole -- QColor
  but* only used when !(option.showDecorationSelected && (option.state & QStyle::State_Selected))
Qt::TextColorRole
  QColor: color of text
Qt::CheckStateRole*/
  default: return QVariant();
  }
}

void MatrixTableModel::DataLinkDestroying(taDataLink* dl) {
  delete this;
}

void MatrixTableModel::DataDataChanged(taDataLink* dl, int dcr,
  void* op1, void* op2)
{
  //this is primarily for code-driven changes
  if (dcr == DCR_ITEM_UPDATED) {
    emit_dataChanged();
  }
}


void MatrixTableModel::emit_dataChanged(int row_fr, int col_fr, int row_to, int col_to) {
  if (!m_mat) return;
  // lookup actual end values when we are called with sentinels
  if (row_to < 0) row_to = rowCount() - 1;
  if (col_to < 0) col_to = columnCount() - 1;  
  
  emit dataChanged(createIndex(row_fr, col_fr), createIndex(row_to, col_to));
}

void MatrixTableModel::emit_layoutChanged() {
  emit layoutChanged();
}

Qt::ItemFlags MatrixTableModel::flags(const QModelIndex& index) const {
  if (!m_mat) return 0;
  //TODO: maybe need to qualify!, plus drag-drop handling, etc.
  Qt::ItemFlags rval = 0;
  
  if (ValidateIndex(index)) {
    rval = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
  }
  return rval; 
}
/*
  index = i0 + ((i1*d0) + i2)*d1 etc.
to find i1,i2... from index:
1. divide by d0 gives rowframe -- remainder is i1
2. divide by d1 gives 2d-frame
*/
QVariant MatrixTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();
//TODO: make the headers show the dimenion # when > 2
  if (orientation == Qt::Horizontal)
    return QString::number(section); //QString("%1").arg(section);
  else {// in form: d1[[:d2]:d3]
    if (m_mat->dims() <= 2)
      return QString::number(section);
    else {
      div_t r;
      String rval;
      // find each nextmost dim n by doing modulo remaining dim[n-1]
      for (int i = 1; i < (m_mat->dims() - 1); ++i) {
        r = div(section, m_mat->dim(i)); 
        section = r.quot;
        if (i > 1) rval = ":" + rval;
        rval = String(r.rem) + rval; 
      }
      rval = String(section) + ":"  + rval; 
      return rval;
    }
  }
}

/*QModelIndex MatrixTableModel::index(int row, int column, const QModelIndex &parent) const
{
  // client passes invalid parent to request top-level index, otherwise it is nested
  if (parent.isValid())
  else { // returning top-level index
    // we only 
    return hasIndex(row, column, parent) ? createIndex(row, column, 0) : QModelIndex();
  }
}*/
int MatrixTableModel::matIndex(const QModelIndex& idx) const {
  //TENT
  //note: we dimensionally reduce all dims >1 to 1
  return (idx.row() * m_mat->dim(0)) + idx.column();
//  return m_mat->SafeElIndex(idx.column(), idx.row());
}

/*QModelIndex MatrixTableModel::parent(const QModelIndex& mi) const
{
    return QModelIndex();
}

bool QAbstractTableModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid())
        return false;
    return rowCount() > 0 && columnCount() > 0;
}*/


QMimeData* MatrixTableModel::mimeData (const QModelIndexList& indexes) const {
  if (!m_mat) return NULL;
  CellRange cr(indexes);
  String str = mat()->FlatRangeToTSV(cr);
  QMimeData* rval = new QMimeData;
  rval->setText(str);
  return rval;
}

QStringList MatrixTableModel::mimeTypes () const {
  QStringList types;
  types << "text/plain";
  return types;
}


int MatrixTableModel::rowCount(const QModelIndex& parent) const {
  return m_mat->rowCount();
  //note: for visual stuff, there is always at least one row
}

bool MatrixTableModel::setData(const QModelIndex& index, const QVariant & value, int role) {
  if (!m_mat) return false;
  if (index.isValid() && role == Qt::EditRole) {
    m_mat->SetFmStr_Flat(value.toString(), matIndex(index));
    emit dataChanged(index, index);
    return true;
  }
  return false;
}

bool MatrixTableModel::ValidateIndex(const QModelIndex& index) const {
  // TODO: maybe need to check bounds???
  return (m_mat);
}

bool MatrixTableModel::ValidateTranslateIndex(const QModelIndex& index, MatrixGeom& tr_index) const {
  bool rval = ValidateIndex(index);
  if (rval) {
    // TODO:
  }
  return rval;
}

