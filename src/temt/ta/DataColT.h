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

#ifndef DataColT_h
#define DataColT_h 1

// parent includes:
#include <DataCol>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(DataColT);

template<class T>
class DataColT : public DataCol {
  // #VIRT_BASE #NO_INSTANCE template for common elements
INHERITED(DataCol)
public:
  const taMatrix* AR() const CPP11_OVERRIDE { return &ar; } // the array pointer
  taMatrix*    AR()    CPP11_OVERRIDE { return &ar; } // the array pointer

  void  CutLinks()
    {ar.CutLinks(); DataCol::CutLinks();}
  TA_TMPLT_ABSTRACT_BASEFUNS(DataColT, T); //
public: //DO NOT ACCESS DIRECTLY
  T             ar;             // #NO_SHOW  the array itself
private:
  void  Copy_(const DataColT<T>& cp)
  { UnSetMatrixViewMode(); ar = cp.ar; SetMatrixViewMode(); }
  void  Initialize()            {}
  void  Destroy()               { CutLinks(); }
};

#endif // DataColT_h
