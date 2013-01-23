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

#ifndef VisRegionSpecBaseList_h
#define VisRegionSpecBaseList_h 1

// parent includes:
#include <VisRegionSpecBase>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(VisRegionSpecBaseList);

class TA_API VisRegionSpecBaseList : public taList<VisRegionSpecBase> {
  // ##CAT_Image a list of visual region image processing filters
INHERITED(taList<VisRegionSpecBase>)
public:

  virtual VisRegionSpecBase* FindRetinalRegion(VisRegionParams::Region reg);
  // find first spec with given retinal region
  virtual VisRegionSpecBase* FindRetinalRes(VisRegionParams::Resolution res);
  // find first spec with given resolution
  virtual VisRegionSpecBase* FindRetinalRegionRes(VisRegionParams::Region reg,
					      VisRegionParams::Resolution res);
  // find first spec with given retinal region and resolution (falls back to res then reg if no perfect match)

  TA_BASEFUNS_NOCOPY(VisRegionSpecBaseList);
private:
  void	Initialize() 		{ SetBaseType(&TA_VisRegionSpecBase); }
  void 	Destroy()		{ };
};

#endif // VisRegionSpecBaseList_h
