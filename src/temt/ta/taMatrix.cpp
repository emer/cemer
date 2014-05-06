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

#include "taMatrix.h"

#include <int_Matrix>
#include <MatrixIndex>
#include <iMatrixTableModel>
#include <ColorScale>
#include <CellRange>
#include <taMatrix_PList>
#include <byte_Matrix>
#include <int_Matrix>
#include <String_Matrix>
#include <float_Matrix>
#include <double_Matrix>
#include <taBaseItr>
#include <taFiler>
#include <MTRnd>
#include <taArray_base>

#include <taMisc>

#include <cmath>

TA_BASEFUNS_CTORS_DEFN(taMatrix);
SMARTREF_OF_CPP(taMatrix);


using namespace std;

bool taMatrix::GeomIsValid(int dims_, const int geom_[],
  String* err_msg, bool allow_flex)
{
  if ((dims_ <= 0) || (dims_ >= TA_MATRIX_DIMS_MAX)) {
    if (err_msg)
      *err_msg = "dims must be: 0 < dims <= " + String(TA_MATRIX_DIMS_MAX) +
        " was: " + String(dims_);
    return false;
  }

  // we only allow a non-zero value in top dim (flex sizing, if enabled)
  for (int i = 0; i < (dims_-1) ; ++i) {
    if (geom_[i] < 0) {
      if (err_msg)
        *err_msg = "geoms must be >= 0";
      return false;
    }
    else if (geom_[i] == 0) {
      if (!(allow_flex && (i == (dims_ - 1)))) {
        if (err_msg)
          *err_msg = "all but highest-most geom must be >0";
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

void taMatrix::Initialize() {
  size = 0;
  alloc_size = 0;
  slices = NULL;
  slice_par = NULL;
  fixed_dealloc = NULL;
  table_model = NULL;
  colorscale = NULL;
  el_view_mode = IDX_UNK;
}

void taMatrix::Destroy() {
  CutLinks();
  size = 0;
  alloc_size = 0;
  if (slice_par) {
    SliceDestroying(slice_par, this);
    slice_par = NULL;
  }
  if (sliceCount() > 0) {
    DebugInfo("Destroy", "taMatrix being destroyed with slice_cnt=", String(sliceCount()));
  }
  if (slices) {
    delete slices;
    slices = NULL;
  }
  if (colorscale) {
    delete colorscale;
    colorscale = NULL;
  }
  if (table_model) {
    //note: hopefully dudes won't call us back during this!
    delete table_model;
    table_model = NULL;
  }
}

void taMatrix::InitLinks() {
  inherited::InitLinks();
  taBase::Own(geom, this);
  taBase::Own(el_view, this);
}

void taMatrix::CutLinks() {
  el_view.CutLinks();
  el_view_mode = IDX_UNK;
  geom.CutLinks();
  el_view.CutLinks();
  inherited::CutLinks();
}

void taMatrix::BatchUpdate(bool begin, bool struc) {
  inherited::BatchUpdate(begin, struc);
  // recursively send blocked updates to slices
  if (slices) for (int i = 0; i < slices->size; ++i) {
    taMatrix* mat = slices->FastEl(i);
    mat->BatchUpdate(begin, struc); // recursive, of course
  }
}

String& taMatrix::Print(String& strm, int indent) const {
  if(IsSingleElemView()) {      // if just a single guy, that's all we print
    TA_FOREACH(vitm, *this) {
      strm << vitm.toString();
      return strm;
    }
  }
  const int dm = dims();
  int dim_break = dm / 2;
  if(dm == 2) {
    float dim_rat = (float)dim(1) / dim(0);
    if(dim_rat > 2.0)           // if inner ratio is small, then keep it inline
      dim_break = dm;
  }
  else if(dm % 2 == 1) {
    dim_break++;                // one more outer is probably better for odd
  }

  taMatrix* elv = ElView();
  MatrixIndex idx(dm);
  MatrixIndex lstidx(dm,0,0,0,0,0,0,0);

  int vis_size = IterCount();   // visible size
  
  if(vis_size > 500) {
    strm << GetTypeDef()->name + " geom: ";
    geom.Print(strm);
    return strm;
  }

  for(int d=0; d<dm; d++) {
    strm += "[";
  }
  strm += " ";
  if(!elv || el_view_mode == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(elv);
    for(int i=0; i<size; i++) {
      geom.DimsFmIndex(i, idx);
      int sc = 0;
      int break_level = -1;
      for(int d=0; d<dm; d++) {
        if(idx[d] == 0 && idx[d] != lstidx[d]) {
          sc++;
          if(sc == 1) strm += " ";
          strm += "]";          // end previous
          if(d+1 == dim_break) {
            break_level = d+1;
          }
        }
      }
      if(break_level >= 0) {
        strm += "\n" + String(dm-break_level, 0, ' ');
      }
      for(int s=0; s<sc; s++) {
        strm += "[";            // start new
        if(s == sc-1) strm += " ";
      }
      if(sc == 0 && i > 0) {
        strm += ", ";
      }
      if(cmat && !((bool)cmat->SafeEl_Flat(i))) {
        strm += "- ";           // filtered
      }
      else {
        strm += FastElAsStr_Flat(i);
      }
      lstidx = idx;             // update
    }
    strm += " ";
    for(int d=0; d<dm; d++) {
      strm += "]";
    }
  }
  else if(el_view_mode == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    int nc = cmat->dim(1);
    strm = "[ ";
    for(int i=0; i<nc; i++) {
      for(int d=0;d<dm;d++) {
        idx.Set(d, cmat->FastEl2d(d, i)); // outer index is count index
      }
      idx.Print(strm) << ": " << SafeElAsStrN(idx);
      if(i < nc-1)
        strm += ", ";
    }
    strm += " ]";
  }
  else if(el_view_mode == IDX_FRAMES) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    int nc = cmat->size;
    int fsz = FrameSize();
    int ic = 0;
    for(int i=0; i<nc; i++) {
      for(int j=0; j<fsz; j++, ic++) {
        geom.DimsFmIndex(ic, idx);
        int sc = 0;
        int break_level = -1;
        for(int d=0; d<dm; d++) {
          if(idx[d] == 0 && idx[d] != lstidx[d]) {
            sc++;
            if(sc == 1) strm += " ";
            strm += "]";          // end previous
            if(d+1 == dim_break) {
              break_level = d+1;
            }
          }
        }
        if(break_level >= 0) {
          strm += "\n" + String(dm-break_level, 0, ' ');
        }
        for(int s=0; s<sc; s++) {
          strm += "[";            // start new
          if(s == sc-1) strm += " ";
        }
        if(sc == 0 && ic > 0) {
          strm += ", ";
        }
        int bi = FrameStartIdx(i) + j;
        strm += SafeElAsStr_Flat(bi);
        lstidx = idx;             // update
      }
    }
    strm += " ";
    for(int d=0; d<dm; d++) {
      strm += "]";
    }
  }
  return strm;
}

//////////////////////////////////////////////
//      std accessor interface

bool taMatrix::SetElView(taMatrix* view_mat, IndexMode md) {
  int dm = dims();
  if(!IterValidate(view_mat, md, dm)) return false;
  el_view = view_mat;
  el_view_mode = md;
  return true;
}

void taMatrix::CopyElView(const taMatrix& cp) {
  // skip the validation from SetElView -- assume already validated
  el_view = cp.el_view;
  el_view_mode = cp.el_view_mode;
}

void taMatrix::ClearElView() {
  el_view = NULL;
}

taMatrix* taMatrix::NewElView(taMatrix* view_mat, IndexMode md) const {
  int dm = dims();
  if(!IterValidate(view_mat, md, dm)) return NULL;
  taMatrix* rval = (taMatrix*)MakeToken(); // make a token of me
  void* base_el = const_cast<void*>(FastEl_Flat_(0));
  rval->SetFixedData_(base_el, geom);      // identical geom, same data
  SliceInitialize(const_cast<taMatrix*>(this), rval);
  if((bool)el_view) {
    // has an existing view -- now we need to compile the new view from that
    if(el_view_mode == IDX_COORDS) {
      TestWarning(true, "NewElView",
                  "existing el_view_mode is IDX_COORDS -- cannot mix with any new mode -- will only show the new mode and ignore the existing mask");
      rval->SetElView(view_mat, md);
    }
    else if(el_view_mode == IDX_MASK) {
      if(md == IDX_MASK) {
        // take intersection of the existing mask
        byte_Matrix* am = (byte_Matrix*)(*view_mat && *ElView());
        rval->SetElView(am, md);
      }
      else {
        TestWarning(true, "NewElView",
                    "existing el_view_mode is IDX_MASK -- cannot mix with new mode that is not also a MASK -- will only show the new mode and ignore the existing mask");
        
        rval->SetElView(view_mat, md);
      }
    }
    else if(el_view_mode == IDX_FRAMES) {
      if(md == IDX_COORDS) {    // 
        int_Matrix* nwvw = new int_Matrix; // copy orig
        nwvw->Copy(view_mat);
        int nc = view_mat->dim(1);
        delete view_mat;        // now done with original
        for(int i=nc-1; i>=0; i--) {
          int& fn = nwvw->FastEl2d(FrameDim(), i); // get the frame index
          if(fn < 0) fn = Frames() + fn;
          if(!FrameInRange(fn, false)) {
            nwvw->RemoveFrames(i, 1);
          }
          else {
            fn = FrameIdx(fn);                // pass through current logical
          }
        }
        TestError(nwvw->size == 0, "NewElView",
                  "no valid frame coordinates provided for new IDX_COORDS view of existing IDX_FRAMES view of matrix");
        rval->SetElView(nwvw, md);
      }
      else if(md == IDX_FRAMES) {
        int_Matrix* nwvw = new int_Matrix; // copy orig
        nwvw->Copy(view_mat);
        delete view_mat;        // now done with original
        for(int i=nwvw->size-1; i>=0; i--) {
          int& fn = nwvw->FastEl_Flat(i);
          if(fn < 0) fn = Frames() + fn;
          if(!FrameInRange(fn, false)) {
            nwvw->RemoveFrames(i, 1);
          }
          else {
            fn = FrameIdx(fn);                // pass through current logical
          }
        }
        TestError(nwvw->size == 0, "NewElView",
                  "no valid frame coordinates provided for new IDX_FRAMES view of existing IDX_FRAMES view of matrix");
        rval->SetElView(nwvw, md);
      }
      else {
        String nw_vw = TA_taMatrix.GetEnumString("IndexMode", md);
        TestWarning(true, "NewElView",
                    "existing el_view_mode is IDX_FRAMES -- cannot mix with new mode:",
                    nw_vw, "that is not IDX_COORDS -- will only show the new mode and ignore the existing mask");
        rval->SetElView(view_mat, md);
      }
    }
  }
  else {
    rval->SetElView(view_mat, md);
  }
  return rval;
}

Variant taMatrix::ElemFmCoord(int_Matrix* cmat) const {
  MatrixIndex idx;
  int dm = dims();
  idx.SetDims(dm);              // set it to our dimensionality
  int mx = MIN(dm, cmat->size);
  int i;
  for(i=0; i<mx; i++) {
    int el_idx = cmat->FastEl_Flat(i);
    if(el_idx < 0) el_idx += dim(i);
    if(el_idx < 0 || el_idx >= dim(i)) {
      return _nilVariant;       // out of bounds!
    }
    idx.Set(i, el_idx);
  }
  for(;i<dm;i++) {              // fill in remaining indicies with 0
    idx.Set(i, 0);
  }
  return SafeElAsVarN(idx);
}

Variant taMatrix::Elem(const Variant& idx, IndexMode mode) const {
  const int dm = dims();
  if(mode == IDX_UNK) {
    mode = IndexModeDecode(idx, dm);
    if(mode == IDX_UNK) return _nilVariant;
  }
  if(!IndexModeValidate(idx, mode, dm))
    return _nilVariant;
  switch(mode) {
  case IDX_IDX: {
    int dx = idx.toInt();
    if(IdxFrameView()) {
      return SafeElAsVar_Flat(FrameViewFlatIdx(dx));
    }
    else {
      if(dx < 0) dx += size;    // wrap around
      return SafeElAsVar_Flat(dx);
    }
    break;
  }
  case IDX_NAME: {
    const String& nm = idx.toString();
    int_Matrix* imat = new int_Matrix(1,0); // return indices
    TA_FOREACH(vitm, *this) { // use iterator so it is recursive on existing filtering
      String val = vitm.toString();
      if(val.matches_wildcard(nm)) {
        imat->Add(FOREACH_itr.el_idx); // add absolute index of item
      }
    }
    if(imat->size == 1) {
      int j = imat->FastEl_Flat(0);
      delete imat;
      return (Variant)j;
    }
    return (Variant)imat;
  }
  case IDX_NAMES: {
    String_Matrix* cmat = dynamic_cast<String_Matrix*>(idx.toMatrix());
    int_Matrix* imat = new int_Matrix(1,0);
    TA_FOREACH(vitm, *this) {   // use iterator so it is recursive on existing filtering
      String val = vitm.toString();
      int el_idx = FOREACH_itr.el_idx; // get before occluded by next iterator
      TA_FOREACH(mitm, *cmat) { // use iterator on matrix so it can be filtered too
        const String nm = mitm.toString();
        if(val.matches_wildcard(nm)) {
          imat->Add(el_idx); // add absolute index of item
        }
      }
    }
    if(imat->size == 1) {
      int j = imat->FastEl_Flat(0);
      delete imat;
      return (Variant)j;
    }
    return (Variant)imat;
  }
  case IDX_MISC: {
    TestError(true, "Elem::IDX_MISC", "index type not support for Matrix");
    return _nilVariant;
    break;
  }
  case IDX_COORD: {
    // always return a matrix so that assignment works!
    // this means: assigning a value to a matrix element -- if we return a Variant of the
    // element, then something like mymat[idx] = 22 will fail to assign new value..
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    int_Matrix* ccmat = new int_Matrix;
    MatrixGeom ng = cmat->geom;
    ng.AddDim(1);
    ccmat->SetGeomN(ng);
    for(int i=0; i<cmat->size; i++) {
      ccmat->FastEl_Flat(i) = cmat->FastEl_Flat(i);
    }
    // return ElemFmCoord(cmat);
    taMatrix* nwvw = NewElView(ccmat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_COORDS: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    taMatrix* nwvw = NewElView(cmat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_FRAMES: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    taMatrix* nwvw = NewElView(cmat, IDX_FRAMES);
    return (Variant)nwvw;
    break;
  }
  case IDX_SLICE: {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    int_Matrix fixsmat;
    fixsmat.SetGeom(2,3,dm); // fixed slice matrix + total n
    MatrixGeom sliceg(dm);              // slice geometry
    for(int i=0;i<dm; i++) {
      int start = 0; int end = -1; int step = 1;
      if(i < cmat->dim(1)) {
        start = cmat->FastEl2d(0,i);
        end = cmat->FastEl2d(1,i);
        step = cmat->FastEl2d(2,i);
      }
      if(step == 0) step = 1;
      if(FixSliceValsFromSize(start, end, dim(i))) {
        int my_n = (end-start) / ABS(step); // number of guys in my slice
        sliceg.Set(i, my_n);
        fixsmat.FastEl2d(0,i) = start;
        fixsmat.FastEl2d(1,i) = end;
        fixsmat.FastEl2d(2,i) = step;
      }
      else {
        return _nilVariant;
      }
    }
    int tot_n = sliceg.Product();
    if(TestError(tot_n <= 0, "Elem", "slicing total was empty")) {
      return _nilVariant;
    }
    int_Matrix* imat = new int_Matrix(2,dm,tot_n); // turn into coords list
    MatrixIndex sidx;                              // slice idx
    for(int i=0;i<tot_n; i++) {
      sliceg.DimsFmIndex(i, sidx); // get index into slice vals
      for(int d=0; d<dm; d++) {
        int start = fixsmat.FastEl2d(0,d);
        int end = fixsmat.FastEl2d(1,d);
        int step = fixsmat.FastEl2d(2,d);
        int sc;
        if(step > 0) {
          sc = start + step * sidx[d];
        }
        else {
          sc = end-1 + step * sidx[d];
        }
        imat->FastEl2d(d, i) = sc;
      }
    }
    taMatrix* nwvw = NewElView(imat, IDX_COORDS);
    return (Variant)nwvw;
    break;
  }
  case IDX_MASK: {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(idx.toMatrix());
    if(TestError(cmat->geom != geom, "Elem::IDX_MASK",
                 "mask matrix geometry:", String(cmat->dim(0)),
                 "is not size of list:", String(size)))
      return false;
    taMatrix* nwvw = NewElView(cmat, IDX_MASK);
    return (Variant)nwvw;
    break;
  }
  case IDX_UNK:
    break;
  }
  return _nilVariant;
}

// use this throughout css to make it act like a scalar
bool taMatrix::IsSingleElemView() const {
  if(!ElView()) return false;
  if(el_view_mode == IDX_COORDS) {
    return (IterCount() == 1);
  }
  else if(el_view_mode == IDX_FRAMES) {
    return (IterCount() == 1 && FrameSize() == 1);
  }
  return false;
}

Variant taMatrix::IterElem(taBaseItr& itr) const {
  if(itr.Done()) return _nilVariant;
  return SafeElAsVar_Flat(itr.el_idx);
}

bool taMatrix::IterValidate(taMatrix* vmat, IndexMode mode, int cont_dims) const {
  bool rval = inherited::IterValidate(vmat, mode, cont_dims);
  if(!rval) return false;
  if(!vmat) return true;
  if(el_view_mode == IDX_MASK) {
    if(TestError(ElView()->geom != geom, "IterValidate::IDX_MASK",
                 "el_view geom:", ElView()->geom.ToString(),
                 "not equal to size of matrix:",
                 geom.ToString()))
      return false;
  }
  return true;
}

bool taMatrix::IterFirst_impl(taBaseItr& itr) const {
  itr.count = 0;
  itr.el_idx = 0;              // just to be sure
  if(!ElView()) {
    if(ElemCount() > 0) return true;
    itr.SetDone();
    return false;
  }
  const int dm = dims();
  if(!IterValidate(ElView(), el_view_mode, dm)) {
    itr.SetDone();
    return false;
  }
  if(el_view_mode == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(!cmat || cmat->size == 0) {
      itr.SetDone();
      return false;
    }
    MatrixIndex idx(dm);
    for(int d=0;d<dm;d++) {
      idx.Set(d, cmat->FastEl2d(d, 0));   // outer index is count index
    }
    itr.el_idx = SafeElIndexN(idx);
    if(itr.el_idx < 0 || itr.el_idx >= ElemCount()) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(el_view_mode == IDX_FRAMES) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(!cmat || cmat->size == 0) {
      itr.SetDone();
      return false;
    }
    itr.el_idx = FrameStartIdx(0); // frame is logical -- startidx does decoding!
    if(itr.el_idx < 0 || itr.el_idx >= ElemCount()) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(el_view_mode == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=0; i<ElemCount(); i++) {
      if(!cmat || cmat->FastEl_Flat(i) != 0) {
        itr.el_idx = i;
        return true;
      }
    }
  }
  itr.SetDone();
  return false;
}

bool taMatrix::IterNext_impl(taBaseItr& itr) const {
  itr.count++;
  if(!ElView()) {
    itr.el_idx++;
    if(itr.el_idx >= ElemCount()) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  const int dm = dims();
  if(el_view_mode == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(!cmat || cmat->dim(1) <= itr.count) {
      itr.SetDone();
      return false;
    }
    MatrixIndex idx(dm);
    for(int d=0;d<dm;d++) {
      idx.Set(d, cmat->FastEl2d(d, itr.count));  // outer index is count index
    }
    itr.el_idx = SafeElIndexN(idx);
    if(itr.el_idx < 0 || itr.el_idx >= ElemCount()) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(el_view_mode == IDX_FRAMES) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    int fsz = MAX(FrameSize(), 1);
    int fr = itr.count / fsz;
    if(!cmat || fr >= cmat->size) {
      itr.SetDone();
      return false;
    }
    int cell = itr.count % fsz;
    itr.el_idx = FrameStartIdx(fr) + cell; // fr is still logical here
    if(itr.el_idx < 0 || itr.el_idx >= ElemCount()) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(el_view_mode == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=itr.el_idx+1; i<ElemCount(); i++) { // search for next
      if(!cmat || cmat->FastEl_Flat(i) != 0) { // true
        itr.el_idx = i;
        return true;
      }
    }
  }
  itr.SetDone();
  return false;
}

// reverse iterator versions

bool taMatrix::IterLast_impl(taBaseItr& itr) const {
  itr.count = 0;
  itr.el_idx = 0;              // just to be sure
  const int ec = ElemCount();
  if(!ElView()) {
    if(ec > 0) {
      itr.el_idx = ec-1;
      return true;
    }
    itr.SetDone();
    return false;
  }
  const int dm = dims();
  if(!IterValidate(ElView(), el_view_mode, dm)) {
    itr.SetDone();
    return false;
  }
  if(el_view_mode == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(!cmat || cmat->size == 0) {
      itr.SetDone();
      return false;
    }
    MatrixIndex idx(dm);
    int last_coord = cmat->Frames()-1;
    for(int d=0;d<dm;d++) {
      idx.Set(d, cmat->FastEl2d(d, last_coord));   // outer index is count index
    }
    itr.el_idx = SafeElIndexN(idx);
    if(itr.el_idx < 0 || itr.el_idx >= ec) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(el_view_mode == IDX_FRAMES) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(!cmat || cmat->size == 0) {
      itr.SetDone();
      return false;
    }
    itr.el_idx = FrameStartIdx(cmat->size-1); // frame is logical -- startidx does decoding!
    itr.el_idx += FrameSize()-1;              // last cell in frame
    if(itr.el_idx < 0 || itr.el_idx >= ec) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(el_view_mode == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=ec-1; i>=0; i--) {
      if(!cmat || cmat->FastEl_Flat(i) != 0) {
        itr.el_idx = i;
        return true;
      }
    }
  }
  itr.SetDone();
  return false;
}

bool taMatrix::IterPrev_impl(taBaseItr& itr) const {
  itr.count++;
  const int ec = ElemCount();
  if(!ElView()) {
    itr.el_idx--;
    if(itr.el_idx < 0) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  const int dm = dims();
  if(el_view_mode == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(!cmat || cmat->dim(1) <= itr.count) {
      itr.SetDone();
      return false;
    }
    MatrixIndex idx(dm);
    int last_coord = cmat->Frames()-1-itr.count;
    for(int d=0;d<dm;d++) {
      idx.Set(d, cmat->FastEl2d(d, last_coord));  // outer index is count index
    }
    itr.el_idx = SafeElIndexN(idx);
    if(itr.el_idx < 0 || itr.el_idx >= ec) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(el_view_mode == IDX_FRAMES) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    int fsz = MAX(FrameSize(), 1);
    int fr = itr.count / fsz;
    if(!cmat || fr >= cmat->size) {
      itr.SetDone();
      return false;
    }
    int cell = itr.count % fsz;
    itr.el_idx = FrameStartIdx(cmat->size - 1 - fr);
    itr.el_idx += fsz-1-cell;
    if(itr.el_idx < 0 || itr.el_idx >= ec) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(el_view_mode == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=ec - 2 - itr.el_idx; i>=0; i--) { // search for prev
      if(!cmat || cmat->FastEl_Flat(i) != 0) { // true
        itr.el_idx = i;
        return true;
      }
    }
  }
  itr.SetDone();
  return false;
}

void taMatrix::Add_(const void* it) {
  if(TestError(!canResize(), "Add", "resizing not allowed")) return;
  if(TestError((dims() != 1), "Add", "only allowed when dims=1")) return;
  int idx = Frames();
  if(EnforceFrames(idx + 1))
    El_Copy_(FastEl_Flat_(idx), it);
}

bool taMatrix::AddFrames(int n) {
  return EnforceFrames(n + Frames());
}

bool taMatrix::Alloc_(int new_alloc) {
  if(TestError((alloc_size < 0), "Alloc_", "cannot alloc a fixed data matrix")) return false;
  // TODO  Check((slice_cnt == 0), "cannot alloc a sliced data matrix");
  // NOTE: this is a low level allocator; alloc is typically managed in frames
  if (alloc_size < new_alloc)   {
    if(fastAlloc() && (alloc_size > 0)) {               // fast alloc = malloc/realloc
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
  if(TestWarning(ElView(), "AllocFrames",
                 "An el_view is currently set -- it is being cleared for alloc")) {
    ClearElView();
  }
  int frsz = FrameSize();
  if(frsz == 0) return false;           // not dimensioned yet -- don't bother
  int cur_n = alloc_size / frsz;
  if(cur_n >= n) return true;   // already sufficient!

  int act_n = n;                // actual n frames to request
  // start w/ 4, double up to 64, then 1.5x thereafter
  if (cur_n == 0) act_n = MAX(4, act_n);
  else if (cur_n < 64) act_n = MAX((cur_n * 2), act_n);
  else act_n =  MAX(((cur_n * 3) / 2), act_n);
  return Alloc_(act_n * frsz);
}

bool taMatrix::canResize() const {
  return (alloc_size >= 0);
}

void taMatrix::Clear(int fm, int to) {
  if (size == 0) return;
  if (to < 0) to = size - 1;
  if (fm > to) return;
  if (TestError((fm >= size), "Clear", "fm exceeds size")) return;
  if (TestError((to >= size), "Clear", "to exceeds size")) return;
  Clear_impl(fm, to);
}

void taMatrix::Clear_impl(int fm, int to) {
  if (fastAlloc()) {
    size_t sz = ((to - fm) + 1) * El_SizeOf_();
    memset(FastEl_Flat_(fm), 0, sz);
  }
  else {
    const void* bl = El_GetBlank_();
    for (int i = fm; i <= to; ++i) {
      El_Copy_(FastEl_Flat_(i), bl);
    }
  }
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
    const taMatrix* cp = static_cast<const taMatrix*>(cp_fm);
    inherited::Copy_impl(*cp); // do all common generic parent class copying
    Copy_Matrix_impl(cp);
  }
  else inherited::CopyFromCustom_impl(cp_fm); // unlikely/prob doesn't happen
}

void taMatrix::Copy_Matrix_impl(const taMatrix* cp) {
  // note: caller has asserted Struct guys
  // assumes copy has been validated
  SetBaseFlag(COPYING); // note: still have to set/reset here, because not nestable

  if(!ElView() && !cp->ElView()) { // neither has views -- simple case
    SetGeomN(cp->geom);
    for (int i = 0; i < size; ++i) {
      El_SetFmVar_(FastEl_Flat_(i), cp->FastElAsVar_Flat(i));
    }
  }
  // views -- need to go through element-by-element in views
  else if(size == 0) {		// i'm empty -- fill me up!
    if(ElView())
      el_view = NULL;           // view no longer valid!
    if(cp->IdxFrameView()) {
      MatrixGeom gm = cp->geom;
      gm.Set(cp->FrameDim(), cp->Frames()); // effective
      SetGeomN(gm);
      TA_FOREACH_INDEX(i, *cp) { // count instead of i should all work out..
        El_SetFmVar_(FastEl_Flat_(FOREACH_itr.count), cp->FastElAsVar_Flat(i));
      }
    }
    else {
      SetGeom(1, cp->IterCount());
      TA_FOREACH_INDEX(i, *cp) {
        El_SetFmVar_(FastEl_Flat_(FOREACH_itr.count), cp->FastElAsVar_Flat(i));
      }
    }
  }
  else {
    TA_FOREACH_INDEX_TWO(mi, *this, ci, *cp) {
      if(FOREACH_itr_b.Done()) ci = cp->IterFirstIndex(FOREACH_itr_b); // start over
      El_SetFmVar_(FastEl_Flat_(mi), cp->FastElAsVar_Flat(ci));
    }
  }
  ClearBaseFlag(COPYING);
}

bool taMatrix::CopyFrame(const taMatrix& src, int frame) {
  if (!src.geom.IsFrameOf(geom)) return false;
  if ((frame < 0) || (frame >= Frames())) return false;
  int n = FrameSize();
  int base = FrameStartIdx(frame);
  // if same data types, we use an optimized copy, else must use variants
  // note that "Inherits" should imply same data type
  DataUpdate(true);
  if (GetTypeDef()->InheritsFrom(src.GetTypeDef())) {
    for (int i = 0; i < n; ++i) {
      El_Copy_(FastEl_Flat_(base + i), src.FastEl_Flat_(i));
    }
  }
  else {
    for (int i = 0; i < n; ++i) {
      El_SetFmVar_(FastEl_Flat_(base + i), src.El_GetVar_(src.FastEl_Flat_(i)));
    }
  }
  DataUpdate(false);

  return true;
}

void taMatrix::SigEmit(int sls, void* op1, void* op2) {
  inherited::SigEmit(sls, op1, op2);
  if (slice_par) {
    slice_par->SigEmit(sls, op1, op2);
  }
}

taBase* taMatrix::GetOwner() const {
  if (slice_par) return slice_par;
  return owner;
}

int taMatrix::defAlignment() const {
  return Qt::AlignRight; // most mats are numeric, so this is the default
}

void taMatrix::Permute() {
  int i, nv;
  void* tmp = El_GetTmp_();
  for(i=0; i<size; i++) {
    nv = (int) ((MTRnd::genrand_int32() % (size - i)) + i); // get someone from the future
    El_Copy_(tmp, FastEl_Flat_(i));
    El_Copy_(FastEl_Flat_(i), FastEl_Flat_(nv));  // swap with yourself
    El_Copy_(FastEl_Flat_(nv), tmp);
  }
}

void taMatrix::Move(int from, int to) {
  if (from == to) return; // nop
  if ((from < 0) || (from >= size) || (to < 0) || (to >= size)) {
    taMisc::Error("taMatrix::Move - argument out of range");
    return;
  }

  void* tmp = El_GetTmp_();

  if (from < to) {
    El_Copy_(tmp, FastEl_Flat_(from));
    for (int j = from; j < to; j++) {
      El_Copy_(FastEl_Flat_(j), FastEl_Flat_(j+1));
    }
    El_Copy_(FastEl_Flat_(to), tmp);
  }
  else {
    El_Copy_(tmp, FastEl_Flat_(from));
    for (int j = from; j > to; j--) {
      El_Copy_(FastEl_Flat_(j), FastEl_Flat_(j-1));
    }
    El_Copy_(FastEl_Flat_(to), tmp);
  }
}

void taMatrix::Sort(bool descending) {
  if(size <= 1) return;
  int lt_compval = -1;          // comparison return value for less-than
  if(descending)
    lt_compval = 1;
  // lets do a heap sort since it requires no secondary storage
  int n = size;
  int l,j,ir,i;
  void* tmp = El_GetTmp_();

  l = (n >> 1)+1;
  ir = n;
  for(;;){
    if(l>1)
      El_Copy_(tmp,FastEl_Flat_(--l -1)); // tmp = ra[--l]
    else {
      El_Copy_(tmp,FastEl_Flat_(ir-1)); // tmp = ra[ir]
      El_Copy_(FastEl_Flat_(ir-1),FastEl_Flat_(0)); // ra[ir] = ra[1]
      if(--ir == 1) {
        El_Copy_(FastEl_Flat_(0),tmp); // ra[1]=tmp
        return;
      }
    }
    i=l;
    j=l << 1;
    while(j<= ir) {
      if(j<ir && (El_Compare_(FastEl_Flat_(j-1),FastEl_Flat_(j)) == lt_compval)) j++;
      if(El_Compare_(tmp,FastEl_Flat_(j-1)) == lt_compval) { // tmp < ra[j]
        El_Copy_(FastEl_Flat_(i-1),FastEl_Flat_(j-1)); // ra[i]=ra[j];
        j += (i=j);
      }
      else j = ir+1;
    }
    El_Copy_(FastEl_Flat_(i-1),tmp); // ra[i] = tmp;
  }
}

String taMatrix::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
                              bool force_inline) const {
  // always inline effectively
  String gval = geom.GetValStr(par, memb_def, sc, force_inline);
  String rval(size*4,0,0);      // allocate buffer
  rval.cat(gval);
  rval << " {";
  for(int i=0;i<size;i++) {
    rval << " " << FastElAsStr_Flat(i) << ",";
  }
  rval << "}";
  return rval;
}

bool taMatrix::SetValStr(const String& val, void* par, MemberDef* memb_def,
                         TypeDef::StrContext sc, bool force_inline) {
  // always inline effectively
  String gmstr = val.before('{');
  MatrixGeom ng;
  ng.SetValStr(gmstr, par, memb_def, sc, force_inline);
  SetGeomN(ng);
  int idx = 0;
  String tmp = val.after('{');
  while(tmp.length() > 0) {
    String el_val = tmp.before(',');
    if(el_val.empty()) {
      el_val = tmp.before('}');
      if (el_val.empty())
        break;
    }
    tmp = tmp.after(',');
    if(TestError(idx >= size, "SetValStr",
                 "more items in val string than shoudl be according to geometry!")) break;
    if (el_val.contains(' '))
      el_val = el_val.after(' ');
    SetFmStr_Flat(el_val, idx);
    idx++;
  }
  return true;
}

int taMatrix::ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
                            void* par, TypeDef* par_typ, MemberDef* memb_def, TypeDef::StrContext sc) {
  int rval = 0;
  String mypath = GetPathNames();
  for(int i=0; i<size; i++) {
    String str = FastElAsStr_Flat(i);
    if(!str.contains(srch)) continue;
    String orig = str;
    rval += str.gsub(srch, repl);
    SetFmStr_Flat(str, i);
    taMisc::Info("Replaced string value in matrix object:",
                 mypath,"orig val:", orig, "new val:", str);
  }
  if(rval > 0)
    UpdateAfterEdit();
  return rval;
}

int taMatrix::Dump_Load_Item(istream& strm, int idx) {
  int c = taMisc::read_till_semi(strm);
  if (c != EOF) {
    SetFmStr_Flat(taMisc::LexBuf, idx);
  }
  return c;
}

int taMatrix::Dump_Load_Value(istream& strm, taBase* par) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)  return EOF;
  if(c == ';')  return 2;       // signal that its a path
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
      ar.AddDim(taMisc::LexBuf.toInt());
    } while ((c != ']') && (c != EOF));
    //note: should always be at least one dim if we had [ but we check anyway
    if (ar.dims() > 0) {
      el_view = NULL;           // make sure no view is set during loading..
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
  if (c==EOF)   return EOF;
  c = taMisc::read_till_semi(strm);
  if (c==EOF)   return EOF;
  return true;
}

void taMatrix::Dump_Save_Item(ostream& strm, int idx) {
  strm << FastElAsStr_Flat(idx);
}

int taMatrix::Dump_Save_Value(ostream& strm, taBase* par, int indent) {
  // save the members -- it puts the { out
  //int rval =
  inherited::Dump_Save_Value(strm, par, indent);

  // save data, if not completely null
  int i;
  if (geom.dims() > 0) {
    taMisc::indent(strm, indent, 1);
    // dims
    strm << "[";
    for (i=0; i< geom.dims(); ++i) {
      if (i > 0) strm << " ";
      strm << geom.FastEl(i);
    }
    strm << "] ";
    // values
    for (i=0; i < size; ++i) {
      Dump_Save_Item(strm, i);
      strm <<  ';';
      if((i+1) % 10 == 0)       // this helps a lot with readability and diffs..
        strm << '\n';
    }
  }

  return true;
}

/////////////////////////////////////
//      Binary Dump Format

int taMatrix::BinaryLoad_strm(istream& strm) {
  if(TestError(!BinaryFile_Supported(), "BinaryLoad", "Binary load is not supported for this type of matrix")) return false;
  int c = taMisc::skip_white(strm);
  if(c == EOF)  return EOF;
  if(c == ';')  return 2;       // signal that its a path
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
      ar.AddDim(taMisc::LexBuf.toInt());
    } while ((c != ']') && (c != EOF));
    //note: should always be at least one dim if we had [ but we check anyway
    if (ar.dims() > 0) {
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
  if (c==EOF)   return EOF;
  c = taMisc::read_till_semi(strm);
  if (c==EOF)   return EOF;
  return true;
}

int taMatrix::BinarySave_strm(ostream& strm) {
  if(TestError(!BinaryFile_Supported(), "BinarySave", "Binary save is not supported for this type of matrix")) return false;
  // save the members -- it puts the { out
  //int rval =
  inherited::Dump_Save_Value(strm, NULL, 0);

  // save data, if not completely null
  int i;
  if (geom.dims() > 0) {
    taMisc::indent(strm, 0);
    // dims
    strm << "[";
    for (i=0; i< geom.dims(); ++i) {
      if (i > 0) strm << " ";
      strm << geom.FastEl(i);
    }
    strm << "]";
    // values
    for (i=0; i < size; ++i) {
      BinarySave_Item(strm, i);
    }
  }
  strm << endl << "};" << endl; // terminate
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
  int new_size = n * FrameSize();
  // avoid spurious notifies -- geom changes do their own notify
  if (new_size == size) {
    geom.Set(FrameDim(), n);
    return true;
  }
  StructUpdate(true);
  if (new_size > size) {
    const void* blank = El_GetBlank_();
    for (int i = size; i < new_size; ++i) {
      El_Copy_(FastEl_Flat_(i), blank);
    }
    size = new_size;
    geom.Set(FrameDim(), n);
  }
  else if (new_size < size) {
    ReclaimOrphans_(new_size, size - 1);
    size = new_size;
    geom.Set(FrameDim(), n);
  }
  StructUpdate(false);
  return true;
}

int taMatrix::FindVal_Flat(const Variant& val, int st_idx) const {
  for(int i=st_idx;i<size; i++) {
    Variant mval = FastElAsVar_Flat(i);
    if(mval == val) return i;
  }
  return -1;
}

const String taMatrix::FlatRangeToTSV(int row_fr, int col_fr, int row_to, int col_to) {
  if ((row_fr < 0) || (col_fr < 0) || (row_to < row_fr) || (col_to < col_fr))
    return _nilString;
  // allocate a reasonable best-guess buffer
  STRING_BUF(rval, (col_to - col_fr + 1) * (row_to - row_fr + 1) * 10);
  // to access in 2d, you just ignore the higher dimensions
  for (int row = row_fr; row <= row_to; ++row) {
    if (row > row_fr) rval.cat('\n');
    //int idx = (row * dim(0)) + col_fr;
    for (int col = col_fr; col <= col_to; ++col/*, ++idx*/) {
      if (col > col_fr) rval.cat('\t');
      // note: we assume range must already have been converted from display to canonical
      // so we explicitly request the TOP_ZERO (no conversion) format
      int idx = geom.IndexFmDims2D(col, row, true, taMisc::TOP_ZERO);
      rval.cat(SafeElAsStr_Flat(idx));
    }
  }
  return rval;
}

const String taMatrix::FlatRangeToTSV(const CellRange& cr) {
  return FlatRangeToTSV(cr.row_fr, cr.col_fr, cr.row_to, cr.col_to);
}


bool taMatrix::ElemWiseOpTest(const taMatrix& t, bool oth_flex,
                              const String& op) const {
  String ope = op;
  if(ope.empty()) ope = "ElemWiseOpTest";
  if(oth_flex) {
    if(TestError(t.IterCount() == 0, ope,
                 "for element-wise operation, no visible elements in other matrix:", t.GetPathNames()))
      return false;
    return true;
  }
  int myic = this->IterCount();
  int tic = t.IterCount();
  if(TestError(myic != tic, ope,
               "for element-wise operation, number of visible elements in other matrix not identical to that in this matrix:",
               t.GetPathNames(), "I have:", String(myic), "other has:", String(tic)))
    return false;
  return true;
}


int taMatrix::Frames() const {
  if (geom.dims() == 0) return 0;
  if(IdxFrameView()) {
    return ViewIntMatrix()->size;
  }
  return geom[FrameDim()];
}

int taMatrix::FrameSize() const {
  if (geom.dims() == 0) return 0;
  if (geom.dims() == 1) return 1;
  int rval = geom[0];
  for (int i = 1; i < (geom.dims() - 1); ++i)
    rval *= geom[i];
  return rval;
}

bool taMatrix::FrameInRange(int fr, bool err_msg) const {
  if(IdxFrameView()) {
    int_Matrix* idx_frames = ViewIntMatrix();
    if(fr >= idx_frames->size || fr < 0) {
      TestError(err_msg, "FrameInRange", "logical frame index out of range for IDX_FRAMES view of matrix, frame:", String(fr), "visible frames:", String(idx_frames->size));
      return false;
    }
    return true;
  }
  int ftot = Frames();
  if(fr >= ftot || fr < 0) {
    TestError(err_msg, "FrameInRange", "frame index out of range for matrix, frame:", String(fr), "total frames:", String(ftot));
    return false;
  }
  return true;
}

int taMatrix::FrameIdx(int fr) const {
  if(IdxFrameView()) {
    int_Matrix* idx_frames = ViewIntMatrix();
    if(fr < 0) fr += idx_frames->size;
    int fidx = idx_frames->SafeEl_Flat(fr);
    return fidx;
  }
  else {
    if(fr < 0) fr += geom[FrameDim()];
    return fr;
  }
}

int taMatrix::FrameToRow(int f) const {
  // this needs to work even when dims<=1, so we just return f
  if (dims() <= 1) return f;
  // for dims==2, we just return f, else we multiple by higher dims
  for (int i = dims() - 2; i >= 1; --i)
    f *= dim(i);
  return f;
}

int taMatrix::FrameViewFlatIdx(int idx) const {
  int fsz = MAX(FrameSize(), 1);
  int fr = idx / fsz;
  if(fr < 0) fr += Frames();
  if(!FrameInRange(fr)) return -1; // will emit error
  int cl = idx % fsz;
  int rval = FrameStartIdx(fr) + cl;
  return rval;
}

int taMatrix::SafeElIndex(int d0, int d1, int d2, int d3,
                          int d4, int d5, int d6) const {
  if(IdxFrameView()) {
    int d[TA_MATRIX_DIMS_MAX]; d[0]=d0; d[1]=d1; d[2]=d2; d[3]=d3;
    d[4]=d4; d[5]=d5; d[6]=d6; d[7]=0;
    d[FrameDim()] = FrameIdx(d[FrameDim()]); // fix frame idx
    return geom.SafeIndexFmDims_(d); 
  }
  else {
    return geom.SafeIndexFmDims(d0, d1, d2, d3, d4, d5, d6);
  }
}

int taMatrix::SafeElIndexN(const MatrixIndex& indices) const {
  if(IdxFrameView()) {
    // use above function
    return SafeElIndex(indices[0], indices[1], indices[2], indices[3], indices[4], 
                       indices[5], indices[6]);
  }
  else {
    return geom.SafeIndexFmDimsN(indices);
  }
}


taMatrix* taMatrix::GetFrameSlice_(int frame) {
  int dims_m1 = dims() - 1; //cache
  if(TestError((dims_m1 <= 0),"GetFrameSlice_", "dims must be >1 to GetFrameSlice"))
    return NULL;
  int frames_ = Frames(); // cache
  // check frame_base and num_frames in bounds
  if(TestError(((frame < 0) || (frame >= frames_)), "GetFrameSlice_",
               "frame is out of bounds")) return NULL;

  MatrixGeom slice_geom(dims_m1);
  for (int i = 0; i < dims_m1; ++i)
    slice_geom.Set(i, dim(i));
  int sl_i = FrameStartIdx(frame); //note: must be valid because of prior checks
  void* base_el = FastEl_Flat_(sl_i);
  taMatrix* rval = FindSlice(base_el, slice_geom);
  if (rval) return rval;

  rval = (taMatrix*)MakeToken(); // an empty instance of our type
  if(TestError((!rval), "GetFrameSlice_", "could not make token of matrix")) return NULL;

  rval->SetFixedData_(base_el, slice_geom);
  // we do all the funky ref counting etc. in one place
  SliceInitialize(this, rval);
  return rval;
}

taMatrix* taMatrix::GetSlice_(const MatrixIndex& base,
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

  if(num_slice_frames > 1 && IdxFrameView()) {
    if(TestError((slice_frame_dims != dims()-1),
                 "GetSlice_", "slice_frame_dims must be same as source matrix when frame view is in effect"))
      return NULL;
    int st_frame = base.SafeEl(slice_frame_dims);
    int_Matrix* idx_frames = ViewIntMatrix();
    if(st_frame < 0) st_frame += idx_frames->size; // allow from-end neg indexing here..
    if(TestError(st_frame < 0 || st_frame >= idx_frames->size,
                 "GetSlice_", "starting frame out of bounds in IDX_FRAMES view. requested starting frame:", String(st_frame), "avail:",
                 String(idx_frames->size-1)))
      return NULL;
    if(TestError(st_frame + num_slice_frames > idx_frames->size,
                 "GetSlice_", "more frames requested than available in IDX_FRAMES view.  requested up to frame:", String(st_frame + num_slice_frames), "avail:",
                 String(idx_frames->size)))
      return NULL;
    // if we have a frame view, must return entire matrix with new view
    int_Matrix* new_frame_view = new int_Matrix;
    new_frame_view->SetGeom(1, num_slice_frames);
    for(int i=0; i<num_slice_frames; i++) {
      new_frame_view->FastEl_Flat(i) = idx_frames->SafeEl_Flat(st_frame + i);
    }
    taMatrix* rval = (taMatrix*)MakeToken(); // make a token of me
    void* base_el = const_cast<void*>(FastEl_Flat_(0));
    rval->SetFixedData_(base_el, geom);      // identical geom, same data
    SliceInitialize(const_cast<taMatrix*>(this), rval);
    rval->SetElView(new_frame_view, IDX_FRAMES);
    return rval;
  }

  // check start cell in bounds and legal
  int sl_i = SafeElIndexN(base); // -1 if out of bounds
  if(TestError((sl_i < 0), "GetSlice_", "slice base is out of bounds")) return NULL;

  // create geom of slice, and copy our dims
  // note that since we are talking in frames, the dims = frames+1
  MatrixGeom slice_geom(slice_frame_dims + 1); // geometry of the resulting slice
  for (int i = 0; i < slice_frame_dims; ++i)
    slice_geom.Set(i, dim(i));
  slice_geom.Set(slice_frame_dims, num_slice_frames);

  void* base_el = FastEl_Flat_(sl_i);
  taMatrix* rval = FindSlice(base_el, slice_geom);
  if (rval) return rval;

  // easiest to now check for frames in bounds
  int sl_tot = slice_geom.Product();
  if(TestError(((sl_i + sl_tot) > size), "GetSlice_", "slice end is out of bounds"))
    return NULL;

  rval = (taMatrix*)MakeToken(); // an empty instance of our type
  if(TestError((!rval), "GetSlice_", "could not make token of matrix")) return NULL;

  rval->SetFixedData_(base_el, slice_geom);
  // we do all the funky ref counting etc. in one place
  SliceInitialize(this, rval);
  return rval;
}

taMatrix* taMatrix::GetFrameRangeSlice_(int st_frame, int n_frames) {
  const int dm = dims();
  MatrixIndex base(dm);
  for (int i = 0; i < dm-1; ++i)
    base.Set(i, 0);
  base.Set(dm-1, st_frame);
  return GetSlice_(base, dm-1, n_frames);
}

taMatrix* taMatrix::FindSlice(void* el_, const MatrixGeom& geom_) {
  if (slices) {
    for (int i = 0; i < slices->size; ++i) {
      taMatrix* mat = slices->FastEl(i);
      if (!mat) continue; // shouldn't happen
      if ((mat->data() == el_) && (mat->geom == geom_))
        return mat;
    }
  }
  return NULL;
}

bool taMatrix::InRange(int d0, int d1, int d2, int d3, int d4, int d5, int d6) const {
  switch (geom.dims()) {
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
  case 5: return ((d0 >= 0) && (d0 < geom[0]))
    && ((d1 >= 0) && (d1 < geom[1]))
    && ((d2 >= 0) && (d2 < geom[2]))
    && ((d3 >= 0) && (d3 < geom[3]))
    && ((d4 >= 0) && (d4 < geom[4]));
  default: return false;
  }
}


bool taMatrix::InRangeN(const MatrixIndex& indices) const {
  if (indices.dims() < geom.dims()) return false;
  for (int i = 0; i < indices.dims(); ++i) {
    int di = indices[i];
    if ((di < 0) || (di >= geom[i])) return false;
  }
  return true;
}

bool taMatrix::RemoveFrames(int st_fr, int n_fr) {
  if(TestError(!canResize(), "RemoveFrames", "resizing not allowed")) return false;
  int frames_ = Frames(); // cache
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
  int frsz = FrameSize();
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
  int sz = Frames(); // cache
  if(st_fr < 0) st_fr = sz;
  if(TestError((st_fr>sz), "InsertFrames", "starting frame number out of range",
               String(st_fr))) return false;
  if(TestError((n_fr <= 0), "InsertFrames", "number of frames <= 0")) return false;
  AddFrames(n_fr);
  if(st_fr==sz) {
    return true;                // done!
  }
  int n_mv = sz - st_fr;        // number that must be moved
  sz += n_fr;                   // update to added size
  int frsz = FrameSize();
  int trg_o = sz-1;
  int src_o = sz-1-n_fr;
  for(int i=0; i<n_mv; i++) {   // shift everyone over
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

void taMatrix::ResetColorScale() {
  if(!colorscale || size == 0) return;
  float min = FastElAsFloat_Flat(0);
  float max = FastElAsFloat_Flat(0);
  for(int i=1; i<size; i++) {
    min = MIN(min, FastElAsFloat_Flat(i));
    max = MAX(max, FastElAsFloat_Flat(i));
  }
  colorscale->SetMinMax(min, max);
}

ColorScale* taMatrix::GetColorScale() {
  if(isDestroying()) return NULL;
  if(GetDataValType() != VT_FLOAT && GetDataValType() != VT_DOUBLE)
    return NULL;                // only for real-valued numerical types
  if(!colorscale) {
    colorscale = new ColorScale;
    taBase::Own(colorscale, this);
    ResetColorScale();
  }
  return colorscale;
}

iMatrixTableModel* taMatrix::GetTableModel() {
  if(isDestroying()) return NULL;
  if(!table_model) {
    GetColorScale();
    table_model = new iMatrixTableModel(this);
    table_model->setPat4D(true); // always
  }
  return table_model;
}

void taMatrix::Reset() {
  EnforceFrames(0);
  UpdateSlices_Collapse();
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

bool taMatrix::SetGeom_(int dims_, const int geom_[]) {
  // dims=0 is special case, which is just a reset
  ClearElView();                // an explicit call to SetGeom implies resetting existing data
  if (dims_ == 0) {
    StructUpdate(true);
    Reset(); // deletes data, and collapses slices
    geom.Reset(); // ok, now you know what 0-d really means!!!
    StructUpdate(false);
    return true;
  }
  else {
    String err_msg;
    bool valid = GeomIsValid(dims_, geom_, &err_msg);
    if (TestError(!valid, "SetGeom_", err_msg)) return false;
  }

  // flex not allowed for fixed data
  if (isFixedData()) {
    if (TestError((geom_[dims_-1] == 0), "SetGeom_",
      "fixed data cannot use flex sizing")) return false;
  }

  // NOTE: following routine is conservative of existing geom
  // if you are using flex sizing, it will collapse any existing!!!
  // only copy bottom N-1 dims, setting 0 frames -- we size frames in next step
  StructUpdate(true);
  // we collapse slices if #dims change, inner dims change, or frames go down
  bool collapse_slices = geom.SetDims(dims_);
  for (int i = 0; i < (dims_ - 1); ++i) {
    if(geom[i] != geom_[i]) {
      collapse_slices = true;
      geom.Set(i, geom_[i]);
    }
  }
  if (geom[dims_ -1] > geom_[dims_ - 1])
    collapse_slices = true;

  // assign storage if not fixed
  if (isFixedData()) {
    geom.Set(dims_ -1, geom_[dims_ - 1]);
    size = geom.Product();
  }
  else {
    // next step actually sets last geom
    if (!EnforceFrames(geom_[dims_-1])) {
      StructUpdate(false); // TODO: RAII
      return false; // failure
    }
  }
  if (collapse_slices) {
    UpdateSlices_Collapse();
  }
  StructUpdate(false);
  return true;
}

void taMatrix::Slice_Collapse() {
  // called when our referent has become invalid, for whatever reason
  SetArray_(NULL);
  geom.Reset();
  size = 0;
  // if we have collapsed, so have any of our slices...
  UpdateSlices_Collapse();
  SigEmitUpdated();
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
  SigEmitUpdated();
}

int taMatrix::sliceCount() const {
  if (slices) return slices->size; // note: allowed to be zero
  else        return 0; // hasn't been needed yet
}

void taMatrix::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //NOTE: you are NOT allowed to change geom this way -- must use the SetGeom api call
  if (taMisc::is_loading) {
    taBase* ev = el_view.ptr();
    el_view = NULL;             // get rid of view for update
    UpdateGeom();
    el_view = ev;               // restore
  }
}

void taMatrix::UpdateGeom() {
  // NOTE: this routine is ONLY intended for the case of updating after loading
  // from a stream -- no slice checking because there can't really be any yet!
  // handle legacy/graceful case wherein size is non-zero, but no dims --
  // set dims to 1, and dim[0] to the size
  if ((size != 0) && (geom.dims() == 0)) {
    geom.SetDims(1);
    geom.Set(0, size);
  }

  // get overall framesize and frames
  int dims_ = geom.dims(); // cache
  // make sure dims are valid (outer dim can be 0, others must be >0)
  int i;
  for (i = 0; i < (dims_ - 1) ; ++i) {
    if (geom[i] <= 0) {
      taMisc::Error(this->GetPathNames(), "geoms[0..N-2] must be > 0; object is now invalid");
      return;
    }
  }
  // assign storage (if not fixed) and set size
  if (isFixedData()) {
    size = Frames() * FrameSize();
  }
  else {
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
  void* post_deletion = (char*)deletion_base + (num * FrameSize() * El_SizeOf_());
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

////////////////////////////////////////////////////
//   sub-matrix reading and writing functions

Variant taMatrix::RenderValue(const Variant& dest_val, const Variant& src_val, RenderOp render_op) {
  Variant rval;
  switch(render_op) {
  case COPY:
    rval = src_val;
    break;
  case ADD:
    rval = dest_val.toDouble() + src_val.toDouble();
    break;
  case SUB:
    rval = dest_val.toDouble() - src_val.toDouble();
    break;
  case MULT:
    rval = dest_val.toDouble() * src_val.toDouble();
    break;
  case DIV:
    rval = dest_val.toDouble() / src_val.toDouble();
    break;
  case MAX:
    rval = MAX(dest_val.toDouble(), src_val.toDouble());
    break;
  case MIN:
    rval = MIN(dest_val.toDouble(), src_val.toDouble());
    break;
  }
  return rval;
}

void taMatrix::WriteFmSubMatrix(const taMatrix* src, RenderOp render_op,
                                int off0, int off1, int off2,
                                int off3, int off4, int off5, int off6) {
  if(!src) return;
  MatrixIndex off(dims(), off0, off1, off2, off3, off4, off5, off6);
  MatrixIndex srcp;
  for(int i=0;i<src->size;i++) {
    src->geom.DimsFmIndex(i, srcp);
    MatrixIndex trgp(off);
    trgp += srcp;
    Variant sval = src->FastElAsVar_Flat(i);
    Variant dval = SafeElAsVarN(trgp);
    dval = RenderValue(dval, sval, render_op);
    SetFmVarN(dval, trgp);
  }
}

void taMatrix::ReadToSubMatrix(taMatrix* dest, RenderOp render_op,
                               int off0, int off1, int off2,
                               int off3, int off4, int off5, int off6) {
  if(!dest) return;
  MatrixIndex off(dims(), off0, off1, off2, off3, off4, off5, off6);
  MatrixIndex srcp;
  for(int i=0;i<dest->size;i++) {
    dest->geom.DimsFmIndex(i, srcp);
    MatrixIndex trgp(off);
    trgp += srcp;
    Variant sval = SafeElAsVarN(trgp);
    if(!sval.isInvalid()) {
      Variant dval = dest->FastElAsVar_Flat(i);
      dval = RenderValue(dval, sval, render_op);
      dest->SetFmVar_Flat(dval, i);
    }
  }
}

void taMatrix::WriteFmSubMatrixFrames(taMatrix* src, RenderOp render_op,
                                      int off0, int off1, int off2,
                                      int off3, int off4, int off5, int off6) {
  if(!src) return;
  MatrixIndex off(dims(), off0, off1, off2, off3, off4, off5, off6);
  MatrixIndex srcp;
  int fr_max = Frames();
  bool src_frames = false;      // source has frames
  if(src->dims() == dims()) {
    fr_max = MIN(fr_max, src->Frames());
    src_frames = true;
  }
  for(int fr=0;fr<fr_max;fr++) {
    taMatrixPtr dfr;  dfr = GetFrameSlice_(fr);
    taMatrixPtr sfr;
    if(src_frames)
      sfr = src->GetFrameSlice_(fr);
    else
      sfr = src;
    for(int i=0;i<sfr->size;i++) {
      sfr->geom.DimsFmIndex(i, srcp);
      MatrixIndex trgp(off);
      trgp += srcp;
      Variant sval = sfr->FastElAsVar_Flat(i);
      Variant dval = dfr->SafeElAsVarN(trgp);
      dval = RenderValue(dval, sval, render_op);
      dfr->SetFmVarN(dval, trgp);
    }
  }
}

void taMatrix::ReadToSubMatrixFrames(taMatrix* dest, RenderOp render_op,
                                     int off0, int off1, int off2,
                                     int off3, int off4, int off5, int off6) {
  if(!dest) return;
  dest->EnforceFrames(Frames()); // match them up
  MatrixIndex off(dims(), off0, off1, off2, off3, off4, off5, off6);
  MatrixIndex srcp;
  int fr_max = Frames();
  for(int fr=0;fr<fr_max;fr++) {
    taMatrixPtr sfr;  sfr = GetFrameSlice_(fr);
    taMatrixPtr dfr;  dfr = dest->GetFrameSlice_(fr);
    for(int i=0;i<dfr->size;i++) {
      dfr->geom.DimsFmIndex(i, srcp);
      MatrixIndex trgp(off);
      trgp += srcp;
      Variant sval = sfr->SafeElAsVarN(trgp);
      if(!sval.isInvalid()) {
        Variant dval = dfr->FastElAsVar_Flat(i);
        dval = RenderValue(dval, sval, render_op);
        dfr->SetFmVar_Flat(dval, i);
      }
    }
  }
}

/////////////////////////////////////////////////////////
//              Operators

taMatrix* taMatrix::operator=(const taArray_base& t) {
  SetGeom(1, t.IterCount());
  int idx = 0;
  TA_FOREACH_INDEX(i, t) {
    SetFmVar_Flat(t.SafeElAsVar(i), idx++);
  }
  return this;
}

taMatrix* taMatrix::operator=(const Variant& t) {
  if(t.isMatrixType()) {
    Copy(t.toMatrix());
    return this;
  }
  if(t.isBaseType()) {
    taBase* tab = t.toBase();
    if(TestError(!tab, "operator=", "attempt to assign matrix from a null object")) {
      return this;
    }
    if(tab->InheritsFrom(&TA_taArray_base)) {
      return operator=(*((taArray_base*)tab));
    }
    if(TestError(true, "operator=", "cannot assign to matrix from object of type:",
                 tab->GetTypeDef()->name)) {
      return this;
    }
  }
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) = vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) = vt;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(t, i);
    }
  }
  return this;
}

//////////// op +
taMatrix* taMatrix::operator+(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "+"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = ((float_Matrix*)this)->FastEl_Flat(mi)
        + ((float_Matrix*)&t)->FastEl_Flat(ti);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = ((double_Matrix*)this)->FastEl_Flat(mi)
        + ((double_Matrix*)&t)->FastEl_Flat(ti);
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->SetFmVar_Flat(FastElAsVar_Flat(mi) + t.FastElAsVar_Flat(ti), mi);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator+(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator+((const taMatrix&)*(t.toMatrix()));
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) + vt;
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) + vt;
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) + t, i);
    }
    return rval;
  }
  return NULL;
}

//////////// op -
taMatrix* taMatrix::operator-(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "-"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = ((float_Matrix*)this)->FastEl_Flat(mi)
        - ((float_Matrix*)&t)->FastEl_Flat(ti);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = ((double_Matrix*)this)->FastEl_Flat(mi)
        - ((double_Matrix*)&t)->FastEl_Flat(ti);
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->SetFmVar_Flat(FastElAsVar_Flat(mi) - t.FastElAsVar_Flat(ti), mi);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator-(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator-((const taMatrix&)*(t.toMatrix()));
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) - vt;
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) - vt;
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) - t, i);
    }
    return rval;
  }
  return NULL;
}

//////////// op - UNARY
taMatrix* taMatrix::operator-() const {
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = -((float_Matrix*)this)->FastEl_Flat(i);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = -((double_Matrix*)this)->FastEl_Flat(i);
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      Variant var = FastElAsVar_Flat(i);
      rval->SetFmVar_Flat(-var, i);
    }
    return rval;
  }
  return NULL;
}

//////////// op *
taMatrix* taMatrix::operator*(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "*"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = ((float_Matrix*)this)->FastEl_Flat(mi)
        * ((float_Matrix*)&t)->FastEl_Flat(ti);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = ((double_Matrix*)this)->FastEl_Flat(mi)
        * ((double_Matrix*)&t)->FastEl_Flat(ti);
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->SetFmVar_Flat(FastElAsVar_Flat(mi) * t.FastElAsVar_Flat(ti), ti);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator*(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator*((const taMatrix&)*(t.toMatrix()));
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) * vt;
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) * vt;
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) * t, i);
    }
    return rval;
  }
  return NULL;
}

//////////// op /
taMatrix* taMatrix::operator/(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "/"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      float den = ((float_Matrix*)&t)->FastEl_Flat(ti);
      if(!TestError(den == 0.0f, "/", "Floating Point Exception: Division by Zero"))
        rval->FastEl_Flat(mi) = ((float_Matrix*)this)->FastEl_Flat(mi) / den;
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      double den = ((double_Matrix*)&t)->FastEl_Flat(ti);
      if(!TestError(den == 0.0, "/", "Floating Point Exception: Division by Zero"))
        rval->FastEl_Flat(mi) = ((double_Matrix*)this)->FastEl_Flat(mi) / den;
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->SetFmVar_Flat(FastElAsVar_Flat(mi) / t.FastElAsVar_Flat(ti), mi);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator/(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator/((const taMatrix&)*(t.toMatrix()));
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    float vt = t.toFloat();
    if(!TestError(vt == 0.0, "/", "Floating Point Exception: Division by Zero")) {
      TA_FOREACH_INDEX(i, *this) {
        rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) / vt;
      }
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    double vt = t.toDouble();
    if(!TestError(vt == 0.0, "/", "Floating Point Exception: Division by Zero")) {
      TA_FOREACH_INDEX(i, *this) {
        rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) / vt;
      }
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    if(!TestError(t == 0.0, "/=", "Floating Point Exception: Division by Zero")) {
      TA_FOREACH_INDEX(i, *this) {
        rval->SetFmVar_Flat(FastElAsVar_Flat(i) / t, i);
      }
    }
    return rval;
  }
  return NULL;
}

//////////// op %
taMatrix* taMatrix::operator%(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "%"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return NULL;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return NULL;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i) % t.FastElAsVar_Flat(i), i);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator%(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator%((const taMatrix&)*(t.toMatrix()));
  }
  if(GetDataValType() == VT_FLOAT) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return NULL;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return NULL;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    if(!TestError(t == 0, "/=", "Floating Point Exception: Division by Zero")) {
      TA_FOREACH_INDEX(i, *this) {
        rval->SetFmVar_Flat(FastElAsVar_Flat(i) % t, i);
      }
    }
    return rval;
  }
  return NULL;
}

//////////// op ^
taMatrix* taMatrix::operator^(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "^"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = powf(((float_Matrix*)this)->FastEl_Flat(mi),
                                   ((float_Matrix*)&t)->FastEl_Flat(ti));
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = pow(((double_Matrix*)this)->FastEl_Flat(mi),
                                  ((double_Matrix*)&t)->FastEl_Flat(ti));
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->SetFmVar_Flat(pow(FastElAsVar_Flat(mi).toDouble(),
                              t.FastElAsVar_Flat(ti).toDouble()), mi);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::operator^(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator^((const taMatrix&)*(t.toMatrix()));
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = powf(((float_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = pow(((double_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(pow(FastElAsVar_Flat(i).toDouble(), vt), i);
    }
    return rval;
  }
  return NULL;
}

//////////// op max
taMatrix* taMatrix::Max(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "Max"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = MAX(((float_Matrix*)this)->FastEl_Flat(mi), 
                                  ((float_Matrix*)&t)->FastEl_Flat(ti));
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = MAX(((double_Matrix*)this)->FastEl_Flat(mi),
                                  ((double_Matrix*)&t)->FastEl_Flat(ti));
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->SetFmVar_Flat(MAX(FastElAsVar_Flat(mi), t.FastElAsVar_Flat(ti)), mi);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::Max(const Variant& t) const {
  if(t.isMatrixType()) {
    return Max((const taMatrix&)*(t.toMatrix()));
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MAX(((float_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MAX(((double_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(MAX(FastElAsVar_Flat(i), t), i);
    }
    return rval;
  }
  return NULL;
}

Variant taMatrix::Max() const {
  if(GetDataValType() == VT_FLOAT) {
    float rval = 0.0f;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
        rval = ((float_Matrix*)this)->FastEl_Flat(i);
        first = false;
      }
      else {
        rval = MAX(((float_Matrix*)this)->FastEl_Flat(i), rval);
      }
    }
    return Variant(rval);
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double rval = 0.0;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
        rval = ((double_Matrix*)this)->FastEl_Flat(i);
        first = false;
      }
      else {
        rval = MAX(((double_Matrix*)this)->FastEl_Flat(i), rval);
      }
    }
    return Variant(rval);
  }
  else {                        // use variants -- no need to optimize
    Variant rval;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
        rval = FastElAsVar_Flat(i);
        first = false;
      }
      else {
        rval = MAX(FastElAsVar_Flat(i), rval);
      }
    }
    return rval;
  }
  return _nilVariant;
}

//////////// op min
taMatrix* taMatrix::Min(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "Min"))
    return NULL;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = MIN(((float_Matrix*)this)->FastEl_Flat(mi), 
                                  ((float_Matrix*)&t)->FastEl_Flat(ti));
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = MIN(((double_Matrix*)this)->FastEl_Flat(mi),
                                  ((double_Matrix*)&t)->FastEl_Flat(ti));
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->SetFmVar_Flat(MIN(FastElAsVar_Flat(mi), t.FastElAsVar_Flat(ti)), mi);
    }
    return rval;
  }
  return NULL;
}

taMatrix* taMatrix::Min(const Variant& t) const {
  if(t.isMatrixType()) {
    return Min((const taMatrix&)*(t.toMatrix()));
  }
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MIN(((float_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = MIN(((double_Matrix*)this)->FastEl_Flat(i), vt);
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(MIN(FastElAsVar_Flat(i), t), i);
    }
    return rval;
  }
  return NULL;
}

Variant taMatrix::Min() const {
  if(GetDataValType() == VT_FLOAT) {
    float rval = 0.0f;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
        rval = ((float_Matrix*)this)->FastEl_Flat(i);
        first = false;
      }
      else {
        rval = MIN(((float_Matrix*)this)->FastEl_Flat(i), rval);
      }
    }
    return Variant(rval);
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double rval = 0.0;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
        rval = ((double_Matrix*)this)->FastEl_Flat(i);
        first = false;
      }
      else {
        rval = MIN(((double_Matrix*)this)->FastEl_Flat(i), rval);
      }
    }
    return Variant(rval);
  }
  else {                        // use variants -- no need to optimize
    Variant rval;
    bool first = true;
    TA_FOREACH_INDEX(i, *this) {
      if(first) {
        rval = FastElAsVar_Flat(i);
        first = false;
      }
      else {
        rval = MIN(FastElAsVar_Flat(i), rval);
      }
    }
    return rval;
  }
  return _nilVariant;
}

//////////// op abs
taMatrix* taMatrix::Abs() const {
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = fabsf(((float_Matrix*)this)->FastEl_Flat(i));
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = fabs(((double_Matrix*)this)->FastEl_Flat(i));
    }
    return rval;
  }
  else if(GetDataValType() == VT_INT) {
    int_Matrix* rval = new int_Matrix(this->geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ABS(((int_Matrix*)this)->FastEl_Flat(i));
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(geom);
    rval->CopyElView(*this);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(ABS(FastElAsVar_Flat(i).toInt()), i);
    }
    return rval;
  }
  return NULL;
}

//////////// op +=
void taMatrix::operator+=(const taMatrix& t) {
  if(!ElemWiseOpTest(t, true, "+="))
    return;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      ((float_Matrix*)this)->FastEl_Flat(mi) += ((float_Matrix*)&t)->FastEl_Flat(ti);
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      ((double_Matrix*)this)->FastEl_Flat(mi) += ((double_Matrix*)&t)->FastEl_Flat(ti);
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      SetFmVar_Flat(FastElAsVar_Flat(mi) + t.FastElAsVar_Flat(ti), mi);
    }
  }
}

void taMatrix::operator+=(const Variant& t) {
  if(t.isMatrixType()) {
    operator+=((const taMatrix&)*(t.toMatrix()));
    return;
  }
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) += vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) += vt;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) + t, i);
    }
  }
}

//////////// op -=
void taMatrix::operator-=(const taMatrix& t) {
  if(!ElemWiseOpTest(t, true, "-="))
    return;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      ((float_Matrix*)this)->FastEl_Flat(mi) -= ((float_Matrix*)&t)->FastEl_Flat(ti);
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      ((double_Matrix*)this)->FastEl_Flat(mi) -= ((double_Matrix*)&t)->FastEl_Flat(ti);
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      SetFmVar_Flat(FastElAsVar_Flat(mi) - t.FastElAsVar_Flat(ti), mi);
    }
  }
}

void taMatrix::operator-=(const Variant& t) {
  if(t.isMatrixType()) {
    operator-=((const taMatrix&)*(t.toMatrix()));
    return;
  }
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) -= vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) -= vt;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) - t, i);
    }
  }
}

//////////// op *=
void taMatrix::operator*=(const taMatrix& t) {
  if(!ElemWiseOpTest(t, true, "*="))
    return;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      ((float_Matrix*)this)->FastEl_Flat(mi) *= ((float_Matrix*)&t)->FastEl_Flat(ti);
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      ((double_Matrix*)this)->FastEl_Flat(mi) *= ((double_Matrix*)&t)->FastEl_Flat(ti);
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      SetFmVar_Flat(FastElAsVar_Flat(mi) * t.FastElAsVar_Flat(ti), mi);
    }
  }
}

void taMatrix::operator*=(const Variant& t) {
  if(t.isMatrixType()) {
    operator*=((const taMatrix&)*(t.toMatrix()));
    return;
  }
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      ((float_Matrix*)this)->FastEl_Flat(i) *= vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      ((double_Matrix*)this)->FastEl_Flat(i) *= vt;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      SetFmVar_Flat(FastElAsVar_Flat(i) * t, i);
    }
  }
}

//////////// op /=
void taMatrix::operator/=(const taMatrix& t) {
  if(!ElemWiseOpTest(t, true, "/=+"))
    return;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      float den = ((float_Matrix*)&t)->FastEl_Flat(ti);
      if(!TestError(den == 0.0, "/=", "Floating Point Exception: Division by Zero"))
        ((float_Matrix*)this)->FastEl_Flat(mi) /= den;
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      double den = ((double_Matrix*)&t)->FastEl_Flat(ti);
      if(!TestError(den == 0.0, "/=", "Floating Point Exception: Division by Zero"))
        ((double_Matrix*)this)->FastEl_Flat(mi) /= den;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      SetFmVar_Flat(FastElAsVar_Flat(mi) / t.FastElAsVar_Flat(ti), mi);
    }
  }
}

void taMatrix::operator/=(const Variant& t) {
  if(t.isMatrixType()) {
    operator/=((const taMatrix&)*(t.toMatrix()));
    return;
  }
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    if(!TestError(vt == 0.0, "/=", "Floating Point Exception: Division by Zero")) {
      TA_FOREACH_INDEX(i, *this) {
        ((float_Matrix*)this)->FastEl_Flat(i) /= vt;
      }
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    if(!TestError(vt == 0.0, "/=", "Floating Point Exception: Division by Zero")) {
      TA_FOREACH_INDEX(i, *this) {
        ((double_Matrix*)this)->FastEl_Flat(i) /= vt;
      }
    }
  }
  else {                        // use variants -- no need to optimize
    if(!TestError(t == 0.0, "/=", "Floating Point Exception: Division by Zero")) {
      TA_FOREACH_INDEX(i, *this) {
        SetFmVar_Flat(FastElAsVar_Flat(i) / t, i);
      }
    }
  }
}

//////////// op %=
void taMatrix::operator%=(const taMatrix& t) {
  if(!ElemWiseOpTest(t, true, "%="))
    return;
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return;
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return;
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      SetFmVar_Flat(FastElAsVar_Flat(mi) % t.FastElAsVar_Flat(ti), mi);
    }
  }
}

void taMatrix::operator%=(const Variant& t) {
  if(t.isMatrixType()) {
    operator%=((const taMatrix&)*(t.toMatrix()));
    return;
  }
  if(GetDataValType() == VT_FLOAT) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    if(TestError(true, "%", "not supported for floating-point types"))
      return;
  }
  else {                        // use variants -- no need to optimize
    if(!TestError(t == 0, "/=", "Floating Point Exception: Division by Zero")) {
      TA_FOREACH_INDEX(i, *this) {
        SetFmVar_Flat(FastElAsVar_Flat(i) % t, i);
      }
    }
  }
}

//////////// op <
taMatrix* taMatrix::operator<(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "<"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((float_Matrix*)this)->FastEl_Flat(mi)
                              < ((float_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((double_Matrix*)this)->FastEl_Flat(mi)
                              < ((double_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = FastElAsVar_Flat(mi) < t.FastElAsVar_Flat(ti);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator<(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator<((const taMatrix&)*(t.toMatrix()));
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) < vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) < vt;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) < t;
    }
  }
  return rval;
}

//////////// op >
taMatrix* taMatrix::operator>(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, ">"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((float_Matrix*)this)->FastEl_Flat(mi)
                              > ((float_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((double_Matrix*)this)->FastEl_Flat(mi)
                              > ((double_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = FastElAsVar_Flat(mi) > t.FastElAsVar_Flat(ti);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator>(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator>((const taMatrix&)*(t.toMatrix()));
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) > vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) > vt;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) > t;
    }
  }
  return rval;
}

//////////// op <=
taMatrix* taMatrix::operator<=(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "<="))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((float_Matrix*)this)->FastEl_Flat(mi)
                              <= ((float_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((double_Matrix*)this)->FastEl_Flat(mi)
                              <= ((double_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = FastElAsVar_Flat(mi) <= t.FastElAsVar_Flat(ti);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator<=(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator<=((const taMatrix&)*(t.toMatrix()));
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) <= vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) <= vt;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) <= t;
    }
  }
  return rval;
}

//////////// op >=
taMatrix* taMatrix::operator>=(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, ">="))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((float_Matrix*)this)->FastEl_Flat(mi)
                              >= ((float_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((double_Matrix*)this)->FastEl_Flat(mi)
                               >= ((double_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = FastElAsVar_Flat(mi) >= t.FastElAsVar_Flat(ti);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator>=(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator>=((const taMatrix&)*(t.toMatrix()));
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) >= vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) >= vt;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) >= t;
    }
  }
  return rval;
}

//////////// op ==
taMatrix* taMatrix::operator==(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "=="))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((float_Matrix*)this)->FastEl_Flat(mi)
                              == ((float_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((double_Matrix*)this)->FastEl_Flat(mi)
                              == ((double_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = FastElAsVar_Flat(mi) == t.FastElAsVar_Flat(ti);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator==(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator==((const taMatrix&)*(t.toMatrix()));
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) == vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) == vt;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) == t;
    }
  }
  return rval;
}

//////////// op !=
taMatrix* taMatrix::operator!=(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "!="))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((float_Matrix*)this)->FastEl_Flat(mi)
                              != ((float_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else if(GetDataValType() == VT_DOUBLE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((double_Matrix*)this)->FastEl_Flat(mi)
                               != ((double_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = FastElAsVar_Flat(mi) != t.FastElAsVar_Flat(ti);
    }
  }
  return rval;
}

taMatrix* taMatrix::operator!=(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator!=((const taMatrix&)*(t.toMatrix()));
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_FLOAT) {
    float vt = t.toFloat();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((float_Matrix*)this)->FastEl_Flat(i) != vt;
    }
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double vt = t.toDouble();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((double_Matrix*)this)->FastEl_Flat(i) != vt;
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i) != t;
    }
  }
  return rval;
}

//////////// op &&
taMatrix* taMatrix::operator&&(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "&&"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_BYTE && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((byte_Matrix*)this)->FastEl_Flat(mi)
                               && ((byte_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else if(GetDataValType() == VT_INT && GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((int_Matrix*)this)->FastEl_Flat(mi)
                               && ((int_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = FastElAsVar_Flat(mi).toBool()
        && t.FastElAsVar_Flat(ti).toBool();
    }
  }
  return rval;
}

taMatrix* taMatrix::operator&&(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator&&((const taMatrix&)*(t.toMatrix()));
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_BYTE) {
    int vt = t.toInt();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((byte_Matrix*)this)->FastEl_Flat(i) && vt;
    }
  }
  else if(GetDataValType() == VT_INT) {
    int vt = t.toInt();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((int_Matrix*)this)->FastEl_Flat(i) && vt;
    }
  }
  else {                        // use variants -- no need to optimize
    bool vt = t.toBool();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i).toBool() && vt;
    }
  }
  return rval;
}

//////////// op ||
taMatrix* taMatrix::operator||(const taMatrix& t) const {
  if(!ElemWiseOpTest(t, true, "||"))
    return NULL;
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_BYTE || GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((byte_Matrix*)this)->FastEl_Flat(mi)
                               || ((byte_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else if(GetDataValType() == VT_INT || GetDataValType() == t.GetDataValType()) {
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = (((int_Matrix*)this)->FastEl_Flat(mi)
                               || ((int_Matrix*)&t)->FastEl_Flat(ti));
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX_TWO(mi, *this, ti, t) {
      if(FOREACH_itr_b.Done()) ti = t.IterFirstIndex(FOREACH_itr_b); // start over
      rval->FastEl_Flat(mi) = FastElAsVar_Flat(mi).toBool()
        || t.FastElAsVar_Flat(ti).toBool();
    }
  }
  return rval;
}

taMatrix* taMatrix::operator||(const Variant& t) const {
  if(t.isMatrixType()) {
    return operator||((const taMatrix&)*(t.toMatrix()));
  }
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_BYTE) {
    int vt = t.toInt();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((byte_Matrix*)this)->FastEl_Flat(i) || vt;
    }
  }
  else if(GetDataValType() == VT_INT) {
    int vt = t.toInt();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = ((int_Matrix*)this)->FastEl_Flat(i) || vt;
    }
  }
  else {                        // use variants -- no need to optimize
    bool vt = t.toBool();
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = FastElAsVar_Flat(i).toBool() || vt;
    }
  }
  return rval;
}

taMatrix* taMatrix::operator!() const {
  byte_Matrix* rval = new byte_Matrix(this->geom);
  if(GetDataValType() == VT_BYTE) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = !((byte_Matrix*)this)->FastEl_Flat(i);
    }
  }
  else if(GetDataValType() == VT_INT) {
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = !((int_Matrix*)this)->FastEl_Flat(i);
    }
  }
  else {                        // use variants -- no need to optimize
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(i) = !FastElAsVar_Flat(i).toBool();
    }
  }
  return rval;
}


//////////// op Flatten
taMatrix* taMatrix::Flatten() const {
  int ic = IterCount();
  int cnt = 0;
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(1, ic);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(cnt++) = ((float_Matrix*)this)->FastEl_Flat(i);
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(1, ic);
    TA_FOREACH_INDEX(i, *this) {
      rval->FastEl_Flat(cnt++) = ((double_Matrix*)this)->FastEl_Flat(i);
    }
    return rval;
  }
  else if(GetDataValType() == VT_VARIANT) {
    // first go through and flatten any sub-matricies!
    taMatrix* tmp_mat = (taMatrix*)Clone(); // we store modified vals in here
    taBase::Ref(tmp_mat);
    int nc = 0;                 // new count with flattend submatricies
    TA_FOREACH_INDEX(i, *this) {
      Variant var = tmp_mat->FastElAsVar_Flat(i);
      if(var.isMatrixType()) {
        taMatrix* smat = var.toMatrix();
        if(smat->ElView()) {    // it is a view
          taMatrix* flmat = smat->Flatten(); // flatten it!
          nc += flmat->size;
          tmp_mat->SetFmVar_Flat((Variant)flmat, i); // replace
        }
        else {
          nc += smat->size;     // keep
        }
      }
      else {
        nc++;                   // just another item
      }
    }

    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeom(1, nc);
    TA_FOREACH_INDEX(i, *this) {
      Variant var = tmp_mat->FastElAsVar_Flat(i);
      if(var.isMatrixType()) {
        taMatrix* smat = var.toMatrix(); // we know it is already flat
        for(int j=0; j < smat->size; j++) {
          Variant fitm = smat->FastElAsVar_Flat(j);
          rval->SetFmVar_Flat(fitm, cnt++);
        }
      }
      else {
        rval->SetFmVar_Flat(var, cnt++);
      }
    }
    taBase::UnRef(tmp_mat);
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeom(1, ic);
    TA_FOREACH_INDEX(i, *this) {
      rval->SetFmVar_Flat(FastElAsVar_Flat(i), cnt++);
    }
    return rval;
  }
  return NULL;
}

//////////// op Find
int_Matrix* taMatrix::Find() const {
  int n_nonz = 0;
  for(int i=0;i<size;i++) {
    if(FastElAsVar_Flat(i).toBool()) n_nonz++;
  }

  int dm = dims();
  MatrixIndex sidx;                                // slice idx
  int_Matrix* rval = new int_Matrix(2, dm, n_nonz);
  int cnt = 0;
  for(int i=0;i<size;i++) {
    if(!FastElAsVar_Flat(i).toBool()) continue;
    geom.DimsFmIndex(i, sidx);
    for(int d=0; d<dm; d++) {
      rval->FastEl2d(d,cnt) = sidx[d];
    }
    cnt++;
  }
  return rval;
}

taMatrix* taMatrix::Transpose() const {
  if(TestError(dims() != 2, "Transpose", "can only transpose a 2D matrix"))
    return NULL;
  int d0 = dim(0);
  int d1 = dim(1);
  MatrixGeom tg(2, d1, d0);
  if(GetDataValType() == VT_FLOAT) {
    float_Matrix* rval = new float_Matrix(tg);
    for(int i=0;i<d0;i++) {
      for(int j=0;j<d1;j++) {
        rval->FastEl2d(j,i) = ((float_Matrix*)this)->FastEl2d(i,j);
      }
    }
    return rval;
  }
  else if(GetDataValType() == VT_DOUBLE) {
    double_Matrix* rval = new double_Matrix(tg);
    for(int i=0;i<d0;i++) {
      for(int j=0;j<d1;j++) {
        rval->FastEl2d(j,i) = ((double_Matrix*)this)->FastEl2d(i,j);
      }
    }
    return rval;
  }
  else {                        // use variants -- no need to optimize
    taMatrix* rval = (taMatrix*)MakeToken();
    rval->SetGeomN(tg);
    for(int i=0;i<d0;i++) {
      for(int j=0;j<d1;j++) {
        rval->SetFmVar(SafeElAsVar(i,j), j,i);
      }
    }
    return rval;
  }
  return NULL;
}

