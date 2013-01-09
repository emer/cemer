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

#ifndef MatrixGeom_h
#define MatrixGeom_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:
class MatrixIndex; //
class int_Matrix; //


class TA_API MatrixGeom: public taBase  {
  // matrix geometry and index set -- holds dims() dimensions worth of integers, which are either the geometry of a matrix or a multidimensional index into a particular matrix location
INHERITED(taBase)
friend class taMatrix;
friend class DataCol;
public:
  int           n_dims; // #READ_ONLY #SHOW number of dimensions represented in this geom -- must be <= TA_MATRIX_DIMS_MAX (8)

  ///////////////////////////////////////////////////////
  //    Access

  inline int    dims() const { return n_dims;}
  // #CAT_Access number of dimensions represented in this geom
  inline bool   InRange(int idx) const {return ((idx >= 0) && (idx < n_dims));}
  // #CAT_Access is the given dimension index value within range of dimensions in this geom
  inline int    SafeEl(int i) const { if (InRange(i)) return el[i]; else return 0; }
  // #CAT_Access return geometry size along given dimension
  inline int    dim(int i) const { return SafeEl(i); }
  // #CAT_Access return geometry size along given dimension
  inline int    size(int i) const { return SafeEl(i); }
  // #CAT_Access return geometry size along given dimension
  int           Product() const;
  // #CAT_Access returns product of all dimension values (i.e., total size of geometry)
  int           IndexFmDimsN(const MatrixIndex& dims) const;
  // #CAT_Access get index from dimension values, based on geometry represented by 'this' geom object
  inline int    IndexFmDims(int d0, int d1=0, int d2=0,
                            int d3=0, int d4=0, int d5=0, int d6=0) const
  { int d[TA_MATRIX_DIMS_MAX]; d[0]=d0; d[1]=d1; d[2]=d2; d[3]=d3;
    d[4]=d4; d[5]=d5; d[6]=d6; d[7]=0; return IndexFmDims_(d); }

  // #CAT_Access get index from dimension values, based on geometry represented by 'this' geom object
  int   SafeIndexFmDimsN(const MatrixIndex& dims) const;
  // #CAT_Access get index from dimension values, based on geometry represented by 'this' geom object -- negative indexes count from end of each dimension, and if any are out of range for dimension, then overall return is -1 (safe range checking)
  inline int    SafeIndexFmDims(int d0, int d1=0, int d2=0,
                            int d3=0, int d4=0, int d5=0, int d6=0) const
  { int d[TA_MATRIX_DIMS_MAX]; d[0]=d0; d[1]=d1; d[2]=d2; d[3]=d3;
    d[4]=d4; d[5]=d5; d[6]=d6; d[7]=0; return SafeIndexFmDims_(d); }

  // #CAT_Access get index from dimension values, based on geometry represented by 'this' geom object -- negative indexes count from end of each dimension, and if any are out of range for dimension, then overall return is -1 (safe range checking)
  void          DimsFmIndex(int idx, MatrixIndex& dims) const;
  // #CAT_Access get dimension values from index, based on geometry represented by 'this' geom object

  ///////////////////////////////////////////////////////
  //    Modify and misc operations

  bool          SetDims(int dms);
  // #CAT_Modify sets number of dimensions, zeroing orphaned or new dims (true if changed size; false if not)
  void          AddDim(int value);
  // #CAT_Modify safely add a new dimension with given value
  void          Set(int i, int value);
  // #CAT_Modify safely set a dimension to given value
  void          SetGeom(int dims, int d0, int d1=0, int d2=0,
                        int d3=0, int d4=0, int d5=0, int d6=0);
  // #CAT_Modify initialize all geometry settings all at once
  void          GetGeom(int& dims, int& d0, int& d1, int& d2,
                                int& d3, int& d4, int& d5, int& d6);
  // #CAT_Access get all geometry information to individual ints
  inline void   Reset() { SetDims(0); }
  // #CAT_Modify set number of dimensions to 0, and clear all dims

  //////////////////////////////////////////////////////
  //   operators
  MatrixGeom*   operator+=(const MatrixGeom& ad);
  MatrixGeom*   operator-=(const MatrixGeom& ad);
  MatrixGeom*   operator*=(const MatrixGeom& ad);
  MatrixGeom*   operator/=(const MatrixGeom& ad);

  MatrixGeom*   operator+(const MatrixGeom& ad) const
    { MatrixGeom* r = new MatrixGeom(this); *r += ad; return r; }
  MatrixGeom*   operator-(const MatrixGeom& ad) const
    { MatrixGeom* r = new MatrixGeom(this); *r -= ad; return r; }
  MatrixGeom*   operator*(const MatrixGeom& ad) const
  { MatrixGeom* r = new MatrixGeom(this); *r *= ad; return r; }
  MatrixGeom*   operator/(const MatrixGeom& ad) const
    { MatrixGeom* r = new MatrixGeom(this); *r /= ad; return r; }

  bool operator==(const MatrixGeom& other) const
  { return Equal(other); }
  bool operator!=(const MatrixGeom& other) const
  { return !Equal(other); }

  operator int_Matrix*() const;
  // convert indicies to equivalent int matrix
  MatrixGeom* operator=(const taMatrix* cp);
  // initialize from a matrix (converts matrix values to ints)

  bool          Equal(const MatrixGeom& other) const;
  // #CAT_Access are two geometries equal to each other?

  bool          IsFrameOf(const MatrixGeom& other) const;
  // 'true' if this is a proper frame of other

  ///////////////////////////////////////////////////////
  //    2D and 4D representations matricies -- for display

  void          Get2DGeom(int& x, int& y) const;
  // #CAT_TwoDRep get geometry collapsed down/up to two dimensions
  void          Get2DGeomGui(int& x, int& y, bool odd_y, int spc) const;
  // #CAT_TwoDRep for rendering routines, provides standardized 2d geom regardless of dimensionality (includes space for extra dimensions), odd_y = for odd dimension sizes, put extra dimension in y (else x): 3d = x, (y+1) * z (vertical time series of 2d patterns, +1=space), 4d = (x+1)*xx, (y+1)*yy (e.g., 2d groups of 2d patterns), 5d = vertical time series of 4d.
  int           IndexFmDims2D(int col, int row, bool pat_4d, int mat_view) const;
  // #IGNORE get index from dimension values for 2d display (MatEditor and GridView) -- mat_view is taMisc::MatrixView def arg = taMisc::DEF_ZERO

  int           colCount(bool pat_4d = false) const;
  // #IGNORE for matrix view display, get column count (modulo 4d mode flag)
  int           rowCount(bool pat_4d = false) const;
  // #IGNORE for matrix view display, get row count (modulo 4d mode flag)

  ///////////////////////////////////////////////////////
  //    Input/Output/String

  String&       Print(String& strm, int indent=0) const;

  String        ToString(const char* ldelim = "[", const char* rdelim = "]") const;
  // #CAT_File returns human-friendly text in form: "[dims:{dim}{,dim}]"
  void          FromString(const String& str, const char* ldelim = "[",
                               const char* rdelim = "]");
  // #CAT_File reads geometry from string (consuming text) in form: "[dims:{dim}{,dim}]"
  override String GetValStr(void* par = NULL, MemberDef* md = NULL,
                            TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                            bool force_inline = false) const;
  override bool  SetValStr(const String& val, void* par = NULL, MemberDef* md = NULL,
                           TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                           bool force_inline = false);
  override int  Dump_Save_Value(ostream& strm, taBase* par=NULL, int indent = 0);
  override int  Dump_Load_Value(istream& strm, taBase* par=NULL);
  void          Copy_(const MatrixGeom& cp);
  explicit      MatrixGeom(int init_size);

  MatrixGeom(int dims, int d0, int d1=0, int d2=0, int d3=0,
             int d4=0, int d5=0, int d6=0);
  TA_BASEFUNS_LITE(MatrixGeom); //

public: // functions for internal/trusted use only
  inline int    FastEl(int i) const { return el[i]; } // #IGNORE
  inline int    operator [](int i) const { return el[i]; }  // #IGNORE

protected:
  override void         UpdateAfterEdit_impl();

  int           el[TA_MATRIX_DIMS_MAX];
  int           elprod[TA_MATRIX_DIMS_MAX]; // products of el's -- updated by UAE -- must be called!

  inline int    SafeIndex_(int d, const int dim) const
  { if(d < 0) d += dim; if(d<0 || d>dim+1) d = -1; return d; }
  // wrap negative values and do range checking
  int           IndexFmDims_(const int* d) const;
  // get index from dimension values, based on geometry
  int           SafeIndexFmDims_(const int* d) const;
  // get index from dimension values, based on geometry -- applies negaitve idx as counting back from end, and safe range checking for each dimension (returns -1 overall if any is out of range)

private:
  void          Initialize();
  void          Destroy();
};

#endif // MatrixGeom_h
