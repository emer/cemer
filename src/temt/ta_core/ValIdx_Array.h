// Copyright 2016, Regents of the University of Colorado,
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

#ifndef ValIdx_Array_h
#define ValIdx_Array_h 1

// parent includes:
#include <ValIdx>
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(ValIdx_Array);

class TA_API ValIdx_Array : public taArray<ValIdx> {
  // #NO_UPDATE_AFTER ##CAT_Math array of value & index items
INHERITED(taArray<ValIdx>)
public:
  static const ValIdx blank; // #HIDDEN #READ_ONLY null value
  TA_BASEFUNS_NOCOPY(ValIdx_Array);
  TA_ARRAY_FUNS(ValIdx_Array,ValIdx);
protected:
  Variant      El_GetVar_(const void* itm) const override
  { return (Variant)(((ValIdx*)itm)->val); }
  String        El_GetStr_(const void* it) const override
  { return (String)((ValIdx*)it); } // #IGNORE
  void          El_SetFmStr_(void* it, const String& val) override
  { ((ValIdx*)it)->val = (float)val; } // #IGNORE
private:
  void Initialize()     { };
  void Destroy()        { };
};

#endif // ValIdx_Array_h
