// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef KwtaSortBuff_h
#define KwtaSortBuff_h 1

// parent includes:
#include "leabra_def.h"
#include <taOBase>

// member includes:
#include <voidptr_Matrix>
#include <int_Matrix>

// declare all other types mentioned but not required to include:
class LeabraUnit; // 

TypeDef_Of(KwtaSortBuff);

class LEABRA_API KwtaSortBuff : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra kwta sort buffer -- structured by group and index
INHERITED(taOBase)
public:
  voidptr_Matrix kbuff;		// #NO_SAVE kwta computation buffer: always 2d [unidx][gpidx]
  int_Matrix 	 sizes;		// #NO_SAVE kwta computation buffer sizes: number of current items in corresponding buf -- always 1d [gpidx]

  LeabraUnit*	Un(int unidx, int gpidx)
  { return (LeabraUnit*)kbuff.FastEl(unidx, gpidx); }
  // get unit pointer from given unit and group index

  int 		Size(int gpidx) {
    return sizes.size > 0 ? sizes.FastEl(gpidx) : 0; }
  // get current used size of specified group -- returns reference that can be modified

  void		Set(LeabraUnit* un, int unidx, int gpidx)
  { kbuff.FastEl(unidx, gpidx) = (void*)un; }
  // set unit pointer at given unit and group index

  void		Add(LeabraUnit* un, int gpidx)
  { kbuff.FastEl(sizes.FastEl(gpidx)++, gpidx) = (void*)un; }
  // set unit pointer at given unit and group index, and increment size counter

  void		ResetGp(int gpidx)
  { sizes.FastEl(gpidx) = 0; }
  // reset list for subsequent adds (just resets size)

  void		ResetAll()
  { sizes.InitVals(0); }
  // reset all sizes for all groups

  void		Alloc(int nunits, int ngps)
  { ngps = MAX(ngps, 1);  kbuff.SetGeom(2, nunits, ngps);  sizes.SetGeom(1, ngps); ResetAll(); }
  // allocate storage to given number of units and groups, and initialize all sizes to 0

  override String       GetTypeDecoKey() const { return "Layer"; }

  void	InitLinks();
  void	CutLinks();
  void	Copy_(const KwtaSortBuff& cp);
  TA_BASEFUNS(KwtaSortBuff);
private:
  void	Initialize();
  void 	Destroy()	{  CutLinks(); }
};

#endif // KwtaSortBuff_h
