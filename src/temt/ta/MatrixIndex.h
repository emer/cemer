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

#ifndef MatrixIndex_h
#define MatrixIndex_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:
class int_Matrix; //


taTypeDef_Of(MatrixIndex);

class TA_API MatrixIndex: public taBase  {
  // matrix index set -- holds dims() dimensions worth of integers, which are a multidimensional index into a particular matrix location
INHERITED(taBase)
friend class taMatrix;
friend class MatrixGeom;
friend class DataCol;
public:
  int           n_dims; // #READ_ONLY #SHOW number of dimensions represented in this index -- must be <= TA_MATRIX_DIMS_MAX (8)

  ///////////////////////////////////////////////////////
  //    Access

  inline int    dims() const { return n_dims;}
  // #CAT_Access number of dimensions represented in this geom
  inline bool   InRange(int idx) const { return ((idx >= 0) && (idx < n_dims)); }
  // #CAT_Access is the given dimension index value within range of dimensions in this geom
  inline int    SafeEl(int i) const { if (InRange(i)) return el[i]; else return 0; }
  // #CAT_Access return index along given dimension, safe access
  inline int    dim(int i) const { return SafeEl(i); }
  // #CAT_Access return index along given dimension

  ///////////////////////////////////////////////////////
  //    Modify and misc operations

  bool          SetDims(int dms);
  // #CAT_Modify sets number of dimensions, zeroing orphaned or new dims (true if changed size; false if not)
  void          AddDim(int value);
  // #CAT_Modify safely add a new dimension with given value
  void          Set(int i, int value) { if (InRange(i)) el[i] = value; }
  // #CAT_Modify safely set a dimension to given value
  void          SetIndexes(int dims, int d0, int d1=0, int d2=0,
                        int d3=0, int d4=0, int d5=0, int d6=0);
  // #CAT_Modify initialize all indicies all at once
  void          GetIndexes(int& dims, int& d0, int& d1, int& d2,
                           int& d3, int& d4, int& d5, int& d6);
  // #CAT_Access get all indicies to individual ints
  inline void   Reset() { SetDims(0); }
  // #CAT_Modify set number of dimensions to 0, and clear all dims

  //////////////////////////////////////////////////////
  //   operators
  MatrixIndex*  operator+=(const MatrixIndex& ad);
  MatrixIndex*  operator-=(const MatrixIndex& ad);
  MatrixIndex*  operator*=(const MatrixIndex& ad);
  MatrixIndex*  operator/=(const MatrixIndex& ad);

  MatrixIndex*  operator+(const MatrixIndex& ad) const
    { MatrixIndex* r = new MatrixIndex(this); *r += ad; return r; }
  MatrixIndex*  operator-(const MatrixIndex& ad) const
    { MatrixIndex* r = new MatrixIndex(this); *r -= ad; return r; }
  MatrixIndex*  operator*(const MatrixIndex& ad) const
  { MatrixIndex* r = new MatrixIndex(this); *r *= ad; return r; }
  MatrixIndex*  operator/(const MatrixIndex& ad) const
    { MatrixIndex* r = new MatrixIndex(this); *r /= ad; return r; }

  bool operator==(const MatrixIndex& other) const
  { return Equal(other); }
  bool operator!=(const MatrixIndex& other) const
  { return !Equal(other); }

  operator int_Matrix*() const;
  // convert indicies to equivalent int matrix
  MatrixIndex* operator=(const taMatrix* cp);
  // initialize from a matrix (converts matrix values to ints)

  bool          Equal(const MatrixIndex& other) const;
  // #CAT_Access are two indexes equal to each other?

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
  override int  Dump_Save_Value(std::ostream& strm, taBase* par=NULL, int indent = 0);
  override int  Dump_Load_Value(std::istream& strm, taBase* par=NULL);

  void          Copy_(const MatrixIndex& cp);

  explicit      MatrixIndex(int init_size);
  MatrixIndex(int dims, int d0, int d1=0, int d2=0, int d3=0,
             int d4=0, int d5=0, int d6=0);
  TA_BASEFUNS_LITE(MatrixIndex); //

public: // functions for internal/trusted use only
  inline int    FastEl(int i) const { return el[i]; } // #IGNORE
  inline int    operator [](int i) const { return el[i]; }  // #IGNORE
  inline int&   FastEl(int i) { return el[i]; } // #IGNORE
  inline int&   operator [](int i) { return el[i]; }  // #IGNORE

protected:
  int           el[TA_MATRIX_DIMS_MAX];

private:
  void          Initialize();
  void          Destroy();
};

#endif // MatrixIndex_h
