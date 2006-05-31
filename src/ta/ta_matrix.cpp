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

//////////////////////////
//  MatrixGeom		//
//////////////////////////

MatrixGeom::MatrixGeom(int init_size) {
  Initialize();
  EnforceSize(init_size);
}
  
MatrixGeom::MatrixGeom(int dims, int d0, int d1, int d2, int d3, int d4) {
  Initialize();
  EnforceSize(dims);
  el[0] = d0;
  el[1] = d1;
  el[2] = d2;
  el[3] = d3;
  el[4] = d4;
}

void MatrixGeom::Initialize() {
  size = 0;
}

void MatrixGeom::Destroy() {
#ifdef DEBUG
  // helps detect multi-delete errors
  for (int i = size - 1; i >= 0; el[i--] = 0); 
  size = 0; 
#endif
}

void MatrixGeom::Copy_(const MatrixGeom& cp) {
  EnforceSize(cp.size);
  for (int i = 0; i < size; ++i) {
    el[i] = cp.el[i];
  }
} 

void MatrixGeom::Add(int value) {
  if (size >= TA_MATRIX_DIMS_MAX) return;
  el[size++] = value;
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


void MatrixGeom::EnforceSize(int new_sz) {
  if ((new_sz < 0) || (new_sz >= TA_MATRIX_DIMS_MAX)) return;
  // zero out new elements
  for (int i = size; i < new_sz; ++i)
    el[i] = 0;
  size = new_sz;
}

bool MatrixGeom::Equal(const MatrixGeom& other) const {
  if (size != other.size) return false;
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



//////////////////////////
//  taMatrix		//
//////////////////////////

bool taMatrix::GeomIsValid(int dims_, const int geom_[], String* err_msg) {
  if (dims_ <= 0) { 
    if (err_msg != NULL)
      *err_msg = "dims must be > 0";
    return false;
  }
  
  int i;
  for (i = 0; i < (dims_ - 1) ; ++i) {
    if (geom_[i] <= 0) {
      if (err_msg != NULL)
        *err_msg = "geoms[0..N-2] must be > 0";
      return false;
    }
  }
  
  return true;
}

String taMatrix::GeomToString(const MatrixGeom& geom) {
  String rval("[");
  for (int i = 0; i < geom.size; ++i) {
    if (i > 0) rval += ',';
    rval += i;
  }
  rval += "]";
  return rval;
}

void taMatrix::SliceDestroying(taMatrix* par_slice, const taMatrix* child_slice) {
  taAtomic::Decrement(par_slice->slice_cnt);
#ifdef DEBUG 
  if (par_slice->slice_cnt < 0) {
    taMisc::Error("taMatrix::SliceDestroying slice_cnt < 0, is: ",
      String(par_slice->slice_cnt));
  }
#endif
  taBase::UnRef(par_slice);
}

void taMatrix::SliceInitialize(taMatrix* par_slice, taMatrix* child_slice) {
  taBase::Ref(par_slice);
  taAtomic::Increment(par_slice->slice_cnt);
  child_slice->slice_par = par_slice;
}

void taMatrix::Initialize()
{
  size = 0;
  alloc_size = 0;
  slice_cnt = 0;
  slice_par = NULL;
}
 
void taMatrix::Destroy() {
  size = 0;
  alloc_size = 0;
  CutLinks();
  if (slice_par) {
    SliceDestroying(slice_par, this);
    slice_par = NULL; // helps prevent multi-destroy bugs
  }
#ifdef DEBUG 
  if (slice_cnt != 0) {
    taMisc::Error("taMatrix being destroyed with slice_cnt=", String(slice_cnt));
  }
#endif
}
  
void taMatrix::Add_(const void* it) {
  Check(canResize(), "resizing not allowed");
  Check((dims() == 1), "Add() only allowed when dims=1");
  int idx = frames();
  EnforceFrames(idx + 1);
  El_Copy_(FastEl_(idx), it);
}

void taMatrix::AddFrames(int n) {
  Check(canResize(), "resizing not allowed");
  
  EnforceFrames(n + frames());
}

void taMatrix::Alloc_(int new_alloc) {
  Check((alloc_size >= 0), "cannot alloc a fixed data matrix");
  Check((slice_cnt == 0), "cannot alloc a sliced data matrix");
  
//NOTE: this is a low level allocator; alloc is typically managed in frames
  if (alloc_size < new_alloc)	{
    char* nw = (char*)MakeArray_(new_alloc);
    for (int i = 0; i < size; ++i) {
      El_Copy_(nw + (El_SizeOf_() * i), FastEl_(i));
    }
    SetArray_(nw);
    alloc_size = new_alloc;
  }
}

void taMatrix::AllocFrames(int n) {
  Alloc_(n * frameSize());
}

bool taMatrix::canResize() const {
  return ((alloc_size >= 0) && (slice_cnt == 0));
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
    SetArray_(const_cast<void*>(cp.data()));
    for (int i = 0; i < size; ++i) {
      El_Copy_(FastEl_(i), cp.FastEl_(i));
    }
  }
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
    strm << " " << El_GetStr_(FastEl_(i)) << ",";
  }
  strm << "}";
}

ostream& taMatrix::Output(ostream& strm, int indent) const {
  taMisc::indent(strm, indent);
  List(strm);
  strm << ";\n";
  return strm;
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

void taMatrix::EnforceFrames(int n) {
  Check(canResize(), "resizing not allowed");
  
  // note: we enforce the size in terms of underlying cells, for when
  // dimensions are changed (even though that is frowned on...)
  int new_size = n * frameSize();
  Alloc_(new_size); // makes sure we have enough raw space
  // blank new or reclaim old
  if (new_size > size) {
    const void* blank = El_GetBlank_();
    for (int i = size; i < new_size; ++i) {
      El_Copy_(FastEl_(i), blank);
    }
  } else if (new_size < size) {
    ReclaimOrphans_(new_size, size - 1);
  }
  size = new_size;
  geom.Set(geom.size-1, n);	
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
    
  
  Assert((rval < size), "matrix element is out of bounds");
  if (rval < 0) rval = 0; // 0 is probably the "safest" unsafe value
  return rval;
}
 
int taMatrix::FastElIndexN(const MatrixGeom& indices) const {
  Assert((geom.size >= 1), "matrix geometry has not been initialized");
  Assert((indices.size >= 1), "at least 1 index must be specified");
  Assert((indices.size <= geom.size), "too many indices for matrix");
  int d0 = indices[0];
  Assert(((d0 >= 0) && (d0 < geom[0])), "matrix index out of bounds");
  
  int rval = 0;
  for (int i = indices.size - 1 ; i > 0; --i) {
    int di = indices[i];
    Assert(((di >= 0) && (di < geom[i])), "matrix index out of bounds");
    rval += di;
    rval *= geom[i-1];
  }
  rval += d0;
  Assert((rval < size), "matrix element is out of bounds");
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
  int sl_i = BaseIndexOfFrame(frame); //note: must be valid because of prior checks
  rval->SetFixedData_(FastEl_(sl_i), slice_geom);
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
  if (slice_frame_dims = -1)
    slice_frame_dims = dims() - 1;
  // check dim size in bounds
  Check((slice_frame_dims >= 0) && (slice_frame_dims < (dims() - 1)),
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
  
  rval->SetFixedData_(FastEl_(sl_i), slice_geom);
  // we do all the funky ref counting etc. in one place
  SliceInitialize(this, rval);
  return rval;
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
 
void taMatrix::RemoveFrame(int n) {
  Check(canResize(), "matrix is not resizable");
  int frames_ = frames(); // cache
  Check(((n >= 0) && (n < frames_)), "frame number out of bounds");
  // check if we have to copy data
  if (n != (frames_ - 1)) {
    int fm = n * frameSize();
    int to = fm - frameSize();
    while (fm < size) {
      El_Copy_(FastEl_(to), FastEl_(fm));
      ++fm;
      ++to;
    }
  }
  EnforceFrames(frames_ - 1); // this properly resizes, and reclaims orphans
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
  
  //NOTE: following routine is conservative of existing geom, and will ignore flex sizing if already sized
  // only copy bottom N-1 dims, setting 0 frames -- we size frames in next step
  geom.EnforceSize(dims_);
  for (int i = 0; i < (dims_ - 1) ; ++i) {
    geom[i] = geom_[i];
  }

  // assign storage if not fixed
  if (isFixedData()) {
    geom[dims_-1] = geom_[dims_-1];
  } else {
    // if flex case, we skip this step -- it will stay zero on new, or retain value if data exists
    if (geom_[dims_-1] != 0) {
      // exact value case
      geom[dims_-1] = 0; // next step actually sets
      EnforceFrames(geom_[dims_-1]); // does nothing if outer dim==0
    }
  }
}

void taMatrix::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  UpdateGeom();
}

void taMatrix::UpdateGeom() {
  // handle legacy/graceful case wherein size is non-zero, but no dims -- 
  // set dims to 1, and dim[0] to the size
  if ((size != 0) && (geom.size == 0)) {
    geom.EnforceSize(1);
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

//////////////////////////
//   String_Matrix	//
//////////////////////////

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

