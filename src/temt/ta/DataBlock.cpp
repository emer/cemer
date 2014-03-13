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

#include "DataBlock.h"
#include <MatrixGeom>
#include <String_Matrix>

TA_BASEFUNS_CTORS_DEFN(DataBlock);
SMARTREF_OF_CPP(DataBlock); // DataBlockRef


const Variant DataBlock::GetData(int chan) { 
  if (ReadAvailable() && SourceChannelInRange(chan)) 
    return GetData_impl(chan);
  else return _nilVariant;
}

bool DataBlock::SetMatrixCellData(const Variant& data, int chan, int cell) 
{ 
  taMatrix* mat = GetSinkMatrix(chan); // mat is ref'ed!
  if (mat) { 
    mat->SetFmVar_Flat(data, cell);
    taBase::unRefDone(mat);
    return true; 
  }
  return false; 
}

bool DataBlock::SetMatrixCellDataByName(const Variant& data, const String& ch_nm,
  int cell) 
{ 
  taMatrix* mat = GetSinkMatrixByName(ch_nm); // mat is ref'ed!
  if(mat) {
    mat->SetFmVar_Flat(data, cell);
    taBase::unRefDone(mat);
    return true; 
  }
  return false;
}
