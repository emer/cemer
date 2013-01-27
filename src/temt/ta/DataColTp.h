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

#ifndef DataColTp_h
#define DataColTp_h 1

// parent includes:
#include <DataCol>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(DataColTp);

template<class T>
class DataColTp : public DataCol {
  // #VIRT_BASE #NO_INSTANCE template for common elements
INHERITED(DataCol)
public:
  override const taMatrix* AR() const { return &ar; } // the array pointer
  override taMatrix*    AR()    { return &ar; } // the array pointer

  void  CutLinks()
    {ar.CutLinks(); DataCol::CutLinks();}
  TA_TMPLT_ABSTRACT_BASEFUNS(DataColTp, T); //
public: //DO NOT ACCESS DIRECTLY
  T             ar;             // #NO_SHOW  the array itself
private:
  void  Copy_(const DataColTp<T>& cp)  { ar = cp.ar; }
  void  Initialize()            {}
  void  Destroy()               { CutLinks(); }
};

#endif // DataColTp_h
