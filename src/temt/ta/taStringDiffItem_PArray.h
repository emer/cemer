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

#ifndef taStringDiffItem_PArray_h
#define taStringDiffItem_PArray_h 1

// parent includes:
#include <taStringDiffItem>
#include <taPlainArray>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taStringDiffItem_PArray : public taPlainArray<taStringDiffItem> {
  // #NO_TOKENS a plain-array of string diff items
INHERITED(taPlainArray<taStringDiffItem>)
public:

  void  operator=(const taStringDiffItem_PArray& cp)    { Copy_Duplicate(cp); }
  taStringDiffItem_PArray()                             { };
  taStringDiffItem_PArray(const taStringDiffItem_PArray& cp)    { Copy_Duplicate(cp); }
protected:
  String        El_GetStr_(const void* it) const { return _nilString; }
  void          El_SetFmStr_(void* it, const String& val) {  };
};

#endif // taStringDiffItem_PArray_h
