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

#ifndef XYNGeom_h
#define XYNGeom_h 1

// parent includes:
#include <PosVector2i>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(XYNGeom);

class TA_API XYNGeom : public PosVector2i {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP two-dimensional X-Y geometry with possibility of total number n != x*y
  INHERITED(PosVector2i)
public:
  bool          n_not_xy;       // #DEF_false total number of units is less than x * y
  int           n;              // #CONDEDIT_ON_n_not_xy:true total number of units (=x*y unless n_not_xy is true)

  int           z;
  // #HIDDEN #READ_ONLY #NO_SAVE legacy v3 third dimension -- used for conversion only -- do not use!!  to be removed at some later date

  void operator=(const taVector2i& cp);

  inline void   SetXYN(int xx, int yy, int nn)
  { x = xx; y = yy; n = nn; UpdateFlag(); }
  // set x, y, and n in one step

  inline void   UpdateFlag()    { n_not_xy = (x*y != n); }
  // update the n_not_xy flag from the current x,y,n values
  inline void   UpdateXYfmN()   { FitN(n); }
  // update x,y from existing n, including updating the n_not_xy flag
  inline void   UpdateNfmXY()   { n = x * y; UpdateFlag(); }
  // update x,y from N

  override bool FitN(int no)
  { bool rval = inherited::FitN(no); n = no; UpdateFlag(); return rval; }

  TA_BASEFUNS_LITE(XYNGeom);
protected:
  void  UpdateAfterEdit_impl();
private:
  void  Copy_(const XYNGeom& cp)
  { n_not_xy = cp.n_not_xy; n = cp.n; z = cp.z; }
  void  Initialize();
  void  Destroy()               { };

};

#endif // XYNGeom_h
