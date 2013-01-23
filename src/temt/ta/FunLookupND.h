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

#ifndef FunLookupND_h
#define FunLookupND_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <float_Array>
#include <int_Array>

// declare all other types mentioned but not required to include:

// the following was written by Dan Cer (modified by O'Reilly)

TypeDef_Of(FunLookupND);

class TA_API FunLookupND : public taNBase {
  // #NO_UPDATE_AFTER ##CAT_Math n-dimensional function lookup: function is defined for n_dims inputs and produces a corresponding interpolated output
INHERITED(taNBase)
public:
  int           n_dims;         // number of dimensions
  float_Array   mesh_pts;       // the n-dimensional mesh of function values at specific points in n_dims space
  int_Array     mesh_dim_sz;    // [n_dims] number of points along each dimension
  float_Array   deltas;         // [n_dims] size of the mesh increment for each dimension
  float_Array   range_mins;     // [n_dims] minimum dimension values for each dimension
  float_Array   range_maxs;     // [n_dims] maximum dimension values for each dimension

  int   MeshPtToMeshIdx(int* pt);
  // #IGNORE converts an n_dims specification of the location of a given point into an index into the array

  float EvalArray(float* x, int* idx_map = NULL);
  // evaluate function given an n_dims array of dimension input values (index map for mapping the array onto internally-stored dimensions if non-NULL)
  float EvalArgs(float d0, float d1=0.0f, float d2=0.0f, float d3=0.0f, float d4=0.0f,
                 float d5=0.0f, float d6=0.0f, float d7=0.0f, float d8=0.0f, float d9=0.0f);
  // #BUTTON evaluate function given n_dims supplied values as args (in direct correspondence with internally-stored dimensions)

#ifndef __MAKETA__
  virtual void  LoadTable(std::istream& is);
  // #BUTTON load a lookup table of function values at specific points in n_dims space
#else
  virtual void  LoadTable(istream& is);
  // #BUTTON load a lookup table of function values at specific points in n_dims space
#endif
  virtual String&  ListTable(String& strm);
  // #BUTTON #USE_RVAL output the lookup table in text format
  virtual void  ShiftNorm(float desired_mean);
  // #BUTTON normalize the function values via an additive shift to achieve overall desired mean
  virtual void  MulNorm(float desired_mean);
  // #BUTTON normalize the function values by multiplying positive and negative values by separate scaling factors to achieve desired mean

  void  Initialize();
  void  Destroy()       { CutLinks(); }
  void  InitLinks();
  void  CutLinks();
  void  Copy_(const FunLookupND& cp);
  TA_BASEFUNS(FunLookupND);
};

#endif // FunLookupND_h
