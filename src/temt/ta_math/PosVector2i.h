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

#ifndef PosVector2i_h
#define PosVector2i_h 1

// parent includes:
#include <taVector2i>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(PosVector2i);

class TA_API PosVector2i : public taVector2i {
  // #NO_UPDATE_AFTER #INLINE #INLINE_DUMP positive-only value in 2D coordinate space
  INHERITED(taVector2i)
public:
  TA_BASEFUNS_LITE(PosVector2i);

  inline PosVector2i& operator=(int cp) { x = cp; y = cp; return *this;}
  inline PosVector2i& operator=(float cp) { x = (int)cp; y = (int)cp; return *this;}
  inline PosVector2i& operator=(double cp) { x = (int)cp; y = (int)cp; return *this;}
  inline PosVector2i& operator=(const taVector2i& cp)
    {x = cp.x; y = cp.y; SetGtEq(0); return *this;}
protected:
  void  UpdateAfterEdit_impl();
private:
  void  Copy_(const PosVector2i&) {}
  void  Initialize()            { }
  void  Destroy()               { }
};

typedef PosVector2i PosTwoDCoord;	// #INSTANCE obsolete version of PosVector2i

#endif // PosVector2i_h
